#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define OPT_SQL_MODE_DEFAULT 1
#define OPT_SQL_MODE_STRICT 2
#define OPT_SQL_MODE_MAX 2
#define SQL_MODE_STRICT() ((u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) || (GetSessionContext()->sqlModeFlags & OPT_SQL_MODE_STRICT))

#define VARCHARTOINT1FUNCOID 4098
#define VARCHARTOINT2FUNCOID 4099
#define BPCHARTOINT1FUNCOID 4085
#define BPCHARTOINT2FUNCOID 4097

extern int32 PgAtoiInternal(char* s, int size, int c, bool sqlModeStrict);
extern int16 PgStrtoint16Internal(const char* s, bool sqlModeStrict);
extern int32 PgStrtoint32Internal(const char* s, bool sqlModeStrict);
extern bool Scanint8Internal(const char* str, bool errorOK, int64* result, bool sqlModeStrict);

#endif /* MYSQLMODE_H */