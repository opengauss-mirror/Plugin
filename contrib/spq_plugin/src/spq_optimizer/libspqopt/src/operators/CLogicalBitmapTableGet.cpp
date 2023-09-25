//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CLogicalBitmapTableGet.cpp
//
//	@doc:
//		Logical operator for table access via bitmap indexes.
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalBitmapTableGet.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/xforms/CXform.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::CLogicalBitmapTableGet
//
//	@doc:
//		Ctor
//		Takes ownership of ptabdesc, pnameTableAlias and pdrspqcrOutput.
//
//---------------------------------------------------------------------------
CLogicalBitmapTableGet::CLogicalBitmapTableGet(CMemoryPool *mp,
											   CTableDescriptor *ptabdesc,
											   ULONG ulOriginOpId,
											   const CName *pnameTableAlias,
											   CColRefArray *pdrspqcrOutput)
	: CLogical(mp),
	  m_ptabdesc(ptabdesc),
	  m_ulOriginOpId(ulOriginOpId),
	  m_pnameTableAlias(pnameTableAlias),
	  m_pdrspqcrOutput(pdrspqcrOutput)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameTableAlias);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::CLogicalBitmapTableGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalBitmapTableGet::CLogicalBitmapTableGet(CMemoryPool *mp)
	: CLogical(mp),
	  m_ptabdesc(NULL),
	  m_ulOriginOpId(spqos::ulong_max),
	  m_pnameTableAlias(NULL),
	  m_pdrspqcrOutput(NULL)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::~CLogicalBitmapTableGet
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalBitmapTableGet::~CLogicalBitmapTableGet()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcrOutput);

	SPQOS_DELETE(m_pnameTableAlias);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalBitmapTableGet::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::Matches
//
//	@doc:
//		Match this operator with the given one.
//
//---------------------------------------------------------------------------
BOOL
CLogicalBitmapTableGet::Matches(COperator *pop) const
{
	return CUtils::FMatchBitmapScan(this, pop);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalBitmapTableGet::DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &
#ifdef SPQOS_DEBUG
																 exprhdl
#endif
)
{
	SPQOS_ASSERT(exprhdl.Pop() == this);

	return SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalBitmapTableGet::DeriveOuterReferences(CMemoryPool *mp,
											  CExpressionHandle &exprhdl)
{
	return PcrsDeriveOuterIndexGet(mp, exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::DerivePropertyConstraint
//
//	@doc:
//		Derive the constraint property.
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalBitmapTableGet::DerivePropertyConstraint(
	CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	return PpcDeriveConstraintFromTableWithPredicates(mp, exprhdl, m_ptabdesc,
													  m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalBitmapTableGet::PstatsDerive(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 IStatisticsArray *stats_ctxt) const
{
	return CStatisticsUtils::DeriveStatsForBitmapTableGet(mp, exprhdl,
														  stats_ctxt);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::OsPrint
//
//	@doc:
//		Debug print of this operator
//
//---------------------------------------------------------------------------
IOstream &
CLogicalBitmapTableGet::OsPrint(IOstream &os) const
{
	os << SzId() << " ";
	os << ", Table Name: (";
	m_ptabdesc->Name().OsPrint(os);
	os << ")";
	os << ", Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "]";

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalBitmapTableGet::PopCopyWithRemappedColumns(
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
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, *m_pnameTableAlias);

	m_ptabdesc->AddRef();

	return SPQOS_NEW(mp) CLogicalBitmapTableGet(mp, m_ptabdesc, m_ulOriginOpId,
											   pnameAlias, pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalBitmapTableGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalBitmapTableGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementBitmapTableGet);

	return xform_set;
}

// EOF
