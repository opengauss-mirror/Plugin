#include "postgres.h"

#include "fmgr.h"

#include "catalog/namespace.h"
#include "catalog/pg_class.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_type.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/json.h"
#include "utils/lsyscache.h"

#include "pg_version_compat.h"

#include "distributed/jsonbutils.h"
#include "distributed/metadata_cache.h"
#include "utils/jsonb.h"

/*
 * Extract scalar value from raw-scalar pseudo-array jsonb.
 */
bool JsonbExtractScalar(Jsonb* in, JsonbValue* res)
{
    JsonbIterator* it;
    JsonbValue tmp;

    if (!JB_ROOT_IS_ARRAY(in) || !JB_ROOT_IS_SCALAR(in)) {
        /* inform caller about actual type of container */
        res->type = (JB_ROOT_IS_ARRAY(in)) ? jbvArray : jbvObject;
        return false;
    }

    /*
     * A root scalar is stored as an array of one element, so we get the array
     * and then its first (and only) member.
     */
    it = JsonbIteratorInit(VARDATA_ANY(in));

    auto tok = JsonbIteratorNext(&it, &tmp, true);
    Assert(tok == WJB_BEGIN_ARRAY);

    JsonbIteratorNext(&it, res, true);
    Assert(tok == WJB_ELEM);
    Assert(IsAJsonbScalar(res));

    JsonbIteratorNext(&it, &tmp, true);
    Assert(tok == WJB_END_ARRAY);

    tok = JsonbIteratorNext(&it, &tmp, true);
    Assert(tok == WJB_DONE);

    return true;
}

/*
 * Emit correct, translatable cast error message
 */
static void cannotCastJsonbValue(enum jbvType type, const char* sqltype)
{
    static const struct {
        enum jbvType type;
        const char* msg;
    } messages[] = {
        {jbvNull, gettext_noop("cannot cast jsonb null to type %s")},
        {jbvString, gettext_noop("cannot cast jsonb string to type %s")},
        {jbvNumeric, gettext_noop("cannot cast jsonb numeric to type %s")},
        {jbvBool, gettext_noop("cannot cast jsonb boolean to type %s")},
        {jbvArray, gettext_noop("cannot cast jsonb array to type %s")},
        {jbvObject, gettext_noop("cannot cast jsonb object to type %s")},
        {jbvBinary, gettext_noop("cannot cast jsonb array or object to type %s")}};
    int i;

    for (i = 0; i < lengthof(messages); i++)
        if (messages[i].type == type)
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg(messages[i].msg, sqltype)));

    /* should be unreachable */
    elog(ERROR, "unknown jsonb type: %d", (int)type);
}

Datum jsonb_bool(PG_FUNCTION_ARGS)
{
    Jsonb* in = PG_GETARG_JSONB(0);
    JsonbValue v;

    if (!JsonbExtractScalar(in, &v) || v.type != jbvBool)
        cannotCastJsonbValue(v.type, "boolean");

    PG_FREE_IF_COPY(in, 0);

    PG_RETURN_BOOL(v.boolean);
}

Datum jsonb_int4(PG_FUNCTION_ARGS)
{
    Jsonb* in = PG_GETARG_JSONB(0);
    JsonbValue v;
    Datum retValue;

    if (!JsonbExtractScalar(in, &v) || v.type != jbvNumeric)
        cannotCastJsonbValue(v.type, "integer");

    retValue = DirectFunctionCall1(numeric_int4, NumericGetDatum(v.numeric));

    PG_FREE_IF_COPY(in, 0);

    PG_RETURN_DATUM(retValue);
}

#define LOCAL_FCINFO(name, nargs)                                        \
    /* use union with FunctionCallInfoBaseData to guarantee alignment */ \
    FunctionCallInfoData name##fcinfo;                                   \
    FunctionCallInfo name = &name##fcinfo;
/*
 * ExtractFieldJsonb gets value of fieldName from jsonbDoc and puts it
 * into result. If not found, returns false. Otherwise, returns true.
 * The field is returned as a Text* Datum if as_text is true, or a Jsonb*
 * Datum if as_text is false.
 */
static bool ExtractFieldJsonb(Datum jsonbDoc, const char* fieldName, Datum* result,
                              bool as_text)
{
    Datum pathArray[1] = {CStringGetTextDatum(fieldName)};
    bool pathNulls[1] = {false};
    bool typeByValue = false;
    char typeAlignment = 0;
    int16 typeLength = 0;
    int dimensions[1] = {1};
    int lowerbounds[1] = {1};

    get_typlenbyvalalign(TEXTOID, &typeLength, &typeByValue, &typeAlignment);

    ArrayType* pathArrayObject =
        construct_md_array(pathArray, pathNulls, 1, dimensions, lowerbounds, TEXTOID,
                           typeLength, typeByValue, typeAlignment);
    Datum pathDatum = PointerGetDatum(pathArrayObject);

    FmgrInfo fmgrInfo;

    if (as_text) {
        fmgr_info(JsonbExtractPathTextFuncId(), &fmgrInfo);
    } else {
        fmgr_info(JsonbExtractPathFuncId(), &fmgrInfo);
    }

    LOCAL_FCINFO(functionCallInfo, 2);
    InitFunctionCallInfoData(*functionCallInfo, &fmgrInfo, 2, DEFAULT_COLLATION_OID, NULL,
                             NULL);

    fcSetArg(functionCallInfo, 0, jsonbDoc);
    fcSetArg(functionCallInfo, 1, pathDatum);

    *result = FunctionCallInvoke(functionCallInfo);
    return !functionCallInfo->isnull;
}

/*
 * ExtractFieldBoolean gets value of fieldName from jsonbDoc, or returns
 * defaultValue if it doesn't exist.
 */
bool ExtractFieldBoolean(Datum jsonbDoc, const char* fieldName, bool defaultValue)
{
    Datum jsonbDatum = 0;
    bool found = ExtractFieldJsonb(jsonbDoc, fieldName, &jsonbDatum, false);
    if (!found) {
        return defaultValue;
    }

    Datum boolDatum = DirectFunctionCall1(jsonb_bool, jsonbDatum);
    return DatumGetBool(boolDatum);
}

/*
 * ExtractFieldInt32 gets value of fieldName from jsonbDoc, or returns
 * defaultValue if it doesn't exist.
 */
int32 ExtractFieldInt32(Datum jsonbDoc, const char* fieldName, int32 defaultValue)
{
    Datum jsonbDatum = 0;
    bool found = ExtractFieldJsonb(jsonbDoc, fieldName, &jsonbDatum, false);
    if (!found) {
        return defaultValue;
    }

    Datum int32Datum = DirectFunctionCall1(jsonb_int4, jsonbDatum);
    return DatumGetInt32(int32Datum);
}

/*
 * ExtractFieldTextP gets value of fieldName as text* from jsonbDoc, or
 * returns NULL if it doesn't exist.
 */
text* ExtractFieldTextP(Datum jsonbDoc, const char* fieldName)
{
    Datum jsonbDatum = 0;

    bool found = ExtractFieldJsonb(jsonbDoc, fieldName, &jsonbDatum, true);
    if (!found) {
        return NULL;
    }

    return DatumGetTextP(jsonbDatum);
}

/*
 * ExtractFieldJsonbDatum gets value of fieldName from jsonbDoc and puts it
 * into result. If not found, returns false. Otherwise, returns true.
 */
bool ExtractFieldJsonbDatum(Datum jsonbDoc, const char* fieldName, Datum* result)
{
    return ExtractFieldJsonb(jsonbDoc, fieldName, result, false);
}
