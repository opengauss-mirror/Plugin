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

#include "distributed/utils/spq_lwlock.h"
#include "knl/knl_thread.h"

namespace Spq {

static constexpr int INVALID_TRANCHE_ID = -1;

struct SpqLWLockTranche {
    int trancheId;

    const char* trancheName;
};

static SpqLWLockTranche spqLwlockTranches[] = {
    {INVALID_TRANCHE_ID, "Spq backend data tranche"},
    {INVALID_TRANCHE_ID, "Shared Connection Tracking Hash Tranche"},
    {INVALID_TRANCHE_ID, "Spq query stat tranche"}};

static_assert(sizeof(spqLwlockTranches) / sizeof(SpqLWLockTranche) == SPQ_INVALID_LWLOCK,
              "The Spq's spqLwlockTranches is not matched with SpqLWLockTrancheIndex");

void InitTrancheLwlock(SpqLWLockTrancheIndex lockIndex, LWLock* lock)
{
    Assert(LWLockHeldByMe(AddinShmemInitLock));
    Assert(lockIndex < SPQ_INVALID_LWLOCK);

    /* get the smallest thrancheId in LWLockTrancheArray that is not registered for
    extensions. The Id search begins at LWTRANCHE_NATIVE_TRANCHE_NUM which is bigger than
    biggest builtin trancheId. */
    int spqThrancheId = LWTRANCHE_NATIVE_TRANCHE_NUM;
    while (LWLockTrancheArray[spqThrancheId] != NULL) {
        spqThrancheId++;
    }

    LWLockRegisterTranche(spqThrancheId, spqLwlockTranches[lockIndex].trancheName);
    LWLockInitialize(lock, spqThrancheId);
    spqLwlockTranches[lockIndex].trancheId = spqThrancheId;
    Assert(spqLwlockTranches[lockIndex].trancheId > 0);
}

}  // namespace Spq