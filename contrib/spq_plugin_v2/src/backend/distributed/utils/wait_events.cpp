/*
 * Portions Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 1996-2017, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * --------------------------------------------------------------------------------------
 */

#include "postgres.h"
#include "knl/knl_variable.h"

#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include "miscadmin.h"
#include "portability/instr_time.h"
#include "postmaster/postmaster.h"
#include "storage/barrier.h"
#include "distributed/utils/wait_events.h"
#include "distributed/utils/spq_io_multiplex.h"
#include "storage/pmsignal.h"
#include "storage/shmem.h"
#include "storage/ipc.h"
#include "gssignal/gs_signal.h"

#ifdef HAVE_POLL_H
#include <poll.h>
#endif
#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif

extern THR_LOCAL volatile sig_atomic_t waiting;
/* Read and write ends of the self-pipe */
extern THR_LOCAL int selfpipe_readfd;
extern THR_LOCAL int selfpipe_writefd;
static THR_LOCAL MemoryContext WaitEventMemCtx = nullptr;

void InitialzeCitusWaitEventSet()
{
    WaitEventMemCtx = AllocSetContextCreate(
        TopMemoryContext, "Spq Wait Sets Context", ALLOCSET_DEFAULT_MINSIZE,
        ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);
}
/*
 * Create a WaitEventSet with space for nevents different events to wait for.
 *
 * These events can then be efficiently waited upon together, using
 * WaitEventSetWait().
 *
 * The WaitEventSet is tracked by the given 'resowner'.  Use NULL for session
 * lifetime.
 */
WaitEventSet* CreateWaitEventSet(ResourceOwner resowner, int nevents)
{
    WaitEventSet* set;
    char* data;
    Size sz = 0;

    /*
     * Use MAXALIGN size/alignment to guarantee that later uses of memory are
     * aligned correctly. E.g. epoll_event might need 8 byte alignment on some
     * platforms, but earlier allocations like WaitEventSet and WaitEvent
     * might not be sized to guarantee that when purely using sizeof().
     */
    sz += MAXALIGN(sizeof(WaitEventSet));
    sz += MAXALIGN(sizeof(WaitEvent) * nevents);

    auto localctx = WaitEventMemCtx;
    data = (char*)MemoryContextAllocZero(localctx, sz);

    set = (WaitEventSet*)data;
    data += MAXALIGN(sizeof(WaitEventSet));

    set->events = (WaitEvent*)data;
    data += MAXALIGN(sizeof(WaitEvent) * nevents);

    set->latch = NULL;

    set->m_IOMultiplexor =
        Spq::IOMultiplexor::CreateIOMultiplexor(nevents, WaitEventMemCtx);
    return set;
}

/*
 * Free a previously created WaitEventSet.
 *
 * Note: preferably, this shouldn't have to free any resources that could be
 * inherited across an exec().  If it did, we'd likely leak those resources in
 * many scenarios.  For the epoll case, we ensure that by setting EPOLL_CLOEXEC
 * when the FD is created.  For the Windows case, we assume that the handles
 * involved are non-inheritable.
 */
void FreeWaitEventSet(WaitEventSet* set)
{
    set->m_IOMultiplexor->Free();

    pfree(set);
}

/*
 * Free a previously created WaitEventSet in a child process after a fork().
 */
void FreeWaitEventSetAfterFork(WaitEventSet* set)
{
    set->m_IOMultiplexor->Free();

    pfree(set);
}

/* ---
 * Add an event to the set. Possible events are:
 * - WL_LATCH_SET: Wait for the latch to be set
 * - WL_POSTMASTER_DEATH: Wait for postmaster to die
 * - WL_SOCKET_READABLE: Wait for socket to become readable,
 *	 can be combined in one event with other WL_SOCKET_* events
 * - WL_SOCKET_WRITEABLE: Wait for socket to become writeable,
 *	 can be combined with other WL_SOCKET_* events
 * - WL_SOCKET_CONNECTED: Wait for socket connection to be established,
 *	 can be combined with other WL_SOCKET_* events (on non-Windows
 *	 platforms, this is the same as WL_SOCKET_WRITEABLE)
 * - WL_SOCKET_ACCEPT: Wait for new connection to a server socket,
 *	 can be combined with other WL_SOCKET_* events (on non-Windows
 *	 platforms, this is the same as WL_SOCKET_READABLE)
 * - WL_SOCKET_CLOSED: Wait for socket to be closed by remote peer.
 * - WL_EXIT_ON_PM_DEATH: Exit immediately if the postmaster dies
 *
 * Returns the offset in WaitEventSet->events (starting from 0), which can be
 * used to modify previously added wait events using ModifyWaitEvent().
 *
 * In the WL_LATCH_SET case the latch must be owned by the current process,
 * i.e. it must be a process-local latch initialized with InitLatch, or a
 * shared latch associated with the current process by calling OwnLatch.
 *
 * In the WL_SOCKET_READABLE/WRITEABLE/CONNECTED/ACCEPT cases, EOF and error
 * conditions cause the socket to be reported as readable/writable/connected,
 * so that the caller can deal with the condition.
 *
 * The user_data pointer specified here will be set for the events returned
 * by WaitEventSetWait(), allowing to easily associate additional data with
 * events.
 */
int AddWaitEventToSet(WaitEventSet* set, uint32 events, pgsocket fd, Latch* latch,
                      void* user_data)
{
    WaitEvent* event;

    /* not enough space */
    Assert(set->nevents < set->m_IOMultiplexor->GetEvents());

    Assert(!(events & WL_POSTMASTER_DEATH));

    if (latch) {
        if (latch->owner_pid != t_thrd.proc_cxt.MyProcPid)
            elog(ERROR, "cannot wait on a latch owned by another process");
        if (set->latch)
            elog(ERROR, "cannot wait on more than one latch");
        if ((events & WL_LATCH_SET) != WL_LATCH_SET)
            elog(ERROR, "latch events only support being set");
    } else {
        if (events & WL_LATCH_SET)
            elog(ERROR, "cannot wait on latch without a specified latch");
    }

    /* waiting for socket readiness without a socket indicates a bug */
    if (fd == PGINVALID_SOCKET && (events & WL_SOCKET_MASK))
        elog(ERROR, "cannot wait on socket event without a socket");

    event = &set->events[set->nevents];
    event->pos = set->nevents++;
    event->fd = fd;
    event->events = events;
    event->user_data = user_data;

    if (events == WL_LATCH_SET) {
        set->latch = latch;
        set->latch_pos = event->pos;
        event->fd = selfpipe_readfd;
    }

    set->m_IOMultiplexor->InitWaitEvent(event, false);

    return event->pos;
}

/*
 * Change the event mask and, in the WL_LATCH_SET case, the latch associated
 * with the WaitEvent.  The latch may be changed to NULL to disable the latch
 * temporarily, and then set back to a latch later.
 *
 * 'pos' is the id returned by AddWaitEventToSet.
 */
void ModifyWaitEvent(WaitEventSet* set, int pos, uint32 events, Latch* latch)
{
    WaitEvent* event;

    Assert(pos < set->nevents);
    event = &set->events[pos];

    /*
     * If neither the event mask nor the associated latch changes, return
     * early. That's an important optimization for some sockets, where
     * ModifyWaitEvent is frequently used to switch from waiting for reads to
     * waiting on writes.
     */
    if (events == event->events &&
        (!(event->events & WL_LATCH_SET) || set->latch == latch))
        return;

    if (event->events & WL_LATCH_SET && events != event->events) {
        elog(ERROR, "cannot modify latch event");
    }

    if (event->events & WL_POSTMASTER_DEATH) {
        elog(ERROR, "cannot modify postmaster death event");
    }

    /* FIXME: validate event mask */
    event->events = events;

    if (events == WL_LATCH_SET) {
        if (latch && latch->owner_pid != t_thrd.proc_cxt.MyProcPid)
            elog(ERROR, "cannot wait on a latch owned by another process");
        set->latch = latch;

        /*
         * On Unix, we don't need to modify the kernel object because the
         * underlying pipe (if there is one) is the same for all latches so we
         * can return immediately.  On Windows, we need to update our array of
         * handles, but we leave the old one in place and tolerate spurious
         * wakeups if the latch is disabled.
         */
        return;
    }

    set->m_IOMultiplexor->InitWaitEvent(event, true);
}

/*
 * Wait for events added to the set to happen, or until the timeout is
 * reached.  At most nevents occurred events are returned.
 *
 * If timeout = -1, block until an event occurs; if 0, check sockets for
 * readiness, but don't block; if > 0, block for at most timeout milliseconds.
 *
 * Returns the number of events occurred, or 0 if the timeout was reached.
 *
 * Returned events will have the fd, pos, user_data fields set to the
 * values associated with the registered event.
 */
int WaitEventSetWait(WaitEventSet* set, long timeout, WaitEvent* occurred_events,
                     int nevents, uint32 wait_event_info)
{
    int returned_events = 0;
    instr_time start_time;
    instr_time cur_time;
    long cur_timeout = -1;

    Assert(nevents > 0);

    /*
     * Initialize timeout if requested.  We must record the current time so
     * that we can determine the remaining timeout if interrupted.
     */
    if (timeout >= 0) {
        INSTR_TIME_SET_CURRENT(start_time);
        Assert(timeout >= 0 && timeout <= INT_MAX);
        cur_timeout = timeout;
    } else
        INSTR_TIME_SET_ZERO(start_time);

    // pgstat_report_wait_start(wait_event_info);

    waiting = true;

    while (returned_events == 0) {
        int rc;

        /*
         * Check if the latch is set already first.  If so, we either exit
         * immediately or ask the kernel for further events available right
         * now without waiting, depending on how many events the caller wants.
         *
         * If someone sets the latch between this and the
         * WaitEventSetWaitBlock() below, the setter will write a byte to the
         * pipe (or signal us and the signal handler will do that), and the
         * readiness routine will return immediately.
         *
         * On unix, If there's a pending byte in the self pipe, we'll notice
         * whenever blocking. Only clearing the pipe in that case avoids
         * having to drain it every time WaitLatchOrSocket() is used. Should
         * the pipe-buffer fill up we're still ok, because the pipe is in
         * nonblocking mode. It's unlikely for that to happen, because the
         * self pipe isn't filled unless we're blocking (waiting = true), or
         * from inside a signal handler in latch_sigurg_handler().
         *
         * On windows, we'll also notice if there's a pending event for the
         * latch when blocking, but there's no danger of anything filling up,
         * as "Setting an event that is already set has no effect.".
         *
         * Note: we assume that the kernel calls involved in latch management
         * will provide adequate synchronization on machines with weak memory
         * ordering, so that we cannot miss seeing is_set if a notification
         * has already been queued.
         */
        if (set->latch && !set->latch->is_set) {
            pg_memory_barrier();
            /* and recheck */
        }

        if (set->latch && set->latch->is_set) {
            occurred_events->fd = PGINVALID_SOCKET;
            occurred_events->pos = set->latch_pos;
            occurred_events->user_data = set->events[set->latch_pos].user_data;
            occurred_events->events = WL_LATCH_SET;
            occurred_events++;
            returned_events++;

            if (returned_events == nevents)
                break; /* output buffer full already */

            /*
             * Even though we already have an event, we'll poll just once with
             * zero timeout to see what non-latch events we can fit into the
             * output buffer at the same time.
             */
            cur_timeout = 0;
            timeout = 0;
        }

        /*
         * Wait for events using the readiness primitive chosen at the top of
         * this file. If -1 is returned, a timeout has occurred, if 0 we have
         * to retry, everything >= 1 is the number of returned events.
         */

        rc = set->m_IOMultiplexor->WaitEvents(set, cur_timeout, occurred_events,
                                              nevents - returned_events);

        if (rc == -1)
            break; /* timeout occurred */
        else
            returned_events += rc;

        /* If we're not done, update cur_timeout for next iteration */
        if (returned_events == 0 && timeout >= 0) {
            INSTR_TIME_SET_CURRENT(cur_time);
            INSTR_TIME_SUBTRACT(cur_time, start_time);
            cur_timeout = timeout - (long)INSTR_TIME_GET_MILLISEC(cur_time);
            if (cur_timeout <= 0)
                break;
        }
    }

    waiting = false;

    // pgstat_report_wait_end();

    return returned_events;
}

/*
 * Return whether the current build options can report WL_SOCKET_CLOSED.
 */
bool WaitEventSetCanReportClosed(void)
{
    if (Spq::SpqIOMultiplexModel == Spq::IOMultiplexType::SPQ_IO_MULTIPLEX_EPOLL) {
        return true;
    }

    if (Spq::SpqIOMultiplexModel == Spq::IOMultiplexType::SPQ_IO_MULTIPLEX_POLL) {
#ifdef POLLRDHUP
        return true;
#endif
    }

    return false;
}

/*
 * Get the number of wait events registered in a given WaitEventSet.
 */
int GetNumRegisteredWaitEvents(WaitEventSet* set)
{
    return set->nevents;
}

/*
 * Like WaitLatch, but with an extra socket argument for WL_SOCKET_*
 * conditions.
 *
 * When waiting on a socket, EOF and error conditions always cause the socket
 * to be reported as readable/writable/connected, so that the caller can deal
 * with the condition.
 *
 * wakeEvents must include either WL_EXIT_ON_PM_DEATH for automatic exit
 * if the postmaster dies or WL_POSTMASTER_DEATH for a flag set in the
 * return value if the postmaster dies.  The latter is useful for rare cases
 * where some behavior other than immediate exit is needed.
 *
 * NB: These days this is just a wrapper around the WaitEventSet API. When
 * using a latch very frequently, consider creating a longer living
 * WaitEventSet instead; that's more efficient.
 */
int WaitLatchOrSocket(Latch* latch, int wakeEvents, pgsocket sock, long timeout,
                      uint32 wait_event_info)
{
    int ret = 0;
    int rc;
    WaitEvent event;
    WaitEventSet* set = CreateWaitEventSet(t_thrd.utils_cxt.CurrentResourceOwner, 3);

    Assert(!(wakeEvents & WL_POSTMASTER_DEATH));
    if (wakeEvents & WL_TIMEOUT)
        Assert(timeout >= 0);
    else
        timeout = -1;

    if (wakeEvents & WL_LATCH_SET)
        AddWaitEventToSet(set, WL_LATCH_SET, PGINVALID_SOCKET, latch, NULL);

    if (wakeEvents & WL_SOCKET_MASK) {
        int ev;

        ev = wakeEvents & WL_SOCKET_MASK;
        AddWaitEventToSet(set, ev, sock, NULL, NULL);
    }

    rc = WaitEventSetWait(set, timeout, &event, 1, wait_event_info);

    if (rc == 0)
        ret |= WL_TIMEOUT;
    else {
        ret |= event.events & (WL_LATCH_SET | WL_SOCKET_MASK);
    }

    FreeWaitEventSet(set);

    return ret;
}