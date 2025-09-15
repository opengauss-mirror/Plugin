
/*
 * Copyright (c) 2025 Huawei Technologies Co.,Ltd.
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

#ifndef SPQ_WAIT_EVENTS
#define SPQ_WAIT_EVENTS

#include "storage/latch.h"
#include "utils/resowner.h"

#ifdef WIN32
#define WL_SOCKET_CONNECTED (1 << 6)
#else
/* avoid having to deal with case on platforms not requiring it */
#define WL_SOCKET_CONNECTED WL_SOCKET_WRITEABLE
#endif
#define WL_SOCKET_CLOSED (1 << 7)
#ifdef WIN32
#define WL_SOCKET_ACCEPT (1 << 8)
#else
/* avoid having to deal with case on platforms not requiring it */
#define WL_SOCKET_ACCEPT WL_SOCKET_READABLE
#endif
#define WL_SOCKET_MASK                                                                   \
    (WL_SOCKET_READABLE | WL_SOCKET_WRITEABLE | WL_SOCKET_CONNECTED | WL_SOCKET_ACCEPT | \
     WL_SOCKET_CLOSED)

typedef struct WaitEvent {
    int pos;         /* position in the event data structure */
    uint32 events;   /* triggered events */
    pgsocket fd;     /* socket fd associated with event */
    void* user_data; /* pointer provided in AddWaitEventToSet */
#ifdef WIN32
    bool reset; /* Is reset of the event required? */
#endif
} WaitEvent;

namespace Spq {
class IOMultiplexor;
}

struct WaitEventSet {
    int nevents; /* number of registered events */

    /*
     * Array, of nevents_space length, storing the definition of events this
     * set is waiting for.
     */
    WaitEvent* events;

    /*
     * If WL_LATCH_SET is specified in any wait event, latch is a pointer to
     * said latch, and latch_pos the offset in the ->events array. This is
     * useful because we check the state of the latch before performing doing
     * syscalls related to waiting.
     */
    Latch* latch;
    int latch_pos;

    Spq::IOMultiplexor* m_IOMultiplexor;
};

extern WaitEventSet* CreateWaitEventSet(ResourceOwner resowner, int nevents);
extern void FreeWaitEventSet(WaitEventSet* set);
extern void FreeWaitEventSetAfterFork(WaitEventSet* set);
extern int AddWaitEventToSet(WaitEventSet* set, uint32 events, pgsocket fd, Latch* latch,
                             void* user_data);
extern void ModifyWaitEvent(WaitEventSet* set, int pos, uint32 events, Latch* latch);

extern int WaitEventSetWait(WaitEventSet* set, long timeout, WaitEvent* occurred_events,
                            int nevents, uint32 wait_event_info);
extern int GetNumRegisteredWaitEvents(WaitEventSet* set);
extern bool WaitEventSetCanReportClosed(void);
extern void InitialzeCitusWaitEventSet();

extern int WaitLatchOrSocket(Latch* latch, int wakeEvents, pgsocket sock, long timeout,
                             uint32 wait_event_info);
class WaitEventSetGuard {
public:
    WaitEventSetGuard() : m_set_ptr(nullptr)
    {}

    void Reset(WaitEventSet* set)
    {
        if (m_set_ptr != nullptr) {
            FreeWaitEventSet(m_set_ptr);
        }
        m_set_ptr = set;
    }

    ~WaitEventSetGuard()
    {
        if (m_set_ptr != nullptr) {
            FreeWaitEventSet(m_set_ptr);
            m_set_ptr = nullptr;
        }
    }

private:
    WaitEventSet* m_set_ptr{nullptr};
};
#endif