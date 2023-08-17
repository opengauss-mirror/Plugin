/*----------------------------------------------------------------------------
 *
 *     varchar2.c
 *     VARCHAR2 type for PostgreSQL.
 *
 *----------------------------------------------------------------------------
 */

#include "postgres.h"
#include "access/hash.h"
#include "libpq/pqformat.h"
#include "nodes/nodeFuncs.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "fmgr.h"

#include "plugin_postgres.h"
#include "plugin_orafce/builtins.h"

PG_FUNCTION_INFO_V1_PUBLIC(varchar2);
PG_FUNCTION_INFO_V1_PUBLIC(orafce_concat2);
PG_FUNCTION_INFO_V1_PUBLIC(orafce_varchar_transform);

/*
 * varchar2_transform()
 * Flatten calls to varchar's length coercion function that set the new maximum
 * length >= the previous maximum length.  We can ignore the isExplicit
 * argument, since that only affects truncation cases.
 *
 * just use varchar_transform()
 */

Datum orafce_varchar_transform(PG_FUNCTION_ARGS)
{
#if PG_VERSION_NUM < 120000

    return varchar_transform(fcinfo);

#else

    return varchar_support(fcinfo);

#endif
}

/*
 * Converts a VARCHAR2 type to the specified size.
 *
 * maxlen is the typmod, ie, declared length plus VARHDRSZ bytes.
 * isExplicit is true if this is for an explicit cast to varchar2(N).
 *
 * Truncation rules: for an explicit cast, silently truncate to the given
 * length; for an implicit cast, raise error if length limit is exceeded
 */
Datum varchar2(PG_FUNCTION_ARGS)
{
    VarChar *source = PG_GETARG_VARCHAR_PP(0);
    int32 typmod = PG_GETARG_INT32(1);
    bool isExplicit = PG_GETARG_BOOL(2);
    int32 len, maxlen;
    char *s_data;

    len = VARSIZE_ANY_EXHDR(source);
    s_data = VARDATA_ANY(source);
    maxlen = typmod - VARHDRSZ;

    /* No work if typmod is invalid or supplied data fits it already */
    if (maxlen < 0 || len <= maxlen)
        PG_RETURN_VARCHAR_P(source);

    /* error out if value too long unless it's an explicit cast */
    if (!isExplicit) {
        if (len > maxlen)
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("input value length is %d; too long for type varchar2(%d)", len, maxlen)));
    }

    PG_RETURN_VARCHAR_P((VarChar *)cstring_to_text_with_len(s_data, maxlen));
}

/*
 * varchar2typmodin -- type modifier input function
 *
 * just use varchartypmodin()
 */

/*
 * varchar2typmodout -- type modifier output function
 *
 * just use varchartypmodout()
 */

/*
 * orafce_concat2 - null safe concat
 *
 * returns NULL instead empty string
 */
Datum orafce_concat2(PG_FUNCTION_ARGS)
{
    text *arg1 = NULL, *arg2 = NULL, *result;
    int32 len1 = 0, len2 = 0, len;
    char *ptr;
    errno_t sret;

    if (!PG_ARGISNULL(0)) {
        arg1 = PG_GETARG_TEXT_PP(0);
        len1 = VARSIZE_ANY_EXHDR(arg1);
    }
    if (!PG_ARGISNULL(1)) {
        arg2 = PG_GETARG_TEXT_PP(1);
        len2 = VARSIZE_ANY_EXHDR(arg2);
    }

    /* default behave should be compatible with Postgres */
    if (!(GetSessionContext()->orafce_varchar2_null_safe_concat)) {
        if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
            PG_RETURN_NULL();
    } else {
        if (len1 == 0 && len2 == 0)
            PG_RETURN_NULL();
    }

    /* hard work, we should to concat strings */
    len = len1 + len2 + VARHDRSZ;

    result = (text *)palloc(len);
    SET_VARSIZE(result, len);

    ptr = VARDATA(result);

    if (len1 > 0) {
        sret = memcpy_s(ptr, len1, VARDATA_ANY(arg1), len1);
        securec_check(sret, "", "");
    }
    if (len2 > 0) {
        sret = memcpy_s(ptr + len1, len2, VARDATA_ANY(arg2), len2);
        securec_check(sret, "", "");
    }

    PG_RETURN_TEXT_P(result);
}
