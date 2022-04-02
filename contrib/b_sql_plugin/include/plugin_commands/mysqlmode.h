#ifndef MYSQLMODE_H
#define MYSQLMODE_H

#include "plugin_postgres.h"
#include "utils/builtins.h"

#define OPT_SQL_MODE_DEFAULT 1
#define OPT_SQL_MODE_STRICT 2
#define OPT_SQL_MODE_MAX 2
#define SQL_MODE_STRICT() ((u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) || (GetSessionContext()->sql_mode_flags & OPT_SQL_MODE_STRICT))

#define VARCHARTOINT1FUNCOID 4098
#define VARCHARTOINT2FUNCOID 4099
#define BPCHARTOINT1FUNCOID 4085
#define BPCHARTOINT2FUNCOID 4097

extern int32 pg_atoi_internal(char* s, int size, int c, bool sql_mode_strict);
extern int16 pg_strtoint16_internal(const char* s, bool sql_mode_strict);
extern int32 pg_strtoint32_internal(const char* s, bool sql_mode_strict);
extern bool scanint8_internal(const char* str, bool errorOK, int64* result, bool sql_mode_strict);

#endif /* MYSQLMODE_H */