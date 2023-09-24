//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC CORP.
//
//	@filename:
//		CReqdPropPlan.cpp
//
//	@doc:
//		Required plan properties;
//---------------------------------------------------------------------------

#include "spqopt/base/CReqdPropPlan.h"

#include "spqos/base.h"
#include "spqos/common/CPrintablePointer.h"
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CCTEReq.h"
#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CEnfdDistribution.h"
#include "spqopt/base/CEnfdOrder.h"
#include "spqopt/base/CEnfdPartitionPropagation.h"
#include "spqopt/base/CEnfdRewindability.h"
#include "spqopt/base/CPartFilterMap.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/base/CPartInfo.h"
#include "spqopt/base/CPartitionPropagationSpec.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogical.h"
#include "spqopt/operators/CPhysical.h"
#include "spqopt/search/CGroupExpression.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::CReqdPropPlan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CReqdPropPlan::CReqdPropPlan(CColRefSet *pcrs, CEnfdOrder *peo,
							 CEnfdDistribution *ped, CEnfdRewindability *per,
							 CCTEReq *pcter)
	: m_pcrs(pcrs),
	  m_peo(peo),
	  m_ped(ped),
	  m_per(per),
	  m_pepp(NULL),
	  m_pcter(pcter)
{
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(NULL != ped);
	SPQOS_ASSERT(NULL != per);
	SPQOS_ASSERT(NULL != pcter);
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::CReqdPropPlan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CReqdPropPlan::CReqdPropPlan(CColRefSet *pcrs, CEnfdOrder *peo,
							 CEnfdDistribution *ped, CEnfdRewindability *per,
							 CEnfdPartitionPropagation *pepp, CCTEReq *pcter)
	: m_pcrs(pcrs),
	  m_peo(peo),
	  m_ped(ped),
	  m_per(per),
	  m_pepp(pepp),
	  m_pcter(pcter)
{
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(NULL != ped);
	SPQOS_ASSERT(NULL != per);
	SPQOS_ASSERT(NULL != pepp);
	SPQOS_ASSERT(NULL != pcter);
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::~CReqdPropPlan
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CReqdPropPlan::~CReqdPropPlan()
{
	CRefCount::SafeRelease(m_pcrs);
	CRefCount::SafeRelease(m_peo);
	CRefCount::SafeRelease(m_ped);
	CRefCount::SafeRelease(m_per);
	CRefCount::SafeRelease(m_pepp);
	CRefCount::SafeRelease(m_pcter);
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::ComputeReqdCols
//
//	@doc:
//		Compute required columns
//
//---------------------------------------------------------------------------
void
CReqdPropPlan::ComputeReqdCols(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   CReqdProp *prpInput, ULONG child_index,
							   CDrvdPropArray *pdrspqdpCtxt)
{
	SPQOS_ASSERT(NULL == m_pcrs);

	CReqdPropPlan *prppInput = CReqdPropPlan::Prpp(prpInput);
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	m_pcrs =
		popPhysical->PcrsRequired(mp, exprhdl, prppInput->PcrsRequired(),
								  child_index, pdrspqdpCtxt, 0 /*ulOptReq*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::ComputeReqdCTEs
//
//	@doc:
//		Compute required CTEs
//
//---------------------------------------------------------------------------
void
CReqdPropPlan::ComputeReqdCTEs(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   CReqdProp *prpInput, ULONG child_index,
							   CDrvdPropArray *pdrspqdpCtxt)
{
	SPQOS_ASSERT(NULL == m_pcter);

	CReqdPropPlan *prppInput = CReqdPropPlan::Prpp(prpInput);
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	m_pcter =
		popPhysical->PcteRequired(mp, exprhdl, prppInput->Pcter(), child_index,
								  pdrspqdpCtxt, 0 /*ulOptReq*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::Compute
//
//	@doc:
//		Compute required props
//
//---------------------------------------------------------------------------
void
CReqdPropPlan::Compute(CMemoryPool *mp, CExpressionHandle &exprhdl,
					   CReqdProp *prpInput, ULONG child_index,
					   CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
{
	SPQOS_CHECK_ABORT;

	CReqdPropPlan *prppInput = CReqdPropPlan::Prpp(prpInput);
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	ComputeReqdCols(mp, exprhdl, prpInput, child_index, pdrspqdpCtxt);
	ComputeReqdCTEs(mp, exprhdl, prpInput, child_index, pdrspqdpCtxt);
	CPartFilterMap *ppfmDerived =
		PpfmCombineDerived(mp, exprhdl, prppInput, child_index, pdrspqdpCtxt);

	ULONG ulOrderReq = 0;
	ULONG ulDistrReq = 0;
	ULONG ulRewindReq = 0;
	ULONG ulPartPropagateReq = 0;
	popPhysical->LookupRequest(ulOptReq, &ulOrderReq, &ulDistrReq, &ulRewindReq,
							   &ulPartPropagateReq);

	m_peo = SPQOS_NEW(mp) CEnfdOrder(
		popPhysical->PosRequired(mp, exprhdl, prppInput->Peo()->PosRequired(),
								 child_index, pdrspqdpCtxt, ulOrderReq),
		popPhysical->Eom(prppInput, child_index, pdrspqdpCtxt, ulOrderReq));

	m_ped = popPhysical->Ped(mp, exprhdl, prppInput, child_index, pdrspqdpCtxt,
							 ulDistrReq);

	SPQOS_ASSERT(
		CDistributionSpec::EdtUniversal != m_ped->PdsRequired()->Edt() &&
		"CDistributionSpecUniversal is a derive-only, cannot be required");

	m_per = SPQOS_NEW(mp) CEnfdRewindability(
		popPhysical->PrsRequired(mp, exprhdl, prppInput->Per()->PrsRequired(),
								 child_index, pdrspqdpCtxt, ulRewindReq),
		popPhysical->Erm(prppInput, child_index, pdrspqdpCtxt, ulRewindReq));

	m_pepp = SPQOS_NEW(mp) CEnfdPartitionPropagation(
		popPhysical->PppsRequired(mp, exprhdl,
								  prppInput->Pepp()->PppsRequired(),
								  child_index, pdrspqdpCtxt, ulPartPropagateReq),
		CEnfdPartitionPropagation::EppmSatisfy, ppfmDerived);
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::PpfmCombineDerived
//
//	@doc:
//		Combine derived part filter map from input requirements and
//		derived plan properties in the passed context
//
//---------------------------------------------------------------------------
CPartFilterMap *
CReqdPropPlan::PpfmCombineDerived(CMemoryPool *mp, CExpressionHandle &exprhdl,
								  CReqdPropPlan *prppInput, ULONG child_index,
								  CDrvdPropArray *pdrspqdpCtxt)
{
	// get partitioning info below required child
	CPartInfo *ppartinfo = exprhdl.DerivePartitionInfo(child_index);
	const ULONG ulConsumers = ppartinfo->UlConsumers();

	CPartFilterMap *ppfmDerived = SPQOS_NEW(mp) CPartFilterMap(mp);

	// a bit set of found scan id's with part filters
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp);

	// copy part filters from input requirements
	for (ULONG ul = 0; ul < ulConsumers; ul++)
	{
		ULONG scan_id = ppartinfo->ScanId(ul);
		BOOL fCopied = ppfmDerived->FCopyPartFilter(
			mp, scan_id, prppInput->Pepp()->PpfmDerived(), NULL);
		if (fCopied)
		{
#ifdef SPQOS_DEBUG
			BOOL fSet =
#endif	// SPQOS_DEBUG
				pbs->ExchangeSet(scan_id);
			SPQOS_ASSERT(!fSet);
		}
	}

	// copy part filters from previously optimized children
	const ULONG size = pdrspqdpCtxt->Size();
	for (ULONG ulDrvdProps = 0; ulDrvdProps < size; ulDrvdProps++)
	{
		CDrvdPropPlan *pdpplan =
			CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[ulDrvdProps]);
		for (ULONG ul = 0; ul < ulConsumers; ul++)
		{
			ULONG scan_id = ppartinfo->ScanId(ul);
			BOOL fFound = pbs->Get(scan_id);

			if (!fFound)
			{
				BOOL fCopied = ppfmDerived->FCopyPartFilter(
					mp, scan_id, pdpplan->Ppfm(), NULL);
				if (fCopied)
				{
#ifdef SPQOS_DEBUG
					BOOL fSet =
#endif	// SPQOS_DEBUG
						pbs->ExchangeSet(scan_id);
					SPQOS_ASSERT(!fSet);
				}
			}
		}
	}

	pbs->Release();

	return ppfmDerived;
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::InitReqdPartitionPropagation
//
//	@doc:
//		Compute hash value using required columns and required sort order
//
//---------------------------------------------------------------------------
void
CReqdPropPlan::InitReqdPartitionPropagation(CMemoryPool *mp,
											CPartInfo *ppartinfo)
{
	SPQOS_ASSERT(NULL == m_pepp &&
				"Required Partition Propagation has been initialized already");

	CPartIndexMap *ppim = SPQOS_NEW(mp) CPartIndexMap(mp);

	CEnfdPartitionPropagation::EPartitionPropagationMatching eppm =
		CEnfdPartitionPropagation::EppmSatisfy;
	for (ULONG ul = 0; ul < ppartinfo->UlConsumers(); ul++)
	{
		ULONG scan_id = ppartinfo->ScanId(ul);
		IMDId *mdid = ppartinfo->GetRelMdId(ul);
		CPartKeysArray *pdrspqpartkeys = ppartinfo->Pdrspqpartkeys(ul);
		CPartConstraint *ppartcnstr = ppartinfo->Ppartcnstr(ul);

		mdid->AddRef();
		pdrspqpartkeys->AddRef();
		ppartcnstr->AddRef();

		ppim->Insert(scan_id, SPQOS_NEW(mp) UlongToPartConstraintMap(mp),
					 CPartIndexMap::EpimConsumer,
					 0,	 //ulExpectedPropagators
					 mdid, pdrspqpartkeys, ppartcnstr);
	}

	m_pepp = SPQOS_NEW(mp) CEnfdPartitionPropagation(
		SPQOS_NEW(mp)
			CPartitionPropagationSpec(ppim, SPQOS_NEW(mp) CPartFilterMap(mp)),
		eppm,
		SPQOS_NEW(mp) CPartFilterMap(mp)	 // derived part filter map
	);
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::Pps
//
//	@doc:
//		Given a property spec type, return the corresponding property spec
//		member
//
//---------------------------------------------------------------------------
CPropSpec *
CReqdPropPlan::Pps(ULONG ul) const
{
	CPropSpec::EPropSpecType epst = (CPropSpec::EPropSpecType) ul;
	switch (epst)
	{
		case CPropSpec::EpstOrder:
			return m_peo->PosRequired();

		case CPropSpec::EpstDistribution:
			return m_ped->PdsRequired();

		case CPropSpec::EpstRewindability:
			return m_per->PrsRequired();

		case CPropSpec::EpstPartPropagation:
			if (NULL != m_pepp)
			{
				return m_pepp->PppsRequired();
			}
			return NULL;

		default:
			SPQOS_ASSERT(!"Invalid property spec index");
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::Equals
//
//	@doc:
//		Check if expression attached to handle provides required columns
//		by all plan properties
//
//---------------------------------------------------------------------------
BOOL
CReqdPropPlan::FProvidesReqdCols(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 ULONG ulOptReq) const
{
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());

	// check if operator provides required columns
	if (!popPhysical->FProvidesReqdCols(exprhdl, m_pcrs, ulOptReq))
	{
		return false;
	}

	CColRefSet *pcrsOutput = exprhdl.DeriveOutputColumns();

	// check if property spec members use columns from operator output
	BOOL fProvidesReqdCols = true;
	for (ULONG ul = 0; fProvidesReqdCols && ul < CPropSpec::EpstSentinel; ul++)
	{
		CPropSpec *pps = Pps(ul);
		if (NULL == pps)
		{
			continue;
		}

		CColRefSet *pcrsUsed = pps->PcrsUsed(mp);
		fProvidesReqdCols = pcrsOutput->ContainsAll(pcrsUsed);
		pcrsUsed->Release();
	}

	return fProvidesReqdCols;
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CReqdPropPlan::Equals(const CReqdPropPlan *prpp) const
{
	SPQOS_ASSERT(NULL != prpp);

	BOOL result = PcrsRequired()->Equals(prpp->PcrsRequired()) &&
				  Pcter()->Equals(prpp->Pcter()) &&
				  Peo()->Matches(prpp->Peo()) && Ped()->Matches(prpp->Ped()) &&
				  Per()->Matches(prpp->Per());

	if (result)
	{
		if (NULL == Pepp() || NULL == prpp->Pepp())
		{
			result = (NULL == Pepp() && NULL == prpp->Pepp());
		}
		else
		{
			result = Pepp()->Matches(prpp->Pepp());
		}
	}

	return result;
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::HashValue
//
//	@doc:
//		Compute hash value using required columns and required sort order
//
//---------------------------------------------------------------------------
ULONG
CReqdPropPlan::HashValue() const
{
	SPQOS_ASSERT(NULL != m_pcrs);
	SPQOS_ASSERT(NULL != m_peo);
	SPQOS_ASSERT(NULL != m_ped);
	SPQOS_ASSERT(NULL != m_per);
	SPQOS_ASSERT(NULL != m_pcter);

	ULONG ulHash = m_pcrs->HashValue();
	ulHash = spqos::CombineHashes(ulHash, m_peo->HashValue());
	ulHash = spqos::CombineHashes(ulHash, m_ped->HashValue());
	ulHash = spqos::CombineHashes(ulHash, m_per->HashValue());
	ulHash = spqos::CombineHashes(ulHash, m_pcter->HashValue());

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::FSatisfied
//
//	@doc:
//		Check if plan properties are satisfied by the given derived properties
//
//---------------------------------------------------------------------------
BOOL
CReqdPropPlan::FSatisfied(const CDrvdPropRelational *pdprel,
						  const CDrvdPropPlan *pdpplan) const
{
	SPQOS_ASSERT(NULL != pdprel);
	SPQOS_ASSERT(NULL != pdpplan);
	SPQOS_ASSERT(pdprel->IsComplete());

	// first, check satisfiability of relational properties
	if (!pdprel->FSatisfies(this))
	{
		return false;
	}

	// second, check satisfiability of plan properties;
	// if max cardinality <= 1, then any order requirement is already satisfied;
	// we only need to check satisfiability of distribution and rewindability
	if (pdprel->GetMaxCard().Ull() <= 1)
	{
		SPQOS_ASSERT(NULL != pdpplan->Ppim());

		return pdpplan->Pds()->FSatisfies(this->Ped()->PdsRequired()) &&
			   pdpplan->Prs()->FSatisfies(this->Per()->PrsRequired()) &&
			   pdpplan->Ppim()->FSatisfies(this->Pepp()->PppsRequired()) &&
			   pdpplan->GetCostModel()->FSatisfies(this->Pcter());
	}

	// otherwise, check satisfiability of all plan properties
	return pdpplan->FSatisfies(this);
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::FCompatible
//
//	@doc:
//		Check if plan properties are compatible with the given derived properties
//
//---------------------------------------------------------------------------
BOOL
CReqdPropPlan::FCompatible(CExpressionHandle &exprhdl, CPhysical *popPhysical,
						   const CDrvdPropRelational *pdprel,
						   const CDrvdPropPlan *pdpplan) const
{
	SPQOS_ASSERT(NULL != pdpplan);
	SPQOS_ASSERT(NULL != pdprel);

	// first, check satisfiability of relational properties, including required columns
	if (!pdprel->FSatisfies(this))
	{
		return false;
	}

	return m_peo->FCompatible(pdpplan->Pos()) &&
		   m_ped->FCompatible(pdpplan->Pds()) &&
		   m_per->FCompatible(pdpplan->Prs()) &&
		   pdpplan->Ppim()->FSatisfies(m_pepp->PppsRequired()) &&
		   popPhysical->FProvidesReqdCTEs(exprhdl, m_pcter);
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::PrppEmpty
//
//	@doc:
//		Generate empty required properties
//
//---------------------------------------------------------------------------
CReqdPropPlan *
CReqdPropPlan::PrppEmpty(CMemoryPool *mp)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CDistributionSpec *pds =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);
	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);
	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);

	return SPQOS_NEW(mp) CReqdPropPlan(pcrs, peo, ped, per, pcter);
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CReqdPropPlan::OsPrint(IOstream &os) const
{
	if (SPQOS_FTRACE(EopttracePrintRequiredColumns))
	{
		os << "req cols: [";
		if (NULL != m_pcrs)
		{
			os << (*m_pcrs);
		}
		os << "], ";
	}

	os << "req CTEs: [";
	if (NULL != m_pcter)
	{
		os << (*m_pcter);
	}

	os << "], req order: [";
	if (NULL != m_peo)
	{
		os << (*m_peo);
	}

	os << "], req dist: [";
	if (NULL != m_ped)
	{
		os << (*m_ped);
	}

	os << "], req rewind: [";
	if (NULL != m_per)
	{
		os << "], req rewind: [" << (*m_per);
	}

	os << "], req partition propagation: [";
	if (NULL != m_pepp)
	{
		os << GetPrintablePtr(m_pepp);
	}
	os << "]";

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::UlHashForCostBounding
//
//	@doc:
//		Hash function used for cost bounding
//
//---------------------------------------------------------------------------
ULONG
CReqdPropPlan::UlHashForCostBounding(const CReqdPropPlan *prpp)
{
	SPQOS_ASSERT(NULL != prpp);

	ULONG ulHash = prpp->PcrsRequired()->HashValue();

	if (NULL != prpp->Ped())
	{
		ulHash = CombineHashes(ulHash, prpp->Ped()->HashValue());
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::FEqualForCostBounding
//
//	@doc:
//		Equality function used for cost bounding
//
//---------------------------------------------------------------------------
BOOL
CReqdPropPlan::FEqualForCostBounding(const CReqdPropPlan *prppFst,
									 const CReqdPropPlan *prppSnd)
{
	SPQOS_ASSERT(NULL != prppFst);
	SPQOS_ASSERT(NULL != prppSnd);

	if (NULL == prppFst->Ped() || NULL == prppSnd->Ped())
	{
		return NULL == prppFst->Ped() && NULL == prppSnd->Ped() &&
			   prppFst->PcrsRequired()->Equals(prppSnd->PcrsRequired());
	}

	return prppFst->PcrsRequired()->Equals(prppSnd->PcrsRequired()) &&
		   prppFst->Ped()->Matches(prppSnd->Ped());
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdPropPlan::PrppRemap
//
//	@doc:
//		Map input required and derived plan properties into new required
//		plan properties for the CTE producer
//
//---------------------------------------------------------------------------
CReqdPropPlan *
CReqdPropPlan::PrppRemapForCTE(CMemoryPool *mp, CReqdPropPlan *prppInput,
							   CDrvdPropPlan *pdpplanInput,
							   UlongToColRefMap *colref_mapping)
{
	SPQOS_ASSERT(NULL != colref_mapping);
	SPQOS_ASSERT(NULL != prppInput);
	SPQOS_ASSERT(NULL != pdpplanInput);

	// Remap derived sort order to a required sort order.
	COrderSpec *pos = pdpplanInput->Pos()->PosCopyWithRemappedColumns(
		mp, colref_mapping, false /*must_exist*/);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, prppInput->Peo()->Eom());

	// Remap derived distribution only if it can be used as required distribution.
	// Also, fix distribution specs with equivalent columns, since those may come
	// from different consumers and NOT be equivalent in the producer.
	// For example:
	//     with cte as (select a,b from foo where b<10)
	//     select * from cte x1 join cte x2 on x1.a=x2.b
	// On the query side, columns x1.a and x2.b are equivalent, but we should NOT
	// treat columns a and b of the producer as equivalent.

	CDistributionSpec *pdsDerived = pdpplanInput->Pds();
	CEnfdDistribution *ped = NULL;
	if (pdsDerived->FRequirable())
	{
		CDistributionSpec *pdsNoEquiv = pdsDerived->StripEquivColumns(mp);
		CDistributionSpec *pds = pdsNoEquiv->PdsCopyWithRemappedColumns(
			mp, colref_mapping, false /*must_exist*/);
		ped = SPQOS_NEW(mp) CEnfdDistribution(pds, prppInput->Ped()->Edm());
		pdsNoEquiv->Release();
	}
	else
	{
		prppInput->Ped()->AddRef();
		ped = prppInput->Ped();
	}

	// other properties are copied from input

	prppInput->PcrsRequired()->AddRef();
	CColRefSet *pcrsRequired = prppInput->PcrsRequired();

	prppInput->Per()->AddRef();
	CEnfdRewindability *per = prppInput->Per();

	prppInput->Pepp()->AddRef();
	CEnfdPartitionPropagation *pepp = prppInput->Pepp();

	prppInput->Pcter()->AddRef();
	CCTEReq *pcter = prppInput->Pcter();

	return SPQOS_NEW(mp) CReqdPropPlan(pcrsRequired, peo, ped, per, pepp, pcter);
}


// EOF
