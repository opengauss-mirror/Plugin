/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * Plugin is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#ifndef VARLENA_H
#define VARLENA_H

#include "plugin_postgres.h"
#include "fmgr.h"
#include "plugin_utils/varbit.h"
#include "plugin_parser/parser.h"

#ifdef DOLPHIN
extern Datum bit_blob(VarBit* input);
double float8in_internal(char* str, char** endptr_p, bool* hasError, CoercionContext ccontext);
char* AnyElementGetCString(Oid anyOid, Datum anyDatum, bool* hasError = nullptr, bool* typIsVarlena = nullptr);
extern void trim_trailing_space(char* str);
extern VarBit* bit_substr_with_byte_align(VarBit *bits, int start, int length, bool length_not_specified);
extern int32 binary_text_position(const char *b, size_t b_length, const char *s, size_t s_length);
bool is_type_with_date(Oid type);
bool is_type_with_time(Oid type);
extern CmpType agg_cmp_type(CmpType a, CmpType b);
extern BpChar* bpchar_input(const char* s, size_t len, int32 atttypmod);
extern VarChar* varchar_input(const char* s, size_t len, int32 atttypmod);
#endif

#endif
