/* -------------------------------------------------------------------------
 *
 * parser.cpp
 *        Main entry point/driver for openGauss grammar
 *
 * Note that the grammar is not allowed to perform any table access
 * (since we need to be able to do basic parsing even while inside an
 * aborted transaction).  Therefore, the data structures returned by
 * the grammar are "raw" parsetrees that still need to be analyzed by
 * analyze.c and related files.
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *      src/common/backend/parser/parser.cpp
 *
 * -------------------------------------------------------------------------
 */

#include "postgres.h"
#include "plugin_postgres.h"
#include "storage/proc.h"
#include "knl/knl_variable.h"
#include "nodes/parsenodes.h"

#include "plugin_parser/gramparse.h"
#include "plugin_parser/parser.h"
#include "utils/guc.h"
#include "miscadmin.h"
#include "catalog/pg_namespace.h"
#include "plugin_protocol/proto_com.h"

#ifdef DOLPHIN
#define ASCII_COMMA 44
#define ASCII_DOT 46
#define ASCII_SEMICOLON 59
#endif

extern void resetOperatorPlusFlag();

static bool is_prefer_parse_cursor_parentheses_as_expr()
{
    return PREFER_PARSE_CURSOR_PARENTHESES_AS_EXPR;
}

static bool is_cursor_function_exist()
{
    return get_func_oid("cursor", InvalidOid, NULL) != InvalidOid;
}

static bool is_select_stmt_definitely(int start_token) {
    return start_token == SELECT || start_token == WITH;
}

static void resetIsTimeCapsuleFlag()
{
    u_sess->parser_cxt.isTimeCapsule = false;
}

static void resetHasPartitionComment()
{
    u_sess->parser_cxt.hasPartitionComment = false;
}

static void resetCreateFuncFlag()
{
    u_sess->parser_cxt.isCreateFuncOrProc = false;
}

static void resetForbidTruncateFlag()
{
    u_sess->parser_cxt.isForbidTruncate = false;
}

static void resetHasSetUservarFlag()
{
    u_sess->parser_cxt.has_set_uservar = false;
}

/*
 * raw_parser
 *        Given a query in string form, do lexical and grammatical analysis.
 *
 * Returns a list of raw (un-analyzed) parse trees.
 */
List* raw_parser(const char* str, List** query_string_locationlist)
{
    core_yyscan_t yyscanner;
    base_yy_extra_type yyextra;
    int yyresult;

    /* reset u_sess->parser_cxt.stmt_contains_operator_plus */
    resetOperatorPlusFlag();
    
    /* reset u_sess->parser_cxt.hasPartitionComment */
    resetHasPartitionComment();
    
    /* reset u_sess->parser_cxt.isTimeCapsule */
    resetIsTimeCapsuleFlag();

    /* reset u_sess->parser_cxt.isCreateFuncOrProc */
    resetCreateFuncFlag();

    /* reset u_sess->parser_cxt.isForbidTruncate */
    resetForbidTruncateFlag();

    /* reset u_sess->parser_cxt.has_set_uservar */
    resetHasSetUservarFlag();

    /* initialize the flex scanner */
    yyscanner = scanner_init(str, &yyextra.core_yy_extra, &ScanKeywords, ScanKeywordTokens);

    /* base_yylex() only needs this much initialization */
    yyextra.lookahead_num = 0;

    /* initialize the bison parser */
    parser_init(&yyextra);

    /* Parse! */
    yyresult = base_yyparse(yyscanner);

    /* Clean up (release memory) */
    scanner_finish(yyscanner);

    if (yyresult) { /* error */
        return NIL;
    }

    /* Get the locationlist of multi-query through lex. */
    if (query_string_locationlist != NULL) {
        *query_string_locationlist = yyextra.core_yy_extra.query_string_locationlist;

        /* Deal with the query sent from client without semicolon at the end. */
        if (PointerIsValid(*query_string_locationlist) &&
            (size_t)lfirst_int(list_tail(*query_string_locationlist)) < (strlen(str) - 1)) {
            *query_string_locationlist = lappend_int(*query_string_locationlist, strlen(str));
        }
    }

    if (list_length(yyextra.parsetree) > 1 && GetSessionContext()->Conn_Mysql_Info != NULL &&
        !(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_MULTI_STATEMENTS)) {
        ereport(ERROR, (errmsg("Multi-statement is not allowed. Please connect with allowMultiQueries=true")));
    }

    return yyextra.parsetree;
}

#define GET_NEXT_TOKEN_WITHOUT_YY()                                                      \
    do {                                                                                 \
        if (yyextra->lookahead_num != 0) {                                               \
            next_token = yyextra->lookahead_token[yyextra->lookahead_num - 1];           \
            lvalp->core_yystype = yyextra->lookahead_yylval[yyextra->lookahead_num - 1]; \
            *llocp = yyextra->lookahead_yylloc[yyextra->lookahead_num - 1];              \
            yyextra->lookahead_num--;                                                    \
        } else {                                                                         \
            next_token = core_yylex(&(lvalp->core_yystype), llocp, yyscanner);           \
        }                                                                                \
    } while (0)

#define GET_NEXT_TOKEN()                                                                 \
    do {                                                                                 \
        cur_yylval = lvalp->core_yystype;                                                \
        cur_yylloc = *llocp;                                                             \
        GET_NEXT_TOKEN_WITHOUT_YY();                                                     \
    } while (0)

#define SET_LOOKAHEAD_TOKEN()                               \
    do {                                                    \
        yyextra->lookahead_token[0] = next_token;           \
        yyextra->lookahead_yylval[0] = lvalp->core_yystype; \
        yyextra->lookahead_yylloc[0] = *llocp;              \
        yyextra->lookahead_num = 1;                         \
    } while (0)


#define PARSE_CURSOR_PARENTHESES_AS_EXPR()                   \
    do {                                                     \
        cur_token = CURSOR_EXPR;                             \
        yyextra->lookahead_token[0] = next_token_2;          \
        yyextra->lookahead_yylval[0] = lvalp->core_yystype;  \
        yyextra->lookahead_yylloc[0] = *llocp;               \
        yyextra->lookahead_num = 1;                          \
        lvalp->core_yystype = core_yystype_2;                \
        *llocp = cur_yylloc_2;                               \
    } while (0)


#define PARSE_CURSOR_PARENTHESES_AS_FUNCTION()               \
    do {                                                     \
        yyextra->lookahead_token[1] = next_token_1;          \
        yyextra->lookahead_yylval[1] = core_yystype_2;       \
        yyextra->lookahead_yylloc[1] = cur_yylloc_2;         \
        yyextra->lookahead_token[0] = next_token_2;          \
        yyextra->lookahead_yylval[0] = lvalp->core_yystype;  \
        yyextra->lookahead_yylloc[0] = *llocp;               \
        yyextra->lookahead_num = 2;                          \
        lvalp->core_yystype = core_yystype_1;                \
        *llocp = cur_yylloc_1;                               \
    } while (0)

#define READ_TWO_TOKEN()                                                                                            \
    do {                                                                                                            \
        /*                                                                                                          \
         * get first token.                                                                                         \
         * we should set the yylaval and yylloc back, letting the parser read the cursor name correctly.            \
         * here we may still have token in lookahead_token, so use GET_NEXT_TOKEN to get                            \
         */                                                                                                         \
        GET_NEXT_TOKEN();                                                                                           \
        yyextra->lookahead_token[1] = next_token;                                                                   \
        yyextra->lookahead_yylval[1] = lvalp->core_yystype;                                                         \
        yyextra->lookahead_yylloc[1] = *llocp;                                                                      \
        /*                                                                                                          \
         * get the second token.                                                                                    \
         * in fact we don't have any lookahead_token here for sure, cause MAX_LOOKAHEAD_NUM is 2.                   \
         * but maybe someday MAX_LOOKAHEAD_NUM increase, so we still use GET_NEXT_TOKEN_WITHOUT_SET_CURYY           \
         */                                                                                                         \
        GET_NEXT_TOKEN_WITHOUT_YY();                                                                                \
        yyextra->lookahead_token[0] = next_token;                                                                   \
        yyextra->lookahead_yylval[0] = lvalp->core_yystype;                                                         \
        yyextra->lookahead_yylloc[0] = *llocp;                                                                      \
        yyextra->lookahead_num = 2;                                                                                 \
    } while (0)

#ifdef DOLPHIN
static inline bool IsDescStmtSymbol(int token)
{
    return (token == ASCII_DOT || token == ASCII_SEMICOLON || token == 0);
}

static inline bool IsDescribeStmt(char* scanbuf)
{
    return (scanbuf && (pg_strcasecmp(scanbuf, "desc") == 0 || pg_strcasecmp(scanbuf, "describe") == 0));
}

static inline bool IsExplainStmt(char* scanbuf)
{
    return (scanbuf && pg_strcasecmp(scanbuf, "explain") == 0);
}

static bool IsAlterStmtAlgorithm(char* scanbuf)
{
    if (!scanbuf) {
        return false;
    }

    const int alterSize = 5;
    if (pg_strncasecmp(scanbuf, "alter", alterSize) != 0) {
        return false;
    }

    char* ptr = scanbuf + alterSize;
    while (isspace((unsigned char)*ptr)) {
        ++ptr;
    }

    return (pg_strcasecmp(ptr, "algorithm") == 0);
}
#endif

/*
 * Intermediate filter between parser and core lexer (core_yylex in scan.l).
 *
 * The filter is needed because in some cases the standard SQL grammar
 * requires more than one token lookahead.    We reduce these cases to one-token
 * lookahead by combining tokens here, in order to keep the grammar LALR(1).
 *
 * Using a filter is simpler than trying to recognize multiword tokens
 * directly in scan.l, because we'd have to allow for comments between the
 * words.  Furthermore it's not clear how to do it without re-introducing
 * scanner backtrack, which would cost more performance than this filter
 * layer does.
 *
 * The filter also provides a convenient place to translate between
 * the core_YYSTYPE and YYSTYPE representations (which are really the
 * same thing anyway, but notationally they're different).
 */
int base_yylex(YYSTYPE* lvalp, YYLTYPE* llocp, core_yyscan_t yyscanner)
{
    base_yy_extra_type* yyextra = pg_yyget_extra(yyscanner);
    int cur_token;
    int next_token;
    core_YYSTYPE cur_yylval;
    YYLTYPE cur_yylloc;
    int next_token_1 = 0;
    core_YYSTYPE core_yystype_1;
    YYLTYPE cur_yylloc_1 = 0;
    int next_token_2 = 0;
    core_YYSTYPE core_yystype_2;
    YYLTYPE cur_yylloc_2 = 0;


    /* Get next token --- we might already have it */
    if (yyextra->lookahead_num != 0) {
        cur_token = yyextra->lookahead_token[yyextra->lookahead_num - 1];
        lvalp->core_yystype = yyextra->lookahead_yylval[yyextra->lookahead_num - 1];
        *llocp = yyextra->lookahead_yylloc[yyextra->lookahead_num - 1];
        yyextra->lookahead_num--;
    } else {
        cur_token = core_yylex(&(lvalp->core_yystype), llocp, yyscanner);
    }

    if (u_sess->attr.attr_sql.sql_compatibility == B_FORMAT && yyextra->lookahead_num == 0) {
        bool is_last_colon;
        if (cur_token == int(';')) {
            is_last_colon = true;
        } else {
            is_last_colon = false;
        }
        if (yyextra->core_yy_extra.is_delimiter_name == true) {
            if (strcmp(";",u_sess->attr.attr_common.delimiter_name) == 0) {
                cur_token = END_OF_INPUT_COLON;
            } else {
                if (yyextra->core_yy_extra.is_last_colon == false ) {
                    cur_token = END_OF_INPUT_COLON;
                } else {
                    cur_token = END_OF_INPUT;
                }
            }
        }
        if (yyextra->core_yy_extra.is_proc_end == true) {
            cur_token = END_OF_PROC;
        }
        yyextra->core_yy_extra.is_proc_end = false;
        yyextra->core_yy_extra.is_delimiter_name = false;
        yyextra->core_yy_extra.is_last_colon = is_last_colon;
    }

#ifdef DOLPHIN
    if (GetSessionContext()->is_first_lable == true && cur_token == PARAM) {
        if (pg_strcasecmp(yyextra->core_yy_extra.scanbuf + *llocp, ":loop") == 0) {
            cur_token = LABEL_LOOP;
        } else if (pg_strcasecmp(yyextra->core_yy_extra.scanbuf + *llocp, ":while") == 0) {
            cur_token = LABEL_WHILE;
        } else if (pg_strcasecmp(yyextra->core_yy_extra.scanbuf + *llocp, ":repeat") == 0) {
            cur_token = LABEL_REPEAT;
        }
    }
#endif

    /* Do we need to look ahead for a possible multiword token? */
    switch (cur_token) {
        case NULLS_P:
            /*
             * NULLS FIRST and NULLS LAST must be reduced to one token
             */
            GET_NEXT_TOKEN();
            switch (next_token) {
                case FIRST_P:
                    cur_token = NULLS_FIRST;
                    break;
                case LAST_P:
                    cur_token = NULLS_LAST;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;

        case NOT:
            /*
             * @hdfs
             *  In order to solve the conflict in gram.y, NOT and ENFORCED must be reduced to one token.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case ENFORCED:
                    cur_token = NOT_ENFORCED;
                    break;
                case IN_P:
                    cur_token = NOT_IN;
                    break;
                case BETWEEN:
                    cur_token = NOT_BETWEEN;
                    break;
                case LIKE:
                    cur_token = NOT_LIKE;
                    break;
                case ILIKE:
                    cur_token = NOT_ILIKE;
                    break;
                case SIMILAR:
                    cur_token = NOT_SIMILAR;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();

                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;

        case EVENT:
            /*
             * Event trigger must be reduced to one token
             */
            GET_NEXT_TOKEN();
            switch (next_token) {
                case TRIGGER:
                    cur_token = EVENT_TRIGGER;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();

                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case LOG_P:
            /*
            * For materialized view log, view log must be reduced to one token
            */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case ON:
                    cur_token = LOG_ON;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();

                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case WITH:
            /*
             * WITH TIME must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case TIME:
                    cur_token = WITH_TIME;
                    break;
#ifdef DOLPHIN
                case ROLLUP:
                    cur_token = WITH_ROLLUP;
                    break;
                case PARSER:
                    cur_token = WITH_PARSER;
                    break;
#endif
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();

                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;

        case INCLUDING:

            /*
             * INCLUDING ALL must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case ALL:
                    cur_token = INCLUDING_ALL;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();

                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;

        case RENAME:

            /*
             * RENAME PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case PARTITION:
                    cur_token = RENAME_PARTITION;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;

        case PARTITION:

            /*
             * RENAME PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case FOR:
                    cur_token = PARTITION_FOR;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case SUBPARTITION:

            GET_NEXT_TOKEN();

            switch (next_token) {
                case FOR:
                    cur_token = SUBPARTITION_FOR;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case ADD_P:
            /*
             * ADD PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case PARTITION:
                    cur_token = ADD_PARTITION;
                    break;
                case SUBPARTITION:
                    cur_token = ADD_SUBPARTITION;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;

        case DROP:

            /*
             * DROP PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case PARTITION:
                    cur_token = DROP_PARTITION;
                    break;
                case SUBPARTITION:
                    cur_token = DROP_SUBPARTITION;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case REBUILD:

            /*
             * REBUILD PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case PARTITION:
                    cur_token = REBUILD_PARTITION;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case MODIFY_P:
            /*
             * MODIFY PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case PARTITION:
                    cur_token = MODIFY_PARTITION;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case DECLARE:
            /*
             * DECLARE foo CUROSR must be looked ahead, and if determined as a DECLARE_CURSOR, we should set the yylaval
             * and yylloc back, letting the parser read the cursor name correctly.
             */
            READ_TWO_TOKEN();
            switch (next_token) {
                case CURSOR:
                case BINARY:
                case INSENSITIVE:
                case NO:
                case SCROLL:
                    cur_token = DECLARE_CURSOR;
                    /* and back up the output info to cur_token  because we should read cursor name correctly. */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
                default:
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case VALID:

            /*
             * VALID BEGIN must be reduced to one token, to avoid conflict with BEGIN TRANSACTIOn and BEGIN anonymous
             * block.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case BEGIN_P:
                case BEGIN_NON_ANOYBLOCK:
                    cur_token = VALID_BEGIN;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case START:
            /*
             * START WITH must be reduced to one token, to allow START as table / column alias.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case WITH:
                    cur_token = START_WITH;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case CONNECT:
            /*
             * CONNECT BY must be reduced to one token, to allow CONNECT as table / column alias.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case BY:
                    cur_token = CONNECT_BY;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case ON:
            READ_TWO_TOKEN();
            switch (next_token) {
            case CURRENT_TIMESTAMP:
            case CURRENT_TIME:
            case CURRENT_DATE:
            case LOCALTIME:
            case LOCALTIMESTAMP:
                cur_token = ON_UPDATE_TIME;
                lvalp->core_yystype = cur_yylval;
                *llocp = cur_yylloc;
                break;
            default:
                /* and back up the output info to cur_token */
                lvalp->core_yystype = cur_yylval;
                *llocp = cur_yylloc;
                break;
            }
            break;
#ifdef DOLPHIN
        case SHOW:
            /*
             * SHOW ERRORS must be reduced to one token, to allow ERRORS as table / column alias.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case ERRORS:
                    cur_token = SHOW_ERRORS;
                    break;
                case STATUS:
                    cur_token = SHOW_STATUS;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        
            case FULL:
            /*
             * FULL OUTER must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case OUTER_P:
                    cur_token = FULL_OUTER;
                    break;
                case JOIN:
                    cur_token = FULL_OUTER;
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
#endif
        case USE:
        /*
         * USE INDEX \USE KEY must be reduced to one token,to allow KEY\USE as table / column alias.
         */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case KEY:
                    cur_token = USE_INDEX;
                    break;
                case INDEX:
                    cur_token = USE_INDEX;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case FORCE:
        /*
         * FORCE INDEX \FORCE KEY must be reduced to one token,to allow KEY\FORCE as table / column alias.
         */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case KEY:
                    cur_token = FORCE_INDEX;
                    break;
                case INDEX:
                    cur_token = FORCE_INDEX;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case IGNORE:
        /*
         * IGNORE INDEX \IGNORE KEY must be reduced to one token,to allow KEY\IGNORE as table / column alias.
         */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case KEY:
                case INDEX:
                    cur_token = IGNORE_INDEX;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case CURSOR:
            GET_NEXT_TOKEN();
            core_yystype_1 = cur_yylval;  // the value of cursor
            cur_yylloc_1 = cur_yylloc;    // the lloc of cursor
            next_token_1 = next_token;    // the token after curosr
            if (next_token_1 != '(') {
                /* save the lookahead token for next time */
                SET_LOOKAHEAD_TOKEN();
                /* and back up the output info to cur_token */
                lvalp->core_yystype = cur_yylval;
                *llocp = cur_yylloc;
            } else {
                GET_NEXT_TOKEN();
                core_yystype_2 = cur_yylval;   // the value after cursor
                cur_yylloc_2 = cur_yylloc;    // the lloc after cursor
                next_token_2 = next_token;   // the token after after curosr

                if (next_token_1 == '(' && (is_select_stmt_definitely(next_token))) {
                    PARSE_CURSOR_PARENTHESES_AS_EXPR();
                } else if (is_prefer_parse_cursor_parentheses_as_expr() && !is_cursor_function_exist()) {
                    PARSE_CURSOR_PARENTHESES_AS_EXPR();
                } else {
                    PARSE_CURSOR_PARENTHESES_AS_FUNCTION();
                }
                if (t_thrd.proc->workingVersionNum < CURSOR_EXPRESSION_VERSION_NUMBER &&
                    cur_token == CURSOR_EXPR) {
                        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("Unsupported feature: cursor expression during the upgrade")));
                }
            }
            break;
        case STATIC_P:
            GET_NEXT_TOKEN();
            switch (next_token) {
                /* static function/procedure */
                case FUNCTION:
                    cur_token = STATIC_FUNCTION;
                    break;
                case PROCEDURE:
                    cur_token = STATIC_PROCEDURE;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case MEMBER:
            GET_NEXT_TOKEN();
            switch (next_token) {
                /* MEMBER function/procedure */
                case FUNCTION:
                    cur_token = MEMBER_FUNCTION;
                    break;
                case PROCEDURE:
                    cur_token = MEMBER_PROCEDURE;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case CONSTRUCTOR:
            GET_NEXT_TOKEN();
            switch (next_token) {
                /* MEMBER function/procedure */
                case FUNCTION:
                    cur_token = CONSTRUCTOR_FUNCTION;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case MAP:
            GET_NEXT_TOKEN();
            switch (next_token) {
                case MEMBER:
                    cur_token = MAP_MEMBER;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case SELF:
            GET_NEXT_TOKEN();
            switch (next_token) {
                case INOUT:
                    cur_token = SELF_INOUT;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
#ifdef DOLPHIN

        case EXPLAIN:
            if (IsExplainStmt(yyextra->core_yy_extra.scanbuf)) {
                READ_TWO_TOKEN();
                if (IsDescStmtSymbol(next_token)) {
                    cur_token = DESC;
                }
                lvalp->core_yystype = cur_yylval;
                *llocp = cur_yylloc;
            }
            break;
        case DESC:
        case DESCRIBE:
            if (IsDescribeStmt(yyextra->core_yy_extra.scanbuf)) {
                READ_TWO_TOKEN();
                if (!IsDescStmtSymbol(next_token)) {
                    cur_token = EXPLAIN;
                }
                lvalp->core_yystype = cur_yylval;
                *llocp = cur_yylloc;
            }
            break;
        case DEFAULT:
            if (GetSessionContext()->is_create_alter_stmt) {
                break;
            }
            /*
             * DEFAULT must be reduced to one token, to allow START as table / column alias.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case '(':
                    cur_token = DEFAULT_FUNC;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
       case DO:
            /* parse DO Sconst To one Token */
           GET_NEXT_TOKEN();
           switch (next_token) {
                case SCONST:
                    cur_token = DO_SCONST;
                    break;
                case LANGUAGE:
                    cur_token = DO_LANGUAGE;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case ALGORITHM:
            if (!IsAlterStmtAlgorithm(yyextra->core_yy_extra.scanbuf)) {
                break;
            }

            READ_TWO_TOKEN();
            if (yyextra->lookahead_token[1] != '=') {
                lvalp->core_yystype = yyextra->lookahead_yylval[1];
                *llocp = yyextra->lookahead_yylloc[1];
                break;
            }

            switch (next_token) {
                case UNDEFINED:
                    cur_token = ALGORITHM_UNDEFINED;
                    yyextra->lookahead_num = 0;
                    break;
                case MERGE:
                    cur_token = ALGORITHM_MERGE;
                    yyextra->lookahead_num = 0;
                    break;
                case TEMPTABLE:
                    cur_token = ALGORITHM_TEMPTABLE;
                    yyextra->lookahead_num = 0;
                    break;
                default:
                    break;
            }
            break;
        case BEGIN_P:
            if (GetSessionContext()->dolphin_kw_mask == B_KWMASK_CREATE_TRIGGER) {
                cur_token = BEGIN_B_BLOCK;
                GetSessionContext()->dolphin_kw_mask = 0;
            }
            break;
        case LOCK_P:
            /*
             * LOCK TABLES must be reduced to one token.
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case TABLES:
                    cur_token = LOCK_TABLES;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case MATCH:
            /*
             * DEFAULT must be reduced to one token, to allow START as table / column alias.
             */
            GET_NEXT_TOKEN();
            switch (next_token) {
                case '(':
                    cur_token = MATCH_FUNC;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case CREATE:
        case ALTER:
            if (pg_strncasecmp(yyextra->core_yy_extra.scanbuf, "alter", strlen("alter")) == 0 ||
                pg_strncasecmp(yyextra->core_yy_extra.scanbuf, "create", strlen("create")) == 0) {
                GetSessionContext()->is_create_alter_stmt = true;
            }
            break;
        case SELECT:
            GetSessionContext()->is_create_alter_stmt = false;
            break;
        case STORAGE:
            /*
             * STORAGE DISK/MEMORY must be reduced to one token, to allow STORAGE(...).
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case DISK:
                    cur_token = STORAGE_DISK;
                    break;
                case MEMORY:
                    cur_token = STORAGE_MEMORY;
                    break;
                default:
                    /* save the lookahead token for next time */
                    SET_LOOKAHEAD_TOKEN();
                    /* and back up the output info to cur_token */
                    lvalp->core_yystype = cur_yylval;
                    *llocp = cur_yylloc;
                    break;
            }
            break;
        case (INTERVAL):
            READ_TWO_TOKEN();
            switch (yyextra->lookahead_token[1]) {
                case SCONST:
                    switch (yyextra->lookahead_token[0]) {
                        case YEAR_P:
                        case QUARTER_P:
                        case MONTH_P:
                        case DAY_P:
                        case HOUR_P:
                        case MINUTE_P:
                        case SECOND_P:
                        case MICROSECOND_P:
                        case WEEK_P:
                        case YEAR_MONTH_P:
                        case DAY_HOUR_P:
                        case DAY_MINUTE_P:
                        case DAY_SECOND_P:
                        case DAY_MICROSECOND_P:
                        case HOUR_MINUTE_P:
                        case HOUR_SECOND_P:
                        case HOUR_MICROSECOND_P:
                        case MINUTE_SECOND_P:
                        case MINUTE_MICROSECOND_P:
                        case SECOND_MICROSECOND_P:
                            break;
                        default:
                            // replace token INTERVAL to INTERVAL_S
                            cur_token = INTERVAL_S;
                            break;
                    }
                    break;
                case YEAR_P:
                    cur_token = INTERVAL_TYPE_YEAR;
                    // combine token INTERVAL and unit
                    yyextra->lookahead_num = 1;
                    break;
                case QUARTER_P:
                    cur_token = INTERVAL_TYPE_QUARTER;
                    yyextra->lookahead_num = 1;
                    break;
                case MONTH_P:
                    cur_token = INTERVAL_TYPE_MONTH;
                    yyextra->lookahead_num = 1;
                    break;
                case DAY_P:
                    cur_token = INTERVAL_TYPE_DAY;
                    yyextra->lookahead_num = 1;
                    break;
                case HOUR_P:
                    cur_token = INTERVAL_TYPE_HOUR;
                    yyextra->lookahead_num = 1;
                    break;
                case MINUTE_P:
                    cur_token = INTERVAL_TYPE_MINUTE;
                    yyextra->lookahead_num = 1;
                    break;
                case SECOND_P:
                    cur_token = INTERVAL_TYPE_SECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case MICROSECOND_P:
                    cur_token = INTERVAL_TYPE_MICROSECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case WEEK_P:
                    cur_token = INTERVAL_TYPE_WEEK;
                    yyextra->lookahead_num = 1;
                    break;
                case YEAR_MONTH_P:
                    cur_token = INTERVAL_TYPE_YEAR_MONTH;
                    yyextra->lookahead_num = 1;
                    break;
                case DAY_HOUR_P:
                    cur_token = INTERVAL_TYPE_DAY_HOUR;
                    yyextra->lookahead_num = 1;
                    break;
                case DAY_MINUTE_P:
                    cur_token = INTERVAL_TYPE_DAY_MINUTE;
                    yyextra->lookahead_num = 1;
                    break;
                case DAY_SECOND_P:
                    cur_token = INTERVAL_TYPE_DAY_SECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case DAY_MICROSECOND_P:
                    cur_token = INTERVAL_TYPE_DAY_MICROSECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case HOUR_MINUTE_P:
                    cur_token = INTERVAL_TYPE_HOUR_MINUTE;
                    yyextra->lookahead_num = 1;
                    break;
                case HOUR_SECOND_P:
                    cur_token = INTERVAL_TYPE_HOUR_SECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case HOUR_MICROSECOND_P:
                    cur_token = INTERVAL_TYPE_HOUR_MICROSECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case MINUTE_SECOND_P:
                    cur_token = INTERVAL_TYPE_MINUTE_SECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case MINUTE_MICROSECOND_P:
                    cur_token = INTERVAL_TYPE_MINUTE_MICROSECOND;
                    yyextra->lookahead_num = 1;
                    break;
                case SECOND_MICROSECOND_P:
                    cur_token = INTERVAL_TYPE_SECOND_MICROSECOND;
                    yyextra->lookahead_num = 1;
                    break;
                default:
                    break;
            }
            /* back up the output info to cur_token */
            lvalp->core_yystype = cur_yylval;
            *llocp = cur_yylloc;
            break;
#endif
        default:
            break;
    }

    return cur_token;
}

/*
 * @Description: Check whether its a empty query with only comments and semicolon.
 * @Param query_string: the query need check.
 * @return:true or false.
 */
static bool is_empty_query(char* query_string)
{
    char begin_comment[3] = "/*";
    char end_comment[3] = "*/";
    char empty_query[2] = ";";
    char* end_comment_position = NULL;

    /* Trim all the spaces at the begin of the string. */
    while (isspace((unsigned char)*query_string)) {
        query_string++;
    }

    /* Trim all the comments of the query_string from the front. */
    while (strncmp(query_string, begin_comment, 2) == 0) {
        /*
         * As query_string have been through parser, whenever it contain the begin_comment
         * it will contain the end_comment and end_comment_position can't be null here.
         */
        end_comment_position = strstr(query_string, end_comment);
        query_string = end_comment_position + 2;
        while (isspace((unsigned char)*query_string)) {
            query_string++;
        }
    }

    /* Check whether query_string is a empty query. */
    if (strcmp(query_string, empty_query) == 0) {
        return true;
    } else {
        return false;
    }
}

/*
 * @Description: split the query_string to distinct single queries.
 * @Param [IN] query_string_single: store the splited single queries.
 * @Param [IN] query_string: initial query string which contain multi statements.
 * @Param [IN] query_string_locationList: record single query terminator-semicolon locations which get from lexer.
 * @Param [IN] stmt_num: show this is the n-ths single query of the multi query.
 * @return [IN/OUT] query_string_single: store the point array of single query.
 * @NOTICE:The caller is responsible for freeing the storage palloced here.
 */
char** get_next_snippet(
    char** query_string_single, const char* query_string, List* query_string_locationlist, int* stmt_num)
{
    int query_string_location_start = 0;
    int query_string_location_end = -1;
    char* query_string_single_p = NULL;
    int single_query_string_len = 0;

    int stmt_count = list_length(query_string_locationlist);

    /* Malloc memory for single query here just for the first time. */
    if (query_string_single == NULL) {
        query_string_single = (char**)palloc0(sizeof(char*) * stmt_count);
    }

    /*
     * Get the snippet of multi_query until we get a non-empty query as the empty query string
     * needn't be dealt with.
     */
    for (; *stmt_num < stmt_count;) {
        /*
         * Notice : The locationlist only store the end position of each single query but not any
         * start position.
         */
        if (*stmt_num == 0) {
            query_string_location_start = 0;
        } else {
            query_string_location_start = list_nth_int(query_string_locationlist, *stmt_num - 1) + 1;
        }
        query_string_location_end = list_nth_int(query_string_locationlist, (*stmt_num)++);

        /* Malloc memory for each single query string. */
        single_query_string_len = query_string_location_end - query_string_location_start + 1;
        query_string_single[*stmt_num - 1] = (char*)palloc0(sizeof(char) * (single_query_string_len + 1));

        /* Copy the query_string between location_start and location_end to query_string_single. */
        query_string_single_p = query_string_single[*stmt_num - 1];
        while (query_string_location_start <= query_string_location_end) {
            *query_string_single_p = *(query_string + query_string_location_start);
            query_string_location_start++;
            query_string_single_p++;
        }

        /*
         * If query_string_single is empty query which only contain comments or null strings,
         * we will skip it.
         */
        if (is_empty_query(query_string_single[*stmt_num - 1])) {
            continue;
        } else {
            break;
        }
    }

    return query_string_single;
}

const struct ignore_keyword_opt_data ignore_keywords[] = {
    {"interval", INTERVAL},
    {"binary_double_infinity", BINARY_DOUBLE_INF},
    {"binary_double_nan", BINARY_DOUBLE_NAN},
    {"straight_join", STRAIGHT_JOIN},
    {"binary", BINARY},
    {"prior", PRIOR}
};
#define INGORE_KEYWORDS_LEN (sizeof(ignore_keywords) / sizeof(struct ignore_keyword_opt_data))

/*
 * @Description: Avoid hooks, privides public interface, this can select ignore_keywords_list based on database
 * compatibility to find ignore_keywords.
 * @Param [IN] item: keyword
 * @return token: token value for import item in ignore_keywords_list, if not in ignore_keywords_list, reutrn -1.
 */
int16 semtc_get_ignore_keyword_token(const char *item)
{
    const struct ignore_keyword_opt_data *ignore_keywords_list = ignore_keywords;
    int ignore_keywords_length = (int)INGORE_KEYWORDS_LEN;

    for (int i = 0; i < ignore_keywords_length; i++) {
        if (strcmp(item, ignore_keywords_list[i].option_name) == 0) {
            return ignore_keywords_list[i].token;
        }
    }
    return -1;
}

bool semtc_is_token_in_ignore_keyword_list(int token, bool isPlpgsqlKeyword)
{
    ListCell *cell = NULL;
    List *keyword_list = isPlpgsqlKeyword ? NULL : u_sess->utils_cxt.ignore_keyword_list;
    foreach(cell, keyword_list)
    {
        int ignore_token = lfirst_int(cell);
        if (token == ignore_token) {
            return true;
        }
    }
    return false;
}

