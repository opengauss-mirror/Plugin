/* -------------------------------------------------------------------------
 *
 * encode.c
 *	  Various data encoding/decoding things.
 *
 * Copyright (c) 2001-2012, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/encode.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"
#include "plugin_postgres.h"
#include "utils/builtins.h"
#include "utils/numeric.h"
#include "utils/varbit.h"
#include <ctype.h>

#include "utils/builtins.h"
#ifdef DOLPHIN
#include "plugin_commands/mysqlmode.h"
#endif

#ifdef DOLPHIN
struct pg_encoding {
    unsigned (*encode_len)(const char* data, unsigned dlen);
    unsigned (*decode_len)(const char* data, unsigned dlen);
    unsigned (*encode)(const char* data, unsigned dlen, char* res);
    unsigned (*decode)(const char* data, unsigned dlen, char* res, bool flag);
};

static const struct pg_encoding* pg_find_encoding(const char* name);

/*
 * SQL functions.
 */

static text* encode_internal(bytea* data, char* namebuf)
{
    text* result = NULL;
    int datalen, resultlen, res;
    const struct pg_encoding* enc;

    datalen = VARSIZE(data) - VARHDRSZ;

    enc = pg_find_encoding(namebuf);
    if (enc == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("unrecognized encoding: \"%s\"", namebuf)));

    resultlen = enc->encode_len(VARDATA(data), datalen);
    result = (text*)palloc(VARHDRSZ + resultlen);

    res = enc->encode(VARDATA(data), datalen, VARDATA(result));

    /* Make this FATAL 'cause we've trodden on memory ... */
    if (res > resultlen)
        ereport(FATAL, (errcode(ERRCODE_DATA_CORRUPTED), errmsg("overflow - encode estimate too small")));

    SET_VARSIZE(result, VARHDRSZ + res);

    return result;
}

PG_FUNCTION_INFO_V1_PUBLIC(base64_encode);
extern "C" DLL_PUBLIC Datum base64_encode(PG_FUNCTION_ARGS);
Datum base64_encode(PG_FUNCTION_ARGS)
{
    bytea* data = PG_GETARG_BYTEA_P(0);
    text* result = encode_internal(data, "base64");

    PG_RETURN_TEXT_P(result);
}

Datum binary_encode(PG_FUNCTION_ARGS)
{
    bytea* data = PG_GETARG_BYTEA_P(0);
    Datum name = PG_GETARG_DATUM(1);
    text* result = NULL;
    char* namebuf = NULL;

    namebuf = TextDatumGetCString(name);
    result = encode_internal(data, namebuf);

    PG_RETURN_TEXT_P(result);
}

static bytea* decode_internal(text* data, char* namebuf, bool flag)
{
    bytea* result = NULL;
    int datalen, resultlen, res;
    const struct pg_encoding* enc;

    datalen = VARSIZE(data) - VARHDRSZ;

    enc = pg_find_encoding(namebuf);
    if (enc == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("unrecognized encoding: \"%s\"", namebuf)));

    resultlen = enc->decode_len(VARDATA(data), datalen);
    const int evenLength = 2;
    if (strcmp(namebuf, "hex") == 0 && !flag && datalen % evenLength != 0) {
        resultlen += 1;
    }
    result = (bytea*)palloc(VARHDRSZ + resultlen);

    res = enc->decode(VARDATA(data), datalen, VARDATA(result), flag);

    /* Make this FATAL 'cause we've trodden on memory ... */
    if (res > resultlen)
        ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("overflow - decode estimate too small")));

    SET_VARSIZE(result, VARHDRSZ + res);
    return result;
}

Datum binary_decode(PG_FUNCTION_ARGS)
{
    text* data = PG_GETARG_TEXT_P(0);
    Datum name = PG_GETARG_DATUM(1);
    bytea* result = NULL;
    char* namebuf = NULL;

    namebuf = TextDatumGetCString(name);
    result = decode_internal(data, namebuf, true);

    PG_RETURN_BYTEA_P(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(base64_decode);
extern "C" DLL_PUBLIC Datum base64_decode(PG_FUNCTION_ARGS);

Datum base64_decode(PG_FUNCTION_ARGS)
{
    text* decodeData = PG_GETARG_TEXT_P(0);
    bytea* decodeResult = decode_internal(decodeData, "base64", false);

    PG_RETURN_TEXT_P(decodeResult);
}

PG_FUNCTION_INFO_V1_PUBLIC(unhex);
extern "C" DLL_PUBLIC Datum unhex(PG_FUNCTION_ARGS);

Datum unhex(PG_FUNCTION_ARGS)
{
    text* decodeData = PG_GETARG_TEXT_P(0);
    bytea* decodeResult = decode_internal(decodeData, "hex", false);
    
    PG_RETURN_TEXT_P(decodeResult);
}
#endif

/*
 * HEX
 */

static const char hextbl[] = "0123456789abcdef";

static const int8 hexlookup[128] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    10,
    11,
    12,
    13,
    14,
    15,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    10,
    11,
    12,
    13,
    14,
    15,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
};

unsigned hex_encode(const char* src, unsigned len, char* dst)
{
    const char* end = src + len;

    while (src < end) {
        *dst++ = hextbl[(*src >> 4) & 0xF];
        *dst++ = hextbl[*src & 0xF];
        src++;
    }
    return len * 2;
}

#ifdef DOLPHIN
static inline char get_hex(char c, bool flag, bool& isValid)
{
    int res = -1;

    if (c > 0 && c < 127)
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

unsigned hex_decode(const char* src, unsigned len, char* dst, bool flag)
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
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid hexadecimal data: odd number of digits")));
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
#endif

static unsigned hex_enc_len(const char* src, unsigned srclen)
{
    return srclen << 1;
}

static unsigned hex_dec_len(const char* src, unsigned srclen)
{
    return srclen >> 1;
}

/*
 * BASE64
 */

static const char _base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int8 b64lookup[128] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    62,
    -1,
    -1,
    -1,
    63,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    -1,
    -1,
    -1,
    -1,
    -1,
};

static unsigned b64_encode(const char* src, unsigned len, char* dst)
{
    char *p = NULL, *lend = dst + 76;
    const char *s = NULL, *end = src + len;
    int pos = 2;
    uint32 buf = 0;

    s = src;
    p = dst;

    while (s < end) {
        buf |= (unsigned char)*s << (pos << 3);
        pos--;
        s++;

        /* write it out */
        if (pos < 0) {
            *p++ = _base64[(buf >> 18) & 0x3f];
            *p++ = _base64[(buf >> 12) & 0x3f];
            *p++ = _base64[(buf >> 6) & 0x3f];
            *p++ = _base64[buf & 0x3f];

            pos = 2;
            buf = 0;
        }
        if (p >= lend) {
            *p++ = '\n';
            lend = p + 76;
        }
    }
    if (pos != 2) {
        *p++ = _base64[(buf >> 18) & 0x3f];
        *p++ = _base64[(buf >> 12) & 0x3f];
        *p++ = (pos == 0) ? _base64[(buf >> 6) & 0x3f] : '=';
        *p++ = '=';
    }

    return p - dst;
}

#ifdef DOLPHIN
static unsigned b64_decode(const char* src, unsigned len, char* dst, bool flag)
{
    const char *srcend = src + len, *s = src;
    char* p = dst;
    char c;
    int b = 0;
    uint32 buf = 0;
    int pos = 0, end = 0;

    while (s < srcend) {
        c = *s++;

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            continue;

        if (c == '=') {
            /* end sequence */
            if (!end) {
                if (pos == 2)
                    end = 1;
                else if (pos == 3)
                    end = 2;
                else {
                    if (flag)
                        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("unexpected \"=\"")));
                    else {
                        *dst = 0;
                        return 0;
                    }
                }
            }
            b = 0;
        } else {
            b = -1;
            if (c > 0 && c < 127)
                b = b64lookup[(unsigned char)c];
            if (b < 0) {
                if (flag)
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid symbol")));
                else {
                    *dst = 0;
                    return 0;
                }
            }
        }
        /* add it to buffer */
        buf = (buf << 6) + b;
        pos++;
        if (pos == 4) {
            *p++ = (buf >> 16) & 255;
            if (end == 0 || end > 1)
                *p++ = (buf >> 8) & 255;
            if (end == 0 || end > 2)
                *p++ = buf & 255;
            buf = 0;
            pos = 0;
        }
    }

    if (pos != 0) {
        if (flag)
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid end sequence")));
        else {
            *dst = 0;
            return 0;
        }
    }
    return p - dst;
}
#endif

static unsigned b64_enc_len(const char* src, unsigned srclen)
{
    /* 3 bytes will be converted to 4, linefeed after 76 chars */
    return (srclen + 2) * 4 / 3 + srclen / (76 * 3 / 4);
}

static unsigned b64_dec_len(const char* src, unsigned srclen)
{
    return (srclen * 3) >> 2;
}

/*
 * Escape
 * Minimally escape bytea to text.
 * De-escape text to bytea.
 *
 * We must escape zero bytes and high-bit-set bytes to avoid generating
 * text that might be invalid in the current encoding, or that might
 * change to something else if passed through an encoding conversion
 * (leading to failing to de-escape to the original bytea value).
 * Also of course backslash itself has to be escaped.
 *
 * De-escaping processes \\ and any \### octal
 */

#define VAL(CH) ((CH) - '0')
#define DIG(VAL) ((VAL) + '0')

static unsigned esc_encode(const char* src, unsigned srclen, char* dst)
{
    const char* end = src + srclen;
    char* rp = dst;
    int len = 0;

    while (src < end) {
        unsigned char c = (unsigned char)*src;

        if (c == '\0' || IS_HIGHBIT_SET(c)) {
            rp[0] = '\\';
            rp[1] = DIG(c >> 6);
            rp[2] = DIG((c >> 3) & 7);
            rp[3] = DIG(c & 7);
            rp += 4;
            len += 4;
        } else if (c == '\\') {
            rp[0] = '\\';
            rp[1] = '\\';
            rp += 2;
            len += 2;
        } else {
            *rp++ = c;
            len++;
        }

        src++;
    }

    return len;
}

#ifdef DOLPHIN
static unsigned esc_decode(const char* src, unsigned srclen, char* dst, bool flag)
{
    const char* end = src + srclen;
    char* rp = dst;
    int len = 0;

    while (src < end) {
        if (src[0] != '\\')
            *rp++ = *src++;
        else if (src + 3 < end && (src[1] >= '0' && src[1] <= '3') && (src[2] >= '0' && src[2] <= '7') &&
                 (src[3] >= '0' && src[3] <= '7')) {
            unsigned int val;

            val = VAL(src[1]);
            val <<= 3;
            val += VAL(src[2]);
            val <<= 3;
            *rp++ = val + VAL(src[3]);
            src += 4;
        } else if (src + 1 < end && (src[1] == '\\')) {
            *rp++ = '\\';
            src += 2;
        } else {
            /*
             * One backslash, not followed by ### valid octal. Should never
             * get here, since esc_dec_len does same check.
             */
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type bytea")));
        }

        len++;
    }

    return len;
}
#endif

static unsigned esc_enc_len(const char* src, unsigned srclen)
{
    const char* end = src + srclen;
    int len = 0;

    while (src < end) {
        if (*src == '\0' || IS_HIGHBIT_SET(*src))
            len += 4;
        else if (*src == '\\')
            len += 2;
        else
            len++;

        src++;
    }

    return len;
}

static unsigned esc_dec_len(const char* src, unsigned srclen)
{
    const char* end = src + srclen;
    int len = 0;

    while (src < end) {
        if (src[0] != '\\')
            src++;
        else if (src + 3 < end && (src[1] >= '0' && src[1] <= '3') && (src[2] >= '0' && src[2] <= '7') &&
                 (src[3] >= '0' && src[3] <= '7')) {
            /*
             * backslash + valid octal
             */
            src += 4;
        } else if (src + 1 < end && (src[1] == '\\')) {
            /*
             * two backslashes = backslash
             */
            src += 2;
        } else {
            /*
             * one backslash, not followed by ### valid octal
             */
            ereport(
                ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for type bytea")));
        }

        len++;
    }
    return len;
}

/*
 * Common
 */

static const struct {
    const char* name;
    struct pg_encoding enc;
} enclist[] =

    {{"hex", {hex_enc_len, hex_dec_len, hex_encode, hex_decode}},
        {"base64", {b64_enc_len, b64_dec_len, b64_encode, b64_decode}},
        {"escape", {esc_enc_len, esc_dec_len, esc_encode, esc_decode}},
        {NULL, {NULL, NULL, NULL, NULL}}};

static const struct pg_encoding* pg_find_encoding(const char* name)
{
    int i;

    for (i = 0; enclist[i].name; i++)
        if (pg_strcasecmp(enclist[i].name, name) == 0)
            return &enclist[i].enc;

    return NULL;
}

#ifdef DOLPHIN
static text* b64_encode_internal(const char* data, int dataLength)
{
    int resultLength, res;
    resultLength = b64_enc_len(data, dataLength);

    text* result = (text*)palloc(VARHDRSZ + resultLength);
    res = b64_encode(data, dataLength, VARDATA(result));
    SET_VARSIZE(result, VARHDRSZ + res);
    return result;
}

static bytea* b64_decode_internal(const char* data, int dataLength)
{
    int resultLength, res = 0;
    resultLength = b64_dec_len(data, dataLength);

    bytea* result = (bytea*)palloc(VARHDRSZ + resultLength);
    if (resultLength > 0) {
        res = b64_decode(data, dataLength, VARDATA(result), false);
    }
    SET_VARSIZE(result, VARHDRSZ + res);
    return result;
}

static bytea* hex_decode_internal(char* data, int dataLength)
{
    int resultLength, res;
    resultLength = hex_dec_len(data, dataLength);
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

bytea* bit_to_bytea(int64 arg_int, int len)
{
    char* hexResult = (char*)palloc(len + 1);
    hexResult[len] = '\0';
    const int andBits = 15;
    const int rightBits = 4;
    do {
        hexResult[--len] = hextbl[arg_int & andBits];
        arg_int >>= rightBits;
    } while (len > 0);
    bytea* decodeResult = hex_decode_internal(hexResult, strlen(hexResult));
    pfree(hexResult);
    return decodeResult;
}

PG_FUNCTION_INFO_V1_PUBLIC(base64_encode_text);
extern "C" DLL_PUBLIC Datum base64_encode_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(base64_encode_bool);
extern "C" DLL_PUBLIC Datum base64_encode_bool(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(base64_encode_bit);
extern "C" DLL_PUBLIC Datum base64_encode_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(base64_decode_text);
extern "C" DLL_PUBLIC Datum base64_decode_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(base64_decode_bool);
extern "C" DLL_PUBLIC Datum base64_decode_bool(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(base64_decode_bit);
extern "C" DLL_PUBLIC Datum base64_decode_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(hex_decode_text);
extern "C" DLL_PUBLIC Datum hex_decode_text(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(hex_decode_bool);
extern "C" DLL_PUBLIC Datum hex_decode_bool(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(hex_decode_bytea);
extern "C" DLL_PUBLIC Datum hex_decode_bytea(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(hex_decode_bit);
extern "C" DLL_PUBLIC Datum hex_decode_bit(PG_FUNCTION_ARGS);

Datum base64_encode_text(PG_FUNCTION_ARGS)
{
    text* data = PG_GETARG_TEXT_P(0);
    int dataLength = VARSIZE(data) - VARHDRSZ;
    text* result = b64_encode_internal(VARDATA(data), dataLength);
    PG_RETURN_TEXT_P(result);
}

Datum base64_encode_bool(PG_FUNCTION_ARGS)
{
    bool firstArg = PG_GETARG_BOOL(0);
    const char* boolString = firstArg ? "1" : "0";
    text* result = b64_encode_internal(boolString, strlen(boolString));
    PG_RETURN_TEXT_P(result);
}

Datum base64_encode_bit(PG_FUNCTION_ARGS)
{
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int len = VARBITBYTES(PG_GETARG_VARBIT_P(0)) * 2;
    bytea* decodeResult = bit_to_bytea(arg_int, len);

    int dataLength = VARSIZE(decodeResult) - VARHDRSZ;
    text* result = b64_encode_internal(VARDATA(decodeResult), dataLength);
    PG_RETURN_TEXT_P(result);
}

Datum base64_decode_text(PG_FUNCTION_ARGS)
{
    text* data = PG_GETARG_TEXT_P(0);
    int dataLength = VARSIZE(data) - VARHDRSZ;
    bytea* result = b64_decode_internal(VARDATA(data), dataLength);
    int resultLength = VARSIZE(result) - VARHDRSZ;
    if (dataLength != 0 && resultLength == 0) PG_RETURN_NULL();
    PG_RETURN_TEXT_P(result);
}

Datum base64_decode_bool(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}

Datum base64_decode_bit(PG_FUNCTION_ARGS)
{
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int len = VARBITBYTES(PG_GETARG_VARBIT_P(0)) * 2;
    bytea* toBytea = bit_to_bytea(arg_int, len);

    int dataLength = VARSIZE(toBytea) - VARHDRSZ;
    bytea* decodeResult = b64_decode_internal(VARDATA(toBytea), dataLength);
    int resultLength = VARSIZE(decodeResult) - VARHDRSZ;
    if (dataLength != 0 && resultLength == 0)  PG_RETURN_NULL();
    PG_RETURN_TEXT_P(decodeResult);
}

Datum hex_decode_text(PG_FUNCTION_ARGS)
{
    text* data = PG_GETARG_TEXT_P(0);
    int dataLength = VARSIZE(data) - VARHDRSZ;
    if (dataLength == 0) PG_RETURN_TEXT_P(data);
    bytea* result = hex_decode_internal(VARDATA(data), dataLength);
    int resultLength = VARSIZE(result) - VARHDRSZ;
    if (dataLength != 0 && resultLength == 0) PG_RETURN_NULL();
    PG_RETURN_TEXT_P(result);
}

Datum hex_decode_bytea(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}

Datum hex_decode_bit(PG_FUNCTION_ARGS)
{
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, PG_GETARG_DATUM(0)));
    int len = VARBITBYTES(PG_GETARG_VARBIT_P(0)) * 2;
    bytea* toBytea = bit_to_bytea(arg_int, len);

    int dataLength = VARSIZE(toBytea) - VARHDRSZ;
    text* result = hex_decode_internal(VARDATA(toBytea), dataLength);
    int resultLength = VARSIZE(result) - VARHDRSZ;
    if (dataLength != 0 && resultLength == 0) PG_RETURN_NULL();
    PG_RETURN_TEXT_P(result);
}

Datum hex_decode_bool(PG_FUNCTION_ARGS)
{
    bool firstArg = PG_GETARG_BOOL(0);
    const int boolLength = 2;
    char* boolString = (char*)palloc(boolLength + 1);
    boolString[boolLength] = '\0';
    boolString[0] = '0';
    boolString[1] = firstArg ? '1' : '0';
    
    bytea* result = hex_decode_internal(boolString, strlen(boolString));
    pfree(boolString);
    PG_RETURN_TEXT_P(result);
}
#endif