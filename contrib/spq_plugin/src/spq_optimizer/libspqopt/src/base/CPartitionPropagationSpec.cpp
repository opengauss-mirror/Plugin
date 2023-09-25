//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPartitionPropagationSpec.cpp
//
//	@doc:
//		Specification of partition propagation requirements
//---------------------------------------------------------------------------

#include "spqopt/base/CPartitionPropagationSpec.h"

#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CPhysicalPartitionSelector.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqos;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::CPartitionPropagationSpec
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec::CPartitionPropagationSpec(CPartIndexMap *ppim,
													 CPartFilterMap *ppfm)
	: m_ppim(ppim), m_ppfm(ppfm)
{
	SPQOS_ASSERT(NULL != ppim);
	SPQOS_ASSERT(NULL != ppfm);
}


//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::~CPartitionPropagationSpec
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec::~CPartitionPropagationSpec()
{
	m_ppim->Release();
	m_ppfm->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::HashValue
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
CPartitionPropagationSpec::HashValue() const
{
	return m_ppim->HashValue();
}


//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::Matches
//
//	@doc:
//		Check whether two partition propagation specs are equal
//
//---------------------------------------------------------------------------
BOOL
CPartitionPropagationSpec::Matches(const CPartitionPropagationSpec *ppps) const
{
	return m_ppim->Equals(ppps->Ppim()) && m_ppfm->Equals(ppps->m_ppfm);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::AppendEnforcers
//
//	@doc:
//		Add required enforcers to dynamic array
//
//---------------------------------------------------------------------------
void
CPartitionPropagationSpec::AppendEnforcers(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CReqdPropPlan *
#ifdef SPQOS_DEBUG
											   prpp
#endif	// SPQOS_DEBUG
										   ,
										   CExpressionArray *pdrspqexpr,
										   CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != prpp);
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != pexpr);

	ULongPtrArray *pdrspqul = m_ppim->PdrspqulScanIds(mp);
	const ULONG size = pdrspqul->Size();

	for (ULONG ul = 0; ul < size; ul++)
	{
		ULONG scan_id = *((*pdrspqul)[ul]);
		SPQOS_ASSERT(m_ppim->Contains(scan_id));

		if (CPartIndexMap::EpimConsumer != m_ppim->Epim(scan_id) ||
			0 < m_ppim->UlExpectedPropagators(scan_id))
		{
			continue;
		}

		if (!FRequiresPartitionPropagation(mp, pexpr, exprhdl, scan_id))
		{
			continue;
		}

		CExpression *pexprResolver = NULL;

		IMDId *mdid = m_ppim->GetRelMdId(scan_id);
		CColRef2dArray *pdrspqdrspqcrKeys = NULL;
		CPartKeysArray *pdrspqpartkeys = m_ppim->Pdrspqpartkeys(scan_id);
		CPartConstraint *ppartcnstr = m_ppim->PpartcnstrRel(scan_id);
		UlongToPartConstraintMap *ppartcnstrmap =
			m_ppim->Ppartcnstrmap(scan_id);
		mdid->AddRef();
		ppartcnstr->AddRef();
		ppartcnstrmap->AddRef();
		pexpr->AddRef();

		// check if there is a predicate on this part index id
		UlongToExprMap *phmulexprEqFilter = SPQOS_NEW(mp) UlongToExprMap(mp);
		UlongToExprMap *phmulexprFilter = SPQOS_NEW(mp) UlongToExprMap(mp);
		CExpression *pexprResidual = NULL;
		if (m_ppfm->FContainsScanId(scan_id))
		{
			CExpression *pexprScalar = PexprFilter(mp, scan_id);

			// find out which keys are used in the predicate, in case there are multiple
			// keys at this point (e.g. from a union of multiple CTE consumers)
			CColRefSet *pcrsUsed = pexprScalar->DeriveUsedColumns();
			const ULONG ulKeysets = pdrspqpartkeys->Size();
			for (ULONG ulKey = 0; NULL == pdrspqdrspqcrKeys && ulKey < ulKeysets;
				 ulKey++)
			{
				// get partition key
				CPartKeys *ppartkeys = (*pdrspqpartkeys)[ulKey];
				if (ppartkeys->FOverlap(pcrsUsed))
				{
					pdrspqdrspqcrKeys = ppartkeys->Pdrspqdrspqcr();
				}
			}

			// if we cannot find partition keys mapping the partition predicates, fall back to planner
			if (NULL == pdrspqdrspqcrKeys)
			{
				SPQOS_RAISE(spqopt::ExmaSPQOPT,
						   spqopt::ExmiUnsatisfiedRequiredProperties);
			}

			pdrspqdrspqcrKeys->AddRef();

			// split predicates and put them in the appropriate hashmaps
			SplitPartPredicates(mp, pexprScalar, pdrspqdrspqcrKeys,
								phmulexprEqFilter, phmulexprFilter,
								&pexprResidual);
			pexprScalar->Release();
		}
		else
		{
			// doesn't matter which keys we use here since there is no filter
			SPQOS_ASSERT(1 <= pdrspqpartkeys->Size());
			pdrspqdrspqcrKeys = (*pdrspqpartkeys)[0]->Pdrspqdrspqcr();
			pdrspqdrspqcrKeys->AddRef();
		}

		pexprResolver = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CPhysicalPartitionSelector(
				mp, scan_id, mdid, pdrspqdrspqcrKeys, ppartcnstrmap, ppartcnstr,
				phmulexprEqFilter, phmulexprFilter, pexprResidual),
			pexpr);

		pdrspqexpr->Append(pexprResolver);
	}
	pdrspqul->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::PexprFilter
//
//	@doc:
//		Return the filter expression for the given Scan Id
//
//---------------------------------------------------------------------------
CExpression *
CPartitionPropagationSpec::PexprFilter(CMemoryPool *mp, ULONG scan_id)
{
	CExpression *pexprScalar = m_ppfm->Pexpr(scan_id);
	SPQOS_ASSERT(NULL != pexprScalar);

	if (CUtils::FScalarIdent(pexprScalar))
	{
		// condition of the form "pkey": translate into pkey = true
		pexprScalar->AddRef();
		pexprScalar = CUtils::PexprScalarEqCmp(
			mp, pexprScalar,
			CUtils::PexprScalarConstBool(mp, true /*value*/,
										 false /*is_null*/));
	}
	else if (CPredicateUtils::FNot(pexprScalar) &&
			 CUtils::FScalarIdent((*pexprScalar)[0]))
	{
		// condition of the form "!pkey": translate into pkey = false
		CExpression *pexprId = (*pexprScalar)[0];
		pexprId->AddRef();

		pexprScalar = CUtils::PexprScalarEqCmp(
			mp, pexprId,
			CUtils::PexprScalarConstBool(mp, false /*value*/,
										 false /*is_null*/));
	}
	else
	{
		pexprScalar->AddRef();
	}

	return pexprScalar;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::FRequiresPartitionPropagation
//
//	@doc:
//		Check if given part index id needs to be enforced on top of the given
//		expression
//
//---------------------------------------------------------------------------
BOOL
CPartitionPropagationSpec::FRequiresPartitionPropagation(
	CMemoryPool *mp, CExpression *pexpr, CExpressionHandle &exprhdl,
	ULONG part_idx_id) const
{
	SPQOS_ASSERT(m_ppim->Contains(part_idx_id));

	// construct partition propagation spec with the given id only, and check if it needs to be
	// enforced on top
	CPartIndexMap *ppim = SPQOS_NEW(mp) CPartIndexMap(mp);

	IMDId *mdid = m_ppim->GetRelMdId(part_idx_id);
	CPartKeysArray *pdrspqpartkeys = m_ppim->Pdrspqpartkeys(part_idx_id);
	CPartConstraint *ppartcnstr = m_ppim->PpartcnstrRel(part_idx_id);
	UlongToPartConstraintMap *ppartcnstrmap =
		m_ppim->Ppartcnstrmap(part_idx_id);
	mdid->AddRef();
	pdrspqpartkeys->AddRef();
	ppartcnstr->AddRef();
	ppartcnstrmap->AddRef();

	ppim->Insert(part_idx_id, ppartcnstrmap, m_ppim->Epim(part_idx_id),
				 m_ppim->UlExpectedPropagators(part_idx_id), mdid,
				 pdrspqpartkeys, ppartcnstr);

	CPartitionPropagationSpec *ppps = SPQOS_NEW(mp)
		CPartitionPropagationSpec(ppim, SPQOS_NEW(mp) CPartFilterMap(mp));

	CEnfdPartitionPropagation *pepp = SPQOS_NEW(mp)
		CEnfdPartitionPropagation(ppps, CEnfdPartitionPropagation::EppmSatisfy,
								  SPQOS_NEW(mp) CPartFilterMap(mp));
	CEnfdProp::EPropEnforcingType epetPartitionPropagation =
		pepp->Epet(exprhdl, CPhysical::PopConvert(pexpr->Pop()),
				   true /*fPartitionPropagationRequired*/);

	pepp->Release();

	return CEnfdProp::FEnforce(epetPartitionPropagation);
}

//---------------------------------------------------------------------------
//      @function:
//		CPartitionPropagationSpec::SplitPartPredicates
//
//	@doc:
//		Split the partition elimination predicates over the various levels
//		as well as the residual predicate and add them to the appropriate
//		hashmaps. These are to be used when creating the partition selector
//
//---------------------------------------------------------------------------
void
CPartitionPropagationSpec::SplitPartPredicates(
	CMemoryPool *mp, CExpression *pexprScalar, CColRef2dArray *pdrspqdrspqcrKeys,
	UlongToExprMap *phmulexprEqFilter,	// output
	UlongToExprMap *phmulexprFilter,	// output
	CExpression **ppexprResidual		// output
)
{
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(NULL != pdrspqdrspqcrKeys);
	SPQOS_ASSERT(NULL != phmulexprEqFilter);
	SPQOS_ASSERT(NULL != phmulexprFilter);
	SPQOS_ASSERT(NULL != ppexprResidual);
	SPQOS_ASSERT(NULL == *ppexprResidual);

	CExpressionArray *pdrspqexprConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);
	CBitSet *pbsUsed = SPQOS_NEW(mp) CBitSet(mp);
	CColRefSet *pcrsKeys = PcrsKeys(mp, pdrspqdrspqcrKeys);

	const ULONG ulLevels = pdrspqdrspqcrKeys->Size();
	for (ULONG ul = 0; ul < ulLevels; ul++)
	{
		CColRef *colref = CUtils::PcrExtractPartKey(pdrspqdrspqcrKeys, ul);
		// find conjuncts for this key and mark their positions
		CExpressionArray *pdrspqexprKey = PdrspqexprPredicatesOnKey(
			mp, pdrspqexprConjuncts, colref, pcrsKeys, &pbsUsed);
		const ULONG length = pdrspqexprKey->Size();
		if (length == 0)
		{
			// no predicates on this key
			pdrspqexprKey->Release();
			continue;
		}

		if (length == 1 && CPredicateUtils::FIdentCompare(
							   (*pdrspqexprKey)[0], IMDType::EcmptEq, colref))
		{
			// EqFilters
			// one equality predicate (key = expr); take out the expression
			// and add it to the equality filters map
			CExpression *pexprPartKey = NULL;
			CExpression *pexprOther = NULL;
			IMDType::ECmpType cmp_type = IMDType::EcmptOther;

			CPredicateUtils::ExtractComponents((*pdrspqexprKey)[0], colref,
											   &pexprPartKey, &pexprOther,
											   &cmp_type);
			SPQOS_ASSERT(NULL != pexprOther);
			pexprOther->AddRef();
#ifdef SPQOS_DEBUG
			BOOL result =
#endif	// SPQOS_DEBUG
				phmulexprEqFilter->Insert(SPQOS_NEW(mp) ULONG(ul), pexprOther);
			SPQOS_ASSERT(result);
			pdrspqexprKey->Release();
		}
		else
		{
			// Filters
			// more than one predicate on this key or one non-simple-equality predicate
#ifdef SPQOS_DEBUG
			BOOL result =
#endif	// SPQOS_DEBUG
				phmulexprFilter->Insert(
					SPQOS_NEW(mp) ULONG(ul),
					CPredicateUtils::PexprConjunction(mp, pdrspqexprKey));
			SPQOS_ASSERT(result);
			continue;
		}
	}

	(*ppexprResidual) = PexprResidualFilter(mp, pdrspqexprConjuncts, pbsUsed);

	pcrsKeys->Release();
	pdrspqexprConjuncts->Release();
	pbsUsed->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::PcrsKeys
//
//	@doc:
//		Return a colrefset containing all the part keys
//
//---------------------------------------------------------------------------
CColRefSet *
CPartitionPropagationSpec::PcrsKeys(CMemoryPool *mp,
									CColRef2dArray *pdrspqdrspqcrKeys)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	const ULONG ulLevels = pdrspqdrspqcrKeys->Size();
	for (ULONG ul = 0; ul < ulLevels; ul++)
	{
		CColRef *colref = CUtils::PcrExtractPartKey(pdrspqdrspqcrKeys, ul);
		pcrs->Include(colref);
	}

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::PexprResidualFilter
//
//	@doc:
//		Return a residual filter given an array of predicates and a bitset
//		indicating which predicates have already been used
//
//---------------------------------------------------------------------------
CExpression *
CPartitionPropagationSpec::PexprResidualFilter(CMemoryPool *mp,
											   CExpressionArray *pdrspqexpr,
											   CBitSet *pbsUsed)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != pbsUsed);

	CExpressionArray *pdrspqexprUnused = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG length = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		if (pbsUsed->Get(ul))
		{
			// predicate already considered
			continue;
		}

		CExpression *pexpr = (*pdrspqexpr)[ul];
		pexpr->AddRef();
		pdrspqexprUnused->Append(pexpr);
	}

	CExpression *pexprResult =
		CPredicateUtils::PexprConjunction(mp, pdrspqexprUnused);
	if (CUtils::FScalarConstTrue(pexprResult))
	{
		pexprResult->Release();
		pexprResult = NULL;
	}

	return pexprResult;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::PdrspqexprPredicatesOnKey
//
//	@doc:
//		Returns an array of predicates on the given partitioning key given
//		an array of predicates on all keys
//
//---------------------------------------------------------------------------
CExpressionArray *
CPartitionPropagationSpec::PdrspqexprPredicatesOnKey(CMemoryPool *mp,
													CExpressionArray *pdrspqexpr,
													CColRef *colref,
													CColRefSet *pcrsKeys,
													CBitSet **ppbs)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != colref);
	SPQOS_ASSERT(NULL != ppbs);
	SPQOS_ASSERT(NULL != *ppbs);

	CExpressionArray *pdrspqexprResult = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG length = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		if ((*ppbs)->Get(ul))
		{
			// this expression has already been added for another column
			continue;
		}

		CExpression *pexpr = (*pdrspqexpr)[ul];
		SPQOS_ASSERT(pexpr->Pop()->FScalar());

		CColRefSet *pcrsUsed = pexpr->DeriveUsedColumns();
		CColRefSet *pcrsUsedKeys = SPQOS_NEW(mp) CColRefSet(mp, *pcrsUsed);
		pcrsUsedKeys->Intersection(pcrsKeys);

		if (1 == pcrsUsedKeys->Size() && pcrsUsedKeys->FMember(colref))
		{
			pexpr->AddRef();
			pdrspqexprResult->Append(pexpr);
			(*ppbs)->ExchangeSet(ul);
		}

		pcrsUsedKeys->Release();
	}

	return pdrspqexprResult;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartitionPropagationSpec::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CPartitionPropagationSpec::OsPrint(IOstream &os) const
{
	os << *m_ppim;

	os << "Filters: [";
	m_ppfm->OsPrint(os);
	os << "]";
	return os;
}


// EOF
