/*
 * execGraphVle.c
 *	  AgensGraph VLE Executor.
 *
 * Copyright (c) 2022 by Bitnine Global, Inc.
 *
 * IDENTIFICATION
 *	  src/backend/executor/execGraphVle.c
 */

#include "postgres.h"

#include "access/htup.h"
#include "access/tableam.h"
#include "access/xact.h"
#include "executor/tuptable.h"
#include "nodes/execnodes.h"
#include "nodes/ag_extensible.h"
#include "nodes/nodes.h"
#include "nodes/plannodes.h"
#include "parser/parse_relation.h"
#include "rewrite/rewriteHandler.h"
#include "utils/rel.h"
#include "executor/executor.h"
#include "utils/lsyscache.h"

#include "catalog/ag_label.h"
#include "executor/cypher_executor.h"
#include "executor/cypher_utils.h"
#include "nodes/cypher_nodes.h"
#include "utils/agtype.h"
#include "utils/ag_cache.h"
#include "utils/graphid.h"
#include "utils/ag_func.h"
#include "commands/label_commands.h"
#include "utils/snapmgr.h"
#include "catalog/pg_inherits_fn.h"
#define VAR_START_VID	0
#define VAR_END_VID		1
#define VAR_EDGE_IDS	2
#define VAR_EDGES		3
#define VAR_VERTICES	4
#define F_GRAPHID_EQ \
    (get_ag_func_oid("graphid_eq",2,GRAPHIDOID,GRAPHIDOID ))

#define VLERel(vleplan) ((cypher_relationship *) ((vleplan)->vle_rel))

#define MAXIMUM_OUTPUT_DEPTH_UNLIMITED  (INT_MAX)

#define make_edge_from_tuple(slot,labelname) (AGTYPE_P_GET_DATUM(make_edge( \
									(slot)->tts_values[Anum_ag_label_edge_table_id - 1], \
									(slot)->tts_values[Anum_ag_label_edge_table_start_id - 1], \
									(slot)->tts_values[Anum_ag_label_edge_table_end_id - 1], \
									string_to_agtype(labelname), \
									((slot)->tts_values[Anum_ag_label_edge_table_properties - 1]) \
									)))

static void ExecInitGraphVLE(ExtensiblePlanState *node, EState *estate,
                                int eflags);
static TupleTableSlot *ExecGraphVLE(ExtensiblePlanState *pstate);
static void ExecReScanGraphVLE(ExtensiblePlanState *node);
static void ExecEndGraphVLE(ExtensiblePlanState *node);

const ExtensibleExecMethods cypher_vle_exec_methods = {VLE_SCAN_STATE_NAME,
    ExecInitGraphVLE,
    ExecGraphVLE,
    ExecEndGraphVLE,
    ExecReScanGraphVLE,
    NULL};

Node *create_cypher_vle_plan_state(ExtensiblePlan *cscan)
{
        cypher_vle_custom_scan_state *cypher_css =
        (cypher_vle_custom_scan_state*)palloc0(sizeof(cypher_vle_custom_scan_state));
		cypher_vle_target_nodes *target_nodes;
		char *serialized_data;
		Const *c;

		cypher_css->cs = cscan;

		// get the serialized data structure from the Const and deserialize it.
		c = (Const*)linitial(cscan->extensible_private);
		serialized_data = (char *)c->constvalue;
		target_nodes = (cypher_vle_target_nodes*)stringToAGNode(serialized_data);

		Assert(is_ag_node(target_nodes, cypher_vle_target_nodes));

		cypher_css->css.ss.ps.type = T_ExtensiblePlanState;
		cypher_css->css.methods = &cypher_vle_exec_methods;

		cypher_css->need_new_sp_tuple = true;
		cypher_css->minimum_output_depth = target_nodes->minimum_output_depth;
		cypher_css->maximum_output_depth = target_nodes->maximum_output_depth;
		cypher_css->cypher_rel_direction = target_nodes->cypher_rel_direction;
		cypher_css->label_name =(char*) ( strlen (target_nodes->label_name) == 0 ? AG_DEFAULT_LABEL_EDGE:target_nodes->label_name);
		cypher_css->graph_oid = target_nodes->graph_oid;
        cypher_css->edge_property_constraint_expr = target_nodes->edge_property_constraint;

		


    return (Node *)cypher_css;
}
/*
 * Check that the specified List is valid (so far as we can tell).
 */
static void check_list_invariants(const List *list)
{
    if (list == NIL) {
        return;
    }

    Assert(list->length > 0);
    Assert(list->head != NULL);
    Assert(list->tail != NULL);

    Assert(list->type == T_List || list->type == T_IntList || list->type == T_OidList);

    if (list->length == 1) {
        Assert(list->head == list->tail);
    }
    if (list->length == 2) {
        Assert(list->head->next == list->tail);
    }
    Assert(list->tail->next == NULL);
}
static List *
list_delete_last(List *list)
{
	check_list_invariants(list);

	if (list == NIL)
		return NIL;				/* would an error be better? */

	/* list_truncate won't free list if it goes to empty, but this should */
	if (list_length(list) <= 1)
	{
		list_free(list);
		return NIL;
	}

	return list_truncate(list, list_length(list) - 1);
}

static ArrayBuildState *
initArrayResult(Oid element_type, MemoryContext rcontext, bool subcontext)
{
	ArrayBuildState *astate;
	MemoryContext arr_context = rcontext;

	/* Make a temporary context to hold all the junk */
	if (subcontext)
		arr_context = AllocSetContextCreate(rcontext,
											"accumArrayResult",
											ALLOCSET_DEFAULT_SIZES);

	astate = (ArrayBuildState *)
		MemoryContextAlloc(arr_context, sizeof(ArrayBuildState));
	astate->mcontext = arr_context;
	astate->alen = (subcontext ? 64 : 8);	/* arbitrary starting array size */
	astate->dvalues = (Datum *)
		MemoryContextAlloc(arr_context, astate->alen * sizeof(Datum));
	astate->dnulls = (bool *)
		MemoryContextAlloc(arr_context, astate->alen * sizeof(bool));
	astate->nelems = 0;
	astate->element_type = element_type;
	get_typlenbyvalalign(element_type,
						 &astate->typlen,
						 &astate->typbyval,
						 &astate->typalign);

	return astate;
}


static void ExecInitGraphVLE(ExtensiblePlanState *pstate, EState *estate,
                                int eflags)
{
		
	List	   *scan_label_oids = NIL;
	char	   *label_name;
	Oid			label_rel_id;
	ResultRelInfo *target_rel_infos;
	ListCell   *list_cell;
	/* check for unsupported flags */
	Assert(!(eflags & (EXEC_FLAG_BACKWARD | EXEC_FLAG_MARK)));

	cypher_vle_custom_scan_state *vle_state = castNode(cypher_vle_custom_scan_state, pstate);
	ExprContext *econtext = vle_state->css.ss.ps.ps_ExprContext;

	Plan  *subplan = (Plan*)linitial(vle_state->cs->extensible_plans);

    vle_state->css.ss.ps.state = estate;
	pstate->ss.ps.lefttree = ExecInitNode(subplan, estate, eflags);
	vle_state->subplan = pstate->ss.ps.lefttree;

    ExecAssignExprContext(estate, &pstate->ss.ps);
    TupleDesc tupledesc = ExecGetResultType(pstate->ss.ps.lefttree);
    ExecInitScanTupleSlot(estate, &pstate->ss);
    ExecAssignScanType(&pstate->ss, tupledesc);

    ExecAssignProjectionInfo(&pstate->ss.ps, tupledesc);


	vle_state->use_vertex_output = tupledesc->natts > VAR_VERTICES;


	/*
	 * Find all target labels.
	 */
    vle_state->graph_oid = vle_state->graph_oid;
	label_name = vle_state->label_name;

	label_rel_id = get_label_relation(
		label_name, vle_state->graph_oid);
	/*
	 * An unknown relationship type has no physical relation to scan. Keep an
	 * empty scan set so a *0..N pattern can still emit its zero-hop row while
	 * every positive-hop attempt returns no match.
	 */
	if (OidIsValid(label_rel_id))
	{
		scan_label_oids = lappend_oid(scan_label_oids, label_rel_id);
		scan_label_oids = list_concat_unique_oid(scan_label_oids,
									 find_all_inheritors(label_rel_id,
              AccessShareLock,
              NULL));
	}
	/*
	 * Fill ResultRelInfos.
	 */
	target_rel_infos = (ResultRelInfo *) palloc(
												list_length(scan_label_oids) * sizeof(ResultRelInfo));
	vle_state->target_rel_infos = target_rel_infos;
	vle_state->num_target_rel_info = list_length(scan_label_oids);
	vle_state->start_id_index_oids = (Oid *)palloc0(
     sizeof(Oid) * vle_state->num_target_rel_info);
	vle_state->end_id_index_oids = (Oid *)palloc0(
     sizeof(Oid) * vle_state->num_target_rel_info);

	/* Will be filled by below logic. */
	vle_state->current_scan_tuple = NULL;

	int relation_index = 0;
	foreach(list_cell, scan_label_oids)
	{
		Oid			label_oid = lfirst_oid(list_cell);
		Relation	relation = heap_open(label_oid, AccessShareLock);

		ensure_age_relation_supports_raw_access(relation);

		if (vle_state->current_scan_tuple == NULL)
		{
				TupleDesc tupleDescriptor = RelationGetDescr(relation);

			TupleTableSlot * slot = MakeTupleTableSlot();
			ExecSetSlotDescriptor(slot, /* slot to change */
				tupleDescriptor) ;
			vle_state->current_scan_tuple = slot;
		}
		
		InitResultRelInfo(target_rel_infos,
						  relation,
						  0,	/* dummy rangetable index */
						  0);
		vle_state->start_id_index_oids[relation_index] =
			find_usable_btree_index_for_attr(
       relation, Anum_ag_label_edge_table_start_id);
		vle_state->end_id_index_oids[relation_index] =
			find_usable_btree_index_for_attr(
       relation, Anum_ag_label_edge_table_end_id);
		target_rel_infos++;
		relation_index++;
	}

	list_free(scan_label_oids);

    
	/*
	 * edge_ids(2) : for edge uniqueness.
	 *
	 * edges(3) : output edge array.
	 *
	 * vertices(4).
	 */
	vle_state->edge_ids = initArrayResult(GRAPHIDOID,
										  CurrentMemoryContext,
										  false);
	vle_state->edges = initArrayResult(AGTYPEOID,
									   CurrentMemoryContext,
									   false);
	if (vle_state->use_vertex_output)
	{
		vle_state->vertices = initArrayResult(AGTYPEOID,
											  CurrentMemoryContext,
											  false);
	}
	else
	{
		vle_state->vertices = NULL;
	}

	if (vle_state->edge_property_constraint_expr)
	{
		bool is_null;
		vle_state->prop_expr_state =
		ExecInitExpr((Expr*)vle_state->edge_property_constraint_expr, (PlanState *)vle_state);
		vle_state->edge_property_constraint = DATUM_GET_AGTYPE_P( ExecEvalExpr(vle_state->prop_expr_state,
															econtext,
															&is_null)) ;
	}
	else
	{
		vle_state->edge_property_constraint = NULL;
	}
	
}



static bool ExecGraphVLEDFS(cypher_vle_custom_scan_state *vle_state, graphid start_id);

static void array_clear(ArrayBuildState *astate);
static void array_pop(ArrayBuildState *astate);
static inline bool array_has(ArrayBuildState *astate, graphid edge_id);

/*
 * Idea for Edge uniqueness.
 *
 * VLEDepthCtx have rel_index. so, we can use rel_index to identify the target
 * table.
 *
 * Bitmap set designed to 2-byte. but, label id is 6-byte...
 */

typedef struct VLEDepthCtx
{
	AgeBtreeEqScan *scan;
	int			rel_index;
	graphid		start_id;
	graphid		end_id;

	graphid		prev_end_id;
	uint8		direction_rotate;
} VLEDepthCtx;

static inline bool is_over_max_depth(cypher_vle_custom_scan_state *vle_state, int depth);
static bool create_scan_desc(cypher_vle_custom_scan_state *vle_state, VLEDepthCtx *vle_depth_ctx);
static bool create_none_direction_scan_desc(cypher_vle_custom_scan_state *vle_state,
											VLEDepthCtx *vle_depth_ctx);
static void free_scan_desc(VLEDepthCtx *vle_depth_ctx);


Datum
get_vertex_from_graphid(graphid vertex_id,Oid graph_oid)
{
	int32		vertex_label_id;
	Oid			vertex_label_relid;
	Oid			graph_path_oid = graph_oid;
	Relation	vertex_rel;
	char *label_name;
	AgeBtreeEqScan *scan;
	TupleTableSlot *slot;
	label_cache_data *label_cache;
	HeapTuple tuple;
	Datum		ret = (Datum) 0;

	vertex_label_id = get_graphid_label_id(vertex_id);

    label_cache = search_label_graph_id_cache(graph_path_oid, vertex_label_id);

    label_name = NameStr(label_cache->name);
	vertex_label_relid =  get_label_relation(label_name,graph_path_oid);
	vertex_rel = heap_open(vertex_label_relid, AccessShareLock);
	ensure_age_relation_supports_raw_access(vertex_rel);
	
	TupleDesc tupleDescriptor = RelationGetDescr(vertex_rel);

    slot = MakeTupleTableSlot();
    ExecSetSlotDescriptor(slot, /* slot to change */
        tupleDescriptor) ;

    scan = age_btree_eq_beginscan(
        vertex_rel,
        GetActiveSnapshot(),
        Anum_ag_label_vertex_table_id,
        F_GRAPHID_EQ,
        GRAPHID_GET_DATUM(vertex_id),
        AccessShareLock);
	for (;;)
    {
		tuple = age_btree_eq_getnext(scan);
        if (!HeapTupleIsValid(tuple))
            break;

        ExecClearTuple(slot);
        ExecStoreTuple(tuple, slot, InvalidBuffer, false);

        heap_slot_getallattrs(slot);
		ret = make_vertex(GRAPHID_GET_DATUM((slot)->tts_values[Anum_ag_label_vertex_table_id - 1]),
      string_to_agtype(label_name),
							AGTYPE_P_GET_DATUM((slot)->tts_values[Anum_ag_label_vertex_table_properties - 1]));
		break;
    }							

	age_btree_eq_endscan(scan);
	heap_close(vertex_rel, AccessShareLock);
	ExecDropSingleTupleTableSlot(slot);

	return ret;
}

/*
 * Project the zero-length path for the seed tuple just fetched. Returns the
 * projected slot, or NULL when the node's qual rejects it.
 */
static TupleTableSlot *
ProjectZeroLengthPath(cypher_vle_custom_scan_state *vle_state, ExprContext *econtext)
{
    List *qual = vle_state->css.ss.ps.qual;

    econtext->ecxt_scantuple = vle_state->subplan_tuple;
    econtext->ecxt_scantuple = ExecProject(vle_state->css.ss.ps.ps_ProjInfo, NULL);

    if (qual == NIL || ExecQual(qual, econtext, false)) {
        return econtext->ecxt_scantuple;
    }

    return NULL;
}

static TupleTableSlot *
ExecGraphVLE(ExtensiblePlanState *pstate)
{
	/*
	 * =====================
	 *
	 * Alex: Many columns in tuple. but, really needed?
	 *
	 * - execProc returns below types.
	 *
	 * Case #1 (7). {prev, curr, ids | next, id} + {edges | edge} graphid,
	 * graphid, graphid[], edge[]
	 *
	 * Case #2 (9) {prev, curr, ids, edges | next, id, edge} + {vertices |
	 * vertex} graphid, graphid, graphid[], edge[], vertex[]
	 *
	 * i.e., start, "end", ARRAY[id], ARRAY[ROW(id, start, "end", properties,
	 * ctid)::edge]
	 *
	 * See genVLESubselect().
	 *
	 *
	 * - depth if, VLE low index start from 0. then, depth start from 0. but,
	 * larger than start from 1.
	 */
	cypher_vle_custom_scan_state *vle_state = castNode(cypher_vle_custom_scan_state, pstate);
	ExprContext *econtext = vle_state->css.ss.ps.ps_ExprContext;

	for (;;)
	{
		TupleTableSlot *zero_length_path = NULL;

		/* fetch new subplan tuple. */
		if (vle_state->need_new_sp_tuple)
		{
			vle_state->subplan_tuple = ExecProcNode(pstate->ss.ps.lefttree);

			// ereport(NOTICE, (errmsg("fetch new sub tuple-------------------------------")));
			vle_state->need_new_sp_tuple = false;

			/* no more exists.. */
			if (TupIsNull(vle_state->subplan_tuple))
				return NULL;

			/*
			 * VLE updates the seed slot's end vertex and array columns in place.
			 * An openGauss index path can return a physical slot with none of its
			 * attributes deformed yet (tts_nvalid == 0).  If we update that slot
			 * directly, ExecProject() later deforms the original tuple and
			 * overwrites the traversed arrays with the empty seed arrays.  This is
			 * observable when an upper LIMIT makes the anonymous VLE seed choose
			 * Unique/MergeAppend/IndexOnlyScan instead of HashAggregate.
			 */
			tableam_tslot_getallattrs(vle_state->subplan_tuple);

			array_clear(vle_state->edges);
			array_clear(vle_state->edge_ids);

			vle_state->first_start_id = DATUM_GET_GRAPHID(vle_state->subplan_tuple->tts_values[VAR_START_VID]);

			if (vle_state->use_vertex_output)
			{
				array_clear(vle_state->vertices);
    			
				accumArrayResult(vle_state->vertices,
								 get_vertex_from_graphid( vle_state->first_start_id, vle_state->graph_oid),
								 false,
								 AGTYPEOID,
								 CurrentMemoryContext);
				
			}

			if (0 >= vle_state->minimum_output_depth &&
				0 <= vle_state->maximum_output_depth)
			{
				zero_length_path = ProjectZeroLengthPath(vle_state, econtext);
			}
		}

		/* emit the zero-length path first; a rejected one falls through to DFS */
		if (zero_length_path != NULL) {
			return zero_length_path;
		}

		/* Do DFS. */
		if (!ExecGraphVLEDFS(vle_state,
							 vle_state->first_start_id))
		{
			vle_state->need_new_sp_tuple = true;
			continue;
			// return NULL;
		}
		else
		{
			MemoryContext tupmctx = econtext->ecxt_per_tuple_memory;

			vle_state->subplan_tuple->tts_values[VAR_END_VID] = vle_state->last_end_id;
			int dims[1];
			int lbs[1];
			dims[0] = vle_state->edge_ids->nelems;
			lbs[0] = 1;
			vle_state->subplan_tuple->tts_values[VAR_EDGE_IDS] =  makeMdArrayResult(vle_state->edge_ids, 1, dims, lbs, tupmctx, false);

			dims[0] = vle_state->edges->nelems;
			lbs[0] = 1;
			vle_state->subplan_tuple->tts_values[VAR_EDGES] =  makeMdArrayResult(vle_state->edges, 1, dims, lbs, tupmctx, false);

			if (vle_state->use_vertex_output)
			{
				int			ndims;
				int			dims[1];
				int			lbs[1];

				ndims = (vle_state->vertices->nelems > 0) ? 1 : 0;
				/* latest vertex is added somewhere. */
				dims[0] = vle_state->vertices->nelems - 1;
				lbs[0] = 1;
				vle_state->subplan_tuple->tts_values[VAR_VERTICES] = makeMdArrayResult(vle_state->vertices,
																					   ndims,
																					   dims,
																					   lbs,
																					   tupmctx,
																					   false);
			}
			econtext->ecxt_scantuple = vle_state->subplan_tuple;
			econtext->ecxt_scantuple =
				ExecProject(vle_state->css.ss.ps.ps_ProjInfo, NULL);

			if (vle_state->css.ss.ps.qual == NIL ||
				ExecQual(vle_state->css.ss.ps.qual, econtext, false))
			{
				return econtext->ecxt_scantuple;
			}

			continue;
		}
	}

	/* No more tuples. */
	return NULL;
}




/*
 * ExecGraphVLEDFS
 *
 * Adds edges, edge_ids using condition cypher relationship. Returns false if
 * there is no tuple that corresponds to the condition.
 */
static bool
ExecGraphVLEDFS(cypher_vle_custom_scan_state *vle_state, graphid start_id)
{
	VLEDepthCtx *vle_depth_ctx = NULL;
	graphid		edge_id;

	/* is first time? */
	if (vle_state->table_scan_desc_list == NIL)
	{
		vle_depth_ctx = (VLEDepthCtx *) palloc(sizeof(VLEDepthCtx));
		vle_depth_ctx->scan = NULL;
		vle_depth_ctx->rel_index = 0;
		vle_depth_ctx->start_id = start_id;
		vle_depth_ctx->end_id = start_id;

		/* None-directional scanning. */
		vle_depth_ctx->direction_rotate = 0;
		vle_depth_ctx->prev_end_id = start_id;

		if (!create_scan_desc(vle_state, vle_depth_ctx))
		{
			pfree(vle_depth_ctx);
			return false;
		}
		vle_state->table_scan_desc_list = lappend(vle_state->table_scan_desc_list,
												  vle_depth_ctx);
	}

	for (;;)
	{
		int			vle_scan_depth;
		bool		return_as_results;
		graphid		new_start_id,
					new_end_id;

		vle_depth_ctx = (VLEDepthCtx*)llast(vle_state->table_scan_desc_list);

		HeapTuple tuple = age_btree_eq_getnext(vle_depth_ctx->scan);
		if (!HeapTupleIsValid(tuple))
		{
			/* find next target relation */
			if (!create_scan_desc(vle_state, vle_depth_ctx))
			{
				/* move back depth. */
				free_scan_desc(vle_depth_ctx);
				vle_state->table_scan_desc_list = list_delete_last(vle_state->table_scan_desc_list);
				if (vle_state->table_scan_desc_list == NIL)
				{
					/* terminate current tuple scan */
					return false;
				}
			}
			continue;
		}else{
 			ExecStoreTuple(tuple, vle_state->current_scan_tuple, InvalidBuffer, false);
		}

		if (vle_state->cypher_rel_direction == CYPHER_REL_DIR_NONE &&
			vle_depth_ctx->direction_rotate != 0)
		{
			bool is_null = false;
            Datum edge_start_id = heap_slot_getattr(
                vle_state->current_scan_tuple,
                Anum_ag_label_edge_table_start_id,
                &is_null);
			if (!is_null &&
				DATUM_GET_GRAPHID(edge_start_id) == vle_depth_ctx->prev_end_id)
			{
				continue;
			}
		}

				/* Property filtering. */
		if (vle_state->edge_property_constraint)
		{
			bool		isnull;
			agtype	   * edge_property = DATUM_GET_AGTYPE_P(heap_slot_getattr(vle_state->current_scan_tuple,
														  Anum_ag_label_edge_table_properties,
														  &isnull));
			agtype_container *agtc_edge_property_constraint = &(vle_state->edge_property_constraint->root);
			agtype_container *agtc_edge_property = &(edge_property->root);

			int num_edge_property_constraints = AGT_ROOT_COUNT(vle_state->edge_property_constraint);
		   /* get the number of properties in the edge to be matched */
			int num_edge_properties = AGTYPE_CONTAINER_SIZE(agtc_edge_property);

			/*
			* Check to see if the edge_properties object has AT LEAST as many pairs
			* to compare as the edge_property_constraint object has pairs. If not, it
			* can't possibly match.
			*/
			if (num_edge_property_constraints > num_edge_properties)
			{
				continue;
			}

			/* get the iterators */
			agtype_iterator * constraint_it = agtype_iterator_init(agtc_edge_property_constraint);
			agtype_iterator * property_it = agtype_iterator_init(agtc_edge_property);

			if (!agtype_deep_contains(&property_it, &constraint_it, false))
			{
				continue;
			}
		}

		/*
		 * Fetch all attributes.
		 */
		heap_slot_getallattrs(vle_state->current_scan_tuple);

		edge_id = vle_state->current_scan_tuple->tts_values[Anum_ag_label_edge_table_id - 1];

		vle_scan_depth = list_length(vle_state->table_scan_desc_list);

		while (vle_state->edge_ids->nelems >= vle_scan_depth)
		{
			array_pop(vle_state->edges);
			array_pop(vle_state->edge_ids);
			if (vle_state->use_vertex_output)
				array_pop(vle_state->vertices);
		}

		/* It is un-efficient. but, easy. */
		if (array_has(vle_state->edge_ids, edge_id))
		{
			continue;
		}

		/* Will be used from ExecGraphVLE() */
		new_start_id = DATUM_GET_GRAPHID(vle_state->current_scan_tuple->tts_values[Anum_ag_label_edge_table_start_id - 1]);
		new_end_id = DATUM_GET_GRAPHID(vle_state->current_scan_tuple->tts_values[Anum_ag_label_edge_table_end_id - 1]);

		if (vle_state->cypher_rel_direction == CYPHER_REL_DIR_RIGHT)
		{
			vle_state->last_end_id = new_end_id;
		}
		else if (vle_state->cypher_rel_direction == CYPHER_REL_DIR_LEFT)
		{
			vle_state->last_end_id = new_start_id;
		}
		else
		{
			if (vle_depth_ctx->prev_end_id == new_end_id)
			{
				vle_state->last_end_id = new_start_id;
			}
			else
			{
				vle_state->last_end_id = new_end_id;
			}
		}
		// get label name
		graphid edge_graph_id = DATUM_GET_GRAPHID ((vle_state->current_scan_tuple)->tts_values[Anum_ag_label_edge_table_id - 1]);
     	int edge_label_id = get_graphid_label_id(edge_graph_id);

        label_cache_data * label_cache = search_label_graph_id_cache(vle_state->graph_oid, edge_label_id);

        char* label_name = NameStr(label_cache->name);
			
		accumArrayResult(vle_state->edges,
						 make_edge_from_tuple(vle_state->current_scan_tuple,label_name),
						 false,
						 AGTYPEOID,
						 CurrentMemoryContext);
		accumArrayResult(vle_state->edge_ids,
						 edge_id,
						 false,
						 GRAPHIDOID,
						 CurrentMemoryContext);

		if (vle_state->use_vertex_output)
		{
			accumArrayResult(vle_state->vertices,
							 get_vertex_from_graphid(vle_state->last_end_id,vle_state->graph_oid),
							 false,
							 AGTYPEOID,
							 CurrentMemoryContext);
		}
		

		return_as_results = vle_state->minimum_output_depth <= vle_scan_depth &&
			vle_state->maximum_output_depth >= vle_scan_depth;

		if (!is_over_max_depth(vle_state, vle_scan_depth + 1))
		{
			/* Move next depth */
			VLEDepthCtx *top_vle_depth_ctx = vle_depth_ctx;

			vle_depth_ctx = (VLEDepthCtx *) palloc(sizeof(VLEDepthCtx));
			vle_depth_ctx->scan = NULL;
			vle_depth_ctx->rel_index = 0;
			vle_depth_ctx->start_id = new_start_id;
			vle_depth_ctx->end_id = new_end_id;

			/* None-directional scanning. */
			vle_depth_ctx->direction_rotate = 0;
			vle_depth_ctx->prev_end_id = top_vle_depth_ctx->prev_end_id == new_start_id ?
				new_end_id : new_start_id;

			create_scan_desc(vle_state, vle_depth_ctx); /* never not failing */
			vle_state->table_scan_desc_list = lappend(vle_state->table_scan_desc_list,
													  vle_depth_ctx);
		}

		if (return_as_results)
		{
			break;
		}
	}

	return true;
}

static void
free_scan_desc(VLEDepthCtx *vle_depth_ctx)
{
	if (vle_depth_ctx->scan != NULL)
	{
		age_btree_eq_endscan(vle_depth_ctx->scan);
		vle_depth_ctx->scan = NULL;
	}
	pfree(vle_depth_ctx);
}

static bool
create_scan_desc(cypher_vle_custom_scan_state *vle_state,
				 VLEDepthCtx *vle_depth_ctx)
{
	ResultRelInfo *result_rel_info;
	AttrNumber endpoint_attribute;
	graphid endpoint_value;
	Oid index_oid;

	if (vle_state->num_target_rel_info == 0)
	{
		return false;
	}

	result_rel_info = vle_state->target_rel_infos + vle_depth_ctx->rel_index;

	if (vle_state->cypher_rel_direction == CYPHER_REL_DIR_NONE)
	{
		return create_none_direction_scan_desc(vle_state, vle_depth_ctx);
	}

	if (vle_depth_ctx->scan != NULL)
	{
		age_btree_eq_endscan(vle_depth_ctx->scan);
		vle_depth_ctx->scan = NULL;
		vle_depth_ctx->rel_index++;

		if (vle_depth_ctx->rel_index >= vle_state->num_target_rel_info)
		{
			return false;
		}
		result_rel_info = vle_state->target_rel_infos + vle_depth_ctx->rel_index;
	}

	if (vle_state->cypher_rel_direction == CYPHER_REL_DIR_RIGHT)
	{
		endpoint_attribute = Anum_ag_label_edge_table_start_id;
		endpoint_value = vle_depth_ctx->end_id;
		index_oid = vle_state->start_id_index_oids[vle_depth_ctx->rel_index];
	}
	else
	{
		Assert(vle_state->cypher_rel_direction == CYPHER_REL_DIR_LEFT);
		endpoint_attribute = Anum_ag_label_edge_table_end_id;
		endpoint_value = vle_depth_ctx->start_id;
		index_oid = vle_state->end_id_index_oids[vle_depth_ctx->rel_index];
	}

    vle_depth_ctx->scan = age_btree_eq_beginscan_with_index(
        result_rel_info->ri_RelationDesc,
        vle_state->css.ss.ps.state->es_snapshot,
        endpoint_attribute,
        F_GRAPHID_EQ,
        GRAPHID_GET_DATUM(endpoint_value),
        AccessShareLock,
        index_oid);

	return true;
}

static bool
create_none_direction_scan_desc(cypher_vle_custom_scan_state *vle_state,
								VLEDepthCtx *vle_depth_ctx)
{
	ResultRelInfo *result_rel_info;
	AttrNumber endpoint_attribute;
	Oid index_oid;

	if (vle_state->num_target_rel_info == 0)
	{
		return false;
	}

	result_rel_info = vle_state->target_rel_infos + vle_depth_ctx->rel_index;

	if (vle_depth_ctx->scan != NULL)
	{
		age_btree_eq_endscan(vle_depth_ctx->scan);
		vle_depth_ctx->scan = NULL;

		if (vle_depth_ctx->direction_rotate > 0)
		{
			vle_depth_ctx->rel_index++;

			if (vle_depth_ctx->rel_index >= vle_state->num_target_rel_info)
			{
				return false;
			}

			result_rel_info = vle_state->target_rel_infos +
				vle_depth_ctx->rel_index;
			vle_depth_ctx->direction_rotate = 0;
		} else {
			vle_depth_ctx->direction_rotate = 1;
		}
	}

	if (vle_depth_ctx->direction_rotate == 0)
	{
		endpoint_attribute = Anum_ag_label_edge_table_start_id;
		index_oid = vle_state->start_id_index_oids[vle_depth_ctx->rel_index];
	}
	else
	{
		endpoint_attribute = Anum_ag_label_edge_table_end_id;
		index_oid = vle_state->end_id_index_oids[vle_depth_ctx->rel_index];
	}

    vle_depth_ctx->scan = age_btree_eq_beginscan_with_index(
        result_rel_info->ri_RelationDesc,
        vle_state->css.ss.ps.state->es_snapshot,
        endpoint_attribute,
        F_GRAPHID_EQ,
        GRAPHID_GET_DATUM(vle_depth_ctx->prev_end_id),
        AccessShareLock,
        index_oid);

	return true;
}

static inline bool
is_over_max_depth(cypher_vle_custom_scan_state *vle_state, int depth)
{
	return vle_state->maximum_output_depth < depth;
}

static void
ExecReScanGraphVLE(ExtensiblePlanState *pstate)
{
	cypher_vle_custom_scan_state *vle_state = castNode(cypher_vle_custom_scan_state, pstate);
	ListCell *lc;

	/*
	 * A correlated EXISTS subquery can stop as soon as the first path is
	 * produced.  In that case the DFS stack still contains live heap/index
	 * scans.  Reusing it for the next outer tuple continues the previous
	 * traversal instead of starting from the new seed vertex, which silently
	 * drops matches in other connected components (Apache AGE #1924).
	 */
	foreach (lc, vle_state->table_scan_desc_list)
	{
		VLEDepthCtx *vle_depth_ctx = (VLEDepthCtx *)lfirst(lc);

		free_scan_desc(vle_depth_ctx);
	}
	list_free(vle_state->table_scan_desc_list);
	vle_state->table_scan_desc_list = NIL;

	array_clear(vle_state->edge_ids);
	array_clear(vle_state->edges);
	if (vle_state->vertices != NULL)
		array_clear(vle_state->vertices);

	vle_state->need_new_sp_tuple = true;
	vle_state->subplan_tuple = NULL;
	ExecReScan(vle_state->subplan);
}

static void
ExecEndGraphVLE(ExtensiblePlanState  *pstate)
{
	cypher_vle_custom_scan_state *vle_state = castNode(cypher_vle_custom_scan_state, pstate);
	int			i;
	ListCell   *lc;
	ResultRelInfo *result_rel_info = vle_state->target_rel_infos;

	foreach(lc, vle_state->table_scan_desc_list)
	{
		VLEDepthCtx *vle_depth_ctx =(VLEDepthCtx *)lfirst(lc);

		free_scan_desc(vle_depth_ctx);
	}
	list_free(vle_state->table_scan_desc_list);

	if (vle_state->current_scan_tuple != NULL)
	{
		ExecDropSingleTupleTableSlot(vle_state->current_scan_tuple);
	}

	for (i = 0; i < vle_state->num_target_rel_info; i++)
	{
		heap_close(result_rel_info->ri_RelationDesc, AccessShareLock);
		result_rel_info++;
	}
	if (vle_state->target_rel_infos != NULL)
	{
		pfree(vle_state->target_rel_infos);
	}
	if (vle_state->start_id_index_oids != NULL)
	{
		pfree(vle_state->start_id_index_oids);
	}
	if (vle_state->end_id_index_oids != NULL)
	{
		pfree(vle_state->end_id_index_oids);
	}

	/*
	 * clean out the tuple table
	 */
	ExecClearTuple(vle_state->css.ss.ps.ps_ResultTupleSlot);
	ExecEndNode(vle_state->subplan);
	ExecFreeExprContext(&vle_state->css.ss.ps);
}

static void
array_clear(ArrayBuildState *astate)
{
	if (!astate->typbyval)
	{
		int			i;

		for (i = 0; i < astate->nelems; i++)
			pfree(DatumGetPointer(astate->dvalues[i]));
	}

	astate->nelems = 0;
}

static void
array_pop(ArrayBuildState *astate)
{
	if (astate->nelems > 0)
	{
		astate->nelems--;
		if (!astate->typbyval)
			pfree(DatumGetPointer(astate->dvalues[astate->nelems]));
	}
}

static inline bool
array_has(ArrayBuildState *astate, graphid edge_id)
{
	int			i;

	for (i = 0; i < astate->nelems; i++)
	{
		graphid		cur_array_gid = DATUM_GET_GRAPHID(astate->dvalues[i]);

		if (cur_array_gid == edge_id)
			return true;
	}

	return false;
}
