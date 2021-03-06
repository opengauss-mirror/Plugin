#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define OPT_SQL_MODE_DEFAULT 1
#define OPT_SQL_MODE_STRICT 2
#define OPT_SQL_MODE_FULL_GROUP 4
#define OPT_SQL_MODE_MAX 3
#define SQL_MODE_STRICT() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_STRICT)
#define SQL_MODE_FULL_GROUP() (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_FULL_GROUP)

extern int32 PgAtoiInternal(char* s, int size, int c, bool sqlModeStrict);
extern int16 PgStrtoint16Internal(const char* s, bool sqlModeStrict);
extern int32 PgStrtoint32Internal(const char* s, bool sqlModeStrict);
extern bool Scanint8Internal(const char* str, bool errorOK, int64* result, bool sqlModeStrict);
extern void CheckSpaceAndDotInternal(bool errorOK, int c, char* digitAfterDot, const char** ptr);

#endif /* MYSQLMODE_H */
