//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		assert.h
//
//	@doc:
//		Macros for assertions in spqos
//
//		Use '&& "explanation"' in assert condition to provide additional
//		information about the failed condition.
//
//			SPQOS_ASSERT(!FSpinlockHeld() && "Must not hold spinlock during allcoation");
//
//		There is no SPQOS_ASSERT_STOP macro, instead use
//
//			SPQOS_ASSERT(!"Should not get here because of xyz");
//
//		Avoid using SPQOS_ASSERT(false);
//---------------------------------------------------------------------------
#ifndef SPQOS_assert_H
#define SPQOS_assert_H

#ifndef USE_CMAKE
#include "pg_config.h"
#endif


// retail assert; available in all builds
#define SPQOS_RTL_ASSERT(x)                                                 \
	((x) ? ((void) 0)                                                      \
		 : spqos::CException::Raise(__FILE__, __LINE__,                     \
								   spqos::CException::ExmaSystem,           \
								   spqos::CException::ExmiAssert, __FILE__, \
								   __LINE__, SPQOS_WSZ_LIT(#x)))

#ifdef SPQOS_DEBUG
// standard debug assert; maps to retail assert in debug builds only
#define SPQOS_ASSERT(x) SPQOS_RTL_ASSERT(x)
#else
#define SPQOS_ASSERT(x) ;
#endif	// !SPQOS_DEBUG

// implication assert
#define SPQOS_ASSERT_IMP(x, y) SPQOS_ASSERT(!(x) || (y))

// if-and-only-if assert
#define SPQOS_ASSERT_IFF(x, y) SPQOS_ASSERT((!(x) || (y)) && (!(y) || (x)))

// compile assert
#define SPQOS_CPL_ASSERT(x) extern int assert_array[(x) ? 1 : -1]

// debug assert, with message
#define SPQOS_ASSERT_MSG(x, msg) SPQOS_ASSERT((x) && (msg))

// retail assert, with message
#define SPQOS_RTL_ASSERT_MSG(x, msg) SPQOS_RTL_ASSERT((x) && (msg))


#endif	// !SPQOS_assert_H

// EOF
