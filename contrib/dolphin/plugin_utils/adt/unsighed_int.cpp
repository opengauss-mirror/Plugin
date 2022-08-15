/* -------------------------------------------------------------------------
 *
 * varlena.c
 * 	  Functions for the variable-length built-in types.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 * Portions Copyright (c) 2021, openGauss Contributors
 *
 *
 * IDENTIFICATION
 * 	  src/backend/utils/adt/varlena.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include <limits.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>

#include "access/hash.h"
#include "access/tuptoaster.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_type.h"
#include "common/int.h"
#include "lib/hyperloglog.h"
#include "libpq/md5.h"
#include "libpq/pqformat.h"
#include "miscadmin.h"
#include "plugin_parser/scansup.h"
#include "port/pg_bswap.h"
#include "regex/regex.h"
#include "utils/builtins.h"
#include "utils/bytea.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/numeric.h"
#include "utils/pg_locale.h"
#include "plugin_parser/parser.h"
#include "utils/int8.h"
#include "utils/sortsupport.h"
#include "executor/node/nodeSort.h"
#include "plugin_utils/my_locale.h"
#include "pgxc/groupmgr.h"
#include "plugin_postgres.h"
#include "parser/parse_coerce.h"
#include "catalog/pg_type.h"
#include "workload/cpwlm.h"
#include "utils/varbit.h"
#include "plugin_commands/mysqlmode.h"
#include "plugin_utils/unsigned_int.h"

#define INTEGER_DIV_INTEGER(arg1, arg2)   \
    float8 result;                        \
    if (arg2 == 0) {                      \
        PG_RETURN_NULL();                 \
    }                                     \
    result = (arg1 * 1.0) / (arg2 * 1.0); \
    PG_RETURN_FLOAT8(result)

#define UINT64_LENGTH 64

Datum uint1in(PG_FUNCTION_ARGS)
{
    char *num = PG_GETARG_CSTRING(0);

    PG_RETURN_UINT8((uint8)pg_atoui(num, sizeof(uint8), '\0'));
}

/*
 * 		uint1out			- converts uint8 to "num"
 */
Datum uint1out(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    char *result = (char *)palloc(7); /* sign, 5 digits, '\0' */

    pg_ltoa((int32)arg1, result);
    PG_RETURN_CSTRING(result);
}

/*
 * 		uint1recv			- converts external binary format to uint8
 */
Datum uint1recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    PG_RETURN_UINT8((uint8)pq_getmsgint(buf, sizeof(uint8)));
}

/*
 * 		int1send			- converts uint8 to binary format
 */
Datum uint1send(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendint(&buf, arg1, sizeof(uint8));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum uint1and(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 & arg2);
}

Datum uint1or(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 | arg2);
}

Datum uint1xor(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 ^ arg2);
}

Datum uint1not(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_UINT8((uint8)(~arg1));
}

Datum uint1shl(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT8(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT64(0);
    }
    PG_RETURN_UINT64((uint64)(arg1 << arg2));
}

Datum uint1shr(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT8(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 > UINT64_LENGTH) {
        PG_RETURN_UINT8(0);
    }
    PG_RETURN_UINT8(arg1 >> arg2);
}

/*
 * adapt dophin, add a new data type uint1
 * 		uint1eq			- returns 1 if arg1 == arg2
 * 		uint1ne			- returns 1 if arg1 != arg2
 * 		uint1lt			- returns 1 if arg1 < arg2
 * 		uint1le			- returns 1 if arg1 <= arg2
 * 		uint1gt			- returns 1 if arg1 > arg2
 * 		uint1ge			- returns 1 if arg1 >= arg2
 */
Datum uint1eq(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum uint1ne(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum uint1lt(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum uint1le(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum uint1gt(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum uint1ge(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint1cmp(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    if (arg1 > arg2)
        PG_RETURN_INT32(1);
    else if (arg1 == arg2)
        PG_RETURN_INT32(0);
    else
        PG_RETURN_INT32(-1);
}

/*
 * 		===================
 * 		CONVERSION ROUTINES
 * 		===================
 */
Datum ui1toi2(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_INT16((int16)arg1);
}

Datum i2toui1(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);

    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }

    PG_RETURN_UINT8((uint8)arg1);
}

Datum ui1toi4(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_INT32((int32)arg1);
}

Datum i4toui1(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);

    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum ui1toi8(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_INT64((int64)arg1);
}

Datum i8toui1(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);

    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }

    PG_RETURN_UINT8((uint8)arg1);
}

Datum ui1tof4(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_FLOAT4((float4)arg1);
}

Datum ui1tof8(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_FLOAT8((float8)arg1);
}

Datum f4toui1(PG_FUNCTION_ARGS)
{
    float4 arg1 = PG_GETARG_FLOAT4(0);
    if (isnan(arg1))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned of range")));
    if (fcinfo->can_ignore && (arg1 < 0 || arg1 > UCHAR_MAX)) {
        ereport(WARNING, (errmsg("tinyint unsigned out of range")));
        PG_RETURN_UINT8(arg1 < 0 ? 0 : UCHAR_MAX);
    }
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)round(arg1));
}

Datum f8toui1(PG_FUNCTION_ARGS)
{
    float8 arg1 = PG_GETARG_FLOAT8(0);
    if (isnan(arg1))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsignedout of range")));
    if (fcinfo->can_ignore && (arg1 < 0 || arg1 > UCHAR_MAX)) {
        ereport(WARNING, (errmsg("tinyint unsigned out of range")));
        PG_RETURN_UINT8(arg1 < 0 ? 0 : UCHAR_MAX);
    }
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsignedout of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)round(arg1));
}

/* OPERATE INT1 */
Datum uint1um(PG_FUNCTION_ARGS)
{
    uint16 result = PG_GETARG_UINT8(0);

    PG_RETURN_INT16(0 - result);
}

Datum uint1up(PG_FUNCTION_ARGS)
{
    uint8 arg = PG_GETARG_UINT8(0);

    PG_RETURN_UINT8(arg);
}

Datum uint1pl(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    uint16 result;

    result = (uint16)arg1 + (uint16)arg2;
    if (result > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }
    PG_RETURN_UINT8(result);
}

Datum uint1mi(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    uint8 result;

    if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    result = arg1 - arg2;

    PG_RETURN_UINT8(result);
}

Datum uint1mul(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    uint16 result16;

    result16 = (uint16)arg1 * (uint16)arg2;
    if (result16 > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }
    PG_RETURN_UINT16(result16);
}

Datum uint1div(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint1abs(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_UINT8(arg1);
}

Datum uint1mod(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT8(arg1 % arg2);
}

Datum uint1larger(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8((arg1 > arg2) ? arg1 : arg2);
}

Datum uint1smaller(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8((arg1 < arg2) ? arg1 : arg2);
}

Datum hashuint1(PG_FUNCTION_ARGS)
{
    return hash_uint32((int32)PG_GETARG_UINT8(0));
}

Datum hashuint2(PG_FUNCTION_ARGS)
{
    return hash_uint32((int32)PG_GETARG_UINT16(0));
}

Datum hashuint4(PG_FUNCTION_ARGS)
{
    return hash_uint32(PG_GETARG_UINT32(0));
}

Datum hashuint8(PG_FUNCTION_ARGS)
{
    uint64 val = PG_GETARG_UINT64(0);
    uint32 lohalf = (uint32)val;
    uint32 hihalf = (uint32)(val >> 32);

    lohalf ^= hihalf;

    return hash_uint32(lohalf);
}

Datum uint2in(PG_FUNCTION_ARGS)
{
    char *num = PG_GETARG_CSTRING(0);

    PG_RETURN_UINT16((uint16)pg_atoui(num, sizeof(uint16), '\0'));
}

Datum uint2out(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    char *result = (char *)palloc(12); /* sign, 10 digits, '\0' */
    pg_lltoa((int64)arg1, result);
    PG_RETURN_CSTRING(result);
}

Datum uint2recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    PG_RETURN_UINT16((uint16)pq_getmsgint(buf, sizeof(uint16)));
}

Datum uint2send(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendint(&buf, arg1, sizeof(uint16));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum uint2and(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 & arg2);
}

Datum uint2or(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 | arg2);
}

Datum uint2xor(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 ^ arg2);
}

Datum uint2not(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_UINT16((uint16)(~arg1));
}

Datum uint2shl(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT16(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT64(0);
    }
    PG_RETURN_UINT64((uint64)(arg1 << arg2));
}

Datum uint2shr(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT16(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT16(0);
    }
    PG_RETURN_UINT16(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint2
 * 		uint2eq			- returns 1 if arg1 == arg2
 * 		uint2ne			- returns 1 if arg1 != arg2
 * 		uint2lt			- returns 1 if arg1 < arg2
 * 		uint2le			- returns 1 if arg1 <= arg2
 * 		uint2gt			- returns 1 if arg1 > arg2
 * 		uint2ge			- returns 1 if arg1 >= arg2
 */
Datum uint2eq(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum uint2ne(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum uint2lt(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum uint2le(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum uint2gt(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum uint2ge(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint2cmp(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    if (arg1 > arg2)
        PG_RETURN_INT32(1);
    else if (arg1 == arg2)
        PG_RETURN_INT32(0);
    else
        PG_RETURN_INT32(-1);
}

/*
 * 		===================
 * 		CONVERSION ROUTINES
 * 		===================
 */
Datum ui1toui2(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui2toui1(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum ui2toi2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > SHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint out of range")));
    } else if (arg1 > SHRT_MAX) {
        arg1 = SHRT_MAX;
    }
    PG_RETURN_INT16((int16)arg1);
}

Datum i2toui2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }
    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui2toi4(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_INT32((int32)arg1);
}

Datum i4toui2(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }

    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui2toi8(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_INT64((int64)arg1);
}

Datum i8toui2(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }

    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui2tof4(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_FLOAT4((float4)arg1);
}

Datum ui2tof8(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_FLOAT8((float8)arg1);
}

Datum f4toui2(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);

    num = round(num);
    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));

    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && (num < (float4)0 || num > ((float4)USHRT_MAX))) {
        ereport(WARNING, (errmsg("smallint unsigned out of range")));
        PG_RETURN_UINT16(num < (float8)0 ? 0 : USHRT_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if (num < (float4)0 || num > ((float4)USHRT_MAX))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (num < (float4)0) {
        PG_RETURN_UINT16(0);
    } else if (num > ((float4)USHRT_MAX)) {
        PG_RETURN_UINT16(USHRT_MAX);
    }

    PG_RETURN_UINT16((uint16)num);
}

Datum f8toui2(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    num = round(num);

    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));

    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && (num < (float8)0 || num > ((float8)USHRT_MAX))) {
        ereport(WARNING, (errmsg("smallint unsigned out of range")));
        PG_RETURN_UINT16(num < (float8)0 ? 0 : USHRT_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if (num < (float8)0 || num >= ((float8)USHRT_MAX))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (num < (float8)0) {
        PG_RETURN_UINT16(0);
    } else if (num > ((float8)USHRT_MAX)) {
        PG_RETURN_UINT16(USHRT_MAX);
    }

    PG_RETURN_UINT16((uint16)num);
}

Datum uint2um(PG_FUNCTION_ARGS)
{
    int32 arg = PG_GETARG_UINT16(0);
    int32 result;

    result = -arg;

    PG_RETURN_INT32(result);
}

Datum uint2up(PG_FUNCTION_ARGS)
{
    uint16 arg = PG_GETARG_UINT16(0);

    PG_RETURN_UINT16(arg);
}

Datum uint2pl(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    uint32 result;

    result = (uint32)arg1 + (uint32)arg2;
    if (result > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }

    PG_RETURN_UINT32(result);
}

Datum uint2mi(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    uint16 result;
    if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }
    result = arg1 - arg2;

    PG_RETURN_UINT16(result);
}

Datum uint2mul(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    uint32 result32;

    result32 = (uint32)arg1 * (uint32)arg2;
    if (result32 > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }
    PG_RETURN_UINT32(result32);
}

Datum uint2div(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint2abs(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_UINT16(arg1);
}

Datum uint2mod(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT16(arg1 % arg2);
}

Datum uint2larger(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16((arg1 > arg2) ? arg1 : arg2);
}

Datum uint2smaller(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16((arg1 < arg2) ? arg1 : arg2);
}

void pg_ulltoa(uint64 value, char *a)
{
    char *start = a;

    /* Compute the result string backwards. */
    do {
        uint64 remainder;
        uint64 oldval = value;

        value /= 10;
        remainder = oldval - value * 10;
        *a++ = '0' + remainder;
    } while (value != 0);

    /* Add trailing NUL byte, and back up 'a' to the last character. */
    *a-- = '\0';

    /* Reverse string. */
    while (start < a) {
        char swap = *start;

        *start++ = *a;
        *a-- = swap;
    }
}

Datum uint4in(PG_FUNCTION_ARGS)
{
    char *num = PG_GETARG_CSTRING(0);

    PG_RETURN_UINT32((uint32)pg_atoui(num, sizeof(uint32), '\0'));
}

Datum uint4out(PG_FUNCTION_ARGS)
{
    uint64 val = PG_GETARG_UINT64(0);
    char buf[MAXINT8LEN + 1];
    char *result;

    pg_ulltoa(val, buf);
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

Datum uint4recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    PG_RETURN_UINT32((uint32)pq_getmsgint(buf, sizeof(uint32)));
}

Datum uint4send(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendint(&buf, arg1, sizeof(uint32));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum uint4and(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 & arg2);
}

Datum uint4or(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 | arg2);
}

Datum uint4xor(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 ^ arg2);
}

Datum uint4not(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_UINT32((uint32)(~arg1));
}

Datum uint4shl(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT64(0);
    }
    PG_RETURN_UINT64((uint64)(arg1 << arg2));
}

Datum uint4shr(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT32(0);
    }
    PG_RETURN_UINT32(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint4
 * 		uint4eq			- returns 1 if arg1 == arg2
 * 		uint4ne			- returns 1 if arg1 != arg2
 * 		uint4lt			- returns 1 if arg1 < arg2
 * 		uint4le			- returns 1 if arg1 <= arg2
 * 		uint4gt			- returns 1 if arg1 > arg2
 * 		uint4ge			- returns 1 if arg1 >= arg2
 */
Datum uint4eq(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum uint4ne(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum uint4lt(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum uint4le(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum uint4gt(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum uint4ge(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint4cmp(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    if (arg1 > arg2)
        PG_RETURN_INT32(1);
    else if (arg1 == arg2)
        PG_RETURN_INT32(0);
    else
        PG_RETURN_INT32(-1);
}

Datum ui4toui1(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum ui1toui4(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui4toi1(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum i1toui4(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui2toi1(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    PG_RETURN_UINT8((uint8)arg1);
}

Datum i1toui2(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui1toi1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    PG_RETURN_UINT8(arg1);
}

Datum i1toui1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    PG_RETURN_UINT8(arg1);
}

Datum ui4toui2(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }
    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui2toui4(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui4toi2(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > SHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint out of range")));
    } else if (arg1 > SHRT_MAX) {
        arg1 = SHRT_MAX;
    }
    PG_RETURN_INT16((int16)arg1);
}

Datum i2toui4(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    }
    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui4toi4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > INT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int out of range")));
    } else if (arg1 > INT_MAX) {
        arg1 = INT_MAX;
    }

    PG_RETURN_INT32((int32)arg1);
}

Datum i4toui4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    }
    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui4toi8(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_INT64((int64)arg1);
}

Datum i8toui4(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0 || arg1 > UINT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    } else if (arg1 > UINT_MAX) {
        arg1 = UINT_MAX;
    }
    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui4tof4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_FLOAT4((float4)arg1);
}

Datum f4toui4(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);
    num = round(num);

    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
    
    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && (num < (float4)0 || num > ((float4)UINT_MAX))) {
        ereport(WARNING, (errmsg("integer unsigned out of range")));
        PG_RETURN_UINT32(num < (float8)0 ? 0 : UINT_MAX);
    }

    if (SQL_MODE_STRICT()) {
        if (num < (float4)0 || num > ((float4)UINT_MAX))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
    } else if (num < (float4)0) {
        PG_RETURN_UINT32(0);
    } else if (num > ((float4)UINT_MAX)) {
        PG_RETURN_UINT32(UINT_MAX);
    }

    PG_RETURN_UINT32((uint32)num);
}

Datum ui4tof8(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_FLOAT8((float8)arg1);
}

Datum f8toui4(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    num = round(num);

    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
    
    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && (num < (float8)0 || num > ((float8)UINT_MAX))) {
        ereport(WARNING, (errmsg("integer unsigned out of range")));
        PG_RETURN_UINT32(num < (float8)0 ? 0 : UINT_MAX);
    }

    if (SQL_MODE_STRICT()) {
        if (num < (float8)0 || num > ((float8)UINT_MAX))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
    } else if (num < (float8)0) {
        PG_RETURN_UINT32(0);
    } else if (num > ((float8)UINT_MAX)) {
        PG_RETURN_UINT32(UINT_MAX);
    }

    PG_RETURN_UINT32((uint32)num);
}

Datum uint4um(PG_FUNCTION_ARGS)
{
    int64 arg = PG_GETARG_UINT32(0);
    int64 result;

    result = -arg;

    PG_RETURN_INT64(result);
}

Datum uint8um(PG_FUNCTION_ARGS)
{
    int128 arg = PG_GETARG_UINT64(0);
    if (arg > LONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    int128 result;

    result = -arg;

    PG_RETURN_INT64((int64)result);
}

Datum uint4up(PG_FUNCTION_ARGS)
{
    uint32 arg = PG_GETARG_UINT32(0);

    PG_RETURN_UINT32(arg);
}

Datum uint4pl(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    uint64 result;

    result = (uint64)arg1 + (uint64)arg2;
    if (result > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }
    PG_RETURN_UINT32(result);
}

Datum uint4mi(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    uint32 result;
    if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }
    result = arg1 - arg2;

    PG_RETURN_UINT32(result);
}

Datum uint4mul(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    uint64 result64;
    result64 = (uint64)arg1 * (uint64)arg2;
    if (result64 > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }
    PG_RETURN_UINT64(result64);
}

Datum uint4div(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint4abs(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_UINT32(arg1);
}

Datum uint4mod(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    /* No overflow is possible */

    PG_RETURN_UINT32(arg1 % arg2);
}

Datum uint4larger(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32((arg1 > arg2) ? arg1 : arg2);
}

Datum uint4smaller(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32((arg1 < arg2) ? arg1 : arg2);
}

Datum int1_pl_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    int16 result = (int16)arg1 + (int16)arg2;

    if (result < 0 || result > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    PG_RETURN_UINT8(result);
}

Datum uint1_pl_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    int16 result = (int16)arg1 + (int16)arg2;

    if (result < 0 || result > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    PG_RETURN_UINT8(result);
}

Datum uint2_pl_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);
    int32 result;

    result = (int32)arg1 + (int32)arg2;
    if (result < 0 || result > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }

    PG_RETURN_UINT16(result);
}

Datum int2_pl_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    int32 result;

    result = (int32)arg1 + (int32)arg2;
    if (result < 0 || result > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }
    PG_RETURN_UINT16(result);
}

Datum uint4_pl_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    int64 result;

    result = (int64)arg1 + (int64)arg2;
    if (result < 0 || result > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }

    PG_RETURN_UINT32(result);
}

Datum int4_pl_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    int64 result;

    result = (int64)arg1 + (int64)arg2;
    if (result < 0 || result > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }

    PG_RETURN_UINT32(result);
}
/* datatype minus */
Datum int1_mi_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    int16 result;
    result = (int16)arg1 - (int16)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    PG_RETURN_UINT8(result);
}

Datum uint1_mi_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    int16 result;

    result = (int16)arg1 - (int16)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    PG_RETURN_UINT8(result);
}

Datum uint2_mi_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);
    int32 result;
    result = (int32)arg1 - (int32)arg2;
    if (result < 0 || result > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }

    PG_RETURN_UINT16(result);
}

Datum int2_mi_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    int32 result;
    result = (int32)arg1 - (int32)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }

    PG_RETURN_UINT16(result);
}

Datum uint4_mi_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    int64 result;
    result = (int64)arg1 - (int64)arg2;
    if (result < 0 || result > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }

    PG_RETURN_UINT32(result);
}

Datum int4_mi_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    int64 result;
    result = (int64)arg1 - (int64)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }

    PG_RETURN_UINT32(result);
}

/* datatype multiply */
Datum int1_mul_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    int16 result;
    result = (int16)arg1 * (int16)arg2;
    if (result > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }
    PG_RETURN_UINT8(result);
}

Datum uint1_mul_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    int16 result;
    result = (int16)arg1 * (int16)arg2;
    if (result > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }
    PG_RETURN_UINT8(result);
}

Datum uint2_mul_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);
    int32 result;
    result = (int32)arg1 * (int32)arg2;
    if (result < 0 || result > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }
    PG_RETURN_UINT16(result);
}

Datum int2_mul_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    int32 result;
    result = (int32)arg1 * (int32)arg2;
    if (result < 0 || result > USHRT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    }
    PG_RETURN_UINT16(result);
    ;
}

Datum uint4_mul_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    int64 result;
    result = (int64)arg1 * (int64)arg2;
    if (result < 0 || result > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }
    PG_RETURN_UINT32(result);
}

Datum int4_mul_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    int64 result;
    result = (int64)arg1 * (int64)arg2;
    if (result < 0 || result > UINT_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    }
    PG_RETURN_UINT32(result);
}

/* datatype divide */
Datum int1_div_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint1_div_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint2_div_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum int2_div_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint4_div_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum int4_div_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum int1_uint1_eq(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum int1_uint1_ne(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum int1_uint1_lt(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum int1_uint1_le(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_INT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum int1_uint1_gt(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_INT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum int1_uint1_ge(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_INT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint1_int1_eq(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_INT8(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum uint1_int1_ne(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_INT8(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum uint1_int1_lt(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_INT8(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum uint1_int1_le(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_INT8(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum uint1_int1_gt(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_INT8(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum uint1_int1_ge(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_INT8(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint2_int2_eq(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum uint2_int2_ne(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum uint2_int2_lt(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum uint2_int2_le(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum uint2_int2_gt(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum uint2_int2_ge(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum int2_uint2_eq(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum int2_uint2_ne(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum int2_uint2_lt(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum int2_uint2_le(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum int2_uint2_gt(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum int2_uint2_ge(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint4_int4_eq(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum uint4_int4_ne(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
}

Datum uint4_int4_lt(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum uint4_int4_le(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
}

Datum uint4_int4_gt(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum uint4_int4_ge(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);
    PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

Datum int4_uint4_eq(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum int4_uint4_ne(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
}

Datum int4_uint4_lt(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum int4_uint4_le(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
}

Datum int4_uint4_gt(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum int4_uint4_ge(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);
    PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

bool scanuint8(const char *str, bool errorOK, uint64 *result)
{
    const char *ptr = str;
    uint64 tmp = 0;
    bool neg = false;
    /* skip leading spaces */
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;

    /* handle sign */
    if (*ptr == '-') {
        if (SQL_MODE_STRICT()) {
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                errmsg("value \"%s\" is out of range for type bigint unsigned", str)));
        }
        ptr++;
        neg = true;
    } else if (*ptr == '+')
        ptr++;

    /* require at least one digit */
    if (!isdigit((unsigned char)*ptr)) {
        if (errorOK)
            return false;
        else if (!SQL_MODE_STRICT()) {
            *result = tmp;
            return true;
        } else {
            ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for integer: \"%s\"", str)));
        }
    }

    /* process digits */
    while (*ptr && isdigit((unsigned char)*ptr)) {
        uint64 newtmp = tmp * 10 + (*ptr++ - '0');

        if ((newtmp / 10) != tmp) { /* overflow? */
            if (errorOK)
                return false;
            else if (SQL_MODE_STRICT()) {
                ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                    errmsg("value \"%s\" is out of range for type bigint unsigned", str)));
            } else if (neg) {
                *result = 0;
                return true;
            } else {
                *result = PG_UINT64_MAX;
                return true;
            }
        }
        tmp = newtmp;
    }

    *result = tmp;

    return true;
}

Datum uint8in(PG_FUNCTION_ARGS)
{
    char *str = PG_GETARG_CSTRING(0);
    uint64 result;
    (void)scanuint8(str, false, &result);
    PG_RETURN_UINT64(result);
}

/* uint8out()
 */
Datum uint8out(PG_FUNCTION_ARGS)
{
    uint64 val = PG_GETARG_UINT64(0);
    char buf[MAXINT8LEN + 1];
    char *result;

    pg_ulltoa(val, buf);
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 * 		uint8recv			- converts external binary format to uint8
 */
Datum uint8recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    PG_RETURN_UINT64(pg_getmsguint64(buf));
}
void pq_senduint64(StringInfo buf, uint64 i)
{
    uint32 n32;

    /* High order half first, since we're doing MSB-first */
    n32 = (uint32)(i >> 32);
    n32 = htonl(n32);
    appendBinaryStringInfo(buf, (char *)&n32, 4);

    /* Now the low order half */
    n32 = (uint32)i;
    n32 = htonl(n32);
    appendBinaryStringInfo(buf, (char *)&n32, 4);
}

/*
 * 		uint8send			- converts uint8 to binary format
 */
Datum uint8send(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_senduint64(&buf, arg1);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum uint8and(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 & arg2);
}

Datum uint8or(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 | arg2);
}

Datum uint8xor(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 ^ arg2);
}

Datum uint8not(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);

    PG_RETURN_UINT64((uint64)(~arg1));
}

Datum uint8shl(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT64(0);
    }
    PG_RETURN_UINT64((uint64)(arg1 << arg2));
}

Datum uint8shr(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int32 arg2 = PG_GETARG_INT32(1);
    if (arg2 < 0 || arg2 >= UINT64_LENGTH) {
        PG_RETURN_UINT64(0);
    }
    PG_RETURN_UINT64(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint8
 * 		uint8eq			- returns 1 if arg1 == arg2
 * 		uint8ne			- returns 1 if arg1 != arg2
 * 		uint8lt			- returns 1 if arg1 < arg2
 * 		uint8le			- returns 1 if arg1 <= arg2
 * 		uint8gt			- returns 1 if arg1 > arg2
 * 		uint8ge			- returns 1 if arg1 >= arg2
 */
Datum uint8eq(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_BOOL(arg1 == arg2);
}

Datum uint8ne(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_BOOL(arg1 != arg2);
}

Datum uint8lt(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_BOOL(arg1 < arg2);
}

Datum uint8le(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_BOOL(arg1 <= arg2);
}

Datum uint8gt(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_BOOL(arg1 > arg2);
}

Datum uint8ge(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_BOOL(arg1 >= arg2);
}

Datum uint8cmp(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    if (arg1 > arg2)
        PG_RETURN_INT32(1);
    else if (arg1 == arg2)
        PG_RETURN_INT32(0);
    else
        PG_RETURN_INT32(-1);
}

/*
 * 		===================
 * 		CONVERSION ROUTINES
 * 		===================
 */

Datum ui8toui1(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);

    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum ui1toui8(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8toi1(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint out of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum i1toui8(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_INT8(0);
    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8toui2(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }
    PG_RETURN_UINT16((uint16)arg1);
}

Datum ui2toui8(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8toi2(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > SHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint out of range")));
    } else if (arg1 > SHRT_MAX) {
        arg1 = SHRT_MAX;
    }
    PG_RETURN_INT16((int16)arg1);
}

Datum i2toui8(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    }
    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8toi4(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > INT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int out of range")));
    } else if (arg1 > INT_MAX) {
        arg1 = INT_MAX;
    }
    PG_RETURN_INT32((int32)arg1);
}

Datum i4toui8(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    }
    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8toui4(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 > UINT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned out of range")));
    } else if (arg1 > UINT_MAX) {
        arg1 = UINT_MAX;
    }
    PG_RETURN_UINT32((uint32)arg1);
}

Datum ui4toui8(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8toi8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 result;

    /*
     * Does it fit in an int64?  Avoid assuming that we have handy constants
     * defined for the range boundaries, instead test for overflow by
     * reverse-conversion.
     */
    result = (int64)arg1;
    if (SQL_MODE_STRICT()) {
        if ((float8)result != arg1)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint out of range")));
    } else if ((float8)result != arg1) {
        arg1 = ULONG_MAX;
    }
    PG_RETURN_INT64((int64)arg1);
}

Datum i8toui8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < 0)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if (arg1 < 0) {
        arg1 = 0;
    }
    PG_RETURN_UINT64((uint64)arg1);
}

Datum ui8tof4(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);

    PG_RETURN_FLOAT4((float4)arg1);
}

Datum f4toui8(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);
    num = round(num);
    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));

    /* keyword IGNORE has higher priority than sql mode */
    if (fcinfo->can_ignore && (num < (float4)0 || num > ULONG_MAX)) {
        ereport(WARNING, (errmsg("bigint unsigned out of range")));
        PG_RETURN_UINT64(num < (float4)0 ? 0 : ULONG_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if (num < (float4)0 || (int128)num > ULONG_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if (num < (float4)0) {
        PG_RETURN_UINT64(0);
    } else if (num > ULONG_MAX) {
        PG_RETURN_UINT64(ULONG_MAX);
    }

    PG_RETURN_UINT64((int128)num);
}

Datum ui8tof8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);

    PG_RETURN_FLOAT8((float8)arg1);
}

Datum f8toui8(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    num = round(num);
    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));

    /* keyword IGNORE has higher priority than sql mode */
    if (fcinfo->can_ignore && (num < (float8)0 || (int128)num > ULONG_MAX)) {
        ereport(WARNING, (errmsg("bigint unsigned out of range")));
        PG_RETURN_UINT64(num < (float8)0 ? 0 : ULONG_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if (num < (float8)0 || (int128)num > ULONG_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if (num < (float8)0) {
        PG_RETURN_UINT64(0);
    } else if ((int128)num > ((float8)ULONG_MAX)) {
        PG_RETURN_UINT64(ULONG_MAX);
    }

    PG_RETURN_UINT64((int128)num);
}

Datum uint8up(PG_FUNCTION_ARGS)
{
    uint64 arg = PG_GETARG_UINT64(0);

    PG_RETURN_UINT64(arg);
}

Datum uint8pl(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    uint128 result;

    result = (uint128)arg1 + (uint128)arg2;
    if (result > ULONG_MAX)
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));

    PG_RETURN_UINT64(result);
}

Datum uint8mi(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    uint64 result;
    if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    result = arg1 - arg2;

    PG_RETURN_UINT64(result);
}

Datum uint8mul(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    uint128 result128;

    result128 = (uint128)arg1 * (uint128)arg2;
    if (result128 > ULONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    uint64 result64;
    result64 = arg1 * arg2;
    PG_RETURN_UINT64(result64);
}

Datum uint8div(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint8abs(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);

    PG_RETURN_UINT64(arg1);
}

Datum uint8mod(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }
    PG_RETURN_UINT64(arg1 % arg2);
}

Datum uint8larger(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64((arg1 > arg2) ? arg1 : arg2);
}

Datum uint8smaller(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64((arg1 < arg2) ? arg1 : arg2);
}

Datum int8_pl_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    int128 result;
    result = (int128)arg1 + (int128)arg2;
    if (result < 0 || result > ULONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    PG_RETURN_UINT64(result);
}

Datum uint8_pl_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);
    int128 result;
    result = (int128)arg1 + (int128)arg2;
    if (result < 0 || result > ULONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    PG_RETURN_UINT64(result);
}

Datum int8_mi_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    int128 result;
    result = (int128)arg1 - (int128)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    PG_RETURN_UINT64(result);
}

Datum uint8_mi_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);
    int128 result;
    result = (int128)arg1 - (int128)arg2;
    if (result < 0 || result > ULONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }

    PG_RETURN_UINT64(result);
}

Datum int8_mul_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    int128 result;
    result = (int128)arg1 * (int128)arg2;
    if (result < 0 || result > ULONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    PG_RETURN_UINT64(result);
}

Datum uint8_mul_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);
    int128 result;
    result = (int128)arg1 * (int128)arg2;
    if (result < 0 || result > ULONG_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    }
    PG_RETURN_UINT64(result);
}

Datum int8_div_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum uint8_div_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);
    INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum int8_uint8_eq(PG_FUNCTION_ARGS)
{
    int64 val1 = PG_GETARG_INT64(0);
    uint64 val2 = PG_GETARG_UINT64(1);
    if (val1 < 0)
        PG_RETURN_BOOL(0);
    PG_RETURN_BOOL((uint64)val1 == val2);
}

Datum uint8_int8_eq(PG_FUNCTION_ARGS)
{
    uint64 val1 = PG_GETARG_UINT64(0);
    int64 val2 = PG_GETARG_INT64(1);
    if (val2 < 0)
        PG_RETURN_BOOL(0);
    PG_RETURN_BOOL(val1 == (uint64)val2);
}

Datum int8_uint8_ne(PG_FUNCTION_ARGS)
{
    int64 val1 = PG_GETARG_INT64(0);
    uint64 val2 = PG_GETARG_UINT64(1);
    if (val1 < 0)
        PG_RETURN_BOOL(1);
    PG_RETURN_BOOL((uint64)val1 != val2);
}

Datum uint8_int8_ne(PG_FUNCTION_ARGS)
{
    uint64 val1 = PG_GETARG_UINT64(0);
    int64 val2 = PG_GETARG_INT64(1);
    if (val2 < 0)
        PG_RETURN_BOOL(1);
    PG_RETURN_BOOL(val1 != (uint64)val2);
}

Datum int8_uint8_lt(PG_FUNCTION_ARGS)
{
    int64 val1 = PG_GETARG_INT64(0);
    uint64 val2 = PG_GETARG_UINT64(1);
    if (val1 < 0)
        PG_RETURN_BOOL(1);
    PG_RETURN_BOOL((uint64)val1 < val2);
}

Datum uint8_int8_lt(PG_FUNCTION_ARGS)
{
    uint64 val1 = PG_GETARG_UINT64(0);
    int64 val2 = PG_GETARG_INT64(1);
    if (val2 < 0)
        PG_RETURN_BOOL(0);
    PG_RETURN_BOOL(val1 < (uint64)val2);
}

Datum uint8_int8_gt(PG_FUNCTION_ARGS)
{
    uint64 val1 = PG_GETARG_UINT64(0);
    int64 val2 = PG_GETARG_INT64(1);
    if (val2 < 0)
        PG_RETURN_BOOL(1);
    PG_RETURN_BOOL(val1 > (uint64)val2);
}

Datum int8_uint8_gt(PG_FUNCTION_ARGS)
{
    int64 val1 = PG_GETARG_INT64(0);
    uint64 val2 = PG_GETARG_UINT64(1);
    if (val1 < 0)
        PG_RETURN_BOOL(0);
    PG_RETURN_BOOL((uint64)val1 > val2);
}

Datum uint8_int8_le(PG_FUNCTION_ARGS)
{
    uint64 val1 = PG_GETARG_UINT64(0);
    int64 val2 = PG_GETARG_INT64(1);
    if (val2 < 0)
        PG_RETURN_BOOL(0);
    PG_RETURN_BOOL(val1 <= (uint64)val2);
}

Datum int8_uint8_le(PG_FUNCTION_ARGS)
{
    int64 val1 = PG_GETARG_INT64(0);
    uint64 val2 = PG_GETARG_UINT64(1);
    if (val1 < 0)
        PG_RETURN_BOOL(1);
    PG_RETURN_BOOL((uint64)val1 <= val2);
}

Datum uint8_int8_ge(PG_FUNCTION_ARGS)
{
    uint64 val1 = PG_GETARG_UINT64(0);
    int64 val2 = PG_GETARG_INT64(1);
    if (val2 < 0)
        PG_RETURN_BOOL(1);
    PG_RETURN_BOOL(val1 >= (uint64)val2);
}

Datum int8_uint8_ge(PG_FUNCTION_ARGS)
{
    int64 val1 = PG_GETARG_INT64(0);
    uint64 val2 = PG_GETARG_UINT64(1);

    if (val1 < 0)
        PG_RETURN_BOOL(0);
    PG_RETURN_BOOL((uint64)val1 >= val2);
}

Datum bool_uint1(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT8(0);
    else
        PG_RETURN_UINT8(1);
}

Datum uint1_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT8(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

Datum uint4_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT32(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

/* Cast bool -> Uint4 */
Datum bool_uint4(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT32(0);
    else
        PG_RETURN_UINT32(1);
}

/* Cast uint2 -> bool */
Datum uint2_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT16(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

/* Cast bool -> uint2 */
Datum bool_uint2(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT16(0);
    else
        PG_RETURN_UINT16(1);
}

Datum uint8_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT64(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

/* Cast bool -> uint8 */
Datum bool_uint8(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT64(0);
    else
        PG_RETURN_UINT64(1);
}

Datum uint1_int1_mod(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT8(arg1 % arg2);
}

Datum int1_uint1_mod(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT8(arg1 % arg2);
}

Datum uint2_int2_mod(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT16((int32)arg1 % (int32)arg2);
}

Datum int2_uint2_mod(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_INT16((int32)arg1 % (int32)arg2);
}

Datum uint4_int4_mod(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT32((int64)arg1 % (int64)arg2);
}

Datum int4_uint4_mod(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_INT32((int64)arg1 % (int64)arg2);
}

Datum uint8_int8_mod(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_UINT64((int128)arg1 % (int128)arg2);
}

Datum int8_uint8_mod(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    if (arg2 == 0) {
        PG_RETURN_NULL();
    }

    PG_RETURN_INT64((int128)arg1 % (int128)arg2);
}

Datum uint1_xor_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 ^ arg2);
}

Datum int1_xor_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 ^ arg2);
}

Datum uint2_xor_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_UINT16(arg1 ^ arg2);
}

Datum int2_xor_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 ^ arg2);
}

Datum uint4_xor_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT32(arg1 ^ arg2);
}

Datum int4_xor_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 ^ arg2);
}

Datum uint8_xor_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);

    PG_RETURN_UINT64(arg1 ^ arg2);
}

Datum int8_xor_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 ^ arg2);
}

Datum i2_cast_ui1(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < SCHAR_MIN || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < SCHAR_MIN) {
        arg1 = SCHAR_MIN;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum i4_cast_ui1(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < SCHAR_MIN || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < SCHAR_MIN) {
        arg1 = SCHAR_MIN;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum i8_cast_ui1(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < SCHAR_MIN || arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 < SCHAR_MIN) {
        arg1 = SCHAR_MIN;
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }
    PG_RETURN_UINT8((uint8)arg1);
}

Datum i2_cast_ui2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    PG_RETURN_UINT16((uint16)arg1);
}

Datum i4_cast_ui2(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < SHRT_MIN || arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 < SHRT_MIN) {
        arg1 = SHRT_MIN;
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }
    PG_RETURN_UINT16((uint16)arg1);
}

Datum i8_cast_ui2(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < SHRT_MIN || arg1 > USHRT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (arg1 < SHRT_MIN) {
        arg1 = SHRT_MIN;
    } else if (arg1 > USHRT_MAX) {
        arg1 = USHRT_MAX;
    }
    PG_RETURN_UINT16((uint16)arg1);
}

Datum i2_cast_ui4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT16(0);
    PG_RETURN_UINT32((uint32)arg1);
}

Datum i4_cast_ui4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    PG_RETURN_UINT32((uint32)arg1);
}

Datum i8_cast_ui4(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    if (SQL_MODE_STRICT()) {
        if (arg1 < INT_MIN || arg1 > UINT_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int out of range")));
    } else if (arg1 < INT_MIN) {
        arg1 = INT_MIN;
    } else if (arg1 > UINT_MAX) {
        arg1 = UINT_MAX;
    }
    PG_RETURN_UINT32((uint32)arg1);
}

Datum i2_cast_ui8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT16(0);
    PG_RETURN_UINT64((uint64)arg1);
}

Datum i4_cast_ui8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT32(0);
    PG_RETURN_UINT64((uint64)arg1);
}

Datum i8_cast_ui8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    PG_RETURN_UINT64((uint64)arg1);
}

Datum f4_cast_ui1(PG_FUNCTION_ARGS)
{
    float4 arg1 = PG_GETARG_FLOAT4(0);
    if (isnan(arg1))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned of range")));
    if (fcinfo->can_ignore && arg1 > UCHAR_MAX) {
        ereport(WARNING, (errmsg("tinyint unsigned out of range")));
        PG_RETURN_UINT8(UCHAR_MAX);
    }
    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }

    PG_RETURN_UINT8((int16)round(arg1));
}

Datum f8_cast_ui1(PG_FUNCTION_ARGS)
{
    float8 arg1 = PG_GETARG_FLOAT8(0);
    if (isnan(arg1))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsignedout of range")));
    if (fcinfo->can_ignore && arg1 > UCHAR_MAX) {
        ereport(WARNING, (errmsg("tinyint unsigned out of range")));
        PG_RETURN_UINT8(UCHAR_MAX);
    }
    if (SQL_MODE_STRICT()) {
        if (arg1 > UCHAR_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsignedout of range")));
    } else if (arg1 > UCHAR_MAX) {
        arg1 = UCHAR_MAX;
    }

    PG_RETURN_UINT8((int16)round(arg1));
}

Datum f4_cast_ui2(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);

    num = round(num);
    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));

    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && num >= (float4)((uint32)USHRT_MAX + 1)) {
        ereport(WARNING, (errmsg("smallint unsigned out of range")));
        PG_RETURN_UINT16( USHRT_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if (num >= (float4)((uint32)USHRT_MAX + 1))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (num >= (float4)((uint32)USHRT_MAX + 1)) {
        PG_RETURN_UINT16(USHRT_MAX);
    }

    PG_RETURN_UINT16((int32)num);
}

Datum f8_cast_ui2(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    num = round(num);

    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));

    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && num >= (float8)((uint32)USHRT_MAX + 1)) {
        ereport(WARNING, (errmsg("smallint unsigned out of range")));
        PG_RETURN_UINT16(USHRT_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if (num >= (float8)((uint32)USHRT_MAX + 1))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
    } else if (num >= (float8)((uint32)USHRT_MAX + 1)) {
        PG_RETURN_UINT16(USHRT_MAX);
    }

    PG_RETURN_UINT16((int32)num);
}

Datum f4_cast_ui4(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);
    num = round(num);

    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));

    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && num >= (float4)((uint64)UINT_MAX + 1)) {
        ereport(WARNING, (errmsg("integer unsigned out of range")));
        PG_RETURN_UINT32(UINT_MAX);
    }

    if (SQL_MODE_STRICT()) {
        if (num >= (float4)((uint64)UINT_MAX + 1))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
    } else if (num >= (float4)((uint64)UINT_MAX + 1)) {
        PG_RETURN_UINT32(UINT_MAX);
    }

    PG_RETURN_UINT32((int64)num);
}

Datum f8_cast_ui4(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    num = round(num);

    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
 
    /* keyword IGNORE has priority than sql mode */
    if (fcinfo->can_ignore && num >= (float8)((uint64)UINT_MAX + 1)) {
        ereport(WARNING, (errmsg("integer unsigned out of range")));
        PG_RETURN_UINT32(UINT_MAX);
    }

    if (SQL_MODE_STRICT()) {
        if (num >= (float8)((uint64)UINT_MAX + 1))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
    } else if (num >= (float8)((uint64)UINT_MAX + 1)) {
        PG_RETURN_UINT32(UINT_MAX);
    }

    PG_RETURN_UINT32((int64)num);
}

Datum f4_cast_ui8(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);
    num = round(num);
    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));

    /* keyword IGNORE has higher priority than sql mode */
    if (fcinfo->can_ignore && (int128)num > ULONG_MAX) {
        ereport(WARNING, (errmsg("bigint unsigned out of range")));
        PG_RETURN_UINT64(ULONG_MAX);
    }
    
    if (SQL_MODE_STRICT()) {
        if ((int128)num > ULONG_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if ((int128)num > ULONG_MAX) {
        PG_RETURN_UINT64(ULONG_MAX);
    }

    PG_RETURN_UINT64((int128)num);
}

Datum f8_cast_ui8(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    num = round(num);
    if (isnan(num))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));

    /* keyword IGNORE has higher priority than sql mode */
    if (fcinfo->can_ignore && (int128)num > ULONG_MAX) {
        ereport(WARNING, (errmsg("bigint unsigned out of range")));
        PG_RETURN_UINT64(ULONG_MAX);
    }
    if (SQL_MODE_STRICT()) {
        if ((int128)num > ULONG_MAX)
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
    } else if ((int128)num > ULONG_MAX) {
        PG_RETURN_UINT64(ULONG_MAX);
    }

    PG_RETURN_UINT64((int128)num);
}

Datum uint1_or_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 | arg2);
}

Datum int1_or_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 | arg2);
}

Datum uint2_or_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_UINT16(arg1 | arg2);
}

Datum int2_or_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 | arg2);
}

Datum uint4_or_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT32(arg1 | arg2);
}

Datum int4_or_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 | arg2);
}

Datum uint8_or_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);

    PG_RETURN_UINT64(arg1 | arg2);
}

Datum int8_or_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 | arg2);
}

Datum uint1_and_int1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 & arg2);
}

Datum int1_and_uint1(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 & arg2);
}

Datum uint2_and_int2(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int16 arg2 = PG_GETARG_INT16(1);

    PG_RETURN_UINT16(arg1 & arg2);
}

Datum int2_and_uint2(PG_FUNCTION_ARGS)
{
    int16 arg1 = PG_GETARG_INT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 & arg2);
}

Datum uint4_and_int4(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT32(arg1 & arg2);
}

Datum int4_and_uint4(PG_FUNCTION_ARGS)
{
    int32 arg1 = PG_GETARG_INT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 & arg2);
}

Datum uint8_and_int8(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int64 arg2 = PG_GETARG_INT64(1);

    PG_RETURN_UINT64(arg1 & arg2);
}

Datum int8_and_uint8(PG_FUNCTION_ARGS)
{
    int64 arg1 = PG_GETARG_INT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 & arg2);
}