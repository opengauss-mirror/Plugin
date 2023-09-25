//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CICGTest.h
//
//	@doc:
//		Test for installcheck-good bugs
//---------------------------------------------------------------------------
#ifndef SPQOPT_CICGTest_H
#define SPQOPT_CICGTest_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"

namespace spqopt
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CICGTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CICGTest
{
private:
	// function pointer type for checking predicates over DXL fragments
	typedef BOOL(FnDXLOpPredicate)(CDXLOperator *);

	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

	// counter used to mark last successful unsupported test
	static ULONG m_ulUnsupportedTestCounter;

	// counter used to mark last successful negative IndexApply test
	static ULONG m_ulNegativeIndexApplyTestCounter;

	// counter to mark last successful test for has joins versus index joins
	static ULONG m_ulTestCounterPreferHashJoinToIndexJoin;

	// counter to mark last successful test for index joins versus hash joins
	static ULONG m_ulTestCounterPreferIndexJoinToHashJoin;

	// counter to mark last successful test without additional traceflag
	static ULONG m_ulTestCounterNoAdditionTraceFlag;

	// check if all the operators in the given dxl fragment satisfy the given predicate
	static BOOL FDXLOpSatisfiesPredicate(CDXLNode *pdxl, FnDXLOpPredicate fdop);

	// check if the given dxl fragment does not contains Index Join
	static BOOL FIsNotIndexJoin(CDXLOperator *dxl_op);

	// check that the given dxl fragment does not contain an Index Join
	static BOOL FHasNoIndexJoin(CDXLNode *pdxl);

	// check that the given dxl fragment contains an Index Join
	static BOOL
	FHasIndexJoin(CDXLNode *pdxl)
	{
		return !FHasNoIndexJoin(pdxl);
	}

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunMinidumpTests();

	static SPQOS_RESULT EresUnittest_RunUnsupportedMinidumpTests();

	static SPQOS_RESULT EresUnittest_NegativeIndexApplyTests();

	// test that hash join is preferred versus index join when estimation risk is high
	static SPQOS_RESULT
	EresUnittest_PreferHashJoinVersusIndexJoinWhenRiskIsHigh();

	static SPQOS_RESULT EresUnittest_RunTestsWithoutAdditionalTraceFlags();

};	// class CICGTest
}  // namespace spqopt

#endif	// !SPQOPT_CICGTest_H

// EOF
