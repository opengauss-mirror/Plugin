/* -------------------------------------------------------------------------
 *
 * parse_oper.h
 *		handle operator things for parser
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_oper.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PARSE_OPER_H
#define PARSE_OPER_H

#include "access/htup.h"
#include "plugin_parser/parse_node.h"

typedef HeapTuple Operator;
#define DOLPHIN_CATALOG_STR "dolphin_catalog"

/* Routines to look up an operator given name and exact input type(s) */
extern Oid LookupOperName(ParseState* pstate, List* opername, Oid oprleft, Oid oprright, bool noError, int location);
extern Oid LookupOperNameTypeNames(
    ParseState* pstate, List* opername, TypeName* oprleft, TypeName* oprright, bool noError, int location);

/* Routines to find operators matching a name and given input types */
/* NB: the selected operator may require coercion of the input types! */
extern Operator oper(
    ParseState* pstate, List* op, Oid arg1, Oid arg2, bool noError, int location, bool inNumeric = false);
extern Operator right_oper(ParseState* pstate, List* op, Oid arg, bool noError, int location);
extern Operator left_oper(ParseState* pstate, List* op, Oid arg, bool noError, int location);

/* Routines to find operators that DO NOT require coercion --- ie, their */
/* input types are either exactly as given, or binary-compatible */
extern Operator compatible_oper(ParseState* pstate, List* op, Oid arg1, Oid arg2, bool noError, int location);

/* currently no need for compatible_left_oper/compatible_right_oper */
/* Routines for identifying "<", "=", ">" operators for a type */
extern void get_sort_group_operators(
    Oid argtype, bool needLT, bool needEQ, bool needGT, Oid* ltOpr, Oid* eqOpr, Oid* gtOpr, bool* isHashable);

/* Convenience routines for common calls on the above */
extern Oid compatible_oper_opid(List* op, Oid arg1, Oid arg2, bool noError);

/* Extract operator OID or underlying-function OID from an Operator tuple */
extern Oid oprid(Operator op);
extern Oid oprfuncid(Operator op);

extern bool IsIntType(Oid typeoid);
extern bool IsCharType(Oid typeoid);
#ifdef DOLPHIN
extern bool IsUnsignedIntType(Oid typeoid);
extern bool IsFloatType(Oid typeoid);
extern bool IsNumericType(Oid typeoid);
extern bool IsDolphinStringType(Oid typeoid);
extern bool IsDatetimeType(Oid typeoid);
#endif

/* Build expression tree for an operator invocation */
extern Node* parse_get_last_srf(ParseState* pstate);
extern Expr *make_op(ParseState *pstate, List *opname, Node *ltree, Node *rtree, Node *last_srf, int location,
                     bool inNumeric = false);
extern Expr* make_scalar_array_op(ParseState* pstate, List* opname, bool useOr, Node* ltree, Node* rtree, int location);
extern Oid OperatorLookup(List* operatorName, Oid leftObjectId, Oid rightObjectId, bool* defined);
extern void InvalidateOprCacheCallBack(Datum arg, int cacheid, uint32 hashvalue);
#ifdef DOLPHIN
extern Oid binary_oper_exact_extern(List* opname, Oid arg1, Oid arg2, bool use_a_style_coercion);
#endif
#endif /* PARSE_OPER_H */
