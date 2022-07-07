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
extern int ValidateDateForBDatabase(bool is2digits, struct pg_tm* tm);
extern int ValidateTimeForBDatabase(bool timeIn24, struct pg_tm* tm, fsec_t* fsec);
extern int DecodeTimeOnlyForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, int D);
extern int DecodeDateTimeForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp);
extern void EncodeDateOnlyForBDatabase(struct pg_tm* tm, int style, char* str);
extern void EncodeDateTimeForBDatabase(struct pg_tm* tm, fsec_t fsec, bool print_tz, int tz, const char* tzn, int style, char* str);

#endif // !FRONTEND_PARSER

#endif /* DATETIME_H */

