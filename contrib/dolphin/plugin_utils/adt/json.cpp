/* -------------------------------------------------------------------------
 *
 * json.c
 *        JSON data type support.
 *
 * Portions Copyright (c) 2021 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 1996-2013, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *      src/backend/utils/adt/json.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"
#include "gaussdb_version.h"
#include "access/htup.h"
#include "access/transam.h"
#include "catalog/pg_cast.h"
#include "catalog/pg_type.h"
#include "executor/spi.h"
#include "lib/stringinfo.h"
#include "libpq/pqformat.h"
#include "plugin_mb/pg_wchar.h"
#include "miscadmin.h"
#include "plugin_parser/parse_coerce.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/json.h"
#include "utils/typcache.h"
#include "utils/syscache.h"

#ifdef DOLPHIN
#include "plugin_postgres.h"
#include "plugin_parser/scansup.h"
#include "plugin_utils/json.h"
#include "plugin_utils/jsonapi.h"
#include <wchar.h>
#define UNICODE_LEN 5        /* u0123 ,without \ */
#define UNICODE_NUM_LEN 4    /* 0123, without \u */
#define UNICODE_STR_LEN 6    /* \u0123 */
#define UNICODE_ESCAPE_LEN 2 /*just \u without 0123*/
#define HEXADECIMAL 16       /*represents hexadecimal*/

static bool is_valid_unicode_escape_sequence(char *str);
static bool is_valid_unicode_escape(char *str);
static char *unescape_unicode(const char *str);
static void DelChar(char *inStr, char *outStr, int &a, int &b);
static void CheckSign(char *inStr, int &x);
static void CheckSingleSign(char *inStr, int &x);
static void depth_object_field(JsonLexContext *lex, JsonSemAction *sem, int &depth);
static void depth_array_element(JsonLexContext *lex, JsonSemAction *sem, int &depth);
static void depth_object(JsonLexContext *lex, JsonSemAction *sem, int &depth);
static void depth_array(JsonLexContext *lex, JsonSemAction *sem, int &depth);
static void sort_json(JsonLexContext *lex, JsonSemAction *sem, int &depth);
static const char *type_case(JsonTokenType tok, bool flag = false);

PG_FUNCTION_INFO_V1_PUBLIC(json_array);
extern "C" DLL_PUBLIC Datum json_array(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_object_mysql);
extern "C" DLL_PUBLIC Datum json_object_mysql(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_object_noarg);
extern "C" DLL_PUBLIC Datum json_object_noarg(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_unquote);
extern "C" DLL_PUBLIC Datum json_unquote(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_quote);
extern "C" DLL_PUBLIC Datum json_quote(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_depth);
extern "C" DLL_PUBLIC Datum json_depth(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_valid);
extern "C" DLL_PUBLIC Datum json_valid(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_type);
extern "C" DLL_PUBLIC Datum json_type(PG_FUNCTION_ARGS);
#endif

/*
 * The context of the parser is maintained by the recursive descent
 * mechanism, but is passed explicitly to the error reporting routine
 * for better diagnostics.
 */
typedef enum               /* contexts of JSON parser */
{ JSON_PARSE_VALUE,        /* expecting a value */
  JSON_PARSE_STRING,       /* expecting a string (for a field name) */
  JSON_PARSE_ARRAY_START,  /* saw '[', expecting value or ']' */
  JSON_PARSE_ARRAY_NEXT,   /* saw array element, expecting ',' or ']' */
  JSON_PARSE_OBJECT_START, /* saw '{', expecting label or '}' */
  JSON_PARSE_OBJECT_LABEL, /* saw object label, expecting ':' */
  JSON_PARSE_OBJECT_NEXT,  /* saw object value, expecting ',' or '}' */
  JSON_PARSE_OBJECT_COMMA, /* saw object ',', expecting next label */
  JSON_PARSE_END           /* saw the end of a document, expect nothing */
} JsonParseContext;
static inline void json_lex(JsonLexContext *lex, bool flag = false);
static inline void json_lex_string(JsonLexContext *lex);
static inline void json_lex_number(JsonLexContext *lex, char *s, bool *num_err);
static inline void parse_scalar(JsonLexContext *lex, JsonSemAction *sem);
static void parse_object_field(JsonLexContext *lex, JsonSemAction *sem);
static void parse_object(JsonLexContext *lex, JsonSemAction *sem);
static void parse_array_element(JsonLexContext *lex, JsonSemAction *sem);
static void parse_array(JsonLexContext *lex, JsonSemAction *sem);
static void report_parse_error(JsonParseContext ctx, JsonLexContext *lex);
static void report_invalid_token(JsonLexContext *lex);
static int report_json_context(JsonLexContext *lex);
static char *extract_mb_char(const char *s);
static void composite_to_json(Datum composite, StringInfo result, bool use_line_feeds);
static void array_dim_to_json(StringInfo result, int dim, int ndims, int *dims, Datum *vals, bool *nulls, int *valcount,
                              TYPCATEGORY tcategory, Oid typoutputfunc, bool use_line_feeds);
static void array_to_json_internal(Datum array, StringInfo result, bool use_line_feeds);
static void datum_to_json(Datum val, bool is_null, StringInfo result, TYPCATEGORY tcategory, Oid typoutputfunc,
                          bool key_scalar);
static void add_json(Datum val, bool is_null, StringInfo result, Oid val_type, bool key_scalar);

/* the null action object used for pure validation */
static JsonSemAction nullSemAction = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static inline bool lex_accept(JsonLexContext *lex, JsonTokenType token, char **lexeme);
static inline void lex_expect(JsonParseContext ctx, JsonLexContext *lex, JsonTokenType token);

/* Recursive Descent parser support routines */
/*
 * lex_peek
 *
 * what is the current look_ahead token?
 */
static inline JsonTokenType lex_peek(JsonLexContext *lex)
{
    return lex->token_type;
}

/*
 * lex_accept
 *
 * accept the look_ahead token and move the lexer to the next token if the
 * look_ahead token matches the token parameter. In that case, and if required,
 * also hand back the de-escaped lexeme.
 *
 * returns true if the token matched, false otherwise.
 */
static inline bool lex_accept(JsonLexContext *lex, JsonTokenType token, char **lexeme)
{
    if (lex->token_type == token) {
        if (lexeme != NULL) {
            if (lex->token_type == JSON_TOKEN_STRING) {
                if (lex->strval != NULL)
                    *lexeme = pstrdup(lex->strval->data);
            } else {
                int len = (lex->token_terminator - lex->token_start);
                char *tokstr = (char *)palloc(len + 1);
                errno_t rc = 0;

                rc = memcpy_s(tokstr, len + 1, lex->token_start, len);
                securec_check(rc, "\0", "\0");
                tokstr[len] = '\0';
                *lexeme = tokstr;
            }
        }
        json_lex(lex);
        return true;
    }
    return false;
}

/*
 * lex_accept
 *
 * move the lexer to the next token if the current look_ahead token matches
 * the parameter token. Otherwise, report an error.
 */
static inline void lex_expect(JsonParseContext ctx, JsonLexContext *lex, JsonTokenType token)
{
    if (!lex_accept(lex, token, NULL))
        report_parse_error(ctx, lex);
}

/*
 * All the defined    type categories are upper case , so use lower case here
 * so we avoid any possible clash.
 */
/* fake type category for JSON so we can distinguish it in datum_to_json */
#define TYPCATEGORY_JSON 'j'
/* fake category for types that have a cast to json */
#define TYPCATEGORY_JSON_CAST 'c'
/* chars to consider as part of an alphanumeric token */
#define JSON_ALPHANUMERIC_CHAR(c)                                                                            \
    (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || ((c) >= '0' && (c) <= '9') || (c) == '_' || \
     IS_HIGHBIT_SET(c))

/*
 * Input.
 */
Datum json_in(PG_FUNCTION_ARGS)
{
    char *json = PG_GETARG_CSTRING(0);
    text *result = cstring_to_text(json);
    JsonLexContext *lex = NULL;

    /* validate it */
    lex = makeJsonLexContext(result, false);
    MemoryContext oldcxt = CurrentMemoryContext;
    PG_TRY();
    {
        pg_parse_json(lex, &nullSemAction);
    }
    PG_CATCH();
    {
        if (fcinfo->can_ignore) {
            (void)MemoryContextSwitchTo(oldcxt);
            ErrorData *edata = CopyErrorData();
            ereport(WARNING,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json")));
            FlushErrorState();
            FreeErrorData(edata);
            PG_RETURN_DATUM((Datum)DirectFunctionCall1(json_in, CStringGetDatum("null")));
        } else {
            PG_RE_THROW();
        }
    }
    PG_END_TRY();

    /* Internal representation is the same as text, for now */
    PG_RETURN_TEXT_P(result);
}

/*
 * Output.
 */
Datum json_out(PG_FUNCTION_ARGS)
{
    /* we needn't detoast because text_to_cstring will handle that */
    Datum txt = PG_GETARG_DATUM(0);

    PG_RETURN_CSTRING(TextDatumGetCString(txt));
}

/*
 * Binary send.
 */
Datum json_send(PG_FUNCTION_ARGS)
{
    text *t = PG_GETARG_TEXT_PP(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendtext(&buf, VARDATA_ANY(t), VARSIZE_ANY_EXHDR(t));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 * Binary receive.
 */
Datum json_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    char *str = NULL;
    int nbytes;
    JsonLexContext *lex = NULL;

    str = pq_getmsgtext(buf, buf->len - buf->cursor, &nbytes);

    /* Validate it. */
    lex = makeJsonLexContextCstringLen(str, nbytes, false);
    pg_parse_json(lex, &nullSemAction);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(str, nbytes));
}

/*
 * makeJsonLexContext
 *
 * lex constructor, with or without StringInfo object
 * for de-escaped lexemes.
 *
 * Without is better as it makes the processing faster, so only make one
 * if really required.
 *
 * If you already have the json as a text* value, use the first of these
 * functions, otherwise use  makeJsonLexContextCstringLen().
 */
JsonLexContext *makeJsonLexContext(text *json, bool need_escapes)
{
    return makeJsonLexContextCstringLen(VARDATA(json), VARSIZE(json) - VARHDRSZ, need_escapes);
}

JsonLexContext *makeJsonLexContextCstringLen(char *json, int len, bool need_escapes)
{
    JsonLexContext *lex = (JsonLexContext *)palloc0(sizeof(JsonLexContext));

    lex->input = lex->token_terminator = lex->line_start = json;
    lex->line_number = 1;
    lex->input_length = len;
    if (need_escapes) {
        lex->strval = makeStringInfo();
    }
    return lex;
}

/*
 * pg_parse_json
 *
 * Publicly visible entry point for the JSON parser.
 *
 * lex is a lexing context, set up for the json to be processed by calling
 * makeJsonLexContext(). sem is a strucure of function pointers to semantic
 * action routines to be called at appropriate spots during parsing, and a
 * pointer to a state object to be passed to those routines.
 */
void pg_parse_json(JsonLexContext *lex, JsonSemAction *sem)
{
    JsonTokenType tok;

    /* get the initial token */
    json_lex(lex);
    tok = lex_peek(lex);

    /* parse by recursive descent */
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            parse_object(lex, sem);
            break;
        case JSON_TOKEN_ARRAY_START:
            parse_array(lex, sem);
            break;
        default:
            parse_scalar(lex, sem); /* json can be a bare scalar */
    }

    lex_expect(JSON_PARSE_END, lex, JSON_TOKEN_END);
}

/*
 * Recursive Descent parse routines. There is one for each structural
 * element in a json document:
 *    - scalar (string, number, true, false, null)
 *    - array  ( [ ] )
 *    - array element
 *    - object ( { } )
 *    - object field
 */
static inline void parse_scalar(JsonLexContext *lex, JsonSemAction *sem)
{
    char *val = NULL;
    json_scalar_action sfunc = sem->scalar;
    char **valaddr = NULL;
    JsonTokenType tok = lex_peek(lex);

    valaddr = sfunc == NULL ? NULL : &val;
    /* a scalar must be a string, a number, true, false, or null */
    switch (tok) {
        case JSON_TOKEN_TRUE:
            lex_accept(lex, JSON_TOKEN_TRUE, valaddr);
            break;
        case JSON_TOKEN_FALSE:
            lex_accept(lex, JSON_TOKEN_FALSE, valaddr);
            break;
        case JSON_TOKEN_NULL:
            lex_accept(lex, JSON_TOKEN_NULL, valaddr);
            break;
        case JSON_TOKEN_NUMBER:
            lex_accept(lex, JSON_TOKEN_NUMBER, valaddr);
            break;
        case JSON_TOKEN_STRING:
            lex_accept(lex, JSON_TOKEN_STRING, valaddr);
            break;
        default:
            report_parse_error(JSON_PARSE_VALUE, lex);
    }

    if (sfunc != NULL) {
        (*sfunc)(sem->semstate, val, tok);
    }
}

static void parse_object_field(JsonLexContext *lex, JsonSemAction *sem)
{
    /*
     * an object field is "fieldname" : value where value can be a scalar,
     * object or array
     */
    char *fname = NULL; /* keep compiler quiet */
    json_ofield_action ostart = sem->object_field_start;
    json_ofield_action oend = sem->object_field_end;
    bool isnull = false;
    char **fnameaddr = NULL;
    JsonTokenType tok;

    if (ostart != NULL || oend != NULL) {
        fnameaddr = &fname;
    }
    if (!lex_accept(lex, JSON_TOKEN_STRING, fnameaddr)) {
        report_parse_error(JSON_PARSE_STRING, lex);
    }
    lex_expect(JSON_PARSE_OBJECT_LABEL, lex, JSON_TOKEN_COLON);
    tok = lex_peek(lex);
    isnull = tok == JSON_TOKEN_NULL;

    if (ostart != NULL) {
        (*ostart)(sem->semstate, fname, isnull);
    }
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            parse_object(lex, sem);
            break;
        case JSON_TOKEN_ARRAY_START:
            parse_array(lex, sem);
            break;
        default:
            parse_scalar(lex, sem);
    }

    if (oend != NULL) {
        (*oend)(sem->semstate, fname, isnull);
    }
    if (fname != NULL) {
        pfree(fname);
    }
}

static void parse_object(JsonLexContext *lex, JsonSemAction *sem)
{
    /*
     * an object is a possibly empty sequence of object fields, separated by
     * commas and surrounde by curly braces.
     */
    json_struct_action ostart = sem->object_start;
    json_struct_action oend = sem->object_end;
    JsonTokenType tok;

    if (ostart != NULL) {
        (*ostart)(sem->semstate);
    }

    /*
     * Data inside an object is at a higher nesting level than the object
     * itself. Note that we increment this after we call the semantic routine
     * for the object start and restore it before we call the routine for the
     * object end.
     */
    lex->lex_level++;

    /* we know this will succeeed, just clearing the token */
    lex_expect(JSON_PARSE_OBJECT_START, lex, JSON_TOKEN_OBJECT_START);
    tok = lex_peek(lex);
    switch (tok) {
        case JSON_TOKEN_STRING:
            parse_object_field(lex, sem);
            while (lex_accept(lex, JSON_TOKEN_COMMA, NULL))
                parse_object_field(lex, sem);
            break;
        case JSON_TOKEN_OBJECT_END:
            break;
        default:
            /* case of an invalid initial token inside the object */
            report_parse_error(JSON_PARSE_OBJECT_START, lex);
    }
    lex_expect(JSON_PARSE_OBJECT_NEXT, lex, JSON_TOKEN_OBJECT_END);
    lex->lex_level--;

    if (oend != NULL) {
        (*oend)(sem->semstate);
    }
}

static void parse_array_element(JsonLexContext *lex, JsonSemAction *sem)
{
    json_aelem_action astart = sem->array_element_start;
    json_aelem_action aend = sem->array_element_end;
    JsonTokenType tok = lex_peek(lex);
    bool isnull;
    isnull = tok == JSON_TOKEN_NULL;

    if (astart != NULL) {
        (*astart)(sem->semstate, isnull);
    }

    /* an array element is any object, array or scalar */
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            parse_object(lex, sem);
            break;
        case JSON_TOKEN_ARRAY_START:
            parse_array(lex, sem);
            break;
        default:
            parse_scalar(lex, sem);
    }

    if (aend != NULL) {
        (*aend)(sem->semstate, isnull);
    }
}

static void parse_array(JsonLexContext *lex, JsonSemAction *sem)
{
    /*
     * an array is a possibly empty sequence of array elements, separated by
     * commas and surrounded by square brackets.
     */
    json_struct_action astart = sem->array_start;
    json_struct_action aend = sem->array_end;

    if (astart != NULL) {
        (*astart)(sem->semstate);
    }

    /*
     * Data inside an array is at a higher nesting level than the array
     * itself. Note that we increment this after we call the semantic routine
     * for the array start and restore it before we call the routine for the
     * array end.
     */
    lex->lex_level++;
    lex_expect(JSON_PARSE_ARRAY_START, lex, JSON_TOKEN_ARRAY_START);
    if (lex_peek(lex) != JSON_TOKEN_ARRAY_END) {
        parse_array_element(lex, sem);
        while (lex_accept(lex, JSON_TOKEN_COMMA, NULL))
            parse_array_element(lex, sem);
    }
    lex_expect(JSON_PARSE_ARRAY_NEXT, lex, JSON_TOKEN_ARRAY_END);
    lex->lex_level--;

    if (aend != NULL) {
        (*aend)(sem->semstate);
    }
}

/*
 * Lex one token from the input stream.
 */
static inline void json_lex(JsonLexContext *lex, bool flag)
{
    char *s = NULL;
    int len;

    /* Skip leading whitespace. */
    s = lex->token_terminator;
    len = s - lex->input;
    while (len < lex->input_length && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')) {
        if (*s == '\n') {
            ++lex->line_number;
        }
        ++s;
        ++len;
    }
    lex->token_start = s;

    /* Determine token type. */
    if (len >= lex->input_length) {
        lex->token_start = NULL;
        lex->prev_token_terminator = lex->token_terminator;
        lex->token_terminator = s;
        lex->token_type = JSON_TOKEN_END;
    } else {
        switch (*s) {
                /* Single-character token, some kind of punctuation mark. */
            case '{':
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = s + 1;
                lex->token_type = JSON_TOKEN_OBJECT_START;
                break;
            case '}':
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = s + 1;
                lex->token_type = JSON_TOKEN_OBJECT_END;
                break;
            case '[':
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = s + 1;
                lex->token_type = JSON_TOKEN_ARRAY_START;
                break;
            case ']':
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = s + 1;
                lex->token_type = JSON_TOKEN_ARRAY_END;
                break;
            case ',':
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = s + 1;
                lex->token_type = JSON_TOKEN_COMMA;
                break;
            case ':':
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = s + 1;
                lex->token_type = JSON_TOKEN_COLON;
                break;
            case '"':
                /* string */
                json_lex_string(lex);
                lex->token_type = JSON_TOKEN_STRING;
                break;
            case '-':
                /* Negative number. */
                json_lex_number(lex, s + 1, NULL);
#ifdef DOLPHIN
                if (flag) {
                    if (lex->ifint) {
                        lex->token_type = JSON_TOKEN_DOUBLE;
                    } else {
                        lex->token_type = JSON_TOKEN_INTEGER;
                    }
                } else {
                    lex->token_type = JSON_TOKEN_NUMBER;
                }
#endif
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                /* Positive number. */
#ifdef DOLPHIN
                json_lex_number(lex, s, NULL);
                if (flag) {
                    if (lex->ifint) {
                        lex->token_type = JSON_TOKEN_DOUBLE;
                    } else {
                        lex->token_type = JSON_TOKEN_INTEGER;
                    }
                } else {
                    lex->token_type = JSON_TOKEN_NUMBER;
                }
#endif
                break;
            default: {
                char *p = NULL;

                /*
                 * We're not dealing with a string, number, legal
                 * punctuation mark, or end of string.    The only legal
                 * tokens we might find here are true, false, and null,
                 * but for error reporting purposes we scan until we see a
                 * non-alphanumeric character.    That way, we can report
                 * the whole word as an unexpected token, rather than just
                 * some unintuitive prefix thereof.
                 */
                for (p = s; p - s < lex->input_length - len && JSON_ALPHANUMERIC_CHAR(*p); p++)
                    ; /* skip */

                /*
                 * We got some sort of unexpected punctuation or an
                 * otherwise unexpected character, so just complain about
                 * that one character.
                 */
                if (p == s) {
                    lex->prev_token_terminator = lex->token_terminator;
                    lex->token_terminator = s + 1;
                    report_invalid_token(lex);
                }

                /*
                 * We've got a real alphanumeric token here.  If it
                 * happens to be true, false, or null, all is well.  If
                 * not, error out.
                 */
                lex->prev_token_terminator = lex->token_terminator;
                lex->token_terminator = p;
                if (p - s == 4) {
                    if (memcmp(s, "true", 4) == 0)
                        lex->token_type = JSON_TOKEN_TRUE;
                    else if (memcmp(s, "null", 4) == 0)
                        lex->token_type = JSON_TOKEN_NULL;
                    else
                        report_invalid_token(lex);
                } else if (p - s == 5 && memcmp(s, "false", 5) == 0) {
                    lex->token_type = JSON_TOKEN_FALSE;
                } else {
                    report_invalid_token(lex);
                }
            }
        } /* end of switch */
    }
}

/*
 * The next token in the input stream is known to be a string; lex it.
 */
static inline void json_lex_string(JsonLexContext *lex)
{
    char *s = NULL;
    int len;
    int hi_surrogate = -1;

    if (lex->strval != NULL) {
        resetStringInfo(lex->strval);
    }

    Assert(lex->input_length > 0);
    s = lex->token_start;
    len = lex->token_start - lex->input;
    for (;;) {
        s++;
        len++;
        /* Premature end of the string. */
        if (len >= lex->input_length) {
            lex->token_terminator = s;
            report_invalid_token(lex);
        } else if (*s == '"') {
            break;
        } else if ((unsigned char)*s < 32) {
            /* Per RFC4627, these characters MUST be escaped. */
            /* Since *s isn't printable, exclude it from the context string */
            lex->token_terminator = s;
            ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                            errdetail("Character with value 0x%02x must be escaped.", (unsigned char)*s),
                            report_json_context(lex)));
        } else if (*s == '\\') {
            /* OK, we have an escape character. */
            s++;
            len++;
            if (len >= lex->input_length) {
                lex->token_terminator = s;
                report_invalid_token(lex);
            } else if (*s == 'u') {
                int i;
                int ch = 0;

                for (i = 1; i <= 4; i++) {
                    s++;
                    len++;
                    if (len >= lex->input_length) {
                        lex->token_terminator = s;
                        report_invalid_token(lex);
                    } else if (*s >= '0' && *s <= '9') {
                        ch = (ch * 16) + (*s - '0');
                    } else if (*s >= 'a' && *s <= 'f') {
                        ch = (ch * 16) + (*s - 'a') + 10;
                    } else if (*s >= 'A' && *s <= 'F') {
                        ch = (ch * 16) + (*s - 'A') + 10;
                    } else {
                        lex->token_terminator = s + pg_mblen(s);
                        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                                        errmsg("invalid input syntax for type json"),
                                        errdetail("\"\\u\" must be followed by four hexadecimal digits."),
                                        report_json_context(lex)));
                    }
                }
                if (lex->strval != NULL) {
                    char utf8str[5];
                    int utf8len;

                    if (ch >= 0xd800 && ch <= 0xdbff) {
                        if (hi_surrogate != -1) {
                            ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                                            errmsg("invalid input syntax for type json"),
                                            errdetail("Unicode high surrogate must not follow a high surrogate."),
                                            report_json_context(lex)));
                        }
                        hi_surrogate = ((uint32)ch & 0x3ff) << 10;
                        continue;
                    } else if (ch >= 0xdc00 && ch <= 0xdfff) {
                        if (hi_surrogate == -1) {
                            ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                                            errmsg("invalid input syntax for type json"),
                                            errdetail("Unicode low surrogate must follow a high surrogate."),
                                            report_json_context(lex)));
                        }
                        ch = 0x10000 + hi_surrogate + ((uint32)ch & 0x3ff);
                        hi_surrogate = -1;
                    }
                    if (hi_surrogate != -1) {
                        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                                        errmsg("invalid input syntax for type json"),
                                        errdetail("Unicode low surrogate must follow a high surrogate."),
                                        report_json_context(lex)));
                    }
                    /*
                     * For UTF8, replace the escape sequence by the actual
                     * utf8 character in lex->strval. Do this also for other
                     * encodings if the escape designates an ASCII character,
                     * otherwise raise an error. We don't ever unescape a
                     * \u0000, since that would result in an impermissible nul
                     * byte.
                     */
                    if (ch == 0) {
                        appendStringInfoString(lex->strval, "\\u0000");
                    } else if (GetDatabaseEncoding() == PG_UTF8) {
                        unicode_to_utf8(ch, (unsigned char *)utf8str);
                        utf8len = pg_utf_mblen((unsigned char *)utf8str);
                        appendBinaryStringInfo(lex->strval, utf8str, utf8len);
                    } else if (ch <= 0x007f) {
                        /*
                         * This is the only way to designate things like a
                         * form feed character in JSON, so it's useful in all
                         * encodings.
                         */
                        appendStringInfoChar(lex->strval, (char)ch);
                    } else {
                        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                                        errmsg("invalid input syntax for type json"),
                                        errdetail("Unicode escape values cannot be used for code point values above "
                                                  "007F when the server encoding is not UTF8."),
                                        report_json_context(lex)));
                    }
                }
            } else if (lex->strval != NULL) {
                if (hi_surrogate != -1) {
                    ereport(
                        ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Unicode low surrogate must follow a high surrogate."), report_json_context(lex)));
                }

                switch (*s) {
                    case '"':
                    case '\\':
                    case '/':
                        appendStringInfoChar(lex->strval, *s);
                        break;
                    case 'b':
                        appendStringInfoChar(lex->strval, '\b');
                        break;
                    case 'f':
                        appendStringInfoChar(lex->strval, '\f');
                        break;
                    case 'n':
                        appendStringInfoChar(lex->strval, '\n');
                        break;
                    case 'r':
                        appendStringInfoChar(lex->strval, '\r');
                        break;
                    case 't':
                        appendStringInfoChar(lex->strval, '\t');
                        break;
                    default:
                        /* Not a valid string escape, so error out. */
                        lex->token_terminator = s + pg_mblen(s);
                        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                                        errmsg("invalid input syntax for type json"),
                                        errdetail("Escape sequence \"\\%s\" is invalid.", extract_mb_char(s)),
                                        report_json_context(lex)));
                }
            } else if (strchr("\"\\/bfnrt", *s) == NULL) {
                /*
                 * Simpler processing if we're not bothered about de-escaping
                 *
                 * It's very tempting to remove the strchr() call here and
                 * replace it with a switch statement, but testing so far has
                 * shown it's not a performance win.
                 */
                lex->token_terminator = s + pg_mblen(s);
                ereport(
                    ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                     errdetail("Escape sequence \"\\%s\" is invalid.", extract_mb_char(s)), report_json_context(lex)));
            }
        } else if (lex->strval != NULL) {
            if (hi_surrogate != -1) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Unicode low surrogate must follow a high surrogate."), report_json_context(lex)));
            }
            appendStringInfoChar(lex->strval, *s);
        } else {
            int step = pg_mblen(s) - 1;
            s += step;
            len += step;
        }
    }

    if (hi_surrogate != -1) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                        errdetail("Unicode low surrogate must follow a high surrogate."), report_json_context(lex)));
    }

    /* Hooray, we found the end of the string! */
    lex->prev_token_terminator = lex->token_terminator;
    lex->token_terminator = s + 1;
}

/*-------------------------------------------------------------------------
 * The next token in the input stream is known to be a number; lex it.
 *
 * In JSON, a number consists of four parts:
 *
 * (1) An optional minus sign ('-').
 *
 * (2) Either a single '0', or a string of one or more digits that does not
 *       begin with a '0'.
 *
 * (3) An optional decimal part, consisting of a period ('.') followed by
 *       one or more digits.    (Note: While this part can be omitted
 *       completely, it's not OK to have only the decimal point without
 *       any digits afterwards.)
 *
 * (4) An optional exponent part, consisting of 'e' or 'E', optionally
 *       followed by '+' or '-', followed by one or more digits.    (Note:
 *       As with the decimal part, if 'e' or 'E' is present, it must be
 *       followed by at least one digit.)
 *
 * The 's' argument to this function points to the ostensible beginning
 * of part 2 - i.e. the character after any optional minus sign, and the
 * first character of the string if there is none.
 *
 *-------------------------------------------------------------------------
 */
static inline void json_lex_number(JsonLexContext *lex, char *s, bool *num_err)
{
    bool error = false;
    char *p = NULL;
    int len;

    len = s - lex->input;
    /* Part (1): leading sign indicator. */
    /* Caller already did this for us; so do nothing. */
    /* Part (2): parse main digit string. */
    if (*s == '0') {
        s++;
        len++;
    } else if (*s >= '1' && *s <= '9') {
        do {
            s++;
            len++;
        } while (len < lex->input_length && *s >= '0' && *s <= '9');
    } else {
        error = true;
    }

    /* Part (3): parse optional decimal portion. */
    if (len < lex->input_length && *s == '.') {
        s++;
        len++;
        if (len == lex->input_length || *s < '0' || *s > '9') {
            error = true;
        } else {
            do {
                s++;
                len++;
            } while (len < lex->input_length && *s >= '0' && *s <= '9');
        }
#ifdef DOLPHIN
        lex->ifint = true;
#endif
    }

    /* Part (4): parse optional exponent. */
    if (len < lex->input_length && (*s == 'e' || *s == 'E')) {
        s++;
        len++;
        if (len < lex->input_length && (*s == '+' || *s == '-')) {
            s++;
            len++;
        }
        if (len == lex->input_length || *s < '0' || *s > '9') {
            error = true;
        } else {
            do {
                s++;
                len++;
            } while (len < lex->input_length && *s >= '0' && *s <= '9');
        }
    }

    /*
     * Check for trailing garbage.    As in json_lex(), any alphanumeric stuff
     * here should be considered part of the token for error-reporting
     * purposes.
     */
    for (p = s; len < lex->input_length && JSON_ALPHANUMERIC_CHAR(*p); p++, len++) {
        error = true;
    }
    if (num_err != NULL) {
        /* let the caller handle the error */
        *num_err = error;
    } else {
        lex->prev_token_terminator = lex->token_terminator;
        lex->token_terminator = p;
        if (error) {
            report_invalid_token(lex);
        }
    }
}

/*
 * Report a parse error.
 *
 * lex->token_start and lex->token_terminator must identify the current token.
 */
static void report_parse_error(JsonParseContext ctx, JsonLexContext *lex)
{
    char *token = NULL;
    int toklen;
    errno_t rc = 0;

    /* Handle case where the input ended prematurely. */
    if (lex->token_start == NULL || lex->token_type == JSON_TOKEN_END) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                        errdetail("The input string ended unexpectedly."), report_json_context(lex)));
    }

    /* Separate out the current token. */
    toklen = lex->token_terminator - lex->token_start;
    token = (char *)palloc(toklen + 1);

    rc = memcpy_s(token, toklen + 1, lex->token_start, toklen);
    securec_check(rc, "\0", "\0");
    token[toklen] = '\0';

    /* Complain, with the appropriate detail message. */
    if (ctx == JSON_PARSE_END) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                        errdetail("Expected end of input, but found \"%s\".", token), report_json_context(lex)));
    } else {
        switch (ctx) {
            case JSON_PARSE_VALUE:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected JSON value, but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_STRING:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected string, but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_ARRAY_START:
                ereport(
                    ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                     errdetail("Expected array element or \"]\", but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_ARRAY_NEXT:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected \",\" or \"]\", but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_OBJECT_START:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected string or \"}\", but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_OBJECT_LABEL:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected \":\", but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_OBJECT_NEXT:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected \",\" or \"}\", but found \"%s\".", token), report_json_context(lex)));
                break;
            case JSON_PARSE_OBJECT_COMMA:
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                         errdetail("Expected string, but found \"%s\".", token), report_json_context(lex)));
                break;
            default:
                elog(ERROR, "unexpected json parse state: %d", ctx);
        }
    }
}

/*
 * Report an invalid input token.
 *
 * lex->token_start and lex->token_terminator must identify the token.
 */
static void report_invalid_token(JsonLexContext *lex)
{
    char *token = NULL;
    int toklen;
    errno_t rc = 0;

    /* Separate out the offending token. */
    toklen = lex->token_terminator - lex->token_start;
    token = (char *)palloc(toklen + 1);
    rc = memcpy_s(token, toklen + 1, lex->token_start, toklen);
    securec_check(rc, "\0", "\0");
    token[toklen] = '\0';

    ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type json"),
                    errdetail("Token \"%s\" is invalid.", token), report_json_context(lex)));
}

/*
 * Report a CONTEXT line for bogus JSON input.
 *
 * lex->token_terminator must be set to identify the spot where we detected
 * the error.  Note that lex->token_start might be NULL, in case we recognized
 * error at EOF.
 *
 * The return value isn't meaningful, but we make it non-void so that this
 * can be invoked inside ereport().
 */
static int report_json_context(JsonLexContext *lex)
{
    const char *context_start = NULL;
    const char *context_end = NULL;
    const char *line_start = NULL;
    int line_number;
    char *ctxt = NULL;
    int ctxtlen;
    const char *prefix = NULL;
    const char *suffix = NULL;
    errno_t rc = 0;

    /* Choose boundaries for the part of the input we will display */
    context_start = lex->input;
    context_end = lex->token_terminator;
    line_start = context_start;
    line_number = 1;
    for (;;) {
        /* Always advance over newlines */
        if (context_start < context_end && *context_start == '\n') {
            context_start++;
            line_start = context_start;
            line_number++;
            continue;
        }
        /* Otherwise, done as soon as we are close enough to context_end */
        if (context_end - context_start < 50) {
            break;
        }
        /* Advance to next multibyte character */
        if (IS_HIGHBIT_SET(*context_start)) {
            context_start += pg_mblen(context_start);
        } else {
            context_start++;
        }
    }

    /*
     * We add "..." to indicate that the excerpt doesn't start at the
     * beginning of the line ... but if we're within 3 characters of the
     * beginning of the line, we might as well just show the whole line.
     */
    if (context_start - line_start <= 3) {
        context_start = line_start;
    }

    /* Get a null-terminated copy of the data to present */
    ctxtlen = context_end - context_start;
    ctxt = (char *)palloc(ctxtlen + 1);
    rc = memcpy_s(ctxt, ctxtlen + 1, context_start, ctxtlen);
    securec_check(rc, "\0", "\0");
    ctxt[ctxtlen] = '\0';

    /*
     * Show the context, prefixing "..." if not starting at start of line, and
     * suffixing "..." if not ending at end of line.
     */
    prefix = (context_start > line_start) ? "..." : "";
    suffix = (lex->token_type != JSON_TOKEN_END && context_end - lex->input < lex->input_length &&
              *context_end != '\n' && *context_end != '\r')
                 ? "..."
                 : "";

    return errcontext("JSON data, line %d: %s%s%s", line_number, prefix, ctxt, suffix);
}

/*
 * Extract a single, possibly multi-byte char from the input string.
 */
static char *extract_mb_char(const char *s)
{
    char *res = NULL;
    int len;
    errno_t rc = 0;

    len = pg_mblen(s);
    res = (char *)palloc(len + 1);
    rc = memcpy_s(res, len + 1, s, len);
    securec_check(rc, "\0", "\0");
    res[len] = '\0';

    return res;
}

/*
 * Turn a scalar Datum into JSON, appending the string to "result".
 *
 * Hand off a non-scalar datum to composite_to_json or array_to_json_internal
 * as appropriate.
 */
static void datum_to_json(Datum val, bool is_null, StringInfo result, TYPCATEGORY tcategory, Oid typoutputfunc,
                          bool key_scalar)
{
    char *outputstr = NULL;
    text *jsontext = NULL;
    bool numeric_error = false;
    JsonLexContext dummy_lex;

    check_stack_depth();

    if (is_null) {
        appendStringInfoString(result, "null");
        return;
    }

    switch (tcategory) {
        case TYPCATEGORY_ARRAY:
            array_to_json_internal(val, result, false);
            break;
        case TYPCATEGORY_COMPOSITE:
            composite_to_json(val, result, false);
            break;
        case TYPCATEGORY_BOOLEAN:
            if (!key_scalar)
                appendStringInfoString(result, DatumGetBool(val) ? "true" : "false");
            else
                escape_json(result, DatumGetBool(val) ? "true" : "false");
            break;
        case TYPCATEGORY_NUMERIC:
            outputstr = OidOutputFunctionCall(typoutputfunc, val);
            if (key_scalar) {
                /* always quote keys */
                escape_json(result, outputstr);
            } else {
                /*
                 * Don't call escape_json for a non-key if it's a valid JSON
                 * number.
                 */
                dummy_lex.input = *outputstr == '-' ? outputstr + 1 : outputstr;
                dummy_lex.input_length = strlen(dummy_lex.input);
                json_lex_number(&dummy_lex, dummy_lex.input, &numeric_error);
                if (!numeric_error) {
                    appendStringInfoString(result, outputstr);
                } else {
                    escape_json(result, outputstr);
                }
            }
            pfree(outputstr);
            break;
        case TYPCATEGORY_JSON:
            /* JSON and JSONB will already be escaped */
            outputstr = OidOutputFunctionCall(typoutputfunc, val);
            appendStringInfoString(result, outputstr);
            pfree(outputstr);
            break;
        case TYPCATEGORY_JSON_CAST:
            jsontext = DatumGetTextP(OidFunctionCall1(typoutputfunc, val));
            outputstr = text_to_cstring(jsontext);
            appendStringInfoString(result, outputstr);
            pfree(outputstr);
            pfree(jsontext);
            break;
        default:
            outputstr = OidOutputFunctionCall(typoutputfunc, val);
            if (key_scalar && *outputstr == '\0') {
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("key value must not be empty")));
            }
            escape_json(result, outputstr);
            pfree(outputstr);
            break;
    }
}

/*
 * Process a single dimension of an array.
 * If it's the innermost dimension, output the values, otherwise call
 * ourselves recursively to process the next dimension.
 */
static void array_dim_to_json(StringInfo result, int dim, int ndims, int *dims, Datum *vals, bool *nulls, int *valcount,
                              TYPCATEGORY tcategory, Oid typoutputfunc, bool use_line_feeds)
{
    int i;
    const char *sep = NULL;

    Assert(dim < ndims);
    sep = use_line_feeds ? ",\n " : ",";
    appendStringInfoChar(result, '[');

    for (i = 1; i <= dims[dim]; i++) {
        if (i > 1) {
            appendStringInfoString(result, sep);
        }

        if (dim + 1 == ndims) {
            datum_to_json(vals[*valcount], nulls[*valcount], result, tcategory, typoutputfunc, false);
            (*valcount)++;
        } else {
            /*
             * Do we want line feeds on inner dimensions of arrays? For now
             * we'll say no.
             */
            array_dim_to_json(result, dim + 1, ndims, dims, vals, nulls, valcount, tcategory, typoutputfunc, false);
        }
    }
    appendStringInfoChar(result, ']');
}

/*
 * Turn an array into JSON.
 */
static void array_to_json_internal(Datum array, StringInfo result, bool use_line_feeds)
{
    ArrayType *v = DatumGetArrayTypeP(array);
    Oid element_type = ARR_ELEMTYPE(v);
    int *dim = NULL;
    int ndim;
    int nitems;
    int count = 0;
    Datum *elements = NULL;
    bool *nulls = NULL;
    int16 typlen;
    bool typbyval = false;
    char typalign, typdelim;
    Oid typioparam;
    Oid typoutputfunc;
    TYPCATEGORY tcategory;
    Oid castfunc = InvalidOid;

    ndim = ARR_NDIM(v);
    dim = ARR_DIMS(v);
    nitems = ArrayGetNItems(ndim, dim);

    if (nitems <= 0) {
        appendStringInfoString(result, "[]");
        return;
    }

    get_type_io_data(element_type, IOFunc_output, &typlen, &typbyval, &typalign, &typdelim, &typioparam,
                     &typoutputfunc);

    if (element_type > FirstNormalObjectId) {
        HeapTuple tuple;
        Form_pg_cast castForm;

        tuple = SearchSysCache2(CASTSOURCETARGET, ObjectIdGetDatum(element_type), ObjectIdGetDatum(JSONOID));
        if (HeapTupleIsValid(tuple)) {
            castForm = (Form_pg_cast)GETSTRUCT(tuple);

            if (castForm->castmethod == COERCION_METHOD_FUNCTION) {
                castfunc = typoutputfunc = castForm->castfunc;
            }

            ReleaseSysCache(tuple);
        }
    }

    deconstruct_array(v, element_type, typlen, typbyval, typalign, &elements, &nulls, &nitems);

    if (castfunc != InvalidOid) {
        tcategory = TYPCATEGORY_JSON_CAST;
    } else if (element_type == RECORDOID) {
        tcategory = TYPCATEGORY_COMPOSITE;
    } else if (element_type == JSONOID || element_type == JSONBOID) {
        tcategory = TYPCATEGORY_JSON;
    } else {
        tcategory = TypeCategory(element_type);
    }

    array_dim_to_json(result, 0, ndim, dim, elements, nulls, &count, tcategory, typoutputfunc, use_line_feeds);

    pfree(elements);
    pfree(nulls);
}

/*
 * Turn a composite / record into JSON.
 */
static void composite_to_json(Datum composite, StringInfo result, bool use_line_feeds)
{
    HeapTupleHeader td;
    Oid tupType;
    int32 tupTypmod;
    TupleDesc tupdesc;
    HeapTupleData tmptup;
    HeapTupleData *tuple = NULL;
    int i;
    bool needsep = false;
    const char *sep = NULL;

    sep = use_line_feeds ? ",\n " : ",";
    td = DatumGetHeapTupleHeader(composite);

    /* Extract rowtype info and find a tupdesc */
    tupType = HeapTupleHeaderGetTypeId(td);
    tupTypmod = HeapTupleHeaderGetTypMod(td);
    tupdesc = lookup_rowtype_tupdesc(tupType, tupTypmod);

    /* Build a temporary HeapTuple control structure */
    tmptup.t_len = HeapTupleHeaderGetDatumLength(td);
    tmptup.t_data = td;
    tuple = &tmptup;
    appendStringInfoChar(result, '{');

    for (i = 0; i < tupdesc->natts; i++) {
        Datum val;
        bool isnull = false;
        char *attname = NULL;
        TYPCATEGORY tcategory;
        Oid typoutput;
        bool typisvarlena = false;
        Oid castfunc = InvalidOid;

        if (tupdesc->attrs[i].attisdropped) {
            continue;
        }
        if (needsep) {
            appendStringInfoString(result, sep);
        }
        needsep = true;

        attname = NameStr(tupdesc->attrs[i].attname);
        escape_json(result, attname);
        appendStringInfoChar(result, ':');
        val = heap_getattr(tuple, i + 1, tupdesc, &isnull);
        getTypeOutputInfo(tupdesc->attrs[i].atttypid, &typoutput, &typisvarlena);

        if (tupdesc->attrs[i].atttypid > FirstNormalObjectId) {
            HeapTuple cast_tuple;
            Form_pg_cast castForm;
            cast_tuple = SearchSysCache2(CASTSOURCETARGET, ObjectIdGetDatum(tupdesc->attrs[i].atttypid),
                                         ObjectIdGetDatum(JSONOID));
            if (HeapTupleIsValid(cast_tuple)) {
                castForm = (Form_pg_cast)GETSTRUCT(cast_tuple);

                if (castForm->castmethod == COERCION_METHOD_FUNCTION) {
                    castfunc = typoutput = castForm->castfunc;
                }
                ReleaseSysCache(cast_tuple);
            }
        }

        if (castfunc != InvalidOid) {
            tcategory = TYPCATEGORY_JSON_CAST;
        } else if (tupdesc->attrs[i].atttypid == RECORDARRAYOID) {
            tcategory = TYPCATEGORY_ARRAY;
        } else if (tupdesc->attrs[i].atttypid == RECORDOID) {
            tcategory = TYPCATEGORY_COMPOSITE;
        } else if (tupdesc->attrs[i].atttypid == JSONOID || tupdesc->attrs[i].atttypid == JSONBOID) {
            tcategory = TYPCATEGORY_JSON;
        } else {
            tcategory = TypeCategory(tupdesc->attrs[i].atttypid);
        }
        datum_to_json(val, isnull, result, tcategory, typoutput, false);
    }
    appendStringInfoChar(result, '}');
    ReleaseTupleDesc(tupdesc);
}

/*
 * append Json for orig_val to result. If it's a field key, make sure it's
 * of an acceptable type and is quoted.
 */
static void add_json(Datum val, bool is_null, StringInfo result, Oid val_type, bool key_scalar)
{
    TYPCATEGORY tcategory;
    Oid typoutput;
    bool typisvarlena = false;
    Oid castfunc = InvalidOid;

    if (val_type == InvalidOid) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("could not determine input data type")));
    }

    getTypeOutputInfo(val_type, &typoutput, &typisvarlena);
    if (val_type > FirstNormalObjectId) {
        HeapTuple tuple;
        Form_pg_cast castForm;

        tuple = SearchSysCache2(CASTSOURCETARGET, ObjectIdGetDatum(val_type), ObjectIdGetDatum(JSONOID));
        if (HeapTupleIsValid(tuple)) {
            castForm = (Form_pg_cast)GETSTRUCT(tuple);
            if (castForm->castmethod == COERCION_METHOD_FUNCTION) {
                castfunc = typoutput = castForm->castfunc;
            }
            ReleaseSysCache(tuple);
        }
    }
    if (castfunc != InvalidOid) {
        tcategory = TYPCATEGORY_JSON_CAST;
    } else if (val_type == RECORDARRAYOID) {
        tcategory = TYPCATEGORY_ARRAY;
    } else if (val_type == RECORDOID) {
        tcategory = TYPCATEGORY_COMPOSITE;
    } else if (val_type == JSONOID) {
        tcategory = TYPCATEGORY_JSON;
    } else {
        tcategory = TypeCategory(val_type);
    }

    if (key_scalar && (tcategory == TYPCATEGORY_ARRAY || tcategory == TYPCATEGORY_COMPOSITE ||
                       tcategory == TYPCATEGORY_JSON || tcategory == TYPCATEGORY_JSON_CAST)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("key value must be scalar, not array, composite or json")));
    }

    datum_to_json(val, is_null, result, tcategory, typoutput, key_scalar);
}

/*
 * SQL function array_to_json(row)
 */
extern Datum array_to_json(PG_FUNCTION_ARGS)
{
    Datum array = PG_GETARG_DATUM(0);
    StringInfo result;

    result = makeStringInfo();
    array_to_json_internal(array, result, false);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * SQL function array_to_json(row, prettybool)
 */
extern Datum array_to_json_pretty(PG_FUNCTION_ARGS)
{
    Datum array = PG_GETARG_DATUM(0);
    bool use_line_feeds = PG_GETARG_BOOL(1);
    StringInfo result;

    result = makeStringInfo();
    array_to_json_internal(array, result, use_line_feeds);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * SQL function row_to_json(row)
 */
extern Datum row_to_json(PG_FUNCTION_ARGS)
{
    Datum array = PG_GETARG_DATUM(0);
    StringInfo result;

    result = makeStringInfo();
    composite_to_json(array, result, false);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * SQL function row_to_json(row, prettybool)
 */
extern Datum row_to_json_pretty(PG_FUNCTION_ARGS)
{
    Datum array = PG_GETARG_DATUM(0);
    bool use_line_feeds = PG_GETARG_BOOL(1);
    StringInfo result;

    result = makeStringInfo();
    composite_to_json(array, result, use_line_feeds);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * SQL function to_json(anyvalue)
 */
Datum to_json(PG_FUNCTION_ARGS)
{
    Datum val = PG_GETARG_DATUM(0);
    Oid val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    StringInfo result;
    TYPCATEGORY tcategory;
    Oid typoutput;
    bool typisvarlena = false;
    Oid castfunc = InvalidOid;

    if (val_type == InvalidOid) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("could not determine input data type")));
    }

    result = makeStringInfo();
    getTypeOutputInfo(val_type, &typoutput, &typisvarlena);
    if (val_type > FirstNormalObjectId) {
        HeapTuple tuple;
        Form_pg_cast castForm;
        tuple = SearchSysCache2(CASTSOURCETARGET, ObjectIdGetDatum(val_type), ObjectIdGetDatum(JSONOID));
        if (HeapTupleIsValid(tuple)) {
            castForm = (Form_pg_cast)GETSTRUCT(tuple);
            if (castForm->castmethod == COERCION_METHOD_FUNCTION) {
                castfunc = typoutput = castForm->castfunc;
            }
            ReleaseSysCache(tuple);
        }
    }

    if (castfunc != InvalidOid) {
        tcategory = TYPCATEGORY_JSON_CAST;
    } else if (val_type == RECORDARRAYOID) {
        tcategory = TYPCATEGORY_ARRAY;
    } else if (val_type == RECORDOID) {
        tcategory = TYPCATEGORY_COMPOSITE;
    } else if (val_type == JSONOID || val_type == JSONBOID) {
        tcategory = TYPCATEGORY_JSON;
    } else {
        tcategory = TypeCategory(val_type);
    }
    datum_to_json(val, false, result, tcategory, typoutput, false);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * json_agg transition function
 */
Datum json_agg_transfn(PG_FUNCTION_ARGS)
{
    Oid val_type = get_fn_expr_argtype(fcinfo->flinfo, 1);
    MemoryContext aggcontext, oldcontext;
    StringInfo state;
    Datum val;
    TYPCATEGORY tcategory;
    Oid typoutput;
    bool typisvarlena = false;
    Oid castfunc = InvalidOid;

    if (val_type == InvalidOid) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("could not determine input data type")));
    }

    if (!AggCheckCallContext(fcinfo, &aggcontext)) {
        /* cannot be called directly because of internal-type argument */
        elog(ERROR, "json_agg_transfn called in non-aggregate context");
    }

    if (PG_ARGISNULL(0)) {
        /*
         * Make this StringInfo in a context where it will persist for the
         * duration off the aggregate call. It's only needed for this initial
         * piece, as the StringInfo routines make sure they use the right
         * context to enlarge the object if necessary.
         */
        oldcontext = MemoryContextSwitchTo(aggcontext);
        state = makeStringInfo();
        MemoryContextSwitchTo(oldcontext);

        appendStringInfoChar(state, '[');
    } else {
        state = (StringInfo)PG_GETARG_POINTER(0);
        appendStringInfoString(state, ", ");
    }

    /* fast path for NULLs */
    if (PG_ARGISNULL(1)) {
        val = (Datum)0;
        datum_to_json(val, true, state, 0, InvalidOid, false);
        PG_RETURN_POINTER(state);
    }

    val = PG_GETARG_DATUM(1);
    getTypeOutputInfo(val_type, &typoutput, &typisvarlena);
    if (val_type > FirstNormalObjectId) {
        HeapTuple tuple;
        Form_pg_cast castForm;

        tuple = SearchSysCache2(CASTSOURCETARGET, ObjectIdGetDatum(val_type), ObjectIdGetDatum(JSONOID));
        if (HeapTupleIsValid(tuple)) {
            castForm = (Form_pg_cast)GETSTRUCT(tuple);
            if (castForm->castmethod == COERCION_METHOD_FUNCTION) {
                castfunc = typoutput = castForm->castfunc;
            }
            ReleaseSysCache(tuple);
        }
    }

    if (castfunc != InvalidOid) {
        tcategory = TYPCATEGORY_JSON_CAST;
    } else if (val_type == RECORDARRAYOID) {
        tcategory = TYPCATEGORY_ARRAY;
    } else if (val_type == RECORDOID) {
        tcategory = TYPCATEGORY_COMPOSITE;
    } else if (val_type == JSONOID || val_type == JSONBOID) {
        tcategory = TYPCATEGORY_JSON;
    } else {
        tcategory = TypeCategory(val_type);
    }
    if (!PG_ARGISNULL(0) && (tcategory == TYPCATEGORY_ARRAY || tcategory == TYPCATEGORY_COMPOSITE)) {
        appendStringInfoString(state, "\n ");
    }
    datum_to_json(val, false, state, tcategory, typoutput, false);
    /*
     * The transition type for array_agg() is declared to be "internal", which
     * is a pass-by-value type the same size as a pointer.    So we can safely
     * pass the ArrayBuildState pointer through nodeAgg.c's machinations.
     */
    PG_RETURN_POINTER(state);
}

/*
 * json_agg final function
 */
Datum json_agg_finalfn(PG_FUNCTION_ARGS)
{
    StringInfo state;

    /* cannot be called directly because of internal-type argument */
    Assert(AggCheckCallContext(fcinfo, NULL));
    state = PG_ARGISNULL(0) ? NULL : (StringInfo)PG_GETARG_POINTER(0);
    if (state == NULL) {
        PG_RETURN_NULL();
    }
    appendStringInfoChar(state, ']');
    PG_RETURN_TEXT_P(cstring_to_text_with_len(state->data, state->len));
}

/*
 * json_object_agg transition function.
 *
 * aggregate two input columns as a single json value.
 */
Datum json_object_agg_transfn(PG_FUNCTION_ARGS)
{
    Oid val_type;
    MemoryContext aggcontext, oldcontext;
    StringInfo state;
    Datum arg;

    if (!AggCheckCallContext(fcinfo, &aggcontext)) {
        /* cannot be called directly because of internal-type argument */
        elog(ERROR, "json_agg_transfn called in non-aggregate context");
    }

    if (PG_ARGISNULL(0)) {
        /*
         * Make this StringInfo in a context where it will persist for the
         * duration off the aggregate call. It's only needed for this initial
         * piece, as the StringInfo routines make sure they use the right
         * context to enlarge the object if necessary.
         */
        oldcontext = MemoryContextSwitchTo(aggcontext);
        state = makeStringInfo();
        MemoryContextSwitchTo(oldcontext);
        appendStringInfoString(state, "{ ");
    } else {
        state = (StringInfo)PG_GETARG_POINTER(0);
        appendStringInfoString(state, ", ");
    }

    if (PG_ARGISNULL(1)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("field name must not be null")));
    }
    val_type = get_fn_expr_argtype(fcinfo->flinfo, 1);
    /*
     * turn a constant (more or less literal) value that's of unknown type
     * into text. Unknowns come in as a cstring pointer.
     */
    if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, 1)) {
        val_type = TEXTOID;
        arg = CStringGetTextDatum(PG_GETARG_POINTER(1));
    } else {
        arg = PG_GETARG_DATUM(1);
    }

    if (val_type == InvalidOid || val_type == UNKNOWNOID) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg 1: could not determine data type")));
    }
    add_json(arg, false, state, val_type, true);
    appendStringInfoString(state, " : ");
    val_type = get_fn_expr_argtype(fcinfo->flinfo, 2);
    /* see comments above */
    if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, 2)) {
        val_type = TEXTOID;
        if (PG_ARGISNULL(2)) {
            arg = (Datum)0;
        } else {
            arg = CStringGetTextDatum(PG_GETARG_POINTER(2));
        }
    } else {
        arg = PG_GETARG_DATUM(2);
    }

    if (val_type == InvalidOid || val_type == UNKNOWNOID) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg 2: could not determine data type")));
    }
    add_json(arg, PG_ARGISNULL(2), state, val_type, false);

    PG_RETURN_POINTER(state);
}

/*
 * json_object_agg final function.
 *
 */
Datum json_object_agg_finalfn(PG_FUNCTION_ARGS)
{
    StringInfo state;

    /* cannot be called directly because of internal-type argument */
    Assert(AggCheckCallContext(fcinfo, NULL));
    state = PG_ARGISNULL(0) ? NULL : (StringInfo)PG_GETARG_POINTER(0);
    if (state == NULL) {
        PG_RETURN_TEXT_P(cstring_to_text("{}"));
    }

    appendStringInfoString(state, " }");
    PG_RETURN_TEXT_P(cstring_to_text_with_len(state->data, state->len));
}

/*
 * SQL function json_build_object(variadic "any")
 */
Datum json_build_object(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    int i;
    Datum arg;
    char *sep = "";
    StringInfo result;
    Oid val_type;

    if (nargs % 2 != 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("invalid number or arguments: object must be matched key value pairs")));
    }

    result = makeStringInfo();
    appendStringInfoChar(result, '{');
    for (i = 0; i < nargs; i += 2) {
        /* process key */
        if (PG_ARGISNULL(i)) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg %d: key cannot be null", i + 1)));
        }
        val_type = get_fn_expr_argtype(fcinfo->flinfo, i);
        /*
         * turn a constant (more or less literal) value that's of unknown type
         * into text. Unknowns come in as a cstring pointer.
         */
        if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, i)) {
            val_type = TEXTOID;
            if (PG_ARGISNULL(i)) {
                arg = (Datum)0;
            } else {
                arg = CStringGetTextDatum(PG_GETARG_POINTER(i));
            }
        } else {
            arg = PG_GETARG_DATUM(i);
        }
        if (val_type == InvalidOid || val_type == UNKNOWNOID) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg %d: could not determine data type", i + 1)));
        }
        appendStringInfoString(result, sep);
        sep = ", ";
        add_json(arg, false, result, val_type, true);
        appendStringInfoString(result, " : ");
        /* process value */
        val_type = get_fn_expr_argtype(fcinfo->flinfo, i + 1);
        /* see comments above */
        if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, i + 1)) {
            val_type = TEXTOID;
            if (PG_ARGISNULL(i + 1)) {
                arg = (Datum)0;
            } else {
                arg = CStringGetTextDatum(PG_GETARG_POINTER(i + 1));
            }
        } else {
            arg = PG_GETARG_DATUM(i + 1);
        }
        if (val_type == InvalidOid || val_type == UNKNOWNOID) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg %d: could not determine data type", i + 2)));
        }
        add_json(arg, PG_ARGISNULL(i + 1), result, val_type, false);
    }
    appendStringInfoChar(result, '}');
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * degenerate case of json_build_object where it gets 0 arguments.
 */
Datum json_build_object_noargs(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text_with_len("{}", 2));
}

/*
 * SQL function json_build_array(variadic "any")
 */
Datum json_build_array(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    int i;
    Datum arg;
    char *sep = "";
    StringInfo result;
    Oid val_type;

    result = makeStringInfo();
    appendStringInfoChar(result, '[');
    for (i = 0; i < nargs; i++) {
        val_type = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i + 1);
        /* see comments in json_build_object above */
        if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, i)) {
            val_type = TEXTOID;
            if (PG_ARGISNULL(i)) {
                arg = (Datum)0;
            } else {
                arg = CStringGetTextDatum(PG_GETARG_POINTER(i));
            }
        } else {
            arg = PG_GETARG_DATUM(i);
        }
        if (val_type == InvalidOid || val_type == UNKNOWNOID) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg %d: could not determine data type", i + 1)));
        }
        appendStringInfoString(result, sep);
        sep = ", ";
        add_json(arg, PG_ARGISNULL(i), result, val_type, false);
    }
    appendStringInfoChar(result, ']');
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

/*
 * degenerate case of json_build_array where it gets 0 arguments.
 */
Datum json_build_array_noargs(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text_with_len("[]", 2));
}

/*
 * SQL function json_object(text[])
 *
 * take a one or two dimensional array of text as name vale pairs
 * for a json object.
 *
 */
Datum json_object(PG_FUNCTION_ARGS)
{
    ArrayType *in_array = PG_GETARG_ARRAYTYPE_P(0);
    int ndims = ARR_NDIM(in_array);
    StringInfoData result;
    Datum *in_datums = NULL;
    bool *in_nulls = NULL;
    int in_count, count, i;
    text *rval = NULL;
    char *v = NULL;

    switch (ndims) {
        case 0:
            PG_RETURN_DATUM(CStringGetTextDatum("{}"));
            break;
        case 1:
            if ((ARR_DIMS(in_array)[0]) % 2)
                ereport(ERROR,
                        (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("array must have even number of elements")));
            break;
        case 2:
            if ((ARR_DIMS(in_array)[1]) != 2)
                ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("array must have two columns")));
            break;
        default:
            ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("wrong number of array subscripts")));
    }

    deconstruct_array(in_array, TEXTOID, -1, false, 'i', &in_datums, &in_nulls, &in_count);
    count = in_count / 2;
    initStringInfo(&result);
    appendStringInfoChar(&result, '{');
    for (i = 0; i < count; ++i) {
        if (in_nulls[i * 2]) {
            ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("null value not allowed for object key")));
        }

        v = TextDatumGetCString(in_datums[i * 2]);
        if (v[0] == '\0') {
            ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("empty value not allowed for object key")));
        }
        if (i > 0) {
            appendStringInfoString(&result, ", ");
        }
        escape_json(&result, v);
        appendStringInfoString(&result, " : ");
        pfree(v);
        if (in_nulls[i * 2 + 1]) {
            appendStringInfoString(&result, "null");
        } else {
            v = TextDatumGetCString(in_datums[i * 2 + 1]);
            escape_json(&result, v);
            pfree(v);
        }
    }

    appendStringInfoChar(&result, '}');
    pfree(in_datums);
    pfree(in_nulls);

    rval = cstring_to_text_with_len(result.data, result.len);
    pfree(result.data);
    PG_RETURN_TEXT_P(rval);
}

/*
 * SQL function json_object(text[], text[])
 *
 * take separate name and value arrays of text to construct a json object
 * pairwise.
 */
Datum json_object_two_arg(PG_FUNCTION_ARGS)
{
    ArrayType *key_array = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *val_array = PG_GETARG_ARRAYTYPE_P(1);
    int nkdims = ARR_NDIM(key_array);
    int nvdims = ARR_NDIM(val_array);
    StringInfoData result;
    Datum *key_datums = NULL;
    Datum *val_datums = NULL;
    bool *key_nulls = NULL;
    bool *val_nulls = NULL;
    int key_count, val_count, i;
    text *rval = NULL;
    char *v = NULL;

    if (nkdims > 1 || nkdims != nvdims) {
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("wrong number of array subscripts")));
    }

    if (nkdims == 0) {
        PG_RETURN_DATUM(CStringGetTextDatum("{}"));
    }

    deconstruct_array(key_array, TEXTOID, -1, false, 'i', &key_datums, &key_nulls, &key_count);
    deconstruct_array(val_array, TEXTOID, -1, false, 'i', &val_datums, &val_nulls, &val_count);
    if (key_count != val_count) {
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("mismatched array dimensions")));
    }

    initStringInfo(&result);
    appendStringInfoChar(&result, '{');
    for (i = 0; i < key_count; ++i) {
        if (key_nulls[i]) {
            ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("null value not allowed for object key")));
        }

        v = TextDatumGetCString(key_datums[i]);
        if (v[0] == '\0') {
            ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("empty value not allowed for object key")));
        }

        if (i > 0) {
            appendStringInfoString(&result, ", ");
        }
        escape_json(&result, v);
        appendStringInfoString(&result, " : ");
        pfree(v);
        if (val_nulls[i]) {
            appendStringInfoString(&result, "null");
        } else {
            v = TextDatumGetCString(val_datums[i]);
            escape_json(&result, v);
            pfree(v);
        }
    }
    appendStringInfoChar(&result, '}');

    pfree(key_datums);
    pfree(key_nulls);
    pfree(val_datums);
    pfree(val_nulls);
    rval = cstring_to_text_with_len(result.data, result.len);
    pfree(result.data);

    PG_RETURN_TEXT_P(rval);
}

/*
 * Produce a JSON string literal, properly escaping characters in the text.
 */
void escape_json(StringInfo buf, const char *str)
{
    const char *p = NULL;
    int charlen = 0;

    appendStringInfoCharMacro(buf, '\"');
    p = str;
    while (*p != '\0') {
        charlen = pg_mblen(p);
        switch (*p) {
            case '\b':
                appendStringInfoString(buf, "\\b");
                break;
            case '\f':
                appendStringInfoString(buf, "\\f");
                break;
            case '\n':
                appendStringInfoString(buf, "\\n");
                break;
            case '\r':
                appendStringInfoString(buf, "\\r");
                break;
            case '\t':
                appendStringInfoString(buf, "\\t");
                break;
            case '"':
                appendStringInfoString(buf, "\\\"");
                break;
            case '\\':
                appendStringInfoString(buf, "\\\\");
                break;
            default:
                if ((unsigned char)*p < ' ') {
                    appendStringInfo(buf, "\\u%04x", (int)*p);
                } else {
                    for (int i = 0; i < charlen; i++) {
                        appendStringInfoCharMacro(buf, *(p + i));
                    }
                }
                break;
        }
        p += charlen;
    }
    appendStringInfoCharMacro(buf, '\"');
}

/*
 * SQL function json_typeof(json) -> text
 *
 * Returns the type of the outermost JSON value as TEXT.  Possible types are
 * "object", "array", "string", "number", "boolean", and "null".
 *
 * Performs a single call to json_lex() to get the first token of the supplied
 * value.  This initial token uniquely determines the value's type.  As our
 * input must already have been validated by json_in() or json_recv(), the
 * initial token should never be JSON_TOKEN_OBJECT_END, JSON_TOKEN_ARRAY_END,
 * JSON_TOKEN_COLON, JSON_TOKEN_COMMA, or JSON_TOKEN_END.
 */

#ifdef DOLPHIN
static const char *type_case(JsonTokenType tok, bool flag)
{
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            return flag ? "OBJECT" : "object";
        case JSON_TOKEN_ARRAY_START:
            return flag ? "ARRAY" : "array";
        case JSON_TOKEN_STRING:
            return flag ? "STRING" : "string";
        case JSON_TOKEN_NUMBER:
            return flag ? "NUMBER" : "number";
        case JSON_TOKEN_TRUE:
        case JSON_TOKEN_FALSE:
            return flag ? "BOOLEAN" : "boolean";
        case JSON_TOKEN_NULL:
            return flag ? "NULL" : "null";
        case JSON_TOKEN_INTEGER:
            return "INTEGER";
        case JSON_TOKEN_DOUBLE:
            return "DOUBLE";
        default:
            elog(ERROR, "unexpected json token: %d", tok);
            return 0;
    }
}
#endif

Datum json_typeof(PG_FUNCTION_ARGS)
{
    text *json = NULL;

    JsonLexContext *lex = NULL;
    JsonTokenType tok;
    const char *type = NULL;

    json = PG_GETARG_TEXT_P(0);
    lex = makeJsonLexContext(json, false);

    /* Lex exactly one token from the input and check its type. */
    json_lex(lex);
    tok = lex_peek(lex);
    type = type_case(tok);
    PG_RETURN_TEXT_P(cstring_to_text(type));
}

#ifdef DOLPHIN
Datum json_array(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(json_build_array(fcinfo));
}

extern void get_keys_order(char **a, int l, int r, int pos[])
{
    char *mid = a[pos[(l + r) / 2]];
    int i = l, j = r;

    while (i <= j) {
        while ((strlen(a[pos[i]]) < strlen(mid) || (strcmp(a[pos[i]], mid) < 0 && strlen(mid) == strlen(a[pos[i]]))))
            i++;
        while ((strlen(a[pos[j]]) > strlen(mid) || (strcmp(a[pos[j]], mid) > 0 && strlen(mid) == strlen(a[pos[j]]))))
            j--;
        if (i <= j) {
            int t = pos[i];
            pos[i] = pos[j];
            pos[j] = t;
            i++;
            j--;
        }
    }
    if (l < j)
        get_keys_order(a, l, j, pos);
    if (i < r)
        get_keys_order(a, i, r, pos);
}

Datum json_object_mysql(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();

    if (nargs % 2 != 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("invalid number or arguments: object must be matched key value pairs")));
    }

    int i;
    Datum arg;
    Oid val_type;
    int cnt = nargs / 2;
    int order = 0;
    char *sep = "";
    StringInfo result;
    char *keys[cnt];
    bool isValid[cnt];
    int pos[cnt];
    TYPCATEGORY tcategory;
    Oid typoutput;
    bool typisvarlena = false;
    Oid castfunc = InvalidOid;
    char *outputstr = NULL;
    StringInfo tempstr;
    bool is_null = false;
    for (i = 0; i < cnt; i++) {
        if (PG_ARGISNULL(i * 2)) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("arg %d: key cannot be null", 2 * i + 1)));
        }
        val_type = get_fn_expr_argtype(fcinfo->flinfo, i * 2);
        if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, i * 2)) {
            val_type = TEXTOID;
            if (PG_ARGISNULL(i * 2)) {
                arg = (Datum)0;
            } else {
                arg = CStringGetTextDatum(PG_GETARG_POINTER(i * 2));
            }
        } else {
            arg = PG_GETARG_DATUM(i * 2);
        }
        if (val_type == InvalidOid || val_type == UNKNOWNOID) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("arg %d: could not determine data type", 2 * i + 1)));
        }
        getTypeOutputInfo(val_type, &typoutput, &typisvarlena);
        if (val_type > FirstNormalObjectId) {
            HeapTuple tuple;
            Form_pg_cast castForm;

            tuple = SearchSysCache2(CASTSOURCETARGET, ObjectIdGetDatum(val_type), ObjectIdGetDatum(JSONOID));
            if (HeapTupleIsValid(tuple)) {
                castForm = (Form_pg_cast)GETSTRUCT(tuple);
                if (castForm->castmethod == COERCION_METHOD_FUNCTION) {
                    castfunc = typoutput = castForm->castfunc;
                }
                ReleaseSysCache(tuple);
            }
        }
        if (castfunc != InvalidOid) {
            tcategory = TYPCATEGORY_JSON_CAST;
        } else if (val_type == RECORDARRAYOID) {
            tcategory = TYPCATEGORY_ARRAY;
        } else if (val_type == RECORDOID) {
            tcategory = TYPCATEGORY_COMPOSITE;
        } else if (val_type == JSONOID) {
            tcategory = TYPCATEGORY_JSON;
        } else {
            tcategory = TypeCategory(val_type);
        }
        switch (tcategory) {
            case TYPCATEGORY_BOOLEAN:
                tempstr = makeStringInfo();
                escape_json(tempstr, DatumGetBool(arg) ? "1" : "0");
                keys[i] = tempstr->data;
                break;
            case TYPCATEGORY_NUMERIC:
                tempstr = makeStringInfo();
                outputstr = OidOutputFunctionCall(typoutput, arg);
                escape_json(tempstr, outputstr);
                keys[i] = tempstr->data;
                pfree(outputstr);
                break;
            default:
                tempstr = makeStringInfo();
                outputstr = OidOutputFunctionCall(typoutput, arg);
                escape_json(tempstr, outputstr);
                keys[i] = tempstr->data;
                pfree(outputstr);
                break;
        }
    }
    for (i = 0; i < cnt; i++) {
        pos[i] = i;
    }
    get_keys_order(keys, 0, cnt - 1, pos);
    isValid[0] = true;
    for (i = 1; i < cnt; i++) {
        isValid[i] = false;
    }
    for (i = 0; i < cnt; i++) {
        for (int j = 0; j < i; j++) {
            if (strcmp(keys[i], keys[j]) == 0) {
                break;
            } else if (j == (i - 1)) {
                isValid[i] = true;
            }
        }
    }

    result = makeStringInfo();
    appendStringInfoChar(result, '{');
    for (i = 0; i < nargs; i += 2) {
        is_null = false;
        order = i / 2;
        if (isValid[pos[order]] == false) {
            continue;
        }
        /* process key */
        appendStringInfoString(result, sep);
        sep = ", ";
        appendStringInfoString(result, keys[pos[order]]);
        appendStringInfoString(result, ": ");
        /* process value */
        val_type = get_fn_expr_argtype(fcinfo->flinfo, 2 * pos[order] + 1);
        /* see comments above */
        if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, 2 * pos[order] + 1)) {
            val_type = TEXTOID;
            if (PG_ARGISNULL(2 * pos[order] + 1)) {
                arg = (Datum)0;
                is_null = true;
            } else {
                arg = CStringGetTextDatum(PG_GETARG_POINTER(2 * pos[order] + 1));
            }
        } else {
            arg = PG_GETARG_DATUM(2 * pos[order] + 1);
        }
        if (val_type == InvalidOid || val_type == UNKNOWNOID) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("arg %d: could not determine data type", 2 * pos[order] + 2)));
        }
        add_json(arg, is_null, result, val_type, false);
    }
    appendStringInfoChar(result, '}');
    for (int i = 0; i < cnt; i++) {
        pfree(keys[i]);
    }
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

Datum json_object_noarg(PG_FUNCTION_ARGS)
{
    StringInfo result;

    result = makeStringInfo();
    appendStringInfoChar(result, '{');
    appendStringInfoChar(result, '}');
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

Datum json_quote(PG_FUNCTION_ARGS)
{
    text *str = PG_GETARG_TEXT_PP(0);
    Datum val = PointerGetDatum(str);
    Oid val_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    StringInfo result;
    TYPCATEGORY tcategory;
    Oid typoutput;
    bool typisvarlena = false;

    result = makeStringInfo();
    getTypeOutputInfo(val_type, &typoutput, &typisvarlena);
    tcategory = TypeCategory(val_type);
    datum_to_json(val, false, result, tcategory, typoutput, false);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

static bool is_valid_unicode_escape_sequence(char *str)
{
    int i;
    for (i = 0; i < UNICODE_NUM_LEN; i++) {
        char c = str[i];
        if (c == '\0' || !((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            return false;
        }
    }
    return true;
}

static bool is_valid_unicode_escape(char *str)
{
    int len = strlen(str);
    int i = 0;
    while (i < len) {
        if (str[i] == '\\' && str[i + 1] == 'u') {
            if (!is_valid_unicode_escape_sequence(str + i + UNICODE_ESCAPE_LEN)) {
                return false;
            }
            i += UNICODE_STR_LEN;
            continue;
        }
        i++;
    }
    return true;
}

static char *unescape_unicode(const char *str)
{
    size_t len = strlen(str);
    char *buffer = (char *)palloc0(len + 1);
    errno_t rc = EOK;
    int bufferTail = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\\' && str[i + 1] == 'u') {
            char hex[5];
            rc = strncpy_s(hex, sizeof(hex), str + i + UNICODE_ESCAPE_LEN, UNICODE_NUM_LEN);
            securec_check(rc, "\0", "\0");
            wchar_t wc = (wchar_t)strtoul(hex, NULL, HEXADECIMAL);
            char utf8[5] = {0};
            if (int(wcrtomb(utf8, wc, NULL)) == -1) {
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("Incorrent hex digit after \\u escape in string")));
            }
            rc = strcat_s(buffer, len + 1, utf8);
            securec_check(rc, "\0", "\0");
            bufferTail += strlen(utf8);
            i += UNICODE_LEN;
        } else {
            buffer[bufferTail++] = str[i];
        }
    }

    return buffer;
}

static void DelChar(char *inStr, char *outStr, int &a, int &b)
{
    char *tmp;
    char *tep;
    tmp = outStr;
    tep = inStr;
    while (*tep != '\0') {
        while (*tep == '"') {
            tep++;
            a++;
        }
        if ((*tep == '\\') && (*(tep + 1) != '\\') && (*(tep - 1) != '\\')) {
            if ((*(tep + 1) == 'u') || (*(tep + 1) == 't') || (*(tep + 1) == 'b') || (*(tep + 1) == 'f') ||
                (*(tep + 1) == 'n') || (*(tep + 1) == 'r')) {
                b = 1;
            }
        }
        *tmp = *tep;
        tmp++;
        if (*tep == '\0')
            break;
        tep++;
    }
}

static void CheckSign(char *inStr, int &x)
{
    char *tmp;
    tmp = inStr;
    if (*tmp == '\"') {
        x++;
    }
    while (*tmp != '\0') {
        tmp++;
    }
    tmp--;
    if (*tmp == '\"') {
        x++;
    }
}

static void CheckSingleSign(char *inStr, int &x)
{
    char *tmp;
    tmp = inStr;
    if (*tmp == '\'') {
        x++;
    }
    while (*tmp != '\0') {
        tmp++;
    }
    tmp--;
    if (*tmp == '\'') {
        x++;
    }
}

Datum json_unquote(PG_FUNCTION_ARGS)
{
    Oid valtype;
    Oid typOutput;
    bool typIsVarlena = false;
    Datum arg = 0;
    char *str = NULL;
    text *result = NULL;
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        str = OidOutputFunctionCall(typOutput, arg);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function json_unquote")));
    }
    if (!is_valid_unicode_escape(str)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Incorrent hex digit after \\u escape in string")));
    }
    char *str1 = NULL;
    str1 = (char *)palloc(strlen(str) + 10);
    int a = 0;
    int b = 0;
    int x = 0;
    int singleSign = 0;
    CheckSingleSign(str, singleSign);
    if (singleSign == 2) {
        result = cstring_to_text(str);
    } else {
        CheckSign(str, x);
        if (x < 0 || x > 2) {
            pfree(str1);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid data type for JSON data in argument 1 to function json_unquote")));
        }
        if (x == 2) {
            DelChar(str, str1, a, b);
            if ((a == 2 && b == 1) || a >= 3) {
                pfree(str1);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("Invalid data type for JSON data in argument 1 to function json_unquote")));
            } else {
                char *str2 = scanstr(str1);
                char *str3 = unescape_unicode(str2);
                char *str4 = scanstr(str3);
                result = cstring_to_text(str4);
            }
        } else if (x == 0 || x == 1) {
            char *str2 = unescape_unicode(str);
            char *str3 = scanstr(str2);
            result = cstring_to_text(str3);
        } else {
            pfree(str1);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid data type for JSON data in argument 1 to function json_unquote")));
        }
    }
    pfree(str1);
    PG_RETURN_TEXT_P(result);
}

static void depth_object_field(JsonLexContext *lex, JsonSemAction *sem, int &depth)
{
    /*
     * an object field is "fieldname" : value where value can be a scalar,
     * object or array
     */
    char *fname = NULL; /* keep compiler quiet */
    json_ofield_action ostart = sem->object_field_start;
    json_ofield_action oend = sem->object_field_end;
    bool isnull = false;
    char **fnameaddr = NULL;
    JsonTokenType tok;

    if (ostart != NULL || oend != NULL) {
        fnameaddr = &fname;
    }
    if (!lex_accept(lex, JSON_TOKEN_STRING, fnameaddr)) {
        report_parse_error(JSON_PARSE_STRING, lex);
    }

    lex_expect(JSON_PARSE_OBJECT_LABEL, lex, JSON_TOKEN_COLON);
    tok = lex_peek(lex);
    isnull = tok == JSON_TOKEN_NULL;

    if (ostart != NULL) {
        (*ostart)(sem->semstate, fname, isnull);
    }
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            depth_object(lex, sem, depth);
            break;
        case JSON_TOKEN_ARRAY_START:
            depth_array(lex, sem, depth);
            break;
        default:
            parse_scalar(lex, sem);
            if (depth >= lex->lex_level + 1) {

            } else {
                depth++;
            }
    }

    if (oend != NULL) {
        (*oend)(sem->semstate, fname, isnull);
    }
    if (fname != NULL) {
        pfree(fname);
    }
}

static void depth_array_element(JsonLexContext *lex, JsonSemAction *sem, int &depth)
{
    json_aelem_action astart = sem->array_element_start;
    json_aelem_action aend = sem->array_element_end;
    JsonTokenType tok = lex_peek(lex);
    bool isnull;
    isnull = tok == JSON_TOKEN_NULL;

    if (astart != NULL) {
        (*astart)(sem->semstate, isnull);
    }

    /* an array element is any object, array or scalar */
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            depth_object(lex, sem, depth);
            break;
        case JSON_TOKEN_ARRAY_START:
            depth_array(lex, sem, depth);
            break;
        default:
            parse_scalar(lex, sem);
            if (depth >= lex->lex_level + 1) {

            } else {
                depth++;
            };
    }

    if (aend != NULL) {
        (*aend)(sem->semstate, isnull);
    }
}

static void depth_object(JsonLexContext *lex, JsonSemAction *sem, int &depth)
{
    /*
     * an object is a possibly empty sequence of object fields, separated by
     * commas and surrounde by curly braces.
     */
    json_struct_action ostart = sem->object_start;
    json_struct_action oend = sem->object_end;
    JsonTokenType tok;

    if (ostart != NULL) {
        (*ostart)(sem->semstate);
    }

    /*
     * Data inside an object is at a higher nesting level than the object
     * itself. Note that we increment this after we call the semantic routine
     * for the object start and restore it before we call the routine for the
     * object end.
     */
    lex->lex_level++;
    if (lex->lex_level > depth)
        depth = lex->lex_level;
    /* we know this will succeeed, just clearing the token */
    lex_expect(JSON_PARSE_OBJECT_START, lex, JSON_TOKEN_OBJECT_START);

    tok = lex_peek(lex);
    switch (tok) {
        case JSON_TOKEN_STRING:
            depth_object_field(lex, sem, depth);
            while (lex_accept(lex, JSON_TOKEN_COMMA, NULL))
                depth_object_field(lex, sem, depth);
            break;
        case JSON_TOKEN_OBJECT_END:
            break;
        default:
            /* case of an invalid initial token inside the object */
            report_parse_error(JSON_PARSE_OBJECT_START, lex);
    }
    lex_expect(JSON_PARSE_OBJECT_NEXT, lex, JSON_TOKEN_OBJECT_END);
    lex->lex_level--;

    if (oend != NULL) {
        (*oend)(sem->semstate);
    }
}

static void depth_array(JsonLexContext *lex, JsonSemAction *sem, int &depth)
{
    /*
     * an array is a possibly empty sequence of array elements, separated by
     * commas and surrounded by square brackets.
     */
    json_struct_action astart = sem->array_start;
    json_struct_action aend = sem->array_end;

    if (astart != NULL) {
        (*astart)(sem->semstate);
    }

    /*
     * Data inside an array is at a higher nesting level than the array
     * itself. Note that we increment this after we call the semantic routine
     * for the array start and restore it before we call the routine for the
     * array end.
     */
    lex->lex_level++;
    if (lex->lex_level > depth)
        depth = lex->lex_level;
    lex_expect(JSON_PARSE_ARRAY_START, lex, JSON_TOKEN_ARRAY_START);
    if (lex_peek(lex) != JSON_TOKEN_ARRAY_END) {
        depth_array_element(lex, sem, depth);
        while (lex_accept(lex, JSON_TOKEN_COMMA, NULL))
            depth_array_element(lex, sem, depth);
    }
    lex_expect(JSON_PARSE_ARRAY_NEXT, lex, JSON_TOKEN_ARRAY_END);
    lex->lex_level--;

    if (aend != NULL) {
        (*aend)(sem->semstate);
    }
}

static void sort_json(JsonLexContext *lex, JsonSemAction *sem, int &depth)
{
    JsonTokenType tok;

    /* get the initial token */
    json_lex(lex);
    tok = lex_peek(lex);

    /* parse by recursive descent */
    switch (tok) {
        case JSON_TOKEN_OBJECT_START:
            depth_object(lex, sem, depth);
            break;
        case JSON_TOKEN_ARRAY_START:
            depth_array(lex, sem, depth);
            break;
        default:
            parse_scalar(lex, sem); /* json can be a bare scalar */
            depth++;
    }

    lex_expect(JSON_PARSE_END, lex, JSON_TOKEN_END);
}

Datum json_depth(PG_FUNCTION_ARGS)
{
    Oid valtype;
    Oid typOutput;
    bool typIsVarlena = false;
    Datum arg = 0;
    char *data = NULL;
    text *json = NULL;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
        json = cstring_to_text(data);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function json_depth")));
    }
    JsonLexContext *lex = NULL;
    int depth = 0;
    /* validate it */
    lex = makeJsonLexContext(json, false);
    sort_json(lex, &nullSemAction, depth);
    PG_RETURN_INT32(depth);
}

Datum json_valid(PG_FUNCTION_ARGS)
{
    Oid valtype;
    Oid typOutput;
    bool typIsVarlena = false;
    Datum arg = 0;
    char *data = NULL;
    text *result = NULL;
    JsonLexContext *lex = NULL;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (!VALTYPE_IS_JSON(valtype))
        PG_RETURN_BOOL(false);

    arg = PG_GETARG_DATUM(0);
    getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
    data = OidOutputFunctionCall(typOutput, arg);
    result = cstring_to_text(data);
    lex = makeJsonLexContext(result, false);

    MemoryContext old_context = CurrentMemoryContext;
    PG_TRY();
    {
        pg_parse_json(lex, &nullSemAction);
    }
    PG_CATCH();
    {
        ErrorData *edata = NULL;
        MemoryContext cxt = MemoryContextSwitchTo(old_context);
        edata = CopyErrorData();
        if (edata->sqlerrcode == ERRCODE_INVALID_TEXT_REPRESENTATION) {
            FlushErrorState();
            /* the old edata is no longer used */
            FreeErrorData(edata);

            MemoryContextSwitchTo(cxt);
            PG_RETURN_BOOL(false);
        } else {
            MemoryContextSwitchTo(cxt);
            ReThrowError(edata);
        }
    }
    PG_END_TRY();

    PG_RETURN_BOOL(true);
}

Datum json_type(PG_FUNCTION_ARGS)
{
    Oid valtype;
    Oid typOutput;
    bool typIsVarlena = false;
    Datum arg = 0;
    char *data = NULL;
    text *json = NULL;
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
        json = cstring_to_text(data);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function json_type")));
    }

    JsonLexContext *lex = NULL;
    JsonTokenType tok;
    const char *type = NULL;

    if (!json_valid(fcinfo)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function json_type")));
    }
    lex = makeJsonLexContext(json, false);

    /* Lex exactly one token from the input and check its type. */
    json_lex(lex, true);
    tok = lex_peek(lex);
    type = type_case(tok, true);
    pfree(lex);
    PG_RETURN_TEXT_P(cstring_to_text(type));
}
#endif
