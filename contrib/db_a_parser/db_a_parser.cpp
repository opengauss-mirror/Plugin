/* -------------------------------------------------------------------------
 *
 * parser.cpp
 *        Main entry point/driver for PostgreSQL grammar
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

#include "fmgr.h"
#include "postgres.h"
#include "pl/pl_a.h"
#include "knl/knl_variable.h"
#include "nodes/parsenodes.h"

#include "parser/scanner.h"
#include "db_a_gram.hpp"
#include "parser/gramparse.h"
#include "parser/analyze.h"
#include "parser/parser.h"
#include "db_a_parser_header.h"
#include "db_a_keywords.h"
#include "miscadmin.h"
#include <unistd.h>

PG_MODULE_MAGIC;

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);

#define MAXARGLEN 1024
char transform_data_type[MAXARGLEN] = "long:text,long raw:bytea,nclob:text,bfile:bytea,urowid:text,rowid:oid,binary_float:float8,"
                                      "xmltype:xml,pls_integer:int4,TIMESTAMP WITH LOCAL TIME ZONE:TIMESTAMP WITH TIME ZONE";

static void* pre_raw_parser;
static void read_conf_file (const char* path);
extern "C" void db_a_parser_invoke(void);
PG_FUNCTION_INFO_V1(db_a_parser_invoke);
void db_a_parser_invoke(void) {
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void _PG_init(void) {
    if (!u_sess->misc_cxt.process_shared_preload_libraries_in_progress && !DB_IS_CMPT(A_FORMAT)) {
        ereport(ERROR, (errmsg("Can't create db_a_parser extension since current database compatibility is not 'A'")));
    }

    char *conf_path = "db_a_parser.conf";
    read_conf_file(conf_path);

    ereport(INFO, (errmsg("db_a parser extension init, this parser is not enabled until enable_custom_parser is to true")));
    pre_raw_parser = g_instance.raw_parser_hook[DB_CMPT_A];
    g_instance.raw_parser_hook[DB_CMPT_A] = (void*)db_a_raw_parser;
    init();
}

void _PG_fini(void) {
    g_instance.raw_parser_hook[DB_CMPT_A] = pre_raw_parser;
    fini();
}



extern void resetOperatorPlusFlag();

/*
 * raw_parser
 *        Given a query in string form, do lexical and grammatical analysis.
 *
 * Returns a list of raw (un-analyzed) parse trees.
 */
List* db_a_raw_parser(const char* str, List** query_string_locationlist)
{
    core_yyscan_t yyscanner;
    base_yy_extra_type yyextra;
    int yyresult;

    /* reset u_sess->parser_cxt.stmt_contains_operator_plus */
    resetOperatorPlusFlag();

    /* initialize the flex scanner */
    yyscanner = db_a_scanner_init(str, &yyextra.core_yy_extra, DbAScanKeywords, DbANumScanKeywords);

    /* base_yylex() only needs this much initialization */
    yyextra.lookahead_num = 0;

    /* initialize the bison parser */
    db_a_parser_init(&yyextra);

    /* Parse! */
    yyresult = db_a_base_yyparse(yyscanner);

    /* Clean up (release memory) */
    db_a_scanner_finish(yyscanner);

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

    return yyextra.parsetree;
}

#define GET_NEXT_TOKEN()                                                                 \
    do {                                                                                 \
        cur_yylval = lvalp->core_yystype;                                                \
        cur_yylloc = *llocp;                                                             \
        if (yyextra->lookahead_num != 0) {                                               \
            next_token = yyextra->lookahead_token[yyextra->lookahead_num - 1];           \
            lvalp->core_yystype = yyextra->lookahead_yylval[yyextra->lookahead_num - 1]; \
            *llocp = yyextra->lookahead_yylloc[yyextra->lookahead_num - 1];              \
            yyextra->lookahead_num--;                                                    \
            Assert(yyextra->lookahead_num == 0);                                         \
        } else {                                                                         \
            next_token = core_yylex(&(lvalp->core_yystype), llocp, yyscanner);           \
        }                                                                                \
    } while (0)

#define SET_LOOKAHEAD_TOKEN()                               \
    do {                                                    \
        yyextra->lookahead_token[0] = next_token;           \
        yyextra->lookahead_yylval[0] = lvalp->core_yystype; \
        yyextra->lookahead_yylloc[0] = *llocp;              \
        yyextra->lookahead_num = 1;                         \
    } while (0)

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
int db_a_base_yylex(YYSTYPE* lvalp, YYLTYPE* llocp, core_yyscan_t yyscanner)
{
    base_yy_extra_type* yyextra = pg_yyget_extra(yyscanner);
    int cur_token;
    int next_token;
    core_YYSTYPE cur_yylval;
    YYLTYPE cur_yylloc;

    /* Get next token --- we might already have it */
    if (yyextra->lookahead_num != 0) {
        cur_token = yyextra->lookahead_token[yyextra->lookahead_num - 1];
        lvalp->core_yystype = yyextra->lookahead_yylval[yyextra->lookahead_num - 1];
        *llocp = yyextra->lookahead_yylloc[yyextra->lookahead_num - 1];
        yyextra->lookahead_num--;
    } else {
        cur_token = core_yylex(&(lvalp->core_yystype), llocp, yyscanner);
    }

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
                case LOCAL:
                    cur_token = WITH_LOCAL;
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
        case ADD_P:
            /*
             * ADD PARTITION must be reduced to one token
             */
            GET_NEXT_TOKEN();

            switch (next_token) {
                case PARTITION:
                    cur_token = ADD_PARTITION;
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
            cur_yylval = lvalp->core_yystype;
            cur_yylloc = *llocp;
            next_token = core_yylex(&(lvalp->core_yystype), llocp, yyscanner);
            /* get first token after DECLARE. We don't care what it is */
            yyextra->lookahead_token[1] = next_token;
            yyextra->lookahead_yylval[1] = lvalp->core_yystype;
            yyextra->lookahead_yylloc[1] = *llocp;

            /* get the second token after DECLARE. If it is cursor grammer, we are sure that this is a cursr stmt */
            next_token = core_yylex(&(lvalp->core_yystype), llocp, yyscanner);
            yyextra->lookahead_token[0] = next_token;
            yyextra->lookahead_yylval[0] = lvalp->core_yystype;
            yyextra->lookahead_yylloc[0] = *llocp;
            yyextra->lookahead_num = 2;

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

        default:
            break;
    }

    return cur_token;
}

static void read_conf_file (const char* path) {
    FILE* infile = NULL;
    int c;
    int linelen = 0;
    int maxlength = 1;
    char* buffer = NULL;
    char* p = NULL;
    char* q = NULL;
    int rc;
    if ((infile = fopen(path, "r")) == NULL) {
        ereport(WARNING, (errmsg("could not open file for reading: %s\n", path)));
        return;
    }
    while ((c = fgetc(infile)) != EOF) {
        linelen++;
        if (c == '\n') {
            if (linelen > maxlength) {
                maxlength = linelen;
            }
            linelen = 0;
        }
    }

    /* handle last line without a terminating newline */
    if (linelen > maxlength) {
        maxlength = linelen;
    }

    if (maxlength <= 1) {
        fclose(infile);
        infile = NULL;
        return;
    }

    buffer = (char*)palloc0(maxlength + 1);
    rewind(infile);

    while (fgets(buffer, maxlength + 1, infile) != NULL) {
        p = buffer;
        /* Skip all the blanks at the begin of the optLine */
        while (isspace((unsigned char)*p)) {
            p++;
        }

        if ('#' == *p) {
            continue;
        }

        if (strncmp(p, "data_type", strlen("data_type")) == 0) {
            p += strlen("data_type");
            while (isspace((unsigned char)*p)) {
                p++;
            }
            if (*p != '=') {
                continue;
            }
            p++;
            /* Skip all the blanks after '=' and before the value */
            while (isspace((unsigned char)*p)) {
                p++;
            }
            if (*p != '\'') {
                continue;
            }
            for (q = p + 1; *q && '\n' != *q && '#' != *q && '\'' != *q; q++);
            p++;
            rc = strncpy_s(transform_data_type, MAXARGLEN, p, q - p);
            securec_check_c(rc, "\0", "\0");
            transform_data_type[q - p] = '\0';
        }
    }
    (void)fclose(infile);
    pfree(buffer);
}
