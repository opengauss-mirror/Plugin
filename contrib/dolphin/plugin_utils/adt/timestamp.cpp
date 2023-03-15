/* -------------------------------------------------------------------------
 *
 * timestamp.c
 *	  Functions for the built-in SQL92 types "timestamp" and "interval".
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 * Portions Copyright (c) 2021, openGauss Contributors
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/timestamp.c
 *
 * -------------------------------------------------------------------------
 */

#include "postgres.h"
#include "knl/knl_variable.h"

#include <math.h>
#include <float.h>
#include <limits.h>
#include <sys/time.h>
#include <algorithm> 

#include "utils/numeric.h"
#include "access/hash.h"
#include "access/xact.h"
#include "catalog/pg_type.h"
#include "commands/copy.h"
#include "funcapi.h"
#include "libpq/pqformat.h"
#include "miscadmin.h"
#include "nodes/nodeFuncs.h"
#include "plugin_parser/scansup.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/formatting.h"
#include "common/int.h"
#include "plugin_utils/timestamp.h"
#include "plugin_utils/datetime.h"
#include "plugin_utils/date.h"
#include "plugin_utils/year.h"
#ifdef DOLPHIN
#include "plugin_utils/my_locale.h"
#include "plugin_commands/mysqlmode.h"
#endif

#ifdef PGXC
#include "pgxc/pgxc.h"
#endif

#if defined(ENABLE_UT) || defined(ENABLE_QUNIT)
#define static
#endif

/*
 * gcc's -ffast-math switch breaks routines that expect exact results from
 * expressions like timeval / SECS_PER_HOUR, where timeval is double.
 */
#ifdef __FAST_MATH__
#error -ffast-math is known to break this code
#endif

#define DATE_WITHOUT_SPC_LEN 14

#define FOUR_DIGIT_LEN 4 /* year is 4 */
#ifdef DOLPHIN
#define THREE_DIGIT_LEN 3
#define SIX_DIGIT_LEN 6
#define SEVEN_DIGIT_LEN 7
#define EIGHT_DIGIT_LEN 8
#endif
#define TWO_DIGIT_LEN 2  /*month,day,hour,minute and second*/
#define MAXLEN_DATE 10   /* with separator, without time-string*/
#define MINLEN_DATE 8    /* without separator, without time-string */
#define MAXLEN_TIME 8
#define MAXLEN_TIME_WITHOUT_SPRTR 6
#define MINLEN_TIME 5
#define UNIT_LEN 10
#define DATESTR_LEN 20
#define TIMESTR_LEN 20
#define TYPMODOUT_LEN 64
#define ROUNDING_BORDER 5

#ifndef DOLPHIN
#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGEST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12
#define MAXYEAR 9999
#define DAYNUM_FEB_LEAPYEAR 29
#define DAYNUM_FEB_NONLEAPYEAR 28
#define DAYNUM_BIGMON 31
#define DAYNUM_LITTLEMON 30
#define MAXNUM_HOUR 24
#define MAXNUM_MIN 60
#define MAXNUM_SEC 60
#define DAYS_PER_COMMON_YEAR 365
#define DAYS_PER_LEAP_YEAR 366
#else
#define MAX_YEAR 9999
#endif

/* NaN and Infinity Macro used in interval_mul and interval_div*/
/*
 * Actually, when this macro is used, it probabily means interval overflow/underflow.
 * However, we have to keep this behavior in order to maintain compatibility.
 */

#define BIT_NUM_PER_CHAR 8
#define NAN_IFINITY_CONVERT_TO_INT 0x8000000000000000
#define INTERVAL_CONVERT_INFINITY_NAN(result)                                               \
    do {                                                                                    \
        (result)->time = NAN_IFINITY_CONVERT_TO_INT;                                        \
        (result)->month = NAN_IFINITY_CONVERT_TO_INT >> (sizeof(int32) * BIT_NUM_PER_CHAR); \
        (result)->day = 0;                                                                  \
    } while (0)

/*
 * all stuffs used for bulkload(end).
 */

typedef struct {
    Timestamp current;
    Timestamp finish;
    Interval step;
    int step_sign;
} generate_series_timestamp_fctx;

typedef struct {
    TimestampTz current;
    TimestampTz finish;
    Interval step;
    int step_sign;
} generate_series_timestamptz_fctx;

static TimeOffset time2t(const int hour, const int min, const int sec, const fsec_t fsec);
static void EncodeSpecialTimestamp(Timestamp dt, char* str);
static Timestamp dt2local(Timestamp dt, int timezone);
static void AdjustTimestampForTypmod(Timestamp* time, int32 typmod);
static void AdjustIntervalForTypmod(Interval* interval, int32 typmod);
static void interval_format_adjust(Interval* interval, char type_mode);
static void interval_result_adjust(Interval* result);
struct pg_tm* GetDateDetail(const char* dateString);
void SplitWholeStrWithoutSeparator(const char* dateString, struct pg_tm* tm);
void AnalyseDate(const char* dateString, struct pg_tm* tm_date);
void SplitDatestrBySeparator(
    const char* dateString, int strLength, const int* separatorPosition, struct pg_tm* tm_date);
void SplitDatestrWithoutSeparator(const char* dateString, struct pg_tm* tm_date);
void AnalyseTime(const char* timeString, struct pg_tm* tm_time);
void SplitTimestrWithoutSeparator(const char* timeString, struct pg_tm* tm_time);
void SplitTimestrBySeparator(
    const char* timeString, int strLength, const int* separatorPosition, struct pg_tm* tm_time);
void CheckDateValidity(struct pg_tm* tm);
void CheckMonthAndDay(struct pg_tm* tm);
void CheckTimeValidity(struct pg_tm* tm);
static int WhetherFebLeapYear(struct pg_tm* tm);
static int WhetherSmallMon(struct pg_tm* tm);
static int WhetherBigMon(struct pg_tm* tm);
static int daydiff_timestamp(const struct pg_tm* tm, const struct pg_tm* tm1, const struct pg_tm* tm2,
                             bool day_fix = false);
#ifdef DOLPHIN
Oid convert_cstring_to_datetime_time(const char* str, Timestamp *datetime, TimeADT *time);
static int cal_weekday_interval(struct pg_tm* tm, bool sunday_is_first_day);
static int b_db_sumdays(int year, int month, int day);
static bool timestampdiff_datetime_internal(int64 *result,  text *units, Timestamp dt1, Timestamp dt2);
static inline long long str2ll_with_endptr(char *start, int tmp_len, int *true_len, int *error);
#endif

void timestamp_FilpSign(pg_tm* tm);
void timestamp_CalculateFields(TimestampTz* dt1, TimestampTz* dt2, fsec_t* fsec, pg_tm* tm, pg_tm* tm1, pg_tm* tm2);

/* compatible with b format datetime and timestamp */
PG_FUNCTION_INFO_V1_PUBLIC(int64_b_format_timestamp);
extern "C" DLL_PUBLIC Datum int64_b_format_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_b_format_timestamp);
extern "C" DLL_PUBLIC Datum int32_b_format_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_b_format_datetime);
PG_FUNCTION_INFO_V1_PUBLIC(int32_b_format_datetime);
extern "C" DLL_PUBLIC Datum int32_b_format_datetime(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_b_format_datetime);
extern "C" DLL_PUBLIC Datum numeric_b_format_datetime(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_b_format_timestamp);
extern "C" DLL_PUBLIC Datum numeric_b_format_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datetime_year_part);
extern "C" DLL_PUBLIC Datum datetime_year_part(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(text_year_part);
extern "C" DLL_PUBLIC Datum text_year_part(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(b_db_sys_real_timestamp);
extern "C" DLL_PUBLIC Datum b_db_sys_real_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_db_statement_start_timestamp);
extern "C" DLL_PUBLIC Datum b_db_statement_start_timestamp(PG_FUNCTION_ARGS);

/* b compatibility time function */
#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(subtime);
extern "C" DLL_PUBLIC Datum subtime(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_format);
extern "C" DLL_PUBLIC Datum time_format(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_format_date);
extern "C" DLL_PUBLIC Datum time_format_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_param1);
extern "C" DLL_PUBLIC Datum timestamp_param1(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_param2);
extern "C" DLL_PUBLIC Datum timestamp_param2(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timediff);
extern "C" DLL_PUBLIC Datum timediff(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_add_numeric);
extern "C" DLL_PUBLIC Datum timestamp_add_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_add_text);
extern "C" DLL_PUBLIC Datum timestamp_add_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(to_seconds);
extern "C" DLL_PUBLIC Datum to_seconds(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(unix_timestamp_no_args);
extern "C" DLL_PUBLIC Datum unix_timestamp_no_args(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(unix_timestamp);
extern "C" DLL_PUBLIC Datum unix_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(utc_timestamp_func);
extern "C" DLL_PUBLIC Datum utc_timestamp_func(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(dayname_text);
extern "C" DLL_PUBLIC Datum dayname_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(dayname_numeric);
extern "C" DLL_PUBLIC Datum dayname_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(monthname_text);
extern "C" DLL_PUBLIC Datum monthname_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(monthname_numeric);
extern "C" DLL_PUBLIC Datum monthname_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(month_text);
extern "C" DLL_PUBLIC Datum month_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(month_numeric);
extern "C" DLL_PUBLIC Datum month_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(last_day_text);
extern "C" DLL_PUBLIC Datum last_day_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(last_day_numeric);
extern "C" DLL_PUBLIC Datum last_day_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_db_date_text);
extern "C" DLL_PUBLIC Datum b_db_date_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_db_date_numeric);
extern "C" DLL_PUBLIC Datum b_db_date_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(dayofmonth_text);
extern "C" DLL_PUBLIC Datum dayofmonth_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(dayofmonth_numeric);
extern "C" DLL_PUBLIC Datum dayofmonth_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(week_text);
extern "C" DLL_PUBLIC Datum week_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(week_numeric);
extern "C" DLL_PUBLIC Datum week_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(yearweek_text);
extern "C" DLL_PUBLIC Datum yearweek_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(yearweek_numeric);
extern "C" DLL_PUBLIC Datum yearweek_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestampdiff_datetime_tt);
extern "C" DLL_PUBLIC Datum timestampdiff_datetime_tt(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestampdiff_datetime_nn);
extern "C" DLL_PUBLIC Datum timestampdiff_datetime_nn(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestampdiff_datetime_tn);
extern "C" DLL_PUBLIC Datum timestampdiff_datetime_tn(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestampdiff_time);
extern "C" DLL_PUBLIC Datum timestampdiff_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestampdiff_time_before_t);
extern "C" DLL_PUBLIC Datum timestampdiff_time_before_t(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestampdiff_time_before_n);
extern "C" DLL_PUBLIC Datum timestampdiff_time_before_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(convert_tz_t);
extern "C" DLL_PUBLIC Datum convert_tz_t(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(convert_tz_n);
extern "C" DLL_PUBLIC Datum convert_tz_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(convert_tz_time);
extern "C" DLL_PUBLIC Datum convert_tz_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(addtime_text);
extern "C" DLL_PUBLIC Datum addtime_text(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(date_format_text);
extern "C" DLL_PUBLIC Datum date_format_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_format_numeric);
extern "C" DLL_PUBLIC Datum date_format_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(str_to_date);
extern "C" DLL_PUBLIC Datum str_to_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(from_unixtime_with_one_arg);
extern "C" DLL_PUBLIC Datum from_unixtime_with_one_arg(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(from_unixtime_with_two_arg);
extern "C" DLL_PUBLIC Datum from_unixtime_with_two_arg(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_uint8);
extern "C" DLL_PUBLIC Datum timestamp_uint8(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datetime_float);
extern "C" DLL_PUBLIC Datum datetime_float(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(timestamp_xor_transfn);
extern "C" DLL_PUBLIC Datum timestamp_xor_transfn(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_agg_finalfn);
extern "C" DLL_PUBLIC Datum timestamp_agg_finalfn(PG_FUNCTION_ARGS);
#endif

/* b format datetime and timestamp type */
static Timestamp int64_b_format_timestamp_internal(bool hasTz, int64 ts, fsec_t fsec);
static int64 integer_b_format_timestamp(bool hasTz, int64 ts);
static void fillZeroBeforeNumericTimestamp(char *str, char *buf);

/* common code for timestamptypmodin and timestamptztypmodin */
static int32 anytimestamp_typmodin(bool istz, ArrayType* ta)
{
    int32 typmod;
    int32* tl = NULL;
    int n;

    tl = ArrayGetIntegerTypmods(ta, &n);

    /*
     * we're not too tense about good error message here because grammar
     * shouldn't allow wrong number of modifiers for TIMESTAMP
     */
    if (n != 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid type modifier")));

    if (*tl < 0)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("TIMESTAMP(%d)%s precision must not be negative", *tl, (istz ? " WITH TIME ZONE" : ""))));
    if (*tl > MAX_TIMESTAMP_PRECISION) {
        ereport(WARNING,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("TIMESTAMP(%d)%s precision reduced to maximum allowed, %d",
                    *tl,
                    (istz ? " WITH TIME ZONE" : ""),
                    MAX_TIMESTAMP_PRECISION)));
        typmod = MAX_TIMESTAMP_PRECISION;
    } else
        typmod = *tl;

    return typmod;
}

/* common code for timestamptypmodout and timestamptztypmodout */
static char* anytimestamp_typmodout(bool istz, int32 typmod)
{
    char* res = (char*)palloc(TYPMODOUT_LEN);
    const char* tz = istz ? " with time zone" : " without time zone";
    int rc = 0;
    if (typmod >= 0) {
        rc = snprintf_s(res, TYPMODOUT_LEN, 63, "(%d)%s", (int)typmod, tz);
        securec_check_ss(rc, "\0", "\0");
    } else {
        rc = snprintf_s(res, TYPMODOUT_LEN, 63, "%s", tz);
        securec_check_ss(rc, "\0", "\0");
    }

    return res;
}

/* b_db_sys_real_timestamp()
 * return sys real timestamp in b compatibility
 */
Datum b_db_sys_real_timestamp(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

        /*lexical analyzer can only recognize positive integers for token ICONST in gram.y*/
    if (typmod > TIMESTAMP_MAX_PRECISION) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Too-big precision %d specified for \'sysdate\'. Maximum is 6.", typmod)));
    }

    TimestampTz sys_timestamp = GetCurrentTimestamp();
    Timestamp result = (Timestamp)DirectFunctionCall1(timestamptz_timestamp, sys_timestamp);
    AdjustTimestampForTypmod(&result, typmod);
    PG_RETURN_TIMESTAMP(result);
}

/* b_db_statement_start_timestamp()
 * return current timestamp in b compatibility
 */
Datum b_db_statement_start_timestamp(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);
    Timestamp result;
    TimestampTz state_start_timestamp;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    double timestamp_guc = DEFAULT_GUC_B_DB_TIMESTAMP;
    int tzp = 0;

    /* lexical analyzer can only recognize positive integers for token ICONST in gram.y */
    if (typmod > TIMESTAMP_MAX_PRECISION) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Too-big precision %d specified for \'now\'. Maximum is 6.", typmod)));
    }

    timestamp_guc = GetSessionContext()->b_db_timestamp;

    /* return current timestamp */
    if (timestamp_guc == DEFAULT_GUC_B_DB_TIMESTAMP) {

        state_start_timestamp = GetCurrentStatementStartTimestamp();
        result = (Timestamp)DirectFunctionCall1(timestamptz_timestamp, state_start_timestamp);

    } else {
    
    /* get the pg_tm struct of b_db_timestamp based on 1970-01-01 00:00:00 UTC, at time zone 0. */
        Unixtimestamp2tm(timestamp_guc, tm, &fsec);
        
        /* find the current session time zone offset. */
        tzp = -DetermineTimeZoneOffset(tm, session_timezone);

        /* Convert the pg_tm structure into timestamp in current timezone. */
        tm2timestamp(tm, fsec, &tzp, &result);

    }

    AdjustTimestampForTypmod(&result, typmod);

    PG_RETURN_TIMESTAMP(result);
}

/*****************************************************************************
 *	 USER I/O ROUTINES														 *
 *****************************************************************************/

/* timestamp_in()
 * Convert a string to internal form.
 */
Datum timestamp_in(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    Timestamp result;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tz;
    int dtype;
    int nf;
    int dterr;
    char* field[MAXDATEFIELDS];
    int ftype[MAXDATEFIELDS];
    char workbuf[MAXDATELEN + MAXDATEFIELDS];
    char* timestamp_fmt = NULL;

    /*
     * this case is used for timestamp format is specified.
     */
    if (4 == PG_NARGS()) {
        timestamp_fmt = PG_GETARG_CSTRING(3);
        if (timestamp_fmt == NULL) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT), errmsg("specified timestamp format is null")));
        }

        /* the following logic shared from to_timestamp(). */
        tz = 0;
        to_timestamp_from_format(tm, &fsec, str, (void*)timestamp_fmt);

        if (tm2timestamp(tm, fsec, &tz, &result) != 0) {
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }
    } else {
        bool flag = false;
    #ifdef DOLPHIN
        bool res = cstring_to_tm(str, tm, fsec);
        flag |= res;
    #endif
        if (flag) {
            tm2timestamp(tm, fsec, NULL, &result);
        } else {
            /*
            * default pg date formatting parsing.
            */
            dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
            if (dterr != 0) {
                DateTimeParseError(dterr, str, "timestamp", fcinfo->can_ignore);
                /*
                 * if error ignorable, function DateTimeParseError reports warning instead, then return current timestamp.
                 */
                PG_RETURN_TIMESTAMP(TIMESTAMP_ZERO);
            }
            if (dterr == 0) {
                if (nf == 1 && ftype[0] == DTK_NUMBER) {
                    /* for example, str = "301210054523", "301210054523.123" */
                    dterr = NumberTimestamp(field[0], tm, &fsec);
                    dtype = DTK_DATE;
                } else {
                    dterr = DecodeDateTimeForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tz);
                }
            }
            if (dterr != 0) {
                DateTimeParseError(dterr, str, "timestamp", fcinfo->can_ignore);
                PG_RETURN_TIMESTAMP(TIMESTAMP_ZERO);
            }
            switch (dtype) {
                case DTK_DATE:
                    if (tm2timestamp(tm, fsec, NULL, &result) != 0)
                        ereport(ERROR,
                            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range: \"%s\"", str)));
                    break;

                case DTK_EPOCH:
                    result = SetEpochTimestamp();
                    break;

                case DTK_LATE:
                    TIMESTAMP_NOEND(result);
                    break;

                case DTK_EARLY:
                    TIMESTAMP_NOBEGIN(result);
                    break;

                case DTK_INVALID:
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("date/time value \"%s\" is no longer supported", str)));

                    TIMESTAMP_NOEND(result);
                    break;

                default:
                    ereport(ERROR,
                        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
                            errmsg("unexpected dtype %d while parsing timestamp \"%s\"", dtype, str)));
                    TIMESTAMP_NOEND(result);
            }
        }
    }

    /*
     * the following logic is unified for timestamp parsing.
     */
    AdjustTimestampForTypmod(&result, typmod);
    PG_RETURN_TIMESTAMP(result);
}

static void fillZeroBeforeNumericTimestamp(char *str, char *buf)
{
    int len = 0;
    char *cp = str;
    error_t rc = EOK;
    while (!(*cp == '\0' || *cp == '.')) {
        ++len;
        ++cp;
    }
    int zeros = 0;
    if (len <= TIMESTAMP_YYMMDD_LEN) {
        zeros = TIMESTAMP_YYMMDD_LEN - len;
    } else if (len <= TIMESTAMP_YYYYMMDD_LEN) {
        zeros = TIMESTAMP_YYYYMMDD_LEN - len;
    } else if (len <= TIMESTAMP_YYMMDDhhmmss_LEN) {
        zeros = TIMESTAMP_YYMMDDhhmmss_LEN - len;
    } else if (len <= TIMESTAMP_YYYYMMDDhhmmss_LEN){
        zeros = TIMESTAMP_YYYYMMDDhhmmss_LEN - len;
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range: \"%s\"", str)));
    }
    for (int i = 0; i < zeros; ++i) {
        buf[i] = '0';
    }
    rc = strncpy_s(buf + zeros, MAXDATELEN + 1 - zeros, str, MAXDATELEN + 1 - zeros);
    securec_check_c(rc, "\0", "\0")
}

Datum numeric_b_format_datetime(PG_FUNCTION_ARGS)
{
    Numeric n = PG_GETARG_NUMERIC(0);
    char *str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(n)));
    char buf[MAXDATELEN + 1];
    fillZeroBeforeNumericTimestamp(str, buf);
    return DirectFunctionCall3(timestamp_in, CStringGetDatum(buf), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
}

Datum numeric_b_format_timestamp(PG_FUNCTION_ARGS)
{
    Numeric n = PG_GETARG_NUMERIC(0);
    char *str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(n)));
    char buf[MAXDATELEN + 1];
    fillZeroBeforeNumericTimestamp(str, buf);
    return DirectFunctionCall3(timestamptz_in, CStringGetDatum(buf), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
}

#ifdef DOLPHIN
int NumberTimestamp(char *str, pg_tm *tm, fsec_t *fsec, unsigned int date_flag)
#else
int NumberTimestamp(char *str, pg_tm *tm, fsec_t *fsec)
#endif
{
    char *cp = str;
    int len = 0;
    int dterr = 0;
    /* validate number str */
    while (*cp != '\0' && *cp != '.') {
        ++len;
        ++cp;
    }
    /* validate len */
    if (len > TIMESTAMP_YYYYMMDDhhmmss_LEN) {
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range: \"%s\"", str)));
    }
    /* treat as date */
    if (len < TIMESTAMP_YYMMDD_LEN) {
        errno_t rc = memset_s(tm, sizeof(*tm), 0, sizeof(*tm));
        securec_check(rc, "\0", "\0");
        *fsec = 0;
#ifdef DOLPHIN
        dterr = NumberDate(str, tm, date_flag);
#else
        dterr = NumberDate(str, tm);
#endif
        return dterr;
    }
    /* 4-digit year, skip date part first
     * update len which now stands for how many digits should appear in time field
     * otherwise zeros would be filled after time_str
     */
    if (len == TIMESTAMP_YYYYMMDD_LEN || len == TIMESTAMP_YYYYMMDDhhmmss_LEN) {
        cp = str + TIMESTAMP_YYYYMMDD_LEN;
    } else {
        cp = str + TIMESTAMP_YYMMDD_LEN;
    }
    char time_str[MAXDATELEN + 1];
    char *tcp = time_str;
    if (*cp == '.') {
        /* treat as a time part, ingore '.'
         * example : 201010.121045 -> 2020-10-10 12:10:45 
         */
        ++cp;
    } 
    /* two characters are grouped together and 3 = TIMESTAMP_YYMMDD_LEN / 2 */
    int cnt = 3;
    /* extract time field first: '101' -> '1001' */
    while (!(*cp == '\0' || *cp == '.')) {
        char next = *(cp + 1);
        if (next == '\0' || next == '.') {
            *tcp = '0';
            *(tcp + 1) = *cp;
            ++cp;
        } else {
            *tcp = *cp;
            *(tcp + 1) = *(cp + 1);
            cp += 2;
        }
        tcp += 2;
        --cnt;
    }
    /* fill zeros after time_str: '1001' -> '100100' */
    for (int i = 0; i < cnt; ++i) {
        *tcp = '0';
        *(tcp + 1) = '0';
        tcp += 2;
    }
    /* has fsec */
    while (*cp != '\0') {
        *tcp = *cp;
        ++tcp;
        ++cp;
    }
    *tcp = '\0';
    dterr = NumberTime(true, time_str, tm, fsec);
    if (dterr) {
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range: \"%s\"", str)));
    }
    /* extract date field */
    if (len == TIMESTAMP_YYYYMMDD_LEN || len == TIMESTAMP_YYYYMMDDhhmmss_LEN) {
        tcp = str + TIMESTAMP_YYYYMMDD_LEN;
    } else {
        tcp = str + TIMESTAMP_YYMMDD_LEN;
    }
    *tcp = '\0';
#ifdef DOLPHIN
    dterr = NumberDate(str, tm, date_flag);
#else
    dterr = NumberDate(str, tm);
#endif
    return dterr;
}

static Timestamp int64_b_format_timestamp_internal(bool hasTz, int64 ts, fsec_t fsec)
{
    Timestamp result;
    struct pg_tm tt, *tm = &tt;
    int tz;
    if (ts < B_FORMAT_DATE_INT_MIN) {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    /* find out how many digits in ts */
    int cnt = 0;                                                                                                                                                        
    int64 date = ts;
    int time = 0;
    int64 tmp = ts;
    while (tmp) {
        ++cnt;
        tmp /= 10;
    }
    if (cnt > TIMESTAMP_YYYYMMDDhhmmss_LEN) {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    /* has time field : YYYYMMDDhhmmss or YYMMDDhhmmss */
    if (cnt > TIMESTAMP_YYYYMMDD_LEN) {
        time = ts % 1000000; /* extract time: hhmmss */
        date = ts / 1000000; /* extract date: YYMMDD or YYYYMMDD */
    } 
    if (int32_b_format_time_internal(tm, true, time, &fsec) || int32_b_format_date_internal(tm, date, true)){
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }

    if (hasTz) {
        /* b format timestamp type */
        tz = DetermineTimeZoneOffset(tm, session_timezone);
        if (tm2timestamp(tm, fsec, &tz, &result) != 0) {
            ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }  
    } else {
        if (tm2timestamp(tm, fsec, NULL, &result) != 0) {
            ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }  
    }
    return result;
}

static int64 integer_b_format_timestamp(bool hasTz, int64 ts)
{
    TimestampTz result;
    result = int64_b_format_timestamp_internal(hasTz, ts, 0);
    PG_RETURN_TIMESTAMP(result);
}

Datum int32_b_format_datetime(PG_FUNCTION_ARGS) 
{
    int64 ts = PG_GETARG_INT64(0);
    PG_RETURN_TIMESTAMP(integer_b_format_timestamp(false, ts));
}

Datum int32_b_format_timestamp(PG_FUNCTION_ARGS)
{
    int64 ts = PG_GETARG_INT64(0);
    PG_RETURN_TIMESTAMP(integer_b_format_timestamp(true, ts));
}

Datum int64_b_format_datetime(PG_FUNCTION_ARGS)
{
    int64 ts = PG_GETARG_INT64(0);
    PG_RETURN_TIMESTAMP(integer_b_format_timestamp(false, ts));
}

Datum int64_b_format_timestamp(PG_FUNCTION_ARGS)
{
    int64 ts = PG_GETARG_INT64(0);
    PG_RETURN_TIMESTAMP(integer_b_format_timestamp(true, ts));
}

/* timestamp_out()
 * Convert a timestamp to external form.
 */
Datum timestamp_out(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    char* result = NULL;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    char buf[MAXDATELEN + 1];

    if (TIMESTAMP_NOT_FINITE(timestamp))
        EncodeSpecialTimestamp(timestamp, buf);
    else if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0)
        EncodeDateTimeForBDatabase(tm, fsec, false, 0, NULL, u_sess->time_cxt.DateStyle, buf);
    else
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *		timestamp_recv			- converts external binary format to timestamp
 *
 * We make no attempt to provide compatibility between int and float
 * timestamp representations ...
 */
Datum timestamp_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    Timestamp timestamp;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;

#ifdef HAVE_INT64_TIMESTAMP
    timestamp = (Timestamp)pq_getmsgint64(buf);
#else
    timestamp = (Timestamp)pq_getmsgfloat8(buf);

    if (isnan(timestamp))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp cannot be NaN")));
#endif

    /* rangecheck: see if timestamp_out would like it */
    if (TIMESTAMP_NOT_FINITE(timestamp))
        /* ok */;
    else if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    AdjustTimestampForTypmod(&timestamp, typmod);

    PG_RETURN_TIMESTAMP(timestamp);
}

/*
 *		timestamp_send			- converts timestamp to binary format
 */
Datum timestamp_send(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
#ifdef HAVE_INT64_TIMESTAMP
    pq_sendint64(&buf, timestamp);
#else
    pq_sendfloat8(&buf, timestamp);
#endif
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum smalldatetime_smaller(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp result;

    /* use timestamp_cmp_internal to be sure this agrees with comparisons */
    if (timestamp_cmp_internal(dt1, dt2) < 0)
        result = dt1;
    else
        result = dt2;
    PG_RETURN_TIMESTAMP(result);
}

Datum smalldatetime_larger(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp result;

    if (timestamp_cmp_internal(dt1, dt2) > 0)
        result = dt1;
    else
        result = dt2;
    PG_RETURN_TIMESTAMP(result);
}

Datum smalldatetime_in(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    Timestamp result;
    fsec_t fsec = 0;
    struct pg_tm tt, *tm = &tt;
    int tz;
    int dtype = 0;
    int nf;
    int dterr;
    char* field[MAXDATEFIELDS];
    int ftype[MAXDATEFIELDS] = {-1};
    char workbuf[MAXDATELEN + MAXDATEFIELDS];
    int sign = 0;
    char* smalldatetime_fmt = NULL;

    /*
     * this case is used for timestamp format is specified.
     */
    if (4 == PG_NARGS()) {
        smalldatetime_fmt = PG_GETARG_CSTRING(3);
        if (smalldatetime_fmt == NULL) {
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT), errmsg("specified smalldatetime format is null")));
        }

        /* the following logic shared from to_timestamp(). */
        tz = 0;
        to_timestamp_from_format(tm, &fsec, str, (void*)smalldatetime_fmt);

        /*
         * the following logic is shared from default smalldatetime formatting parsing.
         */
        if (tm->tm_sec >= 30) {
            sign = 1;
        }
        tm->tm_sec = 0;

        if (tm2timestamp(tm, fsec, &tz, &result) != 0) {
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }
    } else {
        /*
         * default smalldatetime formatting parsing.
         */
        dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
        if (dterr == 0) {
            dterr = DecodeDateTimeForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tz);
            fsec = 0;
        }
        if (dterr != 0) {
            DateTimeParseError(dterr, str, "smalldatetime", fcinfo->can_ignore);
            /* if error ignorable, return epoch time as result */
            GetEpochTime(tm);
        }
        if (tm->tm_sec >= 30) {
            sign = 1;
        }
        tm->tm_sec = 0;
        switch (dtype) {
            case DTK_DATE:
                if (tm2timestamp(tm, fsec, NULL, &result) != 0)
                    ereport(ERROR,
                        (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("smalldatetime out of range: \"%s\"", str)));
                break;

            case DTK_EPOCH:
                result = SetEpochTimestamp();
                break;

            case DTK_LATE:
                TIMESTAMP_NOEND(result);
                break;

            case DTK_EARLY:
                TIMESTAMP_NOBEGIN(result);
                break;

            case DTK_INVALID:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("date/time value \"%s\" is no longer supported", str)));

                TIMESTAMP_NOEND(result);
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_WRONG_OBJECT_TYPE),
                        errmsg("unexpected dtype %d while parsing smalldatetime \"%s\"", dtype, str)));
                TIMESTAMP_NOEND(result);
        }
    }

    /*
     * the following logic is unified for smalldatetime parsing.
     */
    AdjustTimestampForTypmod(&result, typmod);

    /*minute add 1 if second >= 30*/
    if (1 == sign) {
        Interval spanTime;
        Interval* span = NULL;
        spanTime.time = 60000000;
        spanTime.day = 0;
        spanTime.month = 0;
        span = &spanTime;
        result = DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(result), PointerGetDatum(span));
    }

    PG_RETURN_TIMESTAMP(result);
}

/* timestamp_out()
 * Convert a timestamp to external form.
 */
Datum smalldatetime_out(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    char* result = NULL;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    char buf[MAXDATELEN + 1] = {'\0'};
    if (TIMESTAMP_NOT_FINITE(timestamp))
        EncodeSpecialTimestamp(timestamp, buf);
    else if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0)
        EncodeDateTime(tm, fsec, false, 0, NULL, u_sess->time_cxt.DateStyle, buf);
    else
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("smalldatetime out of range")));
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}
/*
 *		timestamp_recv			- converts external binary format to timestamp
 *
 * We make no attempt to provide compatibility between int and float
 * timestamp representations ...
 */
Datum smalldatetime_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    Timestamp timestamp;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;

#ifdef HAVE_INT64_TIMESTAMP
    timestamp = (Timestamp)pq_getmsgint64(buf);
#else
    timestamp = (Timestamp)pq_getmsgfloat8(buf);

    if (isnan(timestamp))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("smalldatetime cannot be NaN")));
#endif

    /* rangecheck: see if timestamp_out would like it */
    if (TIMESTAMP_NOT_FINITE(timestamp))
        /* ok */;
    else if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("smalldatetime out of range")));

    AdjustTimestampForTypmod(&timestamp, typmod);

    PG_RETURN_TIMESTAMP(timestamp);
}

/*
 *		timestamp_send			- converts timestamp to binary format
 */
Datum smalldatetime_send(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
#ifdef HAVE_INT64_TIMESTAMP
    pq_sendint64(&buf, timestamp);
#else
    pq_sendfloat8(&buf, timestamp);
#endif
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum smalldatetime_eq(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum smalldatetime_ne(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum smalldatetime_lt(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum smalldatetime_gt(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum smalldatetime_le(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum smalldatetime_ge(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

Datum smalldatetime_cmp(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_INT32(timestamp_cmp_internal(dt1, dt2));
}
Datum smalldatetime_hash(PG_FUNCTION_ARGS)
{
    /* We can use either hashint8 or hashfloat8 directly */
#ifdef HAVE_INT64_TIMESTAMP
    return hashint8(fcinfo);
#else
    return hashfloat8(fcinfo);
#endif
}

Datum timestamptypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);

    PG_RETURN_INT32(anytimestamp_typmodin(false, ta));
}

Datum timestamptypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    PG_RETURN_CSTRING(anytimestamp_typmodout(false, typmod));
}

/* timestamp_transform()
 * Flatten calls to timestamp_scale() and timestamptz_scale() that solely
 * represent increases in allowed precision.
 */
Datum timestamp_transform(PG_FUNCTION_ARGS)
{
    PG_RETURN_POINTER(TemporalTransform(MAX_TIMESTAMP_PRECISION, (Node*)PG_GETARG_POINTER(0)));
}

/* timestamp_scale()
 * Adjust time type for specified scale factor.
 * Used by openGauss type system to stuff columns.
 */
Datum timestamp_scale(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    int32 typmod = PG_GETARG_INT32(1);
    Timestamp result;

    result = timestamp;

    AdjustTimestampForTypmod(&result, typmod);

    PG_RETURN_TIMESTAMP(result);
}

static void AdjustTimestampForTypmod(Timestamp* time, int32 typmod)
{
#ifdef HAVE_INT64_TIMESTAMP
    static const int64 TimestampScales[MAX_TIMESTAMP_PRECISION + 1] = {INT64CONST(1000000),
        INT64CONST(100000),
        INT64CONST(10000),
        INT64CONST(1000),
        INT64CONST(100),
        INT64CONST(10),
        INT64CONST(1)};

    static const int64 TimestampOffsets[MAX_TIMESTAMP_PRECISION + 1] = {INT64CONST(500000),
        INT64CONST(50000),
        INT64CONST(5000),
        INT64CONST(500),
        INT64CONST(50),
        INT64CONST(5),
        INT64CONST(0)};
#else
    static const double TimestampScales[MAX_TIMESTAMP_PRECISION + 1] = {1, 10, 100, 1000, 10000, 100000, 1000000};
#endif

    if (!TIMESTAMP_NOT_FINITE(*time) && (typmod != -1) && (typmod != MAX_TIMESTAMP_PRECISION)) {
        if (typmod < 0 || typmod > MAX_TIMESTAMP_PRECISION)
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("timestamp(%d) precision must be between %d and %d", typmod, 0, MAX_TIMESTAMP_PRECISION)));

            /*
             * Note: this round-to-nearest code is not completely consistent about
             * rounding values that are exactly halfway between integral values.
             * On most platforms, rint() will implement round-to-nearest-even, but
             * the integer code always rounds up (away from zero).	Is it worth
             * trying to be consistent?
             */
#ifdef HAVE_INT64_TIMESTAMP
        if (*time >= INT64CONST(0)) {
            *time = ((*time + TimestampOffsets[typmod]) / TimestampScales[typmod]) * TimestampScales[typmod];
        } else {
            *time = -((((-*time) + TimestampOffsets[typmod]) / TimestampScales[typmod]) * TimestampScales[typmod]);
        }
#else
        *time = rint((double)*time * TimestampScales[typmod]) / TimestampScales[typmod];
#endif
    }
}

/* timestamptz_in()
 * Convert a string to internal form.
 */
Datum timestamptz_in(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimestampTz result;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tz;
    int dtype;
    int nf;
    int dterr;
    char* field[MAXDATEFIELDS];
    int ftype[MAXDATEFIELDS];
    char workbuf[MAXDATELEN + MAXDATEFIELDS];

    bool flag = false;
#ifdef DOLPHIN
    bool res = cstring_to_tm(str, tm, fsec);
    flag |= res;
#endif

    if (flag) {
        tm2timestamp(tm, fsec, NULL, &result);
        result = timestamp2timestamptz(result);
    } else {
        dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
        if (dterr != 0) {
            DateTimeParseError(dterr, str, "timestamp", fcinfo->can_ignore);
            /*
             * if error ignorable, function DateTimeParseError reports warning instead, then return current timestamp.
             */
            PG_RETURN_TIMESTAMP(GetCurrentTimestamp());
        }
        if (dterr == 0) {
            if (nf == 1 && ftype[0] == DTK_NUMBER) {
                /* for example, str = "301210054523", "301210054523.123" */
                dterr = NumberTimestamp(field[0], tm, &fsec);
                tz = DetermineTimeZoneOffset(tm, session_timezone);
                dtype = DTK_DATE;
            } else {
                dterr = DecodeDateTimeForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tz);
            }
        }
        if (dterr != 0) {
            DateTimeParseError(dterr, str, "timestamp", fcinfo->can_ignore);
            PG_RETURN_TIMESTAMP(GetCurrentTimestamp());
        }
        switch (dtype) {
            case DTK_DATE:
                if (tm2timestamp(tm, fsec, &tz, &result) != 0)
                    ereport(ERROR,
                        (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range: \"%s\"", str)));
                break;

            case DTK_EPOCH:
                result = SetEpochTimestamp();
                break;

            case DTK_LATE:
                TIMESTAMP_NOEND(result);
                break;

            case DTK_EARLY:
                TIMESTAMP_NOBEGIN(result);
                break;

            case DTK_INVALID:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), 
                        errmsg("date/time value \"%s\" is no longer supported", str)));
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_WRONG_OBJECT_TYPE),
                        errmsg("unexpected dtype %d while parsing timestamptz \"%s\"", dtype, str)));
        }
    }

    AdjustTimestampForTypmod(&result, typmod);

    PG_RETURN_TIMESTAMPTZ(result);
}

/* timestamptz_out()
 * Convert a timestamp to external form.
 */
Datum timestamptz_out(PG_FUNCTION_ARGS)
{
    TimestampTz dt = PG_GETARG_TIMESTAMPTZ(0);
    char* result = NULL;
    int tz;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    const char* tzn = NULL;
    char buf[MAXDATELEN + 1];

    if (TIMESTAMP_NOT_FINITE(dt))
        EncodeSpecialTimestamp(dt, buf);
    else if (timestamp2tm(dt, &tz, tm, &fsec, &tzn, NULL) == 0)
        EncodeDateTimeForBDatabase(tm, fsec, true, tz, tzn, u_sess->time_cxt.DateStyle, buf);
    else
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *		timestamptz_recv			- converts external binary format to timestamptz
 *
 * We make no attempt to provide compatibility between int and float
 * timestamp representations ...
 */
Datum timestamptz_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimestampTz timestamp;
    int tz;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;

#ifdef HAVE_INT64_TIMESTAMP
    timestamp = (TimestampTz)pq_getmsgint64(buf);
#else
    timestamp = (TimestampTz)pq_getmsgfloat8(buf);
#endif

    /* rangecheck: see if timestamptz_out would like it */
    if (TIMESTAMP_NOT_FINITE(timestamp))
        /* ok */;
    else if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    AdjustTimestampForTypmod(&timestamp, typmod);

    PG_RETURN_TIMESTAMPTZ(timestamp);
}

/*
 *		timestamptz_send			- converts timestamptz to binary format
 */
Datum timestamptz_send(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
#ifdef HAVE_INT64_TIMESTAMP
    pq_sendint64(&buf, timestamp);
#else
    pq_sendfloat8(&buf, timestamp);
#endif
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum timestamptztypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);

    PG_RETURN_INT32(anytimestamp_typmodin(true, ta));
}

Datum timestamptztypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    PG_RETURN_CSTRING(anytimestamp_typmodout(true, typmod));
}

/* timestamptz_scale()
 * Adjust time type for specified scale factor.
 * Used by openGauss type system to stuff columns.
 */
Datum timestamptz_scale(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    int32 typmod = PG_GETARG_INT32(1);
    TimestampTz result;

    result = timestamp;

    AdjustTimestampForTypmod(&result, typmod);

    PG_RETURN_TIMESTAMPTZ(result);
}

/* interval_in()
 * Convert a string to internal form.
 *
 * External format(s):
 *	Uses the generic date/time parsing and decoding routines.
 */
Datum interval_in(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32        typmod = PG_GETARG_INT32(2);
    Interval     *result = NULL;
    result = char_to_interval(str, typmod, fcinfo->can_ignore);

    AdjustIntervalForTypmod(result, typmod);

    PG_RETURN_INTERVAL_P(result);
}

/* interval_out()
 * Convert a time span to external form.
 */
Datum interval_out(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    char* result = NULL;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    char buf[MAXDATELEN + 1];

    if (interval2tm(*span, tm, &fsec) != 0)
        ereport(ERROR, (errcode(ERRCODE_MOST_SPECIFIC_TYPE_MISMATCH), errmsg("could not convert interval to tm")));

    EncodeInterval(tm, fsec, u_sess->attr.attr_common.IntervalStyle, buf);

    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *		interval_recv			- converts external binary format to interval
 */
Datum interval_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    Interval* interval = NULL;

    interval = (Interval*)palloc(sizeof(Interval));

#ifdef HAVE_INT64_TIMESTAMP
    interval->time = pq_getmsgint64(buf);
#else
    interval->time = pq_getmsgfloat8(buf);
#endif
    interval->day = pq_getmsgint(buf, sizeof(interval->day));
    interval->month = pq_getmsgint(buf, sizeof(interval->month));

    AdjustIntervalForTypmod(interval, typmod);

    PG_RETURN_INTERVAL_P(interval);
}

/*
 *		interval_send			- converts interval to binary format
 */
Datum interval_send(PG_FUNCTION_ARGS)
{
    Interval* interval = PG_GETARG_INTERVAL_P(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
#ifdef HAVE_INT64_TIMESTAMP
    pq_sendint64(&buf, interval->time);
#else
    pq_sendfloat8(&buf, interval->time);
#endif
    pq_sendint32(&buf, interval->day);
    pq_sendint32(&buf, interval->month);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 * The interval typmod stores a "range" in its high 16 bits and a "precision"
 * in its low 16 bits.	Both contribute to defining the resolution of the
 * type.  Range addresses resolution granules larger than one second, and
 * precision specifies resolution below one second.  This representation can
 * express all SQL standard resolutions, but we implement them all in terms of
 * truncating rightward from some position.  Range is a bitmap of permitted
 * fields, but only the temporally-smallest such field is significant to our
 * calculations.  Precision is a count of sub-second decimal places to retain.
 * Setting all bits (INTERVAL_FULL_PRECISION) gives the same truncation
 * semantics as choosing MAX_INTERVAL_PRECISION.
 */
Datum intervaltypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);
    int32* tl = NULL;
    int n;
    int32 typmod;

    tl = ArrayGetIntegerTypmods(ta, &n);

    /*
     * tl[0] - interval range (fields bitmask)	tl[1] - precision (optional)
     *
     * Note we must validate tl[0] even though it's normally guaranteed
     * correct by the grammar --- consider SELECT 'foo'::"interval"(1000).
     */
    if (n > 0) {
        switch (tl[0]) {
            case INTERVAL_MASK(YEAR):
            case INTERVAL_MASK(MONTH):
            case INTERVAL_MASK(DAY):
            case INTERVAL_MASK(HOUR):
            case INTERVAL_MASK(MINUTE):
            case INTERVAL_MASK(SECOND):
            case INTERVAL_MASK(YEAR) | INTERVAL_MASK(MONTH):
            case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR):
            case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
            case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
            case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            case INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            case INTERVAL_FULL_RANGE:
                /* all OK */
                break;
            default:
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid INTERVAL type modifier")));
        }
    }

    if (n == 1) {
        if (tl[0] != INTERVAL_FULL_RANGE)
            typmod = INTERVAL_TYPMOD(INTERVAL_FULL_PRECISION, (unsigned int32)tl[0]);
        else
            typmod = -1;
    } else if (n == 2) {
        if (tl[1] < 0)
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("INTERVAL(%d) precision must not be negative", tl[1])));
        if (tl[1] > MAX_INTERVAL_PRECISION) {
            ereport(WARNING,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("INTERVAL(%d) precision reduced to maximum allowed, %d", tl[1], MAX_INTERVAL_PRECISION)));
            typmod = INTERVAL_TYPMOD(MAX_INTERVAL_PRECISION, (unsigned int32)tl[0]);
        } else
            typmod = INTERVAL_TYPMOD((unsigned int32)tl[1], (unsigned int32)tl[0]);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid INTERVAL type modifier")));
        typmod = 0; /* keep compiler quiet */
    }

    PG_RETURN_INT32(typmod);
}

Datum intervaltypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);
    char* res = (char*)palloc(TYPMODOUT_LEN);
    int fields;
    int precision;
    const char* fieldstr = NULL;
    int rc = 0;
    if (typmod < 0) {
        *res = '\0';
        PG_RETURN_CSTRING(res);
    }

    fields = INTERVAL_RANGE(typmod);
    precision = INTERVAL_PRECISION(typmod);

    switch (fields) {
        case INTERVAL_MASK(YEAR):
            fieldstr = " year";
            break;
        case INTERVAL_MASK(MONTH):
            fieldstr = " month";
            break;
        case INTERVAL_MASK(DAY):
            fieldstr = " day";
            break;
        case INTERVAL_MASK(HOUR):
            fieldstr = " hour";
            break;
        case INTERVAL_MASK(MINUTE):
            fieldstr = " minute";
            break;
        case INTERVAL_MASK(SECOND):
            fieldstr = " second";
            break;
        case INTERVAL_MASK(YEAR) | INTERVAL_MASK(MONTH):
            fieldstr = " year to month";
            break;
        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR):
            fieldstr = " day to hour";
            break;
        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
            fieldstr = " day to minute";
            break;
        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            fieldstr = " day to second";
            break;
        case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
            fieldstr = " hour to minute";
            break;
        case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            fieldstr = " hour to second";
            break;
        case INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            fieldstr = " minute to second";
            break;
        case INTERVAL_FULL_RANGE:
            fieldstr = "";
            break;
        default:
            ereport(ERROR, (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE), errmsg("invalid INTERVAL typmod: 0x%x", typmod)));
            fieldstr = "";
            break;
    }

    if (precision != INTERVAL_FULL_PRECISION) {
        rc = snprintf_s(res, TYPMODOUT_LEN, 63, "%s(%d)", fieldstr, precision);
        securec_check_ss(rc, "\0", "\0");
    } else {
        rc = snprintf_s(res, TYPMODOUT_LEN, 63, "%s", fieldstr);
        securec_check_ss(rc, "\0", "\0");
    }
    PG_RETURN_CSTRING(res);
}

/*
 * Given an interval typmod value, return a code for the least-significant
 * field that the typmod allows to be nonzero, for instance given
 * INTERVAL DAY TO HOUR we want to identify "hour".
 *
 * The results should be ordered by field significance, which means
 * we can't use the dt.h macros YEAR etc, because for some odd reason
 * they aren't ordered that way.  Instead, arbitrarily represent
 * SECOND = 0, MINUTE = 1, HOUR = 2, DAY = 3, MONTH = 4, YEAR = 5.
 */
static int intervaltypmodleastfield(int32 typmod)
{
    if (typmod < 0)
        return 0; /* SECOND */

    switch (INTERVAL_RANGE(typmod)) {
        case INTERVAL_MASK(YEAR):
            return 5; /* YEAR */
        case INTERVAL_MASK(MONTH):
            return 4; /* MONTH */
        case INTERVAL_MASK(DAY):
            return 3; /* DAY */
        case INTERVAL_MASK(HOUR):
            return 2; /* HOUR */
        case INTERVAL_MASK(MINUTE):
            return 1; /* MINUTE */
        case INTERVAL_MASK(SECOND):
            return 0; /* SECOND */
        case INTERVAL_MASK(YEAR) | INTERVAL_MASK(MONTH):
            return 4; /* MONTH */
        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR):
            return 2; /* HOUR */
        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
            return 1; /* MINUTE */
        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            return 0; /* SECOND */
        case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
            return 1; /* MINUTE */
        case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            return 0; /* SECOND */
        case INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
            return 0; /* SECOND */
        default:
            return 0;
    }
    return 0; /* can't get here, but keep compiler quiet */
}

/* interval_transform()
 * Flatten superfluous calls to interval_scale().  The interval typmod is
 * complex to permit accepting and regurgitating all SQL standard variations.
 * For truncation purposes, it boils down to a single, simple granularity.
 */
Datum interval_transform(PG_FUNCTION_ARGS)
{
    FuncExpr* expr = (FuncExpr*)PG_GETARG_POINTER(0);
    Node* ret = NULL;
    Node* typmod = NULL;

    Assert(IsA(expr, FuncExpr));
    Assert(list_length(expr->args) >= 2);

    typmod = (Node*)lsecond(expr->args);

    if (IsA(typmod, Const) && !((Const*)typmod)->constisnull) {
        Node* source = (Node*)linitial(expr->args);
        int32 new_typmod = DatumGetInt32(((Const*)typmod)->constvalue);
        bool noop = false;

        if (new_typmod < 0)
            noop = true;
        else {
            int32 old_typmod = exprTypmod(source);
            int old_least_field;
            int new_least_field;
            int old_precis;
            int new_precis;

            old_least_field = intervaltypmodleastfield(old_typmod);
            new_least_field = intervaltypmodleastfield(new_typmod);
            if (old_typmod < 0) {
                old_precis = INTERVAL_FULL_PRECISION;
            } else {
                old_precis = INTERVAL_PRECISION(old_typmod);
            }
            new_precis = INTERVAL_PRECISION(new_typmod);

            /*
             * Cast is a no-op if least field stays the same or decreases
             * while precision stays the same or increases.  But precision,
             * which is to say, sub-second precision, only affects ranges that
             * include SECOND.
             */
            noop =
                (new_least_field <= old_least_field) &&
                (old_least_field > 0 /* SECOND */ || new_precis >= MAX_INTERVAL_PRECISION || new_precis >= old_precis);
        }
        if (noop)
            ret = relabel_to_typmod(source, new_typmod);
    }

    PG_RETURN_POINTER(ret);
}

/* interval_scale()
 * Adjust interval type for specified fields.
 * Used by openGauss type system to stuff columns.
 */
Datum interval_scale(PG_FUNCTION_ARGS)
{
    Interval* interval = PG_GETARG_INTERVAL_P(0);
    int32 typmod = PG_GETARG_INT32(1);
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));
    *result = *interval;

    AdjustIntervalForTypmod(result, typmod);

    PG_RETURN_INTERVAL_P(result);
}

/*
 *	Adjust interval for specified precision, in both YEAR to SECOND
 *	range and sub-second precision.
 */
static void AdjustIntervalForTypmod(Interval* interval, int32 typmod)
{
#ifdef HAVE_INT64_TIMESTAMP
    static const int64 IntervalScales[MAX_INTERVAL_PRECISION + 1] = {INT64CONST(1000000),
        INT64CONST(100000),
        INT64CONST(10000),
        INT64CONST(1000),
        INT64CONST(100),
        INT64CONST(10),
        INT64CONST(1)};

    static const int64 IntervalOffsets[MAX_INTERVAL_PRECISION + 1] = {INT64CONST(500000),
        INT64CONST(50000),
        INT64CONST(5000),
        INT64CONST(500),
        INT64CONST(50),
        INT64CONST(5),
        INT64CONST(0)};
#else
    static const double IntervalScales[MAX_INTERVAL_PRECISION + 1] = {1, 10, 100, 1000, 10000, 100000, 1000000};
#endif

    /*
     * Unspecified range and precision? Then not necessary to adjust. Setting
     * typmod to -1 is the convention for all data types.
     */
    if (typmod >= 0) {
        int range = INTERVAL_RANGE(typmod);
        int precision = INTERVAL_PRECISION(typmod);
        // mode character decided by typmod
        char type_mode = ' ';

        /*
         * Our interpretation of intervals with a limited set of fields is
         * that fields to the right of the last one specified are zeroed out,
         * but those to the left of it remain valid.  Thus for example there
         * is no operational difference between INTERVAL YEAR TO MONTH and
         * INTERVAL MONTH.	In some cases we could meaningfully enforce that
         * higher-order fields are zero; for example INTERVAL DAY could reject
         * nonzero "month" field.  However that seems a bit pointless when we
         * can't do it consistently.  (We cannot enforce a range limit on the
         * highest expected field, since we do not have any equivalent of
         * SQL's <interval leading field precision>.)  If we ever decide to
         * revisit this, interval_transform will likely require adjusting.
         *
         * Note: before PG 8.4 we interpreted a limited set of fields as
         * actually causing a "modulo" operation on a given value, potentially
         * losing high-order as well as low-order information.	But there is
         * no support for such behavior in the standard, and it seems fairly
         * undesirable on data consistency grounds anyway.	Now we only
         * perform truncation or rounding of low-order fields.
         */
        if (range == INTERVAL_FULL_RANGE) {
            /* Do nothing... */
        } else if (range == INTERVAL_MASK(YEAR)) {
            interval->month = (interval->month / MONTHS_PER_YEAR) * MONTHS_PER_YEAR;
            interval->day = 0;
            interval->time = 0;
        } else if (range == INTERVAL_MASK(MONTH)) {
            interval->day = 0;
            interval->time = 0;
        }
        /* YEAR TO MONTH */
        else if (range == (INTERVAL_MASK(YEAR) | INTERVAL_MASK(MONTH))) {
            interval->day = 0;
            interval->time = 0;
        } else if (range == INTERVAL_MASK(DAY)) {
            interval->time = 0;
        } else if (range == INTERVAL_MASK(HOUR)) {
            // set hour mode character
            type_mode = 'H';
        } else if (range == INTERVAL_MASK(MINUTE)) {
            // set minute mode character
            type_mode = 'M';
        } else if (range == INTERVAL_MASK(SECOND)) {
            /* fractional-second rounding will be dealt with below */
            // set second mode character
            type_mode = 'S';
        }
        /* DAY TO HOUR */
        else if (range == (INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR))) {
            // set hour mode character
            type_mode = 'H';
        }
        /* DAY TO MINUTE */
        else if (range == (INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE))) {
            // set minute mode character
            type_mode = 'M';
        }
        /* DAY TO SECOND */
        else if (range == (INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND))) {
            /* fractional-second rounding will be dealt with below */

            // set second mode character
            type_mode = 'S';
        }
        /* HOUR TO MINUTE */
        else if (range == (INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE))) {
            // set minute mode character
            type_mode = 'M';
        }
        /* HOUR TO SECOND */
        else if (range == (INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND))) {
            /* fractional-second rounding will be dealt with below */
            // set second mode character
            type_mode = 'S';
        }
        /* MINUTE TO SECOND */
        else if (range == (INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND))) {
            /* fractional-second rounding will be dealt with below */
            // set second mode character
            type_mode = 'S';
        } else
            ereport(
                ERROR, (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE), errmsg("unrecognized interval typmod: %d", typmod)));

        // adjust interval's format
        interval_format_adjust(interval, type_mode);

        /* Need to adjust subsecond precision? */
        if (precision != INTERVAL_FULL_PRECISION) {
            if (precision < 0 || precision > MAX_INTERVAL_PRECISION)
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg(
                            "interval(%d) precision must be between %d and %d", precision, 0, MAX_INTERVAL_PRECISION)));

                /*
                 * Note: this round-to-nearest code is not completely consistent
                 * about rounding values that are exactly halfway between integral
                 * values.	On most platforms, rint() will implement
                 * round-to-nearest-even, but the integer code always rounds up
                 * (away from zero).  Is it worth trying to be consistent?
                 */
#ifdef HAVE_INT64_TIMESTAMP
            if (interval->time >= INT64CONST(0)) {
                interval->time = ((interval->time + IntervalOffsets[precision]) / IntervalScales[precision]) *
                                 IntervalScales[precision];
            } else {
                interval->time = -(((-interval->time + IntervalOffsets[precision]) / IntervalScales[precision]) *
                                   IntervalScales[precision]);
            }
#else
            interval->time = rint(((double)interval->time) * IntervalScales[precision]) / IntervalScales[precision];
#endif
        }
    }
}

/* EncodeSpecialTimestamp()
 * Convert reserved timestamp data type to string.
 */
static void EncodeSpecialTimestamp(Timestamp dt, char* str)
{
    int rc = 0;
    if (TIMESTAMP_IS_NOBEGIN(dt)) {
        rc = strcpy_s(str, MAXDATELEN + 1, EARLY);
        securec_check(rc, "\0", "\0");
    } else if (TIMESTAMP_IS_NOEND(dt)) {
        rc = strcpy_s(str, MAXDATELEN + 1, LATE);
        securec_check(rc, "\0", "\0");
    } else {
        /* shouldn't happen */
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_ARGUMENT_FOR_NTH_VALUE), errmsg("invalid argument for EncodeSpecialTimestamp")));
    }
}

Datum now(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(GetCurrentTransactionStartTimestamp());
}

Datum statement_timestamp(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(GetCurrentStmtsysTimestamp());
}

Datum clock_timestamp(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(GetCurrentTimestamp());
}

Datum pg_system_timestamp(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(GetCurrentStmtsysTimestamp());
}

Datum pg_postmaster_start_time(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(t_thrd.time_cxt.pg_start_time);
}

Datum get_node_stat_reset_time(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(g_instance.stat_cxt.NodeStatResetTime);
}

Datum pg_conf_load_time(PG_FUNCTION_ARGS)
{
    PG_RETURN_TIMESTAMPTZ(t_thrd.time_cxt.pg_reload_time);
}

/*
 * GetCurrentTimestamp -- get the current operating system time
 *
 * Result is in the form of a TimestampTz value, and is expressed to the
 * full precision of the gettimeofday() syscall
 */
TimestampTz GetCurrentTimestamp(void)
{
    TimestampTz result;
    struct timeval tp;

    gettimeofday(&tp, NULL);

    result = (TimestampTz)tp.tv_sec - ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY);

#ifdef HAVE_INT64_TIMESTAMP
    result = (result * USECS_PER_SEC) + tp.tv_usec;
#else
    result = result + (tp.tv_usec / 1000000.0);
#endif

    return result;
}

/*
 * TimestampDifference -- convert the difference between two timestamps
 *		into integer seconds and microseconds
 *
 * Both inputs must be ordinary finite timestamps (in current usage,
 * they'll be results from GetCurrentTimestamp()).
 *
 * We expect start_time <= stop_time.  If not, we return zeroes; for current
 * callers there is no need to be tense about which way division rounds on
 * negative inputs.
 */
void TimestampDifference(TimestampTz start_time, TimestampTz stop_time, long* secs, int* microsecs)
{
    TimestampTz diff = stop_time - start_time;

    if (diff <= 0) {
        *secs = 0;
        *microsecs = 0;
    } else {
#ifdef HAVE_INT64_TIMESTAMP
        *secs = (long)(diff / USECS_PER_SEC);
        *microsecs = (int)(diff % USECS_PER_SEC);
#else
        *secs = (long)diff;
        *microsecs = (int)((diff - *secs) * 1000000.0);
#endif
    }
}

/* Compute time interval and milliseconds as result */
int ComputeTimeStamp(TimestampTz start)
{
    TimestampTz end = GetCurrentTimestamp();

    long secs = 0;
    int usecs = 0;
    int ms;
    TimestampDifference(start, end, &secs, &usecs);

    ms = ((int)secs * 1000) + (usecs / 1000);

    return ms;
}

/*
 * TimestampDifferenceExceeds -- report whether the difference between two
 *		timestamps is >= a threshold (expressed in milliseconds)
 *
 * Both inputs must be ordinary finite timestamps (in current usage,
 * they'll be results from GetCurrentTimestamp()).
 */
bool TimestampDifferenceExceeds(TimestampTz start_time, TimestampTz stop_time, int msec)
{
    TimestampTz diff = stop_time - start_time;

#ifdef HAVE_INT64_TIMESTAMP
    return (diff >= msec * INT64CONST(1000));
#else
    return (diff * 1000.0 >= msec);
#endif
}

/*
 * Convert a time_t to TimestampTz.
 *
 * We do not use time_t internally in openGauss, but this is provided for use
 * by functions that need to interpret, say, a stat(2) result.
 *
 * To avoid having the function's ABI vary depending on the width of time_t,
 * we declare the argument as pg_time_t, which is cast-compatible with
 * time_t but always 64 bits wide (unless the platform has no 64-bit type).
 * This detail should be invisible to callers, at least at source code level.
 */
TimestampTz time_t_to_timestamptz(pg_time_t tm)
{
    TimestampTz result;

    result = (TimestampTz)tm - ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY);

#ifdef HAVE_INT64_TIMESTAMP
    result *= USECS_PER_SEC;
#endif

    return result;
}

/*
 * Convert a TimestampTz to time_t.
 *
 * This too is just marginally useful, but some places need it.
 *
 * To avoid having the function's ABI vary depending on the width of time_t,
 * we declare the result as pg_time_t, which is cast-compatible with
 * time_t but always 64 bits wide (unless the platform has no 64-bit type).
 * This detail should be invisible to callers, at least at source code level.
 */
pg_time_t timestamptz_to_time_t(TimestampTz t)
{
    pg_time_t result;

#ifdef HAVE_INT64_TIMESTAMP
    result = (pg_time_t)(t / USECS_PER_SEC + ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY));
#else
    result = (pg_time_t)(t + ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY));
#endif

    return result;
}

/*
 * Produce a C-string representation of a TimestampTz.
 *
 * This is mostly for use in emitting messages.  The primary difference
 * from timestamptz_out is that we force the output format to ISO.	Note
 * also that the result is in a static buffer, not pstrdup'd.
 */
const char* timestamptz_to_str(TimestampTz t)
{
    static THR_LOCAL char buf[MAXDATELEN + 1];
    int tz;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    const char* tzn = NULL;

    if (TIMESTAMP_NOT_FINITE(t))
        EncodeSpecialTimestamp(t, buf);
    else if (timestamp2tm(t, &tz, tm, &fsec, &tzn, NULL) == 0)
        EncodeDateTime(tm, fsec, true, tz, tzn, USE_ISO_DATES, buf);
    else
        strlcpy(buf, "(timestamp out of range)", sizeof(buf));

    return buf;
}

void dt2time(Timestamp jd, int* hour, int* min, int* sec, fsec_t* fsec)
{
    TimeOffset time;

    time = jd;

#ifdef HAVE_INT64_TIMESTAMP
    *hour = time / USECS_PER_HOUR;
    time -= (*hour) * USECS_PER_HOUR;
    *min = time / USECS_PER_MINUTE;
    time -= (*min) * USECS_PER_MINUTE;
    *sec = time / USECS_PER_SEC;
    *fsec = time - (*sec * USECS_PER_SEC);
#else
    *hour = time / SECS_PER_HOUR;
    time -= (*hour) * SECS_PER_HOUR;
    *min = time / SECS_PER_MINUTE;
    time -= (*min) * SECS_PER_MINUTE;
    *sec = time;
    *fsec = time - *sec;
#endif
} /* dt2time() */

/*
 * timestamp2tm() - Convert timestamp data type to POSIX time structure.
 *
 * Note that year is _not_ 1900-based, but is an explicit full value.
 * Also, month is one-based, _not_ zero-based.
 * Returns:
 *	 0 on success
 *	-1 on out of range
 *
 * If attimezone is NULL, the global timezone (including possibly brute forced
 * timezone) will be used.
 */
int timestamp2tm(Timestamp dt, int* tzp, struct pg_tm* tm, fsec_t* fsec, const char** tzn, pg_tz* attimezone)
{
    Timestamp date;
    Timestamp time;
    pg_time_t utime;

    /*
     * If u_sess->time_cxt.HasCTZSet is true then we have a brute force time zone specified. Go
     * ahead and rotate to the local time zone since we will later bypass any
     * calls which adjust the tm fields.
     */
    if (attimezone == NULL && u_sess->time_cxt.HasCTZSet && tzp != NULL) {
#ifdef HAVE_INT64_TIMESTAMP
        dt -= u_sess->time_cxt.CTimeZone * USECS_PER_SEC;
#else
        dt -= u_sess->time_cxt.CTimeZone;
#endif
    }

#ifdef HAVE_INT64_TIMESTAMP
    time = dt;
    TMODULO(time, date, USECS_PER_DAY);

    if (time < INT64CONST(0)) {
        time += USECS_PER_DAY;
        date -= 1;
    }

    /* add offset to go from J2000 back to standard Julian date */
    date += POSTGRES_EPOCH_JDATE;

    /* Julian day routine does not work for negative Julian days */
    if (date < 0 || date > (Timestamp)INT_MAX)
        return -1;

    j2date((int)date, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
    dt2time(time, &tm->tm_hour, &tm->tm_min, &tm->tm_sec, fsec);
#else
    time = dt;
    TMODULO(time, date, (double)SECS_PER_DAY);

    if (time < 0) {
        time += SECS_PER_DAY;
        date -= 1;
    }

    /* add offset to go from J2000 back to standard Julian date */
    date += POSTGRES_EPOCH_JDATE;

recalc_d:
    /* Julian day routine does not work for negative Julian days */
    if (date < 0 || date > (Timestamp)INT_MAX)
        return -1;

    j2date((int)date, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
recalc_t:
    dt2time(time, &tm->tm_hour, &tm->tm_min, &tm->tm_sec, fsec);

    *fsec = TSROUND(*fsec);
    /* roundoff may need to propagate to higher-order fields */
    if (*fsec >= 1.0) {
        time = ceil(time);
        if (time >= (double)SECS_PER_DAY) {
            time = 0;
            date += 1;
            goto recalc_d;
        }
        goto recalc_t;
    }
#endif

    /* Done if no TZ conversion wanted */
    if (tzp == NULL) {
        tm->tm_isdst = -1;
        tm->tm_gmtoff = 0;
        tm->tm_zone = NULL;
        if (tzn != NULL)
            *tzn = NULL;
        return 0;
    }

    /*
     * We have a brute force time zone per SQL99? Then use it without change
     * since we have already rotated to the time zone.
     */
    if (attimezone == NULL && u_sess->time_cxt.HasCTZSet) {
        *tzp = u_sess->time_cxt.CTimeZone;
        tm->tm_isdst = 0;
        tm->tm_gmtoff = u_sess->time_cxt.CTimeZone;
        tm->tm_zone = NULL;
        if (tzn != NULL)
            *tzn = NULL;
        return 0;
    }

    /*
     * If the time falls within the range of pg_time_t, use pg_localtime() to
     * rotate to the local time zone.
     *
     * First, convert to an integral timestamp, avoiding possibly
     * platform-specific roundoff-in-wrong-direction errors, and adjust to
     * Unix epoch.	Then see if we can convert to pg_time_t without loss. This
     * coding avoids hardwiring any assumptions about the width of pg_time_t,
     * so it should behave sanely on machines without int64.
     */
#ifdef HAVE_INT64_TIMESTAMP
    dt = (dt - *fsec) / USECS_PER_SEC + (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY;
#else
    dt = rint(dt - *fsec + (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY);
#endif
    utime = (pg_time_t)dt;
    if ((Timestamp)utime == dt) {
        struct pg_tm* tx = pg_localtime(&utime, attimezone ? attimezone : session_timezone);
#ifdef DOLPHIN
        if (tm->tm_year || tm->tm_mon || tm->tm_mday)
#endif
        {
            tm->tm_year = tx->tm_year + 1900;
            tm->tm_mon = tx->tm_mon + 1;
            tm->tm_mday = tx->tm_mday;
        }
        tm->tm_hour = tx->tm_hour;
        tm->tm_min = tx->tm_min;
        tm->tm_sec = tx->tm_sec;
        tm->tm_isdst = tx->tm_isdst;
        tm->tm_gmtoff = tx->tm_gmtoff;
        tm->tm_zone = tx->tm_zone;
        *tzp = -tm->tm_gmtoff;
        if (tzn != NULL)
            *tzn = tm->tm_zone;
    } else {
        /*
         * When out of range of pg_time_t, treat as GMT
         */
        *tzp = 0;
        /* Mark this as *no* time zone available */
        tm->tm_isdst = -1;
        tm->tm_gmtoff = 0;
        tm->tm_zone = NULL;
        if (tzn != NULL)
            *tzn = NULL;
    }

    return 0;
}

/* tm2timestamp()
 * Convert a tm structure to a timestamp data type.
 * Note that year is _not_ 1900-based, but is an explicit full value.
 * Also, month is one-based, _not_ zero-based.
 *
 * Returns -1 on failure (value out of range).
 */
int tm2timestamp(struct pg_tm* tm, const fsec_t fsec, const int* tzp, Timestamp* result)
{
    TimeOffset date;
    TimeOffset time;

    /* Julian day routines are not correct for negative Julian days */
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday)) {
        *result = 0; /* keep compiler quiet */
        return -1;
    }

    date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
    time = time2t(tm->tm_hour, tm->tm_min, tm->tm_sec, fsec);

#ifdef HAVE_INT64_TIMESTAMP
    *result = date * USECS_PER_DAY + time;
    /* check for major overflow */
    if ((*result - time) / USECS_PER_DAY != date) {
        *result = 0; /* keep compiler quiet */
        return -1;
    }
    /* check for just-barely overflow (okay except time-of-day wraps) */
    /* caution: we want to allow 1999-12-31 24:00:00 */
    if ((*result < 0 && date > 0) || (*result > 0 && date < -1)) {
        *result = 0; /* keep compiler quiet */
        return -1;
    }
#else
    *result = date * SECS_PER_DAY + time;
#endif
    if (tzp != NULL)
        *result = dt2local(*result, -(*tzp));

    return 0;
}

/* interval2tm()
 * Convert a interval data type to a tm structure.
 */
int interval2tm(Interval span, struct pg_tm* tm, fsec_t* fsec)
{
    TimeOffset time;
    TimeOffset tfrac;

    tm->tm_year = span.month / MONTHS_PER_YEAR;
    tm->tm_mon = span.month % MONTHS_PER_YEAR;
    tm->tm_mday = span.day;
    time = span.time;

#ifdef HAVE_INT64_TIMESTAMP
    tfrac = time / USECS_PER_HOUR;
    time -= tfrac * USECS_PER_HOUR;
    tm->tm_hour = tfrac; /* could overflow ... */
    tfrac = time / USECS_PER_MINUTE;
    time -= tfrac * USECS_PER_MINUTE;
    tm->tm_min = tfrac;
    tfrac = time / USECS_PER_SEC;
    *fsec = time - (tfrac * USECS_PER_SEC);
    tm->tm_sec = tfrac;
#else
recalc:
    TMODULO(time, tfrac, (double)SECS_PER_HOUR);
    tm->tm_hour = tfrac; /* could overflow ... */
    TMODULO(time, tfrac, (double)SECS_PER_MINUTE);
    tm->tm_min = tfrac;
    TMODULO(time, tfrac, 1.0);
    tm->tm_sec = tfrac;
    time = TSROUND(time);
    /* roundoff may need to propagate to higher-order fields */
    if (time >= 1.0) {
        time = ceil(span.time);
        goto recalc;
    }
    *fsec = time;
#endif

    return 0;
}

int tm2interval(struct pg_tm* tm, fsec_t fsec, Interval* span)
{
    span->month = tm->tm_year * MONTHS_PER_YEAR + tm->tm_mon;
    span->day = tm->tm_mday;
#ifdef HAVE_INT64_TIMESTAMP
    span->time =
        (((((tm->tm_hour * INT64CONST(60)) + tm->tm_min) * INT64CONST(60)) + tm->tm_sec) * USECS_PER_SEC) + fsec;
#else
    span->time = (((tm->tm_hour * (double)MINS_PER_HOUR) + tm->tm_min) * (double)SECS_PER_MINUTE) + tm->tm_sec + fsec;
#endif

    return 0;
}

static inline TimeOffset time2t(const int hour, const int min, const int sec, const fsec_t fsec)
{
#ifdef HAVE_INT64_TIMESTAMP
    return (((((hour * MINS_PER_HOUR) + min) * SECS_PER_MINUTE) + sec) * USECS_PER_SEC) + fsec;
#else
    return (((hour * MINS_PER_HOUR) + min) * SECS_PER_MINUTE) + sec + fsec;
#endif
}

static Timestamp dt2local(Timestamp dt, int tz)
{
#ifdef HAVE_INT64_TIMESTAMP
    dt -= (tz * USECS_PER_SEC);
#else
    dt -= tz;
#endif
    return dt;
}

/*****************************************************************************
 *	 PUBLIC ROUTINES														 *
 *****************************************************************************/

Datum timestamp_finite(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);

    PG_RETURN_BOOL(!TIMESTAMP_NOT_FINITE(timestamp));
}

Datum interval_finite(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(true);
}

/* ----------------------------------------------------------
 *	Relational operators for timestamp.
 * --------------------------------------------------------- */

void GetEpochTime(struct pg_tm* tm)
{
    struct pg_tm* t0;
    pg_time_t epoch = 0;

    t0 = pg_gmtime(&epoch);

    tm->tm_year = t0->tm_year;
    tm->tm_mon = t0->tm_mon;
    tm->tm_mday = t0->tm_mday;
    tm->tm_hour = t0->tm_hour;
    tm->tm_min = t0->tm_min;
    tm->tm_sec = t0->tm_sec;

    tm->tm_year += 1900;
    tm->tm_mon++;
}

Timestamp SetEpochTimestamp(void)
{
    Timestamp dt;
    struct pg_tm tt, *tm = &tt;

    GetEpochTime(tm);
    /* we don't bother to test for failure ... */
    tm2timestamp(tm, 0, NULL, &dt);

    return dt;
} /* SetEpochTimestamp() */

/*
 * We are currently sharing some code between timestamp and timestamptz.
 * The comparison functions are among them. - thomas 2001-09-25
 *
 *		timestamp_relop - is timestamp1 relop timestamp2
 *
 *		collate invalid timestamp at the end
 */
int timestamp_cmp_internal(Timestamp dt1, Timestamp dt2)
{
#ifdef HAVE_INT64_TIMESTAMP
    return (dt1 < dt2) ? -1 : ((dt1 > dt2) ? 1 : 0);
#else

    /*
     * When using float representation, we have to be wary of NaNs.
     *
     * We consider all NANs to be equal and larger than any non-NAN. This is
     * somewhat arbitrary; the important thing is to have a consistent sort
     * order.
     */
    if (isnan(dt1)) {
        if (isnan(dt2))
            return 0; /* NAN = NAN */
        else
            return 1; /* NAN > non-NAN */
    } else if (isnan(dt2)) {
        return -1; /* non-NAN < NAN */
    } else {
        if (dt1 > dt2)
            return 1;
        else if (dt1 < dt2)
            return -1;
        else
            return 0;
    }
#endif
}

Datum timestamp_eq(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum timestamp_ne(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum timestamp_lt(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum timestamp_gt(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum timestamp_le(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum timestamp_ge(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

Datum timestamp_cmp(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);

    PG_RETURN_INT32(timestamp_cmp_internal(dt1, dt2));
}

/* note: this is used for timestamptz also */
static int timestamp_fastcmp(Datum x, Datum y, SortSupport ssup)
{
    Timestamp a = DatumGetTimestamp(x);
    Timestamp b = DatumGetTimestamp(y);

    return timestamp_cmp_internal(a, b);
}

Datum timestamp_sortsupport(PG_FUNCTION_ARGS)
{
    SortSupport ssup = (SortSupport)PG_GETARG_POINTER(0);

    ssup->comparator = timestamp_fastcmp;
    PG_RETURN_VOID();
}

Datum timestamp_hash(PG_FUNCTION_ARGS)
{
    /* We can use either hashint8 or hashfloat8 directly */
#ifdef HAVE_INT64_TIMESTAMP
    return hashint8(fcinfo);
#else
    return hashfloat8(fcinfo);
#endif
}

/*
 * Crosstype comparison functions for timestamp vs timestamptz
 */

Datum timestamp_eq_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum timestamp_ne_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum timestamp_lt_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum timestamp_gt_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum timestamp_le_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum timestamp_ge_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

Datum timestamp_cmp_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = timestamp2timestamptz(timestampVal);

    PG_RETURN_INT32(timestamp_cmp_internal(dt1, dt2));
}

Datum timestamptz_eq_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum timestamptz_ne_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum timestamptz_lt_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum timestamptz_gt_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum timestamptz_le_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum timestamptz_ge_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

Datum timestamptz_cmp_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp timestampVal = PG_GETARG_TIMESTAMP(1);
    TimestampTz dt2;

    dt2 = timestamp2timestamptz(timestampVal);

    PG_RETURN_INT32(timestamp_cmp_internal(dt1, dt2));
}

/*
 *		interval_relop	- is interval1 relop interval2
 *
 *		collate invalid interval at the end
 */
static inline TimeOffset interval_cmp_value(const Interval* interval)
{
    TimeOffset span;

    span = interval->time;

#ifdef HAVE_INT64_TIMESTAMP
    span += interval->month * INT64CONST(30) * USECS_PER_DAY;
    span += interval->day * INT64CONST(24) * USECS_PER_HOUR;
#else
    span += interval->month * ((double)DAYS_PER_MONTH * SECS_PER_DAY);
    span += interval->day * ((double)HOURS_PER_DAY * SECS_PER_HOUR);
#endif

    return span;
}

int interval_cmp_internal(Interval* interval1, Interval* interval2)
{
    TimeOffset span1 = interval_cmp_value(interval1);
    TimeOffset span2 = interval_cmp_value(interval2);

    return ((span1 < span2) ? -1 : (span1 > span2) ? 1 : 0);
}

Datum interval_eq_withhead(PG_FUNCTION_ARGS)
{
    Interval* interval1 = (Interval*)((char*)PG_GETARG_DATUM(0) + VARHDRSZ_SHORT);
    Interval* interval2 = (Interval*)((char*)PG_GETARG_DATUM(1) + VARHDRSZ_SHORT);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) == 0);
}

Datum interval_eq(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) == 0);
}

Datum interval_ne(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) != 0);
}

Datum interval_lt(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) < 0);
}

Datum interval_gt(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) > 0);
}

Datum interval_le(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) <= 0);
}

Datum interval_ge(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_BOOL(interval_cmp_internal(interval1, interval2) >= 0);
}

Datum interval_cmp(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);

    PG_RETURN_INT32(interval_cmp_internal(interval1, interval2));
}

/*
 * Hashing for intervals
 *
 * We must produce equal hashvals for values that interval_cmp_internal()
 * considers equal.  So, compute the net span the same way it does,
 * and then hash that, using either int64 or float8 hashing.
 */
Datum interval_hash(PG_FUNCTION_ARGS)
{
    Interval* interval = PG_GETARG_INTERVAL_P(0);
    TimeOffset span = interval_cmp_value(interval);

#ifdef HAVE_INT64_TIMESTAMP
    return DirectFunctionCall1(hashint8, Int64GetDatumFast(span));
#else
    return DirectFunctionCall1(hashfloat8, Float8GetDatumFast(span));
#endif
}

/* overlaps_timestamp() --- implements the SQL92 OVERLAPS operator.
 *
 * Algorithm is per SQL92 spec.  This is much harder than you'd think
 * because the spec requires us to deliver a non-null answer in some cases
 * where some of the inputs are null.
 */
Datum overlaps_timestamp(PG_FUNCTION_ARGS)
{
    /*
     * The arguments are Timestamps, but we leave them as generic Datums to
     * avoid unnecessary conversions between value and reference forms --- not
     * to mention possible dereferences of null pointers.
     */
    Datum ts1 = PG_GETARG_DATUM(0);
    Datum te1 = PG_GETARG_DATUM(1);
    Datum ts2 = PG_GETARG_DATUM(2);
    Datum te2 = PG_GETARG_DATUM(3);
    bool ts1IsNull = PG_ARGISNULL(0);
    bool te1IsNull = PG_ARGISNULL(1);
    bool ts2IsNull = PG_ARGISNULL(2);
    bool te2IsNull = PG_ARGISNULL(3);

#define TIMESTAMP_GT(t1, t2) DatumGetBool(DirectFunctionCall2(timestamp_gt, t1, t2))
#define TIMESTAMP_LT(t1, t2) DatumGetBool(DirectFunctionCall2(timestamp_lt, t1, t2))

    /*
     * If both endpoints of interval 1 are null, the result is null (unknown).
     * If just one endpoint is null, take ts1 as the non-null one. Otherwise,
     * take ts1 as the lesser endpoint.
     */
    if (ts1IsNull) {
        if (te1IsNull)
            PG_RETURN_NULL();
        /* swap null for non-null */
        ts1 = te1;
        te1IsNull = true;
    } else if (!te1IsNull) {
        if (TIMESTAMP_GT(ts1, te1)) {
            Datum tt = ts1;

            ts1 = te1;
            te1 = tt;
        }
    }

    /* Likewise for interval 2. */
    if (ts2IsNull) {
        if (te2IsNull)
            PG_RETURN_NULL();
        /* swap null for non-null */
        ts2 = te2;
        te2IsNull = true;
    } else if (!te2IsNull) {
        if (TIMESTAMP_GT(ts2, te2)) {
            Datum tt = ts2;

            ts2 = te2;
            te2 = tt;
        }
    }

    /*
     * At this point neither ts1 nor ts2 is null, so we can consider three
     * cases: ts1 > ts2, ts1 < ts2, ts1 = ts2
     */
    if (TIMESTAMP_GT(ts1, ts2)) {
        /*
         * This case is ts1 < te2 OR te1 < te2, which may look redundant but
         * in the presence of nulls it's not quite completely so.
         */
        if (te2IsNull)
            PG_RETURN_NULL();
        if (TIMESTAMP_LT(ts1, te2))
            PG_RETURN_BOOL(true);
        if (te1IsNull)
            PG_RETURN_NULL();

        /*
         * If te1 is not null then we had ts1 <= te1 above, and we just found
         * ts1 >= te2, hence te1 >= te2.
         */
        PG_RETURN_BOOL(false);
    } else if (TIMESTAMP_LT(ts1, ts2)) {
        /* This case is ts2 < te1 OR te2 < te1 */
        if (te1IsNull)
            PG_RETURN_NULL();
        if (TIMESTAMP_LT(ts2, te1))
            PG_RETURN_BOOL(true);
        if (te2IsNull)
            PG_RETURN_NULL();

        /*
         * If te2 is not null then we had ts2 <= te2 above, and we just found
         * ts2 >= te1, hence te2 >= te1.
         */
        PG_RETURN_BOOL(false);
    } else {
        /*
         * For ts1 = ts2 the spec says te1 <> te2 OR te1 = te2, which is a
         * rather silly way of saying "true if both are nonnull, else null".
         */
        if (te1IsNull || te2IsNull)
            PG_RETURN_NULL();
        PG_RETURN_BOOL(true);
    }

#undef TIMESTAMP_GT
#undef TIMESTAMP_LT
}

/* ----------------------------------------------------------
 *	"Arithmetic" operators on date/times.
 * --------------------------------------------------------- */

Datum timestamp_smaller(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp result;

    /* use timestamp_cmp_internal to be sure this agrees with comparisons */
    if (timestamp_cmp_internal(dt1, dt2) < 0)
        result = dt1;
    else
        result = dt2;
    PG_RETURN_TIMESTAMP(result);
}

Datum timestamp_larger(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp result;

    if (timestamp_cmp_internal(dt1, dt2) > 0)
        result = dt1;
    else
        result = dt2;
    PG_RETURN_TIMESTAMP(result);
}

Datum timestamp_mi(Timestamp dt1, Timestamp dt2)
{
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    if (TIMESTAMP_NOT_FINITE(dt1) || TIMESTAMP_NOT_FINITE(dt2))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("cannot subtract infinite timestamps")));

    result->time = dt1 - dt2;

    result->month = 0;
    result->day = 0;

    /* ----------
     *	This is wrong, but removing it breaks a lot of regression tests.
     *	For example:
     *
     *	test=> SET timezone = 'EST5EDT';
     *	test=> SELECT
     *	test-> ('2005-10-30 13:22:00-05'::timestamptz -
     *	test(>	'2005-10-29 13:22:00-04'::timestamptz);
     *	?column?
     *	----------------
     *	 1 day 01:00:00
     *	 (1 row)
     *
     *	so adding that to the first timestamp gets:
     *
     *	 test=> SELECT
     *	 test-> ('2005-10-29 13:22:00-04'::timestamptz +
     *	 test(> ('2005-10-30 13:22:00-05'::timestamptz -
     *	 test(>  '2005-10-29 13:22:00-04'::timestamptz)) at time zone 'EST';
     *		timezone
     *	--------------------
     *	2005-10-30 14:22:00
     *	(1 row)
     * ----------
     */
    result = DatumGetIntervalP(DirectFunctionCall1(interval_justify_hours, IntervalPGetDatum(result)));

    PG_RETURN_INTERVAL_P(result);
}

Datum timestamp_mi(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    return timestamp_mi(dt1, dt2);
}

/*
 *	interval_justify_interval()
 *
 *	Adjust interval so 'month', 'day', and 'time' portions are within
 *	customary bounds.  Specifically:
 *
 *		0 <= abs(time) < 24 hours
 *		0 <= abs(day)  < 30 days
 *
 *	Also, the sign bit on all three fields is made equal, so either
 *	all three fields are negative or all are positive.
 */
Datum interval_justify_interval(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    Interval* result = NULL;
    TimeOffset wholeday;
    int32 wholemonth;

    result = (Interval*)palloc(sizeof(Interval));
    result->month = span->month;
    result->day = span->day;
    result->time = span->time;

#ifdef HAVE_INT64_TIMESTAMP
    TMODULO(result->time, wholeday, USECS_PER_DAY);
#else
    TMODULO(result->time, wholeday, (double)SECS_PER_DAY);
#endif
    if (pg_add_s32_overflow(result->day, wholeday, &result->day)) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }

    wholemonth = result->day / DAYS_PER_MONTH;
    result->day -= wholemonth * DAYS_PER_MONTH;
    result->month += wholemonth;

    if (result->month > 0 && (result->day < 0 || (result->day == 0 && result->time < 0))) {
        result->day += DAYS_PER_MONTH;
        result->month--;
    } else if (result->month < 0 && (result->day > 0 || (result->day == 0 && result->time > 0))) {
        result->day -= DAYS_PER_MONTH;
        result->month++;
    }

    if (result->day > 0 && result->time < 0) {
#ifdef HAVE_INT64_TIMESTAMP
        result->time += USECS_PER_DAY;
#else
        result->time += (double)SECS_PER_DAY;
#endif
        result->day--;
    } else if (result->day < 0 && result->time > 0) {
#ifdef HAVE_INT64_TIMESTAMP
        result->time -= USECS_PER_DAY;
#else
        result->time -= (double)SECS_PER_DAY;
#endif
        result->day++;
    }

    PG_RETURN_INTERVAL_P(result);
}

/*
 *	interval_justify_hours()
 *
 *	Adjust interval so 'time' contains less than a whole day, adding
 *	the excess to 'day'.  This is useful for
 *	situations (such as non-TZ) where '1 day' = '24 hours' is valid,
 *	e.g. interval subtraction and division.
 */
Datum interval_justify_hours(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    Interval* result = NULL;
    TimeOffset wholeday;

    result = (Interval*)palloc(sizeof(Interval));
    result->month = span->month;
    result->day = span->day;
    result->time = span->time;

#ifdef HAVE_INT64_TIMESTAMP
    TMODULO(result->time, wholeday, USECS_PER_DAY);
#else
    TMODULO(result->time, wholeday, (double)SECS_PER_DAY);
#endif
    result->day += wholeday; /* could overflow... */

    if (result->day > 0 && result->time < 0) {
#ifdef HAVE_INT64_TIMESTAMP
        result->time += USECS_PER_DAY;
#else
        result->time += (double)SECS_PER_DAY;
#endif
        result->day--;
    } else if (result->day < 0 && result->time > 0) {
#ifdef HAVE_INT64_TIMESTAMP
        result->time -= USECS_PER_DAY;
#else
        result->time -= (double)SECS_PER_DAY;
#endif
        result->day++;
    }

    PG_RETURN_INTERVAL_P(result);
}

/*
 *	interval_justify_days()
 *
 *	Adjust interval so 'day' contains less than 30 days, adding
 *	the excess to 'month'.
 */
Datum interval_justify_days(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    Interval* result = NULL;
    int32 wholemonth;

    result = (Interval*)palloc(sizeof(Interval));
    result->month = span->month;
    result->day = span->day;
    result->time = span->time;

    wholemonth = result->day / DAYS_PER_MONTH;
    result->day -= wholemonth * DAYS_PER_MONTH;
    result->month += wholemonth;

    if (result->month > 0 && result->day < 0) {
        result->day += DAYS_PER_MONTH;
        result->month--;
    } else if (result->month < 0 && result->day > 0) {
        result->day -= DAYS_PER_MONTH;
        result->month++;
    }

    PG_RETURN_INTERVAL_P(result);
}

Datum timestamp_pl_interval(Timestamp timestamp, const Interval* span)
{
    Timestamp result;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        result = timestamp;
    else {
        if (span->month != 0) {
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;

            if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

            tm->tm_mon += span->month;
            if (tm->tm_mon > MONTHS_PER_YEAR) {
                tm->tm_year += (tm->tm_mon - 1) / MONTHS_PER_YEAR;
                tm->tm_mon = ((tm->tm_mon - 1) % MONTHS_PER_YEAR) + 1;
            } else if (tm->tm_mon < 1) {
                tm->tm_year += tm->tm_mon / MONTHS_PER_YEAR - 1;
                tm->tm_mon = tm->tm_mon % MONTHS_PER_YEAR + MONTHS_PER_YEAR;
            }

            /* adjust for end of month boundary problems... */
            if (tm->tm_mday > day_tab[isleap(tm->tm_year)][tm->tm_mon - 1])
                tm->tm_mday = (day_tab[isleap(tm->tm_year)][tm->tm_mon - 1]);

            if (tm2timestamp(tm, fsec, NULL, &timestamp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }

        if (span->day != 0) {
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            int julian;

            if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

            /* Add days by converting to and from julian */
            julian = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) + span->day;
            j2date(julian, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);

            if (tm2timestamp(tm, fsec, NULL, &timestamp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }

        timestamp += span->time;
        result = timestamp;
    }

    PG_RETURN_TIMESTAMP(result);
}

/* timestamp_pl_interval()
 * Add a interval to a timestamp data type.
 * Note that interval has provisions for qualitative year/month and day
 *	units, so try to do the right thing with them.
 * To add a month, increment the month, and use the same day of month.
 * Then, if the next month has fewer days, set the day of month
 *	to the last day of month.
 * To add a day, increment the mday, and use the same time of day.
 * Lastly, add in the "quantitative time".
 */
Datum timestamp_pl_interval(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    return timestamp_pl_interval(timestamp, span);
}

Datum timestamp_mi_interval(Timestamp timestamp, const Interval* span)
{
    Interval tspan;

    tspan.month = -span->month;
    tspan.day = -span->day;
    tspan.time = -span->time;

    return DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(timestamp), PointerGetDatum(&tspan));
}

Datum timestamp_mi_interval(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    return timestamp_mi_interval(timestamp, span);
}

/* timestamptz_pl_interval()
 * Add a interval to a timestamp with time zone data type.
 * Note that interval has provisions for qualitative year/month
 *	units, so try to do the right thing with them.
 * To add a month, increment the month, and use the same day of month.
 * Then, if the next month has fewer days, set the day of month
 *	to the last day of month.
 * Lastly, add in the "quantitative time".
 */
Datum timestamptz_pl_interval(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    TimestampTz result;
    int tz;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        result = timestamp;
    else {
        if (span->month != 0) {
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;

            if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

            tm->tm_mon += span->month;
            if (tm->tm_mon > MONTHS_PER_YEAR) {
                tm->tm_year += (tm->tm_mon - 1) / MONTHS_PER_YEAR;
                tm->tm_mon = ((tm->tm_mon - 1) % MONTHS_PER_YEAR) + 1;
            } else if (tm->tm_mon < 1) {
                tm->tm_year += tm->tm_mon / MONTHS_PER_YEAR - 1;
                tm->tm_mon = tm->tm_mon % MONTHS_PER_YEAR + MONTHS_PER_YEAR;
            }

            /* adjust for end of month boundary problems... */
            if (tm->tm_mday > day_tab[isleap(tm->tm_year)][tm->tm_mon - 1])
                tm->tm_mday = (day_tab[isleap(tm->tm_year)][tm->tm_mon - 1]);

            tz = DetermineTimeZoneOffset(tm, session_timezone);

            if (tm2timestamp(tm, fsec, &tz, &timestamp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }

        if (span->day != 0) {
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            int julian;

            if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

            /* Add days by converting to and from julian */
            julian = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) + span->day;
            j2date(julian, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);

            tz = DetermineTimeZoneOffset(tm, session_timezone);

            if (tm2timestamp(tm, fsec, &tz, &timestamp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        }

        timestamp += span->time;
        result = timestamp;
    }

    PG_RETURN_TIMESTAMP(result);
}

Datum timestamptz_mi_interval(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    Interval tspan;

    tspan.month = -span->month;
    tspan.day = -span->day;
    tspan.time = -span->time;

    return DirectFunctionCall2(timestamptz_pl_interval, TimestampGetDatum(timestamp), PointerGetDatum(&tspan));
}

Datum interval_um(PG_FUNCTION_ARGS)
{
    Interval* interval = PG_GETARG_INTERVAL_P(0);
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    result->time = -interval->time;
    result->day = -interval->day;
    result->month = -interval->month;

    PG_RETURN_INTERVAL_P(result);
}

Datum interval_smaller(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);
    Interval* result = NULL;

    /* use interval_cmp_internal to be sure this agrees with comparisons */
    if (interval_cmp_internal(interval1, interval2) < 0)
        result = interval1;
    else
        result = interval2;
    PG_RETURN_INTERVAL_P(result);
}

Datum interval_larger(PG_FUNCTION_ARGS)
{
    Interval* interval1 = PG_GETARG_INTERVAL_P(0);
    Interval* interval2 = PG_GETARG_INTERVAL_P(1);
    Interval* result = NULL;

    if (interval_cmp_internal(interval1, interval2) > 0)
        result = interval1;
    else
        result = interval2;
    PG_RETURN_INTERVAL_P(result);
}

Datum interval_pl(PG_FUNCTION_ARGS)
{
    Interval* span1 = PG_GETARG_INTERVAL_P(0);
    Interval* span2 = PG_GETARG_INTERVAL_P(1);
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    result->month = span1->month + span2->month;
    result->day = span1->day + span2->day;
    result->time = span1->time + span2->time;

    // adjust result's format
    interval_result_adjust(result);

    PG_RETURN_INTERVAL_P(result);
}

Datum interval_mi(PG_FUNCTION_ARGS)
{
    Interval* span1 = PG_GETARG_INTERVAL_P(0);
    Interval* span2 = PG_GETARG_INTERVAL_P(1);
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    result->month = span1->month - span2->month;
    result->day = span1->day - span2->day;
    result->time = span1->time - span2->time;

    // adjust result's format
    interval_result_adjust(result);

    PG_RETURN_INTERVAL_P(result);
}

/*
 *	There is no interval_abs():  it is unclear what value to return:
 *	  http://archives.postgresql.org/pgsql-general/2009-10/msg01031.php
 *	  http://archives.postgresql.org/pgsql-general/2009-11/msg00041.php
 */
Datum interval_mul(const Interval* span, float8 factor)
{
    double month_remainder_days, sec_remainder;
    int32 orig_month = span->month, orig_day = span->day;
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    if (isnan(factor) || isinf(factor)) {
        /* NaN and Infinity convert process */
        INTERVAL_CONVERT_INFINITY_NAN(result);
    } else {
        float8 ans_month = span->month * factor;
        float8 ans_day = span->day * factor;
        if (ans_month > (float8)MAX_INT32 || ans_day > (float8)MAX_INT32 ||
            ans_month < (float8)(-1 - MAX_INT32) || ans_day < (float8)(-1 - MAX_INT32)) {
            ereport(ERROR, (errcode(ERRCODE_INTERVAL_FIELD_OVERFLOW),
                            errmsg("interval_mul result month: %lf, day: %lf overflow", ans_month, ans_day)));
        }
        result->month = (int32)ans_month;
        result->day = (int32)ans_day;

        /*
         * The above correctly handles the whole-number part of the month and day
         * products, but we have to do something with any fractional part
         * resulting when the factor is nonintegral.  We cascade the fractions
         * down to lower units using the conversion factors DAYS_PER_MONTH and
         * SECS_PER_DAY.  Note we do NOT cascade up, since we are not forced to do
         * so by the representation.  The user can choose to cascade up later,
         * using justify_hours and/or justify_days.
         */

        /*
         * Fractional months full days into days.
         *
         * Floating point calculation are inherently inprecise, so these
         * calculations are crafted to produce the most reliable result possible.
         * TSROUND() is needed to more accurately produce whole numbers where
         * appropriate.
         */
        month_remainder_days = (orig_month * factor - result->month) * DAYS_PER_MONTH;
        month_remainder_days = TSROUND(month_remainder_days);
        sec_remainder =
            (orig_day * factor - result->day + month_remainder_days - (int)month_remainder_days) * SECS_PER_DAY;
        sec_remainder = TSROUND(sec_remainder);

        /*
         * Might have 24:00:00 hours due to rounding, or >24 hours because of time
         * cascade from months and days.  It might still be >24 if the combination
         * of cascade and the seconds factor operation itself.
         */
        if (Abs(sec_remainder) >= SECS_PER_DAY) {
            result->day += (int)(sec_remainder / SECS_PER_DAY);
            sec_remainder -= (int)(sec_remainder / SECS_PER_DAY) * SECS_PER_DAY;
        }

        /* cascade units down */
        result->day += (int32)month_remainder_days;
#ifdef HAVE_INT64_TIMESTAMP
        result->time = rint(span->time * factor + sec_remainder * USECS_PER_SEC);
#else
        result->time = span->time * factor + sec_remainder;
#endif
    }

    // adjust result's format
    interval_result_adjust(result);

    PG_RETURN_INTERVAL_P(result);
}

/*
 *	There is no interval_abs():  it is unclear what value to return:
 *	  http://archives.postgresql.org/pgsql-general/2009-10/msg01031.php
 *	  http://archives.postgresql.org/pgsql-general/2009-11/msg00041.php
 */
Datum interval_mul(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    float8 factor = PG_GETARG_FLOAT8(1);
    return interval_mul(span, factor);
}

Datum mul_d_interval(PG_FUNCTION_ARGS)
{
    /* Args are float8 and Interval *, but leave them as generic Datum */
    Datum factor = PG_GETARG_DATUM(0);
    Datum span = PG_GETARG_DATUM(1);

    return DirectFunctionCall2(interval_mul, span, factor);
}

Datum interval_div(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    float8 factor = PG_GETARG_FLOAT8(1);
    double month_remainder_days, sec_remainder;
    int32 orig_month = span->month, orig_day = span->day;
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    if (factor == 0.0)
        ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO), errmsg("division by zero")));
    if (isnan(factor)) {
        /* NaN convert process. It differs from interval_mul to maintain compatibility. */
        INTERVAL_CONVERT_INFINITY_NAN(result);
    } else {
        result->month = (int32)(span->month / factor);
        result->day = (int32)(span->day / factor);

        /*
         * Fractional months full days into days.  See comment in interval_mul().
         */
        month_remainder_days = (orig_month / factor - result->month) * DAYS_PER_MONTH;
        month_remainder_days = TSROUND(month_remainder_days);
        sec_remainder =
            (orig_day / factor - result->day + month_remainder_days - (int)month_remainder_days) * SECS_PER_DAY;
        sec_remainder = TSROUND(sec_remainder);
        if (Abs(sec_remainder) >= SECS_PER_DAY) {
            result->day += (int)(sec_remainder / SECS_PER_DAY);
            sec_remainder -= (int)(sec_remainder / SECS_PER_DAY) * SECS_PER_DAY;
        }

        /* cascade units down */
        result->day += (int32)month_remainder_days;
#ifdef HAVE_INT64_TIMESTAMP
        result->time = rint(span->time / factor + sec_remainder * USECS_PER_SEC);
#else
        /* See TSROUND comment in interval_mul(). */
        result->time = span->time / factor + sec_remainder;
#endif
    }

    // adjust result's format
    interval_result_adjust(result);

    PG_RETURN_INTERVAL_P(result);
}

/*
 * interval_accum and interval_avg implement the AVG(interval)
#ifdef PGXC
 * as well as interval_collect
#endif
 *
 * The transition datatype for this aggregate is a 2-element array of
 * intervals, where the first is the running sum and the second contains
 * the number of values so far in its 'time' field.  This is a bit ugly
 * but it beats inventing a specialized datatype for the purpose.
 */

Datum interval_accum(PG_FUNCTION_ARGS)
{
    ArrayType* transarray = PG_GETARG_ARRAYTYPE_P(0);
    Interval* newval = PG_GETARG_INTERVAL_P(1);
    Datum* transdatums = NULL;
    int ndatums;
    Interval sumX, N;
    Interval* newsum = NULL;
    ArrayType* result = NULL;
    int rc = 0;

    deconstruct_array(transarray, INTERVALOID, sizeof(Interval), false, 'd', &transdatums, NULL, &ndatums);
    if (ndatums != 2)
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("expected 2-element interval array")));

    /*
     * XXX memcpy, instead of just extracting a pointer, to work around buggy
     * array code: it won't ensure proper alignment of Interval objects on
     * machines where double requires 8-byte alignment. That should be fixed,
     * but in the meantime...
     *
     * Note: must use DatumGetPointer here, not DatumGetIntervalP, else some
     * compilers optimize into double-aligned load/store anyway.
     */
    rc = memcpy_s((void*)&sumX, sizeof(Interval), DatumGetPointer(transdatums[0]), sizeof(Interval));
    securec_check(rc, "\0", "\0");
    rc = memcpy_s((void*)&N, sizeof(Interval), DatumGetPointer(transdatums[1]), sizeof(Interval));
    securec_check(rc, "\0", "\0");

    newsum = DatumGetIntervalP(DirectFunctionCall2(interval_pl, IntervalPGetDatum(&sumX), IntervalPGetDatum(newval)));
    N.time += 1;

    transdatums[0] = IntervalPGetDatum(newsum);
    transdatums[1] = IntervalPGetDatum(&N);

    result = construct_array(transdatums, 2, INTERVALOID, sizeof(Interval), false, 'd');

    PG_RETURN_ARRAYTYPE_P(result);
}

Datum interval_avg(PG_FUNCTION_ARGS)
{
    ArrayType* transarray = PG_GETARG_ARRAYTYPE_P(0);
    Datum* transdatums = NULL;
    int ndatums;
    Interval sumX, N;

    deconstruct_array(transarray, INTERVALOID, sizeof(Interval), false, 'd', &transdatums, NULL, &ndatums);
    if (ndatums != 2)
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("expected 2-element interval array")));

    /*
     * XXX memcpy, instead of just extracting a pointer, to work around buggy
     * array code: it won't ensure proper alignment of Interval objects on
     * machines where double requires 8-byte alignment. That should be fixed,
     * but in the meantime...
     *
     * Note: must use DatumGetPointer here, not DatumGetIntervalP, else some
     * compilers optimize into double-aligned load/store anyway.
     */
    errno_t rc = memcpy_s((void*)&sumX, sizeof(Interval), DatumGetPointer(transdatums[0]), sizeof(Interval));
    securec_check(rc, "", "");
    rc = memcpy_s((void*)&N, sizeof(Interval), DatumGetPointer(transdatums[1]), sizeof(Interval));
    securec_check(rc, "", "");

    /* SQL92 defines AVG of no values to be NULL */
    if (N.time == 0)
        PG_RETURN_NULL();

    return DirectFunctionCall2(interval_div, IntervalPGetDatum(&sumX), Float8GetDatum(N.time));
}

/*
 * @Description: Calculate the difference of days between two date.
 *	may be positive or negative.
 * @in tm : timestamp1 - timestamp2.
 * @in tm1 : timestmap1
 * @in tm2 : timestamp2
 */
static int daydiff_timestamp(const struct pg_tm* tm, const struct pg_tm* tm1, const struct pg_tm* tm2, bool day_fix)
{
    int result = 0;
    int tm1_days_of_year = 0, tm2_days_of_year = 0;
    int i = 0, j = 0, k = 0;
    if (tm1 == NULL || tm2 == NULL || tm == NULL) {
        ereport(ERROR,
            (errmodule(MOD_FUNCTION),
                errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                errmsg("the input timestamp must not be null.")));
        return 0;
    }

    /*
     * if tm2 > tm1, then
     * tm1_days_of_year calc how many days have passed since the beginning of the tm1 year
     * tm2_days_of_year calc how many days have passed since the beginning of the tm1 year
     * if tm1 > tm2, then
     * tm1_days_of_year calc how many days have passed since the beginning of the tm2 year
     * tm2_days_of_year calc how many days have passed since the beginning of the tm2 year
     */
    if (tm->tm_year > 0 || tm->tm_mon > 0 || tm->tm_mday > 0) {
        i = tm2->tm_mon - 1;
        while (i > 0) {
            i = i - 1;
            tm2_days_of_year += day_tab[isleap(tm2->tm_year)][i];
        }
        tm2_days_of_year += tm2->tm_mday;

        j = tm1->tm_year - tm2->tm_year;
        while (j > 0) {
            j = j - 1;
            tm1_days_of_year += isleap(j + tm2->tm_year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_COMMON_YEAR;
        }
        k = tm1->tm_mon - 1;
        while (k > 0) {
            k = k - 1;
            tm1_days_of_year += day_tab[isleap(tm1->tm_year)][k];
        }
        tm1_days_of_year += tm1->tm_mday;
    } else {
        i = tm1->tm_mon - 1;
        while (i > 0) {
            i = i - 1;
            tm1_days_of_year += day_tab[isleap(tm1->tm_year)][i];
        }
        tm1_days_of_year += tm1->tm_mday;

        j = tm2->tm_year - tm1->tm_year;
        while (j > 0) {
            j = j - 1;
            tm2_days_of_year += isleap(j + tm1->tm_year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_COMMON_YEAR;
        }
        k = tm2->tm_mon - 1;
        while (k > 0) {
            k = k - 1;
            tm2_days_of_year += day_tab[isleap(tm2->tm_year)][k];
        }
        tm2_days_of_year += tm2->tm_mday;
    }
    result = tm1_days_of_year - tm2_days_of_year;

    if (!day_fix) {
        return result;
    }

    int tm1_secs_of_day = tm1->tm_hour * 60 * 60 + tm1->tm_min * 60 + tm1->tm_sec;
    int tm2_secs_of_day = tm2->tm_hour * 60 * 60 + tm2->tm_min * 60 + tm2->tm_sec;

    if (result < 0 && tm1_secs_of_day > tm2_secs_of_day) {
        return result + 1;
    } else if (result > 0 && tm1_secs_of_day < tm2_secs_of_day) {
        return result - 1;
    }
    return result;
}

/* timestamp_diff()
 * Calculate the difference of timestamp with time zone
 * while retaining units fields.
 */
Datum timestamp_diff(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    TimestampTz dt1 = PG_GETARG_TIMESTAMP(2);
    TimestampTz dt2 = PG_GETARG_TIMESTAMP(1);
    int64 result = PointerGetDatum(0);

    result = timestamp_diff_internal(units, dt1, dt2);
    PG_RETURN_INT64(result);
}

int64 timestamp_diff_internal(text *units, TimestampTz dt1, TimestampTz dt2, bool day_fix)
{
    char* lowunits = NULL;
    int64 result = PointerGetDatum(0);
    int64 sec_result;
    fsec_t fsec, fsec1, fsec2;
    struct pg_tm tt, *tm = &tt;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    int type, val, tz1, tz2;

    /* calculate the difference between two timestamp */
    if (timestamp2tm(dt1, &tz1, tm1, &fsec1, NULL, NULL) == 0 &&
        timestamp2tm(dt2, &tz2, tm2, &fsec2, NULL, NULL) == 0) {
        /* form the symbolic difference */
        fsec = fsec1 - fsec2;
        tm->tm_sec = tm1->tm_sec - tm2->tm_sec;
        tm->tm_min = tm1->tm_min - tm2->tm_min;
        tm->tm_hour = tm1->tm_hour - tm2->tm_hour;
        tm->tm_mday = tm1->tm_mday - tm2->tm_mday;
        tm->tm_mon = tm1->tm_mon - tm2->tm_mon;
        tm->tm_year = tm1->tm_year - tm2->tm_year;

        timestamp_CalculateFields(&dt1, &dt2, &fsec, tm, tm1, tm2);
    } else
        ereport(ERROR,
            (errmodule(MOD_FUNCTION),
                errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                errmsg("timestamp out of range, negative Julian days is not supported")));

    /* decode timestamp_units */
    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        switch (val) {
            case DTK_YEAR:
                result = tm->tm_year;
                break;
            case DTK_MONTH:
                result = tm->tm_year * MONTHS_PER_YEAR + tm->tm_mon;
                break;
            case DTK_QUARTER:
                result = (tm->tm_year * MONTHS_PER_YEAR + tm->tm_mon) / 3;
                break;
            case DTK_DAY:
                result = daydiff_timestamp(tm, tm1, tm2, day_fix);
                break;
            case DTK_WEEK:
                result = daydiff_timestamp(tm, tm1, tm2, day_fix) / 7;
                break;
            case DTK_HOUR:
#ifdef HAVE_INT64_TIMESTAMP
                result = daydiff_timestamp(tm, tm1, tm2, day_fix) * INT64CONST(24) + tm->tm_hour;
#else
                result = daydiff_timestamp(tm, tm1, tm2, day_fix) * HOURS_PER_DAY + tm->tm_hour;
#endif
                break;
            case DTK_MINUTE: {
#ifdef HAVE_INT64_TIMESTAMP
                result = (daydiff_timestamp(tm, tm1, tm2, day_fix) * INT64CONST(24) + tm->tm_hour) *
                         INT64CONST(60) + tm->tm_min;
#else
                result = (daydiff_timestamp(tm, tm1, tm2, day_fix) * HOURS_PER_DAY + tm->tm_hour) *
                         (double)MINS_PER_HOUR + tm->tm_min;
#endif
            } break;
            case DTK_SECOND: {
#ifdef HAVE_INT64_TIMESTAMP
                result = ((daydiff_timestamp(tm, tm1, tm2, day_fix) * INT64CONST(24) + tm->tm_hour) *
                             INT64CONST(60) + tm->tm_min) *
                         INT64CONST(60) + tm->tm_sec;
#else
                result = ((daydiff_timestamp(tm, tm1, tm2, day_fix) * HOURS_PER_DAY + tm->tm_hour) *
                             (double)MINS_PER_HOUR + tm->tm_min) *
                         (double)SECS_PER_MINUTE + tm->tm_sec;
#endif
            } break;
            case DTK_MICROSEC:
                /*
                 * max timestamp is 294276AD, min timestamp is 4713BC,
                 * if timestampdiff function output in seconds, it will not integer overflow.
                 * but if function output in microseconds, integer overflow will occur.
                 */
                {
#ifdef HAVE_INT64_TIMESTAMP
                    sec_result = ((daydiff_timestamp(tm, tm1, tm2, day_fix) * INT64CONST(24) + tm->tm_hour) *
                                     INT64CONST(60) + tm->tm_min) * INT64CONST(60) +
                                 tm->tm_sec;
                    if (unlikely((int128)sec_result * INT64CONST(1000000) > (int128)INT64_MAX ||
                                 (int128)sec_result * INT64CONST(1000000) + (int128)fsec > (int128)INT64_MAX ||
                                 (int128)sec_result * INT64CONST(1000000) < (int128)INT64_MIN ||
                                 (int128)sec_result * INT64CONST(1000000) + (int128)fsec < (int128)INT64_MIN)) {
                        ereport(ERROR,
                            (errmodule(MOD_FUNCTION),
                                errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                                errmsg("The result of timestampdiff out of range.")));
                    }
                    result = sec_result * INT64CONST(1000000) + fsec;
#else
                    sec_result = ((daydiff_timestamp(tm, tm1, tm2, day_fix) * HOURS_PER_DAY + tm->tm_hour) *
                                     (double)MINS_PER_HOUR + tm->tm_min) *
                                 (double)SECS_PER_MINUTE + tm->tm_sec;
                    if (unlikely((int128)sec_result * (double)USECS_PER_SEC > (int128)INT64_MAX ||
                                 (int128)sec_result * (double)USECS_PER_SEC + (int128)fsec > (int128)INT64_MAX ||
                                 (int128)sec_result * (double)USECS_PER_SEC < (int128)INT64_MIN ||
                                 (int128)sec_result * (double)USECS_PER_SEC + (int128)fsec < (int128)INT64_MIN)) {
                        ereport(ERROR,
                            (errmodule(MOD_FUNCTION),
                                errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                                errmsg("The result of timestampdiff out of range.")));
                    }
                    result = sec_result * (double)USECS_PER_SEC + fsec;
#endif
                }
                break;
            default:
                ereport(ERROR,
                    (errmodule(MOD_FUNCTION),
                        errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("timestamp units \"%s\" not supported", lowunits)));
        }
    } else {
        ereport(ERROR,
            (errmodule(MOD_FUNCTION),
                errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("timestamp with time zone units \"%s\" not recognized", lowunits)));
    }
    return result;
}

/* timestamp_age()
 * Calculate time difference while retaining year/month fields.
 * Note that this does not result in an accurate absolute time span
 *	since year and month are out of context once the arithmetic
 *	is done.
 */
Datum timestamp_age(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Interval* result = NULL;
    fsec_t fsec, fsec1, fsec2;
    struct pg_tm tt, *tm = &tt;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;

    result = (Interval*)palloc(sizeof(Interval));

    if (timestamp2tm(dt1, NULL, tm1, &fsec1, NULL, NULL) == 0 &&
        timestamp2tm(dt2, NULL, tm2, &fsec2, NULL, NULL) == 0) {
        /* form the symbolic difference */
        fsec = fsec1 - fsec2;
        tm->tm_sec = tm1->tm_sec - tm2->tm_sec;
        tm->tm_min = tm1->tm_min - tm2->tm_min;
        tm->tm_hour = tm1->tm_hour - tm2->tm_hour;
        tm->tm_mday = tm1->tm_mday - tm2->tm_mday;
        tm->tm_mon = tm1->tm_mon - tm2->tm_mon;
        tm->tm_year = tm1->tm_year - tm2->tm_year;

        timestamp_CalculateFields(&dt1, &dt2, &fsec, tm, tm1, tm2);

        if (tm2interval(tm, fsec, result) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("interval out of range")));
    } else
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    PG_RETURN_INTERVAL_P(result);
}

/* timestamptz_age()
 * Calculate time difference while retaining year/month fields.
 * Note that this does not result in an accurate absolute time span
 *	since year and month are out of context once the arithmetic
 *	is done.
 */
Datum timestamptz_age(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    Interval* result = NULL;
    fsec_t fsec, fsec1, fsec2;
    struct pg_tm tt, *tm = &tt;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    int tz1;
    int tz2;

    result = (Interval*)palloc(sizeof(Interval));

    if (timestamp2tm(dt1, &tz1, tm1, &fsec1, NULL, NULL) == 0 &&
        timestamp2tm(dt2, &tz2, tm2, &fsec2, NULL, NULL) == 0) {
        /* form the symbolic difference */
        fsec = fsec1 - fsec2;
        tm->tm_sec = tm1->tm_sec - tm2->tm_sec;
        tm->tm_min = tm1->tm_min - tm2->tm_min;
        tm->tm_hour = tm1->tm_hour - tm2->tm_hour;
        tm->tm_mday = tm1->tm_mday - tm2->tm_mday;
        tm->tm_mon = tm1->tm_mon - tm2->tm_mon;
        tm->tm_year = tm1->tm_year - tm2->tm_year;

        timestamp_CalculateFields(&dt1, &dt2, &fsec, tm, tm1, tm2);

        if (tm2interval(tm, fsec, result) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("interval out of range")));
    } else
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    PG_RETURN_INTERVAL_P(result);
}

/* ----------------------------------------------------------
 *	Conversion operators.
 * --------------------------------------------------------- */

/* timestamp_trunc()
 * Truncate timestamp to specified units.
 */
Datum timestamp_trunc(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    Timestamp result;
    int type, val;
    char* lowunits = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_TIMESTAMP(timestamp);

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);

    if (type == UNITS) {
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        switch (val) {
            case DTK_WEEK: {
                int woy;

                woy = date2isoweek(tm->tm_year, tm->tm_mon, tm->tm_mday);

                /*
                 * If it is week 52/53 and the month is January, then the
                 * week must belong to the previous year. Also, some
                 * December dates belong to the next year.
                 */
                if (woy >= 52 && tm->tm_mon == 1)
                    --tm->tm_year;
                if (woy <= 1 && tm->tm_mon == MONTHS_PER_YEAR)
                    ++tm->tm_year;
                isoweek2date(woy, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
                tm->tm_hour = 0;
                tm->tm_min = 0;
                tm->tm_sec = 0;
                fsec = 0;
                break;
            }
            case DTK_MILLENNIUM:
                /* see comments in timestamptz_trunc */
                if (tm->tm_year > 0)
                    tm->tm_year = ((tm->tm_year + 999) / 1000) * 1000 - 999;
                else
                    tm->tm_year = -((999 - (tm->tm_year - 1)) / 1000) * 1000 + 1;
                /* fall through */
            case DTK_CENTURY:
                /* see comments in timestamptz_trunc */
                if (tm->tm_year > 0)
                    tm->tm_year = ((tm->tm_year + 99) / 100) * 100 - 99;
                else
                    tm->tm_year = -((99 - (tm->tm_year - 1)) / 100) * 100 + 1;
                /* fall through */
            case DTK_DECADE:
                /* see comments in timestamptz_trunc */
                if (val != DTK_MILLENNIUM && val != DTK_CENTURY) {
                    if (tm->tm_year > 0)
                        tm->tm_year = (tm->tm_year / 10) * 10;
                    else
                        tm->tm_year = -((8 - (tm->tm_year - 1)) / 10) * 10;
                }
                /* fall through */
            case DTK_YEAR:
                tm->tm_mon = 1;
                /* fall through */
            case DTK_QUARTER:
                tm->tm_mon = (3 * ((tm->tm_mon - 1) / 3)) + 1;
                /* fall through */
            case DTK_MONTH:
                tm->tm_mday = 1;
                /* fall through */
            case DTK_DAY:
                tm->tm_hour = 0;
                /* fall through */
            case DTK_HOUR:
                tm->tm_min = 0;
                /* fall through */
            case DTK_MINUTE:
                tm->tm_sec = 0;
                /* fall through */
            case DTK_SECOND:
                fsec = 0;
                break;

            case DTK_MILLISEC:
#ifdef HAVE_INT64_TIMESTAMP
                fsec = (fsec / 1000) * 1000;
#else
                fsec = floor(fsec * 1000) / 1000;
#endif
                break;

            case DTK_MICROSEC:
#ifndef HAVE_INT64_TIMESTAMP
                fsec = floor(fsec * 1000000) / 1000000;
#endif
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("timestamp units \"%s\" not supported", lowunits)));
                result = 0;
        }

        if (tm2timestamp(tm, fsec, NULL, &result) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("timestamp units \"%s\" not recognized", lowunits)));
        result = 0;
    }

    PG_RETURN_TIMESTAMP(result);
}

/* timestamptz_trunc()
 * Truncate timestamp to specified units.
 */
Datum timestamptz_trunc(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz result;
    int tz;
    int type, val;
    bool redotz = false;
    char* lowunits = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_TIMESTAMPTZ(timestamp);

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);

    if (type == UNITS) {
        if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        switch (val) {
            case DTK_WEEK: {
                int woy;

                woy = date2isoweek(tm->tm_year, tm->tm_mon, tm->tm_mday);

                /*
                 * If it is week 52/53 and the month is January, then the
                 * week must belong to the previous year. Also, some
                 * December dates belong to the next year.
                 */
                if (woy >= 52 && tm->tm_mon == 1)
                    --tm->tm_year;
                if (woy <= 1 && tm->tm_mon == MONTHS_PER_YEAR)
                    ++tm->tm_year;
                isoweek2date(woy, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
                tm->tm_hour = 0;
                tm->tm_min = 0;
                tm->tm_sec = 0;
                fsec = 0;
                redotz = true;
                break;
            }
                /* one may consider DTK_THOUSAND and DTK_HUNDRED... */
            case DTK_MILLENNIUM:

                /*
                 * truncating to the millennium? what is this supposed to
                 * mean? let us put the first year of the millennium... i.e.
                 * -1000, 1, 1001, 2001...
                 */
                if (tm->tm_year > 0)
                    tm->tm_year = ((tm->tm_year + 999) / 1000) * 1000 - 999;
                else
                    tm->tm_year = -((999 - (tm->tm_year - 1)) / 1000) * 1000 + 1;
                /* FALL THRU */
            case DTK_CENTURY:
                /* truncating to the century? as above: -100, 1, 101... */
                if (tm->tm_year > 0)
                    tm->tm_year = ((tm->tm_year + 99) / 100) * 100 - 99;
                else
                    tm->tm_year = -((99 - (tm->tm_year - 1)) / 100) * 100 + 1;
                /* FALL THRU */
            case DTK_DECADE:

                /*
                 * truncating to the decade? first year of the decade. must
                 * not be applied if year was truncated before!
                 */
                if (val != DTK_MILLENNIUM && val != DTK_CENTURY) {
                    if (tm->tm_year > 0)
                        tm->tm_year = (tm->tm_year / 10) * 10;
                    else
                        tm->tm_year = -((8 - (tm->tm_year - 1)) / 10) * 10;
                }
                /* FALL THRU */
            case DTK_YEAR:
                tm->tm_mon = 1;
                /* FALL THRU */
            case DTK_QUARTER:
                tm->tm_mon = (3 * ((tm->tm_mon - 1) / 3)) + 1;
                /* FALL THRU */
            case DTK_MONTH:
                tm->tm_mday = 1;
                /* FALL THRU */
            case DTK_DAY:
                tm->tm_hour = 0;
                redotz = true; /* for all cases >= DAY */
                               /* FALL THRU */
            case DTK_HOUR:
                tm->tm_min = 0;
                /* FALL THRU */
            case DTK_MINUTE:
                tm->tm_sec = 0;
                /* FALL THRU */
            case DTK_SECOND:
                fsec = 0;
                break;

            case DTK_MILLISEC:
#ifdef HAVE_INT64_TIMESTAMP
                fsec = (fsec / 1000) * 1000;
#else
                fsec = floor(fsec * 1000) / 1000;
#endif
                break;
            case DTK_MICROSEC:
#ifndef HAVE_INT64_TIMESTAMP
                fsec = floor(fsec * 1000000) / 1000000;
#endif
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("timestamp with time zone units \"%s\" not "
                               "supported",
                            lowunits)));
                result = 0;
        }

        if (redotz)
            tz = DetermineTimeZoneOffset(tm, session_timezone);

        if (tm2timestamp(tm, fsec, &tz, &result) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("timestamp with time zone units \"%s\" not recognized", lowunits)));
        result = 0;
    }

    PG_RETURN_TIMESTAMPTZ(result);
}

/* interval_trunc()
 * Extract specified field from interval.
 */
Datum interval_trunc(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Interval* interval = PG_GETARG_INTERVAL_P(1);
    Interval* result = NULL;
    int type, val;
    char* lowunits = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    result = (Interval*)palloc(sizeof(Interval));

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);

    if (type == UNITS) {
        if (interval2tm(*interval, tm, &fsec) == 0) {
            switch (val) {
                    /* fall through */
                case DTK_MILLENNIUM:
                    /* caution: C division may have negative remainder */
                    tm->tm_year = (tm->tm_year / 1000) * 1000;
                    /* fall through */
                case DTK_CENTURY:
                    /* caution: C division may have negative remainder */
                    tm->tm_year = (tm->tm_year / 100) * 100;
                    /* fall through */
                case DTK_DECADE:
                    /* caution: C division may have negative remainder */
                    tm->tm_year = (tm->tm_year / 10) * 10;
                    /* fall through */
                case DTK_YEAR:
                    tm->tm_mon = 0;
                    /* fall through */
                case DTK_QUARTER:
                    tm->tm_mon = 3 * (tm->tm_mon / 3);
                    /* fall through */
                case DTK_MONTH:
                    tm->tm_mday = 0;
                    /* fall through */
                case DTK_DAY:
                    tm->tm_hour = 0;
                    /* fall through */
                case DTK_HOUR:
                    tm->tm_min = 0;
                    /* fall through */
                case DTK_MINUTE:
                    tm->tm_sec = 0;
                    /* fall through */
                case DTK_SECOND:
                    fsec = 0;
                    break;

                case DTK_MILLISEC:
#ifdef HAVE_INT64_TIMESTAMP
                    fsec = (fsec / 1000) * 1000;
#else
                    fsec = floor(fsec * 1000) / 1000;
#endif
                    break;
                case DTK_MICROSEC:
#ifndef HAVE_INT64_TIMESTAMP
                    fsec = floor(fsec * 1000000) / 1000000;
#endif
                    break;

                default:
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("interval units \"%s\" not supported", lowunits)));
            }

            if (tm2interval(tm, fsec, result) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("interval out of range")));
        } else
            ereport(ERROR, (errcode(ERRCODE_MOST_SPECIFIC_TYPE_MISMATCH), errmsg("could not convert interval to tm")));
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("interval units \"%s\" not recognized", lowunits)));
    }

    PG_RETURN_INTERVAL_P(result);
}

/* timestamp_trunc_alias()
 * truncate timestamp to specified units
 *
 * reuse from timestamp_trunc and
 * provide "trunc" interface for truncating date
 */
Datum timestamp_trunc_alias(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(timestamp_trunc, PG_GETARG_DATUM(1), PG_GETARG_DATUM(0));
}

/* timestamptz_trunc_alias()
 * truncate timestamptz to specified units
 *
 * reuse from timestamptz_trunc and
 * provide "trunc" interface for truncating date
 */
Datum timestamptz_trunc_alias(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(timestamptz_trunc, PG_GETARG_DATUM(1), PG_GETARG_DATUM(0));
}

/* interval_trunc_alias()
 * extract specified field from interval
 *
 * reuse from interval_trunc and
 * provide "trunc" interface for extracting interval
 */
Datum interval_trunc_alias(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall2(interval_trunc, PG_GETARG_DATUM(1), PG_GETARG_DATUM(0));
}

/* isoweek2j()
 *
 *	Return the Julian day which corresponds to the first day (Monday) of the given ISO 8601 year and week.
 *	Julian days are used to convert between ISO week dates and Gregorian dates.
 */
int isoweek2j(int year, int week)
{
    int day0, day4;

    /* fourth day of current year */
    day4 = date2j(year, 1, 4);

    /* day0 == offset to first day of week (Monday) */
    day0 = j2day(day4 - 1);

    return ((week - 1) * 7) + (day4 - day0);
}

/* isoweek2date()
 * Convert ISO week of year number to date.
 * The year field must be specified with the ISO year!
 * karel 2000/08/07
 */
void isoweek2date(int woy, int* year, int* mon, int* mday)
{
    j2date(isoweek2j(*year, woy), year, mon, mday);
}

/* isoweekdate2date()
 *
 *	Convert an ISO 8601 week date (ISO year, ISO week and day of week) into a Gregorian date.
 *	Populates year, mon, and mday with the correct Gregorian values.
 *	year must be passed in as the ISO year.
 */
void isoweekdate2date(int isoweek, int isowday, int* year, int* mon, int* mday)
{
    int jday;

    jday = isoweek2j(*year, isoweek);
    jday += isowday - 1;

    j2date(jday, year, mon, mday);
}

/* date2isoweek()
 *
 *	Returns ISO week number of year.
 */
int date2isoweek(int year, int mon, int mday)
{
    float8 result;
    int day0, day4, dayn;

    /* current day */
    dayn = date2j(year, mon, mday);

    /* fourth day of current year */
    day4 = date2j(year, 1, 4);

    /* day0 == offset to first day of week (Monday) */
    day0 = j2day(day4 - 1);

    /*
     * We need the first week containing a Thursday, otherwise this day falls
     * into the previous year for purposes of counting weeks
     */
    if (dayn < day4 - day0) {
        day4 = date2j(year - 1, 1, 4);

        /* day0 == offset to first day of week (Monday) */
        day0 = j2day(day4 - 1);
    }

    result = (dayn - (day4 - day0)) / 7 + 1;

    /*
     * Sometimes the last few days in a year will fall into the first week of
     * the next year, so check for this.
     */
    if (result >= 52) {
        day4 = date2j(year + 1, 1, 4);

        /* day0 == offset to first day of week (Monday) */
        day0 = j2day(day4 - 1);

        if (dayn >= day4 - day0)
            result = (dayn - (day4 - day0)) / 7 + 1;
    }

    return (int)result;
}

/* date2isoyear()
 *
 *	Returns ISO 8601 year number.
 */
int date2isoyear(int year, int mon, int mday)
{
    float8 result;
    int day0, day4, dayn;

    /* current day */
    dayn = date2j(year, mon, mday);

    /* fourth day of current year */
    day4 = date2j(year, 1, 4);

    /* day0 == offset to first day of week (Monday) */
    day0 = j2day(day4 - 1);

    /*
     * We need the first week containing a Thursday, otherwise this day falls
     * into the previous year for purposes of counting weeks
     */
    if (dayn < day4 - day0) {
        day4 = date2j(year - 1, 1, 4);

        /* day0 == offset to first day of week (Monday) */
        day0 = j2day(day4 - 1);

        year--;
    }

    result = (dayn - (day4 - day0)) / 7 + 1;

    /*
     * Sometimes the last few days in a year will fall into the first week of
     * the next year, so check for this.
     */
    if (result >= 52) {
        day4 = date2j(year + 1, 1, 4);

        /* day0 == offset to first day of week (Monday) */
        day0 = j2day(day4 - 1);

        if (dayn >= day4 - day0)
            year++;
    }

    return year;
}

/* date2isoyearday()
 *
 *	Returns the ISO 8601 day-of-year, given a Gregorian year, month and day.
 *	Possible return values are 1 through 371 (364 in non-leap years).
 */
int date2isoyearday(int year, int mon, int mday)
{
    return date2j(year, mon, mday) - isoweek2j(date2isoyear(year, mon, mday), 1) + 1;
}

/* timestamp_part()
 * Extract specified field from timestamp.
 */
Datum timestamp_part(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    float8 result;
    int type, val;
    char* lowunits = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (TIMESTAMP_NOT_FINITE(timestamp)) {
        result = 0;
        PG_RETURN_FLOAT8(result);
    }

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        switch (val) {
            case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
                result = fsec;
#else
                result = fsec * 1000000;
#endif
                break;

            case DTK_MILLISEC:
#ifdef HAVE_INT64_TIMESTAMP
                result = tm->tm_sec * 1000.0 + fsec / 1000.0;
#else
                result = (tm->tm_sec + fsec) * 1000;
#endif
                break;

            case DTK_SECOND:
#ifdef HAVE_INT64_TIMESTAMP
                result = tm->tm_sec;
#else
                result = tm->tm_sec;
#endif
                break;

            case DTK_MINUTE:
                result = tm->tm_min;
                break;

            case DTK_HOUR:
                result = tm->tm_hour;
                break;

            case DTK_DAY:
                result = tm->tm_mday;
                break;

            case DTK_MONTH:
                result = tm->tm_mon;
                break;

            case DTK_QUARTER:
                result = (tm->tm_mon - 1) / 3 + 1;
                break;

            case DTK_WEEK:
                result = (float8)date2isoweek(tm->tm_year, tm->tm_mon, tm->tm_mday);
                break;

            case DTK_YEAR:
                if (tm->tm_year > 0)
                    result = tm->tm_year;
                else
                    /* there is no year 0, just 1 BC and 1 AD */
                    result = tm->tm_year - 1;
                break;

            case DTK_DECADE:

                /*
                 * what is a decade wrt dates? let us assume that decade 199
                 * is 1990 thru 1999... decade 0 starts on year 1 BC, and -1
                 * is 11 BC thru 2 BC...
                 */
                if (tm->tm_year >= 0)
                    result = tm->tm_year / 10;
                else
                    result = -((8 - (tm->tm_year - 1)) / 10);
                break;

            case DTK_CENTURY:

                /* ----
                 * centuries AD, c>0: year in [ (c-1)* 100 + 1 : c*100 ]
                 * centuries BC, c<0: year in [ c*100 : (c+1) * 100 - 1]
                 * there is no number 0 century.
                 * ----
                 */
                if (tm->tm_year > 0)
                    result = (tm->tm_year + 99) / 100;
                else
                    /* caution: C division may have negative remainder */
                    result = -((99 - (tm->tm_year - 1)) / 100);
                break;

            case DTK_MILLENNIUM:
                /* see comments above. */
                if (tm->tm_year > 0)
                    result = (tm->tm_year + 999) / 1000;
                else
                    result = -((999 - (tm->tm_year - 1)) / 1000);
                break;

            case DTK_JULIAN:
                result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday);
#ifdef HAVE_INT64_TIMESTAMP
                result += ((((tm->tm_hour * MINS_PER_HOUR) + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec +
                              (fsec / 1000000.0)) /
                          (double)SECS_PER_DAY;
#else
                result += ((((tm->tm_hour * MINS_PER_HOUR) + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec + fsec) /
                          (double)SECS_PER_DAY;
#endif
                break;

            case DTK_ISOYEAR:
                result = date2isoyear(tm->tm_year, tm->tm_mon, tm->tm_mday);
                break;
            case DTK_DOW:
            case DTK_ISODOW:
                if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
                    ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                result = j2day(date2j(tm->tm_year, tm->tm_mon, tm->tm_mday));
                if (val == DTK_ISODOW && result == 0)
                    result = 7;
                break;

            case DTK_DOY:
                if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
                    ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                result = (date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - date2j(tm->tm_year, 1, 1) + 1);
                break;

            case DTK_TZ:
            case DTK_TZ_MINUTE:
            case DTK_TZ_HOUR:
            default:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("timestamp units \"%s\" not supported", lowunits)));
                result = 0;
        }
    } else if (type == RESERV) {
        switch (val) {
            case DTK_EPOCH:
#ifdef HAVE_INT64_TIMESTAMP
                result = (timestamp - SetEpochTimestamp()) / 1000000.0;
#else
                result = timestamp - SetEpochTimestamp();
#endif
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("timestamp units \"%s\" not supported", lowunits)));
                result = 0;
        }

    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("timestamp units \"%s\" not recognized", lowunits)));
        result = 0;
    }

    PG_RETURN_FLOAT8(result);
}

/* datetime_year_part()
 * Extract year from b datebase datetime
 */
Datum datetime_year_part(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    float8 result = 0;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0)
        result = tm->tm_year;
    else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    PG_RETURN_FLOAT8(result);
}

Datum text_year_part(PG_FUNCTION_ARGS)
{
    Timestamp timestamp;
    char* str = TextDatumGetCString(PG_GETARG_TEXT_PP(0));
    if (!datetime_in_no_ereport(str, &timestamp)) {
        PG_RETURN_NULL();
    }
    float8 result = 0;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0 && tm->tm_year <= MAX_YEAR) {
        result = tm->tm_year;
    } else {
        PG_RETURN_NULL();
    }
    PG_RETURN_FLOAT8(result);
}

/* timestamptz_part()
 * Extract specified field from timestamp with time zone.
 */
Datum timestamptz_part(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(1);
    float8 result;
    int tz;
    int type, val;
    char* lowunits = NULL;
    double dummy;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (TIMESTAMP_NOT_FINITE(timestamp)) {
        result = 0;
        PG_RETURN_FLOAT8(result);
    }

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        switch (val) {
            case DTK_TZ:
                result = -tz;
                break;

            case DTK_TZ_MINUTE:
                result = -tz;
                result /= MINS_PER_HOUR;
                FMODULO(result, dummy, (double)MINS_PER_HOUR);
                break;

            case DTK_TZ_HOUR:
                dummy = -tz;
                FMODULO(dummy, result, (double)SECS_PER_HOUR);
                break;

            case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
                result = fsec;
#else
                result = fsec * 1000000;
#endif
                break;

            case DTK_MILLISEC:
#ifdef HAVE_INT64_TIMESTAMP
                result = tm->tm_sec * 1000.0 + fsec / 1000.0;
#else
                result = (tm->tm_sec + fsec) * 1000;
#endif
                break;

            case DTK_SECOND:
                result = tm->tm_sec;
                break;

            case DTK_MINUTE:
                result = tm->tm_min;
                break;

            case DTK_HOUR:
                result = tm->tm_hour;
                break;

            case DTK_DAY:
                result = tm->tm_mday;
                break;

            case DTK_MONTH:
                result = tm->tm_mon;
                break;

            case DTK_QUARTER:
                result = (tm->tm_mon - 1) / 3 + 1;
                break;

            case DTK_WEEK:
                result = (float8)date2isoweek(tm->tm_year, tm->tm_mon, tm->tm_mday);
                break;

            case DTK_YEAR:
                if (tm->tm_year > 0)
                    result = tm->tm_year;
                else
                    /* there is no year 0, just 1 BC and 1 AD */
                    result = tm->tm_year - 1;
                break;

            case DTK_DECADE:
                /* see comments in timestamp_part */
                if (tm->tm_year > 0)
                    result = tm->tm_year / 10;
                else
                    result = -((8 - (tm->tm_year - 1)) / 10);
                break;

            case DTK_CENTURY:
                /* see comments in timestamp_part */
                if (tm->tm_year > 0)
                    result = (tm->tm_year + 99) / 100;
                else
                    result = -((99 - (tm->tm_year - 1)) / 100);
                break;

            case DTK_MILLENNIUM:
                /* see comments in timestamp_part */
                if (tm->tm_year > 0)
                    result = (tm->tm_year + 999) / 1000;
                else
                    result = -((999 - (tm->tm_year - 1)) / 1000);
                break;

            case DTK_JULIAN:
                result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday);
#ifdef HAVE_INT64_TIMESTAMP
                result += ((((tm->tm_hour * MINS_PER_HOUR) + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec +
                              (fsec / 1000000.0)) /
                          (double)SECS_PER_DAY;
#else
                result += ((((tm->tm_hour * MINS_PER_HOUR) + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec + fsec) /
                          (double)SECS_PER_DAY;
#endif
                break;

            case DTK_ISOYEAR:
                result = date2isoyear(tm->tm_year, tm->tm_mon, tm->tm_mday);
                break;

            case DTK_DOW:
            case DTK_ISODOW:
                if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
                    ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                result = j2day(date2j(tm->tm_year, tm->tm_mon, tm->tm_mday));
                if (val == DTK_ISODOW && result == 0)
                    result = 7;
                break;

            case DTK_DOY:
                if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
                    ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                result = (date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - date2j(tm->tm_year, 1, 1) + 1);
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("timestamp with time zone units \"%s\" not supported", lowunits)));
                result = 0;
        }

    } else if (type == RESERV) {
        switch (val) {
            case DTK_EPOCH:
#ifdef HAVE_INT64_TIMESTAMP
                result = (timestamp - SetEpochTimestamp()) / 1000000.0;
#else
                result = timestamp - SetEpochTimestamp();
#endif
                break;

            default:
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("timestamp with time zone units \"%s\" not supported", lowunits)));
                result = 0;
        }
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("timestamp with time zone units \"%s\" not recognized", lowunits)));

        result = 0;
    }

    PG_RETURN_FLOAT8(result);
}

static float8 get_interval_by_val(int val, struct pg_tm* tm, fsec_t fsec, char* lowunits)
{
    float8 result;
    switch (val) {
        case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
            result = tm->tm_sec * 1000000.0 + fsec;
#else
            result = (tm->tm_sec + fsec) * 1000000;
#endif
            break;

        case DTK_MILLISEC:
#ifdef HAVE_INT64_TIMESTAMP
            result = tm->tm_sec * 1000.0 + fsec / 1000.0;
#else
            result = (tm->tm_sec + fsec) * 1000;
#endif
            break;

        case DTK_SECOND:
#ifdef HAVE_INT64_TIMESTAMP
            result = tm->tm_sec + fsec / 1000000.0;
#else
            result = tm->tm_sec + fsec;
#endif
            break;

        case DTK_MINUTE:
            result = tm->tm_min;
            break;

        case DTK_HOUR:
            result = tm->tm_hour;
            break;

        case DTK_DAY:
            result = tm->tm_mday;
            break;

        case DTK_MONTH:
            result = tm->tm_mon;
            break;

        case DTK_QUARTER:
            result = (tm->tm_mon / 3) + 1;
            break;

        case DTK_YEAR:
            result = tm->tm_year;
            break;

        case DTK_DECADE:
            /* caution: C division may have negative remainder */
            result = tm->tm_year / 10;
            break;

        case DTK_CENTURY:
            /* caution: C division may have negative remainder */
            result = tm->tm_year / 100;
            break;

        case DTK_MILLENNIUM:
            /* caution: C division may have negative remainder */
            result = tm->tm_year / 1000;
            break;

        default:
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("interval units \"%s\" not supported", lowunits)));
            result = 0;
    }
    return result;
}

/* interval_part()
 * Extract specified field from interval.
 */
Datum interval_part(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Interval* interval = PG_GETARG_INTERVAL_P(1);
    float8 result;
    int type, val;
    char* lowunits = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        if (interval2tm(*interval, tm, &fsec) == 0) {
            result = get_interval_by_val(val, tm, fsec, lowunits);
        } else {
            ereport(ERROR, (errcode(ERRCODE_MOST_SPECIFIC_TYPE_MISMATCH), errmsg("could not convert interval to tm")));
            result = 0;
        }
    } else if (type == RESERV && val == DTK_EPOCH) {
#ifdef HAVE_INT64_TIMESTAMP
        result = interval->time / 1000000.0;
#else
        result = interval->time;
#endif
        result += ((double)DAYS_PER_YEAR * SECS_PER_DAY) * (interval->month / MONTHS_PER_YEAR);
        result += ((double)DAYS_PER_MONTH * SECS_PER_DAY) * (interval->month % MONTHS_PER_YEAR);
        result += ((double)SECS_PER_DAY) * interval->day;
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("interval units \"%s\" not recognized", lowunits)));
        result = 0;
    }

    PG_RETURN_FLOAT8(result);
}

/*	timestamp_zone()
 *	Encode timestamp type with specified time zone.
 *	This function is just timestamp2timestamptz() except instead of
 *	shifting to the global timezone, we shift to the specified timezone.
 *	This is different from the other AT TIME ZONE cases because instead
 *	of shifting to a _to_ a new time zone, it sets the time to _be_ the
 *	specified timezone.
 */
Datum timestamp_zone(PG_FUNCTION_ARGS)
{
    text* zone = PG_GETARG_TEXT_PP(0);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    TimestampTz result;
    int tz;
    char tzname[TZ_STRLEN_MAX + 1];
    char* lowzone = NULL;
    int type, val;
    pg_tz* tzp = NULL;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_TIMESTAMPTZ(timestamp);

    /*
     * Look up the requested timezone.	First we look in the date token table
     * (to handle cases like "EST"), and if that fails, we look in the
     * timezone database (to handle cases like "America/New_York").  (This
     * matches the order in which timestamp input checks the cases; it's
     * important because the timezone database unwisely uses a few zone names
     * that are identical to offset abbreviations.)
     */
    text_to_cstring_buffer(zone, tzname, sizeof(tzname));
    lowzone = downcase_truncate_identifier(tzname, strlen(tzname), false);

    type = DecodeSpecial(0, lowzone, &val);

    if (type == TZ || type == DTZ) {
        tz = -(val * MINS_PER_HOUR);
        result = dt2local(timestamp, tz);
    } else {
        tzp = pg_tzset(tzname);
        if (tzp != NULL) {
            /* Apply the timezone change */
            struct pg_tm tm;
            fsec_t fsec;

            if (timestamp2tm(timestamp, NULL, &tm, &fsec, NULL, tzp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
            tz = DetermineTimeZoneOffset(&tm, tzp);
            if (tm2timestamp(&tm, fsec, &tz, &result) != 0)
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("could not convert to time zone \"%s\"", tzname)));
        } else {
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("time zone \"%s\" not recognized", tzname)));
            result = 0; /* keep compiler quiet */
        }
    }

    PG_RETURN_TIMESTAMPTZ(result);
}

/* timestamp_izone()
 * Encode timestamp type with specified time interval as time zone.
 */
Datum timestamp_izone(PG_FUNCTION_ARGS)
{
    Interval* zone = PG_GETARG_INTERVAL_P(0);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    TimestampTz result;
    int tz;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_TIMESTAMPTZ(timestamp);

    if (zone->month != 0)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("interval time zone \"%s\" must not specify month",
                    DatumGetCString(DirectFunctionCall1(interval_out, PointerGetDatum(zone))))));

#ifdef HAVE_INT64_TIMESTAMP
    tz = zone->time / USECS_PER_SEC;
#else
    tz = zone->time;
#endif

    result = dt2local(timestamp, tz);

    PG_RETURN_TIMESTAMPTZ(result);
} /* timestamp_izone() */

/* timestamp_timestamptz()
 * Convert local timestamp to timestamp at GMT
 */
Datum timestamp_timestamptz(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);

    PG_RETURN_TIMESTAMPTZ(timestamp2timestamptz(timestamp));
}

TimestampTz timestamp2timestamptz(Timestamp timestamp)
{
    TimestampTz result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        result = timestamp;
    else {
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        tz = DetermineTimeZoneOffset(tm, session_timezone);

        if (tm2timestamp(tm, fsec, &tz, &result) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }

    return result;
}

/* timestamptz_datetime()
 * Convert timestamp at GMT to datetime type
 */
Datum timestamptz_datetime(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    Timestamp result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        result = timestamp;
    else {
        if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        if (tm2timestamp(tm, fsec, NULL, &result) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    PG_RETURN_TIMESTAMP(result);
}

/* timestamptz_zone()
 * Evaluate timestamp with time zone type at the specified time zone.
 * Returns a timestamp without time zone.
 */
Datum timestamptz_zone(PG_FUNCTION_ARGS)
{
    text* zone = PG_GETARG_TEXT_PP(0);
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(1);
    Timestamp result;
    int tz;
    char tzname[TZ_STRLEN_MAX + 1];
    char* lowzone = NULL;
    int type, val;
    pg_tz* tzp = NULL;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_TIMESTAMP(timestamp);

    /*
     * Look up the requested timezone.	First we look in the date token table
     * (to handle cases like "EST"), and if that fails, we look in the
     * timezone database (to handle cases like "America/New_York").  (This
     * matches the order in which timestamp input checks the cases; it's
     * important because the timezone database unwisely uses a few zone names
     * that are identical to offset abbreviations.)
     */
    text_to_cstring_buffer(zone, tzname, sizeof(tzname));
    lowzone = downcase_truncate_identifier(tzname, strlen(tzname), false);

    type = DecodeSpecial(0, lowzone, &val);

    if (type == TZ || type == DTZ) {
        tz = val * MINS_PER_HOUR;
        result = dt2local(timestamp, tz);
    } else {
        tzp = pg_tzset(tzname);
        if (tzp != NULL) {
            /* Apply the timezone change */
            struct pg_tm tm;
            fsec_t fsec;

            if (timestamp2tm(timestamp, &tz, &tm, &fsec, NULL, tzp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
            if (tm2timestamp(&tm, fsec, NULL, &result) != 0)
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("could not convert to time zone \"%s\"", tzname)));
        } else {
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("time zone \"%s\" not recognized", tzname)));
            result = 0; /* keep compiler quiet */
        }
    }

    PG_RETURN_TIMESTAMP(result);
}

/* timestamptz_izone()
 * Encode timestamp with time zone type with specified time interval as time zone.
 * Returns a timestamp without time zone.
 */
Datum timestamptz_izone(PG_FUNCTION_ARGS)
{
    Interval* zone = PG_GETARG_INTERVAL_P(0);
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(1);
    Timestamp result;
    int tz;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_TIMESTAMP(timestamp);

    if (zone->month != 0)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("interval time zone \"%s\" must not specify month",
                    DatumGetCString(DirectFunctionCall1(interval_out, PointerGetDatum(zone))))));

#ifdef HAVE_INT64_TIMESTAMP
    tz = -(zone->time / USECS_PER_SEC);
#else
    tz = -zone->time;
#endif

    result = dt2local(timestamp, tz);

    PG_RETURN_TIMESTAMP(result);
}

/* generate_series_timestamp()
 * Generate the set of timestamps from start to finish by step
 */
Datum generate_series_timestamp(PG_FUNCTION_ARGS)
{
    FuncCallContext* funcctx = NULL;
    generate_series_timestamp_fctx* fctx = NULL;
    Timestamp result;

    /* stuff done only on the first call of the function */
    if (SRF_IS_FIRSTCALL()) {
        Timestamp start = PG_GETARG_TIMESTAMP(0);
        Timestamp finish = PG_GETARG_TIMESTAMP(1);
        Interval* step = PG_GETARG_INTERVAL_P(2);
        MemoryContext oldcontext;
        Interval interval_zero;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /*
         * switch to memory context appropriate for multiple function calls
         */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* allocate memory for user context */
        fctx = (generate_series_timestamp_fctx*)palloc(sizeof(generate_series_timestamp_fctx));

        /*
         * Use fctx to keep state from call to call. Seed current with the
         * original start value
         */
        fctx->current = start;
        fctx->finish = finish;
        fctx->step = *step;

        /* Determine sign of the interval */
        errno_t rc = memset_s(&interval_zero, sizeof(Interval), 0, sizeof(Interval));
        securec_check(rc, "", "");
        fctx->step_sign = interval_cmp_internal(&fctx->step, &interval_zero);

        if (fctx->step_sign == 0)
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("step size cannot equal zero")));

        funcctx->user_fctx = fctx;
        MemoryContextSwitchTo(oldcontext);
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();

    /*
     * get the saved state and use current as the result for this iteration
     */
    fctx = (generate_series_timestamp_fctx*)funcctx->user_fctx;
    result = fctx->current;

    if (fctx->step_sign > 0 ? timestamp_cmp_internal(result, fctx->finish) <= 0
                            : timestamp_cmp_internal(result, fctx->finish) >= 0) {
        /* increment current in preparation for next iteration */
        fctx->current = DatumGetTimestamp(
            DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(fctx->current), PointerGetDatum(&fctx->step)));

        /* do when there is more left to send */
        SRF_RETURN_NEXT(funcctx, TimestampGetDatum(result));
    } else {
        /* do when there is no more left */
        SRF_RETURN_DONE(funcctx);
    }
}

/* generate_series_timestamptz()
 * Generate the set of timestamps from start to finish by step
 */
Datum generate_series_timestamptz(PG_FUNCTION_ARGS)
{
    FuncCallContext* funcctx = NULL;
    generate_series_timestamptz_fctx* fctx = NULL;
    TimestampTz result;

    /* stuff done only on the first call of the function */
    if (SRF_IS_FIRSTCALL()) {
        TimestampTz start = PG_GETARG_TIMESTAMPTZ(0);
        TimestampTz finish = PG_GETARG_TIMESTAMPTZ(1);
        Interval* step = PG_GETARG_INTERVAL_P(2);
        MemoryContext oldcontext;
        Interval interval_zero;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /*
         * switch to memory context appropriate for multiple function calls
         */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* allocate memory for user context */
        fctx = (generate_series_timestamptz_fctx*)palloc(sizeof(generate_series_timestamptz_fctx));

        /*
         * Use fctx to keep state from call to call. Seed current with the
         * original start value
         */
        fctx->current = start;
        fctx->finish = finish;
        fctx->step = *step;

        /* Determine sign of the interval */
        MemSet(&interval_zero, 0, sizeof(Interval));
        fctx->step_sign = interval_cmp_internal(&fctx->step, &interval_zero);

        if (fctx->step_sign == 0)
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("step size cannot equal zero")));

        funcctx->user_fctx = fctx;
        MemoryContextSwitchTo(oldcontext);
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();

    /*
     * get the saved state and use current as the result for this iteration
     */
    fctx = (generate_series_timestamptz_fctx*)funcctx->user_fctx;
    result = fctx->current;

    if (fctx->step_sign > 0 ? timestamp_cmp_internal(result, fctx->finish) <= 0
                            : timestamp_cmp_internal(result, fctx->finish) >= 0) {
        /* increment current in preparation for next iteration */
        fctx->current = DatumGetTimestampTz(DirectFunctionCall2(
            timestamptz_pl_interval, TimestampTzGetDatum(fctx->current), PointerGetDatum(&fctx->step)));

        /* do when there is more left to send */
        SRF_RETURN_NEXT(funcctx, TimestampTzGetDatum(result));
    } else {
        /* do when there is no more left */
        SRF_RETURN_DONE(funcctx);
    }
}

#ifdef PGXC
Datum interval_collect(PG_FUNCTION_ARGS)
{
    ArrayType* collectarray = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType* transarray = PG_GETARG_ARRAYTYPE_P(1);
    Datum* collectdatums = NULL;
    Datum* transdatums = NULL;
    int ndatums;
    Interval sumX1, N1, sumX2, N2;
    Interval* newsum = NULL;
    ArrayType* result = NULL;

    deconstruct_array(collectarray, INTERVALOID, sizeof(Interval), false, 'd', &collectdatums, NULL, &ndatums);
    if (ndatums != 2)
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("expected 2-element interval array")));

    deconstruct_array(transarray, INTERVALOID, sizeof(Interval), false, 'd', &transdatums, NULL, &ndatums);
    if (ndatums != 2)
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("expected 2-element interval array")));

    /*
     * XXX memcpy, instead of just extracting a pointer, to work around buggy
     * array code: it won't ensure proper alignment of Interval objects on
     * machines where double requires 8-byte alignment. That should be fixed,
     * but in the meantime...
     *
     * Note: must use DatumGetPointer here, not DatumGetIntervalP, else some
     * compilers optimize into double-aligned load/store anyway.
     */
    int rc = 0;
    rc = memcpy_s((void*)&sumX1, sizeof(Interval), DatumGetPointer(collectdatums[0]), sizeof(Interval));
    securec_check(rc, "\0", "\0");
    rc = memcpy_s((void*)&N1, sizeof(Interval), DatumGetPointer(collectdatums[1]), sizeof(Interval));
    securec_check(rc, "\0", "\0");
    rc = memcpy_s((void*)&sumX2, sizeof(Interval), DatumGetPointer(transdatums[0]), sizeof(Interval));
    securec_check(rc, "\0", "\0");
    rc = memcpy_s((void*)&N2, sizeof(Interval), DatumGetPointer(transdatums[1]), sizeof(Interval));
    securec_check(rc, "\0", "\0");

    newsum = DatumGetIntervalP(DirectFunctionCall2(interval_pl, IntervalPGetDatum(&sumX1), IntervalPGetDatum(&sumX2)));
    N1.time += N2.time;

    collectdatums[0] = IntervalPGetDatum(newsum);
    collectdatums[1] = IntervalPGetDatum(&N1);

    result = construct_array(collectdatums, 2, INTERVALOID, sizeof(Interval), false, 'd');

    PG_RETURN_ARRAYTYPE_P(result);
}
#endif

// In order to simulate A db's interval type, it need adjust interval's input string
// format according to input character of mode
static void interval_format_adjust(Interval* interval, char type_mode)
{
    int32 extra_days = 0;
    int64 seconds_per_unit = 1;

    switch (type_mode) {
        case 'H':
#ifdef HAVE_INT64_TIMESTAMP
            seconds_per_unit = USECS_PER_HOUR;
#else
            seconds_per_unit = SECS_PER_HOUR;
#endif
            break;

        case 'M':
#ifdef HAVE_INT64_TIMESTAMP
            seconds_per_unit = USECS_PER_MINUTE;
#else
            seconds_per_unit = SECS_PER_MINUTE;
#endif
            break;

        case 'S':
            seconds_per_unit = 1;
            break;

        default:
            return;
    }

#ifdef HAVE_INT64_TIMESTAMP
    extra_days = interval->time / USECS_PER_DAY;
    interval->day += extra_days;
    interval->time = ((interval->time / seconds_per_unit) * seconds_per_unit) - (extra_days * USECS_PER_DAY);
#else
    extra_days = (int32)(interval->time / SECS_PER_DAY);
    interval->day += extra_days;
    interval->time =
        (((int32)(interval->time / seconds_per_unit)) * (double)seconds_per_unit) - (extra_days * (double)SECS_PER_DAY);
#endif
}

// Used to adjust computing result of interval type.
static void interval_result_adjust(Interval* result)
{

    while ((result->day > 0) && (result->time < 0)) {
#ifdef HAVE_INT64_TIMESTAMP
        result->day -= (int32)1;
        result->time += USECS_PER_DAY;
#else
        result->day -= (int32)1;
        result->time += (double)SECS_PER_DAY;
#endif
    }

    while ((result->day < 0) && (result->time > 0)) {
#ifdef HAVE_INT64_TIMESTAMP
        result->day += (int32)1;
        result->time -= USECS_PER_DAY;
#else
        result->day += (int32)1;
        result->time -= (double)SECS_PER_DAY;
#endif
    }

    if (result->time != 0)
        interval_format_adjust(result, 'S');
}

/*
 * split the a date-string into vary single units which is year,
 * month, mday, hour, minute, second. And assign these unit values
 * to the struct pg_tm, containing the date elements.
 */
struct pg_tm* GetDateDetail(const char* dateString)
{
    struct pg_tm* tm = NULL;
    int i;
    /*the length of dateString*/
    int strLength;
    int spaceCount = 0;
    /*
     * if the format is correct, the spacePosition[0] will be the position of the
     * space which is used to separate the dateString.The front part of the string
     * is specific date and the back part is the specific time.
     */
    int spacePosition[5] = {0};
    /*the specific date in the string*/
    char dateStr[DATESTR_LEN] = {0};
    /*the specific time in the string*/
    char timeStr[TIMESTR_LEN] = {0};

    if (NULL == dateString) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("the format is not correct")));
        return NULL;
    }

    tm = (struct pg_tm*)palloc(sizeof(struct pg_tm));
    tm->tm_year = 0;
    tm->tm_mon = 0;
    tm->tm_mday = 0;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;

    strLength = strlen(dateString);

    for (i = 0; i < strLength; i++) {
        if (' ' == dateString[i]) {
            if (spaceCount >= 5) {
                pfree(tm);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("the format is not correct")));
            }
            spacePosition[spaceCount] = i;
            spaceCount++;
        }
    }
    /* there is no space in the date-string*/
    if (0 == spaceCount) {
        /* assume that the string is this kind of fommat like "19900304123045"*/
        if (DATE_WITHOUT_SPC_LEN == strLength) {
            SplitWholeStrWithoutSeparator(dateString, tm);
        }
        /* there is only specific date in the string but no time information in the string*/
        else if ((MAXLEN_DATE >= strLength) && (MINLEN_DATE <= strLength)) {
            AnalyseDate(dateString, tm);
        } else {
            pfree_ext(tm);
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        }
    }
    /* there is one space in the date-string*/
    else if (1 == spaceCount) {
        /* get the specific date*/
        errno_t rc = strncpy_s(dateStr, DATESTR_LEN, dateString, spacePosition[0]);
        securec_check(rc, "\0", "\0");
        /* get the specific time*/
        rc = strncpy_s(timeStr, TIMESTR_LEN, dateString + spacePosition[0] + 1, strLength - spacePosition[0]);
        securec_check(rc, "\0", "\0");

        AnalyseDate(dateStr, tm);
        AnalyseTime(timeStr, tm);
    }
    /* there are more than one space in the string*/
    else {
        pfree_ext(tm);
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
    }

    CheckDateValidity(tm);

    return tm;
}

/*
 * analyse the whole string without the space and any separator,
 * and split the string into year, month , day, hour, minute, second
 */
void SplitWholeStrWithoutSeparator(const char* dateString, struct pg_tm* tm)
{
    char year[UNIT_LEN] = {0};
    char month[UNIT_LEN] = {0};
    char day[UNIT_LEN] = {0};
    char hour[UNIT_LEN] = {0};
    char minute[UNIT_LEN] = {0};
    char second[UNIT_LEN] = {0};
    int strLength = 0;
    int i = 0;
    int nonDigitCount = 0;

    if (NULL == dateString || NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    strLength = strlen(dateString);

    for (i = 0; i < strLength; i++) {
        if (dateString[i] < '0' || dateString[i] > '9') {
            nonDigitCount++;
        }
    }
    if (0 == nonDigitCount) {
        /* get year*/
        errno_t rc = strncpy_s(year, UNIT_LEN, dateString, FOUR_DIGIT_LEN);
        securec_check(rc, "\0", "\0");
        /* get month*/
        rc = strncpy_s(month, UNIT_LEN, dateString + FOUR_DIGIT_LEN, TWO_DIGIT_LEN);
        securec_check(rc, "\0", "\0");
        /* get day*/
        rc = strncpy_s(day, UNIT_LEN, dateString + FOUR_DIGIT_LEN + TWO_DIGIT_LEN, TWO_DIGIT_LEN);
        securec_check(rc, "\0", "\0");
        /* get hour*/
        rc = strncpy_s(hour, UNIT_LEN, dateString + FOUR_DIGIT_LEN + TWO_DIGIT_LEN * 2, TWO_DIGIT_LEN);
        securec_check(rc, "\0", "\0");
        /* get minute*/
        rc = strncpy_s(minute, UNIT_LEN, dateString + FOUR_DIGIT_LEN + TWO_DIGIT_LEN * 3, TWO_DIGIT_LEN);
        securec_check(rc, "\0", "\0");
        /* get second*/
        rc = strncpy_s(second, UNIT_LEN, dateString + FOUR_DIGIT_LEN + TWO_DIGIT_LEN * 4, TWO_DIGIT_LEN);
        securec_check(rc, "\0", "\0");

        tm->tm_year = atoi(year);
        tm->tm_mon = atoi(month);
        tm->tm_mday = atoi(day);
        tm->tm_hour = atoi(hour);
        tm->tm_min = atoi(minute);
        tm->tm_sec = atoi(second);
    } else {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
    }
}

/*
 * analyse the date part of the whole string, and split the
 * string into year, month,day.
 */
void AnalyseDate(const char* dateString, struct pg_tm* tm_date)
{
    int strLength = 0;
    int i = 0;
    int dateSeparatorCount = 0;
    int dateSeparatorPosition[5] = {0};

    if (NULL == dateString || NULL == tm_date) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    strLength = strlen(dateString);

    for (i = 0; i < strLength; i++) {
        if (dateString[i] < '0' || dateString[i] > '9') {
            if (dateSeparatorCount >= 5) {
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("the format is not correct")));
            }
            dateSeparatorPosition[dateSeparatorCount] = i;
            dateSeparatorCount++;
        }
    }

    /* start to get the year,mon,day separately*/
    if (0 == dateSeparatorCount && MINLEN_DATE == strLength) {
        SplitDatestrWithoutSeparator(dateString, tm_date);
    } else if (2 == dateSeparatorCount) {
        SplitDatestrBySeparator(dateString, strLength, dateSeparatorPosition, tm_date);
    } else {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct!")));
    }
}

/*
 * split the string by separator into year, month , day.
 * Assign values of year, month, day to the struct pg_tm.
 */
void SplitDatestrBySeparator(const char* dateString, int strLength, const int* separatorPosition, struct pg_tm* tm_date)
{
    char year[UNIT_LEN] = {0};
    char month[UNIT_LEN] = {0};
    char day[UNIT_LEN] = {0};

    if (NULL == dateString || NULL == separatorPosition || NULL == tm_date) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    if (MINLEN_DATE > strLength || MAXLEN_DATE < strLength) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));

    }
    /* check the position of the oparator .4 is first separator position 6 and 7 are the possible second separator
       position*/
    else if (4 != separatorPosition[0]) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the year is invalid")));
    } else if (6 != separatorPosition[1] && 7 != separatorPosition[1]) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the month is invalid!")));
    } else {
        /* get the year, mon , day separately in char*/
        errno_t errorno = EOK;
        errorno = strncpy_s(year, UNIT_LEN, dateString, FOUR_DIGIT_LEN);
        securec_check(errorno, "\0", "\0");
        errorno = strncpy_s(
            month, UNIT_LEN, dateString + separatorPosition[0] + 1, separatorPosition[1] - separatorPosition[0] - 1);
        securec_check(errorno, "\0", "\0");
        errorno = strncpy_s(day, UNIT_LEN, dateString + separatorPosition[1] + 1, strLength - separatorPosition[1] - 1);
        securec_check(errorno, "\0", "\0");

        /* transfer char to int*/
        tm_date->tm_year = atoi(year);
        tm_date->tm_mon = atoi(month);
        tm_date->tm_mday = atoi(day);
    }
}

/*
 * split the string without separator into year, month , day.
 * Assign values of year, month, day to the struct pg_tm.
 */
void SplitDatestrWithoutSeparator(const char* dateString, struct pg_tm* tm_date)
{
    char year[UNIT_LEN] = {0};
    char month[UNIT_LEN] = {0};
    char day[UNIT_LEN] = {0};
    int errorno = 0;

    if (NULL == dateString || NULL == tm_date) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    /* get the year, mon , day separately in char*/
    errorno = strncpy_s(year, UNIT_LEN, dateString, FOUR_DIGIT_LEN);
    securec_check(errorno, "\0", "\0");

    errorno = strncpy_s(month, UNIT_LEN, dateString + FOUR_DIGIT_LEN, TWO_DIGIT_LEN);
    securec_check(errorno, "\0", "\0");

    errorno = strncpy_s(day, UNIT_LEN, dateString + FOUR_DIGIT_LEN + TWO_DIGIT_LEN, TWO_DIGIT_LEN);
    securec_check(errorno, "\0", "\0");

    /* transfer char to int*/
    tm_date->tm_year = atoi(year);
    tm_date->tm_mon = atoi(month);
    tm_date->tm_mday = atoi(day);
}

/*
 * analyse the time part of the whole string, and split the string into
 * hour, minute, second.
 */
void AnalyseTime(const char* timeString, struct pg_tm* tm_time)
{
    int strLength;
    int i;

    int timeSeparatorCount = 0;
    int timeSeparatorPosition[5] = {0};

    if (NULL == timeString || NULL == tm_time) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    strLength = strlen(timeString);
    for (i = 0; i < strLength; i++) {
        if (timeString[i] < '0' || timeString[i] > '9') {
            timeSeparatorPosition[timeSeparatorCount] = i;
            timeSeparatorCount++;
            if (timeSeparatorCount > 5) {
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("the format is not correct")));
            }
        }
    }
    /* start to get the hour,minute,second separately*/
    if (0 == timeSeparatorCount && MAXLEN_TIME_WITHOUT_SPRTR == strLength) {

        SplitTimestrWithoutSeparator(timeString, tm_time);
    } else if (2 == timeSeparatorCount) {
        SplitTimestrBySeparator(timeString, strLength, timeSeparatorPosition, tm_time);
    }
    /* there are more than two separartors in the time-string*/
    else {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct\n")));
    }
}

/*
 * split the string without separator into hour, minute, second.
 * Assign values of hour, minute, second to the struct pg_tm.
 */
void SplitTimestrWithoutSeparator(const char* timeString, struct pg_tm* tm_time)
{
    char hour[UNIT_LEN] = {0};
    char minute[UNIT_LEN] = {0};
    char second[UNIT_LEN] = {0};

    if (NULL == timeString || NULL == tm_time) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    /* get the hour, minute,second separately in char*/
    errno_t errorno = EOK;
    errorno = strncpy_s(hour, UNIT_LEN, timeString, TWO_DIGIT_LEN);
    securec_check(errorno, "\0", "\0");
    errorno = strncpy_s(minute, UNIT_LEN, timeString + TWO_DIGIT_LEN, TWO_DIGIT_LEN);
    securec_check(errorno, "\0", "\0");
    errorno = strncpy_s(second, UNIT_LEN, timeString + TWO_DIGIT_LEN * 2, TWO_DIGIT_LEN);
    securec_check(errorno, "\0", "\0");

    /* transfer char to int*/
    tm_time->tm_hour = atoi(hour);
    tm_time->tm_min = atoi(minute);
    tm_time->tm_sec = atoi(second);
}

/*
 * split the string by separator into hour, minute, second.
 * Assign values of hour, minute, second to the struct pg_tm.
 */
void SplitTimestrBySeparator(const char* timeString, int strLength, const int* separatorPosition, struct pg_tm* tm_time)
{
    char hour[UNIT_LEN] = {0};
    char minute[UNIT_LEN] = {0};
    char second[UNIT_LEN] = {0};

    if (NULL == timeString || NULL == separatorPosition || NULL == tm_time) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    if (MINLEN_TIME > strLength || MAXLEN_TIME < strLength) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the time is not correct!")));
    }
    /* check the position of the oparator .1 and 2 are the posiible position of the first separator*/
    else if (1 != separatorPosition[0] && 2 != separatorPosition[0]) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the hour is invalid!")));

    }
    /* 3 ,4 and 5 are the posiible position of the second separator*/
    else if (3 != separatorPosition[1] && 4 != separatorPosition[1] && 5 != separatorPosition[1]) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the minute is invalid!")));
    } else {
        /* get the year,mon,day separately in char*/
        errno_t errorno = EOK;
        errorno = strncpy_s(hour, separatorPosition[0] + 1, timeString, separatorPosition[0]);
        securec_check(errorno, "\0", "\0");
        errorno = strncpy_s(minute,
            separatorPosition[1] - separatorPosition[0],
            timeString + separatorPosition[0] + 1,
            separatorPosition[1] - separatorPosition[0] - 1);
        securec_check(errorno, "\0", "\0");
        errorno = strncpy_s(second,
            strLength - separatorPosition[1],
            timeString + separatorPosition[1] + 1,
            strLength - separatorPosition[1] - 1);
        securec_check(errorno, "\0", "\0");

        /* transfer char to int*/
        tm_time->tm_hour = atoi(hour);
        tm_time->tm_min = atoi(minute);
        tm_time->tm_sec = atoi(second);
    }
}

/*
 * Check the date's validity
 */
void CheckDateValidity(struct pg_tm* tm)
{
    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    if (0 > tm->tm_year || MAXYEAR < tm->tm_year) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the year = %d is illegal", tm->tm_year)));
    } else if (WhetherFebLeapYear(tm)) {
        if (DAYNUM_FEB_LEAPYEAR < tm->tm_mday) {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("the day = %d for february in leap year is illegal", tm->tm_mday)));
        }
    } else {
        CheckMonthAndDay(tm);
    }

    CheckTimeValidity(tm);
}

void CheckMonthAndDay(struct pg_tm* tm)
{
    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    if (WhetherBigMon(tm)) {
        if (DAYNUM_BIGMON < tm->tm_mday || 1 > tm->tm_mday) {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the day = %d for big month is illegal", tm->tm_mday)));
        }
    } else if (WhetherSmallMon(tm)) {
        if (DAYNUM_LITTLEMON < tm->tm_mday || 1 > tm->tm_mday) {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("the day = %d for small month is illegal", tm->tm_mday)));
        }
    } else if (FEBRUARY == tm->tm_mon) {
        if (DAYNUM_FEB_NONLEAPYEAR < tm->tm_mday) {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("the day = %d for February in commen year is illegal", tm->tm_mday)));
        }
    } else {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the month = %d is illegal", tm->tm_mon)));
    }
}

void CheckTimeValidity(struct pg_tm* tm)
{
    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return;
    }
    if (MAXNUM_HOUR < tm->tm_hour || 0 > tm->tm_hour) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the hour = %d is illegal", tm->tm_hour)));
    }
    if (MAXNUM_MIN < tm->tm_min || 0 > tm->tm_min) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the minute = %d  is illegal", tm->tm_min)));
    }
    if (MAXNUM_SEC < tm->tm_sec || 0 > tm->tm_sec) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the second = %d  is illegal", tm->tm_sec)));
    }
}

static int WhetherFebLeapYear(struct pg_tm* tm)
{
    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return 0;
    }
    if ((((0 == tm->tm_year % 4) && (0 != tm->tm_year % 100)) || (0 == tm->tm_year % 400)) && FEBRUARY == tm->tm_mon) {
        return 1;
    } else {
        return 0;
    }
}

static int WhetherSmallMon(struct pg_tm* tm)
{
    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return 0;
    }
    if (APRIL == tm->tm_mon || JUNE == tm->tm_mon || SEPTEMBER == tm->tm_mon || NOVEMBER == tm->tm_mon) {
        return 1;
    } else {
        return 0;
    }
}

static int WhetherBigMon(struct pg_tm* tm)
{
    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return 0;
    }
    if (JANUARY == tm->tm_mon || MARCH == tm->tm_mon || MAY == tm->tm_mon || JULY == tm->tm_mon ||
        AUGEST == tm->tm_mon || OCTOBER == tm->tm_mon || DECEMBER == tm->tm_mon) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * transfor a string (like "1990-03-04 12:12:12") to the type of timestamp
 */
Datum to_date_default_format(PG_FUNCTION_ARGS)
{
    text* date_txt = PG_GETARG_TEXT_P(0);
    struct pg_tm* tm = NULL;
    const fsec_t fsec = 0;
    int tz = 0;
    TimestampTz result;
    char* date_str = NULL;

    date_str = text_to_cstring(date_txt);

    if (strlen(date_str) > (DATESTR_LEN + TIMESTR_LEN) + 1)  // At most 1 space is allowed.
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("the format is not correct")));
        return (Datum)0;
    }

    tm = GetDateDetail(date_str);

    if (NULL == tm) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("the format is not correct")));
        return (Datum)0;
    }

    if (tm2timestamp(tm, fsec, &tz, &result) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    PG_RETURN_TIMESTAMP(result);
}

Datum timestampzone_text(PG_FUNCTION_ARGS)
{
    TimestampTz dt = PG_GETARG_TIMESTAMPTZ(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(timestamptz_out, dt));

    result = DirectFunctionCall1(textin, CStringGetDatum(tmp));
    pfree_ext(tmp);
    PG_RETURN_DATUM(result);
}

Datum timestamp_text(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(timestamp_out, timestamp));

    result = DirectFunctionCall1(textin, CStringGetDatum(tmp));
    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/*
 * @Description: Convert text to timestamp type
 * @in textValue: text type data.
 * @return: convert result.
 */
Datum text_timestamp(PG_FUNCTION_ARGS)
{
    Datum textValue = PG_GETARG_DATUM(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(textout, textValue));

    result = DirectFunctionCall3(timestamp_in, CStringGetDatum(tmp), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));

    pfree_ext(tmp);

    PG_RETURN_TIMESTAMP(result);
}

/*
 * @Description: Add a int32 to a timestamp with timestamp data type
 * @in textValue: text type data.
 * @return: convert result.
 */
Datum add_months(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    int month_num = PG_GETARG_INT32(1);
    Timestamp result;
    Interval spanTime;

    /* Add month_num month.*/
    spanTime.time = 0;
    spanTime.day = 0;
    spanTime.month = month_num;

    if (END_MONTH_CALCULATE) {
        struct pg_tm tt, *tm = &tt;
        fsec_t fsec;
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        int temp = day_tab[isleap(tm->tm_year)][tm->tm_mon - 1];
        if (tm->tm_mday == day_tab[isleap(tm->tm_year)][tm->tm_mon - 1]) {
            tm->tm_mon += spanTime.month;
            if (tm->tm_mon > MONTHS_PER_YEAR) {
                tm->tm_year += (tm->tm_mon - 1) / MONTHS_PER_YEAR;
                tm->tm_mon = ((tm->tm_mon - 1) % MONTHS_PER_YEAR) + 1;
            } else if (tm->tm_mon < 1) {
                tm->tm_year += tm->tm_mon / MONTHS_PER_YEAR - 1;
                tm->tm_mon = tm->tm_mon % MONTHS_PER_YEAR + MONTHS_PER_YEAR;
            }
            tm->tm_mday = (day_tab[isleap(tm->tm_year)][tm->tm_mon - 1]);
            spanTime.day = tm->tm_mday - temp;
            spanTime.day = (spanTime.day >= 0 ? spanTime.day : 0);
        }
    }

    result = DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(timestamp), PointerGetDatum(&spanTime));

    PG_RETURN_TIMESTAMP(result);
}

#define SUNDAYNUM 1
#define MONDAYNUM 2
#define TUESDAYNUM 3
#define WEDNESDAYNUM 4
#define THURSDAYNUM 5
#define FRIDAYNUM 6
#define SATURDAYNUM 7

/*
 * @Description: Add a next_day_string function with timestamp data type
 * @in theDate: the target timestamp.
 * @in week_day: text from Sunday to Saturday.
 * @return: convert result.
 */
Datum next_day_str(PG_FUNCTION_ARGS)
{
    Datum theDate = PG_GETARG_DATUM(0);
    Datum week_day = PG_GETARG_DATUM(1);
    Datum result = PointerGetDatum(0);

    char* weekday = TextDatumGetCString(week_day);

    if (0 == pg_strcasecmp(weekday, "SUNDAY") || 0 == pg_strcasecmp(weekday, "SUN")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(SUNDAYNUM));
    } else if (0 == pg_strcasecmp(weekday, "MONDAY") || 0 == pg_strcasecmp(weekday, "MON")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(MONDAYNUM));
    } else if (0 == pg_strcasecmp(weekday, "TUESDAY") || 0 == pg_strcasecmp(weekday, "TUE")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(TUESDAYNUM));
    } else if (0 == pg_strcasecmp(weekday, "WEDNESDAY") || 0 == pg_strcasecmp(weekday, "WED")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(WEDNESDAYNUM));
    } else if (0 == pg_strcasecmp(weekday, "THURSDAY") || 0 == pg_strcasecmp(weekday, "THU")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(THURSDAYNUM));
    } else if (0 == pg_strcasecmp(weekday, "FRIDAY") || 0 == pg_strcasecmp(weekday, "FRI")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(FRIDAYNUM));
    } else if (0 == pg_strcasecmp(weekday, "SATURDAY") || 0 == pg_strcasecmp(weekday, "SAT")) {
        result = DirectFunctionCall2(next_day_int, theDate, Int32GetDatum(SATURDAYNUM));
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("Invalid day format: \'%s\' of the week", weekday)));
    }

    PG_RETURN_TIMESTAMP(result);
}

/*
 * @Description: Add a next_day_int function function with timestamp data type
 * @in theDate: the target timestamp.
 * @in target_week: the number of day for the week.
 * @return: convert result.
 */
Datum next_day_int(PG_FUNCTION_ARGS)
{
    Timestamp theDate = PG_GETARG_TIMESTAMP(0);
    int target_week = PG_GETARG_INT32(1);
    text* str_week_day = NULL;
    int source_week_day;
    int add_days;
    Timestamp result;
    Interval spanTime;
    char* string_week = NULL;

    if (target_week < SUNDAYNUM || target_week > SATURDAYNUM) {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                errmsg("Week day out of range, the range is %d ... %d", SUNDAYNUM, SATURDAYNUM)));
    }

    /* Get this timestamp's week day.*/
    str_week_day =
        DatumGetTextP(DirectFunctionCall2(timestamp_to_char, TimestampGetDatum(theDate), CStringGetTextDatum("D")));

    if (str_week_day == NULL) {
        PG_RETURN_NULL();
    }

    string_week = TextDatumGetCString(str_week_day);

    source_week_day = *string_week - '0';

    add_days =
        target_week > source_week_day ? (target_week - source_week_day) : (target_week - source_week_day + SATURDAYNUM);

    /* Add one day.*/
    spanTime.time = 0;
    spanTime.day = add_days; /*translate int type to Interval type*/
    spanTime.month = 0;

    result = DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(theDate), PointerGetDatum(&spanTime));

    PG_RETURN_TIMESTAMP(result);
}

/*
 * @Description: Return the last day of the month
 * @in theDate: the target timestamp.
 * @return: convert result.
 */
Datum last_day(PG_FUNCTION_ARGS)
{
    Timestamp theDate = PG_GETARG_TIMESTAMP(0);
    Timestamp result;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec = 0;

    if (0 != timestamp2tm(theDate, NULL, tm, &fsec, NULL, NULL)) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range.")));
    }

    /* Set day to this month's last day.*/
    tm->tm_mday = day_tab[isleap(tm->tm_year)][tm->tm_mon - 1];

    if (0 != tm2timestamp(tm, fsec, NULL, &result)) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range.")));
    }

    PG_RETURN_TIMESTAMP(result);
}

void timestamp_FilpSign(pg_tm* tm)
{
    tm->tm_sec = -tm->tm_sec;
    tm->tm_min = -tm->tm_min;
    tm->tm_hour = -tm->tm_hour;
    tm->tm_mday = -tm->tm_mday;
    tm->tm_mon = -tm->tm_mon;
    tm->tm_year = -tm->tm_year;
}

void timestamp_CalculateFields(TimestampTz* dt1, TimestampTz* dt2, fsec_t* fsec, pg_tm* tm, pg_tm* tm1, pg_tm* tm2)
{
    /* flip sign if necessary... */
    if (*dt1 < *dt2) {
        *fsec = -(*fsec);
        timestamp_FilpSign(tm);
    }

    /* propagate any negative fields into the next higher field */
    while (*fsec < 0) {
#ifdef HAVE_INT64_TIMESTAMP
        *fsec += USECS_PER_SEC;
#else
        *fsec += 1.0;
#endif
        tm->tm_sec--;
    }

    while (tm->tm_sec < 0) {
        tm->tm_sec += SECS_PER_MINUTE;
        tm->tm_min--;
    }

    while (tm->tm_min < 0) {
        tm->tm_min += MINS_PER_HOUR;
        tm->tm_hour--;
    }

    while (tm->tm_hour < 0) {
        tm->tm_hour += HOURS_PER_DAY;
        tm->tm_mday--;
    }

    while (tm->tm_mday < 0) {
        if (*dt1 < *dt2) {
            tm->tm_mday += day_tab[isleap(tm1->tm_year)][tm1->tm_mon - 1];
            tm->tm_mon--;
        } else {
            tm->tm_mday += day_tab[isleap(tm2->tm_year)][tm2->tm_mon - 1];
            tm->tm_mon--;
        }
    }

    while (tm->tm_mon < 0) {
        tm->tm_mon += MONTHS_PER_YEAR;
        tm->tm_year--;
    }

    /*
     * Note: we deliberately ignore any difference between tz1 and tz2.
     *
     * recover sign if necessary... */
    if (*dt1 < *dt2) {
        *fsec = -(*fsec);
        timestamp_FilpSign(tm);
    }
}

void WalReplicationTimestampToString(WalReplicationTimestampInfo *timeStampInfo, TimestampTz nowtime,
    TimestampTz timeout, TimestampTz lastTimestamp, TimestampTz heartbeat)
{
    // timestamptz_to_str returns char[MAXTIMESTAMPLEN + 1]
    errno_t rc = memcpy_s(timeStampInfo->nowTimeStamp, MAXTIMESTAMPLEN + 1, timestamptz_to_str(nowtime),
                          MAXTIMESTAMPLEN + 1);
    securec_check(rc, "\0", "\0");
    rc = memcpy_s(timeStampInfo->timeoutStamp, MAXTIMESTAMPLEN + 1, timestamptz_to_str(timeout), MAXTIMESTAMPLEN + 1);
    securec_check(rc, "\0", "\0");
    rc = memcpy_s(timeStampInfo->lastRecStamp, MAXTIMESTAMPLEN + 1, timestamptz_to_str(lastTimestamp),
                  MAXTIMESTAMPLEN + 1);
    securec_check(rc, "\0", "\0");
    rc = memcpy_s(timeStampInfo->heartbeatStamp, MAXTIMESTAMPLEN + 1, timestamptz_to_str(heartbeat),
                  MAXTIMESTAMPLEN + 1);
    securec_check(rc, "\0", "\0");
}

/*
 * to_timestamp(double precision)
 * Convert UNIX epoch to timestamptz.
 */
Datum float8_timestamptz(PG_FUNCTION_ARGS)
{
    float8 seconds = PG_GETARG_FLOAT8(0);
    TimestampTz result;

    /* Deal with NaN and infinite inputs ... */
    if (isnan(seconds)) {
        ereport(ERROR, (errmsg("timestamp cannot be NaN")));
    }

    if (isinf(seconds)) {
        if (seconds < 0) {
            TIMESTAMP_NOBEGIN(result);
        } else {
            TIMESTAMP_NOEND(result);
        }
    } else {
        /* Out of range? */
        if (seconds < (float8) SECS_PER_DAY * (DATETIME_MIN_JULIAN - UNIX_EPOCH_JDATE) ||
            seconds >= (float8) SECS_PER_DAY * (TIMESTAMP_END_JULIAN - UNIX_EPOCH_JDATE)) {
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("timestamp out of range: \"%g\"", seconds)));
        }

        /* Convert UNIX epoch to Postgres epoch */
        seconds -= ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY);

        seconds = rint(seconds * USECS_PER_SEC);
        result = (int64) seconds;

        /* Recheck in case roundoff produces something just out of range */
        if (!IS_VALID_TIMESTAMP(result)) {
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("timestamp out of range: \"%g\"", PG_GETARG_FLOAT8(0))));
        }
    }

    PG_RETURN_TIMESTAMP(result);
}

#ifdef DOLPHIN
/*
 * Check whether the datetime value is within the specified range: 
 * [0000-01-01 00:00:00.000000, 9999-12-31 23:59:59.999999].(the 
 * datetime range is from MySQL)
 */
void check_b_format_datetime_range_with_ereport(Timestamp &datetime)
{
    if (datetime < B_FORMAT_TIMESTAMP_MIN_VALUE || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                        errmsg("date/time field value out of range")));
    }
}

/*
 * @Description: Subtract days from a datetime, giving a new datetime and assign it to result.
 * @return: false if parameter datetime or result out of range, otherwise true
 */
bool datetime_sub_days(Timestamp datetime, int days, Timestamp *result, bool is_add_func)
{
    Interval span;
    if (datetime < B_FORMAT_TIMESTAMP_MIN_VALUE || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE)
        return false;
    span.time = 0;
    span.month = 0;
    span.day = days;
    *result = timestamp_mi_interval(datetime, &span);
    if (*result < B_FORMAT_TIMESTAMP_FIRST_YEAR || *result > B_FORMAT_TIMESTAMP_MAX_VALUE) {
        if (*result < B_FORMAT_TIMESTAMP_FIRST_YEAR && is_add_func) {
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            timestamp2tm(*result, NULL, tm, &fsec, NULL, NULL);
            tm->tm_year = 0;
            tm->tm_mon = 0;
            tm->tm_mday = 0;
            tm2timestamp(tm, fsec, NULL, result);
            return true;
        }
        return false;
    }
    return true;
}

/*
 * @Description: Subtract an interval from a datetime, giving a new datetime and assign it to result.
 * @return: false if parameter datetime or result out of range, otherwise true
 */
bool datetime_sub_interval(Timestamp datetime, Interval *span, Timestamp *result, bool is_add_func)
{
    if (datetime < B_FORMAT_TIMESTAMP_MIN_VALUE || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE)
        return false;
    *result = timestamp_mi_interval(datetime, span);

    if (*result < B_FORMAT_TIMESTAMP_MIN_VALUE || *result > B_FORMAT_TIMESTAMP_MAX_VALUE)
        return false;

    if (span->time == 0 && span->day == 0)
        return true;

    if (*result < B_FORMAT_TIMESTAMP_FIRST_YEAR) {
        if (is_add_func) {
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            timestamp2tm(*result, NULL, tm, &fsec, NULL, NULL);
            tm->tm_year = 0;
            tm->tm_mon = 0;
            tm->tm_mday = 0;
            tm2timestamp(tm, fsec, NULL, result);
            return true;
        }
        return false;
    }
    return true;
}

/*
 * @Description: The effect is the same as the timestamp_in(). The difference 
 * is that this function does not report any errors, but instead return 
 * false. 
 * @return false if the input string cannot be converted to datetime, otherwise true.
*/
bool datetime_in_no_ereport(const char *str, Timestamp *datetime)
{
    bool ret = true;
    MemoryContext current_ctx = CurrentMemoryContext;
    PG_TRY();
    {
        *datetime = DatumGetTimestamp(
            DirectFunctionCall3(timestamp_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
    }
    PG_CATCH();
    {
        // If catch an error, just empty the error stack and set return value to false.
        (void)MemoryContextSwitchTo(current_ctx);
        FlushErrorState();
        ret = false;
    }
    PG_END_TRY();
    return ret;
}

/**
 * Convert non-NULL values of the indicated types to the Timestamp value
 */
void convert_to_datetime(Datum value, Oid valuetypid, Timestamp *datetime) 
{
    switch (valuetypid) {
        case UNKNOWNOID:
        case CSTRINGOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall3(timestamp_in, value, ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
            break;
        }
        case CLOBOID:
        case NVARCHAR2OID:
        case BPCHAROID:
        case VARCHAROID:
        case TEXTOID: {
            char *str = TextDatumGetCString(value);
            *datetime = DatumGetTimestamp(
                DirectFunctionCall3(timestamp_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
            break;
        }
        case TIMESTAMPOID: {
            *datetime = DatumGetTimestamp(value);
            break;
        }
        case DATEOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(date_timestamp, value));
            break;
        }
        case TIMEOID: {
            // The conversion rule from Time to Timestamp is:
            // Combining the current date withe the input time
            pg_tm tt, *tm = &tt;
            Timestamp temp;
            TimeADT time = DatumGetTimeADT(value);
            GetCurrentDateTime(tm);
            tm->tm_hour = tm->tm_min = tm->tm_sec = 0;  // Keep date only
            tm2timestamp(tm, 0, NULL, &temp);
            *datetime = temp + time;
            break;
        }
        case INT1OID:
        case INT2OID:
        case INT4OID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(int32_b_format_datetime, value));
            break;
        }
        case INT8OID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(int64_b_format_datetime, value));
            break;
        }
        case NUMERICOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(numeric_b_format_datetime, value));
            break;
        }
        case FLOAT4OID:
            *datetime = DatumGetTimeADT(DirectFunctionCall1(numeric_b_format_datetime, 
                                    DirectFunctionCall1(float4_numeric, value)));
            break;
        case FLOAT8OID: {
            *datetime = DatumGetTimeADT(DirectFunctionCall1(numeric_b_format_datetime, 
                                    DirectFunctionCall1(float8_numeric, value)));
            break;
        }
        case BOOLOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(int32_b_format_datetime, 
                                        DirectFunctionCall1(bool_int4, value)));
            break;
        }
        case TIMESTAMPTZOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(timestamptz_timestamp, value));
            break;
        }
        case ABSTIMEOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(abstime_timestamp, value));
            break;
        }
        default: {
            if (valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint1") || 
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint2") || 
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint4") ||
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint8")) {
                    uint64 uint_val = DatumGetUInt64(value);
                    if (uint_val > (uint64)LONG_LONG_MAX) {
                        ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                                        errmsg("timestamp out of range:: \"%lu\"", uint_val)));
                    } else {
                        *datetime = DatumGetTimeADT(DirectFunctionCall1(int64_b_format_datetime, value));
                    }
            } else {
                ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH), errmsg("unsupported input data type")));
            }
        }      
    }
}

/**
 * @Description:Convert cstring values to the Timestamp or TimeADT value according to its format.
 * Timestamp type cannot exceed this range: [0000-01-01 00:00:00, 9999-12-31 23:59:59.999999]
 * TimeADT type cannot exceed this range: [-838:59:59, 838:59:59]
 * Error will be reported if the above range is exceeded.
 * @return: Actual time type oid. 
 */
Oid convert_cstring_to_datetime_time(const char* str, Timestamp *datetime, TimeADT *time)
{
    size_t len = strlen(str);
    const char *start;
    const char *end = str + len;
    bool null_func_result = false;

    /* Skip space at start */
    for (; str != end && isspace((unsigned char)*str); str++)
        len--;
    start = str;
    /* Determine positive or negative time */
    if (str != end && *str == '-') {
        str++;
        len--;
    }
    /* Check whether the string is a full timestamp */
    if (len >= 12) {
        int fsec = 0, nano = 0, tm_type = DTK_NONE;
        struct pg_tm tt, *tm = &tt;
        bool warnings = false;
        /* Check whether the string is a full timestamp */
        cstring_to_datetime(str, (TIME_FUZZY_DATE | TIME_DATETIME_ONLY), tm_type, tm, fsec, nano, warnings, &null_func_result);
        if (warnings && SQL_MODE_STRICT()) {
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("date/time field value out of range: \"%s\"", str)));
        }
        switch (tm_type) {
            case DTK_NONE:
                break; /* Not a timestamp */
            case DTK_ERROR:
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                                errmsg("date/time field value out of range: \"%s\"", str)));
                break;
            case DTK_DATE:
                datetime_add_nanoseconds_with_round(tm, fsec, nano);
                tm2timestamp(tm, fsec, NULL, datetime);
                AdjustTimestampForTypmod(datetime, -1);
                return DATEOID;
            case DTK_DATE_TIME:
                datetime_add_nanoseconds_with_round(tm, fsec, nano);
                tm2timestamp(tm, fsec, NULL, datetime);
                AdjustTimestampForTypmod(datetime, -1);
                return TIMESTAMPOID;
            default:
                ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                                errmsg("invalid input syntax for type time: \"%s\"", str)));
                return InvalidOid;
        }
    }

    /* Not a timestamp. Try to convert str to time*/
    *time = DatumGetTimeADT(
        DirectFunctionCall3(time_in, CStringGetDatum(start), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
    check_b_format_time_range_with_ereport(*time);
    return TIMEOID;
}

/**
 * Convert non-NULL values of the indicated types to the Timestamp or TimeADT value
 * Timestamp type cannot exceed this range: [0000-01-01 00:00:00, 9999-12-31 23:59:59.999999]
 * TimeADT type cannot exceed this range: [-838:59:59, 838:59:59]
 * Error will be reported if the above range is exceeded.
 * @return: Actual time type oid. 
 */
Oid convert_to_datetime_time(Datum value, Oid valuetypid, Timestamp *datetime, TimeADT *time)
{
    switch (valuetypid) {
        case UNKNOWNOID:
        case CSTRINGOID: {
            return convert_cstring_to_datetime_time(DatumGetCString(value), datetime, time);
        }
        case CLOBOID:
        case NVARCHAR2OID:
        case BPCHAROID:
        case VARCHAROID:
        case TEXTOID: {
            char *str = TextDatumGetCString(value);
            return convert_cstring_to_datetime_time(str, datetime, time);
        }
        case TIMESTAMPOID:
        case TIMESTAMPTZOID:
        case ABSTIMEOID: {
            convert_to_datetime(value, valuetypid, datetime);
            check_b_format_datetime_range_with_ereport(*datetime);
            return TIMESTAMPOID;
        }
        case DATEOID: {
            *datetime = DatumGetTimestamp(DirectFunctionCall1(date_timestamp, value));
            check_b_format_datetime_range_with_ereport(*datetime);
            return DATEOID;
        }
        case TIMEOID:
        case TIMETZOID:
        case INT1OID:
        case INT2OID:
        case INT4OID:
        case BOOLOID: 
        case BITOID: {
            convert_to_time(value, valuetypid, time);
            check_b_format_time_range_with_ereport(*time);
            return TIMEOID;
        }
        case INT8OID: {
            int64 number = DatumGetInt64(value);
            if (number >= (int64)pow_of_10[10]) { /* datetime: 0001-00-00 00-00-00 */
                *datetime = DatumGetTimestamp(DirectFunctionCall1(int64_b_format_datetime, value));
                check_b_format_datetime_range_with_ereport(*datetime);
                return TIMESTAMPOID;
            } else {
                convert_to_time(value, INT4OID, time);
                check_b_format_time_range_with_ereport(*time);
                return TIMEOID;
            }
        }
        case NUMERICOID: {
            /* bound datetime: 0001-00-00 00-00-00 */
            Datum bound = DirectFunctionCall1(int8_numeric, Int64GetDatum((int64)pow_of_10[10]));
            if (DirectFunctionCall2(numeric_ge, value, bound)) {
                *datetime = DatumGetTimestamp(DirectFunctionCall1(numeric_b_format_datetime, value));
                check_b_format_datetime_range_with_ereport(*datetime);
                return TIMESTAMPOID;
            } else {
                convert_to_time(value, valuetypid, time);
                check_b_format_time_range_with_ereport(*time);
                return TIMEOID;
            }
        }
        case FLOAT4OID: {
            double float_val = (double)DatumGetFloat4(value);
            if (float_val >= (double)pow_of_10[10]) {
                convert_to_datetime(value, valuetypid, datetime);
                check_b_format_datetime_range_with_ereport(*datetime);
                return TIMESTAMPOID;
            } else {
                convert_to_time(value, valuetypid, time);
                check_b_format_time_range_with_ereport(*time);
                return TIMEOID;
            }
        }
        case FLOAT8OID: {
            double float_val = (double)DatumGetFloat8(value);
            if (float_val >= (double)pow_of_10[10]) {
                convert_to_datetime(value, valuetypid, datetime);
                check_b_format_datetime_range_with_ereport(*datetime);
                return TIMESTAMPOID;
            } else {
                convert_to_time(value, valuetypid, time);
                check_b_format_time_range_with_ereport(*time);
                return TIMEOID;
            }
        }
        default: {
            if (valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint1") || 
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint2") || 
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint4") ||
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "year")) {
                convert_to_time(value, valuetypid, time);
                check_b_format_time_range_with_ereport(*time);
                return TIMEOID;
            } else if (valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint8")) {
                uint64 number = DatumGetUInt64(value);
                if (number >= (uint64)pow_of_10[10]) {
                    convert_to_datetime(value, valuetypid, datetime);
                    check_b_format_datetime_range_with_ereport(*datetime);
                    return TIMESTAMPOID;
                } else {
                    convert_to_time(value, valuetypid, time);
                    check_b_format_time_range_with_ereport(*time);
                    return TIMEOID;
                }
            }
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("unsupported input data type: %s", format_type_be(valuetypid))));
            return InvalidOid;
        }
    }
}

/* 
 * @Description: Subtract time from a datetime, giving a new datetime and assign it to result.
 * @return: false if parameter date or result out of range, otherwise true
 */
bool datetime_sub_time(Timestamp datetime, TimeADT time_sub, Timestamp* result)
{
    if (datetime < B_FORMAT_TIMESTAMP_MIN_VALUE || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE)
        return false;

    Interval span;
    span.month = 0;
    span.day = 0;
    span.time = -time_sub;

    *result = DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(datetime), PointerGetDatum(&span));

    if (*result < B_FORMAT_TIMESTAMP_MIN_VALUE || *result > B_FORMAT_TIMESTAMP_MAX_VALUE)
        return false;
    return true;
}

/* subtime(expr1, expr2)
 * @Description: Subtract time from a datetime/time converted from text, giving a new datetime/time.
 * The return type is the same as the type of the first parameter (datetime/time). (The actual return 
 * type is CString, and the return type is distinguished by a string in datetime or time format)
 */
Datum subtime(PG_FUNCTION_ARGS)
{
    TimeADT time1, time2, res_time;
    Timestamp datetime1, datetime2, res_datetime;
    Oid val_type1, val_type2;

    val_type1 = get_fn_expr_argtype(fcinfo->flinfo, 0);
    val_type2 = get_fn_expr_argtype(fcinfo->flinfo, 1);
    val_type1 = convert_to_datetime_time(PG_GETARG_DATUM(0), val_type1, &datetime1, &time1);

    switch (val_type2) {
        case TIMESTAMPOID:
        case TIMESTAMPTZOID:
        case ABSTIMEOID:
        case DATEOID:
            convert_to_time(PG_GETARG_DATUM(1), val_type2, &time2);
            check_b_format_time_range_with_ereport(time2);
            val_type2 = TIMEOID;
            break;
        default:
            val_type2 = convert_to_datetime_time(PG_GETARG_DATUM(1), val_type2, &datetime2, &time2);
            if (val_type2 == TIMESTAMPOID) {
                PG_RETURN_NULL();
            }
    }

    switch (val_type1) {
        case DATEOID:
            /* The calculation method is the same as TIMEOID, so break is not required */
            time1 = 0; /* time set to 00:00:00 */
        case TIMEOID: {
            res_time = time1 - time2;
            check_b_format_time_range_with_ereport(res_time);
            return DirectFunctionCall1(time_text, TimeGetDatum(res_time));
        }
        case TIMESTAMPOID: {
            if (datetime_sub_time(datetime1, time2, &res_datetime)) {
                /* The variable datetime or result does not exceed the specified range */
                if (res_datetime >= B_FORMAT_TIMESTAMP_FIRST_YEAR) {
                    return DirectFunctionCall1(datetime_text, TimestampGetDatum(res_datetime));
                }
                PG_RETURN_NULL();
            }
            ereport(ERROR, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                            errmsg("date/time field overflow")));
            break;
        }
        default: {
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("unsupported input data type: %s", format_type_be(val_type1))));
        }
    }

    PG_RETURN_NULL();
}

Datum timediff(PG_FUNCTION_ARGS)
{
    TimeADT time1, time2;
    Timestamp datetime1, datetime2;
    Oid val_type1, val_type2;
    TimeADT result;

    val_type1 = get_fn_expr_argtype(fcinfo->flinfo, 0);
    val_type2 = get_fn_expr_argtype(fcinfo->flinfo, 1);
    val_type1 = convert_to_datetime_time(PG_GETARG_DATUM(0), val_type1, &datetime1, &time1);
    val_type2 = convert_to_datetime_time(PG_GETARG_DATUM(1), val_type2, &datetime2, &time2);
    if (val_type1 != val_type2) {
        // If the datetime/time types of the two parameters are different
        PG_RETURN_NULL();
    }
    switch (val_type1) {
        case TIMEOID: {
            result = time1 - time2;
            break;
        }
        case TIMESTAMPOID:
        case DATEOID: {
            long secs = 0;
            int usecs = 0;
            int sign = 1;
            if (datetime1 < datetime2) {
                TimestampDifference(datetime1, datetime2, &secs, &usecs);
                sign = -1;
            } else {
                TimestampDifference(datetime2, datetime1, &secs, &usecs);
            }
            result = (secs * USECS_PER_SEC + usecs) * sign;
            break;
        }
        default: {
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("unsupported input data type: %s", format_type_be(val_type1))));
        }
    }
    check_b_format_time_range_with_ereport(result);
    PG_RETURN_TIMEADT(result);
}

/* datetime_to_text()
 * Convert datetime to text data type.
 */
Datum datetime_text(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    char *tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(timestamp_out, timestamp));

    result = DirectFunctionCall1(textin, CStringGetDatum(tmp));
    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/* time_to_text()
 * Convert time to text data type.
 */
Datum time_text(PG_FUNCTION_ARGS)
{
    TimeADT timein = PG_GETARG_TIMEADT(0);
    char *tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(time_out, timein));

    result = DirectFunctionCall1(textin, CStringGetDatum(tmp));
    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/*
 * @Description: Create a formated time value in a string.
 * @return: The formated time value as a string.
 */
Datum time_format(PG_FUNCTION_ARGS)
{
    text *time_text = PG_GETARG_TEXT_PP(0);
    text *format_text = PG_GETARG_TEXT_PP(1);
    char buf[MAXDATELEN];          /* string for temporary storage */
    char format[MAXDATELEN];       /* format string */
    char str[MAXDATELEN];          /* return string */
    char *ptr = NULL, *end = NULL; /* head and tail of format */
    char temp_ptr[2];
    int remain = MAXDATELEN;       /* remaining buffer size of variable str */
    int insert_len = 0;            /* number of characters inserted into str */
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int32 hours_i;      /* hour in range of 0..12 */
    int timeSign = 1;
    bool isNeg = false; /* positive or negative time */
    errno_t rc = EOK;

    /* convert time_text and format_text into string from text */
    int len = VARSIZE_ANY_EXHDR(time_text);
    text_to_cstring_buffer(time_text, buf, sizeof(char) * (len + 1));
    len = VARSIZE_ANY_EXHDR(format_text);
    text_to_cstring_buffer(format_text, format, sizeof(char) * (len + 1));

    /* is time_text in datetime or time format? */
    str_to_pg_tm(buf, tt, fsec, timeSign);

    /* check whether it is a negative time */
    isNeg = (timeSign == -1);

    /* carry for fsec */
    if (fsec / USECS_PER_SEC) {
        tt.tm_sec += fsec / USECS_PER_SEC;
        tt.tm_min += tt.tm_sec / SECS_PER_MINUTE;
        tt.tm_sec %= SECS_PER_MINUTE;
        tt.tm_hour += tt.tm_min / MINS_PER_HOUR;
        tt.tm_min %= MINS_PER_HOUR;
        fsec = 0;
    }

    /* is time out of range? */
    if (tm->tm_hour == TIME_MAX_HOUR && tm->tm_min == TIME_MAX_MINUTE &&
        tm->tm_sec == TIME_MAX_SECOND && fsec > 0) {
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date/time field value out of range: \"%s\"", str)));
    }

    if (isNeg) {
        str[0] = '-';
        str[1] = '\0';
        remain--;
    } else {
        str[0] = '\0';
    }

    ptr = format;
    end = ptr + strlen(format);
    for (; ptr != end; ptr++) {
        if (*ptr != '%' || ptr + 1 == end) {
            temp_ptr[0] = *ptr;
            temp_ptr[1] = '\0';
            insert_len = 1;
            rc = strcat_s(str, remain, temp_ptr);
        } else {
            switch (*++ptr) {
                case 'a':
                case 'b':
                case 'D':
                case 'j':
                case 'M':
                case 'U':
                case 'u':
                case 'V':
                case 'v':
                case 'W':
                case 'w':
                case 'X':
                case 'x': /* return null */
                    PG_RETURN_NULL();
                case 'c':
                case 'e':
                    insert_len = 1;
                    rc = strcat_s(str, remain, "0");
                    break;
                case 'd':
                case 'm':
                case 'y':
                    insert_len = YEAR2_LEN;
                    rc = strcat_s(str, remain, "00");
                    break;
                case 'Y':
                    insert_len = YEAR4_LEN;
                    rc = strcat_s(str, remain, "0000");
                    break;
                case 'f':
                    insert_len = sprintf_s(buf, MAXDATELEN, "%06d", fsec);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'H': /* hours in range of 0..24 */
                    insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_hour);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'k': /* hours in range of 0..24 */
                    insert_len = sprintf_s(buf, MAXDATELEN, "%d", tm->tm_hour);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'h':
                case 'I': /* hours in range of 0..12 */
                    hours_i = (tm->tm_hour % MAX_VALUE_24_CLOCK + MAX_VALUE_12_CLOCK - 1) % MAX_VALUE_12_CLOCK + MIN_VALUE_12_CLOCK;
                    insert_len = sprintf_s(buf, MAXDATELEN, "%02d", hours_i);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'i': /* minutes */
                    insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_min);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'p':
                    hours_i = tm->tm_hour % MAX_VALUE_24_CLOCK;
                    insert_len = AM_PM_LEN;
                    rc = strcat_s(str, remain,
                                  (hours_i < MAX_VALUE_12_CLOCK ? "AM" : "PM"));
                    break;
                case 'r':
                    insert_len =
                        sprintf_s(buf, MAXDATELEN,
                                  (((tm->tm_hour % MAX_VALUE_24_CLOCK) < MAX_VALUE_12_CLOCK) ? "%02d:%02d:%02d AM" : "%02d:%02d:%02d PM"),
                                  (tm->tm_hour + MAX_VALUE_12_CLOCK - 1) % MAX_VALUE_12_CLOCK + MIN_VALUE_12_CLOCK,
                                  tm->tm_min, tm->tm_sec);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'S':
                case 's': /* seconds */
                    insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_sec);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                case 'T':
                    insert_len = sprintf_s(
                        buf, MAXDATELEN, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
                    securec_check_ss(insert_len, "", "");
                    rc = strcat_s(str, remain, buf);
                    break;
                default:
                    temp_ptr[0] = *ptr;
                    temp_ptr[1] = '\0';
                    insert_len = 1;
                    rc = strcat_s(str, remain, temp_ptr);
                    break;
                }
        }
        remain -= insert_len;
        securec_check(rc, "", "");
    }
    PG_RETURN_TEXT_P(cstring_to_text(str));
}

/* 
 * The effect is the same as time_format()
 * This function is used to receive date parameter and the date 
 * is required to be in the range of [0000-01-01, 9999-12-31]
 */
Datum time_format_date(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    
    if (date < B_FORMAT_DATEADT_MIN_VALUE || date > B_FORMAT_DATEADT_MAX_VALUE) {
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date field value out of range")));
    }

    return DirectFunctionCall2(time_format, CStringGetTextDatum("0"), PG_GETARG_DATUM(1));
}

/*
 * @Description: substract an Interval from a TimeADT
 * @return: if the result time is in the range that can be represented, return the result.
 * Otherwise return the B_FORMAT_TIME_MAX_VALUE
 */
TimeADT time_sub_interval(TimeADT time, Interval *span) 
{
    if (span->month || span->day < -B_FORMAT_TIME_MAX_VALUE_TO_DAY ||
        span->day > B_FORMAT_TIME_MAX_VALUE_TO_DAY) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), 
                        errmsg("time field value out of range")));
        return B_FORMAT_TIME_MAX_VALUE;
    } else {
        TimeADT result = time - (span->day * SECS_PER_DAY * USECS_PER_SEC + span->time);
        if (result < -B_FORMAT_TIME_MAX_VALUE || result > B_FORMAT_TIME_MAX_VALUE) {
            ereport(ERROR, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), 
                            errmsg("time field value out of range")));
            return B_FORMAT_TIME_MAX_VALUE;
        }
        return result;
    }
}

/*
 * @Description: Calculate the value of interval.
 * @return true if success, false if fail.
 */
bool calc_interval(Interval *span, lldiv_t arg, int dtk)
{
    int64 integer_part = arg.quot;
    int64 frac_part = arg.rem;
    int64 carry = ((abs(frac_part) / FRAC_PART_LEN_IN_NUMERICSEC >= ROUNDING_BORDER) ? 1 : 0);
    int64 signal = ((integer_part >= 0 && frac_part >= 0) ? 1 : -1);

    switch (dtk) {
        case DTK_YEAR:
            span->month = (integer_part + carry * signal) * MONTHS_PER_YEAR;
            break;
        case DTK_MONTH:
            span->month = (integer_part + carry * signal);
            break;
        case DTK_QUARTER:
            span->month = (integer_part + carry * signal) * MONTHS_PER_QUARTER;
            break;
        case DTK_DAY:
            span->day = (integer_part + carry * signal);
            break;
        case DTK_WEEK:
            span->day = (integer_part + carry * signal) * DAYS_PER_WEEK;
            break;
        case DTK_HOUR:
            span->time = time2t((integer_part + carry * signal), 0, 0, 0);
            break;
        case DTK_MINUTE:
            span->time = time2t(0, (integer_part + carry * signal), 0, 0);
            break;
        case DTK_SECOND:
            frac_part /= NANOSEC_PER_USEC;
            span->time = time2t(0, 0, integer_part, frac_part);
            break;
        case DTK_MICROSEC:
            span->time = time2t(0, 0, 0, (integer_part + carry * signal));
            break;
        default:
            return false;
    }
    return true;
}

/**
 * @Description: Determine whether the format of the result should turn to datetime from date.
 * @result: true if need conversion, false otherwise.
 */
bool determine_conversion(int dtk) 
{
    switch (dtk) {
        case DTK_HOUR:
        case DTK_MINUTE:
        case DTK_SECOND:
        case DTK_MICROSEC:
            return true;
        default:
            return false;
    }
}

/*
 * @Description: Internal operation function of timestamp_add_numeric() 
 * and timestamp_add_text() function.
 * @return: a date ,datetime or time value (text type).
 */
Datum timestamp_add_internal(char *lowunits, int unit, int unit_type, Numeric num, Datum expr, Oid expr_type) 
{
    Timestamp datetime, res_datetime;
    DateADT date, res_date;
    TimeADT time, res_time;

    NumericVar tmp;
    lldiv_t delta;
    init_var_from_num(num, &tmp);
    if (!numeric_to_lldiv_t(&tmp, &delta)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("interval out of range")));
    }

    Interval sp, *span = &sp;
    span->time = span->day = span->month = 0;

    if (unit_type == UNITS) {
        if (!calc_interval(span, delta, unit)) {
            ereport(ERROR, (errcode(ERRCODE_ERROR_IN_ASSIGNMENT), errmsg("failed to calculate interval")));
        }
    } else {
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_PARAMETER), errmsg("unknown unit: \"%s\"", lowunits)));
    }

    span->time = -span->time;
    span->day = -span->day;
    span->month = -span->month;

    switch (expr_type) {
        // when the input type is time or timetz
        case TIMETZOID:
        case TIMEOID:
            convert_to_time(expr, expr_type, &time);
            res_time = time_sub_interval(time, span);
            PG_RETURN_TEXT_P(DirectFunctionCall1(time_text, TimeADTGetDatum(res_time)));
    }

    bool conversion = determine_conversion(unit);
    if (conversion) {
        // With specific units, the return type is datetime
        convert_to_datetime(expr, expr_type, &datetime);
        check_b_format_datetime_range_with_ereport(datetime);
        expr_type = TIMESTAMPOID;
    } else {
        expr_type = convert_to_datetime_date(expr, expr_type, &datetime, &date);
    }

    switch (expr_type) {
        case TIMESTAMPOID:
            if (datetime_sub_interval(datetime, span, &res_datetime)) {
                PG_RETURN_TEXT_P(DirectFunctionCall1(datetime_text, res_datetime));
            }
            break;
        case DATEOID:
            if (date_sub_interval(date, span, &res_date)) {
                PG_RETURN_TEXT_P(DirectFunctionCall1(date_text, res_date));
            }
            break;
        default:
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("unsupported input data type: %s", format_type_be(expr_type))));
    }
    ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date/time field overflow")));

    return (Datum)0;
}

/*
 * @Description: Compatible with timestampadd(units, interval, expr) function in mysql.
 * Add a period of time 'interval' in units 'units' to a date/datetime expression expr.
 * This function receives 'interval' parameter of numeric type.
 * @return: a date, datetime or time value (text type).
 */
Datum timestamp_add_numeric(PG_FUNCTION_ARGS) {
    text* units = PG_GETARG_TEXT_PP(0);
    char* lowunits = NULL;
    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);
    Oid expr_type = get_fn_expr_argtype(fcinfo->flinfo, 2);

    /* parse unit */
    int unit = 0;
    int unit_type = DecodeUnits(0, lowunits, &unit);

    if (unit_type == UNKNOWN_FIELD) {
        unit_type = DecodeSpecial(0, lowunits, &unit);
    }

    return timestamp_add_internal(lowunits, unit, unit_type, PG_GETARG_NUMERIC(1), PG_GETARG_DATUM(2), expr_type);
}

/*
 * @Description: Compatible with timestampadd(units, interval, expr) function in mysql.
 * Add a period of time 'interval' in units 'units' to a date/datetime expression expr.
 * This function receives 'interval' parameter of string type.
 * @return: a date or a datetime value (text type).
 */
Datum timestamp_add_text(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    text *num_txt = PG_GETARG_TEXT_PP(1);
    char* lowunits = NULL;
    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);
    Oid expr_type = get_fn_expr_argtype(fcinfo->flinfo, 2);

    Interval sp, *span = &sp;
    span->time = span->day = span->month = 0;

    /* parse unit */
    int unit = 0;
    int unit_type = DecodeUnits(0, lowunits, &unit);

    if (unit_type == UNKNOWN_FIELD) {
        unit_type = DecodeSpecial(0, lowunits, &unit);
    }

    /* check the format of num_str */
    char *num_str = text_to_cstring(num_txt);
    Numeric num;
    int true_len, errorno;
    long long ret = str2ll_with_endptr(num_str, strlen(num_str), &true_len, &errorno);
    if (errorno == EDOM && SQL_MODE_STRICT()) {
        ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION), 
                        errmsg("Truncated incorrect INTEGER value: %s", num_str)));
    }

    /* str to Numeric*/
    if (unit == DTK_SECOND) {
        num = DatumGetNumeric(
            DirectFunctionCall3(numeric_in, CStringGetDatum(num_str), ObjectIdGetDatum(0), Int32GetDatum(-1)));
    } else {
        num = DatumGetNumeric(DirectFunctionCall1(int8_numeric, Int64GetDatum(ret)));
    }

    return timestamp_add_internal(lowunits, unit, unit_type, num, PG_GETARG_DATUM(2), expr_type);
}

/*
 * @Description: Compatible with timestamp(expr) function in mysql.
 * Convert date/datetime expression expr into a datetime value
 * @return: a datetime value.
 */
Datum timestamp_param1(PG_FUNCTION_ARGS)
{
    Timestamp datetime;
    Oid val_type;

    val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    convert_to_datetime(PG_GETARG_DATUM(0), val_type, &datetime);

    if (datetime >= B_FORMAT_TIMESTAMP_MIN_VALUE && datetime <= B_FORMAT_TIMESTAMP_MAX_VALUE)
        PG_RETURN_TIMESTAMP(datetime);

    ereport(ERROR,
            (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
             errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}

/* 
 * @Description: Compatible with timestamp(expr1, expr2) function in mysql. 
 * Adding the time expression expr2 to the date or datetime expression expr1 
 * and returns the result as a datetime value.
 * @return: datetime value.
 */
Datum timestamp_param2(PG_FUNCTION_ARGS)
{
    Timestamp datetime;
    TimeADT time;
    Oid val_type;

    val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    convert_to_datetime(PG_GETARG_DATUM(0), val_type, &datetime);
    if (datetime < B_FORMAT_TIMESTAMP_FIRST_YEAR || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                        errmsg("date/time field value out of range")));
    }

    val_type = get_fn_expr_argtype(fcinfo->flinfo, 1);
    convert_to_time(PG_GETARG_DATUM(1), val_type, &time);
    time_in_range(time);

    Timestamp result = datetime + time;
    if (result < B_FORMAT_TIMESTAMP_FIRST_YEAR || result > B_FORMAT_TIMESTAMP_MAX_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                        errmsg("date/time field overflow")));
    }

    PG_RETURN_TIMESTAMP(result);
}

/**
 * @Description: Calculate timestamps from '0000-00-00 00:00:00.000000' to the input arg,
 * while '0000-02-29' is invalid.
 * @return: Timestamps from '0000-00-00 00:00:00.000000' to the input arg.
 */
Timestamp calc_timestamp_from_zero(Timestamp timestamp)
{
    if (B_FORMAT_TIMESTAMP_APART_LEFT <= timestamp && timestamp < B_FORMAT_TIMESTAMP_APART_RIGHT) {
        return B_FORMAT_TIMESTAMP_MIN_VALUE;
    } else if (timestamp < B_FORMAT_TIMESTAMP_APART_LEFT) {
        return timestamp - B_FORMAT_TIMESTAMP_MIN_VALUE + USECS_PER_DAY;
    } else {
        return timestamp - B_FORMAT_TIMESTAMP_MIN_VALUE;
    }
}

/**
 * @Description: calculate the number of seconds from the year 0 to the given date.
 * Recieve the normal string variable as text.
 * @return: the number of seconds from the year 0 to the given date
 */
Datum to_seconds(PG_FUNCTION_ARGS)
{
    Timestamp timestamp;
    Oid val_type;

    val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    convert_to_datetime(PG_GETARG_DATUM(0), val_type, &timestamp);
    if (timestamp < B_FORMAT_TIMESTAMP_MIN_VALUE || timestamp > B_FORMAT_TIMESTAMP_MAX_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date/time value out of range")));
    }
    Timestamp result = calc_timestamp_from_zero(timestamp);
    if (result == B_FORMAT_TIMESTAMP_MIN_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), errmsg("date/time field out of range")));
    }
    PG_RETURN_NUMERIC(DirectFunctionCall1(int8_numeric, result /= USECS_PER_SEC));
}

/**
 * @Description: Determine how many digits to be saved behind the decimal point.
 * @return: The number of digits will be saved behind the decimal point.
 */
int determine_precision(int micro)
{
    if (!micro) {
        return 0;
    }

    int trailing_zero = 0;
    while (micro % 10 == 0) {
        micro /= 10;
        trailing_zero++;
    }
    return FRAC_PRECISION - trailing_zero;
}

/**
 * @Description: Calculate the number of seconds from '1970-01-01 00:00:00 UTC' to now.
 * @return: The number of seconds since '1970-01-01 00:00:00 UTC'.
 */
Datum unix_timestamp_no_args(PG_FUNCTION_ARGS)
{
    pg_tm tt, *tm = &tt;
    GetCurrentDateTime(tm);
    Timestamp epoch = SetEpochTimestamp();
    Timestamp now;
    tm2timestamp(tm, 0, NULL, &now);
    Timestamp result = now - epoch - tm->tm_gmtoff * USECS_PER_SEC;
    PG_RETURN_NUMERIC(DirectFunctionCall2(numeric_trunc,
                            DirectFunctionCall2(numeric_div,
                                DirectFunctionCall1(int8_numeric, result),
                                DirectFunctionCall1(int8_numeric, USECS_PER_SEC)
                            ),
                            determine_precision(result % USECS_PER_SEC)
                        )
                    );
}

/**
 * @Description: Calculate the number of seconds from '1970-01-01 00:00:00 UTC' to the given datetime,
 * which is represented by text, or can be converted to text.
 * @return: The number of seconds from '1970-01-01 00:00:00 UTC' to the given datetime.
 */
Datum unix_timestamp(PG_FUNCTION_ARGS)
{
    Timestamp timestamp;
    Oid val_type;

    val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    convert_to_datetime(PG_GETARG_DATUM(0), val_type, &timestamp);

    pg_tm tt, *tm = &tt;
    GetCurrentDateTime(tm);
    Timestamp epoch = SetEpochTimestamp();
    Timestamp ans = timestamp - epoch - tm->tm_gmtoff * USECS_PER_SEC;
    if (ans / USECS_PER_SEC > INT_MAX || ans < USECS_PER_SEC) {
        PG_RETURN_NUMERIC(DirectFunctionCall1(int8_numeric, 0));
    }
    PG_RETURN_NUMERIC(DirectFunctionCall2(numeric_trunc,
                            DirectFunctionCall2(numeric_div,
                                DirectFunctionCall1(int8_numeric, ans),
                                DirectFunctionCall1(int8_numeric, USECS_PER_SEC)
                            ),
                            determine_precision(ans % USECS_PER_SEC)
                        )
                    );
}

/**
 * utc_timestamp_func()
 * @return: UTC timestamp
 */
Datum utc_timestamp_func(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);
    TimestampTz now = GetCurrentStmtsysTimestamp();
    Timestamp result = (Timestamp)now;
    AdjustTimestampForTypmod(&result, typmod);
    PG_RETURN_TIMESTAMP(result);
}

bool MaybeRound(struct pg_tm *tm, fsec_t *fsec)
{
    Timestamp result;
    errno_t rc;
    if (
#ifdef HAVE_INT64_TIMESTAMP
        *fsec != USECS_PER_SEC
#else
        *fsec != 1
#endif
        ) return true;

    if (tm->tm_mon == 0 || tm->tm_mday == 0)// bad date can't round by fsec
        return false;

    tm2timestamp(tm, *fsec, NULL, &result);
    rc = memset_s(tm, sizeof(*tm), 0, sizeof(*tm));
    securec_check(rc, "\0", "\0");
    *fsec = 0;
    timestamp2tm(result, NULL, tm, fsec, NULL, NULL);
    if (tm->tm_year > B_FORMAT_MAX_YEAR_OF_DATE)
        return false;
    return true;
}

/**
 *  The function is similar to timestamp_in, but uses date_flag to control the parsing process.
*/
void datetime_in_with_flag_internal(const char *str, struct pg_tm *result_tm, fsec_t *result_fsec, int &result_tm_type,
    unsigned int date_flag)
{
    int nano = 0;
    bool warnings = false;
    bool null_func_result = false;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec = 0;
    int tm_type = DTK_NONE;

    errno_t rc = memset_s(tm, sizeof(struct pg_tm), 0, sizeof(struct pg_tm));
    securec_check(rc, "\0", "\0");
    cstring_to_datetime(str, date_flag, tm_type, tm, fsec, nano, warnings, &null_func_result);
    if (warnings || null_func_result || tm_type == DTK_NONE) {
        int errlevel = (SQL_MODE_STRICT() || null_func_result || tm_type == DTK_NONE) ? ERROR : WARNING;
        if (errlevel == ERROR) {
            ereport(errlevel,
                    (errcode(DTERR_BAD_FORMAT), errmsg("Incorrect datetime value: \"%s\"", str)));
        } else {
            ereport(errlevel,
                    (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect date or datetime value: \"%s\"", str)));
        }
    }
    if (!datetime_add_nanoseconds_with_round(tm, fsec, nano)) {
        ereport(ERROR, (errcode(DTERR_FIELD_OVERFLOW), errmsg("Incorrect datetime value: \"%s\"", str)));
    }
    rc = memcpy_s(result_tm, sizeof(struct pg_tm), tm, sizeof(struct pg_tm));
    securec_check(rc, "\0", "\0");
    *result_fsec = fsec;
    result_tm_type = tm_type;
}

// convert Numeric arg into lldiv_t
// div.quot is the integer of the Numeric
// div.rem is the fractional part of the Numeric
void Numeric_to_lldiv(Numeric num, lldiv_t *div)
{
    NumericVar n;
    init_var_from_num(num, &n);
    NumericVar2lldiv(&n, div);
}

static inline void dayname_internal(struct pg_tm *result_tm, Datum *result)
{
    int weekday = cal_weekday_interval(result_tm, false);
    MyLocale *locale = MyLocaleSearch(GetSessionContext()->lc_time_names);
    *result = DirectFunctionCall1(textin, CStringGetDatum(locale->day_names[weekday]));
}

static inline bool zero_date_tm(struct pg_tm *result_tm)
{
    return (!result_tm->tm_year && !result_tm->tm_mon && !result_tm->tm_mday);
}

/* dayname()
 * @param1 specified date,                                     text
 * @return week name of specified date in b compatibility,     text
 */
Datum dayname_text(PG_FUNCTION_ARGS)
{
    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;
    Datum result;

    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }
    dayname_internal(result_tm, &result);
    PG_RETURN_DATUM(result);
}

// int8, int4 and float8 date input for dayname
Datum dayname_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;
    Datum result;

    Numeric_to_lldiv(num, &div);
    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    dayname_internal(result_tm, &result);
    PG_RETURN_DATUM(result);
}

/* b_db_cal_weekday_internal()
 * @param tm                        specified date structure
 * @param sunday_is_first_day       whether sunday is first day of week, a standard of ODBC
 * @return weekday of the date, if sunday_is_first_day is true, [0,6] means [Sunday, Saturday]
 *                              if sunday_is_first_day is false, [0,6] means [Monday, Sunday]
 */
static int cal_weekday_interval(struct pg_tm* tm, bool sunday_is_first_day)
{
    int weekday = (j2day(date2j(tm->tm_year, tm->tm_mon, tm->tm_mday))); //[0,6] is [Sunday,Saturday]
    if (sunday_is_first_day) {
        weekday = (weekday + 1) % DAYS_PER_WEEK;
        if ((tm->tm_year == B_FORMAT_MIN_YEAR_OF_DATE && tm->tm_mon >= MARCH) || tm->tm_year > B_FORMAT_MIN_YEAR_OF_DATE) {//fix bug because of 0000-2-29
            weekday = (weekday - 1 + DAYS_PER_WEEK) % DAYS_PER_WEEK;
        }
    } else {
        weekday = (weekday - 1 + DAYS_PER_WEEK) % DAYS_PER_WEEK;    //[0,6] is [Monday,Sunday]
        if (tm->tm_year == B_FORMAT_MIN_YEAR_OF_DATE && tm->tm_mon <= FEBRUARY) {   // fix bug because of 0000-2-29
            weekday = (weekday + 1) % DAYS_PER_WEEK;
        }
    }
    return weekday;
}

static inline void monthname_internal(struct pg_tm *result_tm, Datum *result)
{
    MyLocale *locale = MyLocaleSearch(GetSessionContext()->lc_time_names);
    *result = DirectFunctionCall1(textin, CStringGetDatum(locale->month_names[result_tm->tm_mon - 1]));
}

/* monthname()
 * @param1  specified date,                                     text
 * @return  month name of specified date in b compatibility,    text
 */
Datum monthname_text(PG_FUNCTION_ARGS)
{
    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;
    Datum result;
    
    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, NO_ZERO_DATE_SET()) || result_tm->tm_mon == 0) {
        PG_RETURN_NULL();
    }
    monthname_internal(result_tm, &result);
    PG_RETURN_DATUM(result);
}

Datum monthname_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;
    Datum result;

    Numeric_to_lldiv(num, &div);
    if (IS_ZERO_NUMBER_DATE(div.quot, div.rem)) {   // when number input is 0, monthname return NULL
        PG_RETURN_NULL();
    }
    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE) ||
        result_tm->tm_mon == 0) {   // when month number is 0, monthname return NULL
        PG_RETURN_NULL();
    }
    monthname_internal(result_tm, &result);
    PG_RETURN_DATUM(result);
}

/* month()
 * @param1  specified date,                                       text
 * @return  month number of specified date in b compatibility,    int32
 */
Datum month_text(PG_FUNCTION_ARGS)
{
    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;

    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, NO_ZERO_DATE_SET())) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT32(result_tm->tm_mon);
}

Datum month_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;

    Numeric_to_lldiv(num, &div);
    if (IS_ZERO_NUMBER_DATE(div.quot, div.rem)) {
        PG_RETURN_INT32(0);
    }
    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT32(result_tm->tm_mon);
}

static inline void b_last_day_internal(struct pg_tm *result_tm, DateADT *result)
{
    result_tm->tm_mday = day_tab[isleap(result_tm->tm_year)][result_tm->tm_mon - 1];
    //fix 0000-2-29 bug
    if (result_tm->tm_year == 0 && result_tm->tm_mon == 2)
        result_tm->tm_mday = DAYNUM_FEB_NONLEAPYEAR;
    *result = date2j(result_tm->tm_year, result_tm->tm_mon, result_tm->tm_mday) - POSTGRES_EPOCH_JDATE;
}

/* b_db_last_day()
 * @param1 specified date,                                 text
 * @return last day of specified date in b compatibility,  date
 */
Datum last_day_text(PG_FUNCTION_ARGS)
{
    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;
    DateADT result;

    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }
    b_last_day_internal(result_tm, &result);
    PG_RETURN_DATEADT(result);
}

Datum last_day_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;
    DateADT result;

    Numeric_to_lldiv(num, &div);
    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    b_last_day_internal(result_tm, &result);
    PG_RETURN_DATEADT(result);
}

/* b_db_date()
 * @param1 specified date,                                 text
 * @return extract date part of the argument,              text
 */
Datum b_db_date_text(PG_FUNCTION_ARGS)
{
    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;

    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, NO_ZERO_DATE_SET())) {
        PG_RETURN_NULL();
    }
    PG_RETURN_DATEADT(date2j(result_tm->tm_year, result_tm->tm_mon, result_tm->tm_mday) - POSTGRES_EPOCH_JDATE);
}

Datum b_db_date_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;

    Numeric_to_lldiv(num, &div);
    if (IS_ZERO_NUMBER_DATE(div.quot, div.rem)) {
        PG_RETURN_DATEADT(DATE_ALL_ZERO_VALUE);
    }
    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_DATEADT(date2j(result_tm->tm_year, result_tm->tm_mon, result_tm->tm_mday) - POSTGRES_EPOCH_JDATE);
}

/* dayofmonth_text()
 * @param1 specified date,                                 text
 * @return extract day part of the argument,               int32
 */
Datum dayofmonth_text(PG_FUNCTION_ARGS)
{
    text *raw_text = PG_GETARG_TEXT_PP(0);
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;

    char *date_str = text_to_cstring(raw_text);
    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, NO_ZERO_DATE_SET())) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT32(result_tm->tm_mday);
}

Datum dayofmonth_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;

    Numeric_to_lldiv(num, &div);
    if (IS_ZERO_NUMBER_DATE(div.quot, div.rem)) {
        PG_RETURN_INT32(0);
    }
    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT32(result_tm->tm_mday);
}

/* b_db_sumdays()
 * @param year  specified year
 * @param month specified month
 * @param day   specified day
 * @return days from 0000-00-00
 */
static int b_db_sumdays(int year, int month, int day)
{
     int days =  date2j(year,month,day) - date2j(0,1,1) + 1;
     if ((year == B_FORMAT_MIN_YEAR_OF_DATE && month >= MARCH) || year > B_FORMAT_MIN_YEAR_OF_DATE)    //fix bug because of 0000-2-29
        days--;
    return days;
}

/*
There are 8 modes in function week. Each certain mode consists of three features.
Therefor, we use three bits to represent that three features.
The meaning of each bit is as follows:
    Bit(0):
            0 -> Monday is start day of week
            1 -> Sunday is start day of week

    Bit(1):
            0 -> week number is between 0 and 53.
                0 means last week of the previous year.
                1 means first week of given year.

            1 -> week number is between 1 and 53
                One may get week 53 for a date in January (when the week is that last week of previous year)
                and week 1 for a date in December.

    Bit(2):
            0 -> If the week contains January 1 has four or more day in the new year, then it is week 1.
                 Otherwise it is the last week of the previous year, and the next week if week 1.

            1 -> The week that contains the first 'first-day-of-week' is week one.

 *@param mode specified week mode in [0~7]
 *@return adjusted mode
*/
int64 b_db_weekmode(int64 mode)
{
    /* make sure that weekmode is an integer between 0 and 7 */
    int64 weekmode = (mode & 7);

    /* if Monday is not first weekday, invert bit(3) */
    if(!(weekmode & MONDAY_IS_FIRST_WEEKDAY)) weekmode ^= FIRST_FULL_WEEK;
    return weekmode;
}

/*b_db_cal_week()
 *@param tm specified date info structure
 *@param mode specified week mode in [0~7]
 *@param year this parameter may be used in yearweek()
 *@return week numbers
*/
int b_db_cal_week(struct pg_tm* tm, int64 mode, uint *year)
{
    struct pg_tm tmp_tt, *tmp_tm = &tmp_tt;
    long days;
    long weekday;
    long tm_year = tm->tm_year;
    long tm_mon = tm->tm_mon;
    long tm_mday = tm->tm_mday;

    long sum_day = b_db_sumdays(tm_year, tm_mon, tm_mday);
    long sum_start_day = b_db_sumdays(tm_year, 1, 1);

    bool monday_is_first_day = mode & MONDAY_IS_FIRST_WEEKDAY;
    bool week_scope = mode & SCOPE_OF_WEEK;
    bool first_week = mode & FIRST_FULL_WEEK;

    tmp_tm->tm_year = tm->tm_year;
    tmp_tm->tm_mon = 1;
    tmp_tm->tm_mday = 1;
    weekday = cal_weekday_interval(tmp_tm, !monday_is_first_day);
    *year = tm_year;

    if (tm_mon == JANUARY && tm_mday <= DAYS_PER_WEEK - weekday) {
        if (!week_scope && ((first_week && weekday != 0) || (!first_week && weekday >= FOUR_DAYS_IN_YEAR)))
            return 0;
        
        week_scope = true;
        (*year)--;
        days = isleap(*year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_COMMON_YEAR;
        sum_start_day -= days;
        weekday = (weekday + (WEEKS_PER_YEAR + 1) * DAYS_PER_WEEK - days) % DAYS_PER_WEEK;
    }

    if ((first_week && weekday != 0) || (!first_week && weekday >= FOUR_DAYS_IN_YEAR))
        days = sum_day - (sum_start_day + (DAYS_PER_WEEK - weekday));
    else
        days = sum_day - (sum_start_day - weekday);
    
    if (week_scope && days >= WEEKS_PER_YEAR * DAYS_PER_WEEK) {
        weekday = (weekday + (isleap(*year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_COMMON_YEAR)) % DAYS_PER_WEEK;
        if ((first_week && weekday == 0) || (!first_week && weekday < FOUR_DAYS_IN_YEAR)) {
            (*year)++;
            return 1;
        }
    }

    return days / DAYS_PER_WEEK + 1;
}

static inline void week_internal(struct pg_tm *result_tm, int32 *week, int64 mode, uint* year)
{
    uint tmp_year = 0;
    int64 final_mode;
    if (year != NULL) {// yearweek call it, else week call it
        final_mode = b_db_weekmode(mode) | SCOPE_OF_WEEK;
    } else {
        final_mode = b_db_weekmode(mode);
    }

    *week = b_db_cal_week(result_tm, final_mode, &tmp_year);
    if (year != NULL) {
        *year = tmp_year;
    }
}

Datum week_text(PG_FUNCTION_ARGS)
{
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;
    int32 week;
    int64 mode; 

    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));

    if (PG_ARGISNULL(1)) {
        mode = GetSessionContext()->default_week_format;
    } else {
        mode = PG_GETARG_INT64(1);
    }

    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }
    week_internal(result_tm, &week, mode, NULL);
    PG_RETURN_INT32(week);
}

Datum week_numeric(PG_FUNCTION_ARGS)
{
    Numeric num;
    int64 mode;
    lldiv_t div;
    struct pg_tm result_tt, *result_tm = &result_tt;
    int32 week;

    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    num = PG_GETARG_NUMERIC(0);
    Numeric_to_lldiv(num, &div);

    if (PG_ARGISNULL(1)) {
        mode = GetSessionContext()->default_week_format;
    } else {
        mode = PG_GETARG_INT64(1);
    }

    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    week_internal(result_tm, &week, mode, NULL);
    PG_RETURN_INT32(week);
}

Datum yearweek_text(PG_FUNCTION_ARGS)
{
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;
    int32 week;
    uint year;
    int64 mode;
    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    if (PG_ARGISNULL(1)) {
        mode = 0;
    } else {
        mode = PG_GETARG_INT64(1);
    }

    char *date_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    if (!datetime_in_with_sql_mode(date_str, result_tm, &fsec, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }
    week_internal(result_tm, &week, mode, &year);
    PG_RETURN_INT64(year * 100 + week);
}

Datum yearweek_numeric(PG_FUNCTION_ARGS)
{
    Numeric num;
    lldiv_t div;
    int64 mode;
    struct pg_tm result_tt, *result_tm = &result_tt;
    int32 week;
    uint year;

    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    num = PG_GETARG_NUMERIC(0);
    Numeric_to_lldiv(num, &div);
    if (PG_ARGISNULL(1)) {
        mode = 0;
    } else {
        mode = PG_GETARG_INT64(1);
    }

    if (!lldiv_decode_tm_with_sql_mode(num, &div, result_tm, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    week_internal(result_tm, &week, mode, &year);
    PG_RETURN_INT64(year * 100 + week);
}

/*
 * using timestamp_diff_internal to calculate
 * true if success
 * false if failed
 */
static bool timestampdiff_datetime_internal(int64 *result,  text *units, Timestamp dt1, Timestamp dt2)
{
    bool ret = true;
    int code;
    const char *msg = NULL;
    PG_TRY();
    {
        *result = timestamp_diff_internal(units, dt1, dt2, true);
    }
    PG_CATCH();
    {
        ret = false;
        if (SQL_MODE_STRICT()) {
            PG_RE_THROW();
        } else {
            code = geterrcode();
            msg = pstrdup(Geterrmsg());
            FlushErrorState();
        }
    }
    PG_END_TRY();
    if (msg) {
        ereport(WARNING, (errcode(code), errmsg("%s", msg)));
    }
    return ret;
}

/* 
 * function for B compatibility timestampdiff(unit, datetime/date, datetime/date)
 */
Datum timestampdiff_datetime_tt(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    char *str1 = text_to_cstring(PG_GETARG_TEXT_PP(2));
    char *str2 = text_to_cstring(PG_GETARG_TEXT_PP(1));
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec1, fsec2;
    Timestamp datetime1, datetime2;
    int64 result;

    if (!datetime_in_with_sql_mode(str1, tm1, &fsec1, TIME_NO_ZERO_DATE) ||
        !datetime_in_with_sql_mode(str2, tm2, &fsec2, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm1, fsec1, NULL, &datetime1);
    tm2timestamp(tm2, fsec2, NULL, &datetime2);
    if (!timestampdiff_datetime_internal(&result, units, datetime1, datetime2)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT64(result);
}

Datum timestampdiff_datetime_nn(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Numeric num1 = PG_GETARG_NUMERIC(2);
    Numeric num2 = PG_GETARG_NUMERIC(1);
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    lldiv_t div1, div2;
    fsec_t fsec1, fsec2;
    Timestamp datetime1, datetime2;
    int64 result;
    int date_type1 = 0;
    int date_type2 = 0;

    Numeric_to_lldiv(num1, &div1);
    Numeric_to_lldiv(num2, &div2);

    if (!lldiv_decode_datetime(num1, &div1, tm1, &fsec1, NORMAL_DATE, &date_type1) || 
        !lldiv_decode_datetime(num2, &div2, tm2, &fsec2, NORMAL_DATE, &date_type2)) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm1, fsec1, NULL, &datetime1);
    tm2timestamp(tm2, fsec2, NULL, &datetime2);
    if (!timestampdiff_datetime_internal(&result, units, datetime1, datetime2)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT64(result);
}

Datum timestampdiff_datetime_tn(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    char *str = text_to_cstring(PG_GETARG_TEXT_PP(1));
    Numeric num = PG_GETARG_NUMERIC(2);
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    lldiv_t div;
    fsec_t fsec1, fsec2;
    Timestamp datetime1, datetime2;
    int64 result;
    int date_type = 0;

    Numeric_to_lldiv(num, &div);

    if (!datetime_in_with_sql_mode(str, tm1, &fsec1, TIME_NO_ZERO_DATE) || 
        !lldiv_decode_datetime(num, &div, tm2, &fsec2, NORMAL_DATE, &date_type)) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm1, fsec1, NULL, &datetime2);
    tm2timestamp(tm2, fsec2, NULL, &datetime1);
    if (!timestampdiff_datetime_internal(&result, units, datetime1, datetime2)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT64(result);
}

void add_currentdate_to_time(TimeADT time, Timestamp *result)
{
    TimestampTz state_start_timestamp;
    Timestamp datetime;
    int tzp;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    
    state_start_timestamp = GetCurrentStatementStartTimestamp();
    timestamp2tm(state_start_timestamp, &tzp, tm, &fsec, NULL, NULL);
    tt.tm_hour = tt.tm_min = tt.tm_sec = 0;
    tm2timestamp(tm, 0, NULL, &datetime);
    *result = datetime + time;
}

/*
 * function for B compatibility timestampdiff(unit, time, time)
 */
Datum timestampdiff_time(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Timestamp datetime1, datetime2;
    int64 result;

    add_currentdate_to_time(PG_GETARG_TIMEADT(2), &datetime1);
    add_currentdate_to_time(PG_GETARG_TIMEADT(1), &datetime2);
    if (!timestampdiff_datetime_internal(&result, units, datetime1, datetime2)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT64(result);
}

/*
 * function for B compatibility timestampdiff(unit, time, datetime/date)
 */
Datum timestampdiff_time_before_t(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    char *str = text_to_cstring(PG_GETARG_TEXT_PP(2));
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    Timestamp datetime1, datetime2;
    int64 result;

    if (!datetime_in_with_sql_mode(str, tm, &fsec, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm, fsec, NULL, &datetime1);
    add_currentdate_to_time(PG_GETARG_TIMEADT(1), &datetime2);
    if (!timestampdiff_datetime_internal(&result, units, datetime1, datetime2)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT64(result);
}

Datum timestampdiff_time_before_n(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    Numeric num = PG_GETARG_NUMERIC(2);
    struct pg_tm tt, *tm = &tt;
    lldiv_t div;
    fsec_t fsec;
    Timestamp datetime1, datetime2;
    int64 result;
    int date_type = 0;

    Numeric_to_lldiv(num, &div);

    if (!lldiv_decode_datetime(num, &div, tm, &fsec, NORMAL_DATE, &date_type)) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm, fsec, NULL, &datetime1);
    add_currentdate_to_time(PG_GETARG_TIMEADT(1), &datetime2);
    if (!timestampdiff_datetime_internal(&result, units, datetime1, datetime2)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT64(result);
}

/*
 * return -1 if the str is not a zone or izone
 * return 0 if the str maybe a zone
 * return 1 if the str maybe an izone
 */
static inline int tz_type(char *str)
{
    if (!str || strlen(str) == 0)
        return -1;
    if (*str == '+' || *str == '-') {
        return 1;
    } else if (isalpha((unsigned char)*str)) {
        return 0;
    }
    return -1;
}

/*
 * true if datetime is in range [1970-1-1 00:00:01.000000, 2038-01-19 03:14:07.999999]
 */
static inline bool datetime_in_unixtimestmap(Timestamp datetime)
{
    bool ret = true;
#ifdef HAVE_INT64_TIMESTAMP
    if (datetime < UNIXTIMESTAMP_START_VALUE || datetime > UNIXTIMESTAMP_END_VALUE)
        ret = false;
#else
    if (datetime < (double)UNIXTIMESTAMP_START_VALUE / USECS_PER_SEC || 
        datetime > (double)UNIXTIMESTAMP_END_VALUE / USECS_PER_SEC)
        ret = false;
#endif
    return ret;
}

/**
 * function to justify if the izone is in range [-12:59, +13:00]
 * return true if izone is in range [-12:59, +13:00]
 * false if not
 */
static inline bool is_izone_in_range(char *izone, Interval **span)
{
    bool ret = true;
    bool not_in_range = false;
    PG_TRY();
    {
        *span = (Interval*)DirectFunctionCall3(interval_in, CStringGetDatum(izone), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
        if ((*span)->month || (*span)->day || ((*span)->time % SECS_PER_MINUTE) != 0) {
            ret = false;
        }
#ifdef HAVE_INT64_TIMESTAMP
        not_in_range = (*span)->time < -IZONE_BOUND1 || (*span)->time > IZONE_BOUND2;
#else
        not_in_range = (*span)->time < -(IZONE_BOUND1 / USECS_PER_SEC) || (*span)->time > (IZONE_BOUND2 / USECS_PER_SEC);
#endif
        if (not_in_range)
            ret = false;
    }
    PG_CATCH();
    {
        ret = false;
        FlushErrorState();
    }
    PG_END_TRY();
    return ret;
}

/*
 * function for B compatibility convert_tz(datetime, from_tz, to_tz)
 */
static inline bool resolve_tzname(text *zone, char *tzname, pg_tz **tzp, int *type, int *val)
{
    text_to_cstring_buffer(zone, tzname, sizeof(tzname));
    char* lowzone = downcase_truncate_identifier(tzname, strlen(tzname), false);

    *type = DecodeSpecial(0, lowzone, val);
    if(*type == TZ || *type == DTZ) {
        return true;
    } else {
        *tzp = pg_tzset(tzname);
        if (*tzp != NULL)
            return true;
    }
    return false;
}

static inline bool calc_timestamp_internal(text *zone, Timestamp datetime, TimestampTz *result)
{
    int tz = 0;
    int val = 0;
    int type = 0;
    pg_tz *tzp = NULL;
    char tzname[TZ_STRLEN_MAX + 1];
    
    if(!resolve_tzname(zone, tzname, &tzp, &type, &val)) {
        return false;
    }
    
    if(type == TZ || type == DTZ) {
       tz = -(val * MINS_PER_HOUR);
       *result = dt2local(datetime, tz);
    } else {
        if (tzp != NULL) {
            /* Apply the timezone change */
            struct pg_tm tm;
            fsec_t fsec;

            if (timestamp2tm(datetime, NULL, &tm, &fsec, NULL, tzp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
            tz = DetermineTimeZoneOffset(&tm, tzp);
            if (tm2timestamp(&tm, fsec, &tz, result) != 0)
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("could not convert to time zone \"%s\"", tzname)));
        } else {
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("time zone \"%s\" not recognized", tzname)));
            *result = 0; /* keep compiler quiet */
        }
    }

    return true;
}

static inline bool calc_timestamptz_internal(text *zone, TimestampTz timestamp, Timestamp *result)
{
    int tz = 0;
    int val = 0;
    int type = 0;
    pg_tz *tzp = NULL;
    char tzname[TZ_STRLEN_MAX + 1];
    
    if(!resolve_tzname(zone, tzname, &tzp, &type, &val)) {
        return false;
    }
    
    if(type == TZ || type == DTZ) {
       tz = val * MINS_PER_HOUR;
       *result = dt2local(timestamp, tz);
    } else {
        if (tzp != NULL) {
            /* Apply the timezone change */
            struct pg_tm tm;
            fsec_t fsec;

            if (timestamp2tm(timestamp, &tz, &tm, &fsec, NULL, tzp) != 0)
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
            tz = DetermineTimeZoneOffset(&tm, tzp);
            if (tm2timestamp(&tm, fsec, NULL, result) != 0)
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("could not convert to time zone \"%s\"", tzname)));
        } else {
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("time zone \"%s\" not recognized", tzname)));
            *result = 0; /* keep compiler quiet */
        }
    }

    return true;
}

static inline bool convert_tz_internal(Timestamp raw_datetime, text *expr2, text *expr3, char *str2, char *str3, int from_ok, int to_ok, Timestamp *result)
{
    Timestamp datetime;
    Interval *interval1 = NULL, *interval2 = NULL;
    int code;
    const char *msg = NULL;

    datetime = raw_datetime;

    PG_TRY();
    {
        if (from_ok == 0) { // if expr2 is zone
            if (!calc_timestamp_internal(expr2, datetime, &datetime)) {
                return false;
            }
        } else {    // if expr2 is izone
            if (!is_izone_in_range(str2, &interval1)) {
                return false;
            }
            datetime = (Timestamp)DirectFunctionCall2(timestamp_izone, PointerGetDatum(interval1), TimestampGetDatum(datetime));
        }
        if (!datetime_in_unixtimestmap(datetime)) {
            *result = raw_datetime;
            return true;
        }
        if (to_ok == 0) {   // if expr3 is zone
             if (!calc_timestamptz_internal(expr3, datetime, &datetime)) {
                return false;
            }
        } else {    // if expr3 is izone
             if (!is_izone_in_range(str3, &interval2)) {
                return false;
            }
            datetime = (Timestamp)DirectFunctionCall2(timestamptz_izone, PointerGetDatum(interval2), TimestampGetDatum(datetime));
        }
        *result = datetime;
        return true;
    }
    PG_CATCH();
    {
        if (SQL_MODE_STRICT()) {
            PG_RE_THROW();
        } else {
            code = geterrcode();
            msg = pstrdup(Geterrmsg());
            FlushErrorState();
        }
    }
    PG_END_TRY();
    ereport(WARNING, (errcode(code), errmsg("%s", msg)));
    return false;
}

/*
 * function for B compatibility convert_tz(datetime, from_tz, to_tz)
 */
Datum convert_tz_t(PG_FUNCTION_ARGS)
{
    char *str1 = text_to_cstring(PG_GETARG_TEXT_PP(0));
    text *expr2 = PG_GETARG_TEXT_PP(1);
    text *expr3 = PG_GETARG_TEXT_PP(2);
    char *str2 = text_to_cstring(expr2);
    char *str3 = text_to_cstring(expr3);
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    Timestamp result, raw_datetime;

    int from_ok = tz_type(str2);
    int to_ok = tz_type(str3);
    
    if (!datetime_in_with_sql_mode(str1, tm, &fsec, TIME_NO_ZERO_DATE) || from_ok == -1 || to_ok == -1) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm, fsec, NULL, &raw_datetime);
    if (!convert_tz_internal(raw_datetime, expr2, expr3, str2, str3, from_ok, to_ok, &result)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_TIMESTAMP(result);
}

Datum convert_tz_n(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    text *expr2 = PG_GETARG_TEXT_PP(1);
    text *expr3 = PG_GETARG_TEXT_PP(2);
    char *str2 = text_to_cstring(expr2);
    char *str3 = text_to_cstring(expr3);
    lldiv_t div;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int date_type = 0;
    Timestamp raw_datetime, result;
    
    int from_ok = tz_type(str2);
    int to_ok = tz_type(str3);
    Numeric_to_lldiv(num, &div);
    if (!lldiv_decode_datetime(num, &div, tm, &fsec, NORMAL_DATE, &date_type) || from_ok == -1 || to_ok == -1) {
        PG_RETURN_NULL();
    }
    tm2timestamp(tm, fsec, NULL, &raw_datetime);
    if (!convert_tz_internal(raw_datetime, expr2, expr3, str2, str3, from_ok, to_ok, &result)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_TIMESTAMP(result);
}

Datum convert_tz_time(PG_FUNCTION_ARGS)
{
    text *expr2 = PG_GETARG_TEXT_PP(1);
    text *expr3 = PG_GETARG_TEXT_PP(2);
    char *str2 = text_to_cstring(expr2);
    char *str3 = text_to_cstring(expr3);
    Timestamp raw_datetime, result;
    
    int from_ok = tz_type(str2);
    int to_ok = tz_type(str3);

    add_currentdate_to_time(PG_GETARG_TIMEADT(0), &raw_datetime);
    if (!convert_tz_internal(raw_datetime, expr2, expr3, str2, str3, from_ok, to_ok, &result)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_TIMESTAMP(result);
}

/*
 * add an interval to a datetime, giving a new datetime and assign it to result.
 * false if parameter datetime or result out of range, otherwise true
 */
bool datetime_add_interval(Timestamp datetime, Interval *span, Timestamp *result)
{
    span->month = -span->month;
    span->day = -span->day;
    span->time = -span->time;
    return datetime_sub_interval(datetime, span, result, true);
}

/**
 * addtime(date/datetime/time, time)
*/
Datum addtime_text(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT()) ? ERROR : WARNING;
    char *str1 = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char *str2 = text_to_cstring(PG_GETARG_TEXT_PP(1));
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec1 = 0, fsec2 = 0;
    int timeSign1 = 1, timeSign2 = 1;
    int tm_type1, tm_type2;
    bool warnings1 = false, warnings2 = false;
    bool null_func_result1 = false, null_func_result2 = false;
    TimeADT time2;

    /* decode arg1 */
    errno_t rc = memset_s(tm1, sizeof(tt1), 0, sizeof(tt1));
    securec_check(rc, "\0", "\0");
    cstring_to_time(str1, tm1, fsec1, timeSign1, tm_type1, warnings1, &null_func_result1);
    if (warnings1 || null_func_result1) {
        ereport(errlevel,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect time value: \"%s\"", str1)));
        if (null_func_result1) {
            PG_RETURN_NULL();
        }
    }

    /* decode arg2 */
    rc = memset_s(tm2, sizeof(tt2), 0, sizeof(tt2));
    securec_check(rc, "\0", "\0");
    cstring_to_time(str2, tm2, fsec2, timeSign2, tm_type2, warnings2, &null_func_result2);
    /* If arg2 is a datetime, the function returns NULL. */
    if (tm_type2 == DTK_DATE_TIME) {
        if (warnings2) {
            ereport(errlevel,
                    (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect time value: \"%s\"", str2)));
        }
        PG_RETURN_NULL();
    }
    /* If arg2 is a time format but not normal format.*/
    if (warnings2 || null_func_result2) {
        ereport(errlevel,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect time value: \"%s\"", str2)));
        if (null_func_result2) {    // field overflow, for example '10:10:60'
            PG_RETURN_NULL();
        }
    }
    tm2time(tm2, fsec2, &time2);
    time2 *= timeSign2;

    /* calculate result */
    if (tm_type1 == DTK_DATE_TIME) {
        Timestamp datetime1, result;
        tm2timestamp(tm1, fsec1, NULL, &datetime1);
        if (datetime_sub_time(datetime1, -time2, &result)) {
            /* The variable datetime or result does not exceed the specified
             * range*/
            if (result >= B_FORMAT_TIMESTAMP_FIRST_YEAR)
                return DirectFunctionCall1(datetime_text, result);
            else {
                PG_RETURN_NULL();   // When result is less than '0001-1-1 00:00:00', return NULL.
            }
        }
        ereport(errlevel, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                        errmsg("date/time field overflow")));
    } else {
        TimeADT time1, result;
        tm2time(tm1, fsec1, &time1);
        time1*= timeSign1;
        result = time1 + time2;
        if (!time_in_range(result)) {
            ereport(errlevel, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), errmsg("date/time field overflow")));
        }
        return DirectFunctionCall1(time_text, result);
    }
    PG_RETURN_NULL();
}

bool datetime_in_with_sql_mode(char *str, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag)
{
    bool ret = true;
    bool raise_warning = false;
    int code;
    const char *msg = NULL;
    int tm_type = DTK_NONE;
    PG_TRY();
    {
        datetime_in_with_flag_internal(str, tm, fsec, tm_type, date_flag);
    }
    PG_CATCH();
    {
        ret = false;
        if (SQL_MODE_STRICT()) {
            PG_RE_THROW();
        } else {
            raise_warning = true;
            code = geterrcode();
            msg = pstrdup(Geterrmsg());
            FlushErrorState();
        }
    }
    PG_END_TRY();
    if (raise_warning) {
        ereport(WARNING, (errcode(code), errmsg("%s", msg)));
    }
    return ret;
}

bool datetime_in_with_sql_mode_internal(char *str, struct pg_tm *tm, fsec_t *fsec, int &tm_type,
    unsigned int date_flag)
{
    bool ret = true;
    bool raise_warning = false;
    int code;
    const char *msg = NULL;
    PG_TRY();
    {
        datetime_in_with_flag_internal(str, tm, fsec, tm_type, date_flag);
    }
    PG_CATCH();
    {
        ret = false;
        if (SQL_MODE_STRICT()) {
            PG_RE_THROW();
        } else {
            raise_warning = true;
            code = geterrcode();
            msg = pstrdup(Geterrmsg());
            FlushErrorState();
        }
    }
    PG_END_TRY();
    if (raise_warning) {
        ereport(WARNING, (errcode(code), errmsg("%s", msg)));
    }
    return ret;
}

// functions to help date_format to get the result str length
static inline int get_result_len(char *format, int len)
{
    int result = 0;
    char *end = format + len;
    for (;format != end; format++) {
        if (*format != '%' || format + 1 == end) {
            result++;
        } else {
            switch (*++format) {
                case 'e':
                case 'c':
                case 's':
                case 'S':
                case 'p':
                case 'l':
                case 'i':
                case 'I':
                case 'h':
                case 'd':
                case 'm':
                case 'y':
                case 'v':
                case 'V':
                case 'u':
                case 'U':
                    result += TWO_DIGIT_LEN;
                    break;
                case 'j':
                    result += THREE_DIGIT_LEN;
                    break;
                case 'X':
                case 'x':
                case 'Y':
                case 'D':
                    result += FOUR_DIGIT_LEN;
                    break;
                case 'f':
                    result += SIX_DIGIT_LEN;
                    break;
                case 'k':
                case 'H':
                    result += SEVEN_DIGIT_LEN;
                    break;
                case 'T':
                    result += EIGHT_DIGIT_LEN;    // "hh:mm:ss"
                    break;
                case 'r':
                    result += THREE_DIGIT_LEN + EIGHT_DIGIT_LEN;   // "hh:mm:ss [A|P]M", length is 11
                    break;
                case 'a':
                case 'b':
                    result += FOUR_DIGIT_LEN * EIGHT_DIGIT_LEN;    // length is 32
                    break;
                case 'M':
                case 'W':
                    result += EIGHT_DIGIT_LEN * EIGHT_DIGIT_LEN;   // length is 64
                    break;
                case 'w':
                case '%':
                default:
                    result++;
                    break;
            }
        }
    }
    return (result > MAXDATELEN ? result : MAXDATELEN);
}

static inline bool date_format_internal(char *str, char *buf, char *format, int format_len, int remain, struct pg_tm *tm, fsec_t fsec)
{
    int32 hours_i;      /* hour in range of 0..12 */
    MyLocale *locale = NULL;
    errno_t rc = EOK;
    char *ptr = NULL, *end = NULL; /* head and tail of format */
    int insert_len = 0;            /* number of characters inserted into str */

    str[0] = '\0';
    ptr = format;
    end = ptr + format_len;
    for (; ptr != end; ptr++) {
        if (*ptr != '%' || ptr + 1 == end) {
            rc = strncat_s(str, remain, ptr, 1);
            securec_check(rc, "", "");
            insert_len = 1;
            remain -= insert_len;
            continue;
        }
        switch (*++ptr) {
            case 'a':{  // abbreviated weekday name
                if (!(tm->tm_mon || tm->tm_year))
                    return false;
                int weekday = cal_weekday_interval(tm, true);
                if (locale == NULL) {
                    locale = MyLocaleSearch(GetSessionContext()->lc_time_names);
                }
                char *weekname = locale->ab_day_names[weekday - 1];
                insert_len = sprintf_s(buf, MAXDATELEN, "%s", weekname);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'b':{  // abbreviated month name
                if (!tm->tm_mon)
                    return false;
                if (locale == NULL) {
                    locale = MyLocaleSearch(GetSessionContext()->lc_time_names);
                }
                char *monthname = locale->ab_month_names[tm->tm_mon - 1];
                insert_len = sprintf_s(buf, MAXDATELEN, "%s", monthname);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'D':{  // day of month with suffix
                insert_len = sprintf_s(buf, MAXDATELEN, "%d", tm->tm_mday);
                securec_check_ss(insert_len, "", "");
                
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                remain -= insert_len;
                if (tm->tm_mday >= SUFFIX_TH_BEGIN && tm->tm_mday <= SUFFIX_TH_END) {
                    rc = strcat_s(str, remain,"th");
                    securec_check(rc, "", "");
                    insert_len = TWO_DIGIT_LEN;
                    remain -= insert_len;
                    continue;
                }
                switch (tm->tm_mday % SUFFIX_TH_BEGIN) {
                    case SUFFIX_ST:
                        rc = strcat_s(str, remain,"st");
                        break;
                    case SUFFIX_ND:
                        rc = strcat_s(str, remain,"nd");
                        break;
                    case SUFFIX_RD:
                        rc = strcat_s(str, remain,"rd");
                        break;
                    default:
                        rc = strcat_s(str, remain,"th");
                        break;
                    }
                securec_check(rc, "", "");
                insert_len = TWO_DIGIT_LEN;
                break;
            }
            case 'j':{  // day of the year
                int result = (date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - date2j(tm->tm_year, 1, 1) + 1);
                insert_len = sprintf_s(buf, MAXDATELEN, "%03d", result);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'M':{  // full name of month
                if(!tm->tm_mon)
                    return false;
                if (locale == NULL) {
                    locale = MyLocaleSearch(GetSessionContext()->lc_time_names);
                }
                char *monthname = locale->month_names[tm->tm_mon - 1];
                insert_len = sprintf_s(buf, MAXDATELEN, "%s", monthname);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'U':{  // week number under mode 0
                uint year = 0;
                int week = b_db_cal_week(tm, FIRST_FULL_WEEK, &year);
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", week);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'u':{  // week number under mode 1
                uint year = 0;
                int week = b_db_cal_week(tm, MONDAY_IS_FIRST_WEEKDAY, &year);
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", week);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'V':{  // week number under mode 2
                uint year = 0;
                int week = b_db_cal_week(tm, (SCOPE_OF_WEEK | FIRST_FULL_WEEK), &year);
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", week);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'v':{  // week number under mode 3
                uint year = 0;
                int week = b_db_cal_week(tm, (SCOPE_OF_WEEK | MONDAY_IS_FIRST_WEEKDAY), &year);
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", week);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'W':{  // full weekday name
                if(!(tm->tm_mon||tm->tm_year))
                    return false;
                int weekday = cal_weekday_interval(tm, true);
                if (locale == NULL) {
                    locale = MyLocaleSearch(GetSessionContext()->lc_time_names);
                }
                char *weekname = locale->day_names[weekday - 1];
                insert_len = sprintf_s(buf, MAXDATELEN, "%s", weekname);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'w':{  // day of the week
                if(!(tm->tm_mon||tm->tm_year))
                    return false;
                int weekday = cal_weekday_interval(tm, true);
                insert_len = sprintf_s(buf, MAXDATELEN, "%d", weekday);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'X':{  // Year for the week where Sunday is the first day of the week, used with %V
                uint year;
                b_db_cal_week(tm, (SCOPE_OF_WEEK | FIRST_FULL_WEEK), &year);
                insert_len = sprintf_s(buf, MAXDATELEN, "%04u", year);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'x':{  // Year for the week, where Monday is the first day of the week, used with %v
                uint year;
                b_db_cal_week(tm, (SCOPE_OF_WEEK | MONDAY_IS_FIRST_WEEKDAY), &year);
                insert_len = sprintf_s(buf, MAXDATELEN, "%04u", year);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'c':{  // month
                insert_len = sprintf_s(buf, MAXDATELEN, "%d", tm->tm_mon);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'e':{  // day of the month
                insert_len = sprintf_s(buf, MAXDATELEN, "%d", tm->tm_mday);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'd':{  // day of the month, always 2 digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_mday);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'm':{  // month, always 2 digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_mon);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'y':{  // year, 2 digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_year % YEAR_100);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'Y':{  // year, four digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%04d", tm->tm_year);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'f':{  // microseconds
                insert_len = sprintf_s(buf, MAXDATELEN, "%06d", fsec);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'H':{  // hours in range of 0..23, always 2 digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_hour);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'k': { // hours in range of 0..23
                insert_len = sprintf_s(buf, MAXDATELEN, "%d", tm->tm_hour);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'l': { // hours in range of 1..12
                hours_i = (tm->tm_hour % MAX_VALUE_24_CLOCK + MAX_VALUE_12_CLOCK - 1) % MAX_VALUE_12_CLOCK + MIN_VALUE_12_CLOCK;
                insert_len = sprintf_s(buf, MAXDATELEN, "%d", hours_i);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'h':
            case 'I': { // hours in range of 01..12
                hours_i = (tm->tm_hour % MAX_VALUE_24_CLOCK + MAX_VALUE_12_CLOCK - 1) % MAX_VALUE_12_CLOCK + MIN_VALUE_12_CLOCK;
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", hours_i);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'i': { // minutes, always 2 digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_min);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'p':{  // AM or PM
                hours_i = tm->tm_hour % MAX_VALUE_24_CLOCK;
                insert_len = AM_PM_LEN;
                rc = strcat_s(str, remain,
                                (hours_i < MAX_VALUE_12_CLOCK ? "AM" : "PM"));
                securec_check(rc, "", "");
                break;
            }
            case 'r':{  // time with format of 'hh:mm:ss [A/P]M', 12 hours
                insert_len = sprintf_s(buf,
                                       MAXDATELEN,
                                       (((tm->tm_hour % MAX_VALUE_24_CLOCK) < MAX_VALUE_12_CLOCK) ? "%02d:%02d:%02d AM" : "%02d:%02d:%02d PM"),
                                       (tm->tm_hour + MAX_VALUE_12_CLOCK - 1) % MAX_VALUE_12_CLOCK + MIN_VALUE_12_CLOCK,
                                       tm->tm_min,
                                       tm->tm_sec);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'S':
            case 's': { // seconds, always 2 digits
                insert_len = sprintf_s(buf, MAXDATELEN, "%02d", tm->tm_sec);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            case 'T':{  // time with format of 'hh:mm:ss', 24 hours
                insert_len = sprintf_s(
                    buf, MAXDATELEN, "%02d:%02d:%02d",
                    tm->tm_hour, tm->tm_min, tm->tm_sec);
                securec_check_ss(insert_len, "", "");
                rc = strcat_s(str, remain, buf);
                securec_check(rc, "", "");
                break;
            }
            default:
                rc = strncat_s(str, remain, ptr, 1);
                securec_check(rc, "", "");
                break;
        }
        remain -= insert_len;
    }
    return true;
}

/*
 * @Description: Create a formated date value in a string.
 * @return: The formated date value as a string.
 */
Datum date_format_text(PG_FUNCTION_ARGS)
{
    text *date_text = PG_GETARG_TEXT_PP(0);
    text *format_text = PG_GETARG_TEXT_PP(1);
    char buf[MAXDATELEN];          /* string for temporary storage */
    char *format = NULL;           /* format string */
    char *str = NULL;              /* return string */
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec = 0;
    int remain = 0;       /* remaining buffer size of variable str */

    /* convert date_text and format_text into string from text */
    text_to_cstring_buffer(date_text, buf, MAXDATELEN);
    if (!datetime_in_with_sql_mode(buf, tm, &fsec, NO_ZERO_DATE_SET())) {
        PG_RETURN_NULL();
    }

    format = text_to_cstring(format_text);
    int format_len = strlen(format);
    remain = get_result_len(format, format_len);
    str = (char*)palloc(remain + 1);

    if (!date_format_internal(str, buf, format, format_len, remain, tm, fsec)) {
        PG_RETURN_NULL();
    }

    text *result_text = cstring_to_text(str);
    pfree(str);
    PG_RETURN_TEXT_P(result_text);
}

Datum date_format_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    text *format_text = PG_GETARG_TEXT_PP(1);
    char buf[MAXDATELEN];          /* string for temporary storage */
    char *format = NULL;           /* format string */
    char *str = NULL;              /* return string */
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec = 0;
    int remain = 0;       /* remaining buffer size of variable str */
    int date_type = 0;

    Numeric_to_lldiv(num, &div);
    if (IS_ZERO_NUMBER_DATE(div.quot, div.rem)) {
        errno_t rc = memset_s(tm, sizeof(*tm), 0, sizeof(*tm));
        securec_check(rc, "\0", "\0");
        fsec = 0;
    } else if (!lldiv_decode_datetime(num, &div, tm, &fsec, NORMAL_DATE, &date_type)) {
        PG_RETURN_NULL();
    }

    format = text_to_cstring(format_text);
    int format_len = strlen(format);
    remain = get_result_len(format, format_len);
    str = (char*)palloc(remain + 1);

    if (!date_format_internal(str, buf, format, format_len, remain, tm, fsec)) {
        PG_RETURN_NULL();
    }
    text *result_text = cstring_to_text(str);
    pfree(str);
    PG_RETURN_TEXT_P(result_text);
}

/**
 * find the type to return
*/
static inline int find_return_type(char *format)
{
    const char *time_specifiers = "fHISThiklrs";
    const char *date_specifiers = "MVUXYWabcjmvuxyw";
    int len = strlen(format);
    bool contain_time = false, contain_date = false;

    for (int i = 0; i < len; ++i) {
        if (format[i] == '%' && (i + 1) != len) {
            ++i;
            if (!contain_time && strchr(time_specifiers, format[i])) {
                contain_time = true;
            } else if (!contain_date && strchr(date_specifiers, format[i])) {
                contain_date = true;
            }

            if (contain_time && contain_date) {
                return DTK_DATE_TIME;
            }
        }
    }

    if (contain_time) {
        if (contain_date)
            return DTK_DATE_TIME;
        else
            return DTK_TIME;
    } else {
        return DTK_DATE;
    }
    return DTK_DATE;
}

/**
 * return -1 if the result is a normal negtive value
 * reutrn 0 if the result >= 0
*/
static inline long long str2ll_with_endptr(char *start, int tmp_len, int *true_len, int *error)
{
    char cp[tmp_len + 1];
    for (int i = 0; i < tmp_len; ++i) {
        cp[i] = *(start + i);
    }
    cp[tmp_len] = '\0';
    char *endptr = NULL;
    errno = 0;
    long long value = strtoll(cp, &endptr, 10);
    if (endptr == cp)
        *error = EDOM;
    else if (errno == ERANGE)
        *error = ERANGE;
    else if (value < 0)
        *error = -1;
    else if (value >= 0)
        *error = 0;
    *true_len = (int)(endptr - cp);
    return value;
}

static inline int handling_2000_year(int year)
{
    if (year < YY_PART_YEAR)
        return year + YEAR_2000;
    return year + YEAR_1900;
}

/**
 * Find dayname or monthname
 * return -1 if find nothing
 * return >= 0 if find in array[#-0]
*/
static inline int find_index_with_name(char **name_array, int array_len, char *name_start, char *name_end, char **end_of_name)
{
    char *cp = name_start;
    for (; cp < name_end && isalpha((unsigned char)(*cp)); ++cp)
        ;
    bool is_find = false;
    const char* j;
    char *i;
    int pos;
    for (pos = 0; pos < array_len; ++pos) {
        j = name_array[pos];
        for (i = name_start;
             i != name_end && toupper((unsigned char)(*i)) == toupper((unsigned char)(*j)); 
             ++i, ++j)
            ;
        if (i == cp && *j == '\0') {
            is_find = true;
            break;
        }
    }
    if (is_find) {
        *end_of_name = cp;
        return pos;
    }
    return -1;
}

static inline bool ncmp_case_insensitive(const char *str1, const char *str2, int n)
{
    Assert(n >= 0);
    if ((int)strlen(str1) < n || (int)strlen(str2) < n)   // the length of each str can not less than n
        return false;
    for (int i = 0; i < n; ++i) {
        if (toupper((unsigned char)(*str1)) != toupper((unsigned char)(*str2)))
            return false;
        str1++;
        str2++;
    }
    return true;
}

/**
 * founction for calculating '%r' or '%T'.
 * %r -> %I:%i:%S %p
 * %T -> %H:%i:%S
 * return ture if success
*/
static inline bool calc_compound_specifiers(const char *format, char *str, struct pg_tm *tm, char **sub_end)
{
    bool time_in12 = false;
    int daypart = 0;
    int str_len = strlen(str);
    int format_len = strlen(format);
    char *str_end = str + str_len;
    const char *format_ptr = format, *format_end = format + format_len;
    for (; format_ptr != format_end && str != str_end; ++format_ptr) {
        while (isspace((unsigned char)(*str)) && str < str_end)
            ++str;
        if (str >= str_end)
            break;
        
        if (*format_ptr == '%' && (format_ptr + 1) != format_end) {
            int tmp_len = 0, true_len = 0;
            int val_len = (int)(str_end - str);
            ++format_ptr;
            int error = 0;
            switch (*format_ptr) {
                case 'I':
                    time_in12 = true;
                case 'H':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_hour = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                case 'i':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_min = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                case 'S':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_sec = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                case 'p':
                    if (val_len < TWO_DIGIT_LEN || ! time_in12)
                        goto err;
                    if (ncmp_case_insensitive(str, "PM", 2))
                        daypart = HOURS_HALF_DAY;
                    else if (!ncmp_case_insensitive(str, "AM", 2))
                        goto err;
                    str += TWO_DIGIT_LEN;
                    break;
                default:
                    goto err;
            }
            if (error)
                goto err;
        } else if (!isspace((unsigned char)(*format_ptr))) {
            if (*str != *format_ptr)
                goto err;
            str++;
        }
    }

    if (time_in12) {
        if (tm->tm_hour > HOURS_HALF_DAY || tm->tm_hour < 1)
            goto err;
        tm->tm_hour = tm->tm_hour % HOURS_HALF_DAY + daypart;
    }
    *sub_end = str;
    return true;

err:
    return false;
}

#define TIME_ORDER 0
#define DATE_ORDER 1
#define DATETIME_ORDER 2
static const char *type_name_arr[] = {"time", "date", "datetime"};
static inline const char* find_type_name(int type)
{
    switch (type) {
        case DTK_TIME:
            return type_name_arr[TIME_ORDER];
        case DTK_DATE:
            return type_name_arr[DATE_ORDER];
        case DTK_DATE_TIME:
            return type_name_arr[DATETIME_ORDER];
        default:
            return type_name_arr[DATETIME_ORDER];
    }
    return type_name_arr[DATETIME_ORDER];
}

/**
 * some additional calculation for str_to_date
 * return true if successs
 *        false if falied
*/
static inline bool yearday_to_date(struct pg_tm *tm, int yearday)
{
    DateADT date;
    int days = b_db_sumdays(tm->tm_year, 1, 1) + yearday - 1;
    if (days <= 0 || days > MAX_NUMBER_OF_DAY)
        return false;
    date = date2j(tm->tm_year, 1, 1) + yearday - 1;
    j2date(date, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    return true;
}

static inline bool week_year_to_date(struct pg_tm *tm, bool sunday_first_without_iso, bool strict_week_range, int weeknumber, int weekday, bool is_strict_week_year, int  strict_week_year)
{
    int days;
    long long weekday_b;
    struct pg_tm tmp_tt, *tmp_tm = &tmp_tt;
    DateADT date;

    /**
     * Some incorrect case of using %V, %v, %X, %x, %U and %u
     * case1: %V is used with %x
     *        or
     *        %v is used with %X
     * 
     * case2: %U or %u is used with %X or %x
    */
    if ((strict_week_range &&
        (strict_week_year < 0 ||
        is_strict_week_year != sunday_first_without_iso)) ||
        (!strict_week_range && strict_week_year >= 0))
        return false;

    tmp_tt.tm_year = (strict_week_range ? strict_week_year : tm->tm_year);
    tmp_tt.tm_mon = 1;
    tmp_tt.tm_mday = 1;
    days = b_db_sumdays(tmp_tt.tm_year, tmp_tt.tm_mon, tmp_tt.tm_mday);
    date = date2j(tmp_tt.tm_year, tmp_tt.tm_mon, tmp_tt.tm_mday);
    weekday_b = cal_weekday_interval(tmp_tm, sunday_first_without_iso);

    if (sunday_first_without_iso) {
        int tmp = ((weekday_b == 0) ? 0 : DAYS_PER_WEEK) - weekday_b + (weeknumber - 1) * DAYS_PER_WEEK + weekday % DAYS_PER_WEEK;
        days += tmp;
        date += tmp;
    } else {
        int tmp = ((weekday_b <= (FOUR_DAYS_IN_YEAR - 1)) ? 0 : DAYS_PER_WEEK) - weekday_b + (weeknumber - 1) * DAYS_PER_WEEK + (weekday - 1);
        days += tmp;
        date += tmp;
    }

    if (days <= 0 || days > MAX_NUMBER_OF_DAY)
        return false;
    // get the final date
    j2date(date, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
    return true;
}

/**
 * validate the result for str_to_date
 * return true if normal
 * return false if out of range
*/
static inline bool final_range_check(int return_type, struct pg_tm *tm, fsec_t *fsec)
{
    int dterr = 0;
    bool no_zero_date_set = SQL_MODE_NO_ZERO_DATE();
    if (return_type == DTK_DATE || return_type == DTK_DATE_TIME) {
        if (!tm->tm_year && !tm->tm_mon && !tm->tm_mday) {  // case of '0000-00-00 xxx'
            if (no_zero_date_set)
                return false;
        } else {
            dterr = ValidateDateForBDatabase(false, tm);
            if (dterr)
                return false;
        }
        if (return_type == DTK_DATE_TIME) {
            dterr =  ValidateTimeForBDatabase(true, tm, fsec);
            if (dterr)
                return false;
        }
    } else if (return_type == DTK_TIME) {
        if (no_zero_date_set)
            return false;
        dterr = ValidateTimeForBDatabase(true, tm, fsec);
        if (dterr)
            return false;
    }
    return true;
}

// construct text result for str_to_date
static inline Datum make_text_result(int return_type ,struct pg_tm *tm, fsec_t fsec, char *buf)
{
    if (return_type == DTK_DATE) {
        EncodeDateOnlyForBDatabase(tm, u_sess->time_cxt.DateStyle, buf);
    } else if (return_type == DTK_DATE_TIME) {
        EncodeDateTimeForBDatabase(tm, fsec, false, 0, NULL, u_sess->time_cxt.DateStyle, buf);
    } else if (return_type == DTK_TIME) {
        // time can not be negtive here, so we just encode the tm and fsec
        EncodeTimeOnly(tm, fsec, false, 0, u_sess->time_cxt.DateStyle, buf);
    }
    return CStringGetTextDatum(buf);
}

/**
 * compatibility of str_to_date
*/
Datum str_to_date(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    char *str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    if (PG_ARGISNULL(1)) {
        ereport(errlevel, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
            errmsg("Incorrect datetime value: \'%s\' for function str_to_date", str)));
        PG_RETURN_NULL();
    }
    char *format = text_to_cstring(PG_GETARG_TEXT_PP(1));
    int return_type = find_return_type(format);
    const char *type_name = find_type_name(return_type);
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec = 0;
    int fsec_int = 0;
    errno_t rc = memset_s(tm, sizeof(*tm), 0, sizeof(*tm));
    securec_check(rc, "\0", "\0");

    int str_len = strlen(str);
    int format_len = strlen(format);
    char *str_begin = str, *str_end = str + str_len;
    char *format_ptr = format, *format_end = format + format_len;
    
    MyLocale *locale = NULL;
    bool time_in12 = false;
    int frac_part = 0;
    int daypart = 0;
    int weekday = 0;
    int yearday = 0;
    bool sunday_first_without_iso = false;
    bool strict_week_range = false;
    int weeknumber = -1;
    bool is_strict_week_year = false;
    int  strict_week_year = -1;
    Datum result;
    char buf[MAXDATELEN + 1];

    for (; format_ptr != format_end && str != str_end; ++format_ptr) {
        while (isspace((unsigned char)(*str)) && str < str_end)
            ++str;
        if (str >= str_end)
            break;
        
        if (*format_ptr == '%' && (format_ptr + 1) != format_end) {
            int tmp_len = 0, true_len = 0;
            int val_len = (int)(str_end - str);
            ++format_ptr;
            int error = 0;
            switch (*format_ptr) {
            /* year */
                case 'Y':
                    tmp_len = (FOUR_DIGIT_LEN < val_len ? FOUR_DIGIT_LEN : val_len);
                    tm->tm_year = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    if (true_len <= TWO_DIGIT_LEN)
                        tm->tm_year = handling_2000_year(tm->tm_year);
                    str += true_len;
                    break;
                case 'y':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_year = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    tm->tm_year = handling_2000_year(tm->tm_year);
                    str += true_len;
                    break;

                /* month */
                case 'm':
                case 'c':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_mon = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                case 'M':
                    if (locale == NULL) {
                        locale = MyLocaleSearch("en_US");
                    }
                    if ((tm->tm_mon = (find_index_with_name(locale->month_names, MONTHS_PER_YEAR, str, str_end, &str) + 1)) <= 0)
                        goto err;
                    break;
                case 'b':
                    if (locale == NULL) {
                        locale = MyLocaleSearch("en_US");
                    }
                    if ((tm->tm_mon = (find_index_with_name(locale->ab_month_names, MONTHS_PER_YEAR, str, str_end, &str) + 1)) <= 0)
                        goto err;
                    break;

                /* day */
                case 'd':
                case 'e':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_mday = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                case 'D':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_mday = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    tmp_len = (int)(str_end - (str + true_len));
                    str = (str + true_len) + (TWO_DIGIT_LEN < tmp_len ? TWO_DIGIT_LEN : tmp_len);
                    break;

                /* hour */
                case 'h':
                case 'I':
                case 'l':
                    time_in12 = true;
                case 'k':
                case 'H':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_hour = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                
                /* minute */
                case 'i':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_min = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                
                /* second */
                case 's':
                case 'S':
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    tm->tm_sec = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                
                /* fractional second */
                case 'f':
                    tmp_len = (int)(str_end - str);
                    if (tmp_len > TIMESTAMP_MAX_PRECISION)
                        tmp_len = TIMESTAMP_MAX_PRECISION;
                    fsec_int = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    frac_part = TIMESTAMP_MAX_PRECISION - true_len;
                    if (frac_part > 0)
                        fsec_int *= (int)pow_of_10[frac_part];
#ifdef HAVE_INT64_TIMESTAMP
                    fsec = fsec_int;
#else
                    fsec = (double)fsec_int / 1000000;
#endif
                    str += true_len;
                    break;

                /* am / pm */
                case 'p':
                    if (val_len < TWO_DIGIT_LEN || ! time_in12)
                        goto err;
                    if (ncmp_case_insensitive(str, "PM", 2))
                        daypart = HOURS_HALF_DAY;
                    else if (!ncmp_case_insensitive(str, "AM", 2))
                        goto err;
                    str += TWO_DIGIT_LEN;
                    break;
                
                /* something else */
                case 'W':
                    if (locale == NULL) {
                        locale = MyLocaleSearch("en_US");
                    }
                    if ((weekday = (find_index_with_name(locale->day_names, DAYS_PER_WEEK, str, str_end, &str) + 1)) <= 0)
                        goto err;
                    break;
                case 'a':
                    if (locale == NULL) {
                        locale = MyLocaleSearch("en_US");
                    }
                    if ((weekday = (find_index_with_name(locale->ab_day_names, DAYS_PER_WEEK, str, str_end, &str) + 1)) <= 0)
                        goto err;
                    break;
                case 'w':
                    tmp_len = 1;
                    weekday = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    if (weekday < 0 || weekday >= DAYS_PER_WEEK)
                        goto err;
                    if (!weekday)
                        weekday = DAYS_PER_WEEK;
                    str += true_len;
                    break;
                case 'j':
                    tmp_len = (THREE_DIGIT_LEN < val_len ? THREE_DIGIT_LEN : val_len);
                    yearday = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                
                /* week numbers */
                case 'V':
                case 'U':
                case 'v':
                case 'u':
                    sunday_first_without_iso = (*format_ptr =='U' || *format_ptr == 'V');
                    strict_week_range = (*format_ptr =='V' || *format_ptr =='v');
                    tmp_len = (TWO_DIGIT_LEN < val_len ? TWO_DIGIT_LEN : val_len);
                    if ((weeknumber = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error)) < 0 || 
                        (strict_week_range && !weeknumber) || weeknumber > (WEEKS_PER_YEAR + 1))
                        goto err;
                    str += true_len;
                    break;
                /* year used with 'strict' %V and %v week numbers */
                case 'X':
                case 'x':
                    is_strict_week_year = (*format_ptr == 'X');
                    tmp_len = (FOUR_DIGIT_LEN < val_len ? FOUR_DIGIT_LEN : val_len);
                    strict_week_year = (int)str2ll_with_endptr(str, tmp_len, &true_len, &error);
                    str += true_len;
                    break;
                
                /* time in am/pm notation */
                case 'r':
                    if (!calc_compound_specifiers("%I:%i:%S %p", str, tm, &str))
                        goto err;
                    break;
                
                /* time in 24-hour notation */
                case 'T':
                    if (!calc_compound_specifiers("%H:%i:%S", str, tm, &str))
                        goto err;
                    break;
                
                /* conversion specifiers that match classes of characters */
                case '.':
                    while (str < str_end && ispunct((unsigned char)(*str)))
                        str++;
                    break;
                case '@':
                    while (str < str_end && isalpha((unsigned char)(*str)))
                        str++;
                    break;
                case '#':
                    while (str < str_end && isdigit((unsigned char)(*str)))
                        str++;
                    break;
                default:
                    goto err;
            }
            if (error)
                goto err;
       } else if (!isspace((unsigned char)(*format_ptr))) {
            if (*str != *format_ptr)
                goto err;
            str++;
       }
    }

    if (str != str_end) {
        while (isspace((unsigned char)(*str)) && str != str_end) {
            str++;
        }
        if (str != str_end) {
            ereport(errlevel, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Truncated incorrect %s value: \'%s\'", type_name, str_begin)));
        }
    }

    if (time_in12) {
        if (tm->tm_hour > HOURS_HALF_DAY || tm->tm_hour < 1)
            goto err;
        tm->tm_hour = tm->tm_hour % HOURS_HALF_DAY + daypart;
    }
    
    if (yearday > 0 && !yearday_to_date(tm, yearday))
        goto err;

    if (weeknumber >= 0 && weekday && 
        !week_year_to_date(tm, sunday_first_without_iso, strict_week_range, weeknumber, weekday, is_strict_week_year, strict_week_year))
        goto err;

    // a simple quick range check
    if (tm->tm_mon > MONTHS_PER_YEAR || tm->tm_mday > DAYNUM_BIGMON ||
        tm->tm_hour >= HOURS_PER_DAY || tm->tm_min >= MINS_PER_HOUR || tm->tm_sec >= SECS_PER_MINUTE)
        goto err;
    
    if (return_type == DTK_TIME && tm->tm_mday) {
        tm->tm_hour += tm->tm_mday * HOURS_PER_DAY;
        tm->tm_mday = 0;
    }

    // range check
    if (!final_range_check(return_type, tm, &fsec))
        goto err;

    // make the text result
    result = make_text_result(return_type, tm, fsec, buf);
    PG_RETURN_DATUM(result);

err:
    ereport(errlevel, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
            errmsg("Incorrect %s value: \'%s\' for function str_to_date", type_name, str_begin)));
    PG_RETURN_NULL();
}

static inline Timestamp from_unixtime_internal(lldiv_t *div)
{
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tzp = 0;
    double offset = 0;
    Timestamp datetime;

    if (div->rem == 0) {
        offset = div->quot;
    } else {
        offset = div->quot +
                 llround(div->rem / (double)pow_of_10[TWO_DIGIT_LEN]) / (double)pow_of_10[SIX_DIGIT_LEN];
    }
    Unixtimestamp2tm(offset , tm , &fsec);
    /* find the current session time zone offset. */
    tzp = -DetermineTimeZoneOffset(tm, session_timezone);
    tm2timestamp(tm, fsec, &tzp, &datetime);
    return datetime;
}

Datum from_unixtime_with_one_arg(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    lldiv_t div;
    Timestamp datetime;
    
    Numeric_to_lldiv(num, &div);
    if (div.quot > MAX_UNIXTIMESTAMP_VALUE || div.quot < 0 || div.rem < 0)
        PG_RETURN_NULL();
    datetime = from_unixtime_internal(&div);
    PG_RETURN_TIMESTAMP(datetime);
}

Datum from_unixtime_with_two_arg(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    text *format_text = PG_GETARG_TEXT_PP(1);
    lldiv_t div;
    Timestamp datetime;
    char *tmp = NULL;

    Numeric_to_lldiv(num, &div);
    if (div.quot > MAX_UNIXTIMESTAMP_VALUE || div.quot < 0 || div.rem < 0)
        PG_RETURN_NULL();
    
    datetime = from_unixtime_internal(&div);
    tmp = DatumGetCString(DirectFunctionCall1(timestamp_out, TimestampGetDatum(datetime)));
    PG_RETURN_TEXT_P(DirectFunctionCall2(date_format_text, PointerGetDatum(cstring_to_text(tmp)), PointerGetDatum(format_text)));
}

Datum timestamp_uint8(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    char result_buff[TIMESTAMP_YYYYMMDDhhmmss_LEN + 3] = {0};
    uint64 result = 0;
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0) {
        errno_t ret = EOK;
        ret = sprintf_s(result_buff, TIMESTAMP_YYYYMMDDhhmmss_LEN + 3, "%04d%02d%02d%02d%02d%02d", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        securec_check_ss(ret, "\0", "\0");
        result = (uint64)pg_strtouint64(result_buff, NULL, 10);
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range")));
    }
    PG_RETURN_UINT64(result);
}

Datum datetime_float(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    float8 result = 0;
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0) {
        result = tm->tm_year*1e10 + tm->tm_mon*1e8 + tm->tm_mday*1e6 + tm->tm_hour*1e4 + tm->tm_min*1e2 + tm->tm_sec + fsec/1e6;
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range")));
    }
    PG_RETURN_FLOAT8(result);
}
#endif


#ifdef DOLPHIN
Datum timestamp_xor_transfn(PG_FUNCTION_ARGS)
{
    int128 state;
    int128 ts_int;
    Timestamp timestamp;

    state = PG_ARGISNULL(0) ? 0 : PG_GETARG_INT128(0);

    /* Append the value unless null. */
    if (!PG_ARGISNULL(1)) {
        /* On the first time through, we ignore the delimiter. */
        int tz;
        struct pg_tm tt, *tm = &tt;
        fsec_t fsec;
        const char* tzn = NULL;
        timestamp = PG_GETARG_TIMESTAMP(1);

        if (timestamp2tm(timestamp, &tz, tm, &fsec, &tzn, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        
        ts_int = tm->tm_year*1e10 + tm->tm_mon*1e8 + tm->tm_mday*1e6 + tm->tm_hour*1e4 + tm->tm_min*1e2 + tm->tm_sec;

        state = state ^ ts_int;
    }
    /*
     * The transition type for list_agg() is declared to be "internal",
     * which is a pass-by-value type the same size as a pointer.
     */
    PG_RETURN_INT128(state);
}

Datum timestamp_agg_finalfn(PG_FUNCTION_ARGS)
{
    int128 finalResult;
    /* cannot be called directly because of internal-type argument */
    Assert(AggCheckCallContext(fcinfo, NULL));

    finalResult = PG_ARGISNULL(0) ? 0 : PG_GETARG_INT128(0);

    PG_RETURN_INT128(finalResult);
}

PG_FUNCTION_INFO_V1_PUBLIC(timestampxor);
extern "C" DLL_PUBLIC Datum timestampxor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_int8_xor);
extern "C" DLL_PUBLIC Datum timestamp_int8_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_timestamp_xor);
extern "C" DLL_PUBLIC Datum int8_timestamp_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_float8_xor);
extern "C" DLL_PUBLIC Datum timestamp_float8_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(float8_timestamp_xor);
extern "C" DLL_PUBLIC Datum float8_timestamp_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamptzxor);
extern "C" DLL_PUBLIC Datum timestamptzxor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_int8_xor);
extern "C" DLL_PUBLIC Datum timestamptz_int8_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_timestamptz_xor);
extern "C" DLL_PUBLIC Datum int8_timestamptz_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_float8_xor);
extern "C" DLL_PUBLIC Datum timestamptz_float8_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(float8_timestamptz_xor);
extern "C" DLL_PUBLIC Datum float8_timestamptz_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(text_timestamp_xor);
extern "C" DLL_PUBLIC Datum text_timestamp_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_text_xor);
extern "C" DLL_PUBLIC Datum timestamp_text_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_text_xor);
extern "C" DLL_PUBLIC Datum timestamptz_text_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(text_timestamptz_xor);
extern "C" DLL_PUBLIC Datum text_timestamptz_xor(PG_FUNCTION_ARGS);

static int128 text_int128(Datum textValue)
{
    char* tmp = NULL;
    tmp = DatumGetCString(DirectFunctionCall1(textout, textValue));
    errno = 0;
    char* endptr = NULL;
    int128 temp = strtoll(tmp, &endptr, 10);
    if (errno != 0 || (temp == 0 && tmp == endptr))
        ereport(ERROR,
            (errcode(DTERR_BAD_FORMAT), errmsg("invalid INTERGER: \"%s\"", tmp)));
    pfree(tmp);
    return temp;
}

int128 timestamp_int128(Timestamp timestamp)
{
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL);
    int128 ts_int = timestamp2int(tm);
    return ts_int;
}

int128 timestamptz_int128(TimestampTz timestampTz)
{
    struct pg_tm tt, *tm = &tt;
    int tz;
    fsec_t fsec;
    const char* tzn = NULL;
    timestamp2tm(timestampTz, &tz, tm, &fsec, &tzn, NULL);
    int128 ts_int = timestamp2int(tm);
    return ts_int;

}

Datum timestampxor(PG_FUNCTION_ARGS)
{
    Timestamp timestamp1 = PG_GETARG_TIMESTAMP(0);
    Timestamp timestamp2 = PG_GETARG_TIMESTAMP(1);
    int128 ts_int1 = timestamp_int128(timestamp1);
    int128 ts_int2 = timestamp_int128(timestamp2);
    PG_RETURN_INT128(ts_int1 ^ ts_int2);
}

Datum timestamp_int8_xor(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    int128 ts_int = timestamp_int128(timestamp);
    int64 res = PG_GETARG_INT64(1);
    PG_RETURN_INT128(ts_int ^ res);
}

Datum int8_timestamp_xor(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    int128 ts_int = timestamp_int128(timestamp);
    int64 res = PG_GETARG_INT64(0);
    PG_RETURN_INT128(ts_int ^ res);
}

Datum timestamp_float8_xor(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    int128 ts_int = timestamp_int128(timestamp);
    float8 num = PG_GETARG_FLOAT8(1);
    int32 arg = DatumGetInt32(DirectFunctionCall1(dtoi4, Float8GetDatum(num)));
    PG_RETURN_INT128(ts_int ^ arg);
}

Datum float8_timestamp_xor(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    int128 ts_int = timestamp_int128(timestamp);
    float8 num = PG_GETARG_FLOAT8(0);
    int32 arg = DatumGetInt32(DirectFunctionCall1(dtoi4, Float8GetDatum(num)));
    PG_RETURN_INT128(ts_int ^ arg);
}

Datum timestamptzxor(PG_FUNCTION_ARGS)
{
    TimestampTz timestampTz1 = PG_GETARG_TIMESTAMPTZ(0);
    int128 ts_int1 = timestamptz_int128(timestampTz1);
    TimestampTz timestampTz2 = PG_GETARG_TIMESTAMPTZ(1);
    int128 ts_int2 = timestamptz_int128(timestampTz2);
    PG_RETURN_INT128(ts_int1 ^ ts_int2);
}

Datum timestamptz_int8_xor(PG_FUNCTION_ARGS)
{
    TimestampTz timestampTz = PG_GETARG_TIMESTAMPTZ(0);
    int128 ts_int = timestamptz_int128(timestampTz);
    int64 res = PG_GETARG_INT64(1);
    PG_RETURN_INT128(ts_int ^ res);
}

Datum int8_timestamptz_xor(PG_FUNCTION_ARGS)
{
    TimestampTz timestampTz = PG_GETARG_TIMESTAMPTZ(1);
    int128 ts_int = timestamptz_int128(timestampTz);
    int64 res = PG_GETARG_INT64(0);
    PG_RETURN_INT128(ts_int ^ res);
}

Datum timestamptz_float8_xor(PG_FUNCTION_ARGS)
{
    TimestampTz timestampTz = PG_GETARG_TIMESTAMPTZ(0);
    int128 ts_int = timestamptz_int128(timestampTz);
    float8 num = PG_GETARG_FLOAT8(1);
    int32 arg = DatumGetInt32(DirectFunctionCall1(dtoi4, Float8GetDatum(num)));
    PG_RETURN_INT128(ts_int ^ arg);
}

Datum float8_timestamptz_xor(PG_FUNCTION_ARGS)
{
    TimestampTz timestampTz = PG_GETARG_TIMESTAMPTZ(1);
    int128 ts_int = timestamptz_int128(timestampTz);
    float8 num = PG_GETARG_FLOAT8(0);
    int32 arg = DatumGetInt32(DirectFunctionCall1(dtoi4, Float8GetDatum(num)));
    PG_RETURN_INT128(ts_int ^ arg);
}

Datum timestamp_text_xor(PG_FUNCTION_ARGS)
{
    Datum textValue = PG_GETARG_DATUM(1);
    int128 temp = text_int128(textValue);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    int128 ts_int = timestamp_int128(timestamp);
    PG_RETURN_INT128(ts_int ^ temp);
}

Datum text_timestamp_xor(PG_FUNCTION_ARGS)
{
    Datum textValue = PG_GETARG_DATUM(0);
    int128 temp = text_int128(textValue);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    int128 ts_int = timestamp_int128(timestamp);
    PG_RETURN_INT128(ts_int ^ temp);
}

Datum timestamptz_text_xor(PG_FUNCTION_ARGS)
{
    Datum textValue = PG_GETARG_DATUM(1);
    int128 temp = text_int128(textValue);
    TimestampTz timestampTz = PG_GETARG_TIMESTAMPTZ(0);
    int128 ts_int = timestamptz_int128(timestampTz);
    PG_RETURN_INT128(ts_int ^ temp);
}

Datum text_timestamptz_xor(PG_FUNCTION_ARGS)
{
    Datum textValue = PG_GETARG_DATUM(0);
    int128 temp = text_int128(textValue);
    TimestampTz timestampTz = PG_GETARG_TIMESTAMPTZ(1);
    int128 ts_int = timestamptz_int128(timestampTz);
    PG_RETURN_INT128(ts_int ^ temp);
}

/* cast timestamp to numeric */
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_numeric);
extern "C" DLL_PUBLIC Datum timestamp_numeric(PG_FUNCTION_ARGS);
Datum timestamp_numeric(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    Numeric res = NULL;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    int64 quot = timestamp2int(tm);
    char *str = AppendFsec(quot, fsec);
    res =
        DatumGetNumeric(DirectFunctionCall3(numeric_in, CStringGetDatum(str), ObjectIdGetDatum(0), Int32GetDatum(-1)));

    PG_RETURN_NUMERIC(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(timestamp_any_value);
extern "C" DLL_PUBLIC Datum timestamp_any_value(PG_FUNCTION_ARGS);
Datum timestamp_any_value(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    PG_RETURN_TIMESTAMP(timestamp);
}

/* cast timestamptz to int8 */
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_int8);
extern "C" DLL_PUBLIC Datum timestamptz_int8(PG_FUNCTION_ARGS);
Datum timestamptz_int8(PG_FUNCTION_ARGS)
{
    TimestampTz dt = PG_GETARG_TIMESTAMPTZ(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    const char *tzn = NULL;
    if (timestamp2tm(dt, &tz, tm, &fsec, &tzn, NULL) != 0) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    PG_RETURN_INT64(timestamp2int(tm));
}

/* cast timestamptz to float8 */
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_float8);
extern "C" DLL_PUBLIC Datum timestamptz_float8(PG_FUNCTION_ARGS);
Datum timestamptz_float8(PG_FUNCTION_ARGS)
{
    TimestampTz dt = PG_GETARG_TIMESTAMPTZ(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    const char *tzn = NULL;
    if (timestamp2tm(dt, &tz, tm, &fsec, &tzn, NULL) != 0) {
         ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }

    double res =  timestamp2int(tm) + fsec / 1e6;
    PG_RETURN_FLOAT8(res);
}

/* cast timestamptz to numeric */
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_numeric);
extern "C" DLL_PUBLIC Datum timestamptz_numeric(PG_FUNCTION_ARGS);
Datum timestamptz_numeric(PG_FUNCTION_ARGS)
{
    TimestampTz dt = PG_GETARG_TIMESTAMPTZ(0);
    Numeric res = NULL;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    const char *tzn = NULL;
    if (timestamp2tm(dt, &tz, tm, &fsec, &tzn, NULL) != 0) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    int64 quot = timestamp2int(tm);
    char *str = AppendFsec(quot, fsec);
    res =
        DatumGetNumeric(DirectFunctionCall3(numeric_in, CStringGetDatum(str), ObjectIdGetDatum(0), Int32GetDatum(-1)));

    PG_RETURN_NUMERIC(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_any_value);
extern "C" DLL_PUBLIC Datum timestamptz_any_value(PG_FUNCTION_ARGS);
Datum timestamptz_any_value(PG_FUNCTION_ARGS)
{
    TimestampTz dt = PG_GETARG_TIMESTAMPTZ(0);
    PG_RETURN_TIMESTAMPTZ(dt);
}
#endif
