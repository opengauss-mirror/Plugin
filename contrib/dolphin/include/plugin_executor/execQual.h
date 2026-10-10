/* -------------------------------------------------------------------------
 *
 * execQual.h
 *    Server function-result cache interfaces used by Dolphin expressions.
 *
 * Portions Copyright (c) 2020 Huawei Technologies Co.,Ltd.
 *
 * -------------------------------------------------------------------------
 */
#ifndef DOLPHIN_EXECQUAL_H
#define DOLPHIN_EXECQUAL_H

#include "nodes/execnodes.h"

extern void EStateFuncAssignCache(ExprState* state, ExprContext* econtext, FuncExpr* fe, FuncExprState* fcache);
extern bool EStateFuncGetRetCache(FuncExprState* fcache, Datum* retvalue, bool* retnull);
extern bool EStateFuncPutRetCache(FuncExprState* fcache, Datum ret);

#endif /* DOLPHIN_EXECQUAL_H */
