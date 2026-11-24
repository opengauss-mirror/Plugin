/*
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * Plugin is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/* All rights reserved. */

#include "postgres.h"
#include "knl/knl_variable.h"
#include "executor/executor.h"
#include "vecexecutor/vecnodecstorescan.h"
#include "vecexecutor/vecnodeimcstorescan.h"
#include "access/tableam.h"
#include "executor/spq_cstorescan.h"

#ifdef ENABLE_HTAP

THR_LOCAL init_spqcstorescan_hook_type backup_init_spqcstorescan_hook = NULL;
THR_LOCAL exec_spqcstorescan_hook_type backup_exec_spqcstorescan_hook = NULL;

/* ----------------------------------------------------------------
*		ExecSpqCStoreScan(node)
* ----------------------------------------------------------------
*/
VectorBatch *ExecSpqCStoreScan(PlanState *state)
{
    IMCStoreScanState *node = castNode(IMCStoreScanState, state);
    return ExecCStoreScan(node);
}

IMCStoreScanState *ExecInitSpqCStoreScan(SpqCStoreScan *node, Relation parent_heap_rel, EState *estate, int eflags,
    bool codegen_in_up_level)
{
    IMCStoreScanState *cstoreScan =
        ExecInitIMCStoreScan((IMCStoreScan *)node, parent_heap_rel, estate, eflags, codegen_in_up_level);
    return cstoreScan;
}

void init_spqcstorescan_hook()
{
    backup_init_spqcstorescan_hook = init_cstorescan_hook;
    backup_exec_spqcstorescan_hook = exec_cstorescan_hook;
    init_cstorescan_hook = ExecInitSpqCStoreScan;
    exec_cstorescan_hook = ExecSpqCStoreScan;
}

void restore_spqcstorescan_hook()
{
    init_cstorescan_hook = backup_init_spqcstorescan_hook;
    exec_cstorescan_hook = backup_exec_spqcstorescan_hook;
}
#endif