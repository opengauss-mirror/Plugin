//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2020 VMware, Inc.
//
//	@filename:
//		CXformIndexGet2IndexOnlyScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformIndexGet2IndexOnlyScan.h"

#include <cwchar>

#include "spqos/base.h"

#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalIndexGet.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalIndexOnlyScan.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/md/CMDIndexSPQDB.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformIndexGet2IndexOnlyScan::CXformIndexGet2IndexOnlyScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformIndexGet2IndexOnlyScan::CXformIndexGet2IndexOnlyScan(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalIndexGet(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // index lookup predicate
		  ))
{
}

CXform::EXformPromise
CXformIndexGet2IndexOnlyScan::Exfp(CExpressionHandle &exprhdl) const
{
	CLogicalIndexGet *popGet = CLogicalIndexGet::PopConvert(exprhdl.Pop());

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	CIndexDescriptor *pindexdesc = popGet->Pindexdesc();

	if ((pindexdesc->IndexType() == IMDIndex::EmdindBtree &&
		 ptabdesc->IsAORowOrColTable()) ||
		!pindexdesc->SupportsIndexOnlyScan())
	{
		// we don't support btree index scans on AO tables
		// FIXME: relax btree requirement. GiST and SP-GiST indexes can support some operator classes, but Gin cannot
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
//		CXformIndexGet2IndexOnlyScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformIndexGet2IndexOnlyScan::Transform(CXformContext *pxfctxt,
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

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdesc->MDId());
	const IMDIndex *pmdindex = md_accessor->RetrieveIndex(pindexdesc->MDId());

	CColRefArray *pdrspqcrOutput = pop->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	pdrspqcrOutput->AddRef();

	CColRefSet *matched_cols =
		CXformUtils::PcrsIndexKeys(mp, pdrspqcrOutput, pmdindex, pmdrel);
	CColRefSet *output_cols = SPQOS_NEW(mp) CColRefSet(mp);

	// An index only scan is allowed iff each used output column reference also
	// exists as a column in the index.
	for (ULONG i = 0; i < pdrspqcrOutput->Size(); i++)
	{
		CColRef *col = (*pdrspqcrOutput)[i];

		// In most cases we want to treat system columns unconditionally as
		// used. This is because certain transforms like those for DML or
		// CXformPushGbBelowJoin use unique keys in the derived properties,
		// even if they are not referenced in the query. Those keys are system
		// columns spq_segment_id and ctid. We also treat distribution columns
		// as used, since they appear in the CDistributionSpecHashed of
		// physical properties and therefore might be used in the plan.
		if (col->GetUsage(true /*check_system_cols*/,
						  true /*check_distribution_col*/) == CColRef::EUsed)
		{
			output_cols->Include(col);
		}
	}

	if (!matched_cols->ContainsAll(output_cols))
	{
		matched_cols->Release();
		output_cols->Release();
		pdrspqcrOutput->Release();
		return;
	}

	matched_cols->Release();
	output_cols->Release();

	pindexdesc->AddRef();
	ptabdesc->AddRef();

	COrderSpec *pos = pop->Pos();
	SPQOS_ASSERT(NULL != pos);
	pos->AddRef();



	// addref all children
	pexprIndexCond->AddRef();

	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalIndexOnlyScan(
			mp, pindexdesc, ptabdesc, pexpr->Pop()->UlOpId(),
			SPQOS_NEW(mp) CName(mp, pop->NameAlias()), pdrspqcrOutput, pos),
		pexprIndexCond);
	pxfres->Add(pexprAlt);
}


// EOF
