/*
  This code implements one part of functonality of
  free available library PL/Vision. Please look www.quest.com

  Original author: Steven Feuerstein, 1996 - 2002
  PostgreSQL implementation author: Pavel Stehule, 2006-2018

  This module is under BSD Licence

  History:
    1.0. first public version 13. March 2006
*/

#include <sys/time.h>
#include <stdlib.h>

#include "postgres.h"
#include "catalog/pg_type.h"
#include "lib/stringinfo.h"
#include "nodes/pg_list.h"
#include "utils/date.h"
#include "utils/builtins.h"
#include "plvlex.h"
#include "sqlparse.h"
#include "funcapi.h"
#include "orafce.h"
#include "builtins.h"

typedef struct {
	List	*nodes;
	int 	nnodes;
	int	cnode;
	char **values;
} tokensFctx;

PG_FUNCTION_INFO_V1(plvlex_tokens);

extern int      orafce_sql_yyparse();
extern void orafce_sql_yyerror(List **result, const char *message);
extern void orafce_sql_scanner_init(const char *str);
extern void orafce_sql_scanner_finish(void);

#define CSTRING(txt) \
	( \
    get_session_context()->__len = VARSIZE(txt) - VARHDRSZ, \
    get_session_context()->__result = (char*)palloc(get_session_context()->__len + 1), \
    memcpy_s(get_session_context()->__result, get_session_context()->__len, VARDATA(txt), get_session_context()->__len), \
    get_session_context()->__result[get_session_context()->__len] = '\0', \
    get_session_context()->__result)


#define COPY_TO_S(src,dest,col)	(dest->col = (src->col ? pstrdup(src->col) : NULL))
#define COPY_TO(src,dest,col)	(dest->col = src->col)

#define COPY_FIELDS(src,dest) \
	COPY_TO(src, dest, typenode), \
	COPY_TO_S(src,dest,str), \
	COPY_TO(src,dest,keycode), \
	COPY_TO(src,dest,lloc), \
	COPY_TO_S(src,dest,sep), \
	COPY_TO(src,dest,modificator), \
	COPY_TO(src,dest,classname)


#define COPY_NODE(src)   \
  ( \
    get_session_context()->__node = (orafce_lexnode*) palloc(sizeof(orafce_lexnode)),  \
    COPY_FIELDS(src,get_session_context()->__node), \
    get_session_context()->__node)


/* Finding triplet a.b --> a */

#define IsType(node, type)	(node->typenode == X_##type)
#define APPEND_NODE(list,nd)	\
	if (nd) \
	{ \
		list = lappend(list, nd); \
		nd = NULL; \
	}

#define mod(a)  (a->modificator)
#define SF(a)	(a ? a : "")

#define NEWNODE(type) \
	( \
	get_session_context()->__node = (orafce_lexnode *) palloc(sizeof(orafce_lexnode)), \
	get_session_context()->__node->typenode = X_##type, \
	get_session_context()->__node->modificator = NULL, \
	get_session_context()->__node->sep = NULL, \
	get_session_context()->__node->keycode = -1, \
	get_session_context()->__node->classname = #type, \
	get_session_context()->__node->lloc = 0, \
	get_session_context()->__node )



static orafce_lexnode *
compose(orafce_lexnode *a, orafce_lexnode *b)
{
	orafce_lexnode *result;
	StringInfo sinfo;

	sinfo = makeStringInfo();
	result = NEWNODE(IDENT);
	result->lloc = a->lloc;

	if (strcmp(SF(mod(a)), "dq") == 0)
		appendStringInfo(sinfo, "\"%s\".", a->str);
	else
	{
		appendStringInfoString(sinfo, a->str);
		appendStringInfoChar(sinfo, '.');
	}

	if (strcmp(SF(mod(b)), "dq") == 0)
		appendStringInfo(sinfo, "\"%s\"", b->str);
	else
		appendStringInfoString(sinfo, b->str);

	result->str = sinfo->data;

	return result;
}

static List *
filterList(List *list, bool skip_spaces, bool qnames)
{
	List *result = NIL;
	ListCell *cell;
	bool isdot = false;
	orafce_lexnode *a = NULL;
	orafce_lexnode *dot = NULL;

	foreach(cell, list)
	{
		orafce_lexnode *nd = (orafce_lexnode *) lfirst(cell);

		if (qnames)
		{
			isdot = (IsType(nd, OTHERS) && (nd->str[0] == '.'));

			if (IsType(nd, IDENT) && dot && a)
			{
				a = compose(a, nd);
				dot = NULL;
				continue;
			}
			else if (isdot && !dot && a)
			{
				dot = COPY_NODE(nd);
				continue;
			}
			else if (IsType(nd, IDENT) && !a)
			{
				a = COPY_NODE(nd);
				continue;
			}
		}

		/* clean buffered values */
		APPEND_NODE(result,a);
		APPEND_NODE(result,dot);

		if (!(skip_spaces && IsType(nd, WHITESPACE)))
		{
			result = lappend(result, COPY_NODE(nd));
		}
	}

	/* clean buffered values */
	APPEND_NODE(result,a);
	APPEND_NODE(result,dot);

	return result;
}

Datum
plvlex_tokens(PG_FUNCTION_ARGS)
{
	FuncCallContext	   *funcctx;
	TupleDesc			tupdesc;
	AttInMetadata	   *attinmeta;
	tokensFctx		   *fctx;


	if (SRF_IS_FIRSTCALL ())
	{
		MemoryContext  oldcontext;
		List *lexems;
		text *src = PG_GETARG_TEXT_P(0);
		bool skip_spaces = PG_GETARG_BOOL(1);
		bool qnames = PG_GETARG_BOOL(2);

		orafce_sql_scanner_init(CSTRING(src));
		if (orafce_sql_yyparse(&lexems) != 0)
			orafce_sql_yyerror(NULL, "bogus input");

		orafce_sql_scanner_finish();

		funcctx = SRF_FIRSTCALL_INIT ();
		oldcontext = MemoryContextSwitchTo (funcctx->multi_call_memory_ctx);

		fctx = (tokensFctx*) palloc (sizeof (tokensFctx));
		funcctx->user_fctx = (void *)fctx;

		fctx->nodes = filterList(lexems, skip_spaces, qnames);
		fctx->nnodes = list_length(fctx->nodes);
		fctx->cnode = 0;

		fctx->values = (char **) palloc (6 * sizeof (char *));
		fctx->values  [0] = (char*) palloc (16 * sizeof (char));
		fctx->values  [1] = (char*) palloc (1024 * sizeof (char));
		fctx->values  [2] = (char*) palloc (16 * sizeof (char));
		fctx->values  [3] = (char*) palloc (16 * sizeof (char));
		fctx->values  [4] = (char*) palloc (255 * sizeof (char));
		fctx->values  [5] = (char*) palloc (255 * sizeof (char));

#if PG_VERSION_NUM >= 120000

		tupdesc = CreateTemplateTupleDesc (6);

#else

		tupdesc = CreateTemplateTupleDesc (6, false);

#endif

		TupleDescInitEntry (tupdesc,  1, "start_pos", INT4OID, -1, 0);
		TupleDescInitEntry (tupdesc,  2, "token",     TEXTOID, -1, 0);
		TupleDescInitEntry (tupdesc,  3, "keycode",   INT4OID, -1, 0);
		TupleDescInitEntry (tupdesc,  4, "class",     TEXTOID, -1, 0);
		TupleDescInitEntry (tupdesc,  5, "separator", TEXTOID, -1, 0);
		TupleDescInitEntry (tupdesc,  6, "mod",       TEXTOID, -1, 0);

		attinmeta = TupleDescGetAttInMetadata (tupdesc);
		funcctx -> attinmeta = attinmeta;

		MemoryContextSwitchTo (oldcontext);
	}


	funcctx = SRF_PERCALL_SETUP ();
	fctx = (tokensFctx*) funcctx->user_fctx;

	while (fctx->cnode < fctx->nnodes)
	{
		char **values;
		Datum result;
		HeapTuple tuple;
		char *back_vals[6];
		errno_t ret;

		orafce_lexnode *nd = (orafce_lexnode*) list_nth(fctx->nodes, fctx->cnode++);
		values = fctx->values;

		back_vals[2] = values[2];
		back_vals[4] = values[4];
		back_vals[5] = values[5];

		ret = snprintf_s(values[0],    16,    16, "%d", nd->lloc);
		securec_check_ss(ret, "", "");
		ret = snprintf_s(values[1], 10000, 10000, "%s", SF(nd->str));
		securec_check_ss(ret, "", "");
		ret = snprintf_s(values[2],    16,    16, "%d", nd->keycode);
		securec_check_ss(ret, "", "");
		ret = snprintf_s(values[3],    16,    16, "%s", nd->classname);
		securec_check_ss(ret, "", "");
		ret = snprintf_s(values[4],   255,   255, "%s", SF(nd->sep));
		securec_check_ss(ret, "", "");
		ret = snprintf_s(values[5],    48,    48, "%s", SF(nd->modificator));
		securec_check_ss(ret, "", "");

		if (nd->keycode == -1)
			values[2] = NULL;

		if (!nd->sep)
			values[4] = NULL;

		if (!nd->modificator)
			values[5] = NULL;

		tuple = BuildTupleFromCStrings(funcctx->attinmeta, fctx->values);
		result = HeapTupleGetDatum(tuple);

		values[2] = back_vals[2];
		values[4] = back_vals[4];
		values[5] = back_vals[5];

		SRF_RETURN_NEXT (funcctx, result);
	}

	SRF_RETURN_DONE (funcctx);
}
