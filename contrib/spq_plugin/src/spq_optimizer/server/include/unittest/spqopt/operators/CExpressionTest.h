//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CExpressionTest.h
//
//	@doc:
//		Test for CExpression
//---------------------------------------------------------------------------
#ifndef SPQOPT_CExpressionTest_H
#define SPQOPT_CExpressionTest_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/CPrintPrefix.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/COperator.h"


namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CExpressionTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CExpressionTest
{
private:
	static CReqdPropPlan *PrppCreateRequiredProperties(CMemoryPool *mp,
													   CColRefSet *pcrs);

	static CExpression *PexprCreateGbyWithColumnFormat(
		CMemoryPool *mp, const WCHAR *wszColNameFormat);

	// helper for testing required column computation
	static SPQOS_RESULT EresComputeReqdCols(const CHAR *szFilePath);

	// helper for checking cached required columns
	static SPQOS_RESULT EresCheckCachedReqdCols(CMemoryPool *mp,
											   CExpression *pexpr,
											   CReqdPropPlan *prppInput);

	// helper function for the FValidPlan tests
	static void SetupPlanForFValidPlanTest(CMemoryPool *mp,
										   CExpression **ppexprGby,
										   CColRefSet **ppcrs,
										   CExpression **ppexprPlan,
										   CReqdPropPlan **pprpp);

	// return an expression with several joins
	static CExpression *PexprComplexJoinTree(CMemoryPool *mp);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_SimpleOps();
	static SPQOS_RESULT EresUnittest_Union();
	static SPQOS_RESULT EresUnittest_Const();
	static SPQOS_RESULT EresUnittest_BitmapGet();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_ComparisonTypes();
#endif	// SPQOS_DEBUG

	static SPQOS_RESULT EresUnittest_FValidPlan();

	static SPQOS_RESULT EresUnittest_FValidPlan_InvalidOrder();

	static SPQOS_RESULT EresUnittest_FValidPlan_InvalidDistribution();

	static SPQOS_RESULT EresUnittest_FValidPlan_InvalidRewindability();

	static SPQOS_RESULT EresUnittest_FValidPlan_InvalidCTEs();

	static SPQOS_RESULT EresUnittest_FValidPlanError();

	// test for required columns computation
	static SPQOS_RESULT EresUnittest_ReqdCols();

	// negative test for invalid SetOp expression
	static SPQOS_RESULT EresUnittest_InvalidSetOp();

};	// class CExpressionTest
}  // namespace spqopt

#endif	// !SPQOPT_CExpressionTest_H

// EOF
