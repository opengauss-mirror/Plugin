//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalRowTrigger.cpp
//
//	@doc:
//		Implementation of Physical row-level trigger operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalRowTrigger.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::CPhysicalRowTrigger
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalRowTrigger::CPhysicalRowTrigger(CMemoryPool *mp, IMDId *rel_mdid,
										 INT type, CColRefArray *pdrspqcrOld,
										 CColRefArray *pdrspqcrNew)
	: CPhysical(mp),
	  m_rel_mdid(rel_mdid),
	  m_type(type),
	  m_pdrspqcrOld(pdrspqcrOld),
	  m_pdrspqcrNew(pdrspqcrNew),
	  m_pcrsRequiredLocal(NULL)
{
	SPQOS_ASSERT(rel_mdid->IsValid());
	SPQOS_ASSERT(0 != type);
	SPQOS_ASSERT(NULL != pdrspqcrNew || NULL != pdrspqcrOld);
	SPQOS_ASSERT_IMP(NULL != pdrspqcrNew && NULL != pdrspqcrOld,
					pdrspqcrNew->Size() == pdrspqcrOld->Size());

	m_pcrsRequiredLocal = SPQOS_NEW(mp) CColRefSet(mp);
	if (NULL != m_pdrspqcrOld)
	{
		m_pcrsRequiredLocal->Include(m_pdrspqcrOld);
	}

	if (NULL != m_pdrspqcrNew)
	{
		m_pcrsRequiredLocal->Include(m_pdrspqcrNew);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::~CPhysicalRowTrigger
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalRowTrigger::~CPhysicalRowTrigger()
{
	m_rel_mdid->Release();
	CRefCount::SafeRelease(m_pdrspqcrOld);
	CRefCount::SafeRelease(m_pdrspqcrNew);
	m_pcrsRequiredLocal->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PosRequired
//
//	@doc:
//		Compute required sort columns of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalRowTrigger::PosRequired(CMemoryPool *mp,
								 CExpressionHandle &,  //exprhdl,
								 COrderSpec *,		   //posRequired,
								 ULONG
#ifdef SPQOS_DEBUG
									 child_index
#endif
								 ,
								 CDrvdPropArray *,	// pdrspqdpCtxt
								 ULONG				// ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	return SPQOS_NEW(mp) COrderSpec(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalRowTrigger::PosDerive(CMemoryPool *mp,
							   CExpressionHandle &	//exprhdl
) const
{
	return SPQOS_NEW(mp) COrderSpec(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalRowTrigger::EpetOrder(CExpressionHandle &,	 // exprhdl
							   const CEnfdOrder *
#ifdef SPQOS_DEBUG
								   peo
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	return CEnfdProp::EpetRequired;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PcrsRequired
//
//	@doc:
//		Compute required columns of the n-th child;
//		we only compute required columns for the relational child;
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalRowTrigger::PcrsRequired(CMemoryPool *mp,
								  CExpressionHandle &,	// exprhdl,
								  CColRefSet *pcrsRequired,
								  ULONG
#ifdef SPQOS_DEBUG
									  child_index
#endif	// SPQOS_DEBUG
								  ,
								  CDrvdPropArray *,	 // pdrspqdpCtxt
								  ULONG				 // ulOptReq
)
{
	SPQOS_ASSERT(0 == child_index);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, *m_pcrsRequiredLocal);
	pcrs->Union(pcrsRequired);

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalRowTrigger::PdsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CDistributionSpec *pdsInput, ULONG child_index,
								 CDrvdPropArray *,	// pdrspqdpCtxt
								 ULONG				// ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	// if expression has to execute on a single host then we need a gather
	if (exprhdl.NeedsSingletonExecution())
	{
		return PdsRequireSingleton(mp, exprhdl, pdsInput, child_index);
	}

	return SPQOS_NEW(mp) CDistributionSpecAny(this->Eopid());
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PrsRequired
//
//	@doc:
//		Compute required rewindability of the n-th child
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalRowTrigger::PrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CRewindabilitySpec *prsRequired,
								 ULONG child_index,
								 CDrvdPropArray *,	// pdrspqdpCtxt
								 ULONG				// ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	return PrsPassThru(mp, exprhdl, prsRequired, child_index);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalRowTrigger::PppsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								  CPartitionPropagationSpec *pppsRequired,
								  ULONG child_index,
								  CDrvdPropArray *,	 //pdrspqdpCtxt,
								  ULONG				 //ulOptReq
)
{
	SPQOS_ASSERT(0 == child_index);
	SPQOS_ASSERT(NULL != pppsRequired);

	return CPhysical::PppsRequiredPushThru(mp, exprhdl, pppsRequired,
										   child_index);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PcteRequired
//
//	@doc:
//		Compute required CTE map of the n-th child
//
//---------------------------------------------------------------------------
CCTEReq *
CPhysicalRowTrigger::PcteRequired(CMemoryPool *,		//mp,
								  CExpressionHandle &,	//exprhdl,
								  CCTEReq *pcter,
								  ULONG
#ifdef SPQOS_DEBUG
									  child_index
#endif
								  ,
								  CDrvdPropArray *,	 //pdrspqdpCtxt,
								  ULONG				 //ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);
	return PcterPushThru(pcter);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalRowTrigger::FProvidesReqdCols(CExpressionHandle &exprhdl,
									   CColRefSet *pcrsRequired,
									   ULONG  // ulOptReq
) const
{
	return FUnaryProvidesReqdCols(exprhdl, pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalRowTrigger::PdsDerive(CMemoryPool *,  // mp
							   CExpressionHandle &exprhdl) const
{
	return PdsDerivePassThruOuter(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::PrsDerive
//
//	@doc:
//		Derive rewindability
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalRowTrigger::PrsDerive(CMemoryPool *mp,
							   CExpressionHandle &exprhdl) const
{
	return PrsDerivePassThruOuter(mp, exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CPhysicalRowTrigger::HashValue() const
{
	ULONG ulHash =
		spqos::CombineHashes(COperator::HashValue(), m_rel_mdid->HashValue());
	ulHash = spqos::CombineHashes(ulHash, spqos::HashValue<INT>(&m_type));

	if (NULL != m_pdrspqcrOld)
	{
		ulHash =
			spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOld));
	}

	if (NULL != m_pdrspqcrNew)
	{
		ulHash =
			spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrNew));
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::Matches
//
//	@doc:
//		Match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalRowTrigger::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CPhysicalRowTrigger *popRowTrigger = CPhysicalRowTrigger::PopConvert(pop);

	CColRefArray *pdrspqcrOld = popRowTrigger->PdrspqcrOld();
	CColRefArray *pdrspqcrNew = popRowTrigger->PdrspqcrNew();

	return m_rel_mdid->Equals(popRowTrigger->GetRelMdId()) &&
		   m_type == popRowTrigger->GetType() &&
		   CUtils::Equals(m_pdrspqcrOld, pdrspqcrOld) &&
		   CUtils::Equals(m_pdrspqcrNew, pdrspqcrNew);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalRowTrigger::EpetRewindability(CExpressionHandle &exprhdl,
									   const CEnfdRewindability *per) const
{
	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	if (per->FCompatible(prs))
	{
		// required rewindability is already provided
		return CEnfdProp::EpetUnnecessary;
	}

	// always force spool to be on top of trigger
	return CEnfdProp::EpetRequired;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRowTrigger::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalRowTrigger::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " (Type: " << m_type << ")";

	if (NULL != m_pdrspqcrOld)
	{
		os << ", Old Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrOld);
		os << "]";
	}

	if (NULL != m_pdrspqcrNew)
	{
		os << ", New Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrNew);
		os << "]";
	}

	return os;
}


// EOF
