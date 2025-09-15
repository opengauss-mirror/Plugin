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
 * condition_variables.cc
 *
 * --------------------------------------------------------------------------------------
 */
#include "distributed/utils/condition_variable.h"
#include "miscadmin.h"
#include "portability/instr_time.h"
#include "storage/ipc.h"
#include "storage/proc.h"

void ConditionVariable::WaitFor(long timeout)
{
    int wait_events = WL_LATCH_SET | WL_POSTMASTER_DEATH;
    auto currentProc = t_thrd.proc->pgprocno;
    long cur_timeout = -1;
    instr_time start_time;
    instr_time cur_time;

    Assert(!ProcIsWaiting(currentProc));

    if (timeout >= 0) {
        Assert(timeout >= 0 && timeout <= INT_MAX);
        INSTR_TIME_SET_CURRENT(start_time);
        wait_events |= WL_TIMEOUT;
        cur_timeout = timeout;
    }

    ResetLatch(&t_thrd.proc->procLatch);

    AddProcToBeNotified(currentProc);

    while (true) {
        /*
         * Wait for latch to be set.  (If we're awakened for some other
         * reason, the code below will cope anyway.)
         */
        int rc = WaitLatch(&t_thrd.proc->procLatch, wait_events, cur_timeout);

        if (((unsigned int)rc) & WL_POSTMASTER_DEATH) {
            ReleaseProc(currentProc);
            proc_exit(1);
        }

        if (((unsigned int)rc) & WL_TIMEOUT) {
            ReleaseProc(currentProc);
            break;
        }

        Assert(((unsigned int)rc) & WL_LATCH_SET);

        if (!ProcIsWaiting(currentProc)) {
            break;
        }

        /* if signaled by other events, continue waiting.... */
        CHECK_FOR_INTERRUPTS();

        ResetLatch(&t_thrd.proc->procLatch);

        /* If we're not done, update cur_timeout for next iteration */
        if (timeout >= 0) {
            INSTR_TIME_SET_CURRENT(cur_time);
            INSTR_TIME_SUBTRACT(cur_time, start_time);
            cur_timeout = timeout - (long)INSTR_TIME_GET_MILLISEC(cur_time);

            /* Have we crossed the timeout threshold? */
            if (cur_timeout <= 0) {
                ReleaseProc(currentProc);
                break;
            }
        }

        /* double check. */
        if (!ProcIsWaiting(currentProc)) {
            break;
        }
    }

    return;
}

void ConditionVariable::NotifyAll()
{
    int currentProc = t_thrd.proc->pgprocno;

    Assert(!ProcIsWaiting(currentProc));

    SpinLockAcquire(&m_mutex);

    if (list_length(m_waitProcs) == 0) {
        SpinLockRelease(&m_mutex);
        return;
    }

    /* add sentinel */
    m_waitProcs = lappend_int(m_waitProcs, currentProc);

    while (list_length(m_waitProcs) > 0) {
        int pgProcNo = linitial_int(m_waitProcs);
        m_waitProcs = list_delete_first(m_waitProcs);

        SpinLockRelease(&m_mutex);

        /* find the sentinel, we have notified all procs as many as possible, return
         * quickly. */
        if (pgProcNo == currentProc) {
            return;
        }

        auto proc = g_instance.proc_base_all_procs[pgProcNo];
        Assert(proc != nullptr);
        SetLatch(&proc->procLatch);

        SpinLockAcquire(&m_mutex);
    }

    ResetLatch(&t_thrd.proc->procLatch);

    SpinLockRelease(&m_mutex);
}

void ConditionVariable::AddProcToBeNotified(int pgProcNo)
{
    SpinLockAcquire(&m_mutex);
    m_waitProcs = lappend_int(m_waitProcs, pgProcNo);
    SpinLockRelease(&m_mutex);
}

bool ConditionVariable::ProcIsWaiting(int pgProcNo)
{
    bool waiting = false;
    SpinLockAcquire(&m_mutex);
    waiting = list_member_int(m_waitProcs, pgProcNo);
    SpinLockRelease(&m_mutex);

    return waiting;
}

void ConditionVariable::ReleaseProc(int pgProcNo)
{
    SpinLockAcquire(&m_mutex);
    m_waitProcs = list_delete_int(m_waitProcs, pgProcNo);
    SpinLockRelease(&m_mutex);
}