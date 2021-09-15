#include "pl_b.h"
#include "pl_b_sql.h"
#include "parser/parse_type.h"
#include "miscadmin.h"

static void *pre_plsql_parser_hook = NULL;

void init()
{
    ereport(INFO, (errmsg("db_b_extension init")));
    pre_plsql_parser_hook = g_instance.plsql_parser_hook[DB_CMPT_B];
    g_instance.plsql_parser_hook[DB_CMPT_B] = (void *) plbsql_yyparse;
}

void fini()
{
    g_instance.plsql_parser_hook[DB_CMPT_B] = pre_plsql_parser_hook;
}
