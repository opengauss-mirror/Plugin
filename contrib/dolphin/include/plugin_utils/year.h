#ifndef PLUGIN_YEAR_H
#define PLUGIN_YEAR_H

#include <math.h>
#include <ctype.h>

#include "plugin_postgres.h"
#include "fmgr.h"
#include "timestamp.h"

typedef int16 YearADT;

#define DatumGetYearADT(X) ((YearADT)DatumGetInt16(X))

#define YearADTGetDatum(X) Int16GetDatum(X)

#define PG_GETARG_YEARADT(n) DatumGetYearADT(PG_GETARG_DATUM(n))

#define PG_RETURN_YEARADT(x) return YearADTGetDatum(x)

#define MAX_YEAR_NUM 2155
#define MIN_YEAR_NUM 1901
#define YEAR4_LEN 4

#define YEAR2_BOUND_BETWEEN_20C_21C 70
/* 2-digit year >= YEAR2_BOUND_BETWEEN_20C_21C will be treat as 20 century year
 *              <  YEAR2_BOUND_BETWEEN_20C_21C will be treat as 21 century year
 * yy    -> yyyy
 * 70~99 -> 1970~1999
 * 00~69 -> 2000~2069
 */
#define YEAR2_ADJUST_VALUE_20C_21C(x) ((x) + ((x) >= YEAR2_BOUND_BETWEEN_20C_21C ? 1900 : 2000))
#define MIN_YEAR2_NUM (1900 + YEAR2_BOUND_BETWEEN_20C_21C)
#define MAX_YEAR2_NUM (2000 - 1 + YEAR2_BOUND_BETWEEN_20C_21C)

#define Year_to_YearADT(x) ((x) - MIN_YEAR_NUM + 1)
#define YearADT_to_Year(x) ((x) == 0 ? 0 : (int32)(x) + MIN_YEAR_NUM - 1)

#define YEAR4_IN_RANGE(x) ((x) >= MIN_YEAR_NUM && (x) <= MAX_YEAR_NUM)
#define YEAR2_IN_RANGE(x) ((x) >= 0 && (x) <= 99)
#define YEAR_VALID(x) (YEAR4_IN_RANGE(x) || YEAR2_IN_RANGE(x))

/* YEAR(2) -> YEAR(4) or YEAR(4) -> YEAR(2) */
#define YEAR_CONVERT(x) ((x) = (-x))
#define IS_YEAR2(x) ((x) < 0)
#define IS_YEAR4(x) ((x) >= 0)

extern Datum year_mi_interval(YearADT year, const Interval* span);
extern Datum year_pl_interval(YearADT year, const Interval* span);
extern Datum year_mi(YearADT y1, YearADT y2);
extern Datum year_hash(PG_FUNCTION_ARGS);
extern int year_cmp_internal(YearADT y1, YearADT y2);


#endif /* YEAR_T_H */
