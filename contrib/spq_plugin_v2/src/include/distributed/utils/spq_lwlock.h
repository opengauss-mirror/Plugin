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

/** helper tools for all opengauss's lwlock functions. */
#ifndef SPQ_LWLOCK_H
#define SPQ_LWLOCK_H

#include "postgres.h"
#include "storage/lock/lwlock.h"

namespace Spq {

/** All Spq plugin's lwlock, the element value is the index of array spqLwlockTranches */
enum SpqLWLockTrancheIndex {
    /** The lwlock for backend data */
    BACKEND_DATA_LWLOCK = 0,

    /** The Lwlock for shared connection stat's hash table. */
    SHARE_CONN_HASH_LWLOCK = 1,

    /** The Lwlock for query stat */
    QUERY_STAT_LWLOCK = 2,

    /** Invalid tranche lock index */
    SPQ_INVALID_LWLOCK = 3
};

/** Initialize lwlock and registered it to dynamic LWLockTrancheArray.
Note: The caller must hold LW_EXCLUSIVE AddinShmemInitLock
 @param[in]     lockIndex   The tranche lock index in SpqLWLockTrancheIndex
 @param[in/out] lock        The lock pointer. */
void InitTrancheLwlock(SpqLWLockTrancheIndex lockIndex, LWLock* lock);

} /* end of namespace Spq */
#endif