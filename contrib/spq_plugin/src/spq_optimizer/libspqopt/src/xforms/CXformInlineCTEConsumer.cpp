//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInlineCTEConsumer.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformInlineCTEConsumer.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformInlineCTEConsumer::CXformInlineCTEConsumer
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformInlineCTEConsumer::CXformInlineCTEConsumer(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalCTEConsumer(mp)))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformInlineCTEConsumer::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformInlineCTEConsumer::Exfp(CExpressionHandle &exprhdl) const
{
	const ULONG id = CLogicalCTEConsumer::PopConvert(exprhdl.Pop())->UlCTEId();
	CCTEInfo *pcteinfo = COptCtxt::PoctxtFromTLS()->Pcteinfo();

	if ((pcteinfo->FEnableInlining() || 1 == pcteinfo->UlConsumers(id)) &&
		CXformUtils::FInlinableCTE(id))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformInlineCTEConsumer::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformInlineCTEConsumer::Transform(CXformContext *
#ifdef SPQOS_DEBUG
									   pxfctxt
#endif
								   ,
								   CXformResult *pxfres,
								   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	// inline the consumer
	CLogicalCTEConsumer *popConsumer =
		CLogicalCTEConsumer::PopConvert(pexpr->Pop());
	CExpression *pexprAlt = popConsumer->PexprInlined();
	pexprAlt->AddRef();
	// add alternative to xform result
	pxfres->Add(pexprAlt);
}

// EOF
