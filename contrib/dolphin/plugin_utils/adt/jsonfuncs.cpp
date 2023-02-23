/* -------------------------------------------------------------------------
 *
 * jsonfuncs.ca
 *      Functions to process JSON data types.
 *
 * Portions Copyright (c) 2021 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *    src/backend/utils/adt/jsonfuncs.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include <limits.h>

#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "access/htup.h"
#include "catalog/pg_type.h"
#include "lib/stringinfo.h"
#include "plugin_mb/pg_wchar.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/hsearch.h"
#include "utils/json.h"
#include "utils/jsonb.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/typcache.h"

#ifdef DOLPHIN
#include "cjson/cJSON.h"
#include "plugin_postgres.h"
#include "plugin_parser/scansup.h"
#include "plugin_utils/json.h"
#include "plugin_utils/jsonapi.h"
#include "utils/pg_locale.h"

/* for String fuzzy matching and escape conversion */
#define NextByte(p, plen) ((p)++, (plen)--)
#define CHAREQ(p1, p2) (*(p1) == *(p2))
#define NextChar(p, plen) NextByte((p), (plen))
#define CopyAdvChar(dst, src, srclen) (*(dst)++ = *(src)++, (srclen)--)
#define do_like_escape MB_do_like_escape
#define MatchText MB_MatchText
#include "like_match.cpp"
#include "plugin_parser/parse_coerce.h"
#include "catalog/pg_cast.h"
/* fake type category for JSON so we can distinguish it in datum_to_json */
#define TYPCATEGORY_JSON 'j'
/* fake category for types that have a cast to json */
#define TYPCATEGORY_JSON_CAST 'c'

TYPCATEGORY get_value_type(Oid val_type, Oid typoutput)
{
    TYPCATEGORY tcategory;
    Oid castfunc = InvalidOid;

    if (val_type == InvalidOid) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("could not determine input data type")));
    }
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

    return tcategory;
}

typedef enum {
    cJSON_JsonPath_Start,
    cJSON_JsonPath_Key,
    cJSON_JsonPath_Index,
    cJSON_JsonPath_AnyKey,
    cJSON_JsonPath_AnyIndex,
    cJSON_JsonPath_Any
} cJSON_JsonPathType;

typedef struct cJSON_JsonPath {
    struct cJSON_JsonPath *next;
    cJSON_JsonPathType type;
    char *key;
    int index;
} cJSON_JsonPath;

typedef struct cJSON_ResultNode {
    struct cJSON_ResultNode *next;
    struct cJSON *node;
} cJSON_ResultNode;

typedef struct cJSON_ResultWrapper {
    struct cJSON_ResultNode *head;
    int len;
} cJSON_ResultWrapper;

typedef char *ElemType;
typedef struct LinkNode {
    ElemType data;
    struct LinkNode *next;
} * search_LinkStack;

static cJSON *input_to_cjson(Oid valtype, const char *funcName, int pos, Datum arg);

/* funcstions for creating wrapper to restore cJsonPath*/
static cJSON_ResultWrapper *cJSON_CreateResultWrapper();
static void cJSON_ResultWrapperInit(cJSON_ResultWrapper *res);

/* funcstions for cJsonPath parse*/
static cJSON_JsonPath *cJSON_CreateJsonPath(cJSON_JsonPathType type);
static bool cJSON_AddItemToJsonPath(cJSON_JsonPath *jp, cJSON_JsonPath *item);
static inline int get_space_skipped_index(const char *data, int start);
static inline bool get_end_of_key(const char *data, int start, int &end);
static cJSON_JsonPath *jp_parse(const char *data, int &error_pos);
static cJSON_JsonPath *jp_parse_key(const char *data, int *idx);
static cJSON_JsonPath *jp_parse_index(const char *data, int *idx);
static cJSON_JsonPath *jp_parse_any(const char *data, int *idx);

/* functions for cJsonPath match */
static bool jp_match(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static cJSON *jp_match_object(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static cJSON *jp_match_array(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static void jp_match_any(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res, int mode);
static bool cJSON_JsonPathMatch(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static bool cJSON_AddItemToResultWrapper(cJSON_ResultWrapper *res, cJSON *item);
static bool cJSON_JsonPathCanMatchMany(cJSON_JsonPath *jp);

/* functions for cJsonPath delete */
static void cJSON_DeleteJsonPath(cJSON_JsonPath *jp);
static void cJSON_DeleteResultWrapper(cJSON_ResultWrapper *res);

/* functions for the cJson operation*/
static void quicksort(cJSON *item1, cJSON *item2);
static text *formatJsondoc(char *str);
static inline cJSON_JsonPath *jp_pop(cJSON_JsonPath *jp);
static void cJSON_SortObject(cJSON *object);
static cJSON *cJSON_ResultWrapperToArray(cJSON_ResultWrapper *res);
static void cJSON_SwapItemValue(cJSON *item1, cJSON *item2);
static cJSON_bool cJSON_ArrayAppend(cJSON *root, cJSON_JsonPath *jp, cJSON *value);
static cJSON_bool cJSON_JsonInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value);
static bool cJSON_JsonReplace(cJSON *root, cJSON_JsonPath *jp, cJSON *value, bool &invalidPath);
static bool cJSON_JsonRemove(cJSON *root, cJSON_JsonPath *jp, bool *invalidPath);
static bool cJSON_JsonArrayInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value, bool *invlidPath, bool *isArray);

/* functions for the json_contains */
static bool json_contains_unit(const cJSON *const target, const cJSON *const candidate);
static int containsAsterisk(const char *const path);

/* functions for the json_search */
static void search_PushStack(search_LinkStack &s, ElemType x);
static void search_PopStack(search_LinkStack &s);
static search_LinkStack search_ReverseStack(search_LinkStack &s);
static void search_CleanStack(search_LinkStack &s);
static cJSON *jp_match_object_record(cJSON *doc_cJSON, char *path, int wildchar_flag, bool &type_flag,
                                     StringInfo &position, StringInfo &wildchar, int &i);
static cJSON *jp_match_array_record(cJSON *doc_cJSON, char *path, int wildchar_flag, bool &type_flag,
                                    StringInfo &position, StringInfo &wildchar, int &i);
static cJSON *jp_match_object_quote_record(cJSON *doc_cJSON, char *path, int wildchar_flag, StringInfo &position,
                                           StringInfo &wildchar, int &i);
static cJSON *jp_match_record(cJSON *doc_cJSON, char *path, StringInfo &position, StringInfo &wildchar);
static bool json_search_unit(const cJSON *doc_cJSON, const text *search_text, bool mode_match, char *wildchar,
                             char *last_position, search_LinkStack &stk);
static text *remove_duplicate_path(search_LinkStack &stk);

/* functions for json_merge */
static int put_object_keys_into_set(char **keys, cJSON *json1, cJSON *json2);
static int put_object_keys_into_set(char **keys, cJSON *json);
static void appendObject(StringInfo result, cJSON *json, int cnt, char **keys, int *pos);
static void appendStringInfoObject(StringInfo result, cJSON *json);
static void appendStringInfoArray(StringInfo result, cJSON *json);
static void appendStringInfoArrayEle(StringInfo result, cJSON *json);
static void json_regular_format(StringInfo result, cJSON *json);
static cJSON *json_merge_patch_unit(cJSON *j1, cJSON *j2);
static cJSON *json_merge_preserve_unit(cJSON *j1, cJSON *j2);

/* semantic action functions for json_length */
static void length_object_field_start(void *state, char *fname, bool isnull);
static void length_array_element_start(void *state, bool isnull);
static void length_scalar(void *state, char *token, JsonTokenType tokentype);

/* functions for json_pretty */
static void newline_and_indent(StringInfo buf, int depth);
static text *prettyJsondoc(char *str);

/* functions for json_object_field_text*/
static void delchar_oper(char *inStr, char *outStr, int &a, int &b);
static void checksign_oper(char *inStr, int &x);
#endif

/* semantic action functions for json_object_keys */
static void okeys_object_field_start(void *state, char *fname, bool isnull);
static void okeys_array_start(void *state);
static void okeys_scalar(void *state, char *token, JsonTokenType tokentype);

/* semantic action functions for json_get* functions */
static void get_object_start(void *state);
static void get_object_field_start(void *state, char *fname, bool isnull);
static void get_object_field_end(void *state, char *fname, bool isnull);
static void get_array_start(void *state);
static void get_array_element_start(void *state, bool isnull);
static void get_array_element_end(void *state, bool isnull);
static void get_scalar(void *state, char *token, JsonTokenType tokentype);

/* common worker function for json getter functions */
static inline Datum get_path_all(FunctionCallInfo fcinfo, bool as_text);
static inline text *get_worker(text *json, char *field, int elem_index, char **tpath, int *ipath, int npath,
                               bool normalize_results);
static inline Datum get_jsonb_path_all(FunctionCallInfo fcinfo, bool as_text);

/* semantic action functions for json_array_length */
static void alen_object_start(void *state);
static void alen_scalar(void *state, char *token, JsonTokenType tokentype);
static void alen_array_element_start(void *state, bool isnull);

/* common workers for json{b}_each* functions */
static inline Datum each_worker(FunctionCallInfo fcinfo, bool as_text);
static inline Datum each_worker_jsonb(FunctionCallInfo fcinfo, bool as_text);

/* semantic action functions for json_each */
static void each_object_field_start(void *state, char *fname, bool isnull);
static void each_object_field_end(void *state, char *fname, bool isnull);
static void each_array_start(void *state);
static void each_scalar(void *state, char *token, JsonTokenType tokentype);

/* common workers for json{b}_array_elements_* functions */
static inline Datum elements_worker(FunctionCallInfo fcinfo, bool as_text);
static inline Datum elements_worker_jsonb(FunctionCallInfo fcinfo, bool as_text);

/* semantic action functions for json_array_elements */
static void elements_object_start(void *state);
static void elements_array_element_start(void *state, bool isnull);
static void elements_array_element_end(void *state, bool isnull);
static void elements_scalar(void *state, char *token, JsonTokenType tokentype);

/* turn a json object into a hash table */
static HTAB *get_json_object_as_hash(text *json, char *funcname, bool use_json_as_text);

/* common worker for populate_record and to_record */
static inline Datum populate_record_worker(FunctionCallInfo fcinfo, bool have_record_arg);

/* semantic action functions for get_json_object_as_hash */
static void hash_object_field_start(void *state, char *fname, bool isnull);
static void hash_object_field_end(void *state, char *fname, bool isnull);
static void hash_array_start(void *state);
static void hash_scalar(void *state, char *token, JsonTokenType tokentype);

/* semantic action functions for populate_recordset */
static void populate_recordset_object_field_start(void *state, char *fname, bool isnull);
static void populate_recordset_object_field_end(void *state, char *fname, bool isnull);
static void populate_recordset_scalar(void *state, char *token, JsonTokenType tokentype);
static void populate_recordset_object_start(void *state);
static void populate_recordset_object_end(void *state);
static void populate_recordset_array_start(void *state);
static void populate_recordset_array_element_start(void *state, bool isnull);

/* worker function for populate_recordset and to_recordset */
static inline Datum populate_recordset_worker(FunctionCallInfo fcinfo, bool have_record_arg);
/* Worker that takes care of common setup for us */
static JsonbValue *findJsonbValueFromSuperHeaderLen(JsonbSuperHeader sheader, uint32 flags, char *key, uint32 keylen);

/* search type classification for json_get* functions */
typedef enum { JSON_SEARCH_OBJECT = 1, JSON_SEARCH_ARRAY, JSON_SEARCH_PATH } JsonSearch;

/* state for json_object_keys */
typedef struct OkeysState {
    JsonLexContext *lex;
    char **result;
    int result_size;
    int result_count;
    int sent_count;
} OkeysState;

/* state for json_get* functions */
typedef struct GetState {
    JsonLexContext *lex;
    JsonSearch search_type;
    int search_index;
    int array_index;
    char *search_term;
    char *result_start;
    text *tresult;
    bool result_is_null;
    bool normalize_results;
    bool next_scalar;
    char **path;
    int npath;
    char **current_path;
    bool *pathok;
    int *array_level_index;
    int *path_level_index;
} GetState;

/* state for json_array_length */
typedef struct AlenState {
    JsonLexContext *lex;
    int count;
} AlenState;

/* state for json_each */
typedef struct EachState {
    JsonLexContext *lex;
    Tuplestorestate *tuple_store;
    TupleDesc ret_tdesc;
    MemoryContext tmp_cxt;
    char *result_start;
    bool normalize_results;
    bool next_scalar;
    char *normalized_scalar;
} EachState;

/* state for json_array_elements */
typedef struct ElementsState {
    JsonLexContext *lex;
    Tuplestorestate *tuple_store;
    TupleDesc ret_tdesc;
    MemoryContext tmp_cxt;
    char *result_start;
    bool normalize_results;
    bool next_scalar;
    char *normalized_scalar;
} ElementsState;

/* state for get_json_object_as_hash */
typedef struct JhashState {
    JsonLexContext *lex;
    HTAB *hash;
    char *saved_scalar;
    char *save_json_start;
    bool use_json_as_text;
    char *function_name;
} JHashState;

/* used to build the hashtable */
typedef struct JsonHashEntry {
    char fname[NAMEDATALEN];
    char *val;
    char *json;
    bool isnull;
} JsonHashEntry;

/* these two are stolen from hstore / record_out, used in populate_record* */
typedef struct ColumnIOData {
    Oid column_type;
    Oid typiofunc;
    Oid typioparam;
    FmgrInfo proc;
} ColumnIOData;

typedef struct RecordIOData {
    Oid record_type;
    int32 record_typmod;
    int ncolumns;
    ColumnIOData columns[1]; /* VARIABLE LENGTH ARRAY */
} RecordIOData;

/* state for populate_recordset */
typedef struct PopulateRecordsetState {
    JsonLexContext *lex;
    HTAB *json_hash;
    char *saved_scalar;
    char *save_json_start;
    bool use_json_as_text;
    Tuplestorestate *tuple_store;
    TupleDesc ret_tdesc;
    HeapTupleHeader rec;
    RecordIOData *my_extra;
    MemoryContext fn_mcxt; /* used to stash IO funcs */
} PopulateRecordsetState;

/* Turn a jsonb object into a record */
static void make_row_from_rec_and_jsonb(Jsonb *element, PopulateRecordsetState *state);

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(json_contains);
extern "C" DLL_PUBLIC Datum json_contains(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_contains_path);
extern "C" DLL_PUBLIC Datum json_contains_path(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_extract);
extern "C" DLL_PUBLIC Datum json_extract(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_array_append);
extern "C" DLL_PUBLIC Datum json_array_append(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_append);
extern "C" DLL_PUBLIC Datum json_append(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_search);
extern "C" DLL_PUBLIC Datum json_search(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_keys);
extern "C" DLL_PUBLIC Datum json_keys(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_merge_preserve);
extern "C" DLL_PUBLIC Datum json_merge_preserve(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_merge);
extern "C" DLL_PUBLIC Datum json_merge(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_merge_patch);
extern "C" DLL_PUBLIC Datum json_merge_patch(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_insert);
extern "C" DLL_PUBLIC Datum json_insert(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_replace);
extern "C" DLL_PUBLIC Datum json_replace(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_remove);
extern "C" DLL_PUBLIC Datum json_remove(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_array_insert);
extern "C" DLL_PUBLIC Datum json_array_insert(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_set);
extern "C" DLL_PUBLIC Datum json_set(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_length);
extern "C" DLL_PUBLIC Datum json_length(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_objectagg_finalfn);
extern "C" DLL_PUBLIC Datum json_objectagg_finalfn(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_storage_size);
extern "C" DLL_PUBLIC Datum json_storage_size(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(json_pretty);
extern "C" DLL_PUBLIC Datum json_pretty(PG_FUNCTION_ARGS);
#endif

/*
 * SQL function json_object_keys
 *
 * Returns the set of keys for the object argument.
 *
 * This SRF operates in value-per-call mode. It processes the
 * object during the first call, and the keys are simply stashed
 * in an array, whose size is expanded as necessary. This is probably
 * safe enough for a list of keys of a single object, since they are
 * limited in size to NAMEDATALEN and the number of keys is unlikely to
 * be so huge that it has major memory implications.
 */
Datum jsonb_object_keys(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx = NULL;
    OkeysState *state = NULL;
    int i;

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext oldcontext;
        Jsonb *jb = PG_GETARG_JSONB(0);
        bool skipNested = false;
        JsonbIterator *it = NULL;
        JsonbValue v;
        int r;

        if (JB_ROOT_IS_SCALAR(jb)) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call jsonb_object_keys on a scalar")));
        } else if (JB_ROOT_IS_ARRAY(jb)) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call jsonb_object_keys on an array")));
        }

        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        state = (OkeysState *)palloc(sizeof(OkeysState));

        state->result_size = JB_ROOT_COUNT(jb);
        state->result_count = 0;
        state->sent_count = 0;
        state->result = (char **)palloc(state->result_size * sizeof(char *));

        it = JsonbIteratorInit(VARDATA_ANY(jb));

        while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
            skipNested = true;

            if (r == WJB_KEY) {
                char *cstr = NULL;
                cstr = (char *)palloc(v.string.len + 1 * sizeof(char));
                errno_t rc = memcpy_s(cstr, v.string.len + 1 * sizeof(char), v.string.val, v.string.len);
                securec_check(rc, "\0", "\0");
                cstr[v.string.len] = '\0';
                state->result[state->result_count++] = cstr;
            }
        }

        MemoryContextSwitchTo(oldcontext);
        funcctx->user_fctx = (void *)state;
    }

    funcctx = SRF_PERCALL_SETUP();
    state = (OkeysState *)funcctx->user_fctx;
    if (state->sent_count < state->result_count) {
        char *nxt = state->result[state->sent_count++];
        SRF_RETURN_NEXT(funcctx, CStringGetTextDatum(nxt));
    }

    /* cleanup to reduce or eliminate memory leaks */
    for (i = 0; i < state->result_count; i++) {
        pfree(state->result[i]);
    }
    pfree(state->result);
    pfree(state);

    SRF_RETURN_DONE(funcctx);
}

Datum json_object_keys(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx = NULL;
    OkeysState *state = NULL;
    int i;

    if (SRF_IS_FIRSTCALL()) {
        text *json = PG_GETARG_TEXT_P(0);
        JsonLexContext *lex = makeJsonLexContext(json, true);
        JsonSemAction *sem = NULL;

        MemoryContext oldcontext;

        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        state = (OkeysState *)palloc(sizeof(OkeysState));
        sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

        state->lex = lex;
        state->result_size = 256;
        state->result_count = 0;
        state->sent_count = 0;
        state->result = (char **)palloc(256 * sizeof(char *));

        sem->semstate = (void *)state;
        sem->array_start = okeys_array_start;
        sem->scalar = okeys_scalar;
        sem->object_field_start = okeys_object_field_start;
        /* remainder are all NULL, courtesy of palloc0 above */
        pg_parse_json(lex, sem);
        /* keys are now in state->result */
        pfree(lex->strval->data);
        pfree(lex->strval);
        pfree(lex);
        pfree(sem);

        MemoryContextSwitchTo(oldcontext);
        funcctx->user_fctx = (void *)state;
    }

    funcctx = SRF_PERCALL_SETUP();
    state = (OkeysState *)funcctx->user_fctx;
    if (state->sent_count < state->result_count) {
        char *nxt = state->result[state->sent_count++];
        SRF_RETURN_NEXT(funcctx, CStringGetTextDatum(nxt));
    }

    /* cleanup to reduce or eliminate memory leaks */
    for (i = 0; i < state->result_count; i++) {
        pfree(state->result[i]);
    }
    pfree(state->result);
    pfree(state);

    SRF_RETURN_DONE(funcctx);
}

static void okeys_object_field_start(void *state, char *fname, bool isnull)
{
    OkeysState *_state = (OkeysState *)state;

    /* only collecting keys for the top level object */
    if (_state->lex->lex_level != 1) {
        return;
    }

    /* enlarge result array if necessary */
    if (_state->result_count >= _state->result_size) {
        _state->result_size *= 2;
        _state->result = (char **)repalloc(_state->result, sizeof(char *) * _state->result_size);
    }

    /* save a copy of the field name */
    _state->result[_state->result_count++] = pstrdup(fname);
}

static void okeys_array_start(void *state)
{
    OkeysState *_state = (OkeysState *)state;

    /* top level must be a json object */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call json_object_keys on an array")));
    }
}

static void okeys_scalar(void *state, char *token, JsonTokenType tokentype)
{
    OkeysState *_state = (OkeysState *)state;

    /* top level must be a json object */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call json_object_keys on a scalar")));
    }
}

/*
 * json and jsonb getter functions
 * these implement the -> ->> #> and #>> operators
 * and the json{b?}_extract_path*(json, text, ...) functions
 */
#ifdef DOLPHIN
Datum json_object_field(PG_FUNCTION_ARGS)
{
    text *json = PG_GETARG_TEXT_P(0);
    text *in_array = PG_GETARG_TEXT_P(1);
    text *result = NULL;
    char *path = NULL;
    char *data = text_to_cstring(json);
    cJSON_ResultWrapper *res = NULL;
    cJSON_JsonPath *jp = NULL;
    cJSON *root = NULL;
    int error_pos = -1;
    char *r = NULL;
    bool many = false;

    root = cJSON_ParseWithOpts(data, 0, 1);
    if (!root) {
        cJSON_DeleteResultWrapper(res);
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON text in argument 1 to function json_extract.")));
    }
    cJSON_SortObject(root);
    res = cJSON_CreateResultWrapper();
    path = TextDatumGetCString(in_array);
    jp = jp_parse(path, error_pos);
    if (!jp) {
        char *fnamestr = text_to_cstring(in_array);
        result = get_worker(json, fnamestr, -1, NULL, NULL, -1, false);
        if (result != NULL) {
            PG_RETURN_TEXT_P(result);
        } else {
            PG_RETURN_NULL();
        }
    } else {
        many |= cJSON_JsonPathCanMatchMany(jp);
        cJSON_JsonPathMatch(root, jp, res);
        cJSON_DeleteJsonPath(jp);
        if (res->len > 0) {
            if (many) {
                cJSON *arr = cJSON_ResultWrapperToArray(res);
                r = cJSON_PrintUnformatted(arr);
                result = formatJsondoc(r);
                cJSON_Delete(arr);
            } else {
                r = cJSON_PrintUnformatted(res->head->next->node);
                result = formatJsondoc(r);
            }
            cJSON_free(r);
        } else {
            PG_RETURN_NULL();
        }
        cJSON_Delete(root);
        cJSON_DeleteResultWrapper(res);
        PG_RETURN_TEXT_P(result);
    }
}
#endif

Datum jsonb_object_field(PG_FUNCTION_ARGS)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    char *key = text_to_cstring(PG_GETARG_TEXT_P(1));
    int klen = strlen(key);
    JsonbIterator *it = NULL;
    JsonbValue v;
    int r;
    bool skipNested = false;

    if (JB_ROOT_IS_SCALAR(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_object_field (jsonb -> text operator) on a scalar")));
    } else if (JB_ROOT_IS_ARRAY(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_object_field (jsonb -> text operator) on an array")));
    }

    Assert(JB_ROOT_IS_OBJECT(jb));
    it = JsonbIteratorInit(VARDATA_ANY(jb));
    while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
        skipNested = true;

        if (r == WJB_KEY) {
            if (klen == v.string.len && strncmp(key, v.string.val, klen) == 0) {
                /*
                 * The next thing the iterator fetches should be the value, no
                 * matter what shape it is.
                 */
                (void)JsonbIteratorNext(&it, &v, skipNested);
                PG_RETURN_JSONB(JsonbValueToJsonb(&v));
            }
        }
    }
    PG_RETURN_NULL();
}

#ifdef DOLPHIN
static void delchar_oper(char *inStr, char *outStr, int &a, int &b)
{
    char *tmp;
    char *tep;
    tmp = outStr;
    tep = inStr;
    while (*tep != '\0') {
        if (*tep == '\"') {
            tep++;
            a++;
        }
        if (*tep == '\\') {
            b++;
        }
        *tmp = *tep;
        tmp++;
        if (*tep == '\0')
            break;
        tep++;
    }
}

static void checksign_oper(char *inStr, int &x)
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

Datum json_object_field_text(PG_FUNCTION_ARGS)
{
    text *json = PG_GETARG_TEXT_P(0);
    text *in_array = PG_GETARG_TEXT_P(1);
    text *result = NULL;
    char *path = NULL;
    char *data = text_to_cstring(json);
    cJSON_ResultWrapper *res = NULL;
    cJSON_JsonPath *jp = NULL;
    cJSON *root = NULL;
    int error_pos = -1;
    char *r = NULL;
    bool many = false;

    root = cJSON_ParseWithOpts(data, 0, 1);
    if (!root) {
        cJSON_DeleteResultWrapper(res);
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON text in argument 1 to function json_extract.")));
    }
    cJSON_SortObject(root);
    res = cJSON_CreateResultWrapper();
    path = TextDatumGetCString(in_array);
    jp = jp_parse(path, error_pos);
    if (!jp) {
        char *fnamestr = text_to_cstring(in_array);
        result = get_worker(json, fnamestr, -1, NULL, NULL, -1, true);
        if (result != NULL) {
            PG_RETURN_TEXT_P(result);
        } else {
            PG_RETURN_NULL();
        }
    } else {
        many |= cJSON_JsonPathCanMatchMany(jp);
        cJSON_JsonPathMatch(root, jp, res);
        cJSON_DeleteJsonPath(jp);
        if (res->len > 0) {
            if (many) {
                cJSON *arr = cJSON_ResultWrapperToArray(res);
                r = cJSON_PrintUnformatted(arr);
                result = formatJsondoc(r);
                cJSON_Delete(arr);
            } else {
                r = cJSON_PrintUnformatted(res->head->next->node);
                result = formatJsondoc(r);
            }
            cJSON_free(r);
        } else {
            PG_RETURN_NULL();
        }
        cJSON_Delete(root);
        cJSON_DeleteResultWrapper(res);
        text *json_val = result;
        char *str = text_to_cstring(json_val);
        char *str1 = NULL;
        str1 = (char *)palloc(strlen(str) + 10);
        int a = 0;
        int b = 0;
        int x = 0;
        checksign_oper(str, x);
        if (x == 2) {
            delchar_oper(str, str1, a, b);
            if ((a == 2 && b == 1) || a >= 3) {
                pfree(str1);
                PG_RETURN_TEXT_P(NULL);
            } else {
                char *str2 = scanstr(str1);
                char *str3 = scanstr(str2);
                pfree(str1);
                text *result = cstring_to_text(str3);
                PG_RETURN_TEXT_P(result);
            }
        } else if (x == 0 || x == 1) {
            char *str2 = scanstr(str);
            text *result = cstring_to_text(str2);
            pfree(str1);
            PG_RETURN_TEXT_P(result);
        } else {
            pfree(str1);
            PG_RETURN_TEXT_P(NULL);
        }
    }
}
#endif

Datum jsonb_object_field_text(PG_FUNCTION_ARGS)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    char *key = text_to_cstring(PG_GETARG_TEXT_P(1));
    int klen = strlen(key);
    JsonbIterator *it = NULL;
    JsonbValue v;
    int r;
    bool skipNested = false;

    if (JB_ROOT_IS_SCALAR(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_object_field_text (jsonb ->> text operator) on a scalar")));
    } else if (JB_ROOT_IS_ARRAY(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_object_field_text (jsonb ->> text operator) on an array")));
    }

    Assert(JB_ROOT_IS_OBJECT(jb));
    it = JsonbIteratorInit(VARDATA_ANY(jb));
    while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
        skipNested = true;

        if (r == WJB_KEY) {
            if (klen == v.string.len && strncmp(key, v.string.val, klen) == 0) {
                text *result = NULL;

                /*
                 * The next thing the iterator fetches should be the value, no
                 * matter what shape it is.
                 */
                r = JsonbIteratorNext(&it, &v, skipNested);

                /*
                 * if it's a scalar string it needs to be de-escaped,
                 * otherwise just return the text
                 */
                if (v.type == jbvString) {
                    result = cstring_to_text_with_len(v.string.val, v.string.len);
                } else if (v.type == jbvNull) {
                    PG_RETURN_NULL();
                } else {
                    StringInfo jtext = makeStringInfo();
                    Jsonb *tjb = JsonbValueToJsonb(&v);
                    (void)JsonbToCString(jtext, VARDATA(tjb), -1);
                    result = cstring_to_text_with_len(jtext->data, jtext->len);
                }
                PG_RETURN_TEXT_P(result);
            }
        }
    }

    PG_RETURN_NULL();
}

Datum json_array_element(PG_FUNCTION_ARGS)
{
    text *json = PG_GETARG_TEXT_P(0);
    text *result = NULL;
    int element = PG_GETARG_INT32(1);

    result = get_worker(json, NULL, element, NULL, NULL, -1, false);

    if (result != NULL) {
        PG_RETURN_TEXT_P(result);
    } else {
        PG_RETURN_NULL();
    }
}

Datum jsonb_array_element(PG_FUNCTION_ARGS)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    int element = PG_GETARG_INT32(1);
    JsonbIterator *it = NULL;
    JsonbValue v;
    int r;
    bool skipNested = false;
    int element_number = 0;

    if (JB_ROOT_IS_SCALAR(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_array_element (jsonb -> int operator) on a scalar")));
    } else if (JB_ROOT_IS_OBJECT(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_array_element (jsonb -> int operator) on an object")));
    }

    Assert(JB_ROOT_IS_ARRAY(jb));

    it = JsonbIteratorInit(VARDATA_ANY(jb));

    while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
        skipNested = true;

        if (r == WJB_ELEM) {
            if (element_number++ == element) {
                PG_RETURN_JSONB(JsonbValueToJsonb(&v));
            }
        }
    }

    PG_RETURN_NULL();
}

Datum json_array_element_text(PG_FUNCTION_ARGS)
{
    text *json = PG_GETARG_TEXT_P(0);
    text *result = NULL;
    int element = PG_GETARG_INT32(1);

    result = get_worker(json, NULL, element, NULL, NULL, -1, true);

    if (result != NULL) {
        PG_RETURN_TEXT_P(result);
    } else {
        PG_RETURN_NULL();
    }
}

Datum jsonb_array_element_text(PG_FUNCTION_ARGS)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    int element = PG_GETARG_INT32(1);
    JsonbIterator *it = NULL;
    JsonbValue v;
    int r;
    bool skipNested = false;
    int element_number = 0;

    if (JB_ROOT_IS_SCALAR(jb)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call jsonb_array_element_text on a scalar")));
    } else if (JB_ROOT_IS_OBJECT(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_array_element_text on an object")));
    }

    Assert(JB_ROOT_IS_ARRAY(jb));
    it = JsonbIteratorInit(VARDATA_ANY(jb));
    while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
        skipNested = true;
        if (r == WJB_ELEM) {
            if (element_number++ == element) {
                /*
                 * if it's a scalar string it needs to be de-escaped,
                 * otherwise just return the text
                 */
                text *result = NULL;
                if (v.type == jbvString) {
                    result = cstring_to_text_with_len(v.string.val, v.string.len);
                } else if (v.type == jbvNull) {
                    PG_RETURN_NULL();
                } else {
                    StringInfo jtext = makeStringInfo();
                    Jsonb *tjb = JsonbValueToJsonb(&v);
                    (void)JsonbToCString(jtext, VARDATA(tjb), -1);
                    result = cstring_to_text_with_len(jtext->data, jtext->len);
                }
                PG_RETURN_TEXT_P(result);
            }
        }
    }

    PG_RETURN_NULL();
}

Datum json_extract_path(PG_FUNCTION_ARGS)
{
    return get_path_all(fcinfo, false);
}

Datum json_extract_path_text(PG_FUNCTION_ARGS)
{
    return get_path_all(fcinfo, true);
}

/*
 * common routine for extract_path functions
 */
static inline Datum get_path_all(FunctionCallInfo fcinfo, bool as_text)
{
    text *json = NULL;
    ArrayType *path = PG_GETARG_ARRAYTYPE_P(1);
    text *result = NULL;
    Datum *pathtext = NULL;
    bool *pathnulls = NULL;
    int npath;
    char **tpath = NULL;
    int *ipath = NULL;
    int i;
    long ind;
    char *endptr = NULL;

    json = PG_GETARG_TEXT_P(0);

    if (array_contains_nulls(path)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call function with null path elements")));
    }

    deconstruct_array(path, TEXTOID, -1, false, 'i', &pathtext, &pathnulls, &npath);
    /*
     * If the array is empty, return NULL; this is dubious but it's what 9.3
     * did.
     */
    if (npath <= 0)
        PG_RETURN_NULL();

    tpath = (char **)palloc(npath * sizeof(char *));
    ipath = (int *)palloc(npath * sizeof(int));

    for (i = 0; i < npath; i++) {
        tpath[i] = TextDatumGetCString(pathtext[i]);
        if (*tpath[i] == '\0') {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("cannot call function with empty path elements")));
        }

        /*
         * we have no idea at this stage what structure the document is so
         * just convert anything in the path that we can to an integer and set
         * all the other integers to -1 which will never match.
         */
        ind = strtol(tpath[i], &endptr, 10);
        if (*endptr == '\0' && ind <= INT_MAX && ind >= 0) {
            ipath[i] = (int)ind;
        } else {
            ipath[i] = -1;
        }
    }

    result = get_worker(json, NULL, -1, tpath, ipath, npath, as_text);

    if (result != NULL) {
        PG_RETURN_TEXT_P(result);
    } else {
        /* null is NULL, regardless */
        PG_RETURN_NULL();
    }
}

/*
 * get_worker
 *
 * common worker for all the json getter functions
 */
static inline text *get_worker(text *json, char *field, int elem_index, char **tpath, int *ipath, int npath,
                               bool normalize_results)
{
    GetState *state = NULL;
    JsonLexContext *lex = makeJsonLexContext(json, true);
    JsonSemAction *sem = NULL;

    /* only allowed to use one of these */
    Assert(elem_index < 0 || (tpath == NULL && ipath == NULL && field == NULL));
    Assert(tpath == NULL || field == NULL);

    state = (GetState *)palloc0(sizeof(GetState));
    sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

    state->lex = lex;
    /* is it "_as_text" variant? */
    state->normalize_results = normalize_results;
    if (field != NULL) {
        /* single text argument */
        state->search_type = JSON_SEARCH_OBJECT;
        state->search_term = field;
    } else if (tpath != NULL) {
        /* path array argument */
        state->search_type = JSON_SEARCH_PATH;
        state->path = tpath;
        state->npath = npath;
        state->current_path = (char **)palloc(sizeof(char *) * npath);
        state->pathok = (bool *)palloc0(sizeof(bool) * npath);
        state->pathok[0] = true;
        state->array_level_index = (int *)palloc(sizeof(int) * npath);
        state->path_level_index = ipath;
    } else {
        /* single integer argument */
        state->search_type = JSON_SEARCH_ARRAY;
        state->search_index = elem_index;
        state->array_index = -1;
    }
    sem->semstate = (void *)state;

    /*
     * Not all  variants need all the semantic routines. only set the ones
     * that are actually needed for maximum efficiency.
     */
    sem->object_start = get_object_start;
    sem->array_start = get_array_start;
    sem->scalar = get_scalar;
    if (field != NULL || tpath != NULL) {
        sem->object_field_start = get_object_field_start;
        sem->object_field_end = get_object_field_end;
    }
    if (field == NULL) {
        sem->array_element_start = get_array_element_start;
        sem->array_element_end = get_array_element_end;
    }
    pg_parse_json(lex, sem);
    return state->tresult;
}

static void get_object_start(void *state)
{
    GetState *_state = (GetState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0 && _state->search_type == JSON_SEARCH_ARRAY) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot extract array element from a non-array")));
    }
}

static void get_object_field_start(void *state, char *fname, bool isnull)
{
    GetState *_state = (GetState *)state;
    bool get_next = false;
    int lex_level = _state->lex->lex_level;

    if (lex_level == 1 && _state->search_type == JSON_SEARCH_OBJECT && strcmp(fname, _state->search_term) == 0) {
        _state->tresult = NULL;
        _state->result_start = NULL;
        get_next = true;
    } else if (_state->search_type == JSON_SEARCH_PATH && lex_level <= _state->npath &&
               _state->pathok[_state->lex->lex_level - 1] && strcmp(fname, _state->path[lex_level - 1]) == 0) {
        /* path search, path so far is ok,  and we have a match */
        /* this object overrides any previous matching object */
        _state->tresult = NULL;
        _state->result_start = NULL;

        /* if not at end of path just mark path ok */
        if (lex_level < _state->npath) {
            _state->pathok[lex_level] = true;
        }

        /* end of path, so we want this value */
        if (lex_level == _state->npath) {
            get_next = true;
        }
    }

    if (get_next) {
        if (_state->normalize_results && _state->lex->token_type == JSON_TOKEN_STRING) {
            /* for as_text variants, tell get_scalar to set it for us */
            _state->next_scalar = true;
        } else {
            /* for non-as_text variants, just note the json starting point */
            _state->result_start = _state->lex->token_start;
        }
    }
}

static void get_object_field_end(void *state, char *fname, bool isnull)
{
    GetState *_state = (GetState *)state;
    bool get_last = false;
    int lex_level = _state->lex->lex_level;

    /* same tests as in get_object_field_start, mutatis mutandis */
    if (lex_level == 1 && _state->search_type == JSON_SEARCH_OBJECT && strcmp(fname, _state->search_term) == 0) {
        get_last = true;
    } else if (_state->search_type == JSON_SEARCH_PATH && lex_level <= _state->npath && _state->pathok[lex_level - 1] &&
               strcmp(fname, _state->path[lex_level - 1]) == 0) {
        /* done with this field so reset pathok */
        if (lex_level < _state->npath) {
            _state->pathok[lex_level] = false;
        }

        if (lex_level == _state->npath) {
            get_last = true;
        }
    }

    /* for as_test variants our work is already done */
    if (get_last && _state->result_start != NULL) {
        /*
         * make a text object from the string from the prevously noted json
         * start up to the end of the previous token (the lexer is by now
         * ahead of us on whatever came after what we're interested in).
         */
        int len = _state->lex->prev_token_terminator - _state->result_start;

        if (isnull && _state->normalize_results) {
            _state->tresult = (text *)NULL;
        } else {
            _state->tresult = cstring_to_text_with_len(_state->result_start, len);
        }
    }

    /*
     * don't need to reset _state->result_start b/c we're only returning one
     * datum, the conditions should not occur more than once, and this lets us
     * check cheaply that they don't (see object_field_start() )
     */
}

static void get_array_start(void *state)
{
    GetState *_state = (GetState *)state;
    int lex_level = _state->lex->lex_level;

    /* json structure check */
    if (lex_level == 0 && _state->search_type == JSON_SEARCH_OBJECT) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot extract field from a non-object")));
    }

    /*
     * initialize array count for this nesting level Note: the lex_level seen
     * by array_start is one less than that seen by the elements of the array.
     */
    if (_state->search_type == JSON_SEARCH_PATH && lex_level < _state->npath) {
        _state->array_level_index[lex_level] = -1;
    }
}

static void get_array_element_start(void *state, bool isnull)
{
    GetState *_state = (GetState *)state;
    bool get_next = false;
    int lex_level = _state->lex->lex_level;

    if (lex_level == 1 && _state->search_type == JSON_SEARCH_ARRAY) {
        /* single integer search */
        _state->array_index++;
        if (_state->array_index == _state->search_index) {
            get_next = true;
        }
    } else if (_state->search_type == JSON_SEARCH_PATH && lex_level <= _state->npath && _state->pathok[lex_level - 1]) {
        /*
         * path search, path so far is ok
         *
         * increment the array counter. no point doing this if we already know
         * the path is bad.
         *
         * then check if we have a match.
         */
        if (++_state->array_level_index[lex_level - 1] == _state->path_level_index[lex_level - 1]) {
            if (lex_level == _state->npath) {
                /* match and at end of path, so get value */
                get_next = true;
            } else {
                /* not at end of path just mark path ok */
                _state->pathok[lex_level] = true;
            }
        }
    }

    /* same logic as for objects */
    if (get_next) {
        if (_state->normalize_results && _state->lex->token_type == JSON_TOKEN_STRING) {
            _state->next_scalar = true;
        } else {
            _state->result_start = _state->lex->token_start;
        }
    }
}

static void get_array_element_end(void *state, bool isnull)
{
    GetState *_state = (GetState *)state;
    bool get_last = false;
    int lex_level = _state->lex->lex_level;

    /* same logic as in get_object_end, modified for arrays */
    if (lex_level == 1 && _state->search_type == JSON_SEARCH_ARRAY && _state->array_index == _state->search_index) {
        get_last = true;
    } else if (_state->search_type == JSON_SEARCH_PATH && lex_level <= _state->npath && _state->pathok[lex_level - 1] &&
               _state->array_level_index[lex_level - 1] == _state->path_level_index[lex_level - 1]) {
        /* done with this element so reset pathok */
        if (lex_level < _state->npath) {
            _state->pathok[lex_level] = false;
        }

        if (lex_level == _state->npath) {
            get_last = true;
        }
    }
    if (get_last && _state->result_start != NULL) {
        int len = _state->lex->prev_token_terminator - _state->result_start;

        if (isnull && _state->normalize_results) {
            _state->tresult = (text *)NULL;
        } else {
            _state->tresult = cstring_to_text_with_len(_state->result_start, len);
        }
    }
}

static void get_scalar(void *state, char *token, JsonTokenType tokentype)
{
    GetState *_state = (GetState *)state;

    if (_state->lex->lex_level == 0 && _state->search_type != JSON_SEARCH_PATH) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot extract element from a scalar")));
    }
    if (_state->next_scalar) {
        /* a de-escaped text value is wanted, so supply it */
        _state->tresult = cstring_to_text(token);
        /* make sure the next call to get_scalar doesn't overwrite it */
        _state->next_scalar = false;
    }
}

Datum jsonb_extract_path(PG_FUNCTION_ARGS)
{
    return get_jsonb_path_all(fcinfo, false);
}

Datum jsonb_extract_path_text(PG_FUNCTION_ARGS)
{
    return get_jsonb_path_all(fcinfo, true);
}

static inline Datum get_jsonb_path_all(FunctionCallInfo fcinfo, bool as_text)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    ArrayType *path = PG_GETARG_ARRAYTYPE_P(1);
    Datum *pathtext = NULL;
    bool *pathnulls = NULL;
    int npath;
    int i;
    Jsonb *res = NULL;
    bool have_object = false, have_array = false;
    JsonbValue *jbvp = NULL;
    JsonbValue tv;
    JsonbSuperHeader superHeader;

    if (array_contains_nulls(path)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call function with null path elements")));
    }

    deconstruct_array(path, TEXTOID, -1, false, 'i', &pathtext, &pathnulls, &npath);
    /*
     * If the array is empty, return NULL; this is dubious but it's what 9.3
     * did.
     */
    if (npath <= 0)
        PG_RETURN_NULL();

    if (JB_ROOT_IS_OBJECT(jb)) {
        have_object = true;
    } else if (JB_ROOT_IS_ARRAY(jb) && !JB_ROOT_IS_SCALAR(jb)) {
        have_array = true;
    }

    superHeader = (JsonbSuperHeader)VARDATA(jb);

    for (i = 0; i < npath; i++) {
        if (have_object) {
            jbvp = findJsonbValueFromSuperHeaderLen(superHeader, JB_FOBJECT, VARDATA_ANY(pathtext[i]),
                                                    VARSIZE_ANY_EXHDR(pathtext[i]));
        } else if (have_array) {
            long lindex;
            uint32 index;
            char *indextext = TextDatumGetCString(pathtext[i]);
            char *endptr = NULL;

            lindex = strtol(indextext, &endptr, 10);
            if (*endptr != '\0' || lindex > INT_MAX || lindex < 0) {
                PG_RETURN_NULL();
            }
            index = (uint32)lindex;
            jbvp = getIthJsonbValueFromSuperHeader(superHeader, index);
        } else {
            if (i == 0) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call extract path from a scalar")));
            }
            PG_RETURN_NULL();
        }

        if (jbvp == NULL) {
            PG_RETURN_NULL();
        } else if (i == npath - 1) {
            break;
        }

        if (jbvp->type == jbvBinary) {
            JsonbIterator *it = JsonbIteratorInit(jbvp->binary.data);
            int r;

            r = JsonbIteratorNext(&it, &tv, true);
            superHeader = (JsonbSuperHeader)jbvp->binary.data;
            have_object = r == WJB_BEGIN_OBJECT;
            have_array = r == WJB_BEGIN_ARRAY;
        } else {
            have_object = jbvp->type == jbvObject;
            have_array = jbvp->type == jbvArray;
        }
    }

    if (as_text) {
        if (jbvp->type == jbvString) {
            PG_RETURN_TEXT_P(cstring_to_text_with_len(jbvp->string.val, jbvp->string.len));
        } else if (jbvp->type == jbvNull) {
            PG_RETURN_NULL();
        }
    }

    res = JsonbValueToJsonb(jbvp);

    if (as_text) {
        PG_RETURN_TEXT_P(cstring_to_text(JsonbToCString(NULL, VARDATA(res), VARSIZE(res))));
    } else {
        /* not text mode - just hand back the jsonb */
        PG_RETURN_JSONB(res);
    }
}

/*
 * SQL function json_array_length(json) -> int
 */
Datum json_array_length(PG_FUNCTION_ARGS)
{
    text *json = NULL;

    AlenState *state = NULL;
    JsonLexContext *lex = NULL;
    JsonSemAction *sem = NULL;

    json = PG_GETARG_TEXT_P(0);
    lex = makeJsonLexContext(json, false);
    state = (AlenState *)palloc0(sizeof(AlenState));
    sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

    state->lex = lex;

    sem->semstate = (void *)state;
    sem->object_start = alen_object_start;
    sem->scalar = alen_scalar;
    sem->array_element_start = alen_array_element_start;

    pg_parse_json(lex, sem);

    PG_RETURN_INT32(state->count);
}

Datum jsonb_array_length(PG_FUNCTION_ARGS)
{
    Jsonb *jb = PG_GETARG_JSONB(0);

    if (JB_ROOT_IS_SCALAR(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot get array length of a scalar")));
    } else if (!JB_ROOT_IS_ARRAY(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot get array length of a non-array")));
    }

    PG_RETURN_INT32(JB_ROOT_COUNT(jb));
}

/*
 * These next two check ensure that the json is an array (since it can't be
 * a scalar or an object).
 */

static void alen_object_start(void *state)
{
    AlenState *_state = (AlenState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot get array length of a non-array")));
    }
}

static void alen_scalar(void *state, char *token, JsonTokenType tokentype)
{
    AlenState *_state = (AlenState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot get array length of a scalar")));
    }
}

static void alen_array_element_start(void *state, bool isnull)
{
    AlenState *_state = (AlenState *)state;

    /* just count up all the level 1 elements */
    if (_state->lex->lex_level == 1) {
        _state->count++;
    }
}

/*
 * SQL function json_each and json_each_text
 *
 * decompose a json object into key value pairs.
 *
 * Unlike json_object_keys() these SRFs operate in materialize mode,
 * stashing results into a Tuplestore object as they go.
 * The construction of tuples is done using a temporary memory context
 * that is cleared out after each tuple is built.
 */
Datum json_each(PG_FUNCTION_ARGS)
{
    return each_worker(fcinfo, false);
}

Datum jsonb_each(PG_FUNCTION_ARGS)
{
    return each_worker_jsonb(fcinfo, false);
}

Datum json_each_text(PG_FUNCTION_ARGS)
{
    return each_worker(fcinfo, true);
}

Datum jsonb_each_text(PG_FUNCTION_ARGS)
{
    return each_worker_jsonb(fcinfo, true);
}

static inline Datum each_worker_jsonb(FunctionCallInfo fcinfo, bool as_text)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    ReturnSetInfo *rsi = NULL;
    Tuplestorestate *tuple_store = NULL;
    TupleDesc tupdesc;
    TupleDesc ret_tdesc;
    MemoryContext old_cxt, tmp_cxt;
    bool skipNested = false;
    JsonbIterator *it = NULL;
    JsonbValue v;
    int r;

    if (!JB_ROOT_IS_OBJECT(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call jsonb_each%s on a non-object", as_text ? "_text" : "")));
    }

    rsi = (ReturnSetInfo *)fcinfo->resultinfo;

    if (!rsi || !IsA(rsi, ReturnSetInfo) || ((uint32)rsi->allowedModes & SFRM_Materialize) == 0 ||
        rsi->expectedDesc == NULL) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("set-valued function called in context that "
                                                                       "cannot accept a set")));
    }

    rsi->returnMode = SFRM_Materialize;

    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("function returning record called in context "
                                                                       "that cannot accept type record")));
    }

    old_cxt = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);

    ret_tdesc = CreateTupleDescCopy(tupdesc);
    BlessTupleDesc(ret_tdesc);
    tuple_store = tuplestore_begin_heap((uint32)rsi->allowedModes & SFRM_Materialize_Random, false,
                                        u_sess->attr.attr_memory.work_mem);

    MemoryContextSwitchTo(old_cxt);

    tmp_cxt = AllocSetContextCreate(CurrentMemoryContext, "jsonb_each temporary cxt", ALLOCSET_DEFAULT_MINSIZE,
                                    ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);

    it = JsonbIteratorInit(VARDATA_ANY(jb));
    while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
        skipNested = true;
        if (r == WJB_KEY) {
            text *key = NULL;
            HeapTuple tuple;
            Datum values[2];
            bool nulls[2] = {false, false};

            /* Use the tmp context so we can clean up after each tuple is done */
            old_cxt = MemoryContextSwitchTo(tmp_cxt);
            key = cstring_to_text_with_len(v.string.val, v.string.len);

            /*
             * The next thing the iterator fetches should be the value, no
             * matter what shape it is.
             */
            r = JsonbIteratorNext(&it, &v, skipNested);
            values[0] = PointerGetDatum(key);
            if (as_text) {
                if (v.type == jbvNull) {
                    /* a json null is an sql null in text mode */
                    nulls[1] = true;
                    values[1] = (Datum)NULL;
                } else {
                    text *sv = NULL;

                    if (v.type == jbvString) {
                        /* In text mode, scalar strings should be dequoted */
                        sv = cstring_to_text_with_len(v.string.val, v.string.len);
                    } else {
                        /* Turn anything else into a json string */
                        StringInfo jtext = makeStringInfo();
                        Jsonb *jb = JsonbValueToJsonb(&v);
                        (void)JsonbToCString(jtext, VARDATA(jb), 2 * v.estSize);
                        sv = cstring_to_text_with_len(jtext->data, jtext->len);
                    }
                    values[1] = PointerGetDatum(sv);
                }
            } else {
                /* Not in text mode, just return the Jsonb */
                Jsonb *val = JsonbValueToJsonb(&v);
                values[1] = PointerGetDatum(val);
            }
            tuple = heap_form_tuple(ret_tdesc, values, nulls);
            tuplestore_puttuple(tuple_store, tuple);

            /* clean up and switch back */
            MemoryContextSwitchTo(old_cxt);
            MemoryContextReset(tmp_cxt);
        }
    }
    MemoryContextDelete(tmp_cxt);
    rsi->setResult = tuple_store;
    rsi->setDesc = ret_tdesc;
    PG_RETURN_NULL();
}

static inline Datum each_worker(FunctionCallInfo fcinfo, bool as_text)
{
    text *json = NULL;
    JsonLexContext *lex = NULL;
    JsonSemAction *sem = NULL;
    ReturnSetInfo *rsi = NULL;
    MemoryContext old_cxt;
    TupleDesc tupdesc;
    EachState *state = NULL;

    json = PG_GETARG_TEXT_P(0);
    lex = makeJsonLexContext(json, true);
    state = (EachState *)palloc0(sizeof(EachState));
    sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));
    rsi = (ReturnSetInfo *)fcinfo->resultinfo;

    if (!rsi || !IsA(rsi, ReturnSetInfo) || ((uint32)rsi->allowedModes & SFRM_Materialize) == 0 ||
        rsi->expectedDesc == NULL) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("set-valued function called in context that "
                                                                       "cannot accept a set")));
    }

    rsi->returnMode = SFRM_Materialize;
    (void)get_call_result_type(fcinfo, NULL, &tupdesc);
    if (tupdesc == NULL) {
        ereport(ERROR,
                (errmodule(MOD_OPT), errcode(ERRCODE_UNEXPECTED_NULL_VALUE), errmsg("tupdesc should not be NULL value"),
                 errdetail("N/A"), errcause("An error occurred when obtaining the value of tupdesc."),
                 erraction("Contact Huawei Engineer.")));
    }

    /* make these in a sufficiently long-lived memory context */
    old_cxt = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);
    state->ret_tdesc = CreateTupleDescCopy(tupdesc);
    BlessTupleDesc(state->ret_tdesc);
    state->tuple_store = tuplestore_begin_heap((uint32)rsi->allowedModes & SFRM_Materialize_Random, false,
                                               u_sess->attr.attr_memory.work_mem);
    MemoryContextSwitchTo(old_cxt);

    sem->semstate = (void *)state;
    sem->array_start = each_array_start;
    sem->scalar = each_scalar;
    sem->object_field_start = each_object_field_start;
    sem->object_field_end = each_object_field_end;
    state->normalize_results = as_text;
    state->next_scalar = false;
    state->lex = lex;
    state->tmp_cxt = AllocSetContextCreate(CurrentMemoryContext, "json_each temporary cxt", ALLOCSET_DEFAULT_MINSIZE,
                                           ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);
    pg_parse_json(lex, sem);
    MemoryContextDelete(state->tmp_cxt);
    rsi->setResult = state->tuple_store;
    rsi->setDesc = state->ret_tdesc;
    PG_RETURN_NULL();
}

static void each_object_field_start(void *state, char *fname, bool isnull)
{
    EachState *_state = (EachState *)state;

    /* save a pointer to where the value starts */
    if (_state->lex->lex_level == 1) {
        /*
         * next_scalar will be reset in the object_field_end handler, and
         * since we know the value is a scalar there is no danger of it being
         * on while recursing down the tree.
         */
        if (_state->normalize_results && _state->lex->token_type == JSON_TOKEN_STRING) {
            _state->next_scalar = true;
        } else {
            _state->result_start = _state->lex->token_start;
        }
    }
}

static void each_object_field_end(void *state, char *fname, bool isnull)
{
    EachState *_state = (EachState *)state;
    MemoryContext old_cxt;
    int len;
    text *val = NULL;
    HeapTuple tuple;
    Datum values[2];
    bool nulls[2] = {false, false};

    /* skip over nested objects */
    if (_state->lex->lex_level != 1) {
        return;
    }

    /* use the tmp context so we can clean up after each tuple is done */
    old_cxt = MemoryContextSwitchTo(_state->tmp_cxt);

    values[0] = CStringGetTextDatum(fname);

    if (isnull && _state->normalize_results) {
        nulls[1] = true;
        values[1] = (Datum)NULL;
    } else if (_state->next_scalar) {
        values[1] = CStringGetTextDatum(_state->normalized_scalar);
        _state->next_scalar = false;
    } else {
        len = _state->lex->prev_token_terminator - _state->result_start;
        val = cstring_to_text_with_len(_state->result_start, len);
        values[1] = PointerGetDatum(val);
    }

    tuple = heap_form_tuple(_state->ret_tdesc, values, nulls);
    tuplestore_puttuple(_state->tuple_store, tuple);

    /* clean up and switch back */
    MemoryContextSwitchTo(old_cxt);
    MemoryContextReset(_state->tmp_cxt);
}

static void each_array_start(void *state)
{
    EachState *_state = (EachState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot deconstruct an array as an object")));
    }
}

static void each_scalar(void *state, char *token, JsonTokenType tokentype)
{
    EachState *_state = (EachState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot deconstruct a scalar")));
    }

    /* supply de-escaped value if required */
    if (_state->next_scalar) {
        _state->normalized_scalar = token;
    }
}

/*
 * SQL functions json_array_elements and json_array_elements_text
 *
 * get the elements from a json array
 *
 * a lot of this processing is similar to the json_each* functions
 */
Datum jsonb_array_elements(PG_FUNCTION_ARGS)
{
    return elements_worker_jsonb(fcinfo, false);
}

Datum jsonb_array_elements_text(PG_FUNCTION_ARGS)
{
    return elements_worker_jsonb(fcinfo, true);
}

static inline Datum elements_worker_jsonb(FunctionCallInfo fcinfo, bool as_text)
{
    Jsonb *jb = PG_GETARG_JSONB(0);
    ReturnSetInfo *rsi = NULL;
    Tuplestorestate *tuple_store = NULL;
    TupleDesc tupdesc;
    TupleDesc ret_tdesc;
    MemoryContext old_cxt, tmp_cxt;
    bool skipNested = false;
    JsonbIterator *it = NULL;
    JsonbValue v;
    int r;

    if (JB_ROOT_IS_SCALAR(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot extract elements from a scalar")));
    } else if (!JB_ROOT_IS_ARRAY(jb)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot extract elements from an object")));
    }

    rsi = (ReturnSetInfo *)fcinfo->resultinfo;
    if (!rsi || !IsA(rsi, ReturnSetInfo) || (rsi->allowedModes & SFRM_Materialize) == 0 || rsi->expectedDesc == NULL) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("set-valued function called in context that "
                                                                       "cannot accept a set")));
    }

    rsi->returnMode = SFRM_Materialize;
    /* it's a simple type, so don't use get_call_result_type() */
    tupdesc = rsi->expectedDesc;
    old_cxt = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);
    ret_tdesc = CreateTupleDescCopy(tupdesc);
    BlessTupleDesc(ret_tdesc);
    tuple_store =
        tuplestore_begin_heap(rsi->allowedModes & SFRM_Materialize_Random, false, u_sess->attr.attr_memory.work_mem);
    MemoryContextSwitchTo(old_cxt);

    tmp_cxt = AllocSetContextCreate(CurrentMemoryContext, "jsonb_each temporary cxt", ALLOCSET_DEFAULT_MINSIZE,
                                    ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);
    it = JsonbIteratorInit(VARDATA_ANY(jb));
    while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
        skipNested = true;
        if (r == WJB_ELEM) {
            HeapTuple tuple;
            Datum values[1];
            bool nulls[1] = {false};
            /* use the tmp context so we can clean up after each tuple is done */
            old_cxt = MemoryContextSwitchTo(tmp_cxt);
            if (!as_text) {
                Jsonb *val = JsonbValueToJsonb(&v);
                values[0] = PointerGetDatum(val);
            } else {
                if (v.type == jbvNull) {
                    /* a json null is an sql null in text mode */
                    nulls[0] = true;
                    values[0] = (Datum)NULL;
                } else {
                    text *sv = NULL;
                    if (v.type == jbvString) {
                        /* in text mode scalar strings should be dequoted */
                        sv = cstring_to_text_with_len(v.string.val, v.string.len);
                    } else {
                        /* turn anything else into a json string */
                        StringInfo jtext = makeStringInfo();
                        Jsonb *jb = JsonbValueToJsonb(&v);
                        (void)JsonbToCString(jtext, VARDATA(jb), 2 * v.estSize);
                        sv = cstring_to_text_with_len(jtext->data, jtext->len);
                    }
                    values[0] = PointerGetDatum(sv);
                }
            }

            tuple = heap_form_tuple(ret_tdesc, values, nulls);
            tuplestore_puttuple(tuple_store, tuple);

            /* clean up and switch back */
            MemoryContextSwitchTo(old_cxt);
            MemoryContextReset(tmp_cxt);
        }
    }
    MemoryContextDelete(tmp_cxt);

    rsi->setResult = tuple_store;
    rsi->setDesc = ret_tdesc;

    PG_RETURN_NULL();
}

Datum json_array_elements(PG_FUNCTION_ARGS)
{
    return elements_worker(fcinfo, false);
}

Datum json_array_elements_text(PG_FUNCTION_ARGS)
{
    return elements_worker(fcinfo, true);
}

static inline Datum elements_worker(FunctionCallInfo fcinfo, bool as_text)
{
    text *json = PG_GETARG_TEXT_P(0);

    /* elements only needs escaped strings when as_text */
    JsonLexContext *lex = makeJsonLexContext(json, as_text);
    JsonSemAction *sem = NULL;
    ReturnSetInfo *rsi = NULL;
    MemoryContext old_cxt;
    TupleDesc tupdesc;
    ElementsState *state = NULL;

    state = (ElementsState *)palloc0(sizeof(ElementsState));
    sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));
    rsi = (ReturnSetInfo *)fcinfo->resultinfo;

    if (!rsi || !IsA(rsi, ReturnSetInfo) || ((uint32)rsi->allowedModes & SFRM_Materialize) == 0 ||
        rsi->expectedDesc == NULL) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("set-valued function called in context that "
                                                                       "cannot accept a set")));
    }
    rsi->returnMode = SFRM_Materialize;

    /* it's a simple type, so don't use get_call_result_type() */
    tupdesc = rsi->expectedDesc;

    /* make these in a sufficiently long-lived memory context */
    old_cxt = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);
    state->ret_tdesc = CreateTupleDescCopy(tupdesc);
    BlessTupleDesc(state->ret_tdesc);
    state->tuple_store = tuplestore_begin_heap((uint32)rsi->allowedModes & SFRM_Materialize_Random, false,
                                               u_sess->attr.attr_memory.work_mem);
    MemoryContextSwitchTo(old_cxt);

    sem->semstate = (void *)state;
    sem->object_start = elements_object_start;
    sem->scalar = elements_scalar;
    sem->array_element_start = elements_array_element_start;
    sem->array_element_end = elements_array_element_end;
    state->normalize_results = as_text;
    state->next_scalar = false;
    state->lex = lex;
    state->tmp_cxt =
        AllocSetContextCreate(CurrentMemoryContext, "json_array_elements temporary cxt", ALLOCSET_DEFAULT_MINSIZE,
                              ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);
    pg_parse_json(lex, sem);
    MemoryContextDelete(state->tmp_cxt);

    rsi->setResult = state->tuple_store;
    rsi->setDesc = state->ret_tdesc;
    PG_RETURN_NULL();
}

static void elements_array_element_start(void *state, bool isnull)
{
    ElementsState *_state = (ElementsState *)state;

    /* save a pointer to where the value starts */
    if (_state->lex->lex_level == 1) {
        /*
         * next_scalar will be reset in the array_element_end handler, and
         * since we know the value is a scalar there is no danger of it being
         * on while recursing down the tree.
         */
        if (_state->normalize_results && _state->lex->token_type == JSON_TOKEN_STRING) {
            _state->next_scalar = true;
        } else {
            _state->result_start = _state->lex->token_start;
        }
    }
}

static void elements_array_element_end(void *state, bool isnull)
{
    ElementsState *_state = (ElementsState *)state;
    MemoryContext old_cxt;
    int len;
    text *val = NULL;
    HeapTuple tuple;
    Datum values[1];
    bool nulls[1] = {false};

    /* skip over nested objects */
    if (_state->lex->lex_level != 1) {
        return;
    }

    /* use the tmp context so we can clean up after each tuple is done */
    old_cxt = MemoryContextSwitchTo(_state->tmp_cxt);

    if (isnull && _state->normalize_results) {
        nulls[0] = true;
        values[0] = (Datum)NULL;
    } else if (_state->next_scalar) {
        values[0] = CStringGetTextDatum(_state->normalized_scalar);
        _state->next_scalar = false;
    } else {
        len = _state->lex->prev_token_terminator - _state->result_start;
        val = cstring_to_text_with_len(_state->result_start, len);
        values[0] = PointerGetDatum(val);
    }

    tuple = heap_form_tuple(_state->ret_tdesc, values, nulls);
    tuplestore_puttuple(_state->tuple_store, tuple);

    /* clean up and switch back */
    MemoryContextSwitchTo(old_cxt);
    MemoryContextReset(_state->tmp_cxt);
}

static void elements_object_start(void *state)
{
    ElementsState *_state = (ElementsState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call json_array_elements on a non-array")));
    }
}

static void elements_scalar(void *state, char *token, JsonTokenType tokentype)
{
    ElementsState *_state = (ElementsState *)state;

    /* json structure check */
    if (_state->lex->lex_level == 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call json_array_elements on a scalar")));
    }

    /* supply de-escaped value if required */
    if (_state->next_scalar) {
        _state->normalized_scalar = token;
    }
}

/*
 * SQL function json_populate_record
 *
 * set fields in a record from the argument json
 *
 * Code adapted shamelessly from hstore's populate_record
 * which is in turn partly adapted from record_out.
 *
 * The json is decomposed into a hash table, in which each
 * field in the record is then looked up by name. For jsonb
 * we fetch the values direct from the object.
 */
Datum jsonb_populate_record(PG_FUNCTION_ARGS)
{
    return populate_record_worker(fcinfo, true);
}

Datum json_populate_record(PG_FUNCTION_ARGS)
{
    return populate_record_worker(fcinfo, true);
}

Datum json_to_record(PG_FUNCTION_ARGS)
{
    return populate_record_worker(fcinfo, false);
}

static inline Datum populate_record_worker(FunctionCallInfo fcinfo, bool have_record_arg)
{
    Oid argtype;
    Oid jtype = get_fn_expr_argtype(fcinfo->flinfo, have_record_arg ? 1 : 0);
    text *json = NULL;
    Jsonb *jb = NULL;
    bool use_json_as_text = false;
    HTAB *json_hash = NULL;
    HeapTupleHeader rec = NULL;
    Oid tupType = InvalidOid;
    int32 tupTypmod = -1;
    TupleDesc tupdesc;
    HeapTupleData tuple;
    HeapTuple rettuple;
    RecordIOData *my_extra = NULL;
    int ncolumns;
    int i;
    Datum *values = NULL;
    bool *nulls = NULL;
    errno_t rc = 0;

    Assert(jtype == JSONOID || jtype == JSONBOID);

    use_json_as_text = PG_ARGISNULL(have_record_arg ? 2 : 1) ? false : PG_GETARG_BOOL(have_record_arg ? 2 : 1);

    if (have_record_arg) {
        argtype = get_fn_expr_argtype(fcinfo->flinfo, 0);

        if (!type_is_rowtype(argtype)) {
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("first argument of json%s_populate_record must be a row type",
                                   jtype == JSONBOID ? "b" : "")));
        }

        if (PG_ARGISNULL(0)) {
            if (PG_ARGISNULL(1)) {
                PG_RETURN_NULL();
            }
            /*
             * have no tuple to look at, so the only source of type info is
             * the argtype. The lookup_rowtype_tupdesc call below will error
             * out if we don't have a known composite type oid here.
             */
            tupType = argtype;
            tupTypmod = -1;
        } else {
            rec = PG_GETARG_HEAPTUPLEHEADER(0);
            if (PG_ARGISNULL(1)) {
                PG_RETURN_POINTER(rec);
            }
            /* Extract type info from the tuple itself */
            tupType = HeapTupleHeaderGetTypeId(rec);
            tupTypmod = HeapTupleHeaderGetTypMod(rec);
        }
        tupdesc = lookup_rowtype_tupdesc(tupType, tupTypmod);
    } else { /* json{b}_to_record case */
        use_json_as_text = PG_ARGISNULL(1) ? false : PG_GETARG_BOOL(1);
        if (PG_ARGISNULL(0)) {
            PG_RETURN_NULL();
        }
        if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE) {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("function returning record called in context "
                                   "that cannot accept type record"),
                            errhint("Try calling the function in the FROM clause "
                                    "using a column definition list.")));
        }
    }

    if (jtype == JSONOID) {
        /* just get the text */
        json = PG_GETARG_TEXT_P(have_record_arg ? 1 : 0);
        json_hash = get_json_object_as_hash(json, "json_populate_record", use_json_as_text);
        /*
         * if the input json is empty, we can only skip the rest if we were
         * passed in a non-null record, since otherwise there may be issues
         * with domain nulls.
         */
        if (hash_get_num_entries(json_hash) == 0 && rec) {
            PG_RETURN_POINTER(rec);
        }
    } else {
        jb = PG_GETARG_JSONB(have_record_arg ? 1 : 0);
        /* same logic as for json */
        if (!have_record_arg && rec) {
            PG_RETURN_POINTER(rec);
        }
    }
    ncolumns = tupdesc->natts;
    if (rec) {
        /* Build a temporary HeapTuple control structure */
        tuple.t_len = HeapTupleHeaderGetDatumLength(rec);
        ItemPointerSetInvalid(&(tuple.t_self));
        tuple.t_tableOid = InvalidOid;
        tuple.t_data = rec;
    }

    /*
     * We arrange to look up the needed I/O info just once per series of
     * calls, assuming the record type doesn't change underneath us.
     */
    my_extra = (RecordIOData *)fcinfo->flinfo->fn_extra;
    if (my_extra == NULL || my_extra->ncolumns != ncolumns) {
        fcinfo->flinfo->fn_extra = MemoryContextAlloc(
            fcinfo->flinfo->fn_mcxt, sizeof(RecordIOData) - sizeof(ColumnIOData) + ncolumns * sizeof(ColumnIOData));
        my_extra = (RecordIOData *)fcinfo->flinfo->fn_extra;
        my_extra->record_type = InvalidOid;
        my_extra->record_typmod = 0;
    }

    if (have_record_arg && (my_extra->record_type != tupType || my_extra->record_typmod != tupTypmod)) {
        rc = memset_s(my_extra, sizeof(RecordIOData) - sizeof(ColumnIOData) + ncolumns * sizeof(ColumnIOData), 0,
                      sizeof(RecordIOData) - sizeof(ColumnIOData) + ncolumns * sizeof(ColumnIOData));
        securec_check(rc, "\0", "\0");
        my_extra->record_type = tupType;
        my_extra->record_typmod = tupTypmod;
        my_extra->ncolumns = ncolumns;
    }

    values = (Datum *)palloc(ncolumns * sizeof(Datum));
    nulls = (bool *)palloc(ncolumns * sizeof(bool));

    if (rec) {
        /* Break down the tuple into fields */
        heap_deform_tuple(&tuple, tupdesc, values, nulls);
    } else {
        for (i = 0; i < ncolumns; ++i) {
            values[i] = (Datum)0;
            nulls[i] = true;
        }
    }

    for (i = 0; i < ncolumns; ++i) {
        ColumnIOData *column_info = &my_extra->columns[i];
        Oid         column_type = tupdesc->attrs[i].atttypid;
        JsonbValue *v = NULL;
        char fname[NAMEDATALEN];
        JsonHashEntry *hashentry = NULL;

        /* Ignore dropped columns in datatype */
        if (tupdesc->attrs[i].attisdropped) {
            nulls[i] = true;
            continue;
        }

        if (jtype == JSONOID) {
            rc = memset_s(fname, NAMEDATALEN, 0, NAMEDATALEN);
            securec_check(rc, "\0", "\0");
            rc = strncpy_s(fname, NAMEDATALEN, NameStr(tupdesc->attrs[i].attname), NAMEDATALEN - 1);
            securec_check(rc, "\0", "\0");
            hashentry = (JsonHashEntry *)hash_search(json_hash, fname, HASH_FIND, NULL);
        } else {
            char       *key = NameStr(tupdesc->attrs[i].attname);
            v = findJsonbValueFromSuperHeaderLen(VARDATA(jb), JB_FOBJECT, key, strlen(key));
        }

        /*
         * we can't just skip here if the key wasn't found since we might have
         * a domain to deal with. If we were passed in a non-null record
         * datum, we assume that the existing values are valid (if they're
         * not, then it's not our fault), but if we were passed in a null,
         * then every field which we don't populate needs to be run through
         * the input function just in case it's a domain type.
         */
        if (((jtype == JSONOID && hashentry == NULL) || (jtype == JSONBOID && v == NULL)) && rec) {
            continue;
        }

        /*
         * Prepare to convert the column value from text
         */
        if (column_info->column_type != column_type) {
            getTypeInputInfo(column_type, &column_info->typiofunc, &column_info->typioparam);
            fmgr_info_cxt(column_info->typiofunc, &column_info->proc, fcinfo->flinfo->fn_mcxt);
            column_info->column_type = column_type;
        }
        if ((jtype == JSONOID && (hashentry == NULL || hashentry->isnull)) ||
            (jtype == JSONBOID && (v == NULL || v->type == jbvNull))) {
            /*
             * need InputFunctionCall to happen even for nulls, so that domain
             * checks are done
             */
            values[i] = InputFunctionCall(&column_info->proc, NULL, column_info->typioparam,
                                          tupdesc->attrs[i].atttypmod);
            nulls[i] = true;
        } else {
            char *s = NULL;

            if (jtype == JSONOID) {
                /* already done the hard work in the json case */
                s = hashentry->val;
            } else {
                if (v->type == jbvString) {
                    s = pnstrdup(v->string.val, v->string.len);
                } else if (v->type == jbvBool) {
                    s = pnstrdup((v->boolean) ? "t" : "f", 1);
                } else if (v->type == jbvNumeric) {
                    s = DatumGetCString(DirectFunctionCall1(numeric_out, PointerGetDatum(v->numeric)));
                } else if (!use_json_as_text) {
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                    errmsg("cannot populate with a nested object unless use_json_as_text is true")));
                } else if (v->type == jbvBinary) {
                    s = JsonbToCString(NULL, v->binary.data, v->binary.len);
                } else {
                    elog(ERROR, "invalid jsonb type");
                }
            }

            values[i] = InputFunctionCall(&column_info->proc, s,
                                          column_info->typioparam, tupdesc->attrs[i].atttypmod);
            nulls[i] = false;
        }
    }

    rettuple = heap_form_tuple(tupdesc, values, nulls);
    ReleaseTupleDesc(tupdesc);
    PG_RETURN_DATUM(HeapTupleGetDatum(rettuple));
}

/*
 * get_json_object_as_hash
 *
 * decompose a json object into a hash table.
 *
 * Currently doesn't allow anything but a flat object. Should this
 * change?
 *
 * funcname argument allows caller to pass in its name for use in
 * error messages.
 */
static HTAB *get_json_object_as_hash(text *json, char *funcname, bool use_json_as_text)
{
    HASHCTL ctl;
    HTAB *tab = NULL;
    JHashState *state = NULL;
    JsonLexContext *lex = makeJsonLexContext(json, true);
    JsonSemAction *sem = NULL;

    errno_t rc = memset_s(&ctl, sizeof(ctl), 0, sizeof(ctl));
    securec_check(rc, "\0", "\0");
    ctl.keysize = NAMEDATALEN;
    ctl.entrysize = sizeof(JsonHashEntry);
    ctl.hcxt = CurrentMemoryContext;
    tab = hash_create("json object hashtable", 100, &ctl, HASH_ELEM | HASH_CONTEXT);

    state = (JHashState *)palloc0(sizeof(JHashState));
    sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

    state->function_name = funcname;
    state->hash = tab;
    state->lex = lex;
    state->use_json_as_text = use_json_as_text;

    sem->semstate = (void *)state;
    sem->array_start = hash_array_start;
    sem->scalar = hash_scalar;
    sem->object_field_start = hash_object_field_start;
    sem->object_field_end = hash_object_field_end;

    pg_parse_json(lex, sem);

    return tab;
}

static void hash_object_field_start(void *state, char *fname, bool isnull)
{
    JHashState *_state = (JHashState *)state;

    if (_state->lex->lex_level > 1) {
        return;
    }

    if (_state->lex->token_type == JSON_TOKEN_ARRAY_START || _state->lex->token_type == JSON_TOKEN_OBJECT_START) {
        if (!_state->use_json_as_text) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("cannot call %s on a nested object", _state->function_name)));
        }
        _state->save_json_start = _state->lex->token_start;
    } else {
        /* must be a scalar */
        _state->save_json_start = NULL;
    }
}

static void hash_object_field_end(void *state, char *fname, bool isnull)
{
    JHashState *_state = (JHashState *)state;
    JsonHashEntry *hashentry = NULL;
    bool found = false;
    char name[NAMEDATALEN];

    /*
     * ignore field names >= NAMEDATALEN - they can't match a record field
     * ignore nested fields.
     */
    if (_state->lex->lex_level > 2 || strlen(fname) >= NAMEDATALEN) {
        return;
    }

    errno_t rc = memset_s(&name, NAMEDATALEN, 0, NAMEDATALEN);
    securec_check(rc, "\0", "\0");
    rc = strncpy_s(name, NAMEDATALEN, fname, NAMEDATALEN - 1);
    securec_check(rc, "\0", "\0");

    hashentry = (JsonHashEntry *)hash_search(_state->hash, name, HASH_ENTER, &found);

    /*
     * found being true indicates a duplicate. We don't do anything about
     * that, a later field with the same name overrides the earlier field.
     */

    hashentry->isnull = isnull;
    if (_state->save_json_start != NULL) {
        int len = _state->lex->prev_token_terminator - _state->save_json_start;
        char *val = (char *)palloc((len + 1) * sizeof(char));

        rc = memcpy_s(val, (len + 1) * sizeof(char), _state->save_json_start, len);
        securec_check(rc, "\0", "\0");
        val[len] = '\0';
        hashentry->val = val;
    } else {
        /* must have had a scalar instead */
        hashentry->val = _state->saved_scalar;
    }
}

static void hash_array_start(void *state)
{
    JHashState *_state = (JHashState *)state;

    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call %s on an array", _state->function_name)));
    }
}

static void hash_scalar(void *state, char *token, JsonTokenType tokentype)
{
    JHashState *_state = (JHashState *)state;

    if (_state->lex->lex_level == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call %s on a scalar", _state->function_name)));
    }

    if (_state->lex->lex_level == 1) {
        _state->saved_scalar = token;
    }
}

/*
 * SQL function json_populate_recordset
 *
 * set fields in a set of records from the argument json,
 * which must be an array of objects.
 *
 * similar to json_populate_record, but the tuple-building code
 * is pushed down into the semantic action handlers so it's done
 * per object in the array.
 */
Datum jsonb_populate_recordset(PG_FUNCTION_ARGS)
{
    return populate_recordset_worker(fcinfo, true);
}

static void make_row_from_rec_and_jsonb(Jsonb *element, PopulateRecordsetState *state)
{
    Datum *values = NULL;
    bool *nulls = NULL;
    int i;
    RecordIOData *my_extra = state->my_extra;
    int ncolumns = my_extra->ncolumns;
    TupleDesc tupdesc = state->ret_tdesc;
    HeapTupleHeader rec = state->rec;
    HeapTuple rettuple;

    values = (Datum *)palloc(ncolumns * sizeof(Datum));
    nulls = (bool *)palloc(ncolumns * sizeof(bool));

    if (state->rec) {
        HeapTupleData tuple;
        /* Build a temporary HeapTuple control structure */
        tuple.t_len = HeapTupleHeaderGetDatumLength(state->rec);
        ItemPointerSetInvalid(&(tuple.t_self));
        tuple.t_tableOid = InvalidOid;
        tuple.t_data = state->rec;

        /* Break down the tuple into fields */
        heap_deform_tuple(&tuple, tupdesc, values, nulls);
    } else {
        for (i = 0; i < ncolumns; ++i) {
            values[i] = (Datum)0;
            nulls[i] = true;
        }
    }

    for (i = 0; i < ncolumns; ++i) {
        ColumnIOData *column_info = &my_extra->columns[i];
        Oid           column_type = tupdesc->attrs[i].atttypid;
        JsonbValue   *v = NULL;
        char         *key = NULL;

        /* Ignore dropped columns in datatype */
        if (tupdesc->attrs[i].attisdropped) {
            nulls[i] = true;
            continue;
        }
        key = NameStr(tupdesc->attrs[i].attname);
        v = findJsonbValueFromSuperHeaderLen(VARDATA(element), JB_FOBJECT, key, strlen(key));

        /*
         * We can't just skip here if the key wasn't found since we might have
         * a domain to deal with. If we were passed in a non-null record
         * datum, we assume that the existing values are valid (if they're
         * not, then it's not our fault), but if we were passed in a null,
         * then every field which we don't populate needs to be run through
         * the input function just in case it's a domain type.
         */
        if (v == NULL && rec) {
            continue;
        }

        /*
         * Prepare to convert the column value from text
         */
        if (column_info->column_type != column_type) {
            getTypeInputInfo(column_type, &column_info->typiofunc, &column_info->typioparam);
            fmgr_info_cxt(column_info->typiofunc, &column_info->proc, state->fn_mcxt);
            column_info->column_type = column_type;
        }
        if (v == NULL || v->type == jbvNull) {
            /*
             * Need InputFunctionCall to happen even for nulls, so that domain
             * checks are done
             */
            values[i] = InputFunctionCall(&column_info->proc, NULL, column_info->typioparam,
                                          tupdesc->attrs[i].atttypmod);
            nulls[i] = true;
        } else {
            char *s = NULL;

            if (v->type == jbvString) {
                s = pnstrdup(v->string.val, v->string.len);
            } else if (v->type == jbvBool) {
                s = pnstrdup((v->boolean) ? "t" : "f", 1);
            } else if (v->type == jbvNumeric) {
                s = DatumGetCString(DirectFunctionCall1(numeric_out, PointerGetDatum(v->numeric)));
            } else if (!state->use_json_as_text) {
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("cannot populate with a nested object unless use_json_as_text is true")));
            } else if (v->type == jbvBinary) {
                s = JsonbToCString(NULL, v->binary.data, v->binary.len);
            } else {
                elog(ERROR, "invalid jsonb type");
            }

            values[i] = InputFunctionCall(&column_info->proc, s, column_info->typioparam, tupdesc->attrs[i].atttypmod);
            nulls[i] = false;
        }
    }

    rettuple = heap_form_tuple(tupdesc, values, nulls);
    tuplestore_puttuple(state->tuple_store, rettuple);
}

Datum json_populate_recordset(PG_FUNCTION_ARGS)
{
    return populate_recordset_worker(fcinfo, true);
}

Datum json_to_recordset(PG_FUNCTION_ARGS)
{
    return populate_recordset_worker(fcinfo, false);
}

/*
 * common worker for json_populate_recordset() and json_to_recordset()
 */
static inline Datum populate_recordset_worker(FunctionCallInfo fcinfo, bool have_record_arg)
{
    Oid argtype;
    Oid jtype = get_fn_expr_argtype(fcinfo->flinfo, have_record_arg ? 1 : 0);
    bool use_json_as_text = false;
    ReturnSetInfo *rsi = NULL;
    MemoryContext old_cxt;
    Oid tupType;
    int32 tupTypmod;
    HeapTupleHeader rec;
    TupleDesc tupdesc;
    bool needforget = false;
    RecordIOData *my_extra = NULL;
    int ncolumns;
    PopulateRecordsetState *state = NULL;

    if (have_record_arg) {
        argtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
        use_json_as_text = PG_ARGISNULL(2) ? false : PG_GETARG_BOOL(2);
        if (!type_is_rowtype(argtype)) {
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
                            errmsg("first argument of json_populate_recordset must be a row type")));
        }
    } else {
        argtype = InvalidOid;
        use_json_as_text = PG_ARGISNULL(1) ? false : PG_GETARG_BOOL(1);
    }

    rsi = (ReturnSetInfo *)fcinfo->resultinfo;
    if (!rsi || !IsA(rsi, ReturnSetInfo) || ((uint32)rsi->allowedModes & SFRM_Materialize) == 0 ||
        rsi->expectedDesc == NULL) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("set-valued function called in context that "
                                                                       "cannot accept a set")));
    }
    rsi->returnMode = SFRM_Materialize;
    /*
     * get the tupdesc from the result set info - it must be a record type
     * because we already checked that arg1 is a record type, or we're in a
     * to_record function which returns a setof record.
     */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("function returning record called in context "
                                                                       "that cannot accept type record")));
    }

    /* if the json is null send back an empty set */
    if (have_record_arg) {
        if (PG_ARGISNULL(1)) {
            PG_RETURN_NULL();
        }
        if (PG_ARGISNULL(0)) {
            rec = NULL;
        } else {
            /* using the arg tupdesc, because it may not be the same as the result tupdesc. */
            rec = PG_GETARG_HEAPTUPLEHEADER(0);
            tupdesc = lookup_rowtype_tupdesc(HeapTupleHeaderGetTypeId(rec), HeapTupleHeaderGetTypMod(rec));
            needforget = true;
        }
    } else {
        if (PG_ARGISNULL(1)) {
            PG_RETURN_NULL();
        }
        rec = NULL;
    }

    tupType = tupdesc->tdtypeid;
    tupTypmod = tupdesc->tdtypmod;
    ncolumns = tupdesc->natts;

    /*
     * We arrange to look up the needed I/O info just once per series of
     * calls, assuming the record type doesn't change underneath us.
     */
    my_extra = (RecordIOData *)fcinfo->flinfo->fn_extra;
    if (my_extra == NULL || my_extra->ncolumns != ncolumns) {
        fcinfo->flinfo->fn_extra = MemoryContextAlloc(
            fcinfo->flinfo->fn_mcxt, sizeof(RecordIOData) - sizeof(ColumnIOData) + ncolumns * sizeof(ColumnIOData));
        my_extra = (RecordIOData *)fcinfo->flinfo->fn_extra;
        my_extra->record_type = InvalidOid;
        my_extra->record_typmod = 0;
    }

    if (my_extra->record_type != tupType || my_extra->record_typmod != tupTypmod) {
        errno_t rc = memset_s(my_extra, sizeof(RecordIOData) - sizeof(ColumnIOData) + ncolumns * sizeof(ColumnIOData),
                              0, sizeof(RecordIOData) - sizeof(ColumnIOData) + ncolumns * sizeof(ColumnIOData));
        securec_check(rc, "\0", "\0");
        my_extra->record_type = tupType;
        my_extra->record_typmod = tupTypmod;
        my_extra->ncolumns = ncolumns;
    }
    state = (PopulateRecordsetState *)palloc0(sizeof(PopulateRecordsetState));
    /* make these in a sufficiently long-lived memory context */
    old_cxt = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);
    state->ret_tdesc = CreateTupleDescCopy(tupdesc);
    if (needforget) {
        DecrTupleDescRefCount(tupdesc);
    }
    BlessTupleDesc(state->ret_tdesc);
    state->tuple_store = tuplestore_begin_heap((uint32)rsi->allowedModes & SFRM_Materialize_Random, false,
                                               u_sess->attr.attr_memory.work_mem);
    MemoryContextSwitchTo(old_cxt);

    state->my_extra = my_extra;
    state->rec = rec;
    state->use_json_as_text = use_json_as_text;
    state->fn_mcxt = fcinfo->flinfo->fn_mcxt;

    if (jtype == JSONOID) {
        text *json = PG_GETARG_TEXT_P(have_record_arg ? 1 : 0);
        JsonLexContext *lex = NULL;
        JsonSemAction *sem = NULL;

        sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));
        lex = makeJsonLexContext(json, true);
        sem->semstate = (void *)state;
        sem->array_start = populate_recordset_array_start;
        sem->array_element_start = populate_recordset_array_element_start;
        sem->scalar = populate_recordset_scalar;
        sem->object_field_start = populate_recordset_object_field_start;
        sem->object_field_end = populate_recordset_object_field_end;
        sem->object_start = populate_recordset_object_start;
        sem->object_end = populate_recordset_object_end;
        state->lex = lex;

        pg_parse_json(lex, sem);
    } else {
        Jsonb *jb = NULL;
        JsonbIterator *it = NULL;
        JsonbValue v;
        bool skipNested = false;
        int r;

        Assert(jtype == JSONBOID);
        jb = PG_GETARG_JSONB(have_record_arg ? 1 : 0);
        if (JB_ROOT_IS_SCALAR(jb) || !JB_ROOT_IS_ARRAY(jb)) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("cannot call jsonb_populate_recordset on non-array")));
        }
        it = JsonbIteratorInit(VARDATA_ANY(jb));
        while ((r = JsonbIteratorNext(&it, &v, skipNested)) != WJB_DONE) {
            skipNested = true;
            if (r == WJB_ELEM) {
                Jsonb *element = JsonbValueToJsonb(&v);
                if (!JB_ROOT_IS_OBJECT(element)) {
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                    errmsg("jsonb_populate_recordset argument must be an array of objects")));
                }
                make_row_from_rec_and_jsonb(element, state);
            }
        }
    }
    rsi->setResult = state->tuple_store;
    rsi->setDesc = state->ret_tdesc;
    PG_RETURN_NULL();
}

static void populate_recordset_object_start(void *state)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;
    int lex_level = _state->lex->lex_level;
    HASHCTL ctl;

    if (lex_level == 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call json_populate_recordset on an object")));
    } else if (lex_level > 1 && !_state->use_json_as_text) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call json_populate_recordset with nested objects")));
    }

    /* set up a new hash for this entry */
    errno_t rc = memset_s(&ctl, sizeof(ctl), 0, sizeof(ctl));
    securec_check(rc, "\0", "\0");
    ctl.keysize = NAMEDATALEN;
    ctl.entrysize = sizeof(JsonHashEntry);
    ctl.hcxt = CurrentMemoryContext;
    _state->json_hash = hash_create("json object hashtable", 100, &ctl, HASH_ELEM | HASH_CONTEXT);
}

static void populate_recordset_object_end(void *state)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;
    HTAB *json_hash = _state->json_hash;
    Datum *values = NULL;
    bool *nulls = NULL;
    char fname[NAMEDATALEN];
    int i;
    RecordIOData *my_extra = _state->my_extra;
    int ncolumns = my_extra->ncolumns;
    TupleDesc tupdesc = _state->ret_tdesc;
    JsonHashEntry *hashentry = NULL;
    HeapTupleHeader rec = _state->rec;
    HeapTuple rettuple;

    if (_state->lex->lex_level > 1) {
        return;
    }
    values = (Datum *)palloc(ncolumns * sizeof(Datum));
    nulls = (bool *)palloc(ncolumns * sizeof(bool));
    if (_state->rec) {
        HeapTupleData tuple;
        /* Build a temporary HeapTuple control structure */
        tuple.t_len = HeapTupleHeaderGetDatumLength(_state->rec);
        ItemPointerSetInvalid(&(tuple.t_self));
        tuple.t_tableOid = InvalidOid;
        tuple.t_data = _state->rec;
        /* Break down the tuple into fields */
        heap_deform_tuple(&tuple, tupdesc, values, nulls);
    } else {
        for (i = 0; i < ncolumns; ++i) {
            values[i] = (Datum)0;
            nulls[i] = true;
        }
    }

    for (i = 0; i < ncolumns; ++i) {
        ColumnIOData *column_info = &my_extra->columns[i];
        Oid           column_type = tupdesc->attrs[i].atttypid;
        char         *value = NULL;

        /* Ignore dropped columns in datatype */
        if (tupdesc->attrs[i].attisdropped) {
            nulls[i] = true;
            continue;
        }

        errno_t rc = memset_s(fname, NAMEDATALEN, 0, NAMEDATALEN);
        securec_check(rc, "\0", "\0");
        rc = strncpy_s(fname, NAMEDATALEN, NameStr(tupdesc->attrs[i].attname), NAMEDATALEN - 1);
        securec_check(rc, "\0", "\0");
        hashentry = (JsonHashEntry *)hash_search(json_hash, fname, HASH_FIND, NULL);

        /*
         * we can't just skip here if the key wasn't found since we might have
         * a domain to deal with. If we were passed in a non-null record
         * datum, we assume that the existing values are valid (if they're
         * not, then it's not our fault), but if we were passed in a null,
         * then every field which we don't populate needs to be run through
         * the input function just in case it's a domain type.
         */
        if (hashentry == NULL && rec) {
            continue;
        }

        /*
         * Prepare to convert the column value from text
         */
        if (column_info->column_type != column_type) {
            getTypeInputInfo(column_type, &column_info->typiofunc, &column_info->typioparam);
            fmgr_info_cxt(column_info->typiofunc, &column_info->proc, _state->fn_mcxt);
            column_info->column_type = column_type;
        }
        if (hashentry == NULL || hashentry->isnull) {
            /*
             * need InputFunctionCall to happen even for nulls, so that domain
             * checks are done
             */
            values[i] = InputFunctionCall(&column_info->proc, NULL, column_info->typioparam,
                                          tupdesc->attrs[i].atttypmod);
            nulls[i] = true;
        } else {
            value = hashentry->val;
            values[i] = InputFunctionCall(&column_info->proc, value, column_info->typioparam,
                                          tupdesc->attrs[i].atttypmod);
            nulls[i] = false;
        }
    }

    rettuple = heap_form_tuple(tupdesc, values, nulls);
    tuplestore_puttuple(_state->tuple_store, rettuple);
    hash_destroy(json_hash);
}

static void populate_recordset_array_element_start(void *state, bool isnull)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;

    if (_state->lex->lex_level == 1 && _state->lex->token_type != JSON_TOKEN_OBJECT_START) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must call json_populate_recordset on an array of objects")));
    }
}

static void populate_recordset_array_start(void *state)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;
    if (_state->lex->lex_level != 0 && !_state->use_json_as_text) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("cannot call json_populate_recordset with nested arrays")));
    }
}

static void populate_recordset_scalar(void *state, char *token, JsonTokenType tokentype)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;

    if (_state->lex->lex_level == 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call json_populate_recordset on a scalar")));
    }

    if (_state->lex->lex_level == 2) {
        _state->saved_scalar = token;
    }
}

static void populate_recordset_object_field_start(void *state, char *fname, bool isnull)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;

    if (_state->lex->lex_level > 2) {
        return;
    }

    if (_state->lex->token_type == JSON_TOKEN_ARRAY_START || _state->lex->token_type == JSON_TOKEN_OBJECT_START) {
        if (!_state->use_json_as_text) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("cannot call json_populate_recordset on a nested object")));
        }
        _state->save_json_start = _state->lex->token_start;
    } else {
        _state->save_json_start = NULL;
    }
}

static void populate_recordset_object_field_end(void *state, char *fname, bool isnull)
{
    PopulateRecordsetState *_state = (PopulateRecordsetState *)state;
    JsonHashEntry *hashentry = NULL;
    bool found = false;
    char name[NAMEDATALEN];

    /*
     * ignore field names >= NAMEDATALEN - they can't match a record field
     * ignore nested fields.
     */
    if (_state->lex->lex_level > 2 || strlen(fname) >= NAMEDATALEN) {
        return;
    }

    errno_t rc = memset_s(name, NAMEDATALEN, 0, NAMEDATALEN);
    securec_check(rc, "\0", "\0");
    rc = strncpy_s(name, NAMEDATALEN, fname, NAMEDATALEN - 1);
    securec_check(rc, "\0", "\0");

    hashentry = (JsonHashEntry *)hash_search(_state->json_hash, name, HASH_ENTER, &found);
    /*
     * found being true indicates a duplicate. We don't do anything about
     * that, a later field with the same name overrides the earlier field.
     */
    hashentry->isnull = isnull;
    if (_state->save_json_start != NULL) {
        int len = _state->lex->prev_token_terminator - _state->save_json_start;
        char *val = (char *)palloc((len + 1) * sizeof(char));
        errno_t rc = memcpy_s(val, (len + 1) * sizeof(char), _state->save_json_start, len);
        securec_check(rc, "\0", "\0");
        val[len] = '\0';
        hashentry->val = val;
    } else {
        /* must have had a scalar instead */
        hashentry->val = _state->saved_scalar;
    }
}

/*
 * findJsonbValueFromSuperHeader() wrapper that sets up JsonbValue key string.
 */
static JsonbValue *findJsonbValueFromSuperHeaderLen(JsonbSuperHeader sheader, uint32 flags, char *key, uint32 keylen)
{
    JsonbValue k;

    k.type = jbvString;
    k.string.val = key;
    k.string.len = keylen;

    return findJsonbValueFromSuperHeader(sheader, flags, NULL, &k);
}

#ifdef DOLPHIN
static cJSON *input_to_cjson(Oid valtype, const char *funcName, int pos, Datum arg)
{
    Oid typOutput;
    bool typIsVarlena = false;
    char *data = NULL;
    cJSON *root = NULL;

    if (VALTYPE_IS_JSON(valtype)) {
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
        root = cJSON_ParseWithOpts(data, 0, 1);
        if (!root) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON text in argument %d to function %s.", pos, funcName)));
        }
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument %d to function %s", pos, funcName)));
    }
    return root;
}

static cJSON_ResultWrapper *cJSON_CreateResultWrapper()
{
    cJSON_ResultWrapper *res = (cJSON_ResultWrapper *)palloc(sizeof(cJSON_ResultWrapper));
    cJSON_ResultWrapperInit(res);
    return res;
}

static void cJSON_ResultWrapperInit(cJSON_ResultWrapper *res)
{
    cJSON_ResultNode *resnode = (cJSON_ResultNode *)palloc(sizeof(cJSON_ResultNode));
    resnode->next = NULL;
    resnode->node = NULL;
    res->head = resnode;
    res->len = 0;
}

static cJSON_JsonPath *cJSON_CreateJsonPath(cJSON_JsonPathType type)
{
    cJSON_JsonPath *jp = (cJSON_JsonPath *)palloc(sizeof(cJSON_JsonPath));
    if (jp) {
        jp->next = NULL;
        jp->key = NULL;
        jp->index = 0;
        jp->type = type;
    }
    return jp;
}

static bool cJSON_AddItemToJsonPath(cJSON_JsonPath *jp, cJSON_JsonPath *item)
{
    if ((jp == NULL) || (item == NULL)) {
        return false;
    }
    while (jp->next) {
        jp = jp->next;
    }
    jp->next = item;
    return true;
}

static inline int get_space_skipped_index(const char *data, int start)
{
    int i = start;
    while ((data + i) && *(data + i) && *(data + i) <= 32) {
        i++;
    }
    return i;
}

static inline bool get_end_of_key(const char *data, int start, int &end)
{
    int idx = start;
    bool valid = true;
    if (data[idx] == '"') {
        idx++;
        while (true) {
            switch (data[idx]) {
                case '\\':
                    /*
                    Skip the next character after a backslash. It cannot mark
                    the end of the quoted string.
                    */
                    idx++;
                    switch (data[idx]) {
                        case 'b':
                        case 'n':
                        case 'r':
                        case 't':
                            valid = false;
                        default:
                            break;
                    }
                    break;
                case '"':
                    end = idx + 1;
                    return valid;
                case '\0':
                    end = idx;
                    valid = false;
                    return valid;
                default:
                    break;
            }
            idx++;
        }
    }
    bool first = true;
    while (data[idx] != '\0' && data[idx] != '*' && data[idx] != '.' && data[idx] != '[' && data[idx] > 32) {
        if (first) {
            if (!(isalpha(data[idx]) || data[idx] == '_' || data[idx] == '$')) {
                valid = false;
            }
            first = false;
        } else {
            if (!isalpha(data[idx]) && data[idx] != '_' && data[idx] != '$' && (data[idx] > '9' || data[idx] < '0')) {
                valid = false;
            }
        }
        idx++;
    }
    end = idx;
    return valid;
}

static cJSON_JsonPath *jp_parse(const char *data, int &error_pos)
{
    cJSON_JsonPath *jp = NULL;
    cJSON_JsonPath *tmp = NULL;

    int i = get_space_skipped_index(data, 0);
    if (data[i] && data[i++] == '$') {
        jp = cJSON_CreateJsonPath(cJSON_JsonPath_Start);
        i = get_space_skipped_index(data, i);
    } else {
        error_pos = i;
        return NULL;
    }
    while (data[i] != '\0') {
        tmp = NULL;
        switch (data[i]) {
            case '.':
                tmp = jp_parse_key(data, &i);
                break;
            case '[':
                tmp = jp_parse_index(data, &i);
                break;
            case '*':
                tmp = jp_parse_any(data, &i);
                break;
        }
        if (tmp != NULL) {
            cJSON_AddItemToJsonPath(jp, tmp);
        } else {
            error_pos = i;
            cJSON_DeleteJsonPath(jp);
            return NULL;
        }
        i = get_space_skipped_index(data, i);
    }
    return jp;
}

static cJSON_JsonPath *jp_parse_key(const char *data, int *idx)
{
    (*idx)++;  // advance past the .
    cJSON_JsonPath *item = NULL;
    *idx = get_space_skipped_index(data, *idx);
    if (data[*idx] == '\0') {
        return NULL;
    }
    if (data[*idx] == '*') {
        *idx = *idx + 1;
        item = cJSON_CreateJsonPath(cJSON_JsonPath_AnyKey);
    } else {
        int start = *idx;
        int end = start;
        int len = 0;
        errno_t rc;
        bool error = get_end_of_key(data, start, end);
        *idx = end;
        if (!error) {
            return NULL;
        }
        if (data[start] == '"') {
            start += 1;
            end -= 1;
        }
        len = end - start;
        if (len <= 0) {
            return NULL;
        }
        item = cJSON_CreateJsonPath(cJSON_JsonPath_Key);
        item->key = (char *)palloc(sizeof(char) * (len + 1));
        rc = strncpy_s(item->key, len + 1, data + start, len);
        item->key[len] = '\0';
    }
    return item;
}

static cJSON_JsonPath *jp_parse_index(const char *data, int *idx)
{
    (*idx)++;  // advance past the [
    *idx = get_space_skipped_index(data, *idx);
    cJSON_JsonPath *item = NULL;
    if (data[*idx] == '*') {
        (*idx)++;
        item = cJSON_CreateJsonPath(cJSON_JsonPath_AnyIndex);
    } else {
        int num_start = *idx;
        while (data[*idx] >= '0' && data[*idx] <= '9') {
            (*idx)++;
        }
        if (*idx == num_start) {
            return NULL;
        }
        long num = 0;
        num = strtol(data + num_start, NULL, 10);
        if (num > LONG_MAX) {
            cJSON_DeleteJsonPath(item);
            return NULL;
        }
        item = cJSON_CreateJsonPath(cJSON_JsonPath_Index);
        item->index = (int)num;
    }
    *idx = get_space_skipped_index(data, *idx);
    if (data[*idx] != '\0' && data[(*idx)++] == ']') {
        return item;
    }
    cJSON_DeleteJsonPath(item);
    return NULL;
}

static cJSON_JsonPath *jp_parse_any(const char *data, int *idx)
{
    (*idx)++;  // advance past the first *
    cJSON_JsonPath *item = NULL;
    // must followed by a *
    if (data[*idx] != '*') {
        return NULL;
    }
    (*idx)++;
    // can't be last and ***
    if (data[*idx] == 0 || data[*idx] == '*') {
        return NULL;
    }
    item = cJSON_CreateJsonPath(cJSON_JsonPath_Any);
    return item;
}

static bool jp_match(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res)
{
    int old_len = res->len;
    if (!item || !jp) {
        return false;
    }
    while (jp) {
        switch (jp->type) {
            case cJSON_JsonPath_Start:
                if (jp->next == NULL) {
                    cJSON_AddItemToResultWrapper(res, item);
                }
                jp = jp->next;
                break;
            case cJSON_JsonPath_Key:
                item = jp_match_object(item, jp, res);
                jp = jp->next;
                break;
            case cJSON_JsonPath_Index:
                item = jp_match_array(item, jp, res);
                jp = jp->next;
                break;
            case cJSON_JsonPath_AnyKey:
                jp_match_any(item, jp, res, 1);
                jp = NULL;
                break;
            case cJSON_JsonPath_AnyIndex:
                jp_match_any(item, jp, res, 2);
                jp = NULL;
                break;
            case cJSON_JsonPath_Any:
                jp_match(item, jp->next, res);
                jp_match_any(item, jp, res, 3);
                jp = NULL;
                break;
            default:
                break;
        }
    }
    return (res->len > old_len) ? true : false;
}

static cJSON *jp_match_object(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res)
{
    bool is_last = false;
    if (!jp->next) {
        is_last = true;
    }
    cJSON *tmp = NULL;
    tmp = cJSON_GetObjectItem(item, jp->key);
    if (is_last) {
        cJSON_AddItemToResultWrapper(res, tmp);
    }
    return tmp;
}

static cJSON *jp_match_array(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res)
{
    bool is_last = false;
    if (!jp->next) {
        is_last = true;
    }
    cJSON *tmp = NULL;
    if (cJSON_IsArray(item)) {
        tmp = cJSON_GetArrayItem(item, jp->index);
    } else {
        if (jp->index == 0) {
            tmp = item;
        }
    }
    if (is_last) {
        cJSON_AddItemToResultWrapper(res, tmp);
    }
    return tmp;
}

static void jp_match_any(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res, int mode)
{
    cJSON *ele = NULL;
    bool is_last = (jp->next == NULL);
    if (mode == 1 && !cJSON_IsObject(item)) {
        return;
    } else if (mode == 2 && !cJSON_IsArray(item)) {
        return;
    }
    cJSON_ArrayForEach(ele, item)
    {
        if (is_last && mode != 3) {
            cJSON_AddItemToResultWrapper(res, ele);
        }
        jp_match(ele, jp->next, res);
        if (mode == 3) {
            jp_match_any(ele, jp, res, mode);
        }
    }
}

static bool cJSON_JsonPathMatch(cJSON *item, cJSON_JsonPath *head, cJSON_ResultWrapper *res)
{
    if ((head == NULL) || (item == NULL) || (head->type != cJSON_JsonPath_Start)) {
        return false;
    }
    return jp_match(item, head, res);
}

static bool cJSON_AddItemToResultWrapper(cJSON_ResultWrapper *res, cJSON *item)
{
    if (!item || !res) {
        return false;
    }
    cJSON_ResultNode *tmp = (cJSON_ResultNode *)palloc(sizeof(cJSON_ResultNode));
    cJSON_ResultNode *head = res->head;
    tmp->node = item;
    while (head->next) {
        head = head->next;
        if (head->node == item) {
            return false;
        }
    }
    tmp->next = head->next;
    head->next = tmp;
    res->len++;
    return true;
}

static bool cJSON_JsonPathCanMatchMany(cJSON_JsonPath *jp)
{
    bool many = false;
    if (jp != NULL) {
        while (jp) {
            if (jp->type == cJSON_JsonPath_Any || jp->type == cJSON_JsonPath_AnyIndex ||
                jp->type == cJSON_JsonPath_AnyKey) {
                many = true;
                break;
            }
            jp = jp->next;
        }
    }
    return many;
}

static void cJSON_DeleteJsonPath(cJSON_JsonPath *jp)
{
    cJSON_JsonPath *next = NULL;
    while (jp) {
        next = jp->next;
        if (jp->key) {
            pfree(jp->key);
        }
        pfree(jp);
        jp = next;
    }
}

static void cJSON_DeleteResultWrapper(cJSON_ResultWrapper *res)
{
    cJSON_ResultNode *head = res->head;
    cJSON_ResultNode *next = NULL;

    while (head) {
        next = head->next;
        pfree(head);
        head = next;
    }

    pfree(res);
}

static void quicksort(cJSON *start, cJSON *end)
{
    if (start == NULL || start == end)
        return;
    cJSON *q = end;
    cJSON *p = start;
    while (q != p) {
        if ((strlen(start->string) > strlen(q->string) ||
             (strcmp(start->string, q->string) > 0 && strlen(q->string) == strlen(start->string)))) {
            p = p->next;
            cJSON_SwapItemValue(p, q);
        } else {
            q = q->prev;
        }
    }
    cJSON_SwapItemValue(p, start);
    quicksort(start, p);
    quicksort(p->next, end);
}

static text *formatJsondoc(char *str)
{
    StringInfo buf = makeStringInfo();
    bool quoted = false;
    text *res;
    while (*str != '\0') {
        appendStringInfoChar(buf, *str);
        switch (*str++) {
            case '\"':
                quoted = !quoted;
                break;
            case ':':
            case ',':
                if (!quoted) {
                    appendStringInfoChar(buf, ' ');
                }
                break;
            case '\\':
                appendStringInfoChar(buf, *str++);
                break;
        }
    }
    res = cstring_to_text_with_len(buf->data, buf->len);
    pfree(buf->data);
    pfree(buf);
    return res;
}

static inline cJSON_JsonPath *jp_pop(cJSON_JsonPath *jp)
{
    cJSON_JsonPath *last = NULL;
    if (!jp) {
        return NULL;
    }
    while (jp->next && jp->next->next) {
        jp = jp->next;
    }
    last = jp->next;
    jp->next = NULL;
    return last;
}

static void cJSON_SortObject(cJSON *object)
{
    cJSON *start = NULL;
    cJSON *end = NULL;
    cJSON *child = NULL;
    if (object->type == cJSON_Object) {
        start = object->child;
        if (!start) {
            return;
        }
        end = object->child->prev;
        quicksort(start, end);
    }
    cJSON_ArrayForEach(child, object)
    {
        cJSON_SortObject(child);
    }
}

static cJSON *cJSON_ResultWrapperToArray(cJSON_ResultWrapper *res)
{
    cJSON *array = cJSON_CreateArray();
    cJSON_ResultNode *resnode = res->head->next;
    while (resnode) {
        cJSON *item = cJSON_Duplicate(resnode->node, true);
        cJSON_AddItemToArray(array, item);
        resnode = resnode->next;
    }
    return array;
}

static void cJSON_SwapItemValue(cJSON *item1, cJSON *item2)
{
    if (item1 == NULL || item2 == NULL) {
        return;
    }

    cJSON tmp;
    errno_t rc = memset_s(&tmp, sizeof(tmp), 0, sizeof(tmp));
    securec_check(rc, "\0", "\0");
    tmp.child = item1->child;
    tmp.type = item1->type;
    tmp.valuestring = item1->valuestring;
    tmp.valueint = item1->valueint;
    tmp.valuedouble = item1->valuedouble;
    tmp.string = item1->string;

    item1->child = item2->child;
    item1->type = item2->type;
    item1->valuestring = item2->valuestring;
    item1->valueint = item2->valueint;
    item1->valuedouble = item2->valuedouble;
    item1->string = item2->string;

    item2->child = tmp.child;
    item2->type = tmp.type;
    item2->valuestring = tmp.valuestring;
    item2->valueint = tmp.valueint;
    item2->valuedouble = tmp.valuedouble;
    item2->string = tmp.string;
}

static cJSON_bool cJSON_ArrayAppend(cJSON *root, cJSON_JsonPath *jp, cJSON *value)
{
    if (!root || !jp || !value) {
        return false;
    }
    if (cJSON_JsonPathCanMatchMany(jp)) {
        return false;  // can't use * and **
    }
    cJSON_ResultWrapper *w = cJSON_CreateResultWrapper();
    if (!cJSON_JsonPathMatch(root, jp, w)) {
        cJSON_DeleteResultWrapper(w);
        return false;  // not exists
    }
    cJSON_JsonPath *last = jp_pop(jp);
    cJSON *found = w->head->next->node;
    if (cJSON_IsArray(found)) {
        cJSON_AddItemToArray(found, value);
    } else if (cJSON_IsObject(found)) {
        cJSON *n = cJSON_Duplicate(found, false);
        n->child = found->child;
        n->next = NULL;
        n->prev = NULL;
        found->type = cJSON_Array;
        found->child = NULL;
        cJSON_AddItemToArray(found, n);
        cJSON_AddItemToArray(found, value);
    } else if (last->index >= 0) {
        // found a scalar or object and the index is over 0, auto wrap it
        cJSON *n = cJSON_Duplicate(found, false);
        n->child = found->child;
        n->next = NULL;
        n->prev = NULL;
        found->type = cJSON_Array;
        found->child = NULL;
        cJSON_AddItemToArray(found, n);
        cJSON_AddItemToArray(found, value);
    }

    cJSON_DeleteJsonPath(last);
    cJSON_DeleteResultWrapper(w);
    return true;
}

static cJSON_bool cJSON_JsonInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value)
{
    if (!root || !jp || !value) {
        return false;
    }
    if (cJSON_JsonPathCanMatchMany(jp)) {
        return false;  // can't use * and **
    }
    cJSON_ResultWrapper *w = cJSON_CreateResultWrapper();
    if (cJSON_JsonPathMatch(root, jp, w)) {
        cJSON_DeleteResultWrapper(w);
        return false;  // Already exists
    }
    cJSON_JsonPath *last = jp_pop(jp);  // remove the last path node, then search again
    if (!cJSON_JsonPathMatch(root, jp, w)) {
        // we find nothing in the parent path
        cJSON_DeleteResultWrapper(w);
        cJSON_DeleteJsonPath(last);
        return false;
    }
    cJSON *found = w->head->next->node;
    if (last->type == cJSON_JsonPath_Index) {
        if (cJSON_IsArray(found)) {
            cJSON_AddItemToArray(found, value);
        } else if (last->index > 0) {
            // found a scalar or object and the index is not 0, auto wrap it
            cJSON *n = cJSON_Duplicate(found, false);
            n->child = found->child;
            n->next = NULL;
            n->prev = NULL;
            found->type = cJSON_Array;
            found->child = NULL;
            cJSON_AddItemToArray(found, n);
            cJSON_AddItemToArray(found, value);
        }
    } else if (last->type == cJSON_JsonPath_Key && cJSON_IsObject(found)) {
        cJSON_AddItemToObject(found, last->key, value);
    }
    cJSON_DeleteJsonPath(last);
    cJSON_DeleteResultWrapper(w);
    return true;
}

static bool cJSON_JsonReplace(cJSON *root, cJSON_JsonPath *jp, cJSON *value, bool &invalidPath)
{
    if (!root || !jp || !value) {
        return false;
    }
    if (cJSON_JsonPathCanMatchMany(jp)) {
        invalidPath = true;
        return false;  // can't use * and **
    }
    cJSON_ResultWrapper *w = cJSON_CreateResultWrapper();
    if (!cJSON_JsonPathMatch(root, jp, w)) {
        cJSON_DeleteResultWrapper(w);
        return false;  // not exists
    }
    cJSON_DeleteResultWrapper(w);
    w = cJSON_CreateResultWrapper();
    cJSON_JsonPath *last = jp_pop(jp);  // remove the last path node, then search again
    if (!cJSON_JsonPathMatch(root, jp, w)) {
        // we find nothing in the parent path
        cJSON_DeleteResultWrapper(w);
        if (last) {
            cJSON_DeleteJsonPath(last);
        }
        return false;
    }
    cJSON *found = w->head->next->node;
    if (last->type == cJSON_JsonPath_Index) {
        if (cJSON_IsArray(found) && last->index >= 0) {
            cJSON_ReplaceItemInArray(found, last->index, value);
        } else {
            cJSON_DeleteResultWrapper(w);
            if (last) {
                cJSON_DeleteJsonPath(last);
            }
            return false;
        }
    } else if (last->type == cJSON_JsonPath_Key && cJSON_IsObject(found)) {
        cJSON_ReplaceItemInObject(found, last->key, value);
    }
    cJSON_DeleteJsonPath(last);
    cJSON_DeleteResultWrapper(w);
    return true;
}

static bool cJSON_JsonRemove(cJSON *root, cJSON_JsonPath *jp, bool *invalidPath)
{
    if (!root || !jp) {
        return false;
    }
    if (cJSON_JsonPathCanMatchMany(jp)) {
        *invalidPath = true;
        return false;  // can't use * and **
    }
    cJSON_ResultWrapper *w = cJSON_CreateResultWrapper();
    if (!cJSON_JsonPathMatch(root, jp, w)) {
        cJSON_DeleteResultWrapper(w);
        return false;  // not exists
    }

    cJSON_DeleteResultWrapper(w);
    w = cJSON_CreateResultWrapper();
    cJSON_JsonPath *last = jp_pop(jp);

    if (!cJSON_JsonPathMatch(root, jp, w)) {
        cJSON_DeleteResultWrapper(w);
        // we find nothing in the parent path
        return false;
    }
    cJSON *found = w->head->next->node;
    if (last->type == cJSON_JsonPath_Index) {
        if (cJSON_IsArray(found) && last->index >= 0) {
            cJSON_DeleteItemFromArray(found, last->index);
        } else {
            // found a scalar or object and the index is not 0, auto wrap it
            cJSON_DeleteResultWrapper(w);

            if (last) {
                cJSON_DeleteJsonPath(last);
            }
            return false;
        }
    } else if (last->type == cJSON_JsonPath_Key && cJSON_IsObject(found)) {
        cJSON_DeleteItemFromObject(found, last->key);
    }
    cJSON_DeleteResultWrapper(w);

    if (last) {
        cJSON_DeleteJsonPath(last);
    }
    return true;
}

static bool cJSON_JsonArrayInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value, bool *invlidPath, bool *isArray)
{
    if (!root || !jp || !value) {
        return false;
    }
    if (cJSON_JsonPathCanMatchMany(jp)) {
        *invlidPath = true;
        return false;  // can't use * and **
    }
    cJSON_ResultWrapper *w = cJSON_CreateResultWrapper();
    cJSON_JsonPath *last = jp_pop(jp);  // remove the last path node, then search again
    if (!cJSON_JsonPathMatch(root, jp, w)) {
        // we find nothing in the parent path
        cJSON_DeleteResultWrapper(w);
        if (last) {
            cJSON_DeleteJsonPath(last);
        }
        return false;
    }
    cJSON *found = w->head->next->node;
    if (last->type == cJSON_JsonPath_Index) {
        if (cJSON_IsArray(found)) {
            cJSON_InsertItemInArray(found, last->index, value);
        } else if (last->index > 0) {
            // found a scalar or object and the index is not 0, auto wrap it
            cJSON *n = cJSON_Duplicate(found, false);
            n->child = found->child;
            n->next = NULL;
            n->prev = NULL;
            found->type = cJSON_Array;
            found->child = NULL;
            cJSON_AddItemToArray(found, n);
            cJSON_AddItemToArray(found, value);
        }
    } else {
        cJSON_DeleteResultWrapper(w);
        if (last) {
            cJSON_DeleteJsonPath(last);
        }
        *isArray = true;
        return false;
    }
    cJSON_DeleteResultWrapper(w);
    if (last) {
        cJSON_DeleteJsonPath(last);
    }
    return true;
}

static bool json_contains_unit(const cJSON *const target, const cJSON *const candidate)
{
    int tType = target->type;
    int cType = candidate->type;

    switch (tType) {
        case cJSON_Object: {
            switch (cType) {
                case cJSON_Object: {
                    int k;
                    int l;

                    char **candKeys = (char **)palloc(cJSON_GetArraySize(candidate) * sizeof(char *));
                    char **targKeys = (char **)palloc(cJSON_GetArraySize(target) * sizeof(char *));

                    int candKeysNum = 0;
                    cJSON *node = NULL;
                    node = candidate->child;
                    while (node != NULL) {
                        candKeys[candKeysNum] = node->string;
                        node = node->next;
                        candKeysNum++;
                    }
                    int targKeysNum = 0;
                    node = target->child;
                    while (node != NULL) {
                        targKeys[targKeysNum] = node->string;
                        node = node->next;
                        targKeysNum++;
                    }

                    for (l = 0; l < candKeysNum; l++) {
                        bool flag = false;
                        for (k = 0; k < targKeysNum; k++) {
                            if (strcmp(candKeys[l], targKeys[k]) == 0) {
                                cJSON *tResult = cJSON_GetObjectItem(target, targKeys[k]);
                                cJSON *cResult = cJSON_GetObjectItem(candidate, candKeys[l]);
                                if (json_contains_unit(tResult, cResult)) {
                                    flag = true;
                                }
                            }
                        }
                        if (!flag) {
                            break;
                        }
                    }

                    pfree(candKeys);
                    pfree(targKeys);

                    if (l == cJSON_GetArraySize(candidate)) {
                        return true;
                    }

                    break;
                }
                case cJSON_Array:
                case cJSON_String:
                case cJSON_Number:
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL: {
                    return false;
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", cType);
            }

            break;
        }
        case cJSON_Array: {
            switch (cType) {
                case cJSON_Array: {
                    int arr_arr_cand_iter = 0;
                    cJSON *candidateEle = NULL;
                    while (true) {
                        candidateEle = cJSON_GetArrayItem(candidate, arr_arr_cand_iter);
                        if (candidateEle == NULL) {
                            return true;
                        }
                        int arr_arr_targ_iter = 0;
                        bool flag = false;
                        cJSON *targetEle = NULL;
                        while (true) {
                            targetEle = cJSON_GetArrayItem(target, arr_arr_targ_iter);
                            if (targetEle == NULL)
                                break;
                            if (json_contains_unit(targetEle, candidateEle)) {
                                flag = true;
                                break;
                            }
                            arr_arr_targ_iter++;
                        }
                        if (!flag)
                            break;
                        arr_arr_cand_iter++;
                    }

                    break;
                }
                case cJSON_Object:
                case cJSON_String:
                case cJSON_Number:
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL: {
                    int arr_scalar_targ_iter = 0;
                    while (true) {
                        cJSON *result = cJSON_GetArrayItem(target, arr_scalar_targ_iter);
                        if (result == NULL)
                            break;
                        if (json_contains_unit(result, candidate))
                            return true;
                        arr_scalar_targ_iter++;
                    }
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", cType);
            }

            break;
        }
        case cJSON_String:
        case cJSON_Number:
        case cJSON_True:
        case cJSON_False:
        case cJSON_NULL: {
            switch (cType) {
                case cJSON_Object:
                case cJSON_Array: {
                    break;
                }
                case cJSON_String:
                case cJSON_Number:
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL: {
                    if (cJSON_Compare(target, candidate, 1)) {
                        return true;
                    }
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", cType);
            }

            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", tType);
    }
    return false;
}

static int containsAsterisk(const char *const path)
{
    int i = 0;
    bool flag = false;
    while (path[i] != '\0') {
        if (path[i] == '\"')
            flag = !flag;
        if (path[i] == '*' && !flag)
            return i;
        i++;
    }
    return -1;
}

Datum json_contains(PG_FUNCTION_ARGS)
{
    char *path = NULL;
    cJSON_JsonPath *jp = NULL;
    int error_pos = -1;
    bool resBool;
    Oid valtype;
    Datum arg = 0;
    cJSON *target_cJSON = NULL;
    cJSON *candidate_cJSON = NULL;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    target_cJSON = input_to_cjson(valtype, "json_contains", 1, arg);

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 1);
    arg = PG_GETARG_DATUM(1);
    candidate_cJSON = input_to_cjson(valtype, "json_contains", 2, arg);

    if (PG_NARGS() == 2) {
        resBool = json_contains_unit(target_cJSON, candidate_cJSON);
        cJSON_Delete(target_cJSON);
        cJSON_Delete(candidate_cJSON);
        if (resBool)
            PG_RETURN_BOOL(true);
    } else {
        cJSON *result = NULL;
        path = TextDatumGetCString(PG_GETARG_DATUM(2));

        if (containsAsterisk(path) > 0) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("in this situation, path expressions may not contain the * and ** tokens")));
        }

        cJSON_ResultWrapper *res = cJSON_CreateResultWrapper();
        jp = jp_parse(path, error_pos);

        if (!jp) {
            cJSON_DeleteJsonPath(jp);
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(target_cJSON);
            cJSON_Delete(candidate_cJSON);
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("Invalid JSON path expression. The error is around character position %d.", error_pos)));
        }
        cJSON_JsonPathMatch(target_cJSON, jp, res);
        cJSON_DeleteJsonPath(jp);
        if (res->len == 1) {
            result = res->head->next->node;
        }
        cJSON_DeleteResultWrapper(res);
        if (result == NULL) {
            PG_RETURN_NULL();
        }
        resBool = json_contains_unit(result, candidate_cJSON);
        cJSON_Delete(target_cJSON);
        cJSON_Delete(candidate_cJSON);

        PG_RETURN_BOOL(resBool);
    }

    PG_RETURN_BOOL(false);
}

Datum json_contains_path(PG_FUNCTION_ARGS)
{
    char *mode = text_to_cstring(PG_GETARG_TEXT_P(1));
    ArrayType *path_array = PG_GETARG_ARRAYTYPE_P(2);

    Datum *pathtext = NULL;
    bool *pathnulls = NULL;
    int path_num;

    deconstruct_array(path_array, TEXTOID, -1, false, 'i', &pathtext, &pathnulls, &path_num);

    /*
     * If the array is empty, return NULL; this is dubious but it's what 9.3
     * did.
     */
    if (path_num <= 0)
        PG_RETURN_NULL();

    bool flag;
    char *path = NULL;
    Oid valtype;
    Datum arg = 0;
    cJSON_ResultWrapper *res = cJSON_CreateResultWrapper();
    cJSON_JsonPath *jp = NULL;
    cJSON *root = NULL;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_contains_path", 1, arg);

    int error_pos = -1;
    int last_len = 0;

    if (strcmp(mode, "one") == 0) {
        for (int i = 0; i < path_num; i++) {
            if (pathnulls[i]) {
                cJSON_Delete(root);
                cJSON_DeleteResultWrapper(res);
                PG_RETURN_NULL();
            }
            path = TextDatumGetCString(pathtext[i]);
            jp = jp_parse(path, error_pos);

            if (!jp) {
                cJSON_DeleteJsonPath(jp);
                cJSON_DeleteResultWrapper(res);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("Invalid JSON path expression. The error is around character position %d.",
                                       error_pos)));
            }

            cJSON_JsonPathMatch(root, jp, res);
            cJSON_DeleteJsonPath(jp);

            if (res->len > last_len) {
                cJSON_Delete(root);
                cJSON_DeleteResultWrapper(res);
                PG_RETURN_BOOL(true);
            }

            last_len = res->len;
        }

        cJSON_Delete(root);
        cJSON_DeleteResultWrapper(res);

        PG_RETURN_BOOL(false);

    } else if (strcmp(mode, "all") == 0) {
        flag = true;

        for (int i = 0; i < path_num; i++) {
            if (pathnulls[i]) {
                cJSON_Delete(root);
                cJSON_DeleteResultWrapper(res);
                PG_RETURN_NULL();
            }
            path = TextDatumGetCString(pathtext[i]);
            jp = jp_parse(path, error_pos);

            if (!jp) {
                cJSON_DeleteJsonPath(jp);
                cJSON_DeleteResultWrapper(res);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("Invalid JSON path expression. The error is around character position %d.",
                                       error_pos)));
            }

            cJSON_JsonPathMatch(root, jp, res);
            cJSON_DeleteJsonPath(jp);

            if (res->len <= last_len) {
                flag = false;
                break;
            }

            last_len = res->len;
        }

        cJSON_Delete(root);
        cJSON_DeleteResultWrapper(res);

        PG_RETURN_BOOL(flag);

    } else {
        cJSON_DeleteResultWrapper(res);
        cJSON_Delete(root);
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("the oneOrAll argument to json_contains_path may take these values: 'one' or 'all'")));
    }

    PG_RETURN_NULL();
}

Datum json_extract(PG_FUNCTION_ARGS)
{
    ArrayType *in_array = PG_GETARG_ARRAYTYPE_P(1);
    text *result = NULL;
    Datum *in_datums = NULL;
    bool *in_nulls = NULL;
    int in_count;
    char *path = NULL;
    cJSON_ResultWrapper *res = NULL;
    cJSON_JsonPath *jp = NULL;
    cJSON *root = NULL;
    int error_pos = -1;
    char *r = NULL;
    bool many = false;
    Oid valtype;
    Datum arg = 0;

    if (array_contains_nulls(in_array)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call function with null path elements")));
    }

    deconstruct_array(in_array, TEXTOID, -1, false, 'i', &in_datums, &in_nulls, &in_count);

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_extract", 1, arg);

    cJSON_SortObject(root);
    res = cJSON_CreateResultWrapper();
    for (int i = 0; i < in_count; i++) {
        path = TextDatumGetCString(in_datums[i]);
        jp = jp_parse(path, error_pos);
        if (!jp) {
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(root);
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("Invalid JSON path expression. The error is around character position %d.", error_pos)));
        }
        many |= cJSON_JsonPathCanMatchMany(jp);
        cJSON_JsonPathMatch(root, jp, res);
        cJSON_DeleteJsonPath(jp);
    }
    if (res->len > 0) {
        if (many) {
            cJSON *arr = cJSON_ResultWrapperToArray(res);
            r = cJSON_PrintUnformatted(arr);
            result = formatJsondoc(r);
            cJSON_Delete(arr);
        } else {
            r = cJSON_PrintUnformatted(res->head->next->node);
            result = formatJsondoc(r);
        }
        pfree(r);
    } else {
        PG_RETURN_NULL();
    }
    cJSON_Delete(root);
    cJSON_DeleteResultWrapper(res);
    PG_RETURN_TEXT_P(result);
}

static void search_PushStack(search_LinkStack &s, ElemType x)
{
    LinkNode *newnode = (LinkNode *)palloc(sizeof(LinkNode));
    newnode->data = x;
    newnode->next = s;
    s = newnode;
}

static void search_PopStack(search_LinkStack &s)
{
    LinkNode *p = s;
    s = s->next;
    pfree(p);
}

static search_LinkStack search_ReverseStack(search_LinkStack &s)
{
    search_LinkStack reverse_stack = NULL;
    while (s != NULL) {
        search_PushStack(reverse_stack, s->data);
        search_PopStack(s);
    }
    return reverse_stack;
}

static void search_CleanStack(search_LinkStack &s)
{
    while (s != NULL) {
        search_PopStack(s);
    }
}

static cJSON *jp_match_object_record(cJSON *doc_cJSON, char *path, int wildchar_position, bool &type_flag,
                                     StringInfo &position, StringInfo &wildchar, int &i)
{
    int start;
    int end = 0;
    char *path_part;
    errno_t rc;
    start = get_space_skipped_index(path, ++i);
    i = start;
    while (path[i] != '\0' && path[i] != '.' && path[i] != '[' && path[i] != ' ') {
        if (path[i] == '*') {
            type_flag = false;
            return doc_cJSON;
        }
        if (path[i] == '"')
            return doc_cJSON;
        ++i;
    }
    end = i;
    path_part = (char *)palloc((end - start + 1) * sizeof(char));
    rc = strncpy_s(path_part, end - start + 1, path + start, end - start);
    securec_check(rc, "\0", "\0");
    if (wildchar_position > 0) {
        appendStringInfo(wildchar, ".%s", path_part);
        if (end < wildchar_position) {
            appendStringInfo(position, ".%s", path_part);
            doc_cJSON = cJSON_GetObjectItem(doc_cJSON, path_part);
        }
    } else {
        appendStringInfo(position, ".%s", path_part);
        doc_cJSON = cJSON_GetObjectItem(doc_cJSON, path_part);
    }
    pfree(path_part);
    return doc_cJSON;
}

static cJSON *jp_match_array_record(cJSON *doc_cJSON, char *path, int wildchar_position, bool &type_flag,
                                    StringInfo &position, StringInfo &wildchar, int &i)
{
    int start = 0;
    int end = 0;
    char *path_part;
    errno_t rc;
    if ((doc_cJSON->type & 0xFF) != cJSON_Array && wildchar_position > i) {
        return NULL;
    }
    start = get_space_skipped_index(path, ++i);
    i = start;
    while (path[i] != ']' && path[i] != ' ') {
        if (path[i] == '*') {
            type_flag = true;
            return doc_cJSON;
        }
        ++i;
    }
    end = i;
    path_part = (char *)palloc((end - start + 1) * sizeof(char));
    rc = strncpy_s(path_part, end - start + 1, path + start, end - start);
    securec_check(rc, "\0", "\0");
    if (wildchar_position > 0) {
        appendStringInfo(wildchar, "[%s]", path_part);
        if (end < wildchar_position) {
            appendStringInfo(position, "[%s]", path_part);
            doc_cJSON = cJSON_GetArrayItem(doc_cJSON, pg_atoi(path_part, sizeof(int32), 0));
        }
    } else {
        appendStringInfo(position, "[%s]", path_part);
        doc_cJSON = cJSON_GetArrayItem(doc_cJSON, pg_atoi(path_part, sizeof(int32), 0));
    }
    ++i;
    pfree(path_part);
    return doc_cJSON;
}

static cJSON *jp_match_object_quote_record(cJSON *doc_cJSON, char *path, int wildchar_position, StringInfo &position,
                                           StringInfo &wildchar, int &i)
{
    int start = ++i;
    bool quote_flag = false;
    char *path_part;
    errno_t rc;
    if (!isalpha(path[i++])) {
        quote_flag = true;
    }
    while (path[i] != '"') {
        if (!isalpha(path[i]) && !isdigit(path[i]) && path[i] != '_' && path[i] != '$')
            quote_flag = true;
        ++i;
    }
    path_part = (char *)palloc((i - start + 1) * sizeof(char));
    rc = strncpy_s(path_part, i - start + 1, path + start, i - start);
    securec_check(rc, "\0", "\0");
    if (wildchar_position > 0) {
        if (quote_flag) {
            appendStringInfo(wildchar, "._\"%s_\"", path_part);
            if (i < wildchar_position) {
                appendStringInfo(position, ".\\\"%s\\\"", path_part);
                doc_cJSON = cJSON_GetObjectItem(doc_cJSON, path_part);
            }
        } else {
            appendStringInfo(wildchar, ".%s", path_part);
            if (i < wildchar_position) {
                appendStringInfo(position, ".%s", path_part);
                doc_cJSON = cJSON_GetObjectItem(doc_cJSON, path_part);
            }
        }
    } else {
        if (quote_flag) {
            appendStringInfo(position, ".\\\"%s\\\"", path_part);
        } else {
            appendStringInfo(position, ".%s", path_part);
        }
        doc_cJSON = cJSON_GetObjectItem(doc_cJSON, path_part);
    }
    ++i;
    pfree(path_part);
    return doc_cJSON;
}

static cJSON *jp_match_record(cJSON *doc_cJSON, char *path, StringInfo &position, StringInfo &wildchar)
{
    int error_pos = -1;
    int i = 0;
    int wildchar_position = containsAsterisk(path);
    bool type_flag = false;
    cJSON_JsonPath *jp = NULL;
    jp = jp_parse(path, error_pos);
    if (!jp) {
        cJSON_DeleteJsonPath(jp);
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON path expression. The error is around character position %d.", error_pos)));
    }

    appendStringInfoString(position, "\"$");
    if (wildchar_position > 0) {
        appendStringInfoString(wildchar, "\"$");
    }

    while (path[i] != '\0') {
        switch (path[i]) {
            case '.': {
                doc_cJSON =
                    jp_match_object_record(doc_cJSON, path, wildchar_position, type_flag, position, wildchar, i);
                break;
            }
            case '[': {
                doc_cJSON = jp_match_array_record(doc_cJSON, path, wildchar_position, type_flag, position, wildchar, i);
                break;
            }
            case '"': {
                doc_cJSON = jp_match_object_quote_record(doc_cJSON, path, wildchar_position, position, wildchar, i);
                break;
            }
            case '*': {
                if (path[i + 1] == '*') {
                    ++i;
                    appendStringInfoChar(wildchar, '%');
                } else {
                    if (type_flag) {
                        appendStringInfoString(wildchar, "[_]");
                    } else {
                        appendStringInfoString(wildchar, "._");
                    }
                }
                ++i;
                break;
            }
            default: {
                ++i;
                break;
            }
        }
        if (doc_cJSON == NULL) {
            break;
        }
    }
    if (wildchar_position > 0)
        appendStringInfoChar(wildchar, '%');
    return doc_cJSON;
}

static bool json_search_unit(const cJSON *doc_cJSON, const text *search_text, bool mode_match, char *wildchar,
                             char *last_position, search_LinkStack &stk)
{
    int Type = (doc_cJSON->type & 0xFF);
    switch (Type) {
        case cJSON_Object: {
            bool flag = false;
            int num = 0;
            cJSON *node = doc_cJSON->child;
            char **keys = (char **)palloc(cJSON_GetArraySize(doc_cJSON) * sizeof(char *));
            while (node != NULL) {
                keys[num] = node->string;
                node = node->next;
                num++;
            }
            StringInfo position = makeStringInfo();
            for (int k = 0; k < num; k++) {
                cJSON *result = cJSON_GetObjectItem(doc_cJSON, keys[k]);
                int keyLen = strlen(keys[k]);
                bool alpha_flag = true;
                appendStringInfoString(position, last_position);
                if (!isalpha(keys[k][0]) && keyLen > 0) {
                    alpha_flag = false;
                }
                for (int i = 1; i < keyLen; i++) {
                    if (!isalpha(keys[k][i]) && !isdigit(keys[k][i]) && keys[k][i] != '_' && keys[k][i] != '$') {
                        alpha_flag = false;
                        break;
                    }
                }
                if (alpha_flag) {
                    appendStringInfo(position, ".%s", keys[k]);
                } else {
                    appendStringInfo(position, ".\\\"%s\\\"", keys[k]);
                }
                if (json_search_unit(result, search_text, mode_match, wildchar, position->data, stk)) {
                    if (mode_match) {
                        return true;
                    } else {
                        if ((result->type & 0xFF) == cJSON_String) {
                            search_PushStack(stk, ",");
                        }
                        flag = true;
                    }
                }
                resetStringInfo(position);
            }
            DestroyStringInfo(position);
            pfree(keys);
            if (flag) {
                if (strcmp(stk->data, ",") == 0) {
                    search_PopStack(stk);
                }
                return true;
            }
            break;
        }
        case cJSON_Array: {
            bool flag = false;
            int arr_int = 0;
            cJSON *docEle = NULL;
            StringInfo position = makeStringInfo();
            while (true) {
                docEle = cJSON_GetArrayItem(doc_cJSON, arr_int);
                if (docEle == NULL) {
                    break;
                }
                appendStringInfo(position, "%s[%d]", last_position, arr_int);
                if (json_search_unit(docEle, search_text, mode_match, wildchar, position->data, stk)) {
                    if (mode_match) {
                        return true;
                    } else {
                        if ((docEle->type & 0xFF) == cJSON_String) {
                            search_PushStack(stk, ",");
                        }
                        flag = true;
                    }
                }
                resetStringInfo(position);
                arr_int++;
            }
            DestroyStringInfo(position);
            if (flag) {
                if (strcmp(stk->data, ",") == 0) {
                    search_PopStack(stk);
                }
                return true;
            }
            break;
        }
        case cJSON_String: {
            text *value = cstring_to_text(doc_cJSON->valuestring);
            char *f = VARDATA_ANY(value);
            char *b = VARDATA_ANY(search_text);
            int flen = VARSIZE_ANY_EXHDR(value);
            int blen = VARSIZE_ANY_EXHDR(search_text);
            if (MB_MatchText(f, flen, b, blen, 0, true) == LIKE_TRUE) {
                /* After successful matching, judge whether there are wildcards in the path, and if there are wildcards,
                compare the return path with the parameter path to judge whether the return path conforms to the range
                of the parameter path. */
                if (*wildchar != '\0') {
                    text *wildchar_text = cstring_to_text(wildchar);
                    text *last_position_text = cstring_to_text(last_position);
                    f = VARDATA_ANY(last_position_text);
                    flen = VARSIZE_ANY_EXHDR(last_position_text);
                    b = VARDATA_ANY(wildchar_text);
                    blen = VARSIZE_ANY_EXHDR(wildchar_text);
                    if (MB_MatchText(f, flen, b, blen, 0, true) != LIKE_TRUE) {
                        break;
                    }
                }
                StringInfo position = makeStringInfo();
                appendStringInfo(position, "%s\"", last_position);
                search_PushStack(stk, position->data);
                return true;
            }
        }
        default:
            break;
    }
    return false;
}

static text *remove_duplicate_path(search_LinkStack &stk)
{
    StringInfo rpath = makeStringInfo();
    bool flag = false;
    stk = search_ReverseStack(stk);
    appendStringInfoString(rpath, stk->data);
    search_PopStack(stk);
    while (stk != NULL) {
        if (strcmp(stk->data, ",") != 0) {
            if (strstr(rpath->data, stk->data) == NULL) {
                appendStringInfo(rpath, ",%s", stk->data);
                flag = true;
            }
        }
        search_PopStack(stk);
    }

    if (flag) {
        StringInfo fpath = makeStringInfo();
        appendStringInfo(fpath, "[%s]", rpath->data);
        copyStringInfo(rpath, fpath);
        DestroyStringInfo(fpath);
    }
    return cstring_to_text(rpath->data);
}

Datum json_search(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2)) {
        PG_RETURN_NULL();
    }

    char *one_or_all = text_to_cstring(PG_GETARG_TEXT_P(1));
    int nargs = PG_NARGS();
    int path_num;
    int i = 0;
    Oid valtype;
    Oid typOutput;
    bool typIsVarlena = false;
    Datum arg = 0;
    text *search_text;
    text *res = NULL;
    Datum *pathtext = NULL;
    bool *pathnulls = NULL;
    cJSON *result = NULL;
    cJSON *doc_cJSON = NULL;
    search_LinkStack stk = NULL;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    doc_cJSON = input_to_cjson(valtype, "json_search", 1, arg);

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 2);
    arg = PG_GETARG_DATUM(2);
    getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
    search_text = cstring_to_text(OidOutputFunctionCall(typOutput, arg));

    if (nargs > 3 && PG_ARGISNULL(3) == 0) {
        valtype = get_fn_expr_argtype(fcinfo->flinfo, 3);
        if (valtype != BOOLOID) {
            Datum escape_datum = 0;
            switch (valtype) {
                case UNKNOWNOID:
                    escape_datum = CStringGetTextDatum(PG_GETARG_POINTER(3));
                    break;
                case INT4OID:
                    escape_datum = DirectFunctionCall1(int4_text, PG_GETARG_DATUM(3));
                    break;
                default:
                    ereport(ERROR, (errcode(ERRCODE_INVALID_ESCAPE_SEQUENCE), errmsg("invalid escape string"),
                                    errhint("Escape string must be empty or one character.")));
            }
            text *escape = DatumGetTextP(escape_datum);
            search_text = MB_do_like_escape(search_text, escape);
        }
    }

    if (nargs > 4) {
        ArrayType *path_array = PG_GETARG_ARRAYTYPE_P(4);
        if (array_contains_nulls(path_array)) {
            PG_RETURN_NULL();
        }
        deconstruct_array(path_array, TEXTOID, -1, false, 'i', &pathtext, &pathnulls, &path_num);
    }

    if (strcmp(one_or_all, "one") == 0) {
        if (nargs < 5) {
            char *position = "\"$";
            char *wildchar = "";
            if (json_search_unit(doc_cJSON, search_text, true, wildchar, position, stk)) {
                res = cstring_to_text(stk->data);
                search_CleanStack(stk);
                PG_RETURN_TEXT_P(res);
            }
        } else {
            StringInfo wildchar = makeStringInfo();
            StringInfo position = makeStringInfo();
            for (i = 0; i < path_num; i++) {
                char *path = TextDatumGetCString(pathtext[i]);
                result = jp_match_record(doc_cJSON, path, position, wildchar);
                if (result != NULL) {
                    if (json_search_unit(result, search_text, true, wildchar->data, position->data, stk)) {
                        res = cstring_to_text(stk->data);
                        search_CleanStack(stk);
                        DestroyStringInfo(wildchar);
                        DestroyStringInfo(position);
                        PG_RETURN_TEXT_P(res);
                    }
                }
                resetStringInfo(wildchar);
                resetStringInfo(position);
            }
            DestroyStringInfo(wildchar);
            DestroyStringInfo(position);
        }
    } else if (strcmp(one_or_all, "all") == 0) {
        if (nargs < 5) {
            char *position = "\"$";
            char *wildchar = "";
            if (json_search_unit(doc_cJSON, search_text, false, wildchar, position, stk)) {
                res = remove_duplicate_path(stk);
                search_CleanStack(stk);
                PG_RETURN_TEXT_P(res);
            }
        } else {
            bool flag = false;
            StringInfo wildchar = makeStringInfo();
            StringInfo position = makeStringInfo();
            for (i = 0; i < path_num; i++) {
                char *path = TextDatumGetCString(pathtext[i]);
                result = jp_match_record(doc_cJSON, path, position, wildchar);
                if (result != NULL) {
                    if (json_search_unit(result, search_text, false, wildchar->data, position->data, stk)) {
                        flag = true;
                        search_PushStack(stk, ",");
                    }
                }
                resetStringInfo(wildchar);
                resetStringInfo(position);
            }
            DestroyStringInfo(wildchar);
            DestroyStringInfo(position);
            if (flag) {
                search_PopStack(stk);
                res = remove_duplicate_path(stk);
                search_CleanStack(stk);
                PG_RETURN_TEXT_P(res);
            }
        }
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("The oneOrAll argument to json_search may take these values:'one'or'all'.")));
    }
    PG_RETURN_NULL();
}

Datum json_keys(PG_FUNCTION_ARGS)
{
    int num = 0;
    char **keys;
    cJSON *root = NULL;
    cJSON *node;
    Oid valtype;
    Datum arg = 0;
    int error_pos = -1;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_keys", 1, arg);

    if (PG_NARGS() == 2) {
        char *path = text_to_cstring(PG_GETARG_TEXT_P(1));
        cJSON_JsonPath *jp = NULL;
        cJSON_ResultWrapper *res = NULL;

        res = cJSON_CreateResultWrapper();
        if (containsAsterisk(path) > 0) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("in this situation, path expressions may not contain the * and ** tokens")));
        }

        jp = jp_parse(path, error_pos);
        if (!jp) {
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(root);
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("Invalid JSON path expression. The error is around character position %d.", error_pos)));
        }
        if (jp->next!=NULL && jp->next->key == NULL && (root->type & 0xFF) == cJSON_Object &&
            jp->next->index == 0 && jp->next->next == NULL) {
            PG_RETURN_NULL();
        }
        cJSON_JsonPathMatch(root, jp, res);
        if (res->len == 1) {
            root = res->head->next->node;
        } else {
            PG_RETURN_NULL();
        }
        cJSON_DeleteJsonPath(jp);
        cJSON_DeleteResultWrapper(res);
    }

    if ((root->type & 0xFF) != cJSON_Object) {
        PG_RETURN_NULL();
    }
    node = root->child;
    keys = (char **)palloc(cJSON_GetArraySize(root) * sizeof(char *));
    StringInfo result = makeStringInfo();
    while (node != NULL) {
        keys[num] = node->string;
        node = node->next;
        num++;
    }
    appendStringInfoChar(result, '[');
    for (int k = 0; k < num; k++) {
        if (k != 0)
            appendStringInfoString(result, ", ");
        appendStringInfo(result, "\"%s\"", keys[k]);
    }
    appendStringInfoChar(result, ']');
    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

Datum json_array_append(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    Oid valtype;
    Oid typOutput;
    Datum arg = 0;
    bool typIsVarlena = false;
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    int error_pos = -1;
    TYPCATEGORY tcategory;
    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid arg number")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_array_append", 1, arg);

    for (int i = 1; i < nargs; i += 2) {
        /* process path */
        if (PG_ARGISNULL(i)) {
            cJSON_Delete(root);
            PG_RETURN_NULL();
        }
        valtype = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        pathString = OidOutputFunctionCall(typOutput, arg);
        jp = jp_parse(pathString, error_pos);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around argument %d.", i)));
        }
        /* process json to add */
        if (PG_ARGISNULL(i + 1)) {
            value = cJSON_CreateNull();
        } else {
            valtype = get_fn_expr_argtype(fcinfo->flinfo, i + 1);
            arg = PG_GETARG_DATUM(i + 1);
            getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
            valueString = OidOutputFunctionCall(typOutput, arg);
            tcategory = get_value_type(valtype, typOutput);
            switch (tcategory) {
                case TYPCATEGORY_JSON:
                case TYPCATEGORY_JSON_CAST:
                case TYPCATEGORY_ARRAY:
                case TYPCATEGORY_COMPOSITE:
                case TYPCATEGORY_NUMERIC:
                    value = cJSON_Parse(valueString);
                    break;
                case TYPCATEGORY_BOOLEAN:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_CreateString(valueString);
                    break;
            }
            pfree(valueString);
        }
        if (jp->next == NULL && jp->type == cJSON_JsonPath_Start) {
            if (cJSON_IsArray(root)) {
                cJSON_AddItemToArray(root, value);
            } else {
                cJSON *n = cJSON_Duplicate(root, false);
                n->child = root->child;
                n->next = NULL;
                n->prev = NULL;
                root->type = cJSON_Array;
                root->child = NULL;
                cJSON_AddItemToArray(root, n);
                cJSON_AddItemToArray(root, value);
            }
        } else {
            if (!cJSON_ArrayAppend(root, jp, value)) {
                cJSON_Delete(value);  // when value is not inserted, delete it
            }
            if (cJSON_JsonPathCanMatchMany(jp)) {
                cJSON_DeleteJsonPath(jp);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("In this situation, path expressions may not contain the * and ** tokens.")));
            }
        }
        cJSON_DeleteJsonPath(jp);
    }
    cJSON_SortObject(root);
    s = cJSON_PrintUnformatted(root);
    res = formatJsondoc(s);
    cJSON_Delete(root);
    pfree(s);
    PG_RETURN_TEXT_P(res);
}

Datum json_append(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(json_array_append(fcinfo));
}

static int put_object_keys_into_set(char **keys, cJSON *json1, cJSON *json2)
{
    int cnt = 0;
    cJSON *node = NULL;
    bool flag;

    node = json1->child;
    while (node != NULL) {
        flag = true;
        for (int iter = 0; iter < cnt; iter++) {
            if (strcmp(node->string, keys[iter]) == 0)
                flag = false;
        }
        if (flag) {
            keys[cnt] = node->string;
            cnt++;
        }
        node = node->next;
    }
    node = json2->child;
    while (node != NULL) {
        flag = true;
        for (int iter = 0; iter < cnt; iter++) {
            if (strcmp(node->string, keys[iter]) == 0)
                flag = false;
        }
        if (flag) {
            keys[cnt] = node->string;
            cnt++;
        }
        node = node->next;
    }
    return cnt;
}

static int put_object_keys_into_set(char **keys, cJSON *json)
{
    int cnt = 0;
    cJSON *node = NULL;
    node = json->child;
    while (node != NULL) {
        bool flag = true;
        for (int iter = 0; iter < cnt; iter++) {
            if (strcmp(node->string, keys[iter]) == 0)
                flag = false;
        }
        if (flag) {
            keys[cnt] = node->string;
            cnt++;
        }
        node = node->next;
    }
    return cnt;
}

static void appendObject(StringInfo result, cJSON *json, int cnt, char **keys, int *pos)
{
    appendStringInfoChar(result, '{');
    for (int i = 0; i < cnt; i++) {
        if (i != 0)
            appendStringInfoString(result, ", ");
        escape_json(result, keys[pos[i]]);
        appendStringInfoString(result, ": ");
        // appendStringInfo(result,"\"%s\": ", keys[pos[i]]);
        cJSON *tResult = cJSON_GetObjectItem(json, keys[pos[i]]);
        json_regular_format(result, tResult);
    }
    appendStringInfoChar(result, '}');
}

static void appendStringInfoObject(StringInfo result, cJSON *json)
{
    if (cJSON_GetArraySize(json) == 0) {
        appendStringInfoString(result, "{}");
        return;
    }

    char **keys = (char **)palloc(cJSON_GetArraySize(json) * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, json);

    int pos[cnt];
    for (int i = 0; i < cnt; i++)
        pos[i] = i;

    get_keys_order(keys, 0, cnt - 1, pos);

    appendObject(result, json, cnt, keys, pos);

    pfree(keys);
}

static void appendStringInfoArray(StringInfo result, cJSON *json)
{
    appendStringInfoChar(result, '[');
    appendStringInfoArrayEle(result, json);
    appendStringInfoChar(result, ']');
}

static void appendStringInfoArrayEle(StringInfo result, cJSON *json)
{
    int arr_iter = 0;
    while (true) {
        cJSON *val = cJSON_GetArrayItem(json, arr_iter);
        if (val == NULL)
            break;
        if (arr_iter != 0)
            appendStringInfoString(result, ", ");
        json_regular_format(result, val);
        arr_iter++;
    }
}

static void json_regular_format(StringInfo result, cJSON *json)
{
    switch (json->type & 0xFF) {
        case cJSON_Object: {
            appendStringInfoObject(result, json);
            break;
        }
        case cJSON_Array: {
            appendStringInfoArray(result, json);
            break;
        }
        case cJSON_True: {
            appendStringInfoString(result, "true");
            break;
        }
        case cJSON_False: {
            appendStringInfoString(result, "false");
            break;
        }
        case cJSON_NULL: {
            appendStringInfoString(result, "null");
            break;
        }
        case cJSON_Number: {
            if (json->valuedouble == (double)(json->valueint)) {
                appendStringInfo(result, "%d", json->valueint);
            } else {
                errno_t sprintf_rc = 0;
                char *cJSON_double = (char *)palloc(64 * sizeof(char));
                int pos;
                sprintf_rc = sprintf_s(cJSON_double, 64, "%.15f", json->valuedouble);
                securec_check_ss_c(sprintf_rc, "\0", "\0");
                pos = strlen(cJSON_double) - 1;
                while (cJSON_double[pos] == '0') {
                    cJSON_double[pos] = '\0';
                    pos--;
                }

                appendStringInfo(result, "%s", cJSON_double);
            }
            break;
        }
        case cJSON_String: {
            escape_json(result, json->valuestring);
            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", (json->type & 0xFF));
    }
    return;
}

static cJSON *addTwoObjectsPatch(cJSON *j1, cJSON *j2)
{
    cJSON *result = cJSON_CreateObject();

    int i;

    int result_count = cJSON_GetArraySize(j1) + cJSON_GetArraySize(j2);
    char **keys = (char **)palloc(result_count * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, j1, j2);

    int pos[cnt];
    for (i = 0; i < cnt; i++)
        pos[i] = i;

    for (i = 0; i < cnt; i++) {
        cJSON *res1 = cJSON_GetObjectItem(j1, keys[pos[i]]);
        cJSON *res2 = cJSON_GetObjectItem(j2, keys[pos[i]]);

        if (res1 == NULL)
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], res2);
        else if (res2 == NULL)
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], res1);
        else if ((res2->type & 0xFF) == cJSON_NULL) {
            continue;
        } else {
            cJSON *temp = json_merge_patch_unit(res1, res2);
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], temp);
        }
    }

    pfree(keys);

    return result;
}

static cJSON *addOneObjectsPatch(cJSON *json)
{
    cJSON *result = cJSON_CreateObject();

    int i;

    int result_count = cJSON_GetArraySize(json);
    char **keys = (char **)palloc(result_count * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, json);

    int pos[cnt];
    for (i = 0; i < cnt; i++)
        pos[i] = i;

    for (i = 0; i < cnt; i++) {
        cJSON *res = cJSON_GetObjectItem(json, keys[pos[i]]);
        if (res != NULL)
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], res);
    }

    pfree(keys);

    return result;
}

static cJSON *json_merge_patch_unit(cJSON *j1, cJSON *j2)
{
    cJSON *result = NULL;

    if (j1 == NULL && j2 != NULL) {
        switch (j2->type & 0xFF) {
            case cJSON_Object: {
                result = addOneObjectsPatch(j2);
                break;
            }
            case cJSON_Array:
            case cJSON_True:
            case cJSON_False:
            case cJSON_NULL:
            case cJSON_Number:
            case cJSON_String: {
                result = j2;
                break;
            }
            default:
                elog(ERROR, "unexpected json type: %d", j2->type & 0xFF);
        }
        return result;
    }

    switch (j1->type & 0xFF) {
        case cJSON_Object: {
            switch (j2->type & 0xFF) {
                case cJSON_Object: {
                    result = addTwoObjectsPatch(j1, j2);
                    break;
                }
                case cJSON_Array:
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL:
                case cJSON_Number:
                case cJSON_String: {
                    result = j2;
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", j2->type);
            }
            break;
        }
        case cJSON_Array:
        case cJSON_True:
        case cJSON_False:
        case cJSON_NULL:
        case cJSON_Number:
        case cJSON_String: {
            switch (j2->type & 0xFF) {
                case cJSON_Object: {
                    result = addOneObjectsPatch(j2);
                    break;
                }
                case cJSON_Array:
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL:
                case cJSON_Number:
                case cJSON_String: {
                    result = j2;
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", j2->type & 0xFF);
            }
            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", j1->type & 0xFF);
    }

    return result;
}

static cJSON *addObjectsPreserve(cJSON *j1, cJSON *j2)
{
    cJSON *result = cJSON_CreateObject();

    int i;

    int result_count = cJSON_GetArraySize(j1) + cJSON_GetArraySize(j2);

    char **keys = (char **)palloc(result_count * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, j1, j2);

    int pos[cnt];
    for (i = 0; i < cnt; i++)
        pos[i] = i;

    for (i = 0; i < cnt; i++) {
        cJSON *res1 = cJSON_GetObjectItem(j1, keys[pos[i]]);
        cJSON *res2 = cJSON_GetObjectItem(j2, keys[pos[i]]);

        if (res1 == NULL)
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], res2);
        else if (res2 == NULL)
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], res1);
        else {
            cJSON *temp = json_merge_preserve_unit(res1, res2);
            cJSON_AddItemReferenceToObject(result, keys[pos[i]], temp);
        }
    }

    pfree(keys);

    return result;
}

static cJSON *json_merge_preserve_unit(cJSON *j1, cJSON *j2)
{
    cJSON *result = NULL;
    switch (j1->type & 0xFF) {
        case cJSON_Object: {
            switch (j2->type & 0xFF) {
                case cJSON_Object: {
                    result = addObjectsPreserve(j1, j2);
                    break;
                }
                case cJSON_Array: {
                    result = cJSON_CreateArray();
                    cJSON *node = NULL;
                    cJSON_AddItemReferenceToArray(result, j1);
                    cJSON_ArrayForEach(node, j2)
                    {
                        cJSON_AddItemReferenceToArray(result, node);
                    }
                    break;
                }
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL:
                case cJSON_Number:
                case cJSON_String: {
                    result = cJSON_CreateArray();
                    cJSON_AddItemReferenceToArray(result, j1);
                    cJSON_AddItemReferenceToArray(result, j2);
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", j2->type);
            }
            break;
        }
        case cJSON_Array: {
            switch (j2->type & 0xFF) {
                case cJSON_Array: {
                    result = cJSON_CreateArray();
                    cJSON *node = NULL;
                    cJSON_ArrayForEach(node, j1)
                    {
                        cJSON_AddItemReferenceToArray(result, node);
                    }
                    cJSON_ArrayForEach(node, j2)
                    {
                        cJSON_AddItemReferenceToArray(result, node);
                    }
                    break;
                }
                case cJSON_Object: {
                    result = cJSON_CreateArray();
                    cJSON *node = NULL;
                    cJSON_ArrayForEach(node, j1)
                    {
                        cJSON_AddItemReferenceToArray(result, node);
                    }
                    cJSON_AddItemReferenceToArray(result, j2);
                    break;
                }
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL:
                case cJSON_Number:
                case cJSON_String: {
                    result = cJSON_CreateArray();
                    cJSON *node = NULL;
                    cJSON_ArrayForEach(node, j1)
                    {
                        cJSON_AddItemReferenceToArray(result, node);
                    }
                    cJSON_AddItemReferenceToArray(result, j2);
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", j2->type);
            }
            break;
        }
        case cJSON_True:
        case cJSON_False:
        case cJSON_NULL:
        case cJSON_Number:
        case cJSON_String: {
            switch (j2->type & 0xFF) {
                case cJSON_Array: {
                    result = cJSON_CreateArray();
                    cJSON *node = NULL;
                    cJSON_AddItemReferenceToArray(result, j1);
                    cJSON_ArrayForEach(node, j2)
                    {
                        cJSON_AddItemReferenceToArray(result, node);
                    }
                    break;
                }
                case cJSON_Object: {
                    result = cJSON_CreateArray();
                    cJSON_AddItemReferenceToArray(result, j1);
                    cJSON_AddItemReferenceToArray(result, j2);
                    break;
                }
                case cJSON_True:
                case cJSON_False:
                case cJSON_NULL:
                case cJSON_Number:
                case cJSON_String: {
                    result = cJSON_CreateArray();
                    cJSON_AddItemReferenceToArray(result, j1);
                    cJSON_AddItemReferenceToArray(result, j2);
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", j2->type);
            }
            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", j1->type);
    }
    return result;
}

Datum json_merge_patch(PG_FUNCTION_ARGS)
{
    int json_num = PG_NARGS();
    int null_pos = -1;
    bool contain_null = false;
    int start_merge_pos = 0;
    Oid valtype;
    Datum arg = 0;
    int iter = 0;

    if (json_num <= 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Incorrect parameter count")));

    // To find null argument's position
    for (int i = json_num - 1; i >= 0; i--) {
        if (PG_ARGISNULL(i)) {
            if (i == json_num - 1)
                PG_RETURN_NULL();
            else {
                null_pos = i;
                contain_null = true;
                break;
            }
        }
    }

    // Get all cJSON struct behind null_pos
    int jsondoc_num = json_num - 1 - null_pos;
    cJSON **jsondoc = NULL;
    jsondoc = (cJSON **)palloc(jsondoc_num * sizeof(cJSON *));
    for (iter = 0; iter < jsondoc_num; iter++) {
        valtype = get_fn_expr_argtype(fcinfo->flinfo, iter + null_pos + 1);
        arg = PG_GETARG_DATUM(iter + null_pos + 1);
        jsondoc[iter] = input_to_cjson(valtype, "json_merge_patch", iter + null_pos + 2, arg);
    }

    // If args contain null, find location that cannot be null from the front to the back
    if (contain_null) {
        for (iter = 0; iter < jsondoc_num; iter++) {
            if ((jsondoc[iter]->type & 0xFF) != cJSON_Object) {
                start_merge_pos = iter;
                break;
            } else {
                continue;
            }
        }

        // If iter equals jsondoc_num, result is null
        if (iter == jsondoc_num) {
            for (int i = 0; i < jsondoc_num; i++) {
                if (jsondoc[i] != NULL)
                    cJSON_Delete(jsondoc[i]);
            }
            PG_RETURN_NULL();
        }
    }

    // From the back to the front, unnecessary merge operations are ignored
    for (iter = jsondoc_num - 1; iter >= start_merge_pos; iter--) {
        if ((jsondoc[iter]->type & 0xFF) != cJSON_Object) {
            start_merge_pos = iter;
            break;
        } else if (iter - 1 < start_merge_pos) {
            break;
        } else if ((jsondoc[iter - 1]->type & 0xFF) == cJSON_Object) {
            continue;
        } else if ((jsondoc[iter - 1]->type & 0xFF) != cJSON_Object) {
            start_merge_pos = iter;
            break;
        } else {
            elog(ERROR, "unexpected json type: %d", (jsondoc[iter]->type & 0xFF));
        }
    }

    cJSON *result = NULL;

    for (iter = start_merge_pos; iter < jsondoc_num; iter++) {
        if (iter == start_merge_pos) {
            result = json_merge_patch_unit(NULL, jsondoc[start_merge_pos]);
        } else {
            result = json_merge_patch_unit(result, jsondoc[iter]);
        }
    }

    StringInfo resultString = makeStringInfo();
    json_regular_format(resultString, result);
    if ((result->type & 0xFF) == cJSON_Object)
        cJSON_Delete(result);
    for (iter = 0; iter < jsondoc_num; iter++) {
        if (jsondoc[iter] != NULL)
            cJSON_Delete(jsondoc[iter]);
    }

    PG_RETURN_TEXT_P(cstring_to_text(resultString->data));
}

Datum json_merge_preserve(PG_FUNCTION_ARGS)
{
    int json_num = PG_NARGS();
    cJSON *result = NULL;
    cJSON **jsondoc = (cJSON **)palloc(json_num * sizeof(cJSON *));
    int jsondoc_iter;
    Oid valtype;
    Datum arg = 0;

    if (json_num <= 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Incorrect parameter count")));

    /*
     * If the array is empty, return NULL; this is dubious but it's what 9.3
     * did.
     */
    for (int i = json_num - 1; i >= 0; i--) {
        if (PG_ARGISNULL(i)) {
            PG_RETURN_NULL();
        }
    }

    for (jsondoc_iter = 0; jsondoc_iter < json_num; jsondoc_iter++) {
        valtype = get_fn_expr_argtype(fcinfo->flinfo, jsondoc_iter);
        arg = PG_GETARG_DATUM(jsondoc_iter);
        jsondoc[jsondoc_iter] = input_to_cjson(valtype, "json_merge_preserve", jsondoc_iter + 1, arg);
    }

    for (int i = 1; i < json_num; i++) {
        if (i == 1) {
            result = json_merge_preserve_unit(jsondoc[0], jsondoc[1]);
        } else {
            result = json_merge_preserve_unit(result, jsondoc[i]);
        }
    }

    StringInfo resultString = makeStringInfo();
    json_regular_format(resultString, result);
    cJSON_Delete(result);
    for (jsondoc_iter = 0; jsondoc_iter < json_num; jsondoc_iter++) {
        cJSON_Delete(jsondoc[jsondoc_iter]);
    }

    PG_RETURN_TEXT_P(cstring_to_text(resultString->data));
}

Datum json_merge(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(json_merge_preserve(fcinfo));
}

Datum json_insert(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    char *data = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid valtype;
    Oid typOutput;
    Datum arg = 0;
    bool typIsVarlena = false;
    int error_pos = -1;
    TYPCATEGORY tcategory;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'JSON_INSERT'")));
    }
    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_insert", 1, arg);

    for (int i = 1; i < nargs; i += 2) {
        /* process path */
        if (PG_ARGISNULL(i)) {
            cJSON_Delete(root);
            PG_RETURN_NULL();
        }
        valtype = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
        jp = jp_parse(data, error_pos);
        pfree(data);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid JSON path expression.")));
        }
        if (cJSON_JsonPathCanMatchMany(jp)) {
            cJSON_DeleteJsonPath(jp);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("In this situation, path expressions may not contain the * and ** tokens.")));
        }
        /* process json to add */
        if (PG_ARGISNULL(i + 1)) {
            value = cJSON_CreateNull();
        } else {
            valtype = get_fn_expr_argtype(fcinfo->flinfo, i + 1);
            arg = PG_GETARG_DATUM(i + 1);
            getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
            data = OidOutputFunctionCall(typOutput, arg);
            tcategory = get_value_type(valtype, typOutput);
            switch (tcategory) {
                case TYPCATEGORY_JSON:
                case TYPCATEGORY_JSON_CAST:
                case TYPCATEGORY_ARRAY:
                case TYPCATEGORY_COMPOSITE:
                case TYPCATEGORY_NUMERIC:
                    value = cJSON_Parse(data);
                    break;
                case TYPCATEGORY_BOOLEAN:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_CreateString(data);
                    break;
            }
            pfree(data);
        }
        if (!cJSON_JsonInsert(root, jp, value)) {
            cJSON_Delete(value);  // when value is not inserted, delete it
        };
        cJSON_DeleteJsonPath(jp);
    }
    cJSON_SortObject(root);
    s = cJSON_PrintUnformatted(root);
    res = formatJsondoc(s);
    cJSON_Delete(root);
    pfree(s);
    PG_RETURN_TEXT_P(res);
}

Datum json_replace(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    Oid valtype;
    Oid typOutput;
    Datum arg = 0;
    bool typIsVarlena = false;
    bool invalidPath = false;
    StringInfo result = NULL;
    int error_pos = -1;
    TYPCATEGORY tcategory;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'JSON_REPLACE'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_replace", 1, arg);

    for (int i = 1; i < nargs; i += 2) {
        /* process path */
        if (PG_ARGISNULL(i)) {
            cJSON_Delete(root);
            PG_RETURN_NULL();
        }
        valtype = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        pathString = OidOutputFunctionCall(typOutput, arg);
        jp = jp_parse(pathString, error_pos);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around argument %d.", i)));
        }
        /* process json to replace */
        if (PG_ARGISNULL(i + 1)) {
            value = cJSON_CreateNull();
        } else {
            valtype = get_fn_expr_argtype(fcinfo->flinfo, i + 1);
            arg = PG_GETARG_DATUM(i + 1);
            getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
            valueString = OidOutputFunctionCall(typOutput, arg);
            tcategory = get_value_type(valtype, typOutput);
            switch (tcategory) {
                case TYPCATEGORY_JSON:
                case TYPCATEGORY_JSON_CAST:
                case TYPCATEGORY_ARRAY:
                case TYPCATEGORY_COMPOSITE:
                case TYPCATEGORY_NUMERIC:
                    value = cJSON_Parse(valueString);
                    break;
                case TYPCATEGORY_BOOLEAN:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_CreateString(valueString);
                    break;
            }
            pfree(valueString);
        }
        if (jp->next == NULL || (!cJSON_IsArray(root) && jp->next->next == NULL &&
                                 jp->next->type == cJSON_JsonPath_Index && jp->next->index == 0)) {
            cJSON_Delete(root);
            cJSON_DeleteJsonPath(jp);
            root = value;
            continue;
        }
        if (!cJSON_JsonReplace(root, jp, value, invalidPath)) {
            cJSON_Delete(value);  // when value is not replaced, delete it
            if (invalidPath) {
                cJSON_DeleteJsonPath(jp);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("In this situation, path expressions may not contain the * and ** tokens.")));
            }
        };
        cJSON_DeleteJsonPath(jp);
    }
    result = makeStringInfo();
    json_regular_format(result, root);
    cJSON_Delete(root);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

Datum json_remove(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    char *pathString = NULL;
    cJSON *root = NULL;
    cJSON_JsonPath *jp = NULL;
    Oid valtype;
    Oid typOutput;
    Datum arg = 0;
    bool typIsVarlena = false;
    bool invalidPath = false;
    StringInfo result = NULL;
    int error_pos = -1;

    if (nargs < 2) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'json_remove'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_remove", 1, arg);

    for (int i = 1; i < nargs; i++) {
        /* process path */
        if (PG_ARGISNULL(i)) {
            cJSON_Delete(root);
            PG_RETURN_NULL();
        }
        valtype = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        pathString = OidOutputFunctionCall(typOutput, arg);
        jp = jp_parse(pathString, error_pos);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around argument %d.", i)));
        }
        if (jp->next == NULL && jp->type == cJSON_JsonPath_Start) {
            cJSON_Delete(root);
            cJSON_DeleteJsonPath(jp);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg(" The path expression '$' is not allowed in this context.")));
        }

        /* process json to replace */
        cJSON_JsonRemove(root, jp, &invalidPath);
        if (invalidPath) {
            cJSON_DeleteJsonPath(jp);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("In this situation, path expressions may not contain the * and ** tokens.")));
        }
        cJSON_DeleteJsonPath(jp);
    }
    result = makeStringInfo();
    json_regular_format(result, root);
    cJSON_Delete(root);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

Datum json_array_insert(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    Oid valtype;
    Oid typOutput;
    Datum arg = 0;
    bool typIsVarlena = false;
    bool invalidPath = false;
    bool isArray = false;
    StringInfo result = NULL;
    int error_pos = -1;
    TYPCATEGORY tcategory;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'json_array_insert'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_array_insert", 1, arg);

    for (int i = 1; i < nargs; i += 2) {
        /* process path */
        if (PG_ARGISNULL(i)) {
            cJSON_Delete(root);
            PG_RETURN_NULL();
        }
        valtype = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        pathString = OidOutputFunctionCall(typOutput, arg);
        jp = jp_parse(pathString, error_pos);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around argument %d.", i)));
        }
        if (jp->next == NULL && jp->type == cJSON_JsonPath_Start) {
            cJSON_Delete(root);
            cJSON_DeleteJsonPath(jp);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("A path expression is not a path to a cell in an array.")));
        }
        /* process json to add */
        if (PG_ARGISNULL(i + 1)) {
            value = cJSON_CreateNull();
        } else {
            valtype = get_fn_expr_argtype(fcinfo->flinfo, i + 1);
            arg = PG_GETARG_DATUM(i + 1);
            getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
            valueString = OidOutputFunctionCall(typOutput, arg);
            tcategory = get_value_type(valtype, typOutput);
            switch (tcategory) {
                case TYPCATEGORY_JSON:
                case TYPCATEGORY_JSON_CAST:
                case TYPCATEGORY_ARRAY:
                case TYPCATEGORY_COMPOSITE:
                case TYPCATEGORY_NUMERIC:
                    value = cJSON_Parse(valueString);
                    break;
                case TYPCATEGORY_BOOLEAN:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_CreateString(valueString);
                    break;
            }
            pfree(valueString);
        }
        if (!cJSON_JsonArrayInsert(root, jp, value, &invalidPath, &isArray)) {
            cJSON_Delete(value);  // when value is not inserted, delete it
        }
        if (invalidPath) {
            cJSON_DeleteJsonPath(jp);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("In this situation, path expressions may not contain the * and ** tokens.")));
        }
        if (isArray) {
            cJSON_DeleteJsonPath(jp);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("A path expression is not a path to a cell in an array.")));
        }
        cJSON_DeleteJsonPath(jp);
    }
    result = makeStringInfo();
    json_regular_format(result, root);
    cJSON_Delete(root);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(result->data, result->len));
}

Datum json_set(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid valtype;
    Oid typOutput;
    Datum arg = 0;
    bool typIsVarlena = false;
    bool invalidPath = false;
    int error_pos = -1;
    TYPCATEGORY tcategory;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'JSON_SET'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_set", 1, arg);

    for (int i = 1; i < nargs; i += 2) {
        /* process path */
        if (PG_ARGISNULL(i)) {
            cJSON_Delete(root);
            PG_RETURN_NULL();
        }
        valtype = get_fn_expr_argtype(fcinfo->flinfo, i);
        arg = PG_GETARG_DATUM(i);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        pathString = OidOutputFunctionCall(typOutput, arg);
        jp = jp_parse(pathString, error_pos);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around argument %d.", i)));
        }
        /* process json to replace */
        if (PG_ARGISNULL(i + 1)) {
            value = cJSON_CreateNull();
        } else {
            valtype = get_fn_expr_argtype(fcinfo->flinfo, i + 1);
            arg = PG_GETARG_DATUM(i + 1);
            getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
            valueString = OidOutputFunctionCall(typOutput, arg);
            tcategory = get_value_type(valtype, typOutput);
            switch (tcategory) {
                case TYPCATEGORY_JSON:
                case TYPCATEGORY_JSON_CAST:
                case TYPCATEGORY_ARRAY:
                case TYPCATEGORY_COMPOSITE:
                case TYPCATEGORY_NUMERIC:
                    value = cJSON_Parse(valueString);
                    break;
                case TYPCATEGORY_BOOLEAN:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_CreateString(valueString);
                    break;
            }
            pfree(valueString);
        }
        if (jp->next == NULL || (!cJSON_IsArray(root) && jp->next->next == NULL &&
                                 jp->next->type == cJSON_JsonPath_Index && jp->next->index == 0)) {
            cJSON_Delete(root);
            cJSON_DeleteJsonPath(jp);
            root = value;
            continue;
        }
        if (!cJSON_JsonReplace(root, jp, value, invalidPath)) {
            cJSON_JsonInsert(root, jp, value);
            if (invalidPath) {
                cJSON_DeleteJsonPath(jp);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("In this situation, path expressions may not contain the * and ** tokens.")));
            }
        };
        cJSON_DeleteJsonPath(jp);
    }
    cJSON_SortObject(root);
    s = cJSON_PrintUnformatted(root);
    res = cstring_to_text(s);
    cJSON_Delete(root);
    pfree(s);
    PG_RETURN_TEXT_P(res);
}

static void length_object_field_start(void *state, char *fname, bool isnull)
{
    OkeysState *_state = (OkeysState *)state;
    if (_state->lex->lex_level == 1) {
        _state->result_count++;
    }
}

static void length_array_element_start(void *state, bool isnull)
{
    OkeysState *_state = (OkeysState *)state;
    if (_state->lex->lex_level == 1) {
        _state->result_count++;
    }
}

static void length_scalar(void *state, char *token, JsonTokenType tokentype)
{
    OkeysState *_state = (OkeysState *)state;
    if (_state->lex->lex_level == 0) {
        _state->result_count = 1;
    }
}

Datum json_length(PG_FUNCTION_ARGS)
{
    Oid valtype;
    Oid typOutput;
    int result;
    bool typIsVarlena = false;
    Datum arg = 0;
    text *json = NULL;
    char *data = NULL;
    int error_pos = -1;

    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
        json = cstring_to_text(data);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function json_length")));
    }

    if (PG_NARGS() == 2) {
        cJSON *root = NULL;
        char *path = text_to_cstring(PG_GETARG_TEXT_P(1));
        cJSON_JsonPath *jp = NULL;
        cJSON_ResultWrapper *res = NULL;

        root = cJSON_ParseWithOpts(data, 0, 1);
        res = cJSON_CreateResultWrapper();
        if (containsAsterisk(path) > 0) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("in this situation, path expressions may not contain the * and ** tokens")));
        }

        jp = jp_parse(path, error_pos);
        if (!jp) {
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(root);
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("Invalid JSON path expression. The error is around character position %d.", error_pos)));
        }
        cJSON_JsonPathMatch(root, jp, res);
        if (res->len == 1) {
            json = cstring_to_text(cJSON_PrintUnformatted(res->head->next->node));
        } else {
            PG_RETURN_NULL();
        }
        cJSON_DeleteJsonPath(jp);
        cJSON_DeleteResultWrapper(res);
    }
    JsonLexContext *lex = makeJsonLexContext(json, true);
    OkeysState *state = (OkeysState *)palloc(sizeof(OkeysState));
    JsonSemAction *sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

    state->lex = lex;
    state->result_count = 0;
    sem->semstate = (void *)state;
    sem->object_field_start = length_object_field_start;
    sem->array_element_start = length_array_element_start;
    sem->scalar = length_scalar;

    pg_parse_json(lex, sem);
    result = state->result_count;
    pfree(state);
    pfree(sem);
    PG_RETURN_INT32(result);
}

Datum json_objectagg_finalfn(PG_FUNCTION_ARGS)
{
    StringInfo state;
    cJSON *root = NULL;

    /* cannot be called directly because of internal-type argument */
    Assert(AggCheckCallContext(fcinfo, NULL));
    state = PG_ARGISNULL(0) ? NULL : (StringInfo)PG_GETARG_POINTER(0);
    if (state == NULL) {
        PG_RETURN_NULL();
    }

    appendStringInfoString(state, " }");
    root = cJSON_Parse(state->data);
    resetStringInfo(state);
    json_regular_format(state, root);
    cJSON_Delete(root);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(state->data, state->len));
}

Datum json_storage_size(PG_FUNCTION_ARGS)
{
    Oid valtype;
    Datum arg = 0;
    cJSON *doc_cJSON = NULL;
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    doc_cJSON = input_to_cjson(valtype, "json_storage_size", 1, arg);
    int32 n = pg_column_size(fcinfo);
    PG_RETURN_INT32(n);
}

static void newline_and_indent(StringInfo buf, int depth)
{
    appendStringInfoChar(buf, '\n');
    for (int i = 0; i < depth; i++) {
        appendStringInfoChar(buf, ' ');
    }
}

static text *prettyJsondoc(char *str)
{
    StringInfo buf = makeStringInfo();
    text *res;
    bool quoted = false;
    int depth = 0;
    appendStringInfoString(buf, "  ");
    while (*str != '\0') {
        if (!quoted) {
            switch (*str) {
                case '\"':
                    quoted = true;
                    appendStringInfoChar(buf, '"');
                    break;
                case ',':
                    appendStringInfoChar(buf, ',');
                    newline_and_indent(buf, depth);
                    break;
                case ':':
                    appendStringInfoChar(buf, ':');
                    appendStringInfoChar(buf, ' ');
                    break;
                case '{':
                    if (*(str + 1) != '}') {
                        depth += 2;
                        appendStringInfoChar(buf, '{');
                        newline_and_indent(buf, depth);
                    } else {
                        appendStringInfoString(buf, "{}");
                        str++;
                    }
                    break;
                case '[':
                    if (*(str + 1) != ']') {
                        depth += 2;
                        appendStringInfoChar(buf, '[');
                        newline_and_indent(buf, depth);
                    } else {
                        appendStringInfoString(buf, "[]");
                        str++;
                    }
                    break;
                case '}':
                    depth -= 2;
                    newline_and_indent(buf, depth);
                    appendStringInfoChar(buf, '}');
                    break;
                case ']':
                    depth -= 2;
                    newline_and_indent(buf, depth);
                    appendStringInfoChar(buf, ']');
                    break;
                default:
                    appendStringInfoChar(buf, *str);
                    break;
            }
        } else {
            switch (*str) {
                case '"':
                    quoted = false;
                    appendStringInfoChar(buf, '"');
                    break;
                case '\\':
                    str++;
                    appendStringInfoChar(buf, '\\');
                    break;
                default:
                    appendStringInfoChar(buf, *str);
                    break;
            }
        }
        str++;
    }
    res = cstring_to_text_with_len(buf->data, buf->len);
    DestroyStringInfo(buf);
    return res;
}

Datum json_pretty(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();
    Oid valtype;
    Datum arg = 0;
    cJSON *root = NULL;
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    arg = PG_GETARG_DATUM(0);
    root = input_to_cjson(valtype, "json_pretty", 1, arg);
    cJSON_SortObject(root);
    char *unformattedJson = cJSON_PrintUnformatted(root);
    text *res = prettyJsondoc(unformattedJson);
    cJSON_free(unformattedJson);
    cJSON_Delete(root);
    PG_RETURN_TEXT_P(res);
}
#endif
