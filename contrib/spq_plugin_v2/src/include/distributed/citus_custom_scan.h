/*-------------------------------------------------------------------------
 *
 * citus_custom_scan.h
 *	  Export all custom scan and custom exec methods.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#ifndef CITUS_CUSTOM_SCAN_H
#define CITUS_CUSTOM_SCAN_H

#include "executor/exec/execdesc.h"
#include "nodes/plannodes.h"

#include "distributed/distributed_planner.h"
#include "distributed/multi_server_executor.h"

typedef struct CitusScanState {
    ExtensiblePlanState customScanState; /* underlying custom scan node */

    /* function that gets called before postgres starts its execution */
    bool finishedPreScan; /* flag to check if the pre scan is finished */
    void (*PreExecScan)(struct CitusScanState* scanState);

    DistributedPlan* distributedPlan; /* distributed execution plan */
    MultiExecutorType executorType;   /* distributed executor type */
    bool finishedRemoteScan;          /* flag to check if remote scan is finished */
    Tuplestorestate* tuplestorestate; /* tuple store to store distributed results */
} CitusScanState;

/* custom scan methods for all executors */
extern ExtensiblePlanMethods AdaptiveExecutorCustomScanMethods;
extern ExtensiblePlanMethods NonPushableInsertSelectCustomScanMethods;
extern ExtensiblePlanMethods DelayedErrorCustomScanMethods;
extern ExtensiblePlanMethods NonPushableMergeCommandCustomScanMethods;

extern void RegisterCitusCustomScanMethods(void);
extern void CitusExplainScan(ExtensiblePlanState* node, List* ancestors,
                             struct ExplainState* es);
extern TupleDesc ScanStateGetTupleDescriptor(CitusScanState* scanState);
extern EState* ScanStateGetExecutorState(CitusScanState* scanState);

extern ExtensiblePlan* FetchCitusCustomScanIfExists(Plan* plan);
extern bool IsCitusPlan(Plan* plan);
extern bool IsCitusCustomScan(Plan* plan);

extern void SetJobColocationId(Job* job);

#endif /* CITUS_CUSTOM_SCAN_H */
