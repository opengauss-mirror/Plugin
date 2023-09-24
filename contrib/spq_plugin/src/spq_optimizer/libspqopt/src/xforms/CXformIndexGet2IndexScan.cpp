//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformIndexGet2IndexScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformIndexGet2IndexScan.h"

#include "spqos/base.h"

#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalIndexGet.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalIndexScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformIndexGet2IndexScan::CXformIndexGet2IndexScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformIndexGet2IndexScan::CXformIndexGet2IndexScan(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalIndexGet(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // index lookup predicate
		  ))
{
}

CXform::EXformPromise
CXformIndexGet2IndexScan::Exfp(CExpressionHandle &exprhdl) const
{
	CLogicalIndexGet *popGet = CLogicalIndexGet::PopConvert(exprhdl.Pop());

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	CIndexDescriptor *pindexdesc = popGet->Pindexdesc();

	if (pindexdesc->IndexType() == IMDIndex::EmdindBtree &&
		ptabdesc->IsAORowOrColTable())
	{
		// we don't support btree index scans on AO tables
		return CXform::ExfpNone;
	}

	if (exprhdl.DeriveHasSubquery(0))
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformIndexGet2IndexScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformIndexGet2IndexScan::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalIndexGet *pop = CLogicalIndexGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();
	CIndexDescriptor *pindexdesc = pop->Pindexdesc();
	CTableDescriptor *ptabdesc = pop->Ptabdesc();

	// extract components
	CExpression *pexprIndexCond = (*pexpr)[0];
	if (pexprIndexCond->DeriveHasSubquery())
	{
		return;
	}

	pindexdesc->AddRef();
	ptabdesc->AddRef();

	CColRefArray *pdrspqcrOutput = pop->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	pdrspqcrOutput->AddRef();

	COrderSpec *pos = pop->Pos();
	SPQOS_ASSERT(NULL != pos);
	pos->AddRef();

	// addref all children
	pexprIndexCond->AddRef();

	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalIndexScan(
			mp, pindexdesc, ptabdesc, pexpr->Pop()->UlOpId(),
			SPQOS_NEW(mp) CName(mp, pop->NameAlias()), pdrspqcrOutput, pos),
		pexprIndexCond);
	pxfres->Add(pexprAlt);
}


// EOF
