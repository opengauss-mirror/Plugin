//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software, Inc.
//
//	@filename:
//		CCastUtils.cpp
//
//	@doc:
//		Implementation of cast utility functions
//---------------------------------------------------------------------------

#include "spqopt/base/CCastUtils.h"

#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarArrayCoerceExpr.h"
#include "spqopt/operators/CScalarCast.h"
#include "spqopt/operators/CScalarIdent.h"
#include "naucrates/md/CMDArrayCoerceCastSPQDB.h"
#include "naucrates/md/IMDCast.h"

using namespace spqopt;
using namespace spqmd;

// is the given expression a binary coercible cast of a scalar identifier
BOOL
CCastUtils::FBinaryCoercibleCastedScId(CExpression *pexpr, CColRef *colref)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!FBinaryCoercibleCast(pexpr))
	{
		return false;
	}

	CExpression *pexprChild = (*pexpr)[0];

	// cast(col1)
	return COperator::EopScalarIdent == pexprChild->Pop()->Eopid() &&
		   colref == CScalarIdent::PopConvert(pexprChild->Pop())->Pcr();
}

// is the given expression a binary coercible cast of a scalar identifier
BOOL
CCastUtils::FBinaryCoercibleCastedScId(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!FBinaryCoercibleCast(pexpr))
	{
		return false;
	}

	CExpression *pexprChild = (*pexpr)[0];

	// cast(col1)
	return COperator::EopScalarIdent == pexprChild->Pop()->Eopid();
}

// is the given expression a binary coercible cast of a const
BOOL
CCastUtils::FBinaryCoercibleCastedConst(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!FBinaryCoercibleCast(pexpr))
	{
		return false;
	}

	CExpression *pexprChild = (*pexpr)[0];

	// cast(const)
	return COperator::EopScalarConst == pexprChild->Pop()->Eopid();
}

// extract the column reference if the given expression a scalar identifier
// or a cast of a scalar identifier or a function that casts a scalar identifier.
// Else return NULL.
const CColRef *
CCastUtils::PcrExtractFromScIdOrCastScId(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	BOOL fScIdent = COperator::EopScalarIdent == pexpr->Pop()->Eopid();
	BOOL fCastedScIdent = CScalarIdent::FCastedScId(pexpr);

	// col or cast(col)
	if (!fScIdent && !fCastedScIdent)
	{
		return NULL;
	}

	CScalarIdent *popScIdent = NULL;
	if (fScIdent)
	{
		popScIdent = CScalarIdent::PopConvert(pexpr->Pop());
	}
	else
	{
		SPQOS_ASSERT(fCastedScIdent);
		popScIdent = CScalarIdent::PopConvert((*pexpr)[0]->Pop());
	}

	return popScIdent->Pcr();
}

// cast the input column reference to the destination mdid
CExpression *
CCastUtils::PexprCast(CMemoryPool *mp, CMDAccessor *md_accessor,
					  const CColRef *colref, IMDId *mdid_dest)
{
	SPQOS_ASSERT(NULL != mdid_dest);

	IMDId *mdid_src = colref->RetrieveType()->MDId();
	SPQOS_ASSERT(
		CMDAccessorUtils::FCastExists(md_accessor, mdid_src, mdid_dest));

	const IMDCast *pmdcast = md_accessor->Pmdcast(mdid_src, mdid_dest);

	mdid_dest->AddRef();
	pmdcast->GetCastFuncMdId()->AddRef();
	CExpression *pexpr;

	if (pmdcast->GetMDPathType() == IMDCast::EmdtArrayCoerce)
	{
		CMDArrayCoerceCastSPQDB *parrayCoerceCast =
			(CMDArrayCoerceCastSPQDB *) pmdcast;
		pexpr = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CScalarArrayCoerceExpr(
				mp, parrayCoerceCast->GetCastFuncMdId(), mdid_dest,
				parrayCoerceCast->TypeModifier(),
				parrayCoerceCast->IsExplicit(),
				(COperator::ECoercionForm) parrayCoerceCast->GetCoercionForm(),
				parrayCoerceCast->Location()),
			CUtils::PexprScalarIdent(mp, colref));
	}
	else
	{
		CScalarCast *popCast =
			SPQOS_NEW(mp) CScalarCast(mp, mdid_dest, pmdcast->GetCastFuncMdId(),
									 pmdcast->IsBinaryCoercible());
		pexpr = SPQOS_NEW(mp)
			CExpression(mp, popCast, CUtils::PexprScalarIdent(mp, colref));
	}
	return pexpr;
}

// check whether the given expression is a binary coercible cast of something
BOOL
CCastUtils::FBinaryCoercibleCast(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	COperator *pop = pexpr->Pop();

	return FScalarCast(pexpr) &&
		   CScalarCast::PopConvert(pop)->IsBinaryCoercible();
}

BOOL
CCastUtils::FScalarCast(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	COperator *pop = pexpr->Pop();

	return COperator::EopScalarCast == pop->Eopid();
}

// return the given expression without any binary coercible casts
// that exist on the top
CExpression *
CCastUtils::PexprWithoutBinaryCoercibleCasts(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	CExpression *pexprOutput = pexpr;

	while (FBinaryCoercibleCast(pexprOutput))
	{
		SPQOS_ASSERT(1 == pexprOutput->Arity());
		pexprOutput = (*pexprOutput)[0];
	}

	return pexprOutput;
}

// add explicit casting to equality operations between compatible types
CExpressionArray *
CCastUtils::PdrspqexprCastEquality(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	CExpressionArray *pdrspqexpr =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexpr);
	CExpressionArray *pdrspqexprNew = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulPreds = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < ulPreds; ul++)
	{
		CExpression *pexprPred = (*pdrspqexpr)[ul];
		pexprPred->AddRef();
		CExpression *pexprNewPred = pexprPred;

		if (CPredicateUtils::IsEqualityOp(pexprPred) ||
			CPredicateUtils::FINDF(pexprPred))
		{
			CExpression *pexprCasted = PexprAddCast(mp, pexprPred);
			if (NULL != pexprCasted)
			{
				// release predicate since we will construct a new one
				pexprNewPred->Release();
				pexprNewPred = pexprCasted;
			}
		}
		pdrspqexprNew->Append(pexprNewPred);
	}

	pdrspqexpr->Release();

	return pdrspqexprNew;
}

// add explicit casting to left child of given equality or INDF predicate
// and return resulting casted expression;
// the function returns NULL if operation failed
CExpression *
CCastUtils::PexprAddCast(CMemoryPool *mp, CExpression *pexprPred)
{
	SPQOS_ASSERT(NULL != pexprPred);
	SPQOS_ASSERT(CUtils::FScalarCmp(pexprPred) ||
				CPredicateUtils::FINDF(pexprPred));

	CExpression *pexprChild = pexprPred;

	if (!CUtils::FScalarCmp(pexprPred))
	{
		pexprChild = (*pexprPred)[0];
	}

	CExpression *pexprLeft = (*pexprChild)[0];
	CExpression *pexprRight = (*pexprChild)[1];

	IMDId *mdid_type_left = CScalar::PopConvert(pexprLeft->Pop())->MdidType();
	IMDId *mdid_type_right = CScalar::PopConvert(pexprRight->Pop())->MdidType();

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	CExpression *pexprNewPred = NULL;

	BOOL fTypesEqual = mdid_type_left->Equals(mdid_type_right);
	BOOL fCastLtoR = CMDAccessorUtils::FCastExists(md_accessor, mdid_type_left,
												   mdid_type_right);
	BOOL fCastRtoL = CMDAccessorUtils::FCastExists(md_accessor, mdid_type_right,
												   mdid_type_left);

	if (fTypesEqual || !(fCastLtoR || fCastRtoL))
	{
		return pexprNewPred;
	}

	pexprLeft->AddRef();
	pexprRight->AddRef();

	CExpression *pexprNewLeft = pexprLeft;
	CExpression *pexprNewRight = pexprRight;

	if (fCastLtoR)
	{
		pexprNewLeft = PexprCast(mp, md_accessor, pexprLeft, mdid_type_right);
	}
	else
	{
		SPQOS_ASSERT(fCastRtoL);
		pexprNewRight = PexprCast(mp, md_accessor, pexprRight, mdid_type_left);
		;
	}

	SPQOS_ASSERT(NULL != pexprNewLeft && NULL != pexprNewRight);

	if (CUtils::FScalarCmp(pexprPred))
	{
		pexprNewPred = CUtils::PexprScalarCmp(mp, pexprNewLeft, pexprNewRight,
											  IMDType::EcmptEq);
	}
	else
	{
		pexprNewPred = CUtils::PexprINDF(mp, pexprNewLeft, pexprNewRight);
	}

	return pexprNewPred;
}

// add explicit casting on the input expression to the destination type
CExpression *
CCastUtils::PexprCast(CMemoryPool *mp, CMDAccessor *md_accessor,
					  CExpression *pexpr, IMDId *mdid_dest)
{
	IMDId *mdid_src = CScalar::PopConvert(pexpr->Pop())->MdidType();
	const IMDCast *pmdcast = md_accessor->Pmdcast(mdid_src, mdid_dest);

	mdid_dest->AddRef();
	pmdcast->GetCastFuncMdId()->AddRef();
	CExpression *pexprCast;

	if (pmdcast->GetMDPathType() == IMDCast::EmdtArrayCoerce)
	{
		CMDArrayCoerceCastSPQDB *parrayCoerceCast =
			(CMDArrayCoerceCastSPQDB *) pmdcast;
		pexprCast = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CScalarArrayCoerceExpr(
				mp, parrayCoerceCast->GetCastFuncMdId(), mdid_dest,
				parrayCoerceCast->TypeModifier(),
				parrayCoerceCast->IsExplicit(),
				(COperator::ECoercionForm) parrayCoerceCast->GetCoercionForm(),
				parrayCoerceCast->Location()),
			pexpr);
	}
	else
	{
		CScalarCast *popCast =
			SPQOS_NEW(mp) CScalarCast(mp, mdid_dest, pmdcast->GetCastFuncMdId(),
									 pmdcast->IsBinaryCoercible());
		pexprCast = SPQOS_NEW(mp) CExpression(mp, popCast, pexpr);
	}

	return pexprCast;
}

// EOF
