/* -------------------------------------------------------------------------
 *
 * date.c
 *	  implements DATE and TIME data types specified in SQL-92 standard
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994-5, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/date.c
 *
 * -------------------------------------------------------------------------
 */

#include "postgres.h"
#include "knl/knl_variable.h"

#include <limits.h>
#include <float.h>
#include <ctype.h>

#include "access/hash.h"
#include "commands/copy.h"
#include "libpq/pqformat.h"
#include "miscadmin.h"
#include "plugin_parser/scansup.h"
#include "utils/array.h"
#include "utils/numeric.h"
#include "utils/builtins.h"
#include "plugin_utils/date.h"
#include "plugin_utils/year.h"
#include "plugin_utils/datetime.h"
#include "utils/formatting.h"
#include "utils/nabstime.h"
#include "utils/sortsupport.h"
#ifdef DOLPHIN
#include "plugin_commands/mysqlmode.h"
#include "plugin_utils/int8.h"
#endif

/*
 * gcc's -ffast-math switch breaks routines that expect exact results from
 * expressions like timeval / SECS_PER_HOUR, where timeval is double.
 */
#ifdef __FAST_MATH__
#error -ffast-math is known to break this code
#endif

/*
 * all stuffs used for bulkload(end).
 */

static void EncodeSpecialDate(DateADT dt, char* str, int strlen);
#ifndef DOLPHIN
static int tm2time(struct pg_tm* tm, fsec_t fsec, TimeADT* result);
static int time2tm(TimeADT time, struct pg_tm* tm, fsec_t* fsec);
static int timetz2tm(TimeTzADT* time, struct pg_tm* tm, fsec_t* fsec, int* tzp);
#endif
static int tm2timetz(struct pg_tm* tm, fsec_t fsec, int tz, TimeTzADT* result);
static void AdjustTimeForTypmod(TimeADT* time, int32 typmod);
static int getStartingDigits(char* str);

#ifdef DOLPHIN
bool check_pg_tm_time_part(pg_tm *tm, fsec_t fsec);
extern const char* extract_numericstr(const char* str);
extern "C" DLL_PUBLIC Datum uint8out(PG_FUNCTION_ARGS);
static char* adjust_b_format_time(char *str, int *timeSign, int *D, bool *hasD);
int DatetimeDate(char *str, pg_tm *tm, int time_cast_type);
static int64 getPartFromTm(pg_tm* tm, fsec_t fsec, int part);

PG_FUNCTION_INFO_V1_PUBLIC(int8_b_format_time);
extern "C" DLL_PUBLIC Datum int8_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int16_b_format_time);
extern "C" DLL_PUBLIC Datum int16_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_b_format_time);
extern "C" DLL_PUBLIC Datum int32_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_b_format_time);
extern "C" DLL_PUBLIC Datum int64_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_b_format_time);
extern "C" DLL_PUBLIC Datum uint8_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint16_b_format_time);
extern "C" DLL_PUBLIC Datum uint16_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint32_b_format_time);
extern "C" DLL_PUBLIC Datum uint32_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint64_b_format_time);
extern "C" DLL_PUBLIC Datum uint64_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_b_format_time);
extern "C" DLL_PUBLIC Datum numeric_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(float8_b_format_time);
extern "C" DLL_PUBLIC Datum float8_b_format_time(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(bool_cast_time);
extern "C" DLL_PUBLIC Datum bool_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_cast_time);
extern "C" DLL_PUBLIC Datum int8_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int16_cast_time);
extern "C" DLL_PUBLIC Datum int16_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_cast_time);
extern "C" DLL_PUBLIC Datum int32_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_cast_time);
extern "C" DLL_PUBLIC Datum int64_cast_time(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(uint8_cast_time);
extern "C" DLL_PUBLIC Datum uint8_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint16_cast_time);
extern "C" DLL_PUBLIC Datum uint16_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint32_cast_time);
extern "C" DLL_PUBLIC Datum uint32_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint64_cast_time);
extern "C" DLL_PUBLIC Datum uint64_cast_time(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(float4_cast_time);
extern "C" DLL_PUBLIC Datum float4_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(float8_cast_time);
extern "C" DLL_PUBLIC Datum float8_cast_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_cast_time);
extern "C" DLL_PUBLIC Datum numeric_cast_time(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(bool_cast_date);
extern "C" DLL_PUBLIC Datum bool_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_cast_date);
extern "C" DLL_PUBLIC Datum int8_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int16_cast_date);
extern "C" DLL_PUBLIC Datum int16_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_cast_date);
extern "C" DLL_PUBLIC Datum int32_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_cast_date);
extern "C" DLL_PUBLIC Datum int64_cast_date(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(uint8_cast_date);
extern "C" DLL_PUBLIC Datum uint8_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint16_cast_date);
extern "C" DLL_PUBLIC Datum uint16_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint32_cast_date);
extern "C" DLL_PUBLIC Datum uint32_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uint64_cast_date);
extern "C" DLL_PUBLIC Datum uint64_cast_date(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(float4_cast_date);
extern "C" DLL_PUBLIC Datum float4_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(float8_cast_date);
extern "C" DLL_PUBLIC Datum float8_cast_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_cast_date);
extern "C" DLL_PUBLIC Datum numeric_cast_date(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(bool_b_format_date);
extern "C" DLL_PUBLIC Datum bool_b_format_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_b_format_date);
extern "C" DLL_PUBLIC Datum int8_b_format_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int16_b_format_date);
extern "C" DLL_PUBLIC Datum int16_b_format_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_b_format_date);
extern "C" DLL_PUBLIC Datum int32_b_format_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_b_format_date);
extern "C" DLL_PUBLIC Datum int64_b_format_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(negetive_time);
extern "C" DLL_PUBLIC Datum negetive_time(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(curdate);
extern "C" DLL_PUBLIC Datum curdate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_db_statement_start_time);
extern "C" DLL_PUBLIC Datum b_db_statement_start_time(PG_FUNCTION_ARGS);

/* b compatibility time function */
PG_FUNCTION_INFO_V1_PUBLIC(makedate);
extern "C" DLL_PUBLIC Datum makedate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(maketime);
extern "C" DLL_PUBLIC Datum maketime(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(sec_to_time);
extern "C" DLL_PUBLIC Datum sec_to_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(sec_to_time_str);
extern "C" DLL_PUBLIC Datum sec_to_time_str(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(subdate_datetime_days_text);
extern "C" DLL_PUBLIC Datum subdate_datetime_days_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(subdate_datetime_interval_text);
extern "C" DLL_PUBLIC Datum subdate_datetime_interval_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(subdate_time_days);
extern "C" DLL_PUBLIC Datum subdate_time_days(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(subdate_time_interval);
extern "C" DLL_PUBLIC Datum subdate_time_interval(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_mysql);
extern "C" DLL_PUBLIC Datum time_mysql(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(to_days);
extern "C" DLL_PUBLIC Datum to_days(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(utc_date_func);
extern "C" DLL_PUBLIC Datum utc_date_func(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(utc_time_func);
extern "C" DLL_PUBLIC Datum utc_time_func(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(time_to_sec);
extern "C" DLL_PUBLIC Datum time_to_sec(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int64_time_to_sec);
extern "C" DLL_PUBLIC Datum int64_time_to_sec(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_time_to_sec);
extern "C" DLL_PUBLIC Datum numeric_time_to_sec(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_time_to_sec);
extern "C" DLL_PUBLIC Datum timestamp_time_to_sec(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamptz_time_to_sec);
extern "C" DLL_PUBLIC Datum timestamptz_time_to_sec(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datediff_t_t);
extern "C" DLL_PUBLIC Datum datediff_t_t(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datediff_t_n);
extern "C" DLL_PUBLIC Datum datediff_t_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datediff_n_n);
extern "C" DLL_PUBLIC Datum datediff_n_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datediff_time_t);
extern "C" DLL_PUBLIC Datum datediff_time_t(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datediff_time_n);
extern "C" DLL_PUBLIC Datum datediff_time_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(datediff_time_time);
extern "C" DLL_PUBLIC Datum datediff_time_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(from_days_text);
extern "C" DLL_PUBLIC Datum from_days_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(from_days_numeric);
extern "C" DLL_PUBLIC Datum from_days_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_days_t);
extern "C" DLL_PUBLIC Datum adddate_datetime_days_t(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_days_n);
extern "C" DLL_PUBLIC Datum adddate_datetime_days_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_interval_t);
extern "C" DLL_PUBLIC Datum adddate_datetime_interval_t(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_interval_n);
extern "C" DLL_PUBLIC Datum adddate_datetime_interval_n(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_time_days);
extern "C" DLL_PUBLIC Datum adddate_time_days(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_time_interval);
extern "C" DLL_PUBLIC Datum adddate_time_interval(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetHour);
extern "C" DLL_PUBLIC Datum GetHour(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMicrosecond);
extern "C" DLL_PUBLIC Datum GetMicrosecond(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMinute);
extern "C" DLL_PUBLIC Datum GetMinute(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetSecond);
extern "C" DLL_PUBLIC Datum GetSecond(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetHourFromDate);
extern "C" DLL_PUBLIC Datum GetHourFromDate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMicrosecondFromDate);
extern "C" DLL_PUBLIC Datum GetMicrosecondFromDate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMinuteFromDate);
extern "C" DLL_PUBLIC Datum GetMinuteFromDate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetSecondFromDate);
extern "C" DLL_PUBLIC Datum GetSecondFromDate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetHourFromTimeTz);
extern "C" DLL_PUBLIC Datum GetHourFromTimeTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMicrosecondFromTimeTz);
extern "C" DLL_PUBLIC Datum GetMicrosecondFromTimeTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMinuteFromTimeTz);
extern "C" DLL_PUBLIC Datum GetMinuteFromTimeTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetSecondFromTimeTz);
extern "C" DLL_PUBLIC Datum GetSecondFromTimeTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetHourFromTimestampTz);
extern "C" DLL_PUBLIC Datum GetHourFromTimestampTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMicrosecondFromTimestampTz);
extern "C" DLL_PUBLIC Datum GetMicrosecondFromTimestampTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetMinuteFromTimestampTz);
extern "C" DLL_PUBLIC Datum GetMinuteFromTimestampTz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(GetSecondFromTimestampTz);
extern "C" DLL_PUBLIC Datum GetSecondFromTimestampTz(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(get_format);
extern "C" DLL_PUBLIC Datum get_format(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_extract_text);
extern "C" DLL_PUBLIC Datum b_extract_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_extract_numeric);
extern "C" DLL_PUBLIC Datum b_extract_numeric(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(time_xor_transfn);
extern "C" DLL_PUBLIC Datum time_xor_transfn(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timetz_xor_transfn);
extern "C" DLL_PUBLIC Datum timetz_xor_transfn(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(date_xor_transfn);
extern "C" DLL_PUBLIC Datum date_xor_transfn(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(date_agg_finalfn);
extern "C" DLL_PUBLIC Datum date_agg_finalfn(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(time_float);
extern "C" DLL_PUBLIC Datum time_float(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_int);
extern "C" DLL_PUBLIC Datum date_int(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_cast);
extern "C" DLL_PUBLIC Datum date_cast(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(text_date_explicit);
extern "C" DLL_PUBLIC Datum text_date_explicit(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(time_cast);
extern "C" DLL_PUBLIC Datum time_cast(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(time_cast_implicit);
extern "C" DLL_PUBLIC Datum time_cast_implicit(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(text_time_explicit);
extern "C" DLL_PUBLIC Datum text_time_explicit(PG_FUNCTION_ARGS);
#endif
/* common code for timetypmodin and timetztypmodin */
static int32 anytime_typmodin(bool istz, ArrayType* ta)
{
    int32 typmod;
    int32* tl = NULL;
    int n;

    tl = ArrayGetIntegerTypmods(ta, &n);

    /*
     * we're not too tense about good error message here because grammar
     * shouldn't allow wrong number of modifiers for TIME
     */
    if (n != 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid type modifier")));

    if (*tl < 0)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("TIME(%d)%s precision must not be negative", *tl, (istz ? " WITH TIME ZONE" : ""))));
    if (*tl > MAX_TIME_PRECISION) {
        ereport(WARNING,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("TIME(%d)%s precision reduced to maximum allowed, %d",
                    *tl,
                    (istz ? " WITH TIME ZONE" : ""),
                    MAX_TIME_PRECISION)));
        typmod = MAX_TIME_PRECISION;
    } else
        typmod = *tl;

    return typmod;
}

/* common code for timetypmodout and timetztypmodout */
static char* anytime_typmodout(bool istz, int32 typmod)
{
    const int buflen = 64;
    char* res = (char*)palloc(buflen);
    const char* tz = istz ? " with time zone" : " without time zone";
    errno_t rc = EOK;

    if (typmod >= 0) {
        rc = snprintf_s(res, buflen, buflen - 1, "(%d)%s", (int)typmod, tz);
    } else {
        rc = snprintf_s(res, buflen, buflen - 1, "%s", tz);
    }
    securec_check_ss(rc, "", "");
    return res;
}

/*
 * Get starting digits of input string and return as int
 *
 * If the first character is not digit, return -1. NOTICE that if the first character is '+' or '-',
 * it will consider it as invalid digit. So handle starting '+' nad '-' before using this function.
 */
static int getStartingDigits(char* str)
{
    int digitnum = 0;
    long trunc_val = 0;
    while (isdigit((unsigned char)*str)) {
        trunc_val = trunc_val * 10 + (*str++ - '0');
        digitnum++;
        if (trunc_val > PG_INT32_MAX) {
            return PG_INT32_MAX;
        }
    }
    return digitnum == 0 ? -1 : trunc_val;
}

/*****************************************************************************
 *	 Date ADT
 *****************************************************************************/

/* DateTypeCheck()
 * Check date format, and convert to internal date format.
 */
static bool DateTypeCheck(char* str, bool can_ignore, struct pg_tm* tm, DateADT &date, fsec_t &fsec, int &dterr)
{
    int tzp;
    int dtype = DTK_NUMBER;
    int nf;
    char* field[MAXDATEFIELDS];
    int ftype[MAXDATEFIELDS];
    char workbuf[MAXDATELEN + 1];
    /*
     * default pg date formatting parsing.
     */
    dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
    if (dterr == 0)
        dterr = DecodeDateTime(field, ftype, nf, &dtype, tm, &fsec, &tzp);
    if (dterr != 0) {
        DateTimeParseError(dterr, str, "date", can_ignore);
        /*
         * if reporting warning in DateTimeParseError, return 1970-01-01
         */
        date = UNIX_EPOCH_JDATE - POSTGRES_EPOCH_JDATE;
        return true;
    }

    switch (dtype) {
        case DTK_DATE:
            break;

        case DTK_CURRENT:
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("date/time value \"current\" is no longer supported")));
            GetCurrentDateTime(tm);
            break;

        case DTK_EPOCH:
            GetEpochTime(tm);
            break;

        case DTK_LATE:
            DATE_NOEND(date);
            return true;

        case DTK_EARLY:
            DATE_NOBEGIN(date);
            return true;

        default:
            DateTimeParseError(DTERR_BAD_FORMAT, str, "date");
            break;
    }
    return false;
}
#ifdef DOLPHIN
/* curdate()
 * @reruen  current date in b compatibility,    date
 */
Datum curdate(PG_FUNCTION_ARGS)
{
    TimestampTz state_start_timestamp;
    Timestamp result;
    double timestamp_guc = 0.0;
    struct pg_tm tt, *tm = &tt;
    DateADT date;
    fsec_t fsec;
    int tz;

    timestamp_guc = GetSessionContext()->b_db_timestamp;

    if (timestamp_guc == DEFAULT_GUC_B_DB_TIMESTAMP) {

        state_start_timestamp = GetCurrentStatementStartTimestamp();
        if (timestamp2tm(state_start_timestamp, &tz, tm, &fsec, NULL, NULL) != 0) {
            ereport(ERROR, (
                errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), 
                    errmsg("date out of range")));
        }

    } else {

        /* get the pg_tm struct of b_db_timestamp based on 1970-01-01 00:00:00 UTC, at time zone 0. */
        Unixtimestamp2tm(timestamp_guc, tm, &fsec);

        /* find the current session time zone offset. */
        tz = -DetermineTimeZoneOffset(tm, session_timezone);

        /* Convert the tm into timestamp in current timezone.
            Because date may change after adding the timezone offset. 
        */
        tm2timestamp(tm, fsec, &tz, &result);

        if (timestamp2tm(result, NULL, tm, &fsec, NULL, NULL) != 0) {
            ereport(ERROR, (
                errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), 
                    errmsg("date out of range")));
        }
        
    }

    date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;

    PG_RETURN_DATEADT(date);
}

/* b_db_statement_start_time()
 * @param1 precision,                          int32
 * @reruen current time in b compatibility,    time 
 */
Datum b_db_statement_start_time(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    TimestampTz state_start_timestamp;
    Timestamp timestamp_val;
    TimeADT time_val;
    double timestamp_guc = DEFAULT_GUC_B_DB_TIMESTAMP;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz = 0;

    /*lexical analyzer can only recognize positive integers for token ICONST in gram.y*/
    if (typmod > TIMESTAMP_MAX_PRECISION) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Too-big precision %d specified for \'curtime\'. Maximum is 6.", typmod)));
    }


    timestamp_guc = GetSessionContext()->b_db_timestamp;
    
    if (timestamp_guc == DEFAULT_GUC_B_DB_TIMESTAMP) {

        state_start_timestamp = GetCurrentStatementStartTimestamp();
        time_val = (TimeADT)DirectFunctionCall1(timestamptz_time, state_start_timestamp);

    } else {
        
        /* get the pg_tm struct of b_db_timestamp based on 1970-01-01 00:00:00 UTC, at time zone 0. */
        Unixtimestamp2tm(timestamp_guc, tm, &fsec);

        /* find the current session time zone offset. */
        tz = -DetermineTimeZoneOffset(tm, session_timezone);

        /* Convert the tm into timestamp in current timezone.
            Because date may change after adding the timezone offset. 
        */
        tm2timestamp(tm, fsec, &tz, &timestamp_val);

        if (timestamp2tm(timestamp_val, NULL, tm, &fsec, NULL, NULL) != 0) {
            ereport(ERROR, (
                errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), 
                    errmsg("date or time out of range")));
        }

        tm2time(tm, fsec, &time_val);
    }

    AdjustTimeForTypmod(&time_val, typmod);

    PG_RETURN_TIMEADT(time_val);
}
#endif
/*****************************************************************************
 *	 Date ADT
 *****************************************************************************/

/* date_in()
 * Given date text string, convert to internal date format.
 */
Datum date_in(PG_FUNCTION_ARGS)
#ifdef DOLPHIN
{
    Datum result;
    TimeErrorType time_error_type = TIME_CORRECT;
    char* str = PG_GETARG_CSTRING(0);
    result = date_internal(fcinfo, str, TIME_IN, &time_error_type);
    if ((fcinfo->ccontext == COERCION_IMPLICIT || fcinfo->ccontext == COERCION_EXPLICIT) &&
        time_error_type == TIME_INCORRECT) {
        PG_RETURN_NULL();
    }
    return result;
}

Datum date_cast(PG_FUNCTION_ARGS)
{
    TimeErrorType time_error_type = TIME_CORRECT;
    char* str = PG_GETARG_CSTRING(0);
    return date_internal(fcinfo, str, TIME_CAST, &time_error_type);
}

Datum text_date_explicit(PG_FUNCTION_ARGS)
{
    Datum result;
    TimeErrorType time_error_type = TIME_CORRECT;
    char* str = fcinfo->argTypes[0] ?
                      parser_function_input(PG_GETARG_DATUM(0), fcinfo->argTypes[0]) :
                      PG_GETARG_CSTRING(0);
    result = date_internal(fcinfo, str, TEXT_TIME_EXPLICIT, &time_error_type);
    if (time_error_type == TIME_INCORRECT) {
        PG_RETURN_NULL();
    }
    return result;
}

Datum date_internal(PG_FUNCTION_ARGS, char* str, int time_cast_type, TimeErrorType* time_error_type)
#endif
{
#ifndef DOLPHIN
    char* str = PG_GETARG_CSTRING(0);
#endif
    DateADT date;
    int dterr;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tzp;
    int dtype = DTK_NUMBER;
    int nf;
    char* field[MAXDATEFIELDS];
    int ftype[MAXDATEFIELDS];
    char workbuf[MAXDATELEN + 1];
    char* date_fmt = NULL;
#ifdef DOLPHIN
    errno_t rc = EOK;
    rc = memset_s(&tt, sizeof(tt), 0, sizeof(tt));
    securec_check(rc, "\0", "\0");
#endif
    /*
     * this case is used for date format is specified.
     */
    if (4 == PG_NARGS()) {
        date_fmt = PG_GETARG_CSTRING(3);
        if (NULL == date_fmt) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT), errmsg("specified date format is null")));
        }

        /* the following logic shared from to_date(). */
        to_timestamp_from_format(tm, &fsec, str, (void*)date_fmt);
#ifndef DOLPHIN
    } else if (DateTypeCheck(str, fcinfo->can_ignore, tm, date, fsec, dterr)) {
        PG_RETURN_DATEADT(date);
    }

    /*
     * the following logic is unified for date parsing.
     */
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range: \"%s\"", str)));
#else
    } else {
        int invalid_tz;
        bool res = cstring_to_tm(str, tm, fsec, &tzp, &invalid_tz);
        if (!res) {
            /*
             * default pg date formatting parsing.
             */
            dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
            if (dterr != 0) {
                DateTimeParseErrorWithFlag(dterr, str, "date", fcinfo->can_ignore, time_cast_type == TIME_CAST);
                /*
                 * if reporting warning in DateTimeParseError, return 1970-01-01
                 */
                *time_error_type = TIME_INCORRECT;
                PG_RETURN_DATEADT(DATE_ALL_ZERO_VALUE);
            }
            if (dterr == 0) {
                if (ftype[0] == DTK_NUMBER && nf == 1) {
                    dterr = DatetimeDate(field[0], tm, time_cast_type);
                    dtype = DTK_DATE;
                } else {
                    dterr = DecodeDateTimeForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tzp);
                }
            }
            if (dterr != 0) {
                DateTimeParseErrorWithFlag(dterr, str, "date", fcinfo->can_ignore, time_cast_type == TIME_CAST);
                *time_error_type = TIME_INCORRECT;
                PG_RETURN_DATEADT(DATE_ALL_ZERO_VALUE);
            }
            switch (dtype) {
                case DTK_DATE:
                    break;

                case DTK_CURRENT:
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("date/time value \"current\" is no longer supported")));

                    GetCurrentDateTime(tm);
                    break;

                case DTK_EPOCH:
                    GetEpochTime(tm);
                    break;

                case DTK_LATE:
                    DATE_NOEND(date);
                    PG_RETURN_DATEADT(date);

                case DTK_EARLY:
                    DATE_NOBEGIN(date);
                    PG_RETURN_DATEADT(date);

                default:
                    DateTimeParseError(DTERR_BAD_FORMAT, str, "date");
                    break;
            }
        }
    }

    /*
     * the following logic is unified for date parsing.
     */
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday)) {
        int level = fcinfo->can_ignore || !SQL_MODE_STRICT() ? WARNING : ERROR;
        ereport(level, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range: \"%s\"", str)));
        *time_error_type = TIME_INCORRECT;
        PG_RETURN_DATEADT(DATE_ALL_ZERO_VALUE);
    }
#endif

    date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;

    PG_RETURN_DATEADT(date);
}

Datum input_date_in(char* str, bool can_ignore)
{
    if (str == NULL) {
        return (Datum)0;
    }
    DateADT date;
    int dterr;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (u_sess->attr.attr_common.enable_iud_fusion) {
        dterr = ParseIudDateOnly(str, tm);
        if (dterr == 0) {
            if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday)) {
                ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range: \"%s\"", str)));
            }
            date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;

            PG_RETURN_DATEADT(date);
        }
        
    }

    if (DateTypeCheck(str, can_ignore, tm, date, fsec, dterr)) {
        PG_RETURN_DATEADT(date);
    }
    /*
     * the following logic is unified for date parsing.
     */
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range: \"%s\"", str)));

    date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;

    PG_RETURN_DATEADT(date);
}

#ifdef DOLPHIN
extern "C" DLL_PUBLIC Datum timestamp_cast(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum timestamp_explicit(PG_FUNCTION_ARGS);

int DatetimeDate(char *str, pg_tm *tm, int time_cast_type)
{
    fsec_t fsec;
    Datum datetime;
    bool isRetNull = false;
    switch (time_cast_type) {
        case TIME_CAST:
            datetime = DirectFunctionCall3(timestamp_cast, CStringGetDatum(str),
                ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
            break;

        case TEXT_TIME_EXPLICIT:
            datetime = DirectCall3(&isRetNull, timestamp_explicit, InvalidOid, CStringGetDatum(str),
                ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
            if (isRetNull) {
                return ERRCODE_DATETIME_VALUE_OUT_OF_RANGE;
            }
            break;

        default:
            datetime = DirectFunctionCall3(timestamp_in, CStringGetDatum(str),
                ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
            break;
    }

    if (timestamp2tm(datetime, NULL, tm, &fsec, NULL, NULL) != 0) {
        return ERRCODE_DATETIME_VALUE_OUT_OF_RANGE;
    }
    return 0;
}

int NumberDate(char *str, pg_tm *tm, unsigned int date_flag) 
{
    int len = 0;
    char *cp = str;
    errno_t rc = EOK;
    /* validate number str */
    while (*cp != '\0') {
        if (!isdigit((unsigned char) *cp)) {
            return ERRCODE_INVALID_DATETIME_FORMAT;
        }
        ++len;
        ++cp;
    }
    if (len > DATE_YYYYMMDD_LEN || len < B_FORMAT_DATE_NUMBER_MIN_LEN) {
        return ERRCODE_DATETIME_VALUE_OUT_OF_RANGE;
    }
    char adjusted[DATE_YYYYMMDD_LEN + 1];
    rc = strncpy_s(adjusted, sizeof(adjusted), str, len+1);
    securec_check(rc, "\0", "\0");
    /* example: '20101' -> '201001' */
    if (len == B_FORMAT_DATE_NUMBER_MIN_LEN) {
        adjusted[B_FORMAT_DATE_NUMBER_MIN_LEN] = adjusted[B_FORMAT_DATE_NUMBER_MIN_LEN - 1];
        adjusted[B_FORMAT_DATE_NUMBER_MIN_LEN - 1] = '0';
    } 
    if (len == DATE_YYYYMMDD_LEN) {
        adjusted[DATE_YYYYMMDD_LEN] = '\0';
    } else {
        adjusted[DATE_YYMMDD_LEN] = '\0';
        /* example: '2010117' -> '201001' */
        if (len == DATE_YYMMDD_LEN + 1) {
            ereport(WARNING,
                (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                    errmsg("Incorrect %s value: \"%s\". \"YYYYMMDD\" or \"YYMMDD\" format is recommended.", "date", str)));
        }
    }
    int date = atoi(adjusted);
    return int32_b_format_date_internal(tm, date, strlen(adjusted) != DATE_YYYYMMDD_LEN, date_flag);
}

inline bool is_flag_enable(unsigned int date_flag, unsigned int flag)
{
    return (date_flag & flag) != 0;
}

inline bool is_enable_zero_date_bypassed(unsigned int date_flag)
{
    return is_flag_enable(date_flag, ENABLE_ZERO_DATE_BYPASSED);
}

inline bool is_enable_error_on_date_less_than_min_value(unsigned int date_flag)
{
    return !date_flag || is_flag_enable(date_flag, EANBLE_ERROR_ON_DATE_LESS_THAN_MIN);
}


int int32_b_format_date_internal(struct pg_tm *tm, int4 date, bool mayBe2Digit, unsigned int date_flag)
{
    int dterr;
    /* YYYYMMDD or YYMMDD*/
    tm->tm_mday = date % 100; /* DD */
    tm->tm_mon = date / 100 % 100; /* MM */
    tm->tm_year = date / 10000; /* YY or YYYY*/
    bool is2digits = mayBe2Digit && (tm->tm_year >= 0 && tm->tm_year <= 99);
    /* validate b format date */
    if (tm->tm_year > B_FORMAT_MAX_YEAR_OF_DATE) {
        dterr = DTERR_FIELD_OVERFLOW;
    } else if (is2digits && is_enable_zero_date_bypassed(date_flag) && date == 0) {
        return 0;
    } else if (is2digits && !date_flag && date == 0 && !(SQL_MODE_NO_ZERO_DATE() && SQL_MODE_STRICT())) {
        return 0;
    } else if (is2digits && is_enable_error_on_date_less_than_min_value(date_flag) &&
        date > 0 && date < B_FORMAT_DATE_INT_MIN) {
        tm->tm_year = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 0;
        return DTERR_FIELD_OVERFLOW;
    } else {
        dterr = ValidateDateForBDatabase(is2digits, tm, date_flag);
    }
    return dterr;
}

Datum int8_b_format_date(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(int32_b_format_date, PG_GET_COLLATION(),
                                   Int32GetDatum((int32)PG_GETARG_INT8(0)), fcinfo->can_ignore);
}

Datum int16_b_format_date(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(int32_b_format_date, PG_GET_COLLATION(),
                                   Int32GetDatum((int32)PG_GETARG_INT16(0)), fcinfo->can_ignore);
}

Datum bool_b_format_date(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(int32_b_format_date, PG_GET_COLLATION(),
                                   Int32GetDatum((int32)PG_GETARG_BOOL(0)), fcinfo->can_ignore);
}

/* int4 to b format date type conversion */
Datum int32_b_format_date(PG_FUNCTION_ARGS) 
{
    int4 date = PG_GETARG_INT32(0);
    DateADT result;
    struct pg_tm tt, *tm = &tt;
    int errlevel = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    if (int32_b_format_date_internal(tm, date, true)) {
        ereport(errlevel,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("Out of range value for date")));
        tm->tm_year = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 0;
    }
    if (date == 0 && !SQL_MODE_STRICT() && SQL_MODE_NO_ZERO_DATE()) {
        ereport(WARNING,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("Out of range value for date")));
    }
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday)) {
        ereport(errlevel,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("date out of range: \"%d\"", date)));
        tm->tm_year = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 0;
    }
    result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
    PG_RETURN_DATEADT(result);
}

Datum int64_b_format_date(PG_FUNCTION_ARGS)
{
    int64 number = PG_GETARG_INT64(0);
    if (number >= (int64)pow_of_10[7]) { /* date: 1000-00-00 */
        Datum datetime = DirectFunctionCall1(int64_b_format_datetime, Int64GetDatum(number));
        return DirectFunctionCall1(timestamp_date, datetime);
    }
    return DirectFunctionCall1Coll(int32_b_format_date, InvalidOid, Int32GetDatum((int32)number), fcinfo->can_ignore);
}

#endif
/* date_out()
 * Given internal format date, convert to text string.
 */
Datum date_out(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    char* result = NULL;
    struct pg_tm tt, *tm = &tt;
    char buf[MAXDATELEN + 1];

    if (DATE_NOT_FINITE(date))
        EncodeSpecialDate(date, buf, MAXDATELEN + 1);
    else {
        if (unlikely(date > 0 && (INT_MAX - date < POSTGRES_EPOCH_JDATE))) {
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("input julian date is overflow")));
        }
        j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
#ifdef DOLPHIN
        EncodeDateOnlyForBDatabase(tm, u_sess->time_cxt.DateStyle, buf);
#else
        EncodeDateOnly(tm, u_sess->time_cxt.DateStyle, buf);
#endif
    }

    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

char* output_date_out(DateADT date)
{
    struct pg_tm tt, *tm = &tt;

    u_sess->utils_cxt.dateoutput_buffer[0] = '\0';

    if (DATE_NOT_FINITE(date))
        EncodeSpecialDate(date, u_sess->utils_cxt.dateoutput_buffer, MAXDATELEN + 1);
    else {
        if (unlikely(date > 0 && (INT_MAX - date < POSTGRES_EPOCH_JDATE))) {
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("input julian date is overflow")));
        }
        j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
#ifdef DOLPHIN
        EncodeDateOnlyForBDatabase(tm, u_sess->time_cxt.DateStyle, u_sess->utils_cxt.dateoutput_buffer);
#else
        EncodeDateOnly(tm, u_sess->time_cxt.DateStyle, u_sess->utils_cxt.dateoutput_buffer);
#endif
    }

    return u_sess->utils_cxt.dateoutput_buffer;
}

/*
 *		date_recv			- converts external binary format to date
 */
Datum date_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    DateADT result;

    result = (DateADT)pq_getmsgint(buf, sizeof(DateADT));

    /* Limit to the same range that date_in() accepts. */
    if (DATE_NOT_FINITE(result))
        /* ok */;
    else if (result < -POSTGRES_EPOCH_JDATE || result >= JULIAN_MAX - POSTGRES_EPOCH_JDATE)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range")));

    PG_RETURN_DATEADT(result);
}

/*
 *		date_send			- converts date to binary format
 */
Datum date_send(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendint32(&buf, date);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 * Convert reserved date values to string.
 */
static void EncodeSpecialDate(DateADT dt, char* str, int strlen)
{
    errno_t rc = EOK;

    if (DATE_IS_NOBEGIN(dt)) {
        rc = strcpy_s(str, strlen, EARLY);
        securec_check(rc, "", "");
    } else if (DATE_IS_NOEND(dt)) {
        rc = strcpy_s(str, strlen, LATE);
        securec_check(rc, "", "");
    } else /* shouldn't happen */
        ereport(
            ERROR, (errcode(ERRCODE_OPTIMIZER_INCONSISTENT_STATE), errmsg("invalid argument for EncodeSpecialDate")));
}

/*
 * Comparison functions for dates
 */

Datum date_eq(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_BOOL(dateVal1 == dateVal2);
}

Datum date_ne(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_BOOL(dateVal1 != dateVal2);
}

Datum date_lt(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_BOOL(dateVal1 < dateVal2);
}

Datum date_le(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_BOOL(dateVal1 <= dateVal2);
}

Datum date_gt(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_BOOL(dateVal1 > dateVal2);
}

Datum date_ge(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_BOOL(dateVal1 >= dateVal2);
}

Datum date_cmp(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    if (dateVal1 < dateVal2)
        PG_RETURN_INT32(-1);
    else if (dateVal1 > dateVal2)
        PG_RETURN_INT32(1);
    PG_RETURN_INT32(0);
}

static int date_fastcmp(Datum x, Datum y, SortSupport ssup)
{
    DateADT a = DatumGetDateADT(x);
    DateADT b = DatumGetDateADT(y);

    if (a < b)
        return -1;
    else if (a > b)
        return 1;
    return 0;
}

Datum date_sortsupport(PG_FUNCTION_ARGS)
{
    SortSupport ssup = (SortSupport)PG_GETARG_POINTER(0);

    ssup->comparator = date_fastcmp;
    PG_RETURN_VOID();
}

Datum date_finite(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);

    PG_RETURN_BOOL(!DATE_NOT_FINITE(date));
}

Datum date_larger(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_DATEADT((dateVal1 > dateVal2) ? dateVal1 : dateVal2);
}

Datum date_smaller(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    PG_RETURN_DATEADT((dateVal1 < dateVal2) ? dateVal1 : dateVal2);
}

/* Compute difference between two dates in days.
 */
Datum date_mi(PG_FUNCTION_ARGS)
{
    DateADT dateVal1 = PG_GETARG_DATEADT(0);
    DateADT dateVal2 = PG_GETARG_DATEADT(1);

    if (DATE_NOT_FINITE(dateVal1) || DATE_NOT_FINITE(dateVal2))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("cannot subtract infinite dates")));

    PG_RETURN_INT32((int32)(dateVal1 - dateVal2));
}

/* Add a number of days to a date, giving a new date.
 * Must handle both positive and negative numbers of days.
 */
Datum date_pli(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    int32 days = PG_GETARG_INT32(1);

    if (DATE_NOT_FINITE(dateVal))
        days = 0; /* can't change infinity */

    PG_RETURN_DATEADT(dateVal + days);
}

/* Subtract a number of days from a date, giving a new date.
 */
Datum date_mii(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    int32 days = PG_GETARG_INT32(1);

    if (DATE_NOT_FINITE(dateVal))
        days = 0; /* can't change infinity */

    PG_RETURN_DATEADT(dateVal - days);
}

/*
 * Internal routines for promoting date to timestamp and timestamp with
 * time zone
 */

Timestamp date2timestamp(DateADT dateVal)
{
    Timestamp result;

    if (DATE_IS_NOBEGIN(dateVal))
        TIMESTAMP_NOBEGIN(result);
    else if (DATE_IS_NOEND(dateVal))
        TIMESTAMP_NOEND(result);
    else {
#ifdef HAVE_INT64_TIMESTAMP
        /* date is days since 2000, timestamp is microseconds since same... */
        result = dateVal * USECS_PER_DAY;
        /* Date's range is wider than timestamp's, so check for overflow */
        if (result / USECS_PER_DAY != dateVal)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range for timestamp")));
#else
        /* date is days since 2000, timestamp is seconds since same... */
        result = dateVal * (double)SECS_PER_DAY;
#endif
    }

    return result;
}

static TimestampTz date2timestamptz(DateADT dateVal)
{
    TimestampTz result;
    struct pg_tm tt, *tm = &tt;
    int tz;

    if (DATE_IS_NOBEGIN(dateVal))
        TIMESTAMP_NOBEGIN(result);
    else if (DATE_IS_NOEND(dateVal))
        TIMESTAMP_NOEND(result);
    else {
        j2date(dateVal + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        tz = DetermineTimeZoneOffset(tm, session_timezone);

#ifdef HAVE_INT64_TIMESTAMP
        result = dateVal * USECS_PER_DAY + tz * USECS_PER_SEC;
        /* Date's range is wider than timestamp's, so check for overflow */
        if ((result - tz * USECS_PER_SEC) / USECS_PER_DAY != dateVal)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range for timestamp")));
#else
        result = dateVal * (double)SECS_PER_DAY + tz;
#endif
    }

    return result;
}

/*
 * date2timestamp_no_overflow
 *
 * This is chartered to produce a double value that is numerically
 * equivalent to the corresponding Timestamp value, if the date is in the
 * valid range of Timestamps, but in any case not throw an overflow error.
 * We can do this since the numerical range of double is greater than
 * that of non-erroneous timestamps.  The results are currently only
 * used for statistical estimation purposes.
 */
double date2timestamp_no_overflow(DateADT dateVal)
{
    double result;

    if (DATE_IS_NOBEGIN(dateVal))
        result = -DBL_MAX;
    else if (DATE_IS_NOEND(dateVal))
        result = DBL_MAX;
    else {
#ifdef HAVE_INT64_TIMESTAMP
        /* date is days since 2000, timestamp is microseconds since same... */
        result = dateVal * (double)USECS_PER_DAY;
#else
        /* date is days since 2000, timestamp is seconds since same... */
        result = dateVal * (double)SECS_PER_DAY;
#endif
    }

    return result;
}

/*
 * Crosstype comparison functions for dates
 */

Datum date_eq_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum date_ne_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum date_lt_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum date_gt_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum date_le_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum date_ge_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

Datum date_cmp_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    Timestamp dt1;

    dt1 = date2timestamp(dateVal);

    PG_RETURN_INT32(timestamp_cmp_internal(dt1, dt2));
}

Datum date_eq_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) == 0);
}

Datum date_ne_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) != 0);
}

Datum date_lt_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) < 0);
}

Datum date_gt_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) > 0);
}

Datum date_le_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) <= 0);
}

Datum date_ge_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) >= 0);
}

Datum date_cmp_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz dt2 = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz dt1;

    dt1 = date2timestamptz(dateVal);

    PG_RETURN_INT32(timestamptz_cmp_internal(dt1, dt2));
}

Datum timestamp_eq_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum timestamp_ne_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum timestamp_lt_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum timestamp_gt_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum timestamp_le_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum timestamp_ge_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

Datum timestamp_cmp_date(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    Timestamp dt2;

    dt2 = date2timestamp(dateVal);

    PG_RETURN_INT32(timestamp_cmp_internal(dt1, dt2));
}

Datum timestamptz_eq_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) == 0);
}

Datum timestamptz_ne_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) != 0);
}

Datum timestamptz_lt_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) < 0);
}

Datum timestamptz_gt_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) > 0);
}

Datum timestamptz_le_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) <= 0);
}

Datum timestamptz_ge_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_BOOL(timestamptz_cmp_internal(dt1, dt2) >= 0);
}

Datum timestamptz_cmp_date(PG_FUNCTION_ARGS)
{
    TimestampTz dt1 = PG_GETARG_TIMESTAMPTZ(0);
    DateADT dateVal = PG_GETARG_DATEADT(1);
    TimestampTz dt2;

    dt2 = date2timestamptz(dateVal);

    PG_RETURN_INT32(timestamptz_cmp_internal(dt1, dt2));
}

/* Add an interval to a date, giving a new date.
 * Must handle both positive and negative intervals.
 *
 * We implement this by promoting the date to timestamp (without time zone)
 * and then using the timestamp plus interval function.
 */
Datum date_pl_interval(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    Timestamp dateStamp;

    dateStamp = date2timestamp(dateVal);

    return DirectFunctionCall2(timestamp_pl_interval, TimestampGetDatum(dateStamp), PointerGetDatum(span));
}

/* Subtract an interval from a date, giving a new date.
 * Must handle both positive and negative intervals.
 *
 * We implement this by promoting the date to timestamp (without time zone)
 * and then using the timestamp minus interval function.
 */
Datum date_mi_interval(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    Timestamp dateStamp;

    dateStamp = date2timestamp(dateVal);

    return DirectFunctionCall2(timestamp_mi_interval, TimestampGetDatum(dateStamp), PointerGetDatum(span));
}

/* date_to_text()
 * Convert date to text data type.
 */
Datum date_text(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(date_out, dateVal));

    result = DirectFunctionCall1(textin, CStringGetDatum(tmp));
    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/* date_to_varchar()
 * Convert date to varchar type
 */
Datum date_varchar(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(date_out, dateVal));

    result = DirectFunctionCall3(varcharin, CStringGetDatum(tmp), 0, -1);
    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/* date_to_bpchar()
 * Convert date to bpchar type
 */
Datum date_bpchar(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(date_out, dateVal));

    result = DirectFunctionCall3(bpcharin, CStringGetDatum(tmp), 0, -1);
    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/* varchar_date
 * Convert varchar to date type
 */
Datum varchar_date(PG_FUNCTION_ARGS)
{
    Datum varcharValue = PG_GETARG_DATUM(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(varcharout, varcharValue));

    result = DirectFunctionCall1(date_in, CStringGetDatum(tmp));

    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/* bpchar_date()
 * Convert bpchar to date type
 */
Datum bpchar_date(PG_FUNCTION_ARGS)
{
    Datum bpcharValue = PG_GETARG_DATUM(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(bpcharout, bpcharValue));

    result = DirectFunctionCall1(date_in, CStringGetDatum(tmp));

    pfree_ext(tmp);

    PG_RETURN_DATUM(result);
}

/*
 * @Description: Convert text to date type
 * @in textValue: text type data.
 * @return: Convert result.
 */
Datum text_date(PG_FUNCTION_ARGS)
{
    Datum textValue = PG_GETARG_DATUM(0);
    char* tmp = NULL;
    Datum result;
    tmp = DatumGetCString(DirectFunctionCall1(textout, textValue));

    result = DirectFunctionCall1(date_in, CStringGetDatum(tmp));

    pfree_ext(tmp);

    PG_RETURN_DATEADT(result);
}

/* date_timestamp()
 * Convert date to timestamp data type.
 */
Datum date_timestamp(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp result;

    result = date2timestamp(dateVal);

    PG_RETURN_TIMESTAMP(result);
}

Datum timestamp2date(Timestamp timestamp)
{
    DateADT result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;

    if (TIMESTAMP_IS_NOBEGIN(timestamp))
        DATE_NOBEGIN(result);
    else if (TIMESTAMP_IS_NOEND(timestamp))
        DATE_NOEND(result);
    else {
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
    }
    PG_RETURN_DATEADT(result);
}

/* timestamp_date()
 * Convert timestamp to date data type.
 */
Datum timestamp_date(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    return timestamp2date(timestamp);
}

/* date_timestamptz()
 * Convert date to timestamp with time zone data type.
 */
Datum date_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    TimestampTz result;

    result = date2timestamptz(dateVal);

    PG_RETURN_TIMESTAMP(result);
}

/* timestamptz_date()
 * Convert timestamp with time zone to date data type.
 */
Datum timestamptz_date(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMP(0);
    DateADT result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz;

    if (TIMESTAMP_IS_NOBEGIN(timestamp))
        DATE_NOBEGIN(result);
    else if (TIMESTAMP_IS_NOEND(timestamp))
        DATE_NOEND(result);
    else {
        if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

        result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
    }

    PG_RETURN_DATEADT(result);
}

/* abstime_date()
 * Convert abstime to date data type.
 */
Datum abstime_date(PG_FUNCTION_ARGS)
{
    AbsoluteTime abstime = PG_GETARG_ABSOLUTETIME(0);
    DateADT result;
    struct pg_tm tt, *tm = &tt;
    int tz;

    switch (abstime) {
        case INVALID_ABSTIME:
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("cannot convert reserved abstime value to date")));
            result = 0; /* keep compiler quiet */
            break;

        case NOSTART_ABSTIME:
            DATE_NOBEGIN(result);
            break;

        case NOEND_ABSTIME:
            DATE_NOEND(result);
            break;

        default:
            abstime2tm(abstime, &tz, tm, NULL);
            result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
            break;
    }

    PG_RETURN_DATEADT(result);
}

/*****************************************************************************
 *	 Time ADT
 *****************************************************************************/

Datum time_in(PG_FUNCTION_ARGS)
{
#ifdef DOLPHIN
    char* input_str = PG_GETARG_CSTRING(0);
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, input_str, TIME_IN, &time_error_type);
    if (time_error_type == TIME_INCORRECT && ENABLE_B_CMPT_MODE) {
        PG_RETURN_TIMEADT(B_FORMAT_TIME_INVALID_VALUE_TAG);
    }
    return datum_internal;
}

/*
 *	 time_cast_implicit, such as select time'23:65:66'
 *
 */
Datum time_cast(PG_FUNCTION_ARGS)
{
    char* input_str = PG_GETARG_CSTRING(0);
    TimeErrorType time_error_type = TIME_CORRECT;
    return time_internal(fcinfo, input_str, TIME_CAST, &time_error_type);
}

Datum time_cast_implicit(PG_FUNCTION_ARGS)
{
    char* input_str = DatumGetCString(textout(fcinfo));
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, input_str, TIME_CAST_IMPLICIT, &time_error_type);
    return datum_internal;
}

char* parser_function_input(Datum txt, Oid oid)
{
    Oid typeOutput;
    bool typIsVarlena;
    getTypeOutputInfo(oid, &typeOutput, &typIsVarlena);
    return DatumGetCString(OidOutputFunctionCall(typeOutput, txt));
}

/*
 *	 text_time_explicit, such as select cast('23:65:66' as time)
 *
 */
Datum text_time_explicit(PG_FUNCTION_ARGS)
{
    char* input_str = fcinfo->argTypes[0] ?
                      parser_function_input(PG_GETARG_DATUM(0), fcinfo->argTypes[0]) :
                      PG_GETARG_CSTRING(0);
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, input_str, TEXT_TIME_EXPLICIT, &time_error_type);
    if (time_error_type == TIME_INCORRECT) {
        PG_RETURN_NULL();
    }
    return datum_internal;
}

Datum time_internal(PG_FUNCTION_ARGS, char* str, int time_cast_type, TimeErrorType* time_error_type)
{
#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = time_cast_type > 0 ? -1 : PG_GETARG_INT32(2);
    TimeADT result;
    fsec_t fsec;
    struct pg_tm tt;
    struct pg_tm *tm = &tt;
    int tz;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char* field[MAXDATEFIELDS] = {0};
    int dtype;
    int ftype[MAXDATEFIELDS];
    char* time_fmt = NULL;
    int timeSign = 1;
    /* tt2 stores openGauss's parsing result while tt stores M*'s parsing result */
    struct pg_tm tt2;
    bool null_func_result = false;
    /*
     * this case is used for time format is specified.
     */
    if (TIME_WITH_FORMAT_ARGS_SIZE == PG_NARGS() && time_cast_type == 0) {
        time_fmt = PG_GETARG_CSTRING(3);
        if (time_fmt == NULL) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT), errmsg("specified time format is null")));
        }

        /* the following logic shared from to_timestamp(). */
        to_timestamp_from_format(tm, &fsec, str, (void*)time_fmt);
    } else {
        int tm_type;
        bool warnings;
        errno_t rc = memset_s(tm, sizeof(struct pg_tm), 0, sizeof(struct pg_tm));
        securec_check(rc, "\0", "\0");
        cstring_to_time(str, tm, fsec, timeSign, tm_type, warnings, &null_func_result);

        if (warnings) {
            tm = &tt2;
            int D = 0;
            bool hasD = false;
            char *adjusted = adjust_b_format_time(str, &timeSign, &D, &hasD);
            /* check if empty */
            if (strlen(adjusted) == 0) {
                *time_error_type = TIME_INCORRECT;
                PG_RETURN_TIMEADT(0);
            }
            dterr = ParseDateTime(adjusted, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
            if (dterr == 0) {
                dterr = DecodeTimeOnlyForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tz, D);
            }
            if (dterr != 0) {
                /*
                 * If sql mode is strict, then an error had happend,
                 * otherwise we can return tt which stores M*'s parsing result.
                 */
                if (SQL_MODE_STRICT()) {
                    DateTimeParseErrorWithFlag(dterr, str, "time", fcinfo->can_ignore, !fcinfo->can_ignore);
                    /*
                     * can_ignore == true means hint string "ignore_error" used. warning report instead of error.
                     * then we will return 00:00:xx if the first 1 or 2 character is lower than 60, otherwise return 00:00:00
                     */
                    char* field_str = field[0];
                    if (field_str == NULL) {
                        *time_error_type = TIME_INCORRECT;
                        PG_RETURN_TIMEADT(0);
                    }
                    if (*field_str == '+') {
                        field_str++;
                    }
                    int trunc_val = getStartingDigits(field_str);
                    if (trunc_val < 0 || trunc_val >= 60) {
                        PG_RETURN_TIMEADT(0);
                    }
                    *time_error_type = TIME_INCORRECT;
                    PG_RETURN_TIMEADT(trunc_val * TIME_MS_TO_S_RADIX * TIME_MS_TO_S_RADIX);
                } else if (SQL_MODE_NOT_STRICT_ON_INSERT()) {
                    /* for case insert unavailable data, need to set the unavailable data to 0 to compatible with M */
                    DateTimeParseError(dterr, str, "time", true);
                    if (IsResetUnavailableDataTime(dterr, tt, time_cast_type != TIME_CAST_IMPLICIT)) {
                        PG_RETURN_TIMEADT(0);
                    } else {
                        tm = &tt; // switch to M*'s parsing result
                    }
                } else {
                    if (time_cast_type == TEXT_TIME_EXPLICIT || time_cast_type == TIME_IN) {
                        DateTimeParseError(dterr, str, "time", true);
                        tm = &tt; // switch to M*'s parsing result
                        if (dterr != DTERR_TZDISP_OVERFLOW && null_func_result) {
                            *time_error_type = TIME_INCORRECT;
                        }
                    } else if (time_cast_type == TIME_CAST) {
                        DateTimeParseErrorWithFlag(dterr, str, "time", fcinfo->can_ignore, !SQL_MODE_STRICT());
                        tm = &tt; // switch to M*'s parsing result
                    } else {
                        DateTimeParseError(dterr, str, "time", !SQL_MODE_STRICT());
                        tm = &tt; // switch to M*'s parsing result
                    }
                }
            }
        }
    }

    /*
     * the following logic is unified for time parsing.
     */
    *time_error_type = null_func_result ? TIME_INCORRECT : *time_error_type;
    tm2time(tm, fsec, &result);
    AdjustTimeForTypmod(&result, typmod);
    result *= timeSign;
    PG_RETURN_TIMEADT(result);
#else

    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimeADT result;
    fsec_t fsec;
    struct pg_tm tt;
    struct pg_tm *tm = &tt;
    int tz;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char* field[MAXDATEFIELDS] = {0};
    int dtype;
    int ftype[MAXDATEFIELDS];
    char* time_fmt = NULL;
    /*
     * this case is used for time format is specified.
     */
    if (TIME_WITH_FORMAT_ARGS_SIZE == PG_NARGS()) {
        time_fmt = PG_GETARG_CSTRING(3);
        if (time_fmt == NULL) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT), errmsg("specified time format is null")));
        }

        /* the following logic shared from to_timestamp(). */
        to_timestamp_from_format(tm, &fsec, str, (void*)time_fmt);
    } else {
        /*
         * original pg time format parsing
         */
        dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
        if (dterr == 0)
            dterr = DecodeTimeOnly(field, ftype, nf, &dtype, tm, &fsec, &tz);
        if (dterr != 0) {
            DateTimeParseError(dterr, str, "time", fcinfo->can_ignore);
            /*
             * can_ignore == true means hint string "ignore_error" used. warning report instead of error.
             * then we will return 00:00:xx if the first 1 or 2 character is lower than 60, otherwise return 00:00:00
             */
            char* field_str = field[0];
            if (*field_str == '+') {
                field_str++;
            }
            int trunc_val = getStartingDigits(field_str);
            if (trunc_val < 0 || trunc_val >= 60) {
                PG_RETURN_TIMEADT(0);
            }
            PG_RETURN_TIMEADT(trunc_val * TIME_MS_TO_S_RADIX * TIME_MS_TO_S_RADIX);
        }
    }

    /*
     * the following logic is unified for time parsing.
     */
    tm2time(tm, fsec, &result);
    AdjustTimeForTypmod(&result, typmod);
    PG_RETURN_TIMEADT(result);
#endif
}


#ifdef DOLPHIN
int NumberTime(bool timeIn24, char *str, pg_tm *tm, fsec_t *fsec, int D, bool hasD)
{
    *fsec = 0;
    int len = 0;
    char *cp = str;
    char tmp[MAXDATELEN + 1];
    int time = 0;
    /* time field */
    while (isdigit((unsigned char) *cp)) {
        tmp[len] = *cp;
        ++len;
        ++cp;
    }
    /* have fsec */
    if (*cp == '.') {
        double frac;    
        errno = 0;
        frac = strtod(cp, &cp);
        /* check for parse failure */
        if (*cp != '\0' || errno != 0) {
            return ERRCODE_INVALID_DATETIME_FORMAT;
        }
#ifdef HAVE_INT64_TIMESTAMP
        *fsec = rint(frac * 1000000);
#else
        *fsec = frac;
#endif
    }
    /* for example, '10001212' */
    if (!hasD && len > B_FORMAT_TIME_NUMBER_MAX_LEN) {
        return ERRCODE_DATETIME_VALUE_OUT_OF_RANGE;
    }
    /* must be 'D hhh' format */
    if (hasD) {
        /* for example, 'D 0001234', idx = 3 pointing to '1' */
        int idx;
        for (idx = 0; idx < len; ++idx) {
            if (tmp[idx] != '0') break;
        }
        /* validate 'D hhh' format, 3 = # of h */
        if (len - idx > 3) {
            return ERRCODE_DATETIME_VALUE_OUT_OF_RANGE;
        }
    }

    tmp[len] = '\0';
    time = atoi(tmp);
    /* validate D hhh format */
    if (hasD && time < B_FORMAT_TIME_BOUND) {
        time = (time + D * 24) * 10000; // convert to hours, 10000 stands for mmss
    }
    return int32_b_format_time_internal(tm, timeIn24, time, fsec);
}

int int32_b_format_time_internal(struct pg_tm *tm, bool timeIn24, int4 time, fsec_t *fsec)
{
    /* please make sure that time >= 0*/
    Assert(time >= 0);
    int dterr;
    /* hhmmss */
    tm->tm_sec = time % 100; /* ss */
    tm->tm_min = time / 100 % 100; /* mm */
    tm->tm_hour = time / 10000; /* hh */
    dterr = ValidateTimeForBDatabase(timeIn24, tm, fsec);
    return dterr;
}

/* numeric(hhmmss.xxxxxx) convert to b format time */
Datum numeric_b_format_time(PG_FUNCTION_ARGS)
{
    Numeric n = PG_GETARG_NUMERIC(0);
    char *str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(n)));
    return DirectFunctionCall3(time_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
}

Datum float8_b_format_time(PG_FUNCTION_ARGS)
{
    float8 n = PG_GETARG_FLOAT8(0);
    char *str = DatumGetCString(DirectFunctionCall1(float8out, Float8GetDatum(n)));
    return DirectFunctionCall3(time_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1));
}

Datum numeric_cast_time(PG_FUNCTION_ARGS)
{
    Numeric n = PG_GETARG_NUMERIC(0);
    char *str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(n)));
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, str, TEXT_TIME_EXPLICIT, &time_error_type);
    if (time_error_type == TIME_INCORRECT) {
        PG_RETURN_NULL();
    }
    return datum_internal;
}

Datum float8_cast_time(PG_FUNCTION_ARGS)
{
    float8 n = PG_GETARG_FLOAT8(0);
    char *str = DatumGetCString(DirectFunctionCall1(float8out, Float8GetDatum(n)));
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, str, TEXT_TIME_EXPLICIT, &time_error_type);
    if (time_error_type == TIME_INCORRECT) {
        PG_RETURN_NULL();
    }
    return datum_internal;
}

Datum float4_cast_time(PG_FUNCTION_ARGS)
{
    float4 n = PG_GETARG_FLOAT4(0);
    char *str = DatumGetCString(DirectFunctionCall1(float4out, Float4GetDatum(n)));
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, str, TEXT_TIME_EXPLICIT, &time_error_type);
    if (time_error_type == TIME_INCORRECT) {
        PG_RETURN_NULL();
    }
    return datum_internal;
}

Datum numeric_cast_date(PG_FUNCTION_ARGS)
{
    Numeric n = PG_GETARG_NUMERIC(0);
    bool isRetNull = false;
    Datum datetime = DirectCall2(&isRetNull, numeric_b_format_datetime, InvalidOid, NumericGetDatum(n),
        BoolGetDatum(true));
    if (isRetNull) {
        PG_RETURN_NULL();
    }
    return DirectFunctionCall1(timestamp_date, datetime);
}

Datum float8_cast_date(PG_FUNCTION_ARGS)
{
    float8 n = PG_GETARG_FLOAT8(0);
    bool isRetNull = false;
    Datum datetime = DirectCall2(&isRetNull, float8_b_format_datetime, InvalidOid, Float8GetDatum(n),
        BoolGetDatum(true));
    if (isRetNull) {
        PG_RETURN_NULL();
    }
    return DirectFunctionCall1(timestamp_date, datetime);
}

Datum float4_cast_date(PG_FUNCTION_ARGS)
{
    float8 n = (float8)PG_GETARG_FLOAT4(0);
    bool isRetNull = false;
    Datum datetime = DirectCall2(&isRetNull, float8_b_format_datetime, InvalidOid, Float8GetDatum(n),
        BoolGetDatum(true));
    if (isRetNull) {
        PG_RETURN_NULL();
    }
    return DirectFunctionCall1(timestamp_date, datetime);
}

Datum uint8_b_format_time(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(uint64_b_format_time, InvalidOid, UInt64GetDatum((uint64)PG_GETARG_UINT8(0)),
        fcinfo->can_ignore);
}

Datum uint16_b_format_time(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(uint64_b_format_time, InvalidOid, UInt64GetDatum((uint64)PG_GETARG_UINT16(0)),
        fcinfo->can_ignore);
}

Datum uint32_b_format_time(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(uint64_b_format_time, InvalidOid, UInt64GetDatum((uint64)PG_GETARG_UINT32(0)),
        fcinfo->can_ignore);
}

Datum uint64_b_format_time(PG_FUNCTION_ARGS)
{
    uint64 number = PG_GETARG_UINT64(0);
    char *str = DatumGetCString(DirectFunctionCall1(uint8out, UInt64GetDatum(number)));
    return DirectFunctionCall3Coll(time_in, InvalidOid, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid),
        Int32GetDatum(-1), fcinfo->can_ignore);
}

Datum uint8_cast_time(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_time(fcinfo, (uint64)PG_GETARG_UINT8(0));
}

Datum uint16_cast_time(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_time(fcinfo, (uint64)PG_GETARG_UINT16(0));
}

Datum uint32_cast_time(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_time(fcinfo, (uint64)PG_GETARG_UINT32(0));
}

Datum uint64_cast_time(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_time(fcinfo, (uint64)PG_GETARG_UINT64(0));
}

Datum uint64_number_cast_time(PG_FUNCTION_ARGS, uint64 number)
{
    bool isnull = false;
    Datum result = int_cast_time_internal(fcinfo, number, &isnull);
    if (isnull) {
        PG_RETURN_NULL();
    } else {
        return result;
    }
}

Datum uint8_cast_date(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_date(fcinfo, (uint64)PG_GETARG_UINT8(0));
}

Datum uint16_cast_date(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_date(fcinfo, (uint64)PG_GETARG_UINT16(0));
}

Datum uint32_cast_date(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_date(fcinfo, (uint64)PG_GETARG_UINT32(0));
}

Datum uint64_cast_date(PG_FUNCTION_ARGS)
{
    return uint64_number_cast_date(fcinfo, (uint64)PG_GETARG_UINT64(0));
}

Datum uint64_number_cast_date(PG_FUNCTION_ARGS, uint64 number)
{
    bool isnull = false;
    Datum result = int_cast_date_internal(fcinfo, number, &isnull);
    if (isnull) {
        PG_RETURN_NULL();
    } else {
        return result;
    }
}


Datum int8_b_format_time(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(int32_b_format_time, InvalidOid, Int32GetDatum((int32)PG_GETARG_INT8(0)),
        fcinfo->can_ignore);
}

Datum int16_b_format_time(PG_FUNCTION_ARGS)
{
    return DirectFunctionCall1Coll(int32_b_format_time, InvalidOid, Int32GetDatum((int32)PG_GETARG_INT16(0)),
        fcinfo->can_ignore);
}

/* int4(hhmmss) convert to b format time */
Datum int32_b_format_time(PG_FUNCTION_ARGS)
{
    int4 time = PG_GETARG_INT32(0);
    int errlevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;
    TimeADT result;
    fsec_t fsec = 0;
    int dterr;
    struct pg_tm tt, *tm = &tt;
    int sign = time < 0 ? -1 : 1;
    time *= sign;
    dterr = int32_b_format_time_internal(tm, false, time, &fsec);
    if (dterr) {
        ereport(errlevel,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
        if (fcinfo->can_ignore || (SQL_MODE_NOT_STRICT_ON_INSERT())) {
            PG_RETURN_TIMEADT(0);
        }
    }
    tm2time(tm, 0, &result);
    PG_RETURN_TIMEADT(result * sign);
}

/* int8(hhmmss) convert to b format time */
Datum int64_b_format_time(PG_FUNCTION_ARGS)
{
    int64 number = PG_GETARG_INT64(0);
    if (number >= (int64)pow_of_10[10]) { /* datetime: 0001-00-00 00-00-00 */
        Datum datetime = DirectFunctionCall1(int64_b_format_datetime, Int64GetDatum(number));
        return DirectFunctionCall1(timestamp_time, datetime);
    }
    char *str = DatumGetCString(DirectFunctionCall1(int8out, UInt64GetDatum(number)));
    return DirectFunctionCall3Coll(time_in, InvalidOid, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid),
        Int32GetDatum(-1), fcinfo->can_ignore);
}

Datum bool_cast_time(PG_FUNCTION_ARGS)
{
    return int64_number_cast_time(fcinfo, (int64)PG_GETARG_BOOL(0));
}

Datum int8_cast_time(PG_FUNCTION_ARGS)
{
    return int64_number_cast_time(fcinfo, (int64)PG_GETARG_INT8(0));
}

Datum int16_cast_time(PG_FUNCTION_ARGS)
{
    return int64_number_cast_time(fcinfo, (int64)PG_GETARG_INT16(0));
}

Datum int32_cast_time(PG_FUNCTION_ARGS)
{
    return int64_number_cast_time(fcinfo, (int64)PG_GETARG_INT32(0));
}

Datum int64_cast_time(PG_FUNCTION_ARGS)
{
    return int64_number_cast_time(fcinfo, PG_GETARG_INT64(0));
}

Datum int64_number_cast_time(PG_FUNCTION_ARGS, int64 number)
{
    bool isnull = false;
    Datum result = int_cast_time_internal(fcinfo, number, &isnull);
    if (isnull) {
        PG_RETURN_NULL();
    } else {
        return result;
    }
}


Datum int_cast_time_internal(PG_FUNCTION_ARGS, int64 number, bool* isnull)
{
    /* datetime: 0001-00-00 00-00-00 */
    if (number >= (int64)pow_of_10[DATETIME_MIN_LEN] || number <= -(int64)pow_of_10[DATETIME_MIN_LEN]) {
        Datum datetime = DirectCall2(isnull, int64_b_format_datetime, InvalidOid, Int64GetDatum(number),
            BoolGetDatum(true));
        return DirectFunctionCall1(timestamp_time, datetime);
    }
    char *str = DatumGetCString(DirectFunctionCall1(int8out, Int64GetDatum(number)));
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = time_internal(fcinfo, str, TEXT_TIME_EXPLICIT, &time_error_type);
    if (time_error_type == TIME_INCORRECT || number > TIME_MAX_INT || number < TIME_MIN_INT) {
        *isnull = true;
    }
    return datum_internal;
}

Datum bool_cast_date(PG_FUNCTION_ARGS)
{
    return int64_number_cast_date(fcinfo, (int64)PG_GETARG_BOOL(0));
}

Datum int8_cast_date(PG_FUNCTION_ARGS)
{
    return int64_number_cast_date(fcinfo, (int64)PG_GETARG_INT8(0));
}

Datum int16_cast_date(PG_FUNCTION_ARGS)
{
    return int64_number_cast_date(fcinfo, (int64)PG_GETARG_INT16(0));
}

Datum int32_cast_date(PG_FUNCTION_ARGS)
{
    return int64_number_cast_date(fcinfo, (int64)PG_GETARG_INT32(0));
}

Datum int64_cast_date(PG_FUNCTION_ARGS)
{
    return int64_number_cast_date(fcinfo, PG_GETARG_INT64(0));
}

Datum int64_number_cast_date(PG_FUNCTION_ARGS, int64 number)
{
    bool isnull = false;
    Datum result = int_cast_date_internal(fcinfo, number, &isnull);
    if (isnull) {
        PG_RETURN_NULL();
    } else {
        return result;
    }
}

/* int4 to b format date type conversion */
Datum int32_b_format_date(int64 number, bool can_ignore, TimeErrorType* time_error_type)
{
    int4 date = (int4)number;
    DateADT result;
    struct pg_tm tt, *tm = &tt;
    int errlevel = can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    if (int32_b_format_date_internal(tm, date, true,
        (EANBLE_ERROR_ON_DATE_LESS_THAN_MIN | ENABLE_ZERO_DATE_BYPASSED))) {
        ereport(errlevel,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                errmsg("Out of range value for date")));
        tm->tm_year = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 0;
        *time_error_type = TIME_INCORRECT;
    }
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday)) {
        ereport(errlevel,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("date out of range: \"%d\"", date)));
        tm->tm_year = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 0;
        *time_error_type = TIME_INCORRECT;
    }
    result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
    PG_RETURN_DATEADT(result);
}


Datum int_cast_date_internal(PG_FUNCTION_ARGS, int64 number, bool* isnull)
{
    /* datetime: 0001-00-00 00-00-00 */
    if (number >= (int64)pow_of_10[DATETIME_MIN_LEN] || number <= -(int64)pow_of_10[DATETIME_MIN_LEN]) {
        Datum datetime = DirectCall2(isnull, int64_b_format_datetime, InvalidOid, Int64GetDatum(number),
            BoolGetDatum(true));
        return DirectFunctionCall1(timestamp_date, datetime);
    }
    TimeErrorType time_error_type = TIME_CORRECT;
    Datum datum_internal = int32_b_format_date(number, fcinfo->can_ignore, &time_error_type);
    if (time_error_type == TIME_INCORRECT) {
        *isnull = true;
    }
    return datum_internal;
}


static char* adjust_b_format_time(char *str, int *timeSign, int *D, bool *hasD)
{
    *timeSign = 1;
    *D = 0;
    /* Ignore spaces fields */
    while (isspace((unsigned char)*str)) {
        str++;
    }
    /* negetive time */
    if (*str == '-') {
        *timeSign = -1;
        str++;
    } 
    /* try to extract D value for format 'D hh:mm:ss.ss' */
    int tmp = 0;
    if (isdigit((unsigned char)*str)) {
        char *cp = str;
        tmp = (*cp) - '0';
        cp++;
        if (*cp == ' ') {
            *D = tmp;
            *hasD = true;
            str = cp + 1;
        } else if (isdigit((unsigned char)*cp)) {
            tmp = tmp * 10 + (*cp) - '0';
            cp++;
            /* if *cp != ' ', do nothing because D value does not appear. */
            if (*cp == ' ') {
                *hasD = true;
                *D = tmp;
                str = cp + 1;
            }
        }
    }
    return str;
}

Datum negetive_time(PG_FUNCTION_ARGS) {
    TimeADT time = PG_GETARG_TIMEADT(0);
    PG_RETURN_TIMEADT(-time);
}
#endif
/* tm2time()
 * Convert a tm structure to a time data type.
 */
#ifdef DOLPHIN
int tm2time(struct pg_tm* tm, fsec_t fsec, TimeADT* result)
#else
static int tm2time(struct pg_tm* tm, fsec_t fsec, TimeADT* result)
#endif
{
#ifdef HAVE_INT64_TIMESTAMP
    *result = ((((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec) * USECS_PER_SEC) + fsec;
#else
    *result = ((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec + fsec;
#endif
    return 0;
}

/* time2tm()
 * Convert time data type to POSIX time structure.
 *
 * For dates within the range of pg_time_t, convert to the local time zone.
 * If out of this range, leave as UTC (in practice that could only happen
 * if pg_time_t is just 32 bits) - thomas 97/05/27
 */
#ifdef DOLPHIN
int time2tm(TimeADT time, struct pg_tm* tm, fsec_t* fsec)
#else
static int time2tm(TimeADT time, struct pg_tm* tm, fsec_t* fsec)
#endif
{
#ifdef HAVE_INT64_TIMESTAMP
    tm->tm_hour = time / USECS_PER_HOUR;
    time -= tm->tm_hour * USECS_PER_HOUR;
    tm->tm_min = time / USECS_PER_MINUTE;
    time -= tm->tm_min * USECS_PER_MINUTE;
    tm->tm_sec = time / USECS_PER_SEC;
    time -= tm->tm_sec * USECS_PER_SEC;
    *fsec = time;
#else
    double trem;

recalc:
    trem = time;
    TMODULO(trem, tm->tm_hour, (double)SECS_PER_HOUR);
    TMODULO(trem, tm->tm_min, (double)SECS_PER_MINUTE);
    TMODULO(trem, tm->tm_sec, 1.0);
    trem = TIMEROUND(trem);
    /* roundoff may need to propagate to higher-order fields */
    if (trem >= 1.0) {
        time = ceil(time);
        goto recalc;
    }
    *fsec = trem;
#endif

    return 0;
}

Datum time_out(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    char* result = NULL;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
#ifdef DOLPHIN
    char buf[MAXDATELEN + 2];
    char *cp = buf;

    if (time < 0) {
        time *= -1;
        time2tm(time, tm, &fsec);
        *cp = '-';
        ++cp;
    } else {
        time2tm(time, tm, &fsec);
    }
    EncodeTimeOnly(tm, fsec, false, 0, u_sess->time_cxt.DateStyle, cp);
#else
    char buf[MAXDATELEN + 1];

    time2tm(time, tm, &fsec);
    EncodeTimeOnly(tm, fsec, false, 0, u_sess->time_cxt.DateStyle, buf);
#endif
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *		time_recv			- converts external binary format to time
 *
 * We make no attempt to provide compatibility between int and float
 * time representations ...
 */
Datum time_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimeADT result;

#ifdef HAVE_INT64_TIMESTAMP
    result = pq_getmsgint64(buf);
#ifdef DOLPHIN
    if (result <= -B_FORMAT_MAX_TIME_USECS || result >= B_FORMAT_MAX_TIME_USECS)
#else
    if (result < INT64CONST(0) || result > USECS_PER_DAY)
#endif
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#else
    result = pq_getmsgfloat8(buf);
#ifdef DOLPHIN
    if (result <= (double)(-B_FORMAT_MAX_TIME_USECS) || result >= (double)B_FORMAT_MAX_TIME_USECS)
#else
    if (result < 0 || result > (double)SECS_PER_DAY)
#endif
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#endif

    AdjustTimeForTypmod(&result, typmod);

    PG_RETURN_TIMEADT(result);
}

/*
 *		time_send			- converts time to binary format
 */
Datum time_send(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
#ifdef HAVE_INT64_TIMESTAMP
    pq_sendint64(&buf, time);
#else
    pq_sendfloat8(&buf, time);
#endif
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum timetypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);

    PG_RETURN_INT32(anytime_typmodin(false, ta));
}

Datum timetypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    PG_RETURN_CSTRING(anytime_typmodout(false, typmod));
}

/* time_transform()
 * Flatten calls to time_scale() and timetz_scale() that solely represent
 * increases in allowed precision.
 */
Datum time_transform(PG_FUNCTION_ARGS)
{
    PG_RETURN_POINTER(TemporalTransform(MAX_TIME_PRECISION, (Node*)PG_GETARG_POINTER(0)));
}

/* time_scale()
 * Adjust time type for specified scale factor.
 * Used by openGauss type system to stuff columns.
 */
Datum time_scale(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    int32 typmod = PG_GETARG_INT32(1);
    TimeADT result;

    result = time;
    AdjustTimeForTypmod(&result, typmod);

    PG_RETURN_TIMEADT(result);
}

/* AdjustTimeForTypmod()
 * Force the precision of the time value to a specified value.
 * Uses *exactly* the same code as in AdjustTimestampForTypemod()
 * but we make a separate copy because those types do not
 * have a fundamental tie together but rather a coincidence of
 * implementation. - thomas
 */
static void AdjustTimeForTypmod(TimeADT* time, int32 typmod)
{
#ifdef HAVE_INT64_TIMESTAMP
    static const int64 TimeScales[MAX_TIME_PRECISION + 1] = {INT64CONST(1000000),
        INT64CONST(100000),
        INT64CONST(10000),
        INT64CONST(1000),
        INT64CONST(100),
        INT64CONST(10),
        INT64CONST(1)};

    static const int64 TimeOffsets[MAX_TIME_PRECISION + 1] = {INT64CONST(500000),
        INT64CONST(50000),
        INT64CONST(5000),
        INT64CONST(500),
        INT64CONST(50),
        INT64CONST(5),
        INT64CONST(0)};
#else
    /* note MAX_TIME_PRECISION differs in this case */
    static const double TimeScales[MAX_TIME_PRECISION + 1] = {
        1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0, 1000000000.0, 10000000000.0};
#endif

    if (typmod >= 0 && typmod <= MAX_TIME_PRECISION) {
        /*
         * Note: this round-to-nearest code is not completely consistent about
         * rounding values that are exactly halfway between integral values.
         * On most platforms, rint() will implement round-to-nearest-even, but
         * the integer code always rounds up (away from zero).	Is it worth
         * trying to be consistent?
         */
#ifdef HAVE_INT64_TIMESTAMP
        if (*time >= INT64CONST(0))
            *time = ((*time + TimeOffsets[typmod]) / TimeScales[typmod]) * TimeScales[typmod];
        else
            *time = -((((-*time) + TimeOffsets[typmod]) / TimeScales[typmod]) * TimeScales[typmod]);
#else
        *time = rint((double)*time * TimeScales[typmod]) / TimeScales[typmod];
#endif
    }
}

Datum time_eq(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_BOOL(time1 == time2);
}

Datum time_ne(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_BOOL(time1 != time2);
}

Datum time_lt(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_BOOL(time1 < time2);
}

Datum time_le(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_BOOL(time1 <= time2);
}

Datum time_gt(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_BOOL(time1 > time2);
}

Datum time_ge(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_BOOL(time1 >= time2);
}

Datum time_cmp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    if (time1 < time2)
        PG_RETURN_INT32(-1);
    if (time1 > time2)
        PG_RETURN_INT32(1);
    PG_RETURN_INT32(0);
}

Datum time_hash(PG_FUNCTION_ARGS)
{
    /* We can use either hashint8 or hashfloat8 directly */
#ifdef HAVE_INT64_TIMESTAMP
    return hashint8(fcinfo);
#else
    return hashfloat8(fcinfo);
#endif
}

Datum time_larger(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_TIMEADT((time1 > time2) ? time1 : time2);
}

Datum time_smaller(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);

    PG_RETURN_TIMEADT((time1 < time2) ? time1 : time2);
}

void swap_ts_and_te(bool tsIsNull, bool &teIsNull, Datum &ts, Datum &te, bool isz)
{
#define TIMETZ_GT(t1, t2) DatumGetBool(DirectFunctionCall2(timetz_gt, t1, t2))
#define TIMEADT_GT(t1, t2) (DatumGetTimeADT(t1) > DatumGetTimeADT(t2))

    if (tsIsNull && !teIsNull) {
        /* swap null for non-null */
        ts = te;
        teIsNull = true;
    } else if (!teIsNull && (isz ? TIMETZ_GT(ts, te) : TIMEADT_GT(ts, te))) {
        Datum tt = ts;
        ts = te;
        te = tt;
    }
#undef TIMETZ_GT
#undef TIMEADT_GT
}

/* overlaps_time() --- implements the SQL92 OVERLAPS operator.
 *
 * Algorithm is per SQL92 spec.  This is much harder than you'd think
 * because the spec requires us to deliver a non-null answer in some cases
 * where some of the inputs are null.
 */
Datum overlaps_time(PG_FUNCTION_ARGS)
{
    /*
     * The arguments are TimeADT, but we leave them as generic Datums to avoid
     * dereferencing nulls (TimeADT is pass-by-reference!)
     */
    Datum ts1 = PG_GETARG_DATUM(0);
    Datum te1 = PG_GETARG_DATUM(1);
    Datum ts2 = PG_GETARG_DATUM(2);
    Datum te2 = PG_GETARG_DATUM(3);
    bool ts1IsNull = PG_ARGISNULL(0);
    bool te1IsNull = PG_ARGISNULL(1);
    bool ts2IsNull = PG_ARGISNULL(2);
    bool te2IsNull = PG_ARGISNULL(3);

#define TIMEADT_GT(t1, t2) (DatumGetTimeADT(t1) > DatumGetTimeADT(t2))
#define TIMEADT_LT(t1, t2) (DatumGetTimeADT(t1) < DatumGetTimeADT(t2))

    /*
     * If both endpoints of interval 1 are null, the result is null (unknown).
     * If just one endpoint is null, take ts1 as the non-null one. Otherwise,
     * take ts1 as the lesser endpoint.
     */
    if (ts1IsNull && te1IsNull)
        PG_RETURN_NULL();
    swap_ts_and_te(ts1IsNull, te1IsNull, ts1, te1, false);

    /* Likewise for interval 2. */
    if (ts2IsNull && te2IsNull)
        PG_RETURN_NULL();
    swap_ts_and_te(ts2IsNull, te2IsNull, ts2, te2, false);

    /*
     * At this point neither ts1 nor ts2 is null, so we can consider three
     * cases: ts1 > ts2, ts1 < ts2, ts1 = ts2
     */
    if (TIMEADT_GT(ts1, ts2)) {
        /*
         * This case is ts1 < te2 OR te1 < te2, which may look redundant but
         * in the presence of nulls it's not quite completely so.
         */
        if (te2IsNull)
            PG_RETURN_NULL();
        if (TIMEADT_LT(ts1, te2))
            PG_RETURN_BOOL(true);
        if (te1IsNull)
            PG_RETURN_NULL();

        /*
         * If te1 is not null then we had ts1 <= te1 above, and we just found
         * ts1 >= te2, hence te1 >= te2.
         */
        PG_RETURN_BOOL(false);
    } else if (TIMEADT_LT(ts1, ts2)) {
        /* This case is ts2 < te1 OR te2 < te1 */
        if (te1IsNull)
            PG_RETURN_NULL();
        if (TIMEADT_LT(ts2, te1))
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

#undef TIMEADT_GT
#undef TIMEADT_LT
}

/* timestamp_time()
 * Convert timestamp to time data type.
 */
Datum timestamp_time(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    TimeADT result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_NULL();

    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

#ifdef HAVE_INT64_TIMESTAMP

    /*
     * Could also do this with time = (timestamp / USECS_PER_DAY *
     * USECS_PER_DAY) - timestamp;
     */
    result = ((((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec) * USECS_PER_SEC) + fsec;
#else
    result = ((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec + fsec;
#endif

    PG_RETURN_TIMEADT(result);
}

/* timestamptz_time()
 * Convert timestamptz to time data type.
 */
Datum timestamptz_time(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMP(0);
    TimeADT result;
    struct pg_tm tt, *tm = &tt;
    int tz;
    fsec_t fsec;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_NULL();

    if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

#ifdef HAVE_INT64_TIMESTAMP

    /*
     * Could also do this with time = (timestamp / USECS_PER_DAY *
     * USECS_PER_DAY) - timestamp;
     */
    result = ((((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec) * USECS_PER_SEC) + fsec;
#else
    result = ((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec + fsec;
#endif

    PG_RETURN_TIMEADT(result);
}

/* datetime_timestamp()
 * Convert date and time to timestamp data type.
 */
Datum datetime_timestamp(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    TimeADT time = PG_GETARG_TIMEADT(1);
    Timestamp result;

    result = date2timestamp(date);
    if (!TIMESTAMP_NOT_FINITE(result))
        result += time;

    PG_RETURN_TIMESTAMP(result);
}

/* time_interval()
 * Convert time to interval data type.
 */
Datum time_interval(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    result->time = time;
    result->day = 0;
    result->month = 0;

    PG_RETURN_INTERVAL_P(result);
}

/* interval_time()
 * Convert interval to time data type.
 *
 * This is defined as producing the fractional-day portion of the interval.
 * Therefore, we can just ignore the months field.	It is not real clear
 * what to do with negative intervals, but we choose to subtract the floor,
 * so that, say, '-2 hours' becomes '22:00:00'.
 */
Datum interval_time(PG_FUNCTION_ARGS)
{
    Interval* span = PG_GETARG_INTERVAL_P(0);
    TimeADT result;

#ifdef HAVE_INT64_TIMESTAMP
    int64 days;

    result = span->time;
    if (result >= USECS_PER_DAY) {
        days = result / USECS_PER_DAY;
        result -= days * USECS_PER_DAY;
    } else if (result < 0) {
        days = (-result + USECS_PER_DAY - 1) / USECS_PER_DAY;
        result += days * USECS_PER_DAY;
    }
#else
    result = span->time;
    if (result >= (double)SECS_PER_DAY || result < 0)
        result -= floor(result / (double)SECS_PER_DAY) * (double)SECS_PER_DAY;
#endif

    PG_RETURN_TIMEADT(result);
}

/* time_mi_time()
 * Subtract two times to produce an interval.
 */
Datum time_mi_time(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    TimeADT time2 = PG_GETARG_TIMEADT(1);
    Interval* result = NULL;

    result = (Interval*)palloc(sizeof(Interval));

    result->month = 0;
    result->day = 0;
    result->time = time1 - time2;

    PG_RETURN_INTERVAL_P(result);
}

/* time_pl_interval()
 * Add interval to time.
 */
Datum time_pl_interval(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    TimeADT result;

    result = time + span->time;

#ifdef HAVE_INT64_TIMESTAMP
    if (result <= -B_FORMAT_MAX_TIME_USECS || result >= B_FORMAT_MAX_TIME_USECS)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#else
    if (result <= (double)(-B_FORMAT_MAX_TIME_USECS) || result >= (double)B_FORMAT_MAX_TIME_USECS)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#endif

    PG_RETURN_TIMEADT(result);
}

/* time_mi_interval()
 * Subtract interval from time.
 */
Datum time_mi_interval(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    TimeADT result;

    result = time - span->time;

#ifdef HAVE_INT64_TIMESTAMP
    if (result <= -B_FORMAT_MAX_TIME_USECS || result >= B_FORMAT_MAX_TIME_USECS)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#else
    if (result <= (double)(-B_FORMAT_MAX_TIME_USECS) || result >= (double)B_FORMAT_MAX_TIME_USECS)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#endif

    PG_RETURN_TIMEADT(result);
}

/* time_part()
 * Extract specified field from time type.
 */
Datum time_part(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    TimeADT time = PG_GETARG_TIMEADT(1);
    float8 result;
    int type, val;
    char* lowunits = NULL;

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        fsec_t fsec;
        struct pg_tm tt, *tm = &tt;

        time2tm(time, tm, &fsec);

        switch (val) {
            case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
#ifdef DOLPHIN
                result = fsec;
#else
                result = tm->tm_sec * 1000000.0 + fsec;
#endif
#else
#ifdef DOLPHIN
                result = fsec * 1000000;
#else
                result = (tm->tm_sec + fsec) * 1000000;
#endif
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
#ifdef DOLPHIN
                result = tm->tm_sec;
#else
#ifdef HAVE_INT64_TIMESTAMP
                result = tm->tm_sec + fsec / 1000000.0;
#else
                result = tm->tm_sec + fsec;
#endif
#endif
                break;

            case DTK_MINUTE:
                result = tm->tm_min;
                break;

            case DTK_HOUR:
                result = tm->tm_hour;
                break;

#ifdef DOLPHIN
            case DTK_DOW:
            case DTK_ISODOW:
                {
                    int day = tm->tm_hour / 24;
                    int tz;
                    if (timestamp2tm(GetCurrentTimestamp(), &tz, tm, &fsec, NULL, NULL) != 0)
                        ereport(ERROR,
                            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                    result = j2day(date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) + day);
                    if (val == DTK_ISODOW && result == 0)
                        result = 7;
                    break;
                }

            case DTK_DAY:
                {
                    int tz;
                    if (timestamp2tm(GetCurrentTimestamp(), &tz, tm, &fsec, NULL, NULL) != 0)
                        ereport(ERROR,
                            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                    result = tm->tm_mday;
                    break;
                }

            case DTK_DOY:
                {
                    int tz;
                    if (timestamp2tm(GetCurrentTimestamp(), &tz, tm, &fsec, NULL, NULL) != 0)
                        ereport(ERROR,
                            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                    result = (date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - date2j(tm->tm_year, 1, 1) + 1);
                    break;
                }
            
            case DTK_QUARTER:
                {
                    int tz;
                    if (timestamp2tm(GetCurrentTimestamp(), &tz, tm, &fsec, NULL, NULL) != 0)
                        ereport(ERROR,
                            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
                    result = (tm->tm_mon - 1) / MONTH_TO_QUARTER_RADIX + 1;
                    break;
                }
#endif

            case DTK_TZ:
            case DTK_TZ_MINUTE:
            case DTK_TZ_HOUR:
#ifndef DOLPHIN
            case DTK_DAY:
            case DTK_QUARTER:
#endif
            case DTK_MONTH:
            case DTK_YEAR:
            case DTK_DECADE:
            case DTK_CENTURY:
            case DTK_MILLENNIUM:
            case DTK_ISOYEAR:
            default:
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("\"time\" units \"%s\" not recognized", lowunits)));
                result = 0;
        }
    } else if (type == RESERV && val == DTK_EPOCH) {
#ifdef HAVE_INT64_TIMESTAMP
        result = time / 1000000.0;
#else
        result = time;
#endif
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("\"time\" units \"%s\" not recognized", lowunits)));
        result = 0;
    }

    PG_RETURN_FLOAT8(result);
}

/*****************************************************************************
 *	 Time With Time Zone ADT
 *****************************************************************************/

/* tm2timetz()
 * Convert a tm structure to a time data type.
 */
static int tm2timetz(struct pg_tm* tm, fsec_t fsec, int tz, TimeTzADT* result)
{
#ifdef HAVE_INT64_TIMESTAMP
    result->time =
        ((((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec) * USECS_PER_SEC) + fsec;
#else
    result->time = ((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec + fsec;
#endif
    result->zone = tz;

    return 0;
}

Datum timetz_in(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimeTzADT* result = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tz = 0;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char* field[MAXDATEFIELDS] = {0};
    int dtype;
    int ftype[MAXDATEFIELDS];

    dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
    if (dterr == 0)
        dterr = DecodeTimeOnly(field, ftype, nf, &dtype, tm, &fsec, &tz);
    if (dterr != 0) {
        DateTimeParseError(dterr, str, "time with time zone", fcinfo->can_ignore);
        /*
         * can_ignore == true means hint string "ignore_error" used. warning report instead of error.
         * then we will return 00:00:xx if the first 1 or 2 character is lower than 60, otherwise return 00:00:00
         */
        char* field_str = field[0];
        if (field_str == NULL) {
            tm->tm_sec = 0;
        } else {
            if (*field_str == '+') {
                field_str++;
            }
            int trunc_val = getStartingDigits(field_str);
            tm->tm_sec = (trunc_val < 0 || trunc_val >= 60) ? 0 : trunc_val;
        }
        tm->tm_hour = 0;
        tm->tm_min = 0;
        fsec = 0;
    }

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));
    tm2timetz(tm, fsec, tz, result);
    AdjustTimeForTypmod(&(result->time), typmod);

    PG_RETURN_TIMETZADT_P(result);
}

Datum timetz_out(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    char* result = NULL;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz;
    char buf[MAXDATELEN + 1];

    timetz2tm(time, tm, &fsec, &tz);
    EncodeTimeOnly(tm, fsec, true, tz, u_sess->time_cxt.DateStyle, buf);

    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *		timetz_recv			- converts external binary format to timetz
 */
Datum timetz_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimeTzADT* result = NULL;

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

#ifdef HAVE_INT64_TIMESTAMP
    result->time = pq_getmsgint64(buf);

    if (result->time < INT64CONST(0) || result->time > USECS_PER_DAY)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#else
    result->time = pq_getmsgfloat8(buf);

    if (result->time < 0 || result->time > (double)SECS_PER_DAY)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
#endif

    result->zone = pq_getmsgint(buf, sizeof(result->zone));

    /* Check for sane GMT displacement; see notes in datatype/timestamp.h */
    if (result->zone <= -TZDISP_LIMIT || result->zone >= TZDISP_LIMIT)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE), errmsg("time zone displacement out of range")));

    AdjustTimeForTypmod(&(result->time), typmod);

    PG_RETURN_TIMETZADT_P(result);
}

/*
 *		timetz_send			- converts timetz to binary format
 */
Datum timetz_send(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
#ifdef HAVE_INT64_TIMESTAMP
    pq_sendint64(&buf, time->time);
#else
    pq_sendfloat8(&buf, time->time);
#endif
    pq_sendint32(&buf, time->zone);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum timetztypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);

    PG_RETURN_INT32(anytime_typmodin(true, ta));
}

Datum timetztypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    PG_RETURN_CSTRING(anytime_typmodout(true, typmod));
}

/* timetz2tm()
 * Convert TIME WITH TIME ZONE data type to POSIX time structure.
 */
#ifdef DOLPHIN
int timetz2tm(TimeTzADT* time, struct pg_tm* tm, fsec_t* fsec, int* tzp)
#else
static int timetz2tm(TimeTzADT* time, struct pg_tm* tm, fsec_t* fsec, int* tzp)
#endif
{
    TimeOffset trem = time->time;

#ifdef HAVE_INT64_TIMESTAMP
    tm->tm_hour = trem / USECS_PER_HOUR;
    trem -= tm->tm_hour * USECS_PER_HOUR;
    tm->tm_min = trem / USECS_PER_MINUTE;
    trem -= tm->tm_min * USECS_PER_MINUTE;
    tm->tm_sec = trem / USECS_PER_SEC;
    *fsec = trem - tm->tm_sec * USECS_PER_SEC;
#else
recalc:
    TMODULO(trem, tm->tm_hour, (double)SECS_PER_HOUR);
    TMODULO(trem, tm->tm_min, (double)SECS_PER_MINUTE);
    TMODULO(trem, tm->tm_sec, 1.0);
    trem = TIMEROUND(trem);
    /* roundoff may need to propagate to higher-order fields */
    if (trem >= 1.0) {
        trem = ceil(time->time);
        goto recalc;
    }
    *fsec = trem;
#endif

    if (tzp != NULL)
        *tzp = time->zone;

    return 0;
}

/* timetz_scale()
 * Adjust time type for specified scale factor.
 * Used by openGauss type system to stuff columns.
 */
Datum timetz_scale(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    int32 typmod = PG_GETARG_INT32(1);
    TimeTzADT* result = NULL;

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

    result->time = time->time;
    result->zone = time->zone;

    AdjustTimeForTypmod(&(result->time), typmod);

    PG_RETURN_TIMETZADT_P(result);
}

static int timetz_cmp_internal(TimeTzADT* time1, TimeTzADT* time2)
{
    TimeOffset t1, t2;

    /* Primary sort is by true (GMT-equivalent) time */
#ifdef HAVE_INT64_TIMESTAMP
    t1 = time1->time + (time1->zone * USECS_PER_SEC);
    t2 = time2->time + (time2->zone * USECS_PER_SEC);
#else
    t1 = time1->time + time1->zone;
    t2 = time2->time + time2->zone;
#endif

    if (t1 > t2)
        return 1;
    if (t1 < t2)
        return -1;

    /*
     * If same GMT time, sort by timezone; we only want to say that two
     * timetz's are equal if both the time and zone parts are equal.
     */
    if (time1->zone > time2->zone)
        return 1;
    if (time1->zone < time2->zone)
        return -1;

    return 0;
}

Datum timetz_eq_withhead(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = (TimeTzADT*)((char*)PG_GETARG_DATUM(0) + VARHDRSZ_SHORT);
    TimeTzADT* time2 = (TimeTzADT*)((char*)PG_GETARG_DATUM(1) + VARHDRSZ_SHORT);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) == 0);
}

Datum timetz_eq(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) == 0);
}

Datum timetz_ne(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) != 0);
}

Datum timetz_lt(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) < 0);
}

Datum timetz_le(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) <= 0);
}

Datum timetz_gt(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) > 0);
}

Datum timetz_ge(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_BOOL(timetz_cmp_internal(time1, time2) >= 0);
}

Datum timetz_cmp(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);

    PG_RETURN_INT32(timetz_cmp_internal(time1, time2));
}

Datum timetz_hash(PG_FUNCTION_ARGS)
{
    TimeTzADT* key = PG_GETARG_TIMETZADT_P(0);
    uint32 thash;

    /*
     * To avoid any problems with padding bytes in the struct, we figure the
     * field hashes separately and XOR them.  This also provides a convenient
     * framework for dealing with the fact that the time field might be either
     * double or int64.
     */
#ifdef HAVE_INT64_TIMESTAMP
    thash = DatumGetUInt32(DirectFunctionCall1(hashint8, Int64GetDatumFast(key->time)));
#else
    thash = DatumGetUInt32(DirectFunctionCall1(hashfloat8, Float8GetDatumFast(key->time)));
#endif
    thash ^= DatumGetUInt32(hash_uint32(key->zone));
    PG_RETURN_UINT32(thash);
}

Datum timetz_larger(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);
    TimeTzADT* result = NULL;

    if (timetz_cmp_internal(time1, time2) > 0)
        result = time1;
    else
        result = time2;
    PG_RETURN_TIMETZADT_P(result);
}

Datum timetz_smaller(PG_FUNCTION_ARGS)
{
    TimeTzADT* time1 = PG_GETARG_TIMETZADT_P(0);
    TimeTzADT* time2 = PG_GETARG_TIMETZADT_P(1);
    TimeTzADT* result = NULL;

    if (timetz_cmp_internal(time1, time2) < 0)
        result = time1;
    else
        result = time2;
    PG_RETURN_TIMETZADT_P(result);
}

/* timetz_pl_interval()
 * Add interval to timetz.
 */
Datum timetz_pl_interval(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    TimeTzADT* result = NULL;

#ifndef HAVE_INT64_TIMESTAMP
    TimeTzADT time1;
#endif

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

#ifdef HAVE_INT64_TIMESTAMP
    result->time = time->time + span->time;
    result->time -= result->time / USECS_PER_DAY * USECS_PER_DAY;
    if (result->time < INT64CONST(0))
        result->time += USECS_PER_DAY;
#else
    result->time = time->time + span->time;
    TMODULO(result->time, time1.time, (double)SECS_PER_DAY);
    if (result->time < 0)
        result->time += SECS_PER_DAY;
#endif

    result->zone = time->zone;

    PG_RETURN_TIMETZADT_P(result);
}

/* timetz_mi_interval()
 * Subtract interval from timetz.
 */
Datum timetz_mi_interval(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    Interval* span = PG_GETARG_INTERVAL_P(1);
    TimeTzADT* result = NULL;

#ifndef HAVE_INT64_TIMESTAMP
    TimeTzADT time1;
#endif

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

#ifdef HAVE_INT64_TIMESTAMP
    result->time = time->time - span->time;
    result->time -= result->time / USECS_PER_DAY * USECS_PER_DAY;
    if (result->time < INT64CONST(0))
        result->time += USECS_PER_DAY;
#else
    result->time = time->time - span->time;
    TMODULO(result->time, time1.time, (double)SECS_PER_DAY);
    if (result->time < 0)
        result->time += SECS_PER_DAY;
#endif

    result->zone = time->zone;

    PG_RETURN_TIMETZADT_P(result);
}

/* overlaps_timetz() --- implements the SQL92 OVERLAPS operator.
 *
 * Algorithm is per SQL92 spec.  This is much harder than you'd think
 * because the spec requires us to deliver a non-null answer in some cases
 * where some of the inputs are null.
 */
Datum overlaps_timetz(PG_FUNCTION_ARGS)
{
    /*
     * The arguments are TimeTzADT *, but we leave them as generic Datums for
     * convenience of notation --- and to avoid dereferencing nulls.
     */
    Datum ts1 = PG_GETARG_DATUM(0);
    Datum te1 = PG_GETARG_DATUM(1);
    Datum ts2 = PG_GETARG_DATUM(2);
    Datum te2 = PG_GETARG_DATUM(3);
    bool ts1IsNull = PG_ARGISNULL(0);
    bool te1IsNull = PG_ARGISNULL(1);
    bool ts2IsNull = PG_ARGISNULL(2);
    bool te2IsNull = PG_ARGISNULL(3);

#define TIMETZ_GT(t1, t2) DatumGetBool(DirectFunctionCall2(timetz_gt, t1, t2))
#define TIMETZ_LT(t1, t2) DatumGetBool(DirectFunctionCall2(timetz_lt, t1, t2))

    /*
     * If both endpoints of interval 1 are null, the result is null (unknown).
     * If just one endpoint is null, take ts1 as the non-null one. Otherwise,
     * take ts1 as the lesser endpoint.
     */
    if (ts1IsNull && te1IsNull)
        PG_RETURN_NULL();
    swap_ts_and_te(ts1IsNull, te1IsNull, ts1, te1, true);

    /* Likewise for interval 2. */
    if (ts2IsNull && te2IsNull)
        PG_RETURN_NULL();
    swap_ts_and_te(ts2IsNull, te2IsNull, ts2, te2, true);

    /*
     * At this point neither ts1 nor ts2 is null, so we can consider three
     * cases: ts1 > ts2, ts1 < ts2, ts1 = ts2
     */
    if (TIMETZ_GT(ts1, ts2)) {
        /*
         * This case is ts1 < te2 OR te1 < te2, which may look redundant but
         * in the presence of nulls it's not quite completely so.
         */
        if (te2IsNull)
            PG_RETURN_NULL();
        if (TIMETZ_LT(ts1, te2))
            PG_RETURN_BOOL(true);
        if (te1IsNull)
            PG_RETURN_NULL();

        /*
         * If te1 is not null then we had ts1 <= te1 above, and we just found
         * ts1 >= te2, hence te1 >= te2.
         */
        PG_RETURN_BOOL(false);
    } else if (TIMETZ_LT(ts1, ts2)) {
        /* This case is ts2 < te1 OR te2 < te1 */
        if (te1IsNull)
            PG_RETURN_NULL();
        if (TIMETZ_LT(ts2, te1))
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

#undef TIMETZ_GT
#undef TIMETZ_LT
}

Datum timetz_time(PG_FUNCTION_ARGS)
{
    TimeTzADT* timetz = PG_GETARG_TIMETZADT_P(0);
    TimeADT result;

    /* swallow the time zone and just return the time */
    result = timetz->time;

    PG_RETURN_TIMEADT(result);
}

Datum time_timetz(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    TimeTzADT* result = NULL;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz;

    GetCurrentDateTime(tm);
    time2tm(time, tm, &fsec);
    tz = DetermineTimeZoneOffset(tm, session_timezone);

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

    result->time = time;
    result->zone = tz;

    PG_RETURN_TIMETZADT_P(result);
}

/* timestamptz_timetz()
 * Convert timestamp to timetz data type.
 */
Datum timestamptz_timetz(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMP(0);
    TimeTzADT* result = NULL;
    struct pg_tm tt, *tm = &tt;
    int tz;
    fsec_t fsec;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_NULL();

    if (timestamp2tm(timestamp, &tz, tm, &fsec, NULL, NULL) != 0)
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

    tm2timetz(tm, fsec, tz, result);

    PG_RETURN_TIMETZADT_P(result);
}

/* datetimetz_timestamptz()
 * Convert date and timetz to timestamp with time zone data type.
 * Timestamp is stored in GMT, so add the time zone
 * stored with the timetz to the result.
 * - thomas 2000-03-10
 */
Datum datetimetz_timestamptz(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(1);
    TimestampTz result;

    if (DATE_IS_NOBEGIN(date))
        TIMESTAMP_NOBEGIN(result);
    else if (DATE_IS_NOEND(date))
        TIMESTAMP_NOEND(result);
    else {
#ifdef HAVE_INT64_TIMESTAMP
        result = date * USECS_PER_DAY + time->time + time->zone * USECS_PER_SEC;
#else
        result = date * (double)SECS_PER_DAY + time->time + time->zone;
#endif
    }

    PG_RETURN_TIMESTAMP(result);
}

/* timetz_part()
 * Extract specified field from time type.
 */
Datum timetz_part(PG_FUNCTION_ARGS)
{
    text* units = PG_GETARG_TEXT_PP(0);
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(1);
    float8 result;
    int type, val;
    char* lowunits = NULL;

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        double dummy;
        int tz;
        fsec_t fsec;
        struct pg_tm tt, *tm = &tt;

        timetz2tm(time, tm, &fsec, &tz);

        switch (val) {
            case DTK_TZ:
                result = -tz;
                break;

            case DTK_TZ_MINUTE:
                result = -tz;
                result /= SECS_PER_MINUTE;
                FMODULO(result, dummy, (double)SECS_PER_MINUTE);
                break;

            case DTK_TZ_HOUR:
                dummy = -tz;
                FMODULO(dummy, result, (double)SECS_PER_HOUR);
                break;

            case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
#ifdef DOLPHIN
                result = fsec;
#else
                result = tm->tm_sec * 1000000.0 + fsec;
#endif
#else
#ifdef DOLPHIN
                result = fsec * 1000000;
#else
                result = (tm->tm_sec + fsec) * 1000000;
#endif
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
#ifdef DOLPHIN
                result = tm->tm_sec;
#else
#ifdef HAVE_INT64_TIMESTAMP
                result = tm->tm_sec + fsec / 1000000.0;
#else
                result = tm->tm_sec + fsec;
#endif
#endif
                break;

            case DTK_MINUTE:
                result = tm->tm_min;
                break;

            case DTK_HOUR:
                result = tm->tm_hour;
                break;

            case DTK_DAY:
            case DTK_MONTH:
            case DTK_QUARTER:
            case DTK_YEAR:
            case DTK_DECADE:
            case DTK_CENTURY:
            case DTK_MILLENNIUM:
            default:
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("\"time with time zone\" units \"%s\" not recognized", lowunits)));
                result = 0;
        }
    } else if (type == RESERV && val == DTK_EPOCH) {
#ifdef HAVE_INT64_TIMESTAMP
        result = time->time / 1000000.0 + time->zone;
#else
        result = time->time + time->zone;
#endif
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("\"time with time zone\" units \"%s\" not recognized", lowunits)));
        result = 0;
    }

    PG_RETURN_FLOAT8(result);
}

/* timetz_zone()
 * Encode time with time zone type with specified time zone.
 * Applies DST rules as of the current date.
 */
Datum timetz_zone(PG_FUNCTION_ARGS)
{
    text* zone = PG_GETARG_TEXT_PP(0);
    TimeTzADT* t = PG_GETARG_TIMETZADT_P(1);
    TimeTzADT* result = NULL;
    int tz;
    char tzname[TZ_STRLEN_MAX + 1];
    char* lowzone = NULL;
    int type, val;
    pg_tz* tzp = NULL;

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

    if (type == TZ || type == DTZ)
        tz = val * MINS_PER_HOUR;
    else {
        tzp = pg_tzset(tzname);
        if (tzp != NULL) {
            /* Get the offset-from-GMT that is valid today for the zone */
            pg_time_t now = (pg_time_t)time(NULL);
            struct pg_tm* tm;

            tm = pg_localtime(&now, tzp);
            tz = -tm->tm_gmtoff;
        } else {
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("time zone \"%s\" not recognized", tzname)));
            tz = 0; /* keep compiler quiet */
        }
    }

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

#ifdef HAVE_INT64_TIMESTAMP
    result->time = t->time + (t->zone - tz) * USECS_PER_SEC;
    while (result->time < INT64CONST(0))
        result->time += USECS_PER_DAY;
    while (result->time >= USECS_PER_DAY)
        result->time -= USECS_PER_DAY;
#else
    result->time = t->time + (t->zone - tz);
    while (result->time < 0)
        result->time += SECS_PER_DAY;
    while (result->time >= SECS_PER_DAY)
        result->time -= SECS_PER_DAY;
#endif

    result->zone = tz;

    PG_RETURN_TIMETZADT_P(result);
}

/* timetz_izone()
 * Encode time with time zone type with specified time interval as time zone.
 */
Datum timetz_izone(PG_FUNCTION_ARGS)
{
    Interval* zone = PG_GETARG_INTERVAL_P(0);
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(1);
    TimeTzADT* result = NULL;
    int tz;

    if (zone->month != 0)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("\"interval\" time zone \"%s\" not valid",
                    DatumGetCString(DirectFunctionCall1(interval_out, PointerGetDatum(zone))))));

#ifdef HAVE_INT64_TIMESTAMP
    tz = -(zone->time / USECS_PER_SEC);
#else
    tz = -(zone->time);
#endif

    result = (TimeTzADT*)palloc(sizeof(TimeTzADT));

#ifdef HAVE_INT64_TIMESTAMP
    result->time = time->time + (time->zone - tz) * USECS_PER_SEC;
    while (result->time < INT64CONST(0))
        result->time += USECS_PER_DAY;
    while (result->time >= USECS_PER_DAY)
        result->time -= USECS_PER_DAY;
#else
    result->time = time->time + (time->zone - tz);
    while (result->time < 0)
        result->time += SECS_PER_DAY;
    while (result->time >= SECS_PER_DAY)
        result->time -= SECS_PER_DAY;
#endif

    result->zone = tz;

    PG_RETURN_TIMETZADT_P(result);
}

/* GetPartValue()
 * get part values from timestamp
 */
static float8 GetPartValue(Timestamp timestamp, int type, int val, char* lowunits)
{
    float8 result = 0;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;

    if (type == UNITS) {
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
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
    return result;
}

/*
 * @Description: The internal realization of function vtimestamp_part: retrieves subfields such as year or hour from
 * date/time values
 * @in  parg1 - an identifier or string that selects what field to extract from the source value.
 * @in  parg2 - the source value to be extracted.
 * @in  vresult - a vector to save function result
 * @in  idx -current row
 */
static void vtimestamp_part_internal(ScalarVector* parg1, ScalarVector* parg2, ScalarVector* presult, int idx)
{
    uint8* flag1 = parg1->m_flag;
    uint8* flag2 = parg2->m_flag;
    float8 result = 0;
    int val = 0;
    char* lowunits = NULL;
    int type;
    text* arg1 = NULL;
    Timestamp arg2 = 0;

    if (BOTH_NOT_NULL(flag1[idx], flag2[idx])) {
        /* parg1 may be not constant, should fetch arg by idx. */
        arg1 = DatumGetTextPP(parg1->m_vals[idx]);
        arg2 = DatumGetTimestamp(parg2->m_vals[idx]);

        lowunits = downcase_truncate_identifier(VARDATA_ANY(arg1), VARSIZE_ANY_EXHDR(arg1), false);

        type = DecodeUnits(0, lowunits, &val);
        if (UNKNOWN_FIELD == type)
            type = DecodeSpecial(0, lowunits, &val);

        result = GetPartValue(arg2, type, val, lowunits);
        presult->m_vals[idx] = Float8GetDatum(result);
        SET_NOTNULL(presult->m_flag[idx]);

    } else
        SET_NULL(presult->m_flag[idx]);
}

/* timestamp_part()
 * Extract specified field from timestamp.
 * vectorize function
 */
ScalarVector* vtimestamp_part(PG_FUNCTION_ARGS)
{
    ScalarVector* VecParg1 = PG_GETARG_VECTOR(0);
    ScalarVector* VecParg2 = PG_GETARG_VECTOR(1);
    int32 nvalues = PG_GETARG_INT32(2);
    ScalarVector* presult = PG_GETARG_VECTOR(3);
    bool* pselection = PG_GETARG_SELECTION(4);

    if (pselection != NULL) {
        for (int i = 0; i < nvalues; i++) {
            if (pselection[i]) {
                vtimestamp_part_internal(VecParg1, VecParg2, presult, i);
            }
        }
    } else {
        for (int i = 0; i < nvalues; i++) {
            vtimestamp_part_internal(VecParg1, VecParg2, presult, i);
        }
    }

    presult->m_rows = nvalues;
    presult->m_desc.typeId = FLOAT8OID;
    presult->m_desc.encoded = false;

    return presult;
}
#ifdef DOLPHIN

/**
 * @Description: Convert a time string to a pg_tm struct. The parsing logic
 * is compatible with the str_to_time() function in MySQL
 * @param [in] str : A string in full TIMESTAMP format or TIME format
 * @param [out] tm : to save time value
 * @param [out] fsec: to save fractional second part of time
 * @param [out] timeSign: to save sign of time
 * @param [out] tm_type: return the time type corresponding to str.
 * @param [out] warnings: return true when an parse exception occurs
 *
 * @return true if success, otherwise false.
 */
bool cstring_to_time(const char *str, pg_tm *tm, fsec_t &fsec, int &timeSign, int &tm_type, bool &warnings, bool* null_func_result)
{
    size_t length = strlen(str);
    ulong date[4]; /* 0~3 correspond to: days, hours, minutes, seconds in turn*/
    uint pos;
    int nano = 0;
    const char *end = str + length, *end_of_days;
    bool found_days, found_hours;
    uint64 value;
    errno_t rc;

    bool is_internal_format = false; /* If true, str is a number format(HHMMSS[.fsec]) */

    /* init */
    rc = memset_s(date, sizeof(date), 0, sizeof(date));
    securec_check(rc, "\0", "\0");
    fsec = 0;
    warnings = false;

    /* Skip space at start */
    for (; str != end && isspace((unsigned char)*str); str++)
        length--;

    /* Determine positive or negative time */
    if (str != end && *str == '-') {
        timeSign = -1;
        str++;
        length--;
    }
    if (str == end) {
        *null_func_result = true;
        warnings = true;
        return false; // error format
    }

    /* Check first if str is a full TIMESTAMP */
    if (length >= 12) {
        cstring_to_datetime(str, (TIME_FUZZY_DATE | TIME_DATETIME_ONLY), tm_type, tm, fsec, nano, warnings, null_func_result);
        if (nano >= 500) {
            fsec += 1; /* round */
        }
        if (tm_type >= DTK_ERROR) {
            return tm_type != DTK_ERROR;
        }
        fsec = 0;
        nano = 0;
        warnings = false;
        *null_func_result = false;
    }

    /* Not a timestamp. Try to get this as a DAYS_TO_SECOND string */
    for (value = 0; str != end && isdigit((unsigned char)*str); str++) {
        value = value * 10L + (long)(*str - '0');
    }

    if (value > UINT_MAX) {
        *null_func_result = true;
        return false;
    }

    /* Skip all space after 'days' */
    end_of_days = str;
    for (; str != end && isspace((unsigned char)*str); str++)
        ;

    pos = found_days = found_hours = 0;
    if ((end - str) > 1 && str != end_of_days && isdigit((unsigned char)*str)) {
        /* Found days part */
        date[0] = (ulong)value;
        pos = 1; /* Assume next is hours */
        found_days = 1;
    } else if ((end - str) > 1 && *str == ':' && isdigit((unsigned char)*(str + 1))) {
        date[0] = 0; /* not found days part. day set to 0 */
        date[1] = (ulong)value;
        pos = 2; /* Assume next is minutes */
        found_hours = 1;
        str++;
    } else {
        /* str is a number format */
        date[0] = 0;
        date[1] = (ulong)(value / 10000);
        date[2] = (ulong)(value / 100 % 100);
        date[3] = (ulong)(value % 100);
        pos = 4;
        is_internal_format = true;
    }

    if (!is_internal_format) {
        for (;;) {
            /* Parse the remaining time parts (excluding the fractional second part) */
            for (value = 0; str != end && isdigit((unsigned char)*str); str++) {
                value = value * 10L + (long)(*str - '0');
            }
            date[pos++] = (ulong)value;
            if (pos == 4 || (end - str) < 2 || *str != ':' || !isdigit((unsigned char)*(str + 1))) {
                break;
            }
            str++;
        }
    }

    /* Parse fractional second part */
    pos = 4;
    if (str != end && *str == '.') {
        int field_length = MAX_TIME_PRECISION;
        str++;
        for (value = 0; str != end && isdigit((unsigned char)*str); str++) {
            if (field_length-- > 0) {
                value = value * 10L + (long)(*str - '0');
            }
        }
        if (field_length >= 0) {
            value *= (long)pow_of_10[field_length];
        } else {
            /* Compatible with MySQL's special nanosecond carry mode */
            nano = (int)(*(str - 1) - '0') * pow_of_10[2];
        }
        fsec = (int32)value;
    }

    /* field overflow checks */
    if (date[0] > INT_MAX || date[1] > INT_MAX || date[2] > INT_MAX || date[3] > INT_MAX) {
        *null_func_result = true;
        return false; // overflow error
    }
    /* set pg_tm and type*/
    tm->tm_year = 0;
    tm->tm_mon = 0;
    tm->tm_mday = date[0];
    tm->tm_hour = date[1];
    tm->tm_min = date[2];
    tm->tm_sec = date[3];
    tm_type = DTK_TIME;

    if (!check_pg_tm_time_part(tm, fsec)) {
        *null_func_result = true;
        warnings = true; // warning: MYSQL_TIME_WARN_OUT_OF_RANGE
        return false;    // error: out of range
    }

    if (nano >= 500) {
        fsec += 1; /* round */
    }

    /* adjust time into supported time range*/
    adjust_time_range(tm, fsec, warnings);
    tm->tm_hour += HOURS_PER_DAY * tm->tm_mday;

    while (str != end) {
        if (!isspace((unsigned char)*str)) {
            warnings = true;
            break;
        }
        str++;
    }
    return true;
}

/**
 * @Description: Check if TIME parts are out of range.
 * @in tm - pg_tm value to be checked.
 * @in fsec - fractional second part in TIME
 * @return TRUE if any part is out of range. False if all parts is ok.
 */
bool check_pg_tm_time_part(pg_tm *tm, fsec_t fsec)
{
    return tm->tm_hour >= 0 && tm->tm_min >= 0 && tm->tm_min <= TIME_MAX_MINUTE && tm->tm_sec >= 0 &&
           tm->tm_sec <= TIME_MAX_SECOND && fsec >= 0 && fsec <= TIME_MAX_FRAC;
}

/**
 * @Description: Check if TIME value in pg_tm are out of range.
 * @in tm - pg_tm value to be checked.
 * @in fsec - fractional second part in TIME
 * NOTIC: Ensure that all time parts in tm are not out of range before input by check_pg_tm_time_part()
 *
 * @return false if time is out of range, true otherwise.
 */
bool check_pg_tm_time_range(pg_tm *tm, fsec_t fsec)
{
    int64 hour = (int64)tm->tm_hour + HOURS_PER_DAY * (int64)tm->tm_mday;
    if (hour <= TIME_MAX_HOUR &&
        (hour != TIME_MAX_HOUR || tm->tm_min != TIME_MAX_MINUTE || tm->tm_sec != TIME_MAX_SECOND || !fsec)) {
        return true;
    }
    return false;
}

/**
 * @Description: Adjust TIME value to lie in [-838:59:59, 838:59:59].
 * If TIME value lies outside of the range, set it to the closest endpoint of the range
 * and set warnings to true.
 * @param [in] tm : pg_tm value to be adjust.
 * @param [in] fsec - fractional second part in TIME
 * @param [out] warnings - Set to TRUE if time is out of range.
 * NOTIC: Ensure that all time parts in tm are not out of range before input by check_pg_tm_time_part()
 *
 * @return void
 */
void adjust_time_range(pg_tm *tm, fsec_t &fsec, bool &warnings)
{
    if (check_pg_tm_time_range(tm, fsec)) {
        return;
    }
    /* If time out of range, set max time value */
    fsec = 0;
    tm->tm_mday = 0;
    tm->tm_hour = TIME_MAX_HOUR;
    tm->tm_min = TIME_MAX_MINUTE;
    tm->tm_sec = TIME_MAX_SECOND;
    warnings = true;
}

/* makedate()
 * @Description: Convert yyyy.ddd into date type
 * @return: Convert result, date type.
 */
Datum makedate(PG_FUNCTION_ARGS)
{
    int64 year = PG_GETARG_INT64(0);
    int64 dayofyear = PG_GETARG_INT64(1);
    DateADT date;

    if (year < B_FORMAT_MIN_YEAR_OF_DATE || year > B_FORMAT_MAX_YEAR_OF_DATE || 
        dayofyear <= 0 || dayofyear >= (int64)pow_of_10[7])
        PG_RETURN_NULL();

    if (YEAR2_IN_RANGE(year))
        year = YEAR2_ADJUST_VALUE_20C_21C(year);

    date = date2j(year, 1, 0) + dayofyear;
    date -= POSTGRES_EPOCH_JDATE;
    /* Limit day number from year 0001-12-31 to 9999-12-31 */
    if (date < B_FORMAT_DATEADT_MIN_VALUE || date > B_FORMAT_DATEADT_MAX_VALUE)
        PG_RETURN_NULL();

    PG_RETURN_DATEADT(date);
}

/*
 * Check whether the TimeADT value is within the specified range: 
 * [-838:59:59, 838:59:59] (the time range is from MySQL).
 * for case select time('xxx') : should return null if the TimeADT value exceeds the range
 * and for other case : Error will be reported if the TimeADT value exceeds the range.
 */
void check_b_format_time_range_with_ereport(TimeADT &time, bool can_ignore, bool* result_isnull)
{
    int level = (can_ignore || !SQL_MODE_STRICT()) ? WARNING : ERROR;
    if (time < -B_FORMAT_TIME_MAX_VALUE || time > B_FORMAT_TIME_MAX_VALUE) {
        ereport(level, (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), 
                        errmsg("time field value out of range")));
        if (result_isnull != NULL) {
            *result_isnull = true;
        }
    }
}

/*
 * Check whether the Date value is within the specified range: 
 * [0000-01-01, 9999-12-31] (the time range is from MySQL).
 * Error will be reported if the Date value exceeds the range.
 */
void check_b_format_date_range_with_ereport(DateADT &date)
{
    if (date < B_FORMAT_DATEADT_MIN_VALUE || date > B_FORMAT_DATEADT_MAX_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                        errmsg("date field value out of range")));
    }
}

/* 
 * @Description: Keep timeADT in range(-B_FORMAT_TIME_MAX_VALUE, B_FORMAT_TIME_MAX_VALUE)
 * @return: bool, whether in range
 */
bool time_in_range(TimeADT &time)
{
    bool ret = (time >= -B_FORMAT_TIME_MAX_VALUE) && (time <= B_FORMAT_TIME_MAX_VALUE);
    if (time < -B_FORMAT_TIME_MAX_VALUE)
        time = -B_FORMAT_TIME_MAX_VALUE;
    if (time > B_FORMAT_TIME_MAX_VALUE)
        time = B_FORMAT_TIME_MAX_VALUE;
    return ret;
}

/* maketime()
 * @Description: Convert hh, min, sec into time type
 * @return: Convert result, time type.
 */
Datum maketime(PG_FUNCTION_ARGS)
{
    int64 hour = PG_GETARG_INT64(0);
    int64 minute = PG_GETARG_INT64(1);
    NumericVar tmp;
    lldiv_t seconds;
    init_var_from_num(PG_GETARG_NUMERIC(2), &tmp);
    if (!numeric_to_lldiv_t(&tmp, &seconds)) {
        PG_RETURN_NULL();
    }

    struct pg_tm tt;
    TimeADT time;
    unsigned int hour_abs;
    fsec_t fsec;
    bool overflow = 0;
    bool neg = 0;

    if (minute < 0 || minute > TIME_MAX_MINUTE || seconds.quot < 0 || seconds.quot > TIME_MAX_SECOND || seconds.rem < 0)
        PG_RETURN_NULL();

    if (hour < 0)
        neg = 1;

    hour_abs = (uint)((hour < 0 ? -hour : hour));

    if (-hour > UINT_MAX || hour > UINT_MAX || hour_abs > TIME_MAX_HOUR)
        overflow = 1;

    if (!overflow) {
        tt.tm_hour = (int)hour_abs;
        tt.tm_min = (int)minute;
        tt.tm_sec = (int)seconds.quot;
        fsec = static_cast<int32>(seconds.rem / NANO2MICRO_BASE);
        uint nanoseconds = seconds.rem % NANO2MICRO_BASE;
        if (nanoseconds >= HALF_NANO2MICRO_BASE) {
            fsec += 1;
        }
#ifndef HAVE_INT64_TIMESTAMP
        fsec /= USECS_PER_SEC;
#endif

        tm2time(&tt, fsec, &time);
        if (neg)
            time = -time;
        if (time >= -B_FORMAT_TIME_MAX_VALUE && time <= B_FORMAT_TIME_MAX_VALUE)
            PG_RETURN_TIMEADT(time);
    }

    time = neg ? -B_FORMAT_TIME_MAX_VALUE : B_FORMAT_TIME_MAX_VALUE;
    PG_RETURN_TIMEADT(time);
}

/* sec_to_time()
 * @Description: Convert seconds into time type
 * @return: Convert result, time type.
 */
Datum sec_to_time(PG_FUNCTION_ARGS)
{
    NumericVar tmp;
    lldiv_t seconds;
    init_var_from_num(PG_GETARG_NUMERIC(0), &tmp);
    struct pg_tm tt;
    TimeADT time;
    bool neg = 0;
    bool overflow = 0;

    if (!numeric_to_lldiv_t(&tmp, &seconds)) {
        overflow = 1;
    }

    if (seconds.quot < 0) {
        neg = 1;
        seconds.quot = -seconds.quot;
        seconds.rem = -seconds.rem;
    }

    if (seconds.quot > TIME_MAX_VALUE_SECONDS)
        overflow = 1;

    if (!overflow) {
        tt.tm_hour = 0;
        tt.tm_min = 0;
        tt.tm_sec = (int) seconds.quot; // keep only the integer part
        fsec_t fsec = static_cast<int32>(seconds.rem / NANO2MICRO_BASE);
        uint nanoseconds = seconds.rem % NANO2MICRO_BASE;
        if (nanoseconds >= HALF_NANO2MICRO_BASE) {
            fsec += 1;
        }
#ifndef HAVE_INT64_TIMESTAMP
        fsec /= USECS_PER_SEC;
#endif
        tm2time(&tt, fsec, &time);
        if (neg)
            time = -time;
    }

    if (overflow || time < -B_FORMAT_TIME_MAX_VALUE || time > B_FORMAT_TIME_MAX_VALUE)
        time = neg ? -B_FORMAT_TIME_MAX_VALUE : B_FORMAT_TIME_MAX_VALUE;
    PG_RETURN_TIMEADT(time);
}

/* 
 * The effect is the same as sec_to_time()
 * This function is used to receive string parameters.
 * Non pure numeric strings only retain the front numeric part.
 * For example, '2000.1abcd' will be converted to '2000.1'
 */
Datum sec_to_time_str(PG_FUNCTION_ARGS)
{
    text *tmp = PG_GETARG_TEXT_PP(0);
    char *str = text_to_cstring(tmp);
    int sign = 1;
    size_t len = strlen(str);
    const char *start = str;
    const char *end = str + len;
    /* Skip spaces */
    while (str < end && isspace((unsigned char)*str))
        str++;

    /* Skip signs */
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    if (str == end && SQL_MODE_STRICT()) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("Truncated incorrect numeric value: \"%s\"", start)));
    }

    char *numeric_str = (char *)extract_numericstr(str);
    Datum seconds =
        DirectFunctionCall3(numeric_in, CStringGetDatum(numeric_str), ObjectIdGetDatum(0), Int32GetDatum(-1));
    TimeADT time = DatumGetTimeADT(DirectFunctionCall1(sec_to_time, seconds));
    time *= sign;
    PG_RETURN_TIMEADT(time);
}

/*
 * @Description: Check if a string is in date format.
 * @return true if input string is in date format, otherwise false
 */
bool is_date_format(const char *str)
{
    int first_part_len;
    int number_of_parts;
    const char *pos = NULL;
    /* Skip space at start */
    for (; *str != '\0' && isspace((unsigned char)*str); str++)
        ;

    /*  Calculate number of digits in first part. */
    for (pos = str;
         *pos != '\0' && (isdigit((unsigned char)*pos));
         pos++)
        ;
    first_part_len = (uint)(pos - str);

    for (number_of_parts = 0;
         *str != '\0' && isdigit((unsigned char)*str);
         ++number_of_parts) {
        if (number_of_parts >= 3)
            return false;

        while (*str != '\0' && isdigit((unsigned char)*str++))
            ;

        while (*str != '\0') {
            if (isspace((unsigned char)*str)) {
                if (number_of_parts != 2)
                    return false; /* wrong format */
                str++;
                continue;
            }
            if (!ispunct((unsigned char)*str))
                break;
            str++;
        }
    }

    if (number_of_parts == 3 ||
        (number_of_parts == 1 && (first_part_len == 6 || first_part_len == 8)))
        return true;

    return false;
}

/*
 * @Description: The effect is the same as the date_in(). The difference 
 * is that this function does not report any errors, but instead return 
 * false. 
 * @return false if the input string cannot be converted to date, otherwise true.
*/
bool date_in_no_ereport(const char* str, DateADT *date)
{
    bool ret = true;
    MemoryContext current_ctx = CurrentMemoryContext;
    PG_TRY();
    {
        *date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(str)));
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

/*
 * @Description: The effect is the same as the time_in(). The difference
 * is that this function does not report any errors, but instead return
 * false.
 * @return false if the input string cannot be converted to time, otherwise true.
 */
bool time_in_without_overflow(const char *str, TimeADT *time, bool can_ignore)
{
    bool ret = true;
    MemoryContext current_ctx = CurrentMemoryContext;
    PG_TRY();
    {
        *time = DatumGetTimeADT(
            DirectFunctionCall3(time_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
    }
    PG_CATCH();
    {
        if (!can_ignore && SQL_MODE_STRICT()) {
            PG_RE_THROW();
        }
        // If catch an error, just empty the error stack and set return value to false.
        (void)MemoryContextSwitchTo(current_ctx);
        ErrorData* errdata = CopyErrorData();
        if (errdata->sqlerrcode == DTERR_FIELD_OVERFLOW) {
            ret = true;
            *time = B_FORMAT_TIME_MAX_VALUE;
        } else {
            ret = false;
        }
        ereport(WARNING, (errmsg("%s", errdata->message)));
        FlushErrorState();
        FreeErrorData(errdata);
    }
    PG_END_TRY();
    return ret;
}

/*
 * @Description: Subtract days from a date, giving a new date and assign it to result.
 * @return: false if parameter date or result out of range, otherwise true
 */
bool date_sub_days(DateADT date, int days, DateADT *result, bool is_add_func)
{
    if (date < B_FORMAT_DATEADT_MIN_VALUE || date > B_FORMAT_DATEADT_MAX_VALUE)
        return false;
    *result = date - days;
    if (*result < B_FORMAT_DATEADT_FIRST_YEAR || *result > B_FORMAT_DATEADT_MAX_VALUE) {
        if (*result >= B_FORMAT_DATEADT_MIN_VALUE && *result < B_FORMAT_DATEADT_FIRST_YEAR && is_add_func) {
            *result = DATE_ALL_ZERO_VALUE;
            return true;
        }
        return false;
    }

    return true;
}

/*
 * @Description: Subtract an interval from a date, giving a new date and assign it to result.
 * @return: false if parameter date or result out of range, otherwise true
 */
bool date_sub_interval(DateADT date, Interval *span, DateADT *result, bool is_add_func)
{
    if (date < B_FORMAT_DATEADT_MIN_VALUE || date > B_FORMAT_DATEADT_MAX_VALUE)
        return false;
    Timestamp temp =
        DatumGetTimestamp(DirectFunctionCall2(date_mi_interval, DateADTGetDatum(date), PointerGetDatum(span)));
    *result = DatumGetDateADT(timestamp2date(temp));
    if (*result < B_FORMAT_DATEADT_MIN_VALUE || *result > B_FORMAT_DATEADT_MAX_VALUE)
        return false;

    if (span->time == 0 && span->day == 0)
        return true;

    if (*result < B_FORMAT_DATEADT_FIRST_YEAR) {
        if (is_add_func) {
            *result = DATE_ALL_ZERO_VALUE;
            return true;
        }
        return false;
    }
    
    return true;
}

/**
 * Convert non-NULL values of the indicated types to the TimeADT value
 */
void convert_to_time(Datum value, Oid valuetypid, TimeADT *time)
{
    switch (valuetypid) {
        case UNKNOWNOID:
        case CSTRINGOID: {
            *time = DatumGetTimeADT(
                DirectFunctionCall3(time_in, value, ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
            break;
        }
        case CLOBOID:
        case NVARCHAR2OID:
        case BPCHAROID:
        case VARCHAROID:
        case TEXTOID: {
            char *str = TextDatumGetCString(value);
            *time = DatumGetTimeADT(
                DirectFunctionCall3(time_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
            break;
        }
        case TIMESTAMPOID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(timestamp_time, value));
            break;
        }
        case DATEOID: {
            *time = 0;  // time is set to 00:00:00
            break;
        }
        case TIMEOID: {
            *time = DatumGetTimeADT(value);
            break;
        }
        case INT1OID:
        case INT2OID:
        case INT4OID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(int32_b_format_time, value));
            break;
        }
        case INT8OID: {
            int8 int_val = DatumGetInt64(value);
            if (int_val > (int64)INT_MAX){
                ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                                errmsg("invalid input syntax for type time: \"%d\"", int_val)));
            } else {
                *time = DatumGetTimeADT(DirectFunctionCall1(int32_b_format_time, value));
            }
            break;
        }
        case NUMERICOID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(numeric_b_format_time, value));
            break;
        }
        case BOOLOID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(int32_b_format_time, 
                                    DirectFunctionCall1(bool_int4, value)));
            break;
        }
        case FLOAT4OID:
            *time = DatumGetTimeADT(DirectFunctionCall1(numeric_b_format_time, 
                                    DirectFunctionCall1(float4_numeric, value)));
            break;
        case FLOAT8OID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(numeric_b_format_time, 
                                    DirectFunctionCall1(float8_numeric, value)));
            break;
        }
        case TIMETZOID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(timetz_time, value));
            break;
        }
        case TIMESTAMPTZOID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(timestamptz_time, value));
            break;
        }
        case ABSTIMEOID: {
            *time = DatumGetTimeADT(DirectFunctionCall1(timestamp_time, 
                                    DirectFunctionCall1(abstime_timestamp, value)));
            break;
        }
        case BITOID: {
            if (SQL_MODE_STRICT()) {
                ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                                errmsg("unsupported input data type: %s", format_type_be(valuetypid))));
            }
            *time = 0;
            break;
        }
        default: {
            if (valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint1") ||
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint2") ||
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint4") ||
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint8")) {
                uint64 uint_val = DatumGetUInt64(value);
                if (uint_val > (uint64)INT_MAX) {
                    ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                                    errmsg("time out of range: \"%lu\"", uint_val)));
                } else {
                    *time = DatumGetTimeADT(DirectFunctionCall1(int32_b_format_time, value));
                }
            } else if (valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "year")) {
                *time = DatumGetTimeADT(DirectFunctionCall1(int32_b_format_time, 
                                        DirectFunctionCall1(year_integer, value)));
            } else {
                ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH), errmsg("unsupported input data type")));
            }
            break;
        }
    }
}

/**
 * @Description:Convert cstring values to the Timestamp or Date value according to its format.
 * Timestamp type cannot exceed this range: [0000-01-01 00:00:00, 9999-12-31 23:59:59.999999]
 * Date type cannot exceed this range: [0000-01-01, 9999-12-31]
 * Error will be reported if the above range is exceeded.
 * @return: Actual Timestamp or Date type oid. 
 */
Oid convert_cstring_to_datetime_date(const char* str, Timestamp *datetime, DateADT *date)
{
    if (is_date_format(str)){
        *date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(str)));
        check_b_format_date_range_with_ereport(*date);
        return DATEOID;
    }
    *datetime = DatumGetTimestamp(
        DirectFunctionCall3(timestamp_in, CStringGetDatum(str), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
    check_b_format_datetime_range_with_ereport(*datetime);
    return TIMESTAMPOID;
}

/**
 * Convert non-NULL values of the indicated types to the Timestamp or Date value
 * Timestamp type cannot exceed this range: [0000-01-01 00:00:00, 9999-12-31 23:59:59.999999]
 * Date type cannot exceed this range: [0000-01-01, 9999-12-31]
 * Error will be reported if the above range is exceeded.
 * @return: TIMESTAMPOID or DATEOID. 
 */
Oid convert_to_datetime_date(Datum value, Oid valuetypid, Timestamp *datetime, DateADT *date)
{
    switch (valuetypid) {
        case UNKNOWNOID:
        case CSTRINGOID: {
            return convert_cstring_to_datetime_date(DatumGetCString(value), datetime, date);
        }
        case CLOBOID:
        case NVARCHAR2OID:
        case BPCHAROID:
        case VARCHAROID:
        case TEXTOID: {
            char *str = TextDatumGetCString(value);
            return convert_cstring_to_datetime_date(str, datetime, date);
        }
        case TIMESTAMPOID:
        case TIMESTAMPTZOID:
        case ABSTIMEOID: 
        case TIMEOID:
        case TIMETZOID:{
            convert_to_datetime(value, valuetypid, datetime);
            check_b_format_datetime_range_with_ereport(*datetime);
            return TIMESTAMPOID;
        }
        case DATEOID: {
            *date = DatumGetDateADT(value);
            check_b_format_date_range_with_ereport(*date);
            return DATEOID;
        }
        case BOOLOID:{
            *date = DatumGetTimestamp(DirectFunctionCall1(int32_b_format_date, 
                                        DirectFunctionCall1(bool_int4, value)));
            check_b_format_date_range_with_ereport(*date);
            return DATEOID;
        }
        case INT1OID:
        case INT2OID:
        case INT4OID:
        case INT8OID:{
            int64 number = DatumGetInt64(value);
            if (number >= (int64)pow_of_10[8]) {
                /* Fuzzy boundary is used. The exact boundary is 99991231*/
                convert_to_datetime(value, valuetypid, datetime);
                check_b_format_datetime_range_with_ereport(*datetime);
                return TIMESTAMPOID;
            }
            *date = DatumGetTimestamp(DirectFunctionCall1(int32_b_format_date, value));
            check_b_format_date_range_with_ereport(*date);
            return DATEOID;
        }
        case NUMERICOID: {
            Datum bound = DirectFunctionCall1(int8_numeric, Int64GetDatum((int64)pow_of_10[8]));
            if (DirectFunctionCall2(numeric_ge, value, bound)) {
                convert_to_datetime(value, valuetypid, datetime);
                check_b_format_datetime_range_with_ereport(*datetime);
                return TIMESTAMPOID;
            } else {
                NumericVar tmp;
                lldiv_t val;
                init_var_from_num(DatumGetNumeric(value), &tmp);
                if (!numeric_to_lldiv_t(&tmp, &val) ||  val.rem != 0) {
                    ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                                    errmsg("invalid input syntax for type date")));
                }
                *date = DatumGetTimestamp(DirectFunctionCall1(int32_b_format_date, Int64GetDatum(val.quot)));
                check_b_format_date_range_with_ereport(*date);
                return DATEOID;
            }
           
        }
        case FLOAT4OID: 
        case FLOAT8OID: {
            return convert_to_datetime_date(DirectFunctionCall1(float8_numeric, value), 
                                            NUMERICOID, datetime, date);
        }
        default: {
            if (valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint1") || 
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint2") || 
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint4") ||
                valuetypid == get_typeoid(PG_CATALOG_NAMESPACE, "uint8")) {
                uint64 uint_val = DatumGetUInt64(value);
                if (uint_val >= (uint64)pow_of_10[8]) {
                    convert_to_datetime(value, valuetypid, datetime);
                    check_b_format_datetime_range_with_ereport(*datetime);
                    return TIMESTAMPOID;
                } else {
                    *date = DatumGetTimestamp(DirectFunctionCall1(int32_b_format_date, value));
                    check_b_format_date_range_with_ereport(*date);
                    return DATEOID;
                }
            }
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("unsupported input data type: %s", format_type_be(valuetypid))));
            return InvalidOid;
        }
    }
}

/* subdate(Text, int64)
 * @Description: Subtract days from a date/datetime converted from text, giving a new date/datetime.
 * The return type is the same as the type of the first parameter (date/datetime). (The actual return
 * type is CString, and the return type is distinguished by a string in date or datetime format)
 */
Datum subdate_datetime_days_text(PG_FUNCTION_ARGS)
{
    text* tmp = PG_GETARG_TEXT_PP(0);
    int64 days = PG_GETARG_INT64(1);
    char *expr;
    int elevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;

    expr = text_to_cstring(tmp);

    if (is_date_format(expr)) {
        DateADT date, result;
        date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(expr)));
        if (date_sub_days(date, days, &result, false)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(date_text, result);
        }
        ereport(elevel,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date/time field value out of range")));
        PG_RETURN_NULL();
    } else {
        Timestamp datetime, result;
        datetime = DatumGetTimestamp(
            DirectFunctionCall3(timestamp_in, CStringGetDatum(expr), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
        if (datetime_sub_days(datetime, days, &result)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
        }
        ereport(elevel,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date/time field value out of range")));
        PG_RETURN_NULL();
    }
}

/* subdate(Text, Interval expr unit)
 * @Description: Subtract days from a date/datetime converted from text, giving a new date/datetime.
 * The return type is generally the same as the type of the first parameter (date/datetime).An exception
 * is when the first parameter is of type date and interval contains hours, minutes or seconds,
 * then the return type is datetime. (The actual return type is CString, and the return type is
 * distinguished by a string in dete or datetime format)
 */
Datum subdate_datetime_interval_text(PG_FUNCTION_ARGS)
{
    text* tmp = PG_GETARG_TEXT_PP(0);
    Interval *span = PG_GETARG_INTERVAL_P(1);
    char *expr;
    int elevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;

    expr = text_to_cstring(tmp);

    if (is_date_format(expr) && span->time == 0) {
        DateADT date, result;
        date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(expr)));
        if (date_sub_interval(date, span, &result))
            return DirectFunctionCall1(date_text, result);
        ereport(elevel,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date/time field value out of range")));
        PG_RETURN_NULL();
    } else {
        Timestamp datetime, result;
        datetime = DatumGetTimestamp(
            DirectFunctionCall3(timestamp_in, CStringGetDatum(expr), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
        if (datetime_sub_interval(datetime, span, &result))
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
        ereport(elevel,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date/time field value out of range")));
        PG_RETURN_NULL();
    }
}

/* subdate(time, int64)
 * @Description: Subtract days from a time value, giving a new time.The time
 * value must be a primitive time type and not automatically implicitly converted
 * from a string).
 */
Datum subdate_time_days(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    int64 days = PG_GETARG_INT64(1);
    TimeADT time2;
    int elevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;

#ifdef HAVE_INT64_TIMESTAMP
    time2 = days * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE * USECS_PER_SEC;
#else
    time2 = days * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE;
#endif
    time -= time2;
    if (time >= -B_FORMAT_TIME_MAX_VALUE && time <= B_FORMAT_TIME_MAX_VALUE) {
        PG_RETURN_TIMEADT(time);
    }
    ereport(elevel,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
             errmsg("time field value out of range")));
    PG_RETURN_NULL();
}

/* subdate(time, interval)
 * @Description: Subtract interval from a time value, giving a new time. The time
 * value must be a primitive time type and not automatically implicitly converted
 * from a string).
 */
Datum subdate_time_interval(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    Interval *span = PG_GETARG_INTERVAL_P(1);
    TimeADT time2 = span->time;
    int elevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;
    if (span->month != 0) {
        ereport(elevel,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("time field value out of range")));
        PG_RETURN_NULL();
    }
#ifdef HAVE_INT64_TIMESTAMP
    time2 += (span->day * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE * USECS_PER_SEC);
#else
    time2 += (span->day * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE);
#endif
    time -= time2;
    if (time >= -B_FORMAT_TIME_MAX_VALUE && time <= B_FORMAT_TIME_MAX_VALUE) {
        PG_RETURN_TIMEADT(time);
    }
    ereport(elevel,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
             errmsg("time field value out of range")));
    PG_RETURN_NULL();
}

/* 
 * @Description: Determine if a string contains a time type and translate str to 
 * pg_tm struct. The rules of judgment and decode is the same as time_in() but 
 * datetime parsing rules for numeric format are additionally added.
*/
void str_to_pg_tm(char *str, pg_tm &tt, fsec_t &fsec, int &timeSign) 
{
    int tz;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char *field[MAXDATEFIELDS];
    int dtype;
    int ftype[MAXDATEFIELDS];
    int D = 0;

    tt.tm_year = 0;
    tt.tm_mon = 0;
    tt.tm_mday = 0;

    /* check if empty */
    if (strlen(str) == 0) {
        tt.tm_hour = 0;
        tt.tm_min = 0;
        tt.tm_sec = 0;
        fsec = 0;
        return;
    }
    bool hasD = false;
    char *adjusted = adjust_b_format_time(str, &timeSign, &D, &hasD);
    dterr = ParseDateTime(adjusted, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
    if (dterr != 0)
        DateTimeParseError(dterr, str, "time");
    if (dterr == 0) {
        if (ftype[0] == DTK_NUMBER && nf == 1) {
            /* for example: str = "2 121212" , "231034.1234" */
            if (NumberTime(false, field[0], &tt, &fsec, D, hasD) && NumberTimestamp(str, &tt, &fsec)){
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                         errmsg("invalid input syntax for type %s: \"%s\"", "time", str)));
            }
        } else {
            dterr = DecodeTimeOnlyForBDatabase(field, ftype, nf, &dtype, &tt, &fsec, &tz, D);
            if (dterr != 0)
                DateTimeParseError(dterr, str, "time");
        }
    }
    if (tt.tm_mday > 0) {
        // If the input string is recognized as datetime
        // determine whether its date is legal or not
        if (tt.tm_hour >= HOURS_PER_DAY || tt.tm_year > B_FORMAT_MAX_YEAR_OF_DATE) {
            ereport(ERROR,
                    (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                     errmsg("date/time field value out of range: \"%s\"", str)));
        }
    }
}

/*
 * @Description: Compatible with function time(expr) in mysql. Extracts the time part
 * of the time or datetime expression expr and returns it as a string.
 * @return: formatted string.
 */
Datum time_mysql(PG_FUNCTION_ARGS)
{
    TimeADT time;
    TimeADT result = 0;
    Timestamp datetime;
    Oid val_type;

    bool result_isnull = false;
    val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    val_type = convert_to_datetime_time(PG_GETARG_DATUM(0), val_type, &datetime, &time,
                                        fcinfo->can_ignore, &result_isnull);

    if (result_isnull) {
        PG_RETURN_NULL();
    }

    switch (val_type) {
        case TIMEOID: {
            result = time;
            break;
        }
        case TIMESTAMPOID:
        case DATEOID: {
            result = DatumGetTimeADT(DirectFunctionCall1(timestamp_time, TimeADTGetDatum(datetime)));
            break;
        }
        default: {
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("unsupported input data type: %s", format_type_be(val_type))));
        }
    }
    PG_RETURN_TIMEADT(result);
}

/**
 * @Description: Given a parameter of type datetime, calculate the number of days since year 0. 
 * @return: the number of days since year 0 to the given date.
 */
Datum to_days(PG_FUNCTION_ARGS) {
    Timestamp datetime = PG_GETARG_TIMESTAMP(0);
    int64 days;

    if (datetime == TIMESTAMP_ZERO) {
        PG_RETURN_NULL();
    }

    if (datetime < B_FORMAT_TIMESTAMP_MIN_VALUE || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE) {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                        errmsg("datetime value out of range")));
    }
    days = (datetime - B_FORMAT_TIMESTAMP_MIN_VALUE) / USECS_PER_DAY;

    if (datetime <= B_FORMAT_TIMESTAMP_ZERO_YEAR_LEAP_DAY) {
        ++days;
    }

    PG_RETURN_INT64(days);
}

/**
 * utc_date_func()
 * @return: UTC date
 */
Datum utc_date_func(PG_FUNCTION_ARGS)
{
    TimestampTz now = GetCurrentStmtsysTimestamp();
    PG_RETURN_DATEADT(timestamp2date((Timestamp)now));
}

/**
 * utc_time_func()
 * @return: UTC time
 */
Datum utc_time_func(PG_FUNCTION_ARGS) 
{
    int32 typmod = PG_GETARG_INT32(0);
    if (typmod < 0 || typmod > MAX_TIME_PRECISION) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("time(%d) precision must be between %d and %d", typmod, 0, MAX_TIMESTAMP_PRECISION)));
    }

    TimestampTz now = GetCurrentStmtsysTimestamp();
    TimeADT result = DatumGetTimeADT(DirectFunctionCall1(timestamp_time, (Timestamp)now));
    AdjustTimeForTypmod(&result, typmod);
    PG_RETURN_TIMEADT(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(date_bool);
extern "C" DLL_PUBLIC Datum date_bool(PG_FUNCTION_ARGS);

Datum date_bool(PG_FUNCTION_ARGS)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    Timestamp timestamp;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    timestamp = date2timestamp(dateVal);

    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0) {
        if (tm->tm_year == 0 && tm->tm_mon == 0 && tm->tm_mday == 0)
            PG_RETURN_BOOL(false);
        else
            PG_RETURN_BOOL(true);
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1_PUBLIC(time_bool);
extern "C" DLL_PUBLIC Datum time_bool(PG_FUNCTION_ARGS);

Datum time_bool(PG_FUNCTION_ARGS)
{
    TimeADT timeVal = PG_GETARG_TIMEADT(0);
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (time2tm(timeVal, tm, &fsec) == 0) {
        if (tm->tm_hour == 0 && tm->tm_min == 0 && tm->tm_sec == 0)
            PG_RETURN_BOOL(false);
        else
            PG_RETURN_BOOL(true);
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
    }
    PG_RETURN_BOOL(false);
}

/**
 * The function is similar to time_in, but uses flag to control the parsing process.
 *
 * return value :
 * true : there is no warning on str to time, str is correct
 * false : there are some warnings on str to time, str is incorrect
 */
bool time_in_with_flag_internal(char *str, struct pg_tm *result_tm, fsec_t *result_fsec, int *result_timeSign,
    unsigned int date_flag, bool vertify_time = false, bool can_ignore = false)
{
    struct pg_tm tt1, *tm = &tt1;
    fsec_t fsec = 0;
    int tm_type;
    bool warnings;
    int timeSign = 1;
    bool null_func_result = false;
    errno_t rc = memset_s(tm, sizeof(struct pg_tm), 0, sizeof(struct pg_tm));
    securec_check(rc, "\0", "\0");
    cstring_to_time(str, tm, fsec, timeSign, tm_type, warnings, &null_func_result);

    if (!warnings && vertify_time) {
        warnings = !CheckDatetimeRange(tm, fsec, DTK_TIME) || !CheckDateRange(tm, non_zero_date(tm), 0);
    }

    if (warnings) {
        int errlevel = ((SQL_MODE_STRICT() || null_func_result) && !can_ignore) ? ERROR : WARNING;
        ereport(errlevel,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect time value: \"%s\"", str)));
    }

    rc = memcpy_s(result_tm, sizeof(struct pg_tm), tm, sizeof(struct pg_tm));
    securec_check(rc, "\0", "\0");
    *result_fsec = fsec;
    *result_timeSign = timeSign;
    return vertify_time ? !warnings : true;
}

bool time_in_with_flag(char *str, unsigned int date_flag, TimeADT* time_adt, bool vertify_time, bool can_ignore)
{
    TimeADT result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int timeSign;
    bool vertify_time_result = time_in_with_flag_internal(str, tm, &fsec, &timeSign, date_flag, vertify_time,
        can_ignore);
    if (!vertify_time_result) {
        return false;
    }
    tm2time(tm, fsec, &result);
    AdjustTimeForTypmod(&result, TIMESTAMP_MAX_PRECISION);
    result *= timeSign;
    *time_adt = result;
    return true;
}

/**
 * time parser for function extract
 */
bool time_to_tm_with_sql_mode(char *str, struct pg_tm *tm, fsec_t *fsec, int *timeSign, unsigned int date_flag)
{
	TimeADT time;
    bool ret = true;
    int code;
    const char *msg = NULL;
    PG_TRY();
    {
        time_in_with_flag_internal(str, tm, fsec, timeSign, date_flag);
		tm2time(tm, *fsec, &time);
		AdjustTimeForTypmod(&time, TIMESTAMP_MAX_PRECISION);
		time2tm(time, tm, fsec);
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

/* time_to_sec()
 * @param time
 * @return seconds of the given time
 */
Datum time_to_sec(PG_FUNCTION_ARGS)
{
    TimeADT time;
    char *time_str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    pg_tm result_tt;
    pg_tm* result_tm = &result_tt;
    fsec_t fsec;
    int64 result;
    int32 timeSign = 1;

    if (!time_in_with_sql_mode(time_str, &time, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH), false, fcinfo->can_ignore)) {
        PG_RETURN_NULL();
    }
    if (time < 0) {
        timeSign = -1;
        time = -time;
    }
    time2tm(time, result_tm, &fsec);
    result = ((result_tm->tm_hour * MINS_PER_HOUR + result_tm->tm_min) * SECS_PER_MINUTE) + result_tm->tm_sec;
    result *= timeSign;
    PG_RETURN_INT64(result);
}

/**
 * covert unmber to time
 * @param input_str: use for print
 * @param nr: convert time input
 * @param can_ignore: Indicates whether to report an error when an error occurs.
 * @param time: timeStamp destination
 * @param tm: long long tm destination
 * @return: succss or not
 */
bool number_to_time(char* input_str, long long nr, bool can_ignore, TimeADT* time, LongLongTm* tm)
{
    int errlevel = (SQL_MODE_STRICT() && !can_ignore) ? ERROR : WARNING;
    if (nr < -TIME_MAX_VALUE) {
        ereport(errlevel, (errmsg("Truncated incorrect time value: \"%s\"", input_str)));
        return false;
    }
    return longlong_to_tm(nr, time, tm, can_ignore);
}

bool number_to_time(long long nr, bool can_ignore, TimeADT* time, LongLongTm* tm)
{
    char time_str[MAX_LONGLONG_TO_CHAR_LENGTH] = {0};
    errno_t errorno = sprintf_s(time_str, sizeof(time_str), "%lld", nr);
    securec_check_ss(errorno, "\0", "\0");
    return number_to_time(time_str, nr, can_ignore, time, tm);
}

/* int64_time_to_sec
 * @param time, type is int
 * @return seconds of the given time
 */
Datum int64_time_to_sec(PG_FUNCTION_ARGS)
{
    int64 input_time = PG_GETARG_INT64(0);
    TimeADT time;
    int64 result;
    LongLongTm longlongTm = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 1};
    
    if (!number_to_time(input_time, fcinfo->can_ignore, &time, &longlongTm)) {
        PG_RETURN_NULL();
    }
    pg_tm *result_tm = &longlongTm.result_tm;
    result = ((result_tm->tm_hour * MINS_PER_HOUR + result_tm->tm_min) * SECS_PER_MINUTE) + result_tm->tm_sec;
    result *= longlongTm.timeSign;
    PG_RETURN_INT64(result);
}

/* numeric_time_to_sec
 * @param time, type is numeric
 * @return seconds of the given time
 */
Datum numeric_time_to_sec(PG_FUNCTION_ARGS)
{
    Numeric num1 = PG_GETARG_NUMERIC(0);
    lldiv_t div1;
    int64 result;
    TimeADT time;
    LongLongTm longlongTm = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 1};
    Numeric_to_lldiv(num1, &div1);

    char* input_str = DatumGetCString(numeric_out_with_zero(fcinfo));

    if (!number_to_time(input_str, div1.quot, fcinfo->can_ignore, &time, &longlongTm)) {
        PG_RETURN_NULL();
    }
    
    div1.rem = div1.rem < 0 ? -div1.rem : div1.rem;

    pg_tm* result_tm = &longlongTm.result_tm;
    time_add_nanoseconds_with_round(input_str, result_tm, div1.rem, &longlongTm.fsec, fcinfo->can_ignore);
    result = ((result_tm->tm_hour * MINS_PER_HOUR + result_tm->tm_min) * SECS_PER_MINUTE) + result_tm->tm_sec;
    result *= longlongTm.timeSign;
    PG_RETURN_INT64(result);
}


/* timestamp_time_to_sec
 * @param time, type is timestamp
 * @return seconds of the given time
 */
Datum timestamp_time_to_sec(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    fsec_t fsec;
    pg_tm tt;
    pg_tm* tm = &tt;
    int64 result = 0;
    int errlevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_NULL();
       
    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0) {
        ereport(errlevel, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        PG_RETURN_NULL();
    }

    result = ((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec;
    PG_RETURN_INT64(result);
}


/* timestamptz_time_to_sec
 * @param time, type is timestamp
 * @return seconds of the given time
 */
Datum timestamptz_time_to_sec(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMPTZ(0);
    fsec_t fsec;
    pg_tm tt;
    pg_tm* tm = &tt;
    int64 result = 0;
    int tz;
    const char *tzn = NULL;
    int errlevel = (SQL_MODE_STRICT() && !fcinfo->can_ignore) ? ERROR : WARNING;

    if (TIMESTAMP_NOT_FINITE(timestamp))
        PG_RETURN_NULL();
       
    if (timestamp2tm(timestamp, &tz, tm, &fsec, &tzn, NULL) != 0) {
        ereport(errlevel, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        PG_RETURN_NULL();
    }

    result = ((tm->tm_hour * MINS_PER_HOUR + tm->tm_min) * SECS_PER_MINUTE) + tm->tm_sec;
    PG_RETURN_INT64(result);
}


/**
 * same as longlong_to_tm
 */
bool longlong_to_tm(long long nr, TimeADT* time, LongLongTm* tm, bool can_ignore)
{
    return longlong_to_tm(nr, time, &tm->result_tm, &tm->fsec, &tm->timeSign, can_ignore);
}
    
bool longlong_to_tm(long long nr, TimeADT* time, pg_tm* result_tm, fsec_t* fsec, int32* timeSign, bool can_ignore)
{
    errno_t errorno = EOK;
    char time_str[MAX_LONGLONG_TO_CHAR_LENGTH] = {0};
    errorno = sprintf_s(time_str, sizeof(time_str), "%lld", nr);
    securec_check_ss(errorno, "\0", "\0");
    *timeSign = 1;
    if (!time_in_with_sql_mode(time_str, time, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH), true, can_ignore)) {
        return false;
    }
    if (*time < 0) {
        *timeSign = -1;
        *time = -*time;
    }
    time2tm(*time, result_tm, fsec);
    return true;
}

static inline int32 datediff_internal(struct pg_tm* tm1, struct pg_tm* tm2)
{
    DateADT dt1, dt2;
    dt1 = date2j(tm1->tm_year, tm1->tm_mon, tm1->tm_mday);
    dt2 = date2j(tm2->tm_year, tm2->tm_mon, tm2->tm_mday);
    return (dt1 - dt2);
}

Datum datediff_t_t(PG_FUNCTION_ARGS)
{
    char *str1 = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char *str2 = text_to_cstring(PG_GETARG_TEXT_PP(1));

    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec1, fsec2;

    if (!datetime_in_with_sql_mode(str1, tm1, &fsec1, TIME_NO_ZERO_DATE) ||
        !datetime_in_with_sql_mode(str2, tm2, &fsec2, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }

    PG_RETURN_INT32(datediff_internal(tm1, tm2));
}

Datum datediff_t_n(PG_FUNCTION_ARGS)
{
    char *str = text_to_cstring(PG_GETARG_TEXT_PP(0));
    Numeric num = PG_GETARG_NUMERIC(1);
    lldiv_t div;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec;

    Numeric_to_lldiv(num, &div);
    if (!datetime_in_with_sql_mode(str, tm1, &fsec, TIME_NO_ZERO_DATE) ||
        !lldiv_decode_tm_with_sql_mode(num, &div, tm2, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT32(datediff_internal(tm1, tm2));
}

Datum datediff_n_n(PG_FUNCTION_ARGS)
{
    Numeric num1 = PG_GETARG_NUMERIC(0);
    Numeric num2 = PG_GETARG_NUMERIC(1);
    lldiv_t div1, div2;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;

    Numeric_to_lldiv(num1, &div1);
    Numeric_to_lldiv(num2, &div2);
    if (!lldiv_decode_tm_with_sql_mode(num1, &div1, tm1, NORMAL_DATE) || 
        !lldiv_decode_tm_with_sql_mode(num2, &div2, tm2, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }
    PG_RETURN_INT32(datediff_internal(tm1, tm2));
}

Datum datediff_time_t(PG_FUNCTION_ARGS)
{
    char *str2 = text_to_cstring(PG_GETARG_TEXT_PP(1));
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec1, fsec2;
    Timestamp datetime1;

    if (!datetime_in_with_sql_mode(str2, tm2, &fsec2, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }

    add_currentdate_to_time(PG_GETARG_TIMEADT(0), &datetime1);
    timestamp2tm(datetime1, NULL, tm1, &fsec1, NULL, NULL);
    PG_RETURN_INT32(datediff_internal(tm1, tm2));
}

Datum datediff_time_n(PG_FUNCTION_ARGS)
{
    Numeric num2 = PG_GETARG_NUMERIC(1);
    lldiv_t div2;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec1;
    Timestamp datetime1;
    
    Numeric_to_lldiv(num2, &div2);
    if (!lldiv_decode_tm_with_sql_mode(num2, &div2, tm2, NORMAL_DATE)) {
        PG_RETURN_NULL();
    }

    add_currentdate_to_time(PG_GETARG_TIMEADT(0), &datetime1);
    timestamp2tm(datetime1, NULL, tm1, &fsec1, NULL, NULL);
    PG_RETURN_INT32(datediff_internal(tm1, tm2));
}

Datum datediff_time_time(PG_FUNCTION_ARGS)
{
    Timestamp datetime1, datetime2;
    struct pg_tm tt1, *tm1 = &tt1;
    struct pg_tm tt2, *tm2 = &tt2;
    fsec_t fsec1, fsec2;
    add_currentdate_to_time(PG_GETARG_TIMEADT(0), &datetime1);
    add_currentdate_to_time(PG_GETARG_TIMEADT(1), &datetime2);
    timestamp2tm(datetime1, NULL, tm1, &fsec1, NULL, NULL);
    timestamp2tm(datetime2, NULL, tm2, &fsec2, NULL, NULL);
    PG_RETURN_INT32(datediff_internal(tm1, tm2));
}

static inline void from_days_internal(int64 days, Datum *result)
{
    DateADT date;
    struct pg_tm tt, *tm = &tt;
    char buf[MAXDATELEN + 1];
    if (days <= DAYS_PER_COMMON_YEAR || days >= FROM_DAYS_MAX_DAY) { //return '0000-00-00'
        tm->tm_year = tm->tm_mon = tm->tm_mday = 0;
    } else {
        date = date2j(1, 1, 1) - POSTGRES_EPOCH_JDATE;
        date += (days - DAYS_PER_LEAP_YEAR);
        j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    }
    EncodeDateOnlyForBDatabase(tm, u_sess->time_cxt.DateStyle, buf);
    *result = DirectFunctionCall1(textin, CStringGetDatum(buf));
}

Datum from_days_text(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    text* expr = PG_GETARG_TEXT_PP(0);
    const char *str = text_to_cstring(expr);
    char* endptr = NULL;
    errno = 0;
    int64 days;
    Datum result;
    const char *str_end = str + strlen(str);
    double tmp = strtod(str, &endptr);
    if (endptr == str || errno == ERANGE) {
        /* Supposing that we get a "0". LONG_LONG_MAX or LONG_LONG_MIN has the same result -- '0000-00-00'. */
        days = 0;
    } else {
        days = llround(tmp);
    }

    if (endptr != str_end) {
        ereport(errlevel,
            (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect INTEGER value: \"%s\"", str)));
    }
    from_days_internal(days, &result);
    PG_RETURN_DATUM(result);
}

Datum from_days_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    Datum result;
    int64 days;
    /* Days will be set to boundary value of INT64 if !SQL_MODE_STRICT(). */
    days = (int64)DirectFunctionCall1(numeric_int8, NumericGetDatum(num));
    from_days_internal(days, &result);
    PG_RETURN_DATUM(result);
}

/*
 * add an interval to a date, giving a new date and assign it to result.
 * false if parameter date or result out of range, otherwise true
 */
bool date_add_interval(DateADT date, Interval *span, DateADT *result)
{
    span->month = -span->month;
    span->day = -span->day;
    span->time = -span->time;
    return date_sub_interval(date, span, result, true);
}

/**
 * adddate(date/datetime, days)
*/
Datum adddate_datetime_days_t(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    text* tmp = PG_GETARG_TEXT_PP(0);
    int64 days = PG_GETARG_INT64(1);
    char *expr;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tm_type = DTK_NONE;

    expr = text_to_cstring(tmp);
    if (!datetime_in_with_sql_mode_internal(expr, tm, &fsec, tm_type, TIME_NO_ZERO_DATE)) {
        PG_RETURN_NULL();
    }

    if (tm_type == DTK_DATE) {
        DateADT date, result;
        date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
        if (date_sub_days(date, -days, &result, true)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(date_text, result);
        }
    } else {
        Timestamp datetime, result;
        tm2timestamp(tm, fsec, NULL, &datetime);
        if (datetime_sub_days(datetime, -days, &result, true)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
        }
    }
    ereport(errlevel,
        (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
            errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}

Datum adddate_datetime_days_n(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    Numeric num = PG_GETARG_NUMERIC(0);
    int64 days = PG_GETARG_INT64(1);
    struct pg_tm tt, *tm = &tt;
    lldiv_t div;
    fsec_t fsec;
    int date_type = 0;
    
    Numeric_to_lldiv(num, &div);
    if (!lldiv_decode_datetime(num, &div, tm, &fsec, NORMAL_DATE, &date_type)) {
        PG_RETURN_NULL();
    }

    if (date_type == DTK_DATE) {
        DateADT date, result;
        date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
        if (date_sub_days(date, -days, &result, true)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(date_text, result);
        }
    } else {
        Timestamp datetime, result;
        tm2timestamp(tm, fsec, NULL, &datetime);
        if (datetime_sub_days(datetime, -days, &result, true)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
        }
    }
    ereport(errlevel,
        (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
            errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}

/**
 * adddate(date/datetime, INTERVAL expr UNIT)
*/
Datum adddate_datetime_interval_t(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    text* tmp = PG_GETARG_TEXT_PP(0);
    Interval span = *PG_GETARG_INTERVAL_P(1);
    char *expr;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tm_type = DTK_NONE;

    expr = text_to_cstring(tmp);
    if (!datetime_in_with_sql_mode_internal(expr, tm, &fsec, tm_type, TIME_NO_ZERO_DATE, fcinfo->can_ignore)) {
        PG_RETURN_NULL();
    }

    if (tm_type == DTK_DATE && span.time == 0) {
        DateADT date, result;
        date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
        if (date_add_interval(date, &span, &result))
            return DirectFunctionCall1(date_text, result);
    } else {
        Timestamp datetime, result;
        tm2timestamp(tm, fsec, NULL, &datetime);
        if (datetime_add_interval(datetime, &span, &result))
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
    }
    ereport(errlevel,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}

Datum adddate_datetime_interval_n(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    Numeric num = PG_GETARG_NUMERIC(0);
    Interval span = *PG_GETARG_INTERVAL_P(1);
    struct pg_tm tt, *tm = &tt;
    lldiv_t div;
    fsec_t fsec;
    int date_type = 0;

    Numeric_to_lldiv(num, &div);
    if (!lldiv_decode_datetime(num, &div, tm, &fsec, NORMAL_DATE, &date_type)) {
        PG_RETURN_NULL();
    }

    if (date_type == DTK_DATE && span.time == 0) {
        DateADT date, result;
        date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
        if (date_add_interval(date, &span, &result))
            return DirectFunctionCall1(date_text, result);
    } else {
        Timestamp datetime, result;
        tm2timestamp(tm, fsec, NULL, &datetime);
        if (datetime_add_interval(datetime, &span, &result))
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
    }
    ereport(errlevel,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}

/**
 * adddate(time, days)
*/
Datum adddate_time_days(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    TimeADT time = PG_GETARG_TIMEADT(0);
    int64 days = PG_GETARG_INT64(1);
    TimeADT time2;

#ifdef HAVE_INT64_TIMESTAMP
    time2 = days * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE * USECS_PER_SEC;
#else
    time2 = days * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE;
#endif
    time += time2;
    if (time >= -B_FORMAT_TIME_MAX_VALUE && time <= B_FORMAT_TIME_MAX_VALUE) {
        PG_RETURN_TIMEADT(time);
    }
    ereport(errlevel,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
             errmsg("time field value out of range")));
    PG_RETURN_NULL();
}

/**
 * adddate(time, INTERVAL expr UNIT)
*/
Datum adddate_time_interval(PG_FUNCTION_ARGS)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    TimeADT time = PG_GETARG_TIMEADT(0);
    Interval *span = PG_GETARG_INTERVAL_P(1);
    TimeADT time2 = span->time;
    if (span->month != 0) {
        ereport(errlevel,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("time field value out of range")));
        PG_RETURN_NULL();
    }
#ifdef HAVE_INT64_TIMESTAMP
    time2 += (span->day * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE * USECS_PER_SEC);
#else
    time2 += (span->day * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE);
#endif
    time += time2;
    if (time >= -B_FORMAT_TIME_MAX_VALUE && time <= B_FORMAT_TIME_MAX_VALUE) {
        PG_RETURN_TIMEADT(time);
    }
    ereport(errlevel,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
             errmsg("time field value out of range")));
    PG_RETURN_NULL();
}

static int64 getPartFromTm(pg_tm* tm, fsec_t fsec, int part)
{
    int64 result = 0;
    switch (part) {
        case HOUR:
            result = tm->tm_hour;
            break;
        case MINUTE:
            result = tm->tm_min;
            break;
        case SECOND:
            result = tm->tm_sec;
            break;
        case MICROSECOND:
            result = fsec;
            break;
        default:
            break;
    }
    return result;
}

static inline Datum GetSpecificPartOfTime(PG_FUNCTION_ARGS, int part)
{
    char *tString = text_to_cstring(PG_GETARG_TEXT_PP(0));
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    if (*tString == '\0') {
        ereport(errlevel,
                (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                    errmsg("invalid input syntax for type time: \"%s\"", tString)));
        PG_RETURN_NULL();
    }
    struct pg_tm tt;
    struct pg_tm *tm = &tt;
    fsec_t fsec = 0;
    int timeSign = 1;
    bool warnings;
    int tm_type;
    bool null_func_result = false;
    if (!cstring_to_time(tString, tm, fsec, timeSign, tm_type, warnings, &null_func_result) || null_func_result) {
        PG_RETURN_NULL();
    }
    if (warnings) {
        int errlevel = (SQL_MODE_STRICT() || null_func_result) ? ERROR : WARNING;
        ereport(errlevel,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect time value: \"%s\"", tString)));
    }
    PG_RETURN_INT64(getPartFromTm(tm, fsec, part));
}

Datum GetHour(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTime(fcinfo, HOUR);
}

Datum GetMicrosecond(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTime(fcinfo, MICROSECOND);
}

Datum GetMinute(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTime(fcinfo, MINUTE);
}

Datum GetSecond(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTime(fcinfo, SECOND);
}

static Datum GetSpecificPartOfTimeInDate(PG_FUNCTION_ARGS, int part)
{
    DateADT dateVal = PG_GETARG_DATEADT(0);
    fsec_t fsec;
    pg_tm tt;
    pg_tm* tm = &tt;

    if (timestamp2tm(date2timestamp(dateVal), NULL, tm, &fsec, NULL, NULL) == 0) {
        PG_RETURN_INT64(getPartFromTm(tm, fsec, part));
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    PG_RETURN_INT64(0);
}

Datum GetHourFromDate(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInDate(fcinfo, HOUR);
}

Datum GetMicrosecondFromDate(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInDate(fcinfo, MICROSECOND);
}

Datum GetMinuteFromDate(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInDate(fcinfo, MINUTE);
}

Datum GetSecondFromDate(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInDate(fcinfo, SECOND);
}

static Datum GetSpecificPartOfTimeInTimeTz(PG_FUNCTION_ARGS, int part)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    pg_tm tt;
    pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    timetz2tm(time, tm, &fsec, &tz);
    PG_RETURN_INT64(getPartFromTm(tm, fsec, part));
}

Datum GetHourFromTimeTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimeTz(fcinfo, HOUR);
}

Datum GetMicrosecondFromTimeTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimeTz(fcinfo, MICROSECOND);
}

Datum GetMinuteFromTimeTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimeTz(fcinfo, MINUTE);
}

Datum GetSecondFromTimeTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimeTz(fcinfo, SECOND);
}

static Datum GetSpecificPartOfTimeInTimestampTz(PG_FUNCTION_ARGS, int part)
{
    TimestampTz time = PG_GETARG_TIMESTAMPTZ(0);
    pg_tm tt;
    pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    int64 result = 0;
    if (timestamp2tm(time, &tz, tm, &fsec, NULL, NULL) == 0) {
        switch (part) {
            case HOUR:
                result = tm->tm_hour;
                break;
            case MINUTE:
                result = tm->tm_min;
                break;
            case SECOND:
                result = tm->tm_sec;
                break;
            case MICROSECOND:
                result = fsec;
                break;
            default:
                break;
        }
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
    }
    PG_RETURN_INT64(result);
}

Datum GetHourFromTimestampTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimestampTz(fcinfo, HOUR);
}

Datum GetMicrosecondFromTimestampTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimestampTz(fcinfo, MICROSECOND);
}

Datum GetMinuteFromTimestampTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimestampTz(fcinfo, MINUTE);
}

Datum GetSecondFromTimestampTz(PG_FUNCTION_ARGS)
{
    return GetSpecificPartOfTimeInTimestampTz(fcinfo, SECOND);
}

bool time_in_with_sql_mode(char *str, TimeADT *result, unsigned int date_flag, bool vertify_time, bool can_ignore)
{
    bool ret = true;
    int code;
    const char *msg = NULL;
    PG_TRY();
    {
        ret = time_in_with_flag(str, date_flag, result, vertify_time, can_ignore);
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

static DateTimeFormat date_time_formats[] =
{
    {"usa", "%m.%d.%Y", "%Y-%m-%d %H.%i.%s", "%h:%i:%s %p" },
    {"jis", "%Y-%m-%d", "%Y-%m-%d %H:%i:%s", "%H:%i:%s" },
    {"iso", "%Y-%m-%d", "%Y-%m-%d %H:%i:%s", "%H:%i:%s" },
    {"eur", "%d.%m.%Y", "%Y-%m-%d %H.%i.%s", "%H.%i.%s" },
    {"internal", "%Y%m%d", "%Y%m%d%H%i%s", "%H%i%s" }
};
static int szdate_time_formats = sizeof date_time_formats / sizeof date_time_formats[0];

/** 
 * Function for B compatibility get_format(date/datetime/time, 'EUR'|'USA'|'JIS'|'ISO'|'INTERNAL') 
*/
Datum get_format(PG_FUNCTION_ARGS)
{
    int32 time_type = PG_GETARG_INT32(0);
    text* units = PG_GETARG_TEXT_PP(1);
    const char *result_str = NULL;

    char *lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);
    bool find_format = false;
    int i;
    for (i = 0; i < szdate_time_formats; ++i) {
        if (strcmp(lowunits, date_time_formats[i].format_name) == 0) {
            find_format = true;
            break;
        }
    }
    if (!find_format) {
        PG_RETURN_NULL();
    }
    switch (time_type)
    {
        case DTK_DATE:
            result_str = date_time_formats[i].date_format;
            break;
        case DTK_DATE_TIME:
            result_str = date_time_formats[i].datetime_format;
            break;
        case DTK_TIME:
            result_str = date_time_formats[i].time_format;
            break;
        default:
            Assert(0);  /* impossible */
            break;
    }

    PG_RETURN_DATUM(CStringGetTextDatum(result_str));
}

/**
 * parse unit symbol into an enum value
*/
bool resolve_units(char *unit_str, b_units *unit)
{
    for(int i = 0; i < units_size; i++) {
        if(strcmp(unit_str, unitnms[i]) == 0) {
            *unit = (b_units)i;
            return true;
        }
    }
    return false;
}

/**
 * return 1 if we should parse the expr in extract(unit from expr) into date or datetime
 * return 0 if we should parse it into time
*/
static inline int find_type(b_units unit)
{
    switch (unit)
    {
        case UNIT_YEAR:
        case UNIT_QUARTER:
        case UNIT_MONTH:
        case UNIT_WEEK:
        case UNIT_DAY:
        case UNIT_YEAR_MONTH:
            return 1;
        case UNIT_DAY_HOUR:
        case UNIT_DAY_MINUTE:
        case UNIT_DAY_SECOND:
        case UNIT_DAY_MICROSECOND:
        case UNIT_HOUR:
        case UNIT_MINUTE:
        case UNIT_SECOND:
        case UNIT_MICROSECOND:
        case UNIT_HOUR_MINUTE:
        case UNIT_HOUR_SECOND:
        case UNIT_MINUTE_SECOND:
        case UNIT_HOUR_MICROSECOND:
        case UNIT_MINUTE_MICROSECOND:
        case UNIT_SECOND_MICROSECOND:
            return 0;
        default:
            Assert(0);  // impossible
            break;
    }
    return 1;   // can not reach here, just keep compiler silence.
}

static inline int64 extract_internal(b_units enum_unit, struct pg_tm* tm, fsec_t fsec, int time_sign)
{
    int32 int_fsec;
    int64 result;
#ifdef HAVE_INT64_TIMESTAMP
    int_fsec = fsec;
#else
    int_fsec = (int32)(fsec * 1000000);
#endif

    int64 mul_2 = 100, mul_4 = 10000, mul_6 = 1000000;
    switch (enum_unit) {
        case UNIT_YEAR: {
            result = tm->tm_year;
            break;
        }
        case UNIT_QUARTER: {
            result = (tm->tm_mon + 2) / 3;
            break;
        }
        case UNIT_MONTH: {
            result = tm->tm_mon;
            break;
        }
        case UNIT_WEEK: {
            uint year = 0;
            result = b_db_cal_week(tm, b_db_weekmode(GetSessionContext()->default_week_format), &year);
            break;
        }
        case UNIT_DAY: {
            result = tm->tm_mday;
            break;
        }
        case UNIT_HOUR: {
            result = tm->tm_hour * time_sign;
            break;
        }
        case UNIT_MINUTE: {
            result = tm->tm_min * time_sign;
            break;
        }
        case UNIT_SECOND: {
            result = tm->tm_sec * time_sign;
            break;
        }
        case UNIT_MICROSECOND: {
            result = int_fsec * time_sign;
            break;
        }
        case UNIT_YEAR_MONTH: {
            result = tm->tm_year * mul_2 + tm->tm_mon;
            break;
        }
        case UNIT_DAY_HOUR: {
            result = (tm->tm_mday * mul_2 + tm->tm_hour) * time_sign;
            break;
        }
        case UNIT_DAY_MINUTE: {
            result = (tm->tm_mday * mul_4 + tm->tm_hour * mul_2 + tm->tm_min) * time_sign;
            break;
        }
        case UNIT_DAY_SECOND: {
            result = (tm->tm_mday * mul_6 + tm->tm_hour * mul_4 + tm->tm_min * mul_2 + tm->tm_sec) * time_sign;
            break;
        }
        case UNIT_HOUR_MINUTE: {
            result = (tm->tm_hour * mul_2 + tm->tm_min) * time_sign;
            break;
        }
        case UNIT_HOUR_SECOND: {
            result = (tm->tm_hour * mul_4 + tm->tm_min * mul_2 + tm->tm_sec) * time_sign;
            break;
        }
        case UNIT_MINUTE_SECOND: {
            result = (tm->tm_min * mul_2 + tm->tm_sec) * time_sign;
            break;
        }
        case UNIT_DAY_MICROSECOND: {
            result = ((tm->tm_mday * mul_6 + tm->tm_hour * mul_4 + tm->tm_min * mul_2 + tm->tm_sec) * mul_6 + int_fsec) * time_sign;
            break;
        }
        case UNIT_HOUR_MICROSECOND: {
            result = ((tm->tm_hour * mul_4 + tm->tm_min * mul_2 + tm->tm_sec) * mul_6 + int_fsec) * time_sign;
            break;
        }
        case UNIT_MINUTE_MICROSECOND: {
            result = ((tm->tm_min * mul_2 + tm->tm_sec) * mul_6 + int_fsec) * time_sign;
            break;
        }
        case UNIT_SECOND_MICROSECOND: {
            result = (tm->tm_sec * mul_6 + int_fsec) * time_sign;
            break;
        }
        default: {
            Assert(0);// impossible
            break;
        }
    }
    return result;
}

Datum time_xor_transfn(PG_FUNCTION_ARGS)
{
    TimeADT time;
    uint64 internal;
    /* On the first time through, we ignore the delimiter. */
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1)) {
        PG_RETURN_INT128(0);
    } else if (!PG_ARGISNULL(0) && PG_ARGISNULL(1)) {
        internal = (uint64)PG_GETARG_INT128(0);
        PG_RETURN_INT128((int128)internal);
    }
    time = PG_GETARG_TIMEADT(1);
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    time2tm(time, tm, &fsec);

    uint64 res = tmfsec2uint(tm);;
    if (!PG_ARGISNULL(0)) {
        internal = (uint64)PG_GETARG_INT128(0);
        PG_RETURN_INT128((int128)(res ^ internal));
    } else {
        PG_RETURN_INT128((int128)res);
    }
}

Datum timetz_xor_transfn(PG_FUNCTION_ARGS)
{
    TimeTzADT* time;
    uint64 internal;
    /* On the first time through, we ignore the delimiter. */
    if (PG_ARGISNULL(0) && PG_ARGISNULL(1)) {
        PG_RETURN_INT128(0);
    } else if (!PG_ARGISNULL(0) && PG_ARGISNULL(1)) {
        internal = (uint64)PG_GETARG_INT128(0);
        PG_RETURN_INT128((int128)internal);
    }
    time = PG_GETARG_TIMETZADT_P(1);
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int tz;
    timetz2tm(time, tm, &fsec, &tz);

    uint64 res = tmfsec2uint(tm);
    if (!PG_ARGISNULL(0)) {
        internal = (uint64)PG_GETARG_INT128(0);
        PG_RETURN_INT128((int128)(res ^ internal));
    } else {
        PG_RETURN_INT128((int128)res);
    }
}

Datum date_xor_transfn(PG_FUNCTION_ARGS) 
{
    int128 internal = 0;
    DateADT dateVal = 0;
    int128 res = 0;
    int year = 0;
    int month = 0;
    int day = 0;

    /* On the first time through, we ignore the delimiter. */
    if (!PG_ARGISNULL(0)) {
        internal = PG_GETARG_INT128(0);
    }

    if (!PG_ARGISNULL(1)) {
        dateVal = PG_GETARG_DATEADT(1);
        j2date(dateVal + POSTGRES_EPOCH_JDATE, &year, &month, &day);
        res = year*1e4+month*1e2+day;
    }

    PG_RETURN_INT128(res ^ internal);
}

Datum date_agg_finalfn(PG_FUNCTION_ARGS)
{
    int128 finalResult;
    /* cannot be called directly because of internal-type argument */
    Assert(AggCheckCallContext(fcinfo, NULL));

    finalResult = PG_ARGISNULL(0) ? 0 : (int128)PG_GETARG_INT128(0);

    PG_RETURN_INT128(finalResult);
}


Datum b_extract_text(PG_FUNCTION_ARGS)
{
    text *units = PG_GETARG_TEXT_PP(0);
    char *str = text_to_cstring(PG_GETARG_TEXT_PP(1));
    b_units enum_unit;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int time_sign = 1;

    char *lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);
    if (!resolve_units(lowunits, &enum_unit)) {
        ereport(ERROR,
                (errmodule(MOD_FUNCTION),
                errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                errmsg("units \"%s\" not supported", lowunits)));
    }
    if (find_type(enum_unit)) {
        if (!datetime_in_with_sql_mode(str, tm, &fsec, NO_ZERO_DATE_SET(), fcinfo->can_ignore)) {
            PG_RETURN_NULL();
        }
    } else {
        if (!time_to_tm_with_sql_mode(str, tm, &fsec, &time_sign, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH))) {
            PG_RETURN_NULL();
        }
    }

    PG_RETURN_INT64(extract_internal(enum_unit, tm, fsec, time_sign));
}

Datum b_extract_numeric(PG_FUNCTION_ARGS)
{
    text *units = PG_GETARG_TEXT_PP(0);
    Numeric num = PG_GETARG_NUMERIC(1);
    lldiv_t div;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec = 0;
    int time_sign = 1;
    b_units enum_unit;
    int date_type = 0;

    char *lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);
    if (!resolve_units(lowunits, &enum_unit)) {
        ereport(ERROR,
                (errmodule(MOD_FUNCTION),
                errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                errmsg("units \"%s\" not supported", lowunits)));
    }
    Numeric_to_lldiv(num, &div);
    if (find_type(enum_unit)) {
        if (IS_ZERO_NUMBER_DATE(div.quot, div.rem)) {
            fsec = 0;
            errno_t rc = memset_s(&tt, sizeof(tt), 0, sizeof(tt));
            securec_check(rc, "\0", "\0");
        } else if (!lldiv_decode_datetime(num, &div, tm, &fsec, NORMAL_DATE, &date_type)) {
            PG_RETURN_NULL();
        }
    } else {
        char *str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(num)));
        if (!time_to_tm_with_sql_mode(str, tm, &fsec, &time_sign, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH))) {
            PG_RETURN_NULL();
        }
    }

    PG_RETURN_INT64(extract_internal(enum_unit, tm, fsec, time_sign));
}

/* Transfor TimeADT into float8 formate
 * exp: "10:22:33.456" -> 102233.456
 */
Datum time_float(PG_FUNCTION_ARGS)
{
    TimeADT timeVal = PG_GETARG_TIMEADT(0);

    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    bool sig = 0;
    if (timeVal < 0) {
        timeVal *= -1;
        sig = 1;
    }
    time2tm(timeVal, tm, &fsec);

    float8 res = tmfsec2float(tm, fsec);

    if (sig) {
        PG_RETURN_FLOAT8(-res);
    } else {
        PG_RETURN_FLOAT8(res);
    }
}

Datum date_int(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    struct pg_tm tt;
    struct pg_tm *tm = &tt;
    
    if (unlikely(date > 0 && (INT_MAX - date < POSTGRES_EPOCH_JDATE))) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("input julian date is overflow")));
    }
    j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));

    int32 res = date2int(tm);
    PG_RETURN_INT32(res);
}

/**
  format frac part of the time as nanoseconds format
  for example it return .900000000 when src = .900
  for time, xx.900000000 is equal to .900
*/
long long align_to_nanoseconds(long long src)
{
    if (src <= 0) {
        return src;
    }
    while (src <= TIME_MAX_NANO_SECOND) {
        src = src * TIME_NANO_SECOND_RADIX;
    }
    return src;
}

/**
  verity the available of timestamp and micro seconds, compatible with mysql same function
  return true -- unavailable
  return false -- available
*/
bool check_time_mmssff_range(pg_tm *tm, long long microseconds)
{
  return tm->tm_min > TIME_MAX_MINUTE || tm->tm_sec > TIME_MAX_SECOND ||
         microseconds > TIME_MAX_FRAC;
}

/**
  time_add_nanoseconds_with_round refers to mysql for compatible with mysql time_to_sec function.
  the function do the round on nanoseconds of the time
*/
bool time_add_nanoseconds_with_round(char* input_str, pg_tm *tm, long long rem, fsec_t* fsec, bool can_ignore)
{
    int errlevel = (SQL_MODE_STRICT() && !can_ignore) ? ERROR : WARNING;
    /* We expect correct input data */
    if (rem >= MAX_NANO_SECOND) {
        ereport(errlevel, (errmsg("Truncated incorrect time value: \"%s\"", input_str)));
    }

    rem = align_to_nanoseconds(rem);
    uint microseconds = rem / TIME_MS_TO_S_RADIX;
    uint nanoseconds = rem % TIME_MS_TO_S_RADIX;

    if (check_time_mmssff_range(tm, microseconds)) {
        ereport(WARNING, (errmsg("Truncated incorrect time value")));
    }

    if (nanoseconds < NANO_SECOND_ROUND_BASE)
        return false;

    microseconds += (nanoseconds + NANO_SECOND_ROUND_BASE) / TIME_NANO_SECOND_TO_MICRO_SECOND_RADIX;
    if (microseconds < MAX_MICRO_SECOND)
        goto ret;

    microseconds %= MAX_MICRO_SECOND;
    if (tm->tm_sec < TIME_MAX_SECOND)
    {
        tm->tm_sec++;
        goto ret;
    }

    tm->tm_sec= 0;
    if (tm->tm_min < TIME_MAX_SECOND)
    {
        tm->tm_min++;
        goto ret;
    }
    tm->tm_min= 0;
    tm->tm_hour++;

    *fsec = microseconds * TIME_MS_TO_S_RADIX + nanoseconds;

ret:
    /*
      We can get '838:59:59.000001' at this point, which
      is bigger than the maximum possible value '838:59:59.000000'.
      Checking only "hour > 838" is not enough.
      Do full adjust_time_range().
    */
    bool warning = false;
    adjust_time_range(tm, *fsec, warning);
    if (warning == true) {
        ereport(errlevel, (errmsg("Truncated incorrect time value: \"%s\"", input_str)));
    }
    return false;
}

#endif

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(datexor);
extern "C" DLL_PUBLIC Datum datexor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timexor);
extern "C" DLL_PUBLIC Datum timexor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_time_xor);
extern "C" DLL_PUBLIC Datum date_time_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_date_xor);
extern "C" DLL_PUBLIC Datum time_date_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_text_xor);
extern "C" DLL_PUBLIC Datum time_text_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(text_time_xor);
extern "C" DLL_PUBLIC Datum text_time_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_text_xor);
extern "C" DLL_PUBLIC Datum date_text_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(text_date_xor);
extern "C" DLL_PUBLIC Datum text_date_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_date_xor);
extern "C" DLL_PUBLIC Datum int8_date_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_int8_xor);
extern "C" DLL_PUBLIC Datum date_int8_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int8_time_xor);
extern "C" DLL_PUBLIC Datum int8_time_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_int8_xor);
extern "C" DLL_PUBLIC Datum time_int8_xor(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum float8_date_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(date_float8_xor);
extern "C" DLL_PUBLIC Datum date_float8_xor(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(float8_date_xor);

int128 date_int128(DateADT dateVal)
{
    int128 res_date = 0;
    int year = 0;
    int month = 0;
    int day = 0;
    j2date(dateVal + POSTGRES_EPOCH_JDATE, &year, &month, &day);
    res_date = year*10000 + month*100 + day;
    return res_date;
}

uint64 time_uint64(TimeADT time)
{
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    time2tm(time, tm, &fsec);
    uint64 res_time = tmfsec2uint(tm);
    return res_time;
}

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

Datum datexor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    dateVal = PG_GETARG_DATEADT(0);
    int128 res1 = date_int128(dateVal);
    dateVal = PG_GETARG_DATEADT(1);
    int128 res2 = date_int128(dateVal);
    PG_RETURN_INT128(res1 ^ res2);
}

Datum date_time_xor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    dateVal = PG_GETARG_DATEADT(0);
    int128 res_date = date_int128(dateVal);
    TimeADT time = PG_GETARG_TIMEADT(1);
    uint64 res_time = time_uint64(time);
    PG_RETURN_INT128((int128)(res_time ^ res_date));
}

Datum timexor(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    uint64 res_time0 = time_uint64(time);
    time = PG_GETARG_TIMEADT(1);
    uint64 res_time1 = time_uint64(time);
    PG_RETURN_INT128((int128)(res_time0 ^ res_time1));
}

Datum time_date_xor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    dateVal = PG_GETARG_DATEADT(1);
    int128 res_date = date_int128(dateVal);
    TimeADT time = PG_GETARG_TIMEADT(0);
    uint64 res_time = time_uint64(time);
    PG_RETURN_INT128((int128)(res_time ^ res_date));
}

Datum date_text_xor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    int128 res_date = 0;
    dateVal = PG_GETARG_DATEADT(0);
    res_date = date_int128(dateVal);

    Datum textValue = PG_GETARG_DATUM(1);
    int128 temp = text_int128(textValue);
    int128 res = temp ^ res_date;
    PG_RETURN_INT128(res);
}

Datum text_date_xor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    int128 res_date = 0;
    dateVal = PG_GETARG_DATEADT(1);
    res_date = date_int128(dateVal);

    Datum textValue = PG_GETARG_DATUM(0);
    int128 temp = text_int128(textValue);
    int128 res = temp ^ res_date;
    PG_RETURN_INT128(res);
}

Datum time_text_xor(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    uint64 res_time = time_uint64(time);
    Datum textValue = PG_GETARG_DATUM(1);
    int128 temp = text_int128(textValue);
    int128 res = res_time ^ temp;
    PG_RETURN_INT128(res);
}

Datum text_time_xor(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(1);
    uint64 res_time = time_uint64(time);
    Datum textValue = PG_GETARG_DATUM(0);
    int128 temp = text_int128(textValue);
    int128 res = res_time ^ temp;
    PG_RETURN_INT128(res);
}

Datum date_int8_xor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    int128 res_date = 0;
    dateVal = PG_GETARG_DATEADT(0);
    res_date = date_int128(dateVal);

    int64 res_int8 = PG_GETARG_INT64(1);
    PG_RETURN_INT128(res_date ^ res_int8);
}

Datum int8_date_xor(PG_FUNCTION_ARGS)
{
    DateADT dateVal = 0;
    int128 res_date = 0;
    dateVal = PG_GETARG_DATEADT(1);
    res_date = date_int128(dateVal);

    int64 res_int8 = PG_GETARG_INT64(0);
    PG_RETURN_INT128(res_date ^ res_int8);
}

Datum time_int8_xor(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    uint64 res_time = time_uint64(time);

    int64 res_int8 = PG_GETARG_INT64(1);
    PG_RETURN_INT128(res_time ^ res_int8);
}

Datum int8_time_xor(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(1);
    uint64 res_time = time_uint64(time);

    int64 res_int8 = PG_GETARG_INT64(0);
    PG_RETURN_INT128(res_time ^ res_int8);
}

Datum float8_date_xor(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    int32 arg = DatumGetInt32(DirectFunctionCall1(dtoi4, Float8GetDatum(num)));

    DateADT dateVal = 0;
    int128 res_date = 0;
    dateVal = PG_GETARG_DATEADT(1);
    res_date = date_int128(dateVal);

    PG_RETURN_INT128(arg ^ res_date);
}

Datum date_float8_xor(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(1);
    int32 arg = DatumGetInt32(DirectFunctionCall1(dtoi4, Float8GetDatum(num)));

    DateADT dateVal = 0;
    int128 res_date = 0;
    dateVal = PG_GETARG_DATEADT(0);
    res_date = date_int128(dateVal);

    PG_RETURN_INT128(arg ^ res_date);
}

/* cast date to int8 */
PG_FUNCTION_INFO_V1_PUBLIC(date_int8);
extern "C" DLL_PUBLIC Datum date_int8(PG_FUNCTION_ARGS);
Datum date_int8(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;

    if (unlikely(date > 0 && (INT_MAX - date < POSTGRES_EPOCH_JDATE))) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("input julian date is overflow")));
    }
    j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    int64 res = date2int(tm);
    PG_RETURN_INT64(res);
}

/* cast date to numeric */
PG_FUNCTION_INFO_V1_PUBLIC(date_numeric);
extern "C" DLL_PUBLIC Datum date_numeric(PG_FUNCTION_ARGS);
Datum date_numeric(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;

    if (unlikely(date > 0 && (INT_MAX - date < POSTGRES_EPOCH_JDATE))) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("input julian date is overflow")));
    }
    j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    int64 val = date2int(tm);
    Numeric res = NULL;
    res = DatumGetNumeric(DirectFunctionCall1(int8_numeric, Int64GetDatum(val)));

    PG_RETURN_NUMERIC(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(date_any_value);
extern "C" DLL_PUBLIC Datum date_any_value(PG_FUNCTION_ARGS);
Datum date_any_value(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    PG_RETURN_DATEADT(date);
}

/* cast time to int8 */
PG_FUNCTION_INFO_V1_PUBLIC(time_int8);
extern "C" DLL_PUBLIC Datum time_int8(PG_FUNCTION_ARGS);
Datum time_int8(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    time2tm(time, tm, &fsec);
    int64 res = tmfsec2uint(tm);
    PG_RETURN_INT64(res);
}

/* cast time to float8 */
PG_FUNCTION_INFO_V1_PUBLIC(time_float8);
extern "C" DLL_PUBLIC Datum time_float8(PG_FUNCTION_ARGS);
Datum time_float8(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    time2tm(time, tm, &fsec);
    int64 temp = tmfsec2uint (tm);
    double res = temp + fsec / 1e6;

    PG_RETURN_INT64(res);
}

/* cast time to numeric */
PG_FUNCTION_INFO_V1_PUBLIC(time_numeric);
extern "C" DLL_PUBLIC Datum time_numeric(PG_FUNCTION_ARGS);
Datum time_numeric(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    Numeric res = NULL;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    time2tm(time, tm, &fsec);
    int64 quot = tmfsec2uint (tm);
    char *str = AppendFsec(quot, fsec);
    res =
        DatumGetNumeric(DirectFunctionCall3(numeric_in, CStringGetDatum(str), ObjectIdGetDatum(0), Int32GetDatum(-1)));

    PG_RETURN_NUMERIC(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(time_any_value);
extern "C" DLL_PUBLIC Datum time_any_value(PG_FUNCTION_ARGS);
Datum time_any_value(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    PG_RETURN_TIMEADT(time);
}

/* cast timetz to int8 */
PG_FUNCTION_INFO_V1_PUBLIC(timetz_int8);
extern "C" DLL_PUBLIC Datum timetz_int8(PG_FUNCTION_ARGS);
Datum timetz_int8(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    timetz2tm(time, tm, &fsec, &tz);
    int64 res = tmfsec2uint(tm);
    PG_RETURN_INT64(res);
}

/* cast timetz to float8 */
PG_FUNCTION_INFO_V1_PUBLIC(timetz_float8);
extern "C" DLL_PUBLIC Datum timetz_float8(PG_FUNCTION_ARGS);
Datum timetz_float8(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    timetz2tm(time, tm, &fsec, &tz);
    int64 temp = tmfsec2uint (tm);
    double res = temp + fsec / 1e6;

    PG_RETURN_INT64(res);
}

/* cast timetz to numeric */
PG_FUNCTION_INFO_V1_PUBLIC(timetz_numeric);
extern "C" DLL_PUBLIC Datum timetz_numeric(PG_FUNCTION_ARGS);
Datum timetz_numeric(PG_FUNCTION_ARGS)
{
    TimeTzADT *time = PG_GETARG_TIMETZADT_P(0);
    Numeric res = NULL;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec;
    int tz;
    timetz2tm(time, tm, &fsec, &tz);
    int64 quot = tmfsec2uint (tm);
    char *str = AppendFsec(quot, fsec);
    res =
        DatumGetNumeric(DirectFunctionCall3(numeric_in, CStringGetDatum(str), ObjectIdGetDatum(0), Int32GetDatum(-1)));

    PG_RETURN_NUMERIC(res);
}

PG_FUNCTION_INFO_V1_PUBLIC(timetz_any_value);
extern "C" DLL_PUBLIC Datum timetz_any_value(PG_FUNCTION_ARGS);
Datum timetz_any_value(PG_FUNCTION_ARGS)
{
    TimeTzADT* time = PG_GETARG_TIMETZADT_P(0);
    PG_RETURN_TIMETZADT_P(time);
}

/*
 * these code are useless actually, remain here to compat with 3.1.0 version, we can't delete it.
 *  datediff
 *  adddate_datetime_interval_text
 *  adddate_datetime_days_text
 */
/**
 * true if datetime in [0000-01-01 00:00:00.000000, 9999-12-31 23:59:59.999999]
*/
bool datetime_in_range(Timestamp datetime)
{
    bool ret = true;
#ifdef HAVE_INT64_TIMESTAMP
    if (datetime < B_FORMAT_TIMESTAMP_MIN_VALUE || datetime > B_FORMAT_TIMESTAMP_MAX_VALUE)
        ret = false;
#else
    if (datetime < (double)B_FORMAT_TIMESTAMP_MIN_VALUE / USECS_PER_SEC || 
        datetime > (double)B_FORMAT_TIMESTAMP_MAX_VALUE / USECS_PER_SEC)
        ret = false;
#endif
    return ret;
}

PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_days_text);
extern "C" DLL_PUBLIC Datum adddate_datetime_days_text(PG_FUNCTION_ARGS);
/**
 * adddate(date/datetime, days)
*/
Datum adddate_datetime_days_text(PG_FUNCTION_ARGS)
{
    text* tmp = PG_GETARG_TEXT_PP(0);
    int64 days = PG_GETARG_INT64(1);
    char *expr;

    expr = text_to_cstring(tmp);
    if (is_date_format(expr)) {
        DateADT date, result;
        date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(expr)));
        if (date_sub_days(date, -days, &result, true)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(date_text, result);
        }
    } else {
        Timestamp datetime, result;
        datetime = DatumGetTimestamp(
            DirectFunctionCall3(timestamp_in, CStringGetDatum(expr), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
        if (datetime_sub_days(datetime, -days, &result)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
        }
    }
    ereport(ERROR,
        (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
            errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}


PG_FUNCTION_INFO_V1_PUBLIC(datediff);
extern "C" DLL_PUBLIC Datum datediff(PG_FUNCTION_ARGS);
Datum datediff(PG_FUNCTION_ARGS)
{
    text* expr1 = PG_GETARG_TEXT_PP(0);
    text* expr2 = PG_GETARG_TEXT_PP(1);
    char *str1 = text_to_cstring(expr1);
    char *str2 = text_to_cstring(expr2);

    Timestamp datetime1, datetime2;
    DateADT dt1, dt2;

    if (!datetime_in_no_ereport(str1, &datetime1) || !datetime_in_no_ereport(str2, &datetime2) || 
        !datetime_in_range(datetime1) || !datetime_in_range(datetime2)) {
        PG_RETURN_NULL();
    }
    dt1 = DatumGetDateADT(timestamp2date(datetime1));
    dt2 = DatumGetDateADT(timestamp2date(datetime2));
    PG_RETURN_INT32(dt1 - dt2);
}

PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_interval_text);
extern "C" DLL_PUBLIC Datum adddate_datetime_interval_text(PG_FUNCTION_ARGS);
/**
 * adddate(date/datetime, INTERVAL expr UNIT)
*/
Datum adddate_datetime_interval_text(PG_FUNCTION_ARGS)
{
    text* tmp = PG_GETARG_TEXT_PP(0);
    Interval *span = PG_GETARG_INTERVAL_P(1);
    char *expr;

    expr = text_to_cstring(tmp);
    if (is_date_format(expr) && span->time == 0) {
        DateADT date, result;
        date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(expr)));
        if (date_add_interval(date, span, &result))
            return DirectFunctionCall1(date_text, result);
    } else {
        Timestamp datetime, result;
        datetime = DatumGetTimestamp(
            DirectFunctionCall3(timestamp_in, CStringGetDatum(expr), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
        if (datetime_add_interval(datetime, span, &result))
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(datetime_text, result);
    }
    ereport(ERROR,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                errmsg("date/time field value out of range")));
    PG_RETURN_NULL();
}

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(time_eq_timestamp);
extern "C" DLL_PUBLIC Datum time_eq_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_ne_timestamp);
extern "C" DLL_PUBLIC Datum time_ne_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_lt_timestamp);
extern "C" DLL_PUBLIC Datum time_lt_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_le_timestamp);
extern "C" DLL_PUBLIC Datum time_le_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_gt_timestamp);
extern "C" DLL_PUBLIC Datum time_gt_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(time_ge_timestamp);
extern "C" DLL_PUBLIC Datum time_ge_timestamp(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(timestamp_eq_time);
extern "C" DLL_PUBLIC Datum timestamp_eq_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_ne_time);
extern "C" DLL_PUBLIC Datum timestamp_ne_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_lt_time);
extern "C" DLL_PUBLIC Datum timestamp_lt_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_le_time);
extern "C" DLL_PUBLIC Datum timestamp_le_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_gt_time);
extern "C" DLL_PUBLIC Datum timestamp_gt_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(timestamp_ge_time);
extern "C" DLL_PUBLIC Datum timestamp_ge_time(PG_FUNCTION_ARGS);
/* Convert the time to timestamp type.
   Fill in the missing date with the current date */
static Timestamp time2timestamp(TimeADT timeVal)
{
    Timestamp result;
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    fsec_t fsec = 0;
    GetCurrentDateTime(tm);
    time2tm(timeVal, tm, &fsec);
    tm2timestamp(tm, fsec, NULL, &result);
    return result;
}
/* time_timestamp */
Datum time_eq_timestamp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    Timestamp dt1 = time2timestamp(time1);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum time_ne_timestamp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    Timestamp dt1 = time2timestamp(time1);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum time_lt_timestamp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    Timestamp dt1 = time2timestamp(time1);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum time_le_timestamp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    Timestamp dt1 = time2timestamp(time1);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum time_gt_timestamp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    Timestamp dt1 = time2timestamp(time1);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum time_ge_timestamp(PG_FUNCTION_ARGS)
{
    TimeADT time1 = PG_GETARG_TIMEADT(0);
    Timestamp dt1 = time2timestamp(time1);
    Timestamp dt2 = PG_GETARG_TIMESTAMP(1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}
/* timestamp_time */
Datum timestamp_eq_time(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    TimeADT time1 = PG_GETARG_TIMEADT(1);
    Timestamp dt2 = time2timestamp(time1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) == 0);
}

Datum timestamp_ne_time(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    TimeADT time1 = PG_GETARG_TIMEADT(1);
    Timestamp dt2 = time2timestamp(time1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) != 0);
}

Datum timestamp_lt_time(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    TimeADT time1 = PG_GETARG_TIMEADT(1);
    Timestamp dt2 = time2timestamp(time1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) < 0);
}

Datum timestamp_le_time(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    TimeADT time1 = PG_GETARG_TIMEADT(1);
    Timestamp dt2 = time2timestamp(time1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) <= 0);
}

Datum timestamp_gt_time(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    TimeADT time1 = PG_GETARG_TIMEADT(1);
    Timestamp dt2 = time2timestamp(time1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) > 0);
}

Datum timestamp_ge_time(PG_FUNCTION_ARGS)
{
    Timestamp dt1 = PG_GETARG_TIMESTAMP(0);
    TimeADT time1 = PG_GETARG_TIMEADT(1);
    Timestamp dt2 = time2timestamp(time1);
    
    PG_RETURN_BOOL(timestamp_cmp_internal(dt1, dt2) >= 0);
}

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_datenot);
extern "C" DLL_PUBLIC Datum dolphin_datenot(PG_FUNCTION_ARGS);
Datum dolphin_datenot(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    struct pg_tm tt;
    struct pg_tm* tm = &tt;
    if (unlikely(date > 0 && (INT_MAX - date < POSTGRES_EPOCH_JDATE))) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("input julian date is overflow")));
    }
    j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
    PG_RETURN_UINT64(~((uint64)date2int(tm)));
}
#endif

#endif
