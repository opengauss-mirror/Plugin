//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraintTest.h
//
//	@doc:
//		Test for constraints
//---------------------------------------------------------------------------
#ifndef SPQOPT_CConstraintTest_H
#define SPQOPT_CConstraintTest_H

#include "spqos/base.h"

#include "spqopt/base/CConstraintConjunction.h"
#include "spqopt/base/CConstraintDisjunction.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CConstraintNegation.h"
#include "spqopt/base/CRange.h"
#include "spqopt/eval/IConstExprEvaluator.h"

#include "unittest/spqopt/CTestUtils.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CConstraintTest
//
//	@doc:
//		Static unit tests for constraints
//
//---------------------------------------------------------------------------
class CConstraintTest
{
	struct SRangeInfo
	{
		CRange::ERangeInclusion eriLeft;   // inclusion for left end
		INT iLeft;						   // left end value
		CRange::ERangeInclusion eriRight;  // inclusion for right end
		INT iRight;						   // right end value
	};

private:
	// number of microseconds in one day
	static const LINT lMicrosecondsPerDay;

	// integer representation for '01-01-2012'
	static const LINT lInternalRepresentationFor2012_01_01;

	// integer representation for '01-21-2012'
	static const LINT lInternalRepresentationFor2012_01_21;

	// integer representation for '01-02-2012'
	static const LINT lInternalRepresentationFor2012_01_02;

	// integer representation for '01-22-2012'
	static const LINT lInternalRepresentationFor2012_01_22;

	// byte representation for '01-01-2012'
	static const WCHAR *wszInternalRepresentationFor2012_01_01;

	// byte representation for '01-21-2012'
	static const WCHAR *wszInternalRepresentationFor2012_01_21;

	// byte representation for '01-02-2012'
	static const WCHAR *wszInternalRepresentationFor2012_01_02;

	// byte representation for '01-22-2012'
	static const WCHAR *wszInternalRepresentationFor2012_01_22;

	// construct an array of ranges to be used to create an interval
	static CRangeArray *Pdrspqrng(CMemoryPool *mp, IMDId *mdid,
								 const SRangeInfo rgRangeInfo[],
								 ULONG ulRanges);

	static CConstraintInterval *PciFirstInterval(CMemoryPool *mp, IMDId *mdid,
												 CColRef *colref);

	static CConstraintInterval *PciSecondInterval(CMemoryPool *mp, IMDId *mdid,
												  CColRef *colref);

	// interval from scalar comparison
	static SPQOS_RESULT EresUnittest_CIntervalFromScalarCmp(
		CMemoryPool *mp, CMDAccessor *md_accessor, CColRef *colref);

	// generate comparison expression
	static CExpression *PexprScalarCmp(CMemoryPool *mp,
									   CMDAccessor *md_accessor,
									   CColRef *colref,
									   IMDType::ECmpType cmp_type, LINT val);

	// interval from scalar bool op
	static SPQOS_RESULT EresUnittest_CIntervalFromScalarBoolOp(
		CMemoryPool *mp, CMDAccessor *md_accessor, CColRef *colref);

	// debug print
	static void PrintConstraint(CMemoryPool *mp, CConstraint *pcnstr);

	// build a conjunction
	static CConstraintConjunction *Pcstconjunction(CMemoryPool *mp, IMDId *mdid,
												   CColRef *colref);

	// build a disjunction
	static CConstraintDisjunction *Pcstdisjunction(CMemoryPool *mp, IMDId *mdid,
												   CColRef *colref);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_CInterval();
	static SPQOS_RESULT EresUnittest_CIntervalFromScalarExpr();
	static SPQOS_RESULT EresUnittest_CConjunction();
	static SPQOS_RESULT EresUnittest_CDisjunction();
	static SPQOS_RESULT EresUnittest_CNegation();
	static SPQOS_RESULT EresUnittest_CConstraintFromScalarExpr();
	static SPQOS_RESULT EresUnittest_CConstraintIntervalConvertsTo();
	static SPQOS_RESULT EresUnittest_CConstraintIntervalPexpr();
	static SPQOS_RESULT EresUnittest_CConstraintIntervalFromArrayExpr();

#ifdef SPQOS_DEBUG
	// tests for unconstrainable types
	static SPQOS_RESULT EresUnittest_NegativeTests();
#endif	// SPQOS_DEBUG

	// test constraints on date intervals
	static SPQOS_RESULT EresUnittest_ConstraintsOnDates();

	// print equivalence classes
	static void PrintEquivClasses(CMemoryPool *mp, CColRefSetArray *pdrspqcrs,
								  BOOL fExpected = false);
};	// class CConstraintTest
}  // namespace spqopt

#endif	// !SPQOPT_CConstraintTest_H


// EOF
