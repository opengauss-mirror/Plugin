/* -------------------------------------------------------------------------
 *
 * varbit.c
 *	  Functions for the SQL datatypes BIT() and BIT VARYING().
 *
 * Code originally contributed by Adriaan Joubert.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/varbit.c
 *
 * -------------------------------------------------------------------------
 */

#include "postgres.h"
#include "knl/knl_variable.h"

#include "access/htup.h"
#include "common/int.h"
#include "libpq/pqformat.h"
#include "nodes/nodeFuncs.h"
#include "nodes/supportnodes.h"
#include "utils/array.h"
#include "access/tuptoaster.h"
#include "plugin_postgres.h"
#ifdef DOLPHIN
#include "plugin_commands/mysqlmode.h"
#include "plugin_utils/int8.h"
#include "plugin_utils/year.h"
#include "plugin_utils/varbit.h"
#define BYTE_SIZE 8
#define SMALL_SIZE 16
#define M_BIT_LEN 64
#else
#include "utils/varbit.h"
#endif


#define HEXDIG(z) ((z) < 10 ? ((z) + '0') : ((z)-10 + 'A'))

static VarBit* bit_catenate(VarBit* arg1, VarBit* arg2);
static VarBit* bitsubstring(VarBit* arg, int32 s, int32 l, bool length_not_specified);
static VarBit* bit_overlay(VarBit* t1, VarBit* t2, int sp, int sl);

#ifdef DOLPHIN
extern int GetLeadingZeroLen(VarBit* arg);
extern Datum dolphin_bitposition(PG_FUNCTION_ARGS);
static int32 bit_cmp(VarBit* arg1, VarBit* arg2, int leadingZeroLen1 = -1, int leadingZeroLen2 = -1);
extern "C" Datum ui8toi1(PG_FUNCTION_ARGS);
extern "C" Datum ui8toi2(PG_FUNCTION_ARGS);
extern "C" Datum ui8toi4(PG_FUNCTION_ARGS);
extern "C" Datum ui8toui1(PG_FUNCTION_ARGS);
extern "C" Datum ui8toui2(PG_FUNCTION_ARGS);
extern "C" Datum ui8toui4(PG_FUNCTION_ARGS);
extern "C" Datum date_int8(PG_FUNCTION_ARGS);
extern "C" Datum datetime_float(PG_FUNCTION_ARGS);
extern "C" Datum timestamptz_int8(PG_FUNCTION_ARGS);
extern "C" Datum time_int8(PG_FUNCTION_ARGS);
extern "C" Datum year_integer(PG_FUNCTION_ARGS);
extern "C" Datum uint8out(PG_FUNCTION_ARGS);
extern "C" Datum dolphin_binaryin(PG_FUNCTION_ARGS);
Datum bittobigint(VarBit* arg, bool isUnsigned, bool canIgnore = false);
PG_FUNCTION_INFO_V1_PUBLIC(cot_bit);
extern "C" DLL_PUBLIC Datum cot_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(bit_bin_in);
extern "C" DLL_PUBLIC Datum bit_bin_in(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(acos_bit);
extern "C" DLL_PUBLIC Datum acos_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(cos_bit);
extern "C" DLL_PUBLIC Datum cos_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(asin_bit);
extern "C" DLL_PUBLIC Datum asin_bit(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1_PUBLIC(atan_bit);
extern "C" DLL_PUBLIC Datum atan_bit(PG_FUNCTION_ARGS);
#endif


PG_FUNCTION_INFO_V1_PUBLIC(c_bitoctetlength);
extern "C" DLL_PUBLIC Datum c_bitoctetlength(PG_FUNCTION_ARGS);

/*
 * common code for bittypmodin and varbittypmodin
 */
static int32 anybit_typmodin(ArrayType* ta, const char* typname)
{
    int32 typmod;
    int32* tl = NULL;
    int n;

    tl = ArrayGetIntegerTypmods(ta, &n);

    /*
     * we're not too tense about good error message here because grammar
     * shouldn't allow wrong number of modifiers for BIT
     */
    if (n != 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid type modifier")));

    if (*tl < 1)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("length for type %s must be at least 1", typname)));
    if (*tl > (MaxAttrSize * BITS_PER_BYTE))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("length for type %s cannot exceed %d", typname, MaxAttrSize * BITS_PER_BYTE)));

    typmod = *tl;

    return typmod;
}

/*
 * common code for bittypmodout and varbittypmodout
 */
static char* anybit_typmodout(int32 typmod)
{
    const size_t buffer_len = 64;

    char* res = (char*)palloc(buffer_len);
    errno_t rc = 0;

    if (typmod >= 0) {
        rc = snprintf_s(res, buffer_len, buffer_len - 1, "(%d)", typmod);
        securec_check_ss(rc, "\0", "\0");
    } else
        *res = '\0';

    return res;
}

/* ----------
 *	attypmod -- contains the length of the bit string in bits, or for
 *			   varying bits the maximum length.
 *
 *	The data structure contains the following elements:
 *	  header  -- length of the whole data structure (incl header)
 *				 in bytes. (as with all varying length datatypes)
 *	  data section -- private data section for the bits data structures
 *		bitlength -- length of the bit string in bits
 *		bitdata   -- bit string, most significant byte first
 *
 *	The length of the bitdata vector should always be exactly as many
 *	bytes as are needed for the given bitlength.  If the bitlength is
 *	not a multiple of 8, the extra low-order padding bits of the last
 *	byte must be zeroes.
 * ----------
 */

/*
 * bit_in -
 *	  converts a char string to the internal representation of a bitstring.
 *		  The length is determined by the number of bits required plus
 *		  VARHDRSZ bytes or from atttypmod.
 */
#ifdef DOLPHIN
Datum bit_in_internal(char* input_string, int32 atttypmod, bool can_ignore)
{
    VarBit* result = NULL;    /* The resulting bit string			  */
    char* sp = NULL;          /* pointer into the character string  */
    bits8* r = NULL;          /* pointer into the result */
    int len,                  /* Length of the whole data structure */
        bitlen,               /* Number of bits in the bit string   */
        slen;                 /* Length of the input string		  */
    bool bit_not_hex = false; /* false = hex string  true = bit string */
    int bc;
    bits8 x = 0;
    
    /* Check that the first character is a b or an x */
    if (input_string[0] == 'b' || input_string[0] == 'B') {
        bit_not_hex = true;
        sp = input_string + 1;
    } else if (input_string[0] == 'x' || input_string[0] == 'X') {
        bit_not_hex = false;
        sp = input_string + 1;
    } else {
        /*
         * Otherwise it's binary.  This allows things like cast('1001' as bit)
         * to work transparently.
         */
        bit_not_hex = true;
        sp = input_string;
    }

    /*
     * Determine bitlength from input string.  MaxAllocSize ensures a regular
     * input is small enough, but we must check hex input.
     */
    slen = strlen(sp);
    if (bit_not_hex)
        bitlen = slen;
    else {
        if (slen > VARBITMAXLEN / 4)
            ereport(ERROR,
                (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                    errmsg("bit string length exceeds the maximum allowed (%d)", VARBITMAXLEN)));
        bitlen = slen * 4;
    }

    /*
     * Sometimes atttypmod is not supplied. If it is supplied we need to make
     * sure that the bitstring fits.
     */
    if (atttypmod <= 0)
        atttypmod = bitlen;
    else if (bitlen != atttypmod)
        ereport(ERROR,
            (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH),
                errmsg("bit string length %d does not match type bit(%d)", bitlen, atttypmod)));

    len = VARBITTOTALLEN(atttypmod);
    /* set to 0 so that *r is always initialised and string is zero-padded */
    result = (VarBit*)palloc0(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = atttypmod;

    r = VARBITS(result);
    if (bit_not_hex) {
        /* Parse the bit representation of the string */
        /* We know it fits, as bitlen was compared to atttypmod */
        x = HIGHBIT;
        for (; *sp; sp++) {
            if (*sp == '1')
                *r |= x;
            else if (*sp != '0') {
                ereport(can_ignore ? WARNING : ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("\"%c\" is not a valid binary digit", *sp)));
                /* if invalid input erro is ignorable, report warning and return a empty varbit */
                PG_RETURN_DATUM((Datum)DirectFunctionCall3(bit_in, CStringGetDatum(""), ObjectIdGetDatum(0), Int32GetDatum(-1)));
            }

            x >>= 1;
            if (x == 0) {
                x = HIGHBIT;
                r++;
            }
        }
    } else {
        /* Parse the hex representation of the string */
        for (bc = 0; *sp; sp++) {
            if (*sp >= '0' && *sp <= '9')
                x = (bits8)(*sp - '0');
            else if (*sp >= 'A' && *sp <= 'F')
                x = (bits8)(*sp - 'A') + 10;
            else if (*sp >= 'a' && *sp <= 'f')
                x = (bits8)(*sp - 'a') + 10;
            else {
                ereport(can_ignore ? WARNING : ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("\"%c\" is not a valid hexadecimal digit", *sp)));
                /* if invalid input erro is ignorable, report warning and return a empty varbit */
                PG_RETURN_DATUM((Datum)DirectFunctionCall3(bit_in, CStringGetDatum(""), ObjectIdGetDatum(0), Int32GetDatum(-1)));
            }

            if (bc) {
                *r++ |= x;
                bc = 0;
            } else {
                *r = x << 4;
                bc = 1;
            }
        }
    }

    PG_RETURN_VARBIT_P(result);
}
#endif

Datum bit_in(PG_FUNCTION_ARGS)
{
    char* input_string = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 atttypmod = PG_GETARG_INT32(2);
    bool can_ignore = fcinfo->can_ignore;
    VarBit* result = NULL;    /* The resulting bit string			  */
    char* sp = NULL;          /* pointer into the character string  */
    int len,                  /* Length of the whole data structure */
        bitlen,               /* Number of bits in the bit string   */
        slen;                 /* Length of the input string		  */

#ifdef DOLPHIN
    if (ENABLE_B_CMPT_MODE) {
        sp = input_string;
        slen = strlen(sp);
        if (slen == 0) {
            PG_RETURN_DATUM((Datum)DirectFunctionCall3(bit_bin_in, CStringGetDatum(""),
                                                       ObjectIdGetDatum(0), Int32GetDatum(-1)));
        }
        if (slen > VARBITMAXLEN / BYTE_SIZE) {
            ereport(ERROR,
                (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                    errmsg("bit string length exceeds the maximum allowed (%d)", VARBITMAXLEN)));
        }
        bitlen = slen * BYTE_SIZE;

        if (atttypmod <= 0) {
            atttypmod = bitlen;
        } else if (bitlen != atttypmod) {
            ereport(ERROR,
                (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH),
                    errmsg("bit string length %d does not match type bit(%d)", bitlen, atttypmod)));
        }

        len = VARBITTOTALLEN(atttypmod);
        /* set to 0 so that *r is always initialised and string is zero-padded */
        result = (VarBit*)palloc0(len);
        SET_VARSIZE(result, len);
        VARBITLEN(result) = atttypmod;

        errno_t ss_rc = 0;
        ss_rc = memcpy_s(VARBITS(result), slen, sp, slen);
        securec_check(ss_rc, "\0", "\0");

        PG_RETURN_VARBIT_P(result);
    }
#endif

    PG_RETURN_VARBIT_P(bit_in_internal(input_string, atttypmod, can_ignore));
}


bool is_req_from_gsql_or_jdbc()
{
    return is_req_from_jdbc() || is_req_from_gsql();
}

Datum bit_out_encode_as_str(VarBit *bits)
{
    char* rp = NULL;
    rp = bit_to_str(bits, true);
    PG_RETURN_CSTRING(rp);
}

Datum bit_out_encode_as_int64(VarBit *bits)
{
    Datum arg_int_datum = DirectFunctionCall1(bittoint8, VarBitPGetDatum(bits));
    return DirectFunctionCall1(int8out, arg_int_datum);
}

Datum bit_out_as_hex(VarBit *bits)
{
    int len = 0;
    char* result = NULL;
    char* rp = NULL;
    int64 arg_int = DatumGetInt64(DirectFunctionCall1(bittoint8, VarBitPGetDatum(bits)));
    len = VARBITBYTES(bits) * 2;
    result = (char*)palloc(len + 3);
    result[len + 2] = '\0';
    rp = &result[len + 1];
    do {
        *rp-- = HEX_CHARS[arg_int & 15];
        len--;
        arg_int >>= 4;
    } while (len > 0);
    *rp-- = 'x';
    *rp = '\\';
    PG_RETURN_CSTRING(rp);
}


Datum bit_out(PG_FUNCTION_ARGS)
{
#ifdef DOLPHIN
    VarBit *bits = PG_GETARG_VARBIT_P(0);
    if (likely(is_req_from_gsql_or_jdbc())) {
        if (GetSessionContext()->bit_output == BIT_OUTPUT_DEC) {
            if (is_req_from_gsql()) {
                return bit_out_encode_as_str(bits);
            } else {
                return bit_out_encode_as_int64(bits);
            }
        } else if (GetSessionContext()->bit_output == BIT_OUTPUT_HEX) {
            return bit_out_as_hex(bits);
        }
    }
#endif
    return varbit_out(fcinfo);
}

/*
 *		bit_recv			- converts external binary format to bit
 */
Datum bit_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 atttypmod = PG_GETARG_INT32(2);
    VarBit* result = NULL;
    int len, bitlen;
    int ipad;

    bitlen = pq_getmsgint(buf, sizeof(int32));
    if (bitlen < 0 || bitlen > VARBITMAXLEN)
        ereport(
            ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("invalid length in external bit string")));

    /*
     * Sometimes atttypmod is not supplied. If it is supplied we need to make
     * sure that the bitstring fits.
     */
    if (atttypmod > 0 && bitlen != atttypmod)
        ereport(ERROR,
            (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH),
                errmsg("bit string length %d does not match type bit(%d)", bitlen, atttypmod)));

    len = VARBITTOTALLEN(bitlen);
    result = (VarBit*)palloc(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = bitlen;

    pq_copymsgbytes(buf, (char*)VARBITS(result), VARBITBYTES(result));
#ifdef DOLPHIN
    /*
     * do left padding, just call shift right to pad 0
     */
    ipad = VARBITPAD(result);
    if (ipad > 0) {
        PG_RETURN_DATUM(DirectFunctionCall3(bitshiftright, VarBitPGetDatum(result),
                                            Int32GetDatum(ipad), BoolGetDatum(false)));
    }
#else
    /* Make sure last byte is zero-padded if needed */
    ipad = VARBITPAD(result);
    if (ipad > 0) {
        mask = BITMASK << ipad;
        *(VARBITS(result) + VARBITBYTES(result) - 1) &= mask;
    }
#endif
    PG_RETURN_VARBIT_P(result);
}

/*
 *		bit_send			- converts bit to binary format
 */
Datum bit_send(PG_FUNCTION_ARGS)
{
    /* Exactly the same as varbit_send, so share code */
    return varbit_send(fcinfo);
}

/*
 * bit()
 * Converts a bit() type to a specific internal length.
 * len is the bitlength specified in the column definition.
 *
 * If doing implicit cast, raise error when source data is wrong length.
 * If doing explicit cast, silently truncate or zero-pad to specified length.
 */
Datum bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int32 len = PG_GETARG_INT32(1);
    VarBit* result = NULL;
    int rlen;
    errno_t ss_rc = 0;

    /* No work if typmod is invalid or supplied data matches it already */
    if (len <= 0 || len > VARBITMAXLEN || len == VARBITLEN(arg))
        PG_RETURN_VARBIT_P(arg);
#ifdef DOLPHIN
    int errlevel = (!fcinfo->can_ignore && SQL_MODE_STRICT() ? ERROR : WARNING);
    if (VARBITLEN(arg) - GetLeadingZeroLen(arg) > len) {
        ereport(errlevel,
            (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH),
                errmsg("bit string length %d does not match type bit(%d)", VARBITLEN(arg), len)));

        rlen = VARBITTOTALLEN(len);
        /* set to 0 so that string is zero-padded */
        result = (VarBit*)palloc0(rlen);
        SET_VARSIZE(result, rlen);
        VARBITLEN(result) = len;

        securec_check(memset_s(VARBITS(result), VARBITBYTES(result), __UINT8_MAX__, VARBITBYTES(result)), "\0", "\0");
        PG_RETURN_VARBIT_P(result);
    }
#else
    bool isExplicit = PG_GETARG_BOOL(2);
    if (!isExplicit && !fcinfo->can_ignore) {
        ereport(ERROR,
            (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH),
                errmsg("bit string length %d does not match type bit(%d)", VARBITLEN(arg), len)));
    }
#endif

    rlen = VARBITTOTALLEN(len);
    /* set to 0 so that string is zero-padded */
    result = (VarBit*)palloc0(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = len;

#ifdef DOLPHIN
    int shift = VARBITLEN(result) - VARBITLEN(arg);
    size_t min_size = Min(VARBITBYTES(result), VARBITBYTES(arg));
    if (shift < 0) {
        if (min_size > 0) {
            Datum cpy = DirectFunctionCall3(bitshiftright, VarBitPGetDatum(arg),
                                            Int32GetDatum(shift), BoolGetDatum(false));
            ss_rc = memcpy_s(VARBITS(result), min_size, VARBITS((VarBit*)cpy), min_size);
            securec_check(ss_rc, "\0", "\0");
            pfree((void*)cpy);
        }
    } else {
        if (min_size > 0) {
            ss_rc = memcpy_s(VARBITS(result), min_size, VARBITS(arg), min_size);
            securec_check(ss_rc, "\0", "\0");
        }
        /*
        * do left padding, just call shift right to pad 0
        */
        PG_RETURN_DATUM(DirectFunctionCall3(bitshiftright, VarBitPGetDatum(result),
                                            Int32GetDatum(shift), BoolGetDatum(false)));
    }
#else
    size_t min_size = Min(VARBITBYTES(result), VARBITBYTES(arg));
    if (min_size > 0) {
        ss_rc = memcpy_s(VARBITS(result), min_size, VARBITS(arg), min_size);
        securec_check(ss_rc, "\0", "\0");
    }

    /*
     * Make sure last byte is zero-padded if needed.  This is useless but safe
     * if source data was shorter than target length (we assume the last byte
     * of the source data was itself correctly zero-padded).
     */
    ipad = VARBITPAD(result);
    if (ipad > 0) {
        mask = BITMASK << ipad;
        *(VARBITS(result) + VARBITBYTES(result) - 1) &= mask;
    }
#endif
    PG_RETURN_VARBIT_P(result);
}

Datum bittypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);

    PG_RETURN_INT32(anybit_typmodin(ta, "bit"));
}

Datum bittypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    PG_RETURN_CSTRING(anybit_typmodout(typmod));
}

/*
 * varbit_in -
 *	  converts a string to the internal representation of a bitstring.
 *		This is the same as bit_in except that atttypmod is taken as
 *		the maximum length, not the exact length to force the bitstring to.
 */
Datum varbit_in(PG_FUNCTION_ARGS)
{
    char* input_string = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 atttypmod = PG_GETARG_INT32(2);
    VarBit* result = NULL;    /* The resulting bit string			  */
    char* sp = NULL;          /* pointer into the character string  */
    bits8* r = NULL;          /* pointer into the result */
    int len,                  /* Length of the whole data structure */
        bitlen,               /* Number of bits in the bit string   */
        slen;                 /* Length of the input string		  */
    bool bit_not_hex = false; /* false = hex string  true = bit string */
    int bc;
    bits8 x = 0;

    /* Check that the first character is a b or an x */
    if (input_string[0] == 'b' || input_string[0] == 'B') {
        bit_not_hex = true;
        sp = input_string + 1;
    } else if (input_string[0] == 'x' || input_string[0] == 'X') {
        bit_not_hex = false;
        sp = input_string + 1;
    } else {
        bit_not_hex = true;
        sp = input_string;
    }

    /*
     * Determine bitlength from input string.  MaxAllocSize ensures a regular
     * input is small enough, but we must check hex input.
     */
    slen = strlen(sp);
    if (bit_not_hex)
        bitlen = slen;
    else {
        if (slen > VARBITMAXLEN / 4)
            ereport(ERROR,
                (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                    errmsg("bit string length exceeds the maximum allowed (%d)", VARBITMAXLEN)));
        bitlen = slen * 4;
    }

    /*
     * Sometimes atttypmod is not supplied. If it is supplied we need to make
     * sure that the bitstring fits.
     */
    if (atttypmod <= 0)
        atttypmod = bitlen;
    else if (bitlen > atttypmod)
        ereport(ERROR,
            (errcode(ERRCODE_STRING_DATA_RIGHT_TRUNCATION),
                errmsg("bit string too long for type bit varying(%d)", atttypmod)));

    len = VARBITTOTALLEN(bitlen);
    /* set to 0 so that *r is always initialised and string is zero-padded */
    result = (VarBit*)palloc0(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = Min(bitlen, atttypmod);

    r = VARBITS(result);
    if (bit_not_hex) {
        /* Parse the bit representation of the string */
        /* We know it fits, as bitlen was compared to atttypmod */
        x = HIGHBIT;
        for (; *sp; sp++) {
            if (*sp == '1')
                *r |= x;
            else if (*sp != '0')
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("\"%c\" is not a valid binary digit", *sp)));

            x >>= 1;
            if (x == 0) {
                x = HIGHBIT;
                r++;
            }
        }
    } else {
        /* Parse the hex representation of the string */
        for (bc = 0; *sp; sp++) {
            if (*sp >= '0' && *sp <= '9')
                x = (bits8)(*sp - '0');
            else if (*sp >= 'A' && *sp <= 'F')
                x = (bits8)(*sp - 'A') + 10;
            else if (*sp >= 'a' && *sp <= 'f')
                x = (bits8)(*sp - 'a') + 10;
            else
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("\"%c\" is not a valid hexadecimal digit", *sp)));

            if (bc) {
                *r++ |= x;
                bc = 0;
            } else {
                *r = x << 4;
                bc = 1;
            }
        }
    }

    PG_RETURN_VARBIT_P(result);
}

/*
 * varbit_out -
 *	  Prints the string as bits to preserve length accurately
 *
 * XXX varbit_recv() and hex input to varbit_in() can load a value that this
 * cannot emit.  Consider using hex output for such values.
 */
Datum varbit_out(PG_FUNCTION_ARGS)
{
    VarBit* s = PG_GETARG_VARBIT_P(0);
    char *result = NULL, *r = NULL;
    bits8* sp = NULL;
    bits8 x;
    int i, k, len;

    len = VARBITLEN(s);
    result = (char*)palloc(len + 1);
    sp = VARBITS(s);
    r = result;
    for (i = 0; i <= len - BITS_PER_BYTE; i += BITS_PER_BYTE, sp++) {
        /* print full bytes */
        x = *sp;
        for (k = 0; k < BITS_PER_BYTE; k++) {
            *r++ = IS_HIGHBIT_SET(x) ? '1' : '0';
            x <<= 1;
        }
    }
    if (i < len) {
        /* print the last partial byte */
        x = *sp;
        for (k = i; k < len; k++) {
            *r++ = IS_HIGHBIT_SET(x) ? '1' : '0';
            x <<= 1;
        }
    }
    *r = '\0';

    /* free memory if allocated by the toaster */
    PG_FREE_IF_COPY(s, 0);

    PG_RETURN_CSTRING(result);
}

/*
 *		varbit_recv			- converts external binary format to varbit
 *
 * External format is the bitlen as an int32, then the byte array.
 */
Datum varbit_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 atttypmod = PG_GETARG_INT32(2);
    VarBit* result = NULL;
    int len, bitlen;
    int ipad;
    bits8 mask;

    bitlen = pq_getmsgint(buf, sizeof(int32));
    if (bitlen < 0 || bitlen > VARBITMAXLEN)
        ereport(
            ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("invalid length in external bit string")));

    /*
     * Sometimes atttypmod is not supplied. If it is supplied we need to make
     * sure that the bitstring fits.
     */
    if (atttypmod > 0 && bitlen > atttypmod)
        ereport(ERROR,
            (errcode(ERRCODE_STRING_DATA_RIGHT_TRUNCATION),
                errmsg("bit string too long for type bit varying(%d)", atttypmod)));

    len = VARBITTOTALLEN(bitlen);
    result = (VarBit*)palloc(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = bitlen;

    pq_copymsgbytes(buf, (char*)VARBITS(result), VARBITBYTES(result));

    /* Make sure last byte is zero-padded if needed */
    ipad = VARBITPAD(result);
    if (ipad > 0) {
        mask = BITMASK << ipad;
        *(VARBITS(result) + VARBITBYTES(result) - 1) &= mask;
    }

    PG_RETURN_VARBIT_P(result);
}

/*
 *		varbit_send			- converts varbit to binary format
 */
Datum varbit_send(PG_FUNCTION_ARGS)
{
    VarBit* s = PG_GETARG_VARBIT_P(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendint32(&buf, VARBITLEN(s));
    pq_sendbytes(&buf, (char*)VARBITS(s), VARBITBYTES(s));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 * varbit_support()
 *
 * Planner support function for the varbit() length coercion function.
 *
 * Currently, the only interesting thing we can do is flatten calls that set
 * the new maximum length >= the previous maximum length.  We can ignore the
 * isExplicit argument, since that only affects truncation cases.
 */
Datum varbit_support(PG_FUNCTION_ARGS)
{
    Node *rawreq = (Node *) PG_GETARG_POINTER(0);
    Node *ret = NULL;

    if (IsA(rawreq, SupportRequestSimplify)) {
        SupportRequestSimplify *req = (SupportRequestSimplify *) rawreq;
        FuncExpr   *expr = req->fcall;
        Node* typmod = NULL;

        Assert(IsA(expr, FuncExpr));
        Assert(list_length(expr->args) >= 2);

        typmod = (Node*)lsecond(expr->args);

        if (IsA(typmod, Const) && !((Const*)typmod)->constisnull) {
            Node* source = (Node*)linitial(expr->args);
            int32 new_typmod = DatumGetInt32(((Const*)typmod)->constvalue);
            int32 old_max = exprTypmod(source);
            int32 new_max = new_typmod;

            /* Note: varbit() treats typmod 0 as invalid, so we do too */
            if (new_max <= 0 || (old_max > 0 && old_max <= new_max))
                ret = relabel_to_typmod(source, new_typmod);
        }
    }

    PG_RETURN_POINTER(ret);
}

/*
 * varbit()
 * Converts a varbit() type to a specific internal length.
 * len is the maximum bitlength specified in the column definition.
 *
 * If doing implicit cast, raise error when source data is too long.
 * If doing explicit cast, silently truncate to max length.
 */
Datum varbit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int32 len = PG_GETARG_INT32(1);
    bool isExplicit = PG_GETARG_BOOL(2);
    VarBit* result = NULL;
    int rlen;
    int ipad;
    bits8 mask;
    errno_t ss_rc = 0;

    /* No work if typmod is invalid or supplied data matches it already */
    if (len <= 0 || len >= VARBITLEN(arg))
        PG_RETURN_VARBIT_P(arg);

    if (!isExplicit)
        ereport(ERROR,
            (errcode(ERRCODE_STRING_DATA_RIGHT_TRUNCATION),
                errmsg("bit string too long for type bit varying(%d)", len)));

    rlen = VARBITTOTALLEN(len);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = len;

    if (VARBITBYTES(result) > 0) {
        ss_rc = memcpy_s(VARBITS(result), VARBITBYTES(result), VARBITS(arg), VARBITBYTES(result));
        securec_check(ss_rc, "\0", "\0");
    }

    /* Make sure last byte is zero-padded if needed */
    ipad = VARBITPAD(result);
    if (ipad > 0) {
        mask = BITMASK << ipad;
        *(VARBITS(result) + VARBITBYTES(result) - 1) &= mask;
    }

    PG_RETURN_VARBIT_P(result);
}

Datum varbittypmodin(PG_FUNCTION_ARGS)
{
    ArrayType* ta = PG_GETARG_ARRAYTYPE_P(0);

    PG_RETURN_INT32(anybit_typmodin(ta, "varbit"));
}

Datum varbittypmodout(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);

    PG_RETURN_CSTRING(anybit_typmodout(typmod));
}

/*
 * Comparison operators
 *
 * We only need one set of comparison operators for bitstrings, as the lengths
 * are stored in the same way for zero-padded and varying bit strings.
 *
 * Note that the standard is not unambiguous about the comparison between
 * zero-padded bit strings and varying bitstrings. If the same value is written
 * into a zero padded bitstring as into a varying bitstring, but the zero
 * padded bitstring has greater length, it will be bigger.
 *
 * Zeros from the beginning of a bitstring cannot simply be ignored, as they
 * may be part of a bit string and may be significant.
 *
 * Note: btree indexes need these routines not to leak memory; therefore,
 * be careful to free working copies of toasted datums.  Most places don't
 * need to be so careful.
 */
#ifdef DOLPHIN
/* Get leading zero length of a varbit */
int GetLeadingZeroLen(VarBit* arg)
{
    int leadingZeroLen = 0;
    int i;
    /*
     * example: for a value b'1' in bit(5), it will be b'00001000', the first 4 zero is left padding,
     * the last 3 zero are not the real value, should be ignored. So we get the leading zero by:
     * 1. traverse the all bytes
     * 2. for every single byte, loop from the highest bit, check whether it's 0, end loop when meet 1
     */
    for (bits8* r = VARBITS(arg); r < VARBITEND(arg) && leadingZeroLen < VARBITLEN(arg); r++) {
        for (i = BITS_PER_BYTE - 1; i >= 0 && leadingZeroLen < VARBITLEN(arg); i--) {
            if (*r >> i == 1) {
                break;
            }
            leadingZeroLen++;
        }
        if (i != -1) {
            break;
        }
    }

    return leadingZeroLen;
}
#endif
/*
 * bit_cmp
 *
 * Compares two bitstrings and returns <0, 0, >0 depending on whether the first
 * string is smaller, equal, or bigger than the second. All bits are considered
 * but additional zero bits won't affect the result.
 */
static int32 bit_cmp(VarBit* arg1, VarBit* arg2, int leadingZeroLen1, int leadingZeroLen2)
{
#ifdef DOLPHIN
    leadingZeroLen1 = (leadingZeroLen1 == -1 ? GetLeadingZeroLen(arg1) : leadingZeroLen1);
    leadingZeroLen2 = (leadingZeroLen2 == -1 ? GetLeadingZeroLen(arg2) : leadingZeroLen2);
    VarBit* newArg1 = arg1;
    VarBit* newArg2 = arg2;
    /* do shit left to remove leading zero */
    if (leadingZeroLen1 > 0) {
        newArg1 = DatumGetVarBitP(
            DirectFunctionCall3(bitshiftleft, VarBitPGetDatum(arg1),
                                Int32GetDatum(leadingZeroLen1), BoolGetDatum(false)));
    }
    if (leadingZeroLen2 > 0) {
        newArg2 = DatumGetVarBitP(
            DirectFunctionCall3(bitshiftleft, VarBitPGetDatum(arg2),
                                Int32GetDatum(leadingZeroLen2), BoolGetDatum(false)));
    }
    int bytelen1 = VARBITBYTES(newArg1);
    int bytelen2 = VARBITBYTES(newArg2);

    int cmp = memcmp(VARBITS(newArg1), VARBITS(newArg2), Min(bytelen1, bytelen2));
    /*
     * cmp == 0 doesn't mean they are equal, for example b'1' for bit(1) and b'10' for bit(2).
     * we should check bitlen to get the final result.
     * */
    if (cmp == 0) {
        int bitlen1 = VARBITLEN(newArg1) - leadingZeroLen1;
        int bitlen2 = VARBITLEN(newArg2) - leadingZeroLen2;
        if (bitlen1 != bitlen2) {
            cmp = (bitlen1 < bitlen2) ? -1 : 1;
        }
    }
    /* free mem if newArg is palloced by shiftleft */
    if (newArg1 != arg1) {
        pfree(newArg1);
    }
    if (newArg2 != arg2) {
        pfree(newArg2);
    }
#else
    int bitlen1, bytelen1, bitlen2, bytelen2;
    int32 cmp;

    bytelen1 = VARBITBYTES(arg1);
    bytelen2 = VARBITBYTES(arg2);

    cmp = memcmp(VARBITS(arg1), VARBITS(arg2), Min(bytelen1, bytelen2));
    if (cmp == 0) {
        bitlen1 = VARBITLEN(arg1);
        bitlen2 = VARBITLEN(arg2);
        if (bitlen1 != bitlen2)
            cmp = (bitlen1 < bitlen2) ? -1 : 1;
    }
#endif
    return cmp;
}

Datum biteq(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    bool result = false;
    int bitlen1, bitlen2;
    int leadingZeroLen1 = GetLeadingZeroLen(arg1);
    int leadingZeroLen2 = GetLeadingZeroLen(arg2);

    bitlen1 = VARBITLEN(arg1) - leadingZeroLen1;
    bitlen2 = VARBITLEN(arg2) - leadingZeroLen2;

    /* fast path for different-length inputs */
    if (bitlen1 != bitlen2)
        result = false;
    else
        result = (bit_cmp(arg1, arg2, leadingZeroLen1, leadingZeroLen2) == 0);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_BOOL(result);
}

Datum bitne(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    bool result = false;
    int bitlen1, bitlen2;
    int leadingZeroLen1 = GetLeadingZeroLen(arg1);
    int leadingZeroLen2 = GetLeadingZeroLen(arg2);

    bitlen1 = VARBITLEN(arg1) - leadingZeroLen1;
    bitlen2 = VARBITLEN(arg2) - leadingZeroLen2;

    /* fast path for different-length inputs */
    if (bitlen1 != bitlen2)
        result = true;
    else
        result = (bit_cmp(arg1, arg2, leadingZeroLen1, leadingZeroLen2) != 0);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_BOOL(result);
}

Datum bitlt(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    bool result = false;

    result = (bit_cmp(arg1, arg2) < 0);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_BOOL(result);
}

Datum bitle(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    bool result = false;

    result = (bit_cmp(arg1, arg2) <= 0);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_BOOL(result);
}

Datum bitgt(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    bool result = false;

    result = (bit_cmp(arg1, arg2) > 0);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_BOOL(result);
}

Datum bitge(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    bool result = false;

    result = (bit_cmp(arg1, arg2) >= 0);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_BOOL(result);
}

Datum bitcmp(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    int32 result;

    result = bit_cmp(arg1, arg2);

    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_INT32(result);
}

/*
 * bitcat
 * Concatenation of bit strings
 */
Datum bitcat(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);

    PG_RETURN_VARBIT_P(bit_catenate(arg1, arg2));
}

static VarBit* bit_catenate(VarBit* arg1, VarBit* arg2)
{
    VarBit* result = NULL;
    int bitlen1, bitlen2, bytelen, bit1pad, bit2shift;
    bits8 *pr = NULL, *pa = NULL;
    errno_t ss_rc = 0;

    bitlen1 = VARBITLEN(arg1);
    bitlen2 = VARBITLEN(arg2);

    if (bitlen1 > VARBITMAXLEN - bitlen2)
        ereport(ERROR,
            (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                errmsg("bit string length exceeds the maximum allowed (%d)", VARBITMAXLEN)));
    bytelen = VARBITTOTALLEN(bitlen1 + bitlen2);

    result = (VarBit*)palloc(bytelen);
    SET_VARSIZE(result, bytelen);
    VARBITLEN(result) = bitlen1 + bitlen2;

    /* Copy the first bitstring in */
    if (VARBITBYTES(arg1) > 0) {
        ss_rc = memcpy_s(VARBITS(result), VARBITBYTES(arg1), VARBITS(arg1), VARBITBYTES(arg1));
        securec_check(ss_rc, "\0", "\0");
    }

    /* Copy the second bit string */
    bit1pad = VARBITPAD(arg1);
    if (bit1pad == 0) {
        if (VARBITBYTES(arg2) > 0) {
            ss_rc = memcpy_s(VARBITS(result) + VARBITBYTES(arg1), VARBITBYTES(arg2), VARBITS(arg2), VARBITBYTES(arg2));
            securec_check(ss_rc, "\0", "\0");
        }
    } else if (bitlen2 > 0) {
        /* We need to shift all the bits to fit */
        bit2shift = BITS_PER_BYTE - bit1pad;
        pr = VARBITS(result) + VARBITBYTES(arg1) - 1;
        for (pa = VARBITS(arg2); pa < VARBITEND(arg2); pa++) {
            *pr |= ((*pa >> bit2shift) & BITMASK);
            pr++;
            if (pr < VARBITEND(result))
                *pr = (*pa << bit1pad) & BITMASK;
        }
    }

    return result;
}

/*
 * bitsubstr
 * retrieve a substring from the bit string.
 * Note, s is 1-based.
 * SQL draft 6.10 9)
 */
Datum bitsubstr(PG_FUNCTION_ARGS)
{
    PG_RETURN_VARBIT_P(bitsubstring(PG_GETARG_VARBIT_P(0), PG_GETARG_INT32(1), PG_GETARG_INT32(2), false));
}

Datum bitsubstr_no_len(PG_FUNCTION_ARGS)
{
    PG_RETURN_VARBIT_P(bitsubstring(PG_GETARG_VARBIT_P(0), PG_GETARG_INT32(1), -1, true));
}

static VarBit* bitsubstring(VarBit* arg, int32 s, int32 l, bool length_not_specified)
{
    VarBit* result = NULL;
    int bitlen, rbitlen, len, ipad = 0, ishift, i;
    int e, s1, e1;
    bits8 mask, *r = NULL, *ps = NULL;
    errno_t ss_rc = 0;

    bitlen = VARBITLEN(arg);
    s1 = Max(s, 1);
    /* If we do not have an upper bound, use end of string */
    if (length_not_specified) {
        e1 = bitlen + 1;
    } else if (l < 0) {
        /* SQL99 says to throw an error for E < S, i.e., negative length */
        ereport(ERROR,
                (errcode(ERRCODE_SUBSTRING_ERROR),
                 errmsg("negative substring length not allowed")));
        e1 = -1;				/* silence stupider compilers */
    } else if (pg_add_s32_overflow(s, l, &e)) {
        /*
         * L could be large enough for S + L to overflow, in which case the
         * substring must run to end of string.
         */
        e1 = bitlen + 1;
    } else {
        e1 = Min(e, bitlen + 1);
    }
    if (s1 > bitlen || e1 <= s1) {
        /* Need to return a zero-length bitstring */
        len = VARBITTOTALLEN(0);
        result = (VarBit*)palloc(len);
        SET_VARSIZE(result, len);
        VARBITLEN(result) = 0;
    } else {
        /*
         * OK, we've got a true substring starting at position s1-1 and ending
         * at position e1-1
         */
        rbitlen = e1 - s1;
        len = VARBITTOTALLEN(rbitlen);
        result = (VarBit*)palloc(len);
        SET_VARSIZE(result, len);
        VARBITLEN(result) = rbitlen;
        len -= VARHDRSZ + VARBITHDRSZ;
        /* Are we copying from a byte boundary? */
        if ((s1 - 1) % BITS_PER_BYTE == 0) {
            /* Yep, we are copying bytes */
            if (len > 0) {
                ss_rc = memcpy_s(VARBITS(result), len, VARBITS(arg) + (s1 - 1) / BITS_PER_BYTE, len);
                securec_check(ss_rc, "\0", "\0");
            }
        } else {
            /* Figure out how much we need to shift the sequence by */
            ishift = (s1 - 1) % BITS_PER_BYTE;
            r = VARBITS(result);
            ps = VARBITS(arg) + (s1 - 1) / BITS_PER_BYTE;
            for (i = 0; i < len; i++) {
                *r = (*ps << ishift) & BITMASK;
                if ((++ps) < VARBITEND(arg))
                    *r |= *ps >> (BITS_PER_BYTE - ishift);
                r++;
            }
        }
        /* Do we need to pad at the end? */
        ipad = VARBITPAD(result);
        if (ipad > 0) {
            mask = BITMASK << ipad;
            *(VARBITS(result) + len - 1) &= mask;
        }
    }

    return result;
}

/*
 * bitoverlay
 *	Replace specified substring of first string with second
 *
 * The SQL standard defines OVERLAY() in terms of substring and concatenation.
 * This code is a direct implementation of what the standard says.
 */
Datum bitoverlay(PG_FUNCTION_ARGS)
{
    VarBit* t1 = PG_GETARG_VARBIT_P(0);
    VarBit* t2 = PG_GETARG_VARBIT_P(1);
    int sp = PG_GETARG_INT32(2); /* substring start position */
    int sl = PG_GETARG_INT32(3); /* substring length */

    PG_RETURN_VARBIT_P(bit_overlay(t1, t2, sp, sl));
}

Datum bitoverlay_no_len(PG_FUNCTION_ARGS)
{
    VarBit* t1 = PG_GETARG_VARBIT_P(0);
    VarBit* t2 = PG_GETARG_VARBIT_P(1);
    int sp = PG_GETARG_INT32(2); /* substring start position */
    int sl;

    sl = VARBITLEN(t2); /* defaults to length(t2) */
    PG_RETURN_VARBIT_P(bit_overlay(t1, t2, sp, sl));
}

static VarBit* bit_overlay(VarBit* t1, VarBit* t2, int sp, int sl)
{
    VarBit* result = NULL;
    VarBit* s1 = NULL;
    VarBit* s2 = NULL;
    int sp_pl_sl;

    /*
     * Check for possible integer-overflow cases.  For negative sp, throw a
     * "substring length" error because that's what should be expected
     * according to the spec's definition of OVERLAY().
     */
    if (sp <= 0)
        ereport(ERROR, (errcode(ERRCODE_SUBSTRING_ERROR), errmsg("negative substring length not allowed")));
    if (pg_add_s32_overflow(sp, sl, &sp_pl_sl))
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer out of range")));

    s1 = bitsubstring(t1, 1, sp - 1, false);
    s2 = bitsubstring(t1, sp_pl_sl, -1, true);
    result = bit_catenate(s1, t2);
    result = bit_catenate(result, s2);

    return result;
}

/*
 * bitlength, bitoctetlength
 * Return the length of a bit string
 */
Datum bitlength(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
#ifdef DOLPHIN
    PG_RETURN_INT32(VARBITBYTES(arg));
#else
    PG_RETURN_INT32(VARBITLEN(arg));
#endif
}

Datum bitoctetlength(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);

    PG_RETURN_INT32(VARBITBYTES(arg));
}

Datum c_bitoctetlength(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);

    PG_RETURN_INT32(VARBITBYTES(arg));
}

/*
 * bit_and
 * perform a logical AND on two bit strings.
 */
Datum bit_and(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    VarBit* result = NULL;
    int len, bitlen1, bitlen2, i;
    bits8 *p1 = NULL, *p2 = NULL, *r = NULL;

    bitlen1 = VARBITLEN(arg1);
    bitlen2 = VARBITLEN(arg2);
    if (bitlen1 != bitlen2)
        ereport(
            ERROR, (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH), errmsg("cannot AND bit strings of different sizes")));

    len = VARSIZE(arg1);
    result = (VarBit*)palloc(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = bitlen1;

    p1 = VARBITS(arg1);
    p2 = VARBITS(arg2);
    r = VARBITS(result);
    for (i = 0; (unsigned int)(i) < VARBITBYTES(arg1); i++)
        *r++ = *p1++ & *p2++;

    /* Padding is not needed as & of 0 pad is 0 */

    PG_RETURN_VARBIT_P(result);
}

/*
 * bit_or
 * perform a logical OR on two bit strings.
 */
Datum bit_or(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    VarBit* result = NULL;
    int len, bitlen1, bitlen2, i;
    bits8 *p1 = NULL, *p2 = NULL, *r = NULL;
    bits8 mask;

    bitlen1 = VARBITLEN(arg1);
    bitlen2 = VARBITLEN(arg2);
    if (bitlen1 != bitlen2)
        ereport(
            ERROR, (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH), errmsg("cannot OR bit strings of different sizes")));
    len = VARSIZE(arg1);
    result = (VarBit*)palloc(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = bitlen1;

    p1 = VARBITS(arg1);
    p2 = VARBITS(arg2);
    r = VARBITS(result);
    for (i = 0; (unsigned int)(i) < VARBITBYTES(arg1); i++)
        *r++ = *p1++ | *p2++;

    /* Pad the result */
    mask = BITMASK << VARBITPAD(result);
    if (mask) {
        r--;
        *r &= mask;
    }

    PG_RETURN_VARBIT_P(result);
}

/*
 * bitxor
 * perform a logical XOR on two bit strings.
 */
Datum bitxor(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    VarBit* arg2 = PG_GETARG_VARBIT_P(1);
    VarBit* result = NULL;
    int len, bitlen1, bitlen2, i;
    bits8 *p1 = NULL, *p2 = NULL, *r = NULL;
    bits8 mask;

    bitlen1 = VARBITLEN(arg1);
    bitlen2 = VARBITLEN(arg2);
    if (bitlen1 != bitlen2) {
#ifdef DOLPHIN
        if (bitlen1 > bitlen2) {
            arg2 = DatumGetVarBitP(DirectFunctionCall2Coll(bit, InvalidOid, VarBitPGetDatum(arg2), Int32GetDatum(bitlen1)));
        } else {
            arg1 = DatumGetVarBitP(DirectFunctionCall2Coll(bit, InvalidOid, VarBitPGetDatum(arg1), Int32GetDatum(bitlen2)));
        }
#else   
        ereport(
            ERROR, (errcode(ERRCODE_STRING_DATA_LENGTH_MISMATCH), errmsg("cannot XOR bit strings of different sizes")));
#endif
    }

    len = VARSIZE(arg1);
    result = (VarBit*)palloc(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = VARBITLEN(arg1);

    p1 = VARBITS(arg1);
    p2 = VARBITS(arg2);
    r = VARBITS(result);
    for (i = 0; (unsigned int)(i) < VARBITBYTES(arg1); i++)
        *r++ = *p1++ ^ *p2++;

    /* Pad the result */
    mask = BITMASK << VARBITPAD(result);
    if (mask) {
        r--;
        *r &= mask;
    }

    PG_RETURN_VARBIT_P(result);
}

/*
 * bitnot
 * perform a logical NOT on a bit string.
 */
Datum bitnot(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    VarBit* result = NULL;
    bits8 *p = NULL, *r = NULL;
    bits8 mask;

    result = (VarBit*)palloc(VARSIZE(arg));
    SET_VARSIZE(result, VARSIZE(arg));
    VARBITLEN(result) = VARBITLEN(arg);

    p = VARBITS(arg);
    r = VARBITS(result);
    for (; p < VARBITEND(arg); p++)
        *r++ = ~*p;

    /* Pad the result */
    mask = BITMASK << VARBITPAD(result);
    if (mask) {
        r--;
        *r &= mask;
    }

    PG_RETURN_VARBIT_P(result);
}

/*
 * bitshiftleft
 * do a left shift (i.e. towards the beginning of the string)
 */
Datum bitshiftleft(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int32 shft = PG_GETARG_INT32(1);
#ifdef DOLPHIN
    bool shft_more = PG_GETARG_BOOL(2);
#endif
    VarBit* result = NULL;
    int byte_shift, ishift, len;
    bits8 *p = NULL, *r = NULL;
    errno_t ss_rc = 0;

    /* Negative shift is a shift to the right */
    if (shft < 0) {
        /* Prevent integer overflow in negation */
        if (shft < -VARBITMAXLEN)
            shft = -VARBITMAXLEN;
#ifdef DOLPHIN
        PG_RETURN_DATUM(DirectFunctionCall3(bitshiftright, VarBitPGetDatum(arg),
                                            Int32GetDatum(-shft), BoolGetDatum(shft_more)));
#else
        PG_RETURN_DATUM(DirectFunctionCall2(bitshiftright, VarBitPGetDatum(arg), Int32GetDatum(-shft)));
#endif
    }

    result = (VarBit*)palloc(VARSIZE(arg));
    SET_VARSIZE(result, VARSIZE(arg));
    VARBITLEN(result) = VARBITLEN(arg);
    r = VARBITS(result);

    /* If we shifted all the bits out, return an all-zero string */
    if (shft >= VARBITLEN(arg)) {
        ss_rc = memset_s(r, VARBITBYTES(arg), 0, VARBITBYTES(arg));
        securec_check(ss_rc, "\0", "\0");
        PG_RETURN_VARBIT_P(result);
    }

    byte_shift = shft / BITS_PER_BYTE;
    ishift = shft % BITS_PER_BYTE;
    p = VARBITS(arg) + byte_shift;

    if (ishift == 0) {
        /* Special case: we can do a memcpy */
        len = VARBITBYTES(arg) - byte_shift;
        if (len > 0) {
            ss_rc = memcpy_s(r, len, p, len);
            securec_check(ss_rc, "\0", "\0");
        }
        ss_rc = memset_s(r + len, byte_shift, 0, byte_shift);
        securec_check(ss_rc, "\0", "\0");
    } else {
        for (; p < VARBITEND(arg); r++) {
            *r = *p << ishift;
            if ((++p) < VARBITEND(arg))
                *r |= *p >> (BITS_PER_BYTE - ishift);
        }
        for (; r < VARBITEND(result); r++)
            *r = 0;
    }

    PG_RETURN_VARBIT_P(result);
}

/*
 * bitshiftright
 * do a right shift (i.e. towards the end of the string)
 */
Datum bitshiftright(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int32 shft = PG_GETARG_INT32(1);
#ifdef DOLPHIN
    bool shft_more = PG_GETARG_BOOL(2);
#endif
    VarBit* result = NULL;
    int byte_shift, ishift, len;
    bits8 *p = NULL, *r = NULL;
    errno_t ss_rc = 0;

    /* Negative shift is a shift to the left */
    if (shft < 0) {
        /* Prevent integer overflow in negation */
        if (shft < -VARBITMAXLEN)
            shft = -VARBITMAXLEN;
#ifdef DOLPHIN
        PG_RETURN_DATUM(DirectFunctionCall3(bitshiftleft, VarBitPGetDatum(arg),
                                            Int32GetDatum(-shft), BoolGetDatum(false)));
#else
        PG_RETURN_DATUM(DirectFunctionCall2(bitshiftleft, VarBitPGetDatum(arg), Int32GetDatum(-shft)));
#endif
    }

    result = (VarBit*)palloc(VARSIZE(arg));
    SET_VARSIZE(result, VARSIZE(arg));
    VARBITLEN(result) = VARBITLEN(arg);
    r = VARBITS(result);

    /* If we shifted all the bits out, return an all-zero string */
#ifdef DOLPHIN
    if (shft >= VARBITLEN(arg) && !shft_more) {
#else
    if (shft >= VARBITLEN(arg)) {
#endif
        ss_rc = memset_s(r, VARBITBYTES(arg), 0, VARBITBYTES(arg));
        securec_check(ss_rc, "\0", "\0");
        PG_RETURN_VARBIT_P(result);
    }

    byte_shift = shft / BITS_PER_BYTE;
    ishift = shft % BITS_PER_BYTE;
    p = VARBITS(arg);

    /* Set the first part of the result to 0 */
    ss_rc = memset_s(r, byte_shift, 0, byte_shift);
    securec_check(ss_rc, "\0", "\0");
    r += byte_shift;

    if (ishift == 0) {
        /* Special case: we can do a memcpy */
        len = VARBITBYTES(arg) - byte_shift;
        if (len > 0) {
            ss_rc = memcpy_s(r, len, p, len);
            securec_check(ss_rc, "\0", "\0");
        }
    } else {
        if (r < VARBITEND(result))
            *r = 0; /* initialize first byte */
        for (; r < VARBITEND(result); p++) {
            *r |= *p >> ishift;
            if ((++r) < VARBITEND(result))
                *r = (*p << (BITS_PER_BYTE - ishift)) & BITMASK;
        }
    }

    PG_RETURN_VARBIT_P(result);
}

#ifdef DOLPHIN

Datum bittotinyint(VarBit* arg, bool isUnsigned)
{
    uint8 result;
    bits8* r = NULL;
    int errlevel = SQL_MODE_STRICT() ? ERROR : WARNING;

    if (ENABLE_B_CMPT_MODE) {
        result = isUnsigned ? (uint8)DirectFunctionCall1(ui8toui1, Int64GetDatum(bittobigint(arg, true))) :
                 (uint8)DirectFunctionCall1(ui8toi1, Int64GetDatum(bittobigint(arg, true)));
        PG_RETURN_INT8(result);
    }

    /* Check that the bit string is not too long */
    if ((uint32)VARBITLEN(arg) - GetLeadingZeroLen(arg) > sizeof(result) * BITS_PER_BYTE) {
        if (isUnsigned) {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
            PG_RETURN_UINT8(PG_UINT8_MAX);
        } else {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint out of range")));
            PG_RETURN_UINT8(PG_INT8_MAX);
        }
    }

    result = 0;
    for (r = VARBITS(arg); r < VARBITEND(arg); r++) {
        result <<= BITS_PER_BYTE;
        result |= *r;
    }
    /* Now shift the result to take account of the padding at the end */
    result >>= VARBITPAD(arg);
    if (isUnsigned)
        PG_RETURN_UINT8(result);
    PG_RETURN_INT8(result);
}

Datum bittosmallint(VarBit* arg, bool isUnsigned)
{
    uint16 result;
    bits8* r = NULL;
    int errlevel = SQL_MODE_STRICT() ? ERROR : WARNING;

    if (ENABLE_B_CMPT_MODE) {
        result = isUnsigned ? (uint16)DirectFunctionCall1(ui8toui2, Int64GetDatum(bittobigint(arg, true))) :
                 (uint16)DirectFunctionCall1(ui8toi2, Int64GetDatum(bittobigint(arg, true)));
        PG_RETURN_INT16(result);
    }

    /* Check that the bit string is not too long */
    if ((uint32)VARBITLEN(arg) - GetLeadingZeroLen(arg) > sizeof(result) * BITS_PER_BYTE) {
        if (isUnsigned) {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned out of range")));
            PG_RETURN_UINT16(PG_UINT16_MAX);
        } else {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint out of range")));
            PG_RETURN_UINT16(PG_INT16_MAX);
        }
    }

    result = 0;
    for (r = VARBITS(arg); r < VARBITEND(arg); r++) {
        result <<= BITS_PER_BYTE;
        result |= *r;
    }
    /* Now shift the result to take account of the padding at the end */
    result >>= VARBITPAD(arg);
    if (isUnsigned)
        PG_RETURN_UINT16(result);
    PG_RETURN_INT16(result);
}

Datum bittoint(VarBit* arg, bool isUnsigned)
{
    uint32 result;
    bits8* r = NULL;
    int errlevel = SQL_MODE_STRICT() ? ERROR : WARNING;

    if (ENABLE_B_CMPT_MODE) {
        result = isUnsigned ? (uint32)DirectFunctionCall1(ui8toui4, Int64GetDatum(bittobigint(arg, true))) :
                 (uint32)DirectFunctionCall1(ui8toi4, Int64GetDatum(bittobigint(arg, true)));
        PG_RETURN_INT32(result);
    }

    /* Check that the bit string is not too long */
    if ((uint32)VARBITLEN(arg) - GetLeadingZeroLen(arg) > sizeof(result) * BITS_PER_BYTE) {
        if (isUnsigned) {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer unsigned out of range")));
            PG_RETURN_UINT32(PG_UINT32_MAX);
        } else {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer out of range")));
            PG_RETURN_UINT32(PG_INT32_MAX);
        }
    }

    result = 0;
    for (r = VARBITS(arg); r < VARBITEND(arg); r++) {
        result <<= BITS_PER_BYTE;
        result |= *r;
    }
    /* Now shift the result to take account of the padding at the end */
    result >>= VARBITPAD(arg);
    if (isUnsigned)
        PG_RETURN_UINT32(result);
    PG_RETURN_INT32(result);
}

Datum bittobigint(VarBit* arg, bool isUnsigned, bool canIgnore)
{
    uint64 result;
    bits8* r = NULL;
    int errlevel = !canIgnore && SQL_MODE_STRICT() ? ERROR : WARNING;

    /* Check that the bit string is not too long */
    if ((uint32)VARBITLEN(arg) - GetLeadingZeroLen(arg) > sizeof(result) * BITS_PER_BYTE) {
        if (isUnsigned) {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned out of range")));
            PG_RETURN_UINT64(PG_UINT64_MAX);
        } else {
            ereport(errlevel, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint out of range")));
            PG_RETURN_INT64(PG_INT64_MAX);
        }
    }

    result = 0;
    for (r = VARBITS(arg); r < VARBITEND(arg); r++) {
        result <<= BITS_PER_BYTE;
        result |= *r;
    }
    /* Now shift the result to take account of the padding at the end */
    result >>= VARBITPAD(arg);
    if (isUnsigned)
        PG_RETURN_UINT64(result);
    PG_RETURN_INT64(result);
}

Datum bitfromtinyint(int16 a, int32 typmod)
{
    VarBit* result = NULL;
    bits8* r = NULL;
    int rlen;
    int destbitsleft, srcbitsleft;

    if (typmod <= 0 || typmod > VARBITMAXLEN)
        typmod = 1; /* default bit length */

    if (typmod < M_BIT_LEN && ENABLE_B_CMPT_MODE) {
        return DirectFunctionCall2(bit, bitfromtinyint(a, M_BIT_LEN), Int32GetDatum(typmod));
    }

    rlen = VARBITTOTALLEN(typmod);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = typmod;

    r = VARBITS(result);
    destbitsleft = typmod;
    srcbitsleft = BYTE_SIZE;
    /* drop any input bits that don't fit */
    srcbitsleft = Min(srcbitsleft, destbitsleft);
    /* sign-fill any excess bytes in output */
    while (destbitsleft >= srcbitsleft + 8) {
        *r++ = (bits8)((a < 0) ? BITMASK : 0);
        destbitsleft -= 8;
    }
    /* store first fractional byte */
    if (destbitsleft > srcbitsleft) {
        int val = (int)(a >> (destbitsleft - 8));

        /* Force sign-fill in case the compiler implements >> as zero-fill */
        if (a < 0)
            val |= (-1) << (srcbitsleft + 8 - destbitsleft);
        *r++ = (bits8)(val & BITMASK);
        destbitsleft -= 8;
    }
    /* Now srcbitsleft and destbitsleft are the same, need not track both */
    /* store whole bytes */
    while (destbitsleft >= 8) {
        *r++ = (bits8)((a >> (destbitsleft - 8)) & BITMASK);
        destbitsleft -= 8;
    }
    /* store last fractional byte */
    if (destbitsleft > 0)
        *r = (bits8)((a << (8 - destbitsleft)) & BITMASK);

    PG_RETURN_VARBIT_P(result);
}

Datum bitfromsmallint(int32 a, int32 typmod)
{
    VarBit* result = NULL;
    bits8* r = NULL;
    int rlen;
    int destbitsleft, srcbitsleft;

    if (typmod <= 0 || typmod > VARBITMAXLEN)
        typmod = 1; /* default bit length */

    if (typmod < M_BIT_LEN && ENABLE_B_CMPT_MODE) {
        return DirectFunctionCall2(bit, bitfromsmallint(a, M_BIT_LEN), Int32GetDatum(typmod));
    }

    rlen = VARBITTOTALLEN(typmod);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = typmod;

    r = VARBITS(result);
    destbitsleft = typmod;
    srcbitsleft = SMALL_SIZE;
    /* drop any input bits that don't fit */
    srcbitsleft = Min(srcbitsleft, destbitsleft);
    /* sign-fill any excess bytes in output */
    while (destbitsleft >= srcbitsleft + 8) {
        *r++ = (bits8)((a < 0) ? BITMASK : 0);
        destbitsleft -= 8;
    }
    /* store first fractional byte */
    if (destbitsleft > srcbitsleft) {
        int val = (int)(a >> (destbitsleft - 8));

        /* Force sign-fill in case the compiler implements >> as zero-fill */
        if (a < 0)
            val |= (-1) << (srcbitsleft + 8 - destbitsleft);
        *r++ = (bits8)(val & BITMASK);
        destbitsleft -= 8;
    }
    /* Now srcbitsleft and destbitsleft are the same, need not track both */
    /* store whole bytes */
    while (destbitsleft >= 8) {
        *r++ = (bits8)((a >> (destbitsleft - 8)) & BITMASK);
        destbitsleft -= 8;
    }
    /* store last fractional byte */
    if (destbitsleft > 0)
        *r = (bits8)((a << (8 - destbitsleft)) & BITMASK);

    PG_RETURN_VARBIT_P(result);
}

Datum bitfromint(int64 a, int32 typmod)
{
    VarBit* result = NULL;
    bits8* r = NULL;
    int rlen;
    int destbitsleft, srcbitsleft;

    if (typmod <= 0 || typmod > VARBITMAXLEN)
        typmod = 1; /* default bit length */

    if (typmod < M_BIT_LEN && ENABLE_B_CMPT_MODE) {
        return DirectFunctionCall2(bit, bitfromint(a, M_BIT_LEN), Int32GetDatum(typmod));
    }

    rlen = VARBITTOTALLEN(typmod);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = typmod;

    r = VARBITS(result);
    destbitsleft = typmod;
    srcbitsleft = 32;
    /* drop any input bits that don't fit */
    srcbitsleft = Min(srcbitsleft, destbitsleft);
    /* sign-fill any excess bytes in output */
    while (destbitsleft >= srcbitsleft + 8) {
        *r++ = (bits8)((a < 0) ? BITMASK : 0);
        destbitsleft -= 8;
    }
    /* store first fractional byte */
    if (destbitsleft > srcbitsleft) {
        int val = (int)(a >> (destbitsleft - 8));

        /* Force sign-fill in case the compiler implements >> as zero-fill */
        if (a < 0)
            val |= (-1) << (srcbitsleft + 8 - destbitsleft);
        *r++ = (bits8)(val & BITMASK);
        destbitsleft -= 8;
    }
    /* Now srcbitsleft and destbitsleft are the same, need not track both */
    /* store whole bytes */
    while (destbitsleft >= 8) {
        *r++ = (bits8)((a >> (destbitsleft - 8)) & BITMASK);
        destbitsleft -= 8;
    }
    /* store last fractional byte */
    if (destbitsleft > 0)
        *r = (bits8)((a << (8 - destbitsleft)) & BITMASK);

    PG_RETURN_VARBIT_P(result);
}

Datum bitfrombigint(int128 a, int32 typmod, bool canIgnore = false)
{
    VarBit* result = NULL;
    bits8* r = NULL;
    int rlen;
    int destbitsleft, srcbitsleft;

    if (typmod <= 0 || typmod > VARBITMAXLEN)
        typmod = 1; /* default bit length */
    
    if (typmod < M_BIT_LEN && ENABLE_B_CMPT_MODE) {
        return DirectFunctionCall2Coll(bit, InvalidOid, bitfrombigint(a, M_BIT_LEN, canIgnore),
                                       Int32GetDatum(typmod), canIgnore);
    }

    rlen = VARBITTOTALLEN(typmod);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = typmod;

    r = VARBITS(result);
    destbitsleft = typmod;
    srcbitsleft = 64;
    /* drop any input bits that don't fit */
    srcbitsleft = Min(srcbitsleft, destbitsleft);
    /* sign-fill any excess bytes in output */
    while (destbitsleft >= srcbitsleft + 8) {
        *r++ = (bits8)((a < 0) ? BITMASK : 0);
        destbitsleft -= 8;
    }
    /* store first fractional byte */
    if (destbitsleft > srcbitsleft) {
        int val = (int)(a >> (destbitsleft - 8));

        /* Force sign-fill in case the compiler implements >> as zero-fill */
        if (a < 0)
            val |= (-1) << (srcbitsleft + 8 - destbitsleft);
        *r++ = (bits8)(val & BITMASK);
        destbitsleft -= 8;
    }
    /* Now srcbitsleft and destbitsleft are the same, need not track both */
    /* store whole bytes */
    while (destbitsleft >= 8) {
        *r++ = (bits8)((a >> (destbitsleft - 8)) & BITMASK);
        destbitsleft -= 8;
    }
    /* store last fractional byte */
    if (destbitsleft > 0)
        *r = (bits8)((a << (8 - destbitsleft)) & BITMASK);

    PG_RETURN_VARBIT_P(result);
}

PG_FUNCTION_INFO_V1_PUBLIC(bitfromint1);
PG_FUNCTION_INFO_V1_PUBLIC(bittoint1);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromint2);
PG_FUNCTION_INFO_V1_PUBLIC(bittoint2);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromuint1);
PG_FUNCTION_INFO_V1_PUBLIC(bittouint1);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromuint2);
PG_FUNCTION_INFO_V1_PUBLIC(bittouint2);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromuint4);
PG_FUNCTION_INFO_V1_PUBLIC(bittouint4);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromuint8);
PG_FUNCTION_INFO_V1_PUBLIC(bittouint8);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromnumeric);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromfloat4);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromfloat8);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromdate);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromdatetime);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromtimestamp);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromtime);
PG_FUNCTION_INFO_V1_PUBLIC(bitfromyear);
extern "C" DLL_PUBLIC Datum bitfromint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bittoint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bittoint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromuint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bittouint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromuint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bittouint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromuint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bittouint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromuint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bittouint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromnumeric(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromfloat4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromfloat8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromdate(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromdatetime(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromtimestamp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromtime(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bitfromyear(PG_FUNCTION_ARGS);

Datum bitfromuint1(PG_FUNCTION_ARGS)
{
    uint8 a = PG_GETARG_UINT8(0);
    int32 typmod = PG_GETARG_INT32(1);
    return bitfromtinyint(a, typmod);
}

Datum bittouint1(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    return bittotinyint(arg, true);
}

Datum bitfromuint2(PG_FUNCTION_ARGS)
{
    uint16 a = PG_GETARG_UINT16(0);
    int32 typmod = PG_GETARG_INT32(1);
    return bitfromsmallint(a, typmod);
}

Datum bittouint2(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    return bittosmallint(arg, true);
}

Datum bitfromuint4(PG_FUNCTION_ARGS)
{
    uint32 a = PG_GETARG_UINT32(0);
    int32 typmod = PG_GETARG_INT32(1);
    return bitfromint(a, typmod);
}

Datum bittouint4(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    return bittoint(arg, true);
}

Datum bitfromuint8(PG_FUNCTION_ARGS)
{
    uint64 a = PG_GETARG_UINT64(0);
    int32 typmod = PG_GETARG_INT32(1);
    return bitfrombigint(a, typmod, fcinfo->can_ignore);
}

Datum bittouint8(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    return bittobigint(arg, true);
}

Datum bitfromint1(PG_FUNCTION_ARGS)
{
    int8 a = PG_GETARG_INT32(0);
    int32 typmod = PG_GETARG_INT32(1);
    return bitfromtinyint(a, typmod);
}

Datum bittoint1(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    return bittotinyint(arg, false);
}

Datum bitfromint2(PG_FUNCTION_ARGS)
{
    int16 a = PG_GETARG_INT32(0);
    int32 typmod = PG_GETARG_INT32(1);
    return bitfromsmallint(a, typmod);
}

Datum bittoint2(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    return bittosmallint(arg, false);
}

Datum bitfromnumeric(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    int32 typmod = PG_GETARG_INT32(1);
    int64 a = (int64)DirectFunctionCall1(numeric_int8, NumericGetDatum(num));
    return bitfrombigint(a, typmod);
}

Datum bitfromfloat4(PG_FUNCTION_ARGS)
{
    float4 num = PG_GETARG_FLOAT4(0);
    int32 typmod = PG_GETARG_INT32(1);
    int64 a = (int64)DirectFunctionCall1(ftoi8, Float4GetDatum(num));
    return bitfrombigint(a, typmod);
}

Datum bitfromfloat8(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    int32 typmod = PG_GETARG_INT32(1);
    int64 a = (int64)DirectFunctionCall1(dtoi8, Float8GetDatum(num));
    return bitfrombigint(a, typmod);
}

Datum bitfromdate(PG_FUNCTION_ARGS)
{
    int64 a = (int64)DirectFunctionCall1(date_int8, PG_GETARG_DATEADT(0));
    int32 typmod = PG_GETARG_INT32(1);
    return bitfrombigint(a, typmod);
}

Datum bitfromdatetime(PG_FUNCTION_ARGS)
{
    Datum a = DirectFunctionCall1(datetime_float, PG_GETARG_TIMESTAMP(0));
    return DirectFunctionCall2(bitfromfloat8, a, PG_GETARG_INT32(1));
}

Datum bitfromtimestamp(PG_FUNCTION_ARGS)
{
    int64 a = (int64)DirectFunctionCall1(timestamptz_int8, PG_GETARG_TIMESTAMPTZ(0));
    int32 typmod = PG_GETARG_INT32(1);
    return bitfrombigint(a, typmod);
}

Datum bitfromtime(PG_FUNCTION_ARGS)
{
    int64 a = (int64)DirectFunctionCall1(time_int8, PG_GETARG_TIMEADT(0));
    int32 typmod = PG_GETARG_INT32(1);
    return bitfrombigint(a, typmod);
}

Datum bitfromyear(PG_FUNCTION_ARGS)
{
    int32 a = (int32)DirectFunctionCall1(year_integer, PG_GETARG_YEARADT(0));
    int32 typmod = PG_GETARG_INT32(1);
    return bitfromint(a, typmod);
}
#endif

/*
 * This is not defined in any standard. We retain the natural ordering of
 * bits here, as it just seems more intuitive.
 */
Datum bitfromint4(PG_FUNCTION_ARGS)
{
    int32 a = PG_GETARG_INT32(0);
    int32 typmod = PG_GETARG_INT32(1);
#ifdef DOLPHIN
    return bitfromint(a, typmod);
#else
    VarBit* result = NULL;
    bits8* r = NULL;
    int rlen;
    int destbitsleft, srcbitsleft;

    if (typmod <= 0 || typmod > VARBITMAXLEN)
        typmod = 1; /* default bit length */

    rlen = VARBITTOTALLEN(typmod);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = typmod;

    r = VARBITS(result);
    destbitsleft = typmod;
    srcbitsleft = 32;
    /* drop any input bits that don't fit */
    srcbitsleft = Min(srcbitsleft, destbitsleft);
    /* sign-fill any excess bytes in output */
    while (destbitsleft >= srcbitsleft + 8) {
        *r++ = (bits8)((a < 0) ? BITMASK : 0);
        destbitsleft -= 8;
    }
    /* store first fractional byte */
    if (destbitsleft > srcbitsleft) {
        int val = (int)(a >> (destbitsleft - 8));

        /* Force sign-fill in case the compiler implements >> as zero-fill */
        if (a < 0)
            val |= (-1) << (srcbitsleft + 8 - destbitsleft);
        *r++ = (bits8)(val & BITMASK);
        destbitsleft -= 8;
    }
    /* Now srcbitsleft and destbitsleft are the same, need not track both */
    /* store whole bytes */
    while (destbitsleft >= 8) {
        *r++ = (bits8)((a >> (destbitsleft - 8)) & BITMASK);
        destbitsleft -= 8;
    }
    /* store last fractional byte */
    if (destbitsleft > 0)
        *r = (bits8)((a << (8 - destbitsleft)) & BITMASK);

    PG_RETURN_VARBIT_P(result);
#endif
}

Datum bittoint4(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
#ifdef DOLPHIN
    return bittoint(arg, false);
#else
    uint32 result;
    bits8* r = NULL;

    /* Check that the bit string is not too long */
    if ((uint32)VARBITLEN(arg) > sizeof(result) * BITS_PER_BYTE)
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("integer out of range")));

    result = 0;
    for (r = VARBITS(arg); r < VARBITEND(arg); r++) {
        result <<= BITS_PER_BYTE;
        result |= *r;
    }
    /* Now shift the result to take account of the padding at the end */
    result >>= VARBITPAD(arg);

    PG_RETURN_INT32(result);
#endif
}

Datum bitfromint8(PG_FUNCTION_ARGS)
{
    int64 a = PG_GETARG_INT64(0);
    int32 typmod = PG_GETARG_INT32(1);
#ifdef DOLPHIN
    return bitfrombigint(a, typmod);
#else
    VarBit* result = NULL;
    bits8* r = NULL;
    int rlen;
    int destbitsleft, srcbitsleft;

    if (typmod <= 0 || typmod > VARBITMAXLEN)
        typmod = 1; /* default bit length */

    rlen = VARBITTOTALLEN(typmod);
    result = (VarBit*)palloc(rlen);
    SET_VARSIZE(result, rlen);
    VARBITLEN(result) = typmod;

    r = VARBITS(result);
    destbitsleft = typmod;
    srcbitsleft = 64;
    /* drop any input bits that don't fit */
    srcbitsleft = Min(srcbitsleft, destbitsleft);
    /* sign-fill any excess bytes in output */
    while (destbitsleft >= srcbitsleft + 8) {
        *r++ = (bits8)((a < 0) ? BITMASK : 0);
        destbitsleft -= 8;
    }
    /* store first fractional byte */
    if (destbitsleft > srcbitsleft) {
        int val = (int)(a >> (destbitsleft - 8));

        /* Force sign-fill in case the compiler implements >> as zero-fill */
        if (a < 0)
            val |= (-1) << (srcbitsleft + 8 - destbitsleft);
        *r++ = (bits8)(val & BITMASK);
        destbitsleft -= 8;
    }
    /* Now srcbitsleft and destbitsleft are the same, need not track both */
    /* store whole bytes */
    while (destbitsleft >= 8) {
        *r++ = (bits8)((a >> (destbitsleft - 8)) & BITMASK);
        destbitsleft -= 8;
    }
    /* store last fractional byte */
    if (destbitsleft > 0)
        *r = (bits8)((a << (8 - destbitsleft)) & BITMASK);

    PG_RETURN_VARBIT_P(result);
#endif
}

Datum bittoint8(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
#ifdef DOLPHIN
    return bittobigint(arg, false);
#else
    uint64 result;
    bits8* r = NULL;

    /* Check that the bit string is not too long */
    if ((uint32)VARBITLEN(arg) > sizeof(result) * BITS_PER_BYTE)
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint out of range")));

    result = 0;
    for (r = VARBITS(arg); r < VARBITEND(arg); r++) {
        result <<= BITS_PER_BYTE;
        result |= *r;
    }
    /* Now shift the result to take account of the padding at the end */
    result >>= VARBITPAD(arg);

    PG_RETURN_INT64(result);
#endif
}

/*
 * Determines the position of S2 in the bitstring S1 (1-based string).
 * If S2 does not appear in S1 this function returns 0.
 * If S2 is of length 0 this function returns 1.
 * Compatible in usage with POSITION() functions for other data types.
 */
Datum bitposition(PG_FUNCTION_ARGS)
{
#ifdef DOLPHIN
    return dolphin_bitposition(fcinfo);
#else
    VarBit* str = PG_GETARG_VARBIT_P(0);
    VarBit* substr = PG_GETARG_VARBIT_P(1);
    int substr_length, str_length, i, is;
    bits8 *s = NULL, /* pointer into substring */
        *p = NULL;   /* pointer into str */
    bits8 cmp,       /* shifted substring byte to compare */
        mask1,       /* mask for substring byte shifted right */
        mask2,       /* mask for substring byte shifted left */
        end_mask,    /* pad mask for last substring byte */
        str_mask;    /* pad mask for last string byte */
    bool is_match = false;

    /* Get the substring length */
    substr_length = VARBITLEN(substr);
    str_length = VARBITLEN(str);

    /* String has zero length or substring longer than string, return 0 */
    if ((str_length == 0) || (substr_length > str_length))
        PG_RETURN_INT32(0);

    /* zero-length substring means return 1 */
    if (substr_length == 0)
        PG_RETURN_INT32(1);

    /* Initialise the padding masks */
    end_mask = BITMASK << VARBITPAD(substr);
    str_mask = BITMASK << VARBITPAD(str);
    for (i = 0; (unsigned int)(i) < VARBITBYTES(str) - VARBITBYTES(substr) + 1; i++) {
        for (is = 0; is < BITS_PER_BYTE; is++) {
            is_match = true;
            p = VARBITS(str) + i;
            mask1 = BITMASK >> is;
            mask2 = ~mask1;
            for (s = VARBITS(substr); is_match && s < VARBITEND(substr); s++) {
                cmp = *s >> is;
                if (s == VARBITEND(substr) - 1) {
                    mask1 &= end_mask >> is;
                    if (p == VARBITEND(str) - 1) {
                        /* Check that there is enough of str left */
                        if (mask1 & ~str_mask) {
                            is_match = false;
                            break;
                        }
                        mask1 &= str_mask;
                    }
                }
                is_match = ((cmp ^ *p) & mask1) == 0;
                if (!is_match)
                    break;
                /* Move on to the next byte */
                p++;
                if (p == VARBITEND(str)) {
                    mask2 = end_mask << (BITS_PER_BYTE - is);
                    is_match = mask2 == 0;
                    break;
                }
                cmp = *s << (BITS_PER_BYTE - is);
                if (s == VARBITEND(substr) - 1) {
                    mask2 &= end_mask << (BITS_PER_BYTE - is);
                    if (p == VARBITEND(str) - 1) {
                        if (mask2 & ~str_mask) {
                            is_match = false;
                            break;
                        }
                        mask2 &= str_mask;
                    }
                }
                is_match = ((cmp ^ *p) & mask2) == 0;
            }
            /* Have we found a match? */
            if (is_match)
                PG_RETURN_INT32(i * BITS_PER_BYTE + is + 1);
        }
    }
    PG_RETURN_INT32(0);
#endif
}

/*
 * bitsetbit
 *
 * Given an instance of type 'bit' creates a new one with
 * the Nth bit set to the given value.
 *
 * The bit location is specified left-to-right in a zero-based fashion
 * consistent with the other get_bit and set_bit functions, but
 * inconsistent with the standard substring, position, overlay functions
 */
Datum bitsetbit(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    int32 n = PG_GETARG_INT32(1);
    int32 newBit = PG_GETARG_INT32(2);
    VarBit* result = NULL;
    int len, bitlen;
    bits8 *r = NULL, *p = NULL;
    int byteNo, bitNo;
    errno_t ss_rc = 0;

    bitlen = VARBITLEN(arg1);
    if (n < 0 || n >= bitlen)
        ereport(ERROR,
            (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("bit index %d out of valid range (0..%d)", n, bitlen - 1)));

    /*
     * sanity check!
     */
    if (newBit != 0 && newBit != 1)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("new bit must be 0 or 1")));

    len = VARSIZE(arg1);
    result = (VarBit*)palloc(len);
    SET_VARSIZE(result, len);
    VARBITLEN(result) = bitlen;

    p = VARBITS(arg1);
    r = VARBITS(result);

    if (VARBITBYTES(arg1) > 0) {
        ss_rc = memcpy_s(r, VARBITBYTES(arg1), p, VARBITBYTES(arg1));
        securec_check(ss_rc, "\0", "\0");
    }

    byteNo = n / BITS_PER_BYTE;
    bitNo = BITS_PER_BYTE - 1 - (n % BITS_PER_BYTE);

    /*
     * Update the byte.
     */
    if (newBit == 0)
        r[byteNo] &= (~(1 << bitNo));
    else
        r[byteNo] |= (1 << bitNo);

    PG_RETURN_VARBIT_P(result);
}

/*
 * bitgetbit
 *
 * returns the value of the Nth bit of a bit array (0 or 1).
 *
 * The bit location is specified left-to-right in a zero-based fashion
 * consistent with the other get_bit and set_bit functions, but
 * inconsistent with the standard substring, position, overlay functions
 */
Datum bitgetbit(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    int32 n = PG_GETARG_INT32(1);
    int bitlen;
    bits8* p = NULL;
    int byteNo, bitNo;

    bitlen = VARBITLEN(arg1);
    if (n < 0 || n >= bitlen)
        ereport(ERROR,
            (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("bit index %d out of valid range (0..%d)", n, bitlen - 1)));

    p = VARBITS(arg1);

    byteNo = n / BITS_PER_BYTE;
    bitNo = BITS_PER_BYTE - 1 - (n % BITS_PER_BYTE);

    if (p[byteNo] & (1 << bitNo))
        PG_RETURN_INT32(1);
    else
        PG_RETURN_INT32(0);
}

Datum mp_bit_length_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);

    PG_RETURN_INT32(VARBITBYTES(arg) * 8);
}

Datum mp_bit_length_text(PG_FUNCTION_ARGS)
{
    Datum str = PG_GETARG_DATUM(0);

    /* We need not detoast the input at all */
    PG_RETURN_INT32((toast_raw_datum_size(str) - VARHDRSZ) * 8);

}

Datum mp_bit_length_bytea(PG_FUNCTION_ARGS)
{
    Datum str = PG_GETARG_DATUM(0);

    /* We need not detoast the input at all */
    PG_RETURN_INT32((toast_raw_datum_size(str) - VARHDRSZ) * 8);
}

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(bit_bool);
extern "C" DLL_PUBLIC Datum bit_bool(PG_FUNCTION_ARGS);

Datum bit_bool(PG_FUNCTION_ARGS)
{
    VarBit* arg1 = PG_GETARG_VARBIT_P(0);
    bits8* p = NULL;
    int bitlen, i;
    bits8 t = 0;

    bitlen = VARBITLEN(arg1);
    p = VARBITS(arg1);

    for(i=0; i < bitlen; i++) {
        if(p[i/BITS_PER_BYTE] == t)
            continue;
        else
            PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1_PUBLIC(bitlike);
extern "C" DLL_PUBLIC Datum bitlike(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(bitnlike);
extern "C" DLL_PUBLIC Datum bitnlike(PG_FUNCTION_ARGS);

Datum bitlike(PG_FUNCTION_ARGS)
{
    bool result = false;
    result = DatumGetBool(biteq(fcinfo));
    PG_RETURN_BOOL(result);
}

Datum bitnlike(PG_FUNCTION_ARGS)
{
    bool result = false;
    result = DatumGetBool(bitne(fcinfo));
    PG_RETURN_BOOL(result);
}

Datum bittochar(VarBit* arg)
{
    int lzero1 = VARBITLEN(arg) % BYTE_SIZE;
    if (GetLeadingZeroLen(arg) < lzero1) {
        lzero1 = lzero1 - BYTE_SIZE;
    }
    int lbytes = (GetLeadingZeroLen(arg) - lzero1) / BYTE_SIZE;
    int lzero2 = lbytes * BYTE_SIZE;
    int len = (VARBITLEN(arg) - lzero1 - lzero2) / BYTE_SIZE;
    errno_t ss_rc = 0;

    Datum cpy = DirectFunctionCall3(bitshiftleft, VarBitPGetDatum(arg),
                                    Int32GetDatum(lzero1 + lzero2), BoolGetDatum(true));
    char* data = (char*)palloc(len + 1);
    if (len != 0) {
        ss_rc = memcpy_s(data, len, VARBITS((VarBit*)cpy), len);
        securec_check(ss_rc, "\0", "\0");
    }
    *(data + len) = '\0';
    PG_RETURN_DATUM((Datum)data);
}

PG_FUNCTION_INFO_V1_PUBLIC(bittotext);
extern "C" DLL_PUBLIC Datum bittotext(PG_FUNCTION_ARGS);
Datum bittotext(PG_FUNCTION_ARGS)
{
    Datum data = bittochar(PG_GETARG_VARBIT_P(0));
    PG_RETURN_DATUM(DirectFunctionCall1(textin, data));
}

PG_FUNCTION_INFO_V1_PUBLIC(bittobpchar);
extern "C" DLL_PUBLIC Datum bittobpchar(PG_FUNCTION_ARGS);
Datum bittobpchar(PG_FUNCTION_ARGS)
{
    Datum data = bittochar(PG_GETARG_VARBIT_P(0));
    int32 typmod = PG_GETARG_INT32(1);
    PG_RETURN_DATUM(DirectFunctionCall3(bpcharin, data, ObjectIdGetDatum(0), Int32GetDatum(typmod)));
}

PG_FUNCTION_INFO_V1_PUBLIC(bittovarchar);
extern "C" DLL_PUBLIC Datum bittovarchar(PG_FUNCTION_ARGS);
Datum bittovarchar(PG_FUNCTION_ARGS)
{
    Datum data = bittochar(PG_GETARG_VARBIT_P(0));
    int32 typmod = PG_GETARG_INT32(1);
    PG_RETURN_DATUM(DirectFunctionCall3(varcharin, data, ObjectIdGetDatum(0), Int32GetDatum(typmod)));
}

PG_FUNCTION_INFO_V1_PUBLIC(bittobinary);
extern "C" DLL_PUBLIC Datum bittobinary(PG_FUNCTION_ARGS);
Datum bittobinary(PG_FUNCTION_ARGS)
{
    Datum dec = bittobigint(PG_GETARG_VARBIT_P(0), true);
    Datum str = DirectFunctionCall1(uint8out, dec);
    PG_RETURN_DATUM(DirectFunctionCall1(dolphin_binaryin, str));
}

PG_FUNCTION_INFO_V1_PUBLIC(bittovarbinary);
extern "C" DLL_PUBLIC Datum bittovarbinary(PG_FUNCTION_ARGS);
Datum bittovarbinary(PG_FUNCTION_ARGS)
{
    Datum dec = bittobigint(PG_GETARG_VARBIT_P(0), true);
    Datum str = DirectFunctionCall1(uint8out, dec);
    PG_RETURN_DATUM(DirectFunctionCall1(byteain, str));
}

Datum bit_bin_in(PG_FUNCTION_ARGS)
{
    char* input_string = PG_GETARG_CSTRING(0);
#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    int32 atttypmod = PG_GETARG_INT32(2);
    bool can_ignore = fcinfo->can_ignore;
    
    PG_RETURN_VARBIT_P(bit_in_internal(input_string, atttypmod, can_ignore));
}

PG_FUNCTION_INFO_V1_PUBLIC(bool_bit);
extern "C" DLL_PUBLIC Datum bool_bit(PG_FUNCTION_ARGS);
Datum bool_bit(PG_FUNCTION_ARGS)
{
    int32 atttypmod = PG_GETARG_INT32(1);
    if (PG_GETARG_BOOL(0) == false) {
        return DirectFunctionCall2(bitfromint8, Int64GetDatum(0), Int32GetDatum(atttypmod));
    } else {
        return DirectFunctionCall2(bitfromint8, Int64GetDatum(1), Int32GetDatum(atttypmod));
    }
}

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_bitnot);
extern "C" DLL_PUBLIC Datum dolphin_bitnot(PG_FUNCTION_ARGS);
Datum dolphin_bitnot(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    PG_RETURN_UINT64(~bittobigint(arg, true));
}

PG_FUNCTION_INFO_V1_PUBLIC(mp_bit_length_binary);
extern "C" DLL_PUBLIC Datum mp_bit_length_binary(PG_FUNCTION_ARGS);
Datum mp_bit_length_binary(PG_FUNCTION_ARGS)
{
    Datum input = PG_GETARG_DATUM(0);
    PG_RETURN_INT32((toast_raw_datum_size(input) - VARHDRSZ) * BITS_PER_BYTE);
}

static void cp_and_pad_left_zero(char *origin, int originLen, char *target, int targetLen)
{
    char *op = origin, *tp =target;
    int offset = targetLen - originLen;

    int ret = memset_s(tp, targetLen, '0', offset);
    securec_check_c(ret, "\0", "\0");
    tp += offset;
    for (int i = 0; i < originLen; i++) {
        *tp++ = *op++;
    }
}

static void binary_str_to_char(char *str, char *result, int len, bool is_escape_zero)
{
    char *rp = result, *sp = str;
    int zero = '0', s = 0, i, k;

    for (i = 0; i < len; i++) {
        s = 0;
        for (k = BITS_PER_BYTE; k > 0; k--) {
            int n = (int)(*sp++) - zero;
            s = n == 0 ? s : s + (n << (k - 1));
        }
        if (is_escape_zero && s == 0) {
            *rp++ = '\\';
            *rp++ = '0';
            *rp++ = '0';
            *rp++ = '0';
        } else {
            *rp++ = (char) s;
        }
    }
}

/**
 * cast bit value to charater.
 * - if the length of bit value is not a multiple of 8, pad left 0.
 * - do not ignore '\0'
 */
char* bit_to_str(VarBit *bits, bool is_escape_zero)
{
    char *result = NULL, *bitStr = NULL, *bitChars = NULL, *r = NULL;
    bits8* sp = NULL;
    bits8 x;
    int i, k, len = VARBITLEN(bits), byteLen = VARBITBYTES(bits);
    int zero_cnt = 0;

    bitStr = (char*)palloc0(byteLen * BITS_PER_BYTE + 1);
    bitChars = (char*)palloc0(len + 1);

    sp = VARBITS(bits);
    r = bitChars;
    for (i = 0; i <= len - BITS_PER_BYTE; i += BITS_PER_BYTE, sp++) {
        x = *sp;
        for (k = 0; k < BITS_PER_BYTE; k++) {
            *r++ = IS_HIGHBIT_SET(x) ? '1' : '0';
            x <<= 1;
        }
    }
    if (i < len) {
        x = *sp;
        for (k = i; k < len; k++) {
            *r++ = IS_HIGHBIT_SET(x) ? '1' : '0';
            x <<= 1;
        }
    }
    cp_and_pad_left_zero(bitChars, len, bitStr, byteLen * BITS_PER_BYTE);

    if (is_escape_zero) {
        for (i = 0; i < byteLen; i++) {
            r = bitStr + i * BITS_PER_BYTE;
            if (memcmp(r, ZERO_BITS_PER_BYTE, ZERO_BITS_PER_BYTE_LENGTH) == 0) {
                zero_cnt++;
            }
        }
        result = (char*)palloc0(byteLen + ESCAPE_ZERO_SZIE * zero_cnt + 1);
    } else {
        result = (char*)palloc0(byteLen + 1);
    }

    binary_str_to_char(bitStr, result, byteLen, is_escape_zero);

    pfree(bitChars);
    pfree(bitStr);

    return result;
}

/**
 * cast bit as char and substr.
 * - first pad left 0 if bit length is not multi 8,
 * - then substr like char, and don't ignore '\0'
 * - start index from 1
 * - length can't be negative
 * - when start<0, amend the sartPosition to abs(start) from last char
 */
VarBit* bit_substr_with_byte_align(VarBit *bits, int start, int length, bool length_not_specified)
{
    VarBit *result = NULL;
    char *bitostr = NULL, *sp = NULL;
    int len = length, bytelen = VARBITBYTES(bits);

    if (!length_not_specified && length < 0)
        ereport(ERROR, (errcode(ERRCODE_SUBSTRING_ERROR), errmsg("negative substring length not allowed")));

    if (start < 0) {
        start = bytelen + start;
    }

    if (start < 0 || start > bytelen || start > VARBITMAXLEN) {
        /* Need to return a zero-length bitstring */
        len = VARBITTOTALLEN(0);
        result = (VarBit*)palloc(len);
        SET_VARSIZE(result, len);
        VARBITLEN(result) = 0;
        return result;
    }

    bitostr = bit_to_str(bits);
    sp = bitostr + start - 1;

    if (length_not_specified || start + length > bytelen ||
        start + length < start  // integer overflow, substr to end
        ) {
        len = bytelen - start + 1;
    }

    int resultlen = len + VARHDRSZ + VARBITHDRSZ;
    result = (VarBit*)palloc0(resultlen);
    SET_VARSIZE(result, resultlen);
    VARBITLEN(result) = len * BITS_PER_BYTE;
    int ret = memcpy_s(VARBITS(result), len, sp, len);
    securec_check(ret, "\0", "\0");

    pfree_ext(bitostr);

    return result;
}

Datum cot_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int64 arg1 = bittobigint(arg, false, fcinfo->can_ignore);

    float8 res;
    errno = 0;
    res = tan(arg1);
    if (errno != 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("input is out of range")));
    }
    res = 1.0 / res;
    if (isinf(res)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("value out of range: overflow")));
    }
    PG_RETURN_FLOAT8(res);
}

Datum acos_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int64 arg1 = bittobigint(arg, false, fcinfo->can_ignore);
    if (arg1 < -1 || arg1 > 1) {
        PG_RETURN_NULL();
    }
    float8 res;
    errno = 0;
    res = acos(arg1);
    if (errno != 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("input is out of range")));
    }
    if (isinf(res)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("value out of range: overflow")));
    }
    PG_RETURN_FLOAT8(res);
}

Datum cos_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int64 arg1 = bittobigint(arg, false, fcinfo->can_ignore);

    float8 res;
    errno = 0;
    res = cos(arg1);
    if (errno != 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("input is out of range")));
    }
    if (isinf(res)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("value out of range: overflow")));
    }
    PG_RETURN_FLOAT8(res);
}

Datum asin_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int64 arg1 = bittobigint(arg, false, fcinfo->can_ignore);
    if (arg1 < -1 || arg1 > 1) {
        PG_RETURN_NULL();
    }
    float8 res;
    errno = 0;
    res = asin(arg1);
    if (errno != 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("input is out of range")));
    }
    if (isinf(res)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("value out of range: overflow")));
    }
    PG_RETURN_FLOAT8(res);
}

Datum atan_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);
    int64 arg1 = bittobigint(arg, false, fcinfo->can_ignore);
    float8 res;
    errno = 0;
    res = atan(arg1);
    if (errno != 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("input is out of range")));
    }
    if (isinf(res)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("value out of range: overflow")));
    }
    PG_RETURN_FLOAT8(res);
}

/**
 * search binary text position, ignore '\0' in text.
 * - if string lenght is illegal, return -1
*/
int32 binary_text_position(const char *b, size_t b_length, const char *s, size_t s_length)
{
    int32 result = -1;
    const uint8 *pos, *search, *end, *search_end;

    if (b_length <= 0 || s_length > b_length) return -1;

    pos = (const uint8*)b;
    search = (const uint8*)s;
    end = (const uint8*)b + b_length - s_length + 1;
    search_end = (const uint8*)s + s_length;

skip:
    while (pos != end) {
        if ((*pos++) != (*search)) continue;

        const uint8 *i,*j;

        i = pos;
        j = search + 1;
        while (j != search_end)
            if ((*i++) != (*j++))
                goto skip;

        result = (pos - (const uint8*)b);
        break;
    }

    return result;
}

Datum dolphin_bitposition(PG_FUNCTION_ARGS)
{
    VarBit* bit_str = PG_GETARG_VARBIT_P(0);
    VarBit* bit_substr = PG_GETARG_VARBIT_P(1);

    int result = 0;
    int substr_length, str_length;
    char *str, *subStr;

    /* Get the substring bit byte length */
    substr_length = VARBITBYTES(bit_substr);
    /* Empty string is always found */
    if (substr_length <= 0)
        PG_RETURN_INT32(1);

    str_length = VARBITBYTES(bit_str);

    str = bit_to_str(bit_str);
    subStr = bit_to_str(bit_substr);

    result = binary_text_position(str, str_length, subStr, substr_length);

    pfree_ext(str);
    pfree_ext(subStr);

    if (result < 0) 
        PG_RETURN_INT32(0);
    else
        PG_RETURN_INT32(result);
}

static Datum GetPeakVarBit(PG_FUNCTION_ARGS, bool isLarger)
{
    VarBit* bit1 = (VarBit*)PG_GETARG_DATUM(0);
    VarBit* bit2 = (VarBit*)PG_GETARG_DATUM(1);

    if (PG_ARGISNULL(0)) {
        if (PG_ARGISNULL(1)) {
            PG_RETURN_NULL();
        } else {
            PG_RETURN_VARBIT_P(bit2);
        }
    } else if (PG_ARGISNULL(1)) {
        PG_RETURN_VARBIT_P(bit1);
    }

    int cmpFlag = bit_cmp(bit1, bit2);
    if ((isLarger && cmpFlag < 0) ||
        (!isLarger && cmpFlag > 0)) {
        PG_RETURN_VARBIT_P(bit2);
    } else {
        PG_RETURN_VARBIT_P(bit1);
    }
}

PG_FUNCTION_INFO_V1_PUBLIC(varbit_larger);
extern "C" DLL_PUBLIC Datum varbit_larger(PG_FUNCTION_ARGS);
Datum varbit_larger(PG_FUNCTION_ARGS)
{
    return GetPeakVarBit(fcinfo, true);
}

PG_FUNCTION_INFO_V1_PUBLIC(varbit_smaller);
extern "C" DLL_PUBLIC Datum varbit_smaller(PG_FUNCTION_ARGS);
Datum varbit_smaller(PG_FUNCTION_ARGS)
{
    return GetPeakVarBit(fcinfo, false);
}

#endif
