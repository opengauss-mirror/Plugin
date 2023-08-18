#include <cstring>
#include "postgres.h"
#include "plugin_postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "utils/numeric.h"
#include "utils/builtins.h"
#include "utils/typcache.h"
#include "utils/lsyscache.h"
#include "utils/date.h"
#include "utils/varbit.h"
#include "plugin_commands/mysqlmode.h"

#ifdef DOLPHIN

#define WEIGHTSTRING_DESC_LEVEL1 0x00000100    /* if desc order for level1 */
#define WEIGHTSTRING_REVERSE_LEVEL1 0x00010000 /* if reverse order for level1 */

#define SINGLE_BYTE_LENGTH 1
#define DOUBLE_BYTE_LENGTH 2
#define TRIPLE_BYTE_LENGTH 3
#define QURADRUPLE_BYTE_LENGTH 4
#define BUFFER_TO_ULONG_BYTE1_SHIFT 8
#define INVALID_LEN -1

#define TWO_NUM_ARGS 2
#define THREE_NUM_ARGS 3

#define MIN_HEX_LOOP 0
#define MAX_HEX_LOOP 127

PG_FUNCTION_INFO_V1_PUBLIC(weight_string);
extern "C" DLL_PUBLIC Datum weight_string(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_single);
extern "C" DLL_PUBLIC Datum weight_string_single(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_bytea);
extern "C" DLL_PUBLIC Datum weight_string_bytea(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_boolean);
extern "C" DLL_PUBLIC Datum weight_string_boolean(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_date);
extern "C" DLL_PUBLIC Datum weight_string_date(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_time);
extern "C" DLL_PUBLIC Datum weight_string_time(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_timestamp);
extern "C" DLL_PUBLIC Datum weight_string_timestamp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_timestamptz);
extern "C" DLL_PUBLIC Datum weight_string_timestamptz(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_interval);
extern "C" DLL_PUBLIC Datum weight_string_interval(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(weight_string_bit);
extern "C" DLL_PUBLIC Datum weight_string_bit(PG_FUNCTION_ARGS);

static void store16be(pg_wchar u, char *res)
{
    /* Convert lowercase letters to uppercase */
    if (u >= 'a' && u <= 'y') {
        u &= ~0x20;
    }
    *res = (char)((u >> BUFFER_TO_ULONG_BYTE1_SHIFT) & 0xff);
    *(res + 1) = (char)(u & 0xff);
}

static size_t char_to_unicode(char *src, size_t src_len, char *output, int output_len, int encoding)
{
    char *utf8_string = (char *)pg_do_encoding_conversion((unsigned char *)src, (int)src_len, encoding, PG_UTF8);

    pg_wchar *wchar = (pg_wchar *)palloc0((src_len + SINGLE_BYTE_LENGTH) * sizeof(pg_wchar));
    int wchar_size = pg_encoding_mb2wchar_with_len(PG_UTF8, utf8_string, wchar, (int)src_len);
    int complete_len = 0;

    if (output_len == INVALID_LEN) {
        output_len = wchar_size * DOUBLE_BYTE_LENGTH;
    }
    for (int i = 0; complete_len < output_len; i++) {
        if (i < wchar_size) {
            store16be(*(wchar + i), output);
        } else {
            store16be(0x20, output);
        }
        output += DOUBLE_BYTE_LENGTH;
        complete_len += DOUBLE_BYTE_LENGTH;
    }

    if (utf8_string != src)
        pfree_ext(utf8_string);

    pfree_ext(wchar);

    return complete_len;
}

static void reverse_and_transform(char *start, char *end, bool flag_dsc, bool flag_rev)
{
    if (flag_dsc) {
        if (flag_rev) {
            for (end--; start <= end;) {
                u_char tmp = *start;
                *start++ = ~*end;
                *end-- = ~tmp;
            }
        } else {
            for (; start < end; start++)
                *start = ~*start;
        }
    } else if (flag_rev) {
        for (end--; start < end;) {
            u_char tmp = *start;
            *start++ = *end;
            *end-- = tmp;
        }
    }
}

static const char hextbl[] = "0123456789abcdef";

static const int8 hexlookup[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,
    4,  5,  6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

static inline char get_hex(char c, bool flag, bool &isValid)
{
    int res = -1;
    isValid = true;
    if (c > MIN_HEX_LOOP && c < MAX_HEX_LOOP)
        res = hexlookup[(unsigned char)c];

    if (res < 0) {
        if (flag)
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid hexadecimal digit: \"%c\"", c)));
        else {
            isValid = false;
            return 0;
        }
    }

    return (char)res;
}

static inline unsigned hex_decode(const char *src, unsigned len, char *dst, bool flag)
{
    const char *s = NULL;
    const char *srcend;
    char v1;
    char v2;
    char *p;

    srcend = src + len;
    s = src;
    p = dst;
    while (s < srcend) {
        if (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r') {
            s++;
            continue;
        }
        bool isValid;
        const int shiftLeft = 4;
        v1 = get_hex(*s++, flag, isValid) << shiftLeft;
        if (!isValid) {
            *dst = 0;
            return 0;
        }
        if (s >= srcend) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("invalid hexadecimal data: odd number of digits")));
        }

        v2 = get_hex(*s++, flag, isValid);
        if (!isValid) {
            *dst = 0;
            return 0;
        }
        *p++ = v1 | v2;
    }

    return p - dst;
}

static bytea* hex_decode_internal(char* data, int dataLength)
{
    int resultLength;
    int res;
    resultLength = dataLength >> SINGLE_BYTE_LENGTH;
    errno_t rc = EOK;

    char* originData = (char*)palloc(dataLength + 1);
    rc = memcpy_s(originData, dataLength, data, dataLength);
    securec_check(rc, "\0", "\0");
    originData[dataLength] = '\0';

    const int evenLength = 2;
    char* newData;
    if (dataLength % evenLength != 0) {
        resultLength += 1;
        dataLength += 1;
        newData = (char*)palloc(dataLength + 1);
        *newData = '0';
        rc = memcpy_s(newData + 1, dataLength - 1, data, dataLength - 1);
        securec_check(rc, "\0", "\0");
    } else {
        newData = (char*)palloc(dataLength + 1);
        rc = memcpy_s(newData, dataLength, data, dataLength);
        securec_check(rc, "\0", "\0");
    }
    newData[dataLength] = '\0';
    bytea* result = (bytea*)palloc(VARHDRSZ + resultLength);
    res = hex_decode(newData, dataLength, VARDATA(result), false);
    if (dataLength != 0 && res == 0) {
        int errlevel = SQL_MODE_STRICT() ? ERROR : WARNING;
        ereport(errlevel,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Incorrect string value: \"%s\" for function unhex ", originData)));
    }
    pfree(newData);
    pfree(originData);
    SET_VARSIZE(result, VARHDRSZ + res);
    return result;
}

static bytea *bit_to_bytea(int64 arg_int, int len)
{
    char *hexResult = (char *)palloc(len + 1);
    hexResult[len] = '\0';
    const int andBits = 15;
    const int rightBits = 4;
    do {
        hexResult[--len] = hextbl[arg_int & andBits];
        arg_int >>= rightBits;
    } while (len > 0);
    bytea *decodeResult = hex_decode_internal(hexResult, strlen(hexResult));
    pfree(hexResult);
    return decodeResult;
}

Datum weight_string(PG_FUNCTION_ARGS)
{
    int encoding;
    char *value = (char *)VARDATA(PG_GETARG_TEXT_P(0));
    size_t value_len = VARSIZE(PG_GETARG_TEXT_P(0)) - VARHDRSZ;
    char *type = (char *)VARDATA(PG_GETARG_TEXT_P(1));
    size_t type_len = VARSIZE(PG_GETARG_TEXT_P(1)) - VARHDRSZ;
    size_t data_len = PG_GETARG_UINT32(2);

    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    if (VARATT_IS_HUGE_TOAST_POINTER(value)) {
        ereport(err_level,
                (errcode(ERRCODE_DATA_EXCEPTION), errmsg("weight_string data size too large; the maximum size is 1GB "
                                                         "(probably, length of weight_string data was corrupted)")));
        PG_RETURN_NULL();
    }

    if (!strncmp(type, "BINARY", type_len)) {
        if (VARATT_IS_HUGE_TOAST_POINTER(value) || !AllocSizeIsValid(data_len)) {
            ereport(err_level, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("weight_string could not support more than 1GB data")));
            PG_RETURN_NULL();
        }

        if (value_len > data_len)
            value_len = data_len;

        text *result = (text *)palloc0(VARHDRSZ + data_len + SINGLE_BYTE_LENGTH);
        errno_t rc = strncpy_s((char *)VARDATA(result), data_len + SINGLE_BYTE_LENGTH, value, value_len);
        securec_check(rc, "\0", "\0");
        SET_VARSIZE(result, VARHDRSZ + data_len);
        PG_RETURN_TEXT_P(result);
    } else if (!strncmp(type, "CHAR", type_len)) {
        size_t retlen = data_len * 2;
        if (VARATT_IS_HUGE_TOAST_POINTER(value) || !AllocSizeIsValid(retlen)) {
            ereport(err_level, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("weight_string could not support more than 1GB data")));
            PG_RETURN_NULL();
        }
        text *result = (text *)palloc0(VARHDRSZ + retlen + 1);
        char *result_string = (char *)VARDATA(result);
        char *start = result_string;
        char *end = result_string + retlen;

        size_t flags = PG_GETARG_UINT32(3);
        uint flag_dsc = flags & WEIGHTSTRING_DESC_LEVEL1;
        uint flag_rev = flags & WEIGHTSTRING_REVERSE_LEVEL1;

        // get encoding
        encoding = pg_get_client_encoding();
        if (encoding < 0) {
            encoding = PG_SQL_ASCII;
        }
        char_to_unicode(value, value_len, result_string, retlen, encoding);

        if (flag_dsc || flag_rev) {
            reverse_and_transform(start, end, flag_dsc, flag_rev);
        }

        SET_VARSIZE(result, VARHDRSZ + retlen);
        PG_RETURN_TEXT_P(result);
    } else {
        ereport(err_level,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("weight_string function does not support syntax other than 'AS CHAR' and 'AS BINARY'.")));
    }

    PG_RETURN_NULL();
}
Datum weight_string_single(PG_FUNCTION_ARGS)
{
    int encoding;
    char *value = (char *)VARDATA(PG_GETARG_TEXT_P(0));
    size_t data_len = VARSIZE(PG_GETARG_TEXT_P(0)) - VARHDRSZ;
    size_t retlen = data_len * 2;
    text *result = (text *)palloc(VARHDRSZ + retlen + 1);
    char *result_string = (char *)VARDATA(result);
    int rc = memset_s(result_string, retlen + 1, ' ', retlen + 1);
    securec_check(rc, "\0", "\0");
    char *start = result_string;
    char *end = NULL;

    size_t flags = PG_GETARG_UINT32(1);
    uint flag_dsc = flags & WEIGHTSTRING_DESC_LEVEL1;
    uint flag_rev = flags & WEIGHTSTRING_REVERSE_LEVEL1;

    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    if (VARATT_IS_HUGE_TOAST_POINTER(value)) {
        ereport(err_level,
                (errcode(ERRCODE_DATA_EXCEPTION), errmsg("weight_string data size too large; the maximum size is 1GB "
                                                         "(probably, length of weight_string data was corrupted)")));
        PG_RETURN_NULL();
    }
    // get encoding
    encoding = pg_get_client_encoding();
    if (encoding < 0) {
        encoding = PG_SQL_ASCII;
    }
    end = start + retlen;
    retlen = char_to_unicode(value, data_len, result_string, INVALID_LEN, encoding);

    if (flag_dsc || flag_rev) {
        reverse_and_transform(start, end, flag_dsc, flag_rev);
    }

    SET_VARSIZE(result, VARHDRSZ + retlen);
    PG_RETURN_TEXT_P(result);
}

static text* weight_string_bit_bytea_common(char* value, FunctionCallInfoData *fcinfo, size_t value_len, bool toUnicode)
{
    if (fcinfo->nargs >= THREE_NUM_ARGS) {
        char *type = (char *)VARDATA(PG_GETARG_TEXT_P(1));
        size_t type_len = VARSIZE(PG_GETARG_TEXT_P(1)) - VARHDRSZ;
        size_t data_len = PG_GETARG_UINT32(2);
        int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
        if (!strncmp(type, "BINARY", type_len)) {
            if (VARATT_IS_HUGE_TOAST_POINTER(value) || !AllocSizeIsValid(data_len)) {
                ereport(err_level, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("weight_string could not support more than 1GB data")));
                return cstring_to_text("");
            }

            if (value_len > data_len)
                value_len = data_len;

            text *result = (text *)palloc0(VARHDRSZ + data_len + SINGLE_BYTE_LENGTH);
            errno_t rc = strncpy_s((char *)VARDATA(result), data_len + SINGLE_BYTE_LENGTH, value, value_len);
            securec_check(rc, "\0", "\0");
            SET_VARSIZE(result, VARHDRSZ + data_len);
            return result;
        } else if (!strncmp(type, "CHAR", type_len)) {
            size_t retlen = data_len * 2;
            if (VARATT_IS_HUGE_TOAST_POINTER(value) || !AllocSizeIsValid(retlen)) {
                ereport(err_level, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("weight_string could not support more than 1GB data")));
                return cstring_to_text("");
            }
            text *result = (text *)palloc0(VARHDRSZ + retlen + 1);
            char *result_string = (char *)VARDATA(result);
            char *start = result_string;

            size_t flags = PG_GETARG_UINT32(3);
            uint flag_dsc = flags & WEIGHTSTRING_DESC_LEVEL1;
            uint flag_rev = flags & WEIGHTSTRING_REVERSE_LEVEL1;
            // get encoding
            int encoding = pg_get_client_encoding();
            if (encoding < 0) {
                encoding = PG_SQL_ASCII;
            }
            if (toUnicode) {
                char_to_unicode(value, value_len, result_string, retlen, encoding);
            } else {
                errno_t rc = strncpy_s((char *)VARDATA(result), data_len + SINGLE_BYTE_LENGTH, value, value_len);
                securec_check(rc, "\0", "\0");
                retlen = data_len;
            }
            char *end = result_string + retlen;
            if (flag_dsc || flag_rev) {
                reverse_and_transform(start, end, flag_dsc, flag_rev);
            }

            SET_VARSIZE(result, VARHDRSZ + retlen);
            return result;
        } else {
            ereport(err_level,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("weight_string function does not support syntax other than 'AS CHAR' and 'AS BINARY'.")));
        }
        return cstring_to_text("");
    }
    text *result = (text *)palloc0(VARHDRSZ + value_len + SINGLE_BYTE_LENGTH);
    errno_t rc = strncpy_s((char *)VARDATA(result), value_len + SINGLE_BYTE_LENGTH, value, value_len);
    securec_check(rc, "\0", "\0");
    SET_VARSIZE(result, VARHDRSZ + value_len);
    return result;
}

Datum weight_string_bytea(PG_FUNCTION_ARGS)
{
    bytea *in = PG_GETARG_BYTEA_PP(0);
    char *value = VARDATA_ANY(in);
    size_t value_len = VARSIZE_ANY_EXHDR(in);
    text* result = weight_string_bit_bytea_common(value, fcinfo, value_len, true);
    PG_RETURN_TEXT_P(result);
}

Datum weight_string_boolean(PG_FUNCTION_ARGS)
{
    /* return NULL  */
    if (fcinfo->nargs == TWO_NUM_ARGS)
        PG_RETURN_NULL();

    char *type = (char *)VARDATA(PG_GETARG_TEXT_P(1));
    size_t type_len = VARSIZE(PG_GETARG_TEXT_P(1)) - VARHDRSZ;
    if (!strncmp(type, "BINARY", type_len)) {
        bool istrue = PG_GETARG_BOOL(0);
        size_t data_len = PG_GETARG_UINT32(2);
        text *result = (text *)palloc0(VARHDRSZ + data_len);
        *((char *)VARDATA(result)) = istrue ? '1' : '0';
        SET_VARSIZE(result, VARHDRSZ + data_len);
        PG_RETURN_TEXT_P(result);
    } else {
        PG_RETURN_NULL();
    }
}

static text* date_for_binary(char *value, size_t value_len, size_t data_len)
{
    if (value_len > data_len)
        value_len = data_len;
    text *result = (text *)palloc0(VARHDRSZ + data_len + SINGLE_BYTE_LENGTH);
    error_t rc = strncpy_s((char *)VARDATA(result), value_len + SINGLE_BYTE_LENGTH, (char *)value, value_len);
    securec_check(rc, "\0", "\0");
    SET_VARSIZE(result, VARHDRSZ + data_len);
    return result;
}

static text* date_for_char(char *value, size_t value_len, size_t data_len, size_t flags)
{
    if (value_len > data_len)
        value_len = data_len;

    uint flag_dsc = flags & WEIGHTSTRING_DESC_LEVEL1;
    uint flag_rev = flags & WEIGHTSTRING_REVERSE_LEVEL1;

    text *result = (text *)palloc(VARHDRSZ + data_len + SINGLE_BYTE_LENGTH);
    size_t i = 0;
    char *result_string = (char *)VARDATA(result);
    for (; i < value_len; i++) {
        result_string[i] = value[i];
    }
    while (data_len > value_len) {
        result_string[value_len] = ' ';
        value_len++;
    }
    result_string[value_len] = '\0';

    if (flag_dsc || flag_rev) {
        char *start = result_string;
        char *end = start + data_len;
        reverse_and_transform(start, end, flag_dsc, flag_rev);
    }
    SET_VARSIZE(result, VARHDRSZ + data_len);
    return result;
}

static text* weight_string_date_common(char* value, FunctionCallInfoData *fcinfo)
{
    if (fcinfo->nargs == TWO_NUM_ARGS) {
        return cstring_to_text(value);
    } else if (fcinfo->nargs == THREE_NUM_ARGS) {
        size_t value_len = strlen(value);
        size_t data_len = PG_GETARG_UINT32(2);
        text* result = date_for_binary(value, value_len, data_len);
        return result;
    } else {
        size_t value_len = strlen(value);
        size_t data_len = PG_GETARG_UINT32(2);
        size_t flags = PG_GETARG_UINT32(3);
        text* result = date_for_char(value, value_len, data_len, flags);
        return result;
    }
}

Datum weight_string_date(PG_FUNCTION_ARGS)
{
    char* value = NULL;
    DateADT dateVal = PG_GETARG_DATEADT(0);
    value = DatumGetCString(DirectFunctionCall1(date_out, dateVal));
    text* result = weight_string_date_common(value, fcinfo);
    pfree(value);
    PG_RETURN_TEXT_P(result);
}


Datum weight_string_time(PG_FUNCTION_ARGS)
{
    char* value = NULL;
    TimeADT time = PG_GETARG_TIMEADT(0);
    value = DatumGetCString(DirectFunctionCall1(time_out, time));
    text* result = weight_string_date_common(value, fcinfo);
    pfree(value);
    PG_RETURN_TEXT_P(result);
}

Datum weight_string_timestamp(PG_FUNCTION_ARGS)
{
    char* value = NULL;
    Timestamp timestamp = PG_GETARG_TIMESTAMP(0);
    value = DatumGetCString(DirectFunctionCall1(timestamp_out, timestamp));
    text* result = weight_string_date_common(value, fcinfo);
    pfree(value);
    PG_RETURN_TEXT_P(result);
}

Datum weight_string_timestamptz(PG_FUNCTION_ARGS)
{
    char* value = NULL;
    TimestampTz timestamptz = PG_GETARG_TIMESTAMPTZ(0);
    value = DatumGetCString(DirectFunctionCall1(timestamptz_out, timestamptz));
    text* result = weight_string_date_common(value, fcinfo);
    pfree(value);
    PG_RETURN_TEXT_P(result);
}

Datum weight_string_interval(PG_FUNCTION_ARGS)
{
    char* value = NULL;
    Interval* span = PG_GETARG_INTERVAL_P(0);
    value = DatumGetCString(DirectFunctionCall1(interval_out, PointerGetDatum(span)));
    text* result = weight_string_date_common(value, fcinfo);
    pfree(value);
    PG_RETURN_TEXT_P(result);
}

Datum weight_string_bit(PG_FUNCTION_ARGS)
{
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int bit_len = VARBITBYTES(PG_GETARG_VARBIT_P(0));
    bytea* bvalue = bit_to_bytea(arg_int, bit_len);
    char* value = VARDATA_ANY(bvalue);
    size_t value_len = VARSIZE_ANY_EXHDR(value);
    text* result = weight_string_bit_bytea_common(value, fcinfo, value_len, false);
    pfree(bvalue);
    PG_RETURN_TEXT_P(result);
}

#endif

