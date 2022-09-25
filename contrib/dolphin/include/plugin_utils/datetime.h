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
extern void EncodeDateOnlyForBDatabase(struct pg_tm* tm, int style, char* str, unsigned int date_flag = 0);
#else
extern int DecodeTimeOnlyForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, int D);
extern int DecodeDateTimeForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp);
extern void EncodeDateOnlyForBDatabase(struct pg_tm* tm, int style, char* str);
#endif
extern void EncodeDateTimeForBDatabase(struct pg_tm* tm, fsec_t fsec, bool print_tz, int tz, const char* tzn, int style, char* str);

extern void Unixtimestamp2tm(double unixtimestamp, struct pg_tm* tm, fsec_t* fsec);
extern bool numeric_to_lldiv_t(NumericVar *from, lldiv_t *to);

#ifdef DOLPHIN
extern void lldiv_decode_tm(Numeric num, lldiv_t *div, struct pg_tm *tm, unsigned int date_flag);
extern void NumericVar2lldiv(NumericVar *from, lldiv_t *to);
#endif

/* Limits for the TIME data type */
#define TIME_MAX_HOUR 838
#define TIME_MAX_MINUTE 59
#define TIME_MAX_SECOND 59

#ifdef DOLPHIN
#define DTK_NONE -1
#define DTK_ERROR -2

#define DATETIME_MAX_DECIMALS 6
#define MAX_DATE_PARTS 8

/* Limits for the TIME data type */
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
static const time_flags TIME_NO_ZERO_DATE = 32; /* Don't allow 0000-00-00 date */
static const time_flags TIME_INVALID_DATES = 64; /* Allow 2000-02-31 */

extern bool cstring_to_tm(const char *expr, pg_tm *tm, fsec_t &fsec);

#endif

#endif // !FRONTEND_PARSER

#endif /* DATETIME_H */

