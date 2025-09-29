
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

#include "distributed/utils/spq_io_multiplex.h"
#include <sys/epoll.h>
#ifdef HAVE_POLL_H
#include <poll.h>
#endif
#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif

extern THR_LOCAL volatile sig_atomic_t waiting;
extern THR_LOCAL int selfpipe_readfd;

namespace Spq {

int SpqIOMultiplexModel = SPQ_IO_MULTIPLEX_EPOLL;

class PollIO : public IOMultiplexor {
public:
    PollIO(int nevents, MemoryContext mem) : IOMultiplexor(nevents, mem)
    {}

    /** Create all events's memory structure. */
    void CreateEvents() override
    {
        auto size = MAXALIGN(sizeof(struct pollfd) * m_nEvents);
        m_pollfds = static_cast<struct pollfd*>(MemoryContextAllocZero(m_mem, size));
    }

    /** Init a WaitEvent
     @param[in]  event       the waitevent needs to be initialized
     @param[in]  isModify    whether or not invoked by modifing operation. */
    void InitWaitEvent(WaitEvent* event, bool isModify) override;

    /** Wait some waitevent's result.
     @param[in]     set             the waiteventset owned current IOMultiplexor
     @param[in]     curTimeout      timeout for waiting.
     @param[out]    occurredEvents  all occurred events
     @param[in]     expectEvents    the number of event epected to occurr
     @return the number of events received. */
    int WaitEvents(WaitEventSet* set, int curTimeout, WaitEvent* occurredEvents,
                   int expectEvents) override;

private:
    /** all poll's event handler */
    struct pollfd* m_pollfds;
};

void PollIO::InitWaitEvent(WaitEvent* event, bool isModify)
{
    struct pollfd* pollfd = &m_pollfds[event->pos];

    pollfd->revents = 0;
    pollfd->fd = event->fd;

    /* prepare pollfd entry once */
    if (event->events == WL_LATCH_SET) {
        pollfd->events = POLLIN;
    } else {
        Assert(event->events &
               (WL_SOCKET_READABLE | WL_SOCKET_WRITEABLE | WL_SOCKET_CLOSED));
        pollfd->events = 0;
        if (event->events & WL_SOCKET_READABLE)
            pollfd->events |= POLLIN;
        if (event->events & WL_SOCKET_WRITEABLE)
            pollfd->events |= POLLOUT;
#ifdef POLLRDHUP
        if (event->events & WL_SOCKET_CLOSED)
            pollfd->events |= POLLRDHUP;
#endif
    }

    Assert(event->fd != PGINVALID_SOCKET);
}

int PollIO::WaitEvents(WaitEventSet* set, int curTimeout, WaitEvent* occurredEvents,
                       int expectEvents)
{
    int returnedEvents = 0;
    int rc;
    WaitEvent* curEvent;
    struct pollfd* curPollfd;

    /* Sleep */
    rc = poll(m_pollfds, set->nevents, (int)curTimeout);

    /* Check return code */
    if (rc < 0) {
        /* EINTR is okay, otherwise complain */
        if (errno != EINTR) {
            waiting = false;
            ereport(ERROR,
                    (errcode_for_socket_access(), errmsg("%s() failed: %m", "poll")));
        }
        return 0;
    } else if (rc == 0) {
        /* timeout exceeded */
        return -1;
    }

    for (curEvent = set->events, curPollfd = m_pollfds;
         curEvent < (set->events + set->nevents) && returnedEvents < expectEvents;
         curEvent++, curPollfd++) {
        /* no activity on this FD, skip */
        if (curPollfd->revents == 0)
            continue;

        occurredEvents->pos = curEvent->pos;
        occurredEvents->user_data = curEvent->user_data;
        occurredEvents->events = 0;

        if (curEvent->events == WL_LATCH_SET &&
            (curPollfd->revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL))) {
            /* There's data in the self-pipe, clear it. */
            drain();

            if (set->latch && set->latch->is_set) {
                occurredEvents->fd = PGINVALID_SOCKET;
                occurredEvents->events = WL_LATCH_SET;
                occurredEvents++;
                returnedEvents++;
            }
        } else if (curEvent->events &
                   (WL_SOCKET_READABLE | WL_SOCKET_WRITEABLE | WL_SOCKET_CLOSED)) {
            int errflags = POLLHUP | POLLERR | POLLNVAL;

            Assert(curEvent->fd >= PGINVALID_SOCKET);

            if ((curEvent->events & WL_SOCKET_READABLE) &&
                (curPollfd->revents & (POLLIN | errflags))) {
                /* data available in socket, or EOF */
                occurredEvents->events |= WL_SOCKET_READABLE;
            }

            if ((curEvent->events & WL_SOCKET_WRITEABLE) &&
                (curPollfd->revents & (POLLOUT | errflags))) {
                /* writeable, or EOF */
                occurredEvents->events |= WL_SOCKET_WRITEABLE;
            }

#ifdef POLLRDHUP
            if ((curEvent->events & WL_SOCKET_CLOSED) &&
                (curPollfd->revents & (POLLRDHUP | errflags))) {
                /* remote peer closed, or error */
                occurredEvents->events |= WL_SOCKET_CLOSED;
            }
#endif

            if (occurredEvents->events != 0) {
                occurredEvents->fd = curEvent->fd;
                occurredEvents++;
                returnedEvents++;
            }
        }
    }
    return returnedEvents;
}

class EpollIO : public IOMultiplexor {
public:
    EpollIO(int nevents, MemoryContext mem) : IOMultiplexor(nevents, mem)
    {}

    /** Create all events's memory structure. */
    void CreateEvents() override
    {
        auto size = MAXALIGN(sizeof(struct epoll_event) * m_nEvents);
        m_epollRetEvents =
            static_cast<struct epoll_event*>(MemoryContextAllocZero(m_mem, size));
        m_epollfd = epoll_create1(EPOLL_CLOEXEC);
        if (m_epollfd < 0) {
            elog(ERROR, "epoll_create1 failed: %m");
        }
    }

    /** Init a WaitEvent
     @param[in]  event       the waitevent needs to be initialized
     @param[in]  isModify    whether or not invoked by modifing operation. */
    void InitWaitEvent(WaitEvent* event, bool isModify) override;

    /** Wait some waitevent's result.
     @param[in]     set             the waiteventset owned current IOMultiplexor
     @param[in]     curTimeout      timeout for waiting.
     @param[out]    occurredEvents  all occurred events
     @param[in]     expectEvents    the number of event */
    int WaitEvents(WaitEventSet* set, int curTimeout, WaitEvent* occurredEvents,
                   int expectEvents) override;

    /** free some resources in epoll. */
    void Free() override
    {
        close(m_epollfd);
    }

private:
    int m_epollfd;

    /* epoll_wait returns events in a user provided arrays, allocate once */
    struct epoll_event* m_epollRetEvents;
};

void EpollIO::InitWaitEvent(WaitEvent* event, bool isModify)
{
    struct epoll_event epoll_ev;
    int rc;

    if (!isModify && event->events == WL_LATCH_SET) {
        return;
    }

    /* pointer to our event, returned by epoll_wait */
    epoll_ev.data.ptr = event;
    /* always wait for errors */
    epoll_ev.events = EPOLLERR | EPOLLHUP;

    /* prepare pollfd entry once */
    if (event->events == WL_LATCH_SET) {
        epoll_ev.events |= EPOLLIN;
    } else {
        Assert(event->fd != PGINVALID_SOCKET);
        Assert(event->events &
               (WL_SOCKET_READABLE | WL_SOCKET_WRITEABLE | WL_SOCKET_CLOSED));

        if (event->events & WL_SOCKET_READABLE)
            epoll_ev.events |= EPOLLIN;
        if (event->events & WL_SOCKET_WRITEABLE)
            epoll_ev.events |= EPOLLOUT;
        if (event->events & WL_SOCKET_CLOSED)
            epoll_ev.events |= EPOLLRDHUP;
    }

    /*
     * Even though unused, we also pass epoll_ev as the data argument if
     * EPOLL_CTL_DEL is passed as action.  There used to be an epoll bug
     * requiring that, and actually it makes the code simpler...
     */
    rc = epoll_ctl(m_epollfd, isModify ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, event->fd,
                   &epoll_ev);

    if (rc < 0)
        ereport(ERROR,
                (errcode_for_socket_access(), errmsg("%s() failed: %m", "epoll_ctl")));
}

int EpollIO::WaitEvents(WaitEventSet* set, int curTimeout, WaitEvent* occurredEvents,
                        int expectEvents)
{
    int returnedEvents = 0;
    int rc;
    WaitEvent* curEvent;
    struct epoll_event* curEpollEvent;

    /* Sleep */
    rc =
        epoll_wait(m_epollfd, m_epollRetEvents, Min(expectEvents, m_nEvents), curTimeout);

    /* Check return code */
    if (rc < 0) {
        /* EINTR is okay, otherwise complain */
        if (errno != EINTR) {
            waiting = false;
            ereport(ERROR, (errcode_for_socket_access(),
                            errmsg("%s() failed: %m", "epoll_wait")));
        }
        return 0;
    } else if (rc == 0) {
        /* timeout exceeded */
        return -1;
    }

    /*
     * At least one event occurred, iterate over the returned epoll events
     * until they're either all processed, or we've returned all the events
     * the caller desired.
     */
    for (curEpollEvent = m_epollRetEvents;
         curEpollEvent < (m_epollRetEvents + rc) && returnedEvents < expectEvents;
         curEpollEvent++) {

        /* epoll's data pointer is set to the associated WaitEvent */
        curEvent = (WaitEvent*)curEpollEvent->data.ptr;

        occurredEvents->pos = curEvent->pos;
        occurredEvents->user_data = curEvent->user_data;
        occurredEvents->events = 0;

        if (curEvent->events == WL_LATCH_SET &&
            curEpollEvent->events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
            /* Drain the signalfd. */
            drain();

            if (set->latch && set->latch->is_set) {
                occurredEvents->fd = PGINVALID_SOCKET;
                occurredEvents->events = WL_LATCH_SET;
                occurredEvents++;
                returnedEvents++;
            }
        } else if (curEvent->events &
                   (WL_SOCKET_READABLE | WL_SOCKET_WRITEABLE | WL_SOCKET_CLOSED)) {
            Assert(curEvent->fd != PGINVALID_SOCKET);

            if ((curEvent->events & WL_SOCKET_READABLE) &&
                (curEpollEvent->events & (EPOLLIN | EPOLLERR | EPOLLHUP))) {
                /* data available in socket, or EOF */
                occurredEvents->events |= WL_SOCKET_READABLE;
            }

            if ((curEvent->events & WL_SOCKET_WRITEABLE) &&
                (curEpollEvent->events & (EPOLLOUT | EPOLLERR | EPOLLHUP))) {
                /* writable, or EOF */
                occurredEvents->events |= WL_SOCKET_WRITEABLE;
            }

            if ((curEvent->events & WL_SOCKET_CLOSED) &&
                (curEpollEvent->events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))) {
                /* remote peer shut down, or error */
                occurredEvents->events |= WL_SOCKET_CLOSED;
            }

            if (occurredEvents->events != 0) {
                occurredEvents->fd = curEvent->fd;
                occurredEvents++;
                returnedEvents++;
            }
        }
    }

    return returnedEvents;
}

IOMultiplexor* IOMultiplexor::CreateIOMultiplexor(int nevents, MemoryContext mem)
{
    IOMultiplexor* multiplexor = nullptr;

    if (SpqIOMultiplexModel == SPQ_IO_MULTIPLEX_EPOLL) {
        multiplexor =
            new ((MemoryContextAlloc(mem, sizeof(EpollIO)))) EpollIO(nevents, mem);
    } else if (SpqIOMultiplexModel == SPQ_IO_MULTIPLEX_POLL) {
        multiplexor =
            new ((MemoryContextAlloc(mem, sizeof(PollIO)))) PollIO(nevents, mem);
    }

    multiplexor->CreateEvents();

    return multiplexor;
}

void IOMultiplexor::drain()
{
    char buf[1024];
    int rc;
    int fd;

    fd = selfpipe_readfd;

    for (;;) {
        rc = read(fd, buf, sizeof(buf));
        if (rc < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break; /* the descriptor is empty */
            else if (errno == EINTR)
                continue; /* retry */
            else {
                waiting = false;
                elog(ERROR, "read() on self-pipe failed: %m");
            }
        } else if (rc == 0) {
            waiting = false;
            elog(ERROR, "unexpected EOF on self-pipe");
        } else if (rc < sizeof(buf)) {
            /* we successfully drained the pipe; no need to read() again */
            break;
        }
        /* else buffer wasn't big enough, so read again */
    }
}

}  // namespace Spq