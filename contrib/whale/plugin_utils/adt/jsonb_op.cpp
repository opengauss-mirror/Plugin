/* -------------------------------------------------------------------------
 *
 * jsonb_op.cpp
 *   Special operators for jsonb only, used by various index access methods
 *
 * Portions Copyright (c) 2021 Huawei Technologies Co.,Ltd.
 * Copyright (c) 2014, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *    src/common/backend/utils/adt/jsonb_op.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"

#include "miscadmin.h"
#include "utils/jsonb.h"

Datum jsonb_exists(PG_FUNCTION_ARGS)
{
    Jsonb      *jb = PG_GETARG_JSONB(0);
    text       *key = PG_GETARG_TEXT_PP(1);
    JsonbValue  kval;
    JsonbValue *v = NULL;

    /*
     * We only match Object keys (which are naturally always Strings), or
     * string elements in arrays.  In particular, we do not match non-string
     * scalar elements.  Existence of a key/element is only considered at the
     * top level.  No recursion occurs.
     */
    kval.type = jbvString;
    kval.string.val = VARDATA_ANY(key);
    kval.string.len = VARSIZE_ANY_EXHDR(key);

    v = findJsonbValueFromSuperHeader(VARDATA(jb), JB_FOBJECT | JB_FARRAY, NULL, &kval);

    PG_RETURN_BOOL(v != NULL);
}

Datum jsonb_exists_any(PG_FUNCTION_ARGS)
{
    Jsonb      *jb = PG_GETARG_JSONB(0);
    ArrayType  *keys = PG_GETARG_ARRAYTYPE_P(1);
    JsonbValue *arrKey = arrayToJsonbSortedArray(keys);
    uint32     *plowbound = NULL;
    uint32      lowbound = 0;
    int         i;

    if (arrKey == NULL || arrKey->object.nPairs == 0) {
        PG_RETURN_BOOL(false);
    }

    if (JB_ROOT_IS_OBJECT(jb)) {
        plowbound = &lowbound;
    }

    /*
     * We exploit the fact that the pairs list is already sorted into strictly
     * increasing order to narrow the findJsonbValueFromSuperHeader search;
     * each search can start one entry past the previous "found" entry, or at
     * the lower bound of the last search.
     */
    for (i = 0; i < arrKey->array.nElems; i++) {
        if (findJsonbValueFromSuperHeader(VARDATA(jb), JB_FOBJECT | JB_FARRAY,
                                          plowbound, arrKey->array.elems + i) != NULL) {
            PG_RETURN_BOOL(true);
        }
    }

    PG_RETURN_BOOL(false);
}

Datum jsonb_exists_all(PG_FUNCTION_ARGS)
{
    Jsonb      *jb = PG_GETARG_JSONB(0);
    ArrayType  *keys = PG_GETARG_ARRAYTYPE_P(1);
    JsonbValue *arrKey = arrayToJsonbSortedArray(keys);
    uint32     *plowbound = NULL;
    uint32      lowbound = 0;
    int         i;

    if (arrKey == NULL || arrKey->array.nElems == 0) {
        PG_RETURN_BOOL(true);
    }

    if (JB_ROOT_IS_OBJECT(jb)) {
        plowbound = &lowbound;
    }

    /*
     * We exploit the fact that the pairs list is already sorted into strictly
     * increasing order to narrow the findJsonbValueFromSuperHeader search;
     * each search can start one entry past the previous "found" entry, or at
     * the lower bound of the last search.
     */
    for (i = 0; i < arrKey->array.nElems; i++) {
        if (findJsonbValueFromSuperHeader(VARDATA(jb), JB_FOBJECT | JB_FARRAY,
                                          plowbound, arrKey->array.elems + i) == NULL) {
            PG_RETURN_BOOL(false);
        }
    }

    PG_RETURN_BOOL(true);
}

Datum jsonb_contains(PG_FUNCTION_ARGS)
{
    Jsonb *val = PG_GETARG_JSONB(0);
    Jsonb *tmpl = PG_GETARG_JSONB(1);

    JsonbIterator *it1 = NULL;
    JsonbIterator *it2 = NULL;

    if (JB_ROOT_IS_OBJECT(val) != JB_ROOT_IS_OBJECT(tmpl)) {
        PG_RETURN_BOOL(false);
    }

    it1 = JsonbIteratorInit(VARDATA(val));
    it2 = JsonbIteratorInit(VARDATA(tmpl));

    PG_RETURN_BOOL(JsonbDeepContains(&it1, &it2));
}

Datum jsonb_contained(PG_FUNCTION_ARGS)
{
    /* Commutator of "contains" */
    Jsonb *tmpl = PG_GETARG_JSONB(0);
    Jsonb *val = PG_GETARG_JSONB(1);

    JsonbIterator *it1 = NULL;
    JsonbIterator *it2 = NULL;

    if (JB_ROOT_IS_OBJECT(val) != JB_ROOT_IS_OBJECT(tmpl)) {
        PG_RETURN_BOOL(false);
    }

    it1 = JsonbIteratorInit(VARDATA(val));
    it2 = JsonbIteratorInit(VARDATA(tmpl));

    PG_RETURN_BOOL(JsonbDeepContains(&it1, &it2));
}

Datum jsonb_ne(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    bool   res = false;

    res = (compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb)) != 0);

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_BOOL(res);
}

/*
 * B-Tree operator class operators, support function
 */
Datum jsonb_lt(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    bool   res = false;

    res = (compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb)) < 0);

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_BOOL(res);
}

Datum jsonb_gt(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    bool   res = false;

    res = (compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb)) > 0);

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_BOOL(res);
}

Datum jsonb_le(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    bool   res = false;

    res = (compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb)) <= 0);

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_BOOL(res);
}

Datum jsonb_ge(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    bool   res = false;

    res = (compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb)) >= 0);

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_BOOL(res);
}

Datum jsonb_eq(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    bool   res = false;

    res = (compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb)) == 0);

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_BOOL(res);
}

Datum jsonb_cmp(PG_FUNCTION_ARGS)
{
    Jsonb *jba = PG_GETARG_JSONB(0);
    Jsonb *jbb = PG_GETARG_JSONB(1);
    int    res = false;

    res = compareJsonbSuperHeaderValue(VARDATA(jba), VARDATA(jbb));

    PG_FREE_IF_COPY(jba, 0);
    PG_FREE_IF_COPY(jbb, 1);
    PG_RETURN_INT32(res);
}

/*
 * Hash operator class jsonb hashing function
 */
Datum jsonb_hash(PG_FUNCTION_ARGS)
{
    Jsonb      *jb = PG_GETARG_JSONB(0);
    JsonbIterator *it = NULL;
    int32       r;
    JsonbValue  v;
    uint32      hash = 0;

    if (JB_ROOT_COUNT(jb) == 0) {
        PG_RETURN_INT32(0);
    }

    it = JsonbIteratorInit(VARDATA(jb));

    while ((r = JsonbIteratorNext(&it, &v, false)) != WJB_DONE) {
        switch (r) {
            /* Rotation is left to JsonbHashScalarValue() */
            case WJB_BEGIN_ARRAY:
                hash ^= JB_FARRAY;
                break;
            case WJB_BEGIN_OBJECT:
                hash ^= JB_FOBJECT;
                break;
            case WJB_KEY:
            case WJB_VALUE:
            case WJB_ELEM:
                JsonbHashScalarValue(&v, &hash);
                break;
            case WJB_END_ARRAY:
            case WJB_END_OBJECT:
                break;
            default:
                elog(ERROR, "invalid JsonbIteratorNext rc: %d", r);
        }
    }

    PG_FREE_IF_COPY(jb, 0);
    PG_RETURN_INT32(hash);
}
