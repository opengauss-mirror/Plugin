#include <cstdlib>
#include <zlib.h>
#include "postgres.h"
#include "plugin_postgres.h"
#include "knl/knl_variable.h"
#include "utils/builtins.h"
#include "utils/varbit.h"
#include "plugin_commands/mysqlmode.h"

#ifdef DOLPHIN
#define COMP_HDR_SIZE 4
#define BUFFER_TO_ULONG_BYTE0_SHIFT 0
#define BUFFER_TO_ULONG_BYTE1_SHIFT 8
#define BUFFER_TO_ULONG_BYTE2_SHIFT 16
#define BUFFER_TO_ULONG_BYTE3_SHIFT 24
#define BIT_LEN_FACTOR 2
#define MIN_HEX_LOOP 0
#define MAX_HEX_LOOP 127

PG_FUNCTION_INFO_V1_PUBLIC(compress_text);
extern "C" DLL_PUBLIC Datum compress_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(compress_bytea);
extern "C" DLL_PUBLIC Datum compress_bytea(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(compress_boolean);
extern "C" DLL_PUBLIC Datum compress_boolean(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(compress_bit);
extern "C" DLL_PUBLIC Datum compress_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompress_text);
extern "C" DLL_PUBLIC Datum uncompress_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompress_bytea);
extern "C" DLL_PUBLIC Datum uncompress_bytea(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompress_boolean);
extern "C" DLL_PUBLIC Datum uncompress_boolean(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompress_bit);
extern "C" DLL_PUBLIC Datum uncompress_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompressed_length_text);
extern "C" DLL_PUBLIC Datum uncompressed_length_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompressed_length_bytea);
extern "C" DLL_PUBLIC Datum uncompressed_length_bytea(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompressed_length_boolean);
extern "C" DLL_PUBLIC Datum uncompressed_length_boolean(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(uncompressed_length_bit);
extern "C" DLL_PUBLIC Datum uncompressed_length_bit(PG_FUNCTION_ARGS);

static const char hextbl[] = "0123456789abcdef";

static const int8 hexlookup[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,
    4,  5,  6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

static unsigned hex_dec_len(const char *src, unsigned srclen)
{
    return srclen >> 1;
}

static inline char get_hex(char c, bool flag, bool &isValid)
{
    int res = -1;

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
    const char *s = NULL, *srcend;
    char v1, v2, *p;

    srcend = src + len;
    s = src;
    p = dst;
    while (s < srcend) {
        if (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r') {
            s++;
            continue;
        }
        bool isValid = true;
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

static bytea *hex_decode_internal(char *data, int dataLength)
{
    int resultLength, res;
    resultLength = hex_dec_len(data, dataLength);
    errno_t rc = EOK;

    char *originData = (char *)palloc(dataLength + 1);
    rc = memcpy_s(originData, dataLength, data, dataLength);
    securec_check(rc, "\0", "\0");
    originData[dataLength] = '\0';

    const int evenLength = 2;
    char *newData;
    if (dataLength % evenLength != 0) {
        resultLength += 1;
        dataLength += 1;
        newData = (char *)palloc(dataLength + 1);
        *newData = '0';
        rc = memcpy_s(newData + 1, dataLength - 1, data, dataLength - 1);
        securec_check(rc, "\0", "\0");
    } else {
        newData = (char *)palloc(dataLength + 1);
        rc = memcpy_s(newData, dataLength, data, dataLength);
        securec_check(rc, "\0", "\0");
    }
    newData[dataLength] = '\0';

    bytea *result = (bytea *)palloc(VARHDRSZ + resultLength);
    res = hex_decode(newData, dataLength, VARDATA(result), false);
    if (dataLength != 0 && res == 0) {
        int errlevel = SQL_MODE_STRICT() ? ERROR : WARNING;
        ereport(errlevel, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
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

static inline void ulong_to_buffer(Bytef *buffer, ulong value)
{
    *buffer++ = (Bytef)value;
    *buffer++ = (Bytef)(value >> BUFFER_TO_ULONG_BYTE1_SHIFT);
    *buffer++ = (Bytef)(value >> BUFFER_TO_ULONG_BYTE2_SHIFT);
    *buffer = (Bytef)(value >> BUFFER_TO_ULONG_BYTE3_SHIFT);
}

static inline ulong buffer_to_ulong(const Bytef *buffer)
{
    return ((ulong)buffer[0]) | ((ulong)buffer[1] << BUFFER_TO_ULONG_BYTE1_SHIFT) |
           ((ulong)buffer[2] << BUFFER_TO_ULONG_BYTE2_SHIFT) | ((ulong)buffer[3] << BUFFER_TO_ULONG_BYTE3_SHIFT);
}

static bytea *compress_internal(Bytef *input_string, int input_len, ulong *result_len, int err_level)
{
    bytea *result = NULL;
    int estimated_len = 0;
    int err = Z_OK;

    if (input_len == 0) {
        /* Passing in an empty string returns NULL */
        goto err;
    }
    if (VARATT_IS_HUGE_TOAST_POINTER(input_string)) {
        ereport(err_level,
                (errcode(ERRCODE_DATA_EXCEPTION), errmsg("Compressed data size too large; the maximum size is 1GB "
                                                         "(probably, length of compressed data was corrupted)")));
        goto err;
    }

    estimated_len = compressBound(input_len);
    /* estimated buffer should larger than input string. */
    if ((estimated_len + COMP_HDR_SIZE) <= input_len) {
        ereport(err_level, (errcode(ERRCODE_INTERNAL_ERROR),
                            errmsg("ZLIB: compressBound return length %d to small (Input string length id %d)",
                                   estimated_len, input_len)));
        goto err;
    }

    *result_len = COMP_HDR_SIZE + estimated_len;
    result = (bytea *)palloc(VARHDRSZ + COMP_HDR_SIZE + estimated_len);
    err = compress((Bytef *)VARDATA(result) + COMP_HDR_SIZE, result_len, input_string, input_len);
    if (err != Z_OK) {
        if (err == Z_MEM_ERROR) {
            ereport(err_level, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("ZLIB: Not enough memory")));
        } else {
            ereport(err_level,
                    (errcode(ERRCODE_INTERNAL_ERROR), errmsg("ZLIB: Not enough room in the output buffer (probably, "
                                                             "length of uncompressed data was corrupted)")));
        }
        goto err;
    }

    ulong_to_buffer((Bytef *)VARDATA(result), input_len & 0x3FFFFFFF);
    *result_len += COMP_HDR_SIZE;
    SET_VARSIZE(result, VARHDRSZ + *result_len);
    return result;

err:
    *result_len = 0;
    return NULL;
}

static ulong uncompressed_length_internal(Bytef *input_string, ulong input_len, int err_level)
{
    if (input_len == 0) {
        return 0;
    }
    /* If length is less than 4 bytes, data is corrupt */
    if (input_len <= COMP_HDR_SIZE) {
        ereport(err_level, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Input data corrupted")));
        return 0;
    }

    return (buffer_to_ulong(input_string) & 0x3FFFFFFF);
}

static text *uncompress_internal(Bytef *input_string, int input_len, ulong *result_len, int err_level)
{
    text *result = NULL;
    ulong uncompress_len = 0;
    int err = Z_OK;

    if (VARATT_IS_HUGE_TOAST_POINTER(input_string)) {
        ereport(err_level,
                (errcode(ERRCODE_DATA_EXCEPTION), errmsg("Uncompressed data size too large; the maximum size is 1GB "
                                                         "(probably, length of uncompressed data was corrupted)")));
        goto err;
    }

    uncompress_len = uncompressed_length_internal(input_string, input_len, err_level);
    if (uncompress_len == 0) {
        goto err;
    }

    *result_len = uncompress_len + 1;
    result = (text *)palloc(VARHDRSZ + *result_len);
    err = uncompress((Bytef *)VARDATA(result), result_len, input_string + COMP_HDR_SIZE, input_len - COMP_HDR_SIZE);
    if (err != Z_OK) {
        if (err == Z_BUF_ERROR) {
            ereport(err_level,
                    (errcode(ERRCODE_DATA_EXCEPTION), errmsg("ZLIB: Not enough room in the output buffer (probably, "
                                                             "length of uncompressed data was corrupted)")));
        } else if (err == Z_MEM_ERROR) {
            ereport(err_level, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("ZLIB: Not enough memory")));
        } else {
            ereport(err_level, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("ZLIB: Input data corrupted")));
        }
        goto err;
    } else if (*result_len != uncompress_len) {
        ereport(err_level,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("Uncompressed data len %lu dismatch %lu. (probably, length of uncompressed data was corrupted)",
                        *result_len, uncompress_len)));
        goto err;
    }

    *((Bytef *)VARDATA(result) + uncompress_len) = '\0';
    SET_VARSIZE(result, VARHDRSZ + uncompress_len);
    return result;

err:
    *result_len = 0;
    return NULL;
}

Datum compress_text(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    text *input_text = PG_GETARG_TEXT_P(0);
    ulong result_len;

    bytea *result =
        compress_internal((Bytef *)VARDATA(input_text), VARSIZE(input_text) - VARHDRSZ, &result_len, err_level);

    if (result_len <= 0 || result == NULL) {
        PG_RETURN_NULL();
    }

    PG_RETURN_BYTEA_P(result);
}

Datum compress_bytea(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    bytea *input_bytea = PG_GETARG_BYTEA_PP(0);
    ulong result_len;

    bytea *result =
        compress_internal((Bytef *)VARDATA_ANY(input_bytea), VARSIZE_ANY_EXHDR(input_bytea), &result_len, err_level);

    if (result_len <= 0 || result == NULL) {
        PG_RETURN_NULL();
    }

    PG_RETURN_BYTEA_P(result);
}

Datum compress_boolean(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    bool bool_value = PG_GETARG_BOOL(0);
    int bool_length = 1;
    Bytef *bool_string = (Bytef *)palloc(bool_length + 1);
    *bool_string = bool_value ? '1' : '0';
    bool_string[bool_length] = '\0';
    ulong result_len;

    bytea *result = compress_internal(bool_string, bool_length, &result_len, err_level);

    pfree(bool_string);

    if (result_len <= 0 || result == NULL) {
        PG_RETURN_NULL();
    }

    PG_RETURN_BYTEA_P(result);
}

Datum compress_bit(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int bit_len = VARBITBYTES(PG_GETARG_VARBIT_P(0)) * BIT_LEN_FACTOR;
    bytea *input_bytea = bit_to_bytea(arg_int, bit_len);
    ulong result_len;

    bytea *result =
        compress_internal((Bytef *)VARDATA_ANY(input_bytea), VARSIZE_ANY_EXHDR(input_bytea), &result_len, err_level);

    if (result_len <= 0 || result == NULL) {
        PG_RETURN_NULL();
    }

    PG_RETURN_BYTEA_P(result);
}

Datum uncompressed_length_text(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    text *input_text = PG_GETARG_TEXT_P(0);
    int input_len = VARSIZE(PG_GETARG_TEXT_P(0)) - VARHDRSZ;

    PG_RETURN_UINT32(uncompressed_length_internal((Bytef *)VARDATA(input_text), input_len, err_level));
}

Datum uncompressed_length_bytea(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    bytea *input_bytea = PG_GETARG_BYTEA_PP(0);

    PG_RETURN_UINT32(
        uncompressed_length_internal((Bytef *)VARDATA_ANY(input_bytea), VARSIZE_ANY_EXHDR(input_bytea), err_level));
}

Datum uncompressed_length_boolean(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    bool bool_value = PG_GETARG_BOOL(0);
    int bool_length = 1;
    Bytef *bool_string = (Bytef *)palloc(bool_length + 1);
    *bool_string = bool_value ? '1' : '0';
    bool_string[bool_length] = '\0';

    uint32 result = uncompressed_length_internal(bool_string, bool_length, err_level);
    pfree(bool_string);

    PG_RETURN_UINT32(result);
}

Datum uncompressed_length_bit(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int bit_len = VARBITBYTES(PG_GETARG_VARBIT_P(0)) * BIT_LEN_FACTOR;
    bytea *input_bytea = bit_to_bytea(arg_int, bit_len);

    PG_RETURN_UINT32(
        uncompressed_length_internal((Bytef *)VARDATA_ANY(input_bytea), VARSIZE_ANY_EXHDR(input_bytea), err_level));
}

Datum uncompress_text(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    ulong result_len;
    text *input_text = PG_GETARG_TEXT_P(0);

    text *result = uncompress_internal((Bytef *)VARDATA(input_text), VARSIZE(PG_GETARG_TEXT_P(0)) - VARHDRSZ,
                                       &result_len, err_level);

    if (result_len <= 0) {
        PG_RETURN_NULL();
    } else {
        PG_RETURN_TEXT_P(result);
    }
}

Datum uncompress_bytea(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    ulong result_len;
    bytea *input_bytea = PG_GETARG_BYTEA_PP(0);

    text *result =
        uncompress_internal((Bytef *)VARDATA_ANY(input_bytea), VARSIZE_ANY_EXHDR(input_bytea), &result_len, err_level);

    if (result_len <= 0) {
        PG_RETURN_NULL();
    } else {
        PG_RETURN_TEXT_P(result);
    }
}

Datum uncompress_boolean(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    ulong result_len;
    int bool_length = 1;
    bool bool_value = PG_GETARG_BOOL(0);
    Bytef *bool_string = (Bytef *)palloc(bool_length + 1);
    *bool_string = bool_value ? '1' : '0';
    bool_string[bool_length] = '\0';

    text *result = uncompress_internal(bool_string, bool_length, &result_len, err_level);

    pfree(bool_string);

    if (result_len <= 0) {
        PG_RETURN_NULL();
    } else {
        PG_RETURN_TEXT_P(result);
    }
}

Datum uncompress_bit(PG_FUNCTION_ARGS)
{
    int err_level = !fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING;
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int bit_len = VARBITBYTES(PG_GETARG_VARBIT_P(0)) * BIT_LEN_FACTOR;
    bytea *input_bytea = bit_to_bytea(arg_int, bit_len);
    ulong result_len;

    text *result =
        uncompress_internal((Bytef *)VARDATA_ANY(input_bytea), VARSIZE_ANY_EXHDR(input_bytea), &result_len, err_level);

    if (result_len <= 0) {
        PG_RETURN_NULL();
    } else {
        PG_RETURN_TEXT_P(result);
    }
}
#endif /* DOLPHIN */
