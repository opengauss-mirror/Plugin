#ifndef VARLENA_H
#define VARLENA_H

#include "plugin_postgres.h"
#include "fmgr.h"
#include "plugin_utils/varbit.h"

#ifdef DOLPHIN
extern Datum bit_blob(VarBit* input);
double float8in_internal(char* str, char** endptr_p, bool* hasError, CoercionContext ccontext);
extern char* bit_to_str(VarBit *bits);
extern void trim_trailing_space(char* str);
extern VarBit* bit_substr_with_byte_align(VarBit *bits, int start, int length, bool length_not_specified);
#endif

#endif
