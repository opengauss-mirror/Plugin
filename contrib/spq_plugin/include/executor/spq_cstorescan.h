/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 */

#ifndef SPQ_CSTORESCAN_H
#define SPQ_CSTORESCAN_H

#ifdef ENABLE_HTAP
#include "nodes/execnodes.h"
#include "vecexecutor/vecspqcstorescan.h"

IMCStoreScanState* ExecInitSpqCStoreScan(
    SpqCStoreScan* node, Relation parent_heap_rel, EState* estate, int eflags, bool codegen_in_up_level);
VectorBatch* ExecSpqCStoreScan(PlanState* state);

void init_spqcstorescan_hook();
void restore_spqcstorescan_hook();
#endif
#endif  // SPQ_CSTORESCAN_H