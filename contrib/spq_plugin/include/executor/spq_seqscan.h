/* -------------------------------------------------------------------------
* spq_seqscan.h
*
* Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
* Portions Copyright (c) 1994, Regents of the University of California
*
* -------------------------------------------------------------------------
 */
#ifndef SPQ_SEQSCAN_H
#define SPQ_SEQSCAN_H

#include "executor/node/nodeSpqSeqscan.h"
#include "nodes/execnodes.h"

SpqSeqScanState* ExecInitSpqSeqScan(SpqSeqScan* node, EState* estate, int eflags);
TupleTableSlot* ExecSpqSeqScan(PlanState* node);
void ExecEndSpqSeqScan(SpqSeqScanState* node);
void ExecReScanSpqSeqScan(SpqSeqScanState* node);

void init_spqseqscan_hook();
void restore_spqseqscan_hook();

#endif  // SPQ_SEQSCAN_H