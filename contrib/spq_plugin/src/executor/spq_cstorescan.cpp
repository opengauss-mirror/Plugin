/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

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