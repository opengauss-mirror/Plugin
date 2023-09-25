/* -------------------------------------------------------------------------
 *
 * spq_mutate.h
 *    definitions for spq_mutate.cpp utilities
 *
 * Portions Copyright (c) 2005-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 *     src/include/spq/spq_mutate.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef SPQ_MUTATE_H
#define SPQ_MUTATE_H

#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "nodes/plannodes.h"

extern void collect_shareinput_producers(PlannerInfo *root, Plan *plan);
extern Plan *replace_shareinput_targetlists(PlannerInfo *root, Plan *plan);
extern Plan *apply_shareinput_xslice(Plan *plan, PlannerInfo *root);
extern void remove_subquery_in_RTEs(Node *node);
extern bool is_plan_node(Node *node);
extern void make_spq_remote_query(PlannerInfo *root, PlannedStmt *result, PlannerGlobal *glob);
extern Plan *replace_motion_stream_recurse(PlannerInfo* root, Plan *plan, bool &top);

#endif /* SPQ_MUTATE_H */
