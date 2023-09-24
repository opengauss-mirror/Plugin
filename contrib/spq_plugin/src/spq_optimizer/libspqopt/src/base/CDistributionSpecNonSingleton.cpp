//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CDistributionSpecNonSingleton.cpp
//
//	@doc:
//		Specification of non-singleton distribution
//---------------------------------------------------------------------------

#include "spqopt/base/CDistributionSpecNonSingleton.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CDistributionSpecStrictRandom.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPhysicalMotionRandom.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecNonSingleton::CDistributionSpecNonSingleton
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDistributionSpecNonSingleton::CDistributionSpecNonSingleton()
	: m_fAllowReplicated(true), m_fAllowEnforced(true)
{
}


//---------------------------------------------------------------------------
//     @function:
//             CDistributionSpecNonSingleton::CDistributionSpecNonSingleton
//
//     @doc:
//             Ctor
//
//---------------------------------------------------------------------------
CDistributionSpecNonSingleton::CDistributionSpecNonSingleton(
	BOOL fAllowReplicated, BOOL fAllowEnforced)
	: m_fAllowReplicated(fAllowReplicated), m_fAllowEnforced(fAllowEnforced)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecNonSingleton::FSatisfies
//
//	@doc:
//		Check if this distribution spec satisfies the given one
//
//---------------------------------------------------------------------------
BOOL
CDistributionSpecNonSingleton::FSatisfies(const CDistributionSpec *	 // pds
) const
{
	SPQOS_ASSERT(!"Non-Singleton distribution cannot be derived");

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecNonSingleton::AppendEnforcers
//
//	@doc:
//		Add required enforcers to dynamic array;
//		non-singleton distribution is enforced by spraying data randomly
//		on segments
//
//---------------------------------------------------------------------------
void
CDistributionSpecNonSingleton::AppendEnforcers(CMemoryPool *mp,
											   CExpressionHandle &,	 // exprhdl
											   CReqdPropPlan *
#ifdef SPQOS_DEBUG
												   prpp
#endif	// SPQOS_DEBUG
											   ,
											   CExpressionArray *pdrspqexpr,
											   CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != prpp);
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(!SPQOS_FTRACE(EopttraceDisableMotions));
	SPQOS_ASSERT(
		this == prpp->Ped()->PdsRequired() &&
		"required plan properties don't match enforced distribution spec");


	if (SPQOS_FTRACE(EopttraceDisableMotionRandom))
	{
		// random Motion is disabled
		return;
	}

	// add a random distribution enforcer
	CDistributionSpecStrictRandom *pdsrandom =
		SPQOS_NEW(mp) CDistributionSpecStrictRandom();
	pexpr->AddRef();
	CExpression *pexprMotion = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CPhysicalMotionRandom(mp, pdsrandom), pexpr);
	pdrspqexpr->Append(pexprMotion);
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecNonSingleton::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CDistributionSpecNonSingleton::OsPrint(IOstream &os) const
{
	os << "NON-SINGLETON ";
	if (!m_fAllowReplicated)
	{
		os << " (NON-REPLICATED)";
	}
	return os;
}

// EOF
