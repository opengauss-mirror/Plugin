#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define PG_RETURN_INT8(x) return Int8GetDatum(x)
#define CMD_TAG_IS_SELECT() (t_thrd.postgres_cxt.cur_command_tag == T_SelectStmt || \
    t_thrd.postgres_cxt.cur_command_tag == T_ShowEventStmt || \
    t_thrd.postgres_cxt.cur_command_tag == T_VariableShowStmt)

#define GET_QUOTE() (SQL_MODE_ANSI_QUOTES() ? '\"' : '`')
#define OPT_SQL_MODE_DEFAULT (1 << 0)
#define OPT_SQL_MODE_STRICT (1 << 1)
#define OPT_SQL_MODE_FULL_GROUP (1 << 2)
#define OPT_SQL_MODE_PIPES_AS_CONCAT (1 << 3)
#define OPT_SQL_MODE_ANSI_QUOTES (1 << 4)
#define OPT_SQL_MODE_NO_ZERO_DATE (1 << 5)
#define OPT_SQL_MODE_PAD_CHAR_TO_FULL_LENGTH (1 << 6)
#define OPT_SQL_MODE_BLOCK_RETURN_MULTI_RESULTS (1 << 7)
#define OPT_SQL_MODE_ATUO_RECOMPILE_FUNCTION (1 << 8)
#define OPT_SQL_MODE_ERROR_FOR_DIVISION_BY_ZERO (1 << 9)
#define OPT_SQL_MODE_MAX 10
#define SQL_MODE_STRICT() ((GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_STRICT) && !CMD_TAG_IS_SELECT())
#define SQL_MODE_FULL_GROUP() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_FULL_GROUP)
#define SQL_MODE_PIPES_AS_CONCAT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_PIPES_AS_CONCAT)
#define SQL_MODE_ANSI_QUOTES() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_ANSI_QUOTES)
#define SQL_MODE_NO_ZERO_DATE() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_NO_ZERO_DATE)
#define SQL_MODE_PAD_CHAR_TO_FULL_LENGTH() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_PAD_CHAR_TO_FULL_LENGTH)
#define SQL_MODE_AllOW_PROCEDURE_WITH_SELECT()                                      \
    (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_BLOCK_RETURN_MULTI_RESULTS)
#define SQL_MODE_ATUO_RECOMPILE_FUNCTION() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_ATUO_RECOMPILE_FUNCTION)
#define SQL_MODE_ERROR_FOR_DIVISION_BY_ZERO() (GetSessionContext()->sqlModeFlags & \
    OPT_SQL_MODE_ERROR_FOR_DIVISION_BY_ZERO)

extern int32 PgAtoiInternal(char* s, int size, int c, bool sqlModeStrict, bool can_ignore, bool isUnsigned = false);
extern int16 PgStrtoint16Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern int32 PgStrtoint32Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern uint16 PgStrtouint16Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern uint32 PgStrtouint32Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern bool Scanint8Internal(const char* str, bool errorOK, int64* result, bool sqlModeStrict, bool can_ignore);
extern void CheckSpaceAndDotInternal(char& digitAfterDot, const char** ptr,
                                     bool checkDecimal = true, int endChar = '\0');
extern uint64 pg_getmsguint64(StringInfo msg);
extern void pg_ctoa(int8 i, char* a);
extern int get_step_len(unsigned char ch);

extern inline void CheckErrDivByZero(bool ignore)
{
    if (!SQL_MODE_ERROR_FOR_DIVISION_BY_ZERO()) {
        return;
    }
    ereport((!ignore && SQL_MODE_STRICT()) ? ERROR : WARNING,
        (errcode(ERRCODE_DIVISION_BY_ZERO), errmsg("division by zero")));
}
#endif /* MYSQLMODE_H */
