//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDelete.cpp
//
//	@doc:
//		Implementation of logical Delete operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDelete.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::CLogicalDelete
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDelete::CLogicalDelete(CMemoryPool *mp)
	: CLogical(mp),
	  m_ptabdesc(NULL),
	  m_pdrspqcr(NULL),
	  m_pcrCtid(NULL),
	  m_pcrSegmentId(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::CLogicalDelete
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalDelete::CLogicalDelete(CMemoryPool *mp, CTableDescriptor *ptabdesc,
							   CColRefArray *colref_array, CColRef *pcrCtid,
							   CColRef *pcrSegmentId)
	: CLogical(mp),
	  m_ptabdesc(ptabdesc),
	  m_pdrspqcr(colref_array),
	  m_pcrCtid(pcrCtid),
	  m_pcrSegmentId(pcrSegmentId)

{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(NULL != pcrCtid);
	SPQOS_ASSERT(NULL != pcrSegmentId);

	m_pcrsLocalUsed->Include(m_pdrspqcr);
	m_pcrsLocalUsed->Include(m_pcrCtid);
	m_pcrsLocalUsed->Include(m_pcrSegmentId);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::~CLogicalDelete
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalDelete::~CLogicalDelete()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcr);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalDelete::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalDelete *popDelete = CLogicalDelete::PopConvert(pop);

	return m_pcrCtid == popDelete->PcrCtid() &&
		   m_pcrSegmentId == popDelete->PcrSegmentId() &&
		   m_ptabdesc->MDId()->Equals(popDelete->Ptabdesc()->MDId()) &&
		   m_pdrspqcr->Equals(popDelete->Pdrspqcr());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalDelete::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash = spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcr));
	ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrCtid));
	ulHash =
		spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrSegmentId));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDelete::PopCopyWithRemappedColumns(CMemoryPool *mp,
										   UlongToColRefMap *colref_mapping,
										   BOOL must_exist)
{
	CColRefArray *colref_array =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcr, colref_mapping, must_exist);
	CColRef *pcrCtid = CUtils::PcrRemap(m_pcrCtid, colref_mapping, must_exist);
	CColRef *pcrSegmentId =
		CUtils::PcrRemap(m_pcrSegmentId, colref_mapping, must_exist);
	m_ptabdesc->AddRef();

	return SPQOS_NEW(mp)
		CLogicalDelete(mp, m_ptabdesc, colref_array, pcrCtid, pcrSegmentId);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalDelete::DeriveOutputColumns(CMemoryPool *mp,
									CExpressionHandle &	 //exprhdl
)
{
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsOutput->Include(m_pdrspqcr);
	return pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalDelete::DeriveKeyCollection(CMemoryPool *,	// mp
									CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalDelete::DeriveMaxCard(CMemoryPool *,  // mp
							  CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDelete::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfDelete2DML);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDelete::PstatsDerive(CMemoryPool *,	 // mp,
							 CExpressionHandle &exprhdl,
							 IStatisticsArray *	 // not used
) const
{
	return PstatsPassThruOuter(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDelete::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalDelete::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " (";
	m_ptabdesc->Name().OsPrint(os);
	os << "), Deleted Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcr);
	os << "], ";
	m_pcrCtid->OsPrint(os);
	os << ", ";
	m_pcrSegmentId->OsPrint(os);

	return os;
}

// EOF
