/* A Bison parser, made by GNU Bison 3.5.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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

#line 256 "pl/pl_b_gram.hpp"

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
