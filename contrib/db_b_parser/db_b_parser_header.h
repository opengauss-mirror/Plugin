
#ifndef __DB_B_PARSER_HEADER
#define __DB_B_PARSER_HEADER
#include "parser/gramparse.h"
// extern db b parser function
extern core_yyscan_t db_b_scanner_init(
    const char* str, core_yy_extra_type* yyext, const ScanKeyword* keywords, int num_keywords);
extern int db_b_base_yyparse(core_yyscan_t yyscanner);
extern void db_b_scanner_finish(core_yyscan_t yyscanner);
extern void db_b_parser_init(base_yy_extra_type* yyext);
List* db_b_raw_parser(const char* str, List** query_string_locationlist = NULL);
#endif
