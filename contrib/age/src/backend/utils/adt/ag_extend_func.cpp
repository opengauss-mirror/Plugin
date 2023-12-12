/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef AG_EXTEND_FUNC_H
#define AG_EXTEND_FUNC_H

#include "utils/ag_extend_func.h"
#include "postgres.h"
#include "fmgr.h"
#include "utils/numeric.h"
#define NUMERIC_FLAGBITS(n) ((n)->choice.n_header & NUMERIC_SIGN_MASK)
#define NUMERIC_IS_NAN(n)		(NUMERIC_FLAGBITS(n) == NUMERIC_NAN)

#define NUMERIC_SHORT_WEIGHT_SIGN_MASK	0x0040
#define NUMERIC_SHORT_WEIGHT_MASK		0x003F

#define NUMERIC_HEADER_IS_SHORT(n)	(((n)->choice.n_header & 0x8000) != 0)
#define NUMERIC_HEADER_SIZE(n) \
    (VARHDRSZ + sizeof(uint16) + \
    (NUMERIC_HEADER_IS_SHORT(n) ? 0 : sizeof(int16)))

#define NUMERIC_WEIGHT(n)	(NUMERIC_HEADER_IS_SHORT((n)) ? \
	(((n)->choice.n_short.n_header & NUMERIC_SHORT_WEIGHT_SIGN_MASK ? \
		~NUMERIC_SHORT_WEIGHT_MASK : 0) \
	 | ((n)->choice.n_short.n_header & NUMERIC_SHORT_WEIGHT_MASK)) \
	: ((n)->choice.n_long.n_weight))

#define NUMERIC_DIGITS(num) (NUMERIC_HEADER_IS_SHORT(num) ? \
	(num)->choice.n_short.n_data : (num)->choice.n_long.n_data)

#define NUMERIC_NDIGITS(num) \
    ((VARSIZE(num) - NUMERIC_HEADER_SIZE(num)) / sizeof(NumericDigit))

#define UINT32_ALIGN_MASK (sizeof(uint32) - 1)
#define mix(a, b, c) \
{ \
    a -= c;  a ^= rot(c, 4);	c += b; \
    b -= a;  b ^= rot(a, 6);	a += c; \
    c -= b;  c ^= rot(b, 8);	b += a; \
    a -= c;  a ^= rot(c, 16);	c += b; \
    b -= a;  b ^= rot(a, 19);	a += c; \
    c -= b;  c ^= rot(b, 4);	b += a; \
}
#define PG_RETURN_UINT64(x)  return UInt64GetDatum(x)
#define final(a, b, c) \
{ \
    c ^= b; c -= rot(b, 14); \
    a ^= c; a -= rot(c, 11); \
    b ^= a; b -= rot(a, 25); \
    c ^= b; c -= rot(b, 16); \
    a ^= c; a -= rot(c, 4); \
    b ^= a; b -= rot(a, 14); \
    c ^= b; c -= rot(b, 24); \
}
#define rot(x, k) (((x)<<(k)) | ((x)>>(32-(k))))

Datum hash_any_extended(register const unsigned char *k, register int keylen, uint64 seed)
{
    register uint32 a;
    register uint32 b;
    register uint32 c;
    register uint32 len;

    /* Set up the internal state */
    len = keylen;
    a = b = c = 0x9e3779b9 + len + 3923095;

    /* If the seed is non-zero, use it to perturb the internal state. */
    if (seed != 0) {
        /*
         * In essence, the seed is treated as part of the data being hashed,
         * but for simplicity, we pretend that it's padded with four bytes of
         * zeroes so that the seed constitutes a 12-byte chunk.
         */
        a += (uint32) (seed >> 32);
        b += (uint32) seed;
        mix(a, b, c);
    }

    /* If the source pointer is word-aligned, we use word-wide fetches */
    if (((uintptr_t) k & UINT32_ALIGN_MASK) == 0) {
    /* Code path for aligned source data */
    register const uint32 *ka = (const uint32 *) k;

    /* handle most of the key */
    while (len >= 12) {
        a += ka[0];
        b += ka[1];
        c += ka[2];
        mix(a, b, c);
        ka += 3;
        len -= 12;
    }

    /* handle the last 11 bytes */
    k = (const unsigned char *) ka;
#ifdef WORDS_BIGENDIAN
    switch (len) {
    case 11:
        c += ((uint32) k[10] << 8);
        /* fall through */
    case 10:
        c += ((uint32) k[9] << 16);
        /* fall through */
    case 9:
        c += ((uint32) k[8] << 24);
        /* fall through */
    case 8:
        /* the lowest byte of c is reserved for the length */
        b += ka[1];
        a += ka[0];
        break;
    case 7:
        b += ((uint32) k[6] << 8);
        /* fall through */
    case 6:
        b += ((uint32) k[5] << 16);
        /* fall through */
    case 5:
        b += ((uint32) k[4] << 24);
        /* fall through */
    case 4:
        a += ka[0];
        break;
    case 3:
        a += ((uint32) k[2] << 8);
        /* fall through */
    case 2:
        a += ((uint32) k[1] << 16);
        /* fall through */
    case 1:
        a += ((uint32) k[0] << 24);
	    /* case 0: nothing left to add */
    }
#else							/* !WORDS_BIGENDIAN */
    switch (len)
    {
    case 11:
        c += ((uint32) k[10] << 24);
        /* fall through */
    case 10:
        c += ((uint32) k[9] << 16);
        /* fall through */
    case 9:
        c += ((uint32) k[8] << 8);
        /* fall through */
    case 8:
        /* the lowest byte of c is reserved for the length */
        b += ka[1];
        a += ka[0];
        break;
    case 7:
        b += ((uint32) k[6] << 16);
        /* fall through */
    case 6:
        b += ((uint32) k[5] << 8);
        /* fall through */
    case 5:
        b += k[4];
        /* fall through */
    case 4:
        a += ka[0];
        break;
    case 3:
        a += ((uint32) k[2] << 16);
        /* fall through */
    case 2:
        a += ((uint32) k[1] << 8);
        /* fall through */
    case 1:
        a += k[0];
        /* case 0: nothing left to add */
    }
#endif							/* WORDS_BIGENDIAN */
    } else {
		/* Code path for non-aligned source data */

		/* handle most of the key */
                while (len >= 12) {
#ifdef WORDS_BIGENDIAN
                    a += (k[3] + ((uint32) k[2] << 8) + ((uint32) k[1] << 16) + ((uint32) k[0] << 24));
                    b += (k[7] + ((uint32) k[6] << 8) + ((uint32) k[5] << 16) + ((uint32) k[4] << 24));
                    c += (k[11] + ((uint32) k[10] << 8) + ((uint32) k[9] << 16) + ((uint32) k[8] << 24));
#else			     			/* !WORDS_BIGENDIAN */
                    a += (k[0] + ((uint32) k[1] << 8) + ((uint32) k[2] << 16) + ((uint32) k[3] << 24));
                    b += (k[4] + ((uint32) k[5] << 8) + ((uint32) k[6] << 16) + ((uint32) k[7] << 24));
                    c += (k[8] + ((uint32) k[9] << 8) + ((uint32) k[10] << 16) + ((uint32) k[11] << 24));
#endif			     			/* WORDS_BIGENDIAN */
                    mix(a, b, c);
                    k += 12;
                    len -= 12;
                }

		/* handle the last 11 bytes */
#ifdef WORDS_BIGENDIAN
                switch (len) {
                    case 11:
                        c += ((uint32) k[10] << 8);
                        /* fall through */
                    case 10:
                        c += ((uint32) k[9] << 16);
                        /* fall through */
                    case 9:
                        c += ((uint32) k[8] << 24);
                        /* fall through */
                    case 8:
                        /* the lowest byte of c is reserved for the length */
                        b += k[7];
                        /* fall through */
                    case 7:
                        b += ((uint32) k[6] << 8);
                        /* fall through */
                    case 6:
                        b += ((uint32) k[5] << 16);
                        /* fall through */
                    case 5:
                        b += ((uint32) k[4] << 24);
                        /* fall through */
                    case 4:
                        a += k[3];
                        /* fall through */
                    case 3:
                        a += ((uint32) k[2] << 8);
                        /* fall through */
                    case 2:
                        a += ((uint32) k[1] << 16);
                        /* fall through */
                    case 1:
                        a += ((uint32) k[0] << 24);
                        /* case 0: nothing left to add */
                }
#else							/* !WORDS_BIGENDIAN */
                switch (len) {
                    case 11:
                        c += ((uint32) k[10] << 24);
                        /* fall through */
                    case 10:
                        c += ((uint32) k[9] << 16);
                        /* fall through */
                    case 9:
                        c += ((uint32) k[8] << 8);
                        /* fall through */
                    case 8:
                        /* the lowest byte of c is reserved for the length */
                        b += ((uint32) k[7] << 24);
                        /* fall through */
                    case 7:
                        b += ((uint32) k[6] << 16);
                        /* fall through */
                    case 6:
                        b += ((uint32) k[5] << 8);
                        /* fall through */
                    case 5:
                        b += k[4];
                        /* fall through */
                    case 4:
                        a += ((uint32) k[3] << 24);
                        /* fall through */
                    case 3:
                        a += ((uint32) k[2] << 16);
                        /* fall through */
                    case 2:
                        a += ((uint32) k[1] << 8);
                        /* fall through */
                    case 1:
                        a += k[0];
                        /* case 0: nothing left to add */
                }
#endif							/* WORDS_BIGENDIAN */
    }

    final(a, b, c);

    /* report the result */
    PG_RETURN_UINT64(((uint64) b << 32) | c);
}

Datum hash_uint32_extended(uint32 k, uint64 seed)
{
    register uint32 a;
    register uint32 b;
    register uint32 c;

    a = b = c = 0x9e3779b9 + (uint32) sizeof(uint32) + 3923095;

    if (seed != 0) {
        a += (uint32) (seed >> 32);
        b += (uint32) seed;
        mix(a, b, c);
    }

    a += k;

    final(a, b, c);

    /* report the result */
    PG_RETURN_UINT64(((uint64) b << 32) | c);
}


Datum hashint8extended(PG_FUNCTION_ARGS)
{
    /* Same approach as hashint8 */
    int64		val = PG_GETARG_INT64(0);
    uint32		lohalf = (uint32) val;
    uint32		hihalf = (uint32) (val >> 32);

    lohalf ^= (val >= 0) ? hihalf : ~hihalf;

    return hash_uint32_extended(lohalf, PG_GETARG_INT64(1));
}
Datum hashfloat8extended(PG_FUNCTION_ARGS)
{
    float8		key = PG_GETARG_FLOAT8(0);
    uint64		seed = PG_GETARG_INT64(1);

    /* Same approach as hashfloat8 */
    if (key == (float8) 0)
        PG_RETURN_UINT64(seed);

    return hash_any_extended((unsigned char *) &key, sizeof(key), seed);
}

Datum hashcharextended(PG_FUNCTION_ARGS)
{
    return hash_uint32_extended((int32) PG_GETARG_CHAR(0), PG_GETARG_INT64(1));
}

Datum hash_numeric_extended(PG_FUNCTION_ARGS)
{
    Numeric		key = PG_GETARG_NUMERIC(0);
    uint64		seed = PG_GETARG_INT64(1);
    Datum		digit_hash;
    Datum		result;
    int			weight;
    int			start_offset;
    int			end_offset;
    int			i;
    int			hash_len;
    NumericDigit *digits;

    if (NUMERIC_IS_NAN(key))
        PG_RETURN_UINT64(seed);

    weight = NUMERIC_WEIGHT(key);
    start_offset = 0;
    end_offset = 0;

    digits = NUMERIC_DIGITS(key);
    for (i = 0; i < NUMERIC_NDIGITS(key); i++) {
        if (digits[i] != (NumericDigit) 0)
            break;

        start_offset++;

        weight--;
    }

    if (NUMERIC_NDIGITS(key) == start_offset)
        PG_RETURN_UINT64(seed - 1);

    for (i = NUMERIC_NDIGITS(key) - 1; i >= 0; i--) {
        if (digits[i] != (NumericDigit) 0) {
            break;
        }
        end_offset++;
    }

    Assert(start_offset + end_offset < NUMERIC_NDIGITS(key));

    hash_len = NUMERIC_NDIGITS(key) - start_offset - end_offset;
    digit_hash = hash_any_extended((unsigned char *) (NUMERIC_DIGITS(key) + start_offset),
                                                      hash_len * sizeof(NumericDigit),
                                                      seed);

    result = UInt64GetDatum(DatumGetUInt64(digit_hash) ^ weight);

    PG_RETURN_DATUM(result);
}

#endif
