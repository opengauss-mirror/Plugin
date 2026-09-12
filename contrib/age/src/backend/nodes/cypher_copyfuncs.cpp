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

#include "nodes/bitmapset.h"
#include "nodes/ag_extensible.h"
#include "nodes/nodes.h"
#include "utils/palloc.h"

#include "nodes/cypher_copyfuncs.h"
#include "nodes/cypher_nodes.h"

/*
 * Copied From Postgres
 *
 * Macros to simplify copying of different kinds of fields.  Use these
 * wherever possible to reduce the chance for silly typos.  Note that these
 * hard-wire the convention that the local variables in a Copy routine are
 * named 'extended_newnode' and 'extended_from'.
 */

// Declare the local fields needed to copy extensible nodes
#define COPY_LOCALS(nodeTypeName) \
    nodeTypeName *extended_newnode = (nodeTypeName *)newnode; \
    nodeTypeName *extended_from = (nodeTypeName *)from; \
    Assert(is_ag_node(newnode, nodeTypeName)); \
    Assert(is_ag_node(from, nodeTypeName));


// Copy a simple scalar field (int, float, bool, enum, etc)
#define COPY_SCALAR_FIELD(fldname) \
        (extended_newnode->fldname = extended_from->fldname)

// Copy a field that is a pointer to some kind of Node or Node tree
#define COPY_NODE_FIELD(fldname) \
        (extended_newnode->fldname = (decltype(fldname))copyObject(extended_from->fldname))

// Copy a field that is a pointer to a Bitmapset
#define COPY_BITMAPSET_FIELD(fldname) \
        (extended_newnode->fldname = bms_copy(extended_from->fldname))

// Copy a field that is a pointer to a C string, or perhaps NULL
#define COPY_STRING_FIELD(fldname) \
        (extended_newnode->fldname = extended_from->fldname ? \
            pstrdup(extended_from->fldname) : (char *) NULL)

/*
 * Default copy function for cypher nodes. For most nodes, we don't expect
 * the node to ever be copied. So throw an error.
 */
void copy_ag_node(ExtensibleNode *newnode,
                         const ExtensibleNode *oldnode)
{
    ereport(ERROR, (errmsg("unexpected copyObject() over ag_node")));
}

// copy function for cypher_create
void copy_cypher_create(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_create);

    extended_newnode->pattern = (List *)copyObject(extended_from->pattern);
}

// copy function for cypher_path
void copy_cypher_path(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_path);

    extended_newnode->path = (List *)copyObject(extended_from->path);
    COPY_STRING_FIELD(var_name);
    COPY_SCALAR_FIELD(location);
}

// copy function for cypher_node
void copy_cypher_node(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_node);

    COPY_STRING_FIELD(name);
    COPY_STRING_FIELD(label);
    COPY_SCALAR_FIELD(use_equals);
    extended_newnode->props = (Node *)copyObject(extended_from->props);
    COPY_SCALAR_FIELD(location);
}

// copy function for cypher_relationship
void copy_cypher_relationship(ExtensibleNode *newnode,
                              const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_relationship);

    COPY_STRING_FIELD(name);
    COPY_STRING_FIELD(label);
    COPY_SCALAR_FIELD(use_equals);
    extended_newnode->props = (Node *)copyObject(extended_from->props);
    extended_newnode->varlen = (Node *)copyObject(extended_from->varlen);
    COPY_SCALAR_FIELD(dir);
    COPY_SCALAR_FIELD(location);
}

// copy function for cypher_map
void copy_cypher_map(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_map);

    extended_newnode->keyvals = (List *)copyObject(extended_from->keyvals);
    COPY_SCALAR_FIELD(location);
    COPY_SCALAR_FIELD(keep_null);
}

// copy function for cypher_map_projection
void copy_cypher_map_projection(ExtensibleNode *newnode,
                                const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_map_projection);

    extended_newnode->map_var = (ColumnRef *)copyObject(extended_from->map_var);
    extended_newnode->map_elements = (List *)copyObject(extended_from->map_elements);
    COPY_SCALAR_FIELD(location);
}

// copy function for cypher_map_projection_element
void copy_cypher_map_projection_element(ExtensibleNode *newnode,
                                        const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_map_projection_element);

    COPY_SCALAR_FIELD(type);
    COPY_STRING_FIELD(key);
    extended_newnode->value = (Node *)copyObject(extended_from->value);
    COPY_SCALAR_FIELD(location);
}

// copy function for cypher_integer_const
void copy_cypher_integer_const(ExtensibleNode *newnode,
                               const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_integer_const);

    COPY_SCALAR_FIELD(integer);
    COPY_SCALAR_FIELD(location);
}

// copy function for cypher_create_target_nodes
void copy_cypher_create_target_nodes(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_create_target_nodes);

    COPY_SCALAR_FIELD(flags);
    COPY_SCALAR_FIELD(graph_oid);

    extended_newnode->paths = (List *)copyObject(extended_from->paths);
}
void copy_cypher_vle_target_nodes(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_vle_target_nodes);

    COPY_SCALAR_FIELD(minimum_output_depth);
    COPY_SCALAR_FIELD(maximum_output_depth);
    COPY_SCALAR_FIELD(cypher_rel_direction);
    COPY_STRING_FIELD(label_name);
    COPY_SCALAR_FIELD(graph_oid);
    extended_newnode->edge_property_constraint =
        (Node *)copyObject(extended_from->edge_property_constraint);
}

// copy function for cypher_create_path
void copy_cypher_create_path(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_create_path);

    COPY_SCALAR_FIELD(path_attr_num);
    COPY_STRING_FIELD(var_name);
    extended_newnode->target_nodes = (List *)copyObject(extended_from->target_nodes);
}

// copy function for cypher_target_node
void copy_cypher_target_node(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_target_node);

    COPY_SCALAR_FIELD(type);
    COPY_SCALAR_FIELD(flags);
    COPY_SCALAR_FIELD(dir);
    COPY_SCALAR_FIELD(prop_attr_num);
    COPY_SCALAR_FIELD(relid);
    COPY_SCALAR_FIELD(tuple_position);

    COPY_STRING_FIELD(label_name);
    COPY_STRING_FIELD(variable_name);

    extended_newnode->id_expr = (Expr *)copyObject(extended_from->id_expr);
    extended_newnode->id_expr_state = (ExprState *)copyObject(extended_from->id_expr_state);
    extended_newnode->prop_expr = (Expr *)copyObject(extended_from->prop_expr);
    extended_newnode->prop_expr_state = (ExprState *)copyObject(extended_from->prop_expr_state);
    extended_newnode->resultRelInfo = (ResultRelInfo *)copyObject(extended_from->resultRelInfo);
    extended_newnode->elemTupleSlot = (TupleTableSlot *)copyObject(extended_from->elemTupleSlot);
}

// copy function for cypher_update_information
void copy_cypher_update_information(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_update_information);

    extended_newnode->set_items = (List *)copyObject(extended_from->set_items);
    COPY_SCALAR_FIELD(flags);
    COPY_SCALAR_FIELD(tuple_position);
    COPY_STRING_FIELD(graph_name);
    COPY_STRING_FIELD(clause_name);
}

// copy function for cypher_update_item
void copy_cypher_update_item(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_update_item);

    COPY_SCALAR_FIELD(prop_position);
    COPY_SCALAR_FIELD(entity_position);
    COPY_STRING_FIELD(var_name);
    COPY_STRING_FIELD(prop_name);
    extended_newnode->qualified_name = (List *)copyObject(extended_from->qualified_name);
    COPY_SCALAR_FIELD(remove_item);
    COPY_SCALAR_FIELD(replace_properties);
    COPY_SCALAR_FIELD(is_add);
    extended_newnode->prop_expr = (Node *)copyObject(extended_from->prop_expr);
    extended_newnode->prop_expr_state =
        (ExprState *)copyObject(extended_from->prop_expr_state);
}

// copy function for cypher_delete_information
void copy_cypher_delete_information(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_delete_information);

    extended_newnode->delete_items = (List *)copyObject(extended_from->delete_items);
    COPY_SCALAR_FIELD(flags);
    COPY_STRING_FIELD(graph_name);
    COPY_SCALAR_FIELD(graph_oid);
    COPY_SCALAR_FIELD(detach);
}

// copy function for cypher_delete_item
void copy_cypher_delete_item(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_delete_item);

    extended_newnode->entity_position = (Value *)copyObject(extended_from->entity_position);
    COPY_STRING_FIELD(var_name);
}

// copy function for cypher_merge_information
void copy_cypher_merge_information(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_merge_information);

    COPY_SCALAR_FIELD(flags);
    COPY_SCALAR_FIELD(graph_oid);
    COPY_SCALAR_FIELD(merge_function_attr);
    extended_newnode->path = (cypher_create_path *)copyObject(extended_from->path);
    extended_newnode->on_match_set_info =
        (cypher_update_information *)copyObject(extended_from->on_match_set_info);
    extended_newnode->on_create_set_info =
        (cypher_update_information *)copyObject(extended_from->on_create_set_info);
}

// copy function for cypher_list_comprehension
void copy_cypher_list_comprehension(ExtensibleNode *newnode,
                                    const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_list_comprehension);

    COPY_STRING_FIELD(varname);
    extended_newnode->expr = (Node *)copyObject(extended_from->expr);
    extended_newnode->where = (Node *)copyObject(extended_from->where);
    extended_newnode->mapping_expr = (Node *)copyObject(extended_from->mapping_expr);
}

// copy function for cypher_reduce
void copy_cypher_reduce(ExtensibleNode *newnode, const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_reduce);

    COPY_STRING_FIELD(accumname);
    extended_newnode->initial = (Node *)copyObject(extended_from->initial);
    COPY_STRING_FIELD(varname);
    extended_newnode->expr = (Node *)copyObject(extended_from->expr);
    extended_newnode->mapping_expr = (Node *)copyObject(extended_from->mapping_expr);
}

// copy function for cypher_predicate_function
void copy_cypher_predicate_function(ExtensibleNode *newnode,
                                    const ExtensibleNode *from)
{
    COPY_LOCALS(cypher_predicate_function);

    COPY_SCALAR_FIELD(kind);
    COPY_STRING_FIELD(varname);
    extended_newnode->expr = (Node *)copyObject(extended_from->expr);
    extended_newnode->where = (Node *)copyObject(extended_from->where);
}
