/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "postgres.h"

#include "access/sysattr.h"
#include "catalog/pg_type.h"
#include "nodes/ag_extensible.h"
#include "nodes/nodes.h"
#include "nodes/pg_list.h"
#include "nodes/plannodes.h"
#include "nodes/relation.h"

#include "executor/cypher_executor.h"
#include "optimizer/cypher_createplan.h"

const ExtensiblePlanMethods cypher_create_plan_methods = {
    "Cypher Create", create_cypher_create_plan_state};
const ExtensiblePlanMethods cypher_set_plan_methods = {
    "Cypher Set", create_cypher_set_plan_state};
const ExtensiblePlanMethods cypher_delete_plan_methods = {
    "Cypher Delete", create_cypher_delete_plan_state};
const ExtensiblePlanMethods cypher_merge_plan_methods = {
    "Cypher Merge", create_cypher_merge_plan_state};

Plan *plan_cypher_create_path(PlannerInfo *root, RelOptInfo *rel,
                              ExtensiblePath *best_path, List *tlist,
                              List *clauses, List *custom_plans)
{
    ExtensiblePlan *cs;
    Plan *subplan = (Plan *)linitial(custom_plans);

    cs = makeNode(ExtensiblePlan);

    cs->scan.plan.startup_cost = best_path->path.startup_cost;
    cs->scan.plan.total_cost = best_path->path.total_cost;

    cs->scan.plan.plan_rows = best_path->path.rows;
    cs->scan.plan.plan_width = 0;

    cs->scan.plan.plan_node_id = 0; // Set later in set_plan_refs
    cs->scan.plan.targetlist = tlist;
    cs->scan.plan.qual = NIL;
    cs->scan.plan.lefttree = NULL;
    cs->scan.plan.righttree = NULL;
    cs->scan.plan.initPlan = NIL;

    cs->scan.plan.extParam = NULL;
    cs->scan.plan.allParam = NULL;

    cs->scan.scanrelid = 0;

    cs->flags = best_path->flags;

    cs->extensible_plans = custom_plans;
    cs->extensible_exprs = NIL;
    cs->extensible_private = best_path->extensible_private;
    cs->extensible_plan_tlist = subplan->targetlist;
    cs->extensible_relids = NULL;
    cs->methods = (ExtensiblePlanMethods *)&cypher_create_plan_methods;

    return &cs->scan.plan;
}

Plan *plan_cypher_set_path(PlannerInfo *root, RelOptInfo *rel,
                           ExtensiblePath *best_path, List *tlist,
                           List *clauses, List *custom_plans)
{
    ExtensiblePlan *cs;
    Plan *subplan = (Plan *)linitial(custom_plans);

    cs = makeNode(ExtensiblePlan);

    cs->scan.plan.startup_cost = best_path->path.startup_cost;
    cs->scan.plan.total_cost = best_path->path.total_cost;

    cs->scan.plan.plan_rows = best_path->path.rows;
    cs->scan.plan.plan_width = 0;

    cs->scan.plan.plan_node_id = 0; // Set later in set_plan_refs
    cs->scan.plan.targetlist = tlist;
    cs->scan.plan.qual = NIL;
    cs->scan.plan.lefttree = NULL;
    cs->scan.plan.righttree = NULL;
    cs->scan.plan.initPlan = NIL;

    cs->scan.plan.extParam = NULL;
    cs->scan.plan.allParam = NULL;

    cs->scan.scanrelid = 0;

    cs->flags = best_path->flags;

    cs->extensible_plans = custom_plans;
    cs->extensible_exprs = NIL;
    cs->extensible_private = best_path->extensible_private;
    cs->extensible_plan_tlist = subplan->targetlist;

    cs->extensible_relids = NULL;
    cs->methods = (ExtensiblePlanMethods *)&cypher_set_plan_methods;

    return (Plan *)cs;
}

/*
 * Coverts the Scan node representing the delete clause
 * to the delete Plan node
 */
Plan *plan_cypher_delete_path(PlannerInfo *root, RelOptInfo *rel,
                              ExtensiblePath *best_path, List *tlist,
                              List *clauses, List *custom_plans)
{
    ExtensiblePlan *cs;
    Plan *subplan = (Plan *)linitial(custom_plans);

    cs = makeNode(ExtensiblePlan);

    cs->scan.plan.startup_cost = best_path->path.startup_cost;
    cs->scan.plan.total_cost = best_path->path.total_cost;

    cs->scan.plan.plan_rows = best_path->path.rows;
    cs->scan.plan.plan_width = 0;

    cs->scan.plan.plan_node_id = 0; // Set later in set_plan_refs
    /*
     * the scan list of the delete node, used for its ScanTupleSlot used
     * by its parent in the execution phase.
     */
    cs->scan.plan.targetlist = tlist;
    cs->scan.plan.qual = NIL;
    cs->scan.plan.lefttree = NULL;
    cs->scan.plan.righttree = NULL;
    cs->scan.plan.initPlan = NIL;

    cs->scan.plan.extParam = NULL;
    cs->scan.plan.allParam = NULL;

    /*
     * We do not want Postgres to assume we are scanning a table, postgres'
     * optimizer will make assumptions about our targetlist that are false
     */
    cs->scan.scanrelid = 0;

    cs->flags = best_path->flags;

    // child plan nodes are here, Postgres processed them for us.
    cs->extensible_plans = custom_plans;
    cs->extensible_exprs = NIL;
    // transfer delete metadata needed by the delete clause.
    cs->extensible_private = best_path->extensible_private;
    /*
     * the scan list of the delete node's children, used for ScanTupleSlot
     * in execution.
     */
    cs->extensible_plan_tlist = subplan->targetlist;

    cs->extensible_relids = NULL;

    cs->methods = (ExtensiblePlanMethods *)&cypher_delete_plan_methods;

    return (Plan *)cs;
}

/*
 * Coverts the Scan node representing the delete clause
 * to the merge Plan node
 */
Plan *plan_cypher_merge_path(PlannerInfo *root,
                             RelOptInfo *rel,
                             ExtensiblePath *best_path,
                             List *tlist,
                             List *clauses,
                             List *custom_plans)
{
    ExtensiblePlan *cs;
    Plan *subplan = (Plan *)linitial(custom_plans);

    cs = makeNode(ExtensiblePlan);

    cs->scan.plan.startup_cost = best_path->path.startup_cost;
    cs->scan.plan.total_cost = best_path->path.total_cost;

    cs->scan.plan.plan_rows = best_path->path.rows;
    cs->scan.plan.plan_width = 0;

    cs->scan.plan.plan_node_id = 0; // Set later in set_plan_refs
    /*
     * the scan list of the delete node, used for its ScanTupleSlot used
     * by its parent in the execution phase.
     */
    cs->scan.plan.targetlist = tlist;
    cs->scan.plan.qual = NIL;
    cs->scan.plan.lefttree = NULL;
    cs->scan.plan.righttree = NULL;
    cs->scan.plan.initPlan = NIL;

    cs->scan.plan.extParam = NULL;
    cs->scan.plan.allParam = NULL;

    /*
     * We do not want Postgres to assume we are scanning a table, postgres'
     * optimizer will make assumptions about our targetlist that are false
     */
    cs->scan.scanrelid = 0;

    cs->flags = best_path->flags;

    // child plan nodes are here, Postgres processed them for us.
    cs->extensible_plans = custom_plans;
    cs->extensible_exprs = NIL;
    // transfer delete metadata needed by the delete clause.
    cs->extensible_private = best_path->extensible_private;
    /*
     * the scan list of the merge node's children, used for ScanTupleSlot
     * in execution.
     */
    cs->extensible_plan_tlist = subplan->targetlist;

    cs->extensible_relids = NULL;
    cs->methods = (ExtensiblePlanMethods *)&cypher_merge_plan_methods;

    return (Plan *)cs;
}
