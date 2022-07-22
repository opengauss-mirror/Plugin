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

#define WORDNUM(x) ((x) / BITS_PER_BITMAPWORD)
#define BITNUM(x) ((x) % BITS_PER_BITMAPWORD)

/* Ensure that data is sufficient. */
#define BIT_SET_MAX_NUM WORDNUM(T_CentroidPoint) + 10

uint32 ddlSet[BIT_SET_MAX_NUM] = {0};
uint32 dmlSet[BIT_SET_MAX_NUM] = {0};
uint32 dclSet[BIT_SET_MAX_NUM] = {0};
uint32 databaseSet[BIT_SET_MAX_NUM] = {0};

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);

void DDLTagInit();

void DMLTagInit();

void DCLTagInit();

void DatabaseTagInit();

bool BitMapContains(int index, uint32* set)
{
    int wordNum = WORDNUM(index);
    int bitNum = BITNUM(index);
    return (set[wordNum] & ((uint32) 1 << bitNum)) != 0;
}

void BitMapSetIndex(uint32* set, int index)
{
    int wordNum = WORDNUM(index);
    int bitNum = BITNUM(index);
    set[wordNum] |= (((uint32) 1 << bitNum));
}

Datum AssessmentAstTree(PG_FUNCTION_ARGS)
{
    text* sqlText = PG_GETARG_TEXT_PP(0);
    char* sql = text_to_cstring(sqlText);

    List * (*parser_hook)(const char*, List**) = raw_parser;
    if (u_sess->attr.attr_sql.dolphin) {
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
    if (BitMapContains(nodeTag, ddlSet)) {
        return Int32GetDatum(DDL);
    } else if (BitMapContains(nodeTag, dmlSet)) {
        return Int32GetDatum(DML);
    } else if (BitMapContains(nodeTag, dclSet)) {
        return Int32GetDatum(DCL);
    } else if (nodeTag == T_ExplainStmt) {
        return Int32GetDatum(EXPLAIN);
    } else if (nodeTag == T_TransactionStmt) {
        return Int32GetDatum(TRANSACTION);
    } else if (nodeTag == T_VariableSetStmt) {
        return Int32GetDatum(SET_VARIABLE);
    }  else if (BitMapContains(nodeTag, databaseSet)) {
        return Int32GetDatum(DATABASE_COMMAND);
    }else {
        return Int32GetDatum(UNSUPPORTED);
    }
}

void _PG_fini(void)
{
    /* pass */
}

void _PG_init(void)
{
     DDLTagInit();
     DMLTagInit();
     DCLTagInit();
     DatabaseTagInit();
}
void DatabaseTagInit()
{
    /* dcl nodeTag */
    BitMapSetIndex(databaseSet, T_CreatedbStmt);
    BitMapSetIndex(databaseSet, T_DropdbStmt);
    BitMapSetIndex(databaseSet, T_CheckPointStmt);
    BitMapSetIndex(databaseSet, T_CreateForeignServerStmt);
    BitMapSetIndex(databaseSet, T_AlterForeignServerStmt);
    BitMapSetIndex(databaseSet, T_CreateForeignTableStmt);
}

void DCLTagInit()
{
    /* dcl nodeTag */
    BitMapSetIndex(dclSet, T_GrantStmt);
    BitMapSetIndex(dclSet, T_CopyStmt);
    BitMapSetIndex(dclSet, T_RenameStmt);
}

void DMLTagInit()
{
    /* dml nodeTag */
    BitMapSetIndex(dmlSet, T_InsertStmt);
    BitMapSetIndex(dmlSet, T_DeleteStmt);
    BitMapSetIndex(dmlSet, T_UpdateStmt);
    BitMapSetIndex(dmlSet, T_MergeStmt);
    BitMapSetIndex(dmlSet, T_SelectStmt);
}

void DDLTagInit()
{
    /* ddl nodeTag */
    BitMapSetIndex(ddlSet, T_CreateStmt);
    BitMapSetIndex(ddlSet, T_CreateSchemaStmt);
    BitMapSetIndex(ddlSet, T_TruncateStmt);
    BitMapSetIndex(ddlSet, T_CreateFunctionStmt);
    BitMapSetIndex(ddlSet, T_AlterFunctionStmt);
    BitMapSetIndex(ddlSet, T_DropStmt);
    BitMapSetIndex(ddlSet, T_CommentStmt);
    BitMapSetIndex(ddlSet, T_CreateCastStmt);
    BitMapSetIndex(ddlSet, T_CreateConversionStmt);
    BitMapSetIndex(ddlSet, T_CreateSeqStmt);
    BitMapSetIndex(ddlSet, T_CompositeTypeStmt);
    BitMapSetIndex(ddlSet, T_DefineStmt);
    BitMapSetIndex(ddlSet, T_AlterTSConfigurationStmt);
    BitMapSetIndex(ddlSet, T_AlterTableStmt);
    BitMapSetIndex(ddlSet, T_VacuumStmt);
    BitMapSetIndex(ddlSet, T_CreateDomainStmt);
    BitMapSetIndex(ddlSet, T_ViewStmt);
    BitMapSetIndex(ddlSet, T_DoStmt);
    BitMapSetIndex(ddlSet, T_IndexStmt);
}