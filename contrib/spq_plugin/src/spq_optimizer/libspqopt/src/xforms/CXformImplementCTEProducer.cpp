//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementCTEProducer.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementCTEProducer.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalCTEProducer.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalCTEProducer.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementCTEProducer::CXformImplementCTEProducer
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementCTEProducer::CXformImplementCTEProducer(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalCTEProducer(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementCTEProducer::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementCTEProducer::Exfp(CExpressionHandle &  // exprhdl
) const
{
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementCTEProducer::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementCTEProducer::Transform(CXformContext *pxfctxt,
									  CXformResult *pxfres,
									  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalCTEProducer *popCTEProducer =
		CLogicalCTEProducer::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative
	ULONG id = popCTEProducer->UlCTEId();

	CColRefArray *colref_array = popCTEProducer->Pdrspqcr();
	colref_array->AddRef();

	// child of CTEProducer operator
	CExpression *pexprChild = (*pexpr)[0];
	pexprChild->AddRef();

	// create physical CTE Producer
	CExpression *pexprAlt = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CPhysicalCTEProducer(mp, id, colref_array),
					pexprChild);

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
