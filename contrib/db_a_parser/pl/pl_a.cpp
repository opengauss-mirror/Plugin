#include "pl_a.h"
#include "pl_a_sql.h"
#include "parser/parse_type.h"
#include "miscadmin.h"

static void *pre_plsql_parser_hook = NULL;

void init()
{
    ereport(INFO, (errmsg("pl_a_extension init")));
    pre_plsql_parser_hook = g_instance.plsql_parser_hook[DB_CMPT_A];
    g_instance.plsql_parser_hook[DB_CMPT_A] = (void *) plasql_yyparse;
}

void fini()
{
    g_instance.plsql_parser_hook[DB_CMPT_A] = pre_plsql_parser_hook;
}
