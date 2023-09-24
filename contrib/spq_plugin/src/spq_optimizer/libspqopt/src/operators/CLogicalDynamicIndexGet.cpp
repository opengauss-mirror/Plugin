//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDynamicIndexGet.cpp
//
//	@doc:
//		Implementation of index access for partitioned tables
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDynamicIndexGet.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CPartConstraint.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::CLogicalDynamicIndexGet
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDynamicIndexGet::CLogicalDynamicIndexGet(CMemoryPool *mp)
	: CLogicalDynamicGetBase(mp),
	  m_pindexdesc(NULL),
	  m_ulOriginOpId(spqos::ulong_max),
	  m_pos(NULL)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::CLogicalDynamicIndexGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalDynamicIndexGet::CLogicalDynamicIndexGet(
	CMemoryPool *mp, const IMDIndex *pmdindex, CTableDescriptor *ptabdesc,
	ULONG ulOriginOpId, const CName *pnameAlias, ULONG part_idx_id,
	CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrPart,
	ULONG ulSecondaryPartIndexId, CPartConstraint *ppartcnstr,
	CPartConstraint *ppartcnstrRel)
	: CLogicalDynamicGetBase(
		  mp, pnameAlias, ptabdesc, part_idx_id, pdrspqcrOutput, pdrspqdrspqcrPart,
		  ulSecondaryPartIndexId, IsPartialIndex(ptabdesc, pmdindex),
		  ppartcnstr, ppartcnstrRel),
	  m_pindexdesc(NULL),
	  m_ulOriginOpId(ulOriginOpId)
{
	SPQOS_ASSERT(NULL != pmdindex);

	// create the index descriptor
	m_pindexdesc = CIndexDescriptor::Pindexdesc(mp, ptabdesc, pmdindex);

	// compute the order spec
	m_pos = PosFromIndex(m_mp, pmdindex, m_pdrspqcrOutput, ptabdesc);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::~CLogicalDynamicIndexGet
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalDynamicIndexGet::~CLogicalDynamicIndexGet()
{
	CRefCount::SafeRelease(m_pindexdesc);
	CRefCount::SafeRelease(m_pos);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalDynamicIndexGet::HashValue() const
{
	return spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(spqos::HashValue(&m_scan_id),
							m_pindexdesc->MDId()->HashValue()));
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalDynamicIndexGet::Matches(COperator *pop) const
{
	return CUtils::FMatchDynamicIndex(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalDynamicIndexGet::DeriveOuterReferences(CMemoryPool *mp,
											   CExpressionHandle &exprhdl)
{
	return PcrsDeriveOuterIndexGet(mp, exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDynamicIndexGet::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDIndex *pmdindex = md_accessor->RetrieveIndex(m_pindexdesc->MDId());
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, *m_pnameAlias);

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

	CColRef2dArray *pdrspqdrspqcrPart = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrPart, colref_mapping, must_exist);
	CPartConstraint *ppartcnstr =
		m_part_constraint->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
															 must_exist);
	CPartConstraint *ppartcnstrRel =
		m_ppartcnstrRel->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
														   must_exist);

	m_ptabdesc->AddRef();

	return SPQOS_NEW(mp) CLogicalDynamicIndexGet(
		mp, pmdindex, m_ptabdesc, m_ulOriginOpId, pnameAlias, m_scan_id,
		pdrspqcrOutput, pdrspqdrspqcrPart, m_ulSecondaryScanId, ppartcnstr,
		ppartcnstrRel);
}

// Checking if index is partial given the table descriptor and mdid of the index
BOOL
CLogicalDynamicIndexGet::IsPartialIndex(CTableDescriptor *ptabdesc,
										const IMDIndex *pmdindex)
{
	// refer to the relation on which this index is defined for index partial information
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdesc->MDId());
	return pmdrel->IsPartialIndex(pmdindex->MDId());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::FInputOrderSensitive
//
//	@doc:
//		Is input order sensitive
//
//---------------------------------------------------------------------------
BOOL
CLogicalDynamicIndexGet::FInputOrderSensitive() const
{
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDynamicIndexGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfDynamicIndexGet2DynamicIndexScan);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::PstatsDerive
//
//	@doc:
//		Load up statistics from metadata
//
//---------------------------------------------------------------------------

IStatistics *
CLogicalDynamicIndexGet::PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const
{
	return CStatisticsUtils::DeriveStatsForIndexGet(mp, exprhdl, stats_ctxt);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicIndexGet::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalDynamicIndexGet::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " ";
	// index name
	os << "  Index Name: (";
	m_pindexdesc->Name().OsPrint(os);
	// table alias name
	os << ")";
	os << ", Table Name: (";
	m_pnameAlias->OsPrint(os);
	os << "), ";
	m_part_constraint->OsPrint(os);
	os << ", Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "] Scan Id: " << m_scan_id << "." << m_ulSecondaryScanId;

	if (!m_part_constraint->IsConstraintUnbounded())
	{
		os << ", ";
		m_part_constraint->OsPrint(os);
	}

	return os;
}

// EOF
