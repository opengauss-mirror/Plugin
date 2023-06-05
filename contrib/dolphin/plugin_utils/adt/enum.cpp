/* -------------------------------------------------------------------------
 *
 * enum.c
 *	  I/O functions, operators, aggregates etc for enum types
 *
 * Copyright (c) 2006-2012, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/enum.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "catalog/indexing.h"
#include "catalog/pg_enum.h"
#include "catalog/pg_type.h"
#include "libpq/pqformat.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/snapmgr.h"
#include "utils/syscache.h"
#include "utils/typcache.h"
#ifdef DOLPHIN
#include "plugin_postgres.h"
#include "utils/numeric.h"
#include "plugin_parser/parse_coerce.h"
#include "plugin_utils/date.h"
#include "plugin_utils/int8.h"
#include "plugin_utils/varbit.h"
#endif

static Oid enum_endpoint(Oid enumtypoid, ScanDirection direction);
static ArrayType* enum_range_internal(Oid enumtypoid, Oid lower, Oid upper);

/* Basic I/O support */

Datum enum_in(PG_FUNCTION_ARGS)
{
    char* name = PG_GETARG_CSTRING(0);
    Oid enumtypoid = PG_GETARG_OID(1);
    Oid enumoid;
    HeapTuple tup;

    /* must check length to prevent Assert failure within SearchSysCache */
    if (strlen(name) >= NAMEDATALEN)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input value for enum %s: \"%s\"", format_type_be(enumtypoid), name)));

    tup = SearchSysCache2(ENUMTYPOIDNAME, ObjectIdGetDatum(enumtypoid), CStringGetDatum(name));
    if (!HeapTupleIsValid(tup))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input value for enum %s: \"%s\"", format_type_be(enumtypoid), name)));

    /*
     * This comes from pg_enum.oid and stores system oids in user tables. This
     * oid must be preserved by binary upgrades.
     */
    enumoid = HeapTupleGetOid(tup);

    ReleaseSysCache(tup);

    PG_RETURN_OID(enumoid);
}

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(text_enum);
extern "C" DLL_PUBLIC Datum text_enum(PG_FUNCTION_ARGS);
Datum text_enum(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(enum_in, PointerGetDatum(text_to_cstring(PG_GETARG_TEXT_P(0))), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(bpchar_enum);
extern "C" DLL_PUBLIC Datum bpchar_enum(PG_FUNCTION_ARGS);
Datum bpchar_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(bpcharout, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(varchar_enum);
extern "C" DLL_PUBLIC Datum varchar_enum(PG_FUNCTION_ARGS);
Datum varchar_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(varcharout, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(varlena_enum);
extern "C" DLL_PUBLIC Datum varlena_enum(PG_FUNCTION_ARGS);
Datum varlena_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(textout, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(set_enum);
extern "C" DLL_PUBLIC Datum set_enum(PG_FUNCTION_ARGS);
Datum set_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(set_out, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(date_enum);
Datum date_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(date_out, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(datetime_enum);
extern "C" DLL_PUBLIC Datum datetime_enum(PG_FUNCTION_ARGS);
Datum datetime_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(timestamp_out, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(timestamp_enum);
Datum timestamp_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(timestamptz_out, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(time_enum);
extern "C" DLL_PUBLIC Datum time_enum(PG_FUNCTION_ARGS);
Datum time_enum(PG_FUNCTION_ARGS)
{
    char* tmp = DatumGetCString(DirectFunctionCall1(time_out, PG_GETARG_DATUM(0)));
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    pfree_ext(tmp);
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(int4_enum);
extern "C" DLL_PUBLIC Datum int4_enum(PG_FUNCTION_ARGS);
Datum int4_enum(PG_FUNCTION_ARGS)
{
    int order = PG_GETARG_INT32(0);
    char* tmp = "";
    if (order != 0) {
        tmp = getEnumLableByOrder(PG_GETARG_OID(1), PG_GETARG_INT32(0));
    }
    Datum result = DirectFunctionCall2(enum_in, CStringGetDatum(tmp), PG_GETARG_DATUM(1));
    PG_RETURN_OID(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(int1_enum);
extern "C" DLL_PUBLIC Datum int1_enum(PG_FUNCTION_ARGS);
Datum int1_enum(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(int4_enum, (Datum)PG_GETARG_INT8(0), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(int2_enum);
extern "C" DLL_PUBLIC Datum int2_enum(PG_FUNCTION_ARGS);
Datum int2_enum(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(int4_enum, (Datum)PG_GETARG_INT16(0), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(int8_enum);
extern "C" DLL_PUBLIC Datum int8_enum(PG_FUNCTION_ARGS);
Datum int8_enum(PG_FUNCTION_ARGS)
{
    int64 num = PG_GETARG_INT64(0);
    if (num > INT_MAX || num < INT_MIN) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("enum order out of the enum value size")));
    }
    return DirectFunctionCall2(int4_enum, (Datum)num, PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(uint1_enum);
extern "C" DLL_PUBLIC Datum uint1_enum(PG_FUNCTION_ARGS);
Datum uint1_enum(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(int4_enum, (Datum)PG_GETARG_UINT8(0), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(uint2_enum);
extern "C" DLL_PUBLIC Datum uint2_enum(PG_FUNCTION_ARGS);
Datum uint2_enum(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(int4_enum, (Datum)PG_GETARG_UINT16(0), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(uint4_enum);
extern "C" DLL_PUBLIC Datum uint4_enum(PG_FUNCTION_ARGS);
Datum uint4_enum(PG_FUNCTION_ARGS)
{
    uint32 num = PG_GETARG_UINT32(0);
    if (num > INT_MAX) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("enum order out of the enum value size")));
    }
    return DirectFunctionCall2(int4_enum, (Datum)num, PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(uint8_enum);
extern "C" DLL_PUBLIC Datum uint8_enum(PG_FUNCTION_ARGS);
Datum uint8_enum(PG_FUNCTION_ARGS)
{
    uint64 num = PG_GETARG_UINT64(0);
    if (num > INT_MAX) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("enum order out of the enum value size")));
    }
    return DirectFunctionCall2(int4_enum, (Datum)num, PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(float4_enum);
extern "C" DLL_PUBLIC Datum float4_enum(PG_FUNCTION_ARGS);
Datum float4_enum(PG_FUNCTION_ARGS)
{
    float num = PG_GETARG_FLOAT4(0);
    if (num > (float4)INT_MAX) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("enum order out of the enum value size")));
    }
    Datum val = DirectFunctionCall1(ftoi8_floor, Float4GetDatum(num));
    return DirectFunctionCall2(int4_enum, val, PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(float8_enum);
extern "C" DLL_PUBLIC Datum float8_enum(PG_FUNCTION_ARGS);
Datum float8_enum(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    if (num > (float8)INT_MAX) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("enum order out of the enum value size")));
    }
    Datum val = DirectFunctionCall1(dtoi8_floor, Float8GetDatum(num));
    return DirectFunctionCall2(int4_enum, val, PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(numeric_enum);
extern "C" DLL_PUBLIC Datum numeric_enum(PG_FUNCTION_ARGS);
Datum numeric_enum(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    Datum res = DirectFunctionCall1(numeric_float8, NumericGetDatum(num));
    return DirectFunctionCall2(int4_enum, DirectFunctionCall1(dtoi8_floor, res), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(bit_enum);
extern "C" DLL_PUBLIC Datum bit_enum(PG_FUNCTION_ARGS);
Datum bit_enum(PG_FUNCTION_ARGS)
{
    Datum num = bittoint8(fcinfo);
    return DirectFunctionCall2(int8_enum, num, PG_GETARG_DATUM(1));
}

extern "C" Datum year_int64(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_enum);
extern "C" DLL_PUBLIC Datum year_enum(PG_FUNCTION_ARGS);
Datum year_enum(PG_FUNCTION_ARGS)
{
    Datum num = year_int64(fcinfo);
    return DirectFunctionCall2(int8_enum, num, PG_GETARG_DATUM(1));
}
#endif

Datum enum_out(PG_FUNCTION_ARGS)
{
    Oid enumval = PG_GETARG_OID(0);
    char* result = NULL;
    HeapTuple tup;
    Form_pg_enum en;

    tup = SearchSysCache1(ENUMOID, ObjectIdGetDatum(enumval));
    if (!HeapTupleIsValid(tup))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("invalid internal value for enum: %u", enumval)));
    en = (Form_pg_enum)GETSTRUCT(tup);

    result = pstrdup(NameStr(en->enumlabel));

    ReleaseSysCache(tup);

    PG_RETURN_CSTRING(result);
}

/* Binary I/O support */
Datum enum_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    Oid enumtypoid = PG_GETARG_OID(1);
    Oid enumoid;
    HeapTuple tup;
    char* name = NULL;
    int nbytes;

    /* guard against pre-9.3 misdeclaration of enum_recv */
    if (get_fn_expr_argtype(fcinfo->flinfo, 0) == CSTRINGOID)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid argument for enum_recv")));

    name = pq_getmsgtext(buf, buf->len - buf->cursor, &nbytes);

    /* must check length to prevent Assert failure within SearchSysCache */
    if (strlen(name) >= NAMEDATALEN)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input value for enum %s: \"%s\"", format_type_be(enumtypoid), name)));

    tup = SearchSysCache2(ENUMTYPOIDNAME, ObjectIdGetDatum(enumtypoid), CStringGetDatum(name));
    if (!HeapTupleIsValid(tup))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input value for enum %s: \"%s\"", format_type_be(enumtypoid), name)));

    enumoid = HeapTupleGetOid(tup);

    ReleaseSysCache(tup);

    pfree_ext(name);

    PG_RETURN_OID(enumoid);
}

Datum enum_send(PG_FUNCTION_ARGS)
{
    Oid enumval = PG_GETARG_OID(0);
    StringInfoData buf;
    HeapTuple tup;
    Form_pg_enum en;

    tup = SearchSysCache1(ENUMOID, ObjectIdGetDatum(enumval));
    if (!HeapTupleIsValid(tup))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("invalid internal value for enum: %u", enumval)));
    en = (Form_pg_enum)GETSTRUCT(tup);

    pq_begintypsend(&buf);
    pq_sendtext(&buf, NameStr(en->enumlabel), strlen(NameStr(en->enumlabel)));

    ReleaseSysCache(tup);

    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/* Comparison functions and related */

/*
 * enum_cmp_internal is the common engine for all the visible comparison
 * functions, except for enum_eq and enum_ne which can just check for OID
 * equality directly.
 */
static int enum_cmp_internal(Oid arg1, Oid arg2, FunctionCallInfo fcinfo)
{
    TypeCacheEntry* tcache = NULL;

    /* Equal OIDs are equal no matter what */
    if (arg1 == arg2)
        return 0;

    /* Fast path: even-numbered Oids are known to compare correctly */
    if ((arg1 & 1) == 0 && (arg2 & 1) == 0) {
        if (arg1 < arg2)
            return -1;
        else
            return 1;
    }

    /* Locate the typcache entry for the enum type */
    tcache = (TypeCacheEntry*)fcinfo->flinfo->fn_extra;
    if (tcache == NULL) {
        HeapTuple enum_tup;
        Form_pg_enum en;
        Oid typeoid;

        /* Get the OID of the enum type containing arg1 */
        enum_tup = SearchSysCache1(ENUMOID, ObjectIdGetDatum(arg1));
        if (!HeapTupleIsValid(enum_tup))
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("invalid internal value for enum: %u", arg1)));
        en = (Form_pg_enum)GETSTRUCT(enum_tup);
        typeoid = en->enumtypid;
        ReleaseSysCache(enum_tup);
        /* Now locate and remember the typcache entry */
        tcache = lookup_type_cache(typeoid, 0);
        fcinfo->flinfo->fn_extra = (void*)tcache;
    }

    /* The remaining comparison logic is in typcache.c */
    return compare_values_of_enum(tcache, arg1, arg2);
}

Datum enum_lt(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_BOOL(enum_cmp_internal(a, b, fcinfo) < 0);
}

Datum enum_le(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_BOOL(enum_cmp_internal(a, b, fcinfo) <= 0);
}

Datum enum_eq(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_BOOL(a == b);
}

Datum enum_ne(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_BOOL(a != b);
}

Datum enum_ge(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_BOOL(enum_cmp_internal(a, b, fcinfo) >= 0);
}

Datum enum_gt(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_BOOL(enum_cmp_internal(a, b, fcinfo) > 0);
}

Datum enum_smaller(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_OID((enum_cmp_internal(a, b, fcinfo) < 0) ? a : b);
}

Datum enum_larger(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    PG_RETURN_OID((enum_cmp_internal(a, b, fcinfo) > 0) ? a : b);
}

Datum enum_cmp(PG_FUNCTION_ARGS)
{
    Oid a = PG_GETARG_OID(0);
    Oid b = PG_GETARG_OID(1);

    if (a == b)
        PG_RETURN_INT32(0);
    else if (enum_cmp_internal(a, b, fcinfo) > 0)
        PG_RETURN_INT32(1);
    else
        PG_RETURN_INT32(-1);
}

/* Enum programming support functions */

/*
 * enum_endpoint: common code for enum_first/enum_last
 */
static Oid enum_endpoint(Oid enumtypoid, ScanDirection direction)
{
    Relation enum_rel;
    Relation enum_idx;
    SysScanDesc enum_scan;
    HeapTuple enum_tuple;
    ScanKeyData skey;
    Oid minmax;

    /*
     * Find the first/last enum member using pg_enum_typid_sortorder_index.
     * Note we must not use the syscache, and must use an MVCC snapshot here.
     * See comments for RenumberEnumType in catalog/pg_enum.c for more info.
     */
    ScanKeyInit(&skey, Anum_pg_enum_enumtypid, BTEqualStrategyNumber, F_OIDEQ, ObjectIdGetDatum(enumtypoid));

    enum_rel = heap_open(EnumRelationId, AccessShareLock);
    enum_idx = index_open(EnumTypIdSortOrderIndexId, AccessShareLock);
    enum_scan = systable_beginscan_ordered(enum_rel, enum_idx, GetTransactionSnapshot(), 1, &skey);

    enum_tuple = systable_getnext_ordered(enum_scan, direction);
    if (HeapTupleIsValid(enum_tuple))
        minmax = HeapTupleGetOid(enum_tuple);
    else
        minmax = InvalidOid;

    systable_endscan_ordered(enum_scan);
    index_close(enum_idx, AccessShareLock);
    heap_close(enum_rel, AccessShareLock);

    return minmax;
}

Datum enum_first(PG_FUNCTION_ARGS)
{
    Oid enumtypoid;
    Oid min;

    /*
     * We rely on being able to get the specific enum type from the calling
     * expression tree.  Notice that the actual value of the argument isn't
     * examined at all; in particular it might be NULL.
     */
    enumtypoid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (enumtypoid == InvalidOid)
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("could not determine actual enum type")));

    /* Get the OID using the index */
    min = enum_endpoint(enumtypoid, ForwardScanDirection);

    if (!OidIsValid(min))
        ereport(ERROR,
            (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                errmsg("enum %s contains no values", format_type_be(enumtypoid))));

    PG_RETURN_OID(min);
}

Datum enum_last(PG_FUNCTION_ARGS)
{
    Oid enumtypoid;
    Oid max;

    /*
     * We rely on being able to get the specific enum type from the calling
     * expression tree.  Notice that the actual value of the argument isn't
     * examined at all; in particular it might be NULL.
     */
    enumtypoid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (enumtypoid == InvalidOid)
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("could not determine actual enum type")));

    /* Get the OID using the index */
    max = enum_endpoint(enumtypoid, BackwardScanDirection);

    if (!OidIsValid(max))
        ereport(ERROR,
            (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                errmsg("enum %s contains no values", format_type_be(enumtypoid))));

    PG_RETURN_OID(max);
}

/* 2-argument variant of enum_range */
Datum enum_range_bounds(PG_FUNCTION_ARGS)
{
    Oid lower;
    Oid upper;
    Oid enumtypoid;

    if (PG_ARGISNULL(0))
        lower = InvalidOid;
    else
        lower = PG_GETARG_OID(0);
    if (PG_ARGISNULL(1))
        upper = InvalidOid;
    else
        upper = PG_GETARG_OID(1);

    /*
     * We rely on being able to get the specific enum type from the calling
     * expression tree.  The generic type mechanism should have ensured that
     * both are of the same type.
     */
    enumtypoid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (enumtypoid == InvalidOid)
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("could not determine actual enum type")));

    PG_RETURN_ARRAYTYPE_P(enum_range_internal(enumtypoid, lower, upper));
}

/* 1-argument variant of enum_range */
Datum enum_range_all(PG_FUNCTION_ARGS)
{
    Oid enumtypoid;

    /*
     * We rely on being able to get the specific enum type from the calling
     * expression tree.  Notice that the actual value of the argument isn't
     * examined at all; in particular it might be NULL.
     */
    enumtypoid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (enumtypoid == InvalidOid)
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("could not determine actual enum type")));

    PG_RETURN_ARRAYTYPE_P(enum_range_internal(enumtypoid, InvalidOid, InvalidOid));
}

static ArrayType* enum_range_internal(Oid enumtypoid, Oid lower, Oid upper)
{
    ArrayType* result = NULL;
    Relation enum_rel;
    Relation enum_idx;
    SysScanDesc enum_scan;
    HeapTuple enum_tuple;
    ScanKeyData skey;
    Datum* elems = NULL;
    int max, cnt;
    bool left_found = false;

    /*
     * Scan the enum members in order using pg_enum_typid_sortorder_index.
     * Note we must not use the syscache, and must use an MVCC snapshot here.
     * See comments for RenumberEnumType in catalog/pg_enum.c for more info.
     */
    ScanKeyInit(&skey, Anum_pg_enum_enumtypid, BTEqualStrategyNumber, F_OIDEQ, ObjectIdGetDatum(enumtypoid));

    enum_rel = heap_open(EnumRelationId, AccessShareLock);
    enum_idx = index_open(EnumTypIdSortOrderIndexId, AccessShareLock);
    enum_scan = systable_beginscan_ordered(enum_rel, enum_idx, GetTransactionSnapshot(), 1, &skey);

    max = 64;
    elems = (Datum*)palloc(max * sizeof(Datum));
    cnt = 0;
    left_found = !OidIsValid(lower);

    while (HeapTupleIsValid(enum_tuple = systable_getnext_ordered(enum_scan, ForwardScanDirection))) {
        Oid enum_oid = HeapTupleGetOid(enum_tuple);

        if (!left_found && lower == enum_oid)
            left_found = true;

        if (left_found) {
            if (cnt >= max) {
                max *= 2;
                elems = (Datum*)repalloc(elems, max * sizeof(Datum));
            }

            elems[cnt++] = ObjectIdGetDatum(enum_oid);
        }

        if (OidIsValid(upper) && upper == enum_oid)
            break;
    }

    systable_endscan_ordered(enum_scan);
    index_close(enum_idx, AccessShareLock);
    heap_close(enum_rel, AccessShareLock);

    /* and build the result array */
    /* note this hardwires some details about the representation of Oid */
    result = construct_array(elems, cnt, enumtypoid, sizeof(Oid), true, 'i');

    pfree_ext(elems);

    return result;
}

#ifdef DOLPHIN
/* GetEnumDefineStr
 * output the 'enum(\'a\', \'b\', \'c\')' type text by enum type oid
 */
Datum GetEnumDefineStr(Oid enumOid)
{
    HeapTuple enumTup = nullptr;
    Form_pg_enum item = nullptr;
    text* result = nullptr;
    StringInfoData buf;
    initStringInfo(&buf);

    Relation enumRel = heap_open(EnumRelationId, AccessShareLock);
    CatCList* items = SearchSysCacheList1(ENUMTYPOIDNAME, ObjectIdGetDatum(enumOid));
    const int itemCnt = items->n_members;

    if (itemCnt == 0) {
        appendStringInfoString(&buf, "enum()");
    } else {
        char** labels = (char**)palloc0(sizeof(char*) * itemCnt);

        for (int i = 0; i < itemCnt; ++i) {
            enumTup = t_thrd.lsc_cxt.FetchTupleFromCatCList(items, i);
            item = (Form_pg_enum)GETSTRUCT(enumTup);
            labels[(int)item->enumsortorder -1] = pstrdup(NameStr(item->enumlabel));
        }

        appendStringInfo(&buf, "enum(\'%s\'", labels[0]);
        pfree(labels[0]);
        for (int i = 1; i < itemCnt; ++i) {
            appendStringInfo(&buf, ", \'%s\'", labels[i]);
            pfree(labels[i]);
        }
        appendStringInfoString(&buf, ")");
        pfree(labels);
    }

    result = cstring_to_text_with_len(buf.data, buf.len);

    FreeStringInfo(&buf);
    ReleaseSysCacheList(items);
    heap_close(enumRel, AccessShareLock);

    PG_RETURN_TEXT_P(result);
}

bool IsAnonymousEnum(Oid oid)
{
    if (!type_is_enum(oid)) {
        return false;
    }

    HeapTuple enumTup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(oid));
    if (!HeapTupleIsValid(enumTup)) {
        ereport(ERROR, (errcode(ERRCODE_CACHE_LOOKUP_FAILED),
                errmsg("cache lookup failed for type %u", oid)));
    }

    Form_pg_type enumForm = (Form_pg_type)GETSTRUCT(enumTup);
    char* enumName = NameStr(enumForm->typname);
    bool isAnonymous = strstr(enumName, "anonymous_enum");
    ReleaseSysCache(enumTup);
    return isAnonymous;
}

PG_FUNCTION_INFO_V1_PUBLIC(Enum2Float8);
extern "C" DLL_PUBLIC Datum Enum2Float8(PG_FUNCTION_ARGS);
Datum Enum2Float8(PG_FUNCTION_ARGS)
{
    Oid enumval = PG_GETARG_OID(0);
    double result = 0.0;
    HeapTuple tup;
    Form_pg_enum en;

    tup = SearchSysCache1(ENUMOID, ObjectIdGetDatum(enumval));
    if (!HeapTupleIsValid(tup))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("invalid internal value for enum: %u", enumval)));
    en = (Form_pg_enum)GETSTRUCT(tup);

    result = en->enumsortorder;

    ReleaseSysCache(tup);

    PG_RETURN_FLOAT8(result);
}

extern "C" Datum bitfromfloat8(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(Enum2Bit);
extern "C" DLL_PUBLIC Datum Enum2Bit(PG_FUNCTION_ARGS);
Datum Enum2Bit(PG_FUNCTION_ARGS)
{
    Datum num = Enum2Float8(fcinfo);
    int32 typmod = PG_GETARG_INT32(1);
    return DirectFunctionCall2(bitfromfloat8, num, Int32GetDatum(typmod));
}

Datum enumtotext(PG_FUNCTION_ARGS)
{
    Datum enumLabels = DirectFunctionCall1(enum_out, PG_GETARG_DATUM(0));
    return DirectFunctionCall1(textin, PointerGetDatum(enumLabels));
}

/* enum compare with text */
PG_FUNCTION_INFO_V1_PUBLIC(enumtexteq);
extern "C" DLL_PUBLIC Datum enumtexteq(PG_FUNCTION_ARGS);
Datum enumtexteq(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(0));
    return DirectFunctionCall2(texteq, PointerGetDatum(enumDatum), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(enumtextne);
extern "C" DLL_PUBLIC Datum enumtextne(PG_FUNCTION_ARGS);
Datum enumtextne(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(0));
    return DirectFunctionCall2(textne, PointerGetDatum(enumDatum), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(enumtextgt);
extern "C" DLL_PUBLIC Datum enumtextgt(PG_FUNCTION_ARGS);
Datum enumtextgt(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(0));
    return DirectFunctionCall2Coll(text_gt, PG_GET_COLLATION(), PointerGetDatum(enumDatum), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(enumtextge);
extern "C" DLL_PUBLIC Datum enumtextge(PG_FUNCTION_ARGS);
Datum enumtextge(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(0));
    return DirectFunctionCall2Coll(text_ge, PG_GET_COLLATION(), PointerGetDatum(enumDatum), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(enumtextlt);
extern "C" DLL_PUBLIC Datum enumtextlt(PG_FUNCTION_ARGS);
Datum enumtextlt(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(0));
    return DirectFunctionCall2Coll(text_lt, PG_GET_COLLATION(), PointerGetDatum(enumDatum), PG_GETARG_DATUM(1));
}

PG_FUNCTION_INFO_V1_PUBLIC(enumtextle);
extern "C" DLL_PUBLIC Datum enumtextle(PG_FUNCTION_ARGS);
Datum enumtextle(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(0));
    return DirectFunctionCall2Coll(text_le, PG_GET_COLLATION(), PointerGetDatum(enumDatum), PG_GETARG_DATUM(1));
}

/* text compare with enum */
PG_FUNCTION_INFO_V1_PUBLIC(textenumeq);
extern "C" DLL_PUBLIC Datum textenumeq(PG_FUNCTION_ARGS);
Datum textenumeq(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(1));
    return DirectFunctionCall2(texteq, PG_GETARG_DATUM(0), PointerGetDatum(enumDatum));
}

PG_FUNCTION_INFO_V1_PUBLIC(textenumne);
extern "C" DLL_PUBLIC Datum textenumne(PG_FUNCTION_ARGS);
Datum textenumne(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(1));
    return DirectFunctionCall2(textne, PG_GETARG_DATUM(0), PointerGetDatum(enumDatum));
}

PG_FUNCTION_INFO_V1_PUBLIC(textenumgt);
extern "C" DLL_PUBLIC Datum textenumgt(PG_FUNCTION_ARGS);
Datum textenumgt(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(1));
    return DirectFunctionCall2Coll(text_gt, PG_GET_COLLATION(), PG_GETARG_DATUM(0), PointerGetDatum(enumDatum));
}

PG_FUNCTION_INFO_V1_PUBLIC(textenumge);
extern "C" DLL_PUBLIC Datum textenumge(PG_FUNCTION_ARGS);
Datum textenumge(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(1));
    return DirectFunctionCall2Coll(text_ge, PG_GET_COLLATION(), PG_GETARG_DATUM(0), PointerGetDatum(enumDatum));
}

PG_FUNCTION_INFO_V1_PUBLIC(textenumlt);
extern "C" DLL_PUBLIC Datum textenumlt(PG_FUNCTION_ARGS);
Datum textenumlt(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(1));
    return DirectFunctionCall2Coll(text_lt, PG_GET_COLLATION(), PG_GETARG_DATUM(0), PointerGetDatum(enumDatum));
}

PG_FUNCTION_INFO_V1_PUBLIC(textenumle);
extern "C" DLL_PUBLIC Datum textenumle(PG_FUNCTION_ARGS);
Datum textenumle(PG_FUNCTION_ARGS)
{
    Datum enumDatum = DirectFunctionCall1(enumtotext, PG_GETARG_DATUM(1));
    return DirectFunctionCall2Coll(text_le, PG_GET_COLLATION(), PG_GETARG_DATUM(0), PointerGetDatum(enumDatum));
}


#endif
