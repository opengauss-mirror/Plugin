//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CRewindabilitySpec.cpp
//
//	@doc:
//		Specification of rewindability of intermediate query results
//---------------------------------------------------------------------------

#include "spqopt/base/CRewindabilitySpec.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPhysicalSpool.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::CRewindabilitySpec
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CRewindabilitySpec::CRewindabilitySpec(ERewindabilityType rewindability_type,
									   EMotionHazardType motion_hazard,
									   BOOL origin_nl_join)
	: m_rewindability(rewindability_type),
	  m_motion_hazard(motion_hazard),
	  m_origin_nl_join(origin_nl_join)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::~CRewindabilitySpec
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CRewindabilitySpec::~CRewindabilitySpec()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::Matches
//
//	@doc:
//		Check for equality between rewindability specs
//
//---------------------------------------------------------------------------
BOOL
CRewindabilitySpec::Matches(const CRewindabilitySpec *prs) const
{
	SPQOS_ASSERT(NULL != prs);

	return Ert() == prs->Ert() && Emht() == prs->Emht();
}


//	Check if this rewindability spec satisfies the given one
//	based on following satisfiability rules:
//
//	prs  = requested rewindability
//	this = derived rewindability
//
//	Table 1 - Rewindability satisfiability matrix:
//	  T - Satisfied
//	  F - Not satisfied; enforcer required
//	  M - Maybe satisfied; check motion hazard satisfiability (see below)
//	+-------------+------+-------------+------------+-------------+
//	|  Derive ->  | None | Rescannable | Rewindable | MarkRestore |
//	|  Required   |      |             |            |             |
//	+-------------+------+-------------+------------+-------------+
//	| None        | T    | T           | T          | T           |
//	| Rescannable | F    | M           | M          | M           |
//	| Rewindable  | F    | F           | M          | M           |
//	| MarkRestore | F    | F           | F          | M           |
//	+-------------+------+-------------+------------+-------------+
//
//	Table 2 - Motion hazard check matrix:
//	(NB: only applies to the 3 cases in the previous table):
//	+-----------+----------+--------+
//	| Derive -> | NoMotion | Motion |
//	| Required  |          |        |
//	+-----------+----------+--------+
//	| NoMotion  | T        | T      |
//	| Motion    | T        | F      |
//	+-----------+----------+--------+

BOOL
CRewindabilitySpec::FSatisfies(const CRewindabilitySpec *prs) const
{
	// ErtNone requests always satisfied (row 1 in table 1)
	if (prs->Ert() == ErtNone)
	{
		return true;
	}
	// ErtNone derived op cannot satisfy rewindable or rescannable requests
	// (rows 2-4, col 1 in table 1)
	else if (Ert() == ErtNone)
	{
		return false;
	}
	// A rewindable/MarkRestore request cannot be satisfied by a rescannable op
	// (row 3-4 col 2 in table 1)
	if (Ert() == ErtRescannable &&
		(prs->Ert() == ErtRewindable || prs->Ert() == ErtMarkRestore))
	{
		return false;
	}
	// A MarkRestore request cannot be satisified by a rewindable op
	// (row 4 col 3 in table 1)
	if (Ert() == ErtRewindable && prs->Ert() == ErtMarkRestore)
	{
		return false;
	}
	// For the rest, check motion hazard satisfiability:

	// A request to handle motion hazard cannot be satisfied by an op that
	// derived a motion hazard (row 2 col 2 in table 2)
	if (prs->HasMotionHazard() && HasMotionHazard())
	{
		return false;
	}

	// A request without motion hazard handling is satisfied. Also, a op that
	// derived no motion hazard is satisfied
	// (row 1 in table 2 & row 2 col 1 in table 2)
	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::HashValue
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
CRewindabilitySpec::HashValue() const
{
	return spqos::CombineHashes(
		spqos::HashValue<ERewindabilityType>(&m_rewindability),
		spqos::HashValue<EMotionHazardType>(&m_motion_hazard));
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::AppendEnforcers
//
//	@doc:
//		Add required enforcers to dynamic array
//
//---------------------------------------------------------------------------
void
CRewindabilitySpec::AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
									CReqdPropPlan *prpp,
									CExpressionArray *pdrspqexpr,
									CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != prpp);
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(
		this == prpp->Per()->PrsRequired() &&
		"required plan properties don't match enforced rewindability spec");

	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();

	BOOL eager = false;
	if (!SPQOS_FTRACE(EopttraceMotionHazardHandling) ||
		(prpp->Per()->PrsRequired()->HasMotionHazard() &&
		 prs->HasMotionHazard()))
	{
		// If motion hazard handling is disabled then we always want a blocking spool.
		// otherwise, create a blocking spool *only if* the request alerts about motion
		// hazard and the group expression imposes a motion hazard as well, to prevent deadlock
		eager = true;
	}

	pexpr->AddRef();
	CExpression *pexprSpool = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CPhysicalSpool(mp, eager), pexpr);
	pdrspqexpr->Append(pexprSpool);
}


//---------------------------------------------------------------------------
//	@function:
//		CRewindabilitySpec::OsPrint
//
//	@doc:
//		Print rewindability spec
//
//---------------------------------------------------------------------------
IOstream &
CRewindabilitySpec::OsPrint(IOstream &os) const
{
	switch (Ert())
	{
		case ErtRewindable:
			os << "REWINDABLE";
			break;

		case ErtRescannable:
			os << "RESCANNABLE";
			break;

		case ErtNone:
			os << "NONE";
			break;

		case ErtMarkRestore:
			os << "MARK-RESTORE";
			break;

		default:
			SPQOS_ASSERT(!"Unrecognized rewindability type");
			return os;
	}

	switch (Emht())
	{
		case EmhtMotion:
			os << " MOTION";
			break;

		case EmhtNoMotion:
			os << " NO-MOTION";
			break;

		default:
			SPQOS_ASSERT(!"Unrecognized motion hazard type");
			break;
	}

	return os;
}


// EOF
