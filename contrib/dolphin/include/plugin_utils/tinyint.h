/* -------------------------------------------------------------------------
 *
 * tinyint.h
 *	  Definitions for the SQL92 "tinyint" types.
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * include/plugin_utils/tinyint.h
 *
 * -------------------------------------------------------------------------
*/
#ifndef PLUGIN_TINYINT_H
#define PLUGIN_TINYINT_H
#include "plugin_postgres.h"

PG_FUNCTION_INFO_V1_PUBLIC(int12cmp);
PG_FUNCTION_INFO_V1_PUBLIC(int14cmp);
PG_FUNCTION_INFO_V1_PUBLIC(int18cmp);
PG_FUNCTION_INFO_V1_PUBLIC(int12eq);
PG_FUNCTION_INFO_V1_PUBLIC(int12lt);
PG_FUNCTION_INFO_V1_PUBLIC(int12le);
PG_FUNCTION_INFO_V1_PUBLIC(int12gt);
PG_FUNCTION_INFO_V1_PUBLIC(int12ge);
PG_FUNCTION_INFO_V1_PUBLIC(int14eq);
PG_FUNCTION_INFO_V1_PUBLIC(int14lt);
PG_FUNCTION_INFO_V1_PUBLIC(int14le);
PG_FUNCTION_INFO_V1_PUBLIC(int14gt);
PG_FUNCTION_INFO_V1_PUBLIC(int14ge);
PG_FUNCTION_INFO_V1_PUBLIC(int18eq);
PG_FUNCTION_INFO_V1_PUBLIC(int18lt);
PG_FUNCTION_INFO_V1_PUBLIC(int18le);
PG_FUNCTION_INFO_V1_PUBLIC(int18gt);
PG_FUNCTION_INFO_V1_PUBLIC(int18ge);
extern "C" DLL_PUBLIC Datum int12cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int14cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int18cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int12eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int12lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int12le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int12gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int12ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int14eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int14lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int14le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int14gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int14ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int18eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int18lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int18le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int18gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int18ge(PG_FUNCTION_ARGS);
#endif