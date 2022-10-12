#include "postgres.h"
#include "commands/dbcommands.h"
#include "nodes/makefuncs.h"
#include "utils/lsyscache.h"
#include "utils/builtins.h"
#include "plugin_parser/parse_show.h"

SelectStmt* makeFlushBinaryLogsQuery()
{
    Node* func = plpsMakeTargetNode(plpsMakeFunc("pg_switch_xlog", NULL));
    List* tl = (List*)list_make1(func);

    SelectStmt* stmt = plpsMakeSelectStmt(tl, NULL, NULL, NULL);
    return stmt;
}
