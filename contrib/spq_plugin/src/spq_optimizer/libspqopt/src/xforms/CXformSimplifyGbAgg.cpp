//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSimplifyGbAgg.cpp
//
//	@doc:
//		Implementation of simplifying an aggregate expression by finding
//		the minimal grouping columns based on functional dependencies
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSimplifyGbAgg.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifyGbAgg::CXformSimplifyGbAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSimplifyGbAgg::CXformSimplifyGbAgg(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
			  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifyGbAgg::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		aggregate must have grouping columns
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSimplifyGbAgg::Exfp(CExpressionHandle &exprhdl) const
{
	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(exprhdl.Pop());

	SPQOS_ASSERT(COperator::EgbaggtypeGlobal == popAgg->Egbaggtype());

	if (0 == popAgg->Pdrspqcr()->Size() || NULL != popAgg->PdrspqcrMinimal())
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifyGbAgg::FDropGbAgg
//
//	@doc:
//		Return true if GbAgg operator can be dropped because grouping
//		columns include a key
//
//---------------------------------------------------------------------------
BOOL
CXformSimplifyGbAgg::FDropGbAgg(CMemoryPool *mp, CExpression *pexpr,
								CXformResult *pxfres)
{
	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprProjectList = (*pexpr)[1];

	if (0 < pexprProjectList->Arity())
	{
		// GbAgg cannot be dropped if Agg functions are computed
		return false;
	}

	CKeyCollection *pkc = pexprRelational->DeriveKeyCollection();
	if (NULL == pkc)
	{
		// relational child does not have key
		return false;
	}

	const ULONG ulKeys = pkc->Keys();
	BOOL fDrop = false;
	for (ULONG ul = 0; !fDrop && ul < ulKeys; ul++)
	{
		CColRefArray *pdrspqcrKey = pkc->PdrspqcrKey(mp, ul);
		CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrKey);
		pdrspqcrKey->Release();

		CColRefSet *pcrsGrpCols = SPQOS_NEW(mp) CColRefSet(mp);
		pcrsGrpCols->Include(popAgg->Pdrspqcr());
		BOOL fGrpColsHasKey = pcrsGrpCols->ContainsAll(pcrs);

		pcrs->Release();
		pcrsGrpCols->Release();
		if (fGrpColsHasKey)
		{
			// Gb operator can be dropped
			pexprRelational->AddRef();
			CExpression *pexprResult = CUtils::PexprLogicalSelect(
				mp, pexprRelational,
				CPredicateUtils::PexprConjunction(mp, NULL));
			pxfres->Add(pexprResult);
			fDrop = true;
		}
	}

	return fDrop;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifyGbAgg::Transform
//
//	@doc:
//		Actual transformation to simplify a aggregate expression
//
//---------------------------------------------------------------------------
void
CXformSimplifyGbAgg::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	if (FDropGbAgg(mp, pexpr, pxfres))
	{
		// grouping columns could be dropped, GbAgg is transformed to a Select
		return;
	}

	// extract components
	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprProjectList = (*pexpr)[1];

	CColRefArray *colref_array = popAgg->Pdrspqcr();
	CColRefSet *pcrsGrpCols = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsGrpCols->Include(colref_array);

	CColRefSet *pcrsCovered =
		SPQOS_NEW(mp) CColRefSet(mp);  // set of grouping columns covered by FD's
	CColRefSet *pcrsMinimal = SPQOS_NEW(mp)
		CColRefSet(mp);	 // a set of minimal grouping columns based on FD's
	CFunctionalDependencyArray *pdrspqfd = pexpr->DeriveFunctionalDependencies();

	// collect grouping columns FD's
	const ULONG size = (pdrspqfd == NULL) ? 0 : pdrspqfd->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CFunctionalDependency *pfd = (*pdrspqfd)[ul];
		if (pfd->FIncluded(pcrsGrpCols))
		{
			pcrsCovered->Include(pfd->PcrsDetermined());
			pcrsCovered->Include(pfd->PcrsKey());
			pcrsMinimal->Include(pfd->PcrsKey());
		}
	}
	BOOL fCovered = pcrsCovered->Equals(pcrsGrpCols);
	pcrsGrpCols->Release();
	pcrsCovered->Release();

	if (!fCovered)
	{
		// the union of RHS of collected FD's does not cover all grouping columns
		pcrsMinimal->Release();
		return;
	}

	// create a new Agg with minimal grouping columns
	colref_array->AddRef();

	CLogicalGbAgg *popAggNew = SPQOS_NEW(mp) CLogicalGbAgg(
		mp, colref_array, pcrsMinimal->Pdrspqcr(mp), popAgg->Egbaggtype());
	pcrsMinimal->Release();
	SPQOS_ASSERT(!popAgg->Matches(popAggNew) &&
				"Simplified aggregate matches original aggregate");

	pexprRelational->AddRef();
	pexprProjectList->AddRef();
	CExpression *pexprResult = SPQOS_NEW(mp)
		CExpression(mp, popAggNew, pexprRelational, pexprProjectList);
	pxfres->Add(pexprResult);
}


// EOF
