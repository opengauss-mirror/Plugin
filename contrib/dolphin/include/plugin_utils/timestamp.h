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
#define TIMESTAMP_MAX_PRECISION 6

/* compatible with b format datetime and timestamp */
extern void tm2datetime(struct pg_tm* tm, const fsec_t fsec, Timestamp* result);
extern void datetime2tm(Timestamp dt, struct pg_tm* tm, fsec_t* fsec);

#endif // !FRONTEND_PARSER
#endif /* TIMESTAMP_H */
