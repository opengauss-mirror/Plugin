//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDML.cpp
//
//	@doc:
//		Implementation of logical DML operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDML.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"


using namespace spqopt;

const WCHAR CLogicalDML::m_rgwszDml[EdmlSentinel][10] = {
	SPQOS_WSZ_LIT("Insert"), SPQOS_WSZ_LIT("Delete"), SPQOS_WSZ_LIT("Update")};

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::CLogicalDML
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDML::CLogicalDML(CMemoryPool *mp)
	: CLogical(mp),
	  m_ptabdesc(NULL),
	  m_pdrspqcrSource(NULL),
	  m_pbsModified(NULL),
	  m_pcrAction(NULL),
	  m_pcrCtid(NULL),
	  m_pcrSegmentId(NULL),
	  m_pcrTupleOid(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::CLogicalDML
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalDML::CLogicalDML(CMemoryPool *mp, EDMLOperator edmlop,
						 CTableDescriptor *ptabdesc,
						 CColRefArray *pdrspqcrSource, CBitSet *pbsModified,
						 CColRef *pcrAction, CColRef *pcrCtid,
						 CColRef *pcrSegmentId, CColRef *pcrTupleOid)
	: CLogical(mp),
	  m_edmlop(edmlop),
	  m_ptabdesc(ptabdesc),
	  m_pdrspqcrSource(pdrspqcrSource),
	  m_pbsModified(pbsModified),
	  m_pcrAction(pcrAction),
	  m_pcrCtid(pcrCtid),
	  m_pcrSegmentId(pcrSegmentId),
	  m_pcrTupleOid(pcrTupleOid)
{
	SPQOS_ASSERT(EdmlSentinel != edmlop);
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pdrspqcrSource);
	SPQOS_ASSERT(NULL != pbsModified);
	SPQOS_ASSERT(NULL != pcrAction);
	SPQOS_ASSERT_IMP(EdmlDelete == edmlop || EdmlUpdate == edmlop,
					NULL != pcrCtid && NULL != pcrSegmentId);

	m_pcrsLocalUsed->Include(m_pdrspqcrSource);
	m_pcrsLocalUsed->Include(m_pcrAction);
	if (NULL != m_pcrCtid)
	{
		m_pcrsLocalUsed->Include(m_pcrCtid);
	}

	if (NULL != m_pcrSegmentId)
	{
		m_pcrsLocalUsed->Include(m_pcrSegmentId);
	}

	if (NULL != m_pcrTupleOid)
	{
		m_pcrsLocalUsed->Include(m_pcrTupleOid);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::~CLogicalDML
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalDML::~CLogicalDML()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcrSource);
	CRefCount::SafeRelease(m_pbsModified);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalDML::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalDML *popDML = CLogicalDML::PopConvert(pop);

	return m_pcrAction == popDML->PcrAction() &&
		   m_pcrCtid == popDML->PcrCtid() &&
		   m_pcrSegmentId == popDML->PcrSegmentId() &&
		   m_pcrTupleOid == popDML->PcrTupleOid() &&
		   m_ptabdesc->MDId()->Equals(popDML->Ptabdesc()->MDId()) &&
		   m_pdrspqcrSource->Equals(popDML->PdrspqcrSource());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalDML::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrAction));
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrSource));

	if (EdmlDelete == m_edmlop || EdmlUpdate == m_edmlop)
	{
		ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrCtid));
		ulHash =
			spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrSegmentId));
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDML::PopCopyWithRemappedColumns(CMemoryPool *mp,
										UlongToColRefMap *colref_mapping,
										BOOL must_exist)
{
	CColRefArray *colref_array =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrSource, colref_mapping, must_exist);
	CColRef *pcrAction =
		CUtils::PcrRemap(m_pcrAction, colref_mapping, must_exist);

	// no need to remap modified columns bitset as it represent column indexes
	// and not actual columns
	m_pbsModified->AddRef();

	CColRef *pcrCtid = NULL;
	if (NULL != m_pcrCtid)
	{
		pcrCtid = CUtils::PcrRemap(m_pcrCtid, colref_mapping, must_exist);
	}

	CColRef *pcrSegmentId = NULL;
	if (NULL != m_pcrSegmentId)
	{
		pcrSegmentId =
			CUtils::PcrRemap(m_pcrSegmentId, colref_mapping, must_exist);
	}

	CColRef *pcrTupleOid = NULL;
	if (NULL != m_pcrTupleOid)
	{
		pcrTupleOid =
			CUtils::PcrRemap(m_pcrTupleOid, colref_mapping, must_exist);
	}

	m_ptabdesc->AddRef();

	return SPQOS_NEW(mp)
		CLogicalDML(mp, m_edmlop, m_ptabdesc, colref_array, m_pbsModified,
					pcrAction, pcrCtid, pcrSegmentId, pcrTupleOid);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalDML::DeriveOutputColumns(CMemoryPool *mp,
								 CExpressionHandle &  //exprhdl
)
{
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsOutput->Include(m_pdrspqcrSource);
	if (NULL != m_pcrCtid)
	{
		SPQOS_ASSERT(NULL != m_pcrSegmentId);
		pcrsOutput->Include(m_pcrCtid);
		pcrsOutput->Include(m_pcrSegmentId);
	}

	pcrsOutput->Include(m_pcrAction);

	if (NULL != m_pcrTupleOid)
	{
		pcrsOutput->Include(m_pcrTupleOid);
	}

	return pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::DerivePropertyConstraint
//
//	@doc:
//		Derive constraint property
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalDML::DerivePropertyConstraint(CMemoryPool *mp,
									  CExpressionHandle &exprhdl) const
{
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsOutput->Include(m_pdrspqcrSource);
	CPropConstraint *ppc = PpcDeriveConstraintRestrict(mp, exprhdl, pcrsOutput);
	pcrsOutput->Release();

	return ppc;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalDML::DeriveKeyCollection(CMemoryPool *,	 // mp
								 CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalDML::DeriveMaxCard(CMemoryPool *,  // mp
						   CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDML::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementDML);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDML::PstatsDerive(CMemoryPool *,  // mp,
						  CExpressionHandle &exprhdl,
						  IStatisticsArray *  // not used
) const
{
	return PstatsPassThruOuter(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDML::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalDML::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " (";
	os << m_rgwszDml[m_edmlop] << ", ";
	m_ptabdesc->Name().OsPrint(os);
	os << "), Affected Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrSource);
	os << "], Action: (";
	m_pcrAction->OsPrint(os);
	os << ")";

	if (EdmlDelete == m_edmlop || EdmlUpdate == m_edmlop)
	{
		os << ", ";
		m_pcrCtid->OsPrint(os);
		os << ", ";
		m_pcrSegmentId->OsPrint(os);
	}

	return os;
}

// EOF
