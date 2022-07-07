
#include "postgres.h"
#include "knl/knl_variable.h"

#include "funcapi.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "plugin_utils/year.h"

/*
 * gcc's -ffast-math switch breaks routines that expect exact results from
 * expressions like timeval / SECS_PER_HOUR, where timeval is double.
 */
#ifdef __FAST_MATH__
#error -ffast-math is known to break this code
#endif

#define TYPMODOUT_LEN 64

static int year_fastcmp(Datum x, Datum y, SortSupport ssup);
static YearADT int32_to_YearADT(int4 year);

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
extern "C" DLL_PUBLIC Datum year_integer(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(year_scale);
extern "C" DLL_PUBLIC Datum year_scale(PG_FUNCTION_ARGS);

/*****************************************************************************
 *	 Year4 
 *****************************************************************************/

/* year_in()
 * Given year text string, convert to internal year format.
 */
Datum year_in(PG_FUNCTION_ARGS)
{
    char *str_in = PG_GETARG_CSTRING(0);
    /*
     * parse str_in
     * status flag's meaning :
     * 1: begin field  : only allow for space
     * 2: number field : only allow for numbers
     * 4: end field    : only allow for space
     * for example     : ' 1997  '
     */
    int len = strlen(str_in);
    int status = 1;
    int32 tmp = 0;
    int year_len = 0;
    for (int i = 0; i < len; ++i) {
        switch (status)
        {
        case 1:
            if (isdigit(str_in[i])) {
                status <<= 1;
                ++year_len;
                tmp = str_in[i] - '0';
            } else if (str_in[i] != ' ') {
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Invalid integer value: \"%s\"", str_in)));
            }
            break;
        case 2:
            if (isdigit(str_in[i])) {
                ++year_len;
                if (year_len > YEAR4_LEN) {
                    ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Incorrect integer value: \"%s\"", str_in)));
                }
                tmp = tmp * 10 + str_in[i] - '0';
            } else if (str_in[i] == ' ') {
                status <<= 1;
            } else {
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Incorrect integer value: \"%s\"", str_in)));
            }
            break;
        case 4:
            if (str_in[i] != ' ') {
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Incorrect integer value: \"%s\"", str_in)));
            }
            break;
        default:
            break;
        }
    }
    /* empty str_in */
    if (status == 1) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Incorrect integer value: \"%s\"", str_in)));
    }
    /* 
     * "0000" -> 0 
     * "000"  -> 2000
     * "00"   -> 2000
     * "0"    -> 2000
     */
    if (tmp == 0 && year_len != YEAR4_LEN) {
        tmp = 2000;
    }
    PG_RETURN_YEARADT(int32_to_YearADT(tmp));
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

static YearADT int32_to_YearADT(int4 year)
{
    if (year) {
        if (YEAR_VALID(year)) {
            if (YEAR2_IN_RANGE(year))
                year = year + (year >= YEAR2_BOUND_BETWEEN_20C_21C ? 1900 : 2000);
            year = year - MIN_YEAR_NUM + 1;
        } else {
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
    PG_RETURN_YEARADT(int32_to_YearADT(year));
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
