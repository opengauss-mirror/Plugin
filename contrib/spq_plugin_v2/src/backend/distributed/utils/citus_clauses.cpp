/*
 * citus_clauses.c
 *
 * Routines roughly equivalent to postgres' util/clauses.
 *
 * Copyright (c) Citus Data, Inc.
 */

#include "postgres.h"

#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "executor/executor.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "nodes/nodes.h"
#include "nodes/primnodes.h"
#include "optimizer/clauses.h"
#include "optimizer/planner.h"
#include "optimizer/planmain.h"
#include "utils/datum.h"
#include "utils/lsyscache.h"
#include "utils/syscache.h"
#include "utils/builtins.h"
#include "catalog/pg_proc_fn.h"
#include "funcapi.h"

#include "distributed/citus_clauses.h"
#include "distributed/insert_select_planner.h"
#include "distributed/metadata_cache.h"
#include "distributed/multi_router_planner.h"
#include "distributed/version_compat.h"

#include "type_cast.h"
/* private function declarations */
static bool IsVariableExpression(Node* node);
static Expr* citus_evaluate_expr(
    Expr* expr, Oid result_type, int32 result_typmod, Oid result_collation,
    CoordinatorEvaluationContext* coordinatorEvaluationContext);
static bool CitusIsVolatileFunctionIdChecker(Oid func_id, void* context);
static bool CitusIsMutableFunctionIdChecker(Oid func_id, void* context);
static bool ShouldEvaluateExpression(Expr* expression);
static bool ShouldEvaluateFunctions(CoordinatorEvaluationContext* evaluationContext);
static void FixFunctionArguments(Node* expr);
static bool FixFunctionArgumentsWalker(Node* expr, void* context);
static void recheck_cast_function_args(List* args, Oid result_type, HeapTuple func_tuple);
static List* fetch_function_defaults(HeapTuple func_tuple);
static List* reorder_function_arguments(List* args, HeapTuple func_tuple);
static List* add_function_defaults(List* args, HeapTuple func_tuple);
static List* expand_function_arguments(List* args, Oid result_type, HeapTuple func_tuple);
/* callback function for check_functions_in_node */
typedef bool (*check_function_callback)(Oid func_id, void* context);
static bool check_functions_in_node(Node* node, check_function_callback checker,
                                    void* context);
/*
 * recheck_cast_function_args: recheck function args and typecast as needed
 * after adding defaults.
 *
 * It is possible for some of the defaulted arguments to be polymorphic;
 * therefore we can't assume that the default expressions have the correct
 * data types already.	We have to re-resolve polymorphics and do coercion
 * just like the parser did.
 *
 * This should be a no-op if there are no polymorphic arguments,
 * but we do it anyway to be sure.
 *
 * Note: if any casts are needed, the args list is modified in-place;
 * caller should have already copied the list structure.
 */
static void recheck_cast_function_args(List* args, Oid result_type, HeapTuple func_tuple)
{
    Form_pg_proc funcform = (Form_pg_proc)GETSTRUCT(func_tuple);
    int nargs;
    Oid actual_arg_types[FUNC_MAX_ARGS];
    Oid declared_arg_types[FUNC_MAX_ARGS];
    Oid rettype;
    ListCell* lc = NULL;
    bool func_package = false;
    bool isNull = false;
    Oid* proargtypes = NULL;
    int proc_arg = funcform->pronargs;

    if (list_length(args) > FUNC_MAX_ARGS)
        ereport(ERROR, (errmodule(MOD_OPT), errcode(ERRCODE_TOO_MANY_ARGUMENTS),
                        errmsg("too many function arguments")));
    nargs = 0;
    foreach (lc, args) {
        actual_arg_types[nargs++] = exprType((Node*)lfirst(lc));
    }

    Datum ispackage = SysCacheGetAttr(PROCOID, func_tuple, Anum_pg_proc_package, &isNull);
    func_package = DatumGetBool(ispackage);
    if (!func_package || nargs <= funcform->pronargs) {
        Assert(nargs == funcform->pronargs);
        oidvector* proargs = ProcedureGetArgTypes(func_tuple);
        proargtypes = proargs->values;
    } else if (nargs > funcform->pronargs) {
        Datum proallargtypes;
        ArrayType* arr = NULL;
        proallargtypes =
            SysCacheGetAttr(PROCOID, func_tuple, Anum_pg_proc_proallargtypes, &isNull);
        if (!isNull) {
            arr = DatumGetArrayTypeP(proallargtypes); /* ensure not toasted */
            proc_arg = ARR_DIMS(arr)[0];
            proargtypes = (Oid*)ARR_DATA_PTR(arr);
        }
    }

    errno_t errorno;
    errorno = memcpy_s(declared_arg_types, FUNC_MAX_ARGS * sizeof(Oid), proargtypes,
                       proc_arg * sizeof(Oid));
    securec_check(errorno, "", "");

    /* if argtype is table of, change its element type */
    for (int i = 0; i < nargs; i++) {
        Oid baseOid = InvalidOid;
        if (isTableofType(declared_arg_types[i], &baseOid, NULL)) {
            declared_arg_types[i] = baseOid;
        }
    }

    rettype = enforce_generic_type_consistency(actual_arg_types, declared_arg_types,
                                               nargs, funcform->prorettype, false);
    /* let's just check we got the same answer as the parser did ... */
    if (rettype != result_type)
        ereport(ERROR,
                (errmodule(MOD_OPT), errcode(ERRCODE_OPTIMIZER_INCONSISTENT_STATE),
                 errmsg("function's resolved result type changed during planning")));

    /* perform any necessary typecasting of arguments */
    make_fn_arguments(NULL, args, actual_arg_types, declared_arg_types);
}

/*
 * fetch_function_defaults: get function's default arguments as expression list
 */
static List* fetch_function_defaults(HeapTuple func_tuple)
{
    List* defaults = NIL;
    Datum proargdefaults;
    bool isnull = false;
    char* str = NULL;

    /* The error cases here shouldn't happen, but check anyway */
    proargdefaults =
        SysCacheGetAttr(PROCOID, func_tuple, Anum_pg_proc_proargdefaults, &isnull);
    if (isnull)
        ereport(ERROR, (errmodule(MOD_OPT), errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
                        errmsg("not enough default arguments")));
    str = TextDatumGetCString(proargdefaults);
    defaults = (List*)stringToNode(str);

    AssertEreport(IsA(defaults, List), MOD_OPT, "");
    pfree_ext(str);

    return defaults;
}

/*
 * reorder_function_arguments: convert named-notation args to positional args
 *
 * This function also inserts default argument values as needed, since it's
 * impossible to form a truly valid positional call without that.
 */
static List* reorder_function_arguments(List* args, HeapTuple func_tuple)
{
    Form_pg_proc funcform = (Form_pg_proc)GETSTRUCT(func_tuple);
    int pronargs = funcform->pronargs;
    int nargsprovided = list_length(args);
    Node* argarray[FUNC_MAX_ARGS];
    ListCell* lc = NULL;
    int i;
    bool func_package = false;
    bool isNull = false;
    errno_t rc;

    Datum ispackage = SysCacheGetAttr(PROCOID, func_tuple, Anum_pg_proc_package, &isNull);
    func_package = DatumGetBool(ispackage);
    if (!func_package) {
        Assert(nargsprovided <= pronargs);
    } else if (pronargs < nargsprovided) {
        pronargs = nargsprovided;
    }

    if (pronargs > FUNC_MAX_ARGS)
        ereport(ERROR, (errmodule(MOD_OPT), errcode(ERRCODE_TOO_MANY_ARGUMENTS),
                        errmsg("too many function arguments")));
    rc = memset_s(argarray, pronargs * sizeof(Node*), 0, pronargs * sizeof(Node*));
    securec_check(rc, "", "");

    /* Deconstruct the argument list into an array indexed by argnumber */
    i = 0;
    foreach (lc, args) {
        Node* arg = (Node*)lfirst(lc);

        if (!IsA(arg, NamedArgExpr)) {
            /* positional argument, assumed to precede all named args */
            AssertEreport(argarray[i] == NULL, MOD_OPT, "");
            argarray[i++] = arg;
        } else {
            NamedArgExpr* na = (NamedArgExpr*)arg;

            AssertEreport(argarray[na->argnumber] == NULL, MOD_OPT, "");
            argarray[na->argnumber] = (Node*)na->arg;
        }
    }

    /*
     * Fetch default expressions, if needed, and insert into array at proper
     * locations (they aren't necessarily consecutive or all used)
     */
    if (nargsprovided < pronargs) {
        char* argmodes = NULL;
        char** argnames = NULL;
        Oid* argtypes = NULL;
        bool isnull = false;
        int proallargs = 0;
        Datum defposdatum;
        int2vector* defpos = NULL;
        int* argdefpos = NULL;
        List* defaults = NIL;
        int counter = 0;
        int pos = 0;

        proallargs = get_func_arg_info(func_tuple, &argtypes, &argnames, &argmodes);
        if (pronargs <= FUNC_MAX_ARGS_INROW) {
            defposdatum = SysCacheGetAttr(PROCOID, func_tuple,
                                          Anum_pg_proc_prodefaultargpos, &isnull);
            AssertEreport(!isnull, MOD_OPT, "");
            defpos = (int2vector*)DatumGetPointer(defposdatum);
        } else {
            defposdatum = SysCacheGetAttr(PROCOID, func_tuple,
                                          Anum_pg_proc_prodefaultargposext, &isnull);
            AssertEreport(!isnull, MOD_OPT, "");
            defpos = (int2vector*)PG_DETOAST_DATUM(defposdatum);
        }

        FetchDefaultArgumentPos(&argdefpos, defpos, argmodes, proallargs);

        defaults = fetch_function_defaults(func_tuple);

        foreach (lc, defaults) {
            pos = argdefpos[counter];
            if (argarray[pos] == NULL)
                argarray[pos] = (Node*)lfirst(lc);
            counter++;
        }

        pfree_ext(argdefpos);
        if (argtypes != NULL)
            pfree_ext(argtypes);
        if (argmodes != NULL)
            pfree_ext(argmodes);
        if (argnames != NULL) {
            for (counter = 0; counter < proallargs; counter++) {
                if (argnames[counter])
                    pfree_ext(argnames[counter]);
            }
            pfree_ext(argnames);
        }
        list_free_ext(defaults);
    }

    /* Now reconstruct the args list in proper order */
    args = NIL;
    for (i = 0; i < pronargs; i++) {
        AssertEreport(argarray[i] != NULL, MOD_OPT, "");
        args = lappend(args, argarray[i]);
    }

    return args;
}

/*
 * add_function_defaults: add missing function arguments from its defaults
 *
 * This is used only when the argument list was positional to begin with,
 * and so we know we just need to add defaults at the end.
 */
static List* add_function_defaults(List* args, HeapTuple func_tuple)
{
    Form_pg_proc funcform = (Form_pg_proc)GETSTRUCT(func_tuple);
    int nargsprovided = list_length(args);
    List* defaults = NIL;
    int ndelete;

    /* Get all the default expressions from the pg_proc tuple */
    defaults = fetch_function_defaults(func_tuple);

    /* Delete any unused defaults from the list */
    ndelete = nargsprovided + list_length(defaults) - funcform->pronargs;
    if (ndelete < 0)
        ereport(ERROR, (errmodule(MOD_OPT), errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
                        errmsg("not enough default arguments")));
    while (ndelete-- > 0)
        defaults = list_delete_first(defaults);

    /* And form the combined argument list, not modifying the input list */
    return list_concat(list_copy(args), defaults);
}

/*
 * expand_function_arguments: convert named-notation args to positional args
 * and/or insert default args, as needed
 *
 * If we need to change anything, the input argument list is copied, not
 * modified.
 *
 * Note: this gets applied to operator argument lists too, even though the
 * cases it handles should never occur there.  This should be OK since it
 * will fall through very quickly if there's nothing to do.
 */
static List* expand_function_arguments(List* args, Oid result_type, HeapTuple func_tuple)
{
    Form_pg_proc funcform = (Form_pg_proc)GETSTRUCT(func_tuple);
    bool has_named_args = false;
    ListCell* lc = NULL;

    /* Do we have any named arguments? */
    foreach (lc, args) {
        Node* arg = (Node*)lfirst(lc);

        if (IsA(arg, NamedArgExpr)) {
            has_named_args = true;
            break;
        }
    }

    /* If so, we must apply reorder_function_arguments */
    if (has_named_args) {
        args = reorder_function_arguments(args, func_tuple);
        /* Recheck argument types and add casts if needed */
        recheck_cast_function_args(args, result_type, func_tuple);
    } else if (list_length(args) < funcform->pronargs) {
        /* No named args, but we seem to be short some defaults */
        args = add_function_defaults(args, func_tuple);
        /* Recheck argument types and add casts if needed */
        recheck_cast_function_args(args, result_type, func_tuple);
    }

    return args;
}

/*
 *	check_functions_in_node -
 *	  apply checker() to each function OID contained in given expression node
 *
 * Returns true if the checker() function does; for nodes representing more
 * than one function call, returns true if the checker() function does so
 * for any of those functions.  Returns false if node does not invoke any
 * SQL-visible function.  Caller must not pass node == NULL.
 *
 * This function examines only the given node; it does not recurse into any
 * sub-expressions.  Callers typically prefer to keep control of the recursion
 * for themselves, in case additional checks should be made, or because they
 * have special rules about which parts of the tree need to be visited.
 *
 * Note: we ignore MinMaxExpr, SQLValueFunction, XmlExpr, CoerceToDomain,
 * and NextValueExpr nodes, because they do not contain SQL function OIDs.
 * However, they can invoke SQL-visible functions, so callers should take
 * thought about how to treat them.
 */
static bool check_functions_in_node(Node* node, check_function_callback checker,
                                    void* context)
{
    switch (nodeTag(node)) {
        case T_Aggref: {
            Aggref* expr = (Aggref*)node;

            if (checker(expr->aggfnoid, context))
                return true;
        } break;
        case T_WindowFunc: {
            WindowFunc* expr = (WindowFunc*)node;

            if (checker(expr->winfnoid, context))
                return true;
        } break;
        case T_FuncExpr: {
            FuncExpr* expr = (FuncExpr*)node;

            if (checker(expr->funcid, context))
                return true;
        } break;
        case T_OpExpr:
        case T_DistinctExpr: /* struct-equivalent to OpExpr */
        case T_NullIfExpr:   /* struct-equivalent to OpExpr */
        {
            OpExpr* expr = (OpExpr*)node;

            /* Set opfuncid if it wasn't set already */
            set_opfuncid(expr);
            if (checker(expr->opfuncid, context))
                return true;
        } break;
        case T_ScalarArrayOpExpr: {
            ScalarArrayOpExpr* expr = (ScalarArrayOpExpr*)node;

            set_sa_opfuncid(expr);
            if (checker(expr->opfuncid, context))
                return true;
        } break;
        case T_CoerceViaIO: {
            CoerceViaIO* expr = (CoerceViaIO*)node;
            Oid iofunc;
            Oid typioparam;
            bool typisvarlena;

            /* check the result type's input function */
            getTypeInputInfo(expr->resulttype, &iofunc, &typioparam);
            if (checker(iofunc, context))
                return true;
            /* check the input type's output function */
            getTypeOutputInfo(exprType((Node*)expr->arg), &iofunc, &typisvarlena);
            if (checker(iofunc, context))
                return true;
        } break;
        case T_RowCompareExpr: {
            RowCompareExpr* rcexpr = (RowCompareExpr*)node;
            ListCell* opid;

            foreach (opid, rcexpr->opnos) {
                Oid opfuncid = get_opcode(lfirst_oid(opid));

                if (checker(opfuncid, context))
                    return true;
            }
        } break;
        default:
            break;
    }
    return false;
}

/*
 * RequiresCoordinatorEvaluation returns the executor needs to reparse and
 * try to execute this query, which is the case if the query contains
 * any stable or volatile function.
 */
bool RequiresCoordinatorEvaluation(Query* query)
{
    if (query->commandType == CMD_SELECT && !query->hasModifyingCTE) {
        return false;
    }

    return FindNodeMatchingCheckFunction((Node*)query, CitusIsMutableFunction);
}

/*
 * ExecuteCoordinatorEvaluableExpressions evaluates expressions and parameters
 * that can be resolved to a constant.
 */
void ExecuteCoordinatorEvaluableExpressions(Query* query, PlanState* planState)
{
    CoordinatorEvaluationContext coordinatorEvaluationContext;

    coordinatorEvaluationContext.planState = planState;
    if (query->commandType == CMD_SELECT) {
        coordinatorEvaluationContext.evaluationMode = EVALUATE_PARAMS;
    } else {
        coordinatorEvaluationContext.evaluationMode = EVALUATE_FUNCTIONS_PARAMS;
    }

    PartiallyEvaluateExpression((Node*)query, &coordinatorEvaluationContext);
}

/*
 * PartiallyEvaluateExpression descends into an expression tree to evaluate
 * expressions that can be resolved to a constant on the master. Expressions
 * containing a Var are skipped, since the value of the Var is not known
 * on the master.
 */
Node* PartiallyEvaluateExpression(
    Node* expression, CoordinatorEvaluationContext* coordinatorEvaluationContext)
{
    if (expression == NULL || IsA(expression, Const)) {
        return expression;
    }

    NodeTag nodeTag = nodeTag(expression);
    if (nodeTag == T_Param) {
        Param* param = (Param*)expression;
        if (param->paramkind == PARAM_SUBLINK) {
            /* ExecInitExpr cannot handle PARAM_SUBLINK */
            return expression;
        }

        return (Node*)citus_evaluate_expr(
            (Expr*)expression, exprType(expression), exprTypmod(expression),
            exprCollation(expression), coordinatorEvaluationContext);
    } else if (ShouldEvaluateExpression((Expr*)expression) &&
               ShouldEvaluateFunctions(coordinatorEvaluationContext)) {
        /*
         * The planner normally evaluates constant expressions, but we may be
         * working on the original query tree. We could rely on
         * citus_evaluate_expr to evaluate constant expressions, but there are
         * certain node types that citus_evaluate_expr does not expect because
         * the planner normally replaces them (in particular, CollateExpr).
         * Hence, we first evaluate constant expressions using
         * eval_const_expressions before continuing.
         *
         * NOTE: We do not use expression_planner here, since all it does
         * apart from calling eval_const_expressions is call fix_opfuncids.
         * We do not need this, since that is already called in
         * citus_evaluate_expr. So we won't needlessly traverse the expression
         * tree by calling it another time.
         */
        expression = eval_const_expressions(NULL, expression);

        /*
         * It's possible that after evaluating const expressions we
         * actually don't need to evaluate this expression anymore e.g:
         *
         * 1 = 0 AND now() > timestamp '10-10-2000 00:00'
         *
         * This statement would simply resolve to false, because 1 = 0 is
         * false. That's why we now check again if we should evaluate the
         * expression and only continue if we still do.
         */
        if (!ShouldEvaluateExpression((Expr*)expression)) {
            return (Node*)expression_tree_mutator(
                expression,
                reinterpret_cast<Node* (*)(Node*, void*)>(PartiallyEvaluateExpression),
                coordinatorEvaluationContext);
        }

        if (FindNodeMatchingCheckFunction(expression, IsVariableExpression)) {
            /*
             * The expression contains a variable expression (e.g. a stable function,
             * which has a column reference as its input). That means that we cannot
             * evaluate the expression on the coordinator, since the result depends
             * on the input.
             *
             * Skipping function evaluation for these expressions is safe in most
             * cases, since the function will always be re-evaluated for every input
             * value. An exception is function calls that call another stable function
             * that should not be re-evaluated, such as now().
             */
            return (Node*)expression_tree_mutator(
                expression,
                reinterpret_cast<Node* (*)(Node*, void*)>(PartiallyEvaluateExpression),
                coordinatorEvaluationContext);
        }

        return (Node*)citus_evaluate_expr(
            (Expr*)expression, exprType(expression), exprTypmod(expression),
            exprCollation(expression), coordinatorEvaluationContext);
    } else if (nodeTag == T_Query) {
        Query* query = (Query*)expression;
        CoordinatorEvaluationContext subContext = *coordinatorEvaluationContext;
        if (query->commandType != CMD_SELECT) {
            /*
             * Currently INSERT SELECT evaluates stable functions on master,
             * while a plain SELECT does not. For evaluating SELECT evaluationMode is
             * EVALUATE_PARAMS, but if recursing into a modifying CTE switch into
             * EVALUATE_FUNCTIONS_PARAMS.
             */
            subContext.evaluationMode = EVALUATE_FUNCTIONS_PARAMS;
        }

        return (Node*)query_tree_mutator(
            query, reinterpret_cast<Node* (*)(Node*, void*)>(PartiallyEvaluateExpression),
            &subContext, QTW_DONT_COPY_QUERY);
    } else {
        return (Node*)expression_tree_mutator(
            expression,
            reinterpret_cast<Node* (*)(Node*, void*)>(PartiallyEvaluateExpression),
            coordinatorEvaluationContext);
    }

    return expression;
}

/*
 * ShouldEvaluateFunctions is a helper function which is used to
 * decide whether the function/expression should be evaluated with the input
 * coordinatorEvaluationContext.
 */
static bool ShouldEvaluateFunctions(CoordinatorEvaluationContext* evaluationContext)
{
    if (evaluationContext == NULL) {
        /* if no context provided, evaluate, which is the default behaviour */
        return true;
    }

    return evaluationContext->evaluationMode == EVALUATE_FUNCTIONS_PARAMS;
}

/*
 * ShouldEvaluateExpression returns true if Citus should evaluate the
 * input node on the coordinator.
 */
static bool ShouldEvaluateExpression(Expr* expression)
{
    NodeTag nodeTag = nodeTag(expression);

    switch (nodeTag) {
        case T_FuncExpr: {
            FuncExpr* funcExpr = (FuncExpr*)expression;

            /* we cannot evaluate set returning functions */
            bool isSetReturningFunction = funcExpr->funcretset;
            return !isSetReturningFunction;
        }

        case T_OpExpr:
        case T_DistinctExpr:
        case T_NullIfExpr:
        case T_CoerceViaIO:
        case T_ArrayCoerceExpr:
        case T_ScalarArrayOpExpr:
        case T_RowExpr:
        case T_RowCompareExpr:
        case T_RelabelType:
        case T_CoerceToDomain:
#ifdef DISABLE_OG_COMMENTS
        case T_NextValueExpr:
#endif
        {
            return true;
        }

        default:
            return false;
    }
}

/*
 * IsVariableExpression returns whether the given node is a variable expression,
 * meaning its result depends on the input data and is not constant for the whole
 * query.
 */
static bool IsVariableExpression(Node* node)
{
    if (IsA(node, Aggref)) {
        return true;
    }

    if (IsA(node, WindowFunc)) {
        return true;
    }

    if (IsA(node, Param)) {
        /* ExecInitExpr cannot handle PARAM_SUBLINK */
        return ((Param*)node)->paramkind == PARAM_SUBLINK;
    }

    return IsA(node, Var);
}

/*
 * a copy of pg's evaluate_expr, pre-evaluate a constant expression
 *
 * We use the executor's routine ExecEvalExpr() to avoid duplication of
 * code and ensure we get the same result as the executor would get.
 *
 * *INDENT-OFF*
 */
static Expr* citus_evaluate_expr(
    Expr* expr, Oid result_type, int32 result_typmod, Oid result_collation,
    CoordinatorEvaluationContext* coordinatorEvaluationContext)
{
    PlanState* planState = NULL;
    EState* estate;
    ExprState* exprstate;
    Datum const_val;
    bool const_is_null;
    int16 resultTypLen;
    bool resultTypByVal;

    if (coordinatorEvaluationContext) {
        planState = coordinatorEvaluationContext->planState;

        if (IsA(expr, Param)) {
            if (coordinatorEvaluationContext->evaluationMode == EVALUATE_NONE) {
                /* bail out, the caller doesn't want params to be evaluated  */
                return expr;
            }
        } else if (coordinatorEvaluationContext->evaluationMode !=
                   EVALUATE_FUNCTIONS_PARAMS) {
            /* should only get here for node types we should evaluate */
            Assert(ShouldEvaluateExpression(expr));

            /* bail out, the caller doesn't want functions/expressions to be evaluated */
            return expr;
        }
    }

    /*
     * To use the executor, we need an EState.
     */
    estate = CreateExecutorState();

    /* We can use the estate's working context to avoid memory leaks. */
    MemoryContext oldcontext = MemoryContextSwitchTo(estate->es_query_cxt);

    /* handles default values */
    FixFunctionArguments((Node*)expr);

    /* Make sure any opfuncids are filled in. */
    fix_opfuncids((Node*)expr);

    /*
     * Prepare expr for execution.  (Note: we can't use ExecPrepareExpr
     * because it'd result in recursively invoking eval_const_expressions.)
     */
    exprstate = ExecInitExpr(expr, planState);

    /*
     * Get short lived per tuple context as evaluate_expr does. Here we don't
     * use planState->ExprContext as it might cause double-free'ing executor
     * state.
     */
    ExprContext* econtext = GetPerTupleExprContext(estate);
    if (planState) {
        /*
         * If planState exists, then we add es_param_list_info to per tuple
         * ExprContext as we need them when evaluating prepared statements.
         */
        econtext->ecxt_param_list_info = planState->state->es_param_list_info;
    }

    /*
     * And evaluate it.
     */
    const_val = ExecEvalExprSwitchContext(exprstate, econtext, &const_is_null);

    /* Get info needed about result datatype */
    get_typlenbyval(result_type, &resultTypLen, &resultTypByVal);

    /* Get back to outer memory context */
    MemoryContextSwitchTo(oldcontext);

    /*
     * Must copy result out of sub-context used by expression eval.
     *
     * Also, if it's varlena, forcibly detoast it.  This protects us against
     * storing TOAST pointers into plans that might outlive the referenced
     * data.  (makeConst would handle detoasting anyway, but it's worth a few
     * extra lines here so that we can do the copy and detoast in one step.)
     */
    if (!const_is_null) {
        if (resultTypLen == -1)
            const_val = PointerGetDatum(PG_DETOAST_DATUM_COPY(const_val));
        else
            const_val = datumCopy(const_val, resultTypByVal, resultTypLen);
    }

    /* Release all the junk we just created */
    FreeExecutorState(estate);

    /*
     * Make the constant result node.
     */
    return (Expr*)makeConst(result_type, result_typmod, result_collation, resultTypLen,
                            const_val, const_is_null, resultTypByVal);
}

/* *INDENT-ON* */

/*
 * CitusIsVolatileFunctionIdChecker checks if the given function id is
 * a volatile function other than read_intermediate_result().
 */
static bool CitusIsVolatileFunctionIdChecker(Oid func_id, void* context)
{
    if (func_id == CitusReadIntermediateResultFuncId() ||
        func_id == CitusReadIntermediateResultArrayFuncId()) {
        return false;
    }

    return (func_volatile(func_id) == PROVOLATILE_VOLATILE);
}

/*
 * CitusIsVolatileFunction checks if the given node is a volatile function
 * other than Citus's internal functions.
 */
bool CitusIsVolatileFunction(Node* node)
{
    /* Check for volatile functions in node itself */
    if (check_functions_in_node(node, CitusIsVolatileFunctionIdChecker, NULL)) {
        return true;
    }
#ifdef DISABLE_OG_COMMENTS
    if (IsA(node, NextValueExpr)) {
        /* NextValueExpr is volatile */
        return true;
    }
#endif
    return false;
}

/*
 * CitusIsMutableFunctionIdChecker checks if the given function id is
 * a mutable function other than read_intermediate_result().
 */
static bool CitusIsMutableFunctionIdChecker(Oid func_id, void* context)
{
    if (func_id == CitusReadIntermediateResultFuncId() ||
        func_id == CitusReadIntermediateResultArrayFuncId()) {
        return false;
    } else {
        return (func_volatile(func_id) != PROVOLATILE_IMMUTABLE);
    }
}

/*
 * CitusIsMutableFunction checks if the given node is a mutable function
 * other than Citus's internal functions.
 */
bool CitusIsMutableFunction(Node* node)
{
    /* Check for mutable functions in node itself */
    if (check_functions_in_node(node, CitusIsMutableFunctionIdChecker, NULL)) {
        return true;
    }
#ifdef DISABLE_OG_COMMENTS
    if (IsA(node, SQLValueFunction)) {
        /* all variants of SQLValueFunction are stable */
        return true;
    }

    if (IsA(node, NextValueExpr)) {
        /* NextValueExpr is volatile */
        return true;
    }
#endif
    return false;
}

/* FixFunctionArguments applies expand_function_arguments to all function calls. */
static void FixFunctionArguments(Node* expr)
{
    FixFunctionArgumentsWalker(expr, NULL);
}

/* FixFunctionArgumentsWalker is the helper function for fix_funcargs. */
static bool FixFunctionArgumentsWalker(Node* expr, void* context)
{
    if (expr == NULL) {
        return false;
    }

    if (IsA(expr, FuncExpr)) {
        FuncExpr* funcExpr = castNode(FuncExpr, expr);
        HeapTuple func_tuple =
            SearchSysCache1(PROCOID, ObjectIdGetDatum(funcExpr->funcid));
        if (!HeapTupleIsValid(func_tuple)) {
            elog(ERROR, "cache lookup failed for function %u", funcExpr->funcid);
        }

        funcExpr->args = expand_function_arguments(funcExpr->args,
                                                   funcExpr->funcresulttype, func_tuple);
        ReleaseSysCache(func_tuple);
    }

    return expression_tree_walker(expr, walker_cast0(FixFunctionArgumentsWalker), NULL);
}
