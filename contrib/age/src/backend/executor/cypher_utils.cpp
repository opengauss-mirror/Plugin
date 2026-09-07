/*
 * For PostgreSQL Database Management System:
 * (formerly known as Postgres, then as Postgres95)
 *
 * Portions Copyright (c) 1996-2010, The PostgreSQL Global Development Group
 *
 * Portions Copyright (c) 1994, The Regents of the University of California
 *
 * Permission to use, copy, modify, and distribute this software and its documentation for any purpose,
 * without fee, and without a written agreement is hereby granted, provided that the above copyright notice
 * and this paragraph and the following two paragraphs appear in all copies.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
 * OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA
 * HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include "postgres.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "access/htup.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "access/multixact.h"
#include "catalog/heap.h"
#include "catalog/pg_am.h"
#include "catalog/pg_index.h"
#include "miscadmin.h"
#include "nodes/ag_extensible.h"
#include "nodes/makefuncs.h"
#include "nodes/nodes.h"
#include "nodes/nodeFuncs.h"
#include "nodes/plannodes.h"
#include "parser/parsetree.h"
#include "parser/parse_relation.h"
#include "storage/procarray.h"
#include "utils/rel.h"
#include "utils/relcache.h"
#include "utils/sec_rls_utils.h"
#include "utils/snapmgr.h"
#include "executor/executor.h"
#include "executor/node/nodeModifyTable.h"

#include "catalog/ag_label.h"
#include "commands/label_commands.h"
#include "executor/cypher_executor.h"
#include "executor/cypher_utils.h"
#include "utils/agtype.h"
#include "utils/ag_cache.h"
#include "utils/age_global_graph.h"
#include "utils/graphid.h"

typedef struct AgeBtreeEqScan {
    Relation index_relation;
    TableScanDesc heap_scan;
    IndexScanDesc index_scan;
    ScanKeyData key;
    LOCKMODE index_lockmode;
} AgeBtreeEqScan;

void mark_entity_relation_modified(List **modified_relids, Oid relid)
{
    Assert(modified_relids != NULL);
    Assert(OidIsValid(relid));

    if (!list_member_oid(*modified_relids, relid)) {
        *modified_relids = lappend_oid(*modified_relids, relid);
        invalidate_GRAPH_global_contexts_by_relid(relid);
    }
}

void notify_modified_entity_relations(List **modified_relids)
{
    ListCell *lc;

    Assert(modified_relids != NULL);

    foreach (lc, *modified_relids)
        notify_GRAPH_global_contexts_relation_modified(lfirst_oid(lc));

    list_free(*modified_relids);
    *modified_relids = NIL;
}

Oid find_usable_btree_index_for_attr(Relation relation,
                                     AttrNumber heap_attnum)
{
    List *index_oids;
    ListCell *cell;
    Oid result = InvalidOid;

    Assert(relation != NULL);
    Assert(heap_attnum > 0);

    index_oids = RelationGetIndexList(relation);
    foreach (cell, index_oids)
    {
        Oid index_oid = lfirst_oid(cell);
        Relation index_relation = index_open(index_oid, AccessShareLock);
        Form_pg_index index_form = index_relation->rd_index;

        /*
         * This executor-local lookup cannot reproduce the planner's transient
         * plan invalidation contract, so indcheckxmin indexes are not used.
         */
        if (index_form != NULL && IndexIsValid(index_form) &&
            !index_form->indcheckxmin &&
            GetIndexVisibleStateByTuple(index_relation->rd_indextuple) &&
            GetIndexEnableStateByTuple(index_relation->rd_indextuple) &&
            IndexRelationGetNumberOfKeyAttributes(index_relation) >= 1 &&
            index_form->indkey.values[0] == heap_attnum &&
            index_relation->rd_rel->relam == BTREE_AM_OID &&
            RelationGetIndexExpressions(index_relation) == NIL &&
            RelationGetIndexPredicate(index_relation) == NIL) {
            result = index_oid;
            index_close(index_relation, AccessShareLock);
            break;
        }

        index_close(index_relation, AccessShareLock);
    }
    list_free(index_oids);

    return result;
}

AgeBtreeEqScan *age_btree_eq_beginscan(Relation relation, Snapshot snapshot,
                                       AttrNumber heap_attnum,
                                       RegProcedure equality_function,
                                       Datum value, LOCKMODE index_lockmode)
{
    Oid index_oid = find_usable_btree_index_for_attr(relation, heap_attnum);

    return age_btree_eq_beginscan_with_index(
        relation, snapshot, heap_attnum, equality_function, value,
        index_lockmode, index_oid);
}

AgeBtreeEqScan *age_btree_eq_beginscan_with_index(
    Relation relation, Snapshot snapshot, AttrNumber heap_attnum,
    RegProcedure equality_function, Datum value, LOCKMODE index_lockmode,
    Oid index_oid)
{
    AgeBtreeEqScan *scan;

    Assert(relation != NULL);
    Assert(snapshot != NULL);
    Assert(heap_attnum > 0);

    scan = (AgeBtreeEqScan *)palloc0(sizeof(AgeBtreeEqScan));
    scan->index_lockmode = index_lockmode;

    if (OidIsValid(index_oid)) {
        /* Index scan keys address index-key positions, not heap attributes. */
        ScanKeyInit(&scan->key, 1, BTEqualStrategyNumber,
                    equality_function, value);
        scan->index_relation = index_open(index_oid, index_lockmode);
        scan->index_scan = index_beginscan(relation, scan->index_relation,
                                           snapshot, 1, 0);
        index_rescan(scan->index_scan, &scan->key, 1, NULL, 0);
    } else {
        ScanKeyInit(&scan->key, heap_attnum, BTEqualStrategyNumber,
                    equality_function, value);
        scan->heap_scan = heap_beginscan(relation, snapshot, 1, &scan->key);
    }

    return scan;
}

HeapTuple age_btree_eq_getnext(AgeBtreeEqScan *scan)
{
    Assert(scan != NULL);

    if (scan->index_scan != NULL) {
        return (HeapTuple)index_getnext(scan->index_scan,
                                        ForwardScanDirection);
    }

    return heap_getnext(scan->heap_scan, ForwardScanDirection);
}

void age_btree_eq_endscan(AgeBtreeEqScan *scan)
{
    if (scan == NULL) {
        return;
    }

    if (scan->index_scan != NULL) {
        index_endscan(scan->index_scan);
        index_close(scan->index_relation, scan->index_lockmode);
    } else if (scan->heap_scan != NULL) {
        heap_endscan(scan->heap_scan);
    }

    pfree(scan);
}

ResultRelInfo *create_entity_result_rel_info(EState *estate, char *graph_name, char *label_name)
{
    RangeVar *rv;
    Relation label_relation;
    ResultRelInfo *resultRelInfo;

    ParseState *pstate = make_parsestate(NULL);

    resultRelInfo = (ResultRelInfo*)palloc(sizeof(ResultRelInfo));

    if (strlen(label_name) == 0)
    {
        rv = makeRangeVar(graph_name, AG_DEFAULT_LABEL_VERTEX, -1);
    }
    else
    {
        rv = makeRangeVar(graph_name, label_name, -1);
    }

    label_relation = parserOpenTable(pstate, rv, RowExclusiveLock);

    InitResultRelInfo(resultRelInfo, label_relation,
                      list_length(estate->es_range_table),
                      estate->es_instrument);
    // open the parse state
    ExecOpenIndices(resultRelInfo, false);
    free_parsestate(pstate);

    return resultRelInfo;
}

// close the result_rel_info and close all the indices
void destroy_entity_result_rel_info(ResultRelInfo *result_rel_info)
{
    // close the indices
    ExecCloseIndices(result_rel_info);

    // close the rel
    heap_close(result_rel_info->ri_RelationDesc, RowExclusiveLock);
}

/*
 * Clear an entity slot and mark every attribute NULL before AGE fills in the
 * columns it manages (id/start_id/end_id/properties).
 *
 * The slot's tuple descriptor is the full label-table descriptor, which may
 * contain columns AGE does not populate -- e.g. a user-added plain column, or a
 * GENERATED ALWAYS ... STORED column. Without this, those attributes keep stale
 * slot memory and materializing the tuple segfaults dereferencing the garbage
 * (issue #2450). Plain columns then default to NULL; generated columns are
 * recomputed via compute_stored_generated() before the tuple is materialized.
 */
void clear_entity_slot(TupleTableSlot *elemTupleSlot)
{
    int natts = elemTupleSlot->tts_tupleDescriptor->natts;

    ExecClearTuple(elemTupleSlot);
    for (int attno = 0; attno < natts; attno++) {
        elemTupleSlot->tts_isnull[attno] = true;
    }
}

/*
 * Recompute stored generated columns on an entity slot before the heap tuple is
 * materialized. AGE's create/merge/set paths only populate id/properties, so a
 * GENERATED ALWAYS ... STORED column added to the label table would otherwise
 * be left uninitialized (issue #2450). This mirrors the generated-column step
 * of openGauss's own ExecInsert/ExecUpdate paths.
 */
void compute_stored_generated(ResultRelInfo *resultRelInfo,
                              TupleTableSlot *elemTupleSlot, EState *estate,
                              CmdType cmdtype)
{
    TupleConstr *constr = resultRelInfo->ri_RelationDesc->rd_att->constr;

    if (constr != NULL && constr->has_generated_stored) {
        HeapTuple tuple = (HeapTuple)elemTupleSlot->tts_tuple;
        CmdType generated_cmdtype = cmdtype;

        Assert(tuple != NULL);

        /*
         * openGauss has no TupleTableSlot::tts_tableOid. System-column
         * expressions read the OID from the physical tuple instead.
         */
        tuple->t_tableOid = RelationGetRelid(resultRelInfo->ri_RelationDesc);

        /*
         * AGE's custom SET executor does not have the planner-maintained
         * extraUpdatedCols bitmap used by openGauss to select affected
         * generated columns. AGE always rebuilds properties, so recompute all
         * stored generated expressions on SET.
         */
        if (generated_cmdtype == CMD_UPDATE) {
            generated_cmdtype = CMD_INSERT;
        }

        ExecComputeStoredGenerated(resultRelInfo, estate, elemTupleSlot,
                                   elemTupleSlot->tts_tuple,
                                   generated_cmdtype);

        tuple = (HeapTuple)elemTupleSlot->tts_tuple;
        tuple->t_tableOid = RelationGetRelid(resultRelInfo->ri_RelationDesc);
    }
}


TupleTableSlot *populate_vertex_tts(
    TupleTableSlot *elemTupleSlot, agtype_value *id, agtype_value *properties)
{
    bool properties_isnull;

    if (id == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("vertex id field cannot be NULL")));
    }

    clear_entity_slot(elemTupleSlot);

    properties_isnull = properties == NULL;

    elemTupleSlot->tts_values[vertex_tuple_id] = GRAPHID_GET_DATUM(id->val.int_value);
    elemTupleSlot->tts_isnull[vertex_tuple_id] = false;

    elemTupleSlot->tts_values[vertex_tuple_properties] =
        AGTYPE_P_GET_DATUM(agtype_value_to_agtype(properties));
    elemTupleSlot->tts_isnull[vertex_tuple_properties] = properties_isnull;

    return elemTupleSlot;
}

TupleTableSlot *populate_edge_tts(
    TupleTableSlot *elemTupleSlot, agtype_value *id, agtype_value *startid,
    agtype_value *endid, agtype_value *properties)
{
    bool properties_isnull;

    if (id == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("edge id field cannot be NULL")));
        return elemTupleSlot; /* suppress the static check warmings */
    }
    if (startid == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("edge start_id field cannot be NULL")));
        return elemTupleSlot; /* suppress the static check warmings */
    }

    if (endid == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("edge end_id field cannot be NULL")));
        return elemTupleSlot; /* suppress the static check warmings */
    }

    clear_entity_slot(elemTupleSlot);

    properties_isnull = properties == NULL;

    elemTupleSlot->tts_values[edge_tuple_id] =
        GRAPHID_GET_DATUM(id->val.int_value);
    elemTupleSlot->tts_isnull[edge_tuple_id] = false;

    elemTupleSlot->tts_values[edge_tuple_start_id] =
        GRAPHID_GET_DATUM(startid->val.int_value);
    elemTupleSlot->tts_isnull[edge_tuple_start_id] = false;

    elemTupleSlot->tts_values[edge_tuple_end_id] =
        GRAPHID_GET_DATUM(endid->val.int_value);
    elemTupleSlot->tts_isnull[edge_tuple_end_id] = false;

    elemTupleSlot->tts_values[edge_tuple_properties] =
        AGTYPE_P_GET_DATUM(agtype_value_to_agtype(properties));
    elemTupleSlot->tts_isnull[edge_tuple_properties] = properties_isnull;

    return elemTupleSlot;
}


/*
 * Find out if the entity still exists. This is for 'implicit' deletion
 * of an entity.
 */
bool entity_exists(EState *estate, Oid graph_oid, graphid id)
{
    label_cache_data *label;
    Relation rel;
    AgeBtreeEqScan *scan;
    HeapTuple tuple;
    CommandId saved_curcid;

    /*
     * Extract the label id from the graph id and get the table name
     * the entity is part of.
     */
    label = search_label_graph_id_cache(graph_oid, GET_LABEL_ID(id));
    if (label == NULL)
    {
        return false;
    }

    saved_curcid = estate->es_snapshot->curcid;
    estate->es_snapshot->curcid = Max(saved_curcid,
                                      GetCurrentCommandId(false));

    PG_TRY();
    {
        rel = heap_open(label->relation, RowExclusiveLock);
        scan = age_btree_eq_beginscan(rel, SnapshotSelf, 1, F_GRAPHIDEQ,
                                      GRAPHID_GET_DATUM(id), AccessShareLock);
        tuple = age_btree_eq_getnext(scan);

        age_btree_eq_endscan(scan);
        heap_close(rel, RowExclusiveLock);
        estate->es_snapshot->curcid = saved_curcid;
    }
    PG_CATCH();
    {
        estate->es_snapshot->curcid = saved_curcid;
        PG_RE_THROW();
    }
    PG_END_TRY();

    return HeapTupleIsValid(tuple);
}

/*
 * Insert the edge/vertex tuple into the table and indices. Check that the
 * table's constraints have not been violated.
 *
 * This function defaults to, and flags for update, the currentCommandId. If
 * you need to pass a specific cid and avoid using the currentCommandId, use
 * insert_entity_tuple_cid instead.
 */
HeapTuple insert_entity_tuple(ResultRelInfo *resultRelInfo,
                              TupleTableSlot *elemTupleSlot,
                              EState *estate)
{
    return insert_entity_tuple_cid(resultRelInfo, elemTupleSlot, estate,
                                   GetCurrentCommandId(true));
}

/*
 * The heap-level graph access paths (CREATE, MERGE, VLE and the global graph
 * scans) bypass the planner and therefore never see row-level-security
 * policies.  They are only allowed when no policy would apply to the current
 * user: RLS is disabled on the label, or the user may bypass it (superuser,
 * a role with BYPASSRLS, or the label owner unless FORCE ROW LEVEL SECURITY
 * is set) - the same rule the kernel uses for ordinary DML.
 */
void ensure_age_relation_supports_raw_access(Relation relation)
{
    if (CheckEnableRlsPolicies(relation, GetUserId()) == RLS_ENABLED)
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("AGE raw graph access is not supported for RLS-enabled label \"%s\"",
                        RelationGetRelationName(relation)),
                 errhint("Disable row-level security for this label or use a supported executor path.")));
}

/*
 * The heap write paths (CREATE/MERGE inserts, SET updates, DELETE) bypass the
 * SQL executor, so the checks it would have made are done here: the table
 * privilege for the operation, the row-level-security bypass rule and the
 * read-only transaction state.
 */
void check_entity_write_allowed(Relation relation, AclMode mode,
                                const char *clause_name)
{
    AclResult acl_result;

    PreventCommandIfReadOnly(clause_name);

    acl_result = pg_class_aclcheck(RelationGetRelid(relation), GetUserId(),
                                   mode);
    if (acl_result != ACLCHECK_OK)
        aclcheck_error(acl_result, ACL_KIND_CLASS,
                       RelationGetRelationName(relation));

    ensure_age_relation_supports_raw_access(relation);
}

/*
 * Insert the edge/vertex tuple into the table and indices. Check that the
 * table's constraints have not been violated.
 *
 * This function uses the passed cid for the insert.
 */

HeapTuple insert_entity_tuple_cid(ResultRelInfo *resultRelInfo,
                                  TupleTableSlot *elemTupleSlot,
                                  EState *estate, CommandId cid)
{
    HeapTuple tuple;

    ensure_age_relation_supports_raw_access(resultRelInfo->ri_RelationDesc);

    ExecStoreVirtualTuple(elemTupleSlot);
    tuple = ExecMaterializeSlot(elemTupleSlot);

    /*
     * compute_stored_generated() applies the openGauss physical-tuple OID
     * contract, builds a new tuple, and stores it back on the slot.
     */
    compute_stored_generated(resultRelInfo, elemTupleSlot, estate, CMD_INSERT);
    tuple = (HeapTuple) elemTupleSlot->tts_tuple;

    // Check the constraints of the tuple
    tuple->t_tableOid = RelationGetRelid(resultRelInfo->ri_RelationDesc);
    if (resultRelInfo->ri_RelationDesc->rd_att->constr != NULL) {
        ExecConstraints(resultRelInfo, elemTupleSlot, estate);
    }

    // Insert the tuple using the passed in cid
    heap_insert(resultRelInfo->ri_RelationDesc, tuple, cid, 0, NULL);

    // Insert index entries for the tuple
    if (resultRelInfo->ri_NumIndices > 0)
        ExecInsertIndexTuples(elemTupleSlot, &(tuple->t_self), estate,
            NULL, NULL, InvalidBktId, NULL, NULL);

    return tuple;
}
