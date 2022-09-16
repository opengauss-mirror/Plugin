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

#endif // !FRONTEND_PARSER

#endif /* DATETIME_H */

