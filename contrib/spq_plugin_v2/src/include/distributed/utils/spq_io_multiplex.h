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

#ifndef SPQ_IO_MULTIPLEX
#define SPQ_IO_MULTIPLEX

#include "postgres.h"
#include "distributed/utils/wait_events.h"

namespace Spq {

enum IOMultiplexType { SPQ_IO_MULTIPLEX_POLL = 0, SPQ_IO_MULTIPLEX_EPOLL = 1 };

const struct config_enum_entry spq_io_multiplex_model_options[] = {
    {"poll", SPQ_IO_MULTIPLEX_POLL, false},
    {"epoll", SPQ_IO_MULTIPLEX_EPOLL, false},
    {NULL, 0, false}};

extern int SpqIOMultiplexModel;

/** The base class for IO multiplex, Now, we have 'poll' and 'epoll' implements. */
class IOMultiplexor {
public:
    /** Create a IO multiplexor
     @param[in]  nevents    the number of event need to be listened.
     @param[in]  mem        memory context for current multiplexor.
     @return  a IOMultiplexor wrapper including 'poll' or 'epoll'*/
    static IOMultiplexor* CreateIOMultiplexor(int nevents, MemoryContext mem);

    /** Create all events's memory structure. */
    virtual void CreateEvents() = 0;

    /** Init a WaitEvent
     @param[in]  event       the waitevent needs to be initialized
     @param[in]  isModify    whether or not invoked by modifing operation. */
    virtual void InitWaitEvent(WaitEvent* event, bool isModify) = 0;

    /** Wait some waitevent's result.
     @param[in]     set             the waiteventset owned current IOMultiplexor
     @param[in]     curTimeout      timeout for waiting.
     @param[out]    occurredEvents  all occurred events
     @param[in]     expectEvents    the number of event epected to occurr
     @return the number of events received. */
    virtual int WaitEvents(WaitEventSet* set, int curTimeout, WaitEvent* occurredEvents,
                           int expectEvents) = 0;

    /** free some resources */
    virtual void Free(){};

    /** Get the number of all registerred events. */
    size_t GetEvents() const
    {
        return m_nEvents;
    }

protected:
    /*
     * Read all available data from self-pipe or signalfd.
     *
     * Note: this is only called when waiting = true.  If it fails and doesn't
     * return, it must reset that flag first (though ideally, this will never
     * happen).
     */
    void drain();

    /** Constructor
     @param[in]  nevents    the number of event need to be listened.
     @param[in]  mem        memory context for current multiplexor.*/
    IOMultiplexor(int nevents, MemoryContext mem) : m_nEvents(nevents), m_mem(mem)
    {}

    /** the number of registerred events. */
    size_t m_nEvents{0};

    /** memory context for current multiplexor. */
    MemoryContext m_mem{nullptr};
};

}  // namespace Spq

#endif