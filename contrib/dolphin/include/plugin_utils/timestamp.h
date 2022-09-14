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

/* compatible with b format datetime and timestamp */
extern void tm2datetime(struct pg_tm* tm, const fsec_t fsec, Timestamp* result);
extern void datetime2tm(Timestamp dt, struct pg_tm* tm, fsec_t* fsec);
extern int NumberTimestamp(char *str, pg_tm *tm, fsec_t *fsec);
extern bool datetime_in_no_ereport(const char *str, Timestamp *datetime);
extern bool datetime_sub_days(Timestamp datetime, int days, Timestamp *result);
extern bool datetime_sub_interval(Timestamp datetime, Interval *span, Timestamp *result);

extern Datum datetime_text(PG_FUNCTION_ARGS);
extern Datum time_text(PG_FUNCTION_ARGS);

#endif // !FRONTEND_PARSER
#endif /* TIMESTAMP_H */
