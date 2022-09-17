/* -------------------------------------------------------------------------
 *
 * jsonfuncs.c
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
#include "utils/jsonapi.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/typcache.h"

#ifdef DOLPHIN
#include "cjson/cJSON.h"
#include "plugin_postgres.h"
#include "plugin_utils/json.h"
#endif

#ifdef DOLPHIN
#define cJSON_JsonPath_Invalid (0)
#define cJSON_JsonPath_Start (1)
#define cJSON_JsonPath_Key (1 << 1)
#define cJSON_JsonPath_Index (1 << 2)
#define cJSON_JsonPath_AnyKey (1 << 3)
#define cJSON_JsonPath_AnyIndex (1 << 4)
#define cJSON_JsonPath_Any (1 << 5)
#define cJSON_JsonPath_IndexRanger (1 << 6)
#define VALTYPE_IS_JSON(v) \
    ((v) == UNKNOWNOID || (v) == JSONOID || (v) == JSONBOID || (v) == CSTRINGOID || (v) == TEXTOID)

#define NextByte(p, plen) ((p)++, (plen)--)
/* Set up to compile like_match.c for single-byte characters */
#define CHAREQ(p1, p2) (*(p1) == *(p2))
#define NextChar(p, plen) NextByte((p), (plen))
#define CopyAdvChar(dst, src, srclen) (*(dst)++ = *(src)++, (srclen)--)
#define do_like_escape MB_do_like_escape
#define MatchText MB_MatchText
#include "utils/pg_locale.h"
#include "like_match.cpp"

typedef struct cJSON_JsonPath {
    struct cJSON_JsonPath *next;

    int type;
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
static int error_pos = -1;

static bool jp_match(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static cJSON *jp_match_object(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static cJSON *jp_match_array(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
static void jp_match_any(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res, bool skipNested);

static inline int get_space_skipped_index(const char *data, int start);
static inline int get_end(const char *data, int start);
static inline int get_mark(const char *data, int start, const char mark);
static cJSON_JsonPath *jp_parse(const char *data);
static cJSON_JsonPath *jp_parse_key(const char *data, int *idx);
static cJSON_JsonPath *jp_parse_index(const char *data, int *idx);
static cJSON_JsonPath *jp_parse_any(const char *data, int *idx);

void cJSON_SwapItemValue(cJSON *item1, cJSON *item2);
void quicksort(cJSON *item1, cJSON *item2);

cJSON_JsonPath *cJSON_JsonPathParse(const char *data);
cJSON_JsonPath *cJSON_CreateJsonPath();

bool cJSON_JsonPathMatch(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res);
void cJSON_DeleteJsonPath(cJSON_JsonPath *head);
bool cJSON_AddItemToJsonPath(cJSON_JsonPath *jp, cJSON_JsonPath *item);
bool cJSON_AddItemToResultWrapper(cJSON_ResultWrapper *res, cJSON *item);
cJSON_ResultWrapper *cJSON_CreateResultWrapper();
void cJSON_ResultWrapperInit(cJSON_ResultWrapper *res);
void cJSON_DeleteResultWrapper(cJSON_ResultWrapper *res);
cJSON *cJSON_ResultWrapperToArray(cJSON_ResultWrapper *res);
void cJSON_SortObject(cJSON *object);
int cJSON_JsonPathGetError();
bool cJSON_JsonPathCanMatchMany(cJSON_JsonPath *jp);
cJSON_bool cJSON_JsonInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value);
static inline cJSON_JsonPath *jp_pop(cJSON_JsonPath *jp);
text *formatJsondoc(char *str);

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
            case cJSON_JsonPath_AnyIndex:
                jp_match_any(item, jp, res, true);
                jp = NULL;
                break;
            case cJSON_JsonPath_Any:
                jp_match(item, jp->next, res);
                jp_match_any(item, jp, res, false);
                jp = NULL;
                break;
            default:
                break;
        }
    }
    return (res->len > old_len) ? true : false;
}

static void jp_match_any(cJSON *item, cJSON_JsonPath *jp, cJSON_ResultWrapper *res, bool skipNested)
{
    cJSON *ele = NULL;
    bool is_last = (jp->next == NULL);
    cJSON_ArrayForEach(ele, item)
    {
        if (is_last && skipNested) {
            cJSON_AddItemToResultWrapper(res, ele);
        }
        jp_match(ele, jp->next, res);
        if (!skipNested) {
            jp_match_any(ele, jp, res, skipNested);
        }
    }
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

static cJSON_JsonPath *jp_parse(const char *data)
{
    int i = 0;
    cJSON_JsonPath *jp = NULL;
    cJSON_JsonPath *tmp = NULL;
    i = get_space_skipped_index(data, i);
    if (data[i] == '$') {
        jp = cJSON_CreateJsonPath();
        jp->type = cJSON_JsonPath_Start;
        i++;
    } else {
        goto fail;
    }
    while (data[i] != '\0') {
        switch (data[i]) {
            case '.':
                tmp = jp_parse_key(data, &i);
                if (tmp != NULL) {
                    cJSON_AddItemToJsonPath(jp, tmp);
                } else {
                    goto fail;
                }
                break;
            case '[':
                tmp = jp_parse_index(data, &i);
                if (tmp != NULL) {
                    cJSON_AddItemToJsonPath(jp, tmp);
                } else {
                    goto fail;
                }
                break;
            case '*':
                tmp = jp_parse_any(data, &i);
                if (tmp != NULL) {
                    cJSON_AddItemToJsonPath(jp, tmp);
                } else {
                    goto fail;
                }
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                i++;
                break;
            default:
                goto fail;
                break;
        }
    }
    return jp;
fail:
    cJSON_DeleteJsonPath(jp);
    error_pos = i;
    return NULL;
}

static cJSON_JsonPath *jp_parse_any(const char *data, int *idx)
{
    int i = *idx + 1;
    int n = get_space_skipped_index(data, i + 1);
    cJSON_JsonPath *jpItem = NULL;
    if (data[i] == '*' && data[n] != '*') {
        jpItem = cJSON_CreateJsonPath();
        jpItem->type = cJSON_JsonPath_Any;
        *idx = i + 1;
    }
    return jpItem;
}

static cJSON_JsonPath *jp_parse_key(const char *data, int *idx)
{
    int i = *idx + 1;
    errno_t rc = 0;
    cJSON_JsonPath *item = NULL;
    int end = -1;
    i = get_space_skipped_index(data, i);
    if (data[i] == '"') {
        i++;
        end = get_mark(data, i, '"');
        if (end > i) {
            item = cJSON_CreateJsonPath();
            item->type = cJSON_JsonPath_Key;
            item->key = (char *)cJSON_malloc(sizeof(char) * (end - i + 1));
            rc = strncpy_s(item->key, end - i + 1, data + i, end - i);
            securec_check(rc, "\0", "\0");
            item->key[end - i] = '\0';
            *idx = end + 1;
        }
    } else if (data[i] == '*') {
        item = cJSON_CreateJsonPath();
        item->type = cJSON_JsonPath_AnyKey;
        *idx = i + 1;
    } else {
        end = get_end(data, i);
        item = cJSON_CreateJsonPath();
        item->type = cJSON_JsonPath_Key;
        item->key = (char *)cJSON_malloc(sizeof(char) * (end - i + 1));
        rc = strncpy_s(item->key, end - i + 1, data + i, end - i);
        securec_check(rc, "\0", "\0");
        item->key[end - i] = '\0';
        *idx = end;
    }
    return item;
}

static cJSON_JsonPath *jp_parse_index(const char *data, int *idx)
{
    int i = *idx + 1;
    long num = 0;
    char *endptr = NULL;
    int end = -1;
    int n = -1;
    cJSON_JsonPath *item = NULL;

    end = get_mark(data, i, ']');
    i = get_space_skipped_index(data, i);
    num = strtol(data + i, &endptr, 10);

    n = get_space_skipped_index(endptr, 0);
    if ((*endptr == ']' || endptr[n] == ']') && end > i) {
        item = cJSON_CreateJsonPath();
        item->type = cJSON_JsonPath_Index;
        item->index = (int)num;
        *idx = end + 1;
    } else if (*(endptr) == '*' || *(endptr + n) == '*') {
        endptr++;
        n = get_space_skipped_index(endptr, 0);
        if (*endptr == ']' || endptr[n] == ']') {
            item = cJSON_CreateJsonPath();
            item->type = cJSON_JsonPath_AnyIndex;
            *idx = end + 1;
        }
    }
    return item;
}

static inline int get_space_skipped_index(const char *data, int start)
{
    int i = start;
    while ((data + i) && *(data + i) && *(data + i) <= 32) {
        i++;
    }
    return i;
}

static inline int get_end(const char *data, int start)
{
    int i = 0;
    for (i = start; data[i]; i++) {
        switch (data[i]) {
            case ' ':
            case '\t':
            case '.':
            case '"':
            case '[':
                return i;
        }
    }
    return i;
}

static inline int get_mark(const char *data, int start, const char mark)
{
    int i = 0;
    for (i = start; data[i] != '\0'; i++) {
        if (data[i] == mark) {
            return i;
        }
    }
    return (data[i] == mark) ? i : (-1);
}

cJSON_JsonPath *cJSON_JsonPathParse(const char *data)
{
    return jp_parse(data);
}

bool cJSON_JsonPathMatch(cJSON *item, cJSON_JsonPath *head, cJSON_ResultWrapper *res)
{
    if ((head == NULL) || (item == NULL) || (head->type != cJSON_JsonPath_Start)) {
        return false;
    }
    return jp_match(item, head, res);
}

cJSON_JsonPath *cJSON_CreateJsonPath()
{
    cJSON_JsonPath *jp = (cJSON_JsonPath *)cJSON_malloc(sizeof(cJSON_JsonPath));
    if (jp) {
        errno_t rc = memset_s(jp, sizeof(cJSON_JsonPath), 0, sizeof(cJSON_JsonPath));
        securec_check(rc, "\0", "\0");
    }
    return jp;
}

void cJSON_DeleteJsonPath(cJSON_JsonPath *jp)
{
    cJSON_JsonPath *next = NULL;
    while (jp) {
        next = jp->next;
        if (jp->key) {
            cJSON_free(jp->key);
        }
        cJSON_free(jp);
        jp = next;
    }
}

bool cJSON_AddItemToJsonPath(cJSON_JsonPath *jp, cJSON_JsonPath *item)
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

void cJSON_ResultWrapperInit(cJSON_ResultWrapper *res)
{
    cJSON_ResultNode *resnode = (cJSON_ResultNode *)cJSON_malloc(sizeof(cJSON_ResultNode));
    resnode->next = NULL;
    resnode->node = NULL;
    res->head = resnode;
    res->len = 0;
}

cJSON_ResultWrapper *cJSON_CreateResultWrapper()
{
    cJSON_ResultWrapper *res = (cJSON_ResultWrapper *)cJSON_malloc(sizeof(cJSON_ResultWrapper));
    cJSON_ResultWrapperInit(res);
    return res;
}

bool cJSON_AddItemToResultWrapper(cJSON_ResultWrapper *res, cJSON *item)
{
    if (!item) {
        return false;
    }
    cJSON_ResultNode *tmp = (cJSON_ResultNode *)cJSON_malloc(sizeof(cJSON_ResultNode));
    tmp->node = item;
    cJSON_ResultNode *head = res->head;
    tmp->next = head->next;
    head->next = tmp;
    res->len++;
    return true;
}
cJSON *cJSON_ResultWrapperToArray(cJSON_ResultWrapper *res)
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
void cJSON_DeleteResultWrapper(cJSON_ResultWrapper *res)
{
    cJSON_ResultNode *head = res->head;
    cJSON_ResultNode *next = NULL;

    while (head) {
        next = head->next;
        cJSON_free(head);
        head = next;
    }

    cJSON_free(res);
}
void cJSON_SortObject(cJSON *object)
{
    cJSON *start = NULL;
    cJSON *end = NULL;
    cJSON *child = NULL;
    if (object->type == cJSON_Object) {
        start = object->child;
        end = object->child->prev;
        quicksort(start, end);
    }
    cJSON_ArrayForEach(child, object)
    {
        cJSON_SortObject(child);
    }
}

void quicksort(cJSON *start, cJSON *end)
{
    if (start == NULL || start == end)
        return;
    cJSON *q = end;
    cJSON *p = start;
    while (q != p) {
        if (strcmp(start->string, q->string) > 0) {
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

void cJSON_SwapItemValue(cJSON *item1, cJSON *item2)
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

int cJSON_JsonPathGetError()
{
    int p = error_pos;
    error_pos = -1;
    return p;
}

bool cJSON_JsonPathCanMatchMany(cJSON_JsonPath *jp)
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
#endif
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
static text *json_merge_patch_unit(text *j1, text *j2);
static text *json_merge_preserve_unit(text *j1, text *j2);
static text *json_regular_format(text *json);
static OkeysState *get_object_key_info(text *json);
static void free_object_key_info(OkeysState *state);
static int put_object_keys_into_set(char **keys, OkeysState *cState, OkeysState *tState);
static int put_object_keys_into_set(char **keys, OkeysState *tState);
static void appendObject(StringInfo result, text *json, int cnt, char **keys, int *pos);
static void appendStringInfoObject(StringInfo result, text *json);
static void appendStringInfoArray(StringInfo result, text *json);
static void appendStringInfoArrayEle(StringInfo result, text *json);
static void appendStringInfoScalar(StringInfo result, text *json);
static void appendStringInfoObjectsPatch(StringInfo result, text *j1, text *j2);
static void appendStringInfoObjectsPreserve(StringInfo result, text *j1, text *j2);
static bool json_array_null(text *json);
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
Datum json_object_field(PG_FUNCTION_ARGS)
{
    text *json = PG_GETARG_TEXT_P(0);
    text *result = NULL;
    text *fname = PG_GETARG_TEXT_P(1);
    char *fnamestr = text_to_cstring(fname);

    result = get_worker(json, fnamestr, -1, NULL, NULL, -1, false);

    if (result != NULL) {
        PG_RETURN_TEXT_P(result);
    } else {
        PG_RETURN_NULL();
    }
}

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

Datum json_object_field_text(PG_FUNCTION_ARGS)
{
    text *json = PG_GETARG_TEXT_P(0);
    text *result = NULL;
    text *fname = PG_GETARG_TEXT_P(1);
    char *fnamestr = text_to_cstring(fname);

    result = get_worker(json, fnamestr, -1, NULL, NULL, -1, true);

    if (result != NULL) {
        PG_RETURN_TEXT_P(result);
    } else {
        PG_RETURN_NULL();
    }
}

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
        Oid column_type = tupdesc->attrs[i]->atttypid;
        JsonbValue *v = NULL;
        char fname[NAMEDATALEN];
        JsonHashEntry *hashentry = NULL;

        /* Ignore dropped columns in datatype */
        if (tupdesc->attrs[i]->attisdropped) {
            nulls[i] = true;
            continue;
        }

        if (jtype == JSONOID) {
            rc = memset_s(fname, NAMEDATALEN, 0, NAMEDATALEN);
            securec_check(rc, "\0", "\0");
            rc = strncpy_s(fname, NAMEDATALEN, NameStr(tupdesc->attrs[i]->attname), NAMEDATALEN - 1);
            securec_check(rc, "\0", "\0");
            hashentry = (JsonHashEntry *)hash_search(json_hash, fname, HASH_FIND, NULL);
        } else {
            char *key = NameStr(tupdesc->attrs[i]->attname);
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
            values[i] =
                InputFunctionCall(&column_info->proc, NULL, column_info->typioparam, tupdesc->attrs[i]->atttypmod);
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

            values[i] = InputFunctionCall(&column_info->proc, s, column_info->typioparam, tupdesc->attrs[i]->atttypmod);
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
        Oid column_type = tupdesc->attrs[i]->atttypid;
        JsonbValue *v = NULL;
        char *key = NULL;

        /* Ignore dropped columns in datatype */
        if (tupdesc->attrs[i]->attisdropped) {
            nulls[i] = true;
            continue;
        }
        key = NameStr(tupdesc->attrs[i]->attname);
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
            values[i] =
                InputFunctionCall(&column_info->proc, NULL, column_info->typioparam, tupdesc->attrs[i]->atttypmod);
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

            values[i] = InputFunctionCall(&column_info->proc, s, column_info->typioparam, tupdesc->attrs[i]->atttypmod);
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
            /* using the arg tupdesc, becouse it may not be the same as the result tupdesc. */
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
        Oid column_type = tupdesc->attrs[i]->atttypid;
        char *value = NULL;

        /* Ignore dropped columns in datatype */
        if (tupdesc->attrs[i]->attisdropped) {
            nulls[i] = true;
            continue;
        }

        errno_t rc = memset_s(fname, NAMEDATALEN, 0, NAMEDATALEN);
        securec_check(rc, "\0", "\0");
        rc = strncpy_s(fname, NAMEDATALEN, NameStr(tupdesc->attrs[i]->attname), NAMEDATALEN - 1);
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
            values[i] =
                InputFunctionCall(&column_info->proc, NULL, column_info->typioparam, tupdesc->attrs[i]->atttypmod);
            nulls[i] = true;
        } else {
            value = hashentry->val;
            values[i] =
                InputFunctionCall(&column_info->proc, value, column_info->typioparam, tupdesc->attrs[i]->atttypmod);
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
typedef enum {
    JSON_INVALID,
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_BOOL,
    JSON_NULL,
} JsonType;

static JsonType json_type(text *json)
{
    char *type = text_to_cstring(DatumGetTextP(DirectFunctionCall1(json_typeof, PointerGetDatum(json))));
    if (strcmp(type, "object") == 0)
        return JSON_OBJECT;
    else if (strcmp(type, "array") == 0)
        return JSON_ARRAY;
    else if (strcmp(type, "string") == 0)
        return JSON_STRING;
    else if (strcmp(type, "number") == 0)
        return JSON_NUMBER;
    else if (strcmp(type, "boolean") == 0)
        return JSON_BOOL;
    else if (strcmp(type, "null") == 0)
        return JSON_NULL;
    else
        return JSON_INVALID;
}

static bool json_contains_unit(text *target, text *candidate)
{
    JsonType tType = json_type(target);
    JsonType cType = json_type(candidate);

    switch (tType) {
        case JSON_OBJECT: {
            switch (cType) {
                case JSON_OBJECT: {
                    int i;
                    int j;
                    int k;
                    int l;
                    OkeysState *tState = NULL;

                    JsonLexContext *ttLex = makeJsonLexContext(target, true);
                    JsonSemAction *tSem = NULL;

                    tState = (OkeysState *)palloc(sizeof(OkeysState));
                    tSem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

                    tState->lex = ttLex;
                    tState->result_size = 256;
                    tState->result_count = 0;
                    tState->sent_count = 0;
                    tState->result = (char **)palloc(256 * sizeof(char *));

                    tSem->semstate = (void *)tState;
                    tSem->array_start = okeys_array_start;
                    tSem->scalar = okeys_scalar;
                    tSem->object_field_start = okeys_object_field_start;
                    /* remainder are all NULL, courtesy of palloc0 above */
                    pg_parse_json(ttLex, tSem);
                    /* keys are now in state->result */
                    pfree(ttLex->strval->data);
                    pfree(ttLex->strval);
                    pfree(ttLex);
                    pfree(tSem);

                    OkeysState *cState = NULL;

                    JsonLexContext *ccLex = makeJsonLexContext(candidate, true);
                    JsonSemAction *cSem = NULL;

                    cState = (OkeysState *)palloc(sizeof(OkeysState));
                    cSem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

                    cState->lex = ccLex;
                    cState->result_size = 256;
                    cState->result_count = 0;
                    cState->sent_count = 0;
                    cState->result = (char **)palloc(256 * sizeof(char *));

                    cSem->semstate = (void *)cState;
                    cSem->array_start = okeys_array_start;
                    cSem->scalar = okeys_scalar;
                    cSem->object_field_start = okeys_object_field_start;
                    /* remainder are all NULL, courtesy of palloc0 above */
                    pg_parse_json(ccLex, cSem);
                    /* keys are now in state->result */
                    pfree(ccLex->strval->data);
                    pfree(ccLex->strval);
                    pfree(ccLex);
                    pfree(cSem);

                    for (l = 0; l < cState->result_count; l++) {
                        bool flag = false;
                        for (k = 0; k < tState->result_count; k++) {
                            if (strcmp(cState->result[l], tState->result[k]) == 0) {
                                text *cResult = get_worker(candidate, cState->result[l], -1, NULL, NULL, -1, false);
                                text *tResult = get_worker(target, tState->result[k], -1, NULL, NULL, -1, false);
                                if (json_contains_unit(tResult, cResult)) {
                                    flag = true;
                                }
                            }
                        }
                        if (!flag) {
                            break;
                        }
                    }

                    int cCount = cState->result_count;

                    /* cleanup to reduce or eliminate memory leaks */
                    for (j = 0; j < cState->result_count; j++) {
                        pfree(cState->result[j]);
                    }
                    pfree(cState->result);
                    pfree(cState);

                    /* cleanup to reduce or eliminate memory leaks*/
                    for (i = 0; i < tState->result_count; i++) {
                        pfree(tState->result[i]);
                    }
                    pfree(tState->result);
                    pfree(tState);

                    if (l == cCount) {
                        return true;
                    }

                    break;
                }
                case JSON_ARRAY:
                case JSON_STRING:
                case JSON_NUMBER:
                case JSON_BOOL:
                case JSON_NULL: {
                    return false;

                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", cType);
            }

            break;
        }
        case JSON_ARRAY: {
            switch (cType) {
                case JSON_ARRAY: {
                    int arr_arr_cand_iter = 0;
                    text *candidateEle = NULL;
                    while (true) {
                        candidateEle = get_worker(candidate, NULL, arr_arr_cand_iter, NULL, NULL, -1, false);
                        if (candidateEle == NULL) {
                            return true;
                        }

                        int arr_arr_targ_iter = 0;
                        bool flag = false;
                        text *targetEle = NULL;
                        while (true) {
                            targetEle = get_worker(target, NULL, arr_arr_targ_iter, NULL, NULL, -1, false);
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
                case JSON_OBJECT:
                case JSON_STRING:
                case JSON_NUMBER:
                case JSON_BOOL:
                case JSON_NULL: {
                    int arr_scalar_targ_iter = 0;
                    while (true) {
                        text *result = get_worker(target, NULL, arr_scalar_targ_iter, NULL, NULL, -1, false);
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
        case JSON_STRING:
        case JSON_NUMBER:
        case JSON_BOOL:
        case JSON_NULL: {
            switch (cType) {
                case JSON_OBJECT:
                case JSON_ARRAY: {
                    break;
                }
                case JSON_STRING:
                case JSON_NUMBER:
                case JSON_BOOL:
                case JSON_NULL: {
                    if (strcmp(text_to_cstring(target), text_to_cstring(candidate)) == 0) {
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
Datum json_contains(PG_FUNCTION_ARGS)
{
    text *target = PG_GETARG_TEXT_P(0);
    text *candidate = PG_GETARG_TEXT_P(1);
    text *result = NULL;
    char *path = NULL;
    char *data = text_to_cstring(target);
    cJSON_JsonPath *jp = NULL;
    int errpos = -1;
    char *s = NULL;

    if (PG_NARGS() == 2) {
        if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
            PG_RETURN_NULL();
        if (json_contains_unit(target, candidate))
            PG_RETURN_BOOL(true);
    } else {
        if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2))
            PG_RETURN_NULL();

        cJSON_ResultWrapper *res = cJSON_CreateResultWrapper();
        cJSON *root = cJSON_Parse(data);
        if (DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), PG_GETARG_DATUM(2),
                                                  CStringGetTextDatum("(\\*)(?=([^\"]|\"[^\"]*\")*$)")))) {
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("in this situation, path expressions may not contain the * and ** tokens")));
        }

        path = TextDatumGetCString(PG_GETARG_DATUM(2));
        jp = cJSON_JsonPathParse(path);

        errpos = cJSON_JsonPathGetError();
        if (errpos >= 0) {
            cJSON_DeleteJsonPath(jp);
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid jsonpath expression")));
        }

        cJSON_JsonPathMatch(root, jp, res);
        cJSON_DeleteJsonPath(jp);

        if (res->len == 1) {
            s = cJSON_PrintUnformatted(res->head->next->node);
            result = cstring_to_text(s);
            cJSON_free(s);
        }

        cJSON_Delete(root);
        cJSON_DeleteResultWrapper(res);

        if (result == NULL) {
            PG_RETURN_NULL();
        }

        PG_RETURN_BOOL(json_contains_unit(result, candidate));
    }

    PG_RETURN_BOOL(false);
}
Datum json_contains_path(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    text *target = PG_GETARG_TEXT_P(0);
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
    char *data = text_to_cstring(target);
    cJSON_ResultWrapper *res = cJSON_CreateResultWrapper();
    cJSON_JsonPath *jp = NULL;
    cJSON *root = cJSON_Parse(data);
    int errpos = -1;
    int last_len = 0;

    if (strcmp(mode, "one") == 0) {
        for (int i = 0; i < path_num; i++) {
            if (pathnulls[i]) {
                cJSON_Delete(root);
                cJSON_DeleteResultWrapper(res);
                PG_RETURN_NULL();
            }
            path = TextDatumGetCString(pathtext[i]);
            jp = cJSON_JsonPathParse(path);

            errpos = cJSON_JsonPathGetError();
            if (errpos >= 0) {
                cJSON_DeleteJsonPath(jp);
                cJSON_DeleteResultWrapper(res);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid jsonpath expression")));
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
            jp = cJSON_JsonPathParse(path);

            errpos = cJSON_JsonPathGetError();
            if (errpos >= 0) {
                cJSON_DeleteJsonPath(jp);
                cJSON_DeleteResultWrapper(res);
                cJSON_Delete(root);
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid jsonpath expression")));
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
    text *json = PG_GETARG_TEXT_P(0);
    ArrayType *in_array = PG_GETARG_ARRAYTYPE_P(1);
    text *result = NULL;
    Datum resultFormatted;
    Datum *in_datums = NULL;
    bool *in_nulls = NULL;
    int in_count;
    char *path = NULL;
    char *data = text_to_cstring(json);
    cJSON_ResultWrapper *res = NULL;
    cJSON_JsonPath *jp = NULL;
    cJSON *root = NULL;
    int errpos = -1;
    char *r = NULL;
    bool many = false;

    if (array_contains_nulls(in_array)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot call function with null path elements")));
    }

    deconstruct_array(in_array, TEXTOID, -1, false, 'i', &in_datums, &in_nulls, &in_count);

    res = cJSON_CreateResultWrapper();
    root = cJSON_Parse(data);

    for (int i = 0; i < in_count; i++) {
        path = TextDatumGetCString(in_datums[i]);
        jp = cJSON_JsonPathParse(path);
        many |= cJSON_JsonPathCanMatchMany(jp);
        errpos = cJSON_JsonPathGetError();
        if (errpos >= 0) {
            cJSON_DeleteJsonPath(jp);
            cJSON_DeleteResultWrapper(res);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid jsonpath expression")));
        }
        cJSON_JsonPathMatch(root, jp, res);
        cJSON_DeleteJsonPath(jp);
    }
    if (res->len > 0) {
        if (many) {
            cJSON *arr = cJSON_ResultWrapperToArray(res);
            cJSON_SortObject(arr);
            r = cJSON_PrintUnformatted(arr);
            result = cstring_to_text(r);
            cJSON_Delete(arr);
        } else {
            r = cJSON_PrintUnformatted(res->head->next->node);
            result = cstring_to_text(r);
        }
        cJSON_free(r);
    }

    cJSON_Delete(root);
    cJSON_DeleteResultWrapper(res);

    if (result != NULL) {
        resultFormatted = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), PointerGetDatum(result),
                                                  CStringGetTextDatum(",(?=([^\"]|\"[^\"]*\")*$)"),
                                                  CStringGetTextDatum(", "), CStringGetTextDatum("g"));

        resultFormatted = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), resultFormatted,
                                                  CStringGetTextDatum(":(?=([^\"]|\"[^\"]*\")*$)"),
                                                  CStringGetTextDatum(": "), CStringGetTextDatum("g"));
        PG_RETURN_TEXT_P(DatumGetTextP(resultFormatted));
    } else {
        /* null is NULL, regardless */
        PG_RETURN_NULL();
    }
}

#endif

#ifdef DOLPHIN
void search_PushStack(search_LinkStack &s, ElemType x)
{
    LinkNode *newnode = (LinkNode *)malloc(sizeof(LinkNode));
    newnode->data = x;
    newnode->next = s;
    s = newnode;
}
void search_PopStack(search_LinkStack &s)
{
    LinkNode *p = s;
    s = s->next;
    free(p);
}
void search_CleanStack(search_LinkStack &s)
{
    while (s != NULL) {
        search_PopStack(s);
    }
}

static Datum cut_path(Datum path, FunctionCallInfo fcinfo)
{

    Datum first_cut = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), path,
                                              CStringGetTextDatum("\\s(?=([^\"]|\"[^\"]*\")*$)"),
                                              CStringGetTextDatum("\0"), CStringGetTextDatum("g"));

    if (DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), first_cut,
                                              CStringGetTextDatum("(\\$)(?=([^\"]|\"[^\"]*\")*$)"))) != 1) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    if (text_to_cstring(DatumGetTextP(first_cut))[0] != '$') {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    Datum second_cut = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), first_cut,
                                               CStringGetTextDatum("\\$(?=([^\"]|\"[^\"]*\")*$)"),
                                               CStringGetTextDatum("\0"), CStringGetTextDatum("g"));

    if (DatumGetInt32(DirectFunctionCall2Coll(
            regexp_count_noopt, PG_GET_COLLATION(), second_cut,
            CStringGetTextDatum("((\\[)[^\\[\\]]*[^\\[\\d\\]]+[^\\[\\]]*(\\]))(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    if (DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), second_cut,
                                              CStringGetTextDatum("(\\.\\d+)(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    if (DatumGetInt32(DirectFunctionCall2Coll(
            regexp_count_noopt, PG_GET_COLLATION(), second_cut,
            CStringGetTextDatum("(\\[[^\\[\\]]*[\\[\\]][^\\[\\]]*\\])(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    if (DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), second_cut,
                                              CStringGetTextDatum("(\\[\\])(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    int32 match_brackets_num =
        DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), second_cut,
                                              CStringGetTextDatum("(\\[[^\\]\\]]*\\])(?=([^\"]|\"[^\"]*\")*$)")));

    int32 left_brackets_num = DatumGetInt32(DirectFunctionCall2Coll(
        regexp_count_noopt, PG_GET_COLLATION(), second_cut, CStringGetTextDatum("(\\[)(?=([^\"]|\"[^\"]*\")*$)")));

    int32 right_brackets_num = DatumGetInt32(DirectFunctionCall2Coll(
        regexp_count_noopt, PG_GET_COLLATION(), second_cut, CStringGetTextDatum("(\\])(?=([^\"]|\"[^\"]*\")*$)")));

    if (match_brackets_num != left_brackets_num || match_brackets_num != right_brackets_num) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    Datum third_cut = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), second_cut,
                                              CStringGetTextDatum("\\](?=([^\"]|\"[^\"]*\")*$)"),
                                              CStringGetTextDatum("\0"), CStringGetTextDatum("g"));
    Datum forth_cut = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), third_cut,
                                              CStringGetTextDatum("\\[(?=([^\"]|\"[^\"]*\")*$)"),
                                              CStringGetTextDatum("."), CStringGetTextDatum("g"));

    if (DatumGetInt32(
            DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), forth_cut,
                                    CStringGetTextDatum("(([^\\.\\*]\\*)|(\\*[^\\.\\*]))(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    if (DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), forth_cut,
                                              CStringGetTextDatum("(\\*{3,})(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    if (DatumGetInt32(DirectFunctionCall2Coll(regexp_count_noopt, PG_GET_COLLATION(), forth_cut,
                                              CStringGetTextDatum("(\\.\\*\\*)(?=([^\"]|\"[^\"]*\")*$)")))) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    Datum fifth_cut = DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), forth_cut,
                                              CStringGetTextDatum("(\\*\\*)(?=([^\"]|\"[^\"]*\")*$)"),
                                              CStringGetTextDatum(".**"), CStringGetTextDatum("g"));

    char tempchar = text_to_cstring(DatumGetTextP(fifth_cut))[0];
    if (tempchar != '.' && tempchar != '\0') {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid JSON path expression")));
    }

    return fifth_cut;
}

text *by_path(text *target, Datum path_cut, FunctionCallInfo fcinfo)
{
    int npath = 0;
    char **tpath = NULL;
    int *ipath = NULL;
    int i;
    long ind;
    char *endptr = NULL;
    ArrayType *all_tokens =
        DatumGetArrayTypeP(DirectFunctionCall2Coll(regexp_split_to_array_no_flags, PG_GET_COLLATION(), path_cut,
                                                   CStringGetTextDatum("(\\.)(?=([^\"]|\"[^\"]*\")*$)")));

    Datum *path_token_text = NULL;
    bool *path_token_nulls = NULL;
    int path_token_num;

    if (array_contains_nulls(all_tokens)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("path must have tokens")));
    }

    deconstruct_array(all_tokens, TEXTOID, -1, false, 'i', &path_token_text, &path_token_nulls, &path_token_num);

    npath = path_token_num - 1;

    if (npath <= 0)
        return target;

    tpath = (char **)palloc(npath * sizeof(char *));
    ipath = (int *)palloc(npath * sizeof(int));

    for (i = 0; i < npath; i++) {
        tpath[i] = text_to_cstring(DatumGetTextP(
            DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), path_token_text[i + 1],
                                    CStringGetTextDatum("\""), CStringGetTextDatum("\0"), CStringGetTextDatum("g"))));
        if (*tpath[i] == '\0') {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("cannot call function with empty path elements")));
        }
        ind = strtol(tpath[i], &endptr, 10);
        if (*endptr == '\0' && ind <= INT_MAX && ind >= 0) {
            ipath[i] = (int)ind;
        } else {
            ipath[i] = -1;
        }
    }
    return get_worker(target, NULL, -1, tpath, ipath, npath, false);
}
search_LinkStack stk = NULL;
bool json_search_unit(text *target, text *candidate, bool mode_match, char *wildchar, char *last_position)
{
    errno_t rc = 0;
    JsonType tType = json_type(target);
    switch (tType) {
        case JSON_OBJECT: {
            OkeysState *tState = NULL;
            JsonLexContext *ttLex = makeJsonLexContext(target, true);
            JsonSemAction *tSem = NULL;
            tState = (OkeysState *)palloc(sizeof(OkeysState));
            tSem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));
            tState->lex = ttLex;
            tState->result_size = 256;
            tState->result_count = 0;
            tState->sent_count = 0;
            tState->result = (char **)palloc(256 * sizeof(char *));
            tSem->semstate = (void *)tState;
            tSem->array_start = okeys_array_start;
            tSem->scalar = okeys_scalar;
            tSem->object_field_start = okeys_object_field_start;
            /* keys are now in state->result */
            pg_parse_json(ttLex, tSem);
            pfree(ttLex->strval->data);
            pfree(ttLex->strval);
            pfree(ttLex);
            pfree(tSem);
            text *tResult = NULL;

            for (int k = 0; k < tState->result_count; k++) {
                char *position = (char *)palloc(256);
                rc = strncpy_s(position, 256, last_position, 256);
                securec_check(rc, "\0", "\0");
                tResult = get_worker(target, tState->result[k], -1, NULL, NULL, -1, false);
                rc = strcat_s(position, 256, ".");
                securec_check(rc, "\0", "\0");
                rc = strcat_s(position, 256, tState->result[k]);
                securec_check(rc, "\0", "\0");
                if (json_search_unit(tResult, candidate, mode_match, wildchar, position)) {
                    if (mode_match == true) {
                        return true;
                    }
                    if (mode_match == false && json_type(tResult) == JSON_STRING) {
                        search_PushStack(stk, ",");
                    }
                }
            }
            if (stk != NULL) {
                return true;
            }
            for (int i = 0; i < tState->result_count; i++) {
                pfree(tState->result[i]);
            }
            pfree(tState->result);
            pfree(tState);
            break;
        }
        case JSON_ARRAY: {
            int arr_int = 0;
            text *targetEle = NULL;
            while (true) {
                targetEle = get_worker(target, NULL, arr_int, NULL, NULL, -1, false);
                if (targetEle == NULL) {
                    break;
                }
                char *position = (char *)palloc(256);
                rc = strncpy_s(position, 256, last_position, 256);
                securec_check(rc, "\0", "\0");
                char *arr_string = (char *)palloc(8);
                pg_itoa(arr_int, arr_string);
                rc = strcat_s(position, 256, "[");
                securec_check(rc, "\0", "\0");
                rc = strcat_s(position, 256, arr_string);
                securec_check(rc, "\0", "\0");
                rc = strcat_s(position, 256, "]");
                securec_check(rc, "\0", "\0");
                pfree(arr_string);
                if (json_search_unit(targetEle, candidate, mode_match, wildchar, position)) {
                    if (mode_match == true) {
                        return true;
                    }
                    if (mode_match == false && json_type(targetEle) == JSON_STRING) {
                        search_PushStack(stk, ",");
                    }
                }
                arr_int++;
            }
            if (stk != NULL) {
                return true;
            }
            break;
        }
        case JSON_STRING: {
            char *f, *b;
            int flen, blen;
            f = VARDATA_ANY(target);
            flen = VARSIZE_ANY_EXHDR(target);
            b = VARDATA_ANY(candidate);
            blen = VARSIZE_ANY_EXHDR(candidate);
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
                char *position = (char *)palloc(256);
                rc = strncpy_s(position, 256, last_position, 256);
                securec_check(rc, "\0", "\0");
                rc = strcat_s(position, 256, "\"");
                securec_check(rc, "\0", "\0");
                search_PushStack(stk, position);
                return true;
            }
        }
        default: {
            break;
        }
    }
    return false;
}

char *strrpl(char *str, char *oldstr, char *newstr)
{
    int length = strlen(str);
    char bstr[length];
    errno_t rc = memset_s(bstr, length, 0, length);
    securec_check(rc, "\0", "\0");
    for (int i = 0; i < length; i++) {
        if (!strncmp(str + i, oldstr, strlen(oldstr))) {
            rc = strcat_s(bstr, length + 1, newstr);
            securec_check(rc, "\0", "\0");
            i += strlen(oldstr) - 1;
        } else {
            rc = strncat_s(bstr, length + 1, str + i, 1);
            securec_check(rc, "\0", "\0");
        }
    }
    rc = strncpy_s(str, length + 1, bstr, length + 1);
    securec_check(rc, "\0", "\0");
    return str;
}

char *add_index(char *rpath)
{
    char *fpath = (char *)palloc(256);
    errno_t rc = strncpy_s(fpath, 256, "[", 1);
    securec_check(rc, "\0", "\0");
    rc = strcat_s(fpath, 256, rpath);
    securec_check(rc, "\0", "\0");
    rc = strcat_s(fpath, 256, "]");
    securec_check(rc, "\0", "\0");
    return fpath;
}

static char *numeric_to_cstring(Numeric n)
{
    Datum d = NumericGetDatum(n);

    return DatumGetCString(DirectFunctionCall1(numeric_out, d));
}

char *remove_duplicate_path()
{
    errno_t rc = 0;
    search_LinkStack reverse_stack = NULL;
    char *rpath = (char *)palloc(256);
    if (strcmp(stk->data, ",") == 0) {
        search_PopStack(stk);
        while (stk != NULL) {
            search_PushStack(reverse_stack, stk->data);
            search_PopStack(stk);
        }
        char temp[64][256] = {'\0'};
        int i = 0;
        while (reverse_stack != NULL) {
            rc = strncpy_s(temp[i], 256, reverse_stack->data, 256);
            securec_check(rc, "\0", "\0");
            i++;
            search_PopStack(reverse_stack);
        }

        int rpath_num = i;
        for (i = 0; i < rpath_num; i++) {
            if (strcmp(temp[i], ",") == 0) {
                i++;
            }

            for (int j = i + 1; j < rpath_num; j++) {
                if (strcmp(temp[j], ",") != 0) {
                    if (*temp[i] != '\0' && *temp[i] != '\0') {
                        if (strcmp(temp[i], temp[j]) == 0) {
                            *temp[j] = '\0';
                            *temp[j - 1] = '\0';
                        }
                    }
                }
            }
        }
        i = 0;
        rc = strncpy_s(rpath, 256, temp[i], 256);
        securec_check(rc, "\0", "\0");
        i++;
        while (*temp[i] != '\0' && i < rpath_num) {
            rc = strcat_s(rpath, 256, temp[i]);
            securec_check(rc, "\0", "\0");
            i++;
        }
        if (strstr(rpath, ",") != NULL) {
            rpath = add_index(rpath);
        }
    } else {
        rc = strncpy_s(rpath, 256, stk->data, 256);
        securec_check(rc, "\0", "\0");
    }
    return rpath;
}

Datum json_search(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2)) {
        PG_RETURN_NULL();
    }
    text *doc = PG_GETARG_TEXT_P(0);
    char *one_or_all = text_to_cstring(PG_GETARG_TEXT_P(1));
    Oid val_type;
    text *search_text;
    char *search_string = (char *)palloc(32);
    char *output_search_string = (char *)palloc(32);
    errno_t rc = memset_s(output_search_string, 32, 0, 32);
    securec_check(rc, "\0", "\0");
    val_type = get_fn_expr_argtype(fcinfo->flinfo, 2);
    if (val_type == UNKNOWNOID) {
        Datum search_datum = CStringGetTextDatum(PG_GETARG_POINTER(2));
        search_string = TextDatumGetCString(search_datum);
    } else if (val_type == INT4OID) {
        pg_itoa(DatumGetInt32(PG_GETARG_DATUM(2)), search_string);
    } else if (val_type == NUMERICOID) {
        Numeric search_numeric = DatumGetNumeric(PG_GETARG_DATUM(2));
        search_string = numeric_to_cstring(search_numeric);
    } else {
        PG_RETURN_NULL();
    }
    /* Double quotes on both sides to match. */
    rc = strncpy_s(output_search_string, 32, "\"", 2);
    securec_check(rc, "\0", "\0");
    rc = strcat_s(output_search_string, 32, search_string);
    securec_check(rc, "\0", "\0");
    rc = strcat_s(output_search_string, 32, "\"");
    securec_check(rc, "\0", "\0");
    search_text = cstring_to_text(output_search_string);
    pfree(search_string);
    pfree(output_search_string);
    if (PG_NARGS() > 3 && PG_ARGISNULL(3) == 0) {
        val_type = get_fn_expr_argtype(fcinfo->flinfo, 3);
        if (val_type != BOOLOID) {
            if (val_type == INT4OID) {
                char *input_escape_string = (char *)palloc(8);
                pg_itoa(DatumGetInt32(PG_GETARG_DATUM(3)), input_escape_string);
                search_text = MB_do_like_escape(search_text, cstring_to_text(input_escape_string));
            } else if (val_type == UNKNOWNOID) {
                Datum escape_datum = CStringGetTextDatum(PG_GETARG_POINTER(3));
                text *escape = DatumGetTextP(escape_datum);
                if (text_length(PointerGetDatum(escape)) > 0) {
                    search_text = MB_do_like_escape(search_text, escape);
                }
            } else {
                ereport(ERROR, (errcode(ERRCODE_INVALID_ESCAPE_SEQUENCE), errmsg("invalid escape string"),
                                errhint("Escape string must be empty or one character.")));
            }
        }
    }

    Datum path_cut;
    text *result = NULL;
    char *rpath = (char *)palloc(256);
    rc = memset_s(rpath, 256, 0, 256);
    securec_check(rc, "\0", "\0");
    char *path_string = NULL;
    char *position = (char *)palloc(256);
    char *wildchar = (char *)palloc(256);
    rc = memset_s(wildchar, 256, 0, 256);
    securec_check(rc, "\0", "\0");
    Datum *pathtext = NULL;
    bool *pathnulls = NULL;
    int path_num;
    int i;
    if (PG_NARGS() > 4) {
        ArrayType *path_array = PG_GETARG_ARRAYTYPE_P(4);
        if (array_contains_nulls(path_array)) {
            PG_RETURN_NULL();
        }
        deconstruct_array(path_array, TEXTOID, -1, false, 'i', &pathtext, &pathnulls, &path_num);
        if (path_num <= 0) {
            PG_RETURN_NULL();
        }
    }
    if (strcmp(one_or_all, "one") == 0) {
        if (PG_NARGS() > 4 && (PG_ARGISNULL(4) == 0)) {
            for (i = 0; i < path_num; i++) {
                path_cut = cut_path(pathtext[i], fcinfo);
                path_string = TextDatumGetCString(pathtext[i]);
                /* If the path contains "*", replace it with "%" to use Matchtext(). */
                if (strstr(path_string, "*") != NULL) {
                    rc = strncpy_s(wildchar, 256, "\"", 2);
                    securec_check(rc, "\0", "\0");

                    rc = strcat_s(wildchar, 256, strrpl(path_string, "*", "%"));
                    securec_check(rc, "\0", "\0");
                    rc = strcat_s(wildchar, 256, "%");
                    securec_check(rc, "\0", "\0");
                    path_string[1] = '\0';
                    pathtext[i] = CStringGetTextDatum(path_string);
                    path_cut = cut_path(pathtext[i], fcinfo);
                }
                result = by_path(doc, path_cut, fcinfo);
                if (result != NULL) {
                    rc = strncpy_s(position, 256, "\"", 2);
                    securec_check(rc, "\0", "\0");
                    rc = strcat_s(position, 256, path_string);
                    securec_check(rc, "\0", "\0");
                    if (json_search_unit(result, search_text, true, wildchar, position)) {
                        rc = strncpy_s(rpath, 256, stk->data, 256);
                        securec_check(rc, "\0", "\0");
                        search_CleanStack(stk);
                        pfree(position);
                        pfree(wildchar);
                        PG_RETURN_TEXT_P(cstring_to_text(rpath));
                    }
                }
            }
        }
        if (PG_NARGS() < 5) {
            rc = strncpy_s(position, 256, "\"$", 3);
            securec_check(rc, "\0", "\0");
            if (json_search_unit(doc, search_text, true, wildchar, position)) {
                rc = strncpy_s(rpath, 256, stk->data, 256);
                securec_check(rc, "\0", "\0");
                search_CleanStack(stk);
                pfree(position);
                pfree(wildchar);
                PG_RETURN_TEXT_P(cstring_to_text(rpath));
            }
        }
    } else if (strcmp(one_or_all, "all") == 0) {
        if (PG_NARGS() > 4 && PG_ARGISNULL(4) == 0) {
            bool flag = false;
            for (i = 0; i < path_num; i++) {
                path_cut = cut_path(pathtext[i], fcinfo);
                path_string = TextDatumGetCString(pathtext[i]);
                if (strstr(path_string, "*") != NULL) {
                    rc = strncpy_s(wildchar, 256, "\"", 2);
                    securec_check(rc, "\0", "\0");
                    rc = strcat_s(wildchar, 256, strrpl(path_string, "*", "%"));
                    securec_check(rc, "\0", "\0");
                    rc = strcat_s(wildchar, 256, "%");
                    securec_check(rc, "\0", "\0");
                    path_string[1] = '\0';
                    pathtext[i] = CStringGetTextDatum(path_string);
                    path_cut = cut_path(pathtext[i], fcinfo);
                }
                result = by_path(doc, path_cut, fcinfo);
                if (result != NULL) {
                    rc = strncpy_s(position, 256, "\"", 2);
                    securec_check(rc, "\0", "\0");
                    rc = strcat_s(position, 256, path_string);
                    securec_check(rc, "\0", "\0");
                    if (json_search_unit(result, search_text, false, wildchar, position)) {
                        flag = true;
                        search_PushStack(stk, ",");
                    }
                }
            }
            pfree(position);
            pfree(wildchar);
            if (flag == true) {
                search_PopStack(stk);
                rpath = remove_duplicate_path();
                search_CleanStack(stk);
                PG_RETURN_TEXT_P(cstring_to_text(rpath));
            }
        }
        if (PG_NARGS() < 5) {
            rc = strncpy_s(position, 256, "\"$", 3);
            securec_check(rc, "\0", "\0");
            if (json_search_unit(doc, search_text, false, wildchar, position)) {
                rpath = remove_duplicate_path();
                search_CleanStack(stk);
                pfree(position);
                pfree(wildchar);
                PG_RETURN_TEXT_P(cstring_to_text(rpath));
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

    if (PG_NARGS() == 1) {

        OkeysState *state = NULL;
        int i;
        StringInfo str;
        str = makeStringInfo();
        appendStringInfoChar(str, '[');

        text *json = PG_GETARG_TEXT_PP(0);
        JsonLexContext *lex = makeJsonLexContext(json, true);
        JsonSemAction *sem = NULL;

        state = (OkeysState *)palloc(sizeof(OkeysState));
        sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

        state->lex = lex;
        state->result_size = 256;
        state->result_count = 0;
        state->sent_count = 0;
        state->result = (char **)palloc(256 * sizeof(char *));

        sem->semstate = (void *)state;
        sem->scalar = okeys_scalar;
        sem->object_field_start = okeys_object_field_start;
        /* remainder are all NULL, courtesy of palloc0 above */
        pg_parse_json(lex, sem);
        /* keys are now in state->result */
        pfree(lex->strval->data);
        pfree(lex->strval);
        pfree(lex);
        pfree(sem);

        char *strf = text_to_cstring(json);
        int sea = 0;
        while (strf[sea] == ' ') {
            sea++;
        }
        if (strf[sea] == '[') {
            PG_RETURN_NULL();
        }

        while (state->sent_count < state->result_count) {
            int tg = 0;
            char *nxt = state->result[state->sent_count];
            state->sent_count++;
            for (int cha = 0; cha <= (state->sent_count) - 2; cha++) {
                if (strcmp(nxt, state->result[cha]) == 0) {
                    tg = 1;
                    break;
                }
            }
            if (tg == 0) {
                if (state->sent_count == 1) {
                    appendStringInfoChar(str, '"');
                    appendStringInfoString(str, nxt);
                    appendStringInfoChar(str, '"');

                } else {
                    appendStringInfoChar(str, ',');
                    appendStringInfoChar(str, '"');
                    appendStringInfoString(str, nxt);
                    appendStringInfoChar(str, '"');
                }
            }
        }
        appendStringInfoChar(str, ']');

        /* cleanup to reduce or eliminate memory leaks */
        for (i = 0; i < state->result_count; i++) {
            pfree(state->result[i]);
        }
        pfree(state->result);
        pfree(state);
        PG_RETURN_TEXT_P(cstring_to_text_with_len(str->data, str->len));
    } else {
        OkeysState *state = NULL;
        int i;
        StringInfo str;
        str = makeStringInfo();
        appendStringInfoChar(str, '[');

        Datum path_cut = cut_path(PG_GETARG_DATUM(1), fcinfo);
        text *result = NULL;
        result = by_path(PG_GETARG_TEXT_P(0), path_cut, fcinfo);

        text *json = result;
        if (json == NULL) {
            PG_RETURN_NULL();
        }
        JsonLexContext *lex = makeJsonLexContext(json, true);
        JsonSemAction *sem = NULL;
        state = (OkeysState *)palloc(sizeof(OkeysState));
        sem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));
        state->lex = lex;
        state->result_size = 256;
        state->result_count = 0;
        state->sent_count = 0;
        state->result = (char **)palloc(256 * sizeof(char *));

        sem->semstate = (void *)state;
        sem->scalar = okeys_scalar;
        sem->object_field_start = okeys_object_field_start;
        /* remainder are all NULL, courtesy of palloc0 above */
        pg_parse_json(lex, sem);
        /* keys are now in state->result */
        pfree(lex->strval->data);
        pfree(lex->strval);
        pfree(lex);
        pfree(sem);

        char *strf = text_to_cstring(json);
        int sea = 0;
        while (strf[sea] == ' ') {
            sea++;
        }
        if (strf[sea] == '[') {
            PG_RETURN_NULL();
        }
        while (state->sent_count < state->result_count) {
            int tg = 0;
            char *nxt = state->result[state->sent_count];
            state->sent_count++;
            for (int cha = 0; cha <= (state->sent_count) - 2; cha++) {
                if (strcmp(nxt, state->result[cha]) == 0) {
                    tg = 1;
                    break;
                }
            }
            if (tg == 0) {
                if (state->sent_count == 1) {
                    appendStringInfoChar(str, '"');
                    appendStringInfoString(str, nxt);
                    appendStringInfoChar(str, '"');

                } else {
                    appendStringInfoChar(str, ',');
                    appendStringInfoChar(str, '"');
                    appendStringInfoString(str, nxt);
                    appendStringInfoChar(str, '"');
                }
            }
        }
        appendStringInfoChar(str, ']');

        /* cleanup to reduce or eliminate memory leaks */
        for (i = 0; i < state->result_count; i++) {
            pfree(state->result[i]);
        }
        pfree(state->result);
        pfree(state);

        PG_RETURN_TEXT_P(cstring_to_text_with_len(str->data, str->len));
    }
}

#endif

#ifdef DOLPHIN
extern void add_json_test(Datum val, bool is_null, StringInfo result, Oid val_type, bool key_scalar);

static inline text *get_worker_test(text *json, char *field, int elem_index, char **tpath, int *ipath, int npath,
                                    bool normalize_results, Datum put_in, FunctionCallInfo fcinfo, int time)
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
    Datum in = put_in;
    Oid val_type;
    val_type = get_fn_expr_argtype(fcinfo->flinfo, time * 2);
    in = PG_GETARG_DATUM(time * 2 + 1);
    if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, 2 * time)) {
        val_type = TEXTOID;
        if (PG_ARGISNULL(2 * time)) {
            in = (Datum)0;
        } else {
            in = CStringGetTextDatum(PG_GETARG_POINTER(2 * time));
        }
    } else {
        in = PG_GETARG_DATUM(2 * time);
    }
    if (val_type == InvalidOid || val_type == UNKNOWNOID) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("arg %d: could not determine data type", 2 * time + 1)));
    }

    int ju = 0;
    StringInfo mstr;
    mstr = makeStringInfo();

    if (state->tresult == NULL) {
        return json;
    }

    appendStringInfoString(mstr, text_to_cstring(state->tresult));
    char *stre = text_to_cstring(state->tresult);
    int len2 = mstr->len;
    if (stre[0] == '[') {
        ju = 1;
    }

    StringInfo str1;
    str1 = makeStringInfo();
    char *strcheck = text_to_cstring(json);
    appendStringInfoString(str1, strcheck);
    int len = state->result_start - lex->input;
    text *temp;
    temp = cstring_to_text_with_len(str1->data, len);
    StringInfo str;
    str = makeStringInfo();
    appendStringInfoString(str, text_to_cstring(temp));
    int len1 = str->len;

    if (ju == 0) {

        appendStringInfoChar(str, '[');
        appendStringInfoString(str, text_to_cstring(state->tresult));
        appendStringInfoChar(str, ',');
        appendStringInfoChar(str, ' ');
        add_json_test(in, PG_ARGISNULL(2 * time), str, val_type, false);
        appendStringInfoChar(str, ']');
    } else {

        appendStringInfoString(str, text_to_cstring(state->tresult));
        str->len = str->len - 1;
        appendStringInfoChar(str, ',');
        appendStringInfoChar(str, ' ');
        add_json_test(in, PG_ARGISNULL(2 * time), str, val_type, false);
        appendStringInfoChar(str, ']');
    }

    Datum raw;
    raw = CStringGetDatum(text_to_cstring(json));
    StringInfo strd;
    strd = makeStringInfo();
    appendStringInfoString(strd, text_to_cstring(json));
    int length1 = strd->len - len1;
    text *back = text_substring(raw, len1 + len2, length1, false);
    appendStringInfoString(str, text_to_cstring(back));
    state->tresult = cstring_to_text_with_len(str->data, str->len);
    return state->tresult;
}
text *by_path_test(text *target, Datum path_cut, FunctionCallInfo fcinfo, Datum put_in, int time)
{

    int npath = 0;
    char **tpath = NULL;
    int *ipath = NULL;
    int i;
    long ind;
    char *endptr = NULL;

    ArrayType *all_tokens =
        DatumGetArrayTypeP(DirectFunctionCall2Coll(regexp_split_to_array_no_flags, PG_GET_COLLATION(), path_cut,
                                                   CStringGetTextDatum("(\\.)(?=([^\"]|\"[^\"]*\")*$)")));

    Datum *path_token_text = NULL;
    bool *path_token_nulls = NULL;
    int path_token_num;

    if (array_contains_nulls(all_tokens)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("path must have tokens")));
    }

    deconstruct_array(all_tokens, TEXTOID, -1, false, 'i', &path_token_text, &path_token_nulls, &path_token_num);

    npath = path_token_num - 1;
    char *jupath = PG_GETARG_POINTER(2 * time - 1);

    if (strcmp(jupath, "$") == 0) {
        char *stre = text_to_cstring(target);
        Datum in = put_in;
        Oid val_type;
        val_type = get_fn_expr_argtype(fcinfo->flinfo, time * 2);
        in = PG_GETARG_DATUM(time * 2 + 1);
        if (val_type == UNKNOWNOID && get_fn_expr_arg_stable(fcinfo->flinfo, 2 * time)) {
            val_type = TEXTOID;
            if (PG_ARGISNULL(2 * time)) {
                in = (Datum)0;
            } else {
                in = CStringGetTextDatum(PG_GETARG_POINTER(2 * time));
            }
        } else {
            in = PG_GETARG_DATUM(2 * time);
        }
        if (val_type == InvalidOid || val_type == UNKNOWNOID) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("arg %d: could not determine data type", 2 * time + 1)));
        }
        int ju = 0;
        if (stre[0] == '[') {
            ju = 1;
        }
        StringInfo str;
        str = makeStringInfo();
        if (ju == 0) {
            appendStringInfoChar(str, '[');
            appendStringInfoString(str, text_to_cstring(target));
            appendStringInfoChar(str, ',');
            appendStringInfoChar(str, ' ');
            add_json_test(in, PG_ARGISNULL(2 * time), str, val_type, false);
            appendStringInfoChar(str, ']');
        } else {
            appendStringInfoString(str, text_to_cstring(target));
            str->len = str->len - 1;
            appendStringInfoChar(str, ',');
            appendStringInfoChar(str, ' ');
            add_json_test(in, PG_ARGISNULL(2 * time), str, val_type, false);
            appendStringInfoChar(str, ']');
        }
        target = cstring_to_text_with_len(str->data, str->len);
        return target;
    }

    if (npath <= 0) {
        return target;
    }

    tpath = (char **)palloc(npath * sizeof(char *));
    ipath = (int *)palloc(npath * sizeof(int));

    for (i = 0; i < npath; i++) {

        tpath[i] = text_to_cstring(DatumGetTextP(
            DirectFunctionCall4Coll(textregexreplace, PG_GET_COLLATION(), path_token_text[i + 1],
                                    CStringGetTextDatum("\""), CStringGetTextDatum("\0"), CStringGetTextDatum("g"))));
        if (*tpath[i] == '\0') {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("cannot call function with empty path elements")));
        }

        ind = strtol(tpath[i], &endptr, 10);
        if (*endptr == '\0' && ind <= INT_MAX && ind >= 0) {
            ipath[i] = (int)ind;
        } else {
            ipath[i] = -1;
        }
    }

    return get_worker_test(target, NULL, -1, tpath, ipath, npath, false, put_in, fcinfo, time);
}
Datum json_array_append(PG_FUNCTION_ARGS)
{

    int num = PG_NARGS();
    int time = (num - 1) / 2;
    text *result = NULL;
    Datum path_cut;
    Datum temp;

    fcinfo->fncollation = C_COLLATION_OID;

    for (int i = 1; i <= time; i++) {
        if (i == 1) {
            temp = CStringGetTextDatum(PG_GETARG_POINTER(1));

            path_cut = cut_path(temp, fcinfo);

            result = by_path_test(PG_GETARG_TEXT_P(0), path_cut, fcinfo, PG_GETARG_DATUM(2), i);
        } else {
            temp = CStringGetTextDatum(PG_GETARG_POINTER(2 * i - 1));

            path_cut = cut_path(temp, fcinfo);

            result = by_path_test(result, path_cut, fcinfo, PG_GETARG_DATUM(2 * i), i);
        }
    }
    PG_RETURN_TEXT_P(result);
}
Datum json_append(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(json_array_append(fcinfo));
}
#endif

#ifdef DOLPHIN
static OkeysState *get_object_key_info(text *json)
{
    OkeysState *tState = NULL;

    JsonLexContext *ttLex = makeJsonLexContext(json, true);
    JsonSemAction *tSem = NULL;

    tState = (OkeysState *)palloc(sizeof(OkeysState));
    tSem = (JsonSemAction *)palloc0(sizeof(JsonSemAction));

    tState->lex = ttLex;
    tState->result_size = 256;
    tState->result_count = 0;
    tState->sent_count = 0;
    tState->result = (char **)palloc(256 * sizeof(char *));

    tSem->semstate = (void *)tState;
    tSem->array_start = okeys_array_start;
    tSem->scalar = okeys_scalar;
    tSem->object_field_start = okeys_object_field_start;
    /* remainder are all NULL, courtesy of palloc0 above */
    pg_parse_json(ttLex, tSem);
    /* keys are now in state->result */
    pfree(ttLex->strval->data);
    pfree(ttLex->strval);
    pfree(ttLex);
    pfree(tSem);

    return tState;
}

static void free_object_key_info(OkeysState *state)
{
    /* cleanup to reduce or eliminate memory leaks */
    for (int i = 0; i < state->result_count; i++) {
        pfree(state->result[i]);
    }
    pfree(state->result);
    pfree(state);
}

static int put_object_keys_into_set(char **keys, OkeysState *cState, OkeysState *tState)
{
    int i;
    int cnt = 0;
    for (i = 0; i < cState->result_count; i++) {
        bool flag = true;
        for (int iter = 0; iter < cnt; iter++) {
            if (strcmp(cState->result[i], keys[iter]) == 0)
                flag = false;
        }
        if (flag) {
            keys[cnt] = cState->result[i];
            cnt++;
        }
    }

    for (i = 0; i < tState->result_count; i++) {
        bool flag = true;
        for (int iter = 0; iter < cnt; iter++) {
            if (strcmp(tState->result[i], keys[iter]) == 0)
                flag = false;
        }
        if (flag) {
            keys[cnt] = tState->result[i];
            cnt++;
        }
    }
    return cnt;
}

static int put_object_keys_into_set(char **keys, OkeysState *tState)
{
    int i;
    int cnt = 0;
    for (i = 0; i < tState->result_count; i++) {
        bool flag = true;
        for (int iter = 0; iter < cnt; iter++) {
            if (strcmp(tState->result[i], keys[iter]) == 0)
                flag = false;
        }
        if (flag) {
            keys[cnt] = tState->result[i];
            cnt++;
        }
    }
    return cnt;
}

static void appendObject(StringInfo result, text *json, int cnt, char **keys, int *pos)
{
    appendStringInfoChar(result, '{');
    for (int i = 0; i < cnt; i++) {
        if (i != 0)
            appendStringInfoString(result, ", ");
        appendStringInfoChar(result, '"');
        appendStringInfoString(result, keys[pos[i]]);
        appendStringInfoChar(result, '"');
        appendStringInfoString(result, ": ");
        text *tResult = get_worker(json, keys[pos[i]], -1, NULL, NULL, -1, false);
        appendStringInfoString(result, text_to_cstring(json_regular_format(tResult)));
    }
    appendStringInfoChar(result, '}');
}

static void appendStringInfoObject(StringInfo result, text *json)
{
    OkeysState *tState = NULL;
    tState = get_object_key_info(json);

    if (tState->result_count == 0) {
        free_object_key_info(tState);
        appendStringInfoChar(result, '{');
        appendStringInfoChar(result, '}');
        return;
    }

    char **keys = (char **)palloc(tState->result_count * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, tState);

    int pos[cnt];
    for (int i = 0; i < cnt; i++)
        pos[i] = i;

    get_keys_order(keys, 0, cnt - 1, pos);

    appendObject(result, json, cnt, keys, pos);
    free_object_key_info(tState);
    pfree(keys);
}

static void appendStringInfoArray(StringInfo result, text *json)
{
    int arr_iter = 0;
    appendStringInfoChar(result, '[');
    while (true) {
        text *val = get_worker(json, NULL, arr_iter, NULL, NULL, -1, false);
        if (val == NULL)
            break;
        if (arr_iter != 0)
            appendStringInfoString(result, ", ");
        appendStringInfoString(result, text_to_cstring(json_regular_format(val)));
        arr_iter++;
    }
    appendStringInfoChar(result, ']');
}

static void appendStringInfoArrayEle(StringInfo result, text *json)
{
    int arr_iter = 0;
    while (true) {
        text *val = get_worker(json, NULL, arr_iter, NULL, NULL, -1, false);
        if (val == NULL)
            break;
        if (arr_iter != 0)
            appendStringInfoString(result, ", ");
        appendStringInfoString(result, text_to_cstring(json_regular_format(val)));
        arr_iter++;
    }
}

static void appendStringInfoScalar(StringInfo result, text *json)
{
    appendStringInfoString(result, text_to_cstring(json));
}

static void appendStringInfoObjectsPatch(StringInfo result, text *j1, text *j2)
{
    int i;

    OkeysState *tState = NULL;
    OkeysState *cState = NULL;

    tState = get_object_key_info(j2);
    cState = get_object_key_info(j1);

    int result_count = cState->result_count + tState->result_count;
    if (result_count == 0) {
        free_object_key_info(tState);
        free_object_key_info(cState);
        appendStringInfoChar(result, '{');
        appendStringInfoChar(result, '}');
        return;
    }

    char **keys = (char **)palloc(result_count * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, cState, tState);

    int pos[cnt];
    for (i = 0; i < cnt; i++)
        pos[i] = i;
    bool flag = true;

    get_keys_order(keys, 0, cnt - 1, pos);

    appendStringInfoChar(result, '{');
    for (i = 0; i < cnt; i++) {
        text *res1 = get_worker(j1, keys[pos[i]], -1, NULL, NULL, -1, false);
        text *res2 = get_worker(j2, keys[pos[i]], -1, NULL, NULL, -1, false);
        if (res2 == NULL) {
            if (i != 0 && flag)
                appendStringInfoString(result, ", ");
            appendStringInfoChar(result, '"');
            appendStringInfoString(result, keys[pos[i]]);
            appendStringInfoChar(result, '"');
            appendStringInfoString(result, ": ");
            appendStringInfoString(result, text_to_cstring(json_regular_format(res1)));
        } else {
            if (json_type(res2) == JSON_NULL) {
                flag = false;
                continue;
            }
            if (i != 0 && flag)
                appendStringInfoString(result, ", ");
            appendStringInfoChar(result, '"');
            appendStringInfoString(result, keys[pos[i]]);
            appendStringInfoChar(result, '"');
            appendStringInfoString(result, ": ");
            if (res1 != NULL) {
                appendStringInfoString(result, text_to_cstring(json_merge_patch_unit(res1, res2)));
            } else {
                appendStringInfoString(result, text_to_cstring(json_regular_format(res2)));
            }
        }
        flag = true;
    }
    appendStringInfoChar(result, '}');

    free_object_key_info(cState);
    free_object_key_info(tState);
    pfree(keys);
}

static void appendStringInfoObjectsPreserve(StringInfo result, text *j1, text *j2)
{
    int i;

    OkeysState *tState = NULL;
    OkeysState *cState = NULL;
    tState = get_object_key_info(j2);
    cState = get_object_key_info(j1);

    int result_count = cState->result_count + tState->result_count;
    if (result_count == 0) {
        free_object_key_info(tState);
        free_object_key_info(cState);
        appendStringInfoChar(result, '{');
        appendStringInfoChar(result, '}');
        return;
    }

    char **keys = (char **)palloc(result_count * sizeof(char *));
    int cnt = put_object_keys_into_set(keys, cState, tState);

    int pos[cnt];
    for (i = 0; i < cnt; i++)
        pos[i] = i;

    get_keys_order(keys, 0, cnt - 1, pos);

    appendStringInfoChar(result, '{');
    for (i = 0; i < cnt; i++) {
        if (i != 0)
            appendStringInfoString(result, ", ");
        appendStringInfoChar(result, '"');
        appendStringInfoString(result, keys[pos[i]]);
        appendStringInfoChar(result, '"');
        appendStringInfoString(result, ": ");
        text *res1 = get_worker(j1, keys[pos[i]], -1, NULL, NULL, -1, false);
        text *res2 = get_worker(j2, keys[pos[i]], -1, NULL, NULL, -1, false);

        if (res1 == NULL)
            appendStringInfoString(result, text_to_cstring(json_regular_format(res2)));
        else if (res2 == NULL)
            appendStringInfoString(result, text_to_cstring(json_regular_format(res1)));
        else
            appendStringInfoString(result, text_to_cstring(json_merge_preserve_unit(res1, res2)));
    }
    appendStringInfoChar(result, '}');

    free_object_key_info(cState);
    free_object_key_info(tState);
    pfree(keys);
}

static bool json_array_null(text *json)
{
    text *val = get_worker(json, NULL, 0, NULL, NULL, -1, false);
    if (val == NULL)
        return true;
    return false;
}

static text *json_regular_format(text *json)
{
    JsonType tok = json_type(json);
    StringInfo result = makeStringInfo();
    switch (tok) {
        case JSON_ARRAY: {
            appendStringInfoArray(result, json);
            break;
        }
        case JSON_OBJECT: {
            appendStringInfoObject(result, json);
            break;
        }
        case JSON_BOOL:
        case JSON_NULL:
        case JSON_NUMBER:
        case JSON_STRING: {
            appendStringInfoScalar(result, json);
            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", tok);
    }
    text *res = cstring_to_text_with_len(result->data, result->len);
    pfree(result->data);
    pfree(result);
    return res;
}

static text *json_merge_patch_unit(text *j1, text *j2)
{
    JsonType t1 = json_type(j1);
    JsonType t2 = json_type(j2);
    StringInfo result = makeStringInfo();

    switch (t1) {
        case JSON_OBJECT: {
            switch (t2) {
                case JSON_OBJECT: {
                    appendStringInfoObjectsPatch(result, j1, j2);
                    break;
                }
                case JSON_ARRAY: {
                    appendStringInfoArray(result, j2);
                    break;
                }
                case JSON_BOOL:
                case JSON_NULL:
                case JSON_NUMBER:
                case JSON_STRING: {
                    appendStringInfoScalar(result, j2);
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", t2);
            }
            break;
        }
        case JSON_ARRAY:
        case JSON_BOOL:
        case JSON_NULL:
        case JSON_NUMBER:
        case JSON_STRING: {
            switch (t2) {
                case JSON_ARRAY: {
                    appendStringInfoArray(result, j2);
                    break;
                }
                case JSON_OBJECT: {
                    appendStringInfoObject(result, j2);
                    break;
                }
                case JSON_BOOL:
                case JSON_NULL:
                case JSON_NUMBER:
                case JSON_STRING: {
                    appendStringInfoScalar(result, j2);
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", t2);
            }
            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", t1);
    }
    text *res = cstring_to_text_with_len(result->data, result->len);
    pfree(result->data);
    pfree(result);
    return res;
}

static text *json_merge_preserve_unit(text *j1, text *j2)
{
    JsonType t1 = json_type(j1);
    JsonType t2 = json_type(j2);
    StringInfo result = makeStringInfo();

    switch (t1) {
        case JSON_OBJECT: {
            switch (t2) {
                case JSON_OBJECT: {
                    appendStringInfoObjectsPreserve(result, j1, j2);
                    break;
                }
                case JSON_ARRAY: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoObject(result, j1);
                    if (!json_array_null(j2))
                        appendStringInfoString(result, ", ");
                    appendStringInfoArrayEle(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                case JSON_BOOL:
                case JSON_NULL:
                case JSON_NUMBER:
                case JSON_STRING: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoObject(result, j1);
                    appendStringInfoString(result, ", ");
                    appendStringInfoScalar(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", t2);
            }
            break;
        }
        case JSON_ARRAY: {
            switch (t2) {
                case JSON_ARRAY: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoArrayEle(result, j1);
                    if (!json_array_null(j1) && !json_array_null(j2))
                        appendStringInfoString(result, ", ");
                    appendStringInfoArrayEle(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                case JSON_OBJECT: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoArrayEle(result, j1);
                    if (!json_array_null(j1))
                        appendStringInfoString(result, ", ");
                    appendStringInfoObject(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                case JSON_BOOL:
                case JSON_NULL:
                case JSON_NUMBER:
                case JSON_STRING: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoArrayEle(result, j1);
                    if (!json_array_null(j1))
                        appendStringInfoString(result, ", ");
                    appendStringInfoScalar(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", t2);
            }
            break;
        }
        case JSON_BOOL:
        case JSON_NULL:
        case JSON_NUMBER:
        case JSON_STRING: {
            switch (t2) {
                case JSON_ARRAY: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoScalar(result, j1);
                    if (!json_array_null(j2))
                        appendStringInfoString(result, ", ");
                    appendStringInfoArrayEle(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                case JSON_OBJECT: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoScalar(result, j1);
                    appendStringInfoString(result, ", ");
                    appendStringInfoObject(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                case JSON_BOOL:
                case JSON_NULL:
                case JSON_NUMBER:
                case JSON_STRING: {
                    appendStringInfoChar(result, '[');
                    appendStringInfoScalar(result, j1);
                    appendStringInfoString(result, ", ");
                    appendStringInfoScalar(result, j2);
                    appendStringInfoChar(result, ']');
                    break;
                }
                default:
                    elog(ERROR, "unexpected json type: %d", t2);
            }
            break;
        }
        default:
            elog(ERROR, "unexpected json type: %d", t1);
    }
    text *res = cstring_to_text_with_len(result->data, result->len);
    pfree(result->data);
    pfree(result);
    return res;
}

Datum json_merge_patch(PG_FUNCTION_ARGS)
{
    ArrayType *json_array = PG_GETARG_ARRAYTYPE_P(0);

    Datum *jsontext = NULL;
    bool *jsonnulls = NULL;
    int json_num;

    deconstruct_array(json_array, JSONOID, -1, false, 'i', &jsontext, &jsonnulls, &json_num);

    if (json_num <= 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Incorrect parameter count")));

    text *result = cstring_to_text("null");
    bool return_null = false;

    for (int i = json_num - 1; i >= 0; i--) {
        if (jsonnulls[i]) {
            if (i == json_num - 1)
                PG_RETURN_NULL();
            else {
                for (int j = i + 1; j < json_num; j++) {
                    if (json_type(DatumGetTextP(jsontext[j])) == JSON_OBJECT && j == i + 1) {
                        return_null = true;
                    } else if (json_type(DatumGetTextP(jsontext[j])) == JSON_OBJECT && return_null)
                        ;
                    else {
                        result = json_merge_patch_unit(result, DatumGetTextP(jsontext[j]));
                        return_null = false;
                    }
                }
            }
            if (return_null)
                PG_RETURN_NULL();
            else
                PG_RETURN_TEXT_P(result);
        }
    }

    for (int i = 1; i < json_num; i++) {
        if (i == 1)
            result = json_merge_patch_unit(DatumGetTextP(jsontext[0]), DatumGetTextP(jsontext[1]));
        else
            result = json_merge_patch_unit(result, DatumGetTextP(jsontext[i]));
    }

    PG_RETURN_TEXT_P(result);
}

Datum json_merge_preserve(PG_FUNCTION_ARGS)
{
    ArrayType *json_array = PG_GETARG_ARRAYTYPE_P(0);

    Datum *jsontext = NULL;
    bool *jsonnulls = NULL;
    int json_num;

    deconstruct_array(json_array, JSONOID, -1, false, 'i', &jsontext, &jsonnulls, &json_num);

    if (json_num <= 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Incorrect parameter count")));
    /*
     * If the array is empty, return NULL; this is dubious but it's what 9.3
     * did.
     */
    if (array_contains_nulls(json_array)) {
        PG_RETURN_NULL();
    }

    text *result = NULL;
    for (int i = 1; i < json_num; i++) {
        if (i == 1)
            result = json_merge_preserve_unit(DatumGetTextP(jsontext[0]), DatumGetTextP(jsontext[1]));
        else
            result = json_merge_preserve_unit(result, DatumGetTextP(jsontext[i]));
    }

    PG_RETURN_TEXT_P(result);
}

Datum json_merge(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(json_merge_preserve(fcinfo));
}
#endif

#ifdef DOLPHIN
cJSON_bool cJSON_JsonInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value)
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

text *formatJsondoc(char *str)
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

Datum json_insert(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    Datum arg = 0;
    Oid valtype;
    char *data = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid typOutput;
    bool typIsVarlena = false;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'JSON_INSERT'")));
    }
    if (PG_ARGISNULL(0)) {
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function json_insert")));
    }
    root = cJSON_Parse(data);
    pfree(data);
    if (!root) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid JSON text in function json_insert.")));
    }
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
        jp = cJSON_JsonPathParse(data);
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
            switch (valtype) {
                case UNKNOWNOID:
                case TEXTOID:
                case CSTRINGOID:
                    value = cJSON_CreateString(data);
                    break;
                case BOOLOID:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_Parse(data);
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
    cJSON_free(s);
    PG_RETURN_TEXT_P(res);
}
#endif

#ifdef DOLPHIN
bool cJSON_JsonReplace(cJSON *root, cJSON_JsonPath *jp, cJSON *value, bool &invalidPath)
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

bool cJSON_JsonRemove(cJSON *root, cJSON_JsonPath *jp, bool *invalidPath)
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
bool cJSON_JsonArrayInsert(cJSON *root, cJSON_JsonPath *jp, cJSON *value, bool *invlidPath, bool *isArray)
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

Datum json_replace(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    Datum arg = 0;
    Oid valtype;
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid typOutput;
    bool typIsVarlena = false;
    char *data = NULL;
    bool invalidPath = false;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'JSON_REPLACE'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function function json_replace")));
    }
    root = cJSON_Parse(data);
    pfree(data);
    if (!root) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON text in function json_replace: \"Invalid value.\" at position 0.")));
    }
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
        jp = cJSON_JsonPathParse(pathString);
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
            switch (valtype) {
                case UNKNOWNOID:
                case TEXTOID:
                case CSTRINGOID:
                    value = cJSON_CreateString(valueString);
                    break;
                case BOOLOID:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_Parse(valueString);
                    break;
            }
            pfree(valueString);
        }
        if (jp->next == NULL && jp->type == cJSON_JsonPath_Start) {
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
    cJSON_SortObject(root);
    s = cJSON_PrintUnformatted(root);
    res = formatJsondoc(s);
    cJSON_Delete(root);
    cJSON_free(s);
    PG_RETURN_TEXT_P(res);
}

Datum json_remove(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    Datum arg = 0;
    Oid valtype;
    char *pathString = NULL;
    cJSON *root = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid typOutput;
    bool typIsVarlena = false;
    bool invalidPath = false;
    char *data = NULL;
    if (nargs < 2) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'json_remove'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function function json_remove")));
    }
    root = cJSON_Parse(data);
    pfree(data);
    if (!root) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON text in function json_replace: \"Invalid value.\" at position 0.")));
    }
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
        jp = cJSON_JsonPathParse(pathString);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around character position %d.", i)));
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
                            errmsg("Invalid JSON path expression. The error is around character position %d.", i)));
        }
        cJSON_DeleteJsonPath(jp);
    }
    cJSON_SortObject(root);
    s = cJSON_PrintUnformatted(root);
    res = formatJsondoc(s);
    cJSON_Delete(root);
    cJSON_free(s);
    PG_RETURN_TEXT_P(res);
}

Datum json_array_insert(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    Datum arg = 0;
    Oid valtype;
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid typOutput;
    bool typIsVarlena = false;
    bool invalidPath = false;
    bool isArray = false;
    char *data = NULL;
    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid arg number")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
    } else {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid data type for JSON data in argument 1 to function function json_array_insert")));
    }
    root = cJSON_Parse(data);
    pfree(data);
    if (!root) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON text in function json_replace: \"Invalid value.\" at position 0.")));
    }
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
        jp = cJSON_JsonPathParse(pathString);
        pfree(pathString);
        if (!jp) {
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid JSON path expression. The error is around character position %d.", i)));
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
            switch (valtype) {
                case UNKNOWNOID:
                case TEXTOID:
                case CSTRINGOID:
                    value = cJSON_CreateString(valueString);
                    break;
                case BOOLOID:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_Parse(valueString);
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
                            errmsg("Invalid JSON path expression. The error is around character position %d.", i)));
        }
        if (isArray) {
            cJSON_DeleteJsonPath(jp);
            cJSON_Delete(root);
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("A path expression is not a path to a cell in an array.")));
        }
        cJSON_DeleteJsonPath(jp);
    }
    cJSON_SortObject(root);
    s = cJSON_PrintUnformatted(root);
    res = formatJsondoc(s);
    cJSON_Delete(root);
    cJSON_free(s);
    PG_RETURN_TEXT_P(res);
}

Datum json_set(PG_FUNCTION_ARGS)
{
    int nargs = PG_NARGS();
    Datum arg = 0;
    Oid valtype;
    char *pathString = NULL;
    char *valueString = NULL;
    cJSON *root = NULL;
    cJSON *value = NULL;
    cJSON_JsonPath *jp = NULL;
    text *res = NULL;
    char *s = NULL;
    Oid typOutput;
    bool typIsVarlena = false;
    char *data = NULL;
    bool invalidPath = false;

    if (nargs < 3 || nargs % 2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Incorrect parameter count in the call to native function 'JSON_SET'")));
    }
    if (PG_ARGISNULL(0)) {
        cJSON_Delete(root);
        PG_RETURN_NULL();
    }
    valtype = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (VALTYPE_IS_JSON(valtype)) {
        arg = PG_GETARG_DATUM(0);
        getTypeOutputInfo(valtype, &typOutput, &typIsVarlena);
        data = OidOutputFunctionCall(typOutput, arg);
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid data type for JSON data in argument 1 to function function json_set")));
    }
    root = cJSON_Parse(data);
    pfree(data);
    if (!root) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid JSON text in function json_set: \"Invalid value.\" at position 0.")));
    }
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
        jp = cJSON_JsonPathParse(pathString);
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
            switch (valtype) {
                case UNKNOWNOID:
                case TEXTOID:
                case CSTRINGOID:
                    value = cJSON_CreateString(valueString);
                    break;
                case BOOLOID:
                    value = cJSON_CreateBool(DatumGetBool(arg));
                    break;
                default:
                    value = cJSON_Parse(valueString);
                    break;
            }
            pfree(valueString);
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
    cJSON_free(s);
    PG_RETURN_TEXT_P(res);
}
#endif
