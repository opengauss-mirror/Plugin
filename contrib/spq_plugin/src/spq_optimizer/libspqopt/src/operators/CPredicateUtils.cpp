//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2011 EMC Corp.
//
//	@filename:
//		CPredicateUtils.cpp
//
//	@doc:
//		Implementation of predicate normalization
//---------------------------------------------------------------------------

#include "spqopt/operators/CPredicateUtils.h"

#include "spqos/base.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintDisjunction.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CFunctionProp.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CLogicalSetOp.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPhysicalJoin.h"
#include "spqopt/operators/CScalarCmp.h"
#include "spqopt/operators/CScalarFunc.h"
#include "spqopt/operators/CScalarIdent.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDArrayCoerceCastSPQDB.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDTypeBoolSPQDB.h"
#include "naucrates/md/CMDTypeGenericSPQDB.h"
#include "naucrates/md/CMDTypeInt2SPQDB.h"
#include "naucrates/md/CMDTypeInt4SPQDB.h"
#include "naucrates/md/CMDTypeInt8SPQDB.h"
#include "naucrates/md/IMDCast.h"
#include "naucrates/md/IMDIndex.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"

using namespace spqopt;
using namespace spqmd;

// check if the expression is a negated boolean scalar identifier
BOOL
CPredicateUtils::FNegatedBooleanScalarIdent(CExpression *pexprPred)
{
	SPQOS_ASSERT(NULL != pexprPred);

	if (CPredicateUtils::FNot(pexprPred))
	{
		return (FNot(pexprPred) && FBooleanScalarIdent((*pexprPred)[0]));
	}

	return false;
}

// check if the expression is a boolean scalar identifier
BOOL
CPredicateUtils::FBooleanScalarIdent(CExpression *pexprPred)
{
	SPQOS_ASSERT(NULL != pexprPred);

	if (COperator::EopScalarIdent == pexprPred->Pop()->Eopid())
	{
		CScalarIdent *popScIdent = CScalarIdent::PopConvert(pexprPred->Pop());
		if (IMDType::EtiBool ==
			popScIdent->Pcr()->RetrieveType()->GetDatumType())
		{
			return true;
		}
	}

	return false;
}

// is the given expression an equality comparison
BOOL
CPredicateUtils::IsEqualityOp(CExpression *pexpr)
{
	return FComparison(pexpr, IMDType::EcmptEq);
}

// is the given expression a comparison
BOOL
CPredicateUtils::FComparison(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	return COperator::EopScalarCmp == pexpr->Pop()->Eopid();
}

// is the given expression a comparison of the given type
BOOL
CPredicateUtils::FComparison(CExpression *pexpr, IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();

	if (COperator::EopScalarCmp != pop->Eopid())
	{
		return false;
	}

	CScalarCmp *popScCmp = CScalarCmp::PopConvert(pop);
	SPQOS_ASSERT(NULL != popScCmp);

	return cmp_type == popScCmp->ParseCmpType();
}

// Is the given expression a comparison over the given column. A comparison
// can only be between the given column and an expression involving only
// the allowed columns. If the allowed columns set is NULL, then we only want
// constant comparisons.
BOOL
CPredicateUtils::FComparison(
	CExpression *pexpr, CColRef *colref,
	CColRefSet
		*pcrsAllowedRefs  // other column references allowed in the comparison
)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopScalarCmp != pexpr->Pop()->Eopid())
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	if (CUtils::FScalarIdent(pexprLeft, colref) ||
		CScalarIdent::FCastedScId(pexprLeft, colref) ||
		CScalarIdent::FAllowedFuncScId(pexprLeft, colref))
	{
		return FValidRefsOnly(pexprRight, pcrsAllowedRefs);
	}

	if (CUtils::FScalarIdent(pexprRight, colref) ||
		CScalarIdent::FCastedScId(pexprRight, colref) ||
		CScalarIdent::FAllowedFuncScId(pexprRight, colref))
	{
		return FValidRefsOnly(pexprLeft, pcrsAllowedRefs);
	}

	return false;
}

// Is the given expression a range comparison only between the given column and
// an expression involving only the allowed columns. If the allowed columns set
// is NULL, then we only want constant comparisons.
// Also, the comparison type must be one of: LT, GT, LEq, GEq, Eq
// NEq is allowed only when requested by the caller
BOOL
CPredicateUtils::FRangeComparison(
	CExpression *pexpr, CColRef *colref,
	CColRefSet
		*pcrsAllowedRefs,  // other column references allowed in the comparison
	BOOL allowNotEqualPreds)
{
	if (!FComparison(pexpr, colref, pcrsAllowedRefs))
	{
		return false;
	}
	IMDType::ECmpType cmp_type =
		CScalarCmp::PopConvert(pexpr->Pop())->ParseCmpType();
	return (IMDType::EcmptOther != cmp_type &&
			(allowNotEqualPreds || IMDType::EcmptNEq != cmp_type));
}

BOOL
CPredicateUtils::FIdentCompareOuterRefExprIgnoreCast(
	CExpression *pexpr,
	CColRefSet
		*pcrsOuterRefs,	 // other column references allowed in the comparison
	CColRef **localColRef)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopScalarCmp != pexpr->Pop()->Eopid() ||
		NULL == pcrsOuterRefs)
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	BOOL leftIsACol = CUtils::FScalarIdentIgnoreCast(pexprLeft);
	BOOL rightIsACol = CUtils::FScalarIdentIgnoreCast(pexprRight);
	CColRefSet *pcrsUsedLeft = pexprLeft->DeriveUsedColumns();
	CColRefSet *pcrsUsedRight = pexprRight->DeriveUsedColumns();

	// allow any expressions of the form
	//  col = expr(outer refs)
	//  expr(outer refs) = col
	BOOL colOpOuterrefExpr =
		(leftIsACol && !pcrsOuterRefs->FIntersects(pcrsUsedLeft) &&
		 pcrsOuterRefs->ContainsAll(pcrsUsedRight));
	BOOL outerRefExprOpCol =
		(rightIsACol && !pcrsOuterRefs->FIntersects(pcrsUsedRight) &&
		 pcrsOuterRefs->ContainsAll(pcrsUsedLeft));

	if (NULL != localColRef)
	{
		if (colOpOuterrefExpr)
		{
			SPQOS_ASSERT(pcrsUsedLeft->Size() == 1);
			*localColRef = pcrsUsedLeft->PcrFirst();
		}
		else if (outerRefExprOpCol)
		{
			SPQOS_ASSERT(pcrsUsedRight->Size() == 1);
			*localColRef = pcrsUsedRight->PcrFirst();
		}
		else
		{
			// return value with be false, initialize the variable to be nice
			*localColRef = NULL;
		}
	}

	return colOpOuterrefExpr || outerRefExprOpCol;
}

// Check whether the given expression contains references to only the given
// columns. If pcrsAllowedRefs is NULL, then check whether the expression has
// no column references and no volatile functions
BOOL
CPredicateUtils::FValidRefsOnly(CExpression *pexprScalar,
								CColRefSet *pcrsAllowedRefs)
{
	if (NULL != pcrsAllowedRefs)
	{
		return pcrsAllowedRefs->ContainsAll(pexprScalar->DeriveUsedColumns());
	}

	return CUtils::FVarFreeExpr(pexprScalar) &&
		   IMDFunction::EfsVolatile !=
			   pexprScalar->DeriveScalarFunctionProperties()->Efs();
}

// is the given expression a disjunction of ident equality comparisons
// for example: returns true for "b = 2 or (b::int = 3 or b = 4)" if b
// is of type int, and false if b is of type float
BOOL
CPredicateUtils::FDisjunctionOfIdentEqComparisons(CMemoryPool *mp,
												  CExpression *pexpr,
												  CColRef *colref)
{
	SPQOS_ASSERT(NULL != pexpr);

	CExpressionArray *pdrspqexpr = PdrspqexprDisjuncts(mp, pexpr);
	const ULONG ulDisjuncts = pdrspqexpr->Size();

	for (ULONG ul = 0; ul < ulDisjuncts; ul++)
	{
		if (!CPredicateUtils::FIdentCompare((*pdrspqexpr)[ul], IMDType::EcmptEq,
											colref))
		{
			pdrspqexpr->Release();
			return false;
		}
	}

	pdrspqexpr->Release();
	return true;
}

// does the given expression have any NOT children?
BOOL
CPredicateUtils::FHasNegatedChild(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (FNot((*pexpr)[ul]))
		{
			return true;
		}
	}

	return false;
}

// recursively collect conjuncts
void
CPredicateUtils::CollectConjuncts(CExpression *pexpr,
								  CExpressionArray *pdrspqexpr)
{
	SPQOS_CHECK_STACK_SIZE;

	if (FAnd(pexpr))
	{
		const ULONG arity = pexpr->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CollectConjuncts((*pexpr)[ul], pdrspqexpr);
		}
	}
	else
	{
		pexpr->AddRef();
		pdrspqexpr->Append(pexpr);
	}
}

// recursively collect disjuncts
void
CPredicateUtils::CollectDisjuncts(CExpression *pexpr,
								  CExpressionArray *pdrspqexpr)
{
	SPQOS_CHECK_STACK_SIZE;

	if (FOr(pexpr))
	{
		const ULONG arity = pexpr->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CollectDisjuncts((*pexpr)[ul], pdrspqexpr);
		}
	}
	else
	{
		pexpr->AddRef();
		pdrspqexpr->Append(pexpr);
	}
}

// extract conjuncts from a predicate
CExpressionArray *
CPredicateUtils::PdrspqexprConjuncts(CMemoryPool *mp, CExpression *pexpr)
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CollectConjuncts(pexpr, pdrspqexpr);

	return pdrspqexpr;
}

// extract disjuncts from a predicate
CExpressionArray *
CPredicateUtils::PdrspqexprDisjuncts(CMemoryPool *mp, CExpression *pexpr)
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CollectDisjuncts(pexpr, pdrspqexpr);

	return pdrspqexpr;
}

// This function expects an array of disjuncts (children of OR operator),
// the function expands disjuncts in the given array by converting
// ArrayComparison to AND/OR tree and deduplicating resulting disjuncts
CExpressionArray *
CPredicateUtils::PdrspqexprExpandDisjuncts(CMemoryPool *mp,
										  CExpressionArray *pdrspqexprDisjuncts)
{
	SPQOS_ASSERT(NULL != pdrspqexprDisjuncts);

	CExpressionArray *pdrspqexprExpanded = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG size = pdrspqexprDisjuncts->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexpr = (*pdrspqexprDisjuncts)[ul];
		if (COperator::EopScalarArrayCmp == pexpr->Pop()->Eopid())
		{
			CExpression *pexprExpanded =
				CScalarArrayCmp::PexprExpand(mp, pexpr);
			if (FOr(pexprExpanded))
			{
				CExpressionArray *pdrspqexprArrayCmpDisjuncts =
					PdrspqexprDisjuncts(mp, pexprExpanded);
				CUtils::AddRefAppend<CExpression, CleanupRelease>(
					pdrspqexprExpanded, pdrspqexprArrayCmpDisjuncts);
				pdrspqexprArrayCmpDisjuncts->Release();
				pexprExpanded->Release();
			}
			else
			{
				pdrspqexprExpanded->Append(pexprExpanded);
			}

			continue;
		}

		if (FAnd(pexpr))
		{
			CExpressionArray *pdrspqexprConjuncts =
				PdrspqexprConjuncts(mp, pexpr);
			CExpressionArray *pdrspqexprExpandedConjuncts =
				PdrspqexprExpandConjuncts(mp, pdrspqexprConjuncts);
			pdrspqexprConjuncts->Release();
			pdrspqexprExpanded->Append(
				PexprConjunction(mp, pdrspqexprExpandedConjuncts));

			continue;
		}

		pexpr->AddRef();
		pdrspqexprExpanded->Append(pexpr);
	}

	CExpressionArray *pdrspqexprResult =
		CUtils::PdrspqexprDedup(mp, pdrspqexprExpanded);
	pdrspqexprExpanded->Release();

	return pdrspqexprResult;
}

// This function expects an array of conjuncts (children of AND operator),
// the function expands conjuncts in the given array by converting
// ArrayComparison to AND/OR tree and deduplicating resulting conjuncts
CExpressionArray *
CPredicateUtils::PdrspqexprExpandConjuncts(CMemoryPool *mp,
										  CExpressionArray *pdrspqexprConjuncts)
{
	SPQOS_ASSERT(NULL != pdrspqexprConjuncts);

	CExpressionArray *pdrspqexprExpanded = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG size = pdrspqexprConjuncts->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexpr = (*pdrspqexprConjuncts)[ul];
		if (COperator::EopScalarArrayCmp == pexpr->Pop()->Eopid())
		{
			CExpression *pexprExpanded =
				CScalarArrayCmp::PexprExpand(mp, pexpr);
			if (FAnd(pexprExpanded))
			{
				CExpressionArray *pdrspqexprArrayCmpConjuncts =
					PdrspqexprConjuncts(mp, pexprExpanded);
				CUtils::AddRefAppend<CExpression, CleanupRelease>(
					pdrspqexprExpanded, pdrspqexprArrayCmpConjuncts);
				pdrspqexprArrayCmpConjuncts->Release();
				pexprExpanded->Release();
			}
			else
			{
				pdrspqexprExpanded->Append(pexprExpanded);
			}

			continue;
		}

		if (FOr(pexpr))
		{
			CExpressionArray *pdrspqexprDisjuncts =
				PdrspqexprDisjuncts(mp, pexpr);
			CExpressionArray *pdrspqexprExpandedDisjuncts =
				PdrspqexprExpandDisjuncts(mp, pdrspqexprDisjuncts);
			pdrspqexprDisjuncts->Release();
			pdrspqexprExpanded->Append(
				PexprDisjunction(mp, pdrspqexprExpandedDisjuncts));

			continue;
		}

		pexpr->AddRef();
		pdrspqexprExpanded->Append(pexpr);
	}

	CExpressionArray *pdrspqexprResult =
		CUtils::PdrspqexprDedup(mp, pdrspqexprExpanded);
	pdrspqexprExpanded->Release();

	return pdrspqexprResult;
}

// check if a conjunct/disjunct can be skipped
BOOL
CPredicateUtils::FSkippable(CExpression *pexpr, BOOL fConjunction)
{
	return ((fConjunction && CUtils::FScalarConstTrue(pexpr)) ||
			(!fConjunction && CUtils::FScalarConstFalse(pexpr)));
}

// check if a conjunction/disjunction can be reduced to a constant
// True/False based on the given conjunct/disjunct
BOOL
CPredicateUtils::FReducible(CExpression *pexpr, BOOL fConjunction)
{
	return ((fConjunction && CUtils::FScalarConstFalse(pexpr)) ||
			(!fConjunction && CUtils::FScalarConstTrue(pexpr)));
}

// reverse the given operator type, for example > => <, <= => >=
IMDType::ECmpType
CPredicateUtils::EcmptReverse(IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(IMDType::EcmptOther > cmp_type);

	IMDType::ECmpType rgrgecmpt[][2] = {{IMDType::EcmptEq, IMDType::EcmptEq},
										{IMDType::EcmptG, IMDType::EcmptL},
										{IMDType::EcmptGEq, IMDType::EcmptLEq},
										{IMDType::EcmptNEq, IMDType::EcmptNEq}};

	const ULONG size = SPQOS_ARRAY_SIZE(rgrgecmpt);

	for (ULONG ul = 0; ul < size; ul++)
	{
		IMDType::ECmpType *pecmpt = rgrgecmpt[ul];

		if (pecmpt[0] == cmp_type)
		{
			return pecmpt[1];
		}

		if (pecmpt[1] == cmp_type)
		{
			return pecmpt[0];
		}
	}

	SPQOS_ASSERT(!"Comparison does not have a reverse");

	return IMDType::EcmptOther;
}

// is the condition a LIKE predicate
BOOL
CPredicateUtils::FLikePredicate(IMDId *mdid)
{
	SPQOS_ASSERT(NULL != mdid);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDScalarOp *md_scalar_op = md_accessor->RetrieveScOp(mdid);

	const CWStringConst *str_opname = md_scalar_op->Mdname().GetMDName();

	// comparison semantics for statistics purposes is looser
	// than regular comparison
	CWStringConst pstrLike(SPQOS_WSZ_LIT("~~"));
	if (!str_opname->Equals(&pstrLike))
	{
		return false;
	}

	return true;
}

// is the condition a LIKE predicate
BOOL
CPredicateUtils::FLikePredicate(CExpression *pexpr)
{
	COperator *pop = pexpr->Pop();
	if (COperator::EopScalarCmp != pop->Eopid())
	{
		return false;
	}

	CScalarCmp *popScCmp = CScalarCmp::PopConvert(pop);
	IMDId *mdid = popScCmp->MdIdOp();

	return FLikePredicate(mdid);
}

// extract the components of a LIKE predicate
void
CPredicateUtils::ExtractLikePredComponents(CExpression *pexprPred,
										   CExpression **ppexprScIdent,
										   CExpression **ppexprConst)
{
	SPQOS_ASSERT(NULL != pexprPred);
	SPQOS_ASSERT(2 == pexprPred->Arity());
	SPQOS_ASSERT(FLikePredicate(pexprPred));

	CExpression *pexprLeft = (*pexprPred)[0];
	CExpression *pexprRight = (*pexprPred)[1];

	*ppexprScIdent = NULL;
	*ppexprConst = NULL;

	CExpression *pexprLeftNoCast = pexprLeft;
	CExpression *pexprRightNoCast = pexprRight;

	if (COperator::EopScalarCast == pexprLeft->Pop()->Eopid())
	{
		pexprLeftNoCast = (*pexprLeft)[0];
	}

	if (COperator::EopScalarCast == pexprRight->Pop()->Eopid())
	{
		pexprRightNoCast = (*pexprRight)[0];
	}

	if (COperator::EopScalarIdent == pexprLeftNoCast->Pop()->Eopid())
	{
		*ppexprScIdent = pexprLeftNoCast;
	}
	else if (COperator::EopScalarIdent == pexprRightNoCast->Pop()->Eopid())
	{
		*ppexprScIdent = pexprRightNoCast;
	}

	if (COperator::EopScalarConst == pexprLeftNoCast->Pop()->Eopid())
	{
		*ppexprConst = pexprLeftNoCast;
	}
	else if (COperator::EopScalarConst == pexprRightNoCast->Pop()->Eopid())
	{
		*ppexprConst = pexprRightNoCast;
	}
}

// extract components in a comparison expression on the given key
void
CPredicateUtils::ExtractComponents(CExpression *pexprScCmp, CColRef *pcrKey,
								   CExpression **ppexprKey,
								   CExpression **ppexprOther,
								   IMDType::ECmpType *pecmpt)
{
	SPQOS_ASSERT(NULL != pexprScCmp);
	SPQOS_ASSERT(NULL != pcrKey);
	SPQOS_ASSERT(FComparison(pexprScCmp));

	*ppexprKey = NULL;
	*ppexprOther = NULL;

	CExpression *pexprLeft = (*pexprScCmp)[0];
	CExpression *pexprRight = (*pexprScCmp)[1];

	IMDType::ECmpType cmp_type =
		CScalarCmp::PopConvert(pexprScCmp->Pop())->ParseCmpType();

	if (CUtils::FScalarIdent(pexprLeft, pcrKey) ||
		CScalarIdent::FCastedScId(pexprLeft, pcrKey) ||
		CScalarIdent::FAllowedFuncScId(pexprLeft, pcrKey))
	{
		*ppexprKey = pexprLeft;
		*ppexprOther = pexprRight;
		*pecmpt = cmp_type;
	}
	else if (CUtils::FScalarIdent(pexprRight, pcrKey) ||
			 CScalarIdent::FCastedScId(pexprRight, pcrKey) ||
			 CScalarIdent::FAllowedFuncScId(pexprRight, pcrKey))
	{
		*ppexprKey = pexprRight;
		*ppexprOther = pexprLeft;
		*pecmpt = EcmptReverse(cmp_type);
	}
	SPQOS_ASSERT(NULL != *ppexprKey && NULL != *ppexprOther);
}

// Expression is a comparison with a simple identifer on at least one side
BOOL
CPredicateUtils::FIdentCompare(CExpression *pexpr, IMDType::ECmpType pecmpt,
							   CColRef *colref)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != colref);

	if (!FComparison(pexpr, pecmpt))
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	if (CUtils::FScalarIdent(pexprLeft, colref) ||
		CCastUtils::FBinaryCoercibleCastedScId(pexprLeft, colref))
	{
		return true;
	}
	else if (CUtils::FScalarIdent(pexprRight, colref) ||
			 CCastUtils::FBinaryCoercibleCastedScId(pexprRight, colref))
	{
		return true;
	}

	return false;
}

// create conjunction/disjunction from array of components; Takes ownership over the given array of expressions
CExpression *
CPredicateUtils::PexprConjDisj(CMemoryPool *mp, CExpressionArray *pdrspqexpr,
							   BOOL fConjunction)
{
	CScalarBoolOp::EBoolOperator eboolop = CScalarBoolOp::EboolopAnd;
	if (!fConjunction)
	{
		eboolop = CScalarBoolOp::EboolopOr;
	}

	CExpressionArray *pdrspqexprFinal = SPQOS_NEW(mp) CExpressionArray(mp);
	ULONG size = 0;
	if (NULL != pdrspqexpr)
	{
		size = pdrspqexpr->Size();
	}

	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexpr = (*pdrspqexpr)[ul];

		if (FSkippable(pexpr, fConjunction))
		{
			// skip current conjunct/disjunct
			continue;
		}

		if (FReducible(pexpr, fConjunction))
		{
			// a False (True) conjunct (disjunct) yields the whole conjunction (disjunction) False (True)
			CRefCount::SafeRelease(pdrspqexpr);
			CRefCount::SafeRelease(pdrspqexprFinal);

			return CUtils::PexprScalarConstBool(mp, !fConjunction /*fValue*/);
		}

		// add conjunct/disjunct to result array
		pexpr->AddRef();
		pdrspqexprFinal->Append(pexpr);
	}
	CRefCount::SafeRelease(pdrspqexpr);

	// assemble result
	CExpression *pexprResult = NULL;
	if (NULL != pdrspqexprFinal && (0 < pdrspqexprFinal->Size()))
	{
		if (1 == pdrspqexprFinal->Size())
		{
			pexprResult = (*pdrspqexprFinal)[0];
			pexprResult->AddRef();
			pdrspqexprFinal->Release();

			return pexprResult;
		}

		return CUtils::PexprScalarBoolOp(mp, eboolop, pdrspqexprFinal);
	}

	pexprResult = CUtils::PexprScalarConstBool(mp, fConjunction /*fValue*/);
	CRefCount::SafeRelease(pdrspqexprFinal);

	return pexprResult;
}

// create conjunction from array of components;
CExpression *
CPredicateUtils::PexprConjunction(CMemoryPool *mp, CExpressionArray *pdrspqexpr)
{
	return PexprConjDisj(mp, pdrspqexpr, true /*fConjunction*/);
}

// create disjunction from array of components;
CExpression *
CPredicateUtils::PexprDisjunction(CMemoryPool *mp, CExpressionArray *pdrspqexpr)
{
	return PexprConjDisj(mp, pdrspqexpr, false /*fConjunction*/);
}

// create a conjunction/disjunction of two components; Does *not* take ownership over given expressions
CExpression *
CPredicateUtils::PexprConjDisj(CMemoryPool *mp, CExpression *pexprOne,
							   CExpression *pexprTwo, BOOL fConjunction)
{
	SPQOS_ASSERT(NULL != pexprOne);
	SPQOS_ASSERT(NULL != pexprTwo);

	if (pexprOne == pexprTwo)
	{
		pexprOne->AddRef();
		return pexprOne;
	}

	CExpressionArray *pdrspqexprOne = NULL;
	CExpressionArray *pdrspqexprTwo = NULL;

	if (fConjunction)
	{
		pdrspqexprOne = PdrspqexprConjuncts(mp, pexprOne);
		pdrspqexprTwo = PdrspqexprConjuncts(mp, pexprTwo);
	}
	else
	{
		pdrspqexprOne = PdrspqexprDisjuncts(mp, pexprOne);
		pdrspqexprTwo = PdrspqexprDisjuncts(mp, pexprTwo);
	}

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CUtils::AddRefAppend<CExpression, CleanupRelease>(pdrspqexpr, pdrspqexprOne);
	CUtils::AddRefAppend<CExpression, CleanupRelease>(pdrspqexpr, pdrspqexprTwo);

	pdrspqexprOne->Release();
	pdrspqexprTwo->Release();

	return PexprConjDisj(mp, pdrspqexpr, fConjunction);
}

// create a conjunction of two components;
CExpression *
CPredicateUtils::PexprConjunction(CMemoryPool *mp, CExpression *pexprOne,
								  CExpression *pexprTwo)
{
	return PexprConjDisj(mp, pexprOne, pexprTwo, true /*fConjunction*/);
}

// create a disjunction of two components;
CExpression *
CPredicateUtils::PexprDisjunction(CMemoryPool *mp, CExpression *pexprOne,
								  CExpression *pexprTwo)
{
	return PexprConjDisj(mp, pexprOne, pexprTwo, false /*fConjunction*/);
}

// extract equality predicates over scalar identifiers
CExpressionArray *
CPredicateUtils::PdrspqexprPlainEqualities(CMemoryPool *mp,
										  CExpressionArray *pdrspqexpr)
{
	CExpressionArray *pdrspqexprEqualities = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG arity = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprCurr = (*pdrspqexpr)[ul];

		if (FPlainEquality(pexprCurr))
		{
			pexprCurr->AddRef();
			pdrspqexprEqualities->Append(pexprCurr);
		}
	}
	return pdrspqexprEqualities;
}

// is an expression an equality over scalar identifiers
BOOL
CPredicateUtils::FPlainEquality(CExpression *pexpr)
{
	if (IsEqualityOp(pexpr))
	{
		CExpression *pexprLeft = (*pexpr)[0];
		CExpression *pexprRight = (*pexpr)[1];

		// check if the scalar condition is over scalar idents
		if (COperator::EopScalarIdent == pexprLeft->Pop()->Eopid() &&
			COperator::EopScalarIdent == pexprRight->Pop()->Eopid())
		{
			return true;
		}
	}
	return false;
}

// is an expression a self comparison on some column
BOOL
CPredicateUtils::FSelfComparison(CExpression *pexpr, IMDType::ECmpType *pecmpt)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pecmpt);

	*pecmpt = IMDType::EcmptOther;
	COperator *pop = pexpr->Pop();
	if (CUtils::FScalarCmp(pexpr))
	{
		*pecmpt = CScalarCmp::PopConvert(pop)->ParseCmpType();
		COperator *popLeft = (*pexpr)[0]->Pop();
		COperator *popRight = (*pexpr)[1]->Pop();

		// return true if comparison is over the same column, and that column
		// is not nullable
		if (COperator::EopScalarIdent != popLeft->Eopid() ||
			COperator::EopScalarIdent != popRight->Eopid() ||
			CScalarIdent::PopConvert(popLeft)->Pcr() !=
				CScalarIdent::PopConvert(popRight)->Pcr())
		{
			return false;
		}

		CColRef *colref =
			const_cast<CColRef *>(CScalarIdent::PopConvert(popLeft)->Pcr());

		return CColRef::EcrtTable == colref->Ecrt() &&
			   !CColRefTable::PcrConvert(colref)->IsNullable();
	}

	return false;
}

// eliminate self comparison and replace it with True or False if possible
CExpression *
CPredicateUtils::PexprEliminateSelfComparison(CMemoryPool *mp,
											  CExpression *pexpr)
{
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	pexpr->AddRef();
	CExpression *pexprNew = pexpr;
	IMDType::ECmpType cmp_type = IMDType::EcmptOther;
	if (FSelfComparison(pexpr, &cmp_type))
	{
		switch (cmp_type)
		{
			case IMDType::EcmptEq:
			case IMDType::EcmptLEq:
			case IMDType::EcmptGEq:
				pexprNew->Release();
				pexprNew = CUtils::PexprScalarConstBool(mp, true /*value*/);
				break;

			case IMDType::EcmptNEq:
			case IMDType::EcmptL:
			case IMDType::EcmptG:
			case IMDType::EcmptIDF:
				pexprNew->Release();
				pexprNew = CUtils::PexprScalarConstBool(mp, false /*value*/);
				break;

			default:
				break;
		}
	}

	return pexprNew;
}

// is the given expression in the form (col1 Is NOT DISTINCT FROM col2)
BOOL
CPredicateUtils::FINDFScalarIdents(CExpression *pexpr)
{
	if (!FNot(pexpr))
	{
		return false;
	}

	CExpression *pexprChild = (*pexpr)[0];
	if (COperator::EopScalarIsDistinctFrom != pexprChild->Pop()->Eopid())
	{
		return false;
	}

	CExpression *pexprOuter = (*pexprChild)[0];
	CExpression *pexprInner = (*pexprChild)[1];

	return (COperator::EopScalarIdent == pexprOuter->Pop()->Eopid() &&
			COperator::EopScalarIdent == pexprInner->Pop()->Eopid());
}

// is the given expression in the form (col1 Is DISTINCT FROM col2)
BOOL
CPredicateUtils::FIDFScalarIdents(CExpression *pexpr)
{
	if (COperator::EopScalarIsDistinctFrom != pexpr->Pop()->Eopid())
	{
		return false;
	}

	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];

	return (COperator::EopScalarIdent == pexprOuter->Pop()->Eopid() &&
			COperator::EopScalarIdent == pexprInner->Pop()->Eopid());
}

// is the given expression in the form 'expr IS DISTINCT FROM false)'
BOOL
CPredicateUtils::FIDFFalse(CExpression *pexpr)
{
	if (COperator::EopScalarIsDistinctFrom != pexpr->Pop()->Eopid())
	{
		return false;
	}

	return CUtils::FScalarConstFalse((*pexpr)[0]) ||
		   CUtils::FScalarConstFalse((*pexpr)[1]);
}

// is the given expression in the form (expr IS DISTINCT FROM expr)
BOOL
CPredicateUtils::FIDF(CExpression *pexpr)
{
	return (COperator::EopScalarIsDistinctFrom == pexpr->Pop()->Eopid());
}

// is the given expression in the form (expr Is NOT DISTINCT FROM expr)
BOOL
CPredicateUtils::FINDF(CExpression *pexpr)
{
	return (FNot(pexpr) && FIDF((*pexpr)[0]));
}

// generate a conjunction of INDF expressions between corresponding columns in the given arrays
CExpression *
CPredicateUtils::PexprINDFConjunction(CMemoryPool *mp,
									  CColRefArray *pdrspqcrFirst,
									  CColRefArray *pdrspqcrSecond)
{
	SPQOS_ASSERT(NULL != pdrspqcrFirst);
	SPQOS_ASSERT(NULL != pdrspqcrSecond);
	SPQOS_ASSERT(pdrspqcrFirst->Size() == pdrspqcrSecond->Size());
	SPQOS_ASSERT(0 < pdrspqcrFirst->Size());

	const ULONG num_cols = pdrspqcrFirst->Size();
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		pdrspqexpr->Append(
			CUtils::PexprINDF(mp, (*pdrspqcrFirst)[ul], (*pdrspqcrSecond)[ul]));
	}

	return PexprConjunction(mp, pdrspqexpr);
}

// is the given expression a comparison between a scalar ident and a constant
BOOL
CPredicateUtils::FCompareIdentToConst(CExpression *pexpr)
{
	COperator *pop = pexpr->Pop();

	if (COperator::EopScalarCmp != pop->Eopid())
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	// left side must be scalar ident

	if (!(CUtils::FScalarIdent(pexprLeft) ||
		  CCastUtils::FBinaryCoercibleCastedScId(pexprLeft)))
	{
		return false;
	}

	// right side must be a constant
	if (!(CUtils::FScalarConst(pexprRight) ||
		  CCastUtils::FBinaryCoercibleCastedConst(pexprRight)))
	{
		return false;
	}

	return true;
}

// is the given expression of the form (col IS DISTINCT FROM const)
BOOL
CPredicateUtils::FIdentIDFConst(CExpression *pexpr)
{
	COperator *pop = pexpr->Pop();

	if (COperator::EopScalarIsDistinctFrom != pop->Eopid())
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	// left side must be scalar ident
	if (COperator::EopScalarIdent != pexprLeft->Pop()->Eopid())
	{
		return false;
	}

	// right side must be a constant
	if (COperator::EopScalarConst != pexprRight->Pop()->Eopid())
	{
		return false;
	}

	return true;
}

// is the given expression of the form (col = col)
BOOL
CPredicateUtils::FEqIdentsOfSameType(CExpression *pexpr)
{
	if (!CPredicateUtils::IsEqualityOp(pexpr))
	{
		return false;
	}
	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	// left side must be scalar ident
	if (COperator::EopScalarIdent != pexprLeft->Pop()->Eopid())
	{
		return false;
	}

	// right side must be a scalar ident
	if (COperator::EopScalarIdent != pexprRight->Pop()->Eopid())
	{
		return false;
	}

	CScalarIdent *left_ident = CScalarIdent::PopConvert(pexprLeft->Pop());
	CScalarIdent *right_ident = CScalarIdent::PopConvert(pexprRight->Pop());
	if (!left_ident->MdidType()->Equals(right_ident->MdidType()))
	{
		return false;
	}

	return true;
}


// is the given expression is of the form (col IS DISTINCT FROM const)
// ignoring cast on either sides
BOOL
CPredicateUtils::FIdentIDFConstIgnoreCast(CExpression *pexpr)
{
	return FIdentCompareConstIgnoreCast(pexpr,
										COperator::EopScalarIsDistinctFrom);
}

// is the given expression of the form (col cmp constant) ignoring casting on either sides
BOOL
CPredicateUtils::FIdentCompareConstIgnoreCast(CExpression *pexpr,
											  COperator::EOperatorId op_id)
{
	COperator *pop = pexpr->Pop();

	if (op_id != pop->Eopid())
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	// col IDF const
	if (COperator::EopScalarIdent == pexprLeft->Pop()->Eopid() &&
		COperator::EopScalarConst == pexprRight->Pop()->Eopid())
	{
		return true;
	}

	// cast(col) IDF const
	if (CScalarIdent::FCastedScId(pexprLeft) &&
		COperator::EopScalarConst == pexprRight->Pop()->Eopid())
	{
		return true;
	}

	// col IDF cast(constant)
	if (COperator::EopScalarIdent == pexprLeft->Pop()->Eopid() &&
		CScalarConst::FCastedConst(pexprRight))
	{
		return true;
	}

	// cast(col) IDF cast(constant)
	if (CScalarIdent::FCastedScId(pexprLeft) &&
		CScalarConst::FCastedConst(pexprRight))
	{
		return true;
	}

	return false;
}

// is the given expression a comparison between a const and a const
BOOL
CPredicateUtils::FCompareConstToConstIgnoreCast(CExpression *pexpr)
{
	COperator *pop = pexpr->Pop();

	if (COperator::EopScalarCmp != pop->Eopid())
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	// left side must be scalar const

	if (!(CUtils::FScalarConst(pexprLeft) ||
		  CCastUtils::FBinaryCoercibleCastedConst(pexprLeft)))
	{
		return false;
	}

	// right side must be a constant
	if (!(CUtils::FScalarConst(pexprRight) ||
		  CCastUtils::FBinaryCoercibleCastedConst(pexprRight)))
	{
		return false;
	}

	return true;
}

// is the given expression an array comparison between scalar ident
// and a const array or a constant
BOOL
CPredicateUtils::FArrayCompareIdentToConstIgnoreCast(CExpression *pexpr)
{
	if (!CUtils::FScalarArrayCmp(pexpr))
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	if (CUtils::FScalarIdentIgnoreCast(pexprLeft))
	{
		if ((CUtils::FScalarArray(pexprRight) ||
			 CUtils::FScalarArrayCoerce(pexprRight)))
		{
			CExpression *pexprArray = CUtils::PexprScalarArrayChild(pexpr);
			return CUtils::FScalarConstArray(pexprArray);
		}
		return CUtils::FScalarConst(pexprRight) ||
			   CScalarConst::FCastedConst(pexprRight);
	}

	return false;
}

// is the given expression of the form NOT (col IS DISTINCT FROM const) ignoring cast on either sides
BOOL
CPredicateUtils::FIdentINDFConstIgnoreCast(CExpression *pexpr)
{
	if (!FNot(pexpr))
	{
		return false;
	}

	return FIdentCompareConstIgnoreCast((*pexpr)[0],
										COperator::EopScalarIsDistinctFrom);
}

// is the given expression a comparison between a scalar ident under a scalar cast and a constant array
// +--CScalarArrayCmp Any (=)
// |--CScalarCast
// |  +--CScalarIdent
// +--CScalarConstArray:
BOOL
CPredicateUtils::FCompareCastIdentToConstArray(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (CUtils::FScalarArrayCmp(pexpr) &&
		(CCastUtils::FBinaryCoercibleCast((*pexpr)[0]) &&
		 CUtils::FScalarIdent((*(*pexpr)[0])[0])))
	{
		CExpression *pexprArray = CUtils::PexprScalarArrayChild(pexpr);
		return CUtils::FScalarConstArray(pexprArray);
	}

	return false;
}

// is the given expression a comparison between a scalar ident and an array with constants or ScalarIdents
BOOL
CPredicateUtils::FCompareScalarIdentToConstAndScalarIdentArray(
	CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!CUtils::FScalarArrayCmp(pexpr) || !CUtils::FScalarIdent((*pexpr)[0]) ||
		!CUtils::FScalarArray((*pexpr)[1]))
	{
		return false;
	}

	CExpression *pexprArray = CUtils::PexprScalarArrayChild(pexpr);
	return CUtils::FScalarConstAndScalarIdentArray(pexprArray);
}

// is the given expression a comparison between a scalar ident and a constant array
BOOL
CPredicateUtils::FCompareIdentToConstArray(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!CUtils::FScalarArrayCmp(pexpr) || !CUtils::FScalarIdent((*pexpr)[0]) ||
		!CUtils::FScalarArray((*pexpr)[1]))
	{
		return false;
	}

	CExpression *pexprArray = CUtils::PexprScalarArrayChild(pexpr);
	return CUtils::FScalarConstArray(pexprArray);
}

// Find a predicate that can be used for partition pruning with the given
// part key in the array of expressions if one exists. Relevant predicates
// are those that compare the partition key to expressions involving only
// the allowed columns. If the allowed columns set is NULL, then we only want
// constant comparisons.
CExpression *
CPredicateUtils::PexprPartPruningPredicate(
	CMemoryPool *mp, const CExpressionArray *pdrspqexpr, CColRef *pcrPartKey,
	CExpression *pexprCol,	// predicate on pcrPartKey obtained from pcnstr
	CColRefSet
		*pcrsAllowedRefs,  // allowed colrefs in exprs (except pcrPartKey)
	BOOL
		allowNotEqualPreds	// allow NEq to generate partition pruning predicate
)
{
	CExpressionArray *pdrspqexprResult = SPQOS_NEW(mp) CExpressionArray(mp);

	// Assert that pexprCol is an expr on pcrPartKey only and no other colref
	SPQOS_ASSERT(pexprCol == NULL || CUtils::FScalarConstTrue(pexprCol) ||
				(pexprCol->DeriveUsedColumns()->Size() == 1 &&
				 pexprCol->DeriveUsedColumns()->PcrFirst() == pcrPartKey));

	for (ULONG ul = 0; ul < pdrspqexpr->Size(); ul++)
	{
		CExpression *pexpr = (*pdrspqexpr)[ul];

		if (NULL != pcrsAllowedRefs &&
			!SPQOS_FTRACE(EopttraceAllowGeneralPredicatesforDPE))
		{
			// Only allow equal comparison exprs for dynamic partition selection.

			// This will reduce long execution times for partition selection using
			// non-equality predicates. These are expensive to execute since (at the
			// moment) to determine the selected partitions, the executor must, for
			// each row from its subtree, iterate over all the partition rules, and
			// for each such rule, execute the non-equality predicate. So, in case of
			// a large number of rows and/or large number of partitions the execution
			// time of partition selection may outweigh any potential savings earned
			// from skipping the scans of eliminated partitions.
			if (FComparison(pexpr, pcrPartKey, pcrsAllowedRefs) &&
				!pexpr->DeriveScalarFunctionProperties()
					 ->NeedsSingletonExecution())
			{
				CScalarCmp *popCmp = CScalarCmp::PopConvert(pexpr->Pop());

				if (popCmp->ParseCmpType() == IMDType::EcmptEq)
				{
					pexpr->AddRef();
					pdrspqexprResult->Append(pexpr);
				}
			}

			// pexprCol contains a predicate only on partKey, which is useless for
			// dynamic partition selection, so ignore it here
			pexprCol = NULL;
		}
		else
		{
			// (NULL == pcrsAllowedRefs) implies static partition elimination, since
			// the expressions we select can only contain the partition key
			// If EopttraceAllowGeneralPredicatesforDPE is set, allow a larger set
			// of partition predicates for DPE as well (see note above).

			if (FBoolPredicateOnColumn(pexpr, pcrPartKey) ||
				FNullCheckOnColumn(pexpr, pcrPartKey) ||
				IsDisjunctionOfRangeComparison(mp, pexpr, pcrPartKey,
											   pcrsAllowedRefs,
											   allowNotEqualPreds) ||
				(FRangeComparison(pexpr, pcrPartKey, pcrsAllowedRefs,
								  allowNotEqualPreds) &&
				 !pexpr->DeriveScalarFunctionProperties()
					  ->NeedsSingletonExecution()))
			{
				pexpr->AddRef();
				pdrspqexprResult->Append(pexpr);
			}
		}
	}

	// Remove any redundant "IS NOT NULL" filter on the partition key that was derived
	// from contraints
	if (pexprCol != NULL &&
		CPredicateUtils::FNotNullCheckOnColumn(pexprCol, pcrPartKey) &&
		(pdrspqexprResult->Size() > 0 &&
		 ExprsContainsOnlyStrictComparisons(pdrspqexprResult)))
	{
#ifdef SPQOS_DEBUG
		CColRefSet *pcrsUsed = CUtils::PcrsExtractColumns(mp, pdrspqexprResult);
		SPQOS_ASSERT_IMP(pdrspqexprResult->Size() > 0,
						pcrsUsed->FMember(pcrPartKey));
		CRefCount::SafeRelease(pcrsUsed);
#endif
		// pexprCol is a redundent "IS NOT NULL" expr. Ignore it
		pexprCol = NULL;
	}

	// Finally, remove duplicate expressions
	CExpressionArray *pdrspqexprResultNew =
		PdrspqexprAppendConjunctsDedup(mp, pdrspqexprResult, pexprCol);
	pdrspqexprResult->Release();
	pdrspqexprResult = pdrspqexprResultNew;

	if (0 == pdrspqexprResult->Size())
	{
		pdrspqexprResult->Release();
		return NULL;
	}

	return PexprConjunction(mp, pdrspqexprResult);
}

// append the conjuncts from the given expression to the given array, removing
// any duplicates, and return the resulting array
CExpressionArray *
CPredicateUtils::PdrspqexprAppendConjunctsDedup(CMemoryPool *mp,
											   CExpressionArray *pdrspqexpr,
											   CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);

	if (NULL == pexpr)
	{
		pdrspqexpr->AddRef();
		return pdrspqexpr;
	}

	CExpressionArray *pdrspqexprConjuncts = PdrspqexprConjuncts(mp, pexpr);
	CUtils::AddRefAppend(pdrspqexprConjuncts, pdrspqexpr);

	CExpressionArray *pdrspqexprNew =
		CUtils::PdrspqexprDedup(mp, pdrspqexprConjuncts);
	pdrspqexprConjuncts->Release();
	return pdrspqexprNew;
}

// check if the given expression is a boolean expression on the
// given column, e.g. if its of the form "ScalarIdent(colref)" or "Not(ScalarIdent(colref))"
BOOL
CPredicateUtils::FBoolPredicateOnColumn(CExpression *pexpr, CColRef *colref)
{
	BOOL fBoolean =
		(IMDType::EtiBool == colref->RetrieveType()->GetDatumType());

	if (fBoolean &&
		(CUtils::FScalarIdent(pexpr, colref) ||
		 (FNot(pexpr) && CUtils::FScalarIdent((*pexpr)[0], colref))))
	{
		return true;
	}

	return false;
}

// check if the given expression is a null check on the given column
// i.e. "is null" or "is not null"
BOOL
CPredicateUtils::FNullCheckOnColumn(CExpression *pexpr, CColRef *colref)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != colref);

	CExpression *pexprIsNull = pexpr;
	if (FNot(pexpr))
	{
		pexprIsNull = (*pexpr)[0];
	}

	if (CUtils::FScalarNullTest(pexprIsNull))
	{
		CExpression *pexprChild = (*pexprIsNull)[0];
		return (CUtils::FScalarIdent(pexprChild, colref) ||
				CCastUtils::FBinaryCoercibleCastedScId(pexprChild, colref));
	}

	return false;
}

// check if the given expression of the form "col is not null"
BOOL
CPredicateUtils::FNotNullCheckOnColumn(CExpression *pexpr, CColRef *colref)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != colref);

	if (0 == pexpr->Arity())
		return false;

	return (FNullCheckOnColumn(pexpr, colref) && FNot(pexpr));
}


// check if the given expression is a scalar array cmp expression on the
// given column
BOOL
CPredicateUtils::FScArrayCmpOnColumn(CExpression *pexpr, CColRef *colref,
									 BOOL fConstOnly)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopScalarArrayCmp != pexpr->Pop()->Eopid())
	{
		return false;
	}

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	if (!CUtils::FScalarIdent(pexprLeft, colref) ||
		!CUtils::FScalarArray(pexprRight))
	{
		return false;
	}

	const ULONG ulArrayElems = pexprRight->Arity();

	BOOL fSupported = true;
	for (ULONG ul = 0; ul < ulArrayElems && fSupported; ul++)
	{
		CExpression *pexprArrayElem = (*pexprRight)[ul];
		if (fConstOnly && !CUtils::FScalarConst(pexprArrayElem))
		{
			fSupported = false;
		}
	}

	return fSupported;
}

// check if the given expression is a disjunction of scalar cmp expression
// on the given column
BOOL
CPredicateUtils::IsDisjunctionOfRangeComparison(CMemoryPool *mp,
												CExpression *pexpr,
												CColRef *colref,
												CColRefSet *pcrsAllowedRefs,
												BOOL allowNotEqualPreds)
{
	if (!FOr(pexpr))
	{
		return false;
	}

	CExpressionArray *pdrspqexprDisjuncts = PdrspqexprDisjuncts(mp, pexpr);
	const ULONG ulDisjuncts = pdrspqexprDisjuncts->Size();
	for (ULONG ulDisj = 0; ulDisj < ulDisjuncts; ulDisj++)
	{
		CExpression *pexprDisj = (*pdrspqexprDisjuncts)[ulDisj];
		if (!FRangeComparison(pexprDisj, colref, pcrsAllowedRefs,
							  allowNotEqualPreds))
		{
			pdrspqexprDisjuncts->Release();
			return false;
		}
	}

	pdrspqexprDisjuncts->Release();
	return true;
}

// extract interesting expressions involving the partitioning keys;
// the function Add-Refs the returned copy if not null. Relevant predicates
// are those that compare the partition keys to expressions involving only
// the allowed columns. If the allowed columns set is NULL, then we only want
// constant filters.
CExpression *
CPredicateUtils::PexprExtractPredicatesOnPartKeys(
	CMemoryPool *mp, CExpression *pexprScalar,
	CColRef2dArray *pdrspqdrspqcrPartKeys, CColRefSet *pcrsAllowedRefs,
	BOOL fUseConstraints, const IMDRelation *pmdrel)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcrPartKeys);
	if (SPQOS_FTRACE(EopttraceDisablePartSelection))
	{
		return NULL;
	}

	CExpressionArray *pdrspqexprConjuncts = PdrspqexprConjuncts(mp, pexprScalar);
	CColRefSetArray *pdrspqcrsChild = NULL;
	CConstraint *pcnstr = NULL;
	if (pexprScalar->DeriveHasScalarArrayCmp() &&
		!SPQOS_FTRACE(EopttraceArrayConstraints))
	{
		// if we have any Array Comparisons, we expand them into conjunctions/disjunctions
		// of comparison predicates and then reconstruct scalar expression. This is because the
		// DXL translator for partitions would not previously handle array statements
		CExpressionArray *pdrspqexprExpandedConjuncts =
			PdrspqexprExpandConjuncts(mp, pdrspqexprConjuncts);
		pdrspqexprConjuncts->Release();
		CExpression *pexprExpandedScalar =
			PexprConjunction(mp, pdrspqexprExpandedConjuncts);

		// this will no longer contain array statements
		pdrspqexprConjuncts = PdrspqexprConjuncts(mp, pexprExpandedScalar);

		pcnstr = CConstraint::PcnstrFromScalarExpr(mp, pexprExpandedScalar,
												   &pdrspqcrsChild);
		pexprExpandedScalar->Release();
	}
	else
	{
		// skip this step when
		// 1. there are any Array comparisons
		// 2. previously, we expanded array expressions. However, there is now code to handle array
		// constraints in the DXL translator and therefore, it is unnecessary work to expand arrays
		// into disjunctions
		pcnstr =
			CConstraint::PcnstrFromScalarExpr(mp, pexprScalar, &pdrspqcrsChild);
	}
	CRefCount::SafeRelease(pdrspqcrsChild);


	// check if expanded scalar leads to a contradiction in computed constraint
	BOOL fContradiction = (NULL != pcnstr && pcnstr->FContradiction());
	if (fContradiction)
	{
		pdrspqexprConjuncts->Release();
		pcnstr->Release();

		return NULL;
	}

	const ULONG ulLevels = pdrspqdrspqcrPartKeys->Size();
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < ulLevels; ul++)
	{
		CColRef *colref = CUtils::PcrExtractPartKey(pdrspqdrspqcrPartKeys, ul);
		// extract part type for this level
		BOOL isKnownToBeListPartitioned = false;
		if (pmdrel != NULL)
		{
			CHAR szPartType = pmdrel->PartTypeAtLevel(ul);
			// we want to allow NEq predicates for partition selection only for
			// list partitioned tables. We avoid this for ranged partitioned tables
			// since NEq predicates can hardly eliminate any range partitions
			isKnownToBeListPartitioned =
				(IMDRelation::ErelpartitionList == szPartType);
		}

		CExpression *pexprCol =
			PexprPredicateCol(mp, pcnstr, colref, fUseConstraints);

		// look for a filter on the part key
		CExpression *pexprCmp = PexprPartPruningPredicate(
			mp, pdrspqexprConjuncts, colref, pexprCol, pcrsAllowedRefs,
			isKnownToBeListPartitioned /* allowNotEqualPreds */);
		CRefCount::SafeRelease(pexprCol);
		SPQOS_ASSERT_IMP(
			NULL != pexprCmp &&
				COperator::EopScalarCmp == pexprCmp->Pop()->Eopid(),
			IMDType::EcmptOther !=
				CScalarCmp::PopConvert(pexprCmp->Pop())->ParseCmpType());

		if (NULL != pexprCmp && !CUtils::FScalarConstTrue(pexprCmp))
		{
			// include comparison predicate if it is non-trivial
			pexprCmp->AddRef();
			pdrspqexpr->Append(pexprCmp);
		}
		CRefCount::SafeRelease(pexprCmp);
	}

	pdrspqexprConjuncts->Release();
	CRefCount::SafeRelease(pcnstr);

	if (0 == pdrspqexpr->Size())
	{
		pdrspqexpr->Release();
		return NULL;
	}

	return PexprConjunction(mp, pdrspqexpr);
}

// extract the constraint on the given column and return the corresponding scalar expression
CExpression *
CPredicateUtils::PexprPredicateCol(CMemoryPool *mp, CConstraint *pcnstr,
								   CColRef *colref, BOOL fUseConstraints)
{
	if (NULL == pcnstr || !fUseConstraints)
	{
		return NULL;
	}

	CExpression *pexprCol = NULL;
	CConstraint *pcnstrCol = pcnstr->Pcnstr(mp, colref);
	if (NULL != pcnstrCol && !pcnstrCol->IsConstraintUnbounded())
	{
		pexprCol = pcnstrCol->PexprScalar(mp);
		pexprCol->AddRef();
	}

	CRefCount::SafeRelease(pcnstrCol);

	return pexprCol;
}

// checks if comparison is between two columns, or a column and a const
BOOL
CPredicateUtils::FCompareColToConstOrCol(CExpression *pexprScalar)
{
	if (!CUtils::FScalarCmp(pexprScalar))
	{
		return false;
	}

	CExpression *pexprLeft = (*pexprScalar)[0];
	CExpression *pexprRight = (*pexprScalar)[1];

	BOOL fColLeft = CUtils::FScalarIdent(pexprLeft);
	BOOL fColRight = CUtils::FScalarIdent(pexprRight);
	BOOL fConstLeft = CUtils::FScalarConst(pexprLeft);
	BOOL fConstRight = CUtils::FScalarConst(pexprRight);

	return (fColLeft && fColRight) || (fColLeft && fConstRight) ||
		   (fConstLeft && fColRight);
}

// checks if the given constraint specifies a constant column
BOOL
CPredicateUtils::FConstColumn(CConstraint *pcnstr, const CColRef *
#ifdef SPQOS_DEBUG
													   colref
#endif	// SPQOS_DEBUG
)
{
	if (NULL == pcnstr || CConstraint::EctInterval != pcnstr->Ect())
	{
		// no constraint on column or constraint is not an interval
		return false;
	}

	SPQOS_ASSERT(pcnstr->FConstraint(colref));

	CConstraintInterval *pcnstrInterval =
		dynamic_cast<CConstraintInterval *>(pcnstr);
	CRangeArray *pdrspqrng = pcnstrInterval->Pdrspqrng();
	if (1 < pdrspqrng->Size())
	{
		return false;
	}

	if (0 == pdrspqrng->Size())
	{
		return pcnstrInterval->FIncludesNull();
	}

	SPQOS_ASSERT(1 == pdrspqrng->Size());

	const CRange *prng = (*pdrspqrng)[0];

	return prng->FPoint() && !pcnstrInterval->FIncludesNull();
}

// checks if the given constraint specifies a set of constants for a column
BOOL
CPredicateUtils::FColumnDisjunctionOfConst(CConstraint *pcnstr,
										   const CColRef *colref)
{
	if (FConstColumn(pcnstr, colref))
	{
		return true;
	}

	if (NULL == pcnstr || CConstraint::EctInterval != pcnstr->Ect())
	{
		// no constraint on column or constraint is not an interval
		return false;
	}

	SPQOS_ASSERT(pcnstr->FConstraint(colref));

	SPQOS_ASSERT(CConstraint::EctInterval == pcnstr->Ect());

	CConstraintInterval *pcnstrInterval =
		dynamic_cast<CConstraintInterval *>(pcnstr);
	return FColumnDisjunctionOfConst(pcnstrInterval, colref);
}

// checks if the given constraint specifies a set of constants for a column
BOOL
CPredicateUtils::FColumnDisjunctionOfConst(CConstraintInterval *pcnstrInterval,
										   const CColRef *
#ifdef SPQOS_DEBUG
											   colref
#endif
)
{
	SPQOS_ASSERT(pcnstrInterval->FConstraint(colref));

	CRangeArray *pdrspqrng = pcnstrInterval->Pdrspqrng();

	if (0 == pdrspqrng->Size())
	{
		return pcnstrInterval->FIncludesNull();
	}

	SPQOS_ASSERT(0 < pdrspqrng->Size());

	const ULONG ulRanges = pdrspqrng->Size();

	for (ULONG ul = 0; ul < ulRanges; ul++)
	{
		CRange *prng = (*pdrspqrng)[ul];
		if (!prng->FPoint())
		{
			return false;
		}
	}

	return true;
}

// helper to create index lookup comparison predicate with index key on left side
CExpression *
CPredicateUtils::PexprIndexLookupKeyOnLeft(CMemoryPool *mp,
										   CMDAccessor *md_accessor,
										   CExpression *pexprScalar,
										   const IMDIndex *pmdindex,
										   CColRefArray *pdrspqcrIndex,
										   CColRefSet *outer_refs)
{
	SPQOS_ASSERT(NULL != pexprScalar);

	CExpression *pexprLeft = (*pexprScalar)[0];
	CExpression *pexprRight = (*pexprScalar)[1];

	CColRefSet *pcrsIndex = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrIndex);

	if ((CUtils::FScalarIdent(pexprLeft) &&
		 pcrsIndex->FMember(
			 CScalarIdent::PopConvert(pexprLeft->Pop())->Pcr())) ||
		(CCastUtils::FBinaryCoercibleCastedScId(pexprLeft) &&
		 pcrsIndex->FMember(
			 CScalarIdent::PopConvert((*pexprLeft)[0]->Pop())->Pcr())))
	{
		// left expression is a scalar identifier or casted scalar identifier on an index key
		CColRefSet *pcrsUsedRight = pexprRight->DeriveUsedColumns();
		BOOL fSuccess = true;

		if (0 < pcrsUsedRight->Size())
		{
			if (!pcrsUsedRight->IsDisjoint(pcrsIndex))
			{
				// right argument uses index key, cannot use predicate for index lookup
				fSuccess = false;
			}
			else if (NULL != outer_refs)
			{
				CColRefSet *pcrsOuterRefsRight =
					SPQOS_NEW(mp) CColRefSet(mp, *pcrsUsedRight);
				pcrsOuterRefsRight->Difference(pcrsIndex);
				fSuccess = outer_refs->ContainsAll(pcrsOuterRefsRight);
				pcrsOuterRefsRight->Release();
			}
		}

		fSuccess =
			(fSuccess && FCompatibleIndexPredicate(pexprScalar, pmdindex,
												   pdrspqcrIndex, md_accessor));

		if (fSuccess)
		{
			pcrsIndex->Release();
			pexprScalar->AddRef();
			return pexprScalar;
		}
	}

	pcrsIndex->Release();
	return NULL;
}

// helper to create index lookup comparison predicate with index key on right side
CExpression *
CPredicateUtils::PexprIndexLookupKeyOnRight(CMemoryPool *mp,
											CMDAccessor *md_accessor,
											CExpression *pexprScalar,
											const IMDIndex *pmdindex,
											CColRefArray *pdrspqcrIndex,
											CColRefSet *outer_refs)
{
	SPQOS_ASSERT(NULL != pexprScalar);

	CExpression *pexprLeft = (*pexprScalar)[0];
	CExpression *pexprRight = (*pexprScalar)[1];
	if (CUtils::FScalarCmp(pexprScalar))
	{
		CScalarCmp *popScCmp = CScalarCmp::PopConvert(pexprScalar->Pop());
		CScalarCmp *popScCmpCommute =
			popScCmp->PopCommutedOp(mp, pexprScalar->Pop());

		if (popScCmpCommute)
		{
			// build new comparison after switching arguments and using commutative comparison operator
			pexprRight->AddRef();
			pexprLeft->AddRef();
			CExpression *pexprCommuted = SPQOS_NEW(mp)
				CExpression(mp, popScCmpCommute, pexprRight, pexprLeft);
			CExpression *pexprIndexCond =
				PexprIndexLookupKeyOnLeft(mp, md_accessor, pexprCommuted,
										  pmdindex, pdrspqcrIndex, outer_refs);
			pexprCommuted->Release();

			return pexprIndexCond;
		}
	}

	return NULL;
}

// Check if given expression is a valid index lookup predicate, and
// return modified (as needed) expression to be used for index lookup,
// a scalar expression is a valid index lookup predicate if it is in one
// the two forms:
//	[index-key CMP expr]
//	[expr CMP index-key]
// where expr is a scalar expression that is free of index keys and
// may have outer references (in the case of index nested loops)
CExpression *
CPredicateUtils::PexprIndexLookup(CMemoryPool *mp, CMDAccessor *md_accessor,
								  CExpression *pexprScalar,
								  const IMDIndex *pmdindex,
								  CColRefArray *pdrspqcrIndex,
								  CColRefSet *outer_refs,
								  BOOL allowArrayCmpForBTreeIndexes)
{
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(NULL != pdrspqcrIndex);

	IMDType::ECmpType cmptype = IMDType::EcmptOther;

	if (CUtils::FScalarCmp(pexprScalar))
	{
		cmptype = CScalarCmp::PopConvert(pexprScalar->Pop())->ParseCmpType();
	}
	else if (CUtils::FScalarArrayCmp(pexprScalar) &&
			 (IMDIndex::EmdindBitmap == pmdindex->IndexType() ||
			  (allowArrayCmpForBTreeIndexes &&
			   IMDIndex::EmdindBtree == pmdindex->IndexType())))
	{
		// array cmps are always allowed on bitmap indexes and when requested on btree indexes
		cmptype = CUtils::ParseCmpType(
			CScalarArrayCmp::PopConvert(pexprScalar->Pop())->MdIdOp());
	}

	BOOL gin_or_gist_index = (pmdindex->IndexType() == IMDIndex::EmdindGist ||
							  pmdindex->IndexType() == IMDIndex::EmdindGin);

	if (cmptype == IMDType::EcmptNEq || cmptype == IMDType::EcmptIDF ||
		(cmptype == IMDType::EcmptOther &&
		 !gin_or_gist_index) ||	 // only GiST indexes with a comparison type other are ok
		(gin_or_gist_index &&
		 pexprScalar->Arity() <
			 2))  // we do not support index expressions for GiST indexes
	{
		return NULL;
	}

	CExpression *pexprIndexLookupKeyOnLeft = PexprIndexLookupKeyOnLeft(
		mp, md_accessor, pexprScalar, pmdindex, pdrspqcrIndex, outer_refs);
	if (NULL != pexprIndexLookupKeyOnLeft)
	{
		return pexprIndexLookupKeyOnLeft;
	}

	CExpression *pexprIndexLookupKeyOnRight = PexprIndexLookupKeyOnRight(
		mp, md_accessor, pexprScalar, pmdindex, pdrspqcrIndex, outer_refs);
	if (NULL != pexprIndexLookupKeyOnRight)
	{
		return pexprIndexLookupKeyOnRight;
	}

	return NULL;
}

// split predicates into those that refer to an index key, and those that don't
void
CPredicateUtils::ExtractIndexPredicates(
	CMemoryPool *mp, CMDAccessor *md_accessor,
	CExpressionArray *pdrspqexprPredicate, const IMDIndex *pmdindex,
	CColRefArray *pdrspqcrIndex, CExpressionArray *pdrspqexprIndex,
	CExpressionArray *pdrspqexprResidual,
	CColRefSet *
		pcrsAcceptedOuterRefs,	// outer refs that are acceptable in an index predicate
	BOOL allowArrayCmpForBTreeIndexes)
{
	const ULONG length = pdrspqexprPredicate->Size();

	CColRefSet *pcrsIndex = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrIndex);

	for (ULONG ul = 0; ul < length; ul++)
	{
		CExpression *pexprCond = (*pdrspqexprPredicate)[ul];

		pexprCond->AddRef();

		CColRefSet *pcrsUsed =
			SPQOS_NEW(mp) CColRefSet(mp, *pexprCond->DeriveUsedColumns());
		if (NULL != pcrsAcceptedOuterRefs)
		{
			// filter out all accepted outer references
			pcrsUsed->Difference(pcrsAcceptedOuterRefs);
		}

		BOOL fSubset =
			(0 < pcrsUsed->Size()) && (pcrsIndex->ContainsAll(pcrsUsed));
		pcrsUsed->Release();

		if (!fSubset)
		{
			pdrspqexprResidual->Append(pexprCond);
			continue;
		}

		CExpressionArray *pdrspqexprTarget = pdrspqexprIndex;

		if (CUtils::FScalarIdentBoolType(pexprCond))
		{
			// expression is a column identifier of boolean type: convert to "col = true"
			pexprCond = CUtils::PexprScalarEqCmp(
				mp, pexprCond,
				CUtils::PexprScalarConstBool(mp, true /*value*/,
											 false /*is_null*/));
		}
		else if (FNot(pexprCond) &&
				 CUtils::FScalarIdentBoolType((*pexprCond)[0]))
		{
			// expression is of the form "not(col) for a column identifier of boolean type: convert to "col = false"
			CExpression *pexprScId = (*pexprCond)[0];
			pexprCond->Release();
			pexprScId->AddRef();
			pexprCond = CUtils::PexprScalarEqCmp(
				mp, pexprScId,
				CUtils::PexprScalarConstBool(mp, false /*value*/,
											 false /*is_null*/));
		}
		else
		{
			// attempt building index lookup predicate
			CExpression *pexprLookupPred = PexprIndexLookup(
				mp, md_accessor, pexprCond, pmdindex, pdrspqcrIndex,
				pcrsAcceptedOuterRefs, allowArrayCmpForBTreeIndexes);
			if (NULL != pexprLookupPred)
			{
				pexprCond->Release();
				pexprCond = pexprLookupPred;
			}
			else
			{
				// not a supported predicate
				pdrspqexprTarget = pdrspqexprResidual;
			}
		}

		pdrspqexprTarget->Append(pexprCond);
	}

	pcrsIndex->Release();
}

// split given scalar expression into two conjunctions; without outer
// references and with outer references
void
CPredicateUtils::SeparateOuterRefs(CMemoryPool *mp, CExpression *pexprScalar,
								   CColRefSet *outer_refs,
								   CExpression **ppexprLocal,
								   CExpression **ppexprOuterRef)
{
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(NULL != outer_refs);
	SPQOS_ASSERT(NULL != ppexprLocal);
	SPQOS_ASSERT(NULL != ppexprOuterRef);

	CColRefSet *pcrsUsed = pexprScalar->DeriveUsedColumns();
	if (pcrsUsed->IsDisjoint(outer_refs))
	{
		// if used columns are disjoint from outer references, return input expression
		pexprScalar->AddRef();
		*ppexprLocal = pexprScalar;
		*ppexprOuterRef = CUtils::PexprScalarConstBool(mp, true /*fval*/);
		return;
	}

	if (COperator::EopScalarNAryJoinPredList == pexprScalar->Pop()->Eopid())
	{
		// for a ScalarNAryJoinPredList we have to preserve that operator and
		// separate the outer refs from each of its children
		CExpressionArray *localChildren = SPQOS_NEW(mp) CExpressionArray(mp);
		CExpressionArray *outerRefChildren = SPQOS_NEW(mp) CExpressionArray(mp);

		for (ULONG c = 0; c < pexprScalar->Arity(); c++)
		{
			CExpression *childLocalExpr = NULL;
			CExpression *childOuterRefExpr = NULL;

			SeparateOuterRefs(mp, (*pexprScalar)[c], outer_refs,
							  &childLocalExpr, &childOuterRefExpr);
			localChildren->Append(childLocalExpr);
			outerRefChildren->Append(childOuterRefExpr);
		}

		// reassemble the CScalarNAryJoinPredList with its new children without outer refs
		pexprScalar->Pop()->AddRef();
		*ppexprLocal =
			SPQOS_NEW(mp) CExpression(mp, pexprScalar->Pop(), localChildren);

		// do the same with the outer refs
		pexprScalar->Pop()->AddRef();
		*ppexprOuterRef =
			SPQOS_NEW(mp) CExpression(mp, pexprScalar->Pop(), outerRefChildren);

		return;
	}

	CExpressionArray *pdrspqexpr = PdrspqexprConjuncts(mp, pexprScalar);
	CExpressionArray *pdrspqexprLocal = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprOuterRefs = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG size = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexprPred = (*pdrspqexpr)[ul];
		CColRefSet *pcrsPredUsed = pexprPred->DeriveUsedColumns();
		pexprPred->AddRef();
		if (0 == pcrsPredUsed->Size() || outer_refs->IsDisjoint(pcrsPredUsed))
		{
			pdrspqexprLocal->Append(pexprPred);
		}
		else
		{
			pdrspqexprOuterRefs->Append(pexprPred);
		}
	}
	pdrspqexpr->Release();

	*ppexprLocal = PexprConjunction(mp, pdrspqexprLocal);
	*ppexprOuterRef = PexprConjunction(mp, pdrspqexprOuterRefs);
}

// convert predicates of the form (a Cmp b) into (a InvCmp b);
// where InvCmp is the inverse comparison (e.g., '=' --> '<>')
CExpression *
CPredicateUtils::PexprInverseComparison(CMemoryPool *mp, CExpression *pexprCmp)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	IMDId *mdid_op = CScalarCmp::PopConvert(pexprCmp->Pop())->MdIdOp();
	IMDId *pmdidInverseOp =
		md_accessor->RetrieveScOp(mdid_op)->GetInverseOpMdid();
	const CWStringConst *pstrFirst =
		md_accessor->RetrieveScOp(pmdidInverseOp)->Mdname().GetMDName();

	// generate a predicate for the inversion of the comparison involved in the subquery
	(*pexprCmp)[0]->AddRef();
	(*pexprCmp)[1]->AddRef();
	pmdidInverseOp->AddRef();

	return CUtils::PexprScalarCmp(mp, (*pexprCmp)[0], (*pexprCmp)[1],
								  *pstrFirst, pmdidInverseOp);
}

// convert predicates of the form (true = (a Cmp b)) into (a Cmp b);
// do this operation recursively on deep expression tree
CExpression *
CPredicateUtils::PexprPruneSuperfluosEquality(CMemoryPool *mp,
											  CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	if (CUtils::FSubquery(pexpr->Pop()))
	{
		// cannot recurse below subquery
		pexpr->AddRef();
		return pexpr;
	}

	if (IsEqualityOp(pexpr))
	{
		BOOL fConstTrueLeftChild = CUtils::FScalarConstTrue((*pexpr)[0]);
		BOOL fConstTrueRightChild = CUtils::FScalarConstTrue((*pexpr)[1]);
		BOOL fConstFalseLeftChild = CUtils::FScalarConstFalse((*pexpr)[0]);
		BOOL fConstFalseRightChild = CUtils::FScalarConstFalse((*pexpr)[1]);

		BOOL fCmpLeftChild = CUtils::FScalarCmp((*pexpr)[0]);
		BOOL fCmpRightChild = CUtils::FScalarCmp((*pexpr)[1]);

		if (fCmpRightChild)
		{
			if (fConstTrueLeftChild)
			{
				return PexprPruneSuperfluosEquality(mp, (*pexpr)[1]);
			}

			if (fConstFalseLeftChild)
			{
				CExpression *pexprInverse =
					PexprInverseComparison(mp, (*pexpr)[1]);
				CExpression *pexprPruned =
					PexprPruneSuperfluosEquality(mp, pexprInverse);
				pexprInverse->Release();
				return pexprPruned;
			}
		}

		if (fCmpLeftChild)
		{
			if (fConstTrueRightChild)
			{
				return PexprPruneSuperfluosEquality(mp, (*pexpr)[0]);
			}

			if (fConstFalseRightChild)
			{
				CExpression *pexprInverse =
					PexprInverseComparison(mp, (*pexpr)[0]);
				CExpression *pexprPruned =
					PexprPruneSuperfluosEquality(mp, pexprInverse);
				pexprInverse->Release();

				return pexprPruned;
			}
		}
	}

	// process children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild =
			PexprPruneSuperfluosEquality(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// determine if we should test predicate implication for statistics computation
BOOL
CPredicateUtils::FCheckPredicateImplication(CExpression *pexprPred)
{
	// currently restrict testing implication to only equality of column references on scalar
	// ident and binary coercible casted idents
	if (COperator::EopScalarCmp == pexprPred->Pop()->Eopid() &&
		IMDType::EcmptEq ==
			CScalarCmp::PopConvert(pexprPred->Pop())->ParseCmpType())
	{
		CExpression *pexprLeft = (*pexprPred)[0];
		CExpression *pexprRight = (*pexprPred)[1];
		return ((COperator::EopScalarIdent == pexprLeft->Pop()->Eopid() ||
				 CCastUtils::FBinaryCoercibleCastedScId(pexprLeft)) &&
				(COperator::EopScalarIdent == pexprRight->Pop()->Eopid() ||
				 CCastUtils::FBinaryCoercibleCastedScId(pexprRight)));
	}
	return false;
}

// Given a predicate and a list of equivalence classes, return true if that predicate is
// implied by given equivalence classes
BOOL
CPredicateUtils::FImpliedPredicate(CExpression *pexprPred,
								   CColRefSetArray *pdrspqcrsEquivClasses)
{
	SPQOS_ASSERT(pexprPred->Pop()->FScalar());
	SPQOS_ASSERT(FCheckPredicateImplication(pexprPred));

	CColRefSet *pcrsUsed = pexprPred->DeriveUsedColumns();
	const ULONG size = pdrspqcrsEquivClasses->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRefSet *pcrs = (*pdrspqcrsEquivClasses)[ul];
		if (pcrs->ContainsAll(pcrsUsed))
		{
			// predicate is implied by given equivalence classes
			return true;
		}
	}

	return false;
}

// Remove conjuncts that are implied based on child equivalence classes,
// the main use case is minimizing join/selection predicates to avoid
// cardinality under-estimation,
//
// for example, in the expression ((a=b) AND (a=c)), if a child
// equivalence class is {b,c}, then we remove the conjunct (a=c)
// since it can be implied from {b,c}, {a,b}
CExpression *
CPredicateUtils::PexprRemoveImpliedConjuncts(CMemoryPool *mp,
											 CExpression *pexprScalar,
											 CExpressionHandle &exprhdl)
{
	if (COperator::EopScalarNAryJoinPredList == pexprScalar->Pop()->Eopid())
	{
		// for a ScalarNAryJoinPredList we have to preserve that operator and
		// remove implied preds from each child individually
		CExpressionArray *newChildren = SPQOS_NEW(mp) CExpressionArray(mp);

		for (ULONG c = 0; c < pexprScalar->Arity(); c++)
		{
			newChildren->Append(
				PexprRemoveImpliedConjuncts(mp, (*pexprScalar)[c], exprhdl));
		}

		// reassemble the CScalarNAryJoinPredList with its new children without implied conjuncts
		pexprScalar->Pop()->AddRef();

		return SPQOS_NEW(mp) CExpression(mp, pexprScalar->Pop(), newChildren);
	}

	// extract equivalence classes from logical children
	CColRefSetArray *pdrspqcrs =
		CUtils::PdrspqcrsCopyChildEquivClasses(mp, exprhdl);

	// extract all the conjuncts
	CExpressionArray *pdrspqexprConjuncts = PdrspqexprConjuncts(mp, pexprScalar);
	const ULONG size = pdrspqexprConjuncts->Size();
	CExpressionArray *pdrspqexprNewConjuncts = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexprConj = (*pdrspqexprConjuncts)[ul];
		if (FCheckPredicateImplication(pexprConj) &&
			FImpliedPredicate(pexprConj, pdrspqcrs))
		{
			// skip implied conjunct
			continue;
		}

		// add predicate to current equivalence classes
		CColRefSetArray *pdrspqcrsConj = NULL;
		CConstraint *pcnstr =
			CConstraint::PcnstrFromScalarExpr(mp, pexprConj, &pdrspqcrsConj);
		CRefCount::SafeRelease(pcnstr);
		if (NULL != pdrspqcrsConj)
		{
			CColRefSetArray *pdrspqcrsMerged =
				CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs, pdrspqcrsConj);
			pdrspqcrs->Release();
			pdrspqcrsConj->Release();
			pdrspqcrs = pdrspqcrsMerged;
		}

		// add conjunct to new conjuncts array
		pexprConj->AddRef();
		pdrspqexprNewConjuncts->Append(pexprConj);
	}

	pdrspqexprConjuncts->Release();
	pdrspqcrs->Release();

	return PexprConjunction(mp, pdrspqexprNewConjuncts);
}

// check if given correlations are valid for (anti)semi-joins;
// we disallow correlations referring to inner child, since inner
// child columns are not visible above (anti)semi-join
BOOL
CPredicateUtils::FValidSemiJoinCorrelations(
	CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
	CExpressionArray *pdrspqexprCorrelations)
{
	SPQOS_ASSERT(NULL != pexprOuter);
	SPQOS_ASSERT(NULL != pexprInner);

	CColRefSet *pcrsOuterOuput = pexprOuter->DeriveOutputColumns();
	CColRefSet *pcrsInnerOuput = pexprInner->DeriveOutputColumns();

	// collect output columns of both children
	CColRefSet *pcrsChildren = SPQOS_NEW(mp) CColRefSet(mp, *pcrsOuterOuput);
	pcrsChildren->Union(pcrsInnerOuput);

	const ULONG ulCorrs = pdrspqexprCorrelations->Size();
	BOOL fValid = true;
	for (ULONG ul = 0; fValid && ul < ulCorrs; ul++)
	{
		CExpression *pexprPred = (*pdrspqexprCorrelations)[ul];
		CColRefSet *pcrsUsed = pexprPred->DeriveUsedColumns();
		if (0 < pcrsUsed->Size() && !pcrsChildren->ContainsAll(pcrsUsed) &&
			!pcrsUsed->IsDisjoint(pcrsInnerOuput))
		{
			// disallow correlations referring to inner child
			fValid = false;
		}
	}
	pcrsChildren->Release();

	return fValid;
}

// check if given expression is (a conjunction of) simple column
// equality that use columns from the given column set
BOOL
CPredicateUtils::FSimpleEqualityUsingCols(CMemoryPool *mp,
										  CExpression *pexprScalar,
										  CColRefSet *pcrs)
{
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(pexprScalar->Pop()->FScalar());
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(0 < pcrs->Size());

	// break expression into conjuncts
	CExpressionArray *pdrspqexpr = PdrspqexprConjuncts(mp, pexprScalar);
	const ULONG size = pdrspqexpr->Size();
	BOOL fSuccess = true;
	for (ULONG ul = 0; fSuccess && ul < size; ul++)
	{
		// join predicate must be an equality of scalar idents and uses columns from given set
		CExpression *pexprConj = (*pdrspqexpr)[ul];
		CColRefSet *pcrsUsed = pexprConj->DeriveUsedColumns();
		fSuccess = IsEqualityOp(pexprConj) &&
				   CUtils::FScalarIdent((*pexprConj)[0]) &&
				   CUtils::FScalarIdent((*pexprConj)[1]) &&
				   !pcrs->IsDisjoint(pcrsUsed);
	}
	pdrspqexpr->Release();

	return fSuccess;
}

// for all columns in the given expression and are members of the given column set, replace columns with NULLs
CExpression *
CPredicateUtils::PexprReplaceColsWithNulls(CMemoryPool *mp,
										   CExpression *pexprScalar,
										   CColRefSet *pcrs)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexprScalar);

	COperator *pop = pexprScalar->Pop();
	SPQOS_ASSERT(pop->FScalar());

	if (CUtils::FSubquery(pop))
	{
		// do not recurse into subqueries
		pexprScalar->AddRef();
		return pexprScalar;
	}

	if (COperator::EopScalarIdent == pop->Eopid() &&
		pcrs->FMember(CScalarIdent::PopConvert(pop)->Pcr()))
	{
		// replace column with NULL constant
		return CUtils::PexprScalarConstBool(mp, false /*value*/,
											true /*is_null*/);
	}

	// process children recursively
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulChildren = pexprScalar->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild =
			PexprReplaceColsWithNulls(mp, (*pexprScalar)[ul], pcrs);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// check if scalar expression evaluates to (NOT TRUE) when
// all columns in the given set that are included in the expression
// are set to NULL
BOOL
CPredicateUtils::FNullRejecting(CMemoryPool *mp, CExpression *pexprScalar,
								CColRefSet *pcrs)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(pexprScalar->Pop()->FScalar());

	BOOL fHasVolatileFunctions =
		(IMDFunction::EfsVolatile ==
		 pexprScalar->DeriveScalarFunctionProperties()->Efs());

	if (fHasVolatileFunctions || pexprScalar->DeriveHasNonScalarFunction())
	{
		// scalar expression must not have volatile functions, subquery or non-scalar functions
		return false;
	}

	// create another expression copy where we replace columns included in the set with NULL values
	CExpression *pexprColsReplacedWithNulls =
		PexprReplaceColsWithNulls(mp, pexprScalar, pcrs);

	// evaluate the resulting expression
	CScalar::EBoolEvalResult eber =
		CScalar::EberEvaluate(mp, pexprColsReplacedWithNulls);
	pexprColsReplacedWithNulls->Release();

	// return TRUE if expression evaluation  result is (NOT TRUE), which means we need to
	// check if result is NULL or result is False,
	// in these two cases, a predicate will filter out incoming tuple, which means it is Null-Rejecting
	return (CScalar::EberNull == eber || CScalar::EberFalse == eber ||
			CScalar::EberNotTrue == eber);
}

// returns true iff the given expression is a Not operator whose child is an identifier
BOOL
CPredicateUtils::FNotIdent(CExpression *pexpr)
{
	return FNot(pexpr) &&
		   COperator::EopScalarIdent == (*pexpr)[0]->Pop()->Eopid();
}

// returns true iff all predicates in the given array are compatible with the given index.
BOOL
CPredicateUtils::FCompatiblePredicates(CExpressionArray *pdrspqexprPred,
									   const IMDIndex *pmdindex,
									   CColRefArray *pdrspqcrIndex,
									   CMDAccessor *md_accessor)
{
	SPQOS_ASSERT(NULL != pdrspqexprPred);
	SPQOS_ASSERT(NULL != pmdindex);

	const ULONG ulNumPreds = pdrspqexprPred->Size();
	for (ULONG ul = 0; ul < ulNumPreds; ul++)
	{
		if (!FCompatibleIndexPredicate((*pdrspqexprPred)[ul], pmdindex,
									   pdrspqcrIndex, md_accessor))
		{
			return false;
		}
	}

	return true;
}

// returns true iff the given predicate 'pexprPred' is compatible with the given index 'pmdindex'.
BOOL
CPredicateUtils::FCompatibleIndexPredicate(CExpression *pexprPred,
										   const IMDIndex *pmdindex,
										   CColRefArray *pdrspqcrIndex,
										   CMDAccessor *md_accessor)
{
	SPQOS_ASSERT(NULL != pexprPred);
	SPQOS_ASSERT(NULL != pmdindex);

	const IMDScalarOp *pmdobjScCmp = NULL;
	if (COperator::EopScalarCmp == pexprPred->Pop()->Eopid())
	{
		CScalarCmp *popScCmp = CScalarCmp::PopConvert(pexprPred->Pop());
		pmdobjScCmp = md_accessor->RetrieveScOp(popScCmp->MdIdOp());
	}
	else if (COperator::EopScalarArrayCmp == pexprPred->Pop()->Eopid())
	{
		CScalarArrayCmp *popScArrCmp =
			CScalarArrayCmp::PopConvert(pexprPred->Pop());
		pmdobjScCmp = md_accessor->RetrieveScOp(popScArrCmp->MdIdOp());
	}
	else
	{
		return false;
	}

	CExpression *pexprLeft = (*pexprPred)[0];
	CColRefSet *pcrsUsed = pexprLeft->DeriveUsedColumns();
	SPQOS_ASSERT(1 == pcrsUsed->Size());

	CColRef *pcrIndexKey = pcrsUsed->PcrFirst();
	ULONG ulKeyPos = pdrspqcrIndex->IndexOf(pcrIndexKey);
	SPQOS_ASSERT(spqos::ulong_max != ulKeyPos);

	return (pmdindex->IsCompatible(pmdobjScCmp, ulKeyPos));
}

// check if given array of expressions contain a volatile function like random().
BOOL
CPredicateUtils::FContainsVolatileFunction(CExpressionArray *pdrspqexprPred)
{
	SPQOS_ASSERT(NULL != pdrspqexprPred);

	const ULONG ulNumPreds = pdrspqexprPred->Size();
	for (ULONG ul = 0; ul < ulNumPreds; ul++)
	{
		CExpression *pexpr = (CExpression *) (*pdrspqexprPred)[ul];

		if (FContainsVolatileFunction(pexpr))
		{
			return true;
		}
	}

	return false;
}

// check if the expression contains a volatile function like random().
BOOL
CPredicateUtils::FContainsVolatileFunction(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);


	COperator *popCurrent = pexpr->Pop();
	SPQOS_ASSERT(NULL != popCurrent);

	if (COperator::EopScalarFunc == popCurrent->Eopid())
	{
		CScalarFunc *pCurrentFunction = CScalarFunc::PopConvert(popCurrent);
		return IMDFunction::EfsVolatile ==
			   pCurrentFunction->EfsGetFunctionStability();
	}

	// recursively check children
	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		BOOL isVolatile = FContainsVolatileFunction((*pexpr)[ul]);
		if (isVolatile)
		{
			return true;
		}
	}

	// cannot find any
	return false;
}

// check if the expensive CNF conversion is beneficial in finding predicate for hash join
BOOL
CPredicateUtils::FConvertToCNF(CExpression *pexprOuter, CExpression *pexprInner,
							   CExpression *pexprScalar)
{
	SPQOS_ASSERT(NULL != pexprScalar);

	if (FComparison(pexprScalar))
	{
		return CPhysicalJoin::FHashJoinCompatible(pexprScalar, pexprOuter,
												  pexprInner);
	}

	BOOL fOr = FOr(pexprScalar);
	BOOL fAllChidrenDoCNF = true;
	BOOL fExistsChildDoCNF = false;

	// recursively check children
	const ULONG arity = pexprScalar->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		BOOL fCNFConversion =
			FConvertToCNF(pexprOuter, pexprInner, (*pexprScalar)[ul]);

		if (fCNFConversion)
		{
			fExistsChildDoCNF = true;
		}
		else
		{
			fAllChidrenDoCNF = false;
		}
	}

	if (fOr)
	{
		// an OR predicate can only be beneficial is all of it
		// children state that there is a benefit for CNF conversion
		// eg: ((a1 > b1) AND (a2 > b2)) OR ((a3 == b3) AND (a4 == b4))
		// one of the OR children has no equality condition and thus when
		// we convert the expression into CNF none of then will be useful to
		// for hash join

		return fAllChidrenDoCNF;
	}
	else
	{
		// at least one one child states that CNF conversion is beneficial

		return fExistsChildDoCNF;
	}
}

// if the nth child of the given union/union all expression is also a
// union / union all expression, then collect the latter's children and
// set the input columns of the new n-ary union/unionall operator
void
CPredicateUtils::CollectGrandChildrenUnionUnionAll(
	CMemoryPool *mp, CExpression *pexpr, ULONG child_index,
	CExpressionArray *pdrspqexprResult, CColRef2dArray *pdrgdrspqcrResult)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(child_index < pexpr->Arity());
	SPQOS_ASSERT(NULL != pdrspqexprResult);
	SPQOS_ASSERT(NULL != pdrgdrspqcrResult);
	SPQOS_ASSERT(
		CPredicateUtils::FCollapsibleChildUnionUnionAll(pexpr, child_index));


	CExpression *pexprChild = (*pexpr)[child_index];
	SPQOS_ASSERT(NULL != pexprChild);

	CLogicalSetOp *pop = CLogicalSetOp::PopConvert(pexpr->Pop());
	CLogicalSetOp *popChild = CLogicalSetOp::PopConvert(pexprChild->Pop());

	// the parent setop's expected input columns and the child setop's output columns
	// may have different size or order or both. We need to ensure that the new
	// n-ary setop has the right order of the input columns from its grand children
	CColRef2dArray *pdrspqdrspqcrInput = pop->PdrspqdrspqcrInput();
	CColRefArray *pdrspqcrInputExpected = (*pdrspqdrspqcrInput)[child_index];

	const ULONG num_cols = pdrspqcrInputExpected->Size();

	CColRefArray *pdrspqcrOuputChild = popChild->PdrspqcrOutput();
	SPQOS_ASSERT(num_cols <= pdrspqcrOuputChild->Size());

	ULongPtrArray *pdrspqul = SPQOS_NEW(mp) ULongPtrArray(mp);
	for (ULONG ulColIdx = 0; ulColIdx < num_cols; ulColIdx++)
	{
		const CColRef *colref = (*pdrspqcrInputExpected)[ulColIdx];
		ULONG ulPos = pdrspqcrOuputChild->IndexOf(colref);
		SPQOS_ASSERT(spqos::ulong_max != ulPos);
		pdrspqul->Append(SPQOS_NEW(mp) ULONG(ulPos));
	}

	CColRef2dArray *pdrgdrspqcrChild = popChild->PdrspqdrspqcrInput();
	const ULONG ulArityChild = pexprChild->Arity();
	SPQOS_ASSERT(pdrgdrspqcrChild->Size() == ulArityChild);

	for (ULONG ul = 0; ul < ulArityChild; ul++)
	{
		// collect the grand child expression
		CExpression *pexprGrandchild = (*pexprChild)[ul];
		SPQOS_ASSERT(pexprGrandchild->Pop()->FLogical());

		pexprGrandchild->AddRef();
		pdrspqexprResult->Append(pexprGrandchild);

		// collect the correct input columns
		CColRefArray *pdrspqcrOld = (*pdrgdrspqcrChild)[ul];
		CColRefArray *pdrspqcrNew = SPQOS_NEW(mp) CColRefArray(mp);
		for (ULONG ulColIdx = 0; ulColIdx < num_cols; ulColIdx++)
		{
			ULONG ulPos = *(*pdrspqul)[ulColIdx];
			CColRef *colref = (*pdrspqcrOld)[ulPos];
			pdrspqcrNew->Append(colref);
		}

		pdrgdrspqcrResult->Append(pdrspqcrNew);
	}

	pdrspqul->Release();
}

// check if we can collapse the nth child of the given union / union all operator
BOOL
CPredicateUtils::FCollapsibleChildUnionUnionAll(CExpression *pexpr,
												ULONG child_index)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!CPredicateUtils::FUnionOrUnionAll(pexpr))
	{
		return false;
	}

	CExpression *pexprChild = (*pexpr)[child_index];
	SPQOS_ASSERT(NULL != pexprChild);

	// we can only collapse when the parent and child operator are of the same kind
	return (pexprChild->Pop()->Eopid() == pexpr->Pop()->Eopid());
}

// check if the input predicate is a simple predicate that can be used for bitmap
// index lookup directly without separating it's children, such as
// * A simple scalar (array) cmp between an ident and a constant (array)
// * A conjunct with children of the form:
//		+--CScalarCmp (op)
//		|--CScalarIdent
//		+--CScalarConst
// OR
//		+--CScalarArrayCmp (op)
//		|--CScalarIdent
//		+--CScalarConstArray
// OR
//		+--CScalarBoolOp (EboolopNot)
//			+--CScalarIdent (boolean type)
// OR
//		+--CScalarIdent   (boolean type)

// Note: Casted idents, constants or const arrays are allowed, such as
//		+--CScalarArrayCmp (op)
//		|--CScalarCast
//		|  +--CScalarIdent
//		+--CScalarConstArray
BOOL
CPredicateUtils::FBitmapLookupSupportedPredicateOrConjunct(
	CExpression *pexpr, CColRefSet *outer_refs)
{
	if (CPredicateUtils::FAnd(pexpr))
	{
		const ULONG ulArity = pexpr->Arity();
		BOOL result = true;
		for (ULONG ul = 0; ul < ulArity && result; ul++)
		{
			result = result &&
					 CPredicateUtils::FBitmapLookupSupportedPredicateOrConjunct(
						 (*pexpr)[ul], outer_refs);
		}

		return result;
	}

	// indexes allow ident cmp const and ident cmp outerref
	if (CPredicateUtils::FIdentCompareConstIgnoreCast(
			pexpr, COperator::EopScalarCmp) ||
		CPredicateUtils::FIdentCompareOuterRefExprIgnoreCast(pexpr,
															 outer_refs) ||
		CPredicateUtils::FArrayCompareIdentToConstIgnoreCast(pexpr) ||
		CUtils::FScalarIdentBoolType(pexpr) ||
		(!CUtils::FScalarIdentBoolType(pexpr) &&
		 CPredicateUtils::FNotIdent(pexpr)))
	{
		return true;
	}

	return false;
}

BOOL
CPredicateUtils::FBuiltInComparisonIsVeryStrict(IMDId *mdid)
{
	const CMDIdSPQDB *const pmdidSPQDB = CMDIdSPQDB::CastMdid(mdid);

	SPQOS_ASSERT(NULL != pmdidSPQDB);

	switch (pmdidSPQDB->Oid())
	{
		case SPQDB_INT2_EQ_OP:
		case SPQDB_INT2_NEQ_OP:
		case SPQDB_INT2_LT_OP:
		case SPQDB_INT2_LEQ_OP:
		case SPQDB_INT2_GT_OP:
		case SPQDB_INT2_GEQ_OP:
		case SPQDB_INT4_EQ_OP:
		case SPQDB_INT4_NEQ_OP:
		case SPQDB_INT4_LT_OP:
		case SPQDB_INT4_LEQ_OP:
		case SPQDB_INT4_GT_OP:
		case SPQDB_INT4_GEQ_OP:
		case SPQDB_INT8_EQ_OP:
		case SPQDB_INT8_NEQ_OP:
		case SPQDB_INT8_LT_OP:
		case SPQDB_INT8_LEQ_OP:
		case SPQDB_INT8_GT_OP:
		case SPQDB_INT8_GEQ_OP:
		case SPQDB_BOOL_EQ_OP:
		case SPQDB_BOOL_NEQ_OP:
		case SPQDB_BOOL_LT_OP:
		case SPQDB_BOOL_LEQ_OP:
		case SPQDB_BOOL_GT_OP:
		case SPQDB_BOOL_GEQ_OP:
		case SPQDB_TEXT_EQ_OP:
		case SPQDB_TEXT_NEQ_OP:
		case SPQDB_TEXT_LT_OP:
		case SPQDB_TEXT_LEQ_OP:
		case SPQDB_TEXT_GT_OP:
		case SPQDB_TEXT_GEQ_OP:
			// these built-in operators have well-known behavior, they always
			// return NULL when one of the operands is NULL and they
			// never return NULL when both operands are not NULL
			return true;

		default:
			// this operator might also qualify but unfortunately we can't be sure
			return false;
	}
}

BOOL
CPredicateUtils::ExprContainsOnlyStrictComparisons(CMemoryPool *mp,
												   CExpression *expr)
{
	CExpressionArray *conjuncts = PdrspqexprConjuncts(mp, expr);
	BOOL result = ExprsContainsOnlyStrictComparisons(conjuncts);
	conjuncts->Release();
	return result;
}

BOOL
CPredicateUtils::ExprsContainsOnlyStrictComparisons(CExpressionArray *conjuncts)
{
	CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();

	BOOL result = true;
	for (ULONG ul = 0; ul < conjuncts->Size(); ++ul)
	{
		CExpression *conjunct = (*conjuncts)[ul];
		if (FComparison(conjunct))
		{
			CScalarCmp *pscalarCmp = CScalarCmp::PopConvert(conjunct->Pop());
			if (!CMDAccessorUtils::FScalarOpReturnsNullOnNullInput(
					mda, pscalarCmp->MdIdOp()))
			{
				result = false;
				break;
			}
		}
		else
		{
			result = false;
			break;
		}
	}

	return result;
}

// EOF
