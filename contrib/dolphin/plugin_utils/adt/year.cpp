
#include "postgres.h"
#include "knl/knl_variable.h"
#include "access/hash.h"

#include "funcapi.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "plugin_utils/year.h"
#ifdef DOLPHIN
#include "plugin_commands/mysqlmode.h"
#endif

/*
 * gcc's -ffast-math switch breaks routines that expect exact results from
 * expressions like timeval / SECS_PER_HOUR, where timeval is double.
 */
#ifdef __FAST_MATH__
#error -ffast-math is known to break this code
#endif

#define TYPMODOUT_LEN 64

static int year_fastcmp(Datum x, Datum y, SortSupport ssup);
static YearADT int32_to_YearADT(int4 year, bool canIgnore = false);

PG_FUNCTION_INFO_V1_PUBLIC(year_in);
extern "C" DLL_PUBLIC Datum year_in(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_out);
extern "C" DLL_PUBLIC Datum year_out(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(yeartypmodin);
extern "C" DLL_PUBLIC Datum yeartypmodin(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(yeartypmodout);
extern "C" DLL_PUBLIC Datum yeartypmodout(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_recv);
extern "C" DLL_PUBLIC Datum year_recv(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_send);
extern "C" DLL_PUBLIC Datum year_send(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_eq);
extern "C" DLL_PUBLIC Datum year_eq(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_ne);
extern "C" DLL_PUBLIC Datum year_ne(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_lt);
extern "C" DLL_PUBLIC Datum year_lt(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_le);
extern "C" DLL_PUBLIC Datum year_le(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_ge);
extern "C" DLL_PUBLIC Datum year_ge(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_gt);
extern "C" DLL_PUBLIC Datum year_gt(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_larger);
extern "C" DLL_PUBLIC Datum year_larger(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_smaller);
extern "C" DLL_PUBLIC Datum year_smaller(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_pl_interval);
extern "C" DLL_PUBLIC Datum year_pl_interval(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_mi_interval);
extern "C" DLL_PUBLIC Datum year_mi_interval(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_mi);
extern "C" DLL_PUBLIC Datum year_mi(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_cmp);
extern "C" DLL_PUBLIC Datum year_cmp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_sortsupport);
extern "C" DLL_PUBLIC Datum year_sortsupport(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_year);
extern "C" DLL_PUBLIC Datum int32_year(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_integer);
PG_FUNCTION_INFO_V1_PUBLIC(year_scale);
extern "C" DLL_PUBLIC Datum year_scale(PG_FUNCTION_ARGS);

/* b compatibility time function */
PG_FUNCTION_INFO_V1_PUBLIC(period_add);
extern "C" DLL_PUBLIC Datum period_add(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(period_diff);
extern "C" DLL_PUBLIC Datum period_diff(PG_FUNCTION_ARGS);

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(year_xor_transfn);
extern "C" DLL_PUBLIC Datum year_xor_transfn(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_year);
extern "C" DLL_PUBLIC Datum timestamptz_year(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(datetime_year);
extern "C" DLL_PUBLIC Datum datetime_year(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(date_year);
extern "C" DLL_PUBLIC Datum date_year(PG_FUNCTION_ARGS);
#endif

/*****************************************************************************
 *	 Year4 
 *****************************************************************************/

/* year_in()
 * Given year text string, convert to internal year format.
 */
Datum year_in(PG_FUNCTION_ARGS)
{
    char *str_in = PG_GETARG_CSTRING(0);
    bool hasError = false;
    char* endptr = NULL;
    double num = float8in_internal(str_in, &endptr, &hasError);
    int32 tmp = (int32)DirectFunctionCall1(dtoi4, Float8GetDatum(num));
    int len = strlen(str_in);

    hasError = hasError || (endptr != NULL && *endptr != '\0');
    if(hasError) {
        ereport((!fcinfo->can_ignore && SQL_MODE_STRICT()) ? ERROR : WARNING,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for type double precision: \"%s\"", str_in)));
    }

    /* 
     * number equals 0, and string length is 4, the result is 0
     * number equals 0, string length is NOT 4, the result is 2000
     * '0000', '+000', '00.1', '-0.2', ' 000' -- 0
     * '  000 ', '00000', '0.1' --2000
     */
    if (tmp == 0 && len != YEAR4_LEN && !hasError) {
        tmp = 2000;
    }
    YearADT result = int32_to_YearADT(tmp);
    /* year(2) */
    if (PG_GETARG_INT32(2) == YEAR2_LEN) {
        YEAR_CONVERT(result);
    }
    PG_RETURN_YEARADT(result);
}

/* year_out()
 * Given internal format year4, convert to text string.
 */
Datum year_out(PG_FUNCTION_ARGS)
{
    YearADT year = PG_GETARG_INT16(0);
    int32 val = 0;
    char* result = NULL;
    errno_t rc = EOK;
    char buf[5]; /* buffer's size = 5 is enough for "YYYY\0" */
    if (year >= 0) {
        /* year(4) */
        val = YearADT_to_Year(year);
        rc = sprintf_s(buf, sizeof(buf), "%d", val);
    } else {
        /* year(2): displays only the last (least significant) 2 digits */
        val = YearADT_to_Year(-year) % 100;
        rc = sprintf_s(buf, sizeof(buf), "%02d", val);
    }
    securec_check_ss(rc, "\0", "\0");
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *		year_recv			- converts external binary format to year4
 */
Datum year_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    int32 tmp = pq_getmsgint(buf, sizeof(YearADT));
    PG_RETURN_YEARADT(int32_to_YearADT(tmp));
}

/*
 *		year_send			- converts year4 to binary format
 */
Datum year_send(PG_FUNCTION_ARGS)
{
    YearADT year = PG_GETARG_YEARADT(0);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendint32(&buf, YearADT_to_Year(year));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum yeartypmodin(PG_FUNCTION_ARGS)
{
    ArrayType *ta = PG_GETARG_ARRAYTYPE_P(0);
    int32 typmod;
    int32 *tl = NULL;
    int n;
    tl = ArrayGetIntegerTypmods(ta, &n);
    if (n != 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid type modifier")));
    typmod = *tl;
    /* neither year(2) nor year(4) */
    if (typmod != 2 && typmod != 4) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid display width and use YEAR instead")));
    }
    PG_RETURN_INT32(typmod);
}

/* determine display width */
Datum year_scale(PG_FUNCTION_ARGS) 
{
    YearADT result = PG_GETARG_YEARADT(0);
    int32 typmod = PG_GETARG_INT32(1);
    if (typmod != 2 && typmod != 4) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid display width and use YEAR instead")));
    }
    /* Mark the year(2) type as negative */
    if (typmod == 2) {
        YEAR_CONVERT(result);
    }
    PG_RETURN_YEARADT(result);
}

Datum yeartypmodout(PG_FUNCTION_ARGS) 
{
    int32 typmod = PG_GETARG_INT32(0);
    char* ret = (char*)palloc(TYPMODOUT_LEN);
    int rc = 0;
    if (typmod >= 0) {
        rc = snprintf_s(ret, TYPMODOUT_LEN, TYPMODOUT_LEN-1, "(%d)", (int)typmod);
        securec_check_ss(rc, "\0", "\0");
    } else {
        rc = snprintf_s(ret, TYPMODOUT_LEN, TYPMODOUT_LEN-1, "");
        securec_check_ss(rc, "\0", "\0");
    }

    PG_RETURN_CSTRING(ret);
}

static YearADT int32_to_YearADT(int4 year, bool canIgnore)
{
#ifdef DOLPHIN
    int errlevel = (!canIgnore && SQL_MODE_STRICT() ? ERROR : WARNING);
#endif

    if (year) {
        if (YEAR_VALID(year)) {
            if (YEAR2_IN_RANGE(year))
                year = year + (year >= YEAR2_BOUND_BETWEEN_20C_21C ? 1900 : 2000);
            year = year - MIN_YEAR_NUM + 1;
        } else {
#ifdef DOLPHIN
            ereport(errlevel,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                    errmsg("Out of range value for year data type!")));
            return 0;
#endif
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Out of range value for year data type!")));
        }
    }
    return year;
}

Datum year_eq(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_BOOL(year_cmp_internal(y1, y2) == 0);
}

Datum year_ne(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_BOOL(year_cmp_internal(y1, y2) != 0);
}

Datum year_lt(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_BOOL(year_cmp_internal(y1, y2) < 0);
}

Datum year_le(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_BOOL(year_cmp_internal(y1, y2) <= 0);
}

Datum year_ge(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_BOOL(year_cmp_internal(y1, y2) >= 0);
}

Datum year_gt(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_BOOL(year_cmp_internal(y1, y2) > 0);
}

Datum year_larger(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_TIMESTAMP(0);
    YearADT y2 = PG_GETARG_TIMESTAMP(1);
    YearADT result;

    if (year_cmp_internal(y1, y2) > 0)
        result = y1;
    else
        result = y2;
    PG_RETURN_TIMESTAMP(result);
}

Datum year_smaller(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_TIMESTAMP(0);
    YearADT y2 = PG_GETARG_TIMESTAMP(1);
    YearADT result;

    if (year_cmp_internal(y1, y2) < 0)
        result = y1;
    else
        result = y2;
    PG_RETURN_TIMESTAMP(result);
}

Datum int32_year(PG_FUNCTION_ARGS)
{
    int4 year = PG_GETARG_INT32(0);
    PG_RETURN_YEARADT(int32_to_YearADT(year, fcinfo->can_ignore));
}

Datum year_integer(PG_FUNCTION_ARGS) {
    YearADT year = PG_GETARG_YEARADT(0);
    if (IS_YEAR2(year))
        YEAR_CONVERT(year);
    year = YearADT_to_Year(year);
    PG_RETURN_INT32(year);
}

Datum year_cmp(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    PG_RETURN_INT32(year_cmp_internal(y1, y2));
}

static int year_fastcmp(Datum x, Datum y, SortSupport ssup)
{
    YearADT y1 = DatumGetYearADT(x);
    YearADT y2 = DatumGetYearADT(y);
    return year_cmp_internal(y1, y2);
}

Datum year_sortsupport(PG_FUNCTION_ARGS)
{
    SortSupport ssup = (SortSupport)PG_GETARG_POINTER(0);

    ssup->comparator = year_fastcmp;
    PG_RETURN_VOID();
}

int year_cmp_internal(YearADT y1, YearADT y2)
{
    if (IS_YEAR2(y1))
        YEAR_CONVERT(y1);
    if (IS_YEAR2(y2)) 
        YEAR_CONVERT(y2);
    return (y1 < y2) ? -1 : ((y1 > y2) ? 1 : 0);
}

Datum year_pl_interval(PG_FUNCTION_ARGS)
{
    YearADT year = PG_GETARG_YEARADT(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    return year_pl_interval(year, span);
}

Datum year_mi_interval(PG_FUNCTION_ARGS)
{
    YearADT year = PG_GETARG_YEARADT(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    return year_mi_interval(year, span);
}

Datum year_mi_interval(YearADT year, const Interval* span)
{
    Interval tspan;
    tspan.month = -span->month;
    tspan.day = -span->day;
    tspan.time = -span->time;
    return DirectFunctionCall2(year_pl_interval, YearADTGetDatum(year), PointerGetDatum(&tspan));
}

Datum year_pl_interval(YearADT year, const Interval* span)
{
    bool isYear2 = false;
    if (IS_YEAR2(year)) {
        YEAR_CONVERT(year);
        isYear2 = true;
    }
    // year -> timestamp
    Timestamp timestamp;
    fsec_t fsec = 0;
    struct pg_tm tt, *tm = &tt;
    error_t rc = EOK;
    rc = memset_s(&tt, sizeof(pg_tm), 0, sizeof(pg_tm));
    securec_check(rc, "\0", "\0");
    tm->tm_mday = 1;
    tm->tm_mon = 1;
    tm->tm_year = YearADT_to_Year(year);
    if (tm2timestamp(tm, fsec, NULL, &timestamp) != 0) {
       ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
 
    timestamp = DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(timestamp), PointerGetDatum(span));
    // timestamp -> tm
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    // check if res is valid
    if (!YEAR_VALID(tm->tm_year)) {
         ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Out of range value for year data type!")));
    }
    year = Year_to_YearADT(tm->tm_year);
    if (isYear2) {
        YEAR_CONVERT(year);
    }
    PG_RETURN_YEARADT(year);
}

Datum year_mi(PG_FUNCTION_ARGS)
{
    YearADT y1 = PG_GETARG_YEARADT(0);
    YearADT y2 = PG_GETARG_YEARADT(1);
    return year_mi(y1, y2);
}

Datum year_mi(YearADT y1, YearADT y2)
{
    if (IS_YEAR2(y1))
        YEAR_CONVERT(y1);
    if (IS_YEAR2(y2)) 
        YEAR_CONVERT(y2);
    Interval* result = NULL;
    result = (Interval*)palloc(sizeof(Interval));
    result->time = 0;
    result->day = 0;
    result->month = (y1-y2) * MONTHS_PER_YEAR;
    PG_RETURN_INTERVAL_P(result);
}

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(int8_year);
extern "C" DLL_PUBLIC Datum int8_year(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int16_year);
extern "C" DLL_PUBLIC Datum int16_year(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_year);
extern "C" DLL_PUBLIC Datum int64_year(PG_FUNCTION_ARGS);
ulong convert_period_to_month(ulong period)
{
    ulong a, b;
    if (period == 0)
        return 0L;
    if ((a = period / 100) < YEAR2_BOUND_BETWEEN_20C_21C)
        a += 2000;
    else if (a < 100)
        a += 1900;
    b = period % 100;
    return a * 12 + b - 1;
}

ulong convert_month_to_period(ulong month)
{
    ulong year;
    if (month == 0L)
        return 0L;
    if ((year = month / 12) < 100) {
        year += (year < YEAR2_BOUND_BETWEEN_20C_21C) ? 2000 : 1900;
    }
    return year * 100 + month % 12 + 1;
}

/*
 * @Description: Add some months to a period of time.
 * @return: result, int64 type.
 */
Datum period_add(PG_FUNCTION_ARGS)
{
    ulong period = (ulong)PG_GETARG_INT64(0);
    int months = (int)PG_GETARG_INT64(1);
    if (period == 0)
        PG_RETURN_INT32(0);
    PG_RETURN_INT64((int64)convert_month_to_period((uint)((int)convert_period_to_month(period) + months)));
}

/*
 * @Description: Calculate the number of months between two period of times.
 * @return: result, int64 type.
 */
Datum period_diff(PG_FUNCTION_ARGS)
{
    uint64 input1 = (uint64)PG_GETARG_INT64(0);
    uint64 input2 = (uint64)PG_GETARG_INT64(1);

    int64 ret = (int64)((int64)convert_period_to_month(input1) - (int64)convert_period_to_month(input2));
    PG_RETURN_INT64(ret);
}

Datum year_hash(PG_FUNCTION_ARGS)
{
    return hashint8(fcinfo);
}

Datum year_xor_transfn(PG_FUNCTION_ARGS)
{
    uint internal = 0;
    YearADT yearVal = 0;
    uint year = 0;
    /* On the first time through, we ignore the delimiter. */

    if (!PG_ARGISNULL(0)) {
        internal = PG_GETARG_UINT32(0);
    }

    if (!PG_ARGISNULL(1)) {
        yearVal = PG_GETARG_YEARADT(1);
        year = (uint)YearADT_to_Year(yearVal);
    }

    PG_RETURN_UINT32(year ^ internal);
}

Datum int8_year(PG_FUNCTION_ARGS)
{
    int8 year = PG_GETARG_INT8(0);
    PG_RETURN_YEARADT(int32_to_YearADT((int32)year));
}

Datum int16_year(PG_FUNCTION_ARGS)
{
    int16 year = PG_GETARG_INT16(0);
    PG_RETURN_YEARADT(int32_to_YearADT((int32)year));
}

Datum int64_year(PG_FUNCTION_ARGS)
{
    int64 year = PG_GETARG_INT64(0);
    PG_RETURN_YEARADT(int32_to_YearADT((int32)year, fcinfo->can_ignore));
}


Datum timestamptz_year(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    int4 year = MIN_YEAR_NUM - 1;
    fsec_t fsec;
    int tz;
    const char* tzn = NULL;
    pg_tm tt;
    pg_tm* tm = &tt;
    if (timestamp2tm(timestamp, &tz, tm, &fsec, &tzn, NULL) == 0) {
        year = tm->tm_year;
    }
    PG_RETURN_YEARADT(int32_to_YearADT(year, fcinfo->can_ignore));
}

inline Datum datetime_to_year(Timestamp timestamp, bool can_ignore)
{
    int4 year = MIN_YEAR_NUM - 1;
    fsec_t fsec;
    pg_tm tt;
    pg_tm *tm = &tt;
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0) {
        year = tm->tm_year;
    }
    PG_RETURN_YEARADT(int32_to_YearADT(year, can_ignore));
}


Datum datetime_year(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    return datetime_to_year(timestamp, fcinfo->can_ignore);
}

Datum date_year(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    Timestamp timestamp = date2timestamp(date);
    return datetime_to_year(timestamp, fcinfo->can_ignore);
}


PG_FUNCTION_INFO_V1_PUBLIC(year_int8);
extern "C" DLL_PUBLIC Datum year_int8(PG_FUNCTION_ARGS);
Datum year_int8(PG_FUNCTION_ARGS)
{
    int32 res = DatumGetInt32(DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
    if (res == 0) {
        PG_RETURN_INT8(0);
    }
    PG_RETURN_INT8(PG_INT8_MAX);
}

PG_FUNCTION_INFO_V1_PUBLIC(year_int16);
extern "C" DLL_PUBLIC Datum year_int16(PG_FUNCTION_ARGS);
Datum year_int16(PG_FUNCTION_ARGS)
{
    int16 res = DatumGetInt32(DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
    PG_RETURN_INT16(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(year_int64);
extern "C" DLL_PUBLIC Datum year_int64(PG_FUNCTION_ARGS);
Datum year_int64(PG_FUNCTION_ARGS)
{
    int64 res = DatumGetInt32(DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
    PG_RETURN_INT64(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(year_uint8);
extern "C" DLL_PUBLIC Datum year_uint8(PG_FUNCTION_ARGS);
Datum year_uint8(PG_FUNCTION_ARGS)
{
    uint64 res = DatumGetInt32(DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
    PG_RETURN_UINT64(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(year_float4);
extern "C" DLL_PUBLIC Datum year_float4(PG_FUNCTION_ARGS);
Datum year_float4(PG_FUNCTION_ARGS)
{
    float4 res = DatumGetInt32(DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
    PG_RETURN_FLOAT4(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(year_float8);
extern "C" DLL_PUBLIC Datum year_float8(PG_FUNCTION_ARGS);
Datum year_float8(PG_FUNCTION_ARGS)
{
    double res = DatumGetInt32(DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
    PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(year_numeric);
extern "C" DLL_PUBLIC Datum year_numeric(PG_FUNCTION_ARGS);
Datum year_numeric(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1(int4_numeric, DirectFunctionCall1(year_integer, PG_GETARG_DATUM(0)));
}

PG_FUNCTION_INFO_V1_PUBLIC(year_any_value);
extern "C" DLL_PUBLIC Datum year_any_value(PG_FUNCTION_ARGS);
Datum year_any_value(PG_FUNCTION_ARGS)
{
    return PG_GETARG_DATUM(0);
}

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_yearnot);
extern "C" DLL_PUBLIC Datum dolphin_yearnot(PG_FUNCTION_ARGS);
Datum dolphin_yearnot(PG_FUNCTION_ARGS)
{
    PG_RETURN_UINT64(~year_uint8(fcinfo));
}
#endif
