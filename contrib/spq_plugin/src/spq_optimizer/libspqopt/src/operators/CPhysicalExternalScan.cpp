//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalExternalScan.cpp
//
//	@doc:
//		Implementation of external scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalExternalScan.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalExternalScan::CPhysicalExternalScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalExternalScan::CPhysicalExternalScan(CMemoryPool *mp,
											 const CName *pnameAlias,
											 CTableDescriptor *ptabdesc,
											 CColRefArray *pdrspqcrOutput)
	: CPhysicalTableScan(mp, pnameAlias, ptabdesc, pdrspqcrOutput)
{
	// if this table is master only, then keep the original distribution spec.
	if (IMDRelation::EreldistrMasterOnly == ptabdesc->GetRelDistribution())
	{
		return;
	}

	// otherwise, override the distribution spec for external table
	if (m_pds)
	{
		m_pds->Release();
	}

	m_pds = SPQOS_NEW(mp) CDistributionSpecRandom();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalExternalScan::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalExternalScan::Matches(COperator *pop) const
{
	if (Eopid() != pop->Eopid())
	{
		return false;
	}

	CPhysicalExternalScan *popExternalScan =
		CPhysicalExternalScan::PopConvert(pop);
	return m_ptabdesc == popExternalScan->Ptabdesc() &&
		   m_pdrspqcrOutput->Equals(popExternalScan->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalExternalScan::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalExternalScan::EpetRewindability(CExpressionHandle &exprhdl,
										 const CEnfdRewindability *per) const
{
	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	if (per->FCompatible(prs))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}

// EOF
