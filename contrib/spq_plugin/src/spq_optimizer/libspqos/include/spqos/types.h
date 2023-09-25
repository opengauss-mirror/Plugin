//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		types.h
//
//	@doc:
//		Type definitions for spqos to avoid using native types directly;
//
//		TODO: 03/15/2008; the seletion of basic types which then
//		get mapped to internal types should be done by autoconf or other
//		external tools; for the time being these are hard-coded; cpl asserts
//		are used to make sure things are failing if compiled with a different
//		compiler.
//---------------------------------------------------------------------------
#ifndef SPQOS_types_H
#define SPQOS_types_H

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <iostream>

#include "spqos/assert.h"

#define SPQOS_SIZEOF(x) ((spqos::ULONG) sizeof(x))
#define SPQOS_ARRAY_SIZE(x) (SPQOS_SIZEOF(x) / SPQOS_SIZEOF(x[0]))
#define SPQOS_OFFSET(T, M) ((spqos::ULONG)(SIZE_T) & (((T *) 0x1)->M) - 1)

/* wide character string literate */
#define SPQOS_WSZ_LIT(x) L##x

// failpoint simulation is enabled on debug build
#ifdef SPQOS_DEBUG
#define SPQOS_FPSIMULATOR 1
#endif	// SPQOS_DEBUG

namespace spqos
{
// Basic types to be used instead of built-ins
// Add types as needed;

typedef unsigned char BYTE;
typedef char CHAR;
// ignore signed char for the moment

// wide character type
typedef wchar_t WCHAR;

typedef bool BOOL;

// numeric types

typedef size_t SIZE_T;
typedef ssize_t SSIZE_T;
typedef mode_t MODE_T;

// define ULONG,ULLONG as types which implement standard's
// requirements for ULONG_MAX and ULLONG_MAX; eliminate standard's slack
// by fixed sizes rather than min requirements

typedef uint32_t ULONG;
SPQOS_CPL_ASSERT(4 == sizeof(ULONG));
enum
{
	ulong_max = ((::spqos::ULONG) -1)
};

typedef uint64_t ULLONG;
SPQOS_CPL_ASSERT(8 == sizeof(ULLONG));
enum
{
	ullong_max = ((::spqos::ULLONG) -1)
};

typedef uintptr_t ULONG_PTR;
#define ULONG_PTR_MAX (spqos::ullong_max)

typedef uint16_t USINT;
typedef int16_t SINT;
typedef int32_t INT;
typedef int64_t LINT;
typedef intptr_t INT_PTR;

SPQOS_CPL_ASSERT(2 == sizeof(USINT));
SPQOS_CPL_ASSERT(2 == sizeof(SINT));
SPQOS_CPL_ASSERT(4 == sizeof(INT));
SPQOS_CPL_ASSERT(8 == sizeof(LINT));

enum
{
	int_max = ((::spqos::INT)(spqos::ulong_max >> 1)),
	int_min = (-spqos::int_max - 1)
};

enum
{
	lint_max = ((::spqos::LINT)(spqos::ullong_max >> 1)),
	lint_min = (-spqos::lint_max - 1)
};

enum
{
	usint_max = ((::spqos::USINT) -1)
};

enum
{
	sint_max = ((::spqos::SINT)(spqos::usint_max >> 1)),
	sint_min = (-spqos::sint_max - 1)
};

typedef double DOUBLE;

// holds for all platforms
SPQOS_CPL_ASSERT(sizeof(ULONG_PTR) == sizeof(void *));

// variadic parameter list type
using VA_LIST = va_list;

// enum for results on OS level (instead of using a global error variable)
enum SPQOS_RESULT
{
	SPQOS_OK = 0,

	SPQOS_FAILED = 1,
	SPQOS_OOM = 2,
	SPQOS_NOT_FOUND = 3,
	SPQOS_TIMEOUT = 4
};


// enum for locale encoding
enum ELocale
{
	ELocInvalid,  // invalid key for hashtable iteration
	ElocEnUS_Utf8,
	ElocGeDE_Utf8,

	ElocSentinel
};
}  // namespace spqos

#endif	// !SPQOS_types_H

// EOF
