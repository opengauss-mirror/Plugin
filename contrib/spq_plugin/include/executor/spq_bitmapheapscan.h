/* -------------------------------------------------------------------------
* spq_bitmap_heapscan.h
*
* Portions Copyright (c) 2023, Huawei Technologies Co.,Ltd.
*
* -------------------------------------------------------------------------
 */
#ifndef SPQ_BITMAPHEAPSCAN_H
#define SPQ_BITMAPHEAPSCAN_H

#include "nodes/execnodes.h"
#include "executor/node/nodeSpqBitmapHeapscan.h"

IndexScanState* ExecSpqInitBitmapHeapScan(BitmapHeapScan* node, EState* estate, int eflags);
TupleTableSlot* ExecSpqBitmapHeapScan(PlanState* state);
TupleTableSlot* BitmapHeapTblNext(BitmapHeapScanState* node);

void init_spqbitmapheapscan_hook();
void restore_spqbitmapheapscan_hook();

#endif  // SPQ_BITMAPHEAPSCAN_H
