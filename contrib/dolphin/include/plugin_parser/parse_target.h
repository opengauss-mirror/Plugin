/* -------------------------------------------------------------------------
 *
 * parse_target.h
 *	  handle target lists
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_target.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PARSE_TARGET_H
#define PARSE_TARGET_H

#include "plugin_parser/parse_node.h"

extern List* transformTargetList(ParseState* pstate, List* targetlist, ParseExprKind exprKind
#ifdef DOLPHIN
    , bool isUpdateStmt=false
#endif
);
extern List* transformExpressionList(ParseState* pstate, List* exprlist, ParseExprKind exprKind);
extern void markTargetListOrigins(ParseState* pstate, List* targetlist);
extern void resolveTargetListUnknowns(ParseState* pstate, List* targetlist);
extern TargetEntry* transformTargetEntry(ParseState* pstate, Node* node, Node* expr, ParseExprKind exprKind, char* colname, bool resjunk);
extern Expr* transformAssignedExpr(ParseState* pstate, Expr* expr, ParseExprKind exprKind, char* colname, int attrno,
    List* indirection, int location, Relation rd, RangeTblEntry* rte);
extern void updateTargetListEntry(ParseState* pstate, TargetEntry* tle, char* colname, int attrno,
    List* indirection, int location, Relation rd, RangeTblEntry* rte);
extern List* checkInsertTargets(ParseState* pstate, List* cols, List** attrnos);
extern TupleDesc expandRecordVariable(ParseState* pstate, Var* var, int levelsup);
extern char* FigureColname(Node* node);
extern char* FigureIndexColname(Node* node);
#ifdef DOLPHIN
extern List* AppendNotNullCols(ParseState* pstate, List* cols, List** attrnos);
extern void CheckNullValue(Relation relation, Expr* expr, AttrNumber attrNum);
#define TRUE_LEN 4
#define FALSE_LEN 5
#endif
#endif /* PARSE_TARGET_H */
