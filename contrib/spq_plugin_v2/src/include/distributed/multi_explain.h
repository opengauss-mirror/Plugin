/*-------------------------------------------------------------------------
 *
 * multi_explain.h
 *	  Explain support for Citus.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#ifndef MULTI_EXPLAIN_H
#define MULTI_EXPLAIN_H

#include "tuple_destination.h"

#include "executor/executor.h"

typedef enum {
    EXPLAIN_ANALYZE_SORT_BY_TIME = 0,
    EXPLAIN_ANALYZE_SORT_BY_TASK_ID = 1
} ExplainAnalyzeSortMethods;

// @FIXME: copied from pg 17, we don't need it.
/*
 * Private state of a query environment.
 */
struct QueryEnvironment {
    List* namedRelList;
};

extern void FreeSavedExplainPlan(void);
extern List* ExplainAnalyzeTaskList(List* originalTaskList,
                                    TupleDestination* defaultTupleDest,
                                    TupleDesc tupleDesc, ParamListInfo params);
extern bool RequestedForExplainAnalyze(CitusScanState* node);
extern void ResetExplainAnalyzeData(List* taskList);

#endif /* MULTI_EXPLAIN_H */
