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

#include "access/xact.h"
#include "catalog/dependency.h"
#include "catalog/objectaccess.h"
#include "catalog/namespace.h"
#include "catalog/pg_class.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_namespace.h"
#include "commands/defrem.h"
#include "commands/extension.h"
#include "commands/trigger.h"
#include "tcop/utility.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"

#include "catalog/ag_catalog.h"
#include "catalog/ag_graph.h"
#include "catalog/ag_label.h"
#include "catalog/ag_namespace.h"
#include "utils/ag_cache.h"
#include "utils/age_global_graph.h"
#include "utils/agtype.h"
#include "utils/graphid.h"

static THR_LOCAL bool extension_cache_is_valid = false;
static THR_LOCAL bool age_extension_exists = false;
static THR_LOCAL object_access_hook_type prev_object_access_hook;
static THR_LOCAL ProcessUtility_hook_type prev_process_utility_hook;
static THR_LOCAL bool prev_object_hook_is_set;
static THR_LOCAL bool age_drop_in_progress;

static void object_access(ObjectAccessType access, Oid class_id, Oid object_id,
                          int sub_id, void *arg);

static void ag_ProcessUtility_hook(processutility_context* processutility_cxt,
                            DestReceiver *dest,
#ifdef PGXC
    bool sentToRemote,
#endif /* PGXC */
                            char *completionTag, ProcessUtilityContext context, bool isCTAS);

static bool is_age_drop(DropStmt *drop_stmt);
static List *collect_truncated_label_relids(TruncateStmt *stmt);
static void restore_failed_age_drop(void);
static void age_drop_xact_callback(XactEvent event, void *argument);
static void age_drop_subxact_callback(SubXactEvent event,
                                      SubTransactionId transaction_id,
                                      SubTransactionId parent_transaction_id,
                                      void *argument);

static void invalidate_extension_cache_callback(Datum argument, Oid relation_id)
{
    if (!OidIsValid(relation_id) || relation_id == ExtensionRelationId)
        extension_cache_is_valid = false;
}

bool is_age_extension_exists(void)
{
    static THR_LOCAL bool callback_registered = false;

    if (extension_cache_is_valid)
        return age_extension_exists;

    if (!callback_registered) {
        CacheRegisterSessionRelcacheCallback(
            invalidate_extension_cache_callback, (Datum)0);
        callback_registered = true;
    }

    age_extension_exists = OidIsValid(get_extension_oid("age", true));
    extension_cache_is_valid = true;

    return age_extension_exists;
}

void object_access_hook_init(void)
{
    prev_object_access_hook = object_access_hook;
    object_access_hook = object_access;
    prev_object_hook_is_set = true;
}

void object_access_hook_fini(void)
{
    if (prev_object_hook_is_set)
    {
        object_access_hook = prev_object_access_hook;
        prev_object_access_hook = NULL;
        prev_object_hook_is_set = false;
    }

}

void process_utility_hook_init(void)
{
    prev_process_utility_hook = ProcessUtility_hook;
    ProcessUtility_hook = ag_ProcessUtility_hook;
    RegisterXactCallback(age_drop_xact_callback, NULL);
    RegisterSubXactCallback(age_drop_subxact_callback, NULL);
}

void process_utility_hook_fini(void)
{
    UnregisterSubXactCallback(age_drop_subxact_callback, NULL);
    UnregisterXactCallback(age_drop_xact_callback, NULL);
    ProcessUtility_hook = prev_process_utility_hook;
}

static void restore_failed_age_drop(void)
{
    if (!age_drop_in_progress) {
        return;
    }

    if (!prev_object_hook_is_set) {
        object_access_hook_init();
    }

    extension_cache_is_valid = false;
    age_drop_in_progress = false;
}

static void age_drop_xact_callback(XactEvent event, void *argument)
{
    (void)argument;

    if (event == XACT_EVENT_ABORT) {
        restore_failed_age_drop();
    }
}

static void age_drop_subxact_callback(SubXactEvent event,
                                      SubTransactionId transaction_id,
                                      SubTransactionId parent_transaction_id,
                                      void *argument)
{
    (void)transaction_id;
    (void)parent_transaction_id;
    (void)argument;

    if (event == SUBXACT_EVENT_ABORT_SUB) {
        restore_failed_age_drop();
    }
}

static List *collect_truncated_label_relids(TruncateStmt *stmt)
{
    List *relids = NIL;
    ListCell *lc;

    foreach (lc, stmt->relations)
    {
        RangeVar *rv = (RangeVar *)lfirst(lc);
        Oid relid = RangeVarGetRelid(rv, AccessShareLock, true);
        if (OidIsValid(relid) && search_label_relation_cache(relid) != NULL &&
            !OidIsValid(get_trigger_oid(relid, "_age_cache_invalidate", true)) &&
            !list_member_oid(relids, relid)) {
            relids = lappend_oid(relids, relid);
        }
    }

    return relids;
}

/*
 * When Postgres tries to drop AGE using the standard logic, two issues occur:
 *
 * 1. The schema that graphs in stored in are not dropped.
 *
 * 2. While dropping ag_catalog, the object hook is run. Which uses the
 * information in the indexes and tables being dropped. To prevent an error
 * from being thrown, we need to disable the object_access_hook before dropping
 * the extension.
 */
static void ag_ProcessUtility_hook(processutility_context* processutility_cxt, DestReceiver *dest,
#ifdef PGXC
    bool sentToRemote,
#endif /* PGXC */
    char *completionTag, ProcessUtilityContext context, bool isCTAS = false)
{
    Node *parsetree = processutility_cxt->parse_tree;
    bool creating_age = false;
    bool dropping_age = false;
    List *truncated_label_relids = NIL;

    if (!IsAbortedTransactionBlockState())
    {
        if (IsA(parsetree, TruncateStmt) && is_age_extension_exists()) {
            truncated_label_relids = collect_truncated_label_relids(
                (TruncateStmt *)parsetree);
        } else if (IsA(parsetree, CreateExtensionStmt)) {
            CreateExtensionStmt *stmt = (CreateExtensionStmt *)parsetree;
            creating_age = strcmp(stmt->extname, "age") == 0;
        } else if (IsA(parsetree, DropStmt)) {
            DropStmt *stmt = (DropStmt *)parsetree;

            if (stmt->removeType == OBJECT_EXTENSION)
                dropping_age = is_age_drop(stmt);
        }
    }

    if (dropping_age)
    {
        age_drop_in_progress = true;
        drop_graphs(get_graphnames());
        object_access_hook_fini();
    }

    PG_TRY();
    {
        if (prev_process_utility_hook) {
            prev_process_utility_hook(processutility_cxt, dest,
#ifdef PGXC
                                      sentToRemote,
#endif /* PGXC */
                                      completionTag, context, isCTAS);
        } else {
            standard_ProcessUtility(processutility_cxt, dest,
#ifdef PGXC
                                    sentToRemote,
#endif /* PGXC */
                                    completionTag, context, isCTAS);
        }
    }
    PG_CATCH();
    {
        if (age_drop_in_progress && !prev_object_hook_is_set) {
            object_access_hook_init();
        }

        PG_RE_THROW();
    }
    PG_END_TRY();

    if (truncated_label_relids != NIL)
    {
        ListCell *lc;

        foreach (lc, truncated_label_relids)
            notify_GRAPH_global_contexts_relation_modified(lfirst_oid(lc));
        list_free(truncated_label_relids);
    }

    if (dropping_age)
    {
        clear_global_Oids_VERTEX_EDGE();
        clear_global_Oids_AGTYPE();
        clear_global_Oids_GRAPHID();
        object_access_hook_init();
        age_drop_in_progress = false;
    }

    if (creating_age || dropping_age)
    {
        /* The local callback may run only when invalidations are consumed. */
        extension_cache_is_valid = false;
        CacheInvalidateRelcacheByRelid(ExtensionRelationId);
    }
}

// Check to see if the Utility Command is to drop the AGE Extension.
static bool is_age_drop(DropStmt *drop_stmt)
{
    ListCell *lc;

    if (!is_age_extension_exists()) {
        return false;
    }

    foreach(lc, drop_stmt->objects)
    {
        List* objname = (List*)lfirst(lc);
        const char* str = strVal(linitial(objname));

        if (strcmp(str, "age") == 0)
            return true;
    }

    return false;
}

/*
 * object_access_hook is called before actual deletion. So, looking up ag_cache
 * is still valid at this point. For labels, once a backed table is deleted,
 * its corresponding ag_label cache entry will be removed by cache
 * invalidation.
 */
static void object_access(ObjectAccessType access, Oid class_id, Oid object_id,
                          int sub_id, void *arg)
{
    ObjectAccessDrop *drop_arg;

    if (prev_object_access_hook)
        prev_object_access_hook(access, class_id, object_id, sub_id, arg);

    if (!is_age_extension_exists()) {
        return;
    }

    // We are interested in DROP SCHEMA and DROP TABLE commands.
    if (access != OAT_DROP)
        return;

    /* LOAD or shared preload can initialize hooks before CREATE EXTENSION. */
    if (!OidIsValid(get_namespace_oid("ag_catalog", true)))
    {
        return;
    }

    drop_arg = (ObjectAccessDrop *)arg;

    /*
     * PERFORM_DELETION_INTERNAL flag will be set when remove_schema() calls
     * performDeletion(). However, if PostgreSQL does performDeletion() with
     * PERFORM_DELETION_INTERNAL flag over backed schemas of graphs due to
     * side effects of other commands run by user, it is impossible to
     * distinguish between this and drop_graph().
     *
     * The above applies to DROP TABLE command too.
     */

    if (class_id == NamespaceRelationId)
    {
        graph_cache_data *cache_data;

        if (drop_arg->dropflags & PERFORM_DELETION_INTERNAL)
            return;

        cache_data = search_graph_namespace_cache(object_id);
        if (cache_data)
        {
            char *nspname = get_namespace_name(object_id);

            ereport(ERROR, (errcode(ERRCODE_DEPENDENT_OBJECTS_STILL_EXIST),
                            errmsg("schema \"%s\" is for graph \"%s\"",
                                   nspname, NameStr(cache_data->name))));
        }

        return;
    }

    if (class_id == RelationRelationId)
    {
        label_cache_data *cache_data;

        cache_data = search_label_relation_cache(object_id);

        // We are interested in only tables that are labels.
        if (!cache_data)
        {
            return;
        }

        if (drop_arg->dropflags & PERFORM_DELETION_INTERNAL)
        {
            /*
             * Remove the corresponding ag_label entry here first. We don't
             * know whether this operation is drop_label() or a part of
             * drop_graph().
             */
            delete_label(object_id);
        }
        else
        {
            char *relname = get_rel_name(object_id);

            ereport(ERROR, (errcode(ERRCODE_DEPENDENT_OBJECTS_STILL_EXIST),
                            errmsg("table \"%s\" is for label \"%s\"",
                                   relname, NameStr(cache_data->name))));
        }
    }
}

Oid ag_relation_id(const char *name, const char *kind)
{
    Oid id;

    id = get_relname_relid(name, ag_catalog_namespace_id());
    if (!OidIsValid(id))
    {
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_TABLE),
                        errmsg("%s \"%s\" does not exist", kind, name)));
    }

    return id;
}
