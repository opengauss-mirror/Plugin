/* -------------------------------------------------------------------------
 *
 * timestamp.h
 *	  Definitions for the SQL92 "timestamp" and "interval" types.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/timestamp.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PLUGIN_TIMESTAMP_H
#define PLUGIN_TIMESTAMP_H

#ifndef FRONTEND_PARSER
#include "datatype/timestamp.h"
#include "fmgr.h"
#include "pgtime.h"
#include "plugin_postgres.h"

#define TIMESTAMP_YYMMDD_LEN 6
#define TIMESTAMP_YYYYMMDD_LEN 8
#define TIMESTAMP_YYMMDDhhmmss_LEN 12
#define TIMESTAMP_YYYYMMDDhhmmss_LEN 14
#define B_FORMAT_TIMESTAMP_MAX_VALUE INT64CONST(252455615999999999) /* datetime 9999-12-31 23:59:59.999999 */
#define B_FORMAT_TIMESTAMP_MIN_VALUE INT64CONST(-63113904000000000) /* datetime 0000-01-01 00:00:00.000000 */
#define B_FORMAT_TIMESTAMP_ZERO_YEAR_LEAP_DAY INT64CONST(-63108806400000000) /* datetime 0000-02-29 00:00:00.000000 */
#define B_FORMAT_TIMESTAMP_FIRST_YEAR INT64CONST(-63082281600000000) /* datetime 0001-01-01 00:00:00.000000 */
#define B_FORMAT_TIMESTAMP_APART_LEFT INT64CONST(-63108806400000000) /* datetime 0000-02-29 00:00:00.000000 */
#define B_FORMAT_TIMESTAMP_APART_RIGHT INT64CONST(-63108720000000000) /* datetime 0000-03-01 00:00:00.000000 */

#define MONTHS_PER_QUARTER 3
#define AM_PM_LEN 2
#define DAYS_PER_WEEK 7
#define NANOSEC_PER_USEC 1000
#define FRAC_PRECISION 6
#define TIMESTAMP_MAX_PRECISION 6

#ifdef DOLPHIN
#define B_MAX_NUMBER_DATETIME INT64CONST(99991231235959)    /* 9999-12-31 23:59:59 */
#define B_NORMAL_NUMBER_DATETIME INT64CONST(10000101000000) /* 1000-01-01 00:00:00 */
#define TWO_DIGITS_YEAR_DATETIME_ONE INT64CONST(691231235959)   /* 2069-12-31 23:59:59 */
#define TWO_DIGITS_YEAR_DATETIME_TWO INT64CONST(700101000000)   /* 1970-01-01 00:00:00 */
#define TWO_DIGITS_YEAR_DATETIME_THREE INT64CONST(991231235959) /* 1999-12-31 23:59:59*/
#define UNIXTIMESTAMP_START_VALUE INT64CONST(-946684799000000)  /* 1970-01-01 00:00:01.000000 */
#define UNIXTIMESTAMP_END_VALUE INT64CONST(1200798847999999)    /* 2038-01-19 03:14:07.999999 */

#define NORMAL_DATE 0
#define ENABLE_ZERO_DAY 1
#define ENABLE_ZERO_MONTH 2
#define ENABLE_ZERO_DATE 4  /* enable date like 0000-00-00 */

#define DTK_DATE_TIME 5
#define IS_ZERO_NUMBER_DATE(quot, rem) (quot == 0 && rem >= 0 && rem < 99999950)
#define MONDAY_IS_FIRST_WEEKDAY 1
#define SCOPE_OF_WEEK 2
#define FIRST_FULL_WEEK 4
#define FOUR_DAYS_IN_YEAR 4
#define WEEKS_PER_YEAR 52

#define HOURS_HALF_DAY 12
#define MAX_UNIXTIMESTAMP_VALUE INT32_MAX

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
#endif

/* compatible with b format datetime and timestamp */
extern void tm2datetime(struct pg_tm* tm, const fsec_t fsec, Timestamp* result);
extern void datetime2tm(Timestamp dt, struct pg_tm* tm, fsec_t* fsec);
#ifdef DOLPHIN
extern int NumberTimestamp(char *str, pg_tm *tm, fsec_t *fsec, unsigned int date_flag = 0);
#else
extern int NumberTimestamp(char *str, pg_tm *tm, fsec_t *fsec);
#endif
extern bool datetime_in_no_ereport(const char *str, Timestamp *datetime);
extern bool datetime_sub_days(Timestamp datetime, int days, Timestamp *result);
extern bool datetime_sub_interval(Timestamp datetime, Interval *span, Timestamp *result);

#ifdef DOLPHIN
extern void datetime_in_with_flag(const char *str, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag);
extern void datetime_in_with_flag_internal(const char *str, struct pg_tm *tm, fsec_t* fsec, unsigned int date_flag);
extern bool MaybeRound(struct pg_tm *tm, fsec_t *fsec);
extern bool datetime_in_range(Timestamp datetime);
extern bool datetime_add_interval(Timestamp datetime, Interval *span, Timestamp *result);
extern void convert_to_datetime(Datum value, Oid valuetypid, Timestamp *datetime);
extern int64 b_db_weekmode(int64 mode);
extern int b_db_cal_week(struct pg_tm* tm, int64 mode, uint* year);
extern bool datetime_in_with_sql_mode(char *str, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag);
#endif

extern Datum datetime_text(PG_FUNCTION_ARGS);
extern Datum time_text(PG_FUNCTION_ARGS);

#endif // !FRONTEND_PARSER
#endif /* TIMESTAMP_H */
