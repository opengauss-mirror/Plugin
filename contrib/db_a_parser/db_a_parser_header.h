
#ifndef __DB_A_PARSER_HEADER
#define __DB_A_PARSER_HEADER
#include "parser/gramparse.h"
// extern db a parser function
extern core_yyscan_t db_a_scanner_init(
    const char* str, core_yy_extra_type* yyext, const ScanKeyword* keywords, int num_keywords);
extern int db_a_base_yyparse(core_yyscan_t yyscanner);
extern void db_a_scanner_finish(core_yyscan_t yyscanner);
extern void db_a_parser_init(base_yy_extra_type* yyext);
List* db_a_raw_parser(const char* str, List** query_string_locationlist = NULL);
#endif