//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CEnfdRewindability.cpp
//
//	@doc:
//		Implementation of rewindability property
//---------------------------------------------------------------------------

#include "spqopt/base/CEnfdRewindability.h"

#include "spqos/base.h"

#include "spqopt/base/CReqdPropPlan.h"
#include "spqopt/operators/CPhysicalSpool.h"


using namespace spqopt;


// initialization of static variables
const CHAR *CEnfdRewindability::m_szRewindabilityMatching[ErmSentinel] = {
	"satisfy"};


//---------------------------------------------------------------------------
//	@function:
//		CEnfdRewindability::CEnfdRewindability
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CEnfdRewindability::CEnfdRewindability(CRewindabilitySpec *prs,
									   ERewindabilityMatching erm)
	: m_prs(prs), m_erm(erm)
{
	SPQOS_ASSERT(NULL != prs);
	SPQOS_ASSERT(ErmSentinel > erm);
}


//---------------------------------------------------------------------------
//	@function:
//		CEnfdRewindability::~CEnfdRewindability
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CEnfdRewindability::~CEnfdRewindability()
{
	CRefCount::SafeRelease(m_prs);
}


//---------------------------------------------------------------------------
//	@function:
//		CEnfdRewindability::FCompatible
//
//	@doc:
//		Check if the given rewindability specification is compatible
//		with the rewindability specification of this object for the
//		specified matching type
//
//---------------------------------------------------------------------------
BOOL
CEnfdRewindability::FCompatible(CRewindabilitySpec *prs) const
{
	SPQOS_ASSERT(NULL != prs);

	switch (m_erm)
	{
		case ErmSatisfy:
			return prs->FSatisfies(m_prs);

		case ErmSentinel:
			SPQOS_ASSERT("invalid matching type");
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CEnfdRewindability::HashValue
//
//	@doc:
// 		Hash function
//
//---------------------------------------------------------------------------
ULONG
CEnfdRewindability::HashValue() const
{
	return spqos::CombineHashes(m_erm + 1, m_prs->HashValue());
}


//---------------------------------------------------------------------------
//	@function:
//		CEnfdRewindability::Epet
//
//	@doc:
// 		Get rewindability enforcing type for the given operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CEnfdRewindability::Epet(CExpressionHandle &exprhdl, CPhysical *popPhysical,
						 BOOL fRewindabilityReqd) const
{
	if (fRewindabilityReqd)
	{
		return popPhysical->EpetRewindability(exprhdl, this);
	}

	return EpetUnnecessary;
}


//---------------------------------------------------------------------------
//	@function:
//		CEnfdRewindability::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CEnfdRewindability::OsPrint(IOstream &os) const
{
	(void) m_prs->OsPrint(os);

	return os << " match: " << m_szRewindabilityMatching[m_erm];
}


// EOF
