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

#include <unistd.h>

#include "fmgr.h"
#include "utils/builtins.h"

#include "utils/age_session_info.h"

static THR_LOCAL int session_info_pid = -1;
static THR_LOCAL char *session_info_graph_name = NULL;
static THR_LOCAL char *session_info_cypher_statement = NULL;
static THR_LOCAL bool session_info_prepared = false;

static void set_session_info(char *graph_name, char *cypher_statement)
{
    MemoryContext oldctx = NULL;

    if (is_session_info_prepared()) {
        reset_session_info();
    }

    oldctx = MemoryContextSwitchTo(u_sess->cache_mem_cxt);

    session_info_graph_name = graph_name ? pstrdup(graph_name) : NULL;
    session_info_cypher_statement = cypher_statement ? pstrdup(cypher_statement) : NULL;

    MemoryContextSwitchTo(oldctx);

    session_info_pid = getpid();
    session_info_prepared = true;
}

char *get_session_info_graph_name(void)
{
    if (is_session_info_prepared() && session_info_graph_name != NULL)
        return pstrdup(session_info_graph_name);

    return NULL;
}

char *get_session_info_cypher_statement(void)
{
    if (is_session_info_prepared() && session_info_cypher_statement != NULL)
        return pstrdup(session_info_cypher_statement);

    return NULL;
}

bool is_session_info_prepared(void)
{
    return session_info_prepared && session_info_pid == getpid();
}

void reset_session_info(void)
{
    if (session_info_prepared) {
        if (session_info_graph_name != NULL)
            pfree(session_info_graph_name);

        if (session_info_cypher_statement != NULL)
            pfree(session_info_cypher_statement);
    }

    session_info_graph_name = NULL;
    session_info_cypher_statement = NULL;
    session_info_prepared = false;
    session_info_pid = -1;
}

PG_FUNCTION_INFO_V1(age_prepare_cypher);
extern "C" Datum age_prepare_cypher(PG_FUNCTION_ARGS);
Datum age_prepare_cypher(PG_FUNCTION_ARGS)
{
    char *graph_name_str = NULL;
    char *cypher_statement_str = NULL;

    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
        PG_RETURN_BOOL(false);
    }

    graph_name_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    cypher_statement_str = text_to_cstring(PG_GETARG_TEXT_PP(1));
    if (graph_name_str == NULL || cypher_statement_str == NULL) {
        PG_RETURN_BOOL(false);
    }

    set_session_info(graph_name_str, cypher_statement_str);

    PG_RETURN_BOOL(true);
}
