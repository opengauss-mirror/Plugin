//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CLogicalProject.cpp
//
//	@doc:
//		Implementation of project operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalProject.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CDefaultComparator.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CScalarIdent.h"
#include "spqopt/operators/CScalarProjectElement.h"

using namespace spqopt;
using namespace spqnaucrates;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::CLogicalProject
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalProject::CLogicalProject(CMemoryPool *mp) : CLogicalUnary(mp)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalProject::DeriveOutputColumns(CMemoryPool *mp,
									 CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(2 == exprhdl.Arity());

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// the scalar child defines additional columns
	pcrs->Union(exprhdl.DeriveOutputColumns(0));
	pcrs->Union(exprhdl.DeriveDefinedColumns(1));

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalProject::DeriveKeyCollection(CMemoryPool *,	 // mp
									 CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::PdrspqcrsEquivClassFromScIdent
//
//	@doc:
//		Return equivalence class from scalar ident project element
//
//---------------------------------------------------------------------------
CColRefSetArray *
CLogicalProject::PdrspqcrsEquivClassFromScIdent(CMemoryPool *mp,
											   CExpression *pexprPrEl,
											   CColRefSet *not_null_columns)
{
	SPQOS_ASSERT(NULL != pexprPrEl);

	CScalarProjectElement *popPrEl =
		CScalarProjectElement::PopConvert(pexprPrEl->Pop());
	CColRef *pcrPrEl = popPrEl->Pcr();
	CExpression *pexprScalar = (*pexprPrEl)[0];


	if (EopScalarIdent != pexprScalar->Pop()->Eopid())
	{
		return NULL;
	}

	CScalarIdent *popScIdent = CScalarIdent::PopConvert(pexprScalar->Pop());
	const CColRef *pcrScIdent = popScIdent->Pcr();
	SPQOS_ASSERT(pcrPrEl->Id() != pcrScIdent->Id());
	SPQOS_ASSERT(pcrPrEl->RetrieveType()->MDId()->Equals(
		pcrScIdent->RetrieveType()->MDId()));

	if (!CUtils::FConstrainableType(pcrPrEl->RetrieveType()->MDId()))
	{
		return NULL;
	}

	BOOL non_nullable = not_null_columns->FMember(pcrScIdent);

	// only add renamed columns to equivalent class if the column is not null-able
	// this is because equality predicates will be inferred from the equivalent class
	// during preprocessing
	if (CColRef::EcrtTable == pcrScIdent->Ecrt() && non_nullable)
	{
		// equivalence class
		CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

		CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
		pcrs->Include(pcrPrEl);
		pcrs->Include(pcrScIdent);
		pdrspqcrs->Append(pcrs);

		return pdrspqcrs;
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::ExtractConstraintFromScConst
//
//	@doc:
//		Extract constraint from scalar constant project element
//
//---------------------------------------------------------------------------
void
CLogicalProject::ExtractConstraintFromScConst(
	CMemoryPool *mp, CExpression *pexprPrEl,
	CConstraintArray *pdrspqcnstr,  // array of range constraints
	CColRefSetArray *pdrspqcrs	   // array of equivalence class
)
{
	SPQOS_ASSERT(NULL != pexprPrEl);
	SPQOS_ASSERT(NULL != pdrspqcnstr);
	SPQOS_ASSERT(NULL != pdrspqcrs);

	CScalarProjectElement *popPrEl =
		CScalarProjectElement::PopConvert(pexprPrEl->Pop());
	CColRef *colref = popPrEl->Pcr();
	CExpression *pexprScalar = (*pexprPrEl)[0];

	IMDId *mdid_type = colref->RetrieveType()->MDId();

	if (EopScalarConst != pexprScalar->Pop()->Eopid() ||
		!CUtils::FConstrainableType(mdid_type))
	{
		return;
	}

	CScalarConst *popConst = CScalarConst::PopConvert(pexprScalar->Pop());
	IDatum *datum = popConst->GetDatum();

	CRangeArray *pdrspqrng = SPQOS_NEW(mp) CRangeArray(mp);
	BOOL is_null = datum->IsNull();
	if (!is_null)
	{
		datum->AddRef();
		pdrspqrng->Append(SPQOS_NEW(mp) CRange(COptCtxt::PoctxtFromTLS()->Pcomp(),
											 IMDType::EcmptEq, datum));
	}

	pdrspqcnstr->Append(SPQOS_NEW(mp)
						   CConstraintInterval(mp, colref, pdrspqrng, is_null));

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(colref);
	pdrspqcrs->Append(pcrs);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::DerivePropertyConstraint
//
//	@doc:
//		Derive constraint property
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalProject::DerivePropertyConstraint(CMemoryPool *mp,
										  CExpressionHandle &exprhdl) const
{
	CExpression *pexprPrL = exprhdl.PexprScalarExactChild(1);

	if (NULL == pexprPrL)
	{
		return PpcDeriveConstraintPassThru(exprhdl, 0 /*ulChild*/);
	}

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);
	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	const ULONG ulProjElems = pexprPrL->Arity();
	for (ULONG ul = 0; ul < ulProjElems; ul++)
	{
		CExpression *pexprPrEl = (*pexprPrL)[ul];
		CExpression *pexprProjected = (*pexprPrEl)[0];

		if (EopScalarConst == pexprProjected->Pop()->Eopid())
		{
			ExtractConstraintFromScConst(mp, pexprPrEl, pdrspqcnstr, pdrspqcrs);
		}
		else
		{
			CColRefSet *not_null_columns =
				exprhdl.DeriveNotNullColumns(0 /*ulChild*/);
			CColRefSetArray *pdrspqcrsChild =
				PdrspqcrsEquivClassFromScIdent(mp, pexprPrEl, not_null_columns);

			if (NULL != pdrspqcrsChild)
			{
				// merge with the equivalence classes we have so far
				CColRefSetArray *pdrspqcrsMerged =
					CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs,
													  pdrspqcrsChild);

				// clean up
				pdrspqcrs->Release();
				pdrspqcrsChild->Release();

				pdrspqcrs = pdrspqcrsMerged;
			}
		}
	}

	if (0 == pdrspqcnstr->Size() && 0 == pdrspqcrs->Size())
	{
		// no constants or equivalence classes found, so just return the same constraint property of the child
		pdrspqcnstr->Release();
		pdrspqcrs->Release();
		return PpcDeriveConstraintPassThru(exprhdl, 0 /*ulChild*/);
	}

	CPropConstraint *ppcChild =
		exprhdl.DerivePropertyConstraint(0 /* ulChild */);

	// equivalence classes coming from child
	CColRefSetArray *pdrspqcrsChild = ppcChild->PdrspqcrsEquivClasses();
	if (NULL != pdrspqcrsChild)
	{
		// merge with the equivalence classes we have so far
		CColRefSetArray *pdrspqcrsMerged =
			CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs, pdrspqcrsChild);

		// clean up
		pdrspqcrs->Release();
		pdrspqcrs = pdrspqcrsMerged;
	}

	// constraint coming from child
	CConstraint *pcnstr = ppcChild->Pcnstr();
	if (NULL != pcnstr)
	{
		pcnstr->AddRef();
		pdrspqcnstr->Append(pcnstr);
	}

	CConstraint *pcnstrNew = CConstraint::PcnstrConjunction(mp, pdrspqcnstr);

	return SPQOS_NEW(mp) CPropConstraint(mp, pdrspqcrs, pcnstrNew);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalProject::DeriveMaxCard(CMemoryPool *,  // mp
							   CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasNonScalarFunction(1))
	{
		// unbounded by default
		return CMaxCard();
	}

	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalProject::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfSimplifyProjectWithSubquery);
	(void) xform_set->ExchangeSet(CXform::ExfProject2Apply);
	(void) xform_set->ExchangeSet(CXform::ExfProject2ComputeScalar);
	(void) xform_set->ExchangeSet(CXform::ExfCollapseProject);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalProject::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalProject::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  IStatisticsArray *  // stats_ctxt
) const
{
	UlongToIDatumMap *phmuldatum = SPQOS_NEW(mp) UlongToIDatumMap(mp);

	// extract scalar constant expression that can be used for
	// statistics calculation
	CExpression *pexprPrList = exprhdl.PexprScalarRepChild(1 /*child_index*/);
	const ULONG arity = pexprPrList->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprPrElem = (*pexprPrList)[ul];
		SPQOS_ASSERT(1 == pexprPrElem->Arity());
		CColRef *colref =
			CScalarProjectElement::PopConvert(pexprPrElem->Pop())->Pcr();

		CExpression *pexprScalar = (*pexprPrElem)[0];
		COperator *pop = pexprScalar->Pop();
		if (COperator::EopScalarConst == pop->Eopid())
		{
			IDatum *datum = CScalarConst::PopConvert(pop)->GetDatum();
			if (datum->StatsMappable())
			{
				datum->AddRef();
#ifdef SPQOS_DEBUG
				BOOL fInserted =
#endif
					phmuldatum->Insert(SPQOS_NEW(mp) ULONG(colref->Id()), datum);
				SPQOS_ASSERT(fInserted);
			}
		}
	}

	IStatistics *stats = PstatsDeriveProject(mp, exprhdl, phmuldatum);

	// clean up
	phmuldatum->Release();

	return stats;
}


// EOF
