//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		exception.h
//
//	@doc:
//		Definition of SPQOPT-specific exception types
//---------------------------------------------------------------------------
#ifndef SPQOPT_exception_H
#define SPQOPT_exception_H

#include "spqos/memory/CMemoryPool.h"
#include "spqos/types.h"

namespace spqopt
{
// major exception types - reserve range 1000-2000
enum ExMajor
{
	ExmaSPQOPT = 1000,

	ExmaSentinel
};

// minor exception types
enum ExMinor
{
	ExmiNoPlanFound,
	ExmiInvalidPlanAlternative,
	ExmiUnsupportedOp,
	ExmiUnexpectedOp,
	ExmiUnsupportedPred,
	ExmiUnsupportedCompositePartKey,
	ExmiUnsupportedNonDeterministicUpdate,
	ExmiUnsatisfiedRequiredProperties,
	ExmiEvalUnsupportedScalarExpr,
	ExmiCTEProducerConsumerMisAligned,
	ExmiNoStats,

	ExmiSentinel
};

// message initialization for SPQOS exceptions
spqos::SPQOS_RESULT EresExceptionInit(spqos::CMemoryPool *mp);

}  // namespace spqopt

#endif	// !SPQOPT_exception_H


// EOF
