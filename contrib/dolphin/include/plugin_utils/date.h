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
#include "plugin_postgres.h"

/* b format date and time type boundaries*/
#define DATE_YYYYMMDD_LEN 8
#define DATE_YYMMDD_LEN 6
#define B_FORMAT_DATE_NUMBER_MIN_LEN 5
#define B_FORMAT_TIME_BOUND 839
#define B_FORMAT_TIME_NUMBER_MAX_LEN 7
#define B_FORMAT_DATE_INT_MIN 101
#define B_FORMAT_MAX_DATE 99991231
#define B_FORMAT_MAX_TIME_USECS INT64CONST(72489599999999)

/* for b compatibility type*/
extern int int32_b_format_date_internal(struct pg_tm *tm, int4 date);
extern int int32_b_format_time_internal(struct pg_tm *tm, bool timeIn24, int4 time, fsec_t *fsec);
extern int NumberDate(char *str, pg_tm *tm);
extern int NumberTime(bool timeIn24, char *str, pg_tm *tm, fsec_t *fsec);

#endif /* DATE_H */