/*-------------------------------------------------------------------------
 *
 * tuplestore.h
 *	  Utilities regarding calls to PG functions
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#ifndef SPQ_TUPLESTORE_H
#define SPQ_TUPLESTORE_H
#include "funcapi.h"

extern Tuplestorestate* SetupTuplestore(FunctionCallInfo fcinfo, TupleDesc* tupdesc);
#endif
