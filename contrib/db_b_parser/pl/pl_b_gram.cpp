/* A Bison parser, made by GNU Bison 3.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         plbsql_yyparse
#define yylex           plbsql_yylex
#define yyerror         plbsql_yyerror
#define yydebug         plbsql_yydebug
#define yynerrs         plbsql_yynerrs
#define yylval          plbsql_yylval
#define yychar          plbsql_yychar
#define yylloc          plbsql_yylloc

/* First part of user prologue.  */
#line 1 "pl/pl_b_gram.y"

/* -------------------------------------------------------------------------
 *
 * gram.y				- Parser for the PL/pgSQL procedural language
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/pl/plpgsql/src/gram.y
 *
 * -------------------------------------------------------------------------
 */

#include "pl_b_sql.h"

#include "catalog/namespace.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "funcapi.h"
#include "nodes/makefuncs.h"
#include "parser/analyze.h"
#include "parser/keywords.h"
#include "parser/parser.h"
#include "parser/parse_coerce.h"
#include "parser/parse_expr.h"
#include "parser/parse_func.h"
#include "parser/parse_type.h"
#include "parser/scanner.h"
#include "parser/scansup.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "utils/syscache.h"

#include <limits.h>


/* Location tracking support --- simpler than bison's default */
#define YYLLOC_DEFAULT(Current, Rhs, N) \
    do { \
        if (N) \
            (Current) = (Rhs)[1]; \
        else \
            (Current) = (Rhs)[0]; \
    } while (0)

extern List* db_b_raw_parser(const char* str, List** query_string_locationlist = NULL);

/*
 * Helper function to record statements label so that GOTO statements could reach
 * there.
 */
static inline void
record_stmt_label(char * label, PLpgSQL_stmt *stmt)
{
    /*
     * Note, we do plpgsql parsing under "PL/pgSQL Function" memory context,
     * so palloc memory for goto-lable element and the global array under it.
     */
    PLpgSQL_gotoLabel *gl =
            (PLpgSQL_gotoLabel *)palloc0(sizeof(PLpgSQL_gotoLabel));
    gl->label = label;
    gl->stmt = stmt;
    u_sess->plsql_cxt.goto_labels = lappend(u_sess->plsql_cxt.goto_labels, (void *)gl);
}

/*
 * Bison doesn't allocate anything that needs to live across parser calls,
 * so we can easily have it use palloc instead of malloc.  This prevents
 * memory leaks if we error out during parsing.  Note this only works with
 * bison >= 2.0.  However, in bison 1.875 the default is to use alloca()
 * if possible, so there's not really much problem anyhow, at least if
 * you're building with gcc.
 */
#define YYMALLOC palloc
#define YYFREE   pfree


typedef struct
{
    int			location;
    int			leaderlen;
} sql_error_callback_arg;

#define parser_errposition(pos)  plpgsql_scanner_errposition(pos)

union YYSTYPE;					/* need forward reference for tok_is_keyword */

static	bool			tok_is_keyword(int token, union YYSTYPE *lval,
                                       int kw_token, const char *kw_str);
static	void			word_is_not_variable(PLword *word, int location);
static	void			cword_is_not_variable(PLcword *cword, int location);
static	void			current_token_is_not_variable(int tok);
static void yylex_inparam(StringInfoData* func_inparam,
                                            int *params,
                                            int * tok);
static void 	yylex_outparam(char** fieldnames,
                               int *varnos,
                               int nfields,
                               PLpgSQL_row **row,
                               PLpgSQL_rec **rec,
                               int *token,
                               bool overload = false);

static bool is_function(const char *name, bool is_assign, bool no_parenthesis);
static void 	plpgsql_parser_funcname(const char *s, char **output,
                                        int numidents);
static PLpgSQL_stmt 	*make_callfunc_stmt(const char *sqlstart,
                                int location, bool is_assign);
static PLpgSQL_stmt 	*make_callfunc_stmt_no_arg(const char *sqlstart, int location);
static PLpgSQL_expr 	*read_sql_construct5(int until,
                                             int until2,
                                             int until3,
                                             int until4,
                                             int until5,
                                             const char *expected,
                                             const char *sqlstart,
                                             bool isexpression,
                                             bool valid_sql,
                                             bool trim,
                                             int *startloc,
                                             int *endtoken);
                                             
static	PLpgSQL_expr	*read_sql_construct(int until,
                                            int until2,
                                            int until3,
                                            const char *expected,
                                            const char *sqlstart,
                                            bool isexpression,
                                            bool valid_sql,
                                            bool trim,
                                            int *startloc,
                                            int *endtoken);
static	PLpgSQL_expr	*read_sql_expression(int until,
                                             const char *expected);
static	PLpgSQL_expr	*read_sql_expression2(int until, int until2,
                                              const char *expected,
                                              int *endtoken);
static	PLpgSQL_expr	*read_sql_stmt(const char *sqlstart);
static	PLpgSQL_type	*read_datatype(int tok);
static  PLpgSQL_stmt	*parse_lob_open_close(int location);
static	PLpgSQL_stmt	*make_execsql_stmt(int firsttoken, int location);
static	PLpgSQL_stmt_fetch *read_fetch_direction(void);
static	void			 complete_direction(PLpgSQL_stmt_fetch *fetch,
                                            bool *check_FROM);
static	PLpgSQL_stmt	*make_return_stmt(int location);
static	PLpgSQL_stmt	*make_return_next_stmt(int location);
static	PLpgSQL_stmt	*make_return_query_stmt(int location);
static  PLpgSQL_stmt	*make_case(int location, PLpgSQL_expr *t_expr,
                                   List *case_when_list, List *else_stmts);
static	char			*NameOfDatum(PLwdatum *wdatum);
static	void			 check_assignable(PLpgSQL_datum *datum, int location);
static	void			 read_into_target(PLpgSQL_rec **rec, PLpgSQL_row **row,
                                          bool *strict);
static	PLpgSQL_row		*read_into_scalar_list(char *initial_name,
                                               PLpgSQL_datum *initial_datum,
                                               int initial_location);
static	PLpgSQL_row		*read_into_array_scalar_list(char *initial_name,
                                               PLpgSQL_datum *initial_datum,
                                               int initial_location);
static void             read_using_target(List **in_expr, PLpgSQL_row **out_row);

static PLpgSQL_row  *read_into_placeholder_scalar_list(char *initial_name,
                      int initial_location);

static	PLpgSQL_row		*make_scalar_list1(char *initial_name,
                                           PLpgSQL_datum *initial_datum,
                                           int lineno, int location);
static	void			 check_sql_expr(const char *stmt, int location,
                                        int leaderlen);
static	void			 plpgsql_sql_error_callback(void *arg);
static	PLpgSQL_type	*parse_datatype(const char *string, int location);
static	void			 check_labels(const char *start_label,
                                      const char *end_label,
                                      int end_location);
static	PLpgSQL_expr	*read_cursor_args(PLpgSQL_var *cursor,
                                          int until, const char *expected);
static	List			*read_raise_options(void);
static  bool            last_pragma;


#line 261 "pl/pl_b_gram.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_PLBSQL_YY_PL_PL_B_GRAM_HPP_INCLUDED
# define YY_PLBSQL_YY_PL_PL_B_GRAM_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int plbsql_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDENT = 258,
    FCONST = 259,
    SCONST = 260,
    BCONST = 261,
    VCONST = 262,
    XCONST = 263,
    Op = 264,
    CmpOp = 265,
    COMMENTSTRING = 266,
    ICONST = 267,
    PARAM = 268,
    TYPECAST = 269,
    ORA_JOINOP = 270,
    DOT_DOT = 271,
    COLON_EQUALS = 272,
    PARA_EQUALS = 273,
    T_WORD = 274,
    T_CWORD = 275,
    T_DATUM = 276,
    T_PLACEHOLDER = 277,
    T_VARRAY = 278,
    T_ARRAY_FIRST = 279,
    T_ARRAY_LAST = 280,
    T_ARRAY_COUNT = 281,
    T_ARRAY_EXTEND = 282,
    T_VARRAY_VAR = 283,
    T_RECORD = 284,
    LESS_LESS = 285,
    GREATER_GREATER = 286,
    T_REFCURSOR = 287,
    T_SQL_ISOPEN = 288,
    T_SQL_FOUND = 289,
    T_SQL_NOTFOUND = 290,
    T_SQL_ROWCOUNT = 291,
    T_CURSOR_ISOPEN = 292,
    T_CURSOR_FOUND = 293,
    T_CURSOR_NOTFOUND = 294,
    T_CURSOR_ROWCOUNT = 295,
    K_ABSOLUTE = 296,
    K_ALIAS = 297,
    K_ALL = 298,
    K_ALTER = 299,
    K_ARRAY = 300,
    K_BACKWARD = 301,
    K_BEGIN = 302,
    K_BY = 303,
    K_CASE = 304,
    K_CLOSE = 305,
    K_COLLATE = 306,
    K_COMMIT = 307,
    K_CONSTANT = 308,
    K_CONTINUE = 309,
    K_CURRENT = 310,
    K_CURSOR = 311,
    K_DEBUG = 312,
    K_DECLARE = 313,
    K_DEFAULT = 314,
    K_DELETE = 315,
    K_DETAIL = 316,
    K_DIAGNOSTICS = 317,
    K_DUMP = 318,
    K_ELSE = 319,
    K_ELSIF = 320,
    K_END = 321,
    K_ERRCODE = 322,
    K_ERROR = 323,
    K_EXCEPTION = 324,
    K_EXECUTE = 325,
    K_EXIT = 326,
    K_FETCH = 327,
    K_FIRST = 328,
    K_FOR = 329,
    K_FORALL = 330,
    K_FOREACH = 331,
    K_FORWARD = 332,
    K_FROM = 333,
    K_GET = 334,
    K_GOTO = 335,
    K_HINT = 336,
    K_IF = 337,
    K_IMMEDIATE = 338,
    K_IN = 339,
    K_INFO = 340,
    K_INSERT = 341,
    K_INTO = 342,
    K_IS = 343,
    K_LAST = 344,
    K_LOG = 345,
    K_LOOP = 346,
    K_MERGE = 347,
    K_MESSAGE = 348,
    K_MESSAGE_TEXT = 349,
    K_MOVE = 350,
    K_NEXT = 351,
    K_NO = 352,
    K_NOT = 353,
    K_NOTICE = 354,
    K_NULL = 355,
    K_OF = 356,
    K_OPEN = 357,
    K_OPTION = 358,
    K_OR = 359,
    K_OUT = 360,
    K_PERFORM = 361,
    K_PG_EXCEPTION_CONTEXT = 362,
    K_PG_EXCEPTION_DETAIL = 363,
    K_PG_EXCEPTION_HINT = 364,
    K_PRAGMA = 365,
    K_PRIOR = 366,
    K_QUERY = 367,
    K_RAISE = 368,
    K_RECORD = 369,
    K_REF = 370,
    K_RELATIVE = 371,
    K_RESULT_OID = 372,
    K_RETURN = 373,
    K_RETURNED_SQLSTATE = 374,
    K_REVERSE = 375,
    K_ROLLBACK = 376,
    K_ROWTYPE = 377,
    K_ROW_COUNT = 378,
    K_SAVEPOINT = 379,
    K_SELECT = 380,
    K_SCROLL = 381,
    K_SLICE = 382,
    K_SQLSTATE = 383,
    K_STACKED = 384,
    K_STRICT = 385,
    K_SYS_REFCURSOR = 386,
    K_THEN = 387,
    K_TO = 388,
    K_TYPE = 389,
    K_UPDATE = 390,
    K_USE_COLUMN = 391,
    K_USE_VARIABLE = 392,
    K_USING = 393,
    K_VARIABLE_CONFLICT = 394,
    K_VARRAY = 395,
    K_WARNING = 396,
    K_WHEN = 397,
    K_WHILE = 398,
    K_WITH = 399
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 188 "pl/pl_b_gram.y"

        core_YYSTYPE			core_yystype;
        /* these fields must match core_YYSTYPE: */
        int						ival;
        char					*str;
        const char				*keyword;

        PLword					word;
        PLcword					cword;
        PLwdatum				wdatum;
        bool					boolean;
        Oid						oid;
        struct
        {
            char *name;
            int  lineno;
        }						varname;
        struct
        {
            char *name;
            int  lineno;
            PLpgSQL_datum   *scalar;
            PLpgSQL_rec		*rec;
            PLpgSQL_row		*row;
        }						forvariable;
        struct
        {
            char *label;
            int  n_initvars;
            int  *initvarnos;
            bool autonomous;
        }						declhdr;
        struct
        {
            List *stmts;
            char *end_label;
            int   end_label_location;
        }						loop_body;
        List					*list;
        PLpgSQL_type			*dtype;
        PLpgSQL_datum			*datum;
        PLpgSQL_var				*var;
        PLpgSQL_expr			*expr;
        PLpgSQL_stmt			*stmt;
        PLpgSQL_condition		*condition;
        PLpgSQL_exception		*exception;
        PLpgSQL_exception_block	*exception_block;
        PLpgSQL_nsitem			*nsitem;
        PLpgSQL_diag_item		*diagitem;
        PLpgSQL_stmt_fetch		*fetch;
        PLpgSQL_case_when		*casewhen;
        PLpgSQL_rec_attr	*recattr;

#line 512 "pl/pl_b_gram.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern THR_LOCAL YYSTYPE plbsql_yylval;
extern THR_LOCAL YYLTYPE plbsql_yylloc;
int plbsql_yyparse (void);

#endif /* !YY_PLBSQL_YY_PL_PL_B_GRAM_HPP_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   669

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  152
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  99
/* YYNRULES -- Number of rules.  */
#define YYNRULES  252
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  363

#define YYUNDEFTOK  2
#define YYMAXUTOK   399


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   145,     2,     2,     2,     2,
     147,   148,     2,     2,   149,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   146,
       2,   150,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   151,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   443,   443,   449,   450,   453,   457,   461,   465,   471,
     472,   475,   503,   512,   520,   531,   544,   545,   548,   549,
     553,   566,   623,   630,   629,   654,   660,   670,   684,   689,
     711,   722,   735,   751,   755,   761,   793,   796,   800,   807,
     820,   823,   852,   856,   862,   870,   871,   871,   875,   876,
     878,   893,   924,   937,   953,   954,   959,   970,   971,   976,
     983,   984,   988,   990,   997,  1000,  1010,  1011,  1014,  1015,
    1019,  1020,  1024,  1031,  1044,  1046,  1048,  1050,  1052,  1054,
    1056,  1058,  1064,  1067,  1075,  1085,  1087,  1089,  1091,  1093,
    1095,  1097,  1099,  1101,  1103,  1105,  1107,  1109,  1111,  1113,
    1115,  1119,  1132,  1146,  1201,  1204,  1208,  1214,  1218,  1224,
    1237,  1266,  1278,  1283,  1290,  1296,  1302,  1307,  1312,  1317,
    1333,  1347,  1359,  1365,  1382,  1385,  1399,  1402,  1408,  1415,
    1429,  1433,  1439,  1451,  1454,  1469,  1489,  1510,  1550,  1578,
    1824,  1932,  1970,  2002,  2017,  2024,  2070,  2073,  2078,  2080,
    2084,  2099,  2103,  2109,  2135,  2305,  2323,  2327,  2331,  2340,
    2348,  2356,  2360,  2366,  2370,  2420,  2468,  2483,  2556,  2641,
    2673,  2686,  2691,  2704,  2717,  2731,  2745,  2761,  2766,  2774,
    2776,  2775,  2815,  2819,  2825,  2838,  2847,  2853,  2912,  2956,
    2960,  2964,  2968,  2972,  2976,  2984,  2987,  2997,  2999,  3006,
    3010,  3019,  3020,  3021,  3022,  3023,  3024,  3025,  3026,  3027,
    3028,  3029,  3030,  3031,  3032,  3033,  3034,  3035,  3036,  3037,
    3038,  3039,  3040,  3041,  3042,  3043,  3044,  3045,  3046,  3047,
    3048,  3049,  3050,  3051,  3052,  3053,  3054,  3055,  3056,  3057,
    3058,  3059,  3060,  3061,  3062,  3063,  3064,  3065,  3066,  3067,
    3068,  3069,  3070
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "FCONST", "SCONST", "BCONST",
  "VCONST", "XCONST", "Op", "CmpOp", "COMMENTSTRING", "ICONST", "PARAM",
  "TYPECAST", "ORA_JOINOP", "DOT_DOT", "COLON_EQUALS", "PARA_EQUALS",
  "T_WORD", "T_CWORD", "T_DATUM", "T_PLACEHOLDER", "T_VARRAY",
  "T_ARRAY_FIRST", "T_ARRAY_LAST", "T_ARRAY_COUNT", "T_ARRAY_EXTEND",
  "T_VARRAY_VAR", "T_RECORD", "LESS_LESS", "GREATER_GREATER",
  "T_REFCURSOR", "T_SQL_ISOPEN", "T_SQL_FOUND", "T_SQL_NOTFOUND",
  "T_SQL_ROWCOUNT", "T_CURSOR_ISOPEN", "T_CURSOR_FOUND",
  "T_CURSOR_NOTFOUND", "T_CURSOR_ROWCOUNT", "K_ABSOLUTE", "K_ALIAS",
  "K_ALL", "K_ALTER", "K_ARRAY", "K_BACKWARD", "K_BEGIN", "K_BY", "K_CASE",
  "K_CLOSE", "K_COLLATE", "K_COMMIT", "K_CONSTANT", "K_CONTINUE",
  "K_CURRENT", "K_CURSOR", "K_DEBUG", "K_DECLARE", "K_DEFAULT", "K_DELETE",
  "K_DETAIL", "K_DIAGNOSTICS", "K_DUMP", "K_ELSE", "K_ELSIF", "K_END",
  "K_ERRCODE", "K_ERROR", "K_EXCEPTION", "K_EXECUTE", "K_EXIT", "K_FETCH",
  "K_FIRST", "K_FOR", "K_FORALL", "K_FOREACH", "K_FORWARD", "K_FROM",
  "K_GET", "K_GOTO", "K_HINT", "K_IF", "K_IMMEDIATE", "K_IN", "K_INFO",
  "K_INSERT", "K_INTO", "K_IS", "K_LAST", "K_LOG", "K_LOOP", "K_MERGE",
  "K_MESSAGE", "K_MESSAGE_TEXT", "K_MOVE", "K_NEXT", "K_NO", "K_NOT",
  "K_NOTICE", "K_NULL", "K_OF", "K_OPEN", "K_OPTION", "K_OR", "K_OUT",
  "K_PERFORM", "K_PG_EXCEPTION_CONTEXT", "K_PG_EXCEPTION_DETAIL",
  "K_PG_EXCEPTION_HINT", "K_PRAGMA", "K_PRIOR", "K_QUERY", "K_RAISE",
  "K_RECORD", "K_REF", "K_RELATIVE", "K_RESULT_OID", "K_RETURN",
  "K_RETURNED_SQLSTATE", "K_REVERSE", "K_ROLLBACK", "K_ROWTYPE",
  "K_ROW_COUNT", "K_SAVEPOINT", "K_SELECT", "K_SCROLL", "K_SLICE",
  "K_SQLSTATE", "K_STACKED", "K_STRICT", "K_SYS_REFCURSOR", "K_THEN",
  "K_TO", "K_TYPE", "K_UPDATE", "K_USE_COLUMN", "K_USE_VARIABLE",
  "K_USING", "K_VARIABLE_CONFLICT", "K_VARRAY", "K_WARNING", "K_WHEN",
  "K_WHILE", "K_WITH", "'#'", "';'", "'('", "')'", "','", "'='", "'['",
  "$accept", "pl_function", "comp_options", "comp_option", "opt_semi",
  "pl_block", "decl_sect", "decl_start", "decl_stmts", "decl_stmt",
  "decl_statement", "$@1", "record_attr_list", "record_attr",
  "opt_scrollable", "decl_cursor_query", "decl_cursor_args",
  "decl_cursor_arglist", "decl_cursor_arg", "cursor_in_out_option",
  "decl_is_for", "decl_aliasitem", "decl_varname", "decl_const",
  "decl_datatype", "decl_collate", "decl_notnull", "decl_defval",
  "decl_rec_defval", "decl_defkey", "assign_operator", "proc_sect",
  "proc_stmts", "proc_stmt", "goto_block_label", "label_stmts",
  "label_stmt", "stmt_perform", "stmt_assign", "stmt_getdiag",
  "getdiag_area_opt", "getdiag_list", "getdiag_list_item", "getdiag_item",
  "getdiag_target", "varray_var", "record_var", "assign_var", "stmt_goto",
  "label_name", "stmt_if", "stmt_elsifs", "stmt_else", "stmt_case",
  "opt_expr_until_when", "case_when_list", "case_when", "opt_case_else",
  "stmt_loop", "stmt_while", "stmt_for", "for_control", "forall_control",
  "for_variable", "stmt_foreach_a", "foreach_slice", "forall_body",
  "stmt_exit", "exit_type", "stmt_return", "stmt_raise", "loop_body",
  "stmt_execsql", "stmt_dynexecute", "stmt_open", "stmt_fetch",
  "stmt_move", "opt_fetch_direction", "stmt_close", "stmt_null",
  "stmt_commit", "stmt_rollback", "cursor_variable", "exception_sect",
  "@2", "proc_exceptions", "proc_exception", "proc_conditions",
  "proc_condition", "expr_until_semi", "expr_until_rightbracket",
  "expr_until_parenthesis", "expr_until_then", "expr_until_loop",
  "opt_block_label", "opt_label", "opt_exitcond", "any_identifier",
  "unreserved_keyword", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,    35,    59,    40,    41,    44,
      61,    91
};
# endif

#define YYPACT_NINF (-298)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-195)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -298,    35,   -25,  -298,    42,   -83,  -298,  -102,     3,    -1,
    -298,  -298,    39,    14,   -37,  -298,  -298,   473,  -298,   266,
    -298,  -298,  -298,  -298,  -298,    42,   -80,    18,    28,  -298,
     392,  -298,  -298,  -298,  -298,  -298,  -298,  -298,     7,  -298,
      42,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
     111,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,    42,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
     111,  -298,  -298,  -298,  -298,  -298,  -298,   266,  -298,  -298,
       1,  -298,    64,  -298,  -298,    45,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,    86,  -298,  -298,  -298,  -298,
    -298,   -38,    82,  -298,  -298,  -298,  -298,   -31,    86,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
     -13,  -298,  -298,  -298,  -298,    42,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,   -28,    48,    48,    48,   526,
     -15,  -298,    90,   -79,     5,    66,  -298,  -298,  -298,    12,
      73,  -298,    15,  -298,   -11,    16,    38,    25,    42,    27,
    -298,  -298,  -298,    29,    86,  -298,  -298,    71,    30,  -298,
     379,    86,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -108,  -298,  -298,  -298,  -298,  -298,  -298,   526,    76,    -8,
      93,    53,   116,  -298,  -298,   526,  -298,    57,  -298,  -298,
    -298,   -87,  -298,    20,  -298,   134,  -298,  -298,  -298,  -298,
    -298,  -298,    42,    25,  -298,  -298,  -298,   -54,  -298,  -298,
      99,    89,  -298,  -298,    41,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,  -298,   177,   106,   100,
    -298,  -298,    46,    47,   139,    50,  -298,  -298,    52,    54,
     104,   -81,  -298,  -298,  -298,   438,   526,  -298,   140,  -298,
    -298,  -298,  -298,  -117,  -298,    -9,    59,  -298,  -298,  -298,
     164,    42,   111,   -33,   111,    65,   200,  -298,  -298,  -298,
     113,   -11,    42,   494,  -298,  -298,   166,  -298,    89,  -298,
     526,  -298,   150,  -298,    75,   -23,  -298,   -59,  -298,  -298,
    -298,    -5,  -298,  -298,  -298,    69,  -298,  -298,  -298,  -298,
      78,  -298,  -298,  -298,   379,   144,   526,  -298,  -298,   111,
    -298,  -298,  -298,  -298,    95,   111,   104,   135,  -298,  -298,
      97,  -298,  -298,  -298,  -298,  -298,   -10,  -298,  -298,  -298,
    -298,   103,  -298
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,   193,     1,     0,     0,     4,     9,     0,    12,
     199,   200,     0,     0,     0,    10,     2,    82,    15,    13,
     194,     5,     6,     8,     7,     0,     0,   179,    82,    73,
       0,    81,    75,    76,    77,    78,    79,    80,    12,    52,
       0,   201,   202,   203,   204,   205,   206,   207,   208,   209,
       0,   210,    19,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,     0,   232,   233,   234,   235,   236,
     237,   238,   239,   241,   240,   242,   243,   244,   245,   246,
       0,   247,   248,   249,   250,   251,   252,    14,    17,    18,
      54,    53,     0,    74,   180,     0,    72,   164,   165,   116,
     166,   118,   117,   156,   129,     0,   152,   160,   167,   151,
     171,   104,     0,   191,   157,   163,   171,     0,     0,   188,
     154,   153,   162,   158,   159,   161,    84,    93,    85,    94,
       0,    95,    86,    87,    88,   195,    89,    90,    91,    92,
      96,    97,    98,    99,   100,     0,     0,     0,     0,    82,
       0,   192,     0,    36,     0,     0,    16,   114,   115,     0,
       0,    55,     0,    56,     0,     0,    83,     0,   195,     0,
     177,   178,   176,     0,     0,   105,   106,     0,     0,   121,
      82,     0,   173,   168,   101,    69,   190,    68,   189,   188,
       0,   196,   174,   143,   144,   141,   142,    82,     0,     0,
       0,   146,     0,   135,   175,    82,    20,     0,    38,    23,
      32,     0,    26,     0,    27,    57,    67,    62,    29,    63,
      66,    31,     0,   181,   183,    11,   191,   133,   131,   172,
       0,     0,   122,   124,     0,   120,   119,   102,   188,   197,
     150,   137,   139,   138,   149,   148,   140,     0,     0,     0,
     136,    37,    40,     0,     0,     0,    50,    51,     0,     0,
      60,     0,   186,   187,   182,    82,    82,   130,     0,   169,
     112,   113,   111,     0,   108,     0,   126,   170,   198,   147,
       0,   195,     0,     0,     0,     0,     0,    22,    58,    59,
       0,     0,     0,    82,   132,   134,     0,   103,     0,   110,
      82,   191,     0,   192,     0,     0,    42,    47,    49,    48,
      39,     0,    33,    56,    25,     0,    61,    21,   185,   184,
       0,   107,   109,   127,    82,     0,    82,   155,    41,     0,
      45,    46,    56,    24,     0,     0,    60,     0,   128,   125,
       0,   145,    43,    44,    30,    34,    64,    56,   123,    35,
      65,     0,    28
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -298,  -298,  -298,  -298,  -298,   233,  -298,  -298,  -298,   148,
    -298,  -298,  -298,   -92,  -298,  -298,  -298,  -298,   -85,  -298,
    -298,  -298,   -48,  -298,  -297,  -298,   -90,   -44,  -298,   -96,
    -137,   -17,  -298,   234,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,   -47,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,  -298,  -298,  -298,    26,  -298,  -298,  -298,
    -298,  -298,  -298,   -12,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -194,    55,    56,  -298,  -298,  -298,   141,  -298,  -298,
    -298,  -298,  -112,  -298,  -298,  -298,    33,  -298,   -34,  -184,
    -298,  -298,  -222,   -43,   267,  -169,  -298,    -3,  -298
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,    16,    26,     8,    19,    97,    98,
      99,   262,   321,   322,   219,   343,   293,   315,   316,   342,
     320,   268,   100,   173,   225,   270,   301,   228,   359,   229,
     230,   212,    28,    29,    30,    31,   136,   137,   138,   139,
     187,   283,   284,   332,   285,   174,   175,   140,   141,   189,
     142,   286,   312,   143,   179,   237,   238,   278,    32,    33,
      34,   207,   209,   208,    35,   258,   253,   144,   145,   146,
     147,   213,   148,   149,   150,   151,   152,   184,   153,   154,
      36,    37,   183,   105,   177,   233,   234,   271,   272,   194,
     246,   245,   190,   215,    38,   200,   250,   201,   101
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      27,    12,   163,   199,   195,     4,   195,   195,   195,   235,
     276,   107,   108,   251,   275,   247,   193,   185,   217,   110,
      13,   260,   102,   302,   167,   340,   346,   263,   264,   307,
     168,    22,   308,   169,   248,     3,   113,   162,   249,   266,
     267,   318,   165,   170,    15,   353,   341,   218,   226,   226,
      17,   303,   117,   265,   171,   319,    14,    18,    25,   155,
     361,    10,   118,    11,   288,    18,   103,   203,   204,   205,
      20,   164,   240,   298,   299,  -193,   206,    21,   124,   244,
    -193,   156,   157,   158,   125,  -194,  -193,   104,   236,   334,
    -194,   186,   -71,   -71,   -71,   176,  -194,   -71,   159,    23,
      24,   188,  -193,  -193,  -193,   180,   181,   182,   280,   281,
     282,   178,  -194,  -194,  -194,   192,   132,   133,   202,  -193,
       5,   216,   314,   310,   311,   338,   339,   134,   160,  -194,
      39,   214,   172,   241,   196,   227,   135,   197,   198,   197,
     197,   197,   351,   344,   345,   210,   211,   223,   309,  -193,
     161,   220,    41,    42,   221,    43,    44,    45,   222,  -194,
     252,   224,   231,    46,    47,    48,    49,   232,    51,   236,
     -71,  -193,    53,   243,    54,   239,   242,   256,    55,    56,
     257,  -194,   259,   261,    57,   269,   279,   287,    58,   289,
     290,   291,    59,   292,   294,   295,    60,   296,   297,    61,
      62,    63,   300,    64,    65,    66,   306,    67,    68,   313,
      69,   324,   325,   326,    70,   330,   335,   347,    71,    72,
      73,   337,    75,    76,   348,    77,   350,    78,    79,   273,
      80,    81,    82,    83,    84,     7,   357,    85,    86,    87,
      88,   354,    89,   358,   317,   166,   323,    91,    92,   362,
      93,    94,    95,   355,   352,    96,   356,   327,   304,   305,
     360,   331,   106,   277,   254,   255,   274,   191,   328,     9,
     336,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,   329,     0,     0,     0,
       0,   317,     0,   333,     0,     0,    40,   323,     0,   273,
       0,     0,     0,     0,     0,     0,     0,    41,    42,     0,
      43,    44,    45,     0,     0,     0,     0,   349,    46,    47,
      48,    49,    50,    51,    52,     0,     0,    53,     0,    54,
       0,     0,     0,    55,    56,     0,     0,     0,     0,    57,
       0,     0,     0,    58,     0,     0,     0,    59,     0,     0,
       0,    60,     0,     0,    61,    62,    63,     0,    64,    65,
      66,     0,    67,    68,     0,    69,     0,     0,     0,    70,
       0,     0,     0,    71,    72,    73,    74,    75,    76,     0,
      77,     0,    78,    79,     0,    80,    81,    82,    83,    84,
       0,     0,    85,    86,    87,    88,     0,    89,     0,     0,
      90,     0,    91,    92,     0,    93,    94,    95,     0,    25,
      96,   107,   108,   109,     0,     0,     0,     0,     0,   110,
     111,   112,     0,     0,     0,     0,  -193,     0,     0,     0,
       0,  -193,     0,     0,     0,     0,   113,  -193,     0,     0,
       0,   114,   115,   -70,   -70,   -70,   116,     0,     0,     0,
       0,     0,   117,  -193,  -193,  -193,     0,     0,     0,     0,
       0,     0,   118,   119,   120,     0,     0,     0,    25,     0,
    -193,   121,   122,     0,   123,     0,     0,     0,   124,     0,
       0,     0,     0,     0,   125,  -193,     0,   126,     0,     0,
    -193,     0,   127,     0,   128,     0,  -193,     0,   129,     0,
    -193,     0,   -70,    25,   -70,   130,     0,     0,     0,     0,
     131,     0,  -193,  -193,  -193,     0,   132,   133,     0,     0,
    -193,     0,  -193,     0,    25,  -193,     0,   134,     0,  -193,
       0,  -193,     0,     0,     0,     0,   135,     0,     0,   -70,
       0,  -193,   -70,     0,     0,     0,  -193,  -193,  -193,  -193,
       0,     0,  -193,     0,     0,     0,    25,     0,     0,  -193,
     -70,     0,     0,     0,  -193,     0,     0,     0,  -193,  -193,
    -193,     0,     0,  -193,     0,     0,     0,     0,  -193,     0,
     -70,  -193,     0,     0,  -193,  -193,     0,     0,     0,     0,
       0,     0,   -70,     0,  -193,     0,     0,     0,     0,     0,
    -193,  -193,  -193,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -193,  -193,  -193,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   -70,  -193,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -193,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -193
};

static const yytype_int16 yycheck[] =
{
      17,     4,    50,   140,    17,    30,    17,    17,    17,   178,
      64,    19,    20,   207,   236,   199,   128,    55,    97,    27,
     103,   215,    25,   104,    23,    84,   323,   114,   115,   146,
      29,    68,   149,    32,   142,     0,    44,    40,   146,    19,
      20,    74,    90,    42,   146,   342,   105,   126,    59,    59,
      47,   132,    60,   140,    53,    88,   139,    58,    30,    52,
     357,    19,    70,    21,   248,    58,   146,    19,    20,    21,
      31,    74,   184,    19,    20,    47,    28,    63,    86,   191,
      52,    74,    75,    76,    92,    47,    58,    69,   142,   311,
      52,   129,    64,    65,    66,    31,    58,    69,    91,   136,
     137,    19,    74,    75,    76,    19,    20,    21,    19,    20,
      21,    66,    74,    75,    76,   146,   124,   125,   146,    91,
     145,    31,   291,    64,    65,   148,   149,   135,   121,    91,
      19,   146,   131,    62,   147,   146,   144,   150,   151,   150,
     150,   150,   336,   148,   149,   157,   158,    74,   285,   121,
     143,   146,    41,    42,    88,    44,    45,    46,   146,   121,
      84,   146,   146,    52,    53,    54,    55,   142,    57,   142,
     142,   143,    61,   190,    63,   146,   146,    84,    67,    68,
     127,   143,    66,   126,    73,    51,    87,   146,    77,    12,
      84,    91,    81,   147,   147,    56,    85,   147,   146,    88,
      89,    90,    98,    92,    93,    94,    66,    96,    97,    45,
      99,   146,    12,   100,   103,    49,    66,   148,   107,   108,
     109,   146,   111,   112,   146,   114,    82,   116,   117,   232,
     119,   120,   121,   122,   123,     2,   101,   126,   127,   128,
     129,   146,   131,   146,   292,    97,   294,   136,   137,   146,
     139,   140,   141,   345,   339,   144,   346,   301,   275,   276,
     356,   308,    28,   237,   209,   209,   233,   126,   302,     2,
     313,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,   303,    -1,    -1,    -1,
      -1,   339,    -1,   310,    -1,    -1,    30,   345,    -1,   302,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    -1,
      44,    45,    46,    -1,    -1,    -1,    -1,   334,    52,    53,
      54,    55,    56,    57,    58,    -1,    -1,    61,    -1,    63,
      -1,    -1,    -1,    67,    68,    -1,    -1,    -1,    -1,    73,
      -1,    -1,    -1,    77,    -1,    -1,    -1,    81,    -1,    -1,
      -1,    85,    -1,    -1,    88,    89,    90,    -1,    92,    93,
      94,    -1,    96,    97,    -1,    99,    -1,    -1,    -1,   103,
      -1,    -1,    -1,   107,   108,   109,   110,   111,   112,    -1,
     114,    -1,   116,   117,    -1,   119,   120,   121,   122,   123,
      -1,    -1,   126,   127,   128,   129,    -1,   131,    -1,    -1,
     134,    -1,   136,   137,    -1,   139,   140,   141,    -1,    30,
     144,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    44,    58,    -1,    -1,
      -1,    49,    50,    64,    65,    66,    54,    -1,    -1,    -1,
      -1,    -1,    60,    74,    75,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    -1,    -1,    -1,    30,    -1,
      91,    79,    80,    -1,    82,    -1,    -1,    -1,    86,    -1,
      -1,    -1,    -1,    -1,    92,    47,    -1,    95,    -1,    -1,
      52,    -1,   100,    -1,   102,    -1,    58,    -1,   106,    -1,
     121,    -1,    64,    30,    66,   113,    -1,    -1,    -1,    -1,
     118,    -1,    74,    75,    76,    -1,   124,   125,    -1,    -1,
      47,    -1,   143,    -1,    30,    52,    -1,   135,    -1,    91,
      -1,    58,    -1,    -1,    -1,    -1,   144,    -1,    -1,    66,
      -1,    47,    69,    -1,    -1,    -1,    52,    74,    75,    76,
      -1,    -1,    58,    -1,    -1,    -1,    30,    -1,    -1,   121,
      66,    -1,    -1,    -1,    91,    -1,    -1,    -1,    74,    75,
      76,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    -1,
     142,   143,    -1,    -1,    58,    91,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    -1,   121,    -1,    -1,    -1,    -1,    -1,
      74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   121,   143,    91,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   142,   143,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   153,   154,     0,    30,   145,   155,   157,   158,   246,
      19,    21,   249,   103,   139,   146,   156,    47,    58,   159,
      31,    63,    68,   136,   137,    30,   157,   183,   184,   185,
     186,   187,   210,   211,   212,   216,   232,   233,   246,    19,
      30,    41,    42,    44,    45,    46,    52,    53,    54,    55,
      56,    57,    58,    61,    63,    67,    68,    73,    77,    81,
      85,    88,    89,    90,    92,    93,    94,    96,    97,    99,
     103,   107,   108,   109,   110,   111,   112,   114,   116,   117,
     119,   120,   121,   122,   123,   126,   127,   128,   129,   131,
     134,   136,   137,   139,   140,   141,   144,   160,   161,   162,
     174,   250,   249,   146,    69,   235,   185,    19,    20,    21,
      27,    28,    29,    44,    49,    50,    54,    60,    70,    71,
      72,    79,    80,    82,    86,    92,    95,   100,   102,   106,
     113,   118,   124,   125,   135,   144,   188,   189,   190,   191,
     199,   200,   202,   205,   219,   220,   221,   222,   224,   225,
     226,   227,   228,   230,   231,    52,    74,    75,    76,    91,
     121,   143,   249,   174,   249,   174,   161,    23,    29,    32,
      42,    53,   131,   175,   197,   198,    31,   236,    66,   206,
      19,    20,    21,   234,   229,    55,   129,   192,    19,   201,
     244,   229,   146,   234,   241,    17,   147,   150,   151,   182,
     247,   249,   146,    19,    20,    21,    28,   213,   215,   214,
     215,   215,   183,   223,   146,   245,    31,    97,   126,   166,
     146,    88,   146,    74,   146,   176,    59,   146,   179,   181,
     182,   146,   142,   237,   238,   247,   142,   207,   208,   146,
     234,    62,   146,   183,   234,   243,   242,   241,   142,   146,
     248,   223,    84,   218,   224,   225,    84,   127,   217,    66,
     223,   126,   163,   114,   115,   140,    19,    20,   173,    51,
     177,   239,   240,   249,   238,   244,    64,   208,   209,    87,
      19,    20,    21,   193,   194,   196,   203,   146,   241,    12,
      84,    91,   147,   168,   147,    56,   147,   146,    19,    20,
      98,   178,   104,   132,   183,   183,    66,   146,   149,   182,
      64,    65,   204,    45,   247,   169,   170,   174,    74,    88,
     172,   164,   165,   174,   146,    12,   100,   179,   240,   183,
      49,   194,   195,   183,   244,    66,   245,   146,   148,   149,
      84,   105,   171,   167,   148,   149,   176,   148,   146,   183,
      82,   223,   170,   176,   146,   165,   178,   101,   146,   180,
     181,   176,   146
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   152,   153,   154,   154,   155,   155,   155,   155,   156,
     156,   157,   158,   158,   158,   159,   160,   160,   161,   161,
     161,   162,   162,   163,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   164,   164,   165,   166,   166,   166,   167,
     168,   168,   169,   169,   170,   171,   171,   171,   172,   172,
     173,   173,   174,   174,   175,   175,   176,   177,   177,   177,
     178,   178,   179,   179,   180,   180,   181,   181,   182,   182,
     183,   183,   184,   184,   185,   185,   185,   185,   185,   185,
     185,   185,   186,   186,   187,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   189,   190,   191,   192,   192,   192,   193,   193,   194,
     195,   196,   196,   196,   197,   198,   199,   199,   199,   199,
     199,   200,   201,   202,   203,   203,   204,   204,   205,   206,
     207,   207,   208,   209,   209,   210,   211,   212,   212,   213,
     214,   215,   215,   215,   215,   216,   217,   217,   218,   218,
     219,   220,   220,   221,   222,   223,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   234,   234,   235,
     236,   235,   237,   237,   238,   239,   239,   240,   241,   242,
     243,   244,   245,   246,   246,   247,   247,   248,   248,   249,
     249,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     0,     2,     3,     3,     3,     3,     0,
       1,     6,     1,     2,     3,     1,     2,     1,     1,     1,
       3,     6,     5,     0,     7,     6,     3,     3,    10,     3,
       8,     3,     3,     1,     3,     4,     0,     2,     1,     0,
       0,     3,     1,     3,     3,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     0,     1,     0,     0,     2,     2,
       0,     2,     1,     1,     0,     1,     1,     1,     1,     1,
       0,     1,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     5,     0,     1,     1,     3,     1,     3,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     2,     2,     8,     0,     4,     0,     2,     7,     0,
       2,     1,     3,     0,     2,     3,     4,     4,     4,     2,
       2,     1,     1,     1,     1,     8,     0,     2,     1,     1,
       3,     1,     1,     1,     1,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     4,
       4,     0,     3,     2,     3,     3,     1,     1,     1,     0,
       0,     3,     2,     1,     4,     3,     1,     1,     0,     0,
       0,     0,     0,     0,     3,     0,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
THR_LOCAL int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
THR_LOCAL int yychar;

/* The semantic value of the lookahead symbol.  */
THR_LOCAL YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
THR_LOCAL YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
THR_LOCAL int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 444 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_parse_result = (PLpgSQL_stmt_block *) (yyvsp[-1].stmt);
                    }
#line 2201 "pl/pl_b_gram.cpp"
    break;

  case 5:
#line 454 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_DumpExecTree = true;
                    }
#line 2209 "pl/pl_b_gram.cpp"
    break;

  case 6:
#line 458 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_curr_compile->resolve_option = PLPGSQL_RESOLVE_ERROR;
                    }
#line 2217 "pl/pl_b_gram.cpp"
    break;

  case 7:
#line 462 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_curr_compile->resolve_option = PLPGSQL_RESOLVE_VARIABLE;
                    }
#line 2225 "pl/pl_b_gram.cpp"
    break;

  case 8:
#line 466 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_curr_compile->resolve_option = PLPGSQL_RESOLVE_COLUMN;
                    }
#line 2233 "pl/pl_b_gram.cpp"
    break;

  case 11:
#line 476 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_block *newp;

                        newp = (PLpgSQL_stmt_block *)palloc0(sizeof(PLpgSQL_stmt_block));

                        newp->cmd_type	= PLPGSQL_STMT_BLOCK;
                        newp->lineno		= plpgsql_location_to_lineno((yylsp[-4]));
                        newp->label		= (yyvsp[-5].declhdr).label;
#ifndef ENABLE_MULTIPLE_NODES
                        newp->autonomous = (yyvsp[-5].declhdr).autonomous;
#endif
                        newp->n_initvars = (yyvsp[-5].declhdr).n_initvars;
                        newp->initvarnos = (yyvsp[-5].declhdr).initvarnos;
                        newp->body		= (yyvsp[-3].list);
                        newp->exceptions	= (yyvsp[-2].exception_block);

                        check_labels((yyvsp[-5].declhdr).label, (yyvsp[0].str), (yylsp[0]));
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-5].declhdr).label, (PLpgSQL_stmt *)newp);
                    }
#line 2262 "pl/pl_b_gram.cpp"
    break;

  case 12:
#line 504 "pl/pl_b_gram.y"
                    {
                        /* done with decls, so resume identifier lookup */
                        u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.declhdr).label	  = (yyvsp[0].str);
                        (yyval.declhdr).n_initvars = 0;
                        (yyval.declhdr).initvarnos = NULL;
                        (yyval.declhdr).autonomous = false;
                    }
#line 2275 "pl/pl_b_gram.cpp"
    break;

  case 13:
#line 513 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.declhdr).label	  = (yyvsp[-1].str);
                        (yyval.declhdr).n_initvars = 0;
                        (yyval.declhdr).initvarnos = NULL;
                        (yyval.declhdr).autonomous = false;
                    }
#line 2287 "pl/pl_b_gram.cpp"
    break;

  case 14:
#line 521 "pl/pl_b_gram.y"
                    {
                        u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.declhdr).label	  = (yyvsp[-2].str);
                        /* Remember variables declared in decl_stmts */
                        (yyval.declhdr).n_initvars = plpgsql_add_initdatums(&((yyval.declhdr).initvarnos));
                        (yyval.declhdr).autonomous = last_pragma;
                        last_pragma = false;
                    }
#line 2300 "pl/pl_b_gram.cpp"
    break;

  case 15:
#line 532 "pl/pl_b_gram.y"
                    {
                        /* Forget any variables created before block */
                        plpgsql_add_initdatums(NULL);
                        last_pragma = false;
                        /*
                         * Disable scanner lookup of identifiers while
                         * we process the decl_stmts
                         */
                        u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_DECLARE;
                    }
#line 2315 "pl/pl_b_gram.cpp"
    break;

  case 19:
#line 550 "pl/pl_b_gram.y"
                    {
                        /* We allow useless extra DECLAREs */
                    }
#line 2323 "pl/pl_b_gram.cpp"
    break;

  case 20:
#line 554 "pl/pl_b_gram.y"
                    {
                        /*
                         * Throw a helpful error if user tries to put block
                         * label just before BEGIN, instead of before DECLARE.
                         */
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("block label must be placed before DECLARE, not after"),
                                 parser_errposition((yylsp[-2]))));
                    }
#line 2338 "pl/pl_b_gram.cpp"
    break;

  case 21:
#line 567 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_variable	*var;

                        /*
                         * If a collation is supplied, insert it into the
                         * datatype.  We assume decl_datatype always returns
                         * a freshly built struct not shared with other
                         * variables.
                         */
                        if (OidIsValid((yyvsp[-2].oid)))
                        {
                            if (!OidIsValid((yyvsp[-3].dtype)->collation))
                                ereport(ERROR,
                                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                                         errmsg("collations are not supported by type %s",
                                                format_type_be((yyvsp[-3].dtype)->typoid)),
                                         parser_errposition((yylsp[-2]))));
                            (yyvsp[-3].dtype)->collation = (yyvsp[-2].oid);
                        }

                        var = plpgsql_build_variable((yyvsp[-5].varname).name, (yyvsp[-5].varname).lineno,
                                                     (yyvsp[-3].dtype), true);
                        if ((yyvsp[-4].boolean))
                        {
                            if (var->dtype == PLPGSQL_DTYPE_VAR)
                                ((PLpgSQL_var *) var)->isconst = (yyvsp[-4].boolean);
                            else
                                ereport(ERROR,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("row or record variable cannot be CONSTANT"),
                                         parser_errposition((yylsp[-4]))));
                        }
                        if ((yyvsp[-1].boolean))
                        {
                            if (var->dtype == PLPGSQL_DTYPE_VAR)
                                ((PLpgSQL_var *) var)->notnull = (yyvsp[-1].boolean);
                            else
                                ereport(ERROR,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("row or record variable cannot be NOT NULL"),
                                         parser_errposition((yylsp[-2]))));

                        }
                        if ((yyvsp[0].expr) != NULL)
                        {
                            if (var->dtype == PLPGSQL_DTYPE_VAR)
                                ((PLpgSQL_var *) var)->default_val = (yyvsp[0].expr);
                            else
                                ereport(ERROR,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("default value for row or record variable is not supported"),
                                         parser_errposition((yylsp[-1]))));
                        }

                        pfree_ext((yyvsp[-5].varname).name);
                    }
#line 2399 "pl/pl_b_gram.cpp"
    break;

  case 22:
#line 624 "pl/pl_b_gram.y"
                    {
                        plpgsql_ns_additem((yyvsp[-1].nsitem)->itemtype,
                                           (yyvsp[-1].nsitem)->itemno, (yyvsp[-4].varname).name);
                        pfree_ext((yyvsp[-4].varname).name);
                    }
#line 2409 "pl/pl_b_gram.cpp"
    break;

  case 23:
#line 630 "pl/pl_b_gram.y"
                    { plpgsql_ns_push((yyvsp[-1].varname).name); }
#line 2415 "pl/pl_b_gram.cpp"
    break;

  case 24:
#line 632 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_var *newp;

                        /* pop local namespace for cursor args */
                        plpgsql_ns_pop();

                        newp = (PLpgSQL_var *)
                                plpgsql_build_variable((yyvsp[-5].varname).name, (yyvsp[-5].varname).lineno,
                                                    plpgsql_build_datatype(REFCURSOROID,
                                                                            -1,
                                                                            InvalidOid),
                                                                            true);

                        newp->cursor_explicit_expr = (yyvsp[0].expr);
                        if ((yyvsp[-2].datum) == NULL)
                            newp->cursor_explicit_argrow = -1;
                        else
                            newp->cursor_explicit_argrow = (yyvsp[-2].datum)->dno;
                        newp->cursor_options = CURSOR_OPT_FAST_PLAN | (yyvsp[-4].ival);

                        pfree_ext((yyvsp[-5].varname).name);
                    }
#line 2442 "pl/pl_b_gram.cpp"
    break;

  case 25:
#line 655 "pl/pl_b_gram.y"
                    {
                        /* add name of cursor type to PLPGSQL_NSTYPE_REFCURSOR */
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_REFCURSOR,0,(yyvsp[-4].varname).name);
                        pfree_ext((yyvsp[-4].varname).name);
                    }
#line 2452 "pl/pl_b_gram.cpp"
    break;

  case 26:
#line 661 "pl/pl_b_gram.y"
                    {
                        plpgsql_build_variable(
                                (yyvsp[-2].varname).name, 
                                (yyvsp[-2].varname).lineno,
                                plpgsql_build_datatype(REFCURSOROID,-1,InvalidOid),
                                true);
                        pfree_ext((yyvsp[-2].varname).name);
                    }
#line 2465 "pl/pl_b_gram.cpp"
    break;

  case 27:
#line 671 "pl/pl_b_gram.y"
                    {
                        plpgsql_build_variable(
                                (yyvsp[-2].varname).name, 
                                (yyvsp[-2].varname).lineno,
                                plpgsql_build_datatype(REFCURSOROID,-1,InvalidOid),
                                true);
                        pfree_ext((yyvsp[-2].varname).name);
                    }
#line 2478 "pl/pl_b_gram.cpp"
    break;

  case 28:
#line 685 "pl/pl_b_gram.y"
                    {
                        plpgsql_build_varrayType((yyvsp[-8].varname).name, (yyvsp[-8].varname).lineno, (yyvsp[-1].dtype), true);
                        pfree_ext((yyvsp[-8].varname).name);
                    }
#line 2487 "pl/pl_b_gram.cpp"
    break;

  case 29:
#line 690 "pl/pl_b_gram.y"
                    {
                        char *type_name;
                        errno_t ret;
                        PLpgSQL_type * var_type = ((PLpgSQL_var *)u_sess->plsql_cxt.plpgsql_Datums[(yyvsp[-1].ival)])->datatype;
                        PLpgSQL_var *newp;
                        int len = strlen(var_type->typname) + 3;
                        type_name = (char *)palloc0(len);
                        ret = strcpy_s(type_name, len, var_type->typname);
                        securec_check(ret, "", "");
                        ret = strcat_s(type_name, len, "[]");
                        securec_check(ret, "", "");
                        var_type = parse_datatype(type_name, yylloc);

                        newp = (PLpgSQL_var *)plpgsql_build_variable((yyvsp[-2].varname).name, (yyvsp[-2].varname).lineno,var_type,true);
                        if (NULL == newp)
                            ereport(ERROR,
                                    (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("build variable failed")));
                        pfree_ext((yyvsp[-2].varname).name);
                        pfree_ext(type_name);
                    }
#line 2513 "pl/pl_b_gram.cpp"
    break;

  case 30:
#line 712 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_rec_type	*newp = NULL;

                        newp = plpgsql_build_rec_type((yyvsp[-6].varname).name, (yyvsp[-6].varname).lineno, (yyvsp[-2].list), true);
                        if (NULL == newp)
                            ereport(ERROR,
                                    (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("build variable failed")));
                        pfree_ext((yyvsp[-6].varname).name);
                    }
#line 2528 "pl/pl_b_gram.cpp"
    break;

  case 31:
#line 723 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_var *newp = NULL;
                        PLpgSQL_type * var_type = (PLpgSQL_type *)u_sess->plsql_cxt.plpgsql_Datums[(yyvsp[-1].ival)];

                        newp = (PLpgSQL_var *)plpgsql_build_variable((yyvsp[-2].varname).name,(yyvsp[-2].varname).lineno,
                                                                    var_type,true);
                        if (NULL == newp)
                            ereport(ERROR,
                                    (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("build variable failed")));
                        pfree_ext((yyvsp[-2].varname).name);
                    }
#line 2545 "pl/pl_b_gram.cpp"
    break;

  case 32:
#line 736 "pl/pl_b_gram.y"
                    {
			if (pg_strcasecmp((yyvsp[-1].str), "autonomous_transaction") == 0)
#ifndef ENABLE_MULTIPLE_NODES			
				last_pragma = true;
#else			     

				ereport(ERROR,
		                	(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
		                         errmsg("autonomous transaction is not yet supported.")));
#endif				
			else
				elog(ERROR, "invalid pragma");
		    }
#line 2563 "pl/pl_b_gram.cpp"
    break;

  case 33:
#line 752 "pl/pl_b_gram.y"
                   {
                        (yyval.list) = list_make1((yyvsp[0].recattr));
                   }
#line 2571 "pl/pl_b_gram.cpp"
    break;

  case 34:
#line 756 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].recattr));
                    }
#line 2579 "pl/pl_b_gram.cpp"
    break;

  case 35:
#line 762 "pl/pl_b_gram.y"
                  {
                        PLpgSQL_rec_attr	*attr = NULL;

                        attr = (PLpgSQL_rec_attr*)palloc0(sizeof(PLpgSQL_rec_attr));

                        attr->attrname = (yyvsp[-3].varname).name;
                        attr->type = (yyvsp[-2].dtype);

                        attr->notnull = (yyvsp[-1].boolean);
                        if ((yyvsp[0].expr) != NULL)
                        {
                            if (attr->type->ttype == PLPGSQL_TTYPE_SCALAR)
                                attr->defaultvalue = (yyvsp[0].expr);
                            else
                                ereport(ERROR,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("default value for row or record variable is not supported"),
                                         parser_errposition((yylsp[-1]))));
                        }

                        if ((yyvsp[-1].boolean) && (yyvsp[0].expr) == NULL)
                            ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("variables declared as NOT NULL must have "
                                "a default value.")));

                        (yyval.recattr) = attr;
                    }
#line 2612 "pl/pl_b_gram.cpp"
    break;

  case 36:
#line 793 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = 0;
                    }
#line 2620 "pl/pl_b_gram.cpp"
    break;

  case 37:
#line 797 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = CURSOR_OPT_NO_SCROLL;
                    }
#line 2628 "pl/pl_b_gram.cpp"
    break;

  case 38:
#line 801 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = CURSOR_OPT_SCROLL;
                    }
#line 2636 "pl/pl_b_gram.cpp"
    break;

  case 39:
#line 807 "pl/pl_b_gram.y"
                    {
                        int tok;
                        tok = yylex();
                        plbsql_push_back_token(tok);

                        /* check cursor syntax, cursor query only accept select query */
                        {
                            (yyval.expr) = read_sql_stmt("");
                        }
                    }
#line 2651 "pl/pl_b_gram.cpp"
    break;

  case 40:
#line 820 "pl/pl_b_gram.y"
                    {
                        (yyval.datum) = NULL;
                    }
#line 2659 "pl/pl_b_gram.cpp"
    break;

  case 41:
#line 824 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_row *newp;
                        int i;
                        ListCell *l;

                        newp = (PLpgSQL_row *)palloc0(sizeof(PLpgSQL_row));
                        newp->dtype = PLPGSQL_DTYPE_ROW;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->rowtupdesc = NULL;
                        newp->nfields = list_length((yyvsp[-1].list));
                        newp->fieldnames = (char **)palloc(newp->nfields * sizeof(char *));
                        newp->varnos = (int *)palloc(newp->nfields * sizeof(int));

                        i = 0;
                        foreach (l, (yyvsp[-1].list))
                        {
                            PLpgSQL_variable *arg = (PLpgSQL_variable *) lfirst(l);
                            newp->fieldnames[i] = arg->refname;
                            newp->varnos[i] = arg->dno;
                            i++;
                        }
                        list_free_ext((yyvsp[-1].list));

                        plpgsql_adddatum((PLpgSQL_datum *) newp);
                        (yyval.datum) = (PLpgSQL_datum *) newp;
                    }
#line 2690 "pl/pl_b_gram.cpp"
    break;

  case 42:
#line 853 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = list_make1((yyvsp[0].datum));
                    }
#line 2698 "pl/pl_b_gram.cpp"
    break;

  case 43:
#line 857 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].datum));
                    }
#line 2706 "pl/pl_b_gram.cpp"
    break;

  case 44:
#line 863 "pl/pl_b_gram.y"
                    {
                        (yyval.datum) = (PLpgSQL_datum *)
                            plpgsql_build_variable((yyvsp[-2].varname).name, (yyvsp[-2].varname).lineno,
                                                   (yyvsp[0].dtype), true);
                        pfree_ext((yyvsp[-2].varname).name);
                    }
#line 2717 "pl/pl_b_gram.cpp"
    break;

  case 50:
#line 879 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_nsitem *nsi;

                        nsi = plpgsql_ns_lookup(plpgsql_ns_top(), false,
                                                (yyvsp[0].word).ident, NULL, NULL,
                                                NULL);
                        if (nsi == NULL)
                            ereport(ERROR,
                                    (errcode(ERRCODE_UNDEFINED_OBJECT),
                                     errmsg("variable \"%s\" does not exist",
                                            (yyvsp[0].word).ident),
                                     parser_errposition((yylsp[0]))));
                        (yyval.nsitem) = nsi;
                    }
#line 2736 "pl/pl_b_gram.cpp"
    break;

  case 51:
#line 894 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_nsitem *nsi;

                        if (list_length((yyvsp[0].cword).idents) == 2)
                            nsi = plpgsql_ns_lookup(plpgsql_ns_top(), false,
                                                    strVal(linitial((yyvsp[0].cword).idents)),
                                                    strVal(lsecond((yyvsp[0].cword).idents)),
                                                    NULL,
                                                    NULL);
                        else if (list_length((yyvsp[0].cword).idents) == 3)
                            nsi = plpgsql_ns_lookup(plpgsql_ns_top(), false,
                                                    strVal(linitial((yyvsp[0].cword).idents)),
                                                    strVal(lsecond((yyvsp[0].cword).idents)),
                                                    strVal(lthird((yyvsp[0].cword).idents)),
                                                    NULL);
                        else
                            nsi = NULL;
                        if (nsi == NULL)
                            ereport(ERROR,
                                    (errcode(ERRCODE_UNDEFINED_OBJECT),
                                     errmsg("variable \"%s\" does not exist",
                                            NameListToString((yyvsp[0].cword).idents)),
                                     parser_errposition((yylsp[0]))));
                        (yyval.nsitem) = nsi;
                    }
#line 2766 "pl/pl_b_gram.cpp"
    break;

  case 52:
#line 925 "pl/pl_b_gram.y"
                    {
                        (yyval.varname).name = (yyvsp[0].word).ident;
                        (yyval.varname).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        /*
                         * Check to make sure name isn't already declared
                         * in the current block.
                         */
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].word).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                    }
#line 2783 "pl/pl_b_gram.cpp"
    break;

  case 53:
#line 938 "pl/pl_b_gram.y"
                    {
                        (yyval.varname).name = pstrdup((yyvsp[0].keyword));
                        (yyval.varname).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        /*
                         * Check to make sure name isn't already declared
                         * in the current block.
                         */
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].keyword), NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                    }
#line 2800 "pl/pl_b_gram.cpp"
    break;

  case 54:
#line 953 "pl/pl_b_gram.y"
                    { (yyval.boolean) = false; }
#line 2806 "pl/pl_b_gram.cpp"
    break;

  case 55:
#line 955 "pl/pl_b_gram.y"
                    { (yyval.boolean) = true; }
#line 2812 "pl/pl_b_gram.cpp"
    break;

  case 56:
#line 959 "pl/pl_b_gram.y"
                    {
                        /*
                         * If there's a lookahead token, read_datatype
                         * should consume it.
                         */
                        (yyval.dtype) = read_datatype(yychar);
                        yyclearin;
                    }
#line 2825 "pl/pl_b_gram.cpp"
    break;

  case 57:
#line 970 "pl/pl_b_gram.y"
                    { (yyval.oid) = InvalidOid; }
#line 2831 "pl/pl_b_gram.cpp"
    break;

  case 58:
#line 972 "pl/pl_b_gram.y"
                    {
                        (yyval.oid) = get_collation_oid(list_make1(makeString((yyvsp[0].word).ident)),
                                               false);
                    }
#line 2840 "pl/pl_b_gram.cpp"
    break;

  case 59:
#line 977 "pl/pl_b_gram.y"
                    {
                        (yyval.oid) = get_collation_oid((yyvsp[0].cword).idents, false);
                    }
#line 2848 "pl/pl_b_gram.cpp"
    break;

  case 60:
#line 983 "pl/pl_b_gram.y"
                    { (yyval.boolean) = false; }
#line 2854 "pl/pl_b_gram.cpp"
    break;

  case 61:
#line 985 "pl/pl_b_gram.y"
                    { (yyval.boolean) = true; }
#line 2860 "pl/pl_b_gram.cpp"
    break;

  case 62:
#line 989 "pl/pl_b_gram.y"
                    { (yyval.expr) = NULL; }
#line 2866 "pl/pl_b_gram.cpp"
    break;

  case 63:
#line 991 "pl/pl_b_gram.y"
                    {
                        (yyval.expr) = read_sql_expression(';', ";");
                    }
#line 2874 "pl/pl_b_gram.cpp"
    break;

  case 64:
#line 997 "pl/pl_b_gram.y"
                    {
                        (yyval.expr) = NULL;
                    }
#line 2882 "pl/pl_b_gram.cpp"
    break;

  case 65:
#line 1001 "pl/pl_b_gram.y"
                    {
                        int tok;

                        (yyval.expr) = read_sql_expression2(',', ')', ")", &tok);

                        plbsql_push_back_token(tok);
                    }
#line 2894 "pl/pl_b_gram.cpp"
    break;

  case 70:
#line 1019 "pl/pl_b_gram.y"
                    { (yyval.list) = NIL; }
#line 2900 "pl/pl_b_gram.cpp"
    break;

  case 71:
#line 1021 "pl/pl_b_gram.y"
                    { (yyval.list) = (yyvsp[0].list); }
#line 2906 "pl/pl_b_gram.cpp"
    break;

  case 72:
#line 1025 "pl/pl_b_gram.y"
                        {
                            if ((yyvsp[0].stmt) == NULL)
                                (yyval.list) = (yyvsp[-1].list);
                            else
                                (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].stmt));
                        }
#line 2917 "pl/pl_b_gram.cpp"
    break;

  case 73:
#line 1032 "pl/pl_b_gram.y"
                        {
                            if ((yyvsp[0].stmt) == NULL)
                                (yyval.list) = NIL;
                            else
                                (yyval.list) = list_make1((yyvsp[0].stmt));
                        }
#line 2928 "pl/pl_b_gram.cpp"
    break;

  case 74:
#line 1045 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 2934 "pl/pl_b_gram.cpp"
    break;

  case 75:
#line 1047 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2940 "pl/pl_b_gram.cpp"
    break;

  case 76:
#line 1049 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2946 "pl/pl_b_gram.cpp"
    break;

  case 77:
#line 1051 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2952 "pl/pl_b_gram.cpp"
    break;

  case 78:
#line 1053 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2958 "pl/pl_b_gram.cpp"
    break;

  case 79:
#line 1055 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2964 "pl/pl_b_gram.cpp"
    break;

  case 80:
#line 1057 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2970 "pl/pl_b_gram.cpp"
    break;

  case 81:
#line 1059 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2976 "pl/pl_b_gram.cpp"
    break;

  case 82:
#line 1064 "pl/pl_b_gram.y"
                    {
                        (yyval.str) = NULL;
                    }
#line 2984 "pl/pl_b_gram.cpp"
    break;

  case 83:
#line 1068 "pl/pl_b_gram.y"
                    {
                        plpgsql_ns_push((yyvsp[-1].str));
                        (yyval.str) = (yyvsp[-1].str);
                    }
#line 2993 "pl/pl_b_gram.cpp"
    break;

  case 84:
#line 1076 "pl/pl_b_gram.y"
                    {
                        /*
                         * If label is not null, we record it in current execution
                         * block so that the later or former GOTO can redirect the plpgsql execution steps
                         */
                        record_stmt_label((yyvsp[-1].str), (yyvsp[0].stmt));
                        (yyval.stmt) = (yyvsp[0].stmt);
                    }
#line 3006 "pl/pl_b_gram.cpp"
    break;

  case 85:
#line 1086 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3012 "pl/pl_b_gram.cpp"
    break;

  case 86:
#line 1088 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3018 "pl/pl_b_gram.cpp"
    break;

  case 87:
#line 1090 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3024 "pl/pl_b_gram.cpp"
    break;

  case 88:
#line 1092 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3030 "pl/pl_b_gram.cpp"
    break;

  case 89:
#line 1094 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3036 "pl/pl_b_gram.cpp"
    break;

  case 90:
#line 1096 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3042 "pl/pl_b_gram.cpp"
    break;

  case 91:
#line 1098 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3048 "pl/pl_b_gram.cpp"
    break;

  case 92:
#line 1100 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3054 "pl/pl_b_gram.cpp"
    break;

  case 93:
#line 1102 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3060 "pl/pl_b_gram.cpp"
    break;

  case 94:
#line 1104 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3066 "pl/pl_b_gram.cpp"
    break;

  case 95:
#line 1106 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3072 "pl/pl_b_gram.cpp"
    break;

  case 96:
#line 1108 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3078 "pl/pl_b_gram.cpp"
    break;

  case 97:
#line 1110 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3084 "pl/pl_b_gram.cpp"
    break;

  case 98:
#line 1112 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3090 "pl/pl_b_gram.cpp"
    break;

  case 99:
#line 1114 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3096 "pl/pl_b_gram.cpp"
    break;

  case 100:
#line 1116 "pl/pl_b_gram.y"
                        { (yyval.stmt) = (yyvsp[0].stmt); }
#line 3102 "pl/pl_b_gram.cpp"
    break;

  case 101:
#line 1120 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_perform *newp;

                        newp = (PLpgSQL_stmt_perform *)palloc0(sizeof(PLpgSQL_stmt_perform));
                        newp->cmd_type = PLPGSQL_STMT_PERFORM;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->expr  = (yyvsp[0].expr);

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 3117 "pl/pl_b_gram.cpp"
    break;

  case 102:
#line 1133 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_assign *newp;

                        newp = (PLpgSQL_stmt_assign *)palloc0(sizeof(PLpgSQL_stmt_assign));
                        newp->cmd_type = PLPGSQL_STMT_ASSIGN;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->varno = (yyvsp[-2].ival);
                        newp->expr  = (yyvsp[0].expr);

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 3133 "pl/pl_b_gram.cpp"
    break;

  case 103:
#line 1147 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_getdiag	 *newp;
                        ListCell		*lc;

                        newp = (PLpgSQL_stmt_getdiag *)palloc0(sizeof(PLpgSQL_stmt_getdiag));
                        newp->cmd_type = PLPGSQL_STMT_GETDIAG;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-4]));
                        newp->is_stacked = (yyvsp[-3].boolean);
                        newp->diag_items = (yyvsp[-1].list);

                        /*
                         * Check information items are valid for area option.
                         */
                        foreach(lc, newp->diag_items)
                        {
                            PLpgSQL_diag_item *ditem = (PLpgSQL_diag_item *) lfirst(lc);

                            switch (ditem->kind)
                            {
                                /* these fields are disallowed in stacked case */
                                case PLPGSQL_GETDIAG_ROW_COUNT:
                                case PLPGSQL_GETDIAG_RESULT_OID:
                                    if (newp->is_stacked)
                                        ereport(ERROR,
                                                (errcode(ERRCODE_SYNTAX_ERROR),
                                                 errmsg("diagnostics item %s is not allowed in GET STACKED DIAGNOSTICS",
                                                        plpgsql_getdiag_kindname(ditem->kind)),
                                                 parser_errposition((yylsp[-4]))));
                                    break;
                                /* these fields are disallowed in current case */
                                case PLPGSQL_GETDIAG_ERROR_CONTEXT:
                                case PLPGSQL_GETDIAG_ERROR_DETAIL:
                                case PLPGSQL_GETDIAG_ERROR_HINT:
                                case PLPGSQL_GETDIAG_RETURNED_SQLSTATE:
                                case PLPGSQL_GETDIAG_MESSAGE_TEXT:
                                    if (!newp->is_stacked)
                                        ereport(ERROR,
                                                (errcode(ERRCODE_SYNTAX_ERROR),
                                                 errmsg("diagnostics item %s is not allowed in GET CURRENT DIAGNOSTICS",
                                                        plpgsql_getdiag_kindname(ditem->kind)),
                                                 parser_errposition((yylsp[-4]))));
                                    break;
                                default:
                                    elog(ERROR, "unrecognized diagnostic item kind: %d",
                                         ditem->kind);
                                    break;
                            }
                        }

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 3189 "pl/pl_b_gram.cpp"
    break;

  case 104:
#line 1201 "pl/pl_b_gram.y"
                    {
                        (yyval.boolean) = false;
                    }
#line 3197 "pl/pl_b_gram.cpp"
    break;

  case 105:
#line 1205 "pl/pl_b_gram.y"
                    {
                        (yyval.boolean) = false;
                    }
#line 3205 "pl/pl_b_gram.cpp"
    break;

  case 106:
#line 1209 "pl/pl_b_gram.y"
                    {
                        (yyval.boolean) = true;
                    }
#line 3213 "pl/pl_b_gram.cpp"
    break;

  case 107:
#line 1215 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].diagitem));
                    }
#line 3221 "pl/pl_b_gram.cpp"
    break;

  case 108:
#line 1219 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = list_make1((yyvsp[0].diagitem));
                    }
#line 3229 "pl/pl_b_gram.cpp"
    break;

  case 109:
#line 1225 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_diag_item *newp;

                        newp = (PLpgSQL_diag_item *)palloc(sizeof(PLpgSQL_diag_item));
                        newp->target = (yyvsp[-2].ival);
                        newp->kind = (yyvsp[0].ival);

                        (yyval.diagitem) = newp;
                    }
#line 3243 "pl/pl_b_gram.cpp"
    break;

  case 110:
#line 1237 "pl/pl_b_gram.y"
                    {
                        int	tok = yylex();

                        if (tok_is_keyword(tok, &yylval,
                                           K_ROW_COUNT, "row_count"))
                            (yyval.ival) = PLPGSQL_GETDIAG_ROW_COUNT;
                        else if (tok_is_keyword(tok, &yylval,
                                                K_RESULT_OID, "result_oid"))
                            (yyval.ival) = PLPGSQL_GETDIAG_RESULT_OID;
                        else if (tok_is_keyword(tok, &yylval,
                                                K_PG_EXCEPTION_DETAIL, "pg_exception_detail"))
                            (yyval.ival) = PLPGSQL_GETDIAG_ERROR_DETAIL;
                        else if (tok_is_keyword(tok, &yylval,
                                                K_PG_EXCEPTION_HINT, "pg_exception_hint"))
                            (yyval.ival) = PLPGSQL_GETDIAG_ERROR_HINT;
                        else if (tok_is_keyword(tok, &yylval,
                                                K_PG_EXCEPTION_CONTEXT, "pg_exception_context"))
                            (yyval.ival) = PLPGSQL_GETDIAG_ERROR_CONTEXT;
                        else if (tok_is_keyword(tok, &yylval,
                                                K_MESSAGE_TEXT, "message_text"))
                            (yyval.ival) = PLPGSQL_GETDIAG_MESSAGE_TEXT;
                        else if (tok_is_keyword(tok, &yylval,
                                                K_RETURNED_SQLSTATE, "returned_sqlstate"))
                            (yyval.ival) = PLPGSQL_GETDIAG_RETURNED_SQLSTATE;
                        else
                            yyerror("unrecognized GET DIAGNOSTICS item");
                    }
#line 3275 "pl/pl_b_gram.cpp"
    break;

  case 111:
#line 1267 "pl/pl_b_gram.y"
                    {
                        check_assignable((yyvsp[0].wdatum).datum, (yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW ||
                            (yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("\"%s\" is not a scalar variable",
                                            NameOfDatum(&((yyvsp[0].wdatum)))),
                                     parser_errposition((yylsp[0]))));
                        (yyval.ival) = (yyvsp[0].wdatum).datum->dno;
                    }
#line 3291 "pl/pl_b_gram.cpp"
    break;

  case 112:
#line 1279 "pl/pl_b_gram.y"
                    {
                        /* just to give a better message than "syntax error" */
                        word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    }
#line 3300 "pl/pl_b_gram.cpp"
    break;

  case 113:
#line 1284 "pl/pl_b_gram.y"
                    {
                        /* just to give a better message than "syntax error" */
                        cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                    }
#line 3309 "pl/pl_b_gram.cpp"
    break;

  case 114:
#line 1291 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = (yyvsp[0].wdatum).datum->dno;
                    }
#line 3317 "pl/pl_b_gram.cpp"
    break;

  case 115:
#line 1297 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = (yyvsp[0].wdatum).datum->dno;
                    }
#line 3325 "pl/pl_b_gram.cpp"
    break;

  case 116:
#line 1303 "pl/pl_b_gram.y"
                    {
                        check_assignable((yyvsp[0].wdatum).datum, (yylsp[0]));
                        (yyval.ival) = (yyvsp[0].wdatum).datum->dno;
                    }
#line 3334 "pl/pl_b_gram.cpp"
    break;

  case 117:
#line 1308 "pl/pl_b_gram.y"
                    {
                        check_assignable((yyvsp[0].wdatum).datum, (yylsp[0]));
                        (yyval.ival) = (yyvsp[0].wdatum).datum->dno;
                    }
#line 3343 "pl/pl_b_gram.cpp"
    break;

  case 118:
#line 1313 "pl/pl_b_gram.y"
                    {
                        check_assignable((yyvsp[0].wdatum).datum, (yylsp[0]));
                        (yyval.ival) = (yyvsp[0].wdatum).datum->dno;
                    }
#line 3352 "pl/pl_b_gram.cpp"
    break;

  case 119:
#line 1318 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_arrayelem	*newp;

                        newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
                        newp->dtype		= PLPGSQL_DTYPE_ARRAYELEM;
                        newp->subscript	= (yyvsp[0].expr);
                        newp->arrayparentno = (yyvsp[-2].ival);
                        /* initialize cached type data to "not valid" */
                        newp->parenttypoid = InvalidOid;

                        plpgsql_adddatum((PLpgSQL_datum *) newp);

                        (yyval.ival) = newp->dno;
                    }
#line 3371 "pl/pl_b_gram.cpp"
    break;

  case 120:
#line 1334 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_arrayelem	*newp;
                        newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
                        newp->dtype		= PLPGSQL_DTYPE_ARRAYELEM;
                        newp->subscript	= (yyvsp[0].expr);
                        newp->arrayparentno = (yyvsp[-2].ival);
                        /* initialize cached type data to "not valid" */
                        newp->parenttypoid = InvalidOid;
                        plpgsql_adddatum((PLpgSQL_datum *)newp);
                        (yyval.ival) = newp->dno;
                    }
#line 3387 "pl/pl_b_gram.cpp"
    break;

  case 121:
#line 1348 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_goto *newp;

                        newp = (PLpgSQL_stmt_goto *)palloc0(sizeof(PLpgSQL_stmt_goto));
                        newp->cmd_type  = PLPGSQL_STMT_GOTO;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->label = (yyvsp[0].str);

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 3402 "pl/pl_b_gram.cpp"
    break;

  case 122:
#line 1360 "pl/pl_b_gram.y"
                    {
                        (yyval.str) = (yyvsp[-1].word).ident;
                    }
#line 3410 "pl/pl_b_gram.cpp"
    break;

  case 123:
#line 1366 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_if *newp;

                        newp = (PLpgSQL_stmt_if *)palloc0(sizeof(PLpgSQL_stmt_if));
                        newp->cmd_type	= PLPGSQL_STMT_IF;
                        newp->lineno		= plpgsql_location_to_lineno((yylsp[-7]));
                        newp->cond		= (yyvsp[-6].expr);
                        newp->then_body	= (yyvsp[-5].list);
                        newp->elsif_list = (yyvsp[-4].list);
                        newp->else_body  = (yyvsp[-3].list);

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 3428 "pl/pl_b_gram.cpp"
    break;

  case 124:
#line 1382 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = NIL;
                    }
#line 3436 "pl/pl_b_gram.cpp"
    break;

  case 125:
#line 1386 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_if_elsif *newp;

                        newp = (PLpgSQL_if_elsif *)palloc0(sizeof(PLpgSQL_if_elsif));
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->cond   = (yyvsp[-1].expr);
                        newp->stmts  = (yyvsp[0].list);

                        (yyval.list) = lappend((yyvsp[-3].list), newp);
                    }
#line 3451 "pl/pl_b_gram.cpp"
    break;

  case 126:
#line 1399 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = NIL;
                    }
#line 3459 "pl/pl_b_gram.cpp"
    break;

  case 127:
#line 1403 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = (yyvsp[0].list);
                    }
#line 3467 "pl/pl_b_gram.cpp"
    break;

  case 128:
#line 1409 "pl/pl_b_gram.y"
                    {
                        (yyval.stmt) = make_case((yylsp[-6]), (yyvsp[-5].expr), (yyvsp[-4].list), (yyvsp[-3].list));
                    }
#line 3475 "pl/pl_b_gram.cpp"
    break;

  case 129:
#line 1415 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_expr *expr = NULL;
                        int	tok = yylex();

                        if (tok != K_WHEN)
                        {
                            plbsql_push_back_token(tok);
                            expr = read_sql_expression(K_WHEN, "WHEN");
                        }
                        plbsql_push_back_token(K_WHEN);
                        (yyval.expr) = expr;
                    }
#line 3492 "pl/pl_b_gram.cpp"
    break;

  case 130:
#line 1430 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].casewhen));
                    }
#line 3500 "pl/pl_b_gram.cpp"
    break;

  case 131:
#line 1434 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = list_make1((yyvsp[0].casewhen));
                    }
#line 3508 "pl/pl_b_gram.cpp"
    break;

  case 132:
#line 1440 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_case_when *newp = (PLpgSQL_case_when *)palloc(sizeof(PLpgSQL_case_when));

                        newp->lineno	= plpgsql_location_to_lineno((yylsp[-2]));
                        newp->expr	= (yyvsp[-1].expr);
                        newp->stmts	= (yyvsp[0].list);
                        (yyval.casewhen) = newp;
                    }
#line 3521 "pl/pl_b_gram.cpp"
    break;

  case 133:
#line 1451 "pl/pl_b_gram.y"
                    {
                        (yyval.list) = NIL;
                    }
#line 3529 "pl/pl_b_gram.cpp"
    break;

  case 134:
#line 1455 "pl/pl_b_gram.y"
                    {
                        /*
                         * proc_sect could return an empty list, but we
                         * must distinguish that from not having ELSE at all.
                         * Simplest fix is to return a list with one NULL
                         * pointer, which make_case() must take care of.
                         */
                        if ((yyvsp[0].list) != NIL)
                            (yyval.list) = (yyvsp[0].list);
                        else
                            (yyval.list) = list_make1(NULL);
                    }
#line 3546 "pl/pl_b_gram.cpp"
    break;

  case 135:
#line 1470 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_loop *newp;

                        newp = (PLpgSQL_stmt_loop *)palloc0(sizeof(PLpgSQL_stmt_loop));
                        newp->cmd_type = PLPGSQL_STMT_LOOP;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->label	  = (yyvsp[-2].str);
                        newp->body	  = (yyvsp[0].loop_body).stmts;

                        check_labels((yyvsp[-2].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-2].str), (PLpgSQL_stmt *)newp);
                    }
#line 3568 "pl/pl_b_gram.cpp"
    break;

  case 136:
#line 1490 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_while *newp;

                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label	  = (yyvsp[-3].str);
                        newp->cond	  = (yyvsp[-1].expr);
                        newp->body	  = (yyvsp[0].loop_body).stmts;

                        check_labels((yyvsp[-3].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                    }
#line 3591 "pl/pl_b_gram.cpp"
    break;

  case 137:
#line 1511 "pl/pl_b_gram.y"
                    {
                        /* This runs after we've scanned the loop body */
                        if ((yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORI)
                        {
                            PLpgSQL_stmt_fori		*newp;

                            newp = (PLpgSQL_stmt_fori *) (yyvsp[-1].stmt);
                            newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->label	  = (yyvsp[-3].str);
                            newp->body	  = (yyvsp[0].loop_body).stmts;
                            (yyval.stmt) = (PLpgSQL_stmt *) newp;

                            /* register the stmt if it is labeled */
                            record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                        }
                        else
                        {
                            PLpgSQL_stmt_forq		*newp;

                            AssertEreport((yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORS ||
                                          (yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORC ||
                                          (yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_DYNFORS,
                                            MOD_PLSQL,
                                            "unexpected node type.");
                            /* forq is the common supertype of all three */
                            newp = (PLpgSQL_stmt_forq *) (yyvsp[-1].stmt);
                            newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->label	  = (yyvsp[-3].str);
                            newp->body	  = (yyvsp[0].loop_body).stmts;
                            (yyval.stmt) = (PLpgSQL_stmt *) newp;

                            /* register the stmt if it is labeled */
                            record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                        }

                        check_labels((yyvsp[-3].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        /* close namespace started in opt_block_label */
                        plpgsql_ns_pop();
                    }
#line 3635 "pl/pl_b_gram.cpp"
    break;

  case 138:
#line 1551 "pl/pl_b_gram.y"
                    {
                        /* This runs after we've scanned the loop body */
                        /* A db FORALL support 3 types like below. We implemented the first one.  
                         * FORALL index_name IN lower_bound .. upper_bound
                         * FORALL index_name IN INDICES OF collection between lower_bound and upper_bound
                         * FORALL index_name IN VALUES OF index_collection
                         * forall_body can only have one statement.
                         */
                        if ((yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORI)
                        {
                            PLpgSQL_stmt_fori		*newm;

                            newm = (PLpgSQL_stmt_fori *) (yyvsp[-1].stmt);
                            newm->label	  = NULL;
                            newm->body	  = (yyvsp[0].list);
                            (yyval.stmt) = (PLpgSQL_stmt *) newm;
                        }
                        else
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("please use \'FORALL index_name IN lower_bound .. upper_bound\'")));

                        /* close namespace started in opt_block_label */
                        plpgsql_ns_pop();
                    }
#line 3665 "pl/pl_b_gram.cpp"
    break;

  case 139:
#line 1579 "pl/pl_b_gram.y"
                    {
                        int			tok = yylex();
                        int			tokloc = yylloc;

                        if (tok == K_EXECUTE)
                        {
                            /* EXECUTE means it's a dynamic FOR loop */
                            PLpgSQL_stmt_dynfors	*newp;
                            PLpgSQL_expr			*expr;
                            int						term;

                            expr = read_sql_expression2(K_LOOP, K_USING,
                                                        "LOOP or USING",
                                                        &term);

                            newp = (PLpgSQL_stmt_dynfors *)palloc0(sizeof(PLpgSQL_stmt_dynfors));
                            newp->cmd_type = PLPGSQL_STMT_DYNFORS;
                            if ((yyvsp[-1].forvariable).rec)
                            {
                                newp->rec = (yyvsp[-1].forvariable).rec;
                                check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
                            }
                            else if ((yyvsp[-1].forvariable).row)
                            {
                                newp->row = (yyvsp[-1].forvariable).row;
                                check_assignable((PLpgSQL_datum *) newp->row, (yylsp[-1]));
                            }
                            else if ((yyvsp[-1].forvariable).scalar)
                            {
                                /* convert single scalar to list */
                                newp->row = make_scalar_list1((yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).scalar,
                                                             (yyvsp[-1].forvariable).lineno, (yylsp[-1]));
                                /* no need for check_assignable */
                            }
                            else
                            {
                                ereport(ERROR,
                                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                                         errmsg("loop variable of loop over rows must be a record or row variable or list of scalar variables"),
                                         parser_errposition((yylsp[-1]))));
                            }
                            newp->query = expr;

                            if (term == K_USING)
                            {
                                do
                                {
                                    expr = read_sql_expression2(',', K_LOOP,
                                                                ", or LOOP",
                                                                &term);
                                    newp->params = lappend(newp->params, expr);
                                } while (term == ',');
                            }

                            (yyval.stmt) = (PLpgSQL_stmt *) newp;
                        }
                        else if (tok == T_DATUM &&
                                 yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_VAR &&
                                 ((PLpgSQL_var *) yylval.wdatum.datum)->datatype->typoid == REFCURSOROID)
                        {
                            /* It's FOR var IN cursor */
                            PLpgSQL_stmt_forc	*newp;
                            PLpgSQL_var			*cursor = (PLpgSQL_var *) yylval.wdatum.datum;

                            newp = (PLpgSQL_stmt_forc *) palloc0(sizeof(PLpgSQL_stmt_forc));
                            newp->cmd_type = PLPGSQL_STMT_FORC;
                            newp->curvar = cursor->dno;

                            /* Should have had a single variable name */
                            if ((yyvsp[-1].forvariable).scalar && (yyvsp[-1].forvariable).row)
                                ereport(ERROR,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("cursor FOR loop must have only one target variable"),
                                         parser_errposition((yylsp[-1]))));

                            /* can't use an unbound cursor this way */
                            if (cursor->cursor_explicit_expr == NULL)
                                ereport(ERROR,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("cursor FOR loop must use a bound cursor variable"),
                                         parser_errposition(tokloc)));

                            /* collect cursor's parameters if any */
                            newp->argquery = read_cursor_args(cursor,
                                                             K_LOOP,
                                                             "LOOP");

                            /* create loop's private RECORD variable */
                            newp->rec = plpgsql_build_record((yyvsp[-1].forvariable).name,
                                                            (yyvsp[-1].forvariable).lineno,
                                                            true);

                            (yyval.stmt) = (PLpgSQL_stmt *) newp;
                        }
                        else
                        {
                            PLpgSQL_expr	*expr1;
                            int				expr1loc;
                            bool			reverse = false;

                            /*
                             * We have to distinguish between two
                             * alternatives: FOR var IN a .. b and FOR
                             * var IN query. Unfortunately this is
                             * tricky, since the query in the second
                             * form needn't start with a SELECT
                             * keyword.  We use the ugly hack of
                             * looking for two periods after the first
                             * token. We also check for the REVERSE
                             * keyword, which means it must be an
                             * integer loop.
                             */
                            if (tok_is_keyword(tok, &yylval,
                                               K_REVERSE, "reverse"))
                                reverse = true;
                            else
                                plbsql_push_back_token(tok);

                            /*
                             * Read tokens until we see either a ".."
                             * or a LOOP. The text we read may not
                             * necessarily be a well-formed SQL
                             * statement, so we need to invoke
                             * read_sql_construct directly.
                             */
                            expr1 = read_sql_construct(DOT_DOT,
                                                       K_LOOP,
                                                       0,
                                                       "LOOP",
                                                       "SELECT ",
                                                       true,
                                                       false,
                                                       true,
                                                       &expr1loc,
                                                       &tok);

                            if (tok == DOT_DOT)
                            {
                                /* Saw "..", so it must be an integer loop */
                                PLpgSQL_expr		*expr2;
                                PLpgSQL_expr		*expr_by;
                                PLpgSQL_var			*fvar;
                                PLpgSQL_stmt_fori	*newp;

                                /* Check first expression is well-formed */
                                check_sql_expr(expr1->query, expr1loc, 7);

                                /* Read and check the second one */
                                expr2 = read_sql_expression2(K_LOOP, K_BY,
                                                             "LOOP",
                                                             &tok);

                                /* Get the BY clause if any */
                                if (tok == K_BY)
                                    expr_by = read_sql_expression(K_LOOP,
                                                                  "LOOP");
                                else
                                    expr_by = NULL;

                                /* Should have had a single variable name */
                                if ((yyvsp[-1].forvariable).scalar && (yyvsp[-1].forvariable).row)
                                    ereport(ERROR,
                                            (errcode(ERRCODE_SYNTAX_ERROR),
                                             errmsg("integer FOR loop must have only one target variable"),
                                             parser_errposition((yylsp[-1]))));

                                /* create loop's private variable */
                                fvar = (PLpgSQL_var *)
                                    plpgsql_build_variable((yyvsp[-1].forvariable).name,
                                                           (yyvsp[-1].forvariable).lineno,
                                                           plpgsql_build_datatype(INT4OID,
                                                                                  -1,
                                                                                  InvalidOid),
                                                           true);

                                newp = (PLpgSQL_stmt_fori *)palloc0(sizeof(PLpgSQL_stmt_fori));
                                newp->cmd_type = PLPGSQL_STMT_FORI;
                                newp->var	  = fvar;
                                newp->reverse  = reverse;
                                newp->lower	  = expr1;
                                newp->upper	  = expr2;
                                newp->step	  = expr_by;

                                (yyval.stmt) = (PLpgSQL_stmt *) newp;
                            }
                            else
                            {
                                /*
                                 * No "..", so it must be a query loop. We've
                                 * prefixed an extra SELECT to the query text,
                                 * so we need to remove that before performing
                                 * syntax checking.
                                 */
                                char				*tmp_query;
                                PLpgSQL_stmt_fors	*newp;

                                if (reverse)
                                    ereport(ERROR,
                                            (errcode(ERRCODE_SYNTAX_ERROR),
                                             errmsg("cannot specify REVERSE in query FOR loop"),
                                             parser_errposition(tokloc)));

                                AssertEreport(strncmp(expr1->query, "SELECT ", 7) == 0,
                                                    MOD_PLSQL,
                                                    "It should be SELECT");
                                tmp_query = pstrdup(expr1->query + 7);
                                pfree_ext(expr1->query);
                                expr1->query = tmp_query;

                                check_sql_expr(expr1->query, expr1loc, 0);

                                newp = (PLpgSQL_stmt_fors *)palloc0(sizeof(PLpgSQL_stmt_fors));
                                newp->cmd_type = PLPGSQL_STMT_FORS;
                                if ((yyvsp[-1].forvariable).rec)
                                {
                                    newp->rec = (yyvsp[-1].forvariable).rec;
                                    check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
                                }
                                else if ((yyvsp[-1].forvariable).row)
                                {
                                    newp->row = (yyvsp[-1].forvariable).row;
                                    check_assignable((PLpgSQL_datum *) newp->row, (yylsp[-1]));
                                }
                                else if ((yyvsp[-1].forvariable).scalar)
                                {
                                    /* convert single scalar to list */
                                    newp->row = make_scalar_list1((yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).scalar,
                                                                 (yyvsp[-1].forvariable).lineno, (yylsp[-1]));
                                    /* no need for check_assignable */
                                }
                                else
                                {
                                    PLpgSQL_type dtype;
                                    dtype.ttype = PLPGSQL_TTYPE_REC;
                                    newp->rec = (PLpgSQL_rec *) 
                                        plpgsql_build_variable((yyvsp[-1].forvariable).name,(yyvsp[-1].forvariable).lineno, &dtype, true);
                                    check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
                                }

                                newp->query = expr1;
                                (yyval.stmt) = (PLpgSQL_stmt *) newp;
                            }
                        }
                    }
#line 3914 "pl/pl_b_gram.cpp"
    break;

  case 140:
#line 1825 "pl/pl_b_gram.y"
                    {
                        int		tok;
                        int		expr1loc;
                        PLpgSQL_expr	*expr1;

                        /*
                         * Read tokens until we see either a ".."
                         * or a LOOP. The text we read may not
                         * necessarily be a well-formed SQL
                         * statement, so we need to invoke
                         * read_sql_construct directly.
                         */
                        expr1 = read_sql_construct(DOT_DOT,
                                                   0,
                                                   0,
                                                   "..",
                                                   "SELECT ",
                                                   true,
                                                   false,
                                                   true,
                                                   &expr1loc,
                                                   &tok);

                        if (DOT_DOT == tok)
                        {
                            /* Saw "..", so it must be an integer loop */
                            PLpgSQL_expr		*expr2 = NULL;
                            PLpgSQL_var		*fvar = NULL;
                            PLpgSQL_stmt_fori	*newm = NULL;

                            /* Check first expression is well-formed */
                            check_sql_expr(expr1->query, expr1loc, 7);

                            /* Read and check the second one */
                            expr2 = read_sql_construct5(K_MERGE,
                                                        K_INSERT, 
                                                        K_SELECT,
                                                        K_UPDATE,
                                                        K_DELETE,
                                                        "DML",
                                                        "SELECT ",
                                                        true,
                                                        false,
                                                        true,
                                                        NULL,
                                                        &tok);

                            plbsql_push_back_token(tok);

                            if (';' == tok)
                                ereport(ERROR,
                                        (errcode(ERRCODE_FORALL_NEED_DML),
                                         errmsg("FORALL must follow DML statement.")));

                            /* should follow DML statement */
                            if (tok != K_INSERT && tok != K_UPDATE && tok != K_DELETE && tok != K_SELECT && tok != K_MERGE)
                                ereport(ERROR,
                                        (errcode(ERRCODE_FORALL_NEED_DML),
                                         errmsg("FORALL must follow DML statement.")));

                            /* Should have had a single variable name */
                            if ((yyvsp[-1].forvariable).scalar && (yyvsp[-1].forvariable).row)
                                ereport(ERROR,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("integer FORALL must have just one target variable")));

                            /* create loop's private variable */
                            fvar = (PLpgSQL_var *)
                                plpgsql_build_variable((yyvsp[-1].forvariable).name,
                                                       (yyvsp[-1].forvariable).lineno,
                                                       plpgsql_build_datatype(INT4OID, -1, InvalidOid),
                                                       true);

                            newm = (PLpgSQL_stmt_fori *)palloc0(sizeof(PLpgSQL_stmt_fori));
                            newm->cmd_type = PLPGSQL_STMT_FORI;
                            newm->var	 = fvar;
                            newm->reverse = false;
                            newm->lower	 = expr1;
                            newm->upper	 = expr2;
                            newm->step	 = NULL;

                            (yyval.stmt) = (PLpgSQL_stmt *) newm;
                        }
                        else
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("please use \'FORALL index_name IN lower_bound .. upper_bound\'")));						
                    }
#line 4007 "pl/pl_b_gram.cpp"
    break;

  case 141:
#line 1933 "pl/pl_b_gram.y"
                    {
                        (yyval.forvariable).name = NameOfDatum(&((yyvsp[0].wdatum)));
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_RECORD)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = (PLpgSQL_rec *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).row = NULL;
                        }
                        else
                        {
                            int			tok;

                            (yyval.forvariable).scalar = (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = NULL;
                            /* check for comma-separated list */
                            tok = yylex();
                            plbsql_push_back_token(tok);
                            if (tok == ',')
                                (yyval.forvariable).row = read_into_scalar_list((yyval.forvariable).name,
                                                               (yyval.forvariable).scalar,
                                                               (yylsp[0]));
                        }
                    }
#line 4049 "pl/pl_b_gram.cpp"
    break;

  case 142:
#line 1971 "pl/pl_b_gram.y"
                    {
                        (yyval.forvariable).name = NameOfDatum(&((yyvsp[0].wdatum)));
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = (PLpgSQL_rec *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).row = NULL;
                        }
                        else
                        {
                            int			tok;

                            (yyval.forvariable).scalar = (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = NULL;
                            /* check for comma-separated list */
                            tok = yylex();
                            plbsql_push_back_token(tok);
                            if (tok == ',')
                                (yyval.forvariable).row = read_into_scalar_list((yyval.forvariable).name,
                                                               (yyval.forvariable).scalar,
                                                               (yylsp[0]));
                        }
                    }
#line 4085 "pl/pl_b_gram.cpp"
    break;

  case 143:
#line 2003 "pl/pl_b_gram.y"
                    {
                        int			tok;

                        (yyval.forvariable).name = (yyvsp[0].word).ident;
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.forvariable).scalar = NULL;
                        (yyval.forvariable).rec = NULL;
                        (yyval.forvariable).row = NULL;
                        /* check for comma-separated list */
                        tok = yylex();
                        plbsql_push_back_token(tok);
                        if (tok == ',')
                            word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    }
#line 4104 "pl/pl_b_gram.cpp"
    break;

  case 144:
#line 2018 "pl/pl_b_gram.y"
                    {
                        /* just to give a better message than "syntax error" */
                        cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                    }
#line 4113 "pl/pl_b_gram.cpp"
    break;

  case 145:
#line 2025 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_foreach_a *newp;

                        newp = (PLpgSQL_stmt_foreach_a *)palloc0(sizeof(PLpgSQL_stmt_foreach_a));
                        newp->cmd_type = PLPGSQL_STMT_FOREACH_A;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-6]));
                        newp->label = (yyvsp[-7].str);
                        newp->slice = (yyvsp[-4].ival);
                        newp->expr = (yyvsp[-1].expr);
                        newp->body = (yyvsp[0].loop_body).stmts;

                        if ((yyvsp[-5].forvariable).rec)
                        {
                            newp->varno = (yyvsp[-5].forvariable).rec->dno;
                            check_assignable((PLpgSQL_datum *) (yyvsp[-5].forvariable).rec, (yylsp[-5]));
                        }
                        else if ((yyvsp[-5].forvariable).row)
                        {
                            newp->varno = (yyvsp[-5].forvariable).row->dno;
                            check_assignable((PLpgSQL_datum *) (yyvsp[-5].forvariable).row, (yylsp[-5]));
                        }
                        else if ((yyvsp[-5].forvariable).scalar)
                        {
                            newp->varno = (yyvsp[-5].forvariable).scalar->dno;
                            check_assignable((yyvsp[-5].forvariable).scalar, (yylsp[-5]));
                        }
                        else
                        {
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("loop variable of FOREACH must be a known variable or list of variables"),
                                             parser_errposition((yylsp[-5]))));
                        }

                        check_labels((yyvsp[-7].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *) newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-7].str), (PLpgSQL_stmt *)newp);
                    }
#line 4160 "pl/pl_b_gram.cpp"
    break;

  case 146:
#line 2070 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = 0;
                    }
#line 4168 "pl/pl_b_gram.cpp"
    break;

  case 147:
#line 2074 "pl/pl_b_gram.y"
                    {
                        (yyval.ival) = (yyvsp[0].ival);
                    }
#line 4176 "pl/pl_b_gram.cpp"
    break;

  case 148:
#line 2079 "pl/pl_b_gram.y"
                    { (yyval.list) = list_make1((yyvsp[0].stmt)); }
#line 4182 "pl/pl_b_gram.cpp"
    break;

  case 149:
#line 2081 "pl/pl_b_gram.y"
                    { (yyval.list) = list_make1((yyvsp[0].stmt)); }
#line 4188 "pl/pl_b_gram.cpp"
    break;

  case 150:
#line 2085 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_exit *newp;

                        newp = (PLpgSQL_stmt_exit *)palloc0(sizeof(PLpgSQL_stmt_exit));
                        newp->cmd_type = PLPGSQL_STMT_EXIT;
                        newp->is_exit  = (yyvsp[-2].boolean);
                        newp->lineno	  = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label	  = (yyvsp[-1].str);
                        newp->cond	  = (yyvsp[0].expr);

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 4205 "pl/pl_b_gram.cpp"
    break;

  case 151:
#line 2100 "pl/pl_b_gram.y"
                    {
                        (yyval.boolean) = true;
                    }
#line 4213 "pl/pl_b_gram.cpp"
    break;

  case 152:
#line 2104 "pl/pl_b_gram.y"
                    {
                        (yyval.boolean) = false;
                    }
#line 4221 "pl/pl_b_gram.cpp"
    break;

  case 153:
#line 2110 "pl/pl_b_gram.y"
                    {
                        int	tok;

                        tok = yylex();
                        if (tok == 0)
                            yyerror("unexpected end of function definition");

                        if (tok_is_keyword(tok, &yylval,
                                           K_NEXT, "next"))
                        {
                            (yyval.stmt) = make_return_next_stmt((yylsp[0]));
                        }
                        else if (tok_is_keyword(tok, &yylval,
                                                K_QUERY, "query"))
                        {
                            (yyval.stmt) = make_return_query_stmt((yylsp[0]));
                        }
                        else
                        {
                            plbsql_push_back_token(tok);
                            (yyval.stmt) = make_return_stmt((yylsp[0]));
                        }
                    }
#line 4249 "pl/pl_b_gram.cpp"
    break;

  case 154:
#line 2136 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_raise		*newp;
                        int	tok;
                        
                        PLpgSQL_row * row = NULL;
                        char message[32] = "";
                        StringInfoData ds;
                        initStringInfo(&ds);

                        newp = (PLpgSQL_stmt_raise *)palloc(sizeof(PLpgSQL_stmt_raise));

                        newp->cmd_type	= PLPGSQL_STMT_RAISE;
                        newp->lineno		= plpgsql_location_to_lineno((yylsp[0]));
                        newp->elog_level = ERROR;	/* default */
                        newp->condname	= NULL;
                        newp->message	= NULL;
                        newp->params		= NIL;
                        newp->options	= NIL;

                        tok = yylex();
                        if (tok == 0)
                            yyerror("unexpected end of function definition");

                        /*
                         * We could have just RAISE, meaning to re-throw
                         * the current error.
                         */
                        if (tok != ';')
                        {
                            if (T_DATUM == tok && PLPGSQL_DTYPE_ROW == yylval.wdatum.datum->dtype)
                            {
                                PLpgSQL_expr *expr = NULL;

                                sprintf(message, "line:%d ", plpgsql_location_to_lineno((yylsp[0])));
                                appendStringInfoString(&ds, message);
                                appendStringInfoString(&ds,"%");

                                row = (PLpgSQL_row *)yylval.wdatum.datum;

                                /* condname is system embedded error name, so it is still null in this case. */
                                newp->condname = pstrdup(unpack_sql_state(row->customErrorCode));
                                newp->message = pstrdup(ds.data);
                                plbsql_push_back_token(tok);
                                expr = read_sql_construct(';', 0, 0, ";",
                                                          "SELECT ", true, true, true, NULL, &tok);

                                if (tok != ';')
                                    yyerror("syntax error");

                                newp->params = lappend(newp->params, expr);
                            }
                            else
                            {
                                /*
                                 * First is an optional elog severity level.
                                 */
                                if (tok_is_keyword(tok, &yylval,
                                                   K_EXCEPTION, "exception"))
                                {
                                    newp->elog_level = ERROR;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_WARNING, "warning"))
                                {
                                    newp->elog_level = WARNING;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_NOTICE, "notice"))
                                {
                                    newp->elog_level = NOTICE;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_INFO, "info"))
                                {
                                    newp->elog_level = INFO;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_LOG, "log"))
                                {
                                    newp->elog_level = LOG;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_DEBUG, "debug"))
                                {
                                    newp->elog_level = DEBUG1;
                                    tok = yylex();
                                }
                                
                                if (tok == 0)
                                    yyerror("unexpected end of function definition");

                            /*
                             * Next we can have a condition name, or
                             * equivalently SQLSTATE 'xxxxx', or a string
                             * literal that is the old-style message format,
                             * or USING to start the option list immediately.
                             */
                            if (tok == SCONST)
                            {
                                /* old style message and parameters */
                                newp->message = yylval.str;
                                /*
                                 * We expect either a semi-colon, which
                                 * indicates no parameters, or a comma that
                                 * begins the list of parameter expressions,
                                 * or USING to begin the options list.
                                 */
                                tok = yylex();
                                if (tok != ',' && tok != ';' && tok != K_USING)
                                    yyerror("syntax error");

                                while (tok == ',')
                                {
                                    PLpgSQL_expr *expr;

                                    expr = read_sql_construct(',', ';', K_USING,
                                                              ", or ; or USING",
                                                              "SELECT ",
                                                              true, true, true,
                                                              NULL, &tok);
                                    newp->params = lappend(newp->params, expr);
                                }
                            }
                            else if (tok != K_USING)
                            {
                                /* must be condition name or SQLSTATE */
                                if (tok_is_keyword(tok, &yylval,
                                                   K_SQLSTATE, "sqlstate"))
                                {
                                    /* next token should be a string literal */
                                    char   *sqlstatestr;

                                    if (yylex() != SCONST)
                                        yyerror("syntax error");
                                    sqlstatestr = yylval.str;

                                    if (strlen(sqlstatestr) != 5)
                                        yyerror("invalid SQLSTATE code");
                                    if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
                                        yyerror("invalid SQLSTATE code");
                                    newp->condname = sqlstatestr;
                                }
                                else
                                {
                                    if (tok != T_WORD)
                                        yyerror("syntax error");
                                    newp->condname = yylval.word.ident;
                                    plpgsql_recognize_err_condition(newp->condname,
                                                                    false);
                                }
                                tok = yylex();
                                if (tok != ';' && tok != K_USING)
                                    yyerror("syntax error");
                            }

                            if (tok == K_USING)
                                newp->options = read_raise_options();
                            }
                        }
                        pfree_ext(ds.data);
                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 4421 "pl/pl_b_gram.cpp"
    break;

  case 155:
#line 2306 "pl/pl_b_gram.y"
                    {
                        (yyval.loop_body).stmts = (yyvsp[-4].list);
                        (yyval.loop_body).end_label = (yyvsp[-1].str);
                        (yyval.loop_body).end_label_location = (yylsp[-1]);
                    }
#line 4431 "pl/pl_b_gram.cpp"
    break;

  case 156:
#line 2324 "pl/pl_b_gram.y"
                    {
                        (yyval.stmt) = make_execsql_stmt(K_ALTER, (yylsp[0]));
                    }
#line 4439 "pl/pl_b_gram.cpp"
    break;

  case 157:
#line 2328 "pl/pl_b_gram.y"
                    {
                        (yyval.stmt) = make_execsql_stmt(K_INSERT, (yylsp[0]));
                    }
#line 4447 "pl/pl_b_gram.cpp"
    break;

  case 158:
#line 2332 "pl/pl_b_gram.y"
                    {
                        int tok = -1;

                        tok = yylex();
                        plbsql_push_back_token(tok);
                        (yyval.stmt) = make_execsql_stmt(K_SELECT, (yylsp[0]));
                    }
#line 4459 "pl/pl_b_gram.cpp"
    break;

  case 159:
#line 2341 "pl/pl_b_gram.y"
                    {
                        int			tok = -1;

                        tok = yylex();
                        plbsql_push_back_token(tok);
                        (yyval.stmt) = make_execsql_stmt(K_UPDATE, (yylsp[0]));
                    }
#line 4471 "pl/pl_b_gram.cpp"
    break;

  case 160:
#line 2349 "pl/pl_b_gram.y"
                    {
                        int			tok = -1;

			tok = yylex();
			plbsql_push_back_token(tok);
			(yyval.stmt) = make_execsql_stmt(K_DELETE, (yylsp[0]));
		    }
#line 4483 "pl/pl_b_gram.cpp"
    break;

  case 161:
#line 2357 "pl/pl_b_gram.y"
                    {
		    	(yyval.stmt) = make_execsql_stmt(K_WITH, (yylsp[0]));
		    }
#line 4491 "pl/pl_b_gram.cpp"
    break;

  case 162:
#line 2361 "pl/pl_b_gram.y"
                    {
		    	ereport(ERROR,
			    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			     errmsg("SAVEPOINT in function/procedure is not yet supported.")));
		    }
#line 4501 "pl/pl_b_gram.cpp"
    break;

  case 163:
#line 2367 "pl/pl_b_gram.y"
                    {
		    	(yyval.stmt) = make_execsql_stmt(K_MERGE, (yylsp[0]));
		    }
#line 4509 "pl/pl_b_gram.cpp"
    break;

  case 164:
#line 2371 "pl/pl_b_gram.y"
            {
                int tok = -1;
                bool isCallFunc = false;
                bool FuncNoarg = false;

                if (0 == strcasecmp((yyvsp[0].word).ident, "DBMS_LOB")
                && (plpgsql_is_token_match2('.', K_OPEN)
                || plpgsql_is_token_match2('.', K_CLOSE)))
                    (yyval.stmt) = parse_lob_open_close((yylsp[0]));
                else
                {
                    tok = yylex();
                    if ('(' == tok)
                        isCallFunc = is_function((yyvsp[0].word).ident, false, false);
                    else if ('=' ==tok || COLON_EQUALS == tok || '[' == tok)
                        word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    else if (';' == tok)
                    {
                        isCallFunc = is_function((yyvsp[0].word).ident, false, true);
                        FuncNoarg = true;
                    }

                    plbsql_push_back_token(tok);
                    if(isCallFunc)
                    {
                        if (FuncNoarg)
                        {
                            (yyval.stmt) = make_callfunc_stmt_no_arg((yyvsp[0].word).ident, (yylsp[0]));
                        }
                        else
                        {
                            PLpgSQL_stmt *stmt = make_callfunc_stmt((yyvsp[0].word).ident, (yylsp[0]), false);
                            if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
                            {
                                ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
                            }
                            else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
                            {
                                ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                            }
                            (yyval.stmt) = stmt;
                        }
                    }
                    else
                    {
                        (yyval.stmt) = make_execsql_stmt(T_WORD, (yylsp[0]));
                    }
                }
            }
#line 4563 "pl/pl_b_gram.cpp"
    break;

  case 165:
#line 2421 "pl/pl_b_gram.y"
            {
                int tok = yylex();
                char *name = NULL;
                bool isCallFunc = false;
                bool FuncNoarg = false;

                if ('(' == tok)
                {
                    MemoryContext colCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);
                    name = NameListToString((yyvsp[0].cword).idents);
                    (void)MemoryContextSwitchTo(colCxt);
                    isCallFunc = is_function(name, false, false);
                }
                else if ('=' == tok || COLON_EQUALS == tok || '[' == tok)
                    cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                else if (';' == tok) {
                    MemoryContext colCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);
                    name = NameListToString((yyvsp[0].cword).idents);
                    (void)MemoryContextSwitchTo(colCxt);
                    isCallFunc = is_function(name, false, true);
                    FuncNoarg = true;
                }

                plbsql_push_back_token(tok);
                if (isCallFunc)
                {
                    if (FuncNoarg)
                        (yyval.stmt) = make_callfunc_stmt_no_arg(name, (yylsp[0]));
                    else
                    {
                        PLpgSQL_stmt *stmt = make_callfunc_stmt(name, (yylsp[0]), false);
                        if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
                        {
                            ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
                        }
                        else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
                        {
                            ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                        }
                        (yyval.stmt) = stmt;
                    }
                }
                else
                {
                    (yyval.stmt) = make_execsql_stmt(T_CWORD, (yylsp[0]));
                }
            }
#line 4615 "pl/pl_b_gram.cpp"
    break;

  case 166:
#line 2469 "pl/pl_b_gram.y"
            {
                int tok = yylex();
                if (';' == tok)
                {
                    (yyval.stmt) =  NULL;
                }
                else
                {
                    plbsql_push_back_token(tok);
                    (yyval.stmt) = make_callfunc_stmt("array_extend", (yylsp[0]), false);
                }
            }
#line 4632 "pl/pl_b_gram.cpp"
    break;

  case 167:
#line 2484 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_dynexecute *newp;
                        PLpgSQL_expr *expr;
                        int endtoken;


                        if((endtoken = yylex()) != K_IMMEDIATE)
                        {
                            plbsql_push_back_token(endtoken);
                        }

                        expr = read_sql_construct(K_INTO, K_USING, ';',
                                                      "INTO or USING or ;",
                                                      "SELECT ",
                                                      true, true, true,
                                                      NULL, &endtoken);
                        newp = (PLpgSQL_stmt_dynexecute*)palloc0(sizeof(PLpgSQL_stmt_dynexecute));
                        newp->cmd_type = PLPGSQL_STMT_DYNEXECUTE;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        newp->query = expr;
                        newp->into = false;
                        newp->strict = false;
                        newp->rec = NULL;
                        newp->row = NULL;
                        newp->params = NIL;
                        newp->out_row = NULL;
                        newp->isinouttype = false;
                        newp->ppd = NULL;
                        newp->isanonymousblock = true;

                        /* If we found "INTO", collect the argument */
                        
                        if (endtoken == K_INTO)
                        {
                            if (newp->into)			/* multiple INTO */
                                yyerror("syntax error");
                            newp->into = true;
                            read_into_target(&newp->rec, &newp->row, &newp->strict);
                            endtoken = yylex();
                        }
                        /* If we found "USING", collect the argument */
                        if(endtoken == K_USING)
                        {
                            PLpgSQL_row *out_row;
                            if (newp->params)		/* multiple USING */
                                    yyerror("syntax error");

                            read_using_target(&(newp->params), &out_row );
                            if(out_row)
                            {
                                newp->into   = true;
                                newp->strict = true;
                                newp->isinouttype = true;
                            }
                            endtoken = yylex();
                            if( out_row && newp->row )
                            {
                                    ereport(ERROR,
                                            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                             errmsg("target into is conflicted with using out (inout)"),
                                             errdetail("\"select clause\" can't has out parameters, can only use \"into\"")));
                            }
                            newp->out_row = newp->row ? newp->row:out_row;

                        }
                        if (endtoken != ';')
                            yyerror("syntax error");

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 4707 "pl/pl_b_gram.cpp"
    break;

  case 168:
#line 2557 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_open *newp;
                        int				  tok;
                        int					 endtoken;
                        PLpgSQL_row			*out_row = NULL;

                        newp = (PLpgSQL_stmt_open *)palloc0(sizeof(PLpgSQL_stmt_open));
                        newp->cmd_type = PLPGSQL_STMT_OPEN;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->curvar = (yyvsp[0].var)->dno;
                        newp->cursor_options = CURSOR_OPT_FAST_PLAN;

                        if ((yyvsp[0].var)->cursor_explicit_expr == NULL)
                        {
                            /* be nice if we could use opt_scrollable here */
                            tok = yylex();
                            if (tok_is_keyword(tok, &yylval,
                                               K_NO, "no"))
                            {
                                tok = yylex();
                                if (tok_is_keyword(tok, &yylval,
                                                   K_SCROLL, "scroll"))
                                {
                                    newp->cursor_options |= CURSOR_OPT_NO_SCROLL;
                                    tok = yylex();
                                }
                            }
                            else if (tok_is_keyword(tok, &yylval,
                                                    K_SCROLL, "scroll"))
                            {
                                newp->cursor_options |= CURSOR_OPT_SCROLL;
                                tok = yylex();
                            }

                            if (tok != K_FOR)
                                yyerror("syntax error, expected \"FOR\"");

                            tok = yylex();
                            if (tok == K_EXECUTE)
                            {
                                newp->dynquery = read_sql_stmt("select ");
                            }
                            else
                            {
                                plbsql_push_back_token(tok);
                                
                                if (tok == K_SELECT || tok == K_WITH)
                                {
                                    newp->query = read_sql_stmt("");
                                }
                                else
                                {
                                    newp->dynquery =
                                    read_sql_expression2(K_USING, ';',
                                                         "USING or ;",
                                                         &endtoken);
                                    
                                    /* If we found "USING", collect argument(s) */
                                    if(K_USING == endtoken)
                                    {
                                        read_using_target(&(newp->params), &out_row);
                                        if(NULL != out_row)
                                            yyerror("syntax error");
                                        if(NULL == newp->params)
                                            yyerror("syntax error");					  	

                                        endtoken = plbsql_yylex();
                                    }

                                    if(';' != endtoken )
                                        yyerror("syntax error");
                                }
                            }
                        }
                        else
                        {
                            /* predefined cursor query, so read args */
                            newp->argquery = read_cursor_args((yyvsp[0].var), ';', ";");
                        }

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 4794 "pl/pl_b_gram.cpp"
    break;

  case 169:
#line 2642 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_fetch *fetch = (yyvsp[-2].fetch);
                        PLpgSQL_rec	   *rec;
                        PLpgSQL_row	   *row;

                        /* We have already parsed everything through the INTO keyword */
                        read_into_target(&rec, &row, NULL);

                        if (yylex() != ';')
                            yyerror("syntax error");

                        /*
                         * We don't allow multiple rows in PL/pgSQL's FETCH
                         * statement, only in MOVE.
                         */
                        if (fetch->returns_multiple_rows)
                            ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("FETCH statement cannot return multiple rows"),
                                     parser_errposition((yylsp[-3]))));

                        fetch->lineno = plpgsql_location_to_lineno((yylsp[-3]));
                        fetch->rec		= rec;
                        fetch->row		= row;
                        fetch->curvar	= (yyvsp[-1].var)->dno;
                        fetch->is_move	= false;

                        (yyval.stmt) = (PLpgSQL_stmt *)fetch;
                    }
#line 4828 "pl/pl_b_gram.cpp"
    break;

  case 170:
#line 2674 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_fetch *fetch = (yyvsp[-2].fetch);

                        fetch->lineno = plpgsql_location_to_lineno((yylsp[-3]));
                        fetch->curvar	= (yyvsp[-1].var)->dno;
                        fetch->is_move	= true;

                        (yyval.stmt) = (PLpgSQL_stmt *)fetch;
                    }
#line 4842 "pl/pl_b_gram.cpp"
    break;

  case 171:
#line 2686 "pl/pl_b_gram.y"
                    {
                        (yyval.fetch) = read_fetch_direction();
                    }
#line 4850 "pl/pl_b_gram.cpp"
    break;

  case 172:
#line 2692 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_close *newp;

                        newp = (PLpgSQL_stmt_close *)palloc(sizeof(PLpgSQL_stmt_close));
                        newp->cmd_type = PLPGSQL_STMT_CLOSE;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->curvar = (yyvsp[-1].var)->dno;

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 4865 "pl/pl_b_gram.cpp"
    break;

  case 173:
#line 2705 "pl/pl_b_gram.y"
                    {
                        /* We do building a node for NULL for GOTO */
                        PLpgSQL_stmt *newp;

                        newp = (PLpgSQL_stmt_null *)palloc(sizeof(PLpgSQL_stmt_null));
                        newp->cmd_type = PLPGSQL_STMT_NULL;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 4880 "pl/pl_b_gram.cpp"
    break;

  case 174:
#line 2718 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_commit *newp;

                        newp = (PLpgSQL_stmt_commit *)palloc(sizeof(PLpgSQL_stmt_commit));
                        newp->cmd_type = PLPGSQL_STMT_COMMIT;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                        record_stmt_label((yyvsp[-2].str), (PLpgSQL_stmt *)newp);
                    }
#line 4896 "pl/pl_b_gram.cpp"
    break;

  case 175:
#line 2732 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_stmt_rollback *newp;

                        newp = (PLpgSQL_stmt_rollback *) palloc(sizeof(PLpgSQL_stmt_rollback));
                        newp->cmd_type = PLPGSQL_STMT_ROLLBACK;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                        record_stmt_label((yyvsp[-2].str), (PLpgSQL_stmt *)newp);
                    }
#line 4912 "pl/pl_b_gram.cpp"
    break;

  case 176:
#line 2746 "pl/pl_b_gram.y"
                    {
                        if ((yyvsp[0].wdatum).datum->dtype != PLPGSQL_DTYPE_VAR)
                            ereport(ERROR,
                                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                                     errmsg("cursor variable must be a simple variable"),
                                     parser_errposition((yylsp[0]))));

                        if (((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->datatype->typoid != REFCURSOROID)
                            ereport(ERROR,
                                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                                     errmsg("variable \"%s\" must be of type cursor or refcursor",
                                            ((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->refname),
                                     parser_errposition((yylsp[0]))));
                        (yyval.var) = (PLpgSQL_var *) (yyvsp[0].wdatum).datum;
                    }
#line 4932 "pl/pl_b_gram.cpp"
    break;

  case 177:
#line 2762 "pl/pl_b_gram.y"
                    {
                        /* just to give a better message than "syntax error" */
                        word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    }
#line 4941 "pl/pl_b_gram.cpp"
    break;

  case 178:
#line 2767 "pl/pl_b_gram.y"
                    {
                        /* just to give a better message than "syntax error" */
                        cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                    }
#line 4950 "pl/pl_b_gram.cpp"
    break;

  case 179:
#line 2774 "pl/pl_b_gram.y"
                    { (yyval.exception_block) = NULL; }
#line 4956 "pl/pl_b_gram.cpp"
    break;

  case 180:
#line 2776 "pl/pl_b_gram.y"
                    {
                        /*
                         * We use a mid-rule action to add these
                         * special variables to the namespace before
                         * parsing the WHEN clauses themselves.  The
                         * scope of the names extends to the end of the
                         * current block.
                         */
                        int			lineno = plpgsql_location_to_lineno((yylsp[0]));
                        PLpgSQL_exception_block *newp = (PLpgSQL_exception_block *)palloc(sizeof(PLpgSQL_exception_block));
                        PLpgSQL_variable *var;

                        var = plpgsql_build_variable("sqlstate", lineno,
                                                     plpgsql_build_datatype(TEXTOID,
                                                                            -1,
                                                                            u_sess->plsql_cxt.plpgsql_curr_compile->fn_input_collation),
                                                     true);
                        ((PLpgSQL_var *) var)->isconst = true;
                        newp->sqlstate_varno = var->dno;

                        var = plpgsql_build_variable("sqlerrm", lineno,
                                                     plpgsql_build_datatype(TEXTOID,
                                                                            -1,
                                                                            u_sess->plsql_cxt.plpgsql_curr_compile->fn_input_collation),
                                                     true);
                        ((PLpgSQL_var *) var)->isconst = true;
                        newp->sqlerrm_varno = var->dno;

                        (yyval.exception_block) = newp;
                    }
#line 4991 "pl/pl_b_gram.cpp"
    break;

  case 181:
#line 2807 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_exception_block *newp = (yyvsp[-1].exception_block);
                        newp->exc_list = (yyvsp[0].list);

                        (yyval.exception_block) = newp;
                    }
#line 5002 "pl/pl_b_gram.cpp"
    break;

  case 182:
#line 2816 "pl/pl_b_gram.y"
                        {
                            (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].exception));
                        }
#line 5010 "pl/pl_b_gram.cpp"
    break;

  case 183:
#line 2820 "pl/pl_b_gram.y"
                        {
                            (yyval.list) = list_make1((yyvsp[0].exception));
                        }
#line 5018 "pl/pl_b_gram.cpp"
    break;

  case 184:
#line 2826 "pl/pl_b_gram.y"
                    {
                        PLpgSQL_exception *newp;

                        newp = (PLpgSQL_exception *)palloc0(sizeof(PLpgSQL_exception));
                        newp->lineno     = plpgsql_location_to_lineno((yylsp[-3]));
                        newp->conditions = (yyvsp[-2].condition);
                        newp->action	    = (yyvsp[0].list);

                        (yyval.exception) = newp;
                    }
#line 5033 "pl/pl_b_gram.cpp"
    break;

  case 185:
#line 2839 "pl/pl_b_gram.y"
                        {
                            PLpgSQL_condition	*old;

                            for (old = (yyvsp[-2].condition); old->next != NULL; old = old->next)
                                /* skip */ ;
                            old->next = (yyvsp[0].condition);
                            (yyval.condition) = (yyvsp[-2].condition);
                        }
#line 5046 "pl/pl_b_gram.cpp"
    break;

  case 186:
#line 2848 "pl/pl_b_gram.y"
                        {
                            (yyval.condition) = (yyvsp[0].condition);
                        }
#line 5054 "pl/pl_b_gram.cpp"
    break;

  case 187:
#line 2854 "pl/pl_b_gram.y"
                        {
                            if (strcmp((yyvsp[0].str), "sqlstate") != 0)
                            {
                                if (PLPGSQL_DTYPE_ROW == yylval.wdatum.datum->dtype)
                                {
                                    PLpgSQL_condition *newp = NULL;
                                    PLpgSQL_row * row = ( PLpgSQL_row* ) u_sess->plsql_cxt.plpgsql_Datums[yylval.wdatum.datum->dno];
                                    TupleDesc	rowtupdesc = row ? row->rowtupdesc : NULL;

                                    if(rowtupdesc && 
                                        0 == strcmp(format_type_be(rowtupdesc->tdtypeid), "exception"))
                                    {
                                        newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                                        newp->sqlerrstate = row->customErrorCode;
                                        newp->condname = pstrdup(row->refname);
                                        newp->next = NULL;
                                    }

                                    if(NULL == newp)
                                        ereport(ERROR,
                                                (errcode(ERRCODE_UNDEFINED_OBJECT),
                                                errmsg("unrecognized exception condition \"%s\"",
                                                        row? row->refname : "??")));
                                    (yyval.condition) = newp;
                                }
                                else
                                    (yyval.condition) = plpgsql_parse_err_condition((yyvsp[0].str));
                            }
                            else
                            {
                                PLpgSQL_condition *newp;
                                char   *sqlstatestr;

                                /* next token should be a string literal */
                                if (yylex() != SCONST)
                                    yyerror("syntax error");
                                sqlstatestr = yylval.str;

                                if (strlen(sqlstatestr) != 5)
                                    yyerror("invalid SQLSTATE code");
                                if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
                                    yyerror("invalid SQLSTATE code");

                                newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                                newp->sqlerrstate =
                                    MAKE_SQLSTATE(sqlstatestr[0],
                                                  sqlstatestr[1],
                                                  sqlstatestr[2],
                                                  sqlstatestr[3],
                                                  sqlstatestr[4]);
                                newp->condname = sqlstatestr;
                                newp->next = NULL;

                                (yyval.condition) = newp;
                            }
                        }
#line 5115 "pl/pl_b_gram.cpp"
    break;

  case 188:
#line 2912 "pl/pl_b_gram.y"
                    {
                        /* 
                         * support invoking function with out
                         * argument in a := expression 
                         */
                        PLpgSQL_stmt *stmt = NULL;
                        int tok = -1;
                        char *name = NULL;
                        bool isCallFunc = false;
                        PLpgSQL_expr* expr = NULL;

                        if (plpgsql_is_token_match2(T_WORD, '(') || 
                            plpgsql_is_token_match2(T_CWORD,'('))
                        {
                            tok = yylex();
                            if (T_WORD == tok)
                                name = yylval.word.ident;
                            else
                                name = NameListToString(yylval.cword.idents);

                            isCallFunc = is_function(name, true, false);
                        }

                        if (isCallFunc)
                        {
                            stmt = make_callfunc_stmt(name, yylloc, true);
                            if (PLPGSQL_STMT_EXECSQL == stmt->cmd_type)
                                expr = ((PLpgSQL_stmt_execsql*)stmt)->sqlstmt;
                            else if (PLPGSQL_STMT_PERFORM == stmt->cmd_type)
                                expr = ((PLpgSQL_stmt_perform*)stmt)->expr;

                            expr->is_funccall = true;
                            (yyval.expr) = expr;
                        }
                        else
                        {
                            if (name != NULL)
                                plbsql_push_back_token(tok);
                            (yyval.expr) = read_sql_expression(';', ";");
                        }	
                    }
#line 5161 "pl/pl_b_gram.cpp"
    break;

  case 189:
#line 2956 "pl/pl_b_gram.y"
                    { (yyval.expr) = read_sql_expression(']', "]"); }
#line 5167 "pl/pl_b_gram.cpp"
    break;

  case 190:
#line 2960 "pl/pl_b_gram.y"
                    { (yyval.expr) = read_sql_expression(')', ")"); }
#line 5173 "pl/pl_b_gram.cpp"
    break;

  case 191:
#line 2964 "pl/pl_b_gram.y"
                    { (yyval.expr) = read_sql_expression(K_THEN, "THEN"); }
#line 5179 "pl/pl_b_gram.cpp"
    break;

  case 192:
#line 2968 "pl/pl_b_gram.y"
                    { (yyval.expr) = read_sql_expression(K_LOOP, "LOOP"); }
#line 5185 "pl/pl_b_gram.cpp"
    break;

  case 193:
#line 2972 "pl/pl_b_gram.y"
                    {
                        plpgsql_ns_push(NULL);
                        (yyval.str) = NULL;
                    }
#line 5194 "pl/pl_b_gram.cpp"
    break;

  case 194:
#line 2977 "pl/pl_b_gram.y"
                    {
                        plpgsql_ns_push((yyvsp[-1].str));
                        (yyval.str) = (yyvsp[-1].str);
                    }
#line 5203 "pl/pl_b_gram.cpp"
    break;

  case 195:
#line 2984 "pl/pl_b_gram.y"
                    {
                        (yyval.str) = NULL;
                    }
#line 5211 "pl/pl_b_gram.cpp"
    break;

  case 196:
#line 2988 "pl/pl_b_gram.y"
                    {
#if 0
                        if (plpgsql_ns_lookup_label(plpgsql_ns_top(), (yyvsp[0].str)) == NULL)
                            yyerror("label does not exist");
#endif
                        (yyval.str) = (yyvsp[0].str);
                    }
#line 5223 "pl/pl_b_gram.cpp"
    break;

  case 197:
#line 2998 "pl/pl_b_gram.y"
                    { (yyval.expr) = NULL; }
#line 5229 "pl/pl_b_gram.cpp"
    break;

  case 198:
#line 3000 "pl/pl_b_gram.y"
                    { (yyval.expr) = (yyvsp[0].expr); }
#line 5235 "pl/pl_b_gram.cpp"
    break;

  case 199:
#line 3007 "pl/pl_b_gram.y"
                    {
                        (yyval.str) = (yyvsp[0].word).ident;
                    }
#line 5243 "pl/pl_b_gram.cpp"
    break;

  case 200:
#line 3011 "pl/pl_b_gram.y"
                    {
                        if ((yyvsp[0].wdatum).ident == NULL) /* composite name not OK */
                            yyerror("syntax error");
                        (yyval.str) = (yyvsp[0].wdatum).ident;
                    }
#line 5253 "pl/pl_b_gram.cpp"
    break;


#line 5257 "pl/pl_b_gram.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 3073 "pl/pl_b_gram.y"


#define MAX_EXPR_PARAMS  1024

/*
 * Check whether a token represents an "unreserved keyword".
 * We have various places where we want to recognize a keyword in preference
 * to a variable name, but not reserve that keyword in other contexts.
 * Hence, this kluge.
 */
static bool
tok_is_keyword(int token, union YYSTYPE *lval,
               int kw_token, const char *kw_str)
{
    if (token == kw_token)
    {
        /* Normal case, was recognized by scanner (no conflicting variable) */
        return true;
    }
    else if (token == T_DATUM)
    {
        /*
         * It's a variable, so recheck the string name.  Note we will not
         * match composite names (hence an unreserved word followed by "."
         * will not be recognized).
         */
        if (!lval->wdatum.quoted && lval->wdatum.ident != NULL &&
            strcmp(lval->wdatum.ident, kw_str) == 0)
            return true;
    }
    return false;				/* not the keyword */
}

/*
 * Convenience routine to complain when we expected T_DATUM and got T_WORD,
 * ie, unrecognized variable.
 */
static void
word_is_not_variable(PLword *word, int location)
{
    ereport(ERROR,
            (errcode(ERRCODE_SYNTAX_ERROR),
             errmsg("\"%s\" is not a known variable",
                    word->ident),
             parser_errposition(location)));
}

/* Same, for a CWORD */
static void
cword_is_not_variable(PLcword *cword, int location)
{
    ereport(ERROR,
            (errcode(ERRCODE_SYNTAX_ERROR),
             errmsg("\"%s\" is not a known variable",
                    NameListToString(cword->idents)),
             parser_errposition(location)));
}

/*
 * Convenience routine to complain when we expected T_DATUM and got
 * something else.  "tok" must be the current token, since we also
 * look at yylval and yylloc.
 */
static void
current_token_is_not_variable(int tok)
{
    if (tok == T_WORD)
        word_is_not_variable(&(yylval.word), yylloc);
    else if (tok == T_CWORD)
        cword_is_not_variable(&(yylval.cword), yylloc);
    else
        yyerror("syntax error");
}

/*
 * Brief		: handle the C-style function IN arguments.
 * Description	: 
 * Notes		:
 */ 
static void 
yylex_inparam(StringInfoData* func_inparam, 
              int *nparams,
              int * tok)
{
    PLpgSQL_expr * expr =   NULL;

    if (*nparams)
        appendStringInfoString(func_inparam, ",");

     /* 
      * handle the problem that the function 
      * arguments can only be variable. the argment validsql is set FALSE to
      * ignore sql expression check to the "$n" type of arguments. 
      */
    expr = read_sql_construct(',', ')', 0, ",|)", "", true, false, false, NULL, tok);

    if (*nparams >= MAX_EXPR_PARAMS)
        ereport(ERROR,
                (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                 errmsg("too many variables specified in SQL statement ,more than %d", MAX_EXPR_PARAMS)));	

    (*nparams)++;

    /* 
     * handle the problem that the function 
     * arguments can only be variable. After revising, the arguments can be any
     * expression. 
     */
    appendStringInfoString(func_inparam, expr->query);
}

/*
 * Brief		: handle the C-style function OUT arguments.
 * Description	: 
 * Notes		:
 */
static void 
yylex_outparam(char** fieldnames,
               int *varnos,
               int nfields,
               PLpgSQL_row **row,
               PLpgSQL_rec **rec,
               int *token,
               bool overload)
{
    *token = yylex();

    if (T_DATUM == *token)
    {
        if (PLPGSQL_TTYPE_ROW == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.datum->dno;
            *row = (PLpgSQL_row *)yylval.wdatum.datum;
        }
        else if (PLPGSQL_TTYPE_REC == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.datum->dno;
            *rec = (PLpgSQL_rec *)yylval.wdatum.datum;
        }
        else if (PLPGSQL_TTYPE_SCALAR == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.datum->dno;
        }
        else
            ereport(ERROR,
                (errcode(ERRCODE_SYNTAX_ERROR),
                errmsg("\"%s\" is not a scalar variable",
                NameOfDatum(&yylval.wdatum))));
    }
    else if (overload)
    {
        fieldnames[nfields] = NULL;
        varnos[nfields] = -1;
    }
}

/*
 * Brief: passe the called function name and return it into output.
 * Description: 
 * in s: original idents
 * inout output:  splited idents.
 * in numidnets: max ident number.
 * returns: void
 * Notes	: No need to consider double quoted ident because it has been handled in lex.
 * Ident here has been downcase if no double quoted, and been without quote if it had double quote before.
 */ 
void 
plpgsql_parser_funcname(const char *s, char **output, int numidents)
{
    int			ident_num = 0;
    char *outer_ptr = NULL;
    char *p = NULL;
    char delimiter[] = ".";
    errno_t ret;
    char *str = (char *)palloc0(sizeof(char) * (strlen(s) + 1));

    for (int i = 0; i < numidents; i++)
        output[i] = (char *)palloc0(sizeof(char) * NAMEDATALEN * 2);

    ret = strcpy_s(str, sizeof(char) * (strlen(s) + 1), s);
    securec_check(ret, "\0", "\0");

    p = strtok_s(str, delimiter, &outer_ptr);

    while (p != NULL)
    {
        ret = strcpy_s(output[ident_num++], sizeof(char) * NAMEDATALEN * 2, p);
        securec_check(ret, "\0", "\0");
        p = strtok_s(NULL, delimiter, &outer_ptr);
    }

    pfree_ext(str);
}

/*
 * Brief		: handle A-style function call.
 * Description	: handle A-style function call. First read the function 
 *  			  name, then read the arguments. At last assembing the function
 *				  name and arguments into the postgresql-style function call.
 * in sqlstart: the sql stmt string to be handle.
 * in location: the location number to erreport.
 * in is_assign: is an assign stmt or not.
 * returns: the parsed stmt.
 * Notes		:
 */ 
static PLpgSQL_stmt *
make_callfunc_stmt(const char *sqlstart, int location, bool is_assign)
{
    int nparams = 0;
    int nfields = 0;
    int narg = 0;
    int inArgNum = 0;
    int i= 0;
    int tok = 0;
    Oid *p_argtypes = NULL;
    char *cp[3];
    char **p_argnames = NULL;
    char *p_argmodes = NULL;
    /* pos_outer is the postion got by matching the name. */
    int pos_outer = 0;
    /* pos_inner is the position got by its real postion in function invoke */
    int pos_inner = -1;
    int	varnos[FUNC_MAX_ARGS];
    bool	namedarg[FUNC_MAX_ARGS];
    char*	namedargnamses[FUNC_MAX_ARGS];
    char *fieldnames[FUNC_MAX_ARGS];

    List *funcname = NIL;
    PLpgSQL_row *row = NULL;
    PLpgSQL_rec *rec = NULL;
    PLpgSQL_expr* expr = NULL;
    HeapTuple proctup = NULL;
    Form_pg_proc procStruct;
    FuncCandidateList clist = NULL;
    StringInfoData func_inparas;
    bool noargs = FALSE;
    int ndefaultargs = 0;
    StringInfoData argname;
    int j = 0;
    int placeholders = 0;
    char *quoted_sqlstart = NULL;

    MemoryContext oldCxt = NULL;
    bool	multi_func = false;
    /*get the function's name*/
    cp[0] = NULL;
    cp[1] = NULL;
    cp[2] = NULL;
    /* the function make_callfunc_stmt is only to assemble a sql statement, so the context is set to tmp context */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);
    plpgsql_parser_funcname(sqlstart, cp, 3);

    if (cp[2] && cp[2][0] != '\0')
        funcname = list_make3(makeString(cp[0]), makeString(cp[1]), makeString(cp[2]));
    else if (cp[1] && cp[1][0] != '\0')
        funcname = list_make2(makeString(cp[0]), makeString(cp[1]));
    else
        funcname = list_make1(makeString(cp[0]));


    /* search the function */
    clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
    if (!clist)
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function \"%s\" doesn't exist ", sqlstart)));
        return NULL;
    }

    if (clist->next)
    {
        multi_func = true;
        if (IsPackageFunction(funcname) == false)
        {
            ereport(ERROR,
            (errcode(ERRCODE_DUPLICATE_FUNCTION),
             errmsg("function \"%s\" isn't exclusive ", sqlstart)));
        }
    }

    if (multi_func == false)
    {
        proctup = SearchSysCache(PROCOID,
                                 ObjectIdGetDatum(clist->oid),
                                 0, 0, 0);

        /*
         * function may be deleted after clist be searched.
         */
        if (!HeapTupleIsValid(proctup))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_FUNCTION),
                     errmsg("function \"%s\" doesn't exist ", sqlstart)));
            return NULL;
        }
        /* get the all args informations, only "in" parameters if p_argmodes is null */
        narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
        if (p_argmodes)
        {
            for (i = 0; i < narg; i++)
            {
                if (p_argmodes[i] == 'i' || p_argmodes[i] == 'b')
                {
                    inArgNum++;
                }
            }
        }
        else
        {
            inArgNum = narg;
        }
        procStruct = (Form_pg_proc) GETSTRUCT(proctup);
        ndefaultargs = procStruct->pronargdefaults;
        ReleaseSysCache(proctup);
    }

    initStringInfo(&func_inparas);

    tok = yylex();

    /* check the format for the function without parameters */
    if ((tok = yylex()) == ')')
        noargs = TRUE;
    plbsql_push_back_token(tok);

    /* has any "out" parameters, user execsql stmt */
    if (is_assign)
    {
        appendStringInfoString(&func_inparas, "SELECT ");
    }
    else
    {
        appendStringInfoString(&func_inparas, "CALL ");
    }

    /*
     * Properly double-quote schema name and function name to handle uppercase
     * and special characters when making 'CALL func_name;' statement.
     */
    quoted_sqlstart = NameListToQuotedString(funcname);
    appendStringInfoString(&func_inparas, quoted_sqlstart);
    pfree_ext(quoted_sqlstart);

    appendStringInfoString(&func_inparas, "(");

    /* analyse all parameters */
    if (noargs)
    {
        i = 0;
        tok = yylex();
    }
    else if (!multi_func)
    {
        if (p_argmodes)
        {
            for ( i = 0; i < narg ; i++)
            {
                initStringInfo(&argname);
                pos_outer = -1;
                if ('o' == p_argmodes[i] || 'b' == p_argmodes[i])
                    pos_inner++;

                /*
                 * see if an arg is 'defargname => realargname' type.
                 * if so, p_argnames must be searched to match the defined
                 * argument name.
                 */
                if (plpgsql_is_token_match2(T_DATUM, PARA_EQUALS)
                    || plpgsql_is_token_match2(T_WORD, PARA_EQUALS))
                {
                    tok = yylex();
                    if (T_DATUM == tok)
                        appendStringInfoString(&argname, NameOfDatum(&yylval.wdatum));
                    else
                        appendStringInfoString(&argname, yylval.word.ident);
                    plbsql_push_back_token(tok);
                    /*
                     * search the p_argnames to match the right argname with current arg,
                     * so the real postion of current arg can be determined 
                     */
                    for (j = 0; j < narg; j++)
                    {
                        if ('o' == p_argmodes[j] || 'b' == p_argmodes[j])
                            pos_outer++;
                        if (0 == pg_strcasecmp(argname.data, p_argnames[j]))
                        {
                            /*
                             * if argmodes is 'i', just append the text, else, 
                             * row or rec should be assigned to store the out arg values
                             */
                            switch (p_argmodes[j])
                            {
                                case 'i':
                                    yylex_inparam(&func_inparas, &nparams, &tok);
                                    break;
                                case 'o':
                                case 'b':
                                    if (is_assign && 'o' == p_argmodes[j])
                                    {
                                        (void)yylex();
                                        (void)yylex();
                                        if (T_DATUM == (tok = yylex()))
                                        {
                                            plbsql_push_back_token(tok);
                                            (void)read_sql_expression2(',', ')', ",|)", &tok);
                                        }
                                        else
                                            yyerror("syntax error");
                                        break;
                                    }

                                    if (nparams)
                                        appendStringInfoChar(&func_inparas, ',');

                                    tok = yylex();
                                    if (T_DATUM == tok)
                                        appendStringInfoString(&func_inparas, NameOfDatum(&yylval.wdatum));
                                    else
                                        appendStringInfoString(&func_inparas, yylval.word.ident);

                                    appendStringInfoString(&func_inparas, "=>");
                                    (void)yylex();
                                    yylex_outparam(fieldnames, varnos, pos_outer, &row, &rec, &tok);
                                    if (T_DATUM == tok)
                                    {
                                        nfields++;
                                        appendStringInfoString(&func_inparas, NameOfDatum(&yylval.wdatum));
                                    }

                                    tok = yylex();
                                    nparams++;
                                    break;
                                default:
                                    ereport(ERROR,
                                            (errcode(ERRCODE_UNEXPECTED_NODE_STATE),
                                             errmsg("parameter mode %c doesn't exist", p_argmodes[j])));
                            }
                            break;
                        }
                    }
                }
                else
                {
                    tok = yylex();
                    /* p_argmodes may be null, 'i'->in , 'o'-> out ,'b' inout,others error */
                    switch (p_argmodes[i])
                    {
                        case 'i':
                            if (T_PLACEHOLDER == tok)
                                placeholders++;
                            plbsql_push_back_token(tok);
                            yylex_inparam(&func_inparas, &nparams, &tok);
                            break;
                        case 'o':
                            /*
                             * if the function is in an assign expr, just ignore the 
                             * out parameters.
                             */
                            if (is_assign)
                            {
                                if (T_DATUM == tok || T_PLACEHOLDER == tok)
                                {
                                    plbsql_push_back_token(tok);
                                    (void)read_sql_expression2(',', ')', ",|)", &tok);
                                }
                                else
                                    yyerror("syntax error");
                                break;
                            }

                            if (T_PLACEHOLDER == tok)
                            {
                                placeholders++;
                                if (nparams)
                                    appendStringInfoChar(&func_inparas, ',');
                                appendStringInfoString(&func_inparas, yylval.word.ident);
                                nparams++;
                            }
                            else if (T_DATUM == tok)
                            {
                                if (nparams)
                                    appendStringInfoString(&func_inparas, ",");
                                appendStringInfoString(&func_inparas, NameOfDatum(&yylval.wdatum));
                                nparams++;
                                nfields++;
                            }

                            plbsql_push_back_token(tok);
                            yylex_outparam(fieldnames, varnos, pos_inner, &row, &rec, &tok);

                            tok = yylex();
                            break;
                        case 'b':
                            if (is_assign)
                            {
                                /*
                                 * if the function is in an assign expr, read the inout
                                 * parameters.
                                 */
                                if (T_DATUM == tok)
                                {
                                    plbsql_push_back_token(tok);
                                    yylex_inparam(&func_inparas, &nparams, &tok);
                                }
                                else
                                    yyerror("syntax error");
                                break;
                            }

                            if (T_PLACEHOLDER == tok)
                                placeholders++;
                            plbsql_push_back_token(tok);
                            yylex_outparam(fieldnames, varnos, pos_inner, &row, &rec, &tok);
                            if (T_DATUM == tok)
                                nfields++;
                            plbsql_push_back_token(tok);
                            yylex_inparam(&func_inparas, &nparams, &tok);
                            break;
                        default:
                            ereport(ERROR,
                                    (errcode(ERRCODE_UNEXPECTED_NODE_STATE),
                                     errmsg("parameter mode %c doesn't exist", p_argmodes[i])));
                    }
                }

                if (')' == tok)
                {
                    i++;
                    break;
                }

                if (narg - 1 == i)
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, expected \")\", maybe input something superfluous.", sqlstart)));

                if (',' != tok)
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, expected \",\"", sqlstart)));
                pfree_ext(argname.data);
            }
        }
        else
        {	
            for ( i = 0; i < narg; i++)
            {
                tok = yylex();
                if (T_PLACEHOLDER == tok)
                    placeholders++;
                plbsql_push_back_token(tok);
        
                yylex_inparam(&func_inparas, &nparams, &tok);
        
                if (')' == tok)
                {
                    i++;
                    break;
                }
        
                if (narg - 1 == i)
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, expected \")\", maybe input something superfluous.", sqlstart)));
            }
        }
    }
    else
    {
        while (true)
        {

            /* for named arguemnt */
            if (plpgsql_is_token_match2(T_DATUM, PARA_EQUALS)
                || plpgsql_is_token_match2(T_WORD, PARA_EQUALS))
            {
                tok = yylex();
                if (nparams)
                    appendStringInfoString(&func_inparas, ",");
                if (T_DATUM == tok)
                {
                    appendStringInfoString(&func_inparas, NameOfDatum(&yylval.wdatum));
                    namedargnamses[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
                }
                else
                {
                    appendStringInfoString(&func_inparas, yylval.word.ident);
                    namedargnamses[nfields] = yylval.word.ident;
                }

                appendStringInfoString(&func_inparas, "=>");
                (void)yylex();
                
                yylex_outparam(fieldnames, varnos, nfields, &row, &rec, &tok, true);
                int loc = yylloc;
                tok = yylex();
                int curloc = yylloc;
                plbsql_push_back_token(tok);
                plpgsql_append_source_text(&func_inparas, loc, curloc);	
                
                tok = yylex();
                nparams++;
                namedarg[nfields] = true;
            }
            else
            {
                yylex_outparam(fieldnames, varnos, nfields, &row, &rec, &tok, true);
                plbsql_push_back_token(tok);
                yylex_inparam(&func_inparas, &nparams, &tok);
                namedarg[nfields] = false;
                namedargnamses[nfields] = NULL;
            }
            nfields++;
            if (')' == tok)
            {
                break;
            }
        }
    }
    appendStringInfoString(&func_inparas, ")");
    /* read the end token */
    if ((tok = yylex()) != ';')
    {
        if (!is_assign)
        {
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("when invoking function %s, maybe input something superfluous.", sqlstart)));
        }
        else
        {
            /* there may be other thing after the function invoke, just append it */
            plbsql_push_back_token(tok);
            expr = read_sql_construct(';', 0, 0, ";", "", true, false, true, NULL, &tok);
            appendStringInfoString(&func_inparas, expr->query);
            pfree_ext(expr->query);
            pfree_ext(expr);
        }
    }
    
    if (!multi_func && inArgNum - i >  ndefaultargs)
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function %s has no enough parameters", sqlstart)));

    (void)MemoryContextSwitchTo(oldCxt);

    /* generate the expression */
    expr 				= (PLpgSQL_expr*)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(func_inparas.data);
    expr->plan			= NULL;
    expr->paramnos 		= NULL;
    expr->ns			= plpgsql_ns_top();
    expr->idx			= (uint32)-1;

    if (multi_func)
    {
        PLpgSQL_function *function = NULL;
        PLpgSQL_execstate *estate = (PLpgSQL_execstate*)palloc(sizeof(PLpgSQL_execstate));
        expr->func = (PLpgSQL_function *) palloc0(sizeof(PLpgSQL_function));
        function = expr->func;
        function->fn_is_trigger = false;
        function->fn_input_collation = InvalidOid;
        function->out_param_varno = -1;		/* set up for no OUT param */
        function->resolve_option = (PLpgSQL_resolve_option)u_sess->plsql_cxt.plpgsql_variable_conflict;
        function->fn_cxt = CurrentMemoryContext;

        estate->ndatums = u_sess->plsql_cxt.plpgsql_nDatums;
        estate->datums = (PLpgSQL_datum **)palloc(sizeof(PLpgSQL_datum *) * u_sess->plsql_cxt.plpgsql_nDatums);
        for (int i = 0; i < u_sess->plsql_cxt.plpgsql_nDatums; i++)
            estate->datums[i] = u_sess->plsql_cxt.plpgsql_Datums[i];

        function->cur_estate = estate;
        function->cur_estate->func = function;
        
        Oid funcid = getMultiFuncInfo(func_inparas.data, expr);
        if (expr->func != NULL)
            pfree_ext(expr->func);
        if (estate->datums != NULL)
            pfree_ext(estate->datums);
        if (estate != NULL)
            pfree_ext(estate);
        expr->func = NULL;
        int	all_arg = 0;
        if (OidIsValid(funcid))
        {
            proctup = SearchSysCache(PROCOID,
                                     ObjectIdGetDatum(funcid),
                                     0, 0, 0);
            
            /*
             * function may be deleted after clist be searched.
             */
            if (!HeapTupleIsValid(proctup))
            {
                ereport(ERROR,
                        (errcode(ERRCODE_UNDEFINED_FUNCTION),
                         errmsg("function \"%s\" doesn't exist ", sqlstart)));
                return NULL;
            }
            /* get the all args informations, only "in" parameters if p_argmodes is null */
            all_arg = get_func_arg_info(proctup, &p_argtypes, &p_argnames, &p_argmodes);
            procStruct = (Form_pg_proc) GETSTRUCT(proctup);
            ndefaultargs = procStruct->pronargdefaults;
            narg = procStruct->pronargs;
            ReleaseSysCache(proctup);

            /* if there is no "out" parameters ,use perform stmt,others use exesql */
            if ((0 == all_arg || NULL == p_argmodes))
            {
                PLpgSQL_stmt_perform *perform = NULL;
                perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
                perform->cmd_type = PLPGSQL_STMT_PERFORM;
                perform->lineno   = plpgsql_location_to_lineno(location);
                perform->expr	  = expr;
                return (PLpgSQL_stmt *)perform;
            }
            else if (all_arg >= narg)
            {
                if (NULL == rec &&  NULL == row)
                {
                    int new_nfields = 0;
                    int i = 0, j = 0;
                    for (i = 0; i < all_arg; i++)
                    {
                        if (p_argmodes[i] == 'i')
                            continue;	
                        else
                            new_nfields++;
                    }
                    row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
                    row->dtype = PLPGSQL_DTYPE_ROW;
                    row->refname = pstrdup("*internal*");
                    row->lineno = plpgsql_location_to_lineno(location);
                    row->rowtupdesc = NULL;
                    row->nfields = new_nfields;
                    row->fieldnames = (char **)palloc0(sizeof(char *) * new_nfields);
                    row->varnos = (int *)palloc0(sizeof(int) * new_nfields);

                    /* fetch out argument from fieldnames into row->fieldnames */
                    for (i = 0; i < nfields; i++)
                    {
                        if (namedarg[i] == false)
                        {
                            if (p_argmodes[i] == 'i')
                                continue;
                            else if (varnos[i] >= 0)
                            {
                                row->fieldnames[j] = fieldnames[i];
                                row->varnos[j] = varnos[i];
                                j++;
                            }
                        }
                        else
                        {
                            char argmode = FUNC_PARAM_IN;
                            pos_outer = -1;
                            for (int curpos = 0; curpos < all_arg; curpos++)
                            {
                                char* paraname = p_argnames[curpos];

                                if (p_argmodes[curpos] != 'i')
                                    pos_outer++;
                                else
                                {
                                    continue;
                                }

                                if (paraname != NULL && !strcmp(paraname, namedargnamses[i]))
                                {
                                    if (!p_argmodes)
                                        argmode = FUNC_PARAM_IN;
                                    else
                                        argmode = p_argmodes[curpos];

                                    break;
                                }
                            }
                            
                            if (argmode == 'i')
                                continue;

                            if (fieldnames[i] == NULL)
                            {
                                ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                        errmsg("Named argument \"%s\" can not be a const", namedargnamses[i])));
                            }

                            if (row->varnos[pos_outer] > 0)
                            {
                                ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("parameter \"%s\" is assigned more than once", row->fieldnames[pos_outer])));
                            }
                            
                            if (varnos[i] >= 0)
                            {
                                row->fieldnames[pos_outer] = fieldnames[i];
                                row->varnos[pos_outer] = varnos[i];
                            }
                        }
                    }
                    plpgsql_adddatum((PLpgSQL_datum *)row);
                }

                PLpgSQL_stmt_execsql * execsql = NULL;
                execsql = (PLpgSQL_stmt_execsql *)palloc(sizeof(PLpgSQL_stmt_execsql));
                execsql->cmd_type = PLPGSQL_STMT_EXECSQL;
                execsql->lineno  = plpgsql_location_to_lineno(location);
                execsql->sqlstmt = expr;
                execsql->into	 = row || rec ? true:false;
                execsql->strict  = true;
                execsql->rec	 = rec;
                execsql->row	 = row;
                execsql->placeholders = placeholders;
                execsql->multi_func = multi_func;
                return (PLpgSQL_stmt *)execsql;
            }
        }
        else
        {
            ereport(ERROR,
            (errcode(ERRCODE_DUPLICATE_FUNCTION),
             errmsg("function \"%s\" isn't exclusive ", sqlstart)));
        }
    }
    else
    {
        /* if have out parameters, generate row type to store the results. */
        if (nfields && NULL == rec &&  NULL == row)
        {
            row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
            row->dtype = PLPGSQL_DTYPE_ROW;
            row->refname = pstrdup("*internal*");
            row->lineno = plpgsql_location_to_lineno(location);
            row->rowtupdesc = NULL;
            row->nfields = nfields;
            row->fieldnames = (char **)palloc(sizeof(char *) * nfields);
            row->varnos = (int *)palloc(sizeof(int) * nfields);
            while (--nfields >= 0)
            {
                row->fieldnames[nfields] = fieldnames[nfields];
                row->varnos[nfields] = varnos[nfields];
            }
            plpgsql_adddatum((PLpgSQL_datum *)row);
        }
        
        /* if there is no "out" parameters ,use perform stmt,others use exesql */
        if (0 == narg || NULL == p_argmodes)
        {
            PLpgSQL_stmt_perform *perform = NULL;
            perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
            perform->cmd_type = PLPGSQL_STMT_PERFORM;
            perform->lineno   = plpgsql_location_to_lineno(location);
            perform->expr	  = expr;
            return (PLpgSQL_stmt *)perform;
        }
        else
        {
            PLpgSQL_stmt_execsql * execsql = NULL;
            execsql = (PLpgSQL_stmt_execsql *)palloc(sizeof(PLpgSQL_stmt_execsql));
            execsql->cmd_type = PLPGSQL_STMT_EXECSQL;
            execsql->lineno  = plpgsql_location_to_lineno(location);
            execsql->sqlstmt = expr;
            execsql->into	 = row || rec ? true:false;
            execsql->strict  = true;
            execsql->rec	 = rec;
            execsql->row	 = row;
            execsql->placeholders = placeholders;
            execsql->multi_func = multi_func;
            return (PLpgSQL_stmt *)execsql;
        }
    }

    pfree_ext(func_inparas.data);
    /* skip compile warning */
    return NULL;
}

/*
 * Brief		: check if it is an function invoke
 * Description	: 
 * in name: the ident name
 * in is_assign: is an assign stmt or not.
 * in no_parenthesis: if no-parenthesis function is called.
 * returns: true if is a function.
 * Notes		: 
 */ 
static bool 
is_function(const char *name, bool is_assign, bool no_parenthesis)
{
    ScanKeyword * keyword = NULL;
    List *funcname = NIL;
    FuncCandidateList clist = NULL;
    bool have_outargs = false;
    char **p_argnames = NULL;
    char *p_argmodes = NULL;
    Oid *p_argtypes = NULL;
    HeapTuple proctup = NULL;
    int narg = 0;
    int i = 0;
    char *cp[3] = {0};

    /* the function is_function is only to judge if it's a function call, so memory used in it is all temp */
    AutoContextSwitch plCompileCxtGuard(u_sess->plsql_cxt.compile_tmp_cxt);

    plpgsql_parser_funcname(name, cp, 3);

    /* 
     * if A.B.C, it's not a function invoke, because can not use function of 
     * other database. if A.B, then it must be a function invoke. 
     */
    if (cp[2] && cp[2][0] != '\0')
        return false;
    else if (cp[0] && cp[0][0] != '\0')
    {
        /* these are function in pg_proc who are overloaded, so they can not be
         * execured by := or direct call.
         */
        if (pg_strcasecmp("ts_stat", cp[0]) == 0 ||
            pg_strcasecmp("ts_token_type", cp[0]) == 0 ||
            pg_strcasecmp("ts_parse", cp[0]) == 0 ||
            pg_strcasecmp("dblink_get_notify", cp[0]) == 0 ||
            pg_strcasecmp("ts_debug", cp[0]) == 0)
            return false;

        keyword = (ScanKeyword *)ScanKeywordLookup(cp[0], ScanKeywords, NumScanKeywords);
        /* function name can not be reserved keyword */
        if (keyword && RESERVED_KEYWORD == keyword->category)
            return false;
        /* and function name can not be unreserved keyword when no-parenthesis function is called.*/
        if(keyword && no_parenthesis && UNRESERVED_KEYWORD == keyword->category)
            return false;

        if (cp[1] && cp[1][0] != '\0')
            funcname = list_make2(makeString(cp[0]), makeString(cp[1]));
        else
            funcname = list_make1(makeString(cp[0]));
        /* search the function */
        clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
        if (!clist)
        {
            if (!is_assign)
                ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_FUNCTION),
                     errmsg("function \"%s\" doesn't exist ", name)));
            return false;
        }
        else if (clist->next)
        {
            if (is_assign)
                return false;
            else
                return true;
        }
        else
        {
            proctup = SearchSysCache(PROCOID,
                             ObjectIdGetDatum(clist->oid),
                             0, 0, 0);
            if (!HeapTupleIsValid(proctup))
            {
                if (!is_assign)
                    ereport(ERROR,
                        (errcode(ERRCODE_UNDEFINED_FUNCTION),
                         errmsg("function \"%s\" doesn't exist ", name)));
                return false;
            }

            /* get the all args informations, only "in" parameters if p_argmodes is null */
            narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
            if (p_argmodes)
            {
                for (i = 0; i < narg; i++)
                {
                    if ('o' == p_argmodes[i])
                    {
                        have_outargs = true;
                        break;
                    }
                }
            }
            ReleaseSysCache(proctup);
        }

        if (!have_outargs && is_assign)
            return false;
        
        return true;/* passed all test */
    }
    return false;
}

/* Convenience routine to read an expression with one possible terminator */
static PLpgSQL_expr *
read_sql_expression(int until, const char *expected)
{
    return read_sql_construct(until, 0, 0, expected,
                              "SELECT ", true, true, true, NULL, NULL);
}

/* Convenience routine to read an expression with two possible terminators */
static PLpgSQL_expr *
read_sql_expression2(int until, int until2, const char *expected,
                     int *endtoken)
{
    return read_sql_construct(until, until2, 0, expected,
                              "SELECT ", true, true, true, NULL, endtoken);
}

/* Convenience routine to read a SQL statement that must end with ';' */
static PLpgSQL_expr *
read_sql_stmt(const char *sqlstart)
{
    return read_sql_construct(';', 0, 0, ";",
                              sqlstart, false, true, true, NULL, NULL);
}

/*
 * Brief		: read a sql construct who has 4 expected tokens.
 * Description	: just like read_sql_construct function below, except that 
 * 				  this function has 4 expected tokens.
 * Notes		:
 */ 

/*
 * Read a SQL construct and build a PLpgSQL_expr for it.
 *
 * until:		token code for expected terminator
 * until2:		token code for alternate terminator (pass 0 if none)
 * until3:		token code for another alternate terminator (pass 0 if none)
 * until4:		token code for alternate terminator (pass 0 if none)
 * until5:		token code for alternate terminator (pass 0 if none)
 * expected:	text to use in complaining that terminator was not found
 * sqlstart:	text to prefix to the accumulated SQL text
 * isexpression: whether to say we're reading an "expression" or a "statement"
 * valid_sql:   whether to check the syntax of the expr (prefixed with sqlstart)
 * trim:		trim trailing whitespace
 * startloc:	if not NULL, location of first token is stored at *startloc
 * endtoken:	if not NULL, ending token is stored at *endtoken
 *				(this is only interesting if until2 or until3 isn't zero)
 */
static PLpgSQL_expr *
read_sql_construct5(int until,
                   int until2,
                   int until3,
                   int until4,
                   int until5,
                   const char *expected,
                   const char *sqlstart,
                   bool isexpression,
                   bool valid_sql,
                   bool trim,
                   int *startloc,
                   int *endtoken)
{
    int					tok;
    StringInfoData		ds;
    IdentifierLookup	save_IdentifierLookup;
    int					startlocation = -1;
    int					parenlevel = 0;
    PLpgSQL_expr		*expr;
    MemoryContext		oldCxt = NULL;

    /* 
     * buf stores the cursor attribute internal variables or 
     * varray ident with operate function, so NAMEDATALEN + 128 is enough 
     */
    char				buf[NAMEDATALEN + 128];
    bool				ds_changed = false;
    List				*list_left_bracket = 0;
    List*				list_right_bracket = 0;
    const char			left_bracket[2] = "[";
    const char			right_bracket[2] = "]";
    const char			left_brack[2] = "(";
    const char			right_brack[2] = ")";
    int					loc = 0;
    int					curloc = 0;

    /*
     * ds will do a lot of enlarge, which need to relloc the space, and the old space 
     * will be return to current context. So if we don't switch the context, the original 
     * context will be Plpgsql function context, which has a long term life cycle, 
     * may cause memory accumulation.
     */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);
    initStringInfo(&ds);
    MemoryContextSwitchTo(oldCxt);

    appendStringInfoString(&ds, sqlstart);

    /* special lookup mode for identifiers within the SQL text */
    save_IdentifierLookup = u_sess->plsql_cxt.plpgsql_IdentifierLookup;
    u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_EXPR;

    for (;;)
    {
        tok = yylex();
        loc = yylloc;
        if (startlocation < 0)			/* remember loc of first token */
            startlocation = yylloc;
        if (tok == until && parenlevel == 0)
            break;
        if (tok == until2 && parenlevel == 0)
            break;
        if (tok == until3 && parenlevel == 0)
            break;
        if (tok == until4 && parenlevel == 0)
            break;
        if (tok == until5 && parenlevel == 0)
            break;
        if (tok == '(' || tok == '[')
            parenlevel++;
        else if (tok == ')' || tok == ']')
        {
            parenlevel--;
            if (parenlevel < 0)
                yyerror("mismatched parentheses");
        }
        /*
         * End of function definition is an error, and we don't expect to
         * hit a semicolon either (unless it's the until symbol, in which
         * case we should have fallen out above).
         */
        if (tok == 0 || tok == ';')
        {
            if (parenlevel != 0)
                yyerror("mismatched parentheses");
            if (isexpression)
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("missing \"%s\" at end of SQL expression",
                                expected),
                         parser_errposition(yylloc)));
            else
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("missing \"%s\" at end of SQL statement",
                                expected),
                         parser_errposition(yylloc)));
        }

        switch (tok)
        {		     
            case T_SQL_FOUND:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_found__ ");
                ds_changed = true;
                break;
            case T_CURSOR_FOUND:	
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_found__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_SQL_NOTFOUND:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_notfound__ ");
                ds_changed = true;
                break;
            case T_CURSOR_NOTFOUND:
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_notfound__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_SQL_ISOPEN:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_isopen__ ");
                ds_changed = true;
                break;
            case T_CURSOR_ISOPEN:
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_isopen__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_SQL_ROWCOUNT:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_rowcount__ ");
                ds_changed = true;
                break;
            case T_CURSOR_ROWCOUNT:
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_rowcount__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_VARRAY_VAR:
                tok = yylex();
                if('(' == tok)
                {
                    list_left_bracket = lcons_int(parenlevel,list_left_bracket);
                    list_right_bracket = lcons_int(parenlevel,list_right_bracket);
                }	
                curloc = yylloc;
                plbsql_push_back_token(tok);
                plpgsql_append_source_text(&ds, loc, curloc);
                ds_changed = true;	
                break;
            case T_ARRAY_FIRST:
                appendStringInfo(&ds, " ARRAY_LOWER(%s, 1) ", ((Value *)linitial(yylval.wdatum.idents))->val.str);
                ds_changed = true;
                break;
            case T_ARRAY_LAST:
                appendStringInfo(&ds, " ARRAY_UPPER(%s, 1) ", ((Value *)linitial(yylval.wdatum.idents))->val.str);
                ds_changed = true;
                break;
            case T_ARRAY_COUNT:
                appendStringInfo(&ds, " ARRAY_LENGTH(%s, 1) ", ((Value *)linitial(yylval.wdatum.idents))->val.str);
                ds_changed = true;
                break;
            case ')':
                if (list_right_bracket && list_right_bracket->length
                    && linitial_int(list_right_bracket) == parenlevel)
                {
                    appendStringInfoString(&ds, right_bracket);
                    list_right_bracket = list_delete_first(list_right_bracket);
                }
                else
                    appendStringInfoString(&ds,right_brack);
                ds_changed = true;
                break;
            case '(':
                if (list_left_bracket && list_left_bracket->length
                    && linitial_int(list_left_bracket) == parenlevel-1)
                {
                    appendStringInfoString(&ds, left_bracket);
                    list_left_bracket = list_delete_first(list_left_bracket);
                }
                else
                    appendStringInfoString(&ds, left_brack);
                ds_changed = true;
                break;
            case T_VARRAY:
                if ('(' == yylex() && ')' == yylex())
                {
                    snprintf(buf, sizeof(buf), " NULL ");
                    appendStringInfoString(&ds, buf);
                    ds_changed = true;
                }
                else
                {
                    /* do nothing */
                }
                break;
            default:
                tok = yylex();

                if(tok > INT_MAX)
                {
                    ereport(ERROR,
                            (errcode(ERRCODE_UNDEFINED_FUNCTION),
                             errmsg("token value %d is bigger than INT_MAX ", tok)));
                }

                curloc = yylloc;
                plbsql_push_back_token(tok);
                plpgsql_append_source_text(&ds, loc, curloc);
                ds_changed = true;
                break;
        }
    }

    u_sess->plsql_cxt.plpgsql_IdentifierLookup = save_IdentifierLookup;

    if (startloc)
        *startloc = startlocation;
    if (endtoken)
        *endtoken = tok;

    /* give helpful complaint about empty input */
    if (startlocation >= yylloc)
    {
        if (isexpression)
            yyerror("missing expression");
        else
            yyerror("missing SQL statement");
    }

    if (!ds_changed)
        plpgsql_append_source_text(&ds, startlocation, yylloc);
    
    /* trim any trailing whitespace, for neatness */
    if (trim)
    {
        while (ds.len > 0 && scanner_isspace(ds.data[ds.len - 1]))
            ds.data[--ds.len] = '\0';
    }

    expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(ds.data);
    expr->plan			= NULL;
    expr->paramnos		= NULL;
    expr->ns			= plpgsql_ns_top();
    expr->isouttype 		= false;
    expr->idx			= (uint32)-1;

    pfree_ext(ds.data);

    if (valid_sql)
        check_sql_expr(expr->query, startlocation, strlen(sqlstart));

    return expr;
}

/*
 * Read a SQL construct and build a PLpgSQL_expr for it.
 *
 * until:		token code for expected terminator
 * until2:		token code for alternate terminator (pass 0 if none)
 * until3:		token code for another alternate terminator (pass 0 if none)
 * expected:	text to use in complaining that terminator was not found
 * sqlstart:	text to prefix to the accumulated SQL text
 * isexpression: whether to say we're reading an "expression" or a "statement"
 * valid_sql:   whether to check the syntax of the expr (prefixed with sqlstart)
 * trim:		trim trailing whitespace
 * startloc:	if not NULL, location of first token is stored at *startloc
 * endtoken:	if not NULL, ending token is stored at *endtoken
 *				(this is only interesting if until2 or until3 isn't zero)
 */
static PLpgSQL_expr *
read_sql_construct(int until,
                   int until2,
                   int until3,
                   const char *expected,
                   const char *sqlstart,
                   bool isexpression,
                   bool valid_sql,
                   bool trim,
                   int *startloc,
                   int *endtoken)
{
    return read_sql_construct5(until, until2, until3, until3, 0,
                               expected, sqlstart, isexpression, valid_sql, trim, startloc, endtoken);
}

static PLpgSQL_type *
read_datatype(int tok)
{
    StringInfoData		ds;
    char			   *type_name;
    int					startlocation;
    PLpgSQL_type		*result;
    int					parenlevel = 0;

    /* Should only be called while parsing DECLARE sections */
    AssertEreport(u_sess->plsql_cxt.plpgsql_IdentifierLookup == IDENTIFIER_LOOKUP_DECLARE,
                        MOD_PLSQL,
                        "Should only be called while parsing DECLARE sections");

    /* Often there will be a lookahead token, but if not, get one */
    if (tok == YYEMPTY)
        tok = yylex();

    startlocation = yylloc;

    /*
     * If we have a simple or composite identifier, check for %TYPE
     * and %ROWTYPE constructs.
     */
    if (tok == T_WORD)
    {
        char   *dtname = yylval.word.ident;

        tok = yylex();
        if (tok == '%')
        {
            tok = yylex();
            if (tok_is_keyword(tok, &yylval,
                               K_TYPE, "type"))
            {
                result = plpgsql_parse_wordtype(dtname);
                if (result)
                    return result;
            }
            else if (tok_is_keyword(tok, &yylval,
                                    K_ROWTYPE, "rowtype"))
            {
                PLpgSQL_nsitem *ns;
                ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, 
                            dtname, NULL, NULL, NULL);
                if (ns && ns->itemtype == PLPGSQL_NSTYPE_VAR)
                {
                    PLpgSQL_var* var = (PLpgSQL_var*)u_sess->plsql_cxt.plpgsql_Datums[ns->itemno];
                    if(var && var->datatype 
                           && var->datatype->typoid == REFCURSOROID)
                    {
                        return plpgsql_build_datatype(RECORDOID, -1, InvalidOid);
                    }
                }
                result = plpgsql_parse_wordrowtype(dtname);
                if (result)
                    return result;
            }
        }
    }
    else if (tok == T_CWORD)
    {
        List   *dtnames = yylval.cword.idents;

        tok = yylex();
        if (tok == '%')
        {
            tok = yylex();
            if (tok_is_keyword(tok, &yylval,
                               K_TYPE, "type"))
            {
                result = plpgsql_parse_cwordtype(dtnames);
                if (result)
                    return result;
            }
            else if (tok_is_keyword(tok, &yylval,
                                    K_ROWTYPE, "rowtype"))
            {
                result = plpgsql_parse_cwordrowtype(dtnames);
                if (result)
                    return result;
            }
        }
    }

    while (tok != ';')
    {
        if (tok == 0)
        {
            if (parenlevel != 0)
                yyerror("mismatched parentheses");
            else
                yyerror("incomplete data type declaration");
        }
        /* Possible followers for datatype in a declaration */
        if (tok == K_COLLATE || tok == K_NOT ||
            tok == '=' || tok == COLON_EQUALS || tok == K_DEFAULT)
            break;
        /* Possible followers for datatype in a cursor_arg list */
        if ((tok == ',' || tok == ')') && parenlevel == 0)
            break;
        if (tok == '(')
            parenlevel++;
        else if (tok == ')')
            parenlevel--;

        tok = yylex();
    }

    /* set up ds to contain complete typename text */
    initStringInfo(&ds);
    plpgsql_append_source_text(&ds, startlocation, yylloc);
    type_name = ds.data;

    if (type_name[0] == '\0')
        yyerror("missing data type declaration");

    result = parse_datatype(type_name, startlocation);

    pfree_ext(ds.data);

    plbsql_push_back_token(tok);

    return result;
}

static PLpgSQL_stmt *
make_execsql_stmt(int firsttoken, int location)
{
    StringInfoData		ds;
    IdentifierLookup	save_IdentifierLookup;
    PLpgSQL_stmt_execsql *execsql;
    PLpgSQL_expr		*expr;
    PLpgSQL_row			*row = NULL;
    PLpgSQL_rec			*rec = NULL;
    int					tok;
    int					prev_tok;
    bool				have_into = false;
    bool				have_strict = false;
    int					into_start_loc = -1;
    int					into_end_loc = -1;
    int					placeholders = 0;

    initStringInfo(&ds);

    /* special lookup mode for identifiers within the SQL text */
    save_IdentifierLookup = u_sess->plsql_cxt.plpgsql_IdentifierLookup;
    u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_EXPR;

    /*
     * Scan to the end of the SQL command.  Identify any INTO-variables
     * clause lurking within it, and parse that via read_into_target().
     *
     * Because INTO is sometimes used in the main SQL grammar, we have to be
     * careful not to take any such usage of INTO as a PL/pgSQL INTO clause.
     * There are currently three such cases:
     *
     * 1. SELECT ... INTO.  We don't care, we just override that with the
     * PL/pgSQL definition.
     *
     * 2. INSERT INTO.  This is relatively easy to recognize since the words
     * must appear adjacently; but we can't assume INSERT starts the command,
     * because it can appear in CREATE RULE or WITH.  Unfortunately, INSERT is
     * *not* fully reserved, so that means there is a chance of a false match;
     * but it's not very likely.
     *
     * 3. ALTER TABLE PARTITION MERGE or SPLIT INTO.
     * We just check for ALTER as the command's first token.
     *
     * Fortunately, INTO is a fully reserved word in the main grammar, so
     * at least we need not worry about it appearing as an identifier.
     *
     * Any future additional uses of INTO in the main grammar will doubtless
     * break this logic again ... beware!
     */
    tok = firsttoken;

    for (;;)
    {
        prev_tok = tok;
        tok = yylex();

        if (have_into && into_end_loc < 0)
            into_end_loc = yylloc;		/* token after the INTO part */
        if (tok == ';')
            break;
        if (tok == 0)
            yyerror("unexpected end of function definition");

        if(tok == T_PLACEHOLDER)
        {
            placeholders++;
        }

        if (tok == K_INTO)
        {
            if (prev_tok == K_INSERT)
                continue;	/* INSERT INTO is not an INTO-target */
            if (firsttoken == K_ALTER)
                continue;	/* ALTER ... INTO is not an INTO-target */
            if (prev_tok == K_MERGE)
                continue;	/* MERGE INTO is not an INTO-target */
            if (have_into)
                yyerror("INTO specified more than once");
            have_into = true;
            into_start_loc = yylloc;
            u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
            read_into_target(&rec, &row, &have_strict);
            u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_EXPR;
        }
    }

    u_sess->plsql_cxt.plpgsql_IdentifierLookup = save_IdentifierLookup;

    if (have_into)
    {
        /*
         * Insert an appropriate number of spaces corresponding to the
         * INTO text, so that locations within the redacted SQL statement
         * still line up with those in the original source text.
         */
        plpgsql_append_source_text(&ds, location, into_start_loc);
        appendStringInfoSpaces(&ds, into_end_loc - into_start_loc);
        plpgsql_append_source_text(&ds, into_end_loc, yylloc);
    }
    else
        plpgsql_append_source_text(&ds, location, yylloc);

    /* trim any trailing whitespace, for neatness */
    while (ds.len > 0 && scanner_isspace(ds.data[ds.len - 1]))
        ds.data[--ds.len] = '\0';

    expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(ds.data);
    expr->plan			= NULL;
    expr->paramnos		= NULL;
    expr->ns			= plpgsql_ns_top();
    expr->idx			= (uint32)-1;
    pfree_ext(ds.data);

    check_sql_expr(expr->query, location, 0);

    execsql = (PLpgSQL_stmt_execsql *)palloc(sizeof(PLpgSQL_stmt_execsql));
    execsql->cmd_type = PLPGSQL_STMT_EXECSQL;
    execsql->lineno  = plpgsql_location_to_lineno(location);
    execsql->sqlstmt = expr;
    execsql->into	 = have_into;
    execsql->strict	 = have_strict;
    execsql->rec	 = rec;
    execsql->row	 = row;
    execsql->placeholders = placeholders;

    return (PLpgSQL_stmt *) execsql;
}


/*
 * Read FETCH or MOVE direction clause (everything through FROM/IN).
 */
static PLpgSQL_stmt_fetch *
read_fetch_direction(void)
{
    PLpgSQL_stmt_fetch *fetch;
    int			tok;
    bool		check_FROM = true;

    /*
     * We create the PLpgSQL_stmt_fetch struct here, but only fill in
     * the fields arising from the optional direction clause
     */
    fetch = (PLpgSQL_stmt_fetch *) palloc0(sizeof(PLpgSQL_stmt_fetch));
    fetch->cmd_type = PLPGSQL_STMT_FETCH;
    /* set direction defaults: */
    fetch->direction = FETCH_FORWARD;
    fetch->how_many  = 1;
    fetch->expr		 = NULL;
    fetch->returns_multiple_rows = false;

    tok = yylex();
    if (tok == 0)
        yyerror("unexpected end of function definition");

    if (tok_is_keyword(tok, &yylval,
                       K_NEXT, "next"))
    {
        /* use defaults */
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_PRIOR, "prior"))
    {
        fetch->direction = FETCH_BACKWARD;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_FIRST, "first"))
    {
        fetch->direction = FETCH_ABSOLUTE;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_LAST, "last"))
    {
        fetch->direction = FETCH_ABSOLUTE;
        fetch->how_many  = -1;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_ABSOLUTE, "absolute"))
    {
        fetch->direction = FETCH_ABSOLUTE;
        fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                           "FROM or IN",
                                           NULL);
        check_FROM = false;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_RELATIVE, "relative"))
    {
        fetch->direction = FETCH_RELATIVE;
        fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                           "FROM or IN",
                                           NULL);
        check_FROM = false;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_ALL, "all"))
    {
        fetch->how_many = FETCH_ALL;
        fetch->returns_multiple_rows = true;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_FORWARD, "forward"))
    {
        complete_direction(fetch, &check_FROM);
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_BACKWARD, "backward"))
    {
        fetch->direction = FETCH_BACKWARD;
        complete_direction(fetch, &check_FROM);
    }
    else if (tok == K_FROM || tok == K_IN)
    {
        /* empty direction */
        check_FROM = false;
    }
    else if (tok == T_DATUM)
    {
        /* Assume there's no direction clause and tok is a cursor name */
        plbsql_push_back_token(tok);
        check_FROM = false;
    }
    else
    {
        /*
         * Assume it's a count expression with no preceding keyword.
         * Note: we allow this syntax because core SQL does, but we don't
         * document it because of the ambiguity with the omitted-direction
         * case.  For instance, "MOVE n IN c" will fail if n is a variable.
         * Perhaps this can be improved someday, but it's hardly worth a
         * lot of work.
         */
        plbsql_push_back_token(tok);
        fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                           "FROM or IN",
                                           NULL);
        fetch->returns_multiple_rows = true;
        check_FROM = false;
    }

    /* check FROM or IN keyword after direction's specification */
    if (check_FROM)
    {
        tok = yylex();
        if (tok != K_FROM && tok != K_IN)
            yyerror("expected FROM or IN");
    }

    return fetch;
}

/*
 * Process remainder of FETCH/MOVE direction after FORWARD or BACKWARD.
 * Allows these cases:
 *   FORWARD expr,  FORWARD ALL,  FORWARD
 *   BACKWARD expr, BACKWARD ALL, BACKWARD
 */
static void
complete_direction(PLpgSQL_stmt_fetch *fetch,  bool *check_FROM)
{
    int			tok;

    tok = yylex();
    if (tok == 0)
        yyerror("unexpected end of function definition");

    if (tok == K_FROM || tok == K_IN)
    {
        *check_FROM = false;
        return;
    }

    if (tok == K_ALL)
    {
        fetch->how_many = FETCH_ALL;
        fetch->returns_multiple_rows = true;
        *check_FROM = true;
        return;
    }

    plbsql_push_back_token(tok);
    fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                       "FROM or IN",
                                       NULL);
    fetch->returns_multiple_rows = true;
    *check_FROM = false;
}


static PLpgSQL_stmt *
make_return_stmt(int location)
{
    PLpgSQL_stmt_return *newp;
    int token = -1;

    newp = (PLpgSQL_stmt_return *)palloc0(sizeof(PLpgSQL_stmt_return));
    newp->cmd_type = PLPGSQL_STMT_RETURN;
    newp->lineno   = plpgsql_location_to_lineno(location);
    newp->expr	  = NULL;
    newp->retvarno = -1;

    if (u_sess->plsql_cxt.plpgsql_curr_compile->fn_retset)
    {
        if (yylex() != ';')
            ereport(ERROR,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("RETURN cannot have a parameter in function returning set"),
                     errhint("Use RETURN NEXT or RETURN QUERY."),
                     parser_errposition(yylloc)));
    }

    // adapting A db, where return value is independent from OUT parameters 
    else if (';' == (token = yylex()) && u_sess->plsql_cxt.plpgsql_curr_compile->out_param_varno >= 0)
        newp->retvarno = u_sess->plsql_cxt.plpgsql_curr_compile->out_param_varno;
    else
    {
        plbsql_push_back_token(token);
        if (u_sess->plsql_cxt.plpgsql_curr_compile->fn_rettype == VOIDOID)
        {
            if (yylex() != ';')
                ereport(ERROR,
                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                         errmsg("RETURN cannot have a parameter in function returning void"),
                         parser_errposition(yylloc)));
        }
        else if (u_sess->plsql_cxt.plpgsql_curr_compile->fn_retistuple)
        {
            int     tok = yylex();
            if(tok < 0)
            {
                ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("token value %d is smaller than 0 ", tok)));
                return NULL;
            }

            switch (tok)
            {
                case K_NULL:
                    /* we allow this to support RETURN NULL in triggers */
                    break;

                case T_DATUM:
                    if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                        yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC)
                        newp->retvarno = yylval.wdatum.datum->dno;
                    else
                        ereport(ERROR,
                                (errcode(ERRCODE_DATATYPE_MISMATCH),
                                 errmsg("RETURN must specify a record or row variable in function returning row"),
                                 parser_errposition(yylloc)));
                    break;

                default:
                    ereport(ERROR,
                            (errcode(ERRCODE_DATATYPE_MISMATCH),
                             errmsg("RETURN must specify a record or row variable in function returning row"),
                             parser_errposition(yylloc)));
                    break;
            }
            if (yylex() != ';')
            yyerror("syntax error");
        }
        else
        {
            /*
             * Note that a well-formed expression is _required_ here;
             * anything else is a compile-time error.
             */
            newp->expr = read_sql_expression(';', ";");
        }
    }

    return (PLpgSQL_stmt *) newp;
}


static PLpgSQL_stmt *
make_return_next_stmt(int location)
{
    PLpgSQL_stmt_return_next *newp;

    if (!u_sess->plsql_cxt.plpgsql_curr_compile->fn_retset)
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                 errmsg("cannot use RETURN NEXT in a non-SETOF function"),
                 parser_errposition(location)));

    newp = (PLpgSQL_stmt_return_next *)palloc0(sizeof(PLpgSQL_stmt_return_next));
    newp->cmd_type	= PLPGSQL_STMT_RETURN_NEXT;
    newp->lineno		= plpgsql_location_to_lineno(location);
    newp->expr		= NULL;
    newp->retvarno	= -1;

    if (u_sess->plsql_cxt.plpgsql_curr_compile->out_param_varno >= 0)
    {
        if (yylex() != ';')
            ereport(ERROR,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("RETURN NEXT cannot have a parameter in function with OUT parameters"),
                     parser_errposition(yylloc)));
        newp->retvarno = u_sess->plsql_cxt.plpgsql_curr_compile->out_param_varno;
    }
    else if (u_sess->plsql_cxt.plpgsql_curr_compile->fn_retistuple)
    {
        switch (yylex())
        {
            case T_DATUM:
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC)
                    newp->retvarno = yylval.wdatum.datum->dno;
                else
                    ereport(ERROR,
                            (errcode(ERRCODE_DATATYPE_MISMATCH),
                             errmsg("RETURN NEXT must specify a record or row variable in function returning row"),
                             parser_errposition(yylloc)));
                break;

            default:
                ereport(ERROR,
                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                         errmsg("RETURN NEXT must specify a record or row variable in function returning row"),
                         parser_errposition(yylloc)));
                break;
        }
        if (yylex() != ';')
            yyerror("syntax error");
    }
    else
        newp->expr = read_sql_expression(';', ";");

    return (PLpgSQL_stmt *) newp;
}


static PLpgSQL_stmt *
make_return_query_stmt(int location)
{
    PLpgSQL_stmt_return_query *newp;
    int			tok;

    if (!u_sess->plsql_cxt.plpgsql_curr_compile->fn_retset)
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                 errmsg("cannot use RETURN QUERY in a non-SETOF function"),
                 parser_errposition(location)));

    newp = (PLpgSQL_stmt_return_query *)palloc0(sizeof(PLpgSQL_stmt_return_query));
    newp->cmd_type = PLPGSQL_STMT_RETURN_QUERY;
    newp->lineno = plpgsql_location_to_lineno(location);

    /* check for RETURN QUERY EXECUTE */
    if ((tok = yylex()) != K_EXECUTE)
    {
        /* ordinary static query */
        plbsql_push_back_token(tok);
        newp->query = read_sql_stmt("");
    }
    else
    {
        /* dynamic SQL */
        int		term;

        newp->dynquery = read_sql_expression2(';', K_USING, "; or USING",
                                             &term);
        if (term == K_USING)
        {
            do
            {
                PLpgSQL_expr *expr;

                expr = read_sql_expression2(',', ';', ", or ;", &term);
                newp->params = lappend(newp->params, expr);
            } while (term == ',');
        }
    }

    return (PLpgSQL_stmt *) newp;
}


/* convenience routine to fetch the name of a T_DATUM */
static char *
NameOfDatum(PLwdatum *wdatum)
{
    if (wdatum->ident)
        return wdatum->ident;
    AssertEreport(wdatum->idents != NIL,
                        MOD_PLSQL,
                        "It should not be null");
    return NameListToString(wdatum->idents);
}

static void
check_assignable(PLpgSQL_datum *datum, int location)
{
    switch (datum->dtype)
    {
        case PLPGSQL_DTYPE_VAR:
            if (((PLpgSQL_var *) datum)->isconst)
                ereport(ERROR,
                        (errcode(ERRCODE_ERROR_IN_ASSIGNMENT),
                         errmsg("\"%s\" is declared CONSTANT",
                                ((PLpgSQL_var *) datum)->refname),
                         parser_errposition(location)));
            break;
        case PLPGSQL_DTYPE_ROW:
            /* always assignable? */
            break;
        case PLPGSQL_DTYPE_REC:
            /* always assignable?  What about NEW/OLD? */
            break;
        case PLPGSQL_DTYPE_RECORD:
            break;
        case PLPGSQL_DTYPE_RECFIELD:
            /* always assignable? */
            break;
        case PLPGSQL_DTYPE_ARRAYELEM:
            /* always assignable? */
            break;
        default:
            elog(ERROR, "unrecognized dtype: %d", datum->dtype);
            break;
    }
}

/*
 * Brief		: support array variable as select into and using out target. 
 * Description	: if an array element is detected, add it to the u_sess->plsql_cxt.plpgsql_Datums[]
 *				  and return true, else return false.  
 * Notes		: 
 */ 
static bool 
read_into_using_add_arrayelem(char **fieldnames, int *varnos, int *nfields, int tmpdno, int *tok)
{
    PLpgSQL_arrayelem *newp = NULL;
    char           tokExpected[2];
    int            toktmp = 0;

    /* validation of arguments */
    if ((NULL == fieldnames) || (NULL == varnos) || (NULL == nfields) || (NULL == tok))
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                 errmsg(" pointer is null in read_into_add_arrayelem function! ")));
        return false;
    }

    if (('[' == *tok) || ('(' == *tok))
    {
        if ('[' == *tok)
            toktmp = ']';
        else
            toktmp = ')';

        tokExpected[0] = (char)toktmp;
        tokExpected[1] = '\0';

        newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
        newp->arrayparentno = tmpdno;
        newp->dtype      = PLPGSQL_DTYPE_ARRAYELEM;
        /* get the array index expression */
        newp->subscript = read_sql_expression(toktmp, &tokExpected[0]);
        
        if(NULL == newp->subscript)
            ereport(ERROR,
                (errcode(ERRCODE_SYNTAX_ERROR),
                  errmsg(" error near arrary name! ")));

        plpgsql_adddatum((PLpgSQL_datum *)newp);

        fieldnames[*nfields]   = pstrdup("arrayelem");
        varnos[(*nfields)++]   = newp->dno;

        *tok = yylex();

        /* is an array element, return true */
        return true;
    }

    return false;
}

/*
 * Read the argument of an INTO clause.  On entry, we have just read the
 * INTO keyword.
 */
static void
read_into_target(PLpgSQL_rec **rec, PLpgSQL_row **row, bool *strict)
{
    int			tok;

    /* Set default results */
    *rec = NULL;
    *row = NULL;
    if (strict)
        *strict = true;

    tok = yylex();
    if (strict && tok == K_STRICT)
    {
        *strict = true;
        tok = yylex();
    }

    /*
     * Currently, a row or record variable can be the single INTO target,
     * but not a member of a multi-target list.  So we throw error if there
     * is a comma after it, because that probably means the user tried to
     * write a multi-target list.  If this ever gets generalized, we should
     * probably refactor read_into_scalar_list so it handles all cases.
     */
    switch (tok)
    {
        case T_DATUM:
        case T_VARRAY_VAR:
            if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW
            || yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_RECORD)
            {
                check_assignable(yylval.wdatum.datum, yylloc);
                *row = (PLpgSQL_row *) yylval.wdatum.datum;

                if ((tok = yylex()) == ',')
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("record or row variable cannot be part of multiple-item INTO list"),
                             parser_errposition(yylloc)));
                plbsql_push_back_token(tok);
            }
            else if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC)
            {
                check_assignable(yylval.wdatum.datum, yylloc);
                *rec = (PLpgSQL_rec *) yylval.wdatum.datum;

                if ((tok = yylex()) == ',')
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("record or row variable cannot be part of multiple-item INTO list"),
                             parser_errposition(yylloc)));
                plbsql_push_back_token(tok);
            }
            else
            {
                *row = read_into_array_scalar_list(NameOfDatum(&(yylval.wdatum)),
                                             yylval.wdatum.datum, yylloc);
            }
            break;

        case  T_PLACEHOLDER:
                *row = read_into_placeholder_scalar_list(yylval.word.ident, yylloc);
            break;

        default:
            /* just to give a better message than "syntax error" */
            current_token_is_not_variable(tok);
            break;
    }
}

/*
 *  Read the argument of an USING IN / OUT clause. 
 */
static void
read_using_target(List  **in_expr, PLpgSQL_row **out_row)
{
    int				tok		= 0;
    int				out_nfields=   0;
    int				out_varnos[1024] = {0};
    char			*out_fieldnames[1024] = {0};
    bool          	isin	= false;

    *in_expr = NULL;
    *out_row = NULL;

    do 
    {
        tok=yylex();
        if(K_IN == tok)
        {
            tok = yylex();
            isin = true; 
        }

        if(K_OUT == tok)
        {
            char * tempvar = NULL;
            int    tempdno = 0;
            PLpgSQL_expr * tempexpr = NULL;

            tok = yylex();

            switch(tok)
            {
                case T_DATUM:
                    tempvar = pstrdup(NameOfDatum(&(yylval.wdatum)));
                    tempdno = yylval.wdatum.datum->dno;
                    plbsql_push_back_token(tok);
                    tempexpr  =read_sql_construct(',',';',',',", or ;","SELECT ",true,true,true,NULL,&tok);
                    tempexpr->isouttype = true;
                    *in_expr=lappend((*in_expr), tempexpr);

                    if (!read_into_using_add_arrayelem(out_fieldnames, out_varnos, &out_nfields,tempdno,&tok)) 
                    {
                        out_fieldnames[out_nfields] = tempvar;
                        out_varnos[out_nfields++]	= tempdno;
                    }
                    else
                    {
                        if (isin)
                            ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg(" using can't support array parameter with in out !")));

                    }

                    break;

                default:
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("not all the parameters are scalar variables.")));
            }
        }
        else
        {
            PLpgSQL_expr * expr = NULL;
            plbsql_push_back_token(tok);
            expr  = read_sql_construct(',',';',',',", or ;","SELECT ",true,true,true,NULL,&tok);
            *in_expr=lappend((*in_expr), expr);

            isin = false;

        }

    }while(tok== ',');

    /*
    * We read an extra, non-comma token from yylex(), so push it
    * back onto the input stream
    */
    plbsql_push_back_token(tok);

    if(out_nfields)
    {
        (*out_row) = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
        (*out_row)->dtype = PLPGSQL_DTYPE_ROW;
        (*out_row)->refname = pstrdup("*internal*");
        (*out_row)->lineno = plpgsql_location_to_lineno(yylloc);
        (*out_row)->rowtupdesc = NULL;
        (*out_row)->nfields = out_nfields;
        (*out_row)->fieldnames = (char **)palloc(sizeof(char *) * out_nfields);
        (*out_row)->varnos = (int *)palloc(sizeof(int) * out_nfields);
        while (--out_nfields >= 0)
        {
            (*out_row)->fieldnames[out_nfields] = out_fieldnames[out_nfields];
            (*out_row)->varnos[out_nfields] = out_varnos[out_nfields];
        }
        plpgsql_adddatum((PLpgSQL_datum *)(*out_row));
    }
}

/*
 * Given the first datum and name in the INTO list, continue to read
 * comma-separated scalar variables until we run out. Then construct
 * and return a fake "row" variable that represents the list of
 * scalars.
 */
static PLpgSQL_row *
read_into_scalar_list(char *initial_name,
                      PLpgSQL_datum *initial_datum,
                      int initial_location)
{
    int				 nfields;
    char			*fieldnames[1024] = {NULL};
    int				 varnos[1024] = {0};
    PLpgSQL_row		*row;
    int				 tok;

    check_assignable(initial_datum, initial_location);
    fieldnames[0] = initial_name;
    varnos[0]	  = initial_datum->dno;
    nfields		  = 1;

    while ((tok = yylex()) == ',')
    {
        /* Check for array overflow */
        if (nfields >= 1024)
            ereport(ERROR,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                     errmsg("too many INTO variables specified"),
                     parser_errposition(yylloc)));

        tok = yylex();
        switch (tok)
        {
            case T_DATUM:
                check_assignable(yylval.wdatum.datum, yylloc);
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC)
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("\"%s\" is not a scalar variable",
                                    NameOfDatum(&(yylval.wdatum))),
                             parser_errposition(yylloc)));
                fieldnames[nfields] = NameOfDatum(&(yylval.wdatum));
                varnos[nfields++]	= yylval.wdatum.datum->dno;
                break;

            default:
                /* just to give a better message than "syntax error" */
                current_token_is_not_variable(tok);
                break;
        }
    }

    /*
     * We read an extra, non-comma token from yylex(), so push it
     * back onto the input stream
     */
    plbsql_push_back_token(tok);

    row = (PLpgSQL_row *)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = plpgsql_location_to_lineno(initial_location);
    row->rowtupdesc = NULL;
    row->nfields = nfields;
    row->fieldnames = (char **)palloc(sizeof(char *) * nfields);
    row->varnos = (int *)palloc(sizeof(int) * nfields);
    while (--nfields >= 0)
    {
        row->fieldnames[nfields] = fieldnames[nfields];
        row->varnos[nfields] = varnos[nfields];
    }

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}

/*
 * Given the first datum and name in the INTO list, continue to read
 * comma-separated scalar variables until we run out. Then construct
 * and return a fake "row" variable that represents the list of
 * scalars.
 */
static PLpgSQL_row *
read_into_array_scalar_list(char *initial_name,
                      PLpgSQL_datum *initial_datum,
                      int initial_location)
{
    int				 nfields = 0;
    char			*fieldnames[1024] = {NULL};
    int				 varnos[1024] = {0};
    PLpgSQL_row		*row;
    int				 tok;
    int				 toktmp = 0;
    int 			 tmpdno = 0;
    char* 			 nextname = NULL;

    check_assignable(initial_datum, initial_location);
    tmpdno = yylval.wdatum.datum->dno;
    tok = yylex();
    if (!read_into_using_add_arrayelem(fieldnames, varnos, &nfields, tmpdno, &tok))
    {
        fieldnames[0] = initial_name;
        varnos[0]	  = initial_datum->dno;
        nfields		  = 1;
    }
    while (',' == tok)
    {
        /* Check for array overflow */
        if (nfields >= 1024)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                     errmsg("too many INTO variables specified"),
                     parser_errposition(yylloc)));
            return NULL;
        }

        toktmp = yylex();

        switch (toktmp)
        {
            case T_DATUM:
                check_assignable(yylval.wdatum.datum, yylloc);
                
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC)
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("\"%s\" is not a scalar variable",
                                    NameOfDatum(&(yylval.wdatum))),
                            parser_errposition(yylloc)));

                tmpdno = yylval.wdatum.datum->dno;
                nextname = NameOfDatum(&(yylval.wdatum));
                fieldnames[nfields] = nextname;
                varnos[nfields++] = tmpdno;
                tok = yylex();
                break;
            case T_VARRAY_VAR:
                check_assignable(yylval.wdatum.datum, yylloc);
                tmpdno = yylval.wdatum.datum->dno;
                tok = yylex();
                if (tok < -1)
                    return NULL;
                if (!read_into_using_add_arrayelem(fieldnames, varnos, &nfields, tmpdno, &tok))
                {
                    ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg(" error near arrary name! ")));
                    return NULL;
                }		
                break;
            default:
                tok = yylex();
                if (tok < -1)
                    return NULL;

                /* just to give a better message than "syntax error" */
                current_token_is_not_variable(tok);
                break;
        }
    }
    
    /*
     * We read an extra, non-comma token from yylex(), so push it
     * back onto the input stream
     */
    plbsql_push_back_token(tok);

    row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = plpgsql_location_to_lineno(initial_location);
    row->rowtupdesc = NULL;
    row->nfields = nfields;
    row->fieldnames = (char**)palloc(sizeof(char *) * nfields);
    row->varnos = (int*)palloc(sizeof(int) * nfields);
    while (--nfields >= 0)
    {
        row->fieldnames[nfields] = fieldnames[nfields];
        row->varnos[nfields] = varnos[nfields];
    }

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}



/*
 * Given the first datum and name in the INTO list, continue to read
 * comma-separated scalar variables until we run out. Then construct
 * and return a fake "row" variable that represents the list of
 * scalars. The function is for placeholders.
 */
static PLpgSQL_row *
read_into_placeholder_scalar_list(char *initial_name,
                      int initial_location)
{
    int				 intoplaceholders;
    PLpgSQL_row		*row;
    int				 tok;

    intoplaceholders		  = 1;

    while ((tok = yylex()) == ',')
    {
        /* Check for array overflow */
        if ( intoplaceholders >= 1024 )
            ereport(ERROR,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                     errmsg("too many INTO placeholders specified"),
                     parser_errposition(yylloc)));

        tok = yylex();
        switch (tok)
        {
            case T_PLACEHOLDER:
                intoplaceholders++;
                break;

            default:
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("invalid placeholder specified"),
                         parser_errposition(yylloc)));
                break;
        }
    }

    /*
     * We read an extra, non-comma token from yylex(), so push it
     * back onto the input stream
     */
    plbsql_push_back_token(tok);

    row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = plpgsql_location_to_lineno(initial_location);
    row->rowtupdesc = NULL;
    row->nfields = 0;
    row->fieldnames = NULL;
    row->varnos = NULL;
    row->intoplaceholders = intoplaceholders;
    row->intodatums = NULL;

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}



/*
 * Convert a single scalar into a "row" list.  This is exactly
 * like read_into_scalar_list except we never consume any input.
 *
 * Note: lineno could be computed from location, but since callers
 * have it at hand already, we may as well pass it in.
 */
static PLpgSQL_row *
make_scalar_list1(char *initial_name,
                  PLpgSQL_datum *initial_datum,
                  int lineno, int location)
{
    PLpgSQL_row		*row;

    check_assignable(initial_datum, location);

    row = (PLpgSQL_row *)palloc(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = lineno;
    row->rowtupdesc = NULL;
    row->nfields = 1;
    row->fieldnames = (char **)palloc(sizeof(char *));
    row->varnos = (int *)palloc(sizeof(int));
    row->fieldnames[0] = initial_name;
    row->varnos[0] = initial_datum->dno;

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}

/*
 * When the PL/pgSQL parser expects to see a SQL statement, it is very
 * liberal in what it accepts; for example, we often assume an
 * unrecognized keyword is the beginning of a SQL statement. This
 * avoids the need to duplicate parts of the SQL grammar in the
 * PL/pgSQL grammar, but it means we can accept wildly malformed
 * input. To try and catch some of the more obviously invalid input,
 * we run the strings we expect to be SQL statements through the main
 * SQL parser.
 *
 * We only invoke the raw parser (not the analyzer); this doesn't do
 * any database access and does not check any semantic rules, it just
 * checks for basic syntactic correctness. We do this here, rather
 * than after parsing has finished, because a malformed SQL statement
 * may cause the PL/pgSQL parser to become confused about statement
 * borders. So it is best to bail out as early as we can.
 *
 * It is assumed that "stmt" represents a copy of the function source text
 * beginning at offset "location", with leader text of length "leaderlen"
 * (typically "SELECT ") prefixed to the source text.  We use this assumption
 * to transpose any error cursor position back to the function source text.
 * If no error cursor is provided, we'll just point at "location".
 */
static void
check_sql_expr(const char *stmt, int location, int leaderlen)
{
    sql_error_callback_arg cbarg;
    ErrorContextCallback  syntax_errcontext;
    MemoryContext oldCxt;

    if (!u_sess->plsql_cxt.plpgsql_check_syntax)
        return;

    cbarg.location = location;
    cbarg.leaderlen = leaderlen;

    syntax_errcontext.callback = plpgsql_sql_error_callback;
    syntax_errcontext.arg = &cbarg;
    syntax_errcontext.previous = t_thrd.log_cxt.error_context_stack;
    t_thrd.log_cxt.error_context_stack = &syntax_errcontext;

    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);
    (void) db_b_raw_parser(stmt);
    MemoryContextSwitchTo(oldCxt);

    /* Restore former ereport callback */
    t_thrd.log_cxt.error_context_stack = syntax_errcontext.previous;
}

static void
plpgsql_sql_error_callback(void *arg)
{
    sql_error_callback_arg *cbarg = (sql_error_callback_arg *) arg;
    int			errpos;

    /*
     * First, set up internalerrposition to point to the start of the
     * statement text within the function text.  Note this converts
     * location (a byte offset) to a character number.
     */
    parser_errposition(cbarg->location);

    /*
     * If the core parser provided an error position, transpose it.
     * Note we are dealing with 1-based character numbers at this point.
     */
    errpos = geterrposition();
    if (errpos > cbarg->leaderlen)
    {
        int		myerrpos = getinternalerrposition();

        if (myerrpos > 0)		/* safety check */
            internalerrposition(myerrpos + errpos - cbarg->leaderlen - 1);
    }

    /* In any case, flush errposition --- we want internalerrpos only */
    errposition(0);
}

/*
 * Parse a SQL datatype name and produce a PLpgSQL_type structure.
 *
 * The heavy lifting is done elsewhere.  Here we are only concerned
 * with setting up an errcontext link that will let us give an error
 * cursor pointing into the plpgsql function source, if necessary.
 * This is handled the same as in check_sql_expr(), and we likewise
 * expect that the given string is a copy from the source text.
 */
static PLpgSQL_type *
parse_datatype(const char *string, int location)
{
    Oid			type_id;
    int32		typmod;
    sql_error_callback_arg cbarg;
    ErrorContextCallback  syntax_errcontext;
    MemoryContext oldCxt = NULL;

    cbarg.location = location;
    cbarg.leaderlen = 0;

    syntax_errcontext.callback = plpgsql_sql_error_callback;
    syntax_errcontext.arg = &cbarg;
    syntax_errcontext.previous = t_thrd.log_cxt.error_context_stack;
    t_thrd.log_cxt.error_context_stack = &syntax_errcontext;

    /*
     * parseTypeString is only for getting type_id and typemod, who are both scalars, 
     * so memory used in it is all temp.
     */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);

    /* Let the main parser try to parse it under standard SQL rules */
    parseTypeString(string, &type_id, &typmod);

    (void)MemoryContextSwitchTo(oldCxt);

    /* Restore former ereport callback */
    t_thrd.log_cxt.error_context_stack = syntax_errcontext.previous;

    /* Okay, build a PLpgSQL_type data structure for it */
    return plpgsql_build_datatype(type_id, typmod,
                                  u_sess->plsql_cxt.plpgsql_curr_compile->fn_input_collation);
}

/*
 * Check block starting and ending labels match.
 */
static void
check_labels(const char *start_label, const char *end_label, int end_location)
{
    if (end_label)
    {
        if (!start_label)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("end label \"%s\" specified for unlabelled block",
                            end_label),
                     parser_errposition(end_location)));

        if (strcmp(start_label, end_label) != 0)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("end label \"%s\" differs from block's label \"%s\"",
                            end_label, start_label),
                     parser_errposition(end_location)));
    }
}

/*
 * Read the arguments (if any) for a cursor, followed by the until token
 *
 * If cursor has no args, just swallow the until token and return NULL.
 * If it does have args, we expect to see "( arg [, arg ...] )" followed
 * by the until token, where arg may be a plain expression, or a named
 * parameter assignment of the form argname := expr. Consume all that and
 * return a SELECT query that evaluates the expression(s) (without the outer
 * parens).
 */
static PLpgSQL_expr *
read_cursor_args(PLpgSQL_var *cursor, int until, const char *expected)
{
    PLpgSQL_expr *expr;
    PLpgSQL_row *row;
    int			tok;
    int			argc;
    char	  **argv;
    StringInfoData ds;
    char	   *sqlstart = "SELECT ";
    bool		any_named = false;

    tok = yylex();
    if (cursor->cursor_explicit_argrow < 0)
    {
        /* No arguments expected */
        if (tok == '(')
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("cursor \"%s\" has no arguments",
                            cursor->refname),
                     parser_errposition(yylloc)));

        if (tok != until)
            yyerror("syntax error");

        return NULL;
    }

    /* Else better provide arguments */
    if (tok != '(')
        ereport(ERROR,
                (errcode(ERRCODE_SYNTAX_ERROR),
                 errmsg("cursor \"%s\" has arguments",
                        cursor->refname),
                 parser_errposition(yylloc)));

    /*
     * Read the arguments, one by one.
     */
    row = (PLpgSQL_row *) u_sess->plsql_cxt.plpgsql_Datums[cursor->cursor_explicit_argrow];
    argv = (char **) palloc0(row->nfields * sizeof(char *));

    for (argc = 0; argc < row->nfields; argc++)
    {
        PLpgSQL_expr *item;
        int		endtoken;
        int		argpos;
        int		tok1,
                tok2;
        int		arglocation;

        /* Check if it's a named parameter: "param := value" */
        plpgsql_peek2(&tok1, &tok2, &arglocation, NULL);
        if (tok1 == IDENT && tok2 == COLON_EQUALS)
        {
            char   *argname;
            IdentifierLookup save_IdentifierLookup;

            /* Read the argument name, ignoring any matching variable */
            save_IdentifierLookup = u_sess->plsql_cxt.plpgsql_IdentifierLookup;
            u_sess->plsql_cxt.plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_DECLARE;
            yylex();
            argname = yylval.str;
            u_sess->plsql_cxt.plpgsql_IdentifierLookup = save_IdentifierLookup;

            /* Match argument name to cursor arguments */
            for (argpos = 0; argpos < row->nfields; argpos++)
            {
                if (strcmp(row->fieldnames[argpos], argname) == 0)
                    break;
            }
            if (argpos == row->nfields)
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("cursor \"%s\" has no argument named \"%s\"",
                                cursor->refname, argname),
                         parser_errposition(yylloc)));

            /*
             * Eat the ":=". We already peeked, so the error should never
             * happen.
             */
            tok2 = yylex();
            if (tok2 != COLON_EQUALS)
                yyerror("syntax error");

            any_named = true;
        }
        else
            argpos = argc;

        if (argv[argpos] != NULL)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("value for parameter \"%s\" of cursor \"%s\" specified more than once",
                            row->fieldnames[argpos], cursor->refname),
                     parser_errposition(arglocation)));

        /*
         * Read the value expression. To provide the user with meaningful
         * parse error positions, we check the syntax immediately, instead of
         * checking the final expression that may have the arguments
         * reordered. Trailing whitespace must not be trimmed, because
         * otherwise input of the form (param -- comment\n, param) would be
         * translated into a form where the second parameter is commented
         * out.
         */
        item = read_sql_construct(',', ')', 0,
                                  ",\" or \")",
                                  sqlstart,
                                  true, true,
                                  false, /* do not trim */
                                  NULL, &endtoken);

        argv[argpos] = item->query + strlen(sqlstart);

        if (endtoken == ')' && !(argc == row->nfields - 1))
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("not enough arguments for cursor \"%s\"",
                            cursor->refname),
                     parser_errposition(yylloc)));

        if (endtoken == ',' && (argc == row->nfields - 1))
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("too many arguments for cursor \"%s\"",
                            cursor->refname),
                     parser_errposition(yylloc)));
    }

    /* Make positional argument list */
    initStringInfo(&ds);
    appendStringInfoString(&ds, sqlstart);
    for (argc = 0; argc < row->nfields; argc++)
    {
        AssertEreport(argv[argc] != NULL,
                            MOD_PLSQL,
                            "It should not be null");

        /*
         * Because named notation allows permutated argument lists, include
         * the parameter name for meaningful runtime errors.
         */
        appendStringInfoString(&ds, argv[argc]);
        if (any_named)
            appendStringInfo(&ds, " AS %s",
                             quote_identifier(row->fieldnames[argc]));
        if (argc < row->nfields - 1)
            appendStringInfoString(&ds, ", ");
    }
    appendStringInfoChar(&ds, ';');

    expr = (PLpgSQL_expr*)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(ds.data);
    expr->plan			= NULL;
    expr->paramnos		= NULL;
    expr->ns            = plpgsql_ns_top();
    expr->idx			= (uint32)-1;
    pfree_ext(ds.data);

    /* Next we'd better find the until token */
    tok = yylex();
    if (tok != until)
        yyerror("syntax error");

    return expr;
}

/*
 * Parse RAISE ... USING options
 */
static List *
read_raise_options(void)
{
    List	   *result = NIL;

    for (;;)
    {
        PLpgSQL_raise_option *opt;
        int		tok;

        if ((tok = yylex()) == 0)
            yyerror("unexpected end of function definition");

        opt = (PLpgSQL_raise_option *) palloc(sizeof(PLpgSQL_raise_option));

        if (tok_is_keyword(tok, &yylval,
                           K_ERRCODE, "errcode"))
            opt->opt_type = PLPGSQL_RAISEOPTION_ERRCODE;
        else if (tok_is_keyword(tok, &yylval,
                                K_MESSAGE, "message"))
            opt->opt_type = PLPGSQL_RAISEOPTION_MESSAGE;
        else if (tok_is_keyword(tok, &yylval,
                                K_DETAIL, "detail"))
            opt->opt_type = PLPGSQL_RAISEOPTION_DETAIL;
        else if (tok_is_keyword(tok, &yylval,
                                K_HINT, "hint"))
            opt->opt_type = PLPGSQL_RAISEOPTION_HINT;
        else
            yyerror("unrecognized RAISE statement option");

        tok = yylex();
        if (tok != '=' && tok != COLON_EQUALS)
            yyerror("syntax error, expected \"=\"");

        opt->expr = read_sql_expression2(',', ';', ", or ;", &tok);

        result = lappend(result, opt);

        if (tok == ';')
            break;
    }

    return result;
}

/*
 * Fix up CASE statement
 */
static PLpgSQL_stmt *
make_case(int location, PLpgSQL_expr *t_expr,
          List *case_when_list, List *else_stmts)
{
    PLpgSQL_stmt_case 	*newp;

    newp = (PLpgSQL_stmt_case 	*)palloc(sizeof(PLpgSQL_stmt_case));
    newp->cmd_type = PLPGSQL_STMT_CASE;
    newp->lineno = plpgsql_location_to_lineno(location);
    newp->t_expr = t_expr;
    newp->t_varno = 0;
    newp->case_when_list = case_when_list;
    newp->have_else = (else_stmts != NIL);
    /* Get rid of list-with-NULL hack */
    if (list_length(else_stmts) == 1 && linitial(else_stmts) == NULL)
        newp->else_stmts = NIL;
    else
        newp->else_stmts = else_stmts;

    /*
     * When test expression is present, we create a var for it and then
     * convert all the WHEN expressions to "VAR IN (original_expression)".
     * This is a bit klugy, but okay since we haven't yet done more than
     * read the expressions as text.  (Note that previous parsing won't
     * have complained if the WHEN ... THEN expression contained multiple
     * comma-separated values.)
     */
    if (t_expr)
    {
        char	varname[32];
        PLpgSQL_var *t_var;
        ListCell *l;

        /* use a name unlikely to collide with any user names */
        snprintf(varname, sizeof(varname), "__Case__Variable_%d__",
                 u_sess->plsql_cxt.plpgsql_nDatums);

        /*
         * We don't yet know the result datatype of t_expr.  Build the
         * variable as if it were INT4; we'll fix this at runtime if needed.
         */
        t_var = (PLpgSQL_var *)
            plpgsql_build_variable(varname, newp->lineno,
                                   plpgsql_build_datatype(INT4OID,
                                                          -1,
                                                          InvalidOid),
                                   true);
        newp->t_varno = t_var->dno;

        foreach(l, case_when_list)
        {
            PLpgSQL_case_when *cwt = (PLpgSQL_case_when *) lfirst(l);
            PLpgSQL_expr *expr = cwt->expr;
            StringInfoData	ds;

            /* copy expression query without SELECT keyword (expr->query + 7) */
            AssertEreport(strncmp(expr->query, "SELECT ", 7) == 0,
                                MOD_PLSQL,
                                "copy expression query without SELECT keyword");

            /* And do the string hacking */
            initStringInfo(&ds);

            appendStringInfo(&ds, "SELECT \"%s\" IN (%s)",
                             varname, expr->query + 7);

            pfree_ext(expr->query);
            expr->query = pstrdup(ds.data);
            /* Adjust expr's namespace to include the case variable */
            expr->ns = plpgsql_ns_top();

            pfree_ext(ds.data);
        }
    }

    return (PLpgSQL_stmt *) newp;
}

static PLpgSQL_stmt *
make_callfunc_stmt_no_arg(const char *sqlstart, int location)
{
    char *cp[3] = {0};
    HeapTuple proctup = NULL;
    Oid *p_argtypes = NULL;
    char **p_argnames = NULL;
    char *p_argmodes = NULL;
    int narg = 0;
    List *funcname = NIL;
    PLpgSQL_expr* expr = NULL;
    FuncCandidateList clist = NULL;
    StringInfoData func_inparas;
    char *quoted_sqlstart = NULL;
    MemoryContext oldCxt = NULL;

    /*
     * the function make_callfunc_stmt_no_arg is only to assemble a sql statement, 
     * so the context is set to tmp context. 
     */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.compile_tmp_cxt);
    /* get the function's name. */
    plpgsql_parser_funcname(sqlstart, cp, 3);

    if (cp[2] != NULL && cp[2][0] != '\0')
        funcname = list_make3(makeString(cp[0]), makeString(cp[1]), makeString(cp[2]));
    else if (cp[1] && cp[1][0] != '\0')
        funcname = list_make2(makeString(cp[0]), makeString(cp[1]));
    else
        funcname = list_make1(makeString(cp[0]));

    /* search the function */
    clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
    if (clist == NULL)
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function \"%s\" doesn't exist ", sqlstart)));
    }

    proctup = SearchSysCache(PROCOID,
                            ObjectIdGetDatum(clist->oid),
                            0, 0, 0);
    /* function may be deleted after clist be searched. */
    if (!HeapTupleIsValid(proctup))
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                errmsg("function \"%s\" doesn't exist ", sqlstart)));
    }
    /* get the all args informations, only "in" parameters if p_argmodes is null */
    narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
    ReleaseSysCache(proctup);
    if (narg != 0)
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function %s has no enough parameters", sqlstart)));

    initStringInfo(&func_inparas);

    appendStringInfoString(&func_inparas, "CALL ");

    quoted_sqlstart = NameListToQuotedString(funcname);
    appendStringInfoString(&func_inparas, quoted_sqlstart);
    pfree_ext(quoted_sqlstart);

    appendStringInfoString(&func_inparas, "(");

    appendStringInfoString(&func_inparas, ")");

    /* read the end token */
    yylex();

    (void)MemoryContextSwitchTo(oldCxt);

    /* generate the expression */
    expr = (PLpgSQL_expr*)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype = PLPGSQL_DTYPE_EXPR;
    expr->query = pstrdup(func_inparas.data);
    expr->plan = NULL;
    expr->paramnos = NULL;
    expr->ns = plpgsql_ns_top();
    expr->idx = (uint32)-1;

    PLpgSQL_stmt_perform *perform = NULL;
    perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
    perform->cmd_type = PLPGSQL_STMT_PERFORM;
    perform->lineno = plpgsql_location_to_lineno(location);
    perform->expr = expr;

    return (PLpgSQL_stmt *)perform;
}

/*
 * Brief		: special handling of dbms_lob.open and dbms_lob.close 
 * Description	: open and close are keyword, which need to be handled separately
 * Notes		: 
 */ 
static PLpgSQL_stmt *
parse_lob_open_close(int location)
{
	StringInfoData func;
	int tok = yylex();
	char *mode = NULL;
	bool is_open = false;
	PLpgSQL_expr *expr = NULL;
	PLpgSQL_stmt_perform *perform = NULL;

	initStringInfo(&func);
	appendStringInfoString(&func, "CALL DBMS_LOB.");
	tok = yylex();
	if (K_OPEN == tok)
	{
		is_open = true;
		appendStringInfoString(&func,"OPEN(");
	}
	else
		appendStringInfoString(&func,"CLOSE(");

	if ('(' == (tok = yylex()))
	{
		tok = yylex();
		if (T_DATUM == tok)
			appendStringInfoString(&func, NameOfDatum(&yylval.wdatum));
		else if (T_PLACEHOLDER == tok)
			appendStringInfoString(&func, yylval.word.ident);
		else
			yyerror("syntax error");

		if (is_open)
		{
			if (',' == (tok = yylex()) && T_CWORD == (tok = yylex()))
			{
				mode = NameListToString(yylval.cword.idents);
				if (strcasecmp(mode, "DBMS_LOB.LOB_READWRITE") != 0 
					&& strcasecmp(mode, "DBMS_LOB.LOB_READWRITE") != 0)
					yyerror("syntax error");
				else if (!(')' == yylex() && ';' == yylex()))
					yyerror("syntax error");

				appendStringInfoChar(&func, ')');
			}
			else
				yyerror("syntax error");
		}
		else if(')' == yylex() && ';' == yylex())
			appendStringInfoChar(&func, ')');
		else
			yyerror("syntax error");

		expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
		expr->dtype = PLPGSQL_DTYPE_EXPR;
		expr->query = pstrdup(func.data);
		expr->plan = NULL;
		expr->paramnos = NULL;
		expr->ns = plpgsql_ns_top();
        expr->idx = (uint32)-1;

		perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
		perform->cmd_type = PLPGSQL_STMT_PERFORM;
		perform->lineno = plpgsql_location_to_lineno(location);
		perform->expr = expr;
		return (PLpgSQL_stmt *)perform;
	}
	yyerror("syntax error");
	return NULL;
}
