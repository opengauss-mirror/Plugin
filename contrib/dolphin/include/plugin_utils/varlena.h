#ifndef VARLENA_H
#define VARLENA_H

#include "plugin_postgres.h"
#include "fmgr.h"

#ifdef DOLPHIN
extern Datum bit_blob(VarBit* input);
double float8in_internal(char* str, char** endptr_p, bool* hasError, CoercionContext ccontext);
char* AnyElementGetCString(Oid anyOid, Datum anyDatum, bool* hasError = nullptr);
#endif

#endif