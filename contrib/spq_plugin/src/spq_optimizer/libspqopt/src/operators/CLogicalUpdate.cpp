//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalUpdate.cpp
//
//	@doc:
//		Implementation of logical Update operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalUpdate.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::CLogicalUpdate
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalUpdate::CLogicalUpdate(CMemoryPool *mp)
	: CLogical(mp),
	  m_ptabdesc(NULL),
	  m_pdrspqcrDelete(NULL),
	  m_pdrspqcrInsert(NULL),
	  m_pcrCtid(NULL),
	  m_pcrSegmentId(NULL),
	  m_pcrTupleOid(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::CLogicalUpdate
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalUpdate::CLogicalUpdate(CMemoryPool *mp, CTableDescriptor *ptabdesc,
							   CColRefArray *pdrspqcrDelete,
							   CColRefArray *pdrspqcrInsert, CColRef *pcrCtid,
							   CColRef *pcrSegmentId, CColRef *pcrTupleOid)
	: CLogical(mp),
	  m_ptabdesc(ptabdesc),
	  m_pdrspqcrDelete(pdrspqcrDelete),
	  m_pdrspqcrInsert(pdrspqcrInsert),
	  m_pcrCtid(pcrCtid),
	  m_pcrSegmentId(pcrSegmentId),
	  m_pcrTupleOid(pcrTupleOid)

{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pdrspqcrDelete);
	SPQOS_ASSERT(NULL != pdrspqcrInsert);
	SPQOS_ASSERT(pdrspqcrDelete->Size() == pdrspqcrInsert->Size());
	SPQOS_ASSERT(NULL != pcrCtid);
	SPQOS_ASSERT(NULL != pcrSegmentId);

	m_pcrsLocalUsed->Include(m_pdrspqcrDelete);
	m_pcrsLocalUsed->Include(m_pdrspqcrInsert);
	m_pcrsLocalUsed->Include(m_pcrCtid);
	m_pcrsLocalUsed->Include(m_pcrSegmentId);

	if (NULL != m_pcrTupleOid)
	{
		m_pcrsLocalUsed->Include(m_pcrTupleOid);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::~CLogicalUpdate
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalUpdate::~CLogicalUpdate()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcrDelete);
	CRefCount::SafeRelease(m_pdrspqcrInsert);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalUpdate::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalUpdate *popUpdate = CLogicalUpdate::PopConvert(pop);

	return m_pcrCtid == popUpdate->PcrCtid() &&
		   m_pcrSegmentId == popUpdate->PcrSegmentId() &&
		   m_pcrTupleOid == popUpdate->PcrTupleOid() &&
		   m_ptabdesc->MDId()->Equals(popUpdate->Ptabdesc()->MDId()) &&
		   m_pdrspqcrDelete->Equals(popUpdate->PdrspqcrDelete()) &&
		   m_pdrspqcrInsert->Equals(popUpdate->PdrspqcrInsert());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalUpdate::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrDelete));
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrInsert));
	ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrCtid));
	ulHash =
		spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrSegmentId));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalUpdate::PopCopyWithRemappedColumns(CMemoryPool *mp,
										   UlongToColRefMap *colref_mapping,
										   BOOL must_exist)
{
	CColRefArray *pdrspqcrDelete =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrDelete, colref_mapping, must_exist);
	CColRefArray *pdrspqcrInsert =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInsert, colref_mapping, must_exist);
	CColRef *pcrCtid = CUtils::PcrRemap(m_pcrCtid, colref_mapping, must_exist);
	CColRef *pcrSegmentId =
		CUtils::PcrRemap(m_pcrSegmentId, colref_mapping, must_exist);
	m_ptabdesc->AddRef();

	CColRef *pcrTupleOid = NULL;
	if (NULL != m_pcrTupleOid)
	{
		pcrTupleOid =
			CUtils::PcrRemap(m_pcrTupleOid, colref_mapping, must_exist);
	}
	return SPQOS_NEW(mp)
		CLogicalUpdate(mp, m_ptabdesc, pdrspqcrDelete, pdrspqcrInsert, pcrCtid,
					   pcrSegmentId, pcrTupleOid);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalUpdate::DeriveOutputColumns(CMemoryPool *mp,
									CExpressionHandle &	 //exprhdl
)
{
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsOutput->Include(m_pdrspqcrInsert);
	pcrsOutput->Include(m_pcrCtid);
	pcrsOutput->Include(m_pcrSegmentId);

	if (NULL != m_pcrTupleOid)
	{
		pcrsOutput->Include(m_pcrTupleOid);
	}
	return pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalUpdate::DeriveKeyCollection(CMemoryPool *,	// mp
									CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalUpdate::DeriveMaxCard(CMemoryPool *,  // mp
							  CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalUpdate::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfUpdate2DML);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalUpdate::PstatsDerive(CMemoryPool *,	 // mp,
							 CExpressionHandle &exprhdl,
							 IStatisticsArray *	 // not used
) const
{
	return PstatsPassThruOuter(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUpdate::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalUpdate::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " (";
	m_ptabdesc->Name().OsPrint(os);
	os << "), Delete Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrDelete);
	os << "], Insert Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrInsert);
	os << "], ";
	m_pcrCtid->OsPrint(os);
	os << ", ";
	m_pcrSegmentId->OsPrint(os);

	return os;
}

// EOF
