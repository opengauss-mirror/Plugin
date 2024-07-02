#ifndef VARLENA_H
#define VARLENA_H

#include "plugin_postgres.h"
#include "fmgr.h"
#include "plugin_utils/varbit.h"

#ifdef DOLPHIN
extern Datum bit_blob(VarBit* input);
double float8in_internal(char* str, char** endptr_p, bool* hasError, CoercionContext ccontext);
extern char* bit_to_str(VarBit *bits);
char* AnyElementGetCString(Oid anyOid, Datum anyDatum, bool* hasError = nullptr, bool* typIsVarlena = nullptr);
extern void trim_trailing_space(char* str);
extern VarBit* bit_substr_with_byte_align(VarBit *bits, int start, int length, bool length_not_specified);
extern bool is_req_from_jdbc();
extern int32 binary_text_position(const char *b, size_t b_length, const char *s, size_t s_length);
bool is_type_with_date(Oid type);
bool is_type_with_time(Oid type);
#endif

#endif
