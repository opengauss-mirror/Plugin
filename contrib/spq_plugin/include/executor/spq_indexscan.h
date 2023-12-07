/* -------------------------------------------------------------------------
* spq_indexscan.h
*
* Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
*
* -------------------------------------------------------------------------
 */
#ifndef SPQ_INDEXSCAN_H
#define SPQ_INDEXSCAN_H

#include "nodes/execnodes.h"
#include "executor/node/nodeSpqIndexscan.h"

IndexScanState* ExecInitSpqIndexScan(SpqIndexScan* node, EState* estate, int eflags);
TupleTableSlot* ExecSpqIndexScan(PlanState* state);
TupleTableSlot* SpqIndexNext(IndexScanState* node);

void init_spqindexscan_hook();
void restore_spqindexscan_hook();

#endif  // SPQ_INDEXSCAN_H
