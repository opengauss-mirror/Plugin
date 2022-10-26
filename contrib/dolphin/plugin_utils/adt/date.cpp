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
static int time2tm(TimeADT time, struct pg_tm* tm, fsec_t* fsec);
static int timetz2tm(TimeTzADT* time, struct pg_tm* tm, fsec_t* fsec, int* tzp);
static int tm2time(struct pg_tm* tm, fsec_t fsec, TimeADT* result);
static int tm2timetz(struct pg_tm* tm, fsec_t fsec, int tz, TimeTzADT* result);
static void AdjustTimeForTypmod(TimeADT* time, int32 typmod);
#ifdef DOLPHIN
static char* adjust_b_format_time(char *str, int *timeSign, int *D, bool *hasD);

PG_FUNCTION_INFO_V1_PUBLIC(int32_b_format_time);
extern "C" DLL_PUBLIC Datum int32_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(numeric_b_format_time);
extern "C" DLL_PUBLIC Datum numeric_b_format_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(int32_b_format_date);
extern "C" DLL_PUBLIC Datum int32_b_format_date(PG_FUNCTION_ARGS);
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
PG_FUNCTION_INFO_V1_PUBLIC(datediff);
extern "C" DLL_PUBLIC Datum datediff(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(from_days_text);
extern "C" DLL_PUBLIC Datum from_days_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(from_days_numeric);
extern "C" DLL_PUBLIC Datum from_days_numeric(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_days_text);
extern "C" DLL_PUBLIC Datum adddate_datetime_days_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(adddate_datetime_interval_text);
extern "C" DLL_PUBLIC Datum adddate_datetime_interval_text(PG_FUNCTION_ARGS);
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

PG_FUNCTION_INFO_V1_PUBLIC(get_format);
extern "C" DLL_PUBLIC Datum get_format(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(b_extract);
extern "C" DLL_PUBLIC Datum b_extract(PG_FUNCTION_ARGS);
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
{
    char* str = PG_GETARG_CSTRING(0);
    DateADT date;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tzp;
    int dtype = DTK_NUMBER;
    int nf;
    int dterr;
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
    } else {
        /*
         * default pg date formatting parsing.
         */
        dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
#ifndef DOLPHIN
        if (dterr == 0)
            dterr = DecodeDateTime(field, ftype, nf, &dtype, tm, &fsec, &tzp);
#else
        if (dterr != 0)
            DateTimeParseError(dterr, str, "date");
        if (dterr == 0) {
            if (ftype[0] == DTK_NUMBER && nf == 1) {
                dterr = NumberDate(field[0], tm);
                dtype = DTK_DATE;
            } else {
                dterr = DecodeDateTimeForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tzp);
            }
        }
#endif
        if (dterr != 0)
            DateTimeParseError(dterr, str, "date");
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

    /*
     * the following logic is unified for date parsing.
     */
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range: \"%s\"", str)));

    date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;

    PG_RETURN_DATEADT(date);
}
#ifdef DOLPHIN
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
    } else {
        dterr = ValidateDateForBDatabase(is2digits, tm, date_flag);
    }
    return dterr;
}

/* int4 to b format date type conversion */
Datum int32_b_format_date(PG_FUNCTION_ARGS) 
{
    int4 date = PG_GETARG_INT32(0);
    DateADT result;
    struct pg_tm tt, *tm = &tt;
    if (int32_b_format_date_internal(tm, date, true)) {
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                            errmsg("Out of range value for date")));
    }

    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("date out of range: \"%d\"", date)));

    result = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;
    PG_RETURN_DATEADT(result);
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
    char* str = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 typmod = PG_GETARG_INT32(2);
    TimeADT result;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tz;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char* field[MAXDATEFIELDS];
    int dtype;
    int ftype[MAXDATEFIELDS];
    char* time_fmt = NULL;
#ifdef DOLPHIN
    int timeSign = 1;
    int D = 0;
#endif
    /*
     * this case is used for time format is specified.
     */
    if (4 == PG_NARGS()) {
        time_fmt = PG_GETARG_CSTRING(3);
        if (time_fmt == NULL) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_DATETIME_FORMAT), errmsg("specified time format is null")));
        }

        /* the following logic shared from to_timestamp(). */
        to_timestamp_from_format(tm, &fsec, str, (void*)time_fmt);
    } else {
#ifdef DOLPHIN
        /* check if empty */
        if (strlen(str) == 0) {
            PG_RETURN_TIMEADT(0);
        }
        bool hasD = false;
        char *adjusted = adjust_b_format_time(str, &timeSign, &D, &hasD);
        dterr = ParseDateTime(adjusted, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
#else
        /*
         * original pg time format parsing
         */
        dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
        if (dterr == 0)
            dterr = DecodeTimeOnly(field, ftype, nf, &dtype, tm, &fsec, &tz);
#endif
        if (dterr != 0)
            DateTimeParseError(dterr, str, "time");
#ifdef DOLPHIN
        if (dterr == 0) {
            if (ftype[0] == DTK_NUMBER && nf == 1) {
                /* for example: str = "2 121212" , "231034.1234" */
                if (NumberTime(false, field[0], tm, &fsec, D, hasD)) {
                    ereport(ERROR,
                        (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                            errmsg("invalid input syntax for type %s: \"%s\"", "time", str)));
                }
            } else {
                dterr = DecodeTimeOnlyForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tz, D);
                if (dterr != 0)
                    DateTimeParseError(dterr, str, "time");
            }
        }
#endif
    }

    /*
     * the following logic is unified for time parsing.
     */
    tm2time(tm, fsec, &result);
    AdjustTimeForTypmod(&result, typmod);
#ifdef DOLPHIN
    result *= timeSign;
#endif
    PG_RETURN_TIMEADT(result);
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

/* int4(hhmmss) convert to b format time */
Datum int32_b_format_time(PG_FUNCTION_ARGS)
{
    int4 time = PG_GETARG_INT32(0);
    TimeADT result;
    fsec_t fsec = 0;
    int dterr;
    struct pg_tm tt, *tm = &tt;
    int sign = time < 0 ? -1 : 1;
    time *= sign;
    dterr = int32_b_format_time_internal(tm, false, time, &fsec);
    if (dterr) {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
    }
    tm2time(tm, 0, &result);
    PG_RETURN_TIMEADT(result * sign);
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
static int tm2time(struct pg_tm* tm, fsec_t fsec, TimeADT* result)
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
static int time2tm(TimeADT time, struct pg_tm* tm, fsec_t* fsec)
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

            case DTK_TZ:
            case DTK_TZ_MINUTE:
            case DTK_TZ_HOUR:
            case DTK_DAY:
            case DTK_MONTH:
            case DTK_QUARTER:
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
    int tz;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char* field[MAXDATEFIELDS];
    int dtype;
    int ftype[MAXDATEFIELDS];

    dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
    if (dterr == 0)
        dterr = DecodeTimeOnly(field, ftype, nf, &dtype, tm, &fsec, &tz);
    if (dterr != 0)
        DateTimeParseError(dterr, str, "time with time zone");

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
static int timetz2tm(TimeTzADT* time, struct pg_tm* tm, fsec_t* fsec, int* tzp)
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
/* makedate()
 * @Description: Convert yyyy.ddd into date type
 * @return: Convert result, date type.
 */
Datum makedate(PG_FUNCTION_ARGS)
{
    int64 year = PG_GETARG_INT64(0);
    int64 dayofyear = PG_GETARG_INT64(1);
    DateADT date;

    if (year < B_FORMAT_MIN_YEAR_OF_DATE || year > B_FORMAT_MAX_YEAR_OF_DATE || dayofyear <= 0)
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
 * @Description: Convert nanoseconds to microseconds and round up, Require: nanoseconds < 1000000000。
 * @return: int32 in microseconds
 */
fsec_t nano2microsecond(uint nanoseconds)
{
    fsec_t microseconds;
    int32 remain;
    microseconds = nanoseconds / NANO2MICRO_BASE;
    remain = nanoseconds % NANO2MICRO_BASE;
    /* round up */
    if (remain >= HALF_NANO2MICRO_BASE)
        microseconds += 1;

    return microseconds;
}

/*
 * @Description: Convert NumericVar second value to NumericSec value. Keep 8 digits after the decimal point.
 * @return: void.
 */
void sec_to_numericsec(NumericVar *from, NumericSec *to)
{
    if (!from->ndigits) {
        /* from == 0 */
        to->int_val = 0;
        to->frac_val = 0;
        return;
    }
    int int_part = from->weight + 1;
    int frac_part = from->ndigits - int_part;
    if (int_part > 2) {
        to->int_val = NBASE * NBASE;
        to->frac_val = 0;
        return;
    }
    if (int_part == 2) {
        to->int_val = from->digits[0] * NBASE + (from->ndigits > 1 ? from->digits[1] : 0);
    } else if (int_part == 1) {
        to->int_val = from->digits[0];
    } else {
        to->int_val = 0;
    }

    if (frac_part >= 2) {
        to->frac_val = from->digits[int_part] * NBASE + from->digits[int_part + 1];
    } else if (frac_part == 1) {
        to->frac_val = from->digits[int_part] * NBASE;
    } else {
        to->frac_val = 0;
    }

    if (from->sign) {
        to->int_val = -to->int_val;
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
    NumericSec second;
    init_var_from_num(PG_GETARG_NUMERIC(2), &tmp);
    sec_to_numericsec(&tmp, &second);

    struct pg_tm tt;
    TimeADT time;
    bool overflow = 0;
    bool neg = 0;

    if (minute < 0 || minute >= MINS_PER_HOUR || second.int_val < 0 || second.int_val >= SECS_PER_MINUTE)
        PG_RETURN_NULL();

    if (hour < 0)
        neg = 1;
    if (-hour > UINT_MAX || hour > UINT_MAX)
        overflow = 1;

    if (!overflow) {
        tt.tm_hour = (uint)((hour < 0 ? -hour : hour));
        tt.tm_min = (uint)minute;
        tt.tm_sec = (uint)second.int_val;
        fsec_t fsec = nano2microsecond(second.frac_val * 10);
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
    NumericSec second;
    init_var_from_num(PG_GETARG_NUMERIC(0), &tmp);
    sec_to_numericsec(&tmp, &second);
    struct pg_tm tt;
    TimeADT time;
    bool neg = 0;
    bool overflow = 0;
    if (second.int_val < 0) {
        neg = 1;
        second.int_val = -second.int_val;
    }

    if (second.int_val >= B_FORMAT_TIME_BOUND * 3600)
        overflow = 1;

    if (!overflow) {
        tt.tm_hour = 0;
        tt.tm_min = 0;
        tt.tm_sec = (uint) second.int_val; // keep only the integer part
        fsec_t fsec = nano2microsecond(second.frac_val * 10);
        tm2time(&tt, fsec, &time);
        if (neg)
            time = -time;
    }

    if (overflow || time < -B_FORMAT_TIME_MAX_VALUE || time > B_FORMAT_TIME_MAX_VALUE)
        time = neg ? -B_FORMAT_TIME_MAX_VALUE : B_FORMAT_TIME_MAX_VALUE;
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
bool time_in_no_ereport(const char *str, TimeADT *time)
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
        // If catch an error, just empty the error stack and set return value to false.
        (void)MemoryContextSwitchTo(current_ctx);
        FlushErrorState();
        ret = false;
    }
    PG_END_TRY();
    return ret;
}

/*
 * @Description: Subtract days from a date, giving a new date and assign it to result.
 * @return: false if parameter date or result out of range, otherwise true
 */
bool date_sub_days(DateADT date, int days, DateADT *result)
{
    if (date < B_FORMAT_DATEADT_MIN_VALUE || date > B_FORMAT_DATEADT_MAX_VALUE)
        return false;
    *result = date - days;
    if (*result < B_FORMAT_DATEADT_FIRST_YEAR || *result > B_FORMAT_DATEADT_MAX_VALUE)
        return false;
    return true;
}

/*
 * @Description: Subtract an interval from a date, giving a new date and assign it to result.
 * @return: false if parameter date or result out of range, otherwise true
 */
bool date_sub_interval(DateADT date, Interval *span, DateADT *result)
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

    if (*result < B_FORMAT_DATEADT_FIRST_YEAR)
        return false;
    return true;
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

    expr = text_to_cstring(tmp);

    if (is_date_format(expr)) {
        DateADT date, result;
        date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(expr)));
        if (date_sub_days(date, days, &result)) {
            /* The variable datetime or result does not exceed the specified range*/
            return DirectFunctionCall1(date_text, result);
        }
        ereport(ERROR,
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
        ereport(ERROR,
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

    expr = text_to_cstring(tmp);

    if (is_date_format(expr) && span->time == 0) {
        DateADT date, result;
        date = DatumGetDateADT(DirectFunctionCall1(date_in, CStringGetDatum(expr)));
        if (date_sub_interval(date, span, &result))
            return DirectFunctionCall1(date_text, result);
        ereport(ERROR,
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
        ereport(ERROR,
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

#ifdef HAVE_INT64_TIMESTAMP
    time2 = days * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE * USECS_PER_SEC;
#else
    time2 = days * HOURS_PER_DAY * MINS_PER_HOUR * SECS_PER_MINUTE;
#endif
    time -= time2;
    if (time >= -B_FORMAT_TIME_MAX_VALUE && time <= B_FORMAT_TIME_MAX_VALUE) {
        PG_RETURN_TIMEADT(time);
    }
    ereport(ERROR,
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
    if (span->month != 0) {
        ereport(ERROR,
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
    ereport(ERROR,
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
    text *expr = PG_GETARG_TEXT_PP(0);
    char *str;
    char buf[MAXDATELEN + 2];
    char *cp = buf;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int timeSign = 1;

    str = text_to_cstring(expr);

    str_to_pg_tm(str, tt, fsec, timeSign);

    if (fsec / USECS_PER_SEC) {
        tt.tm_sec += fsec / USECS_PER_SEC;
        tt.tm_min += tt.tm_sec / SECS_PER_MINUTE;
        tt.tm_sec %= SECS_PER_MINUTE;
        tt.tm_hour += tt.tm_min / MINS_PER_HOUR;
        tt.tm_min %= MINS_PER_HOUR;
        fsec = 0;
    }

    if (tm->tm_hour == TIME_MAX_HOUR && tm->tm_min == TIME_MAX_MINUTE &&
        tm->tm_sec == TIME_MAX_SECOND && fsec > 0) {
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                 errmsg("date/time field value out of range: \"%s\"", str)));
    }

    if (timeSign == -1) {
        *cp = '-';
        ++cp;
    }

    EncodeTimeOnly(tm, fsec, false, 0, u_sess->time_cxt.DateStyle, cp);
    PG_RETURN_TEXT_P(cstring_to_text(buf));
}

/**
 * @Description: Given a parameter of type datetime, calculate the number of days since year 0. 
 * @return: the number of days since year 0 to the given date.
 */
Datum to_days(PG_FUNCTION_ARGS) {
    Timestamp datetime = PG_GETARG_TIMESTAMP(0);
    int64 days;

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
    float8 result = 0;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    timestamp = date2timestamp(dateVal);

    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0) {
        result = tm->tm_year;
        if (result > 0)
            PG_RETURN_BOOL(true);
        else 
            PG_RETURN_BOOL(false);
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
    float8 result = 0;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if(time2tm(timeVal, tm, &fsec) == 0) {
        result = tm->tm_hour;
        if(result > 0)
            PG_RETURN_BOOL(true);
        else
            PG_RETURN_BOOL(false);
    } else {
        ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("time out of range")));
    }
    PG_RETURN_BOOL(false);
}

/**
 * The function is similar to time_in, but uses flag to control the parsing process.
 */
TimeADT time_in_with_flag(char *str, unsigned int date_flag)
{
    TimeADT result;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    int tz;
    int nf;
    int dterr;
    char workbuf[MAXDATELEN + 1];
    char* field[MAXDATEFIELDS];
    int dtype;
    int ftype[MAXDATEFIELDS];
    int timeSign = 1;
    int D = 0;
    /* check if empty */
    if (strlen(str) == 0) {
        PG_RETURN_TIMEADT(0);
    }
    bool hasD = false;
    char *adjusted = adjust_b_format_time(str, &timeSign, &D, &hasD);
    dterr = ParseDateTime(adjusted, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
    if (dterr != 0)
        DateTimeParseError(dterr, str, "time");
    if (dterr == 0) {
        if (ftype[0] == DTK_NUMBER && nf == 1) {
            /* for example: str = "2 121212" , "231034.1234" */
            if (NumberTime(false, field[0], tm, &fsec, D, hasD)) {
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                        errmsg("invalid input syntax for type %s: \"%s\"", "time", str)));
            }
        } else {
            dterr = DecodeTimeOnlyForBDatabase(field, ftype, nf, &dtype, tm, &fsec, &tz, D, date_flag);
            if (dterr != 0)
                DateTimeParseError(dterr, str, "time");
        }
    }

    if (tm->tm_year > B_FORMAT_MAX_YEAR_OF_DATE || (tm->tm_year == B_FORMAT_MIN_YEAR_OF_DATE && tm->tm_mon == FEBRUARY && tm->tm_mday == DAYNUM_FEB_LEAPYEAR)) {
        ereport(ERROR,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect time value: \"%s\"", str)));
    }

    tm2time(tm, fsec, &result);
    AdjustTimeForTypmod(&result, TIMESTAMP_MAX_PRECISION);
    result *= timeSign;
    return result;
}

/* time_to_sec()
 * @param time
 * @return seconds of the given time
 */
Datum time_to_sec(PG_FUNCTION_ARGS)
{
    TimeADT time;
    text *raw_text = PG_GETARG_TEXT_PP(0);
    struct pg_tm result_tt, *result_tm = &result_tt;
    fsec_t fsec;
    int32 result;
    int32 timeSign = 1;

    char *time_str = text_to_cstring(raw_text);
    time = time_in_with_flag(time_str, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH));
    if (time < 0) {
        timeSign = -1;
        time = -time;
    }
    time2tm(time, result_tm, &fsec);
    result = ((result_tm->tm_hour * MINS_PER_HOUR + result_tm->tm_min) * SECS_PER_MINUTE) + result_tm->tm_sec;
    result *= timeSign;
    PG_RETURN_INT32(result);
}

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
    EncodeDateOnlyForBDatabase(tm, u_sess->time_cxt.DateStyle, buf, ENABLE_ZERO_MONTH);
    *result = DirectFunctionCall1(textin, CStringGetDatum(buf));
}

Datum from_days_text(PG_FUNCTION_ARGS)
{
    text* expr = PG_GETARG_TEXT_PP(0);
    const char *str = text_to_cstring(expr);
    char* endptr = NULL;
    errno = 0;
    int64 days;
    Datum result;
    double tmp = strtod(str, &endptr);
    if (endptr == str || errno == ERANGE) {
        /* Supposing that we get a "0". LONG_LONG_MAX or LONG_LONG_MIN has the same result —— '0000-00-00'. */
        days = 0;
    } else {
        days = llround(tmp);
    }
    from_days_internal(days, &result);
    PG_RETURN_DATUM(result);
}

Datum from_days_numeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    Datum result;
    int64 days;
    PG_TRY();
    {
        days = (int64)DirectFunctionCall1(numeric_int8, NumericGetDatum(num));
    }
    PG_CATCH();
    {
        // If catch an error, just empty the error stack and set days to LONG_LONG_MAX
        FlushErrorState();
        days = LONG_LONG_MAX;   // supposing overflow
    }
    PG_END_TRY();
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
    return date_sub_interval(date, span, result);
}

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
        if (date_sub_days(date, -days, &result)) {
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

/**
 * adddate(time, days)
*/
Datum adddate_time_days(PG_FUNCTION_ARGS)
{
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
    ereport(ERROR,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
             errmsg("time field value out of range")));
    PG_RETURN_NULL();
}

/**
 * adddate(time, INTERVAL expr UNIT)
*/
Datum adddate_time_interval(PG_FUNCTION_ARGS)
{
    TimeADT time = PG_GETARG_TIMEADT(0);
    Interval *span = PG_GETARG_INTERVAL_P(1);
    TimeADT time2 = span->time;
    if (span->month != 0) {
        ereport(ERROR,
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
    ereport(ERROR,
            (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
             errmsg("time field value out of range")));
    PG_RETURN_NULL();
}

static inline Datum GetSepecificPartOfTime(PG_FUNCTION_ARGS, const char *part)
{
    char *tString = text_to_cstring(PG_GETARG_TEXT_PP(0));
    TimeADT tm;
    if (time_in_no_ereport(tString, &tm)) {
        return DirectFunctionCall2(time_part, CStringGetTextDatum(part), TimeADTGetDatum(tm));
    }
    Timestamp ts = DatumGetTimestamp(
        DirectFunctionCall3(timestamp_in, CStringGetDatum(tString), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
    pfree(tString);
    return DirectFunctionCall2(timestamp_part, CStringGetTextDatum(part), TimestampGetDatum(ts));
}

Datum GetHour(PG_FUNCTION_ARGS)
{
    return GetSepecificPartOfTime(fcinfo, "hour");
}

Datum GetMicrosecond(PG_FUNCTION_ARGS)
{
    return GetSepecificPartOfTime(fcinfo, "microsecond");
}

Datum GetMinute(PG_FUNCTION_ARGS)
{
    return GetSepecificPartOfTime(fcinfo, "minute");
}

Datum GetSecond(PG_FUNCTION_ARGS)
{
    return GetSepecificPartOfTime(fcinfo, "second");
}

bool time_in_with_sql_mode(char *str, TimeADT *result, unsigned int date_flag)
{
    bool ret = true;
    bool raise_warning = false;
    int code;
    const char *msg = NULL;
    PG_TRY();
    {
        *result = time_in_with_flag(str, date_flag);
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
static inline void resolve_units(char *unit_str, b_units *unit)
{
    for(int i = 0; i < units_size; i++) {
        if(strcmp(unit_str, unitnms[i]) == 0) {
            *unit = (b_units)i;
            return;
        }
    }
    ereport(ERROR, 
            (errmodule(MOD_FUNCTION),
            errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
            errmsg("units \"%s\" not supported", unit_str)));
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
        case UNIT_DAY_HOUR:
        case UNIT_DAY_MINUTE:
        case UNIT_DAY_SECOND:
        case UNIT_DAY_MICROSECOND:
            return 1;
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

Datum b_extract(PG_FUNCTION_ARGS)
{
    text *units = PG_GETARG_TEXT_PP(0);
    char *str = text_to_cstring(PG_GETARG_TEXT_PP(1));
    b_units enum_unit;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    int32 int_fsec;
    TimeADT time;
    int time_sign = 1;
    int64 result;

    char *lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);
    resolve_units(lowunits, &enum_unit);
    if (find_type(enum_unit)) {
        if (!datetime_in_with_sql_mode(str, tm, &fsec, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH))) {
            PG_RETURN_NULL();
        }
    } else {
        if (!time_in_with_sql_mode(str, &time, (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH))) {
            PG_RETURN_NULL();
        }
        if (time < 0) {
            time_sign = -1;
            time = -time;
        }
        time2tm(time, tm, &fsec);
    }

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
    PG_RETURN_INT64(result);
}
#endif