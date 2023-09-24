//---------------------------------------------------------------------------
//
// funcs.h
//    API for invoking optimizer using SPQDB udfs
//
// Copyright (c) 2019-Present Pivotal Software, Inc.
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_funcs_H
#define SPQOPT_funcs_H


#include "postgres.h"

#include "fmgr.h"
#include "utils/builtins.h"

extern Datum DisableXform(PG_FUNCTION_ARGS);
extern Datum EnableXform(PG_FUNCTION_ARGS);
extern Datum LibraryVersion();

#endif	// SPQOPT_funcs_H
