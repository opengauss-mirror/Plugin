//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSimplifySubquery.cpp
//
//	@doc:
//		Simplify existential/quantified subqueries by transforming
//		into count(*) subqueries
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSimplifySubquery.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/md/IMDScalarOp.h"

using namespace spqmd;
using namespace spqopt;


// initialization of simplify function mappings
const CXformSimplifySubquery::SSimplifySubqueryMapping
	CXformSimplifySubquery::m_rgssm[] = {
		{FSimplifyExistential, CUtils::FExistentialSubquery},
		{FSimplifyQuantified, CUtils::FQuantifiedSubquery},

		// the last entry is used to replace existential subqueries with count(*)
		// after quantified subqueries have been replaced in the input expression
		{FSimplifyExistential, CUtils::FExistentialSubquery},
};

//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifySubquery::CXformSimplifySubquery
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSimplifySubquery::CXformSimplifySubquery(CExpression *pexprPattern)
	: CXformExploration(pexprPattern)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifySubquery::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		subqueries must exist in scalar tree
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSimplifySubquery::Exfp(CExpressionHandle &exprhdl) const
{
	// consider this transformation only if subqueries exist
	if (exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
	;
}



//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifySubquery::FSimplifyQuantified
//
//	@doc:
//		Transform quantified subqueries to count(*) subqueries;
//		the function returns true if transformation succeeded
//
//---------------------------------------------------------------------------
BOOL
CXformSimplifySubquery::FSimplifyQuantified(CMemoryPool *mp,
											CExpression *pexprScalar,
											CExpression **ppexprNewScalar)
{
	SPQOS_ASSERT(CUtils::FQuantifiedSubquery(pexprScalar->Pop()));

	CExpression *pexprNewSubquery = NULL;
	CExpression *pexprCmp = NULL;
	CXformUtils::QuantifiedToAgg(mp, pexprScalar, &pexprNewSubquery, &pexprCmp);

	// create a comparison predicate involving subquery expression
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	(*pexprCmp)[1]->AddRef();
	pdrspqexpr->Append(pexprNewSubquery);
	pdrspqexpr->Append((*pexprCmp)[1]);
	pexprCmp->Pop()->AddRef();

	*ppexprNewScalar = SPQOS_NEW(mp) CExpression(mp, pexprCmp->Pop(), pdrspqexpr);
	pexprCmp->Release();

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifySubquery::FSimplifyExistential
//
//	@doc:
//		Transform existential subqueries to count(*) subqueries;
//		the function returns true if transformation succeeded
//
//---------------------------------------------------------------------------
BOOL
CXformSimplifySubquery::FSimplifyExistential(CMemoryPool *mp,
											 CExpression *pexprScalar,
											 CExpression **ppexprNewScalar)
{
	SPQOS_ASSERT(CUtils::FExistentialSubquery(pexprScalar->Pop()));

	CExpression *pexprNewSubquery = NULL;
	CExpression *pexprCmp = NULL;
	CXformUtils::ExistentialToAgg(mp, pexprScalar, &pexprNewSubquery,
								  &pexprCmp);

	// create a comparison predicate involving subquery expression
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	(*pexprCmp)[1]->AddRef();
	pdrspqexpr->Append(pexprNewSubquery);
	pdrspqexpr->Append((*pexprCmp)[1]);
	pexprCmp->Pop()->AddRef();

	*ppexprNewScalar = SPQOS_NEW(mp) CExpression(mp, pexprCmp->Pop(), pdrspqexpr);
	pexprCmp->Release();

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifySubquery::FSimplify
//
//	@doc:
//		Transform existential/quantified subqueries to count(*) subqueries;
//		the function returns true if transformation succeeded
//
//---------------------------------------------------------------------------
BOOL
CXformSimplifySubquery::FSimplify(CMemoryPool *mp, CExpression *pexprScalar,
								  CExpression **ppexprNewScalar,
								  FnSimplify *pfnsimplify, FnMatch *pfnmatch)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexprScalar);

	if (pfnmatch(pexprScalar->Pop()))
	{
		return pfnsimplify(mp, pexprScalar, ppexprNewScalar);
	}

	// for all other types of subqueries, or if no other subqueries are
	// below this point, we add-ref root node and return immediately
	if (CUtils::FSubquery(pexprScalar->Pop()) ||
		!pexprScalar->DeriveHasSubquery())
	{
		pexprScalar->AddRef();
		*ppexprNewScalar = pexprScalar;

		return true;
	}

	// otherwise, recursively process children
	const ULONG arity = pexprScalar->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	BOOL fSuccess = true;
	for (ULONG ul = 0; fSuccess && ul < arity; ul++)
	{
		CExpression *pexprChild = NULL;
		fSuccess = FSimplify(mp, (*pexprScalar)[ul], &pexprChild, pfnsimplify,
							 pfnmatch);
		if (fSuccess)
		{
			pdrspqexprChildren->Append(pexprChild);
		}
		else
		{
			CRefCount::SafeRelease(pexprChild);
		}
	}

	if (fSuccess)
	{
		COperator *pop = pexprScalar->Pop();
		pop->AddRef();
		*ppexprNewScalar = SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
	}
	else
	{
		pdrspqexprChildren->Release();
	}

	return fSuccess;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSimplifySubquery::Transform
//
//	@doc:
//		Actual transformation to simplify subquery expression
//
//---------------------------------------------------------------------------
void
CXformSimplifySubquery::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpression *pexprInput = pexpr;
	const ULONG size = SPQOS_ARRAY_SIZE(m_rgssm);
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexprOuter = (*pexprInput)[0];
		CExpression *pexprScalar = (*pexprInput)[1];
		CExpression *pexprNewScalar = NULL;

		if (!FSimplify(mp, pexprScalar, &pexprNewScalar,
					   m_rgssm[ul].m_pfnsimplify, m_rgssm[ul].m_pfnmatch))
		{
			CRefCount::SafeRelease(pexprNewScalar);
			continue;
		}

		pexprOuter->AddRef();
		CExpression *pexprResult = NULL;
		if (COperator::EopLogicalSelect == pexprInput->Pop()->Eopid())
		{
			pexprResult =
				CUtils::PexprLogicalSelect(mp, pexprOuter, pexprNewScalar);
		}
		else
		{
			SPQOS_ASSERT(COperator::EopLogicalProject ==
						pexprInput->Pop()->Eopid());

			pexprResult = CUtils::PexprLogicalProject(
				mp, pexprOuter, pexprNewScalar, false /*fNewComputedCol*/);
		}

		// normalize resulting expression
		CExpression *pexprNormalized =
			CNormalizer::PexprNormalize(mp, pexprResult);
		pexprResult->Release();

		pxfres->Add(pexprNormalized);

		if (1 == ul)
		{
			pexprInput = pexprNormalized;
		}
	}
}


// EOF
