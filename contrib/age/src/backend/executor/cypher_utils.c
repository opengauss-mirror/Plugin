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

#include "access/htup_details.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "access/multixact.h"
#include "nodes/extensible.h"
#include "nodes/makefuncs.h"
#include "nodes/nodes.h"
#include "nodes/nodeFuncs.h"
#include "nodes/plannodes.h"
#include "parser/parsetree.h"
#include "parser/parse_relation.h"
#include "storage/procarray.h"
#include "utils/rel.h"

#include "catalog/ag_label.h"
#include "commands/label_commands.h"
#include "executor/cypher_executor.h"
#include "executor/cypher_utils.h"
#include "utils/agtype.h"
#include "utils/ag_cache.h"
#include "utils/graphid.h"

ResultRelInfo *create_entity_result_rel_info(EState *estate, char *graph_name, char *label_name)
{
    RangeVar *rv;
    Relation label_relation;
    ResultRelInfo *resultRelInfo;

    ParseState *pstate = make_parsestate(NULL);

    resultRelInfo = palloc(sizeof(ResultRelInfo));

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
                      list_length(estate->es_range_table), NULL,
                      estate->es_instrument);

    free_parsestate(pstate);

    return resultRelInfo;
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
    }
    if (startid == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("edge start_id field cannot be NULL")));
    }

    if (endid == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("edge end_id field cannot be NULL")));
    }

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
    ScanKeyData scan_keys[1];
    HeapScanDesc scan_desc;
    HeapTuple tuple;
    Relation rel;
    bool result = true;

    /*
     * Extract the label id from the graph id and get the table name
     * the entity is part of.
     */
    label = search_label_graph_id_cache(graph_oid, GET_LABEL_ID(id));

    // Setup the scan key to be the graphid
    ScanKeyInit(&scan_keys[0], 1, BTEqualStrategyNumber,
                F_GRAPHIDEQ, GRAPHID_GET_DATUM(id));

    rel = heap_open(label->relation, RowExclusiveLock);
    scan_desc = heap_beginscan(rel, estate->es_snapshot, 1, scan_keys);

    tuple = heap_getnext(scan_desc, ForwardScanDirection);

    /*
     * If a single tuple was returned, the tuple is still valid, otherwise'
     * set to false.
     */
    if (!HeapTupleIsValid(tuple))
    {
        result = false;
    }

    heap_endscan(scan_desc);
    heap_close(rel, RowExclusiveLock);

    return result;
}

/*
 * Insert the edge/vertex tuple into the table and indices. If the table's
 * constraints have not been violated.
 */
HeapTuple insert_entity_tuple(ResultRelInfo *resultRelInfo,
                              TupleTableSlot *elemTupleSlot,
                              EState *estate)
{
    HeapTuple tuple;

    ExecStoreVirtualTuple(elemTupleSlot);
    tuple = ExecMaterializeSlot(elemTupleSlot);

    // Check the constraints of the tuple
    tuple->t_tableOid = RelationGetRelid(resultRelInfo->ri_RelationDesc);
    if (resultRelInfo->ri_RelationDesc->rd_att->constr != NULL)
    {
        ExecConstraints(resultRelInfo, elemTupleSlot, estate);
    }

    // Insert the tuple normally
    heap_insert(resultRelInfo->ri_RelationDesc, tuple,
                GetCurrentCommandId(true), 0, NULL);

    // Insert index entries for the tuple
    if (resultRelInfo->ri_NumIndices > 0)
    {
        ExecInsertIndexTuples(elemTupleSlot, &(tuple->t_self), estate, false,
                              NULL, NIL);
    }

    return tuple;
}
