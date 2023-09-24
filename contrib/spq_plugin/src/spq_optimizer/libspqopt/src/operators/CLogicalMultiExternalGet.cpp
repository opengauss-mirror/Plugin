//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CLogicalMultiExternalGet.cpp
//
//	@doc:
//		Implementation of external get
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalMultiExternalGet.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalMultiExternalGet::CLogicalMultiExternalGet
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalMultiExternalGet::CLogicalMultiExternalGet(CMemoryPool *mp)
	: CLogicalDynamicGetBase(mp), m_part_mdids(NULL)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMultiExternalGet::CLogicalMultiExternalGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalMultiExternalGet::CLogicalMultiExternalGet(
	CMemoryPool *mp, IMdIdArray *part_mdids, const CName *pnameAlias,
	CTableDescriptor *ptabdesc, ULONG scan_id, CColRefArray *pdrspqcrOutput)
	: CLogicalDynamicGetBase(mp, pnameAlias, ptabdesc, scan_id, pdrspqcrOutput),
	  m_part_mdids(part_mdids)
{
	SPQOS_ASSERT(NULL != m_part_mdids && m_part_mdids->Size() > 0);
}

CLogicalMultiExternalGet::~CLogicalMultiExternalGet()
{
	CRefCount::SafeRelease(m_part_mdids);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMultiExternalGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalMultiExternalGet::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}
	CLogicalMultiExternalGet *popGet =
		CLogicalMultiExternalGet::PopConvert(pop);

	return Ptabdesc() == popGet->Ptabdesc() &&
		   PdrspqcrOutput()->Equals(popGet->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMultiExternalGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalMultiExternalGet::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput = NULL;
	if (must_exist)
	{
		pdrspqcrOutput =
			CUtils::PdrspqcrRemapAndCreate(mp, PdrspqcrOutput(), colref_mapping);
	}
	else
	{
		pdrspqcrOutput = CUtils::PdrspqcrRemap(mp, PdrspqcrOutput(),
											 colref_mapping, must_exist);
	}
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, Name());

	CTableDescriptor *ptabdesc = Ptabdesc();
	ptabdesc->AddRef();

	m_part_mdids->AddRef();

	return SPQOS_NEW(mp) CLogicalMultiExternalGet(
		mp, m_part_mdids, pnameAlias, ptabdesc, ScanId(), pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMultiExternalGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalMultiExternalGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(
		CXform::ExfMultiExternalGet2MultiExternalScan);

	return xform_set;
}

IStatistics *
CLogicalMultiExternalGet::PstatsDerive(CMemoryPool *mp,
									   CExpressionHandle &exprhdl,
									   IStatisticsArray *  // not used
) const
{
	// requesting stats on distribution columns to estimate data skew
	IStatistics *pstatsTable =
		PstatsBaseTable(mp, exprhdl, m_ptabdesc, m_pcrsDist);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrOutput);
	CUpperBoundNDVs *upper_bound_NDVs =
		SPQOS_NEW(mp) CUpperBoundNDVs(pcrs, pstatsTable->Rows());
	CStatistics::CastStats(pstatsTable)->AddCardUpperBound(upper_bound_NDVs);

	return pstatsTable;
}

// EOF
