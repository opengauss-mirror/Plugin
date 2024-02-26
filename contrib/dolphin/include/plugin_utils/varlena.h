#ifndef VARLENA_H
#define VARLENA_H

#include "plugin_postgres.h"
#include "fmgr.h"

#ifdef DOLPHIN
extern Datum bit_blob(VarBit* input);
double float8in_internal(char* str, char** endptr_p, bool* hasError, CoercionContext ccontext);
extern Datum bit_to_str(VarBit *bits);
char* AnyElementGetCString(Oid anyOid, Datum anyDatum, bool* hasError = nullptr, bool* typIsVarlena = nullptr);
#endif

#endif
