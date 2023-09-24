//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformSelect2PartialDynamicIndexGet.cpp
//
//	@doc:
//		Implementation of select over a partitioned table to a dynamic index get
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSelect2PartialDynamicIndexGet.h"

#include "spqos/base.h"

#include "spqopt/metadata/CPartConstraint.h"
#include "spqopt/operators/CLogicalDynamicGet.h"
#include "spqopt/operators/CLogicalGet.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2PartialDynamicIndexGet::CXformSelect2PartialDynamicIndexGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSelect2PartialDynamicIndexGet::CXformSelect2PartialDynamicIndexGet(
	CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalDynamicGet(mp)),	 // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2PartialDynamicIndexGet::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSelect2PartialDynamicIndexGet::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2PartialDynamicIndexGet::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformSelect2PartialDynamicIndexGet::Transform(CXformContext *pxfctxt,
											   CXformResult *pxfres,
											   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	// get the indexes on this relation
	CLogicalDynamicGet *popGet =
		CLogicalDynamicGet::PopConvert(pexprRelational->Pop());

	if (popGet->IsPartial())
	{
		// already a partial dynamic get; do not try to split further
		return;
	}

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdesc->MDId());
	const ULONG ulIndices = pmdrel->IndexCount();

	if (0 == ulIndices)
	{
		// no indexes on the table
		return;
	}

	// array of expressions in the scalar expression
	CExpressionArray *pdrspqexpr =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);
	SPQOS_ASSERT(0 < pdrspqexpr->Size());

	// derive the scalar and relational properties to build set of required columns
	CColRefSet *pcrsOutput = pexpr->DeriveOutputColumns();
	CColRefSet *pcrsScalarExpr = pexprScalar->DeriveUsedColumns();

	CColRefSet *pcrsReqd = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsReqd->Include(pcrsOutput);
	pcrsReqd->Include(pcrsScalarExpr);

	CPartConstraint *ppartcnstr = popGet->Ppartcnstr();
	ppartcnstr->AddRef();

	// find a candidate set of partial index combinations
	SPartDynamicIndexGetInfoArrays *pdrspqdrspqpartdig =
		CXformUtils::PdrspqdrspqpartdigCandidates(
			mp, md_accessor, pdrspqexpr, popGet->PdrspqdrspqcrPart(), pmdrel,
			ppartcnstr, popGet->PdrspqcrOutput(), pcrsReqd, pcrsScalarExpr,
			NULL  // pcrsAcceptedOuterRefs
		);

	// construct alternative partial index scan plans
	const ULONG ulCandidates = pdrspqdrspqpartdig->Size();
	for (ULONG ul = 0; ul < ulCandidates; ul++)
	{
		SPartDynamicIndexGetInfoArray *pdrspqpartdig = (*pdrspqdrspqpartdig)[ul];
		CreatePartialIndexGetPlan(mp, pexpr, pdrspqpartdig, pmdrel, pxfres);
	}

	ppartcnstr->Release();
	pcrsReqd->Release();
	pdrspqexpr->Release();
	pdrspqdrspqpartdig->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2PartialDynamicIndexGet::CreatePartialIndexGetPlan
//
//	@doc:
//		Create a plan as a union of the given partial index get candidates and
//		possibly a dynamic table scan
//
//---------------------------------------------------------------------------
void
CXformSelect2PartialDynamicIndexGet::CreatePartialIndexGetPlan(
	CMemoryPool *mp, CExpression *pexpr,
	SPartDynamicIndexGetInfoArray *pdrspqpartdig, const IMDRelation *pmdrel,
	CXformResult *pxfres) const
{
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	CLogicalDynamicGet *popGet =
		CLogicalDynamicGet::PopConvert(pexprRelational->Pop());
	CColRefArray *pdrspqcrGet = popGet->PdrspqcrOutput();

	const ULONG ulPartialIndexes = pdrspqpartdig->Size();

	CColRef2dArray *pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);
	CExpressionArray *pdrspqexprInput = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < ulPartialIndexes; ul++)
	{
		SPartDynamicIndexGetInfo *ppartdig = (*pdrspqpartdig)[ul];

		const IMDIndex *pmdindex = ppartdig->m_pmdindex;
		CPartConstraint *ppartcnstr = ppartdig->m_part_constraint;
		CExpressionArray *pdrspqexprIndex = ppartdig->m_pdrspqexprIndex;
		CExpressionArray *pdrspqexprResidual = ppartdig->m_pdrspqexprResidual;

		CColRefArray *pdrspqcrNew = pdrspqcrGet;

		if (0 < ul)
		{
			pdrspqcrNew = CUtils::PdrspqcrCopy(mp, pdrspqcrGet);
		}
		else
		{
			pdrspqcrNew->AddRef();
		}

		CExpression *pexprDynamicScan = NULL;
		if (NULL != pmdindex)
		{
			pexprDynamicScan = CXformUtils::PexprPartialDynamicIndexGet(
				mp, popGet, pexpr->Pop()->UlOpId(), pdrspqexprIndex,
				pdrspqexprResidual, pdrspqcrNew, pmdindex, pmdrel, ppartcnstr,
				NULL,  // pcrsAcceptedOuterRefs
				NULL,  // pdrspqcrOuter
				NULL   // pdrspqcrNewOuter
			);
		}
		else
		{
			pexprDynamicScan = PexprSelectOverDynamicGet(
				mp, popGet, pexprScalar, pdrspqcrNew, ppartcnstr);
		}
		SPQOS_ASSERT(NULL != pexprDynamicScan);

		pdrspqdrspqcrInput->Append(pdrspqcrNew);
		pdrspqexprInput->Append(pexprDynamicScan);
	}

	ULONG ulInput = pdrspqexprInput->Size();
	if (0 < ulInput)
	{
		CExpression *pexprResult = NULL;
		if (1 < ulInput)
		{
			pdrspqcrGet->AddRef();
			CColRefArray *pdrspqcrOuter = pdrspqcrGet;

			// construct a new union all operator
			pexprResult = SPQOS_NEW(mp) CExpression(
				mp,
				SPQOS_NEW(mp) CLogicalUnionAll(
					mp, pdrspqcrOuter, pdrspqdrspqcrInput, popGet->ScanId()),
				pdrspqexprInput);
		}
		else
		{
			pexprResult = (*pdrspqexprInput)[0];
			pexprResult->AddRef();

			// clean up
			pdrspqexprInput->Release();
			pdrspqdrspqcrInput->Release();
		}

		// if scalar expression involves the partitioning key, keep a SELECT node
		// on top for the purposes of partition selection
		CColRef2dArray *pdrspqdrspqcrPartKeys = popGet->PdrspqdrspqcrPart();
		CExpression *pexprPredOnPartKey =
			CPredicateUtils::PexprExtractPredicatesOnPartKeys(
				mp, pexprScalar, pdrspqdrspqcrPartKeys, NULL, /*pcrsAllowedRefs*/
				true										/*fUseConstraints*/
			);

		if (NULL != pexprPredOnPartKey)
		{
			pexprResult =
				SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalSelect(mp),
										 pexprResult, pexprPredOnPartKey);
		}

		pxfres->Add(pexprResult);

		return;
	}

	// clean up
	pdrspqdrspqcrInput->Release();
	pdrspqexprInput->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2PartialDynamicIndexGet::PexprSelectOverDynamicGet
//
//	@doc:
//		Create a partial dynamic get expression with a select on top
//
//---------------------------------------------------------------------------
CExpression *
CXformSelect2PartialDynamicIndexGet::PexprSelectOverDynamicGet(
	CMemoryPool *mp, CLogicalDynamicGet *popGet, CExpression *pexprScalar,
	CColRefArray *pdrspqcrDGet, CPartConstraint *ppartcnstr)
{
	UlongToColRefMap *colref_mapping =
		CUtils::PhmulcrMapping(mp, popGet->PdrspqcrOutput(), pdrspqcrDGet);

	// construct a partial dynamic get with the negated constraint
	CPartConstraint *ppartcnstrPartialDynamicGet =
		ppartcnstr->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
													  true /*must_exist*/);

	CLogicalDynamicGet *popPartialDynamicGet =
		(CLogicalDynamicGet *) popGet->PopCopyWithRemappedColumns(
			mp, colref_mapping, true /*must_exist*/);
	popPartialDynamicGet->SetPartConstraint(ppartcnstrPartialDynamicGet);
	popPartialDynamicGet->SetSecondaryScanId(
		COptCtxt::PoctxtFromTLS()->UlPartIndexNextVal());
	popPartialDynamicGet->SetPartial();

	CExpression *pexprSelect = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalSelect(mp),
					SPQOS_NEW(mp) CExpression(mp, popPartialDynamicGet),
					pexprScalar->PexprCopyWithRemappedColumns(
						mp, colref_mapping, true /*must_exist*/));

	colref_mapping->Release();

	return pexprSelect;
}

// EOF
