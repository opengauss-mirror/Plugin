//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalCTEConsumer.cpp
//
//	@doc:
//		Implementation of CTE consumer operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalCTEConsumer.h"

#include "spqos/base.h"

#include "spqopt/base/CCTEMap.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalCTEProducer.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::CPhysicalCTEConsumer
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalCTEConsumer::CPhysicalCTEConsumer(CMemoryPool *mp, ULONG id,
										   CColRefArray *colref_array,
										   UlongToColRefMap *colref_mapping)
	: CPhysical(mp),
	  m_id(id),
	  m_pdrspqcr(colref_array),
	  m_phmulcr(colref_mapping)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(NULL != colref_mapping);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::~CPhysicalCTEConsumer
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalCTEConsumer::~CPhysicalCTEConsumer()
{
	m_pdrspqcr->Release();
	m_phmulcr->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PcrsRequired
//
//	@doc:
//		Compute required output columns of the n-th child
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalCTEConsumer::PcrsRequired(CMemoryPool *,		 // mp,
								   CExpressionHandle &,	 // exprhdl,
								   CColRefSet *,		 // pcrsRequired,
								   ULONG,				 // child_index,
								   CDrvdPropArray *,	 // pdrspqdpCtxt
								   ULONG				 // ulOptReq
)
{
	SPQOS_ASSERT(!"CPhysicalCTEConsumer has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalCTEConsumer::PosRequired(CMemoryPool *,		// mp,
								  CExpressionHandle &,	// exprhdl,
								  COrderSpec *,			// posRequired,
								  ULONG,				// child_index,
								  CDrvdPropArray *,		// pdrspqdpCtxt
								  ULONG					// ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalCTEConsumer has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalCTEConsumer::PdsRequired(CMemoryPool *,		// mp,
								  CExpressionHandle &,	// exprhdl,
								  CDistributionSpec *,	// pdsRequired,
								  ULONG,				//child_index
								  CDrvdPropArray *,		// pdrspqdpCtxt
								  ULONG					// ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalCTEConsumer has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PrsRequired
//
//	@doc:
//		Compute required rewindability of the n-th child
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalCTEConsumer::PrsRequired(CMemoryPool *,		 // mp,
								  CExpressionHandle &,	 // exprhdl,
								  CRewindabilitySpec *,	 // prsRequired,
								  ULONG,				 // child_index,
								  CDrvdPropArray *,		 // pdrspqdpCtxt
								  ULONG					 // ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalCTEConsumer has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalCTEConsumer::PppsRequired(CMemoryPool *,				 //mp,
								   CExpressionHandle &,			 //exprhdl,
								   CPartitionPropagationSpec *,	 //pppsRequired,
								   ULONG,						 //child_index,
								   CDrvdPropArray *,			 //pdrspqdpCtxt,
								   ULONG						 //ulOptReq
)
{
	SPQOS_ASSERT(!"CPhysicalCTEConsumer has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PcteRequired
//
//	@doc:
//		Compute required CTE map of the n-th child
//
//---------------------------------------------------------------------------
CCTEReq *
CPhysicalCTEConsumer::PcteRequired(CMemoryPool *,		 //mp,
								   CExpressionHandle &,	 //exprhdl,
								   CCTEReq *,			 //pcter,
								   ULONG,				 //child_index,
								   CDrvdPropArray *,	 //pdrspqdpCtxt,
								   ULONG				 //ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalCTEConsumer has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalCTEConsumer::PosDerive(CMemoryPool *,		 // mp
								CExpressionHandle &	 //exprhdl
) const
{
	SPQOS_ASSERT(!"Unexpected call to CTE consumer order property derivation");

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalCTEConsumer::PdsDerive(CMemoryPool *,		 // mp
								CExpressionHandle &	 //exprhdl
) const
{
	SPQOS_ASSERT(
		!"Unexpected call to CTE consumer distribution property derivation");

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PrsDerive
//
//	@doc:
//		Derive rewindability
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalCTEConsumer::PrsDerive(CMemoryPool *,		 //mp
								CExpressionHandle &	 //exprhdl
) const
{
	SPQOS_ASSERT(
		!"Unexpected call to CTE consumer rewindability property derivation");

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::PcmDerive
//
//	@doc:
//		Derive cte map
//
//---------------------------------------------------------------------------
CCTEMap *
CPhysicalCTEConsumer::PcmDerive(CMemoryPool *mp, CExpressionHandle &
#ifdef SPQOS_DEBUG
													 exprhdl
#endif
) const
{
	SPQOS_ASSERT(0 == exprhdl.Arity());

	CCTEMap *pcmConsumer = SPQOS_NEW(mp) CCTEMap(mp);
	pcmConsumer->Insert(m_id, CCTEMap::EctConsumer, NULL /*pdpplan*/);

	return pcmConsumer;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalCTEConsumer::FProvidesReqdCols(CExpressionHandle &exprhdl,
										CColRefSet *pcrsRequired,
										ULONG  // ulOptReq
) const
{
	SPQOS_ASSERT(NULL != pcrsRequired);

	CColRefSet *pcrsOutput = exprhdl.DeriveOutputColumns();
	return pcrsOutput->ContainsAll(pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalCTEConsumer::EpetOrder(CExpressionHandle &exprhdl,
								const CEnfdOrder *peo) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	COrderSpec *pos = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pos();
	if (peo->FCompatible(pos))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalCTEConsumer::EpetRewindability(CExpressionHandle &exprhdl,
										const CEnfdRewindability *per) const
{
	SPQOS_ASSERT(NULL != per);

	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	if (per->FCompatible(prs))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CPhysicalCTEConsumer::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CPhysicalCTEConsumer *popCTEConsumer =
		CPhysicalCTEConsumer::PopConvert(pop);

	return m_id == popCTEConsumer->UlCTEId() &&
		   m_pdrspqcr->Equals(popCTEConsumer->Pdrspqcr());
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CPhysicalCTEConsumer::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(), m_id);
	ulHash = spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcr));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalCTEConsumer::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalCTEConsumer::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << m_id;
	os << "), Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcr);
	os << "]";

	return os;
}

// EOF
