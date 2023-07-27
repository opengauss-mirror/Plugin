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

#include "catalog/pg_type.h"
#include "funcapi.h"
#include "utils/lsyscache.h"

#include "utils/age_vle.h"
#include "catalog/ag_graph.h"
#include "utils/graphid.h"
#include "utils/age_graphid_ds.h"
#include "nodes/cypher_nodes.h"

/* defines */
#define GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc) \
            (graphid *) (&vpc->graphid_array_data)
#define EDGE_STATE_HTAB_NAME "Edge state "
#define EDGE_STATE_HTAB_INITIAL_SIZE 100000
#define EXISTS_HTAB_NAME "known edges"
#define EXISTS_HTAB_NAME_INITIAL_SIZE 1000

/* edge state entry for the edge_state_hashtable */
typedef struct edge_state_entry
{
    graphid edge_id;               /* edge id, it is also the hash key */
    bool used_in_path;             /* like visited but more descriptive */
    bool has_been_matched;         /* have we checked for a  match */
    bool matched;                  /* is it a match */
} edge_state_entry;

/*
 * VLE_path_function is an enum for the path function to use. This currently can
 * be one of two possibilities - where the target vertex is provided and where
 * it isn't.
 */
typedef enum
{                                  /* Given a path (u)-[e]-(v)                */

    VLE_FUNCTION_PATHS_FROM,       /* Paths from a (u) without a provided (v) */
    VLE_FUNCTION_PATHS_TO,         /* Paths to a (v) without a provided (u)   */
    VLE_FUNCTION_PATHS_BETWEEN,    /* Paths between a (u) and a provided (v)  */
    VLE_FUNCTION_PATHS_ALL,        /* All paths without a provided (u) or (v) */
    VLE_FUNCTION_NONE
} VLE_path_function;

/* VLE local context per each unique age_vle function activation */
typedef struct VLE_local_context
{
    char *graph_name;              /* name of the graph */
    Oid graph_oid;                 /* graph oid for searching */
    GRAPH_global_context *ggctx;   /* global graph context pointer */
    graphid vsid;                  /* starting vertex id */
    graphid veid;                  /* ending vertex id */
    char *edge_label_name;         /* edge label name for match */
    agtype_value *edge_conditions; /* edge property conditions for match */
    int64 lidx;                    /* lower (start) bound index */
    int64 uidx;                    /* upper (end) bound index */
    bool uidx_infinite;            /* flag if the upper bound is omitted */
    cypher_rel_dir edge_direction; /* the direction of the edge */
    HTAB *edge_state_hashtable;    /* local state hashtable for our edges */
    ListGraphId *dfs_vertex_stack; /* dfs stack for vertices */
    ListGraphId *dfs_edge_stack;   /* dfs stack for edges */
    ListGraphId *dfs_path_stack;   /* dfs stack containing the path */
    VLE_path_function path_function; /* which path function to use */
    GraphIdNode *next_vertex;      /* for VLE_FUNCTION_PATHS_TO */
} VLE_local_context;

/*
 * Container to hold the graphid array that contains one valid path. This
 * structure will allow it to be easily passed as an AGTYPE pointer. The
 * structure is set up to contains a BINARY container that can be accessed by
 * functions that need to process the path.
 */
typedef struct VLE_path_container
{
    char vl_len_[4]; /* Do not touch this field! */
    uint32 header;
    uint32 graph_oid;
    int64 graphid_array_size;
    int64 container_size_bytes;
    graphid graphid_array_data;
} VLE_path_container;

/* declarations */
/* agtype functions */
static agtype_iterator *get_next_object_pair(agtype_iterator *it,
                                             agtype_container *agtc,
                                             agtype_value *key,
                                             agtype_value *value);
static bool is_an_edge_match(VLE_local_context *vlelctx, edge_entry *ee);
/* VLE local context functions */
static VLE_local_context *build_local_vle_context(FunctionCallInfo fcinfo);
static void create_VLE_local_state_hashtable(VLE_local_context *vlelctx);
static void free_VLE_local_context(VLE_local_context *vlelctx);
/* VLE graph traversal functions */
static edge_state_entry *get_edge_state(VLE_local_context *vlelctx,
                                        graphid edge_id);
/* graphid data structures */
static void load_initial_dfs_stacks(VLE_local_context *vlelctx);
static bool dfs_find_a_path_between(VLE_local_context *vlelctx);
static bool dfs_find_a_path_from(VLE_local_context *vlelctx);
static bool do_vsid_and_veid_exist(VLE_local_context *vlelctx);
static void add_valid_vertex_edges(VLE_local_context *vlelctx,
                                   graphid vertex_id);
static graphid get_next_vertex(VLE_local_context *vlelctx, edge_entry *ee);
static bool is_edge_in_path(VLE_local_context *vlelctx, graphid edge_id);
/* VLE path and edge building functions */
static VLE_path_container *create_VLE_path_container(int64 path_size);
static VLE_path_container *build_VLE_path_container(VLE_local_context *vlelctx);
static VLE_path_container *build_VLE_zero_container(VLE_local_context *vlelctx);
static agtype_value *build_path(VLE_path_container *vpc);
static agtype_value *build_edge_list(VLE_path_container *vpc);

/* definitions */
/* helper function to create the local VLE edge state hashtable. */
static void create_VLE_local_state_hashtable(VLE_local_context *vlelctx)
{
    HASHCTL edge_state_ctl;
    char *graph_name = NULL;
    char *eshn = NULL;
    int glen;
    int elen;

    /* get the graph name and length */
    graph_name = vlelctx->graph_name;
    glen = strlen(graph_name);
    /* get the edge state htab name length */
    elen = strlen(EDGE_STATE_HTAB_NAME);
    /* allocate the space and build the name */
    eshn = palloc0(elen + glen + 1);
    /* copy in the name */
    strcpy(eshn, EDGE_STATE_HTAB_NAME);
    /* add in the graph name */
    eshn = strncat(eshn, graph_name, glen);

    /* initialize the edge state hashtable */
    MemSet(&edge_state_ctl, 0, sizeof(edge_state_ctl));
    edge_state_ctl.keysize = sizeof(int64);
    edge_state_ctl.entrysize = sizeof(edge_state_entry);
    edge_state_ctl.hash = tag_hash;
    vlelctx->edge_state_hashtable = hash_create(eshn,
                                                EDGE_STATE_HTAB_INITIAL_SIZE,
                                                &edge_state_ctl,
                                                HASH_ELEM | HASH_FUNCTION);
}

/*
 * Helper function to step through an object's key/value pairs.
 * The function will return NULL if there is nothing to do. Otherwise,
 * it will return the iterator and the passed key and value agtype_values
 * will be populated. The function should initially be called with a NULL for
 * the iterator, to initialize it to the beginning of the object.
 *
 * Note: This function is only for OBJECT containers.
 */
static agtype_iterator *get_next_object_pair(agtype_iterator *it,
                                             agtype_container *agtc,
                                             agtype_value *key,
                                             agtype_value *value)
{
    agtype_iterator_token itok;
    agtype_value tmp;

    /* verify input params */
    Assert(agtc != NULL);
    Assert(key != NULL);
    Assert(value != NULL);

    /* check to see if the container is empty */
    if (AGTYPE_CONTAINER_SIZE(agtc) == 0)
    {
        return NULL;
    }

    /* if the passed iterator is NULL, this is the first time, create it */
    if (it == NULL)
    {
        /* initial the iterator */
        it = agtype_iterator_init(agtc);
        /* get the first token */
        itok = agtype_iterator_next(&it, &tmp, false);
        /* it should be WAGT_BEGIN_OBJECT */
        Assert(itok == WAGT_BEGIN_OBJECT);
    }

    /* the next token should be a key or the end of the object */
    itok = agtype_iterator_next(&it, &tmp, false);
    Assert(itok == WAGT_KEY || WAGT_END_OBJECT);
    /* if this is the end of the object return NULL */
    if (itok == WAGT_END_OBJECT)
    {
        return NULL;
    }

    /* this should be the key, copy it */
    if (itok == WAGT_KEY)
    {
        memcpy(key, &tmp, sizeof(agtype_value));
    }

    /*
     * The next token should be a value but, it could be a begin tokens for
     * arrays or objects. For those we don't want to step into them, just leave
     * them as is.
     */
    itok = agtype_iterator_next(&it, &tmp, true);
    Assert(itok == WAGT_VALUE);
    if (itok == WAGT_VALUE)
    {
        memcpy(value, &tmp, sizeof(agtype_value));
    }

    /* return the iterator */
    return it;
}

/*
 * Helper function to compare the edge conditions (properties we are looking
 * for in a matching edge) against an edge entry.
 *
 * Note: Currently the edge properties - to match - that are stored in the local
 *       context are of type agtype_value (in memory) while those from the edge
 *       are of agtype (on disk). This may change.
 */
static bool is_an_edge_match(VLE_local_context *vlelctx, edge_entry *ee)
{
    agtype_value *agtv_edge_conditions = NULL;
    agtype *agt_edge_properties = NULL;
    agtype_iterator *iterator = NULL;
    agtype_container *agtc_edge_properties = NULL;
    char *edge_label_name = NULL;
    int num_conditions = 0;
    int num_edge_properties = 0;
    int num_matches = 0;
    int property_index = 0;

    /* for easier reading */
    agtv_edge_conditions = vlelctx->edge_conditions;

    /* get the number of conditions from the prototype edge */
    num_conditions = agtv_edge_conditions->val.object.num_pairs;

    /*
     * We only care about verifying that we have all of the property conditions.
     * We don't care about extra unmatched properties. If there aren't any edge
     * conditions, then the edge passes by default.
     */
    if (vlelctx->edge_label_name == NULL && num_conditions == 0)
    {
        return true;
    }

    /* get the edge label name from the oid */
    edge_label_name = get_rel_name(get_edge_entry_label_table_oid(ee));
    /* get our edge's properties */
    agt_edge_properties = DATUM_GET_AGTYPE_P(get_edge_entry_properties(ee));
    /* get the container */
    agtc_edge_properties = &agt_edge_properties->root;
    /* get the number of properties in the edge to be matched */
    num_edge_properties = AGTYPE_CONTAINER_SIZE(agtc_edge_properties);

    /*
     * Check to see if the edge_properties object has AT LEAST as many pairs
     * to compare as the edge_conditions object has pairs. If not, it can't
     * possibly match.
     */
    if (num_conditions > num_edge_properties)
    {
        return false;
    }

    /*
     * Check for a label constraint. If the label name is NULL, there isn't one.
     */
    if (vlelctx->edge_label_name != NULL &&
        strcmp(vlelctx->edge_label_name, edge_label_name) != 0)
    {
        return false;
    }

    if (num_conditions == 0)
    {
        return true;
    }

    /*
     * Iterate through the edge's properties, matching them against the
     * condtions required.
     */
    do
    {
        agtype_value edge_property_key = {0};
        agtype_value edge_property_value = {0};

        property_index++;

        /* get the next key/value pair from the edge_properties if it exists */
        iterator = get_next_object_pair(iterator, agtc_edge_properties,
                                        &edge_property_key,
                                        &edge_property_value);

        /* if there is a pair, see if the key is in the edge conditions */
        if (iterator != NULL)
        {
            agtype_value *condition_value = NULL;

            /* get the condition_value for the specified edge_property_key */
            condition_value = get_agtype_value_object_value(agtv_edge_conditions,
                                  edge_property_key.val.string.val,
                                  edge_property_key.val.string.len);

            /* if one exists, we have a key match */
            if (condition_value != NULL)
            {
                bool match = false;

                /* are they both scalars */
                if (IS_A_AGTYPE_SCALAR(condition_value) &&
                    IS_A_AGTYPE_SCALAR(&edge_property_value))
                {
                    match = compare_agtype_scalar_values(condition_value,
                                                         &edge_property_value)
                            == 0;
                }
                /* or are they both containers */
                else if (!IS_A_AGTYPE_SCALAR(condition_value) &&
                         !IS_A_AGTYPE_SCALAR(&edge_property_value))
                {
                    agtype *agt_condition = NULL;
                    agtype_container *agtc_condition = NULL;
                    agtype_container *agtc_property = NULL;

                    /* serialize this condition */
                    agt_condition = agtype_value_to_agtype(condition_value);

                    /* put them into containers for comparison */
                    agtc_condition = &agt_condition->root;
                    agtc_property = edge_property_value.val.binary.data;

                    /* check for an exact match */
                    if (compare_agtype_containers_orderability(agtc_condition,
                                                               agtc_property)
                        == 0)
                    {
                        match = true;
                    }
                }

                /* count matches */
                if (match)
                {
                    num_matches++;
                }
            }

            /* check to see if a match is no longer possible */
            if ((num_edge_properties - property_index) <
                (num_conditions - num_matches))
            {
                pfree(iterator);
                return false;
            }
        }
    }
    while (iterator != NULL);

    return true;
}

/*
 * Helper function to free up the memory used by the VLE_local_context.
 *
 * Currently, the only structures that needs to be freed are the edge state
 * hashtable and the dfs stacks (vertex, edge, and path). The hashtable is easy
 * because hash_create packages everything into its own memory context. So, you
 * only need to do a destroy.
 */
static void free_VLE_local_context(VLE_local_context *vlelctx)
{
    /* if the VLE context is NULL, do nothing */
    if (vlelctx == NULL)
    {
        return;
    }

    /* we need to free our state hashtable */
    hash_destroy(vlelctx->edge_state_hashtable);
    vlelctx->edge_state_hashtable = NULL;

    /* we need to free our stacks */
    free_graphid_stack(vlelctx->dfs_vertex_stack);
    free_graphid_stack(vlelctx->dfs_edge_stack);
    free_graphid_stack(vlelctx->dfs_path_stack);
    pfree(vlelctx->dfs_vertex_stack);
    pfree(vlelctx->dfs_edge_stack);
    pfree(vlelctx->dfs_path_stack);
    vlelctx->dfs_vertex_stack = NULL;
    vlelctx->dfs_edge_stack = NULL;
    vlelctx->dfs_path_stack = NULL;

    /* and finally the context itself */
    pfree(vlelctx);
}

/* helper function to check if our start and end vertices exist */
static bool do_vsid_and_veid_exist(VLE_local_context *vlelctx)
{
    /* if we are only using the starting vertex */
    if (vlelctx->path_function == VLE_FUNCTION_PATHS_FROM ||
        vlelctx->path_function == VLE_FUNCTION_PATHS_ALL)
    {
        return (get_vertex_entry(vlelctx->ggctx, vlelctx->vsid) != NULL);
    }

    /* if we are only using the ending vertex */
    if (vlelctx->path_function == VLE_FUNCTION_PATHS_TO)
    {
        return (get_vertex_entry(vlelctx->ggctx, vlelctx->veid) != NULL);
    }

    /* if we are using both start and end */
    return ((get_vertex_entry(vlelctx->ggctx, vlelctx->vsid) != NULL) &&
            (get_vertex_entry(vlelctx->ggctx, vlelctx->veid) != NULL));
}

/* load the initial edges into the dfs_edge_stack */
static void load_initial_dfs_stacks(VLE_local_context *vlelctx)
{
    /*
     * If either the vsid or veid don't exist - don't load anything because
     * there won't be anything to find.
     */
    if (!do_vsid_and_veid_exist(vlelctx))
    {
        return;
    }

    /* add in the edges for the start vertex */
    add_valid_vertex_edges(vlelctx, vlelctx->vsid);
}

/*
 * Helper function to build the local VLE context. This is also the point
 * where, if necessary, the global GRAPH contexts are created and freed.
 */
static VLE_local_context *build_local_vle_context(FunctionCallInfo fcinfo)
{
    GRAPH_global_context *ggctx = NULL;
    VLE_local_context *vlelctx = NULL;
    agtype_value *agtv_temp = NULL;
    char *graph_name = NULL;
    Oid graph_oid;

    /* get the graph name - this is a required argument */
    agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(0),
                                 AGTV_STRING, true);
    graph_name = pnstrdup(agtv_temp->val.string.val,
                          agtv_temp->val.string.len);
    /* get the graph oid */
    graph_oid = get_graph_oid(graph_name);

    /*
     * Create or retrieve the GRAPH global context for this graph. This function
     * will also purge off invalidated contexts.
    */
    ggctx = manage_GRAPH_global_contexts(graph_name, graph_oid);

    /* allocate and initialize local VLE context */
    vlelctx = palloc0(sizeof(VLE_local_context));

    /* set the graph name and id */
    vlelctx->graph_name = graph_name;
    vlelctx->graph_oid = graph_oid;

    /* set the global context referenced by this local VLE context */
    vlelctx->ggctx = ggctx;

    /* initialize the path function */
    vlelctx->path_function = VLE_FUNCTION_PATHS_BETWEEN;

    /* initialize the next vertex, in this case the first */
    vlelctx->next_vertex = peek_stack_head(get_graph_vertices(ggctx));

    /* if there isn't one, the graph is empty */
    if (vlelctx->next_vertex == NULL)
    {
        elog(ERROR, "age_vle: empty graph");
    }

    /*
     * Get the start vertex id - this is an optional parameter and determines
     * which path function is used. If a start vertex isn't provided, we
     * retrieve them incrementally from the vertices list.
     */
    if (PG_ARGISNULL(1) || is_agtype_null(AG_GET_ARG_AGTYPE_P(1)))
    {
        /* set _TO */
        vlelctx->path_function = VLE_FUNCTION_PATHS_TO;

        /* get the start vertex */
        vlelctx->vsid = get_graphid(vlelctx->next_vertex);
        /* increment to the next vertex */
        vlelctx->next_vertex = next_GraphIdNode(vlelctx->next_vertex);
    }
    else
    {
        agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(1),
                                     AGTV_VERTEX, false);
        if (agtv_temp != NULL && agtv_temp->type == AGTV_VERTEX)
        {
            agtv_temp = GET_AGTYPE_VALUE_OBJECT_VALUE(agtv_temp, "id");
        }
        else if (agtv_temp == NULL || agtv_temp->type != AGTV_INTEGER)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("start vertex argument must be a vertex or the integer id")));
        }
        vlelctx->vsid = agtv_temp->val.int_value;
    }

    /*
     * Get the end vertex id - this is an optional parameter and determines
     * which path function is used.
     */
    if (PG_ARGISNULL(2) || is_agtype_null(AG_GET_ARG_AGTYPE_P(2)))
    {
        if (vlelctx->path_function == VLE_FUNCTION_PATHS_TO)
        {
            vlelctx->path_function = VLE_FUNCTION_PATHS_ALL;
        }
        else
        {
            vlelctx->path_function = VLE_FUNCTION_PATHS_FROM;
        }
        vlelctx->veid = 0;
    }
    else
    {
        agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(2),
                                     AGTV_VERTEX, false);
        if (agtv_temp != NULL && agtv_temp->type == AGTV_VERTEX)
        {
            agtv_temp = GET_AGTYPE_VALUE_OBJECT_VALUE(agtv_temp, "id");
        }
        else if (agtv_temp == NULL || agtv_temp->type != AGTV_INTEGER)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("end vertex argument must be a vertex or the integer id")));
        }
        vlelctx->path_function = VLE_FUNCTION_PATHS_BETWEEN;
        vlelctx->veid = agtv_temp->val.int_value;
    }

    /* get the VLE edge prototype */
    agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(3),
                                 AGTV_EDGE, true);

    /* get the edge prototype's property conditions */
    vlelctx->edge_conditions = GET_AGTYPE_VALUE_OBJECT_VALUE(agtv_temp,
                                                             "properties");
    /* get the edge prototype's label name */
    agtv_temp = GET_AGTYPE_VALUE_OBJECT_VALUE(agtv_temp, "label");
    if (agtv_temp->type == AGTV_STRING &&
        agtv_temp->val.string.len != 0)
    {
        vlelctx->edge_label_name = pnstrdup(agtv_temp->val.string.val,
                                            agtv_temp->val.string.len);
    }
    else
    {
        vlelctx->edge_label_name = NULL;
    }

    /* get the left range index */
    if (PG_ARGISNULL(4) || is_agtype_null(AG_GET_ARG_AGTYPE_P(4)))
    {
        vlelctx->lidx = 1;
    }
    else
    {
        agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(4),
                                     AGTV_INTEGER, true);
        vlelctx->lidx = agtv_temp->val.int_value;
    }

    /* get the right range index. NULL means infinite */
    if (PG_ARGISNULL(5) || is_agtype_null(AG_GET_ARG_AGTYPE_P(5)))
    {
        vlelctx->uidx_infinite = true;
        vlelctx->uidx = 0;
    }
    else
    {
        agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(5),
                                     AGTV_INTEGER, true);
        vlelctx->uidx = agtv_temp->val.int_value;
        vlelctx->uidx_infinite = false;
    }
    /* get edge direction */
    agtv_temp = get_agtype_value("age_vle", AG_GET_ARG_AGTYPE_P(6),
                                 AGTV_INTEGER, true);
    vlelctx->edge_direction = agtv_temp->val.int_value;

    /* create the local state hashtable */
    create_VLE_local_state_hashtable(vlelctx);

    /* initialize the dfs stacks */
    vlelctx->dfs_vertex_stack = new_graphid_stack();
    vlelctx->dfs_edge_stack = new_graphid_stack();
    vlelctx->dfs_path_stack = new_graphid_stack();

    /* load in the starting edge(s) */
    load_initial_dfs_stacks(vlelctx);

    return vlelctx;
}

/*
 * Helper function to get the specified edge's state. If it does not find it, it
 * creates and initializes it.
 */
static edge_state_entry *get_edge_state(VLE_local_context *vlelctx,
                                        graphid edge_id)
{
    edge_state_entry *ese = NULL;
    bool found = false;

    /* retrieve the edge_state_entry from the edge state hashtable */
    ese = (edge_state_entry *)hash_search(vlelctx->edge_state_hashtable,
                                          (void *)&edge_id, HASH_ENTER, &found);

    /* if it isn't found, it needs to be created and initialized */
    if (!found)
    {
        /* the edge id is also the hash key for resolving collisions */
        ese->edge_id = edge_id;
        ese->used_in_path = false;
        ese->has_been_matched = false;
        ese->matched = false;
    }
    return ese;
}

/*
 * Helper function to get the id of the next vertex to move to. This is to
 * simplify finding the next vertex due to the VLE edge's direction.
 */
static graphid get_next_vertex(VLE_local_context *vlelctx, edge_entry *ee)
{
    graphid terminal_vertex_id;

    /* get the result based on the specified VLE edge direction */
    switch (vlelctx->edge_direction)
    {
        case CYPHER_REL_DIR_RIGHT:
            terminal_vertex_id = get_edge_entry_end_vertex_id(ee);
            break;

        case CYPHER_REL_DIR_LEFT:
            terminal_vertex_id = get_edge_entry_start_vertex_id(ee);
            break;

        case CYPHER_REL_DIR_NONE:
        {
            ListGraphId *vertex_stack = NULL;
            graphid parent_vertex_id;

            vertex_stack = vlelctx->dfs_vertex_stack;
            /*
             * Get the parent vertex of this edge. When we are looking at edges
             * as un-directional, where we go to next depends on where we came
             * from. This is because we can go against an edge.
             */
            parent_vertex_id = PEEK_GRAPHID_STACK(vertex_stack);
            /* find the terminal vertex */
            if (get_edge_entry_start_vertex_id(ee) == parent_vertex_id)
            {
                terminal_vertex_id = get_edge_entry_end_vertex_id(ee);
            }
            else if (get_edge_entry_end_vertex_id(ee) == parent_vertex_id)
            {
                terminal_vertex_id = get_edge_entry_start_vertex_id(ee);
            }
            else
            {
                elog(ERROR, "get_next_vertex: no parent match");
            }

            break;
        }

        default:
            elog(ERROR, "get_next_vertex: unknown edge direction");
    }

    return terminal_vertex_id;
}

/*
 * Helper function to find one path BETWEEN two vertices.
 *
 * Note: On the very first entry into this function, the starting vertex's edges
 * should have already been loaded into the edge stack (this should have been
 * done by the SRF initialization phase).
 *
 * This function will always return on either a valid path found (true) or none
 * found (false). If one is found, the position (vertex & edge) will still be in
 * the stack. Each successive invocation within the SRF will then look for the
 * next available path until there aren't any left.
 */
static bool dfs_find_a_path_between(VLE_local_context *vlelctx)
{
    ListGraphId *vertex_stack = NULL;
    ListGraphId *edge_stack = NULL;
    ListGraphId *path_stack = NULL;
    graphid end_vertex_id;

    Assert(vlelctx != NULL);

    /* for ease of reading */
    vertex_stack = vlelctx->dfs_vertex_stack;
    edge_stack = vlelctx->dfs_edge_stack;
    path_stack = vlelctx->dfs_path_stack;
    end_vertex_id = vlelctx->veid;

    /* while we have edges to process */
    while (!(IS_GRAPHID_STACK_EMPTY(edge_stack)))
    {
        graphid edge_id;
        graphid next_vertex_id;
        edge_state_entry *ese = NULL;
        edge_entry *ee = NULL;
        bool found = false;

        /* get an edge, but leave it on the stack for now */
        edge_id = PEEK_GRAPHID_STACK(edge_stack);
        /* get the edge's state */
        ese = get_edge_state(vlelctx, edge_id);
        /*
         * If the edge is already in use, it means that the edge is in the path.
         * So, we need to see if it is the last path entry (we are backing up -
         * we need to remove the edge from the path stack and reset its state
         * and from the edge stack as we are done with it) or an interior edge
         * in the path (loop - we need to remove the edge from the edge stack
         * and start with the next edge).
         */
        if (ese->used_in_path)
        {
            graphid path_edge_id;

            /* get the edge id on the top of the path stack (last edge) */
            path_edge_id = PEEK_GRAPHID_STACK(path_stack);
            /*
             * If the ids are the same, we're backing up. So, remove it from the
             * path stack and reset used_in_path.
             */
            if (edge_id == path_edge_id)
            {
                pop_graphid_stack(path_stack);
                ese->used_in_path = false;
            }
            /* now remove it from the edge stack */
            pop_graphid_stack(edge_stack);
            /*
             * Remove its source vertex, if we are looking at edges as
             * un-directional. We only maintain the vertex stack when the
             * edge_direction is CYPHER_REL_DIR_NONE. This is to save space
             * and time.
             */
            if (vlelctx->edge_direction == CYPHER_REL_DIR_NONE)
            {
                pop_graphid_stack(vertex_stack);
            }
            /* move to the next edge */
            continue;
        }

        /*
         * Mark it and push it on the path stack. There is no need to push it on
         * the edge stack as it is already there.
         */
        ese->used_in_path = true;
        push_graphid_stack(path_stack, edge_id);

        /* now get the edge entry so we can get the next vertex to move to */
        ee = get_edge_entry(vlelctx->ggctx, edge_id);
        next_vertex_id = get_next_vertex(vlelctx, ee);

        /*
         * Is this the end of a path that meets our requirements? Is its length
         * within the bounds specified?
         */
        if (next_vertex_id == end_vertex_id &&
            get_stack_size(path_stack) >= vlelctx->lidx &&
            (vlelctx->uidx_infinite ||
             get_stack_size(path_stack) <= vlelctx->uidx))
        {
            /* we found one */
            found = true;
        }
        /*
         * If we have found the end vertex but, we are not within our upper
         * bounds, we need to back up. We still need to continue traversing
         * the graph if we aren't within our lower bounds, though.
         */
        if (next_vertex_id == end_vertex_id &&
            !vlelctx->uidx_infinite &&
            get_stack_size(path_stack) > vlelctx->uidx)
        {
            continue;
        }

        /* add in the edges for the next vertex if we won't exceed the bounds */
        if (vlelctx->uidx_infinite ||
            get_stack_size(path_stack) < vlelctx->uidx)
        {
            add_valid_vertex_edges(vlelctx, next_vertex_id);
        }

        if (found)
        {
            return true;
        }
    }

    return false;
}

/*
 * Helper function to find one path FROM a start vertex.
 *
 * Note: On the very first entry into this function, the starting vertex's edges
 * should have already been loaded into the edge stack (this should have been
 * done by the SRF initialization phase).
 *
 * This function will always return on either a valid path found (true) or none
 * found (false). If one is found, the position (vertex & edge) will still be in
 * the stack. Each successive invocation within the SRF will then look for the
 * next available path until there aren't any left.
 */
static bool dfs_find_a_path_from(VLE_local_context *vlelctx)
{
    ListGraphId *vertex_stack = NULL;
    ListGraphId *edge_stack = NULL;
    ListGraphId *path_stack = NULL;

    Assert(vlelctx != NULL);

    /* for ease of reading */
    vertex_stack = vlelctx->dfs_vertex_stack;
    edge_stack = vlelctx->dfs_edge_stack;
    path_stack = vlelctx->dfs_path_stack;

    /* while we have edges to process */
    while (!(IS_GRAPHID_STACK_EMPTY(edge_stack)))
    {
        graphid edge_id;
        graphid next_vertex_id;
        edge_state_entry *ese = NULL;
        edge_entry *ee = NULL;
        bool found = false;

        /* get an edge, but leave it on the stack for now */
        edge_id = PEEK_GRAPHID_STACK(edge_stack);
        /* get the edge's state */
        ese = get_edge_state(vlelctx, edge_id);
        /*
         * If the edge is already in use, it means that the edge is in the path.
         * So, we need to see if it is the last path entry (we are backing up -
         * we need to remove the edge from the path stack and reset its state
         * and from the edge stack as we are done with it) or an interior edge
         * in the path (loop - we need to remove the edge from the edge stack
         * and start with the next edge).
         */
        if (ese->used_in_path)
        {
            graphid path_edge_id;

            /* get the edge id on the top of the path stack (last edge) */
            path_edge_id = PEEK_GRAPHID_STACK(path_stack);
            /*
             * If the ids are the same, we're backing up. So, remove it from the
             * path stack and reset used_in_path.
             */
            if (edge_id == path_edge_id)
            {
                pop_graphid_stack(path_stack);
                ese->used_in_path = false;
            }
            /* now remove it from the edge stack */
            pop_graphid_stack(edge_stack);
            /*
             * Remove its source vertex, if we are looking at edges as
             * un-directional. We only maintain the vertex stack when the
             * edge_direction is CYPHER_REL_DIR_NONE. This is to save space
             * and time.
             */
            if (vlelctx->edge_direction == CYPHER_REL_DIR_NONE)
            {
                pop_graphid_stack(vertex_stack);
            }
            /* move to the next edge */
            continue;
        }

        /*
         * Mark it and push it on the path stack. There is no need to push it on
         * the edge stack as it is already there.
         */
        ese->used_in_path = true;
        push_graphid_stack(path_stack, edge_id);

        /* now get the edge entry so we can get the next vertex to move to */
        ee = get_edge_entry(vlelctx->ggctx, edge_id);
        next_vertex_id = get_next_vertex(vlelctx, ee);

        /*
         * Is this a path that meets our requirements? Is its length within the
         * bounds specified?
         */
        if (get_stack_size(path_stack) >= vlelctx->lidx &&
            (vlelctx->uidx_infinite ||
             get_stack_size(path_stack) <= vlelctx->uidx))
        {
            /* we found one */
            found = true;
        }

        /* add in the edges for the next vertex if we won't exceed the bounds */
        if (vlelctx->uidx_infinite ||
            get_stack_size(path_stack) < vlelctx->uidx)
        {
            add_valid_vertex_edges(vlelctx, next_vertex_id);
        }

        if (found)
        {
            return true;
        }
    }

    return false;
}

/*
 * Helper routine to quickly check if an edge_id is in the path stack. It is
 * only meant as a quick check to avoid doing a much more costly hash search for
 * smaller sized lists. But, it is O(n) so it should only be used for small
 * path_stacks and where appropriate.
 */
static bool is_edge_in_path(VLE_local_context *vlelctx, graphid edge_id)
{
    GraphIdNode *edge = NULL;

    /* start at the top of the stack */
    edge = peek_stack_head(vlelctx->dfs_path_stack);

    /* go through the path stack, return true if we find the edge */
    while (edge != NULL)
    {
        if (get_graphid(edge) == edge_id)
        {
            return true;
        }
        /* get the next stack element */
        edge = next_GraphIdNode(edge);
    }
    /* we didn't find it if we get here */
    return false;
}

/*
 * Helper function to add in valid vertex edges as part of the dfs path
 * algorithm. What constitutes a valid edge is the following -
 *
 *     1) Edge matches the correct direction specified.
 *     2) Edge is not currently in the path.
 *     3) Edge matches minimum edge properties specified.
 *
 * Note: The vertex must exist.
 * Note: Edge lists contain both entering and exiting edges. The direction
 *       determines which is followed.
 */
static void add_valid_vertex_edges(VLE_local_context *vlelctx,
                                   graphid vertex_id)
{
    ListGraphId *vertex_stack = NULL;
    ListGraphId *edge_stack = NULL;
    vertex_entry *ve = NULL;
    GraphIdNode *edge = NULL;

    /* get the vertex entry */
    ve = get_vertex_entry(vlelctx->ggctx, vertex_id);
    /* there better be a valid vertex */
    Assert(ve != NULL);
    /* point to stacks */
    vertex_stack = vlelctx->dfs_vertex_stack;
    edge_stack = vlelctx->dfs_edge_stack;
    /* get a pointer to the first edge */
    if (get_vertex_entry_edges(ve) != NULL)
    {
        edge = peek_stack_head(get_vertex_entry_edges(ve));
    }
    else
    {
        edge = NULL;
    }
    /* add in the vertex's edge(s) */
    while (edge != NULL)
    {
        edge_entry *ee = NULL;
        edge_state_entry *ese = NULL;
        graphid edge_id;

        /* get the edge_id */
        edge_id = get_graphid(edge);

        /*
         * This is a fast existence check, relative to the hash search, for when
         * the path stack is small.
         */
        if (get_stack_size(vlelctx->dfs_path_stack) < 10 &&
            is_edge_in_path(vlelctx, edge_id))
        {
            edge = next_GraphIdNode(edge);
            continue;
        }

        /* get the edge entry */
        ee = get_edge_entry(vlelctx->ggctx, edge_id);
        /* it better exist */
        Assert(ee != NULL);

        /* is the edge going in the correct direction */
        if ((vlelctx->edge_direction == CYPHER_REL_DIR_NONE) ||
            (vertex_id == get_edge_entry_start_vertex_id(ee) &&
             vlelctx->edge_direction == CYPHER_REL_DIR_RIGHT) ||
            (vertex_id == get_edge_entry_end_vertex_id(ee) &&
             vlelctx->edge_direction == CYPHER_REL_DIR_LEFT))
        {
            /* get its state */
            ese = get_edge_state(vlelctx, edge_id);
            /*
             * Don't add any edges that we have already seen because they will
             * cause a loop to form.
             */
            if (!ese->used_in_path)
            {
                /* validate the edge if it hasn't been already */
                if (!ese->has_been_matched && is_an_edge_match(vlelctx, ee))
                {
                    ese->has_been_matched = true;
                    ese->matched = true;
                }
                else if (!ese->has_been_matched)
                {
                    ese->has_been_matched = true;
                    ese->matched = false;
                }
                /* if it is a match, add it */
                if (ese->has_been_matched && ese->matched)
                {
                    /*
                     * We need to maintain our source vertex for each edge added
                     * if the edge_direction is CYPHER_REL_DIR_NONE. This is due
                     * to the edges having a fixed direction and the dfs
                     * algorithm working strictly through edges. With an
                     * un-directional VLE edge, you don't know the vertex that
                     * you just came from. So, we need to store it.
                     */
                    if (vlelctx->edge_direction == CYPHER_REL_DIR_NONE)
                    {
                        push_graphid_stack(vertex_stack,
                                           get_vertex_entry_id(ve));
                    }
                    push_graphid_stack(edge_stack, edge_id);
                }
            }
        }
        edge = next_GraphIdNode(edge);
    }
}

/*
 * Helper function to create the VLE path container that holds the graphid array
 * containing the found path. The path_size is the total number of vertices and
 * edges in the path.
 */
static VLE_path_container *create_VLE_path_container(int64 path_size)
{
    VLE_path_container *vpc = NULL;
    int container_size_bytes = 0;

    /*
     * For the total container size (in graphids int64s) we need to add the
     * following space (in graphids) to hold each of the following fields -
     *
     *     One for the VARHDRSZ which is a int32 and a pad of 32.
     *     One for both the header and graph oid (they are both 32 bits).
     *     One for the size of the graphid_array_size.
     *     One for the container_size_bytes.
     *
     */
    container_size_bytes = sizeof(graphid) * (path_size + 4);

    /* allocate the container */
    vpc = palloc0(container_size_bytes);

    /* initialze the PG headers */
    SET_VARSIZE(vpc, container_size_bytes);

    /* initialize the container */
    vpc->header = AGT_FBINARY | AGT_FBINARY_TYPE_VLE_PATH;
    vpc->graphid_array_size = path_size;
    vpc->container_size_bytes = container_size_bytes;

    /* the graphid array is already zeroed out */
    /* all of the other fields are set by the caller */

    return vpc;
}

/*
 * Helper function to build a VLE_path_container containing the graphid array
 * from the path_stack. The graphid array will be a complete path (vertices and
 * edges interleaved) -
 *
 *     start vertex, first edge,... nth edge, end vertex
 *
 * The VLE_path_container is allocated in such a way as to wrap the array and
 * include the following additional data -
 *
 *     The header is to allow the graphid array to be encoded as an agtype
 *     container of type BINARY. This way the array doesn't need to be
 *     transformed back and forth.
 *
 *     The graph oid to facilitate the retrieval of the correct vertex and edge
 *     entries.
 *
 *     The total number of elements in the array.
 *
 *     The total size of the container for copying.
 *
 * Note: Remember to pfree it when done. Even though it should be destroyed on
 *       exiting the SRF context.
 */

static VLE_path_container *build_VLE_path_container(VLE_local_context *vlelctx)
{
    ListGraphId *stack = vlelctx->dfs_path_stack;
    VLE_path_container *vpc = NULL;
    graphid *graphid_array = NULL;
    GraphIdNode *edge = NULL;
    graphid vid = 0;
    int index = 0;
    int ssize = 0;

    if (stack == NULL)
    {
        return NULL;
    }

    /* allocate the graphid array */
    ssize = get_stack_size(stack);

    /*
     * Create the container. Note that the path size will always be 2 times the
     * number of edges plus 1 -> (u)-[e]-(v)
     */
    vpc = create_VLE_path_container((ssize * 2) + 1);

    /* set the graph_oid */
    vpc->graph_oid = vlelctx->graph_oid;

    /* get the graphid_array from the container */
    graphid_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc);

    /* get and store the start vertex */
    vid = vlelctx->vsid;
    graphid_array[0] = vid;

    /* get the head of the stack */
    edge = peek_stack_head(stack);

    /*
     * We need to fill in the array from the back to the front. This is due
     * to the order of the path stack - last in first out. Remember that the
     * last entry is a vertex.
     */
    index = vpc->graphid_array_size - 2;

    /* copy while we have an edge to copy */
    while (edge != NULL)
    {
        /* 0 is the vsid, we should never get here */
        Assert(index > 0);

        /* store and set to the next edge */
        graphid_array[index] = get_graphid(edge);
        edge = next_GraphIdNode(edge);

        /* we need to skip over the interior vertices */
        index -= 2;
    }

    /* now add in the interior vertices, starting from the first edge */
    for (index = 1; index < vpc->graphid_array_size - 1; index += 2)
    {
        edge_entry *ee = NULL;

        ee = get_edge_entry(vlelctx->ggctx, graphid_array[index]);
        vid = (vid == get_edge_entry_start_vertex_id(ee)) ?
                   get_edge_entry_end_vertex_id(ee) :
                   get_edge_entry_start_vertex_id(ee);
        graphid_array[index+1] = vid;
    }

    /* return the container */
    return vpc;
}

/* helper function to build a VPC for just the start vertex */
static VLE_path_container *build_VLE_zero_container(VLE_local_context *vlelctx)
{
    ListGraphId *stack = vlelctx->dfs_path_stack;
    VLE_path_container *vpc = NULL;
    graphid *graphid_array = NULL;
    graphid vid = 0;

    /* we should have an empty stack */
    Assert(get_stack_size(stack) == 0);

    /*
     * Create the container. Note that the path size will always be 1 as this is
     * just the starting vertex.
     */
    vpc = create_VLE_path_container(1);

    /* set the graph_oid */
    vpc->graph_oid = vlelctx->graph_oid;

    /* get the graphid_array from the container */
    graphid_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc);

    /* get and store the start vertex */
    vid = vlelctx->vsid;
    graphid_array[0] = vid;

    return vpc;
}

/*
 * Helper function to build an AGTV_ARRAY of edges from an array of graphids.
 *
 * Note: You should free the array when done. Although, it should be freed
 *       when the context is destroyed from the return of the SRF call.
 */
static agtype_value *build_edge_list(VLE_path_container *vpc)
{
    GRAPH_global_context *ggctx = NULL;
    agtype_in_state edges_result;
    Oid graph_oid = InvalidOid;
    graphid *graphid_array = NULL;
    int64 graphid_array_size = 0;
    int index = 0;

    /* get the graph_oid */
    graph_oid = vpc->graph_oid;

    /* get the GRAPH global context for this graph */
    ggctx = find_GRAPH_global_context(graph_oid);
    /* verify we got a global context */
    Assert(ggctx != NULL);

    /* get the graphid_array and size */
    graphid_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc);
    graphid_array_size = vpc->graphid_array_size;

    /* initialize our agtype array */
    MemSet(&edges_result, 0, sizeof(agtype_in_state));
    edges_result.res = push_agtype_value(&edges_result.parse_state,
                                         WAGT_BEGIN_ARRAY, NULL);

    for (index = 1; index < graphid_array_size - 1; index += 2)
    {
        char *label_name = NULL;
        edge_entry *ee = NULL;
        agtype_value *agtv_edge = NULL;

        /* get the edge entry from the hashtable */
        ee = get_edge_entry(ggctx, graphid_array[index]);
        /* get the label name from the oid */
        label_name = get_rel_name(get_edge_entry_label_table_oid(ee));
        /* reconstruct the edge */
        agtv_edge = agtype_value_build_edge(get_edge_entry_id(ee), label_name,
                                            get_edge_entry_end_vertex_id(ee),
                                            get_edge_entry_start_vertex_id(ee),
                                            get_edge_entry_properties(ee));
        /* push the edge*/
        edges_result.res = push_agtype_value(&edges_result.parse_state,
                                             WAGT_ELEM, agtv_edge);
    }

    /* close our agtype array */
    edges_result.res = push_agtype_value(&edges_result.parse_state,
                                         WAGT_END_ARRAY, NULL);

    /* make it an array */
    edges_result.res->type = AGTV_ARRAY;

    /* return it */
    return edges_result.res;
}

/*
 * Helper function to build an array of type AGTV_PATH from an array of
 * graphids.
 *
 * Note: You should free the array when done. Although, it should be freed
 *       when the context is destroyed from the return of the SRF call.
 */
static agtype_value *build_path(VLE_path_container *vpc)
{
    GRAPH_global_context *ggctx = NULL;
    agtype_in_state path_result;
    Oid graph_oid = InvalidOid;
    graphid *graphid_array = NULL;
    int64 graphid_array_size = 0;
    int index = 0;

    /* get the graph_oid */
    graph_oid = vpc->graph_oid;

    /* get the GRAPH global context for this graph */
    ggctx = find_GRAPH_global_context(graph_oid);
    /* verify we got a global context */
    Assert(ggctx != NULL);

    /* get the graphid_array and size */
    graphid_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc);
    graphid_array_size = vpc->graphid_array_size;

    /* initialize our agtype array */
    MemSet(&path_result, 0, sizeof(agtype_in_state));
    path_result.res = push_agtype_value(&path_result.parse_state,
                                        WAGT_BEGIN_ARRAY, NULL);

    for (index = 0; index < graphid_array_size; index += 2)
    {
        char *label_name = NULL;
        vertex_entry *ve = NULL;
        edge_entry *ee = NULL;
        agtype_value *agtv_vertex = NULL;
        agtype_value *agtv_edge = NULL;

        /* get the vertex entry from the hashtable */
        ve = get_vertex_entry(ggctx, graphid_array[index]);
        /* get the label name from the oid */
        label_name = get_rel_name(get_vertex_entry_label_table_oid(ve));
        /* reconstruct the vertex */
        agtv_vertex = agtype_value_build_vertex(get_vertex_entry_id(ve),
                                                label_name,
                                                get_vertex_entry_properties(ve));
        /* push the vertex */
        path_result.res = push_agtype_value(&path_result.parse_state, WAGT_ELEM,
                                            agtv_vertex);

        /*
         * Remember that we have more vertices than edges. So, we need to check
         * if the above vertex was the last vertex in the path.
         */
        if (index + 1 >= graphid_array_size)
        {
            break;
        }

        /* get the edge entry from the hashtable */
        ee = get_edge_entry(ggctx, graphid_array[index+1]);
        /* get the label name from the oid */
        label_name = get_rel_name(get_edge_entry_label_table_oid(ee));
        /* reconstruct the edge */
        agtv_edge = agtype_value_build_edge(get_edge_entry_id(ee), label_name,
                                            get_edge_entry_end_vertex_id(ee),
                                            get_edge_entry_start_vertex_id(ee),
                                            get_edge_entry_properties(ee));
        /* push the edge*/
        path_result.res = push_agtype_value(&path_result.parse_state, WAGT_ELEM,
                                            agtv_edge);
    }

    /* close our agtype array */
    path_result.res = push_agtype_value(&path_result.parse_state,
                                        WAGT_END_ARRAY, NULL);

    /* make it a path */
    path_result.res->type = AGTV_PATH;

    /* return the path */
    return path_result.res;
}

/*
 * All front facing PG and exposed functions below
 */

/*
 * PG VLE function that takes the following input and returns a row called edges
 * of type agtype BINARY VLE_path_container (this is an internal structure for
 * returning a graphid array of the path. You need to use internal routines to
 * properly use this data) -
 *
 *     0 - agtype REQUIRED (graph name as string)
 *                 Note: This is automatically added by transform_FuncCall.
 *
 *     1 - agtype OPTIONAL (start vertex as a vertex or the integer id)
 *                 Note: Leaving this NULL switches the path algorithm from
 *                       VLE_FUNCTION_PATHS_BETWEEN to VLE_FUNCTION_PATHS_TO
 *     2 - agtype OPTIONAL (end vertex as a vertex or the integer id)
 *                 Note: Leaving this NULL switches the path algorithm from
 *                       VLE_FUNCTION_PATHS_BETWEEN to VLE_FUNCTION_PATHS_FROM
 *                       or - if the starting vertex is NULL - from
 *                       VLE_FUNCTION_PATHS_TO to VLE_FUNCTION_PATHS_ALL
 *     3 - agtype REQUIRED (edge prototype to match as an edge)
 *                 Note: Only the label and properties are used. The
 *                       rest is ignored.
 *     4 - agtype OPTIONAL lidx (lower range index)
 *                 Note: 0 itself is currently not supported but here it is
 *                       equivalent to 1.
 *                       A NULL is appropriate here for a 0 lower bound.
 *     5 - agtype OPTIONAL uidx (upper range index)
 *                 Note: A NULL is appropriate here for an infinite upper bound.
 *     6 - agtype REQUIRED edge direction (enum) as an integer. REQUIRED
 *
 * This is a set returning function. This means that the first call sets
 * up the initial structures and then outputs the first row. After that each
 * subsequent call generates one row of output data. PG will continue to call
 * the function until the function tells PG that it doesn't have any more rows
 * to output. At that point, the function needs to clean up all of its data
 * structures that are not meant to last between SRFs.
 */
PG_FUNCTION_INFO_V1(age_vle);

Datum age_vle(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;
    VLE_local_context *vlelctx = NULL;
    bool found_a_path = false;
    bool done = false;
    bool is_zero_bound = false;
    MemoryContext oldctx;

    /* Initialization for the first call to the SRF */
    if (SRF_IS_FIRSTCALL())
    {
        /* all of these arguments need to be non NULL */
        if (PG_ARGISNULL(0) || /* graph name */
            PG_ARGISNULL(3) || /* edge prototype */
            PG_ARGISNULL(6))   /* direction */
        {
             ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("age_vle: invalid NULL argument passed")));
        }

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /* switch to memory context appropriate for multiple function calls */
        oldctx = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* build the local vle context */
        vlelctx = build_local_vle_context(fcinfo);

        /*
         * Point the function call context's user pointer to the local VLE
         * context just created
         */
        funcctx->user_fctx = vlelctx;

        MemoryContextSwitchTo(oldctx);

        /* if we are starting from zero [*0..x] flag it */
        if (vlelctx->lidx == 0)
        {
            is_zero_bound = true;
            done = true;
        }
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();

    /* restore our VLE local context */
    vlelctx = (VLE_local_context *)funcctx->user_fctx;

    /*
     * All work done in dfs_find_a_path needs to be done in a context that
     * survives multiple SRF calls. So switch to the appropriate context.
     */
    oldctx = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

    while (done == false)
    {
        /* find one path based on specific input */
        switch (vlelctx->path_function)
        {
            case VLE_FUNCTION_PATHS_TO:
            case VLE_FUNCTION_PATHS_BETWEEN:
                found_a_path = dfs_find_a_path_between(vlelctx);
                break;

            case VLE_FUNCTION_PATHS_ALL:
            case VLE_FUNCTION_PATHS_FROM:
                found_a_path = dfs_find_a_path_from(vlelctx);
                break;

            default:
                found_a_path = false;
                break;
        }

        /* if we found a path, or are done, flag it so we can output the data */
        if (found_a_path == true ||
            (found_a_path == false && vlelctx->next_vertex == NULL) ||
            (found_a_path == false &&
             (vlelctx->path_function == VLE_FUNCTION_PATHS_BETWEEN ||
              vlelctx->path_function == VLE_FUNCTION_PATHS_FROM)))
        {
            done = true;
        }
        /* if we need to fetch a new vertex and rerun the find */
        else if ((vlelctx->path_function == VLE_FUNCTION_PATHS_ALL) ||
                 (vlelctx->path_function == VLE_FUNCTION_PATHS_TO))
        {
            /* get the next start vertex id */
            vlelctx->vsid = get_graphid(vlelctx->next_vertex);

            /* increment to the next vertex */
            vlelctx->next_vertex = next_GraphIdNode(vlelctx->next_vertex);

            /* load in the starting edge(s) */
            load_initial_dfs_stacks(vlelctx);

            /* if we are starting from zero [*0..x] flag it */
            if (vlelctx->lidx == 0)
            {
                is_zero_bound = true;
                done = true;
            }
            /* otherwise we need to loop back around */
            else
            {
                done = false;
            }
        }
        /* we shouldn't get here */
        else
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("age_vle() invalid path function")));
        }
    }

    /* switch back to a more volatile context */
    MemoryContextSwitchTo(oldctx);

    /*
     * If we find a path, we need to convert the path_stack into a list that
     * the outside world can use.
     */
    if (found_a_path || is_zero_bound)
    {
        VLE_path_container *vpc = NULL;

        /* if this isn't the zero boundary case generate a normal vpc */
        if (!is_zero_bound)
        {
            /* the path_stack should have something in it if we have a path */
            Assert(vlelctx->dfs_path_stack > 0);

            /*
             * Build the graphid array into a VLE_path_container from the
             * path_stack. This will also correct for the path_stack being last
             * in, first out.
             */
            vpc = build_VLE_path_container(vlelctx);
        }
        /* otherwise, this is the zero boundary case [*0..x] */
        else
        {
            vpc = build_VLE_zero_container(vlelctx);
        }

        /* return the result and signal that the function is not yet done */
        SRF_RETURN_NEXT(funcctx, PointerGetDatum(vpc));
    }
    /* otherwise, we are done and we need to cleanup and signal done */
    else
    {
        /* free the local context */
        free_VLE_local_context(vlelctx);

        /* signal that we are done */
        SRF_RETURN_DONE(funcctx);
    }
}

/*
 * Exposed helper function to make an agtype AGTV_PATH from a
 * VLE_path_container.
 */
agtype *agt_materialize_vle_path(agtype *agt_arg_vpc)
{
    /* convert the agtype_value to agtype and return it */
    return agtype_value_to_agtype(agtv_materialize_vle_path(agt_arg_vpc));
}

/*
 * Exposed helper function to make an agtype_value AGTV_PATH from a
 * VLE_path_container.
 */
agtype_value *agtv_materialize_vle_path(agtype *agt_arg_vpc)
{
    VLE_path_container *vpc = NULL;
    agtype_value *agtv_path = NULL;

    /* the passed argument should not be NULL */
    Assert(agt_arg_vpc != NULL);

    /*
     * The path must be a binary container and the type of the object in the
     * container must be an AGT_FBINARY_TYPE_VLE_PATH.
     */
    Assert(AGT_ROOT_IS_BINARY(agt_arg_vpc));
    Assert(AGT_ROOT_BINARY_FLAGS(agt_arg_vpc) == AGT_FBINARY_TYPE_VLE_PATH);

    /* get the container */
    vpc = (VLE_path_container *)agt_arg_vpc;

    /* it should not be null */
    Assert(vpc != NULL);

    /* build the AGTV_PATH from the VLE_path_container */
    agtv_path = build_path(vpc);

    return agtv_path;
}

/* PG function to match 2 VLE edges */
PG_FUNCTION_INFO_V1(age_match_two_vle_edges);

Datum age_match_two_vle_edges(PG_FUNCTION_ARGS)
{
    agtype *agt_arg_vpc = NULL;
    VLE_path_container *left_path = NULL, *right_path = NULL;
    graphid *left_array, *right_array;
    int left_array_size;

    /* get the VLE_path_container argument */
    agt_arg_vpc = AG_GET_ARG_AGTYPE_P(0);

    if (!AGT_ROOT_IS_BINARY(agt_arg_vpc) ||
        AGT_ROOT_BINARY_FLAGS(agt_arg_vpc) != AGT_FBINARY_TYPE_VLE_PATH)
    {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("argument 1 of age_match_two_vle_edges must be a VLE_Path_Container")));
    }

    /* cast argument as a VLE_Path_Container and extract graphid array */
    left_path = (VLE_path_container *)agt_arg_vpc;
    left_array_size = left_path->graphid_array_size;
    left_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(left_path);

    agt_arg_vpc = AG_GET_ARG_AGTYPE_P(1);

    if (!AGT_ROOT_IS_BINARY(agt_arg_vpc) ||
        AGT_ROOT_BINARY_FLAGS(agt_arg_vpc) != AGT_FBINARY_TYPE_VLE_PATH)
    {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("argument 2 of age_match_two_vle_edges must be a VLE_Path_Container")));
    }

    /* cast argument as a VLE_Path_Container and extract graphid array */
    right_path = (VLE_path_container *)agt_arg_vpc;
    right_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(right_path);

    if (left_array[left_array_size - 1] != right_array[0])
    {
        PG_RETURN_BOOL(false);
    }

    PG_RETURN_BOOL(true);
}

/*
 * This function is used when we need to know if the passed in id is at the end
 * of a path. The first arg is the path the second is the vertex id to check and
 * the last is a boolean that syas whether to check the start or the end of the
 * vle path.
 */
PG_FUNCTION_INFO_V1(age_match_vle_edge_to_id_qual);

Datum age_match_vle_edge_to_id_qual(PG_FUNCTION_ARGS)
{
    agtype *agt_arg_vpc = NULL;
    agtype *edge_id = NULL;
    agtype *pos_agt = NULL;
    agtype_value *id, *position;
    VLE_path_container *vle_path = NULL;
    graphid *array;
    bool vle_is_on_left;

    /* get the VLE_path_container argument */
    agt_arg_vpc = AG_GET_ARG_AGTYPE_P(0);

    if (!AGT_ROOT_IS_BINARY(agt_arg_vpc) ||
        AGT_ROOT_BINARY_FLAGS(agt_arg_vpc) != AGT_FBINARY_TYPE_VLE_PATH)
    {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("argument 1 of age_match_vle_edge_to_edge_qual must be a VLE_Path_Container")));
    }

    /* cast argument as a VLE_Path_Container and extract graphid array */
    vle_path = (VLE_path_container *)agt_arg_vpc;
    array = GET_GRAPHID_ARRAY_FROM_CONTAINER(vle_path);

    /* Get the edge id we are checking the end of the list too */
    edge_id = AG_GET_ARG_AGTYPE_P(1);

    if (!AGT_ROOT_IS_SCALAR(edge_id))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("argument 2 of age_match_vle_edge_to_edge_qual must be an integer")));
    }

    id = get_ith_agtype_value_from_container(&edge_id->root, 0);

    if (id->type != AGTV_INTEGER)
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("argument 2 of age_match_vle_edge_to_edge_qual must be an integer")));
    }

    pos_agt = AG_GET_ARG_AGTYPE_P(2);

    if (!AGT_ROOT_IS_SCALAR(pos_agt))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("argument 3 of age_match_vle_edge_to_edge_qual must be an integer")));
    }

    position = get_ith_agtype_value_from_container(&pos_agt->root, 0);

    if (position->type != AGTV_BOOL)
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("argument 3 of age_match_vle_edge_to_edge_qual must be an integer")));
    }

    vle_is_on_left = position->val.boolean;

    if (vle_is_on_left)
    {
        int array_size = vle_path->graphid_array_size;

        /*
         * Path is like ...[vle_edge]-()-[regular_edge]... Get the graphid of
         * the vertex at the endof the path and check that it matches the id
         * that was passed in the second arg. The transform logic is responsible
         * for making that the start or end id, depending on its direction.
         */
        if (id->val.int_value != array[array_size - 1])
        {
            PG_RETURN_BOOL(false);
        }

        PG_RETURN_BOOL(true);
    }
    else
    {
        /*
         * Path is like ...[edge]-()-[vle_edge]... Get the vertex at the start
         * of the vle edge and check against id.
         */
       if (id->val.int_value != array[0])
        {
            PG_RETURN_BOOL(false);
        }

        PG_RETURN_BOOL(true);
    }
}

/*
 * Exposed helper function to make an agtype_value AGTV_ARRAY of edges from a
 * VLE_path_container.
 */
agtype_value *agtv_materialize_vle_edges(agtype *agt_arg_vpc)
{
    VLE_path_container *vpc = NULL;
    agtype_value *agtv_array = NULL;

    /* the passed argument should not be NULL */
    Assert(agt_arg_vpc != NULL);

    /*
     * The path must be a binary container and the type of the object in the
     * container must be an AGT_FBINARY_TYPE_VLE_PATH.
     */
    Assert(AGT_ROOT_IS_BINARY(agt_arg_vpc));
    Assert(AGT_ROOT_BINARY_FLAGS(agt_arg_vpc) == AGT_FBINARY_TYPE_VLE_PATH);

    /* get the container */
    vpc = (VLE_path_container *)agt_arg_vpc;

    /* it should not be null */
    Assert(vpc != NULL);

    /* build the AGTV_ARRAY of edges from the VLE_path_container */
    agtv_array = build_edge_list(vpc);

    /* convert the agtype_value to agtype and return it */
    return agtv_array;

}

/* PG wrapper function for agtv_materialize_vle_edges */
PG_FUNCTION_INFO_V1(age_materialize_vle_edges);

Datum age_materialize_vle_edges(PG_FUNCTION_ARGS)
{
    agtype *agt_arg_vpc = NULL;
    agtype_value *agtv_array = NULL;

    /* if we have a NULL VLE_path_container, return NULL */
    if (PG_ARGISNULL(0))
    {
        PG_RETURN_NULL();
    }

    /* get the VLE_path_container argument */
    agt_arg_vpc = AG_GET_ARG_AGTYPE_P(0);

    /* if NULL, return NULL */
    if (is_agtype_null(agt_arg_vpc))
    {
        PG_RETURN_NULL();
    }

    agtv_array = agtv_materialize_vle_edges(agt_arg_vpc);

    PG_RETURN_POINTER(agtype_value_to_agtype(agtv_array));
}

/* PG wrapper function for agt_materialize_vle_path */
PG_FUNCTION_INFO_V1(age_materialize_vle_path);

Datum age_materialize_vle_path(PG_FUNCTION_ARGS)
{
    agtype *agt_arg_vpc = NULL;

    /* if we have a NULL VLE_path_container, return NULL */
    if (PG_ARGISNULL(0))
    {
        PG_RETURN_NULL();
    }

    /* get the VLE_path_container argument */
    agt_arg_vpc = AG_GET_ARG_AGTYPE_P(0);

    /* if NULL, return NULL */
    if (is_agtype_null(agt_arg_vpc))
    {
        PG_RETURN_NULL();
    }

    PG_RETURN_POINTER(agt_materialize_vle_path(agt_arg_vpc));
}

/*
 * PG function to take a VLE_path_container and return whether the supplied end
 * vertex (target/veid) matches against the last edge in the VLE path. The VLE
 * path is encoded in a BINARY container.
 */
PG_FUNCTION_INFO_V1(age_match_vle_terminal_edge);

Datum age_match_vle_terminal_edge(PG_FUNCTION_ARGS)
{
    VLE_path_container *vpc = NULL;
    agtype *agt_arg_vsid = NULL;
    agtype *agt_arg_veid = NULL;
    agtype *agt_arg_path = NULL;
    agtype_value *agtv_temp = NULL;
    graphid vsid = 0;
    graphid veid = 0;
    bool has_vsid = false;
    bool has_veid = false;
    graphid *gida = NULL;
    int gidasize = 0;

    /* the VLE_path_container argument cannot be NULL */
    if (PG_ARGISNULL(2))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("match_vle_terminal_edge() argument 3 must be non NULL")));
    }

    /* one or both vsid and veid needs to be non NULL */
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("match_vle_terminal_edge() argument 1 or 2 must be non NULL")));

    }

    /* get the vpc */
    agt_arg_path = AG_GET_ARG_AGTYPE_P(2);

    /* it cannot be NULL */
    if (is_agtype_null(agt_arg_path))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("match_vle_terminal_edge() argument 3 must be non NULL")));
    }

    /*
     * The vpc (path) must be a binary container and the type of the object in
     * the container must be an AGT_FBINARY_TYPE_VLE_PATH.
     */
    Assert(AGT_ROOT_IS_BINARY(agt_arg_path));
    Assert(AGT_ROOT_BINARY_FLAGS(agt_arg_path) == AGT_FBINARY_TYPE_VLE_PATH );

    /* get the container */
    vpc = (VLE_path_container *)agt_arg_path;

    /* get the graphid array from the container */
    gida = GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc);

    /* get the gida array size */
    gidasize = vpc->graphid_array_size;

    /* verify the minimum size is 3 or 1 */
    Assert(gidasize >= 3 || gidasize == 1);

    /* get the vsid */
    if (!PG_ARGISNULL(0))
    {
        agt_arg_vsid = AG_GET_ARG_AGTYPE_P(0);

        if (!is_agtype_null(agt_arg_vsid))
        {
            agtv_temp = get_ith_agtype_value_from_container(&agt_arg_vsid->root,
                                                            0);
            Assert(agtv_temp->type == AGTV_INTEGER);
            vsid = agtv_temp->val.int_value;
            has_vsid = true;
        }
    }

    /* get the veid */
    if (!PG_ARGISNULL(1))
    {
        agt_arg_veid = AG_GET_ARG_AGTYPE_P(1);
        if (!is_agtype_null(agt_arg_veid))
        {
            agtv_temp = get_ith_agtype_value_from_container(&agt_arg_veid->root,
                                                            0);
            Assert(agtv_temp->type == AGTV_INTEGER);
            veid = agtv_temp->val.int_value;
            has_veid = true;
        }
    }

    if (!(has_vsid || has_veid))
    {
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("match_vle_terminal_edge() argument 1 or 2 must be non NULL")));
    }

    /* compare the path beginning or end points */
    PG_RETURN_BOOL((has_vsid ? gida[0] == vsid : true) &&
                   (has_veid ? veid == gida[gidasize - 1] : true));
}

/* PG helper function to build an agtype (Datum) edge for matching */
PG_FUNCTION_INFO_V1(age_build_vle_match_edge);

Datum age_build_vle_match_edge(PG_FUNCTION_ARGS)
{
    agtype_in_state result;
    agtype_value agtv_zero;
    agtype_value agtv_nstr;
    agtype_value *agtv_temp = NULL;

    /* create an agtype_value integer 0 */
    agtv_zero.type = AGTV_INTEGER;
    agtv_zero.val.int_value = 0;

    /* create an agtype_value null string */
    agtv_nstr.type = AGTV_STRING;
    agtv_nstr.val.string.len = 0;
    agtv_nstr.val.string.val = NULL;

    /* zero the state */
    memset(&result, 0, sizeof(agtype_in_state));

    /* start the object */
    result.res = push_agtype_value(&result.parse_state, WAGT_BEGIN_OBJECT,
                                   NULL);
    /* create dummy graph id */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("id"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, &agtv_zero);
    /* process the label */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("label"));
    if (!PG_ARGISNULL(0))
    {
        agtv_temp = get_agtype_value("build_vle_match_edge",
                                     AG_GET_ARG_AGTYPE_P(0), AGTV_STRING, true);
        result.res = push_agtype_value(&result.parse_state, WAGT_VALUE,
                                       agtv_temp);
    }
    else
    {
        result.res = push_agtype_value(&result.parse_state, WAGT_VALUE,
                                       &agtv_nstr);
    }
    /* create dummy end_id */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("end_id"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, &agtv_zero);
    /* create dummy start_id */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("start_id"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE, &agtv_zero);

    /* process the properties */
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("properties"));
    if (!PG_ARGISNULL(1))
    {
        agtype *properties = NULL;

        properties = AG_GET_ARG_AGTYPE_P(1);

        if (!AGT_ROOT_IS_OBJECT(properties))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("build_vle_match_edge(): properties argument must be an object")));
        }

        add_agtype((Datum)properties, false, &result, AGTYPEOID, false);

    }
    else
    {
        result.res = push_agtype_value(&result.parse_state, WAGT_BEGIN_OBJECT,
                                       NULL);
        result.res = push_agtype_value(&result.parse_state, WAGT_END_OBJECT,
                                       NULL);
    }

    result.res = push_agtype_value(&result.parse_state, WAGT_END_OBJECT, NULL);

    result.res->type = AGTV_EDGE;

    PG_RETURN_POINTER(agtype_value_to_agtype(result.res));
}

/*
 * This function checks the edges in a MATCH clause to see if they are unique or
 * not. Filters out all the paths where the edge uniques rules are not met.
 * Arguements can be a combination of agtype ints and VLE_path_containers.
 */
PG_FUNCTION_INFO_V1(_ag_enforce_edge_uniqueness);

Datum _ag_enforce_edge_uniqueness(PG_FUNCTION_ARGS)
{
    HTAB *exists_hash = NULL;
    HASHCTL exists_ctl;
    Datum *args = NULL;
    bool *nulls = NULL;
    Oid *types = NULL;
    int nargs = 0;
    int i = 0;

    /* extract our arguments */
    nargs = extract_variadic_args(fcinfo, 0, true, &args, &types, &nulls);

    /* verify the arguments */
    for (i = 0; i < nargs; i++)
    {
        if (nulls[i])
        {
             ereport(ERROR,
                     (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                      errmsg("_ag_enforce_edge_uniqueness argument %d must not be NULL",
                             i)));
        }
        if (types[i] != AGTYPEOID &&
            types[i] != INT8OID &&
            types[i] != GRAPHIDOID)
        {
             ereport(ERROR,
                     (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                      errmsg("_ag_enforce_edge_uniqueness argument %d must be AGTYPE, INT8, or GRAPHIDOID",
                             i)));
        }
    }

    /* configure the hash table */
    MemSet(&exists_ctl, 0, sizeof(exists_ctl));
    exists_ctl.keysize = sizeof(int64);
    exists_ctl.entrysize = sizeof(int64);
    exists_ctl.hash = tag_hash;

    /* create exists_hash table */
    exists_hash = hash_create(EXISTS_HTAB_NAME, EXISTS_HTAB_NAME_INITIAL_SIZE,
                              &exists_ctl, HASH_ELEM | HASH_FUNCTION);

    /* insert arguments into hash table */
    for (i = 0; i < nargs; i++)
    {
        agtype *agt_i = NULL;

        /* get the argument */
        agt_i = DATUM_GET_AGTYPE_P(args[i]);

        /* if the argument is an AGTYPE VLE_path_container */
        if (types[i] == AGTYPEOID &&
            AGT_ROOT_IS_BINARY(agt_i) &&
            AGT_ROOT_BINARY_FLAGS(agt_i) == AGT_FBINARY_TYPE_VLE_PATH)
        {
            VLE_path_container *vpc = NULL;
            graphid *graphid_array = NULL;
            int64 graphid_array_size = 0;
            int64 j = 0;

            /* cast to VLE_path_container */
            vpc = (VLE_path_container *)agt_i;

            /* get the graphid array */
            graphid_array = GET_GRAPHID_ARRAY_FROM_CONTAINER(vpc);

            /* get the graphid array size */
            graphid_array_size = vpc->graphid_array_size;

            /* insert all the edges in the vpc, into the hash table */
            for (j = 1; j < graphid_array_size - 1; j+=2)
            {
                int64 *value = NULL;
                bool found = false;
                graphid edge_id = 0;

                /* get the edge id */
                edge_id = graphid_array[j];

                /* insert the edge id */
                value = (int64 *)hash_search(exists_hash, (void *)&edge_id,
                                             HASH_ENTER, &found);

                /* if we found it, we're done, we have a duplicate */
                if (found)
                {
                    hash_destroy(exists_hash);
                    PG_RETURN_BOOL(false);
                }
                /* otherwise, add it to the returned bucket */
                else
                {
                    *value = edge_id;
                }
            }
        }
        /* if it is a regular AGTYPE scalar */
        else if (types[i] == AGTYPEOID &&
                 AGT_ROOT_IS_SCALAR(agt_i))
        {
            agtype_value *agtv_id = NULL;
            int64 *value = NULL;
            bool found = false;
            graphid edge_id = 0;

            agtv_id = get_ith_agtype_value_from_container(&agt_i->root, 0);

            if (agtv_id->type != AGTV_INTEGER)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("_ag_enforce_edge_uniqueness parameter %d must resolve to an agtype integer",
                                i)));
            }

            edge_id = agtv_id->val.int_value;

            /* insert the edge_id */
            value = (int64 *)hash_search(exists_hash, (void *)&edge_id,
                                         HASH_ENTER, &found);

            /* if we found it, we're done, we have a duplicate */
            if (found)
            {
                hash_destroy(exists_hash);
                PG_RETURN_BOOL(false);
            }
            /* otherwise, add it to the returned bucket */
            else
            {
                *value = edge_id;
            }
        }
        /* if it is an INT8OID or a GRAPHIDOID */
        else if (types[i] == INT8OID ||
                 types[i] == GRAPHIDOID)
        {
            graphid edge_id = 0;
            bool found = false;
            int64 *value = NULL;

            edge_id = DatumGetInt64(args[i]);

            /* insert the edge_id */
            value = (int64 *)hash_search(exists_hash, (void *)&edge_id,
                                         HASH_ENTER, &found);

            /* if we found it, we're done, we have a duplicate */
            if (found)
            {
                hash_destroy(exists_hash);
                PG_RETURN_BOOL(false);
            }
            /* otherwise, add it to the returned bucket */
            else
            {
                *value = edge_id;
            }
        }
        /* it is neither a VLE_path_container, AGTYPE, INT8, or a GRAPHIDOID */
        else
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("_ag_enforce_edge_uniqueness invalid parameter type %d",
                            i)));
        }
    }

    /* if all entries were successfully inserted, we have no duplicates */
    hash_destroy(exists_hash);
    PG_RETURN_BOOL(true);
}
