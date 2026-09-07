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

#include "fmgr.h"

#include "catalog/ag_catalog.h"
#include "nodes/ag_nodes.h"
#include "optimizer/cypher_paths.h"
#include "parser/cypher_analyze.h"
#include "utils/ag_guc.h"
#include "utils/age_global_graph.h"

PG_MODULE_MAGIC;

extern "C" void _PG_init(void);

void _PG_init(void)
{
    if (u_sess->proc_cxt.IsBinaryUpgrade) {
        return;
    }

    /*
     * Pure topology gate first: if the deployment role is unsupported, fail
     * before registering GUCs or installing hooks so a rejected LOAD leaves
     * no session state behind.
     */
#ifdef ENABLE_MULTIPLE_NODES
    if (g_instance.role != VSINGLENODE) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("AGE is not supported in multiple-node mode"),
                 errhint("Run AGE with the VSINGLENODE role.")));
    }
#endif

    define_config_params();

    if (g_instance.attr.attr_common.enable_thread_pool) {
        ereport(ERROR, (errmsg("Currently age is not compatiable with thread pool. "),
                        errhint("please disable thread pool by configuring "
                                "enable_thread_pool = OFF. ")));
    }
    register_ag_nodes();
    register_GRAPH_global_context_relcache_callback();
    set_rel_pathlist_init();
    object_access_hook_init();
    process_utility_hook_init();
    post_parse_analyze_init();
}

extern "C" void _PG_fini(void);

void _PG_fini(void)
{
    post_parse_analyze_fini();
    process_utility_hook_fini();
    object_access_hook_fini();
    set_rel_pathlist_fini();
}
