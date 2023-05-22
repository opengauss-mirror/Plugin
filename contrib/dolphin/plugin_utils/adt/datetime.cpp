/* -------------------------------------------------------------------------
 *
 * datetime.c
 *	  Support functions for date/time types.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/datetime.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#include "access/xact.h"
#include "catalog/pg_type.h"
#include "common/int.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "nodes/nodeFuncs.h"
#include "utils/builtins.h"
#include "plugin_utils/date.h"
#include "plugin_utils/datetime.h"
#include "utils/memutils.h"
#include "utils/tzparser.h"
#include "plugin_parser/scansup.h"
#ifdef DOLPHIN
#include "plugin_commands/mysqlmode.h"
#endif

static int DecodeNumber(int flen, char* field, bool haveTextMonth, unsigned int fmask, unsigned int* tmask,
    struct pg_tm* tm, fsec_t* fsec, bool* is2digits);
static int DecodeNumberField(
    int len, char* str, unsigned int fmask, unsigned int* tmask, struct pg_tm* tm, fsec_t* fsec, bool* is2digits);
static int DecodeTime(
    const char* str, unsigned int fmask, int range, unsigned int* tmask, struct pg_tm* tm, fsec_t* fsec
#ifdef DOLPHIN
    , bool timeIn24 = true
#endif
    );
static int DecodeTimezone(const char* str, int* tzp);
static const datetkn* datebsearch(const char* key, const datetkn* base, int nel);
static int DecodeDate(char* str, unsigned int fmask, unsigned int* tmask, bool* is2digits, struct pg_tm* tm);
#ifdef DOLPHIN
static int ValidateDate(unsigned int fmask, bool isjulian, bool is2digits, bool bc, bool ad, struct pg_tm* tm, unsigned int date_flag = 0);
#else
static int ValidateDate(unsigned int fmask, bool isjulian, bool is2digits, bool bc, struct pg_tm* tm);
#endif
static void AppendTrailingZeros(char* str);
#ifndef HAVE_INT64_TIMESTAMP
static char* TrimTrailingZeros(char* str);
#endif   /* HAVE_INT64_TIMESTAMP */

static char* AppendSeconds(char* cp, int sec, fsec_t fsec, int precision, bool fillzeros);
static void AdjustFractSeconds(double frac, struct pg_tm* tm, fsec_t* fsec, int scale);
static void AdjustFractDays(double frac, struct pg_tm* tm, fsec_t* fsec, int scale);
#ifdef DOLPHIN
static bool int8_to_tm(int64 int_val, struct pg_tm *tm, unsigned int date_flag, int *date_type);
static bool lldiv_decode_tm_internal(lldiv_t *arg,  struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag, int *date_type);
#endif
const int day_tab[2][13] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0}, {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0}};

char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};

char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", NULL};

const char* unitnms[20] = {"year", "quarter", "month", "week", "day", "hour", "minute", "second", "microsecond", 
                           "year_month", "day_hour", "day_minute", "day_second", "hour_minute", "hour_second",
                           "minute_second", "day_microsecond", "hour_microsecond", "minute_microsecond", "second_microsecond"};

const int units_size = sizeof(unitnms) / sizeof(unitnms[0]);

/*****************************************************************************
 *	 PRIVATE ROUTINES														 *
 *****************************************************************************/

/*
 * Definitions for squeezing values into "value"
 * We set aside a high bit for a sign, and scale the timezone offsets
 * in minutes by a factor of 15 (so can represent quarter-hour increments).
 */
#define ABS_SIGNBIT ((char)0200)
#define VALMASK ((char)0177)
#define POS(n) (n)
#define NEG(n) ((n) | ABS_SIGNBIT)
#define SIGNEDCHAR(c) ((c)&ABS_SIGNBIT ? -((c)&VALMASK) : (c))
#define FROMVAL(tp) ((-SIGNEDCHAR((tp)->value)) * 15) /* uncompress */
#define TOVAL(tp, v) ((tp)->value = (((v) < 0) ? NEG((-(v)) / 15) : POS(v) / 15))

static const datetkn datetktbl[] = {
    /*	text, token, lexval */
    {EARLY, RESERV, DTK_EARLY},     /* "-infinity" reserved for "early time" */
    {DA_D, ADBC, AD},               /* "ad" for years > 0 */
    {"allballs", RESERV, DTK_ZULU}, /* 00:00:00 */
    {"am", AMPM, AM},
    {"apr", MONTH, 4},
    {"april", MONTH, 4},
    {"at", IGNORE_DTF, 0}, /* "at" (throwaway) */
    {"aug", MONTH, 8},
    {"august", MONTH, 8},
    {DB_C, ADBC, BC},                /* "bc" for years <= 0 */
    {DCURRENT, RESERV, DTK_CURRENT}, /* "current" is always now */
    {"d", UNITS, DTK_DAY},           /* "day of month" for ISO input */
    {"dec", MONTH, 12},
    {"december", MONTH, 12},
    {"dow", UNITS, DTK_DOW}, /* day of week */
    {"doy", UNITS, DTK_DOY}, /* day of year */
    {"dst", DTZMOD, 6},
    {EPOCH, RESERV, DTK_EPOCH}, /* "epoch" reserved for system epoch time */
    {"feb", MONTH, 2},
    {"february", MONTH, 2},
    {"fri", DOW, 5},
    {"friday", DOW, 5},
    {"h", UNITS, DTK_HOUR},          /* "hour" */
    {LATE, RESERV, DTK_LATE},        /* "infinity" reserved for "late time" */
    {INVALID, RESERV, DTK_INVALID},  /* "invalid" reserved for bad time */
    {"isodow", UNITS, DTK_ISODOW},   /* ISO day of week, Sunday == 7 */
    {"isoyear", UNITS, DTK_ISOYEAR}, /* year in terms of the ISO week date */
    {"j", UNITS, DTK_JULIAN},
    {"jan", MONTH, 1},
    {"january", MONTH, 1},
    {"jd", UNITS, DTK_JULIAN},
    {"jul", MONTH, 7},
    {"julian", UNITS, DTK_JULIAN},
    {"july", MONTH, 7},
    {"jun", MONTH, 6},
    {"june", MONTH, 6},
    {"m", UNITS, DTK_MONTH}, /* "month" for ISO input */
    {"mar", MONTH, 3},
    {"march", MONTH, 3},
    {"may", MONTH, 5},
    {"mm", UNITS, DTK_MINUTE}, /* "minute" for ISO input */
    {"mon", DOW, 1},
    {"monday", DOW, 1},
    {"nov", MONTH, 11},
    {"november", MONTH, 11},
    {NOW, RESERV, DTK_NOW}, /* current transaction time */
    {"oct", MONTH, 10},
    {"october", MONTH, 10},
    {"on", IGNORE_DTF, 0}, /* "on" (throwaway) */
    {"pm", AMPM, PM},
    {"s", UNITS, DTK_SECOND}, /* "seconds" for ISO input */
    {"sat", DOW, 6},
    {"saturday", DOW, 6},
    {"sep", MONTH, 9},
    {"sept", MONTH, 9},
    {"september", MONTH, 9},
    {"sun", DOW, 0},
    {"sunday", DOW, 0},
    {"t", ISOTIME, DTK_TIME}, /* Filler for ISO time fields */
    {"thu", DOW, 4},
    {"thur", DOW, 4},
    {"thurs", DOW, 4},
    {"thursday", DOW, 4},
    {TODAY, RESERV, DTK_TODAY},       /* midnight */
    {TOMORROW, RESERV, DTK_TOMORROW}, /* tomorrow midnight */
    {"tue", DOW, 2},
    {"tues", DOW, 2},
    {"tuesday", DOW, 2},
    {"undefined", RESERV, DTK_INVALID}, /* pre-v6.1 invalid time */
    {"wed", DOW, 3},
    {"wednesday", DOW, 3},
    {"weds", DOW, 3},
    {"y", UNITS, DTK_YEAR},            /* "year" for ISO input */
    {YESTERDAY, RESERV, DTK_YESTERDAY} /* yesterday midnight */
};

static int szdatetktbl = sizeof datetktbl / sizeof datetktbl[0];

static datetkn deltatktbl[] = {
    /* text, token, lexval */
    {"@", IGNORE_DTF, 0},                 /* openGauss relative prefix */
    {DAGO, AGO, 0},                       /* "ago" indicates negative time offset */
    {"c", UNITS, DTK_CENTURY},            /* "century" relative */
    {"cc", UNITS, DTK_CENTURY},           /* "century" relative */
    {"cent", UNITS, DTK_CENTURY},         /* "century" relative */
    {"centuries", UNITS, DTK_CENTURY},    /* "centuries" relative */
    {DCENTURY, UNITS, DTK_CENTURY},       /* "century" relative */
    {"d", UNITS, DTK_DAY},                /* "day" relative */
    {DDAY, UNITS, DTK_DAY},               /* "day" relative */
    {"days", UNITS, DTK_DAY},             /* "days" relative */
    {"dd", UNITS, DTK_DAY},               /* "day" relative */
    {"ddd", UNITS, DTK_DAY},              /* "day" relative */
    {"dec", UNITS, DTK_DECADE},           /* "decade" relative */
    {DDECADE, UNITS, DTK_DECADE},         /* "decade" relative */
    {"decades", UNITS, DTK_DECADE},       /* "decades" relative */
    {"decs", UNITS, DTK_DECADE},          /* "decades" relative */
    {"h", UNITS, DTK_HOUR},               /* "hour" relative */
    {"hh", UNITS, DTK_HOUR},              /* "hour" relative */
    {DHOUR, UNITS, DTK_HOUR},             /* "hour" relative */
    {"hours", UNITS, DTK_HOUR},           /* "hours" relative */
    {"hr", UNITS, DTK_HOUR},              /* "hour" relative */
    {"hrs", UNITS, DTK_HOUR},             /* "hours" relative */
    {INVALID, RESERV, DTK_INVALID},       /* reserved for invalid time */
    {"j", UNITS, DTK_DAY},                /* "day" relative */
    {"m", UNITS, DTK_MINUTE},             /* "minute" relative */
    {"mi", UNITS, DTK_MINUTE},            /* "minute" relative */
    {"microsecon", UNITS, DTK_MICROSEC},  /* "microsecond" relative */
    {"mil", UNITS, DTK_MILLENNIUM},       /* "millennium" relative */
    {"millennia", UNITS, DTK_MILLENNIUM}, /* "millennia" relative */
    {DMILLENNIUM, UNITS, DTK_MILLENNIUM}, /* "millennium" relative */
    {"millisecon", UNITS, DTK_MILLISEC},  /* relative */
    {"mils", UNITS, DTK_MILLENNIUM},      /* "millennia" relative */
    {"min", UNITS, DTK_MINUTE},           /* "minute" relative */
    {"mins", UNITS, DTK_MINUTE},          /* "minutes" relative */
    {DMINUTE, UNITS, DTK_MINUTE},         /* "minute" relative */
    {"minutes", UNITS, DTK_MINUTE},       /* "minutes" relative */
    {"mm", UNITS, DTK_MONTH},             /* "month" relative */
    {"mon", UNITS, DTK_MONTH},            /* "months" relative */
    {"mons", UNITS, DTK_MONTH},           /* "months" relative */
    {DMONTH, UNITS, DTK_MONTH},           /* "month" relative */
    {"months", UNITS, DTK_MONTH},
    {"ms", UNITS, DTK_MILLISEC},
    {"msec", UNITS, DTK_MILLISEC},
    {DMILLISEC, UNITS, DTK_MILLISEC},
    {"mseconds", UNITS, DTK_MILLISEC},
    {"msecs", UNITS, DTK_MILLISEC},
    {"q", UNITS, DTK_QUARTER},      /* "quarter" relative */
    {"qtr", UNITS, DTK_QUARTER},    /* "quarter" relative */
    {DQUARTER, UNITS, DTK_QUARTER}, /* "quarter" relative */
    {"s", UNITS, DTK_SECOND},
    {"sec", UNITS, DTK_SECOND},
    {DSECOND, UNITS, DTK_SECOND},
    {"seconds", UNITS, DTK_SECOND},
    {"secs", UNITS, DTK_SECOND},
    {DTIMEZONE, UNITS, DTK_TZ},           /* "timezone" time offset */
    {"timezone_h", UNITS, DTK_TZ_HOUR},   /* timezone hour units */
    {"timezone_m", UNITS, DTK_TZ_MINUTE}, /* timezone minutes units */
    {"undefined", RESERV, DTK_INVALID},   /* pre-v6.1 invalid time */
    {"us", UNITS, DTK_MICROSEC},          /* "microsecond" relative */
    {"usec", UNITS, DTK_MICROSEC},        /* "microsecond" relative */
    {DMICROSEC, UNITS, DTK_MICROSEC},     /* "microsecond" relative */
    {"useconds", UNITS, DTK_MICROSEC},    /* "microseconds" relative */
    {"usecs", UNITS, DTK_MICROSEC},       /* "microseconds" relative */
    {"w", UNITS, DTK_WEEK},               /* "week" relative */
    {DWEEK, UNITS, DTK_WEEK},             /* "week" relative */
    {"weeks", UNITS, DTK_WEEK},           /* "weeks" relative */
    {"y", UNITS, DTK_YEAR},               /* "year" relative */
    {DYEAR, UNITS, DTK_YEAR},             /* "year" relative */
    {"years", UNITS, DTK_YEAR},           /* "years" relative */
    {"yr", UNITS, DTK_YEAR},              /* "year" relative */
    {"yrs", UNITS, DTK_YEAR},             /* "years" relative */
    {"yyyy", UNITS, DTK_YEAR}             /* "year" relative */
};

static int szdeltatktbl = sizeof deltatktbl / sizeof deltatktbl[0];
#ifdef DOLPHIN
#define NUMERIC_MAX_STRING_LENGTH (NUMERIC_MAX_PRECISION + 2)
/* Position for YYYY-DD-MM HH-MM-DD.FFFFFF AM in default format */
static unsigned char internal_format_positions[]=
{0, 1, 2, 3, 4, 5, 6, (unsigned char) 255};
unsigned long long pow_of_10[20]=
{
  1, 10, 100, 1000, 10000UL, 100000UL, 1000000UL, 10000000UL,
  100000000ULL, 1000000000ULL, 10000000000ULL, 100000000000ULL,
  1000000000000ULL, 10000000000000ULL, 100000000000000ULL,
  1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL,
  1000000000000000000ULL, 10000000000000000000ULL
};
#endif
/*
 * strtoi --- just like strtol, but returns int not long
 */
static int strtoi(const char* nptr, char** endptr, int base)
{
    long val;

    val = strtol(nptr, endptr, base);
#ifdef HAVE_LONG_INT_64
    if (val != (long)((int32)val))
        errno = ERANGE;
#endif
    return (int)val;
}

/*
 * Calendar time to Julian date conversions.
 * Julian date is commonly used in astronomical applications,
 *	since it is numerically accurate and computationally simple.
 * The algorithms here will accurately convert between Julian day
 *	and calendar date for all non-negative Julian days
 *	(i.e. from Nov 24, -4713 on).
 *
 * These routines will be used by other date/time packages
 * - thomas 97/02/25
 *
 * Rewritten to eliminate overflow problems. This now allows the
 * routines to work correctly for all Julian day counts from
 * 0 to 2147483647	(Nov 24, -4713 to Jun 3, 5874898) assuming
 * a 32-bit integer. Longer types should also work to the limits
 * of their precision.
 */

int date2j(int y, int m, int d)
{
#ifdef DOLPHIN
    if (y == 0 && m == 0 && d == 0) return JDATE_ALL_ZERO_VALUE;
#endif
    int julian;
    int century;

    if (m > 2) {
        m += 1;
        y += 4800;
    } else {
        m += 13;
        y += 4799;
    }

    century = y / 100;
    julian = y * 365 - 32167;
    julian += y / 4 - century + century / 4;
    julian += 7834 * m / 256 + d;

    return julian;
} /* date2j() */

void j2date(int jd, int* year, int* month, int* day)
{
    unsigned int julian;
    unsigned int quad;
    unsigned int extra;
    int y;

    julian = jd;
    julian += 32044;
    quad = julian / 146097;
    extra = (julian - quad * 146097) * 4 + 3;
    julian += 60 + quad * 3 + extra / 146097;
    quad = julian / 1461;
    julian -= quad * 1461;
    y = julian * 4 / 1461;
    julian = ((y != 0) ? ((julian + 305) % 365) : ((julian + 306) % 366)) + 123;
    y += quad * 4;
    *year = y - 4800;
    quad = julian * 2141 / 65536;
    *day = julian - 7834 * quad / 256;
    *month = (quad + 10) % MONTHS_PER_YEAR + 1;

#ifdef DOLPHIN
    if (jd == JDATE_ALL_ZERO_VALUE) {
        *year = 0;
        *month = 0;
        *day = 0;
    }
#endif

    return;
} /* j2date() */

/*
 * j2day - convert Julian date to day-of-week (0..6 == Sun..Sat)
 *
 * Note: various places use the locution j2day(date - 1) to produce a
 * result according to the convention 0..6 = Mon..Sun.	This is a bit of
 * a crock, but will work as long as the computation here is just a modulo.
 */
int j2day(int date)
{
    unsigned int day;

    day = date;

    day += 1;
    day %= 7;

    return (int)day;
} /* j2day() */

/*
 * GetCurrentDateTime()
 *
 * Get the transaction start time ("now()") broken down as a struct pg_tm.
 */
void GetCurrentDateTime(struct pg_tm* tm)
{
    int tz;
    fsec_t fsec;

    timestamp2tm(GetCurrentTransactionStartTimestamp(), &tz, tm, &fsec, NULL, NULL);
    /* Note: don't pass NULL tzp to timestamp2tm; affects behavior */
}

/*
 * GetCurrentTimeUsec()
 *
 * Get the transaction start time ("now()") broken down as a struct pg_tm,
 * including fractional seconds and timezone offset.
 *
 * Internally, we cache the result, since this could be called many times
 * in a transaction, within which now() doesn't change.
 */
void GetCurrentTimeUsec(struct pg_tm* tm, fsec_t* fsec, int* tzp)
{
    TimestampTz cur_ts = GetCurrentTransactionStartTimestamp();

    /*
     * The cache key must include both current time and current timezone.
     * By representing the timezone by just a pointer, we're assuming that
     * distinct timezone settings could never have the same pointer value.
     * This is true by virtue of the hashtable used inside pg_tzset();
     * however, it might need another look if we ever allow entries in that
     * hash to be recycled.
     */
    if (cur_ts != u_sess->cache_ts || session_timezone != u_sess->cache_timezone) {
        /*
         * Make sure cache is marked invalid in case of error after partial
         * update within timestamp2tm.
         */
        u_sess->cache_timezone = NULL;

        /*
         * Perform the computation, storing results into cache.  We do not
         * really expect any error here, since current time surely ought to be
         * within range, but check just for sanity's sake.
         */
        if (timestamp2tm(cur_ts,
                         &u_sess->cache_tz, &u_sess->cache_tm, &u_sess->cache_fsec,
                         NULL, session_timezone) != 0) {
            ereport(ERROR,
                    (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                     errmsg("timestamp out of range")));
        }

        /* OK, so mark the cache valid. */
        u_sess->cache_ts = cur_ts;
        u_sess->cache_timezone = session_timezone;
    }

    *tm = u_sess->cache_tm;
    *fsec = u_sess->cache_fsec;
    if (tzp != NULL) {
        *tzp = u_sess->cache_tz;
    }
}

/* TrimTrailingZeros()
 * ... resulting from printing numbers with full precision.
 *
 * Returns a pointer to the new end of string.  No NUL terminator is put
 * there; callers are responsible for NUL terminating str themselves.
 *
 * Before Postgres 8.4, this always left at least 2 fractional digits,
 * but conversations on the lists suggest this isn't desired
 * since showing '0.10' is misleading with values of precision(1).
 */
#ifndef HAVE_INT64_TIMESTAMP
static char* TrimTrailingZeros(char* str)
{
    int len = strlen(str);

    while (len > 1 && *(str + len - 1) == '0' && *(str + len - 2) != '.') {
        len--;
    }
    return str + len;
}
#endif

/*
 * Append seconds and fractional seconds (if any) at *cp.
 *
 * precision is the max number of fraction digits, fillzeros says to
 * pad to two integral-seconds digits.
 *
 * Returns a pointer to the new end of string.  No NUL terminator is put
 * there; callers are responsible for NUL terminating str themselves.
 *
 * Note that any sign is stripped from the input seconds values.
 */
static char* AppendSeconds(char* cp, int sec, fsec_t fsec, int precision, bool fillzeros)
{
    Assert(precision >= 0);

#ifdef HAVE_INT64_TIMESTAMP
    /* fsec_t is just an int32 */

    if (fillzeros)
        cp = pg_ultostr_zeropad(cp, Abs(sec), 2);
    else
        cp = pg_ultostr(cp, Abs(sec));

    if (fsec != 0)
    {
        int32 value = Abs(fsec);
        char *end = &cp[precision + 1];
        bool gotnonzero = false;

        *cp++ = '.';

        /*
         * Append the fractional seconds part.  Note that we don't want any
         * trailing zeros here, so since we're building the number in reverse
         * we'll skip appending zeros until we've output a non-zero digit.
         */
        while (precision--)
        {
            int32 oldval = value;
            int32 remainder;

            value /= 10;
            remainder = oldval - value * 10;

            /* check if we got a non-zero */
            if (remainder)
                gotnonzero = true;

            if (gotnonzero)
                cp[precision] = '0' + remainder;
            else
                end = &cp[precision];
        }

        /*
         * If we still have a non-zero value then precision must have not been
         * enough to print the number.  We punt the problem to pg_ultostr(),
         * which will generate a correct answer in the minimum valid width.
         */
        if (value)
            return pg_ultostr(cp, Abs(fsec));

        return end;
    }
    else
        return cp;
#else
    /* fsec_t is a double */

    if (fsec == 0)
    {
        if (fillzeros)
            return pg_ultostr_zeropad(cp, Abs(sec), 2);
        else
            return pg_ultostr(cp, Abs(sec));
    }
    else
    {
        if (fillzeros)
            rc = sprintf_s(cp, MAXDATELEN, "%0*.*f", precision + 3, precision, fabs(sec + fsec));
        else
            rc = sprintf_s(cp, MAXDATELEN, "%.*f", precision, fabs(sec + fsec));
        securec_check_ss(rc, "\0", "\0");
        return TrimTrailingZeros(cp);
    }
#endif   /* HAVE_INT64_TIMESTAMP */
}

/*
 * Variant of above that's specialized to timestamp case.
 *
 * Returns a pointer to the new end of string.  No NUL terminator is put
 * there; callers are responsible for NUL terminating str themselves.
 */
static char* AppendTimestampSeconds(char* cp, struct pg_tm* tm, fsec_t fsec)
{
    /*
     * In float mode, don't print fractional seconds before 1 AD, since it's
     * unlikely there's any precision left ...
     */
#ifndef HAVE_INT64_TIMESTAMP
    if (tm->tm_year <= 0)
        fsec = 0;
#endif
    return AppendSeconds(cp, tm->tm_sec, fsec, MAX_TIMESTAMP_PRECISION, true);
}

/*
 * Multiply frac by scale (to produce seconds) and add to *tm & *fsec.
 * We assume the input frac is less than 1 so overflow is not an issue.
 */
static void AdjustFractSeconds(double frac, struct pg_tm* tm, fsec_t* fsec, int scale)
{
    int sec;

    if (frac == 0)
        return;
    frac *= scale;
    sec = (int)frac;
    tm->tm_sec += sec;
    frac -= sec;
#ifdef HAVE_INT64_TIMESTAMP
    *fsec += rint(frac * 1000000);
#else
    *fsec += frac;
#endif
}

/* As above, but initial scale produces days */
static void AdjustFractDays(double frac, struct pg_tm* tm, fsec_t* fsec, int scale)
{
    int extra_days;

    if (frac == 0)
        return;
    frac *= scale;
    extra_days = (int)frac;
    tm->tm_mday += extra_days;
    frac -= extra_days;
    AdjustFractSeconds(frac, tm, fsec, SECS_PER_DAY);
}

/* Fetch a fractional-second value with suitable error checking */
static int ParseFractionalSecond(char* cp, fsec_t* fsec)
{
    double frac;

    /* Caller should always pass the start of the fraction part */
    Assert(*cp == '.');
    errno = 0;
    frac = strtod(cp, &cp);
    /* check for parse failure */
    if (*cp != '\0' || errno != 0)
        return DTERR_BAD_FORMAT;
#ifdef HAVE_INT64_TIMESTAMP
    *fsec = rint(frac * 1000000);
#else
    *fsec = frac;
#endif
    return 0;
}

#ifdef DOLPHIN
/**
 * A special method for handling fractional second, compatible with MySQL.
*/
static int SpecialFractionalSecond(char* cp, fsec_t* fsec, unsigned int fmask)
{
    *fsec = 0;

    if (strlen(cp) >= 2 && *cp == '.' && isdigit(cp[1])) {
        int field_length = 6;
        ulong value = 0;
        ++cp;
        while (*cp != '\0' && isdigit(*cp) && field_length--) {
            value = value * 10 + (*cp - '0');
            ++cp;
        }

        if (field_length > 0) {
            value *= pow_of_10[field_length];
        } else {
            if ((fmask & DTK_DATE_M) != DTK_DATE_M && isdigit(*cp)) {
                while (isdigit(*(cp + 1))) {
                    ++cp;
                }
            }

            if ((*cp - '0') >= 5) {
                ++value;
            }
        }
        *fsec = value;
    }

    return 0;
}
#endif

/* ParseDateTime()
 *	Break string into tokens based on a date/time context.
 *	Returns 0 if successful, DTERR code if bogus input detected.
 *
 * timestr - the input string
 * workbuf - workspace for field string storage. This must be
 *	 larger than the largest legal input for this datetime type --
 *	 some additional space will be needed to NUL terminate fields.
 * buflen - the size of workbuf
 * field[] - pointers to field strings are returned in this array
 * ftype[] - field type indicators are returned in this array
 * maxfields - dimensions of the above two arrays
 * *numfields - set to the actual number of fields detected
 *
 * The fields extracted from the input are stored as separate,
 * null-terminated strings in the workspace at workbuf. Any text is
 * converted to lower case.
 *
 * Several field types are assigned:
 *	DTK_NUMBER - digits and (possibly) a decimal point
 *	DTK_DATE - digits and two delimiters, or digits and text
 *	DTK_TIME - digits, colon delimiters, and possibly a decimal point
 *	DTK_STRING - text (no digits or punctuation)
 *	DTK_SPECIAL - leading "+" or "-" followed by text
 *	DTK_TZ - leading "+" or "-" followed by digits (also eats ':', '.', '-')
 *
 * Note that some field types can hold unexpected items:
 *	DTK_NUMBER can hold date fields (yy.ddd)
 *	DTK_STRING can hold months (January) and time zones (PST)
 *	DTK_DATE can hold time zone names (America/New_York, GMT-8)
 */
int ParseDateTime(
    const char* timestr, char* workbuf, size_t buflen, char** field, int* ftype, int maxfields, int* numfields)
{
    int nf = 0;
    const char* cp = timestr;
    char* bufp = workbuf;
    const char* bufend = workbuf + buflen;

    /*
     * Set the character pointed-to by "bufptr" to "newchar", and increment
     * "bufptr". "end" gives the end of the buffer -- we return an error if
     * there is no space left to append a character to the buffer. Note that
     * "bufptr" is evaluated twice.
     */
#define APPEND_CHAR(bufptr, end, newchar) \
    do {                                  \
        if (((bufptr) + 1) >= (end))      \
            return DTERR_BAD_FORMAT;      \
        *(bufptr)++ = newchar;            \
    } while (0)

    /* outer loop through fields */
    while (*cp != '\0') {
        /* Ignore spaces between fields */
        if (isspace((unsigned char)*cp)) {
            cp++;
            continue;
        }

        /* Record start of current field */
        if (nf >= maxfields)
            return DTERR_BAD_FORMAT;
        field[nf] = bufp;

        /* leading digit? then date or time */
        if (isdigit((unsigned char)*cp)) {
            APPEND_CHAR(bufp, bufend, *cp++);
            while (isdigit((unsigned char)*cp)) {
                APPEND_CHAR(bufp, bufend, *cp++);
            }

            /* time field? */
            if (*cp == ':') {
                ftype[nf] = DTK_TIME;
                APPEND_CHAR(bufp, bufend, *cp++);
                while (isdigit((unsigned char)*cp) || (*cp == ':') || (*cp == '.')) {
                    APPEND_CHAR(bufp, bufend, *cp++);
                }
            }
            /* date field? allow embedded text month */
            else if (*cp == '-' || *cp == '/' || *cp == '.') {
                /* save delimiting character to use later */
                char delim = *cp;

                APPEND_CHAR(bufp, bufend, *cp++);
                /* second field is all digits? then no embedded text month */
                if (isdigit((unsigned char)*cp)) {
                    ftype[nf] = ((delim == '.') ? DTK_NUMBER : DTK_DATE);
                    while (isdigit((unsigned char)*cp)) {
                        APPEND_CHAR(bufp, bufend, *cp++);
                    }

                    /*
                     * insist that the delimiters match to get a three-field
                     * date.
                     */
                    if (*cp == delim) {
                        ftype[nf] = DTK_DATE;
                        APPEND_CHAR(bufp, bufend, *cp++);
                        while (isdigit((unsigned char)*cp) || *cp == delim) {
                            APPEND_CHAR(bufp, bufend, *cp++);
                        }
                    }
                } else {
                    ftype[nf] = DTK_DATE;
                    while (isalnum((unsigned char)*cp) || *cp == delim) {
                        APPEND_CHAR(bufp, bufend, pg_tolower((unsigned char)*cp++));
                    }
                }
            }

            /*
             * otherwise, number only and will determine year, month, day, or
             * concatenated fields later...
             */
            else
                ftype[nf] = DTK_NUMBER;
        }
        /* Leading decimal point? Then fractional seconds... */
        else if (*cp == '.') {
            APPEND_CHAR(bufp, bufend, *cp++);
            while (isdigit((unsigned char)*cp)) {
                APPEND_CHAR(bufp, bufend, *cp++);
            }

            ftype[nf] = DTK_NUMBER;
        }

        /*
         * text? then date string, month, day of week, special, or timezone
         */
        else if (isalpha((unsigned char)*cp)) {
            bool is_date = false;

            ftype[nf] = DTK_STRING;
            APPEND_CHAR(bufp, bufend, pg_tolower((unsigned char)*cp++));
            while (isalpha((unsigned char)*cp)) {
                APPEND_CHAR(bufp, bufend, pg_tolower((unsigned char)*cp++));
            }

            /*
             * Dates can have embedded '-', '/', or '.' separators.  It could
             * also be a timezone name containing embedded '/', '+', '-', '_',
             * or ':' (but '_' or ':' can't be the first punctuation). If the
             * next character is a digit or '+', we need to check whether what
             * we have so far is a recognized non-timezone keyword --- if so,
             * don't believe that this is the start of a timezone.
             */
            is_date = false;
            if (*cp == '-' || *cp == '/' || *cp == '.')
                is_date = true;
            else if (*cp == '+' || isdigit((unsigned char)*cp)) {
                *bufp = '\0'; /* null-terminate current field value */
                /* we need search only the core token table, not TZ names */
                if (datebsearch(field[nf], datetktbl, szdatetktbl) == NULL)
                    is_date = true;
            }
            if (is_date) {
                ftype[nf] = DTK_DATE;
                do {
                    APPEND_CHAR(bufp, bufend, pg_tolower((unsigned char)*cp++));
                } while (*cp == '+' || *cp == '-' || *cp == '/' || *cp == '_' || *cp == '.' || *cp == ':' ||
                         isalnum((unsigned char)*cp));
            }
        }
        /* sign? then special or numeric timezone */
        else if (*cp == '+' || *cp == '-') {
            APPEND_CHAR(bufp, bufend, *cp++);
            /* soak up leading whitespace */
            while (isspace((unsigned char)*cp)) {
                cp++;
            }
            /* numeric timezone? */
            /* note that "DTK_TZ" could also be a signed float or yyyy-mm */
            if (isdigit((unsigned char)*cp)) {
                ftype[nf] = DTK_TZ;
                APPEND_CHAR(bufp, bufend, *cp++);
                while (isdigit((unsigned char)*cp) || *cp == ':' || *cp == '.' || *cp == '-') {
                    APPEND_CHAR(bufp, bufend, *cp++);
                }
            }
            /* special? */
            else if (isalpha((unsigned char)*cp)) {
                ftype[nf] = DTK_SPECIAL;
                APPEND_CHAR(bufp, bufend, pg_tolower((unsigned char)*cp++));
                while (isalpha((unsigned char)*cp)) {
                    APPEND_CHAR(bufp, bufend, pg_tolower((unsigned char)*cp++));
                }
            }
            /* otherwise something wrong... */
            else
                return DTERR_BAD_FORMAT;
        }
        /* ignore other punctuation but use as delimiter */
        else if (ispunct((unsigned char)*cp)) {
            cp++;
            continue;
        }
        /* otherwise, something is not right... */
        else
            return DTERR_BAD_FORMAT;

        /* force in a delimiter after each field */
        *bufp++ = '\0';
        nf++;
    }

    *numfields = nf;

    return 0;
}

/* DecodeDateTime()
 * Interpret previously parsed fields for general date and time.
 * Return 0 if full date, 1 if only time, and negative DTERR code if problems.
 * (Currently, all callers treat 1 as an error return too.)
 *
 *		External format(s):
 *				"<weekday> <month>-<day>-<year> <hour>:<minute>:<second>"
 *				"Fri Feb-7-1997 15:23:27"
 *				"Feb-7-1997 15:23:27"
 *				"2-7-1997 15:23:27"
 *				"1997-2-7 15:23:27"
 *				"1997.038 15:23:27"		(day of year 1-366)
 *		Also supports input in compact time:
 *				"970207 152327"
 *				"97038 152327"
 *				"20011225T040506.789-07"
 *
 * Use the system-provided functions to get the current time zone
 * if not specified in the input string.
 *
 * If the date is outside the range of pg_time_t (in practice that could only
 * happen if pg_time_t is just 32 bits), then assume UTC time zone - thomas
 * 1997-05-27
 */
#ifdef DOLPHIN
int DecodeDateTimeForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, unsigned int date_flag)
#else
int DecodeDateTimeForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp)
#endif
{
    unsigned int fmask = 0, tmask;
    int type;
    int ptype = 0; /* "prefix type" for ISO y2001m02d04 format */
    int i;
    int val;
    int dterr;
    int mer = HR24;
    bool haveTextMonth = FALSE;
    bool isjulian = FALSE;
    bool is2digits = FALSE;
    bool bc = FALSE;
#ifdef DOLPHIN
    bool ad = FALSE;
#endif
    pg_tz* namedTz = NULL;
    struct pg_tm cur_tm;

    /*
     * We'll insist on at least all of the date fields, but initialize the
     * remaining fields in case they are not set later...
     */
    *dtype = DTK_DATE;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    *fsec = 0;
    /* don't know daylight savings time status apriori */
    tm->tm_isdst = -1;
    if (tzp != NULL)
        *tzp = 0;

    for (i = 0; i < nf; i++) {
        switch (ftype[i]) {
            case DTK_DATE:
                /***
                 * Integral julian day with attached time zone?
                 * All other forms with JD will be separated into
                 * distinct fields, so we handle just this case here.
                 ***/
                if (ptype == DTK_JULIAN) {
                    char* cp = NULL;
                    int val;

                    if (tzp == NULL)
                        return DTERR_BAD_FORMAT;

                    errno = 0;
                    val = strtoi(field[i], &cp, 10);
                    if (errno == ERANGE || val < 0)
                        return DTERR_FIELD_OVERFLOW;

                    j2date(val, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
                    isjulian = TRUE;

                    /* Get the time zone from the end of the string */
                    dterr = DecodeTimezone(cp, tzp);
                    if (dterr)
                        return dterr;

                    tmask = DTK_DATE_M | DTK_TIME_M | DTK_M(TZ);
                    ptype = 0;
                    break;
                }
                /***
                 * Already have a date? Then this might be a time zone name
                 * with embedded punctuation (e.g. "America/New_York") or a
                 * run-together time with trailing time zone (e.g. hhmmss-zz).
                 * - thomas 2001-12-25
                 *
                 * We consider it a time zone if we already have month & day.
                 * This is to allow the form "mmm dd hhmmss tz year", which
                 * we've historically accepted.
                 ***/
                else if (ptype != 0 || ((fmask & (DTK_M(MONTH) | DTK_M(DAY))) == (DTK_M(MONTH) | DTK_M(DAY)))) {
                    /* No time zone accepted? Then quit... */
                    if (tzp == NULL)
                        return DTERR_BAD_FORMAT;

                    if (isdigit((unsigned char)*field[i]) || ptype != 0) {
                        char* cp = NULL;

                        if (ptype != 0) {
                            /* Sanity check; should not fail this test */
                            if (ptype != DTK_TIME)
                                return DTERR_BAD_FORMAT;
                            ptype = 0;
                        }

                        /*
                         * Starts with a digit but we already have a time
                         * field? Then we are in trouble with a date and time
                         * already...
                         */
                        if ((fmask & DTK_TIME_M) == DTK_TIME_M)
                            return DTERR_BAD_FORMAT;

                        if ((cp = strchr(field[i], '-')) == NULL)
                            return DTERR_BAD_FORMAT;

                        /* Get the time zone from the end of the string */
                        dterr = DecodeTimezone(cp, tzp);
                        if (dterr)
                            return dterr;
                        *cp = '\0';

                        /*
                         * Then read the rest of the field as a concatenated
                         * time
                         */
                        dterr = DecodeNumberField(strlen(field[i]), field[i], fmask, &tmask, tm, fsec, &is2digits);
                        if (dterr < 0)
                            return dterr;

                        /*
                         * modify tmask after returning from
                         * DecodeNumberField()
                         */
                        tmask |= DTK_M(TZ);
                    } else {
                        namedTz = pg_tzset(field[i]);
                        if (NULL == namedTz) {
                            /*
                             * We should return an error code instead of
                             * ereport'ing directly, but then there is no way
                             * to report the bad time zone name.
                             */
                            ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                    errmsg("time zone \"%s\" not recognized", field[i])));
                        }
                        /* we'll apply the zone setting below */
                        tmask = DTK_M(TZ);
                    }
                } else {
                    dterr = DecodeDate(field[i], fmask, &tmask, &is2digits, tm);
                    if (dterr)
                        return dterr;
                }
                break;

            case DTK_TIME:
                dterr = DecodeTime(field[i], fmask, INTERVAL_FULL_RANGE, &tmask, tm, fsec);
                if (dterr)
                    return dterr;

                /*
                 * Check upper limit on hours; other limits checked in
                 * DecodeTime()
                 */
                /* test for > 24:00:00 */
#ifdef DOLPHIN
                if (INVALID_DAY_TIME(tm->tm_hour, tm->tm_min, tm->tm_sec, *fsec))
#else
                if (tm->tm_hour > HOURS_PER_DAY ||
                    (tm->tm_hour == HOURS_PER_DAY && (tm->tm_min > 0 || tm->tm_sec > 0 || *fsec > 0)))
#endif
                    return DTERR_FIELD_OVERFLOW;
                break;

            case DTK_TZ: {
                int tz;

                if (tzp == NULL)
                    return DTERR_BAD_FORMAT;

                dterr = DecodeTimezone(field[i], &tz);
                if (dterr)
                    return dterr;
                *tzp = tz;
                tmask = DTK_M(TZ);
            } break;

            case DTK_NUMBER:

                /*
                 * Was this an "ISO date" with embedded field labels? An
                 * example is "y2001m02d04" - thomas 2001-02-04
                 */
                if (ptype != 0) {
                    char* cp = NULL;
                    int val;

                    errno = 0;
                    val = strtoi(field[i], &cp, 10);
                    if (errno == ERANGE)
                        return DTERR_FIELD_OVERFLOW;

                    /*
                     * only a few kinds are allowed to have an embedded
                     * decimal
                     */
                    if (*cp == '.')
                        switch (ptype) {
                            case DTK_JULIAN:
                            case DTK_TIME:
                            case DTK_SECOND:
                                break;
                            default:
                                return DTERR_BAD_FORMAT;
                                break;
                        }
                    else if (*cp != '\0')
                        return DTERR_BAD_FORMAT;

                    switch (ptype) {
                        case DTK_YEAR:
                            tm->tm_year = val;
                            tmask = DTK_M(YEAR);
                            break;

                        case DTK_MONTH:

                            /*
                             * already have a month and hour? then assume
                             * minutes
                             */
                            if ((fmask & DTK_M(MONTH)) != 0 && (fmask & DTK_M(HOUR)) != 0) {
                                tm->tm_min = val;
                                tmask = DTK_M(MINUTE);
                            } else {
                                tm->tm_mon = val;
                                tmask = DTK_M(MONTH);
                            }
                            break;

                        case DTK_DAY:
                            tm->tm_mday = val;
                            tmask = DTK_M(DAY);
                            break;

                        case DTK_HOUR:
                            tm->tm_hour = val;
                            tmask = DTK_M(HOUR);
                            break;

                        case DTK_MINUTE:
                            tm->tm_min = val;
                            tmask = DTK_M(MINUTE);
                            break;

                        case DTK_SECOND:
                            tm->tm_sec = val;
                            tmask = DTK_M(SECOND);
                            if (*cp == '.') {
                                dterr = ParseFractionalSecond(cp, fsec);
                                if (dterr)
                                    return dterr;
                                tmask = DTK_ALL_SECS_M;
                            }
                            break;

                        case DTK_TZ:
                            tmask = DTK_M(TZ);
                            dterr = DecodeTimezone(field[i], tzp);
                            if (dterr)
                                return dterr;
                            break;

                        case DTK_JULIAN:
                            /* previous field was a label for "julian date" */
                            if (val < 0)
                                return DTERR_FIELD_OVERFLOW;
                            tmask = DTK_DATE_M;
                            j2date(val, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
                            isjulian = TRUE;

                            /* fractional Julian Day? */
                            if (*cp == '.') {
                                double time;

                                errno = 0;
                                time = strtod(cp, &cp);
                                if (*cp != '\0' || errno != 0)
                                    return DTERR_BAD_FORMAT;

#ifdef HAVE_INT64_TIMESTAMP
                                time *= USECS_PER_DAY;
#else
                                time *= SECS_PER_DAY;
#endif
                                dt2time(time, &tm->tm_hour, &tm->tm_min, &tm->tm_sec, fsec);
                                tmask |= DTK_TIME_M;
                            }
                            break;

                        case DTK_TIME:
                            /* previous field was "t" for ISO time */
                            dterr = DecodeNumberField(
                                strlen(field[i]), field[i], (fmask | DTK_DATE_M), &tmask, tm, fsec, &is2digits);
                            if (dterr < 0)
                                return dterr;
                            if (tmask != DTK_TIME_M)
                                return DTERR_BAD_FORMAT;
                            break;

                        default:
                            return DTERR_BAD_FORMAT;
                            break;
                    }

                    ptype = 0;
                    *dtype = DTK_DATE;
                } else {
                    char* cp = NULL;
                    int flen;

                    flen = strlen(field[i]);
                    cp = strchr(field[i], '.');

                    /* Embedded decimal and no date yet? */
                    if (cp != NULL && !(fmask & DTK_DATE_M)) {
                        dterr = DecodeDate(field[i], fmask, &tmask, &is2digits, tm);
                        if (dterr)
                            return dterr;
                    }
                    /* embedded decimal and several digits before? */
                    else if (cp != NULL && flen - strlen(cp) > 2) {
                        /*
                         * Interpret as a concatenated date or time Set the
                         * type field to allow decoding other fields later.
                         * Example: 20011223 or 040506
                         */
                        dterr = DecodeNumberField(flen, field[i], fmask, &tmask, tm, fsec, &is2digits);
                        if (dterr < 0)
                            return dterr;
                    } else if (flen > 4) {
                        dterr = DecodeNumberField(flen, field[i], fmask, &tmask, tm, fsec, &is2digits);
                        if (dterr < 0)
                            return dterr;
                    }
                    /* otherwise it is a single date/time field... */
                    else {
                        dterr = DecodeNumber(flen, field[i], haveTextMonth, fmask, &tmask, tm, fsec, &is2digits);
                        if (dterr)
                            return dterr;
                    }
                }
                break;

            case DTK_STRING:
            case DTK_SPECIAL:
                type = DecodeSpecial(i, field[i], &val);
                if (type == IGNORE_DTF)
                    continue;

                tmask = DTK_M(type);
                switch (type) {
                    case RESERV:
                        switch (val) {
                            case DTK_CURRENT:
                                ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("date/time value \"current\" is no longer supported")));

                                return DTERR_BAD_FORMAT;
                                break;

                            case DTK_NOW:
                                tmask = (DTK_DATE_M | DTK_TIME_M | DTK_M(TZ));
                                *dtype = DTK_DATE;
                                GetCurrentTimeUsec(tm, fsec, tzp);
                                break;

                            case DTK_YESTERDAY:
                                tmask = DTK_DATE_M;
                                *dtype = DTK_DATE;
                                GetCurrentDateTime(&cur_tm);
                                j2date(date2j(cur_tm.tm_year, cur_tm.tm_mon, cur_tm.tm_mday) - 1,
                                    &tm->tm_year,
                                    &tm->tm_mon,
                                    &tm->tm_mday);
                                break;

                            case DTK_TODAY:
                                tmask = DTK_DATE_M;
                                *dtype = DTK_DATE;
                                GetCurrentDateTime(&cur_tm);
                                tm->tm_year = cur_tm.tm_year;
                                tm->tm_mon = cur_tm.tm_mon;
                                tm->tm_mday = cur_tm.tm_mday;
                                break;

                            case DTK_TOMORROW:
                                tmask = DTK_DATE_M;
                                *dtype = DTK_DATE;
                                GetCurrentDateTime(&cur_tm);
                                j2date(date2j(cur_tm.tm_year, cur_tm.tm_mon, cur_tm.tm_mday) + 1,
                                    &tm->tm_year,
                                    &tm->tm_mon,
                                    &tm->tm_mday);
                                break;

                            case DTK_ZULU:
                                tmask = (DTK_TIME_M | DTK_M(TZ));
                                *dtype = DTK_DATE;
                                tm->tm_hour = 0;
                                tm->tm_min = 0;
                                tm->tm_sec = 0;
                                if (tzp != NULL)
                                    *tzp = 0;
                                break;

                            default:
                                *dtype = val;
                                break;
                        }

                        break;

                    case MONTH:

                        /*
                         * already have a (numeric) month? then see if we can
                         * substitute...
                         */
                        if ((fmask & DTK_M(MONTH)) && !haveTextMonth && !(fmask & DTK_M(DAY)) && tm->tm_mon >= 1 &&
                            tm->tm_mon <= 31) {
                            tm->tm_mday = tm->tm_mon;
                            tmask = DTK_M(DAY);
                        }
                        haveTextMonth = TRUE;
                        tm->tm_mon = val;
                        break;

                    case DTZMOD:

                        /*
                         * daylight savings time modifier (solves "MET DST"
                         * syntax)
                         */
                        tmask |= DTK_M(DTZ);
                        tm->tm_isdst = 1;
                        if (tzp == NULL)
                            return DTERR_BAD_FORMAT;
                        *tzp += val * MINS_PER_HOUR;
                        break;

                    case DTZ:

                        /*
                         * set mask for TZ here _or_ check for DTZ later when
                         * getting default timezone
                         */
                        tmask |= DTK_M(TZ);
                        tm->tm_isdst = 1;
                        if (tzp == NULL)
                            return DTERR_BAD_FORMAT;
                        *tzp = val * MINS_PER_HOUR;
                        break;

                    case TZ:
                        tm->tm_isdst = 0;
                        if (tzp == NULL)
                            return DTERR_BAD_FORMAT;
                        *tzp = val * MINS_PER_HOUR;
                        break;

                    case IGNORE_DTF:
                        break;

                    case AMPM:
                        mer = val;
                        break;

                    case ADBC:
                        bc = (val == BC);
#ifdef DOLPHIN
                        ad = (val == AD);
#endif
                        break;

                    case DOW:
                        tm->tm_wday = val;
                        break;

                    case UNITS:
                        tmask = 0;
                        ptype = val;
                        break;

                    case ISOTIME:

                        /*
                         * This is a filler field "t" indicating that the next
                         * field is time. Try to verify that this is sensible.
                         */
                        tmask = 0;

                        /* No preceding date? Then quit... */
                        if ((fmask & DTK_DATE_M) != DTK_DATE_M)
                            return DTERR_BAD_FORMAT;

                        /***
                         * We will need one of the following fields:
                         *	DTK_NUMBER should be hhmmss.fff
                         *	DTK_TIME should be hh:mm:ss.fff
                         *	DTK_DATE should be hhmmss-zz
                         ***/
                        if (i >= nf - 1 ||
                            (ftype[i + 1] != DTK_NUMBER && ftype[i + 1] != DTK_TIME && ftype[i + 1] != DTK_DATE))
                            return DTERR_BAD_FORMAT;

                        ptype = val;
                        break;

                    case UNKNOWN_FIELD:

                        /*
                         * Before giving up and declaring error, check to see
                         * if it is an all-alpha timezone name.
                         */
                        namedTz = pg_tzset(field[i]);
                        if (NULL == namedTz)
                            return DTERR_BAD_FORMAT;
                        /* we'll apply the zone setting below */
                        tmask = DTK_M(TZ);
                        break;

                    default:
                        return DTERR_BAD_FORMAT;
                }
                break;

            default:
                return DTERR_BAD_FORMAT;
        }

        if (tmask & fmask)
            return DTERR_BAD_FORMAT;
        fmask |= tmask;
    } /* end loop over fields */

    /* do final checking/adjustment of Y/M/D fields */
#ifdef DOLPHIN
    dterr = ValidateDate(fmask, isjulian, is2digits, bc, ad, tm, date_flag);
#else
    dterr = ValidateDate(fmask, isjulian, is2digits, bc, tm);
#endif
    if (dterr)
        return dterr;

    /* handle AM/PM */
    if (mer != HR24 && tm->tm_hour > HOURS_PER_DAY / 2)
        return DTERR_FIELD_OVERFLOW;
    if (mer == AM && tm->tm_hour == HOURS_PER_DAY / 2)
        tm->tm_hour = 0;
    else if (mer == PM && tm->tm_hour != HOURS_PER_DAY / 2)
        tm->tm_hour += HOURS_PER_DAY / 2;

    /* do additional checking for full date specs... */
    if (*dtype == DTK_DATE) {
        if ((fmask & DTK_DATE_M) != DTK_DATE_M) {
            if ((fmask & DTK_TIME_M) == DTK_TIME_M)
                return 1;
            return DTERR_BAD_FORMAT;
        }

        /*
         * If we had a full timezone spec, compute the offset (we could not do
         * it before, because we need the date to resolve DST status).
         */
        if (namedTz != NULL && tzp != NULL) {
            /* daylight savings time modifier disallowed with full TZ */
            if (fmask & DTK_M(DTZMOD))
                return DTERR_BAD_FORMAT;

            *tzp = DetermineTimeZoneOffset(tm, namedTz);
        }

        /* timezone not specified? then find local timezone if possible */
        if (tzp != NULL && !(fmask & DTK_M(TZ))) {
            /*
             * daylight savings time modifier but no standard timezone? then
             * error
             */
            if (fmask & DTK_M(DTZMOD))
                return DTERR_BAD_FORMAT;

            *tzp = DetermineTimeZoneOffset(tm, session_timezone);
        }
    }

    return 0;
}

/* DetermineTimeZoneOffset()
 *
 * Given a struct pg_tm in which tm_year, tm_mon, tm_mday, tm_hour, tm_min, and
 * tm_sec fields are set, attempt to determine the applicable time zone
 * (ie, regular or daylight-savings time) at that time.  Set the struct pg_tm's
 * tm_isdst field accordingly, and return the actual timezone offset.
 *
 * Note: it might seem that we should use mktime() for this, but bitter
 * experience teaches otherwise.  This code is much faster than most versions
 * of mktime(), anyway.
 */
int DetermineTimeZoneOffset(struct pg_tm* tm, pg_tz* tzp)
{
    int date, sec;
    pg_time_t day, mytime, prevtime, boundary, beforetime, aftertime;
    long int before_gmtoff, after_gmtoff;
    int before_isdst, after_isdst;
    int res;

    if (tzp == session_timezone && u_sess->time_cxt.HasCTZSet) {
        tm->tm_isdst = 0; /* for lack of a better idea */
        return u_sess->time_cxt.CTimeZone;
    }

    /*
     * First, generate the pg_time_t value corresponding to the given
     * y/m/d/h/m/s taken as GMT time.  If this overflows, punt and decide the
     * timezone is GMT.  (We only need to worry about overflow on machines
     * where pg_time_t is 32 bits.)
     */
    if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
        goto overflow;
    date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - UNIX_EPOCH_JDATE;

    day = ((pg_time_t)date) * SECS_PER_DAY;
    if (day / SECS_PER_DAY != date)
        goto overflow;
    sec = tm->tm_sec + (tm->tm_min + tm->tm_hour * MINS_PER_HOUR) * SECS_PER_MINUTE;
    mytime = day + sec;
    /* since sec >= 0, overflow could only be from +day to -mytime */
    if (mytime < 0 && day > 0)
        goto overflow;

    /*
     * Find the DST time boundary just before or following the target time. We
     * assume that all zones have GMT offsets less than 24 hours, and that DST
     * boundaries can't be closer together than 48 hours, so backing up 24
     * hours and finding the "next" boundary will work.
     */
    prevtime = mytime - SECS_PER_DAY;
    if (mytime < 0 && prevtime > 0)
        goto overflow;

    res = pg_next_dst_boundary(&prevtime, &before_gmtoff, &before_isdst, &boundary, &after_gmtoff, &after_isdst, tzp);
    if (res < 0)
        goto overflow; /* failure? */

    if (res == 0) {
        /* Non-DST zone, life is simple */
        tm->tm_isdst = before_isdst;
        return -(int)before_gmtoff;
    }

    /*
     * Form the candidate pg_time_t values with local-time adjustment
     */
    beforetime = mytime - before_gmtoff;
    if ((before_gmtoff > 0 && mytime < 0 && beforetime > 0) || (before_gmtoff <= 0 && mytime > 0 && beforetime < 0))
        goto overflow;
    aftertime = mytime - after_gmtoff;
    if ((after_gmtoff > 0 && mytime < 0 && aftertime > 0) || (after_gmtoff <= 0 && mytime > 0 && aftertime < 0))
        goto overflow;

    /*
     * If both before or both after the boundary time, we know what to do
     */
    if (beforetime <= boundary && aftertime < boundary) {
        tm->tm_isdst = before_isdst;
        return -(int)before_gmtoff;
    }
    if (beforetime > boundary && aftertime >= boundary) {
        tm->tm_isdst = after_isdst;
        return -(int)after_gmtoff;
    }

    /*
     * It's an invalid or ambiguous time due to timezone transition. Prefer
     * the standard-time interpretation.
     */
    if (after_isdst == 0) {
        tm->tm_isdst = after_isdst;
        return -(int)after_gmtoff;
    }
    tm->tm_isdst = before_isdst;
    return -(int)before_gmtoff;

overflow:
    /* Given date is out of range, so assume UTC */
    tm->tm_isdst = 0;
    return 0;
}

/* DecodeTimeOnly()
 * Interpret parsed string as time fields only.
 * Returns 0 if successful, DTERR code if bogus input detected.
 *
 * Note that support for time zone is here for
 * SQL92 TIME WITH TIME ZONE, but it reveals
 * bogosity with SQL92 date/time standards, since
 * we must infer a time zone from current time.
 * - thomas 2000-03-10
 * Allow specifying date to get a better time zone,
 * if time zones are allowed. - thomas 2001-12-26
 */
#ifdef DOLPHIN
int DecodeTimeOnlyForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, int D, unsigned int date_flag)
#else
int DecodeTimeOnlyForBDatabase(char** field, int* ftype, int nf, int* dtype, struct pg_tm* tm, fsec_t* fsec, int* tzp, int D)
#endif
{
    unsigned int fmask = 0, tmask;
    int type;
    int ptype = 0; /* "prefix type" for ISO h04mm05s06 format */
    int i;
    int val;
    int dterr;
    bool isjulian = FALSE;
    bool is2digits = FALSE;
    bool bc = FALSE;
#ifdef DOLPHIN
    bool ad = FALSE;
#endif
    int mer = HR24;
    pg_tz* namedTz = NULL;

    *dtype = DTK_TIME;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
#ifdef DOLPHIN
    tm->tm_year = 0;
    tm->tm_mon = 0;
    tm->tm_mday = 0;
#endif
    *fsec = 0;
    /* don't know daylight savings time status apriori */
    tm->tm_isdst = -1;

    if (tzp != NULL)
        *tzp = 0;

    for (i = 0; i < nf; i++) {
        switch (ftype[i]) {
            case DTK_DATE:

                /*
                 * Time zone not allowed? Then should not accept dates or time
                 * zones no matter what else!
                 */
                if (tzp == NULL)
                    return DTERR_BAD_FORMAT;

                /* Under limited circumstances, we will accept a date... */
                if (i == 0 && nf >= 2 && (ftype[nf - 1] == DTK_DATE || ftype[1] == DTK_TIME)) {
                    dterr = DecodeDate(field[i], fmask, &tmask, &is2digits, tm);
                    if (dterr)
                        return dterr;
                }
                /* otherwise, this is a time and/or time zone */
                else {
                    if (isdigit((unsigned char)*field[i])) {
                        char* cp = NULL;

                        /*
                         * Starts with a digit but we already have a time
                         * field? Then we are in trouble with time already...
                         */
                        if ((fmask & DTK_TIME_M) == DTK_TIME_M)
                            return DTERR_BAD_FORMAT;

                        /*
                         * Should not get here and fail. Sanity check only...
                         */
                        if ((cp = strchr(field[i], '-')) == NULL)
                            return DTERR_BAD_FORMAT;

                        /* Get the time zone from the end of the string */
                        dterr = DecodeTimezone(cp, tzp);
                        if (dterr)
                            return dterr;
                        *cp = '\0';

                        /*
                         * Then read the rest of the field as a concatenated
                         * time
                         */
                        dterr = DecodeNumberField(
                            strlen(field[i]), field[i], (fmask | DTK_DATE_M), &tmask, tm, fsec, &is2digits);
                        if (dterr < 0)
                            return dterr;
                        ftype[i] = dterr;

                        tmask |= DTK_M(TZ);
                    } else {
                        namedTz = pg_tzset(field[i]);
                        if (NULL == namedTz) {
                            /*
                             * We should return an error code instead of
                             * ereport'ing directly, but then there is no way
                             * to report the bad time zone name.
                             */
                            ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                    errmsg("time zone \"%s\" not recognized", field[i])));
                        }
                        /* we'll apply the zone setting below */
                        ftype[i] = DTK_TZ;
                        tmask = DTK_M(TZ);
                    }
                }
                break;

            case DTK_TIME:
                dterr = DecodeTime(field[i],
#ifdef DOLPHIN
                fmask
#else
                (fmask | DTK_DATE_M)
#endif
                , INTERVAL_FULL_RANGE, &tmask, tm, fsec
#ifdef DOLPHIN
                , false
#endif
                );
                if (dterr)
                    return dterr;
                break;

            case DTK_TZ: {
                int tz;

                if (tzp == NULL)
                    return DTERR_BAD_FORMAT;

                dterr = DecodeTimezone(field[i], &tz);
                if (dterr)
                    return dterr;
                *tzp = tz;
                tmask = DTK_M(TZ);
            } break;

            case DTK_NUMBER:

                /*
                 * Was this an "ISO time" with embedded field labels? An
                 * example is "h04m05s06" - thomas 2001-02-04
                 */
                if (ptype != 0) {
                    char* cp = NULL;
                    int val;

                    /* Only accept a date under limited circumstances */
                    switch (ptype) {
                        case DTK_JULIAN:
                        case DTK_YEAR:
                        case DTK_MONTH:
                        case DTK_DAY:
                            if (tzp == NULL) {
                                return DTERR_BAD_FORMAT;
                            }
                            /* fall through */
                        default:
                            break;
                    }

                    errno = 0;
                    val = strtoi(field[i], &cp, 10);
                    if (errno == ERANGE)
                        return DTERR_FIELD_OVERFLOW;

                    /*
                     * only a few kinds are allowed to have an embedded
                     * decimal
                     */
                    if (*cp == '.')
                        switch (ptype) {
                            case DTK_JULIAN:
                            case DTK_TIME:
                            case DTK_SECOND:
                                break;
                            default:
                                return DTERR_BAD_FORMAT;
                                break;
                        }
                    else if (*cp != '\0')
                        return DTERR_BAD_FORMAT;

                    switch (ptype) {
                        case DTK_YEAR:
                            tm->tm_year = val;
                            tmask = DTK_M(YEAR);
                            break;

                        case DTK_MONTH:

                            /*
                             * already have a month and hour? then assume
                             * minutes
                             */
                            if ((fmask & DTK_M(MONTH)) != 0 && (fmask & DTK_M(HOUR)) != 0) {
                                tm->tm_min = val;
                                tmask = DTK_M(MINUTE);
                            } else {
                                tm->tm_mon = val;
                                tmask = DTK_M(MONTH);
                            }
                            break;

                        case DTK_DAY:
                            tm->tm_mday = val;
                            tmask = DTK_M(DAY);
                            break;

                        case DTK_HOUR:
                            tm->tm_hour = val;
                            tmask = DTK_M(HOUR);
                            break;

                        case DTK_MINUTE:
                            tm->tm_min = val;
                            tmask = DTK_M(MINUTE);
                            break;

                        case DTK_SECOND:
                            tm->tm_sec = val;
                            tmask = DTK_M(SECOND);
                            if (*cp == '.') {
                                dterr = ParseFractionalSecond(cp, fsec);
                                if (dterr)
                                    return dterr;
                                tmask = DTK_ALL_SECS_M;
                            }
                            break;

                        case DTK_TZ:
                            tmask = DTK_M(TZ);
                            dterr = DecodeTimezone(field[i], tzp);
                            if (dterr)
                                return dterr;
                            break;

                        case DTK_JULIAN:
                            /* previous field was a label for "julian date" */
                            if (val < 0)
                                return DTERR_FIELD_OVERFLOW;
                            tmask = DTK_DATE_M;
                            j2date(val, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
                            isjulian = TRUE;

                            if (*cp == '.') {
                                double time;

                                errno = 0;
                                time = strtod(cp, &cp);
                                if (*cp != '\0' || errno != 0)
                                    return DTERR_BAD_FORMAT;

#ifdef HAVE_INT64_TIMESTAMP
                                time *= USECS_PER_DAY;
#else
                                time *= SECS_PER_DAY;
#endif
                                dt2time(time, &tm->tm_hour, &tm->tm_min, &tm->tm_sec, fsec);
                                tmask |= DTK_TIME_M;
                            }
                            break;

                        case DTK_TIME:
                            /* previous field was "t" for ISO time */
                            dterr = DecodeNumberField(
                                strlen(field[i]), field[i], (fmask | DTK_DATE_M), &tmask, tm, fsec, &is2digits);
                            if (dterr < 0)
                                return dterr;
                            ftype[i] = dterr;

                            if (tmask != DTK_TIME_M)
                                return DTERR_BAD_FORMAT;
                            break;

                        default:
                            return DTERR_BAD_FORMAT;
                            break;
                    }

                    ptype = 0;
                    *dtype = DTK_DATE;
                } else {
                    char* cp = NULL;
                    int flen;

                    flen = strlen(field[i]);
                    cp = strchr(field[i], '.');

                    /* Embedded decimal? */
                    if (cp != NULL) {
                        /*
                         * Under limited circumstances, we will accept a
                         * date...
                         */
                        if (i == 0 && nf >= 2 && ftype[nf - 1] == DTK_DATE) {
                            dterr = DecodeDate(field[i], fmask, &tmask, &is2digits, tm);
                            if (dterr)
                                return dterr;
                        }
                        /* embedded decimal and several digits before? */
                        else if (flen - strlen(cp) > 2) {
                            /*
                             * Interpret as a concatenated date or time Set
                             * the type field to allow decoding other fields
                             * later. Example: 20011223 or 040506
                             */
                            dterr =
                                DecodeNumberField(flen, field[i], (fmask | DTK_DATE_M), &tmask, tm, fsec, &is2digits);
                            if (dterr < 0)
                                return dterr;
                            ftype[i] = dterr;
                        } else
                            return DTERR_BAD_FORMAT;
                    } else if (flen > 4) {
                        dterr = DecodeNumberField(flen, field[i], (fmask | DTK_DATE_M), &tmask, tm, fsec, &is2digits);
                        if (dterr < 0)
                            return dterr;
                        ftype[i] = dterr;
                    }
                    /* otherwise it is a single date/time field... */
                    else {
                        dterr = DecodeNumber(flen, field[i], FALSE, (fmask | DTK_DATE_M), &tmask, tm, fsec, &is2digits);
                        if (dterr)
                            return dterr;
                    }
                }
                break;

            case DTK_STRING:
            case DTK_SPECIAL:
                type = DecodeSpecial(i, field[i], &val);
                if (type == IGNORE_DTF)
                    continue;

                tmask = DTK_M(type);
                switch (type) {
                    case RESERV:
                        switch (val) {
                            case DTK_CURRENT:
                                ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("date/time value \"current\" is no longer supported")));
                                return DTERR_BAD_FORMAT;
                                break;

                            case DTK_NOW:
                                tmask = DTK_TIME_M;
                                *dtype = DTK_TIME;
                                GetCurrentTimeUsec(tm, fsec, NULL);
                                break;

                            case DTK_ZULU:
                                tmask = (DTK_TIME_M | DTK_M(TZ));
                                *dtype = DTK_TIME;
                                tm->tm_hour = 0;
                                tm->tm_min = 0;
                                tm->tm_sec = 0;
                                tm->tm_isdst = 0;
                                break;

                            default:
                                return DTERR_BAD_FORMAT;
                        }

                        break;

                    case DTZMOD:

                        /*
                         * daylight savings time modifier (solves "MET DST"
                         * syntax)
                         */
                        tmask |= DTK_M(DTZ);
                        tm->tm_isdst = 1;
                        if (tzp == NULL)
                            return DTERR_BAD_FORMAT;
                        *tzp += val * MINS_PER_HOUR;
                        break;

                    case DTZ:

                        /*
                         * set mask for TZ here _or_ check for DTZ later when
                         * getting default timezone
                         */
                        tmask |= DTK_M(TZ);
                        tm->tm_isdst = 1;
                        if (tzp == NULL)
                            return DTERR_BAD_FORMAT;
                        *tzp = val * MINS_PER_HOUR;
                        ftype[i] = DTK_TZ;
                        break;

                    case TZ:
                        tm->tm_isdst = 0;
                        if (tzp == NULL)
                            return DTERR_BAD_FORMAT;
                        *tzp = val * MINS_PER_HOUR;
                        ftype[i] = DTK_TZ;
                        break;

                    case IGNORE_DTF:
                        break;

                    case AMPM:
                        mer = val;
                        break;

                    case ADBC:
                        bc = (val == BC);
#ifdef DOLPHIN
                        ad = (val == AD);
#endif
                        break;

                    case UNITS:
                        tmask = 0;
                        ptype = val;
                        break;

                    case ISOTIME:
                        tmask = 0;

                        /***
                         * We will need one of the following fields:
                         *	DTK_NUMBER should be hhmmss.fff
                         *	DTK_TIME should be hh:mm:ss.fff
                         *	DTK_DATE should be hhmmss-zz
                         ***/
                        if (i >= nf - 1 ||
                            (ftype[i + 1] != DTK_NUMBER && ftype[i + 1] != DTK_TIME && ftype[i + 1] != DTK_DATE))
                            return DTERR_BAD_FORMAT;

                        ptype = val;
                        break;

                    case UNKNOWN_FIELD:

                        /*
                         * Before giving up and declaring error, check to see
                         * if it is an all-alpha timezone name.
                         */
                        namedTz = pg_tzset(field[i]);
                        if (NULL == namedTz)
                            return DTERR_BAD_FORMAT;
                        /* we'll apply the zone setting below */
                        tmask = DTK_M(TZ);
                        break;

                    default:
                        return DTERR_BAD_FORMAT;
                }
                break;

            default:
                return DTERR_BAD_FORMAT;
        }

        if (tmask & fmask)
            return DTERR_BAD_FORMAT;
        fmask |= tmask;
    } /* end loop over fields */
#ifdef DOLPHIN
    tm->tm_hour += D * 24;
    /* do final checking/adjustment of Y/M/D fields */
    dterr = ValidateDate(fmask, isjulian, is2digits, bc, ad, tm, date_flag);
#else
    dterr = ValidateDate(fmask, isjulian, is2digits, bc, tm);
#endif
    if (dterr)
        return dterr;

    /* handle AM/PM */
    if (mer != HR24 && tm->tm_hour > HOURS_PER_DAY / 2)
        return DTERR_FIELD_OVERFLOW;
    if (mer == AM && tm->tm_hour == HOURS_PER_DAY / 2)
        tm->tm_hour = 0;
    else if (mer == PM && tm->tm_hour != HOURS_PER_DAY / 2)
        tm->tm_hour += HOURS_PER_DAY / 2;
#ifdef DOLPHIN
    /* check validation when contain date, like '2001-10-10 30:00:1' */
    if (tm->tm_year && (tm->tm_sec >= SECS_PER_MINUTE || tm->tm_min >= MINS_PER_HOUR || tm->tm_hour >= HOURS_PER_DAY))
        return DTERR_FIELD_OVERFLOW;

    /* validate time value */
    if (ValidateTimeForBDatabase(false, tm, fsec))
        return DTERR_FIELD_OVERFLOW;
#else
    if (tm->tm_hour < 0 || tm->tm_min < 0 || tm->tm_min > MINS_PER_HOUR - 1 || tm->tm_sec < 0 ||
        tm->tm_sec > SECS_PER_MINUTE || tm->tm_hour > HOURS_PER_DAY ||
        /* test for > 24:00:00 */
        (tm->tm_hour == HOURS_PER_DAY && (tm->tm_min > 0 || tm->tm_sec > 0 || *fsec > 0)) ||
#ifdef HAVE_INT64_TIMESTAMP
        *fsec < INT64CONST(0) || *fsec > USECS_PER_SEC
#else
        *fsec < 0 || *fsec > 1
#endif
    )
        return DTERR_FIELD_OVERFLOW;
#endif
    if ((fmask & DTK_TIME_M) != DTK_TIME_M)
        return DTERR_BAD_FORMAT;

    /*
     * If we had a full timezone spec, compute the offset (we could not do it
     * before, because we may need the date to resolve DST status).
     */
    if (namedTz != NULL) {
        long int gmtoff;

        /* daylight savings time modifier disallowed with full TZ */
        if (fmask & DTK_M(DTZMOD))
            return DTERR_BAD_FORMAT;

        /* if non-DST zone, we do not need to know the date */
        if (pg_get_timezone_offset(namedTz, &gmtoff)) {
            *tzp = -(int)gmtoff;
        } else {
            /* a date has to be specified */
            if ((fmask & DTK_DATE_M) != DTK_DATE_M)
                return DTERR_BAD_FORMAT;
            *tzp = DetermineTimeZoneOffset(tm, namedTz);
        }
    }

    /* timezone not specified? then find local timezone if possible */
    if (tzp != NULL && !(fmask & DTK_M(TZ))) {
        struct pg_tm tt, *tmp = &tt;

        /*
         * daylight savings time modifier but no standard timezone? then error
         */
        if (fmask & DTK_M(DTZMOD))
            return DTERR_BAD_FORMAT;

        if ((fmask & DTK_DATE_M) == 0)
            GetCurrentDateTime(tmp);
        else {
            tmp->tm_year = tm->tm_year;
            tmp->tm_mon = tm->tm_mon;
            tmp->tm_mday = tm->tm_mday;
        }
        tmp->tm_hour = tm->tm_hour;
        tmp->tm_min = tm->tm_min;
        tmp->tm_sec = tm->tm_sec;
        *tzp = DetermineTimeZoneOffset(tmp, session_timezone);
        tm->tm_isdst = tmp->tm_isdst;
    }

    return 0;
}

/* DecodeDate()
 * Decode date string which includes delimiters.
 * Return 0 if okay, a DTERR code if not.
 *
 *	str: field to be parsed
 *	fmask: bitmask for field types already seen
 *	*tmask: receives bitmask for fields found here
 *	*is2digits: set to TRUE if we find 2-digit year
 *	*tm: field values are stored into appropriate members of this struct
 */
static int DecodeDate(char* str, unsigned int fmask, unsigned int* tmask, bool* is2digits, struct pg_tm* tm)
{
    fsec_t fsec;
    int nf = 0;
    int i, len;
    int dterr;
    bool haveTextMonth = FALSE;
    int type, val;
    unsigned int dmask = 0;
    char* field[MAXDATEFIELDS];

    *tmask = 0;

    /* parse this string... */
    while (*str != '\0' && nf < MAXDATEFIELDS) {
        /* skip field separators */
        while (*str != '\0' && !isalnum((unsigned char)*str))
            str++;

        if (*str == '\0')
            return DTERR_BAD_FORMAT; /* end of string after separator */

        field[nf] = str;
        if (isdigit((unsigned char)*str)) {
            while (isdigit((unsigned char)*str))
                str++;
        } else if (isalpha((unsigned char)*str)) {
            while (isalpha((unsigned char)*str))
                str++;
        }

        /* Just get rid of any non-digit, non-alpha characters... */
        if (*str != '\0')
            *str++ = '\0';
        nf++;
    }
#ifdef PGXC
    if (3 == nf && strlen(field[0]) < 3) {
        if (u_sess->time_cxt.DateStyle == USE_GERMAN_DATES && u_sess->time_cxt.DateOrder == DATEORDER_MDY) {
            int month_val = 0;
            int day_val = 0;
            char* cp = NULL;
            month_val = strtoi(field[0], &cp, 10);
            day_val = strtoi(field[1], &cp, 10);
            if (month_val >= 1 && month_val <= 12 && day_val >= 1 && day_val <= 31) {

            } else {
                char* p = field[0];
                field[0] = field[1];
                field[1] = p;
            }
        } else if (3 == nf && strlen(field[0]) < 3 && u_sess->time_cxt.DateStyle == USE_GERMAN_DATES &&
                   u_sess->time_cxt.DateOrder == DATEORDER_YMD) {
            char* p = field[0];
            field[0] = field[2];
            field[2] = p;
        } else if (3 == nf && strlen(field[0]) < 3 && u_sess->time_cxt.DateStyle == USE_SQL_DATES &&
                   u_sess->time_cxt.DateOrder == DATEORDER_YMD) {
            char* p = field[0];
            char* q = field[1];
            field[0] = field[2];
            field[1] = p;
            field[2] = q;
        }
    }
#endif

    /* look first for text fields, since that will be unambiguous month */
    for (i = 0; i < nf; i++) {
        if (isalpha((unsigned char)*field[i])) {
            type = DecodeSpecial(i, field[i], &val);
            if (type == IGNORE_DTF)
                continue;

            dmask = DTK_M(type);
            switch (type) {
                case MONTH:
                    tm->tm_mon = val;
                    haveTextMonth = TRUE;
                    break;

                default:
                    return DTERR_BAD_FORMAT;
            }
            if (fmask & dmask)
                return DTERR_BAD_FORMAT;

            fmask |= dmask;
            *tmask |= dmask;

            /* mark this field as being completed */
            field[i] = NULL;
        }
    }

    /* now pick up remaining numeric fields */
    for (i = 0; i < nf; i++) {
        if (field[i] == NULL)
            continue;

        if ((len = strlen(field[i])) <= 0)
            return DTERR_BAD_FORMAT;

        dterr = DecodeNumber(len, field[i], haveTextMonth, fmask, &dmask, tm, &fsec, is2digits);
        if (dterr)
            return dterr;

        if (fmask & dmask)
            return DTERR_BAD_FORMAT;

        fmask |= dmask;
        *tmask |= dmask;
    }

    if ((fmask & ~(DTK_M(DOY) | DTK_M(TZ))) != DTK_DATE_M)
        return DTERR_BAD_FORMAT;

    /* validation of the field values must wait until ValidateDate() */

    return 0;
}
#ifdef DOLPHIN
int ValidateTimeForBDatabase(bool timeIn24, struct pg_tm* tm, fsec_t* fsec)
{
    /* validate min, sec, fsec */
    if (tm->tm_min < 0 || tm->tm_min >= MINS_PER_HOUR || tm->tm_sec < 0 ||
        tm->tm_sec >= SECS_PER_MINUTE ||
#ifdef HAVE_INT64_TIMESTAMP
        *fsec < INT64CONST(0) || *fsec > USECS_PER_SEC
#else
        *fsec < 0 || *fsec > 1
#endif
        )
        return DTERR_FIELD_OVERFLOW;

    /* validate hour */
    if (timeIn24) {
        if (tm->tm_hour < 0 || tm->tm_hour > HOURS_PER_DAY ||  
            (tm->tm_hour == HOURS_PER_DAY && (tm->tm_min > 0 || tm->tm_sec > 0 || *fsec > 0)))
            return DTERR_FIELD_OVERFLOW;
    } else {
        /* b format time type validation: range is in[-838, 838] */
        if (tm->tm_hour >= B_FORMAT_TIME_BOUND || 
                (tm->tm_hour == B_FORMAT_TIME_BOUND - 1 && tm->tm_min == MINS_PER_HOUR - 1 && 
                tm->tm_sec == SECS_PER_MINUTE - 1 && *fsec)) {
            if (SQL_MODE_STRICT()) {
                return DTERR_FIELD_OVERFLOW;
            } else {
                tm->tm_hour = B_FORMAT_TIME_BOUND - 1;
                tm->tm_min = MINS_PER_HOUR - 1;
                tm->tm_sec = SECS_PER_MINUTE - 1;
                *fsec = 0;
                ereport(WARNING, (errcode(DTERR_FIELD_OVERFLOW), errmsg("Truncated incorrect time value.")));
            }
        }
    }
    return 0;
}

int ValidateDateForBDatabase(bool is2digits, struct pg_tm* tm, unsigned int date_flag) 
{
    return ValidateDate(DTK_DATE_M, false, is2digits, false, false, tm, date_flag);
}
#endif
/* ValidateDate()
 * Check valid year/month/day values, handle BC and DOY cases
 * Return 0 if okay, a DTERR code if not.
 */
#ifdef DOLPHIN
static int ValidateDate(unsigned int fmask, bool isjulian, bool is2digits, bool bc, bool ad, struct pg_tm* tm, unsigned int date_flag)
#else
static int ValidateDate(unsigned int fmask, bool isjulian, bool is2digits, bool bc, struct pg_tm* tm)
#endif
{
    if (fmask & DTK_M(YEAR)) {
        if (isjulian) {
            /* tm_year is correct and should not be touched */
        } else if (bc) {
            /* there is no year zero in AD/BC notation */
            if (tm->tm_year <= 0)
                return DTERR_FIELD_OVERFLOW;
            /* internally, we represent 1 BC as year zero, 2 BC as -1, etc */
            tm->tm_year = -(tm->tm_year - 1);
        } else if (is2digits) {
            /* process 1 or 2-digit input as 1970-2069 AD, allow '0' and '00' */
            if (tm->tm_year < 0) /* just paranoia */
                return DTERR_FIELD_OVERFLOW;
            if (tm->tm_year < 70)
                tm->tm_year += 2000;
            else if (tm->tm_year < 100)
                tm->tm_year += 1900;
        } else {
            /* there is no year zero in AD/BC notation */
#ifdef DOLPHIN
            if (tm->tm_year < 0 || (ad && tm->tm_year == 0))
#else
            if (tm->tm_year <= 0)
#endif
                return DTERR_FIELD_OVERFLOW;
        }
    }

    /* now that we have correct year, decode DOY */
    if (fmask & DTK_M(DOY)) {
        j2date(date2j(tm->tm_year, 1, 1) + tm->tm_yday - 1, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
    }

    /* check for valid month */
    if (fmask & DTK_M(MONTH)) {
#ifdef DOLPHIN
        if ((date_flag & ENABLE_ZERO_MONTH) || (SQL_MODE_NO_ZERO_DATE() && SQL_MODE_STRICT())) {
            if (tm->tm_mon < 1 || tm->tm_mon > MONTHS_PER_YEAR)
                return DTERR_MD_FIELD_OVERFLOW;
        } else if (tm->tm_mon < 0 || tm->tm_mon > MONTHS_PER_YEAR)
#else
        if (tm->tm_mon < 1 || tm->tm_mon > MONTHS_PER_YEAR)
#endif
            return DTERR_MD_FIELD_OVERFLOW;
    }

    /* minimal check for valid day */
    if (fmask & DTK_M(DAY)) {
#ifdef DOLPHIN
        if ((date_flag & ENABLE_ZERO_DAY) || (SQL_MODE_NO_ZERO_DATE() && SQL_MODE_STRICT())) {
            if (tm->tm_mday < 1 || tm->tm_mday > 31)
                return DTERR_MD_FIELD_OVERFLOW;
        } else if(tm->tm_mday < 0 || tm->tm_mday > 31)
#else
        if(tm->tm_mday < 1 || tm->tm_mday > 31)
#endif
            return DTERR_MD_FIELD_OVERFLOW;
    }

#ifdef DOLPHIN
    if ((date_flag & (ENABLE_ZERO_DAY | ENABLE_ZERO_MONTH)) == 0 && (fmask & (DTK_M(DAY) | DTK_M(MONTH) | DTK_M(YEAR)))) {
        int zero_cnt = (tm->tm_mon == 0) + (tm->tm_mday == 0) + (tm->tm_year == 0);
        if (zero_cnt == 3 && SQL_MODE_NO_ZERO_DATE() && !SQL_MODE_STRICT()) {
            return DTERR_ZERO_DATE;
        }
        if (zero_cnt > 1 && zero_cnt < 3) {
            return DTERR_FIELD_OVERFLOW;
        }
    }
#endif

    if ((fmask & DTK_DATE_M) == DTK_DATE_M && (tm->tm_mon && tm->tm_year && tm->tm_mday)) {
        /*
         * Check for valid day of month, now that we know for sure the month
         * and year.  Note we don't use MD_FIELD_OVERFLOW here, since it seems
         * unlikely that "Feb 29" is a YMD-order error.
         */
#ifdef DOLPHIN
        if ((date_flag & ENABLE_ZERO_MONTH) && (tm->tm_mon == 0)) {
            return 0;
        } else if (tm->tm_mday > day_tab[isleap(tm->tm_year)][tm->tm_mon - 1])
#else
        if (tm->tm_mday > day_tab[isleap(tm->tm_year)][tm->tm_mon - 1])
#endif
            return DTERR_FIELD_OVERFLOW;
    }

    return 0;
}

/* DecodeTime()
 * Decode time string which includes delimiters.
 * Return 0 if okay, a DTERR code if not.
 *
 * Only check the lower limit on hours, since this same code can be
 * used to represent time spans.
 *
 * If the range is interval's minute to second mode, in order to simulate
 * A db, there may need some adjustments.
 */
static int DecodeTime(
    const char* str, unsigned int fmask, int range, unsigned int* tmask, struct pg_tm* tm, fsec_t* fsec
#ifdef DOLPHIN
    , bool timeIn24
#endif
    )
{
    char* cp = NULL;
    int dterr;

    *tmask = DTK_TIME_M;

    errno = 0;
    tm->tm_hour = strtoi(str, &cp, 10);
    if (errno == ERANGE)
        return DTERR_FIELD_OVERFLOW;
    if (*cp != ':')
        return DTERR_BAD_FORMAT;
    errno = 0;
    tm->tm_min = strtoi(cp + 1, &cp, 10);
    if (errno == ERANGE)
        return DTERR_FIELD_OVERFLOW;
    if (*cp == '\0') {
        tm->tm_sec = 0;
        *fsec = 0;
        /* If it's a MINUTE TO SECOND interval, take 2 fields as being mm:ss */
        if (range == (INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND))) {
            tm->tm_sec = tm->tm_min;
            tm->tm_min = tm->tm_hour;
            tm->tm_hour = tm->tm_min / MINS_PER_HOUR;
            tm->tm_min -= tm->tm_hour * MINS_PER_HOUR;
        }
    } else if (*cp == '.') {
        /* opengauss: always assume mm:ss.sss is MINUTE TO SECOND 
         * b database : always assume hh:mm:00.sss 
         */
#ifdef DOLPHIN
        dterr = SpecialFractionalSecond(cp, fsec, fmask);
#else
        dterr = ParseFractionalSecond(cp, fsec);
#endif
        if (dterr)
            return dterr;
#ifdef DOLPHIN
        tm->tm_sec = 0;
#else
        tm->tm_sec = tm->tm_min;
        tm->tm_min = tm->tm_hour;
        tm->tm_hour = tm->tm_min / MINS_PER_HOUR;
        tm->tm_min -= tm->tm_hour * MINS_PER_HOUR;
#endif
    } else if (*cp == ':') {
        errno = 0;
        tm->tm_sec = strtoi(cp + 1, &cp, 10);
        if (errno == ERANGE)
            return DTERR_FIELD_OVERFLOW;
        if (*cp == '\0')
            *fsec = 0;
        else if (*cp == '.') {
#ifdef DOLPHIN
            dterr = SpecialFractionalSecond(cp, fsec, fmask);
#else
            dterr = ParseFractionalSecond(cp, fsec);
#endif
            if (dterr)
                return dterr;
        } else
            return DTERR_BAD_FORMAT;
    } else
        return DTERR_BAD_FORMAT;
#ifdef DOLPHIN
    /* do a sanity check */
    return ValidateTimeForBDatabase(timeIn24, tm, fsec);
#else
#ifdef HAVE_INT64_TIMESTAMP
    if (tm->tm_hour < 0 || tm->tm_min < 0 || tm->tm_min > MINS_PER_HOUR - 1 || tm->tm_sec < 0 ||
        tm->tm_sec > SECS_PER_MINUTE || *fsec < INT64CONST(0) || *fsec > USECS_PER_SEC)
        return DTERR_FIELD_OVERFLOW;
#else
    if (tm->tm_hour < 0 || tm->tm_min < 0 || tm->tm_min > MINS_PER_HOUR - 1 || tm->tm_sec < 0 ||
        tm->tm_sec > SECS_PER_MINUTE || *fsec < 0 || *fsec > 1)
        return DTERR_FIELD_OVERFLOW;
#endif

    return 0;
#endif
}

/* DecodeNumber()
 * Interpret plain numeric field as a date value in context.
 * Return 0 if okay, a DTERR code if not.
 */
static int DecodeNumber(int flen, char* str, bool haveTextMonth, unsigned int fmask, unsigned int* tmask,
    struct pg_tm* tm, fsec_t* fsec, bool* is2digits)
{
    int val;
    char* cp = NULL;
    int dterr;

    *tmask = 0;

    errno = 0;
    val = strtoi(str, &cp, 10);
    if (errno == ERANGE)
        return DTERR_FIELD_OVERFLOW;
    if (cp == str)
        return DTERR_BAD_FORMAT;

    if (*cp == '.') {
        /*
         * More than two digits before decimal point? Then could be a date or
         * a run-together time: 2001.360 20011225 040506.789
         */
        if (cp - str > 2) {
            dterr = DecodeNumberField(flen, str, (fmask | DTK_DATE_M), tmask, tm, fsec, is2digits);
            if (dterr < 0)
                return dterr;
            return 0;
        }

        dterr = ParseFractionalSecond(cp, fsec);
        if (dterr)
            return dterr;
    } else if (*cp != '\0')
        return DTERR_BAD_FORMAT;

    /* Special case for day of year */
    if (flen == 3 && (fmask & DTK_DATE_M) == DTK_M(YEAR) && val >= 1 && val <= 366) {
        *tmask = (DTK_M(DOY) | DTK_M(MONTH) | DTK_M(DAY));
        tm->tm_yday = val;
        /* tm_mon and tm_mday can't actually be set yet ... */
        return 0;
    }

    /* Switch based on what we have so far */
    switch (fmask & DTK_DATE_M) {
        case 0:
#ifdef DOLPHIN
            /* b format date: default format is YMD */
            *tmask = DTK_M(YEAR);
            tm->tm_year = val;
#else
            /*
             * Nothing so far; make a decision about what we think the input
             * is.	There used to be lots of heuristics here, but the
             * consensus now is to be paranoid.  It *must* be either
             * YYYY-MM-DD (with a more-than-two-digit year field), or the
             * field order defined by u_sess->time_cxt.DateOrder.
             */
            if (flen >= 3 || u_sess->time_cxt.DateOrder == DATEORDER_YMD) {
                *tmask = DTK_M(YEAR);
                tm->tm_year = val;
            } else if (u_sess->time_cxt.DateOrder == DATEORDER_DMY) {
                *tmask = DTK_M(DAY);
                tm->tm_mday = val;
            } else {
                *tmask = DTK_M(MONTH);
                tm->tm_mon = val;
            }
#endif
            break;

        case (DTK_M(YEAR)):
            /* Must be at second field of YY-MM-DD */
            *tmask = DTK_M(MONTH);
            tm->tm_mon = val;
            break;

        case (DTK_M(MONTH)):
            if (haveTextMonth) {
                /*
                 * We are at the first numeric field of a date that included a
                 * textual month name.	We want to support the variants
                 * MON-DD-YYYY, DD-MON-YYYY, and YYYY-MON-DD as unambiguous
                 * inputs.	We will also accept MON-DD-YY or DD-MON-YY in
                 * either DMY or MDY modes, as well as YY-MON-DD in YMD mode.
                 */
                if (flen >= 3 || u_sess->time_cxt.DateOrder == DATEORDER_YMD) {
                    *tmask = DTK_M(YEAR);
                    tm->tm_year = val;
                } else {
                    *tmask = DTK_M(DAY);
                    tm->tm_mday = val;
                }
            } else {
                /* Must be at second field of MM-DD-YY */
                *tmask = DTK_M(DAY);
                tm->tm_mday = val;
            }
            break;

        case (DTK_M(YEAR) | DTK_M(MONTH)):
            if (haveTextMonth) {
                /* Need to accept DD-MON-YYYY even in YMD mode */
                if (flen >= 3 && *is2digits) {
                    /* Guess that first numeric field is day was wrong */
                    *tmask = DTK_M(DAY); /* YEAR is already set */
                    tm->tm_mday = tm->tm_year;
                    tm->tm_year = val;
                    *is2digits = FALSE;
                } else {
                    *tmask = DTK_M(DAY);
                    tm->tm_mday = val;
                }
            } else {
                /* Must be at third field of YY-MM-DD */
                *tmask = DTK_M(DAY);
                tm->tm_mday = val;
            }
            break;

        case (DTK_M(DAY)):
            /* Must be at second field of DD-MM-YY */
            *tmask = DTK_M(MONTH);
            tm->tm_mon = val;
            break;

        case (DTK_M(MONTH) | DTK_M(DAY)):
            /* Must be at third field of DD-MM-YY or MM-DD-YY */
            *tmask = DTK_M(YEAR);
            tm->tm_year = val;
            break;

        case (DTK_M(YEAR) | DTK_M(MONTH) | DTK_M(DAY)):
            /* we have all the date, so it must be a time field */
            dterr = DecodeNumberField(flen, str, fmask, tmask, tm, fsec, is2digits);
            if (dterr < 0)
                return dterr;
            return 0;

        default:
            /* Anything else is bogus input */
            return DTERR_BAD_FORMAT;
    }

    /*
     * When processing a year field, mark it for adjustment if it's only one
     * or two digits.
     */
    if (*tmask == DTK_M(YEAR))
#ifdef DOLPHIN
        *is2digits = (flen == 2);
#else
        *is2digits = (flen <= 2);
#endif

    return 0;
}

/* DecodeNumberField()
 * Interpret numeric string as a concatenated date or time field.
 * Return a DTK token (>= 0) if successful, a DTERR code (< 0) if not.
 *
 * Use the context of previously decoded fields to help with
 * the interpretation.
 */
static int DecodeNumberField(
    int len, char* str, unsigned int fmask, unsigned int* tmask, struct pg_tm* tm, fsec_t* fsec, bool* is2digits)
{
    char* cp = NULL;

    /*
     * Have a decimal point? Then this is a date or something with a seconds
     * field...
     */
    if ((cp = strchr(str, '.')) != NULL) {
        /*
         * Can we use ParseFractionalSecond here?  Not clear whether trailing
         * junk should be rejected ...
         */
        double frac;

        errno = 0;
        frac = strtod(cp, NULL);
        if (errno != 0)
            return DTERR_BAD_FORMAT;
#ifdef HAVE_INT64_TIMESTAMP
        *fsec = rint(frac * 1000000);
#else
        *fsec = frac;
#endif
        /* Now truncate off the fraction for further processing */
        *cp = '\0';
        len = strlen(str);
    }
    /* No decimal point and no complete date yet? */
    else if ((fmask & DTK_DATE_M) != DTK_DATE_M) {
        /* yyyymmdd? */
        if (len == 8) {
            *tmask = DTK_DATE_M;

            tm->tm_mday = atoi(str + 6);
            *(str + 6) = '\0';
            tm->tm_mon = atoi(str + 4);
            *(str + 4) = '\0';
            tm->tm_year = atoi(str + 0);

            return DTK_DATE;
        }
        /* yymmdd? */
        else if (len == 6) {
            *tmask = DTK_DATE_M;
            tm->tm_mday = atoi(str + 4);
            *(str + 4) = '\0';
            tm->tm_mon = atoi(str + 2);
            *(str + 2) = '\0';
            tm->tm_year = atoi(str + 0);
            *is2digits = TRUE;

            return DTK_DATE;
        }
    }

    /* not all time fields are specified? */
    if ((fmask & DTK_TIME_M) != DTK_TIME_M) {
        /* hhmmss */
        if (len == 6) {
            *tmask = DTK_TIME_M;
            tm->tm_sec = atoi(str + 4);
            *(str + 4) = '\0';
            tm->tm_min = atoi(str + 2);
            *(str + 2) = '\0';
            tm->tm_hour = atoi(str + 0);

            return DTK_TIME;
        }
        /* hhmm? */
        else if (len == 4) {
            *tmask = DTK_TIME_M;
            tm->tm_sec = 0;
            tm->tm_min = atoi(str + 2);
            *(str + 2) = '\0';
            tm->tm_hour = atoi(str + 0);

            return DTK_TIME;
        }
    }

    return DTERR_BAD_FORMAT;
}

/* DecodeTimezone()
 * Interpret string as a numeric timezone.
 *
 * Return 0 if okay (and set *tzp), a DTERR code if not okay.
 *
 * NB: this must *not* ereport on failure; see commands/variable.c.
 */
static int DecodeTimezone(const char* str, int* tzp)
{
    int tz;
    int hr, min, sec = 0;
    char* cp = NULL;

    /* leading character must be "+" or "-" */
    if (*str != '+' && *str != '-')
        return DTERR_BAD_FORMAT;

    errno = 0;
    hr = strtoi(str + 1, &cp, 10);
    if (errno == ERANGE)
        return DTERR_TZDISP_OVERFLOW;

    /* explicit delimiter? */
    if (*cp == ':') {
        errno = 0;
        min = strtoi(cp + 1, &cp, 10);
        if (errno == ERANGE)
            return DTERR_TZDISP_OVERFLOW;
        if (*cp == ':') {
            errno = 0;
            sec = strtoi(cp + 1, &cp, 10);
            if (errno == ERANGE)
                return DTERR_TZDISP_OVERFLOW;
        }
    }
    /* otherwise, might have run things together... */
    else if (*cp == '\0' && strlen(str) > 3) {
        min = hr % 100;
        hr = hr / 100;
        /* we could, but don't, support a run-together hhmmss format */
    } else
        min = 0;

    /* Range-check the values; see notes in datatype/timestamp.h */
    if (hr < 0 || hr > MAX_TZDISP_HOUR)
        return DTERR_TZDISP_OVERFLOW;
    if (min < 0 || min >= MINS_PER_HOUR)
        return DTERR_TZDISP_OVERFLOW;
    if (sec < 0 || sec >= SECS_PER_MINUTE)
        return DTERR_TZDISP_OVERFLOW;

    tz = (hr * MINS_PER_HOUR + min) * SECS_PER_MINUTE + sec;
    if (*str == '-')
        tz = -tz;

    *tzp = -tz;

    if (*cp != '\0')
        return DTERR_BAD_FORMAT;

    return 0;
}

/* DecodeSpecial()
 * Decode text string using lookup table.
 *
 * Implement a cache lookup since it is likely that dates
 *	will be related in format.
 *
 * NB: this must *not* ereport on failure;
 * see commands/variable.c.
 */
int DecodeSpecial(int field, const char* lowtoken, int* val)
{
    int type;
    const datetkn* tp = NULL;

    tp = u_sess->time_cxt.datecache[field];
    if (tp == NULL || strncmp(lowtoken, tp->token, TOKMAXLEN) != 0) {
        tp = datebsearch(lowtoken, u_sess->time_cxt.timezone_tktbl, u_sess->time_cxt.sz_timezone_tktbl);
        if (tp == NULL)
            tp = datebsearch(lowtoken, datetktbl, szdatetktbl);
    }
    if (tp == NULL) {
        type = UNKNOWN_FIELD;
        *val = 0;
    } else {
        u_sess->time_cxt.datecache[field] = tp;
        type = tp->type;
        switch (type) {
            case TZ:
            case DTZ:
            case DTZMOD:
                *val = FROMVAL(tp);
                break;

            default:
                *val = tp->value;
                break;
        }
    }

    return type;
}

/* ClearPgTM
 *
 * Zero out a pg_tm and associated fsec_t
 */
static inline void ClearPgTm(struct pg_tm* tm, fsec_t* fsec)
{
    tm->tm_year = 0;
    tm->tm_mon = 0;
    tm->tm_mday = 0;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    *fsec = 0;
}

/* DecodeInterval()
 * Interpret previously parsed fields for general time interval.
 * Returns 0 if successful, DTERR code if bogus input detected.
 * dtype, tm, fsec are output parameters.
 *
 * Allow "date" field DTK_DATE since this could be just
 *	an unsigned floating point number. - thomas 1997-11-16
 *
 * Allow ISO-style time span, with implicit units on number of days
 *	preceding an hh:mm:ss field. - thomas 1998-04-30
 */
int DecodeInterval(char** field, const int* ftype, int nf, int range, int* dtype, struct pg_tm* tm, fsec_t* fsec)
{
    bool is_before = FALSE;
    char* cp = NULL;
    unsigned int fmask = 0, tmask;
    int type;
    int i;
    int dterr;
    int val;
    double fval;

    *dtype = DTK_DELTA;
    type = IGNORE_DTF;
    ClearPgTm(tm, fsec);

    /* read through list backwards to pick up units before values */
    for (i = nf - 1; i >= 0; i--) {
        switch (ftype[i]) {
            case DTK_TIME:
                dterr = DecodeTime(field[i], fmask, range, &tmask, tm, fsec);
                if (dterr)
                    return dterr;
                type = DTK_DAY;
                break;

            case DTK_TZ:

                /*
                 * Timezone means a token with a leading sign character and at
                 * least one digit; there could be ':', '.', '-' embedded in
                 * it as well.
                 */
                Assert(*field[i] == '-' || *field[i] == '+');

                /*
                 * Check for signed hh:mm or hh:mm:ss.  If so, process exactly
                 * like DTK_TIME case above, plus handling the sign.
                 */
                if (strchr(field[i] + 1, ':') != NULL &&
                    DecodeTime(field[i] + 1, fmask, range, &tmask, tm, fsec) == 0) {
                    if (*field[i] == '-') {
                        /* flip the sign on all fields */
                        tm->tm_hour = -tm->tm_hour;
                        tm->tm_min = -tm->tm_min;
                        tm->tm_sec = -tm->tm_sec;
                        *fsec = -(*fsec);
                    }

                    /*
                     * Set the next type to be a day, if units are not
                     * specified. This handles the case of '1 +02:03' since we
                     * are reading right to left.
                     */
                    type = DTK_DAY;
                    break;
                }

                /*
                 * Otherwise, fall through to DTK_NUMBER case, which can
                 * handle signed float numbers and signed year-month values.
                 */

                /* fall through */
            case DTK_DATE:
            case DTK_NUMBER:
                if (type == IGNORE_DTF) {
                    /* use typmod to decide what rightmost field is */
                    switch (range) {
                        case INTERVAL_MASK(YEAR):
                            type = DTK_YEAR;
                            break;
                        case INTERVAL_MASK(MONTH):
                        case INTERVAL_MASK(YEAR) | INTERVAL_MASK(MONTH):
                            type = DTK_MONTH;
                            break;
                        case INTERVAL_MASK(DAY):
                            type = DTK_DAY;
                            break;
                        case INTERVAL_MASK(HOUR):
                        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR):
                            type = DTK_HOUR;
                            break;
                        case INTERVAL_MASK(MINUTE):
                        case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
                        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE):
                            type = DTK_MINUTE;
                            break;
                        case INTERVAL_MASK(SECOND):
                        case INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
                        case INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
                        case INTERVAL_MASK(DAY) | INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE) | INTERVAL_MASK(SECOND):
                            type = DTK_SECOND;
                            break;
                        default:
                            type = DTK_DAY;
                            break;
                    }
                }

                errno = 0;
                val = strtoi(field[i], &cp, 10);
                if (errno == ERANGE)
                    return DTERR_FIELD_OVERFLOW;

                if (*cp == '-') {
                    /* SQL "years-months" syntax */
                    int val2;

                    val2 = strtoi(cp + 1, &cp, 10);
                    if (errno == ERANGE || val2 < 0 || val2 >= MONTHS_PER_YEAR)
                        return DTERR_FIELD_OVERFLOW;
                    if (*cp != '\0')
                        return DTERR_BAD_FORMAT;
                    type = DTK_MONTH;
                    if (*field[i] == '-')
                        val2 = -val2;
                    val = val * MONTHS_PER_YEAR + val2;
                    fval = 0;
                } else if (*cp == '.') {
                    errno = 0;
                    fval = strtod(cp, &cp);
                    if (*cp != '\0' || errno != 0)
                        return DTERR_BAD_FORMAT;

                    if (*field[i] == '-')
                        fval = -fval;
                } else if (*cp == '\0')
                    fval = 0;
                else
                    return DTERR_BAD_FORMAT;

                tmask = 0; /* DTK_M(type); */

                switch (type) {
                    case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
                        *fsec += rint(val + fval);
#else
                        *fsec += (val + fval) * 1e-6;
#endif
                        tmask = DTK_M(MICROSECOND);
                        break;

                    case DTK_MILLISEC:
                        /* avoid overflowing the fsec field */
                        tm->tm_sec += val / 1000;
                        val -= (val / 1000) * 1000;
#ifdef HAVE_INT64_TIMESTAMP
                        *fsec += rint((val + fval) * 1000);
#else
                        *fsec += (val + fval) * 1e-3;
#endif
                        tmask = DTK_M(MILLISECOND);
                        break;

                    case DTK_SECOND:
                        tm->tm_sec += val;
#ifdef HAVE_INT64_TIMESTAMP
                        *fsec += rint(fval * 1000000);
#else
                        *fsec += fval;
#endif

                        /*
                         * If any subseconds were specified, consider this
                         * microsecond and millisecond input as well.
                         */
                        if (fval == 0)
                            tmask = DTK_M(SECOND);
                        else
                            tmask = DTK_ALL_SECS_M;
                        break;

                    case DTK_MINUTE:
                        tm->tm_min += val;
                        AdjustFractSeconds(fval, tm, fsec, SECS_PER_MINUTE);
                        tmask = DTK_M(MINUTE);
                        break;

                    case DTK_HOUR:
                        tm->tm_hour += val;
                        AdjustFractSeconds(fval, tm, fsec, SECS_PER_HOUR);
                        tmask = DTK_M(HOUR);
                        type = DTK_DAY; /* set for next field */
                        break;

                    case DTK_DAY:
                        tm->tm_mday += val;
                        AdjustFractSeconds(fval, tm, fsec, SECS_PER_DAY);
                        tmask = DTK_M(DAY);
                        break;

                    case DTK_WEEK:
                        tm->tm_mday += val * 7;
                        AdjustFractDays(fval, tm, fsec, 7);
                        tmask = DTK_M(WEEK);
                        break;

                    case DTK_MONTH:
                        tm->tm_mon += val;
                        AdjustFractDays(fval, tm, fsec, DAYS_PER_MONTH);
                        tmask = DTK_M(MONTH);
                        break;

                    case DTK_YEAR:
                        tm->tm_year += val;
                        if (fval != 0)
                            tm->tm_mon += (int)(fval * MONTHS_PER_YEAR);
                        tmask = DTK_M(YEAR);
                        break;

                    case DTK_DECADE:
                        tm->tm_year += val * 10;
                        if (fval != 0)
                            tm->tm_mon += (int)(fval * MONTHS_PER_YEAR * 10);
                        tmask = DTK_M(DECADE);
                        break;

                    case DTK_CENTURY:
                        tm->tm_year += val * 100;
                        if (fval != 0)
                            tm->tm_mon += (int)(fval * MONTHS_PER_YEAR * 100);
                        tmask = DTK_M(CENTURY);
                        break;

                    case DTK_MILLENNIUM:
                        tm->tm_year += val * 1000;
                        if (fval != 0)
                            tm->tm_mon += (int)(fval * MONTHS_PER_YEAR * 1000);
                        tmask = DTK_M(MILLENNIUM);
                        break;

                    default:
                        return DTERR_BAD_FORMAT;
                }
                break;

            case DTK_STRING:
            case DTK_SPECIAL:
                type = DecodeUnits(i, field[i], &val);
                if (type == IGNORE_DTF)
                    continue;

                tmask = 0; /* DTK_M(type); */
                switch (type) {
                    case UNITS:
                        type = val;
                        break;

                    case AGO:
                        is_before = TRUE;
                        type = val;
                        break;

                    case RESERV:
                        tmask = (DTK_DATE_M | DTK_TIME_M);
                        *dtype = val;
                        break;

                    default:
                        return DTERR_BAD_FORMAT;
                }
                break;

            default:
                return DTERR_BAD_FORMAT;
        }

        if (tmask & fmask)
            return DTERR_BAD_FORMAT;
        fmask |= tmask;
    }

    /* ensure that at least one time field has been found */
    if (fmask == 0)
        return DTERR_BAD_FORMAT;

    /* ensure fractional seconds are fractional */
    if (*fsec != 0) {
        int sec;

#ifdef HAVE_INT64_TIMESTAMP
        sec = *fsec / USECS_PER_SEC;
        *fsec -= sec * USECS_PER_SEC;
#else
        TMODULO(*fsec, sec, 1.0);
#endif
        tm->tm_sec += sec;
    }

    /* ----------
     * The SQL standard defines the interval literal
     *	 '-1 1:00:00'
     * to mean "negative 1 days and negative 1 hours", while openGauss
     * traditionally treats this as meaning "negative 1 days and positive
     * 1 hours".  In SQL_STANDARD intervalstyle, we apply the leading sign
     * to all fields if there are no other explicit signs.
     *
     * We leave the signs alone if there are additional explicit signs.
     * This protects us against misinterpreting openGauss-style dump output,
     * since the postgres-style output code has always put an explicit sign on
     * all fields following a negative field.  But note that SQL-spec output
     * is ambiguous and can be misinterpreted on load!	(So it's best practice
     * to dump in openGauss style, not SQL style.)

     * Because A db used standard SQL interval format. In order to simulate A db,
     * there deleted the judge of IntervalStyle. So it will always use SQL_STANDARD.
     * ----------
     */
    /* use standard sql interval */
    if (*field[0] == '-') {
        /* Check for additional explicit signs */
        bool more_signs = false;

        for (i = 1; i < nf; i++) {
            if (*field[i] == '-' || *field[i] == '+') {
                more_signs = true;
                break;
            }
        }

        if (!more_signs) {
            /*
             * Rather than re-determining which field was field[0], just force
             * 'em all negative.
             */
            if (*fsec > 0)
                *fsec = -(*fsec);
            if (tm->tm_sec > 0)
                tm->tm_sec = -tm->tm_sec;
            if (tm->tm_min > 0)
                tm->tm_min = -tm->tm_min;
            if (tm->tm_hour > 0)
                tm->tm_hour = -tm->tm_hour;
            if (tm->tm_mday > 0)
                tm->tm_mday = -tm->tm_mday;
            if (tm->tm_mon > 0)
                tm->tm_mon = -tm->tm_mon;
            if (tm->tm_year > 0)
                tm->tm_year = -tm->tm_year;
        }
    }

    /* finally, AGO negates everything */
    if (is_before) {
        *fsec = -(*fsec);
        tm->tm_sec = -tm->tm_sec;
        tm->tm_min = -tm->tm_min;
        tm->tm_hour = -tm->tm_hour;
        tm->tm_mday = -tm->tm_mday;
        tm->tm_mon = -tm->tm_mon;
        tm->tm_year = -tm->tm_year;
    }

    return 0;
}

/*
 * Helper functions to avoid duplicated code in DecodeISO8601Interval.
 *
 * Parse a decimal value and break it into integer and fractional parts.
 * Returns 0 or DTERR code.
 */
static int ParseISO8601Number(const char* str, char** endptr, int* ipart, double* fpart)
{
    double val;

    if (!(isdigit((unsigned char)*str) || *str == '-' || *str == '.'))
        return DTERR_BAD_FORMAT;
    errno = 0;
    val = strtod(str, endptr);
    /* did we not see anything that looks like a double? */
    if (*endptr == str || errno != 0)
        return DTERR_BAD_FORMAT;
    /* watch out for overflow */
    if (val < INT_MIN || val > INT_MAX)
        return DTERR_FIELD_OVERFLOW;
    /* be very sure we truncate towards zero (cf dtrunc()) */
    if (val >= 0) {
        *ipart = (int)floor(val);
    } else {
        *ipart = (int)-floor(-val);
    }
    *fpart = val - *ipart;
    return 0;
}

/*
 * Determine number of integral digits in a valid ISO 8601 number field
 * (we should ignore sign and any fraction part)
 */
static int ISO8601IntegerWidth(const char* fieldstart)
{
    /* We might have had a leading '-' */
    if (*fieldstart == '-')
        fieldstart++;
    return strspn(fieldstart, "0123456789");
}

/* DecodeISO8601Interval()
 *	Decode an ISO 8601 time interval of the "format with designators"
 *	(section 4.4.3.2) or "alternative format" (section 4.4.3.3)
 *	Examples:  P1D	for 1 day
 *			   PT1H for 1 hour
 *			   P2Y6M7DT1H30M for 2 years, 6 months, 7 days 1 hour 30 min
 *			   P0002-06-07T01:30:00 the same value in alternative format
 *
 * Returns 0 if successful, DTERR code if bogus input detected.
 * Note: error code should be DTERR_BAD_FORMAT if input doesn't look like
 * ISO8601, otherwise this could cause unexpected error messages.
 * dtype, tm, fsec are output parameters.
 *
 *	A couple exceptions from the spec:
 *	 - a week field ('W') may coexist with other units
 *	 - allows decimals in fields other than the least significant unit.
 */
int DecodeISO8601Interval(char* str, int* dtype, struct pg_tm* tm, fsec_t* fsec)
{
    bool datepart = true;
    bool havefield = false;

    *dtype = DTK_DELTA;
    ClearPgTm(tm, fsec);

    if (strlen(str) < 2 || str[0] != 'P')
        return DTERR_BAD_FORMAT;

    str++;
    while (*str) {
        char* fieldstart = NULL;
        int val;
        double fval;
        char unit;
        int dterr;

        if (*str == 'T') /* T indicates the beginning of the time part */
        {
            datepart = false;
            havefield = false;
            str++;
            continue;
        }

        fieldstart = str;
        dterr = ParseISO8601Number(str, &str, &val, &fval);
        if (dterr)
            return dterr;

        /*
         * Note: we could step off the end of the string here.	Code below
         * *must* exit the loop if unit == '\0'.
         */
        unit = *str++;

        if (datepart) {
            switch (unit) /* before T: Y M W D */
            {
                case 'Y':
                    tm->tm_year += val;
                    tm->tm_mon += (int)(fval * MONTHS_PER_YEAR);
                    break;
                case 'M':
                    tm->tm_mon += val;
                    AdjustFractDays(fval, tm, fsec, DAYS_PER_MONTH);
                    break;
                case 'W':
                    tm->tm_mday += val * 7;
                    AdjustFractDays(fval, tm, fsec, 7);
                    break;
                case 'D':
                    tm->tm_mday += val;
                    AdjustFractSeconds(fval, tm, fsec, SECS_PER_DAY);
                    break;
                case 'T': /* ISO 8601 4.4.3.3 Alternative Format / Basic */
                case '\0':
                    if (ISO8601IntegerWidth(fieldstart) == 8 && !havefield) {
                        tm->tm_year += val / 10000;
                        tm->tm_mon += (val / 100) % 100;
                        tm->tm_mday += val % 100;
                        AdjustFractSeconds(fval, tm, fsec, SECS_PER_DAY);
                        if (unit == '\0')
                            return 0;
                        datepart = false;
                        havefield = false;
                        continue;
                    }
                    /* Else fall through to extended alternative format */
                    /* fall through */
                case '-': /* ISO 8601 4.4.3.3 Alternative Format,
                           * Extended */
                    if (havefield)
                        return DTERR_BAD_FORMAT;

                    tm->tm_year += val;
                    tm->tm_mon += (int)(fval * MONTHS_PER_YEAR);
                    if (unit == '\0')
                        return 0;
                    if (unit == 'T') {
                        datepart = false;
                        havefield = false;
                        continue;
                    }

                    dterr = ParseISO8601Number(str, &str, &val, &fval);
                    if (dterr)
                        return dterr;
                    tm->tm_mon += val;
                    AdjustFractDays(fval, tm, fsec, DAYS_PER_MONTH);
                    if (*str == '\0')
                        return 0;
                    if (*str == 'T') {
                        datepart = false;
                        havefield = false;
                        continue;
                    }
                    if (*str != '-')
                        return DTERR_BAD_FORMAT;
                    str++;

                    dterr = ParseISO8601Number(str, &str, &val, &fval);
                    if (dterr)
                        return dterr;
                    tm->tm_mday += val;
                    AdjustFractSeconds(fval, tm, fsec, SECS_PER_DAY);
                    if (*str == '\0')
                        return 0;
                    if (*str == 'T') {
                        datepart = false;
                        havefield = false;
                        continue;
                    }
                    return DTERR_BAD_FORMAT;
                default:
                    /* not a valid date unit suffix */
                    return DTERR_BAD_FORMAT;
            }
        } else {
            switch (unit) /* after T: H M S */
            {
                case 'H':
                    tm->tm_hour += val;
                    AdjustFractSeconds(fval, tm, fsec, SECS_PER_HOUR);
                    break;
                case 'M':
                    tm->tm_min += val;
                    AdjustFractSeconds(fval, tm, fsec, SECS_PER_MINUTE);
                    break;
                case 'S':
                    tm->tm_sec += val;
                    AdjustFractSeconds(fval, tm, fsec, 1);
                    break;
                case '\0': /* ISO 8601 4.4.3.3 Alternative Format */
                    if (ISO8601IntegerWidth(fieldstart) == 6 && !havefield) {
                        tm->tm_hour += val / 10000;
                        tm->tm_min += (val / 100) % 100;
                        tm->tm_sec += val % 100;
                        AdjustFractSeconds(fval, tm, fsec, 1);
                        return 0;
                    }
                    /* Else fall through to extended alternative format */
                    /* fall through */
                case ':': /* ISO 8601 4.4.3.3 Alternative Format,
                           * Extended */
                    if (havefield)
                        return DTERR_BAD_FORMAT;

                    tm->tm_hour += val;
                    AdjustFractSeconds(fval, tm, fsec, SECS_PER_HOUR);
                    if (unit == '\0')
                        return 0;

                    dterr = ParseISO8601Number(str, &str, &val, &fval);
                    if (dterr)
                        return dterr;
                    tm->tm_min += val;
                    AdjustFractSeconds(fval, tm, fsec, SECS_PER_MINUTE);
                    if (*str == '\0')
                        return 0;
                    if (*str != ':')
                        return DTERR_BAD_FORMAT;
                    str++;

                    dterr = ParseISO8601Number(str, &str, &val, &fval);
                    if (dterr)
                        return dterr;
                    tm->tm_sec += val;
                    AdjustFractSeconds(fval, tm, fsec, 1);
                    if (*str == '\0')
                        return 0;
                    return DTERR_BAD_FORMAT;

                default:
                    /* not a valid time unit suffix */
                    return DTERR_BAD_FORMAT;
            }
        }

        havefield = true;
    }

    return 0;
}

/* DecodeUnits()
 * Decode text string using lookup table.
 * This routine supports time interval decoding
 * (hence, it need not recognize timezone names).
 */
int DecodeUnits(int field, const char* lowtoken, int* val)
{
    int type;
    const datetkn* tp = NULL;

    tp = u_sess->time_cxt.deltacache[field];
    if (tp == NULL || strncmp(lowtoken, tp->token, TOKMAXLEN) != 0) {
        tp = datebsearch(lowtoken, deltatktbl, szdeltatktbl);
    }
    if (tp == NULL) {
        type = UNKNOWN_FIELD;
        *val = 0;
    } else {
        u_sess->time_cxt.deltacache[field] = tp;
        type = tp->type;
        if (type == TZ || type == DTZ)
            *val = FROMVAL(tp);
        else
            *val = tp->value;
    }

    return type;
} /* DecodeUnits() */

/*
 * Report an error detected by one of the datetime input processing routines.
 *
 * dterr is the error code, str is the original input string, datatype is
 * the name of the datatype we were trying to accept.
 *
 * Note: it might seem useless to distinguish DTERR_INTERVAL_OVERFLOW and
 * DTERR_TZDISP_OVERFLOW from DTERR_FIELD_OVERFLOW, but SQL99 mandates three
 * separate SQLSTATE codes, so ...
 */
void DateTimeParseError(int dterr, const char* str, const char* datatype, bool can_ignore)
{
    int level = can_ignore || !SQL_MODE_STRICT() ? WARNING : ERROR;
    switch (dterr) {
        case DTERR_FIELD_OVERFLOW:
            ereport(level,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), errmsg("date/time field value out of range: \"%s\"", str)));
            break;
        case DTERR_MD_FIELD_OVERFLOW:
            /* <nanny>same as above, but add hint about u_sess->time_cxt.DateStyle</nanny> */
            ereport(level,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW),
                    errmsg("date/time field value out of range: \"%s\"", str),
                    errhint("Perhaps you need a different \"datestyle\" setting.")));
            break;
        case DTERR_INTERVAL_OVERFLOW:
            ereport(level,
                (errcode(ERRCODE_INTERVAL_FIELD_OVERFLOW), errmsg("interval field value out of range: \"%s\"", str)));
            break;
        case DTERR_TZDISP_OVERFLOW:
            ereport(level,
                (errcode(ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE),
                    errmsg("time zone displacement out of range: \"%s\"", str)));
            break;
        case DTERR_ZERO_DATE:
            ereport(WARNING,
                (errcode(ERRCODE_DATETIME_FIELD_OVERFLOW), errmsg("date/time field value out of range: \"%s\"", str)));
            break;
        case DTERR_BAD_FORMAT:
        default:
            ereport(level,
                (errcode(ERRCODE_INVALID_DATETIME_FORMAT),
                    errmsg("invalid input syntax for type %s: \"%s\"", datatype, str)));
            break;
    }
}

/* datebsearch()
 * Binary search -- from Knuth (6.2.1) Algorithm B.  Special case like this
 * is WAY faster than the generic bsearch().
 */
static const datetkn* datebsearch(const char* key, const datetkn* base, int nel)
{
    if (nel > 0) {
        const datetkn *last = base + nel - 1, *position = NULL;
        int result;

        while (last >= base) {
            position = base + ((last - base) >> 1);
            result = key[0] - position->token[0];
            if (result == 0) {
                result = strncmp(key, position->token, TOKMAXLEN);
                if (result == 0)
                    return position;
            }
            if (result < 0)
                last = position - 1;
            else
                base = position + 1;
        }
    }
    return NULL;
}

/* EncodeTimezone()
 *		Copies representation of a numeric timezone offset to str.
 *
 * Returns a pointer to the new end of string.  No NUL terminator is put
 * there; callers are responsible for NUL terminating str themselves.
 */
static char* EncodeTimezone(char* str, int tz, int style)
{
    int hour, min, sec;
    sec = abs(tz);
    min = sec / SECS_PER_MINUTE;
    sec -= min * SECS_PER_MINUTE;
    hour = min / MINS_PER_HOUR;
    min -= hour * MINS_PER_HOUR;

    /* TZ is negated compared to sign we wish to display ... */
    *str++ = ((tz <= 0) ? '+' : '-');

    if (sec != 0)
    {
        str = pg_ultostr_zeropad(str, hour, 2);
        *str++ = ':';
        str = pg_ultostr_zeropad(str, min, 2);
        *str++ = ':';
        str = pg_ultostr_zeropad(str, sec, 2);
    }
    else if (min != 0 || style == USE_XSD_DATES)
    {
        str = pg_ultostr_zeropad(str, hour, 2);
        *str++ = ':';
        str = pg_ultostr_zeropad(str, min, 2);
    }
    else
        str = pg_ultostr_zeropad(str, hour, 2);
    return str;
}

/* EncodeDateOnly()
 * Encode date as local time.
 */
#ifdef DOLPHIN
void EncodeDateOnlyForBDatabase(struct pg_tm* tm, int style, char* str, unsigned int date_flag)
#else
void EncodeDateOnlyForBDatabase(struct pg_tm* tm, int style, char* str)
#endif
{
    errno_t rc;
#ifdef DOLPHIN
    Assert(tm->tm_mon >= 0 && tm->tm_mon <= MONTHS_PER_YEAR);
#else
    Assert(tm->tm_mon >= 1 && tm->tm_mon <= MONTHS_PER_YEAR);
#endif

    switch (style) {
        case USE_ISO_DATES:
        case USE_XSD_DATES:
            /* compatible with ISO date formats */
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            *str++ = '-';
            str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            *str++ = '-';
            str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            break;

        case USE_SQL_DATES:
            /* compatible with A db/Ingres date formats */
            if (u_sess->time_cxt.DateOrder == DATEORDER_DMY) {
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
                *str++ = '/';
                str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            }
            else {
                str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
                *str++ = '/';
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            }
            *str++ = '/';
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            break;

        case USE_GERMAN_DATES:
            /* German-style date format */
            str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            *str++ = '.';
            str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            *str++ = '.';
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            break;

        case USE_POSTGRES_DATES:
        default:
            /* traditional date-only style for openGauss */
            if (u_sess->time_cxt.DateOrder == DATEORDER_DMY) {
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
                *str++ = '-';
                str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            }
            else {
                str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
                *str++ = '-';
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            }
            *str++ = '-';
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            break;
    }
#ifdef DOLPHIN
    if (tm->tm_year < 0)
#else
    if (tm->tm_year <= 0)
#endif
    {
        errno_t rc = memcpy_s(str, 3, " BC", 3);  /* Don't copy NUL */
        securec_check(rc, "", "");
        str += 3;
    }
    *str = '\0';
}

/* EncodeTimeOnly()
 * Encode time fields only.
 *
 * tm and fsec are the value to encode, print_tz determines whether to include
 * a time zone (the difference between time and timetz types), tz is the
 * numeric time zone offset, style is the date style, str is where to write the
 * output.
 */
void EncodeTimeOnly(struct pg_tm* tm, fsec_t fsec, bool print_tz, int tz, int style, char* str)
{
    str = pg_ultostr_zeropad(str, tm->tm_hour, 2);
    *str++ = ':';
    str = pg_ultostr_zeropad(str, tm->tm_min, 2);
    *str++ = ':';
    str = AppendSeconds(str, tm->tm_sec, fsec, MAX_TIME_PRECISION, true);
    if (print_tz)
        str = EncodeTimezone(str, tz, style);
    *str = '\0';
}

/* EncodeDateTime()
 * Encode date and time interpreted as local time.
 *
 * tm and fsec are the value to encode, print_tz determines whether to include
 * a time zone (the difference between timestamp and timestamptz types), tz is
 * the numeric time zone offset, tzn is the textual time zone, which if
 * specified will be used instead of tz by some styles, style is the date
 * style, str is where to write the output.
 *
 * Supported date styles:
 *	openGauss - day mon hh:mm:ss yyyy tz
 *	SQL - mm/dd/yyyy hh:mm:ss.ss tz
 *	ISO - yyyy-mm-dd hh:mm:ss+/-tz
 *	German - dd.mm.yyyy hh:mm:ss tz
 *	XSD - yyyy-mm-ddThh:mm:ss.ss+/-tz
 */
void EncodeDateTimeForBDatabase(struct pg_tm* tm, fsec_t fsec, bool print_tz, int tz, const char* tzn, int style, char* str)
{
    int day;
    errno_t rc = EOK;
#ifdef DOLPHIN
    Assert(tm->tm_mon >= 0 && tm->tm_mon <= MONTHS_PER_YEAR);
#else
    Assert(tm->tm_mon >= 1 && tm->tm_mon <= MONTHS_PER_YEAR);
#endif

#ifdef DOLPHIN   
    if (style == USE_POSTGRES_DATES && tm->tm_year == 0  && tm->tm_mon == 0 && tm->tm_mday == 0) {
        style = USE_ISO_DATES;
    }
#endif

    /*
     * Negative tm_isdst means we have no valid time zone translation.
     */
    if (tm->tm_isdst < 0)
        print_tz = false;

    switch (style) {
        case USE_ISO_DATES:
        case USE_XSD_DATES:
            /* Compatible with ISO-8601 date formats */
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            *str++ = '-';
            str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            *str++ = '-';
            str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            *str++ = (style == USE_ISO_DATES) ? ' ' : 'T';
            str = pg_ultostr_zeropad(str, tm->tm_hour, 2);
            *str++ = ':';
            str = pg_ultostr_zeropad(str, tm->tm_min, 2);
            *str++ = ':';
            str = AppendTimestampSeconds(str, tm, fsec);
            if (print_tz)
                str = EncodeTimezone(str, tz, style);
            break;

        case USE_SQL_DATES:
            /* Compatible with A db/Ingres date formats */
            if (u_sess->time_cxt.DateOrder == DATEORDER_DMY) {
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
                *str++ = '/';
                str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            } else {
                str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
                *str++ = '/';
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            }
            *str++ = '/';
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            *str++ = ' ';
            str = pg_ultostr_zeropad(str, tm->tm_hour, 2);
            *str++ = ':';
            str = pg_ultostr_zeropad(str, tm->tm_min, 2);
            *str++ = ':';
            str = AppendTimestampSeconds(str, tm, fsec);

            /*
             * Note: the uses of %.*s in this function would be risky if the
             * timezone names ever contain non-ASCII characters.  However, all
             * TZ abbreviations in the IANA database are plain ASCII.
             */
            if (print_tz) {
                if (NULL != tzn) {
                    rc = sprintf_s(str, MAXDATELEN + 1, " %.*s", MAXTZLEN, tzn);
                    securec_check_ss(rc, "\0", "\0");
                    str += strlen(str);
                } else
                    str = EncodeTimezone(str, tz, style);
            }
            break;

        case USE_GERMAN_DATES:
            /* German variant on European style */
            str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            *str++ = '.';
            str = pg_ultostr_zeropad(str, tm->tm_mon, 2);
            *str++ = '.';
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif
            *str++ = ' ';
            str = pg_ultostr_zeropad(str, tm->tm_hour, 2);
            *str++ = ':';
            str = pg_ultostr_zeropad(str, tm->tm_min, 2);
            *str++ = ':';
            str = AppendTimestampSeconds(str, tm, fsec);

            if (print_tz) {
                if (NULL != tzn) {
                    rc = sprintf_s(str, MAXDATELEN + 1, " %.*s", MAXTZLEN, tzn);
                    securec_check_ss(rc, "\0", "\0");
                    str += strlen(str);
                } else
                    str = EncodeTimezone(str, tz, style);
            }
            break;

        case USE_POSTGRES_DATES:
        default:
            /* Backward-compatible with traditional openGauss abstime dates */
            day = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday);
            tm->tm_wday = j2day(day);
            rc = memcpy_s(str, 3, days[tm->tm_wday], 3);
            securec_check(rc, "", "");
            str += 3;
            *str++ = ' ';
            if (u_sess->time_cxt.DateOrder == DATEORDER_DMY) {
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
                *str++ = ' ';
                rc = memcpy_s(str, 3, months[tm->tm_mon - 1], 3);
                securec_check(rc, "", "");
                str += 3;
            } else {
                rc = memcpy_s(str, 3, months[tm->tm_mon - 1], 3);
                securec_check(rc, "", "");
                str += 3;
                *str++ = ' ';
                str = pg_ultostr_zeropad(str, tm->tm_mday, 2);
            }
            *str++ = ' ';
            str = pg_ultostr_zeropad(str, tm->tm_hour, 2);
            *str++ = ':';
            str = pg_ultostr_zeropad(str, tm->tm_min, 2);
            *str++ = ':';
            str = AppendTimestampSeconds(str, tm, fsec);
            *str++ = ' ';
            str = pg_ultostr_zeropad(str,
#ifdef DOLPHIN
                    (tm->tm_year >= 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#else
                    (tm->tm_year > 0) ? tm->tm_year : -(tm->tm_year - 1), 4);
#endif

            if (print_tz) {
                if (NULL != tzn) {
                    rc = sprintf_s(str, MAXDATELEN + 1, " %.*s", MAXTZLEN, tzn);
                    securec_check_ss(rc, "\0", "\0");
                    str += strlen(str);
                } else {
                    /*
                     * We have a time zone, but no string version. Use the
                     * numeric form, but be sure to include a leading space to
                     * avoid formatting something which would be rejected by
                     * the date/time parser later. - thomas 2001-10-19
                     */
                    *str++ = ' ';
                    str = EncodeTimezone(str, tz, style);
                }
            }
            break;
    }
#ifdef DOLPHIN
    if (tm->tm_year < 0)
#else
    if (tm->tm_year <= 0)
#endif
    {
        rc = memcpy_s(str, 3, " BC", 3);  /* Don't copy NUL */
        securec_check(rc, "", "");
        str += 3;
    }
    *str = '\0';
}

/*
 * Helper functions to avoid duplicated code in EncodeInterval.
 */

/* Append an ISO-8601-style interval field, but only if value isn't zero */
static char* AddISO8601IntPart(char* cp, int value, char units)
{
    if (value == 0)
        return cp;
    errno_t rc = sprintf_s(cp, MAXDATELEN + 1, "%d%c", value, units);
    securec_check_ss(rc, "\0", "\0");
    return cp + strlen(cp);
}

/* Append a openGauss interval field, but only if value isn't zero */
static char* AddPostgresIntPart(char* cp, int value, const char* units, bool* is_zero, bool* is_before)
{
    if (value == 0)
        return cp;
    errno_t rc = sprintf_s(cp,
        MAXDATELEN + 1,
        "%s%s%d %s%s",
        (!*is_zero) ? " " : "",
        (*is_before && value > 0) ? "+" : "",
        value,
        units,
        (value != 1) ? "s" : "");
    securec_check_ss(rc, "\0", "\0");
    /*
     * Each nonzero field sets is_before for (only) the next one.  This is a
     * tad bizarre but it's how it worked before...
     */
    *is_before = (value < 0);
    *is_zero = FALSE;
    return cp + strlen(cp);
}

/* Append a verbose-style interval field, but only if value isn't zero */
static char* AddVerboseIntPart(char* cp, int value, const char* units, bool* is_zero, bool* is_before)
{
    if (value == 0)
        return cp;
    /* first nonzero value sets is_before */
    if (*is_zero) {
        *is_before = (value < 0);
        value = abs(value);
    } else if (*is_before)
        value = -value;
    errno_t rc = sprintf_s(cp, MAXDATELEN + 1, " %d %s%s", value, units, (value == 1) ? "" : "s");
    securec_check_ss(rc, "\0", "\0");
    *is_zero = FALSE;
    return cp + strlen(cp);
}

/* AppendTrailingZeros()
 * To compatible A db, numdstointerval() reserve 9 digits after seconds.
 */
static void AppendTrailingZeros(char* str)
{
    char* tmp = NULL;
    int len;
    const int expected_len = 10;

    tmp = strchr(str, '.');

    if (tmp == NULL)
        return;

    len = strlen(tmp);

    if (len > expected_len) {
        *(tmp + expected_len) = '\0';
    } else {
        while (len < 10) {
            *(tmp + len) = '0';
            len++;
        }

        *(tmp + 10) = '\0';
    }
}

/* EncodeInterval()
 * Interpret time structure as a delta time and convert to string.
 *
 * Support "traditional Postgres" and ISO-8601 styles.
 * Actually, afaik ISO does not address time interval formatting,
 *	but this looks similar to the spec for absolute date/time.
 * - thomas 1998-04-30
 *
 * Actually, afaik, ISO 8601 does specify formats for "time
 * intervals...[of the]...format with time-unit designators", which
 * are pretty ugly.  The format looks something like
 *	   P1Y1M1DT1H1M1.12345S
 * but useful for exchanging data with computers instead of humans.
 * - ron 2003-07-14
 *
 * And ISO's SQL 2008 standard specifies standards for
 * "year-month literal"s (that look like '2-3') and
 * "day-time literal"s (that look like ('4 5:6:7')
 */
void EncodeInterval(struct pg_tm* tm, fsec_t fsec, int style, char* str)
{
    char* cp = str;
    int year = tm->tm_year;
    int mon = tm->tm_mon;
    int mday = tm->tm_mday;
    int hour = tm->tm_hour;
    int min = tm->tm_min;
    int sec = tm->tm_sec;
    errno_t rc = EOK;
    bool is_before = FALSE;
    bool is_zero = TRUE;
    int curlen = MAXDATELEN + 1;

    /*
     * The sign of year and month are guaranteed to match, since they are
     * stored internally as "month". But we'll need to check for is_before and
     * is_zero when determining the signs of day and hour/minute/seconds
     * fields.
     */
    switch (style) {
        case INTSTYLE_A: {
            bool has_negative = year < 0 || mon < 0 || mday < 0 || hour < 0 || min < 0 || sec < 0 || fsec < 0;
            bool has_positive = year > 0 || mon > 0 || mday > 0 || hour > 0 || min > 0 || sec > 0 || fsec > 0;
            bool has_year_month = year != 0 || mon != 0;
            bool has_day_time = mday != 0 || hour != 0 || min != 0 || sec != 0 || fsec != 0;
            bool sql_standard_value = !(has_negative && has_positive) && !(has_year_month && has_day_time);
            bool has_day_sec = !has_year_month && has_day_time;
            errno_t rc = EOK;

            if (year == 0 && mon == 0 && mday == 0 && hour == 0 && min == 0 && sec == 0 && fsec == 0) {
                has_day_sec = true;
            }
            /*
             * SQL Standard wants only 1 "<sign>" preceding the whole
             * interval ... but can't do that if mixed signs.
             */
            if (has_negative && sql_standard_value) {
                *cp++ = '-';
                curlen--;
            }
            /*
             * SQL Standard value has day time and don't has year month
             * And the value is postive, add + to convert A db format
             */
            if (has_positive && sql_standard_value && has_day_sec) {
                *cp++ = '+';
                curlen--;
            }

            if (!has_negative && !has_positive) {
                rc = snprintf_s(cp, curlen, curlen - 1, "+000000000 00:00:00.000000000");
                securec_check_ss(rc, "\0", "\0");
            } else if (!sql_standard_value) {
                /*
                 * For non sql-standard interval values, force outputting
                 * the signs to avoid ambiguities with intervals with
                 * mixed sign components.
                 */
                char year_sign = (year < 0 || mon < 0) ? '-' : '+';
                char day_sign = (mday < 0) ? '-' : '+';
                char sec_sign = (hour < 0 || min < 0 || sec < 0 || fsec < 0) ? '-' : '+';

                rc = snprintf_s(cp,
                    curlen,
                    curlen - 1,
                    "%c%d-%d %c%d %c%d:%02d:",
                    year_sign,
                    abs(year),
                    abs(mon),
                    day_sign,
                    abs(mday),
                    sec_sign,
                    abs(hour),
                    abs(min));
                securec_check_ss(rc, "\0", "\0");
                cp += strlen(cp);
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, true);
                *cp = '\0';
            }
            /* the format for has_year_month */
            else if (has_year_month) {
                rc = snprintf_s(cp, curlen, curlen - 1, "%d-%d", abs(year), abs(mon));
                securec_check_ss(rc, "\0", "\0");
            }
            /* convert hour to day and output in A db format */
            else {
                int len = 0;
                if (pg_add_s32_overflow(mday, hour / 24, &mday)) {
                    ereport(ERROR,
                        (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                            errmsg("the interval value is overflow, it can not perform as A db interval-style")));
                }
                hour = hour % 24;
                rc = snprintf_s(cp, curlen, curlen - 1, "%09d %02d:%02d:", abs(mday), abs(hour), abs(min));
                securec_check_ss(rc, "\0", "\0");
                len = strlen(cp);
                cp += len;
                curlen -= len;
#ifdef HAVE_INT64_TIMESTAMP
                rc = snprintf_s(cp, curlen, curlen - 1, "%02d.%06d", abs(sec), (int)Abs(fsec));
#else
                rc = snprintf_s(cp, curlen, curlen - 1, "%0*.*f", 9, 9, fabs(sec + fsec));
#endif
                securec_check_ss(rc, "\0", "\0");
            }
            AppendTrailingZeros(cp);
        } break;
            /* SQL Standard interval format */
        case INTSTYLE_SQL_STANDARD: {
            bool has_negative = year < 0 || mon < 0 || mday < 0 || hour < 0 || min < 0 || sec < 0 || fsec < 0;
            bool has_positive = year > 0 || mon > 0 || mday > 0 || hour > 0 || min > 0 || sec > 0 || fsec > 0;
            bool has_year_month = year != 0 || mon != 0;
            bool has_day_time = mday != 0 || hour != 0 || min != 0 || sec != 0 || fsec != 0;
            bool has_day = mday != 0;
            bool sql_standard_value = !(has_negative && has_positive) && !(has_year_month && has_day_time);

            /*
             * SQL Standard wants only 1 "<sign>" preceding the whole
             * interval ... but can't do that if mixed signs.
             */
            if (has_negative && sql_standard_value) {
                *cp++ = '-';
                year = -year;
                mon = -mon;
                mday = -mday;
                hour = -hour;
                min = -min;
                sec = -sec;
                fsec = -fsec;
                curlen--;
            }

            if (!has_negative && !has_positive) {
                rc = sprintf_s(cp, curlen, "0");
                securec_check_ss(rc, "\0", "\0");
            } else if (!sql_standard_value) {
                /*
                 * For non sql-standard interval values, force outputting
                 * the signs to avoid ambiguities with intervals with
                 * mixed sign components.
                 */
                char year_sign = (year < 0 || mon < 0) ? '-' : '+';
                char day_sign = (mday < 0) ? '-' : '+';
                char sec_sign = (hour < 0 || min < 0 || sec < 0 || fsec < 0) ? '-' : '+';

                rc = sprintf_s(cp,
                    curlen,
                    "%c%d-%d %c%d %c%d:%02d:",
                    year_sign,
                    abs(year),
                    abs(mon),
                    day_sign,
                    abs(mday),
                    sec_sign,
                    abs(hour),
                    abs(min));
                securec_check_ss(rc, "\0", "\0");
                cp += strlen(cp);
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, true);
                *cp = '\0';
            } else if (has_year_month) {
                rc = sprintf_s(cp, curlen, "%d-%d", year, mon);
                securec_check_ss(rc, "\0", "\0");
            } else if (has_day) {
                rc = sprintf_s(cp, curlen, "%d %d:%02d:", mday, hour, min);
                securec_check_ss(rc, "\0", "\0");
                cp += strlen(cp);
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, true);
                *cp = '\0';
            } else {
                rc = sprintf_s(cp, curlen, "%d:%02d:", hour, min);
                securec_check_ss(rc, "\0", "\0");
                cp += strlen(cp);
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, true);
                *cp = '\0';
            }
        } break;

            /* ISO 8601 "time-intervals by duration only" */
        case INTSTYLE_ISO_8601:
            /* special-case zero to avoid printing nothing */
            if (year == 0 && mon == 0 && mday == 0 && hour == 0 && min == 0 && sec == 0 && fsec == 0) {
                rc = sprintf_s(cp, MAXDATELEN + 1, "PT0S");
                securec_check_ss(rc, "\0", "\0");
                break;
            }
            *cp++ = 'P';
            cp = AddISO8601IntPart(cp, year, 'Y');
            cp = AddISO8601IntPart(cp, mon, 'M');
            cp = AddISO8601IntPart(cp, mday, 'D');
            if (hour != 0 || min != 0 || sec != 0 || fsec != 0)
                *cp++ = 'T';
            cp = AddISO8601IntPart(cp, hour, 'H');
            cp = AddISO8601IntPart(cp, min, 'M');
            if (sec != 0 || fsec != 0) {
                if (sec < 0 || fsec < 0)
                    *cp++ = '-';
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, false);
                *cp++ = 'S';
                *cp++ = '\0';
            }
            break;

            /* Compatible with postgresql < 8.4 when u_sess->time_cxt.DateStyle = 'iso' */
        case INTSTYLE_POSTGRES:
            cp = AddPostgresIntPart(cp, year, "year", &is_zero, &is_before);

            /*
             * Ideally we should spell out "month" like we do for "year" and
             * "day".  However, for backward compatibility, we can't easily
             * fix this.  bjm 2011-05-24
             */
            cp = AddPostgresIntPart(cp, mon, "mon", &is_zero, &is_before);
            cp = AddPostgresIntPart(cp, mday, "day", &is_zero, &is_before);
            if (is_zero || hour != 0 || min != 0 || sec != 0 || fsec != 0) {
                bool minus = (hour < 0 || min < 0 || sec < 0 || fsec < 0);

                rc = sprintf_s(cp,
                    MAXDATELEN + 1,
                    "%s%s%02d:%02d:",
                    is_zero ? "" : " ",
                    (minus ? "-" : (is_before ? "+" : "")),
                    abs(hour),
                    abs(min));
                securec_check_ss(rc, "\0", "\0");
                cp += strlen(cp);
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, true);
                *cp = '\0';
            }
            break;

            /* Compatible with postgresql < 8.4 when u_sess->time_cxt.DateStyle != 'iso' */
        case INTSTYLE_POSTGRES_VERBOSE:
        default:
            rc = strcpy_s(cp, MAXDATELEN + 1, "@");
            securec_check_ss_c(rc, "\0", "\0");
            cp++;
            curlen--;
            cp = AddVerboseIntPart(cp, year, "year", &is_zero, &is_before);
            cp = AddVerboseIntPart(cp, mon, "mon", &is_zero, &is_before);
            cp = AddVerboseIntPart(cp, mday, "day", &is_zero, &is_before);
            cp = AddVerboseIntPart(cp, hour, "hour", &is_zero, &is_before);
            cp = AddVerboseIntPart(cp, min, "min", &is_zero, &is_before);
            if (sec != 0 || fsec != 0) {
                *cp++ = ' ';
                curlen--;
                if (sec < 0 || (sec == 0 && fsec < 0)) {
                    if (is_zero) {
                        is_before = TRUE;
                    } else if (!is_before) {
                        *cp++ = '-';
                        curlen--;
                    }
                } else if (is_before) {
                    *cp++ = '-';
                    curlen--;
                }
                cp = AppendSeconds(cp, sec, fsec, MAX_INTERVAL_PRECISION, false);
                rc = sprintf_s(cp, curlen, " sec%s", (abs(sec) != 1 || fsec != 0) ? "s" : "");
                securec_check_ss(rc, "\0", "\0");
                is_zero = FALSE;
            }
            /* identically zero? then put in a unitless zero... */
            if (is_zero) {
                rc = strcat_s(cp, curlen, " 0");
                securec_check(rc, "\0", "\0");
            }
            if (is_before) {
                rc = strcat_s(cp, curlen, " ago");
                securec_check(rc, "\0", "\0");
            }
            break;
    }
}

/*
 * We've been burnt by stupid errors in the ordering of the datetkn tables
 * once too often.	Arrange to check them during postmaster start.
 */
static bool CheckDateTokenTable(const char* tablename, const datetkn* base, int nel)
{
    bool ok = true;
    int i;

    for (i = 1; i < nel; i++) {
        if (strncmp(base[i - 1].token, base[i].token, TOKMAXLEN) >= 0) {
            /* %.*s is safe since all our tokens are ASCII */
            elog(LOG,
                "ordering error in %s table: \"%.*s\" >= \"%.*s\"",
                tablename,
                TOKMAXLEN,
                base[i - 1].token,
                TOKMAXLEN,
                base[i].token);
            ok = false;
        }
    }
    return ok;
}

bool CheckDateTokenTables(void)
{
    bool ok = true;

    Assert(UNIX_EPOCH_JDATE == date2j(1970, 1, 1));
    Assert(POSTGRES_EPOCH_JDATE == date2j(2000, 1, 1));

    ok = ok && CheckDateTokenTable("datetktbl", datetktbl, szdatetktbl);
    ok = ok && CheckDateTokenTable("deltatktbl", deltatktbl, szdeltatktbl);
    return ok;
}

/*
 * Common code for temporal protransform functions.  Types time, timetz,
 * timestamp and timestamptz each have a range of allowed precisions.  An
 * unspecified precision is rigorously equivalent to the highest specifiable
 * precision.
 *
 * Note: timestamp_scale throws an error when the typmod is out of range, but
 * we can't get there from a cast: our typmodin will have caught it already.
 */
Node* TemporalTransform(int32 max_precis, Node* node)
{
    FuncExpr* expr = (FuncExpr*)node;
    Node* ret = NULL;
    Node* typmod = NULL;

    Assert(IsA(expr, FuncExpr));
    Assert(list_length(expr->args) >= 2);

    typmod = (Node*)lsecond(expr->args);

    if (IsA(typmod, Const) && !((Const*)typmod)->constisnull) {
        Node* source = (Node*)linitial(expr->args);
        int32 old_precis = exprTypmod(source);
        int32 new_precis = DatumGetInt32(((Const*)typmod)->constvalue);

        if (new_precis < 0 || new_precis == max_precis || (old_precis >= 0 && new_precis >= old_precis))
            ret = relabel_to_typmod(source, new_precis);
    }

    return ret;
}

/*
 * This function gets called during timezone config file load or reload
 * to create the final array of timezone tokens.  The argument array
 * is already sorted in name order.  The data is converted to datetkn
 * format and installed in *tbl, which must be allocated by the caller.
 */
void ConvertTimeZoneAbbrevs(TimeZoneAbbrevTable* tbl, struct tzEntry* abbrevs, int n)
{
    datetkn* newtbl = tbl->abbrevs;
    int i;

    tbl->numabbrevs = n;
    for (i = 0; i < n; i++) {
        errno_t rc = strncpy_s(newtbl[i].token, TOKMAXLEN + 1, abbrevs[i].abbrev, TOKMAXLEN);
        securec_check(rc, "\0", "\0");
        newtbl[i].type = abbrevs[i].is_dst ? DTZ : TZ;
        TOVAL(&newtbl[i], abbrevs[i].offset / MINS_PER_HOUR);
    }

    /* Check the ordering, if testing */
    Assert(CheckDateTokenTable("timezone offset", newtbl, n));
}

/*
 * Install a TimeZoneAbbrevTable as the active table.
 *
 * Caller is responsible that the passed table doesn't go away while in use.
 */
void InstallTimeZoneAbbrevs(TimeZoneAbbrevTable* tbl)
{
    int i;

    u_sess->time_cxt.timezone_tktbl = tbl->abbrevs;
    u_sess->time_cxt.sz_timezone_tktbl = tbl->numabbrevs;

    /* clear date cache in case it contains any stale timezone names */
    for (i = 0; i < MAXDATEFIELDS; i++)
        u_sess->time_cxt.datecache[i] = NULL;
}

/*
 * This set-returning function reads all the available time zone abbreviations
 * and returns a set of (abbrev, utc_offset, is_dst).
 */
Datum pg_timezone_abbrevs(PG_FUNCTION_ARGS)
{
    FuncCallContext* funcctx = NULL;
    int* pindex = NULL;
    Datum result;
    HeapTuple tuple;
    Datum values[3];
    bool nulls[3];
    char buffer[TOKMAXLEN + 1];
    unsigned char* p = NULL;
    struct pg_tm tm;
    Interval* resInterval = NULL;

    /* stuff done only on the first call of the function */
    if (SRF_IS_FIRSTCALL()) {
        TupleDesc tupdesc;
        MemoryContext oldcontext;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /*
         * switch to memory context appropriate for multiple function calls
         */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* allocate memory for user context */
        pindex = (int*)palloc(sizeof(int));
        *pindex = 0;
        funcctx->user_fctx = (void*)pindex;

        /*
         * build tupdesc for result tuples. This must match this function's
         * pg_proc entry!
         */
        tupdesc = CreateTemplateTupleDesc(3, false);
        TupleDescInitEntry(tupdesc, (AttrNumber)1, "abbrev", TEXTOID, -1, 0);
        TupleDescInitEntry(tupdesc, (AttrNumber)2, "utc_offset", INTERVALOID, -1, 0);
        TupleDescInitEntry(tupdesc, (AttrNumber)3, "is_dst", BOOLOID, -1, 0);

        funcctx->tuple_desc = BlessTupleDesc(tupdesc);
        MemoryContextSwitchTo(oldcontext);
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();
    pindex = (int*)funcctx->user_fctx;

    if (*pindex >= u_sess->time_cxt.sz_timezone_tktbl)
        SRF_RETURN_DONE(funcctx);

    errno_t errorno = EOK;
    errorno = memset_s(nulls, sizeof(nulls), 0, sizeof(nulls));
    securec_check(errorno, "\0", "\0");

    /*
     * Convert name to text, using upcasing conversion that is the inverse of
     * what ParseDateTime() uses.
     */
    errno_t rc = strncpy_s(buffer, TOKMAXLEN + 1, u_sess->time_cxt.timezone_tktbl[*pindex].token, TOKMAXLEN);
    securec_check(rc, "\0", "\0");
    buffer[TOKMAXLEN] = '\0'; /* may not be null-terminated */
    for (p = (unsigned char*)buffer; *p; p++)
        *p = pg_toupper(*p);

    values[0] = CStringGetTextDatum(buffer);

    rc = memset_s(&tm, sizeof(struct pg_tm), 0, sizeof(struct pg_tm));
    securec_check(rc, "\0", "\0");
    tm.tm_min = (-1) * FROMVAL(&u_sess->time_cxt.timezone_tktbl[*pindex]);
    resInterval = (Interval*)palloc(sizeof(Interval));
    tm2interval(&tm, 0, resInterval);
    values[1] = IntervalPGetDatum(resInterval);

    Assert(u_sess->time_cxt.timezone_tktbl[*pindex].type == DTZ || u_sess->time_cxt.timezone_tktbl[*pindex].type == TZ);
    values[2] = BoolGetDatum(u_sess->time_cxt.timezone_tktbl[*pindex].type == DTZ);

    (*pindex)++;

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);

    SRF_RETURN_NEXT(funcctx, result);
}

/*
 * This set-returning function reads all the available full time zones
 * and returns a set of (name, abbrev, utc_offset, is_dst).
 */
Datum pg_timezone_names(PG_FUNCTION_ARGS)
{
    MemoryContext oldcontext;
    FuncCallContext* funcctx = NULL;
    pg_tzenum* tzenum = NULL;
    pg_tz* tz = NULL;
    Datum result;
    HeapTuple tuple;
    Datum values[4];
    bool nulls[4];
    int tzoff;
    struct pg_tm tm;
    fsec_t fsec;
    const char* tzn = NULL;
    Interval* resInterval = NULL;
    struct pg_tm itm;

    /* stuff done only on the first call of the function */
    if (SRF_IS_FIRSTCALL()) {
        TupleDesc tupdesc;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /*
         * switch to memory context appropriate for multiple function calls
         */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* initialize timezone scanning code */
        tzenum = pg_tzenumerate_start();
        funcctx->user_fctx = (void*)tzenum;

        /*
         * build tupdesc for result tuples. This must match this function's
         * pg_proc entry!
         */
        tupdesc = CreateTemplateTupleDesc(4, false);
        TupleDescInitEntry(tupdesc, (AttrNumber)1, "name", TEXTOID, -1, 0);
        TupleDescInitEntry(tupdesc, (AttrNumber)2, "abbrev", TEXTOID, -1, 0);
        TupleDescInitEntry(tupdesc, (AttrNumber)3, "utc_offset", INTERVALOID, -1, 0);
        TupleDescInitEntry(tupdesc, (AttrNumber)4, "is_dst", BOOLOID, -1, 0);

        funcctx->tuple_desc = BlessTupleDesc(tupdesc);
        MemoryContextSwitchTo(oldcontext);
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();
    tzenum = (pg_tzenum*)funcctx->user_fctx;

    /* search for another zone to display */
    for (;;) {
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
        tz = pg_tzenumerate_next(tzenum);
        MemoryContextSwitchTo(oldcontext);

        if (NULL == tz) {
            pg_tzenumerate_end(tzenum);
            funcctx->user_fctx = NULL;
            SRF_RETURN_DONE(funcctx);
        }

        /* Convert now() to local time in this zone */
        if (timestamp2tm(GetCurrentTransactionStartTimestamp(), &tzoff, &tm, &fsec, &tzn, tz) != 0)
            continue; /* ignore if conversion fails */

        /* Ignore zic's rather silly "Factory" time zone */
        if (tzn && strcmp(tzn, "Local time zone must be set--see zic manual page") == 0)
            continue;

        /* Found a displayable zone */
        break;
    }

    errno_t errorno = EOK;
    errorno = memset_s(nulls, sizeof(nulls), 0, sizeof(nulls));
    securec_check(errorno, "\0", "\0");

    values[0] = CStringGetTextDatum(pg_get_timezone_name(tz));
    values[1] = CStringGetTextDatum(tzn ? tzn : "");

    errorno = memset_s(&itm, sizeof(struct pg_tm), 0, sizeof(struct pg_tm));
    securec_check(errorno, "\0", "\0");
    itm.tm_sec = -tzoff;
    resInterval = (Interval*)palloc(sizeof(Interval));
    tm2interval(&itm, 0, resInterval);
    values[2] = IntervalPGetDatum(resInterval);

    values[3] = BoolGetDatum(tm.tm_isdst > 0);

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);

    SRF_RETURN_NEXT(funcctx, result);
}

static void check_dtype (int dtype, struct pg_tm *tm, fsec_t fsec, Interval *result, char *str)
{
    switch (dtype) {
        case DTK_DELTA:
            if (tm2interval(tm, fsec, result) != 0)
                ereport(ERROR,
                        (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                         errmsg("interval out of range")));
            break;
        case DTK_INVALID:
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("date/time value \"%s\" is no longer supported", str)));
            break;
        default:
            ereport(ERROR,
                    (errcode(ERRCODE_WRONG_OBJECT_TYPE),
                     errmsg("unexpected dtype %d while parsing interval \"%s\"", dtype, str)));
    }
}

Interval *char_to_interval(char *str, int32 typmod, bool can_ignore) {
    Interval       *result = NULL;
    fsec_t         fsec = 0;
    struct pg_tm tt, *tm = &tt;
    int            dtype;
    int            nf;
    int            range = INTERVAL_FULL_RANGE;
    int            dterr;
    char           *field[MAXDATEFIELDS];
    int            ftype[MAXDATEFIELDS];
    char           workbuf[256];
    // negative ISO8601 interval flag
    bool           isnegative = false;

    tm->tm_year = 0;
    tm->tm_mon = 0;
    tm->tm_mday = 0;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;

    if (typmod >= 0)
        range = INTERVAL_RANGE(typmod);

    dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);
    if (dterr == 0)
        dterr = DecodeInterval(field, ftype, nf, range, &dtype, tm, &fsec);

    /* if those functions think it's a bad format, try ISO8601 style */
    if (dterr == DTERR_BAD_FORMAT) {
        while (' ' == *str) {
            str++;
        }
        if ('-' == *str) {
            isnegative = true;
            str++;
        }
        dterr = DecodeISO8601Interval(str, &dtype, tm, &fsec);
    }

    if (dterr != 0) {
        if (dterr == DTERR_FIELD_OVERFLOW)
            dterr = DTERR_INTERVAL_OVERFLOW;
        DateTimeParseError(dterr, str, "interval", can_ignore);
        /* if invalid input error is ignorable, set the result to 0 */
        tm->tm_year = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 0;
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
    }
    // process negative ISO8601 interval
    if (true == isnegative) {
        tm->tm_year = -tm->tm_year;
        tm->tm_mon = -tm->tm_mon;
        tm->tm_mday = -tm->tm_mday;
        tm->tm_hour = -tm->tm_hour;
        tm->tm_min = -tm->tm_min;
        tm->tm_sec = -tm->tm_sec;
        fsec = -fsec;
    }

    result = (Interval *) palloc(sizeof(Interval));
    check_dtype (dtype, tm, fsec, result, str);
    return result;
}
#ifdef DOLPHIN
/* Convert a double value into pg_tm based on 1970-01-01 00:00:00 UTC, in timezone 0.
 * Therefore, you may need to add the timezone offset outside.
 * @param unixtimestamp seconds from 1970-01-01 00:00:00
 */
void Unixtimestamp2tm(double unixtimestamp, struct pg_tm* tm, fsec_t* fsec)
{
    DateADT date = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    int days = 0;
    double int_val = 0.0;
    double frac_val = 0.0;
    long sec_time = 0L;
    long frac_time = 0L;

    frac_val = modf(unixtimestamp, &int_val);
    sec_time = lrint(int_val);
    long temp = lrint(frac_val * 1000000.0);
    frac_time = temp < 999999L ? temp : 999999L;
#ifdef HAVE_INT64_TIMESTAMP
    *fsec = frac_time;
#else
    *fsec = frac_time / 1000000.0;
#endif
    days = sec_time / SECS_PER_DAY;
    date = date2j(1970, 1, 1);
    date += days;
    j2date(date, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));

    sec_time -= days * SECS_PER_DAY;
    hour = sec_time / SECS_PER_HOUR;
    min = (sec_time - hour * SECS_PER_HOUR) / SECS_PER_MINUTE;
    sec = sec_time - hour * SECS_PER_HOUR - min * SECS_PER_MINUTE;

    tm->tm_hour = hour;
    tm->tm_min = min;
    tm->tm_sec = sec;
}

/*
 * @Description: Convert NumericVar value to lldiv_t value.
 * @return: true: Success to convert.
 *          false: Overflow
 */
bool numeric_to_lldiv_t(NumericVar *from, lldiv_t *to)
{
    if (!from->ndigits) {
        /* from == 0 */
        to->quot = 0;
        to->rem = 0;
        return true;
    }

    to->quot = to->rem = 0;
    int int_end_pos = (from->weight < 0 ? -1 : from->weight);
    int frac_start_pos = int_end_pos + 1;
    /* Avoid to overflow. */
    if (int_end_pos > DEC_DIGITS ||
        (from->weight == DEC_DIGITS && from->digits[0] >= (int64)(LLDIV_MAX / pow_of_10[from->weight * DEC_DIGITS]))) {
        to->quot = from->sign ? LLDIV_MIN : LLDIV_MAX;
        to->rem = 0;
        return false;
    }

    for (int i = from->weight, j = 0; i >= 0 && j < from->ndigits; i--, j++) {
        to->quot += from->digits[j] * pow_of_10[i * DEC_DIGITS];
    }

    int frac_group = from->ndigits - frac_start_pos;
    if (from->dscale > 0 && from->weight >= -2) {
        if (frac_group == 1) {
            if (from->weight == -2) {
                to->rem = from->digits[frac_start_pos] * pow_of_10[1];
            } else {
                to->rem = from->digits[frac_start_pos] * NBASE * pow_of_10[1];
            }
        } else {
            to->rem = (from->digits[frac_start_pos] * NBASE + from->digits[frac_start_pos + 1]) * pow_of_10[1];
        }
    }

    if (from->sign) {
        to->quot = -to->quot;
        to->rem = -to->rem;
    }

    return true;
}

// extract integer part and fractional part from numeric
// attention： to->rem must be divided by (long long)pow_of_10[8] to get a correct fractional part
void NumericVar2lldiv(NumericVar *from, lldiv_t *to)
{
    if (!from->ndigits  || from->weight < -2) {
        /* from == 0 */
        to->quot = 0;
        to->rem = 0;
        return;
    }
    to->quot = to->rem = 0;

    long long bound_val = from->sign ? LONG_LONG_MIN : LONG_LONG_MAX;

    if (from->weight > DEC_DIGITS) {
        to->quot = bound_val;
        to->rem = 0;
        return;
    }

    int int_remain_field = (from->weight >= 0) ? (from->weight + 1) : 0;
    
    int i = 0;
    for (; int_remain_field > 0 && i < from->ndigits; --int_remain_field, ++i) {
        if (from->weight == DEC_DIGITS) {
            if((long long)(from->digits[i]) > abs(bound_val / (long long)pow_of_10[(int_remain_field - 1) * DEC_DIGITS] % 10000)) {
                to->quot = bound_val;
                to->rem = 0;
                return;
            }
        }
        to->quot += (long long)(from->digits[i]) * (long long)pow_of_10[(int_remain_field - 1) * DEC_DIGITS];
    }

    if (i == from->ndigits) {
        to->rem = 0;
    } else {
        int frac_remain_field = 2;
        if (from->weight >= -1) {
            for (; i < from->ndigits && frac_remain_field > 0; ++i, --frac_remain_field) {
                to->rem = to->rem * pow_of_10[4] + from->digits[i];
            }
            if (frac_remain_field) {
                for(; frac_remain_field > 0; --frac_remain_field)
                    to->rem *= pow_of_10[4];
            }
        } else {
            to->rem = from->digits[i];
        }
    }

    if (from->sign) {
        to->quot = -to->quot;
        to->rem = -to->rem;
    }

    return;
}

//decode the integer to pg_tm and do a range check as what MSQ does
//true if success
//false if faild
static bool int8_to_tm(int64 int_val, struct pg_tm *tm, unsigned int date_flag, int *date_type)
{
    long long date_part = 0;
    long long time_part = 0;
    long long date = (long long)int_val;
    *date_type = DTK_DATE;

    errno_t rc = memset_s(tm, sizeof(*tm), 0, sizeof(*tm));
    securec_check(rc, "\0", "\0");

    if (date == 0 || date >= B_NORMAL_NUMBER_DATETIME) { // 1000-01-01 00:00:00
        *date_type = DTK_DATE_TIME;
        if (date > B_MAX_NUMBER_DATETIME) // 9999-12-31 23:59:59
            return false;
        goto decode_tm;
    }

    /* first consider the case of xxxx-xx-xx 00:00:00 */

    if (date < B_FORMAT_DATE_INT_MIN)
        return false;
    if (date <= TWO_DIGITS_YEAR_DATE_ONE) {  // 2000-1-1 ~ 2069-12-31
        date = (date + 20000000) * 1000000;
        goto decode_tm;
    }
    if (date < TWO_DIGITS_YEAR_DATE_TWO)  // 1970-1999
        return false;
    if (date <= TWO_DIGITS_YEAR_DATE_THREE) {
        date = (date + 19000000) * 1000000; 
        goto decode_tm;
    }

    if (date <= B_FORMAT_MAX_DATE) {
        date = date * 1000000;
        goto decode_tm;
    }
    if (date < (B_FORMAT_DATE_INT_MIN * 1000000))
        return false;

    //now consider the case of xxxx-xx-xx xx:xx:xx
    
    *date_type = DTK_DATE_TIME;

    if (date <= TWO_DIGITS_YEAR_DATETIME_ONE) { // 2069-12-31 23:59:59
        date = date + 20000000000000LL;
        goto decode_tm;
    }
    if (date < TWO_DIGITS_YEAR_DATETIME_TWO) // 1970-01-01 00:00:00
        return false;
    if (date <= TWO_DIGITS_YEAR_DATETIME_THREE)   //1999-12-31 23:59:59
        date = date + 19000000000000LL;

decode_tm:
    date_part = (long long)(date / 1000000);//date part
    time_part = (long long)(date - date_part * 1000000);//time part
    tm->tm_year = (int)(date_part / 10000);
    date_part = date_part % 10000;
    tm->tm_mon = (int)(date_part / 100);
    tm->tm_mday = (int)(date_part % 100);
    tm->tm_hour = (int)(time_part / 10000);
    time_part = time_part % 10000;
    tm->tm_min = (int)(time_part / 100);
    tm->tm_sec = (int)(time_part % 100);

    //year_check
    if (tm->tm_year < B_FORMAT_MIN_YEAR_OF_DATE || tm->tm_year > B_FORMAT_MAX_YEAR_OF_DATE) {
        return false;
    }
    //month_check
    if ((date_flag & ENABLE_ZERO_MONTH)) {
        if (tm->tm_mon < 0 || tm->tm_mon > MONTHS_PER_YEAR)
            return false;
    } else if (tm->tm_mon < 1 || tm->tm_mon > MONTHS_PER_YEAR)
        return false;
    //day_check
    if ((date_flag & ENABLE_ZERO_DAY)) {
        if (tm->tm_mday < 0 || tm->tm_mday > DAYNUM_BIGMON)
            return false;
    } else if(tm->tm_mday < 1 || tm->tm_mday > DAYNUM_BIGMON)
        return false;
    //day and month check
    if (tm->tm_year == B_FORMAT_MIN_YEAR_OF_DATE && tm->tm_mon == FEBRUARY && tm->tm_mday == DAYNUM_FEB_LEAPYEAR)
        return false;
    if ((tm->tm_mon == 0) && (date_flag & ENABLE_ZERO_MONTH)) {//do nothing
    } else if (tm->tm_mday > day_tab[isleap(tm->tm_year)][tm->tm_mon - 1])
        return false;

    /* validate min, sec,*/
    if (tm->tm_min < 0 || tm->tm_min >= MINS_PER_HOUR || tm->tm_sec < 0 ||
        tm->tm_sec >= SECS_PER_MINUTE)
        return false;

    /* validate hour */
    if (tm->tm_hour < 0 || tm->tm_hour >= HOURS_PER_DAY)
        return false;

    return true;
}

static bool lldiv_decode_tm_internal(lldiv_t *div,  struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag, int *date_type)
{
    if (div->quot < 0 || div->rem < 0)
        return false;
    long frac_val = lrint((double)div->rem / pow_of_10[2]);
#ifdef HAVE_INT64_TIMESTAMP
    *fsec = (int32)frac_val;
#else
    *fsec = frac_val / 1000000.0;
#endif
    return int8_to_tm(div->quot, tm, date_flag, date_type);
}

void lldiv_decode_tm(Numeric num, lldiv_t *div, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag, int *date_type)
{
    int errlevel = (SQL_MODE_STRICT() ? ERROR : WARNING);
    if (!lldiv_decode_tm_internal(div, tm, fsec, date_flag, date_type)) {
        char * str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(num)));
        ereport(ERROR,
            (errcode(DTERR_BAD_FORMAT), errmsg("Incorrect datetime value: \"%s\"", str)));
    }

    if (*date_type == DTK_DATE_TIME && !MaybeRound(tm, fsec)) {
        char * str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(num)));
        ereport(ERROR,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect datetime value: \"%s\"", str)));
    } else if (*date_type == DTK_DATE && *fsec) {
        char * str = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(num)));
        ereport(errlevel,
                (errcode(DTERR_BAD_FORMAT), errmsg("Truncated incorrect date value: \"%s\"", str)));
    }
    return;
}

bool lldiv_decode_datetime(Numeric num, lldiv_t *div, struct pg_tm *tm, fsec_t *fsec, unsigned int date_flag, int *date_type)
{
    bool ret = true;
    int code;
    const char *msg = NULL;
    PG_TRY();
    {
        lldiv_decode_tm(num, div, tm, fsec, date_flag, date_type);
    }
    PG_CATCH();
    {
        ret = false;
        if (SQL_MODE_STRICT()) {
            PG_RE_THROW();
        } else {
            code = geterrcode();
            msg = pstrdup(Geterrmsg());
            FlushErrorState();
        }
    }
    PG_END_TRY();
    if (msg) {
        ereport(WARNING, (errcode(code), errmsg("%s", msg)));
    }
    return ret;
}

bool lldiv_decode_tm_with_sql_mode(Numeric num, lldiv_t *div, struct pg_tm *tm, unsigned int date_flag)
{
    fsec_t fsec = 0;
    int date_type = 0;
    return lldiv_decode_datetime(num, div, tm, &fsec, date_flag, &date_type);
}

/* Calc days in one year. works with 0 <= year <= 99 */
unsigned int calc_days_in_year(int year)
{
    return ((year & 3) == 0 && (year % 100 || (year % 400 == 0 && year)) ? DAYS_PER_LEAP_YEAR : DAYS_PER_COMMON_YEAR);
}

/**
  @brief Check datetime value for validity according to flags.

  @param[in] tm Date to check.
  @param[in] not_zero_date tm is not the zero date
  @param[in] flags flags to check (see details in datetime.h)

  @details Here we assume that year and month is ok!
    If month is 0 we allow any date. (This only happens if we allow zero
    date parts in str_to_datetime())
    Disallow dates with zero year and non-zero month and/or day.

  @return
   @retval true - success
   @retval false - error
*/

bool CheckDateRange(const pg_tm *tm, bool not_zero_date, time_flags flags)
{
    if (not_zero_date) {
        if (((flags & TIME_NO_ZERO_IN_DATE) || !(flags & TIME_FUZZY_DATE)) &&
            (tm->tm_mon == 0 || tm->tm_mday == 0)) {
            return false;
        } else if ((!(flags & TIME_INVALID_DATES) &&
                    tm->tm_mon && tm->tm_mday > day_tab[0][tm->tm_mon - 1] &&
                    (tm->tm_mon != FEBRUARY || calc_days_in_year(tm->tm_year) != DAYS_PER_LEAP_YEAR ||
                     tm->tm_mday != DAYNUM_FEB_LEAPYEAR))) {
            return false;
        }
    } else if (flags & TIME_NO_ZERO_DATE) {
        return false;
    }
    return true;
}

/**
  @brief Check datetime, date, or normalized time (i.e. time without days) range.
  
  @param tm variable to store datetime
  
  @returns
   @retval true - success
   @retval false - error
*/
bool CheckDatetimeRange(const pg_tm *tm, const fsec_t fsec, const int tm_type)
{
    /*
      In case of DTK_TIME hour value can be up to TIME_MAX_HOUR.
      In case of DTK_DATE_TIME it cannot be bigger than 23.
    */
    return !(tm->tm_year > 9999 || tm->tm_mon > 12 || tm->tm_mday > 31 ||
             tm->tm_min > 59 || tm->tm_sec > 59 || fsec > 999999 ||
             (tm->tm_hour > (tm_type == DTK_TIME ? TIME_MAX_HOUR : 23)));
            
}

/**
  @brief Convert a timestamp string to a PG_TIME value.

  @param[in] str String to parse
  @param[in] tm Date is stored here
  @param[in] fsec Microsecond is stored here
  @param[in] nano Nanosecond is stored here
  @param[in] tm_type Type of the result, such as date or datetime

  @details
    At least the following formats are recogniced (based on number of digits)
    YYMMDD, YYYYMMDD, YYMMDDHHMMSS, YYYYMMDDHHMMSS
    YY-MM-DD, YYYY-MM-DD, YY-MM-DD HH.MM.SS
    YYYYMMDDTHHMMSS  where T is a the character T (ISO8601)
    Also dates where all parts are zero are allowed

    The second part may have an optional .###### fraction part.

  @note
   This function should work with a format position vector as long as the
   following things holds:
   - All date are kept together and all time parts are kept together
   - Date and time parts must be separated by blank
   - Second fractions must come after second part and be separated
     by a '.'.  (The second fractions are optional)
   - AM/PM must come after second fractions (or after seconds if no fractions)
   - Year must always been specified.
   - If time is before date, then we will use datetime format only if
     the argument consist of two parts, separated by space.
     Otherwise we will assume the argument is a date.
   - The hour part must be specified in hour-minute-second order.

  @return
   @retval true - success
   @retval false - error
*/

bool cstring_to_datetime(const char* str,  time_flags flags, int &tm_type, 
                        pg_tm *tm, fsec_t &fsec, int &nano, bool &warnings, bool *null_func_result)
{
    size_t length = strlen(str);
    unsigned int field_length = 0, year_length = 0, digits, i, number_of_fields;
    unsigned int date[MAX_DATE_PARTS], date_len[MAX_DATE_PARTS];
    unsigned int start_loop = 0; /* Start of scan loop */
    unsigned long not_zero_date, allow_space;
    bool is_internal_format = false;
    const char *pos, *last_field_pos = NULL;
    const char* end = str + length;
    const unsigned char* format_position;
    bool found_delimitier = 0, found_space = 0;
    unsigned int frac_pos, frac_len;
    unsigned int fractional_digits = 0;

    /* Skip space at start */
    for (; str != end && isspace(*str); str++) {}
    if (str == end || !isdigit(*str)) {
        tm_type = DTK_NONE;
        return false;
    }
    
    /* This has to be changed if want to activate different timestamp formats */
    format_position = internal_format_positions;

    /*
      Calculate number of digits in first part.
      If length= 8 or >= 14 then year is of format YYYY.
      (YYYY-MM-DD,  YYYYMMDD, YYYYYMMDDHHMMSS)
    */
    for (pos = str;
         pos != end && (isdigit(*pos) || *pos == 'T');
         pos++) {}

    digits = (unsigned int)(pos - str);
    date_len[format_position[0]] = 0; /* Length of year field */
    if (pos == end || *pos == '.') {
        /* Found date in internal format (only numbers like YYYYMMDD) */
        year_length = (digits == 4 || digits == 8 || digits >= 14) ? 4 : 2;
        field_length = year_length;
        is_internal_format = 1;
        format_position = internal_format_positions;
    } else {
        field_length = format_position[0] == 0 ? 4 : 2;
    }

    /*
      Only allow space in the first "part" of the datetime field and
      after days, part seconds
    */
    allow_space = ((1 << format_position[2]) | (1 << format_position[6]));

    not_zero_date = 0;
    for (i = start_loop;
         i < MAX_DATE_PARTS - 1 && str != end && isdigit(*str);
         i++) {
        const char* start = str;
        unsigned long tmp_value = (unsigned int)(unsigned char)(*str++ - '0');

        /*
          Internal format means no delimiters; every field has a fixed
          width. Otherwise, we scan until we find a delimiter and discard
          leading zeroes -- except for the microsecond part, where leading
          zeroes are significant, and where we never process more than six
          digits.
        */
        bool scan_until_delim = !is_internal_format &&
                                ((i != format_position[6]));

        while (str != end && isdigit(str[0]) &&
               (scan_until_delim || --field_length)) {
            tmp_value = tmp_value * 10 + (unsigned long)(unsigned char)(*str - '0');
            str++;
        }
        date_len[i] = (unsigned int)(str - start);
        /* Impossible date part */
        if (tmp_value > 999999) {
            tm_type = DTK_NONE;
            return false;
        }
        date[i] = tmp_value;
        not_zero_date |= tmp_value;

        /* Length of next field */
        field_length = format_position[i + 1] == 0 ? 4 : 2;

        if ((last_field_pos = str) == end) {
            i++; /* Register last found part */
            break;
        }
        /* Allow a 'T' after day to allow CCYYMMDDT type of fields */
        if (i == format_position[2] && *str == 'T') {
            str++; /* ISO8601:  CCYYMMDDThhmmss */
            continue;
        }
        /* Seconds */
        if (i == format_position[5]) {
            /* Followed by part seconds */
            if (*str == '.') {
                str++;
                /*
                  Shift last_field_pos, so '2001-01-01 00:00:00.'
                  is treated as a valid value
                */
                last_field_pos = str;
                field_length = 6; /* 6 digits */
                continue;
            } else if (isdigit(str[0])) {
                /*
                  We do not see a decimal point which would have indicated a
                  fractional second part in further read. So we skip the further
                  processing of digits.
                */
                i++;
                break;
            }
        }
        while (str != end &&
               (ispunct(*str) || isspace(*str))) {
            if (isspace(*str)) {
                if (!(allow_space & (1 << i))) {
                    tm_type = DTK_NONE;
                    return false;
                }
                found_space = 1;
            }
            str++;
            found_delimitier = 1; /* Should be a 'normal' date */
        }
        /* Check if next position is AM/PM */
        /* Seconds, time for AM/PM */
        if (i == format_position[6]) {
            i++; /* Skip AM/PM part */
        }
        last_field_pos = str;
    }
    if (found_delimitier && !found_space && (flags & TIME_DATETIME_ONLY)) {
        tm_type = DTK_NONE;
        *null_func_result = true;
        return false; /* Can't be a datetime */
    }

    str = last_field_pos;

    number_of_fields = i - start_loop;
    while (i < MAX_DATE_PARTS) {
        date_len[i] = 0;
        date[i++] = 0;
    }

    if (!is_internal_format) {
        year_length = date_len[(unsigned int)format_position[0]];
        /* Year must be specified */
        if (!year_length) {
            tm_type = DTK_NONE;
            *null_func_result = true;
            return false;
        }

        tm->tm_year = date[(unsigned int)format_position[0]];
        tm->tm_mon = date[(unsigned int)format_position[1]];
        tm->tm_mday = date[(unsigned int)format_position[2]];
        tm->tm_hour = date[(unsigned int)format_position[3]];
        tm->tm_min = date[(unsigned int)format_position[4]];
        tm->tm_sec = date[(unsigned int)format_position[5]];

        frac_pos = (unsigned int)format_position[6];
        frac_len = date_len[frac_pos];
        fractional_digits = frac_len;
        if (frac_len < 6) {
            date[frac_pos] *= (unsigned int)pow_of_10[DATETIME_MAX_DECIMALS - frac_len];
        }
        fsec = date[frac_pos];
    } else {
        tm->tm_year = date[0];
        tm->tm_mon = date[1];
        tm->tm_mday = date[2];
        tm->tm_hour = date[3];
        tm->tm_min = date[4];
        tm->tm_sec = date[5];
        if (date_len[6] < 6) {
            date[6] *= (unsigned int)pow_of_10[DATETIME_MAX_DECIMALS - date_len[6]];
        }
        fsec = date[6];
        fractional_digits = date_len[6];
    }

    if (year_length == 2 && not_zero_date) {
        tm->tm_year += (tm->tm_year < YY_PART_YEAR ? 2000 : 1900);
    }

    /*
      Set time_type before CheckDatetime(),
      as the latter relies on initialized time_type value.
    */
    tm_type = (number_of_fields <= 3 ? DTK_DATE : DTK_DATE_TIME);

    if (number_of_fields < 3 || !CheckDatetimeRange(tm, fsec, tm_type)) {
        /* Only give warning for a zero date if there is some garbage after */
        /* If zero date */
        if (!not_zero_date) {
            for (; str != end; str++) {
                if (!isspace(*str)) {
                    not_zero_date = 1; /* Give warning */
                    break;
                }
            }
        }
        warnings = true;
        *null_func_result = true;
        goto ERROR_STRING_DATETIME;
    }

    if (!CheckDateRange(tm, not_zero_date != 0, flags)) {
        *null_func_result = true;
        goto ERROR_STRING_DATETIME;
    }

    /* Scan all digits left after microseconds */
    if (fractional_digits == 6 && str != end) {
        if (isdigit(*str)) {
            /*
              We don't need the exact nanoseconds value.
              Knowing the first digit is enough for rounding.
            */
            nano = 100 * (int)(*str++ - '0');
            for (; str != end && isdigit(*str); str++) {}
        }
    }

    for (; str != end; str++) {
        if (!isspace(*str)) {
            warnings = true;
            break;
        }
    }

    return true;

ERROR_STRING_DATETIME:
    tm_type = DTK_ERROR;
    errno_t rc = memset_s(tm, sizeof(struct pg_tm), 0, sizeof(struct pg_tm));
    securec_check(rc, "\0", "\0");
    fsec = 0;
    return false;
}

/**
  @brief Add nanoseconds to a datetime value with rounding.

  @param [in] tm   variable to store datetime
  @param [in] fsec variable to add to
  @param [in] nano nanosecond

  @return
   @retval true - success
   @retval false - error
*/
bool datetime_add_nanoseconds_with_round(pg_tm *tm, fsec_t &fsec, int nano)
{
    if (nano >= 1000000000)
        return true;
    if (nano < 500)
        return true;

    fsec += (nano + 500) / 1000;
    if (fsec < USECS_PER_SEC)
        return true;

    fsec %= USECS_PER_SEC;
    /* date_add_interval cannot handle bad dates */
    bool non_zero_date = (tm->tm_year || tm->tm_mon || tm->tm_mday);
    if (!CheckDateRange(tm, non_zero_date,
                   (TIME_NO_ZERO_IN_DATE | TIME_NO_ZERO_DATE)))
        return false;

    tm->tm_sec += 1;
    
    Timestamp datetime;
    if (tm2timestamp(tm, fsec, NULL, &datetime) == -1) {
        return false;
    }
    if (datetime >= B_FORMAT_TIMESTAMP_MIN_VALUE && datetime <= B_FORMAT_TIMESTAMP_MAX_VALUE) {
        fsec_t tmp_fsec = fsec;
        timestamp2tm(datetime, NULL, tm, &tmp_fsec, NULL, NULL);
        fsec = tmp_fsec;
        return true;
    }
    return false;
}

/**
 * @brief Convert cstring to pg_tm with rounding
 * 
 * @param[in] expr cstring to parse
 * @param[in] tm pg_tm to store the result of datetime
 * @param[in] fsec variable to store the result of microsecond
 * 
 * @return
 *  @retval true - success
 *  @retval false - error
 */
bool cstring_to_tm(const char *expr, pg_tm *tm, fsec_t &fsec)
{
    int nano = 0, tm_type = DTK_NONE;
    bool warnings = false;
    bool null_func_result = false;
    if (!cstring_to_datetime(expr, TIME_NO_ZERO_DATE, tm_type, tm, fsec, nano, warnings, &null_func_result) ||
        !datetime_add_nanoseconds_with_round(tm, fsec, nano)) {
            return false;
        }

    return true;
}

char* AppendFsec(int64 quot, fsec_t fsec)
{
    errno_t rc = EOK;
    char* result = (char*)palloc(NUMERIC_MAX_STRING_LENGTH);
    if (fsec == 0) {
        rc = sprintf_s(result, NUMERIC_MAX_STRING_LENGTH, "%ld", quot);
    } else {
#ifdef HAVE_INT64_TIMESTAMP
        rc = sprintf_s(result, NUMERIC_MAX_STRING_LENGTH, "%ld.%d", quot, (int)Abs(fsec));
#else
        rc = sprintf_s(result, NUMERIC_MAX_STRING_LENGTH, "%f", fabs(((double)quot) + fsec));
#endif
    }
    securec_check_ss(rc, "\0", "\0");
    return result;
}
#endif