/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
#include "plugin_utils/vecfunc_plugin.h"
#include "utils/builtins.h"

#ifndef MADLIB
#include "../../gausskernel/runtime/vecexecutor/vecprimitive/varchar.inl"
#endif

template<bool orclcompat,bool withlen, int  eml, bool is_compress>
Datum 
vec_text_substr_plugin(Datum str, int32 start, int32 length, bool *is_null, mblen_converter fun_mblen)
{
    /* start position */
    int32 S  = start;

    /* adjusted start position */
    int32 S1;

    /* adjusted substring length */
    int32 L1;
    text *result = NULL;
    text *slice = NULL;
    int32 i;
    char *p = NULL;
    char *s = NULL;
    /* end position */
    int E;
    int32 slice_size;
    int32 slice_strlen;
    int32 E1;
    int total;
    errno_t rc;

    total = vec_text_len<eml>(str, fun_mblen);

    if(orclcompat) {
        if(withlen) {
            if ((length <= 0) || (start > total) || (start + total < 0)) {
                return null_return(is_null);
            }
        } else {
            if ((start > total) || (start + total < 0)) {
                return null_return(is_null);
            }
        }

        /*
         * amend the start position. when S < 0,
         * amend the sartPosition to abs(start) from last char,
         * when s==0, the start position is set 1.
         */
        if (S < 0) {
            S = total + S + 1;
        } else if (0 == S) {
            return null_return(is_null);
        }
    }

    S1 = Max(S, 1);
    if (!withlen) {
        slice_size = L1 = -1;
        E = total + 1;
    } else {
        E = S + length;

        /*
         * A negative value for L is the only way for the end position to
         * be before the start. SQL99 says to throw an error.
         */
        if (E < S) {
            if (length < 0) {
                ereport(ERROR,
                        (errcode(ERRCODE_SUBSTRING_ERROR),
                         errmsg("negative substring length not allowed")));
            } else {
                ereport(ERROR,
                        (errcode(ERRCODE_SUBSTRING_ERROR),
                        errmsg("the giving length is too long, it lets the end postion integer out of range")));
            }
        }

        /*
         * A zero or negative value for the end position can happen if the
         * start was negative or one. SQL99 says to return a zero-length
         * string.
         */
        if (E < 1) {
            return null_return(is_null);
        }
        L1 = E - S1;
        if(eml > 1)
            slice_size = E * eml;
    }


    E = Min(E,total + 1);

    if(eml == 1) {
        result = DatumGetTextPSlice(str, S1 - 1, L1);

        /* null return for A db format */
        if (result == NULL || 0 == VARSIZE_ANY_EXHDR(result))
            return null_return(is_null);
        else
            PG_RETURN_TEXT_P(result);
    } else {
        /*
         * If we're working with an untoasted source, no need to do an extra
         * copying step.
         */
        if (is_compress) {
            slice = DatumGetTextPSlice(str, 0, slice_size);

            /* Now we can get the actual length of the slice in MB characters */
            slice_strlen = pg_mbstrlen_with_len(VARDATA_ANY(slice), VARSIZE_ANY_EXHDR(slice));

            /*
             * Check that the start position wasn't > slice_strlen. If so, SQL99
             * says to return a zero-length string.
             */
            if (S1 > slice_strlen) {
                if (slice != (text *) DatumGetPointer(str))
                    pfree_ext(slice);
                return null_return(is_null);
            }

            /*
             * Adjust L1 and E1 now that we know the slice string length. Again
             * remember that S1 is one based, and slice_start is zero based.
             */
            if (L1 > -1)
                E1 = Min(S1 + L1, 1 + slice_strlen);
            else
                E1 = 1 + slice_strlen;
        } else {
            slice = (text *) DatumGetPointer(str);
            E1 = E ;
        }
        
        if (S1 >= E1) {
            return null_return(is_null);
        }
        
        p = VARDATA_ANY(slice);
        for (i = 0; i < S1 - 1; i++) {
            p += fun_mblen((const unsigned char *)p);
        }

        /* hang onto a pointer to our start position */
        s = p;

        /*
         * Count the actual bytes used by the substring of the requested
         * length.
         */
        for (i = S1; i < E1; i++) {
            p += fun_mblen((const unsigned char *)p);
        }

        if(0 == (p - s)) {
            return null_return(is_null);
        }
        
        result = (text *) palloc(VARHDRSZ + (p - s));
        SET_VARSIZE(result, VARHDRSZ + (p - s));
        rc = memcpy_s(VARDATA(result), (p - s), s, (p - s));
        securec_check(rc,"\0","\0");
        PG_RETURN_TEXT_P(result);
    }
}

#define InitSubstrFuncTemplatePlugin(eml)                                     \
    do {                                                                \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<false, false, eml, false>; \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<false, false, eml, true>;  \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<false, true, eml, false>;  \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<false, true, eml, true>;   \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<true, false, eml, false>;  \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<true, false, eml, true>;   \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<true, true, eml, false>;   \
        substr_Array_Plugin[i++] = &vec_text_substr_plugin<true, true, eml, true>;    \
    } while (0);

sub_Array_Plugin substr_Array_Plugin[32];

void InitVecsubarrayPlugin(void)
{
    int i = 0;

    InitSubstrFuncTemplatePlugin(1);
    InitSubstrFuncTemplatePlugin(2);
    InitSubstrFuncTemplatePlugin(3);
    InitSubstrFuncTemplatePlugin(4);
};
