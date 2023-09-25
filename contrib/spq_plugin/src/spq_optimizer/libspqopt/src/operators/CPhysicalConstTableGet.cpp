//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalConstTableGet.cpp
//
//	@doc:
//		Implementation of physical const table get operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalConstTableGet.h"

#include "spqos/base.h"

#include "spqopt/base/CCTEMap.h"
#include "spqopt/base/CDistributionSpecUniversal.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::CPhysicalConstTableGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalConstTableGet::CPhysicalConstTableGet(
	CMemoryPool *mp, CColumnDescriptorArray *pdrspqcoldesc,
	IDatum2dArray *pdrspqdrspqdatum, CColRefArray *pdrspqcrOutput)
	: CPhysical(mp),
	  m_pdrspqcoldesc(pdrspqcoldesc),
	  m_pdrspqdrspqdatum(pdrspqdrspqdatum),
	  m_pdrspqcrOutput(pdrspqcrOutput)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::~CPhysicalConstTableGet
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalConstTableGet::~CPhysicalConstTableGet()
{
	m_pdrspqcoldesc->Release();
	m_pdrspqdrspqdatum->Release();
	m_pdrspqcrOutput->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::Matches
//
//	@doc:
//		Match operators
//
//---------------------------------------------------------------------------
BOOL
CPhysicalConstTableGet::Matches(COperator *pop) const
{
	if (Eopid() == pop->Eopid())
	{
		CPhysicalConstTableGet *popCTG =
			CPhysicalConstTableGet::PopConvert(pop);
		return m_pdrspqcoldesc == popCTG->Pdrspqcoldesc() &&
			   m_pdrspqdrspqdatum == popCTG->Pdrspqdrspqdatum() &&
			   m_pdrspqcrOutput == popCTG->PdrspqcrOutput();
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PcrsRequired
//
//	@doc:
//		Compute required columns of the n-th child;
//		we only compute required columns for the relational child;
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalConstTableGet::PcrsRequired(CMemoryPool *,		   // mp,
									 CExpressionHandle &,  // exprhdl,
									 CColRefSet *,		   // pcrsRequired,
									 ULONG,				   // child_index,
									 CDrvdPropArray *,	   // pdrspqdpCtxt
									 ULONG				   // ulOptReq
)
{
	SPQOS_ASSERT(!"CPhysicalConstTableGet has no children");
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalConstTableGet::PosRequired(CMemoryPool *,		  // mp,
									CExpressionHandle &,  // exprhdl,
									COrderSpec *,		  // posRequired,
									ULONG,				  // child_index,
									CDrvdPropArray *,	  // pdrspqdpCtxt
									ULONG				  // ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalConstTableGet has no children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalConstTableGet::PdsRequired(CMemoryPool *,		  // mp,
									CExpressionHandle &,  // exprhdl,
									CDistributionSpec *,  // pdsRequired,
									ULONG,				  //child_index
									CDrvdPropArray *,	  // pdrspqdpCtxt
									ULONG				  // ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalConstTableGet has no children");
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PrsRequired
//
//	@doc:
//		Compute required rewindability of the n-th child
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalConstTableGet::PrsRequired(CMemoryPool *,		   // mp,
									CExpressionHandle &,   // exprhdl,
									CRewindabilitySpec *,  // prsRequired,
									ULONG,				   // child_index,
									CDrvdPropArray *,	   // pdrspqdpCtxt
									ULONG				   // ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalConstTableGet has no children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PcteRequired
//
//	@doc:
//		Compute required CTE map of the n-th child
//
//---------------------------------------------------------------------------
CCTEReq *
CPhysicalConstTableGet::PcteRequired(CMemoryPool *,		   //mp,
									 CExpressionHandle &,  //exprhdl,
									 CCTEReq *,			   //pcter,
									 ULONG,				   //child_index,
									 CDrvdPropArray *,	   //pdrspqdpCtxt,
									 ULONG				   //ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalConstTableGet has no children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalConstTableGet::FProvidesReqdCols(CExpressionHandle &,	// exprhdl,
										  CColRefSet *pcrsRequired,
										  ULONG	 // ulOptReq
) const
{
	SPQOS_ASSERT(NULL != pcrsRequired);

	CColRefSet *pcrs = SPQOS_NEW(m_mp) CColRefSet(m_mp);
	pcrs->Include(m_pdrspqcrOutput);

	BOOL result = pcrs->ContainsAll(pcrsRequired);

	pcrs->Release();

	return result;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalConstTableGet::PosDerive(CMemoryPool *mp,
								  CExpressionHandle &  // exprhdl
) const
{
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalConstTableGet::PdsDerive(CMemoryPool *mp,
								  CExpressionHandle &  // exprhdl
) const
{
	return SPQOS_NEW(mp) CDistributionSpecUniversal();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PrsDerive
//
//	@doc:
//		Derive rewindability
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalConstTableGet::PrsDerive(CMemoryPool *mp,
								  CExpressionHandle &  // exprhdl
) const
{
	return SPQOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtMarkRestore,
										   CRewindabilitySpec::EmhtNoMotion);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::PcmDerive
//
//	@doc:
//		Derive cte map
//
//---------------------------------------------------------------------------
CCTEMap *
CPhysicalConstTableGet::PcmDerive(CMemoryPool *mp,
								  CExpressionHandle &  //exprhdl
) const
{
	return SPQOS_NEW(mp) CCTEMap(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalConstTableGet::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalConstTableGet::EpetOrder(CExpressionHandle &,	// exprhdl
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
//		CPhysicalConstTableGet::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalConstTableGet::EpetRewindability(CExpressionHandle &,		  // exprhdl
										  const CEnfdRewindability *  // per
) const
{
	// rewindability is already provided
	return CEnfdProp::EpetUnnecessary;
}

// print values in const table
IOstream &
CPhysicalConstTableGet::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}
	else
	{
		os << SzId() << " ";
		os << "Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
		os << "] ";
		os << "Values: [";
		for (ULONG ulA = 0; ulA < m_pdrspqdrspqdatum->Size(); ulA++)
		{
			if (0 < ulA)
			{
				os << "; ";
			}
			os << "(";
			IDatumArray *pdrspqdatum = (*m_pdrspqdrspqdatum)[ulA];

			const ULONG length = pdrspqdatum->Size();
			for (ULONG ulB = 0; ulB < length; ulB++)
			{
				IDatum *datum = (*pdrspqdatum)[ulB];
				datum->OsPrint(os);

				if (ulB < length - 1)
				{
					os << ", ";
				}
			}
			os << ")";
		}
		os << "]";
	}

	return os;
}


// EOF
