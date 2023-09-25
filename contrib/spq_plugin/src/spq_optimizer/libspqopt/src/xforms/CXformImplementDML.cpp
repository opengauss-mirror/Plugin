//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementDML.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementDML.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalDML.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalDML.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementDML::CXformImplementDML
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementDML::CXformImplementDML(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalDML(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementDML::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementDML::Exfp(CExpressionHandle &  // exprhdl
) const
{
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementDML::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementDML::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalDML *popDML = CLogicalDML::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative

	CLogicalDML::EDMLOperator edmlop = popDML->Edmlop();

	CTableDescriptor *ptabdesc = popDML->Ptabdesc();
	ptabdesc->AddRef();

	CColRefArray *pdrspqcrSource = popDML->PdrspqcrSource();
	pdrspqcrSource->AddRef();
	CBitSet *pbsModified = popDML->PbsModified();
	pbsModified->AddRef();

	CColRef *pcrAction = popDML->PcrAction();
	CColRef *pcrCtid = popDML->PcrCtid();
	CColRef *pcrSegmentId = popDML->PcrSegmentId();
	CColRef *pcrTupleOid = popDML->PcrTupleOid();

	// child of DML operator
	CExpression *pexprChild = (*pexpr)[0];
	pexprChild->AddRef();

	// create physical DML
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CPhysicalDML(mp, edmlop, ptabdesc, pdrspqcrSource, pbsModified,
						 pcrAction, pcrCtid, pcrSegmentId, pcrTupleOid),
		pexprChild);
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
