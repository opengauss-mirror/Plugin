/* -------------------------------------------------------------------------
 *
 * spq_plan.h
 * 	  definitions for spq_plan.cpp utilities
 *
 * Portions Copyright (c) 2004-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 * 	    src/include/spq/spq_plan.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef SPQ_PLAN_H
#define SPQ_PLAN_H

#include "optimizer/planmem_walker.h"

extern Node *plan_tree_mutator(Node *node, Node *(*mutator)(Node *, void *), void *context, bool recurse_into_subplans);

extern Value *get_tle_name(TargetEntry *tle, List *rtable, const char *default_name);

#endif /* SPQ_PLAN_H */
