#include "postgres.h"
#include <stdlib.h>
#include <locale.h>
#include "catalog/pg_operator.h"
#include "catalog/pg_type.h"
#include "fmgr.h"
#include "lib/stringinfo.h"
#include "nodes/nodeFuncs.h"
#include "nodes/pg_list.h"
#include "nodes/primnodes.h"
#include "parser/parse_expr.h"
#include "parser/parse_oper.h"
#include "utils/builtins.h"
#include "utils/datum.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/syscache.h"
#include "plugin_postgres.h"
#include "plugin_orafce/builtins.h"

/*
 * Source code for nlssort is taken from postgresql-nls-string
 * package by Jan Pazdziora
 */

PG_FUNCTION_INFO_V1_PUBLIC(ora_lnnvl);

Datum ora_lnnvl(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_BOOL(true);

    PG_RETURN_BOOL(!PG_GETARG_BOOL(0));
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_nvl);

Datum ora_nvl(PG_FUNCTION_ARGS)
{
    if (!PG_ARGISNULL(0))
        PG_RETURN_DATUM(PG_GETARG_DATUM(0));

    if (!PG_ARGISNULL(1))
        PG_RETURN_DATUM(PG_GETARG_DATUM(1));

    PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_nvl2);

Datum ora_nvl2(PG_FUNCTION_ARGS)
{
    if (!PG_ARGISNULL(0)) {
        if (!PG_ARGISNULL(1))
            PG_RETURN_DATUM(PG_GETARG_DATUM(1));
    } else {
        if (!PG_ARGISNULL(2))
            PG_RETURN_DATUM(PG_GETARG_DATUM(2));
    }
    PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_set_nls_sort);

Datum ora_set_nls_sort(PG_FUNCTION_ARGS)
{
    errno_t sret;
    text *arg = PG_GETARG_TEXT_P(0);

    if (GetSessionContext()->def_locale != NULL) {
        pfree(GetSessionContext()->def_locale);
        GetSessionContext()->def_locale = NULL;
    }

    GetSessionContext()->def_locale = (text *)MemoryContextAlloc(u_sess->self_mem_cxt, VARSIZE(arg));
    sret = memcpy_s(GetSessionContext()->def_locale, VARSIZE(arg), arg, VARSIZE(arg));
    securec_check(sret, "", "");

    PG_RETURN_VOID();
}


PG_FUNCTION_INFO_V1_PUBLIC(ora_decode);

/*
 * decode(lhs, [rhs, ret], ..., [default])
 */
Datum ora_decode(PG_FUNCTION_ARGS)
{
    int nargs;
    int i;
    int retarg;

    /* default value is last arg or NULL. */
    nargs = PG_NARGS();
    if (nargs % 2 == 0) {
        retarg = nargs - 1;
        nargs -= 1;  /* ignore the last argument */
    } else
        retarg = -1; /* NULL */

    if (PG_ARGISNULL(0)) {
        for (i = 1; i < nargs; i += 2) {
            if (PG_ARGISNULL(i)) {
                retarg = i + 1;
                break;
            }
        }
    } else {
        FmgrInfo *eq;
        Oid collation = PG_GET_COLLATION();

        /*
         * On first call, get the input type's operator '=' and save at
         * fn_extra.
         */
        if (fcinfo->flinfo->fn_extra == NULL) {
            MemoryContext oldctx;
            Oid typid = get_fn_expr_argtype(fcinfo->flinfo, 0);
            Oid eqoid = equality_oper_funcid(typid);

            oldctx = MemoryContextSwitchTo(fcinfo->flinfo->fn_mcxt);
            eq = (FmgrInfo *)palloc(sizeof(FmgrInfo));
            fmgr_info(eqoid, eq);
            MemoryContextSwitchTo(oldctx);

            fcinfo->flinfo->fn_extra = eq;
        } else
            eq = (FmgrInfo *)fcinfo->flinfo->fn_extra;

        for (i = 1; i < nargs; i += 2) {
            Datum result;

            if (PG_ARGISNULL(i))
                continue;

            result = FunctionCall2Coll(eq, collation, PG_GETARG_DATUM(0), PG_GETARG_DATUM(i));

            if (DatumGetBool(result)) {
                retarg = i + 1;
                break;
            }
        }
    }

    if (retarg < 0 || PG_ARGISNULL(retarg))
        PG_RETURN_NULL();
    else
        PG_RETURN_DATUM(PG_GETARG_DATUM(retarg));
}

Oid equality_oper_funcid(Oid argtype)
{
    Oid eq;
    get_sort_group_operators(argtype, false, true, false, NULL, &eq, NULL, NULL);
    return get_opcode(eq);
}

/*
 * dump(anyexpr [,format])
 *
 *  the dump function returns a varchar2 value that includes the datatype code,
 *  the length in bytes, and the internal representation of the expression.
 */
PG_FUNCTION_INFO_V1_PUBLIC(orafce_dump);

static void appendDatum(StringInfo str, const void *ptr, size_t length, int format)
{
    if (!PointerIsValid(ptr))
        appendStringInfoChar(str, ':');
    else {
        const unsigned char *s = (const unsigned char *)ptr;
        const char *formatstr;
        size_t i;

        switch (format) {
            case 8:
                formatstr = "%ho";
                break;
            case 10:
                formatstr = "%hu";
                break;
            case 16:
                formatstr = "%hx";
                break;
            case 17:
                formatstr = "%hc";
                break;
            default:
                elog(ERROR, "unknown format");
                formatstr = NULL; /* quite compiler */
        }

        /* append a byte array with the specified format */
        for (i = 0; i < length; i++) {
            if (i > 0)
                appendStringInfoChar(str, ',');

            /* print only ANSI visible chars */
            if (format == 17 && (iscntrl(s[i]) || !isascii(s[i])))
                appendStringInfoChar(str, '?');
            else
                appendStringInfo(str, formatstr, s[i]);
        }
    }
}

Datum orafce_dump(PG_FUNCTION_ARGS)
{
    Oid valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    int16 typlen;
    bool typbyval;
    Size length;
    Datum value;
    int format;
    StringInfoData str;

    if (!OidIsValid(valtype))
        elog(ERROR, "function is called from invalid context");

    if (PG_ARGISNULL(0))
        elog(ERROR, "argument is NULL");

    value = PG_GETARG_DATUM(0);
    format = PG_GETARG_IF_EXISTS(1, INT32, 10);

    get_typlenbyval(valtype, &typlen, &typbyval);
    length = datumGetSize(value, typbyval, typlen);

    initStringInfo(&str);
    appendStringInfo(&str, "Typ=%d Len=%d: ", valtype, (int)length);

    if (!typbyval) {
        appendDatum(&str, DatumGetPointer(value), length, format);
    } else if (length <= 1) {
        char v = DatumGetChar(value);
        appendDatum(&str, &v, sizeof(char), format);
    } else if (length <= 2) {
        int16 v = DatumGetInt16(value);
        appendDatum(&str, &v, sizeof(int16), format);
    } else if (length <= 4) {
        int32 v = DatumGetInt32(value);
        appendDatum(&str, &v, sizeof(int32), format);
    } else {
        int64 v = DatumGetInt64(value);
        appendDatum(&str, &v, sizeof(int64), format);
    }

    PG_RETURN_TEXT_P(cstring_to_text(str.data));
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_get_major_version);

/*
 * Returns current version etc, PostgreSQL 9.6, PostgreSQL 10, ..
 */
Datum ora_get_major_version(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text(PACKAGE_STRING));
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_get_major_version_num);

/*
 * Returns major version number 9.5, 9.6, 10, 11, ..
 */
Datum ora_get_major_version_num(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text(PG_MAJORVERSION));
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_get_full_version_num);

/*
 * Returns version number string - 9.5.1, 10.2, ..
 */
Datum ora_get_full_version_num(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text(PG_VERSION));
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_get_platform);

/*
 * 32bit, 64bit
 */
Datum ora_get_platform(PG_FUNCTION_ARGS)
{
#ifdef USE_FLOAT8_BYVAL
    PG_RETURN_TEXT_P(cstring_to_text("64bit"));
#else
    PG_RETURN_TEXT_P(cstring_to_text("32bit"));
#endif
}

PG_FUNCTION_INFO_V1_PUBLIC(ora_get_status);

/*
 * Production | Debug
 */
Datum ora_get_status(PG_FUNCTION_ARGS)
{
#ifdef USE_ASSERT_CHECKING
    PG_RETURN_TEXT_P(cstring_to_text("Debug"));
#else
    PG_RETURN_TEXT_P(cstring_to_text("Production"));
#endif
}
