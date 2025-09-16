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
 *
 * condition_variables.h
 *
 * --------------------------------------------------------------------------------------
 */
#ifndef CONDITION_VARIABLE_H
#define CONDITION_VARIABLE_H

#include "postgres.h"

/** Class like std::condition_variable, but the signal is controlled by procLatch */
class ConditionVariable {
public:
    /** Constructor  */
    ConditionVariable()
    {
        SpinLockInit(&m_mutex);
    }

    /** Deconstructor  */
    ~ConditionVariable()
    {
        SpinLockFree(&m_mutex);
    }

    /** Wait on current condtion variable util current thread is wakeup by other thread.
     */
    void Wait()
    {
        WaitFor(-1);
    }

    /** Wait on current condition variable util current thread is wakeup by other thread
    or for a given specified timeout
    @param[in]      timeout(ms)     the timeout for waiting on current condition
    variable.*/
    void WaitFor(long timeout);

    /** notify all waiting backend proc thread. */
    void NotifyAll();

private:
    /** Add current thread into waiting list, the proc will be wakeup by other thread.
     @param[in]  pgProcNo the proc no in global proc array. */
    void AddProcToBeNotified(int pgProcNo);

    /** Whether or not the proc is in waiting list.
     @param[in] pgProcNo the proc no in global proc array.
     @return true if the proc is waiting for nofication. */
    bool ProcIsWaiting(int pgProcNo);

    /** Remove the proc in waiting list.
     @param[in] pgProcNo the proc no in global proc array. */
    void ReleaseProc(int pgProcNo);

    /** The spin lock to protect the following m_waitProcs list. */
    slock_t m_mutex{};

    /** The waiting list, all procs in this list need to be notified. */
    List* m_waitProcs{NIL};
};

#endif