//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDistributionSpecRouted.cpp
//
//	@doc:
//		Specification of routed distribution
//---------------------------------------------------------------------------

#include "spqopt/base/CDistributionSpecRouted.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPhysicalMotionBroadcast.h"
#include "spqopt/operators/CPhysicalMotionRoutedDistribute.h"
#include "naucrates/traceflags/traceflags.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::CDistributionSpecRouted
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDistributionSpecRouted::CDistributionSpecRouted(CColRef *pcrSegmentId)
	: m_pcrSegmentId(pcrSegmentId)
{
	SPQOS_ASSERT(NULL != pcrSegmentId);
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::~CDistributionSpecRouted
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDistributionSpecRouted::~CDistributionSpecRouted()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::FSatisfies
//
//	@doc:
//		Check if this distribution spec satisfies the given one
//
//---------------------------------------------------------------------------
BOOL
CDistributionSpecRouted::FSatisfies(const CDistributionSpec *pds) const
{
	if (Matches(pds))
	{
		// exact match implies satisfaction
		return true;
	}

	if (EdtAny == pds->Edt())
	{
		// routed distribution satisfies the "any" distribution requirement
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::PdsCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the distribution spec with remapped columns
//
//---------------------------------------------------------------------------
CDistributionSpec *
CDistributionSpecRouted::PdsCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	ULONG id = m_pcrSegmentId->Id();
	CColRef *pcrSegmentId = colref_mapping->Find(&id);
	if (NULL == pcrSegmentId)
	{
		if (must_exist)
		{
			// not found in hashmap, so create a new colref and add to hashmap
			CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
			pcrSegmentId = col_factory->PcrCopy(m_pcrSegmentId);

#ifdef SPQOS_DEBUG
			BOOL result =
#endif	// SPQOS_DEBUG
				colref_mapping->Insert(SPQOS_NEW(mp) ULONG(id), pcrSegmentId);
			SPQOS_ASSERT(result);
		}
		else
		{
			pcrSegmentId = m_pcrSegmentId;
		}
	}

	return SPQOS_NEW(mp) CDistributionSpecRouted(pcrSegmentId);
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::AppendEnforcers
//
//	@doc:
//		Add required enforcers to dynamic array
//
//---------------------------------------------------------------------------
void
CDistributionSpecRouted::AppendEnforcers(CMemoryPool *mp,
										 CExpressionHandle &,  // exprhdl
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

	if (SPQOS_FTRACE(EopttraceDisableMotionRountedDistribute))
	{
		// routed-distribute Motion is disabled
		return;
	}

	// add a routed distribution enforcer
	AddRef();
	pexpr->AddRef();
	CExpression *pexprMotion = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CPhysicalMotionRoutedDistribute(mp, this), pexpr);
	pdrspqexpr->Append(pexprMotion);
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDistributionSpecRouted::HashValue() const
{
	return spqos::CombineHashes((ULONG) Edt(),
							   spqos::HashPtr<CColRef>(m_pcrSegmentId));
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::PcrsUsed
//
//	@doc:
//		Extract columns used by the distribution spec
//
//---------------------------------------------------------------------------
CColRefSet *
CDistributionSpecRouted::PcrsUsed(CMemoryPool *mp) const
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(m_pcrSegmentId);

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CDistributionSpecRouted::Matches(const CDistributionSpec *pds) const
{
	if (Edt() != pds->Edt())
	{
		return false;
	}

	const CDistributionSpecRouted *pdsRouted =
		CDistributionSpecRouted::PdsConvert(pds);
	return m_pcrSegmentId == pdsRouted->Pcr();
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecRouted::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CDistributionSpecRouted::OsPrint(IOstream &os) const
{
	os << "ROUTED: [ ";
	m_pcrSegmentId->OsPrint(os);
	return os << " ]";
}

// EOF
