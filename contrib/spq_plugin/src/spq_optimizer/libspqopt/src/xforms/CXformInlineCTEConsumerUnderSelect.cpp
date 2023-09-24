//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInlineCTEConsumerUnderSelect.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformInlineCTEConsumerUnderSelect.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformInlineCTEConsumerUnderSelect::CXformInlineCTEConsumerUnderSelect
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformInlineCTEConsumerUnderSelect::CXformInlineCTEConsumerUnderSelect(
	CMemoryPool *mp)
	: CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalCTEConsumer(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformInlineCTEConsumerUnderSelect::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformInlineCTEConsumerUnderSelect::Exfp(CExpressionHandle &  //exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformInlineCTEConsumerUnderSelect::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformInlineCTEConsumerUnderSelect::Transform(CXformContext *pxfctxt,
											  CXformResult *pxfres,
											  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CExpression *pexprConsumer = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	CLogicalCTEConsumer *popConsumer =
		CLogicalCTEConsumer::PopConvert(pexprConsumer->Pop());
	ULONG id = popConsumer->UlCTEId();
	CCTEInfo *pcteinfo = COptCtxt::PoctxtFromTLS()->Pcteinfo();
	// only continue if inlining is enabled or if this CTE has only 1 consumer
	if (!pcteinfo->FEnableInlining() && 1 < pcteinfo->UlConsumers(id))
	{
		return;
	}

	// don't push down selects with a const true or false, in case we end up
	// with a select(1) (coming from the anchor) right on top of the consumer
	if (CUtils::FScalarConstTrue(pexprScalar) ||
		CUtils::FScalarConstFalse(pexprScalar) ||
		!CXformUtils::FInlinableCTE(id))
	{
		return;
	}

	CMemoryPool *mp = pxfctxt->Pmp();

	// inline consumer
	SPQOS_ASSERT(NULL != popConsumer->Phmulcr());
	CExpression *pexprInlinedConsumer = popConsumer->PexprInlined();
	pexprInlinedConsumer->AddRef();
	pexprScalar->AddRef();

	CExpression *pexprSelect =
		CUtils::PexprLogicalSelect(mp, pexprInlinedConsumer, pexprScalar);

	CExpression *pexprNormalized = CNormalizer::PexprNormalize(mp, pexprSelect);
	pexprSelect->Release();

	// add alternative to xform result
	pxfres->Add(pexprNormalized);
}

// EOF
