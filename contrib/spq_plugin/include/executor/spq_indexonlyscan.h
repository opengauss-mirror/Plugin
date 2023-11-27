/* -------------------------------------------------------------------------
* spq_indexonlyscan.h
* 
* Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
* -------------------------------------------------------------------------
 */
#ifndef SPQ_INDEXONLYSCAN_H
#define SPQ_INDEXONLYSCAN_H

#include "nodes/execnodes.h"
#include "executor/node/nodeSpqIndexonlyscan.h"

IndexScanState* ExecInitSpqIndexOnlyScan(SpqIndexScan* node, EState* estate, int eflags);
TupleTableSlot* ExecSpqIndexOnlyScan(PlanState* state);
TupleTableSlot* SpqIndexOnlyNext(IndexOnlyScanState* node);

void init_spqindexonlyscan_hook();
void restore_spqindexonlyscan_hook();

#endif  // SPQ_INDEXONLYSCAN_H
