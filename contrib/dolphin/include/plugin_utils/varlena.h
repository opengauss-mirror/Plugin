#ifndef VARLENA_H
#define VARLENA_H

#include "plugin_postgres.h"
#include "fmgr.h"

#ifdef DOLPHIN
extern Datum bit_blob(VarBit* input);
#endif

#endif