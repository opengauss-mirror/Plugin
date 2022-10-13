/* -------------------------------------------------------------------------
 *
 * parser.h
 *		Definitions for the "raw" parser (flex and bison phases only)
 *
 * This is the external API for the raw lexing/parsing functions.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parser.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PARSER_H
#define PARSER_H

#ifndef FRONTEND_PARSER
#include "nodes/parsenodes.h"
#else
#include "plugin_nodes/parsenodes_common.h"
#endif /* FRONTEND_PARSER */

#include "plugin_parser/backslash_quotes.h"

#define UPSERT_TO_MERGE_VERSION_NUM 92022
#define GS_SHOW_CREATE_TABLE 0
#define GS_SHOW_CREATE_VIEW 1
#define GS_SHOW_CREATE_FUNCTION 0
#define GS_SHOW_CREATE_PROCEDURE 1

/* Primary entry point for the raw parsing functions */
extern List* raw_parser(const char* str, List** query_string_locationlist = NULL);

#ifdef FRONTEND_PARSER

class PGClientLogic;

extern List *fe_raw_parser(PGClientLogic*, const char *str, List **query_string_locationlist = NULL);
#endif /* FRONTEND_PARSER */

/* Utility functions exported by gram.y (perhaps these should be elsewhere) */
extern List* SystemFuncName(char* name);
extern TypeName* SystemTypeName(char* name);
extern Node* makeBoolAConst(bool state, int location);
extern char** get_next_snippet(
    char** query_string_single, const char* query_string, List* query_string_locationlist, int* stmt_num);

extern void fixResTargetNameWithAlias(List* clause_list, const char* aliasname);

SelectStmt* makeDescribeQuery(char* schemaName, char* tableName);
SelectStmt* makeShowProcesslistQuery(bool isFull);
SelectStmt* makeShowColumnsDirectQuery(char* schemaname, char* tablename,
                                       bool fullmode, bool smallcase, Node* likeWhereOpt = NULL);
SelectStmt* makeShowColumnsQuery(char* schemaName, char* tableName, char* dbname,
                                 bool fullMode, bool isLikeExpr, Node* likeWhereOpt = NULL);
SelectStmt *makeShowCreateDatabaseQuery(bool ifexists, char *databaseName);
SelectStmt* makeChecksumsTablesQuery(List *tablelist, bool isExtendMode, bool doFastChksum);
SelectStmt* makeShowTablesQuery(bool fullmode, char* optDbName, Node* likeWhereOpt, bool isLikeExpr);
SelectStmt* makeShowPluginsQuery(void);
SelectStmt* makeShowPrivilegesQuery(void);
SelectStmt* makeShowDatabasesQuery(Node* likeNode, Node* whereExpr);
SelectStmt* makeShowMasterStatusQuery(void);
SelectStmt* makeShowSlaveHostsQuery(void);
#ifdef DOLPHIN
SelectStmt *MakeShowTriggersQuery(List *args, Node *likeWhereOpt, bool isLikeExpr);
SelectStmt *MakeShowFuncProQuery(List *args, Node *likeWhereOpt, bool isLikeExpr);
SelectStmt *MakeShowCharacterQuery(List *args, Node *likeWhereOpt, bool isLikeExpr);
SelectStmt *MakeShowCollationQuery(List *args, Node *likeWhereOpt, bool isLikeExpr);
#endif

SelectStmt* makeShowIndexQuery(char *schemaName, char *tableName, Node *whereClause);
SelectStmt *makeShowVariablesQuery(bool globalMode, Node *likeWhereOpt, bool isLikeExpr);
SelectStmt *findCreateClass(RangeVar *classrel, int mode);
SelectStmt *findCreateProc(char *schemaName, char *name, int model);
SelectStmt *findCreateTrigger(RangeVar *trel);

SelectStmt* makeShowTableStatusQuery(char* schemaName, Node* likeWhereOpt, bool isLikeExpr);
SelectStmt* makeFlushBinaryLogsQuery(void);

#endif /* PARSER_H */
