/*-------------------------------------------------------------------------
 *
 * parse_expr.h
 *	  handle expressions in parser
 *
 * Portions Copyright (c) 1996-2018, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_expr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef AG_PARSE_EXPR_H
#define AG_PARSE_EXPR_H

#include "parser/parse_node.h"
#include "utils/plpgsql.h"

/* GUC parameters */
extern Node* ag_transformExpr(ParseState* pstate, Node* expr, ParseExprKind exprKind);

extern char *makeStartWithDummayColname(char *alias, char *column);

#endif							/* PARSE_EXPR_H */
