//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDynamicGet.cpp
//
//	@doc:
//		Implementation of dynamic table access
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDynamicGet.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::CLogicalDynamicGet
//
//	@doc:
//		ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDynamicGet::CLogicalDynamicGet(CMemoryPool *mp)
	: CLogicalDynamicGetBase(mp)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::CLogicalDynamicGet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalDynamicGet::CLogicalDynamicGet(
	CMemoryPool *mp, const CName *pnameAlias, CTableDescriptor *ptabdesc,
	ULONG ulPartIndex, CColRefArray *pdrspqcrOutput,
	CColRef2dArray *pdrspqdrspqcrPart, ULONG ulSecondaryPartIndexId,
	BOOL is_partial, CPartConstraint *ppartcnstr,
	CPartConstraint *ppartcnstrRel)
	: CLogicalDynamicGetBase(
		  mp, pnameAlias, ptabdesc, ulPartIndex, pdrspqcrOutput, pdrspqdrspqcrPart,
		  ulSecondaryPartIndexId, is_partial, ppartcnstr, ppartcnstrRel)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::CLogicalDynamicGet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalDynamicGet::CLogicalDynamicGet(CMemoryPool *mp, const CName *pnameAlias,
									   CTableDescriptor *ptabdesc,
									   ULONG ulPartIndex)
	: CLogicalDynamicGetBase(mp, pnameAlias, ptabdesc, ulPartIndex,
							 NULL /* pdrspqcrOutput*/)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::~CLogicalDynamicGet
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CLogicalDynamicGet::~CLogicalDynamicGet()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalDynamicGet::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalDynamicGet::Matches(COperator *pop) const
{
	return CUtils::FMatchDynamicScan(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDynamicGet::PopCopyWithRemappedColumns(CMemoryPool *mp,
											   UlongToColRefMap *colref_mapping,
											   BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput = NULL;
	if (must_exist)
	{
		pdrspqcrOutput =
			CUtils::PdrspqcrRemapAndCreate(mp, m_pdrspqcrOutput, colref_mapping);
	}
	else
	{
		pdrspqcrOutput = CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput,
											 colref_mapping, must_exist);
	}
	CColRef2dArray *pdrspqdrspqcrPart =
		PdrspqdrspqcrCreatePartCols(mp, pdrspqcrOutput, m_ptabdesc->PdrspqulPart());
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, *m_pnameAlias);
	m_ptabdesc->AddRef();

	CPartConstraint *ppartcnstr =
		m_part_constraint->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
															 must_exist);
	CPartConstraint *ppartcnstrRel =
		m_ppartcnstrRel->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
														   must_exist);

	return SPQOS_NEW(mp) CLogicalDynamicGet(
		mp, pnameAlias, m_ptabdesc, m_scan_id, pdrspqcrOutput, pdrspqdrspqcrPart,
		m_ulSecondaryScanId, m_is_partial, ppartcnstr, ppartcnstrRel);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::FInputOrderSensitive
//
//	@doc:
//		Not called for leaf operators
//
//---------------------------------------------------------------------------
BOOL
CLogicalDynamicGet::FInputOrderSensitive() const
{
	SPQOS_ASSERT(!"Unexpected function call of FInputOrderSensitive");
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDynamicGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfDynamicGet2DynamicTableScan);
	(void) xform_set->ExchangeSet(
		CXform::ExfExpandDynamicGetWithExternalPartitions);
	return xform_set;
}



//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalDynamicGet::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}
	else
	{
		os << SzId() << " ";
		// alias of table as referenced in the query
		m_pnameAlias->OsPrint(os);

		// actual name of table in catalog and columns
		os << " (";
		m_ptabdesc->Name().OsPrint(os);
		os << "), ";
		m_part_constraint->OsPrint(os);
		os << "), Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
		os << "] Scan Id: " << m_scan_id << "." << m_ulSecondaryScanId;

		if (!m_part_constraint->IsConstraintUnbounded())
		{
			os << ", ";
			m_part_constraint->OsPrint(os);
		}
	}

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGet::PstatsDerive
//
//	@doc:
//		Load up statistics from metadata
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDynamicGet::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 IStatisticsArray *	 // not used
) const
{
	CReqdPropRelational *prprel =
		CReqdPropRelational::GetReqdRelationalProps(exprhdl.Prp());
	IStatistics *stats =
		PstatsDeriveFilter(mp, exprhdl, prprel->PexprPartPred());

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrOutput);
	CUpperBoundNDVs *upper_bound_NDVs =
		SPQOS_NEW(mp) CUpperBoundNDVs(pcrs, stats->Rows());
	CStatistics::CastStats(stats)->AddCardUpperBound(upper_bound_NDVs);

	return stats;
}

// EOF
