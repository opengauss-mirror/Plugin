#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define CMD_TAG_IS_SELECT() (t_thrd.postgres_cxt.cur_command_tag == T_SelectStmt || \
    t_thrd.postgres_cxt.cur_command_tag == T_ShowEventStmt || \
    t_thrd.postgres_cxt.cur_command_tag == T_VariableShowStmt || \
    t_thrd.postgres_cxt.cur_command_tag == T_DolphinCallStmt)

#define OPT_SQL_MODE_DEFAULT (1 << 0)
#define OPT_SQL_MODE_STRICT (1 << 1)
#define OPT_SQL_MODE_FULL_GROUP (1 << 2)
#define OPT_SQL_MODE_PIPES_AS_CONCAT (1 << 3)
#define OPT_SQL_MODE_ANSI_QUOTES (1 << 4)
#define OPT_SQL_MODE_NO_ZERO_DATE (1 << 5)
#define OPT_SQL_MODE_PAD_CHAR_TO_FULL_LENGTH (1 << 6)
#define OPT_SQL_MODE_BLOCK_RETURN_MULTI_RESULTS (1 << 7)
#define OPT_SQL_MODE_ATUO_RECOMPILE_FUNCTION (1 << 8)
#define OPT_SQL_MODE_MAX 9
#define SQL_MODE_STRICT() ((GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_STRICT) && !CMD_TAG_IS_SELECT())
#define SQL_MODE_FULL_GROUP() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_FULL_GROUP)
#define PG_RETURN_INT8(x) return Int8GetDatum(x)
#define SQL_MODE_PIPES_AS_CONCAT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_PIPES_AS_CONCAT)
#define SQL_MODE_ANSI_QUOTES() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_ANSI_QUOTES)
#define SQL_MODE_NO_ZERO_DATE() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_NO_ZERO_DATE)
#define SQL_MODE_PAD_CHAR_TO_FULL_LENGTH() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_PAD_CHAR_TO_FULL_LENGTH)
#define SQL_MODE_AllOW_PROCEDURE_WITH_SELECT()                                      \
    (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_BLOCK_RETURN_MULTI_RESULTS)
#define SQL_MODE_ATUO_RECOMPILE_FUNCTION() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_ATUO_RECOMPILE_FUNCTION)
#define  GET_QUOTE() (SQL_MODE_ANSI_QUOTES() ? '\"' : '`')

extern int32 PgAtoiInternal(char* s, int size, int c, bool sqlModeStrict, bool can_ignore, bool isUnsigned = false);
extern void CheckSpaceAndDotInternal(char& digitAfterDot, const char** ptr,
                                     bool checkDecimal = true, int endChar = '\0');
extern uint64 pg_getmsguint64(StringInfo msg);
extern void pg_ctoa(int8 i, char* a);
extern int get_step_len(unsigned char ch);

template <bool is_unsigned>
int64 PgStrToIntInternal(const char* s, bool errOk, uint64 max, int64 min, const char* typname)
{
    const char* ptr = s;
    int128 tmp = 0;
    bool neg = false;
    char digitAfterDot = '\0';
    int errlevel = errOk ? WARNING : ERROR;
    const int baseDecimal = 10;

    if (*s == 0) {
        goto invalid_syntax;
    }

    /* skip leading spaces */
    while (likely(*ptr) && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    /* handle sign */
    if (*ptr == '-') {
        ptr++;
        neg = true;
    } else if (*ptr == '+')
        ptr++;

    /* require at least one digit */
    if (unlikely(!isdigit((unsigned char)*ptr))) {
        goto invalid_syntax;
    }

    /* process digits */
    while (*ptr && isdigit((unsigned char)*ptr)) {
        int8 digit = (*ptr++ - '0');
        if (is_unsigned) {
            tmp = tmp * baseDecimal + digit;
            if (tmp > max) {
                goto out_of_range;
            }
        } else {
            tmp = tmp * baseDecimal - digit;
            if (tmp < min || tmp > max) {
                goto out_of_range;
            }
        }
    }

    /* allow trailing whitespace, but not other trailing chars */
    CheckSpaceAndDotInternal(digitAfterDot, &ptr);

    /* could fail if input is most negative number */
    if (is_unsigned) {
        if (neg && tmp > min) {
            goto out_of_range;
        }
    } else if (!neg) {
        if (unlikely(tmp == min)) {
            goto out_of_range;
        }
        tmp = -tmp;
    }

    if ((isdigit(digitAfterDot)) && digitAfterDot >= '5') {
        if (is_unsigned) {
            if (tmp == max) {
                ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                    errmsg("value \"%s\" is out of range for type %s", s, typname)));
            }
            if (!neg && tmp < max) {
                tmp++;
            }
        } else {
            if (tmp == max || tmp == min) {
                ereport(errlevel,
                    (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                        errmsg("value \"%s\" is out of range for type %s", s, typname)));
            }
            if (!neg && tmp < max) {
                tmp++;
            } else if (neg && tmp > min) {
                tmp--;
            }
        }
    }

    /* we check *ptr at last */
    if (unlikely(*ptr != '\0')) {
        goto invalid_syntax;
    }

    return (int64)tmp;

out_of_range:
    ereport(errlevel,
        (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
            errmsg("value \"%s\" is out of range for type %s", s, typname)));
    return neg ? min : (int64)max;

invalid_syntax:
    ereport(errlevel,
        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for %s: \"%s\"", typname, s)));
    return (int64)tmp;
}

#endif /* MYSQLMODE_H */
