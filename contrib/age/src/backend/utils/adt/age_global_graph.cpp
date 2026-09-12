/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "postgres.h"

#include "access/heapam.h"
#include "catalog/namespace.h"
#include "storage/buf/bufmgr.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/rel.h"
#include "utils/snapmgr.h"
#include "utils/builtins.h"
#include "commands/label_commands.h"
#include "commands/trigger.h"
#include "executor/cypher_utils.h"

#include "utils/ag_cache.h"
#include "utils/age_global_graph.h"
#include "utils/agehash.h"
#include "utils/agtype.h"
#include "catalog/ag_graph.h"
#include "catalog/ag_label.h"
#include "utils/graphid.h"
#include "utils/age_graphid_ds.h"

/* defines */
#define VERTEX_HTAB_NAME "Vertex to edge lists " /* added a space at end for */
#define EDGE_HTAB_NAME "Edge to vertex mapping " /* the graph name to follow */
#define VERTEX_HTAB_INITIAL_SIZE 1000000
#define EDGE_HTAB_INITIAL_SIZE 1000000
#define MURMURHASH_FMIX_SHIFT 33
#define EDGE_RELATION_ATTRIBUTE_COUNT 4
#define EDGE_ENDPOINT_FIRST_ATTRIBUTE 1
#define EDGE_ENDPOINT_LAST_ATTRIBUTE 2

/* internal data structures implementation */
/* vertex entry for the vertex_hastable */
typedef struct vertex_entry
{
    graphid vertex_id;             /* vertex id, it is also the hash key */
    VertexEdgeArray edges_in;      /* incoming edge graphids (flat array) */
    VertexEdgeArray edges_out;     /* outgoing edge graphids (flat array) */
    VertexEdgeArray edges_self;    /* self-loop edge graphids (flat array) */
    Oid vertex_label_table_oid;    /* the label table oid */
    ItemPointerData tid;           /* physical tuple location for lazy fetch */
} vertex_entry;

/*
 * VertexEdgeArray helpers -- flat-array adjacency container used by
 * vertex_entry's edges_in / edges_out / edges_self. Growth doubles the backing
 * array; the first append allocates from the current memory context (the graph
 * global context, matching where the linked-list nodes used to live).
 */
static inline void vea_append(VertexEdgeArray *vea, graphid edge_id)
{
    if (vea->size == vea->capacity) {
        int32 new_capacity = (vea->capacity == 0) ? 4 : vea->capacity * 2;

        if (vea->array == NULL) {
            vea->array = (graphid *) palloc(new_capacity * sizeof(graphid));
        } else {
            vea->array = (graphid *) repalloc(vea->array,
                                              new_capacity * sizeof(graphid));
        }
        vea->capacity = new_capacity;
    }
    vea->array[vea->size++] = edge_id;
}

static inline void vea_free(VertexEdgeArray *vea)
{
    if (vea->array != NULL) {
        pfree(vea->array);
        vea->array = NULL;
    }
    vea->size = 0;
    vea->capacity = 0;
}

/*
 * Fast hash function for graphid (int64) keys. Replaces dynahash's tag_hash
 * (Jenkins lookup3) with the MurmurHash3 fmix64 finalizer for better
 * distribution and a lower instruction count. Signature matches HashValueFunc
 * so it can be dropped into HASHCTL.hash, and matches agehash_hash_fn so the
 * same function feeds the agehash edge table.
 */
uint32 graphid_hash(const void *key, Size keysize)
{
    uint64 k;

    /* keysize is always sizeof(int64) for every graphid hashtable */
    Assert(keysize == sizeof(int64));
    (void) keysize;

    /* graphid keys are stored as int64; callers always pass &graphid */
    memcpy(&k, key, sizeof(uint64));

    /* MurmurHash3 fmix64 (Austin Appleby, public domain). */
    k ^= k >> MURMURHASH_FMIX_SHIFT;
    k *= UINT64CONST(0xff51afd7ed558ccd);
    k ^= k >> MURMURHASH_FMIX_SHIFT;
    k *= UINT64CONST(0xc4ceb9fe1a85ec53);
    k ^= k >> MURMURHASH_FMIX_SHIFT;

    return (uint32) k;
}

/* Equality predicate for graphid (int64) keys; agehash_keyeq_fn signature. */
bool graphid_keyeq(const void *a, const void *b, Size keysize)
{
    Assert(keysize == sizeof(int64));
    (void) keysize;
    return memcmp(a, b, sizeof(int64)) == 0;
}

/*
 * Edge entry for the edge_table (agehash, INLINE mode).
 *
 * The edge_id is NOT stored here: it is the hash key and lives in the agehash
 * slot header, immediately preceding this payload. Recover it via
 * agehash_key_from_payload(ee, sizeof(graphid)) (see get_edge_entry_id). This
 * saves an 8-byte field on every edge (~400MB on SF3, ~1.4GB on SF10).
 */
typedef struct edge_entry
{
    Oid edge_label_table_oid;      /* the label table oid */
    ItemPointerData tid;           /* physical tuple location for lazy fetch */
    graphid start_vertex_id;       /* start vertex */
    graphid end_vertex_id;         /* end vertex */
} edge_entry;

/*
 * GRAPH global context per graph. They are chained together via next.
 * Be aware that the global pointer will point to the root BUT that
 * the root will change as new graphs are added to the top.
 */
typedef struct GRAPH_global_context
{
    char *graph_name;              /* graph name */
    Oid graph_oid;                 /* graph oid for searching */
    HTAB *vertex_hashtable;        /* hashtable to hold vertex edge lists */
    AgeHashTable *edge_table;      /* edge to vertex map (Robin Hood agehash) */
    MemoryContext edge_table_mcxt; /* private context owning edge_table */
    TransactionId xmin;            /* transaction ids for this graph */
    TransactionId xmax;
    CommandId curcid;              /* currentCommandId graph was created with */
    bool dirty;                    /* relation invalidation requires rebuild */
    uint64 generation;             /* identity of this loaded graph instance */
    List *loaded_relids;           /* label relations loaded into this graph */
    /*
     * Edge label relations this context was built for. NIL means "every edge
     * label", which is what callers that need the whole graph ask for. A
     * narrower set lets traversals that name their edge labels skip loading
     * unrelated edge tables, which dominates build cost on large graphs.
     */
    List *edge_label_relids;
    bool all_edge_labels_loaded;
    int64 num_loaded_vertices;     /* number of loaded vertices in this graph */
    int64 num_loaded_edges;        /* number of loaded edges in this graph */
    ListGraphId *vertices;         /* vertices for vertex hashtable cleanup */
    struct GRAPH_global_context *next; /* next graph */
} GRAPH_global_context;

/* global variable to hold the per process GRAPH global context */
static THR_LOCAL GRAPH_global_context *global_graph_contexts = NULL;
static THR_LOCAL uint64 next_graph_context_generation = 1;
static THR_LOCAL bool relcache_callback_registered = false;
static THR_LOCAL Oid cached_ag_label_relid = InvalidOid;

/* declarations */
/* GRAPH global context functions */
static void invalidate_GRAPH_global_contexts_relcache_callback(Datum argument,
                                                               Oid relid);
static void register_loaded_relid(GRAPH_global_context *ggctx, Oid relid);
static bool graph_context_loaded_relid(GRAPH_global_context *ggctx, Oid relid);
static void free_specific_GRAPH_global_context(GRAPH_global_context *ggctx);
static bool delete_specific_GRAPH_global_contexts(char *graph_name);
static bool delete_GRAPH_global_contexts(void);
static void create_GRAPH_global_hashtables(GRAPH_global_context *ggctx);
static void load_GRAPH_global_hashtables(GRAPH_global_context *ggctx);
static List *collect_endpoint_vertex_relids(GRAPH_global_context *ggctx);
static void load_vertex_hashtable(GRAPH_global_context *ggctx);
static void load_edge_hashtable(GRAPH_global_context *ggctx);
static void freeze_GRAPH_global_hashtables(GRAPH_global_context *ggctx);
static List *get_ag_labels_names(Snapshot snapshot, Oid graph_oid,
                                 char label_type);
static bool insert_edge_entry(GRAPH_global_context *ggctx, graphid edge_id,
                              ItemPointerData tid, graphid start_vertex_id,
                              graphid end_vertex_id, Oid edge_label_table_oid);
static bool insert_vertex_edge(GRAPH_global_context *ggctx,
    graphid start_vertex_id, graphid end_vertex_id,
    graphid edge_id, char *edge_label_name);
static bool insert_vertex_entry(GRAPH_global_context *ggctx, graphid vertex_id,
                                Oid vertex_label_table_oid,
                                ItemPointerData tid);
/* definitions */

static bool graph_context_loaded_relid(GRAPH_global_context *ggctx, Oid relid)
{
    ListCell *lc;

    foreach (lc, ggctx->loaded_relids)
    {
        if (lfirst_oid(lc) == relid)
            return true;
    }

    return false;
}

static void register_loaded_relid(GRAPH_global_context *ggctx, Oid relid)
{
    if (OidIsValid(relid) && !graph_context_loaded_relid(ggctx, relid))
        ggctx->loaded_relids = lappend_oid(ggctx->loaded_relids, relid);
}

void invalidate_GRAPH_global_contexts_by_relid(Oid relid)
{
    GRAPH_global_context *ggctx;

    for (ggctx = global_graph_contexts; ggctx != NULL; ggctx = ggctx->next) {
        if (!OidIsValid(relid) || graph_context_loaded_relid(ggctx, relid))
            ggctx->dirty = true;
    }
}

void notify_GRAPH_global_contexts_relation_modified(Oid relid)
{
    Assert(OidIsValid(relid));

    invalidate_GRAPH_global_contexts_by_relid(relid);
    CacheInvalidateRelcacheByRelid(relid);
}

void notify_GRAPH_global_contexts_catalog_modified(void)
{
    Oid ag_label_relid = ag_label_relation_id();

    cached_ag_label_relid = ag_label_relid;
    invalidate_GRAPH_global_contexts_by_relid(InvalidOid);
    CacheInvalidateRelcacheByRelid(ag_label_relid);
}

static void invalidate_GRAPH_global_contexts_relcache_callback(Datum argument,
                                                               Oid relid)
{
    (void)argument;

    if (!OidIsValid(relid) ||
        (OidIsValid(cached_ag_label_relid) &&
         relid == cached_ag_label_relid))
        invalidate_GRAPH_global_contexts_by_relid(InvalidOid);
    else
        invalidate_GRAPH_global_contexts_by_relid(relid);
}

void register_GRAPH_global_context_relcache_callback(void)
{
    if (relcache_callback_registered) {
        return;
    }

    CacheRegisterThreadRelcacheCallback(
        invalidate_GRAPH_global_contexts_relcache_callback,
        (Datum)0);
    relcache_callback_registered = true;
}

uint64 get_GRAPH_global_context_generation(GRAPH_global_context *ggctx)
{
    Assert(ggctx != NULL);
    return ggctx->generation;
}

/*
 * Helper function to determine validity of the passed GRAPH_global_context.
 * This is based off of the current active snaphot, to see if the graph could
 * have been modified. Ideally, we should find a way to more accurately know
 * whether the particular graph was modified.
 */
bool is_ggctx_invalid(GRAPH_global_context *ggctx)
{
    Snapshot snap = GetActiveSnapshot();

    /*
     * If the transaction ids (xmin or xmax) or currentCommandId (curcid) have
     * changed, then we have a graph that was updated. This means that the
     * global context for this graph is no longer valid.
     */
    return (ggctx->dirty ||
            ggctx->xmin != snap->xmin ||
            ggctx->xmax != snap->xmax ||
            ggctx->curcid != snap->curcid);
}
/*
 * Helper function to create the global vertex and edge hashtables. One
 * hashtable will hold the vertex, its edges (both incoming and exiting) as a
 * list, and its properties datum. The other hashtable will hold the edge, its
 * properties datum, and its source and target vertex.
 */
static void create_GRAPH_global_hashtables(GRAPH_global_context *ggctx)
{
    HASHCTL vertex_ctl;
    HASHCTL edge_ctl;
    char *graph_name = NULL;
    char *vhn = NULL;
    char *ehn = NULL;
    int glen;
    int vlen;
    int elen;

    /* get the graph name and length */
    graph_name = ggctx->graph_name;
    glen = strlen(graph_name);
    /* get the vertex htab name length */
    vlen = strlen(VERTEX_HTAB_NAME);
    /* get the edge htab name length */
    elen = strlen(EDGE_HTAB_NAME);
    /* allocate the space and build the names */
    vhn = (char *) palloc0(vlen + glen + 1);
    ehn = (char *) palloc0(elen + glen + 1);
    /* copy in the names */
    strcpy(vhn, VERTEX_HTAB_NAME);
    strcpy(ehn, EDGE_HTAB_NAME);
    /* add in the graph name */
    vhn = strncat(vhn, graph_name, glen);
    ehn = strncat(ehn, graph_name, glen);

    /* initialize the vertex hashtable */
    MemSet(&vertex_ctl, 0, sizeof(vertex_ctl));
    vertex_ctl.keysize = sizeof(int64);
    vertex_ctl.entrysize = sizeof(vertex_entry);
    vertex_ctl.hash = graphid_hash;
    vertex_ctl.hcxt = CurrentMemoryContext;
    ggctx->vertex_hashtable = hash_create(vhn, VERTEX_HTAB_INITIAL_SIZE,
                                          &vertex_ctl,
                                          HASH_ELEM | HASH_FUNCTION);
    pfree(vhn);
    /*
     * Initialize the edge_table (agehash, INLINE mode). It owns a private
     * MemoryContext as a child of CurrentMemoryContext (TopMemoryContext for
     * the lifetime of the cached GRAPH_global_context). Cleanup is a single
     * MemoryContextDelete in free_specific_GRAPH_global_context, so an elog
     * during build cannot leak slots. The edge_id key is stored in the slot
     * header, so the payload is just edge_entry.
     */
    (void) edge_ctl;
    pfree(ehn);
    ggctx->edge_table_mcxt =
        AllocSetContextCreate(CurrentMemoryContext,
                              "AGE edge_table",
                              ALLOCSET_DEFAULT_SIZES);
    ggctx->edge_table = agehash_create_inline(ggctx->edge_table_mcxt,
                                              sizeof(graphid),
                                              sizeof(edge_entry),
                                              EDGE_HTAB_INITIAL_SIZE,
                                              graphid_hash,
                                              graphid_keyeq);
}

/* helper function to get a List of all label names for the specified graph */
static List *get_ag_labels_names(Snapshot snapshot, Oid graph_oid,
                                 char label_type)
{
    List *labels = NIL;
    Relation ag_label;
    Oid ag_label_relid;
    TupleDesc tupdesc;
    AgeBtreeEqScan *scan;
    HeapTuple tuple;

    Assert(snapshot != NULL);

    ag_label_relid = ag_label_relation_id();
    cached_ag_label_relid = ag_label_relid;
    ag_label = heap_open(ag_label_relid, AccessShareLock);
    tupdesc = RelationGetDescr(ag_label);
    Assert(tupdesc->natts == Natts_ag_label);

    scan = age_btree_eq_beginscan(
        ag_label, snapshot, Anum_ag_label_graph, F_OIDEQ,
        ObjectIdGetDatum(graph_oid), AccessShareLock);

    while (HeapTupleIsValid(tuple = age_btree_eq_getnext(scan)))
    {
        bool is_null = false;
        Datum kind = heap_getattr(tuple, Anum_ag_label_kind, tupdesc,
                                  &is_null);
        if (is_null || DatumGetChar(kind) != label_type) {
            continue;
        }

        Datum name = heap_getattr(tuple, Anum_ag_label_name, tupdesc,
                                  &is_null);
        if (!is_null) {
            Name source = DatumGetName(name);
            Name copy = (Name) palloc(NAMEDATALEN);

            namestrcpy(copy, NameStr(*source));
            labels = lappend(labels, copy);
        }
    }

    age_btree_eq_endscan(scan);
    heap_close(ag_label, AccessShareLock);

    return labels;
}

/*
 * Helper function to insert one edge/edge->vertex, key/value pair, in the
 * current GRAPH global edge hashtable.
 */
static bool insert_edge_entry(GRAPH_global_context *ggctx, graphid edge_id,
                              ItemPointerData tid, graphid start_vertex_id,
                              graphid end_vertex_id, Oid edge_label_table_oid)
{
    edge_entry *ee = NULL;
    bool found = false;

    /* search for / insert the edge in the agehash edge_table */
    ee = (edge_entry *) agehash_insert(ggctx->edge_table, (void *)&edge_id,
                                       &found);
    /* agehash never returns NULL on insert; a NULL would indicate a bug. */
    if (ee == NULL) {
        elog(ERROR, "insert_edge_entry: hash table returned NULL for ee");
    }

    /*
     * If we found the key, either we have a duplicate, or we made a mistake and
     * inserted it already. Either way, this isn't good so warn and return
     * false. This way the caller can decide what to do. The previous edge's id
     * is the same edge_id (agehash key), so it is reported from the argument.
     */
    if (found) {
        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("edge: [id: %ld, start: %ld, end: %ld, label oid: %d] %s",
                        edge_id, start_vertex_id, end_vertex_id,
                        edge_label_table_oid, "duplicate edge found")));

        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("previous edge: [id: %ld, start: %ld, end: %ld, label oid: %d]",
                        edge_id, ee->start_vertex_id, ee->end_vertex_id,
                        ee->edge_label_table_oid)));

        return false;
    }

    /* not sure if we really need to zero out the entry, as we set everything */
    /*
     * agehash_insert already zero-filled the payload on a fresh insert. The
     * edge_id is the agehash slot key (recoverable via get_edge_entry_id), so
     * it is not stored in the payload.
     */
    ee->tid = tid;
    ee->start_vertex_id = start_vertex_id;
    ee->end_vertex_id = end_vertex_id;
    ee->edge_label_table_oid = edge_label_table_oid;

    /* increment the number of loaded edges */
    ggctx->num_loaded_edges++;

    return true;
}

/*
 * Helper function to insert an entire vertex into the current GRAPH global
 * vertex hashtable. It will return false if there is a duplicate.
 */
static bool insert_vertex_entry(GRAPH_global_context *ggctx, graphid vertex_id,
                                Oid vertex_label_table_oid,
                                ItemPointerData tid)
{
    vertex_entry *ve = NULL;
    bool found = false;

    /* search for the vertex */
    ve = (vertex_entry *)hash_search(ggctx->vertex_hashtable,
                                     (void *)&vertex_id, HASH_ENTER, &found);
    if (ve == NULL)
    {
        elog(ERROR, "insert_vertex_entry: hash table returned NULL for ve");
    }

    /* we should never have duplicates, warn and return false */
    if (found)
    {
        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("vertex: [id: %ld, label oid: %d] %s",
                        vertex_id, vertex_label_table_oid,
                        "duplicate vertex found")));

        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("previous vertex: [id: %ld, label oid: %d]",
                        ve->vertex_id, ve->vertex_label_table_oid)));

        return false;
    }

    /* again, MemSet may not be needed here */
    MemSet(ve, 0, sizeof(vertex_entry));

    /*
     * Set the vertex id - this is important as this is the hash key value
     * used for hash function collisions.
     */
    ve->vertex_id = vertex_id;
    /* set the label table oid for this vertex */
    ve->vertex_label_table_oid = vertex_label_table_oid;
    /* set the TID for lazy property fetch */
    ve->tid = tid;
    /* start with empty edge arrays (array == NULL, size == capacity == 0) */
    ve->edges_in.array = NULL;
    ve->edges_in.size = 0;
    ve->edges_in.capacity = 0;
    ve->edges_out.array = NULL;
    ve->edges_out.size = 0;
    ve->edges_out.capacity = 0;
    ve->edges_self.array = NULL;
    ve->edges_self.size = 0;
    ve->edges_self.capacity = 0;

    /* we also need to store the vertex id for clean up of vertex lists */
    ggctx->vertices = append_graphid(ggctx->vertices, vertex_id);

    /* increment the number of loaded vertices */
    ggctx->num_loaded_vertices++;

    return true;
}

/*
 * Helper function to append one edge to an existing vertex in the current
 * global vertex hashtable.
 */
static bool insert_vertex_edge(GRAPH_global_context *ggctx, 
                               graphid start_vertex_id, graphid end_vertex_id,
                               graphid edge_id, char *edge_label_name)
{
    vertex_entry *value = NULL;
    bool start_found = false;
    bool end_found = false;
    bool is_selfloop = false;

    /* is it a self loop */
    is_selfloop = (start_vertex_id == end_vertex_id);

    /* search for the start vertex of the edge */
    value = (vertex_entry *)hash_search(ggctx->vertex_hashtable,
                                        (void *)&start_vertex_id, HASH_FIND,
                                        &start_found);

    /*
     * If we found the start_vertex_id and it is a self loop, add the edge to
     * edges_self and we're done.
     */
    if (start_found && is_selfloop)
    {
        vea_append(&value->edges_self, edge_id);
        return true;
    } else if (start_found) {
        vea_append(&value->edges_out, edge_id);
    }

    /* search for the end vertex of the edge */
    value = (vertex_entry *)hash_search(ggctx->vertex_hashtable,
                                        (void *)&end_vertex_id, HASH_FIND,
                                        &end_found);

    if (start_found && end_found)
    {
        vea_append(&value->edges_in, edge_id);
        return true;
    }

    if (!start_found && end_found)
    {
        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("edge: [id: %ld, start: %ld, end: %ld, label: %s] %s",
                        edge_id, start_vertex_id, end_vertex_id,
                        edge_label_name, "start vertex not found")));
    } else if (start_found && !end_found) {
        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("edge: [id: %ld, start: %ld, end: %ld, label: %s] %s",
                        edge_id, start_vertex_id, end_vertex_id,
                        edge_label_name, "end vertex not found")));
    } else {
        ereport(WARNING,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("edge: [id: %ld, start: %ld, end: %ld, label: %s] %s",
                        edge_id, start_vertex_id, end_vertex_id,
                        edge_label_name, "start and end vertices not found")));
    }

    return false;
}

/*
 * Collects the vertex label relations reachable from the edge labels this
 * context loads, by reading only the label id bits of each edge's endpoint
 * graphids. Returns NIL when every edge label is loaded, meaning "no vertex
 * filtering".
 *
 * This pre-pass is far cheaper than the vertex load it lets us skip: on LDBC
 * SF1 scanning the knows endpoints costs tens of milliseconds while loading
 * every vertex label costs ~1.7 s (comment and post alone are 96% of all
 * vertices and no knows edge can reach them).
 *
 * insert_vertex_edge() looks endpoints up with HASH_FIND and warns when one is
 * missing, so every label an edge can reach must be present. Deriving the set
 * from the edges themselves guarantees that.
 */
static List *collect_endpoint_vertex_relids(GRAPH_global_context *ggctx)
{
    Oid graph_namespace_oid;
    Snapshot snapshot;
    List *edge_label_names = NIL;
    List *label_ids = NIL;
    List *vertex_relids = NIL;
    ListCell *lc;

    if (ggctx->all_edge_labels_loaded) {
        return NIL;
    }

    graph_namespace_oid = get_namespace_oid(ggctx->graph_name, false);
    snapshot = GetActiveSnapshot();
    edge_label_names = get_ag_labels_names(snapshot, ggctx->graph_oid,
        LABEL_TYPE_EDGE);

    foreach (lc, edge_label_names)
    {
        char *edge_label_name = (char *) lfirst(lc);
        Oid edge_relid = get_relname_relid(edge_label_name,
                                           graph_namespace_oid);
        Relation edge_rel;
        TableScanDesc scan_desc;
        TupleDesc tupdesc;
        HeapTuple tuple;

        if (!list_member_oid(ggctx->edge_label_relids, edge_relid)) {
            continue;
        }

        edge_rel = heap_open(edge_relid, AccessShareLock);
        ensure_age_relation_supports_raw_access(edge_rel);
        tupdesc = RelationGetDescr(edge_rel);
        if (tupdesc->natts != EDGE_RELATION_ATTRIBUTE_COUNT) {
            heap_close(edge_rel, AccessShareLock);
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_TABLE),
                     errmsg("Invalid number of attributes for %s.%s",
                            ggctx->graph_name, edge_label_name)));
        }

        scan_desc = heap_beginscan(edge_rel, snapshot, 0, NULL);
        while ((tuple = heap_getnext(scan_desc, ForwardScanDirection)) != NULL) {
            int i;

            /* attnums 1 and 2 are start_id and end_id */
            for (i = EDGE_ENDPOINT_FIRST_ATTRIBUTE;
                 i <= EDGE_ENDPOINT_LAST_ATTRIBUTE; i++) {
                graphid endpoint = DatumGetInt64(
                    column_get_datum(tupdesc, tuple, i,
                                     i == 1 ? "start_id" : "end_id",
                                     GRAPHIDOID, true));
                int32 label_id = get_graphid_label_id(endpoint);
                if (!list_member_int(label_ids, label_id)) {
                    label_ids = lappend_int(label_ids, label_id);
                }
            }
        }
        heap_endscan(scan_desc);
        heap_close(edge_rel, AccessShareLock);
    }

    /* an edge label set that matched no relation constrains nothing */
    if (label_ids == NIL) {
        return NIL;
    }

    foreach (lc, label_ids)
    {
        int32 label_id = lfirst_int(lc);
        label_cache_data *cached = search_label_graph_id_cache(ggctx->graph_oid,
                                                               label_id);

        /*
         * A label id with no catalog entry cannot be resolved to a relation.
         * Fall back to loading every vertex label rather than risk dropping
         * one an edge needs.
         */
        if (cached == NULL || !OidIsValid(cached->relation)) {
            list_free(vertex_relids);
            return NIL;
        }

        if (!list_member_oid(vertex_relids, cached->relation)) {
            vertex_relids = lappend_oid(vertex_relids, cached->relation);
        }
    }

    list_free(label_ids);

    return vertex_relids;
}

/* helper routine to load all vertices into the GRAPH global vertex hashtable */
static void load_vertex_hashtable(GRAPH_global_context *ggctx)
{
    Oid graph_oid;
    Oid graph_namespace_oid;
    Snapshot snapshot;
    List *vertex_label_names = NIL;
    List *wanted_vertex_relids = NIL;
    ListCell *lc;

    /* get the specific graph OID and namespace (schema) OID */
    graph_oid = ggctx->graph_oid;
    graph_namespace_oid = get_namespace_oid(ggctx->graph_name, false);
    /* get the active snapshot */
    snapshot = GetActiveSnapshot();
    /* get the names of all of the vertex label tables */
    vertex_label_names = get_ag_labels_names(snapshot, graph_oid,
                                             LABEL_TYPE_VERTEX);

    /* NIL means load every vertex label */
    wanted_vertex_relids = collect_endpoint_vertex_relids(ggctx);

    /* go through all vertex label tables in list */
    foreach (lc, vertex_label_names)
    {
        Relation graph_vertex_label;
        TableScanDesc scan_desc;
        HeapTuple tuple;
        char *vertex_label_name;
        Oid vertex_label_table_oid;
        TupleDesc tupdesc;

        /* get the vertex label name */
        vertex_label_name = (char *)lfirst(lc);
        /* get the vertex label name's OID */
        vertex_label_table_oid = get_relname_relid(vertex_label_name,
                                                   graph_namespace_oid);
        /* skip vertex labels no loaded edge can reach */
        if (wanted_vertex_relids != NIL &&
            !list_member_oid(wanted_vertex_relids, vertex_label_table_oid))
        {
            continue;
        }

        register_loaded_relid(ggctx, vertex_label_table_oid);
        /* open the relation (table) and begin the scan */
        graph_vertex_label = heap_open(vertex_label_table_oid, AccessShareLock);
        ensure_age_relation_supports_raw_access(graph_vertex_label);
        scan_desc = heap_beginscan(graph_vertex_label, snapshot, 0, NULL);
        /* get the tupdesc - we don't need to release this one */
        tupdesc = RelationGetDescr(graph_vertex_label);
        /* bail if the number of columns differs */
        if (tupdesc->natts != 2)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_TABLE),
                     errmsg("Invalid number of attributes for %s.%s",
                     ggctx->graph_name, vertex_label_name)));
        }
        /* get all tuples in table and insert them into graph hashtables */
        while((tuple = heap_getnext(scan_desc, ForwardScanDirection)) != NULL)
        {
            graphid vertex_id;
            bool inserted = false;

            /* something is wrong if this isn't true */
            Assert(HeapTupleIsValid(tuple));
            /* get the vertex id */
            vertex_id = DatumGetInt64(column_get_datum(tupdesc, tuple, 0, "id",
                                                       GRAPHIDOID, true));
            /* insert vertex into vertex hashtable with TID */
            inserted = insert_vertex_entry(ggctx, vertex_id,
                                           vertex_label_table_oid,
                                           tuple->t_self);
            /* warn if there is a duplicate */
            if (!inserted)
            {
                 ereport(WARNING,
                         (errcode(ERRCODE_DATA_EXCEPTION),
                          errmsg("ignored duplicate vertex")));
            }
        }

        /* end the scan and close the relation */
        heap_endscan(scan_desc);
        heap_close(graph_vertex_label, AccessShareLock);
    }
}

/*
 * Helper function to load all of the GRAPH global hashtables (vertex & edge)
 * for the current global context.
 */
static void load_GRAPH_global_hashtables(GRAPH_global_context *ggctx)
{
    /* initialize statistics */
    ggctx->num_loaded_vertices = 0;
    ggctx->num_loaded_edges = 0;

    /* insert all of our vertices */
    load_vertex_hashtable(ggctx);

    /* insert all of our edges */
    load_edge_hashtable(ggctx);
}

/*
 * Helper routine to load all edges into the GRAPH global edge and vertex
 * hashtables.
 */
static void load_edge_hashtable(GRAPH_global_context *ggctx)
{
    Oid graph_oid;
    Oid graph_namespace_oid;
    Snapshot snapshot;
    List *edge_label_names = NIL;
    ListCell *lc;

    /* get the specific graph OID and namespace (schema) OID */
    graph_oid = ggctx->graph_oid;
    graph_namespace_oid = get_namespace_oid(ggctx->graph_name, false);
    /* get the active snapshot */
    snapshot = GetActiveSnapshot();
    /* get the names of all of the edge label tables */
    edge_label_names = get_ag_labels_names(snapshot, graph_oid,
                                           LABEL_TYPE_EDGE);
    /* go through all edge label tables in list */
    foreach (lc, edge_label_names)
    {
        Relation graph_edge_label;
        TableScanDesc scan_desc;
        HeapTuple tuple;
        char *edge_label_name;
        Oid edge_label_table_oid;
        TupleDesc tupdesc;

        /* get the edge label name */
        edge_label_name = (char *)lfirst(lc);
        /* get the edge label name's OID */
        edge_label_table_oid = get_relname_relid(edge_label_name,
                                                 graph_namespace_oid);
        /* skip edge labels this context was not asked to load */
        if (!ggctx->all_edge_labels_loaded &&
            !list_member_oid(ggctx->edge_label_relids, edge_label_table_oid))
        {
            continue;
        }

        register_loaded_relid(ggctx, edge_label_table_oid);
        /* open the relation (table) and begin the scan */
        graph_edge_label = heap_open(edge_label_table_oid, AccessShareLock);
        ensure_age_relation_supports_raw_access(graph_edge_label);
        scan_desc = heap_beginscan(graph_edge_label, snapshot, 0, NULL);
        /* get the tupdesc - we don't need to release this one */
        tupdesc = RelationGetDescr(graph_edge_label);
        /* bail if the number of columns differs */
        if (tupdesc->natts != 4)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_TABLE),
                     errmsg("Invalid number of attributes for %s.%s",
                     ggctx->graph_name, edge_label_name)));
        }
        /* get all tuples in table and insert them into graph hashtables */
        while((tuple = heap_getnext(scan_desc, ForwardScanDirection)) != NULL)
        {
            graphid edge_id;
            graphid edge_vertex_start_id;
            graphid edge_vertex_end_id;
            bool inserted = false;

            /* something is wrong if this isn't true */
            Assert(HeapTupleIsValid(tuple));
            /* get the edge id */
            edge_id = DatumGetInt64(column_get_datum(tupdesc, tuple, 0, "id",
                                                     GRAPHIDOID, true));
            /* get the edge start_id (start vertex id) */
            edge_vertex_start_id = DatumGetInt64(column_get_datum(tupdesc,
                                                                  tuple, 1,
                                                                  "start_id",
                                                                  GRAPHIDOID,
                                                                  true));
            /* get the edge end_id (end vertex id)*/
            edge_vertex_end_id = DatumGetInt64(column_get_datum(tupdesc, tuple,
                                                                2, "end_id",
                                                                GRAPHIDOID,
                                                                true));
            /* insert edge into edge hashtable with TID */
            inserted = insert_edge_entry(ggctx, edge_id, tuple->t_self,
                                         edge_vertex_start_id,
                                         edge_vertex_end_id,
                                         edge_label_table_oid);
            /* warn if there is a duplicate */
            if (!inserted)
            {
                 ereport(WARNING,
                         (errcode(ERRCODE_DATA_EXCEPTION),
                          errmsg("ignored duplicate edge")));
            }

             /* insert the edge into the start and end vertices edge lists */
            inserted = insert_vertex_edge(ggctx, edge_vertex_start_id,
                                          edge_vertex_end_id, edge_id,
                                          edge_label_name);
            if (!inserted)
            {
                 ereport(WARNING,
                         (errcode(ERRCODE_DATA_EXCEPTION),
                          errmsg("ignored malformed or dangling edge")));
            }
        }

        /* end the scan and close the relation */
        heap_endscan(scan_desc);
        heap_close(graph_edge_label, AccessShareLock);
    }
}

/*
 * Helper function to freeze the GRAPH global hashtables from additional
 * inserts. This may, or may not, be useful. Currently, these hashtables are
 * only seen by the creating process and only for reading.
 */
static void freeze_GRAPH_global_hashtables(GRAPH_global_context *ggctx)
{
    hash_freeze(ggctx->vertex_hashtable);
    agehash_freeze(ggctx->edge_table);
}

/*
 * Helper function to free the entire specified GRAPH global context. After
 * running this you should not use the pointer in ggctx.
 */
static void free_specific_GRAPH_global_context(GRAPH_global_context *ggctx)
{
    GraphIdNode *curr_vertex = NULL;

    /* don't do anything if NULL */
    if (ggctx == NULL)
    {
        return;
    }

    /* free the graph name */
    pfree(ggctx->graph_name);
    ggctx->graph_name = NULL;

    list_free(ggctx->loaded_relids);
    ggctx->loaded_relids = NIL;

    list_free(ggctx->edge_label_relids);
    ggctx->edge_label_relids = NIL;

    ggctx->graph_oid = InvalidOid;
    ggctx->next = NULL;

    /* free the vertex edge lists, starting with the head */
    curr_vertex = peek_stack_head(ggctx->vertices);
    while (curr_vertex != NULL)
    {
        GraphIdNode *next_vertex = NULL;
        vertex_entry *value = NULL;
        bool found = false;
        graphid vertex_id;

        /* get the next vertex in the list, if any */
        next_vertex = next_GraphIdNode(curr_vertex);

        /* get the current vertex id */
        vertex_id = get_graphid(curr_vertex);

        /* retrieve the vertex entry */
        value = (vertex_entry *)hash_search(ggctx->vertex_hashtable,
                                            (void *)&vertex_id, HASH_FIND,
                                            &found);
        /* this is bad if it isn't found */
        Assert(found);

        /* free the edge arrays associated with this vertex */
        vea_free(&value->edges_in);
        vea_free(&value->edges_out);
        vea_free(&value->edges_self);
        /* move to the next vertex */
        curr_vertex = next_vertex;
    }

    /* free the vertices list */
    free_ListGraphId(ggctx->vertices);
    ggctx->vertices = NULL;

    /* free the vertex hashtable (dynahash) */
    hash_destroy(ggctx->vertex_hashtable);
    ggctx->vertex_hashtable = NULL;

    /*
     * Free the edge_table. All agehash slots live in edge_table_mcxt, so a
     * single MemoryContextDelete reclaims the table and every slot.
     */
    if (ggctx->edge_table_mcxt != NULL)
    {
        MemoryContextDelete(ggctx->edge_table_mcxt);
    }
    ggctx->edge_table = NULL;
    ggctx->edge_table_mcxt = NULL;

    /* free the context */
    pfree(ggctx);
    ggctx = NULL;
}

/*
 * True when a context built for loaded_relids can serve a request for
 * wanted_relids. A context holding every edge label serves any request; a
 * narrower one only serves requests contained in what it loaded.
 */
static bool edge_labels_satisfy(GRAPH_global_context *ggctx,
                                List *wanted_relids)
{
    ListCell *lc;

    if (ggctx->all_edge_labels_loaded) {
        return true;
    }

    /* a request for everything cannot be served by a filtered context */
    if (wanted_relids == NIL) {
        return false;
    }

    foreach (lc, wanted_relids)
    {
        if (!list_member_oid(ggctx->edge_label_relids, lfirst_oid(lc))) {
            return false;
        }
    }

    return true;
}

/*
 * Helper function to manage the GRAPH global contexts. It will create the
 * context for the graph specified, provided it isn't already built and valid.
 * During processing it will free (delete) all invalid GRAPH contexts. It
 * returns the GRAPH global context for the specified graph.
 *
 * edge_label_relids restricts which edge label relations get loaded. Pass NIL
 * to load every edge label. A context is reused only when what it already
 * holds covers the request; otherwise it is rebuilt. Rebuilding rather than
 * topping up matters because the hashtables are frozen after load, and callers
 * rely on entry pointers staying valid, which an incremental insert (and the
 * rehash it can trigger) would break.
 */
GRAPH_global_context *manage_GRAPH_global_contexts_for_labels(
    char *graph_name, Oid graph_oid, List *edge_label_relids)
{
    GRAPH_global_context *new_ggctx = NULL;
    GRAPH_global_context *curr_ggctx = NULL;
    GRAPH_global_context *prev_ggctx = NULL;
    MemoryContext oldctx = NULL;

    /* we need a higher context, or one that isn't destroyed by SRF exit */
    oldctx = MemoryContextSwitchTo(u_sess->cache_mem_cxt);

    /*
     * We need to see if any GRAPH global contexts already exist and if any do
     * for this particular graph. There are 5 possibilities -
     *
     *     1) There are no global contexts.
     *     2) One does exist for this graph but, is invalid.
     *     3) One does exist for this graph and is valid.
     *     4) One or more other contexts do exist and all are valid.
     *     5) One or more other contexts do exist but, one or more are invalid.
     */

    /* free the invalidated GRAPH global contexts first */
    prev_ggctx = NULL;
    curr_ggctx = global_graph_contexts;
    while (curr_ggctx != NULL) {
        GRAPH_global_context *next_ggctx = curr_ggctx->next;

        /* discard graph data invalidated by a snapshot or relcache event */
        if (is_ggctx_invalid(curr_ggctx)) {
            /*
             * If prev_ggctx is NULL then we are freeing the top of the
             * contexts. So, we need to point the global variable to the
             * new (next) top context, if there is one.
             */
            if (prev_ggctx == NULL) {
                global_graph_contexts = next_ggctx;
            } else {
                prev_ggctx->next = curr_ggctx->next;
            }

            /* free the current graph context */
            free_specific_GRAPH_global_context(curr_ggctx);
        } else {
            prev_ggctx = curr_ggctx;
        }

        /* advance to the next context */
        curr_ggctx = next_ggctx;
    }

    /* find our graph's context. if it covers the request, we are done */
    prev_ggctx = NULL;
    curr_ggctx = global_graph_contexts;
    while (curr_ggctx != NULL) {
        if (curr_ggctx->graph_oid == graph_oid) {
            if (edge_labels_satisfy(curr_ggctx, edge_label_relids)) {
                /* switch our context back */
                MemoryContextSwitchTo(oldctx);
                /* we are done */
                return curr_ggctx;
            }

            /*
             * Present but too narrow for this request. Detach and free it so
             * the rebuild below covers both the old and the new labels.
             */
            if (prev_ggctx == NULL) {
                global_graph_contexts = curr_ggctx->next;
            } else {
                prev_ggctx->next = curr_ggctx->next;
            }

            /*
             * Widen the request to also cover what the discarded context held,
             * so a caller that alternates between label sets converges instead
             * of thrashing. A NIL request already means "all labels".
             */
            if (edge_label_relids != NIL) {
                edge_label_relids =
                    list_union_oid(curr_ggctx->edge_label_relids,
                                   edge_label_relids);
            }

            free_specific_GRAPH_global_context(curr_ggctx);
            break;
        }
        prev_ggctx = curr_ggctx;
        curr_ggctx = curr_ggctx->next;
    }

    /* otherwise, we need to create one and possibly attach it */
    new_ggctx = (GRAPH_global_context *)palloc0(sizeof(GRAPH_global_context));

    if (global_graph_contexts != NULL) {
        new_ggctx->next = global_graph_contexts;
    } else {
        new_ggctx->next = NULL;
    }

    /* set the global context variable */
    global_graph_contexts = new_ggctx;

    /* set the graph name and oid */
    new_ggctx->graph_name = pstrdup(graph_name);
    new_ggctx->graph_oid = graph_oid;
    new_ggctx->dirty = false;
    new_ggctx->loaded_relids = NIL;
    new_ggctx->edge_label_relids = list_copy(edge_label_relids);
    new_ggctx->all_edge_labels_loaded = (edge_label_relids == NIL);
    if (next_graph_context_generation == 0)
        next_graph_context_generation = 1;
    new_ggctx->generation = next_graph_context_generation++;

    /* set the transaction ids */
    new_ggctx->xmin = GetActiveSnapshot()->xmin;
    new_ggctx->xmax = GetActiveSnapshot()->xmax;
    new_ggctx->curcid = GetActiveSnapshot()->curcid;

    /* initialize our vertices list */
    new_ggctx->vertices = NULL;

    /* build the hashtables for this graph */
    create_GRAPH_global_hashtables(new_ggctx);
    load_GRAPH_global_hashtables(new_ggctx);
    freeze_GRAPH_global_hashtables(new_ggctx);

    /* switch back to the previous memory context */
    MemoryContextSwitchTo(oldctx);

    return new_ggctx;
}

GRAPH_global_context *manage_GRAPH_global_contexts(char *graph_name,
                                                   Oid graph_oid)
{
    return manage_GRAPH_global_contexts_for_labels(graph_name, graph_oid, NIL);
}

/*
 * Helper function to delete all of the global graph contexts used by the
 * process. When done the global global_graph_contexts will be NULL.
 */
static bool delete_GRAPH_global_contexts(void)
{
    GRAPH_global_context *curr_ggctx = NULL;
    bool retval = false;

    /* get the first context, if any */
    curr_ggctx = global_graph_contexts;

    /* free all GRAPH global contexts */
    while (curr_ggctx != NULL)
    {
        GRAPH_global_context *next_ggctx = curr_ggctx->next;

        /* free the current graph context */
        free_specific_GRAPH_global_context(curr_ggctx);

        /* advance to the next context */
        curr_ggctx = next_ggctx;

        retval = true;
    }

    /* clear the global variable */
    global_graph_contexts = NULL;

    return retval;
}

/*
 * Helper function to delete a specific global graph context used by the
 * process.
 */
static bool delete_specific_GRAPH_global_contexts(char *graph_name)
{
    GRAPH_global_context *prev_ggctx = NULL;
    GRAPH_global_context *curr_ggctx = NULL;
    Oid graph_oid = InvalidOid;

    if (graph_name == NULL)
    {
        return false;
    }

    /* get the graph oid */
    graph_oid = get_graph_oid(graph_name);

    /* get the first context, if any */
    curr_ggctx = global_graph_contexts;

    /* find the specified GRAPH global context */
    while (curr_ggctx != NULL)
    {
        GRAPH_global_context *next_ggctx = curr_ggctx->next;

        if (curr_ggctx->graph_oid == graph_oid)
        {
            /*
             * If prev_ggctx is NULL then we are freeing the top of the
             * contexts. So, we need to point the global variable to the
             * new (next) top context, if there is one.
             */
            if (prev_ggctx == NULL)
            {
                global_graph_contexts = next_ggctx;
            }
            else
            {
                prev_ggctx->next = curr_ggctx->next;
            }

            /* free the current graph context */
            free_specific_GRAPH_global_context(curr_ggctx);

            /* we found and freed it, return true */
            return true;
        }

        /* maintain the predecessor before advancing to the next one */
        prev_ggctx = curr_ggctx;
        curr_ggctx = next_ggctx;
    }

    /* we didn't find it, return false */
    return false;
}

/*
 * Helper function to retrieve a vertex_entry from the graph's vertex hash
 * table. If there isn't one, it returns a NULL. The latter is necessary for
 * checking if the vsid and veid entries exist.
 */
vertex_entry *get_vertex_entry(GRAPH_global_context *ggctx, graphid vertex_id)
{
    vertex_entry *ve = NULL;
    bool found = false;

    /* retrieve the current vertex entry */
    ve = (vertex_entry *)hash_search(ggctx->vertex_hashtable,
                                     (void *)&vertex_id, HASH_FIND, &found);
    return ve;
}

/* helper function to retrieve an edge_entry from the graph's edge hash table */
edge_entry *get_edge_entry(GRAPH_global_context *ggctx, graphid edge_id)
{
    edge_entry *ee = NULL;

    /* retrieve the current edge entry from the agehash edge_table */
    ee = (edge_entry *) agehash_lookup(ggctx->edge_table, (void *)&edge_id);
    /* it should be found, otherwise we have problems */
    Assert(ee != NULL);

    return ee;
}

/*
 * Variant of get_edge_entry accepting a precomputed hash value, so the same
 * graphid_hash() result can be reused across paired lookups (e.g. the VLE DFS
 * edge_state_hashtable + edge_table). Caller must ensure
 * hashvalue == graphid_hash(&edge_id, sizeof(int64)).
 */
edge_entry *get_edge_entry_with_hash(GRAPH_global_context *ggctx,
                                     graphid edge_id, uint32 hashvalue)
{
    edge_entry *ee = NULL;

    ee = (edge_entry *) agehash_lookup_with_hash(ggctx->edge_table,
                                                 (void *)&edge_id, hashvalue);
    Assert(ee != NULL);

    return ee;
}

/*
 * Helper function to find the GRAPH_global_context used by the specified
 * graph_oid. If not found, it returns NULL.
 */
GRAPH_global_context *find_GRAPH_global_context(Oid graph_oid)
{
    GRAPH_global_context *ggctx = NULL;

    /* get the root */
    ggctx = global_graph_contexts;

    while(ggctx != NULL)
    {
        /* if we found it return it */
        if (ggctx->graph_oid == graph_oid)
        {
            return ggctx;
        }

        /* advance to the next context */
        ggctx = ggctx->next;
    }

    /* we did not find it so return NULL */
    return NULL;
}

/* graph vertices accessor */
ListGraphId *get_graph_vertices(GRAPH_global_context *ggctx)
{
    return ggctx->vertices;
}

/* vertex_entry accessor functions */
graphid get_vertex_entry_id(vertex_entry *ve)
{
    return ve->vertex_id;
}

VertexEdgeArray *get_vertex_entry_edges_in_array(vertex_entry *ve)
{
    return &ve->edges_in;
}

VertexEdgeArray *get_vertex_entry_edges_out_array(vertex_entry *ve)
{
    return &ve->edges_out;
}

VertexEdgeArray *get_vertex_entry_edges_self_array(vertex_entry *ve)
{
    return &ve->edges_self;
}

Oid get_vertex_entry_label_table_oid(vertex_entry *ve)
{
    return ve->vertex_label_table_oid;
}

Datum get_vertex_entry_properties(vertex_entry *ve)
{
    Relation rel;
    HeapTupleData tuple;
    Buffer buffer;
    Datum result = (Datum) 0;

    rel = heap_open(ve->vertex_label_table_oid, AccessShareLock);
    ensure_age_relation_supports_raw_access(rel);
    tuple.t_self = ve->tid;

    if (heap_fetch(rel, GetActiveSnapshot(), &tuple, &buffer, true, NULL)) {
        TupleDesc tupdesc = RelationGetDescr(rel);
        bool isnull;
        Datum props;

        props = heap_getattr(&tuple, Anum_ag_label_vertex_table_properties,
                             tupdesc, &isnull);
        if (!isnull) {
            result = datumCopy(props, false, -1);
        }

        ReleaseBuffer(buffer);
    }

    heap_close(rel, AccessShareLock);

    if (result == (Datum) 0) {
        elog(ERROR, "get_vertex_entry_properties: stale TID - "
             "vertex entry references a tuple that is no longer visible");
    }

    return result;
}

/* edge_entry accessor functions */
graphid get_edge_entry_id(edge_entry *ee)
{
    /*
     * The edge_id is stored as the agehash slot key, immediately preceding the
     * payload pointer handed back as `edge_entry *`. Recover it via the public
     * agehash_key_from_payload helper (no redundant per-entry field).
     */
    graphid k;

    memcpy(&k, agehash_key_from_payload(ee, sizeof(graphid)), sizeof(graphid));
    return k;
}

Oid get_edge_entry_label_table_oid(edge_entry *ee)
{
    return ee->edge_label_table_oid;
}

Datum get_edge_entry_properties(edge_entry *ee)
{
    Relation rel;
    HeapTupleData tuple;
    Buffer buffer;
    Datum result = (Datum) 0;

    rel = heap_open(ee->edge_label_table_oid, AccessShareLock);
    ensure_age_relation_supports_raw_access(rel);
    tuple.t_self = ee->tid;

    if (heap_fetch(rel, GetActiveSnapshot(), &tuple, &buffer, true, NULL)) {
        TupleDesc tupdesc = RelationGetDescr(rel);
        bool isnull;
        Datum props;

        props = heap_getattr(&tuple, Anum_ag_label_edge_table_properties,
                             tupdesc, &isnull);
        if (!isnull) {
            result = datumCopy(props, false, -1);
        }

        ReleaseBuffer(buffer);
    }

    heap_close(rel, AccessShareLock);

    if (result == (Datum) 0) {
        elog(ERROR, "get_edge_entry_properties: stale TID - "
             "edge entry references a tuple that is no longer visible");
    }

    return result;
}

graphid get_edge_entry_start_vertex_id(edge_entry *ee)
{
    return ee->start_vertex_id;
}

graphid get_edge_entry_end_vertex_id(edge_entry *ee)
{
    return ee->end_vertex_id;
}

/* PostgreSQL SQL facing functions */

PG_FUNCTION_INFO_V1(age_invalidate_graph_cache);
extern "C" Datum age_invalidate_graph_cache(PG_FUNCTION_ARGS);
Datum age_invalidate_graph_cache(PG_FUNCTION_ARGS)
{
    TriggerData *trigdata;
    Oid relid;

    if (!CALLED_AS_TRIGGER(fcinfo)) {
        ereport(ERROR,
                (errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
                 errmsg("age_invalidate_graph_cache: not called as trigger")));
    }

    trigdata = (TriggerData *)fcinfo->context;
    relid = RelationGetRelid(trigdata->tg_relation);
    notify_GRAPH_global_contexts_relation_modified(relid);

    PG_RETURN_POINTER(NULL);
}

/* PG wrapper function for age_delete_global_graphs */
PG_FUNCTION_INFO_V1(age_delete_global_graphs);
extern "C" Datum  age_delete_global_graphs(PG_FUNCTION_ARGS);
Datum age_delete_global_graphs(PG_FUNCTION_ARGS)
{
    agtype_value *agtv_temp = NULL;
    bool success = false;

    /* get the graph name if supplied */
    if (!PG_ARGISNULL(0))
    {
        agtv_temp = get_agtype_value("delete_global_graphs",
                                     AG_GET_ARG_AGTYPE_P(0),
                                     AGTV_STRING, false);
    }

    if (agtv_temp == NULL || agtv_temp->type == AGTV_NULL)
    {
        success = delete_GRAPH_global_contexts();
    }
    else if (agtv_temp->type == AGTV_STRING)
    {
        char *graph_name = NULL;

        graph_name = agtv_temp->val.string.val;
        success = delete_specific_GRAPH_global_contexts(graph_name);
    }
    else
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("delete_global_graphs: invalid graph name type")));
    }

    PG_RETURN_BOOL(success);
}

/* PG wrapper function for age_vertex_degree */
PG_FUNCTION_INFO_V1(age_vertex_stats);
extern "C" Datum  age_vertex_stats(PG_FUNCTION_ARGS);
Datum age_vertex_stats(PG_FUNCTION_ARGS)
{
    GRAPH_global_context *ggctx = NULL;
    vertex_entry *ve = NULL;
    VertexEdgeArray *edges = NULL;
    agtype_value *agtv_vertex = NULL;
    agtype_value *agtv_temp = NULL;
    agtype_value agtv_integer;
    agtype_in_state result;
    char *graph_name = NULL;
    Oid graph_oid = InvalidOid;
    graphid vid = 0;
    int64 self_loops = 0;
    int64 degree = 0;

    /* the graph name is required, but this generally isn't user supplied */
    if (PG_ARGISNULL(0))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("vertex_stats: graph name cannot be NULL")));
    }

    /* get the graph name */
    agtv_temp = get_agtype_value("vertex_stats", AG_GET_ARG_AGTYPE_P(0),
                                 AGTV_STRING, true);

    /* we need the vertex */
    if (PG_ARGISNULL(1))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("vertex_stats: vertex cannot be NULL")));
    }

    /* get the vertex */
    agtv_vertex = get_agtype_value("vertex_stats", AG_GET_ARG_AGTYPE_P(1),
                                   AGTV_VERTEX, true);

    graph_name = pnstrdup(agtv_temp->val.string.val,
                          agtv_temp->val.string.len);

    /* get the graph oid */
    graph_oid = get_graph_oid(graph_name);

    /*
     * Create or retrieve the GRAPH global context for this graph. This function
     * will also purge off invalidated contexts.
     */
    ggctx = manage_GRAPH_global_contexts(graph_name, graph_oid);

    /* free the graph name */
    pfree(graph_name);

    /* get the id */
    agtv_temp = GET_AGTYPE_VALUE_OBJECT_VALUE(agtv_vertex, "id");
    vid = agtv_temp->val.int_value;

    /* get the vertex entry */
    ve = get_vertex_entry(ggctx, vid);

    /* zero the state */
    memset(&result, 0, sizeof(agtype_in_state));

    /* start the object */
    result.res = push_agtype_value(&result.parse_state, WAGT_BEGIN_OBJECT,
                                   NULL);
    /* store the id */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("id"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* store the label */
    agtv_temp = GET_AGTYPE_VALUE_OBJECT_VALUE(agtv_vertex, "label");
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("label"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* set up an integer for returning values */
    agtv_temp = &agtv_integer;
    agtv_temp->type = AGTV_INTEGER;
    agtv_temp->val.int_value = 0;

    /* get and store the self_loops */
    edges = get_vertex_entry_edges_self_array(ve);
    self_loops = (edges != NULL) ? edges->size : 0;
    agtv_temp->val.int_value = self_loops;
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("self_loops"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* get and store the in_degree */
    edges = get_vertex_entry_edges_in_array(ve);
    degree = (edges != NULL) ? edges->size : 0;
    agtv_temp->val.int_value = degree + self_loops;
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("in_degree"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* get and store the out_degree */
    edges = get_vertex_entry_edges_out_array(ve);
    degree = (edges != NULL) ? edges->size : 0;
    agtv_temp->val.int_value = degree + self_loops;
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("out_degree"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* close the object */
    result.res = push_agtype_value(&result.parse_state, WAGT_END_OBJECT, NULL);

    result.res->type = AGTV_OBJECT;

    PG_RETURN_POINTER(agtype_value_to_agtype(result.res));
}

/* PG wrapper function for age_graph_stats */
PG_FUNCTION_INFO_V1(age_graph_stats);
extern "C" Datum age_graph_stats(PG_FUNCTION_ARGS);
Datum age_graph_stats(PG_FUNCTION_ARGS)
{
    GRAPH_global_context *ggctx = NULL;
    agtype_value *agtv_temp = NULL;
    agtype_value agtv_integer;
    agtype_in_state result;
    char *graph_name = NULL;
    char *graph_name_result = NULL;
    Oid graph_oid = InvalidOid;

    /* the graph name is required, but this generally isn't user supplied */
    if (PG_ARGISNULL(0)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("graph_stats: graph name cannot be NULL")));
    }

    /* get the graph name */
    agtv_temp = get_agtype_value("graph_stats", AG_GET_ARG_AGTYPE_P(0),
                                 AGTV_STRING, true);

    graph_name = pnstrdup(agtv_temp->val.string.val,
                          agtv_temp->val.string.len);
    graph_name_result = pstrdup(graph_name);

    /*
     * Remove any context for this graph. This is done to allow graph_stats to
     * show any load issues.
     */
    delete_specific_GRAPH_global_contexts(graph_name);

    /* get the graph oid */
    graph_oid = get_graph_oid(graph_name);

    /*
     * Create or retrieve the GRAPH global context for this graph. This function
     * will also purge off invalidated contexts.
     */
    ggctx = manage_GRAPH_global_contexts(graph_name, graph_oid);

    /* free the graph name */
    pfree(graph_name);

    /* zero the state */
    memset(&result, 0, sizeof(agtype_in_state));

    /* start the object */
    result.res = push_agtype_value(&result.parse_state, WAGT_BEGIN_OBJECT,
                                   NULL);
    /* store the graph name */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("graph"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE,
                                   string_to_agtype_value(graph_name_result));

    /* set up an integer for returning values */
    agtv_temp = &agtv_integer;
    agtv_temp->type = AGTV_INTEGER;
    agtv_temp->val.int_value = 0;

    /* get and store num_loaded_vertices */
    agtv_temp->val.int_value = ggctx->num_loaded_vertices;
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("num_loaded_vertices"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* get and store num_loaded_edges */
    agtv_temp->val.int_value = ggctx->num_loaded_edges;
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("num_loaded_edges"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, agtv_temp);

    /* close the object */
    result.res = push_agtype_value(&result.parse_state, WAGT_END_OBJECT, NULL);

    result.res->type = AGTV_OBJECT;

    PG_RETURN_POINTER(agtype_value_to_agtype(result.res));
}
