/* -------------------------------------------------------------------------
 *
 * date.h
 *	  Definitions for the SQL92 "date" and "time" types.
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/date.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PLUGIN_DATE_H
#define PLUGIN_DATE_H

#include <math.h>

#include "fmgr.h"
#include "timestamp.h"
#include "utils/date.h"
#include "utils/numeric.h"
#include "plugin_postgres.h"

/* b format date and time type boundaries*/
#define DATE_YYYYMMDD_LEN 8
#define DATE_YYMMDD_LEN 6
#define B_FORMAT_DATE_NUMBER_MIN_LEN 5
#define B_FORMAT_TIME_BOUND INT64CONST(839)
#define B_FORMAT_TIME_MAX_VALUE INT64CONST(B_FORMAT_TIME_BOUND * INT64CONST(3600000000) - 1000000)
#define B_FORMAT_TIME_NUMBER_MAX_LEN 7
#define B_FORMAT_DATE_INT_MIN 101
#define B_FORMAT_MAX_DATE 99991231
#define B_FORMAT_MAX_TIME_USECS INT64CONST(3020400000000)
#define B_FORMAT_DATEADT_MAX_VALUE 2921939 /* date 9999-12-31 */
#define B_FORMAT_DATEADT_MIN_VALUE -730485 /* date 0000-01-01 */
#define B_FORMAT_DATEADT_FIRST_YEAR -730119 /* date 0001-01-01 */
#define B_FORMAT_MAX_YEAR_OF_DATE 9999
#define B_FORMAT_MIN_YEAR_OF_DATE 0

#define B_FORMAT_TIME_MAX_VALUE_TO_DAY 70
#define NANO2MICRO_BASE 1000
#define HALF_NANO2MICRO_BASE 500
#define FRAC_PART_LEN_IN_NUMERICSEC 100000000

#ifdef DOLPHIN
#define TWO_DIGITS_YEAR_DATE_ONE 691231 /* 2069-12-31 */
#define TWO_DIGITS_YEAR_DATE_TWO 700101 /* 1970-01-01 */
#define TWO_DIGITS_YEAR_DATE_THREE 991231   /* 1999-12-31 */
#define TIME_BOUND_WITHOUT_DICIMAL_SEC INT64CONST(B_FORMAT_MAX_TIME_USECS - 1000000)
#define FROM_DAYS_MAX_DAY 3652500
#define MAX_NUMBER_OF_DAY 3652424
#endif

/* for b compatibility type*/
extern int int32_b_format_date_internal(struct pg_tm *tm, int4 date, bool mayBe2Digit, unsigned int date_flag = 0);
extern int int32_b_format_time_internal(struct pg_tm *tm, bool timeIn24, int4 time, fsec_t *fsec);
extern int NumberDate(char *str, pg_tm *tm, unsigned int date_flag = 0);
extern int NumberTime(bool timeIn24, char *str, pg_tm *tm, fsec_t *fsec, int D = 0, bool hasD = false);
/* for b compatibility time function*/
extern void str_to_pg_tm(char *str, pg_tm &tt, fsec_t &fsec, int &timeSign);
extern bool time_in_range(TimeADT &time);
extern bool time_in_no_ereport(const char *str, TimeADT *time);
extern bool is_date_format(const char *str);
extern bool date_in_no_ereport(const char *str, DateADT *date);
extern bool date_sub_interval(DateADT date, Interval *span, DateADT *result);
extern void convert_to_time(Datum value, Oid valuetypid, TimeADT *time);

#ifdef DOLPHIN
extern void check_b_format_time_range_with_ereport(TimeADT &time);
extern TimeADT time_in_with_flag(char *str, unsigned int date_flag);
extern bool time_in_with_sql_mode(char *str, TimeADT *result, unsigned int date_flag);
extern bool date_add_interval(DateADT date, Interval *span, DateADT *result);

typedef struct DateTimeFormat
{
    const char *format_name;
    const char *date_format;
    const char *datetime_format;
    const char *time_format;
}DateTimeFormat;
#endif

#endif /* DATE_H */