//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraint.cpp
//
//	@doc:
//		Implementation of constraints
//---------------------------------------------------------------------------

#include "spqopt/base/CConstraint.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CColConstraintsArrayMapper.h"
#include "spqopt/base/CColConstraintsHashMapper.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintConjunction.h"
#include "spqopt/base/CConstraintDisjunction.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CConstraintNegation.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/base/IColConstraintsMapper.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarArrayCmp.h"
#include "spqopt/operators/CScalarCmp.h"
#include "spqopt/operators/CScalarIdent.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CConstraint);

// initialize constant true
BOOL CConstraint::m_fTrue(true);

// initialize constant false
BOOL CConstraint::m_fFalse(false);

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::CConstraint
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CConstraint::CConstraint(CMemoryPool *mp)
	: m_phmcontain(NULL), m_mp(mp), m_pcrsUsed(NULL), m_pexprScalar(NULL)
{
	m_phmcontain = SPQOS_NEW(m_mp) ConstraintContainmentMap(m_mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::~CConstraint
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CConstraint::~CConstraint()
{
	CRefCount::SafeRelease(m_pexprScalar);
	m_phmcontain->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrFromScalarArrayCmp
//
//	@doc:
//		Create constraint from scalar array comparison expression
//		originally generated for "scalar op ANY/ALL (array)" construct
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrFromScalarArrayCmp(CMemoryPool *mp, CExpression *pexpr,
									  CColRef *colref, BOOL infer_nulls_as)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CUtils::FScalarArrayCmp(pexpr));

	CScalarArrayCmp *popScArrayCmp = CScalarArrayCmp::PopConvert(pexpr->Pop());
	CScalarArrayCmp::EArrCmpType earrccmpt = popScArrayCmp->Earrcmpt();

	if ((CScalarArrayCmp::EarrcmpAny == earrccmpt ||
		 CScalarArrayCmp::EarrcmpAll == earrccmpt) &&
		(CPredicateUtils::FCompareIdentToConstArray(pexpr) ||
		 CPredicateUtils::FCompareCastIdentToConstArray(pexpr)))
	{
#ifdef SPQOS_DEBUG
		// verify column in expr is the same as column which was passed in
		CScalarIdent *popScId = NULL;
		if (CUtils::FScalarIdent((*pexpr)[0]))
		{
			popScId = CScalarIdent::PopConvert((*pexpr)[0]->Pop());
		}
		else
		{
			SPQOS_ASSERT(CScalarIdent::FCastedScId((*pexpr)[0]));
			popScId = CScalarIdent::PopConvert((*(*pexpr)[0])[0]->Pop());
		}
		SPQOS_ASSERT(colref == (CColRef *) popScId->Pcr());
#endif	// SPQOS_DEBUG

		// get comparison type
		IMDType::ECmpType cmp_type =
			CUtils::ParseCmpType(popScArrayCmp->MdIdOp());

		if (IMDType::EcmptOther == cmp_type)
		{
			// unsupported comparison operator for constraint derivation

			return NULL;
		}
		CExpression *pexprArray = CUtils::PexprScalarArrayChild(pexpr);

		const ULONG arity = CUtils::UlScalarArrayArity(pexprArray);

		// When array size exceeds the constraint derivation threshold,
		// don't expand it into a DNF and don't derive constraints
		COptimizerConfig *optimizer_config =
			COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
		ULONG array_expansion_threshold =
			optimizer_config->GetHint()->UlArrayExpansionThreshold();

		if (arity > array_expansion_threshold)
		{
			return NULL;
		}

		if (arity == 0)
		{
			if (earrccmpt == CScalarArrayCmp::EarrcmpAny)
			{
				CRangeArray *emptyRangeArray = SPQOS_NEW(mp) CRangeArray(mp);
				// comparing with an empty array for any ANY comparison produces a "false" constraint
				// which is represented by an empty CConstraintInterval
				return SPQOS_NEW(mp) CConstraintInterval(
					mp, colref, emptyRangeArray, false /*includes NULL*/);
			}
			else
			{
				// for an all comparison with an empty array, don't do further processing as we won't
				// do simplification anyway
				return NULL;
			}
		}

		CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

		for (ULONG ul = 0; ul < arity; ul++)
		{
			CScalarConst *popScConst =
				CUtils::PScalarArrayConstChildAt(pexprArray, ul);
			CConstraintInterval *pci =
				CConstraintInterval::PciIntervalFromColConstCmp(
					mp, colref, cmp_type, popScConst, infer_nulls_as);
			pdrspqcnstr->Append(pci);
		}

		if (earrccmpt == CScalarArrayCmp::EarrcmpAny)
		{
			// predicate is of the form 'A IN (1,2,3)'
			// return a disjunction of ranges {[1,1], [2,2], [3,3]}
			return SPQOS_NEW(mp) CConstraintDisjunction(mp, pdrspqcnstr);
		}

		// predicate is of the form 'A NOT IN (1,2,3)'
		// return a conjunctive negation on {[1,1], [2,2], [3,3]}
		return SPQOS_NEW(mp) CConstraintConjunction(mp, pdrspqcnstr);
	}

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrFromScalarExpr
//
//	@doc:
//		Create constraint from scalar expression and pass back any discovered
//		equivalence classes
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrFromScalarExpr(
	CMemoryPool *mp, CExpression *pexpr,
	CColRefSetArray **ppdrspqcrs,  // output equivalence classes
	BOOL infer_nulls_as)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FScalar());
	SPQOS_ASSERT(NULL != ppdrspqcrs);
	SPQOS_ASSERT(NULL == *ppdrspqcrs);

	CColRefSet *pcrs = pexpr->DeriveUsedColumns();
	ULONG num_cols = pcrs->Size();

	if (0 == num_cols)
	{
		// TODO:  - May 29, 2012: in case of an expr with no columns (e.g. 1 < 2),
		// possibly evaluate the expression, and return a "TRUE" or "FALSE" constraint
		return NULL;
	}

	if (1 == num_cols)
	{
		CColRef *colref = pcrs->PcrFirst();
		if (!CUtils::FConstrainableType(colref->RetrieveType()->MDId()))
		{
			return NULL;
		}

		CConstraint *pcnstr = NULL;

		// first, try creating a single interval constraint from the expression
		pcnstr = CConstraintInterval::PciIntervalFromScalarExpr(
			mp, pexpr, colref, infer_nulls_as);
		if (NULL == pcnstr)
		{
			// if the interval creation failed, try creating a disjunction or conjunction
			// of several interval constraints in the array case
			if (CUtils::FScalarArrayCmp(pexpr))
			{
				pcnstr =
					PcnstrFromScalarArrayCmp(mp, pexpr, colref, infer_nulls_as);
			}
			else
			{
				return PcnstrFromExistsAnySubquery(mp, pexpr, ppdrspqcrs);
			}
		}

		if (NULL != pcnstr)
		{
			*ppdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
			AddColumnToEquivClasses(mp, colref, *ppdrspqcrs);
		}
		return pcnstr;
	}

	switch (pexpr->Pop()->Eopid())
	{
		case COperator::EopScalarBoolOp:
			return PcnstrFromScalarBoolOp(mp, pexpr, ppdrspqcrs, infer_nulls_as);

		case COperator::EopScalarCmp:
			return PcnstrFromScalarCmp(mp, pexpr, ppdrspqcrs, infer_nulls_as);

		case COperator::EopScalarNAryJoinPredList:
			// return the constraints of the inner join predicates
			return PcnstrFromScalarExpr(mp, (*pexpr)[0], ppdrspqcrs,
										infer_nulls_as);

		case COperator::EopScalarSubqueryAny:
		case COperator::EopScalarSubqueryExists:
			return PcnstrFromExistsAnySubquery(mp, pexpr, ppdrspqcrs);

		default:
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrConjunction
//
//	@doc:
//		Create conjunction from array of constraints
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrConjunction(CMemoryPool *mp, CConstraintArray *pdrspqcnstr)
{
	return PcnstrConjDisj(mp, pdrspqcnstr, true /*fConj*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrDisjunction
//
//	@doc:
//		Create disjunction from array of constraints
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrDisjunction(CMemoryPool *mp, CConstraintArray *pdrspqcnstr)
{
	return PcnstrConjDisj(mp, pdrspqcnstr, false /*fConj*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrConjDisj
//
//	@doc:
//		Create conjunction/disjunction from array of constraints
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrConjDisj(CMemoryPool *mp, CConstraintArray *pdrspqcnstr,
							BOOL fConj)
{
	SPQOS_ASSERT(NULL != pdrspqcnstr);

	CConstraint *pcnstr = NULL;

	const ULONG length = pdrspqcnstr->Size();

	switch (length)
	{
		case 0:
		{
			pdrspqcnstr->Release();
			break;
		}

		case 1:
		{
			pcnstr = (*pdrspqcnstr)[0];
			pcnstr->AddRef();
			pdrspqcnstr->Release();
			break;
		}

		default:
		{
			if (fConj)
			{
				pcnstr = SPQOS_NEW(mp) CConstraintConjunction(mp, pdrspqcnstr);
			}
			else
			{
				pcnstr = SPQOS_NEW(mp) CConstraintDisjunction(mp, pdrspqcnstr);
			}
		}
	}

	return pcnstr;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::AddColumnToEquivClasses
//
//	@doc:
//		Add column as a new equivalence class, if it is not already in one of the
//		existing equivalence classes
//
//---------------------------------------------------------------------------
void
CConstraint::AddColumnToEquivClasses(CMemoryPool *mp, const CColRef *colref,
									 CColRefSetArray *ppdrspqcrs)
{
	const ULONG length = ppdrspqcrs->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRefSet *pcrs = (*ppdrspqcrs)[ul];
		if (pcrs->FMember(colref))
		{
			return;
		}
	}

	CColRefSet *pcrsNew = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsNew->Include(colref);

	ppdrspqcrs->Append(pcrsNew);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrFromScalarCmp
//
//	@doc:
//		Create constraint from scalar comparison
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrFromScalarCmp(
	CMemoryPool *mp, CExpression *pexpr,
	CColRefSetArray **ppdrspqcrs,  // output equivalence classes
	BOOL infer_nulls_as)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CUtils::FScalarCmp(pexpr));
	SPQOS_ASSERT(NULL != ppdrspqcrs);
	SPQOS_ASSERT(NULL == *ppdrspqcrs);

	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];

	// check if the scalar comparison is over scalar idents or binary coercible casted scalar idents
	if ((CUtils::FScalarIdent(pexprLeft) ||
		 CCastUtils::FBinaryCoercibleCastedScId(pexprLeft)) &&
		(CUtils::FScalarIdent(pexprRight) ||
		 CCastUtils::FBinaryCoercibleCastedScId(pexprRight)))
	{
		CScalarIdent *popScIdLeft, *popScIdRight;
		if (CUtils::FScalarIdent(pexprLeft))
		{
			// col1 = ...
			popScIdLeft = CScalarIdent::PopConvert(pexprLeft->Pop());
		}
		else
		{
			// cast(col1) = ...
			SPQOS_ASSERT(CCastUtils::FBinaryCoercibleCastedScId(pexprLeft));
			popScIdLeft = CScalarIdent::PopConvert((*pexprLeft)[0]->Pop());
		}

		if (CUtils::FScalarIdent(pexprRight))
		{
			// ... = col2
			popScIdRight = CScalarIdent::PopConvert(pexprRight->Pop());
		}
		else
		{
			// ... = cost(col2)
			SPQOS_ASSERT(CCastUtils::FBinaryCoercibleCastedScId(pexprRight));
			popScIdRight = CScalarIdent::PopConvert((*pexprRight)[0]->Pop());
		}

		const CColRef *pcrLeft = popScIdLeft->Pcr();
		const CColRef *pcrRight = popScIdRight->Pcr();

		IMDId *left_mdid = pcrLeft->RetrieveType()->MDId();
		IMDId *right_mdid = pcrRight->RetrieveType()->MDId();
		if (!CUtils::FConstrainableType(left_mdid) ||
			!CUtils::FConstrainableType(right_mdid))
		{
			return NULL;
		}

		if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
		{
			CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();
			CScalarCmp *sc_cmp = CScalarCmp::PopConvert(pexpr->Pop());
			const IMDScalarOp *op = mda->RetrieveScOp(sc_cmp->MdIdOp());

			// The left type and right type are both scalar ident types
			// In case of casting, such as
			// --CScalarCast
			//   +--CScalarIdent "a" (0)
			// We look at the data type before casting, instead of after
			// This is because equivalent classes are built with columns
			// based on their input types

			// Eg. foo.a -- varchar, bar.b -- char
			// Joining foo and bar on foo.a = bar.b requires casting
			// foo.a to bpchar. But since hash of varchar (before cast)
			// and hash of char don't belong to the same opfamily, we
			// cannot generate equivalent classes with foo.a and bar.b

			const IMDType *left_type = mda->RetrieveType(left_mdid);

			const IMDType *right_type = mda->RetrieveType(right_mdid);

			// Build constraint (e.g for equivalent classes) only when the hash families
			// of the operator and operands match.
			if (!CUtils::Equals(op->HashOpfamilyMdid(),
								left_type->GetDistrOpfamilyMdid()) ||
				!CUtils::Equals(op->HashOpfamilyMdid(),
								right_type->GetDistrOpfamilyMdid()))
			{
				return NULL;
			}
		}

		BOOL pcrLeftIncludesNull =
			infer_nulls_as && CColRef::EcrtTable == pcrLeft->Ecrt() &&
			CColRefTable::PcrConvert(const_cast<CColRef *>(pcrLeft))
				->IsNullable();
		BOOL pcrRightIncludesNull =
			infer_nulls_as && CColRef::EcrtTable == pcrRight->Ecrt() &&
			CColRefTable::PcrConvert(const_cast<CColRef *>(pcrRight))
				->IsNullable();

		*ppdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
		BOOL checkEquality = CPredicateUtils::IsEqualityOp(pexpr) &&
							 !pcrLeftIncludesNull && !pcrRightIncludesNull;
		if (checkEquality)
		{
			// col1 = col2 or bcast(col1) = col2 or col1 = bcast(col2) or bcast(col1) = bcast(col2)
			CColRefSet *pcrsNew = SPQOS_NEW(mp) CColRefSet(mp);
			pcrsNew->Include(pcrLeft);
			pcrsNew->Include(pcrRight);

			(*ppdrspqcrs)->Append(pcrsNew);
		}

		CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);
		pdrspqcnstr->Append(CConstraintInterval::PciUnbounded(
			mp, pcrLeft, pcrLeftIncludesNull /*fIncludesNull*/));
		pdrspqcnstr->Append(CConstraintInterval::PciUnbounded(
			mp, pcrRight, pcrRightIncludesNull /*fIncludesNull*/));
		return CConstraint::PcnstrConjunction(mp, pdrspqcnstr);
	}

	// TODO: , May 28, 2012; add support for other cases besides (col cmp col)

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrFromScalarBoolOp
//
//	@doc:
//		Create constraint from scalar boolean expression
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrFromScalarBoolOp(
	CMemoryPool *mp, CExpression *pexpr,
	CColRefSetArray **ppdrspqcrs,  // output equivalence classes
	BOOL infer_nulls_as)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CUtils::FScalarBoolOp(pexpr));
	SPQOS_ASSERT(NULL != ppdrspqcrs);
	SPQOS_ASSERT(NULL == *ppdrspqcrs);

	const ULONG arity = pexpr->Arity();

	// Large IN/NOT IN lists that can not be converted into
	// CScalarArrayCmp, are expanded into its disjunctive normal form,
	// represented by a large boolean expression tree.
	// For instance constructs of the form:
	// "(expression1, expression2) scalar op ANY/ALL ((const-x1,const-y1), ... (const-xn,const-yn))"
	// Deriving constraints from this is quite expensive; hence don't
	// bother when the arity of OR exceeds the threshold
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	ULONG array_expansion_threshold =
		optimizer_config->GetHint()->UlArrayExpansionThreshold();

	if (CPredicateUtils::FOr(pexpr) && arity > array_expansion_threshold)
	{
		return NULL;
	}

	*ppdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CColRefSetArray *pdrspqcrsChild = NULL;
		CConstraint *pcnstrChild = PcnstrFromScalarExpr(
			mp, (*pexpr)[ul], &pdrspqcrsChild, infer_nulls_as);
		if (NULL == pcnstrChild || pcnstrChild->IsConstraintUnbounded())
		{
			CRefCount::SafeRelease(pcnstrChild);
			CRefCount::SafeRelease(pdrspqcrsChild);
			if (CPredicateUtils::FOr(pexpr))
			{
				pdrspqcnstr->Release();
				return NULL;
			}
			continue;
		}
		SPQOS_ASSERT(NULL != pdrspqcrsChild);

		pdrspqcnstr->Append(pcnstrChild);
		CColRefSetArray *pdrspqcrsMerged =
			PdrspqcrsMergeFromBoolOp(mp, pexpr, *ppdrspqcrs, pdrspqcrsChild);

		(*ppdrspqcrs)->Release();
		*ppdrspqcrs = pdrspqcrsMerged;
		pdrspqcrsChild->Release();
	}

	const ULONG length = pdrspqcnstr->Size();
	if (0 == length)
	{
		pdrspqcnstr->Release();
		return NULL;
	}

	if (1 == length)
	{
		CConstraint *pcnstrChild = (*pdrspqcnstr)[0];
		pcnstrChild->AddRef();
		pdrspqcnstr->Release();

		if (CPredicateUtils::FNot(pexpr))
		{
			return SPQOS_NEW(mp) CConstraintNegation(mp, pcnstrChild);
		}

		return pcnstrChild;
	}

	// we know we have more than one child
	if (CPredicateUtils::FAnd(pexpr))
	{
		return SPQOS_NEW(mp) CConstraintConjunction(mp, pdrspqcnstr);
	}

	if (CPredicateUtils::FOr(pexpr))
	{
		return SPQOS_NEW(mp) CConstraintDisjunction(mp, pdrspqcnstr);
	}

	return NULL;
}

// create constraint from EXISTS/ANY scalar subquery
CConstraint *
CConstraint::PcnstrFromExistsAnySubquery(CMemoryPool *mp, CExpression *pexpr,
										 CColRefSetArray **ppdrspqcrs)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!CUtils::FCorrelatedExistsAnySubquery(pexpr))
	{
		return NULL;
	}

	CExpression *pexprRel = (*pexpr)[0];
	SPQOS_ASSERT(pexprRel->Pop()->FLogical());

	CPropConstraint *ppc = pexprRel->DerivePropertyConstraint();
	if (ppc == NULL)
	{
		return NULL;
	}

	*ppdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);
	CColRefSet *outRefs = pexprRel->DeriveOuterReferences();
	CColRefSetIter crsi(*outRefs);

	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		CColRefSet *equivOutRefs = ppc->PcrsEquivClass(colref);
		if (equivOutRefs == NULL || equivOutRefs->Size() == 0)
		{
			CRefCount::SafeRelease(equivOutRefs);
			continue;
		}
		CConstraint *cnstr4Outer = ppc->Pcnstr()->Pcnstr(mp, equivOutRefs);
		if (cnstr4Outer == NULL || cnstr4Outer->IsConstraintUnbounded())
		{
			CRefCount::SafeRelease(equivOutRefs);
			CRefCount::SafeRelease(cnstr4Outer);
			continue;
		}

		CConstraint *cnstrCol = cnstr4Outer->PcnstrRemapForColumn(mp, colref);
		pdrspqcnstr->Append(cnstrCol);
		cnstr4Outer->Release();
		AddColumnToEquivClasses(mp, colref, *ppdrspqcrs);
	}

	return CConstraint::PcnstrConjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PdrspqcrsMergeFromBoolOp
//
//	@doc:
//		Merge equivalence classes coming from children of a bool op
//
//---------------------------------------------------------------------------
CColRefSetArray *
CConstraint::PdrspqcrsMergeFromBoolOp(CMemoryPool *mp, CExpression *pexpr,
									 CColRefSetArray *pdrspqcrsFst,
									 CColRefSetArray *pdrspqcrsSnd)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CUtils::FScalarBoolOp(pexpr));
	SPQOS_ASSERT(NULL != pdrspqcrsFst);
	SPQOS_ASSERT(NULL != pdrspqcrsSnd);

	if (CPredicateUtils::FAnd(pexpr))
	{
		// merge with the equivalence classes we have so far
		return CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrsFst, pdrspqcrsSnd);
	}

	if (CPredicateUtils::FOr(pexpr))
	{
		// in case of an OR, an equivalence class must be coming from all
		// children to be part of the output
		return CUtils::PdrspqcrsIntersectEquivClasses(mp, pdrspqcrsFst,
													 pdrspqcrsSnd);
	}

	SPQOS_ASSERT(CPredicateUtils::FNot(pexpr));
	return SPQOS_NEW(mp) CColRefSetArray(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PdrspqcnstrOnColumn
//
//	@doc:
//		Return a subset of the given constraints which reference the
//		given column
//
//---------------------------------------------------------------------------
CConstraintArray *
CConstraint::PdrspqcnstrOnColumn(
	CMemoryPool *mp, CConstraintArray *pdrspqcnstr, CColRef *colref,
	BOOL fExclusive	 // returned constraints must reference ONLY the given col
)
{
	CConstraintArray *pdrspqcnstrSubset = SPQOS_NEW(mp) CConstraintArray(mp);

	const ULONG length = pdrspqcnstr->Size();

	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstr = (*pdrspqcnstr)[ul];
		CColRefSet *pcrs = pcnstr->PcrsUsed();

		// if the fExclusive flag is true, then colref must be the only column
		if (pcrs->FMember(colref) && (!fExclusive || 1 == pcrs->Size()))
		{
			pcnstr->AddRef();
			pdrspqcnstrSubset->Append(pcnstr);
		}
	}

	return pdrspqcnstrSubset;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PexprScalarConjDisj
//
//	@doc:
//		Construct a conjunction or disjunction scalar expression from an
//		array of constraints
//
//---------------------------------------------------------------------------
CExpression *
CConstraint::PexprScalarConjDisj(CMemoryPool *mp, CConstraintArray *pdrspqcnstr,
								 BOOL fConj) const
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG length = pdrspqcnstr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CExpression *pexpr = (*pdrspqcnstr)[ul]->PexprScalar(mp);
		pexpr->AddRef();
		pdrspqexpr->Append(pexpr);
	}

	if (fConj)
	{
		return CPredicateUtils::PexprConjunction(mp, pdrspqexpr);
	}

	return CPredicateUtils::PexprDisjunction(mp, pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PdrspqcnstrFlatten
//
//	@doc:
//		Flatten an array of constraints to be used as children for a conjunction
//		or disjunction. If any of these children is of the same type then use
//		its children directly instead of having multiple levels of the same type
//
//---------------------------------------------------------------------------
CConstraintArray *
CConstraint::PdrspqcnstrFlatten(CMemoryPool *mp, CConstraintArray *pdrspqcnstr,
							   EConstraintType ect) const
{
	CConstraintArray *pdrspqcnstrNew = SPQOS_NEW(mp) CConstraintArray(mp);

	const ULONG length = pdrspqcnstr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstrChild = (*pdrspqcnstr)[ul];
		EConstraintType ectChild = pcnstrChild->Ect();

		if (EctConjunction == ectChild && EctConjunction == ect)
		{
			CConstraintConjunction *pcconj =
				(CConstraintConjunction *) pcnstrChild;
			CUtils::AddRefAppend<CConstraint, CleanupRelease>(
				pdrspqcnstrNew, pcconj->Pdrspqcnstr());
		}
		else if (EctDisjunction == ectChild && EctDisjunction == ect)
		{
			CConstraintDisjunction *pcdisj =
				(CConstraintDisjunction *) pcnstrChild;
			CUtils::AddRefAppend<CConstraint, CleanupRelease>(
				pdrspqcnstrNew, pcdisj->Pdrspqcnstr());
		}
		else
		{
			pcnstrChild->AddRef();
			pdrspqcnstrNew->Append(pcnstrChild);
		}
	}

	pdrspqcnstr->Release();
	return PdrspqcnstrDeduplicate(mp, pdrspqcnstrNew, ect);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PdrspqcnstrDeduplicate
//
//	@doc:
//		Simplify an array of constraints to be used as children for a conjunction
//		or disjunction. If there are two or more elements that reference only one
//		particular column, these constraints are combined into one
//
//---------------------------------------------------------------------------
CConstraintArray *
CConstraint::PdrspqcnstrDeduplicate(CMemoryPool *mp,
								   CConstraintArray *pdrspqcnstr,
								   EConstraintType ect) const
{
	CConstraintArray *pdrspqcnstrNew = SPQOS_NEW(mp) CConstraintArray(mp);

	CAutoRef<CColRefSet> pcrsDeduped(SPQOS_NEW(mp) CColRefSet(mp));
	CAutoRef<IColConstraintsMapper> arccm;

	const ULONG length = pdrspqcnstr->Size();

	pdrspqcnstr->AddRef();
	if (length >= 5)
	{
		arccm = SPQOS_NEW(mp) CColConstraintsHashMapper(mp, pdrspqcnstr);
	}
	else
	{
		arccm = SPQOS_NEW(mp) CColConstraintsArrayMapper(mp, pdrspqcnstr);
	}

	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstrChild = (*pdrspqcnstr)[ul];
		CColRefSet *pcrs = pcnstrChild->PcrsUsed();

		SPQOS_ASSERT(0 != pcrs->Size());
		// we only simplify constraints that reference a single column, otherwise
		// we add constraint as is
		if (1 != pcrs->Size())
		{
			pcnstrChild->AddRef();
			pdrspqcnstrNew->Append(pcnstrChild);
			continue;
		}

		CColRef *colref = pcrs->PcrFirst();
		if (pcrsDeduped->FMember(colref))
		{
			// current constraint has already been combined with a previous one
			continue;
		}

		CConstraintArray *pdrspqcnstrCol = arccm->PdrgPcnstrLookup(colref);

		if (1 == pdrspqcnstrCol->Size())
		{
			// if there is only one such constraint, then no simplification
			// for this column
			pdrspqcnstrCol->Release();
			pcnstrChild->AddRef();
			pdrspqcnstrNew->Append(pcnstrChild);
			continue;
		}

		CExpression *pexpr = NULL;

		if (EctConjunction == ect)
		{
			pexpr = PexprScalarConjDisj(mp, pdrspqcnstrCol, true /*fConj*/);
		}
		else
		{
			SPQOS_ASSERT(EctDisjunction == ect);
			pexpr = PexprScalarConjDisj(mp, pdrspqcnstrCol, false /*fConj*/);
		}
		pdrspqcnstrCol->Release();
		SPQOS_ASSERT(NULL != pexpr);

		CConstraint *pcnstrNew =
			CConstraintInterval::PciIntervalFromScalarExpr(mp, pexpr, colref);
		if (NULL == pcnstrNew)
		{
			// We ran into a type conflict that prevents us from using this method to simplify the constraint.
			// Give up and return the un-flattened constraint.
			// Note that if we get here, that means that
			//   a) a single constraint
			//   b) in case of a conjunction expression, none of the constraints
			//   c) in case of a disjunction, at least one of the constraints
			// could not be converted.
			pcnstrChild->AddRef();
			pcnstrNew = pcnstrChild;
		}

		pexpr->Release();
		pdrspqcnstrNew->Append(pcnstrNew);
		pcrsDeduped->Include(colref);
	}

	pdrspqcnstr->Release();

	return pdrspqcnstrNew;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::Phmcolconstr
//
//	@doc:
//		Construct mapping between columns and arrays of constraints
//
//---------------------------------------------------------------------------
ColRefToConstraintArrayMap *
CConstraint::Phmcolconstr(CMemoryPool *mp, CColRefSet *pcrs,
						  CConstraintArray *pdrspqcnstr) const
{
	SPQOS_ASSERT(NULL != m_pcrsUsed);

	ColRefToConstraintArrayMap *phmcolconstr =
		SPQOS_NEW(mp) ColRefToConstraintArrayMap(mp);

	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		CConstraintArray *pdrspqcnstrCol =
			PdrspqcnstrOnColumn(mp, pdrspqcnstr, colref, false /*fExclusive*/);

#ifdef SPQOS_DEBUG
		BOOL fres =
#endif	//SPQOS_DEBUG
			phmcolconstr->Insert(colref, pdrspqcnstrCol);
		SPQOS_ASSERT(fres);
	}

	return phmcolconstr;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PcnstrConjDisjRemapForColumn
//
//	@doc:
//		Return a copy of the conjunction/disjunction constraint for a different column
//
//---------------------------------------------------------------------------
CConstraint *
CConstraint::PcnstrConjDisjRemapForColumn(CMemoryPool *mp, CColRef *colref,
										  CConstraintArray *pdrspqcnstr,
										  BOOL fConj) const
{
	SPQOS_ASSERT(NULL != colref);

	CConstraintArray *pdrspqcnstrNew = SPQOS_NEW(mp) CConstraintArray(mp);

	const ULONG length = pdrspqcnstr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		// clone child
		CConstraint *pcnstrChild =
			(*pdrspqcnstr)[ul]->PcnstrRemapForColumn(mp, colref);
		SPQOS_ASSERT(NULL != pcnstrChild);

		pdrspqcnstrNew->Append(pcnstrChild);
	}

	if (fConj)
	{
		return PcnstrConjunction(mp, pdrspqcnstrNew);
	}
	return PcnstrDisjunction(mp, pdrspqcnstrNew);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::Contains
//
//	@doc:
//		Does the current constraint contain the given one?
//
//---------------------------------------------------------------------------
BOOL
CConstraint::Contains(CConstraint *pcnstr)
{
	if (IsConstraintUnbounded())
	{
		return true;
	}

	if (NULL == pcnstr || pcnstr->IsConstraintUnbounded())
	{
		return false;
	}

	if (this == pcnstr)
	{
		// a constraint always contains itself
		return true;
	}

	// check if we have computed this containment query before
	BOOL *pfContains = m_phmcontain->Find(pcnstr);
	if (NULL != pfContains)
	{
		return *pfContains;
	}

	BOOL fContains = true;

	// for each column used by the current constraint, we have to make sure that
	// the constraint on this column contains the corresponding given constraint
	CColRefSetIter crsi(*m_pcrsUsed);
	while (fContains && crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		CConstraint *pcnstrColThis = Pcnstr(m_mp, colref);
		SPQOS_ASSERT(NULL != pcnstrColThis);
		CConstraint *pcnstrColOther = pcnstr->Pcnstr(m_mp, colref);

		// convert each of them to interval (if they are not already)
		CConstraintInterval *pciThis =
			CConstraintInterval::PciIntervalFromConstraint(m_mp, pcnstrColThis,
														   colref);
		CConstraintInterval *pciOther =
			CConstraintInterval::PciIntervalFromConstraint(m_mp, pcnstrColOther,
														   colref);

		fContains = pciThis->FContainsInterval(m_mp, pciOther);
		pciThis->Release();
		pciOther->Release();
		pcnstrColThis->Release();
		CRefCount::SafeRelease(pcnstrColOther);
	}

	// insert containment query into the local map
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		m_phmcontain->Insert(pcnstr, PfVal(fContains));
	SPQOS_ASSERT(fSuccess);

	return fContains;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CConstraint::Equals(CConstraint *pcnstr)
{
	if (NULL == pcnstr || pcnstr->IsConstraintUnbounded())
	{
		return IsConstraintUnbounded();
	}

	// check for pointer equality first
	if (this == pcnstr)
	{
		return true;
	}

	return (this->Contains(pcnstr) && pcnstr->Contains(this));
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraint::PrintConjunctionDisjunction
//
//	@doc:
//		Common functionality for printing conjunctions and disjunctions
//
//---------------------------------------------------------------------------
IOstream &
CConstraint::PrintConjunctionDisjunction(IOstream &os,
										 CConstraintArray *pdrspqcnstr) const
{
	EConstraintType ect = Ect();
	SPQOS_ASSERT(EctConjunction == ect || EctDisjunction == ect);

	os << "(";
	const ULONG arity = pdrspqcnstr->Size();
	(*pdrspqcnstr)[0]->OsPrint(os);

	for (ULONG ul = 1; ul < arity; ul++)
	{
		if (EctConjunction == ect)
		{
			os << " AND ";
		}
		else
		{
			os << " OR ";
		}
		(*pdrspqcnstr)[ul]->OsPrint(os);
	}
	os << ")";

	return os;
}

// EOF
