/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * assessment.cpp
 *    extension
 *
 * IDENTIFICATION
 *    contrib/assessment/assessment.cpp
 *
 * -------------------------------------------------------------------------
 */

#include "assessment.h"
#include "postgres.h"
#include "fmgr/fmgr_comp.h"
#include "utils/builtins.h"
#include "utils/datum.h"
#include "parser/parser.h"

/* for MemoryContextUnSeal */
#include "utils/memutils.h"

/* for g_instance */
#include "knl/knl_instance.h"

/* for GetCustomParserId */
#include "miscadmin.h"


PG_MODULE_MAGIC;
extern "C" Datum AssessmentAstTree(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(AssessmentAstTree);

static Bitmapset* ddlSet = nullptr;
static Bitmapset* dmlSet = nullptr;
static Bitmapset* dclSet = nullptr;

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);

Datum AssessmentAstTree(PG_FUNCTION_ARGS)
{
    text* sqlText = PG_GETARG_TEXT_PP(0);
    char* sql = text_to_cstring(sqlText);

    List * (*parser_hook)(
    const char*, List * *) = raw_parser;
    if (u_sess->attr.attr_sql.b_sql_plugin) {
        int id = GetCustomParserId();
        if (id >= 0 && g_instance.raw_parser_hook[id] != NULL) {
            parser_hook = (List* (*)(const char*, List**)) g_instance.raw_parser_hook[id];
        }
    }

    List* result = parser_hook(sql, nullptr);
    if (result == NIL) {
        /* Empty input string.	This is legal. */
        return Int32GetDatum(COMMENT);
    }
    Assert(list_length(result) == 1);
    Node* node = (Node*) list_nth(result, 0);
    auto nodeTag = node->type;
    if (bms_is_member(nodeTag, ddlSet)) {
        return Int32GetDatum(DDL);
    } else if (bms_is_member(nodeTag, dmlSet)) {
        return Int32GetDatum(DML);
    } else if (bms_is_member(nodeTag, dclSet)) {
        return Int32GetDatum(DCL);
    } else if (nodeTag == T_ExplainStmt) {
        return Int32GetDatum(EXPLAIN);
    } else if (nodeTag == T_TransactionStmt) {
        return Int32GetDatum(TRANSACTION);
    } else if (nodeTag == T_VariableSetStmt) {
        return Int32GetDatum(SET_VARIABLE);
    } else {
        return Int32GetDatum(UNSUPPORTED);
    }
}

void _PG_fini(void)
{
    bms_free(ddlSet);
    bms_free(dmlSet);
    bms_free(dclSet);
}

void _PG_init(void)
{
    /* Ensure that data is sufficient. */

    auto oldContext = MemoryContextSwitchTo(g_instance.instance_context);
    MemoryContextUnSeal(g_instance.instance_context);
    auto size = NodeTag::T_CentroidPoint + 1000;
    if (ddlSet == nullptr) {
        /* ddl nodeTag */
        ddlSet = bms_make_singleton(size);
        ddlSet = bms_add_member(ddlSet, T_CreateStmt);
        ddlSet = bms_add_member(ddlSet, T_CreateSchemaStmt);
        ddlSet = bms_add_member(ddlSet, T_TruncateStmt);
        ddlSet = bms_add_member(ddlSet, T_CreateFunctionStmt);
        ddlSet = bms_add_member(ddlSet, T_AlterFunctionStmt);
        ddlSet = bms_add_member(ddlSet, T_DropStmt);
        ddlSet = bms_add_member(ddlSet, T_CommentStmt);
        ddlSet = bms_add_member(ddlSet, T_CreateCastStmt);
        ddlSet = bms_add_member(ddlSet, T_CreateConversionStmt);
        ddlSet = bms_add_member(ddlSet, T_CreateSeqStmt);
        ddlSet = bms_add_member(ddlSet, T_CompositeTypeStmt);
        ddlSet = bms_add_member(ddlSet, T_DefineStmt);
        ddlSet = bms_add_member(ddlSet, T_AlterTSConfigurationStmt);
        ddlSet = bms_add_member(ddlSet, T_CreateDomainStmt);
        ddlSet = bms_add_member(ddlSet, T_ViewStmt);
    }
    if (dmlSet == nullptr) {
        /* dml nodeTag */
        dmlSet = bms_make_singleton(size);
        dmlSet = bms_add_member(dmlSet, T_InsertStmt);
        dmlSet = bms_add_member(dmlSet, T_DeleteStmt);
        dmlSet = bms_add_member(dmlSet, T_UpdateStmt);
        dmlSet = bms_add_member(dmlSet, T_MergeStmt);
        dmlSet = bms_add_member(dmlSet, T_SelectStmt);
    }
    if (dclSet == nullptr) {
        /* dcl nodeTag */
        dclSet = bms_make_singleton(size);
        dclSet = bms_add_member(dclSet, T_GrantStmt);
    }
    (void) MemoryContextSwitchTo(oldContext);
    MemoryContextSeal(g_instance.instance_context);
}
