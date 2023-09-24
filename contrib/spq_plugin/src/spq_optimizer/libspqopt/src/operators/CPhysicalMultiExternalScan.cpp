//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalMultiExternalScan.cpp
//
//	@doc:
//		Implementation of external scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalMultiExternalScan.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMultiExternalScan::CPhysicalMultiExternalScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalMultiExternalScan::CPhysicalMultiExternalScan(
	CMemoryPool *mp, IMdIdArray *part_mdids, BOOL is_partial,
	CTableDescriptor *ptabdesc, ULONG ulOriginOpId, const CName *pnameAlias,
	ULONG scan_id, CColRefArray *pdrspqcrOutput,
	CColRef2dArray *pdrspqdrspqcrParts, ULONG ulSecondaryScanId,
	CPartConstraint *ppartcnstr, CPartConstraint *ppartcnstrRel)
	: CPhysicalDynamicScan(mp, is_partial, ptabdesc, ulOriginOpId, pnameAlias,
						   scan_id, pdrspqcrOutput, pdrspqdrspqcrParts,
						   ulSecondaryScanId, ppartcnstr, ppartcnstrRel),
	  m_part_mdids(part_mdids)
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

	SPQOS_ASSERT(NULL != m_part_mdids && m_part_mdids->Size() > 0);
}

CPhysicalMultiExternalScan::~CPhysicalMultiExternalScan()
{
	CRefCount::SafeRelease(m_part_mdids);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMultiExternalScan::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalMultiExternalScan::Matches(COperator *pop) const
{
	if (Eopid() != pop->Eopid())
	{
		return false;
	}

	CPhysicalMultiExternalScan *popExternalScan =
		CPhysicalMultiExternalScan::PopConvert(pop);
	return m_ptabdesc == popExternalScan->Ptabdesc() &&
		   m_pdrspqcrOutput->Equals(popExternalScan->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMultiExternalScan::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalMultiExternalScan::EpetRewindability(
	CExpressionHandle &exprhdl, const CEnfdRewindability *per) const
{
	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	if (per->FCompatible(prs))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}

// EOF
