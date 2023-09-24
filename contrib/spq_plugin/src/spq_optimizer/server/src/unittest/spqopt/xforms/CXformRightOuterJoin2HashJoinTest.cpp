//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformRightOuterJoin2HashJoinTest.cpp
//
//	@doc:
//		Test for right outer join to hash join transform
//---------------------------------------------------------------------------
#include "unittest/spqopt/xforms/CXformRightOuterJoin2HashJoinTest.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/ops.h"
#include "spqopt/xforms/CXformRightOuterJoin2HashJoin.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


//---------------------------------------------------------------------------
//	@function:
//		CXformRightOuterJoin2HashJoinTest::EresUnittest
//
//	@doc:
//		Unittest for predicate utilities
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformRightOuterJoin2HashJoinTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(
			CXformRightOuterJoin2HashJoinTest::EresUnittest_Transform),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CXformRightOuterJoin2HashJoinTest::EresUnittest_Transform
//
//	@doc:
//		Driver for transform tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformRightOuterJoin2HashJoinTest::EresUnittest_Transform()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	CXformContext *context = SPQOS_NEW(mp) CXformContext(mp);
	CXformResult *result = SPQOS_NEW(mp) CXformResult(mp);

	CXformRightOuterJoin2HashJoin *xform =
		SPQOS_NEW(mp) CXformRightOuterJoin2HashJoin(mp);

	CExpression *pLogicalRightOuterJoin = CTestUtils::PexprRightOuterJoin(mp);

	// xform should work even if child stats aren't derived yet.
	SPQOS_ASSERT(NULL == (*pLogicalRightOuterJoin)[0]->Pstats());
	SPQOS_ASSERT(NULL == (*pLogicalRightOuterJoin)[1]->Pstats());

	(void) xform->Transform(context, result, pLogicalRightOuterJoin);

	pLogicalRightOuterJoin->Release();
	xform->Release();
	context->Release();
	result->Release();

	return SPQOS_OK;
}

// EOF
