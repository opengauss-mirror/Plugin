#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define OPT_SQL_MODE_DEFAULT 1
#define OPT_SQL_MODE_STRICT 2
#define OPT_SQL_MODE_FULL_GROUP 4
#define OPT_SQL_MODE_PIPES_AS_CONCAT 8
#define OPT_SQL_MODE_MAX 4
#define SQL_MODE_STRICT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_STRICT)
#define SQL_MODE_FULL_GROUP() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_FULL_GROUP)
#define SQL_MODE_PIPES_AS_CONCAT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_PIPES_AS_CONCAT)

extern int32 PgAtoiInternal(char* s, int size, int c, bool sqlModeStrict);
extern int16 PgStrtoint16Internal(const char* s, bool sqlModeStrict);
extern int32 PgStrtoint32Internal(const char* s, bool sqlModeStrict);
extern uint16 PgStrtouint16Internal(const char* s, bool sqlModeStrict);
extern uint32 PgStrtouint32Internal(const char* s, bool sqlModeStrict);
extern bool Scanint8Internal(const char* str, bool errorOK, int64* result, bool sqlModeStrict);
extern void CheckSpaceAndDotInternal(bool errorOK, int c, char* digitAfterDot, const char** ptr);
extern uint64 pg_getmsguint64(StringInfo msg);
#endif /* MYSQLMODE_H */
