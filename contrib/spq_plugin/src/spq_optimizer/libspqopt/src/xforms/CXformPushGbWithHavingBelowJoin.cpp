//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformPushGbWithHavingBelowJoin.cpp
//
//	@doc:
//		Implementation of pushing group by below join transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformPushGbWithHavingBelowJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/xforms/CXformUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformPushGbWithHavingBelowJoin::CXformPushGbWithHavingBelowJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformPushGbWithHavingBelowJoin::CXformPushGbWithHavingBelowJoin(
	CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // join outer child
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // join inner child
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CPatternTree(mp))  // join predicate
				  ),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
			  ),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // Having clause
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformPushGbWithHavingBelowJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		we only push down global aggregates
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformPushGbWithHavingBelowJoin::Exfp(CExpressionHandle &  // exprhdl
) const
{
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformPushGbWithHavingBelowJoin::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformPushGbWithHavingBelowJoin::Transform(CXformContext *pxfctxt,
										   CXformResult *pxfres,
										   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpression *pexprGb = (*pexpr)[0];
	CLogicalGbAgg *popGbAgg = CLogicalGbAgg::PopConvert(pexprGb->Pop());
	if (!popGbAgg->FGlobal())
	{
		// xform only applies to global aggs
		return;
	}

	CExpression *pexprResult = CXformUtils::PexprPushGbBelowJoin(mp, pexpr);

	if (NULL != pexprResult)
	{
		// add alternative to results
		pxfres->Add(pexprResult);
	}
}


// EOF
