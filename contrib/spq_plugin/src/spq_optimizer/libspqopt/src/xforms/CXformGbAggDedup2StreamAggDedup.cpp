//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformGbAggDedup2StreamAggDedup.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformGbAggDedup2StreamAggDedup.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAggDeduplicate.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalStreamAggDeduplicate.h"
#include "spqopt/xforms/CXformGbAgg2HashAgg.h"
#include "spqopt/xforms/CXformUtils.h"
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggDedup2StreamAggDedup::CXformGbAggDedup2StreamAggDedup
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformGbAggDedup2StreamAggDedup::CXformGbAggDedup2StreamAggDedup(
	CMemoryPool *mp)
	: CXformGbAgg2StreamAgg(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAggDeduplicate(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggDedup2StreamAggDedup::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformGbAggDedup2StreamAggDedup::Transform(CXformContext *pxfctxt,
										   CXformResult *pxfres,
										   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprRel = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];
	SPQOS_ASSERT(0 == pexprScalar->Arity());

	// addref children
	pexprRel->AddRef();
	pexprScalar->AddRef();

	CLogicalGbAggDeduplicate *popAggDedup =
		CLogicalGbAggDeduplicate::PopConvert(pexpr->Pop());
	CColRefArray *colref_array = popAggDedup->Pdrspqcr();
	colref_array->AddRef();

	CColRefArray *pdrspqcrKeys = popAggDedup->PdrspqcrKeys();
	pdrspqcrKeys->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalStreamAggDeduplicate(
			mp, colref_array, popAggDedup->PdrspqcrMinimal(),
			popAggDedup->Egbaggtype(), pdrspqcrKeys,
			popAggDedup->FGeneratesDuplicates(),
			CXformUtils::FMultiStageAgg(pexpr),
			CXformUtils::FAggGenBySplitDQAXform(pexpr), popAggDedup->AggStage(),
			!CXformUtils::FLocalAggCreatedByEagerAggXform(pexpr)),
		pexprRel, pexprScalar);

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
