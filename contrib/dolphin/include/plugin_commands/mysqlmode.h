#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define OPT_SQL_MODE_DEFAULT 1
#define OPT_SQL_MODE_STRICT 2
#define OPT_SQL_MODE_FULL_GROUP 4
#define OPT_SQL_MODE_PIPES_AS_CONCAT 8
#define OPT_SQL_MODE_ANSI_QUOTES 16
#define OPT_SQL_MODE_NO_ZERO_DATE 32
#define OPT_SQL_MODE_MAX 6
#define SQL_MODE_STRICT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_STRICT)
#define SQL_MODE_FULL_GROUP() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_FULL_GROUP)
#define PG_RETURN_INT8(x) return Int8GetDatum(x)
#define SQL_MODE_PIPES_AS_CONCAT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_PIPES_AS_CONCAT)
#define SQL_MODE_ANSI_QUOTES() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_ANSI_QUOTES)
#define SQL_MODE_NO_ZERO_DATE() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_NO_ZERO_DATE)

extern int32 PgAtoiInternal(char* s, int size, int c, bool sqlModeStrict, bool can_ignore, bool isUnsigned = false);
extern int16 PgStrtoint16Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern int32 PgStrtoint32Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern uint16 PgStrtouint16Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern uint32 PgStrtouint32Internal(const char* s, bool sqlModeStrict, bool can_ignore);
extern bool Scanint8Internal(const char* str, bool errorOK, int64* result, bool sqlModeStrict, bool can_ignore);
extern void CheckSpaceAndDotInternal(bool errorOK, int c, char* digitAfterDot, const char** ptr);
extern uint64 pg_getmsguint64(StringInfo msg);
extern void pg_ctoa(int8 i, char* a);
extern int get_step_len(unsigned char ch);
#endif /* MYSQLMODE_H */
