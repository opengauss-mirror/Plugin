//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CLogicalDynamicBitmapTableGet.cpp
//
//	@doc:
//		Logical operator for dynamic table access via bitmap indexes.
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDynamicBitmapTableGet.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/xforms/CXform.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::CLogicalDynamicBitmapTableGet
//
//	@doc:
//		Ctor
//		Takes ownership of ptabdesc, pnameTableAlias and pdrspqcrOutput.
//
//---------------------------------------------------------------------------
CLogicalDynamicBitmapTableGet::CLogicalDynamicBitmapTableGet(
	CMemoryPool *mp, CTableDescriptor *ptabdesc, ULONG ulOriginOpId,
	const CName *pnameTableAlias, ULONG ulPartIndex,
	CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrPart,
	ULONG ulSecondaryPartIndexId, BOOL is_partial, CPartConstraint *ppartcnstr,
	CPartConstraint *ppartcnstrRel)
	: CLogicalDynamicGetBase(mp, pnameTableAlias, ptabdesc, ulPartIndex,
							 pdrspqcrOutput, pdrspqdrspqcrPart,
							 ulSecondaryPartIndexId, is_partial, ppartcnstr,
							 ppartcnstrRel),
	  m_ulOriginOpId(ulOriginOpId)

{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::CLogicalDynamicBitmapTableGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalDynamicBitmapTableGet::CLogicalDynamicBitmapTableGet(CMemoryPool *mp)
	: CLogicalDynamicGetBase(mp), m_ulOriginOpId(spqos::ulong_max)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::~CLogicalDynamicBitmapTableGet
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalDynamicBitmapTableGet::~CLogicalDynamicBitmapTableGet()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalDynamicBitmapTableGet::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash = spqos::CombineHashes(ulHash, spqos::HashValue(&m_scan_id));
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::Matches
//
//	@doc:
//		Match this operator with the given one.
//
//---------------------------------------------------------------------------
BOOL
CLogicalDynamicBitmapTableGet::Matches(COperator *pop) const
{
	return CUtils::FMatchDynamicBitmapScan(this, pop);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::DerivePropertyConstraint
//
//	@doc:
//		Derive the constraint property.
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalDynamicBitmapTableGet::DerivePropertyConstraint(
	CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	return PpcDeriveConstraintFromTableWithPredicates(mp, exprhdl, m_ptabdesc,
													  m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalDynamicBitmapTableGet::DeriveOuterReferences(CMemoryPool *mp,
													 CExpressionHandle &exprhdl)
{
	return PcrsDeriveOuterIndexGet(mp, exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDynamicBitmapTableGet::PstatsDerive(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											IStatisticsArray *stats_ctxt) const
{
	return CStatisticsUtils::DeriveStatsForBitmapTableGet(mp, exprhdl,
														  stats_ctxt);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::OsPrint
//
//	@doc:
//		Debug print of this operator
//
//---------------------------------------------------------------------------
IOstream &
CLogicalDynamicBitmapTableGet::OsPrint(IOstream &os) const
{
	os << SzId() << " ";
	os << ", Table Name: (";
	m_ptabdesc->Name().OsPrint(os);
	os << ") Scan Id: " << m_scan_id;
	os << ", Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "]";

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDynamicBitmapTableGet::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
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
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, *m_pnameAlias);

	m_ptabdesc->AddRef();

	CColRef2dArray *pdrspqdrspqcrPart = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrPart, colref_mapping, must_exist);
	CPartConstraint *ppartcnstr =
		m_part_constraint->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
															 must_exist);
	CPartConstraint *ppartcnstrRel =
		m_ppartcnstrRel->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
														   must_exist);

	return SPQOS_NEW(mp) CLogicalDynamicBitmapTableGet(
		mp, m_ptabdesc, m_ulOriginOpId, pnameAlias, m_scan_id, pdrspqcrOutput,
		pdrspqdrspqcrPart, m_ulSecondaryScanId, m_is_partial, ppartcnstr,
		ppartcnstrRel);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicBitmapTableGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDynamicBitmapTableGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementDynamicBitmapTableGet);

	return xform_set;
}

// EOF
