//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalIndexGet.cpp
//
//	@doc:
//		Implementation of basic index access
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalIndexGet.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::CLogicalIndexGet
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalIndexGet::CLogicalIndexGet(CMemoryPool *mp)
	: CLogical(mp),
	  m_pindexdesc(NULL),
	  m_ptabdesc(NULL),
	  m_ulOriginOpId(spqos::ulong_max),
	  m_pnameAlias(NULL),
	  m_pdrspqcrOutput(NULL),
	  m_pcrsOutput(NULL),
	  m_pos(NULL),
	  m_pcrsDist(NULL)
{
	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::CLogicalIndexGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalIndexGet::CLogicalIndexGet(CMemoryPool *mp, const IMDIndex *pmdindex,
								   CTableDescriptor *ptabdesc,
								   ULONG ulOriginOpId, const CName *pnameAlias,
								   CColRefArray *pdrspqcrOutput)
	: CLogical(mp),
	  m_pindexdesc(NULL),
	  m_ptabdesc(ptabdesc),
	  m_ulOriginOpId(ulOriginOpId),
	  m_pnameAlias(pnameAlias),
	  m_pdrspqcrOutput(pdrspqcrOutput),
	  m_pcrsOutput(NULL),
	  m_pcrsDist(NULL)
{
	SPQOS_ASSERT(NULL != pmdindex);
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameAlias);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	// create the index descriptor
	m_pindexdesc = CIndexDescriptor::Pindexdesc(mp, ptabdesc, pmdindex);

	// compute the order spec
	m_pos = PosFromIndex(m_mp, pmdindex, m_pdrspqcrOutput, ptabdesc);

	// create a set representation of output columns
	m_pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrOutput);

	m_pcrsDist = CLogical::PcrsDist(mp, m_ptabdesc, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::~CLogicalIndexGet
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalIndexGet::~CLogicalIndexGet()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pindexdesc);
	CRefCount::SafeRelease(m_pdrspqcrOutput);
	CRefCount::SafeRelease(m_pcrsOutput);
	CRefCount::SafeRelease(m_pos);
	CRefCount::SafeRelease(m_pcrsDist);

	SPQOS_DELETE(m_pnameAlias);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalIndexGet::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_pindexdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));
	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalIndexGet::Matches(COperator *pop) const
{
	return CUtils::FMatchIndex(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalIndexGet::PopCopyWithRemappedColumns(CMemoryPool *mp,
											 UlongToColRefMap *colref_mapping,
											 BOOL must_exist)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDIndex *pmdindex = md_accessor->RetrieveIndex(m_pindexdesc->MDId());

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

	return SPQOS_NEW(mp) CLogicalIndexGet(
		mp, pmdindex, m_ptabdesc, m_ulOriginOpId, pnameAlias, pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalIndexGet::DeriveOutputColumns(CMemoryPool *mp,
									  CExpressionHandle &  // exprhdl
)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(m_pdrspqcrOutput);

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalIndexGet::DeriveOuterReferences(CMemoryPool *mp,
										CExpressionHandle &exprhdl)
{
	return PcrsDeriveOuterIndexGet(mp, exprhdl);
}

CKeyCollection *
CLogicalIndexGet::DeriveKeyCollection(CMemoryPool *mp,
									  CExpressionHandle &  // exprhdl
) const
{
	const CBitSetArray *pdrspqbs = m_ptabdesc->PdrspqbsKeys();

	return CLogical::PkcKeysBaseTable(mp, pdrspqbs, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::FInputOrderSensitive
//
//	@doc:
//		Is input order sensitive
//
//---------------------------------------------------------------------------
BOOL
CLogicalIndexGet::FInputOrderSensitive() const
{
	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalIndexGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfIndexGet2IndexScan);
	(void) xform_set->ExchangeSet(CXform::ExfIndexGet2IndexOnlyScan);

    /* SPQ: for shareindexscan */
    (void) xform_set->ExchangeSet(CXform::ExfIndexGet2ShareIndexScan);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalIndexGet::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   IStatisticsArray *stats_ctxt) const
{
	return CStatisticsUtils::DeriveStatsForIndexGet(mp, exprhdl, stats_ctxt);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalIndexGet::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalIndexGet::OsPrint(IOstream &os) const
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
	os << ")";
	os << ", Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "]";

	return os;
}

// EOF
