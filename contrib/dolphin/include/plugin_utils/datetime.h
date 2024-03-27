/* -------------------------------------------------------------------------
 *
 * datetime.h
 *	  Definitions for date/time support code.
 *	  The support code is shared with other date data types,
 *	   including abstime, reltime, date, and time.
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/datetime.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PLUGIN_DATETIME_H
#define PLUGIN_DATETIME_H

#include "nodes/nodes.h"
#include "utils/timestamp.h"
#include "utils/datetime.h"

#define INVALID_DAY_TIME(h, m, s, fsec) ((h) > HOURS_PER_DAY || ((h) == HOURS_PER_DAY && ((m) > 0 || (s) > 0 || (fsec) > 0)))

#ifndef FRONTEND_PARSER

/* validate b database date and time type */
extern int ValidateDateForBDatabase(bool is2digits, struct pg_tm* tm, unsigned int date_flag = 0);
extern int ValidateTimeForBDatabase(bool timeIn24, struct pg_tm* tm, fsec_t* fsec);
#ifdef DOLPHIN
extern int DecodeTimeOnlyForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, int D, unsigned int date_flag = 0);
extern int DecodeDateTimeForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, unsigned int date_flag = 0);
extern char* AppendFsec(int64 quot, fsec_t fsec);
#else
extern int DecodeTimeOnlyForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, int D);
extern int DecodeDateTimeForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp);
#endif
extern void EncodeDateOnlyForBDatabase(struct pg_tm* tm, int style, char* str);
extern void EncodeDateTimeForBDatabase(struct pg_tm* tm, fsec_t fsec, bool print_tz, int tz, const char* tzn, int style, char* str);

extern void Unixtimestamp2tm(double unixtimestamp, struct pg_tm* tm, fsec_t* fsec);
extern bool numeric_to_lldiv_t(NumericVar *from, lldiv_t *to);

#ifdef DOLPHIN
extern void lldiv_decode_tm(Numeric num, lldiv_t *div, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag, int *date_type);
extern bool lldiv_decode_tm_with_sql_mode(Numeric num, lldiv_t *div, struct pg_tm *tm, unsigned int date_flag);
extern void Numeric_to_lldiv(Numeric num, lldiv_t *div);
extern void NumericVar2lldiv(NumericVar *from, lldiv_t *to);
extern bool lldiv_decode_datetime(Numeric num, lldiv_t *div, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag, int *date_type);

extern unsigned long long pow_of_10[20];
extern const char* unitnms[20];
extern const int units_size;
enum b_units
{
  UNIT_YEAR,
  UNIT_QUARTER,
  UNIT_MONTH,
  UNIT_WEEK,
  UNIT_DAY,
  UNIT_HOUR,
  UNIT_MINUTE,
  UNIT_SECOND,
  UNIT_MICROSECOND,
  UNIT_YEAR_MONTH,
  UNIT_DAY_HOUR,
  UNIT_DAY_MINUTE,
  UNIT_DAY_SECOND,
  UNIT_HOUR_MINUTE,
  UNIT_HOUR_SECOND,
  UNIT_MINUTE_SECOND,
  UNIT_DAY_MICROSECOND,
  UNIT_HOUR_MICROSECOND,
  UNIT_MINUTE_MICROSECOND,
  UNIT_SECOND_MICROSECOND
};

#define DOLPHIN_TOKMAXLEN 20

typedef struct dolphin_datetkn {
    char token[DOLPHIN_TOKMAXLEN + 1];
    char type;
    char value;
} dolphin_datetkn;
#endif

/* Limits for the TIME data type */
#define TIME_MAX_HOUR 838
#define TIME_MAX_MINUTE 59
#define TIME_MAX_SECOND 59

#ifdef DOLPHIN
#define DTK_NONE -2
#define DTK_ERROR -1

#define DATETIME_MAX_DECIMALS 6
#define MAX_DATE_PARTS 8
#define LLDIV_MIN -1000000000000000000LL
#define LLDIV_MAX  1000000000000000000LL

/* Limits for the TIME data type */
#define TIME_MAX_INT 8385959
#define TIME_MIN_INT -8385959
#define TIME_MAX_VALUE (TIME_MAX_HOUR*10000 + TIME_MAX_MINUTE*100 + \
                        TIME_MAX_SECOND)
#define TIME_MAX_VALUE_SECONDS (TIME_MAX_HOUR * 3600L + \
                                TIME_MAX_MINUTE * 60L + TIME_MAX_SECOND)

/* two-digit years < this are 20..; >= this are 19.. */
#define YY_PART_YEAR 70

/* Time handling defaults */
#define TIMESTAMP_MAX_YEAR 2038
#define TIMESTAMP_MIN_YEAR (1900 + YY_PART_YEAR - 1)
#define TIMESTAMP_MAX_VALUE INT_MAX32
#define TIMESTAMP_MIN_VALUE 1
/** Flags to str_to_datetime and number_to_datetime.
 *  They may be useful in the future.
 */
typedef unsigned int time_flags;
static const time_flags TIME_FUZZY_DATE = 1; /* Set if we should allow partial dates */
static const time_flags TIME_DATETIME_ONLY = 2; /* Set if we only allow full datetimes */
static const time_flags TIME_NO_NSEC_ROUNDING = 4;
static const time_flags TIME_NO_DATE_FRAC_WARN = 8;
static const time_flags TIME_NO_ZERO_IN_DATE = 16; /* Don't allow partial dates */
extern int ParseIudDateTime(char* str, struct pg_tm* tm, fsec_t* fsec);
extern int ParseIudDateOnly(char* str, struct pg_tm* tm);
static const time_flags TIME_NO_ZERO_DATE = 32; /* Don't allow 0000-00-00 date */
static const time_flags TIME_INVALID_DATES = 64; /* Allow 2000-02-31 */

extern bool cstring_to_datetime(const char* str,  time_flags flags, int &tm_type, pg_tm *tm, fsec_t &fsec, int &nano,
                                bool &warnings, bool *null_func_result, int* tzp = NULL, int* invalid_tz = NULL);
#ifdef DOLPHIN
extern void DateTimeParseErrorWithFlag(int dterr, const char* str, const char* datatype, int time_cast_type,
    bool can_ignore = false, bool is_error = false);
extern void DateTimeParseErrorInternal(int dterr, const char* str, const char* datatype, int level);

bool CheckDateRange(const pg_tm *tm, bool not_zero_date, time_flags flags);
bool CheckDatetimeRange(const pg_tm *tm, const fsec_t fsec, const int tm_type);

#endif
extern bool datetime_add_nanoseconds_with_round(pg_tm *tm, fsec_t &fsec, int nano);
extern bool cstring_to_tm(const char *expr, pg_tm *tm, fsec_t &fsec, int* tzp = NULL, int* invalid_tz = NULL);

extern bool IsResetUnavailableDataTime(int dterr, pg_tm tm, bool is_support_reset_unavailable_datatime = false);

#define tmfsec2float(tm, fsec) ((tm)->tm_hour * 10000 + (tm)->tm_min * 100 + (tm)->tm_sec + (fsec) / 1000000.0)

#define date2int(tm) ((tm)->tm_year * 10000 + (tm)->tm_mon * 100 + (tm)->tm_mday)
#define tmfsec2uint(tm) ((tm)->tm_hour * 10000 + (tm)->tm_min * 100 + (tm)->tm_sec)
#define timestamp2int(tm) ((tm)->tm_year * 10000000000 + (tm)->tm_mon * 100000000 + (tm)->tm_mday * 1000000 + \
                           (tm)->tm_hour * 10000 + (tm)->tm_min * 100 + (tm)->tm_sec)

#define BC_STR_LEN (2)
#define SHORT_YEAR_LEN (2)

#endif

#endif // !FRONTEND_PARSER

#endif /* DATETIME_H */

