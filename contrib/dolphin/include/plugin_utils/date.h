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
#include "datetime.h"

/* b format date and time type boundaries*/
#define DATE_YYYYMMDD_LEN 8
#define DATE_YYMMDD_LEN 6
#define B_FORMAT_DATE_NUMBER_MIN_LEN 5
#define B_FORMAT_TIME_BOUND INT64CONST(839)
#define B_FORMAT_TIME_MAX_VALUE INT64CONST(B_FORMAT_TIME_BOUND * INT64CONST(3600000000) - 1000000)
#define B_FORMAT_TIME_INVALID_VALUE_TAG (-B_FORMAT_TIME_MAX_VALUE-1)
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
#define TIME_WITH_FORMAT_ARGS_SIZE 4
#define TIME_MS_TO_S_RADIX 1000
#ifdef DOLPHIN
#define TWO_DIGITS_YEAR_DATE_ONE 691231 /* 2069-12-31 */
#define TWO_DIGITS_YEAR_DATE_TWO 700101 /* 1970-01-01 */
#define TWO_DIGITS_YEAR_DATE_THREE 991231   /* 1999-12-31 */
#define TIME_BOUND_WITHOUT_DICIMAL_SEC INT64CONST(B_FORMAT_MAX_TIME_USECS - 1000000)
#define FROM_DAYS_MAX_DAY 3652500
#define MAX_NUMBER_OF_DAY 3652424
#define TIME_MAX_FRAC 999999
#define DATE_ALL_ZERO_VALUE (-2451508)
#define JDATE_ALL_ZERO_VALUE (DATE_ALL_ZERO_VALUE + POSTGRES_EPOCH_JDATE)
#define MONTH_TO_QUARTER_RADIX 3
#define MAX_LONGLONG_TO_CHAR_LENGTH 21
#define TIME_NANO_SECOND_RADIX 10
#define MAX_NANO_SECOND 1000000000
#define NANO_SECOND_ROUND_BASE 500
#define MAX_MICRO_SECOND 1000000
#define TIME_MAX_NANO_SECOND 99999999
#define TIME_NANO_SECOND_TO_MICRO_SECOND_RADIX 1000
#define DATETIME_MIN_LEN (10) /* datetime: 0001-00-00 00-00-00 */
struct LongLongTm {
    pg_tm result_tm; /* yyyy-mm-dd hh:MM:ss timestamp */
    fsec_t fsec;     /* ms of result_tm */
    int timeSign;    /* timeSign */
};
#endif

/* for b compatibility type*/
extern int int32_b_format_date_internal(struct pg_tm *tm, int4 date, bool mayBe2Digit, unsigned int date_flag = 0);
extern int int32_b_format_time_internal(struct pg_tm *tm, bool timeIn24, int4 time, fsec_t *fsec);
extern int NumberDate(char *str, pg_tm *tm, unsigned int date_flag = 0);
extern int NumberTime(bool timeIn24, char *str, pg_tm *tm, fsec_t *fsec, int D = 0, bool hasD = false);
/* for b compatibility time function*/
extern void str_to_pg_tm(char *str, pg_tm &tt, fsec_t &fsec, int &timeSign);
extern bool time_in_range(TimeADT &time);
extern bool time_in_without_overflow(const char *str, TimeADT *time, bool can_ignore);
extern bool is_date_format(const char *str);
extern bool date_in_no_ereport(const char *str, DateADT *date);
extern bool date_sub_interval(DateADT date, Interval *span, DateADT *result, bool is_add_func = false);
extern void convert_to_time(Datum value, Oid valuetypid, TimeADT *time);

#ifdef DOLPHIN
extern int tm2time(struct pg_tm* tm, fsec_t fsec, TimeADT* result);
extern int time2tm(TimeADT time, struct pg_tm* tm, fsec_t* fsec);
extern int timetz2tm(TimeTzADT* time, struct pg_tm* tm, fsec_t* fsec, int* tzp);
extern bool cstring_to_time(const char *str, pg_tm *tm, fsec_t &fsec, int &timeSign, int &tm_type, bool &warnings, bool *null_func_result);
extern void check_b_format_time_range_with_ereport(TimeADT &time, bool can_ignore = false, bool* result_isnull = NULL);
extern void check_b_format_date_range_with_ereport(DateADT &date);
extern Oid convert_to_datetime_date(Datum value, Oid valuetypid, Timestamp *datetime, DateADT *date);
extern void adjust_time_range(pg_tm *tm, fsec_t &fsec, bool &warnings);
extern bool time_in_with_flag(char *str, unsigned int date_flag, TimeADT* time_adt, bool vertify_time = false,
    bool can_ignore = false);
extern bool time_in_with_sql_mode(char *str, TimeADT *result, unsigned int date_flag, bool vertify_time = false,
    bool can_ignore = false);
extern bool date_add_interval(DateADT date, Interval *span, DateADT *result);
extern Datum date_internal(PG_FUNCTION_ARGS, char* str, int time_cast_type, TimeErrorType* time_error_type);
extern "C" Datum time_float(PG_FUNCTION_ARGS);
extern Datum textout (PG_FUNCTION_ARGS);
extern bool time_add_nanoseconds_with_round(char* input_str, pg_tm *tm, long long rem, fsec_t* fsec, bool can_ignore);
extern long long align_to_nanoseconds(long long src);
extern bool check_time_mmssff_range(pg_tm *tm, long long microseconds);
extern bool longlong_to_tm(long long nr, TimeADT* time, pg_tm* result_tm, fsec_t* fsec, int32* timeSign,
    bool can_ignore = false);
bool check_time_min_value(char* input_str, long long nr, bool can_ignore = false);
bool resolve_units(char *unit_str, b_units *unit);
extern bool longlong_to_tm(long long nr, TimeADT* time, LongLongTm* tm, bool can_ignore = false);

typedef struct DateTimeFormat
{
    const char *format_name;
    const char *date_format;
    const char *datetime_format;
    const char *time_format;
}DateTimeFormat;


typedef struct GaussTimeResult {
    long time_adt;
    bool handle_continuous;
    TimeErrorType time_error_type;
    /* tm2 stores openGauss's parsing result while tt stores M*'s parsing result */
    pg_tm* tm;
}GaussTimeResult;


#define GET_TIME_ERROR_TYPE() (SQL_MODE_NOT_STRICT_ON_INSERT() || fcinfo->can_ignore ?  \
            TIME_CORRECT : TIME_INCORRECT)

extern Datum time_internal(PG_FUNCTION_ARGS, char* str, int is_time_sconst, TimeErrorType* time_error_type);
Datum int64_number_cast_time(PG_FUNCTION_ARGS, int64 number);
Datum int_cast_time_internal(PG_FUNCTION_ARGS, int64 number, bool* isnull);
Datum uint64_number_cast_time(PG_FUNCTION_ARGS, uint64 number);

char* parser_function_input(Datum txt, Oid oid);
Datum int64_number_cast_date(PG_FUNCTION_ARGS, int64 number);
Datum int_cast_date_internal(PG_FUNCTION_ARGS, int64 number, bool* isnull);
Datum uint64_number_cast_date(PG_FUNCTION_ARGS, uint64 number);

extern "C" DLL_PUBLIC Datum float8_b_format_datetime(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum numeric_b_format_datetime(PG_FUNCTION_ARGS);

#endif

#endif /* DATE_H */