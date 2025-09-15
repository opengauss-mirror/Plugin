/*-------------------------------------------------------------------------
 *
 * combine_query_planner.h
 *	  Function declarations for building planned statements; these statements
 *	  are then executed on the coordinator node.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef COMBINE_QUERY_PLANNER_H
#define COMBINE_QUERY_PLANNER_H

#include "lib/stringinfo.h"
#include "nodes/parsenodes.h"
#include "nodes/relation.h"
#include "nodes/plannodes.h"

/* Function declarations for building local plans on the coordinator node */
struct DistributedPlan;
struct ExtensiblePlan;
extern Path* CreateCitusCustomScanPath(PlannerInfo* root, RelOptInfo* relOptInfo,
                                       Index restrictionIndex, RangeTblEntry* rte,
                                       ExtensiblePlan* remoteScan);
extern PlannedStmt* PlanCombineQuery(struct DistributedPlan* distributedPlan,
                                     struct ExtensiblePlan* dataScan);
extern bool FindCitusExtradataContainerRTE(Node* node, RangeTblEntry** result);

#endif /* COMBINE_QUERY_PLANNER_H */
