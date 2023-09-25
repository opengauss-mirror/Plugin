//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalSplit.cpp
//
//	@doc:
//		Implementation of logical split operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalSplit.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::CLogicalSplit
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalSplit::CLogicalSplit(CMemoryPool *mp)
	: CLogical(mp),
	  m_pdrspqcrDelete(NULL),
	  m_pdrspqcrInsert(NULL),
	  m_pcrCtid(NULL),
	  m_pcrSegmentId(NULL),
	  m_pcrAction(NULL),
	  m_pcrTupleOid(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::CLogicalSplit
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalSplit::CLogicalSplit(CMemoryPool *mp, CColRefArray *pdrspqcrDelete,
							 CColRefArray *pdrspqcrInsert, CColRef *pcrCtid,
							 CColRef *pcrSegmentId, CColRef *pcrAction,
							 CColRef *pcrTupleOid)
	: CLogical(mp),
	  m_pdrspqcrDelete(pdrspqcrDelete),
	  m_pdrspqcrInsert(pdrspqcrInsert),
	  m_pcrCtid(pcrCtid),
	  m_pcrSegmentId(pcrSegmentId),
	  m_pcrAction(pcrAction),
	  m_pcrTupleOid(pcrTupleOid)

{
	SPQOS_ASSERT(NULL != pdrspqcrDelete);
	SPQOS_ASSERT(NULL != pdrspqcrInsert);
	SPQOS_ASSERT(pdrspqcrInsert->Size() == pdrspqcrDelete->Size());
	SPQOS_ASSERT(NULL != pcrCtid);
	SPQOS_ASSERT(NULL != pcrSegmentId);
	SPQOS_ASSERT(NULL != pcrAction);

	m_pcrsLocalUsed->Include(m_pdrspqcrDelete);
	m_pcrsLocalUsed->Include(m_pdrspqcrInsert);
	m_pcrsLocalUsed->Include(m_pcrCtid);
	m_pcrsLocalUsed->Include(m_pcrSegmentId);
	m_pcrsLocalUsed->Include(m_pcrAction);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::~CLogicalSplit
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalSplit::~CLogicalSplit()
{
	CRefCount::SafeRelease(m_pdrspqcrDelete);
	CRefCount::SafeRelease(m_pdrspqcrInsert);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalSplit::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CLogicalSplit *popSplit = CLogicalSplit::PopConvert(pop);

		return m_pcrCtid == popSplit->PcrCtid() &&
			   m_pcrSegmentId == popSplit->PcrSegmentId() &&
			   m_pcrAction == popSplit->PcrAction() &&
			   m_pcrTupleOid == popSplit->PcrTupleOid() &&
			   m_pdrspqcrDelete->Equals(popSplit->PdrspqcrDelete()) &&
			   m_pdrspqcrInsert->Equals(popSplit->PdrspqcrInsert());
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalSplit::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   CUtils::UlHashColArray(m_pdrspqcrInsert));
	ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrCtid));
	ulHash =
		spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrSegmentId));
	ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(m_pcrAction));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalSplit::PopCopyWithRemappedColumns(CMemoryPool *mp,
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
	CColRef *pcrAction =
		CUtils::PcrRemap(m_pcrAction, colref_mapping, must_exist);

	CColRef *pcrTupleOid = NULL;
	if (NULL != m_pcrTupleOid)
	{
		pcrTupleOid =
			CUtils::PcrRemap(m_pcrTupleOid, colref_mapping, must_exist);
	}

	return SPQOS_NEW(mp) CLogicalSplit(mp, pdrspqcrDelete, pdrspqcrInsert, pcrCtid,
									  pcrSegmentId, pcrAction, pcrTupleOid);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalSplit::DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(2 == exprhdl.Arity());

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Union(exprhdl.DeriveOutputColumns(0));
	pcrs->Include(m_pcrAction);

	if (NULL != m_pcrTupleOid)
	{
		pcrs->Include(m_pcrTupleOid);
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalSplit::DeriveKeyCollection(CMemoryPool *,  // mp
								   CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalSplit::DeriveMaxCard(CMemoryPool *,	 // mp
							 CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalSplit::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementSplit);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalSplit::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							IStatisticsArray *	// not used
) const
{
	// split returns double the number of tuples coming from its child
	IStatistics *stats = exprhdl.Pstats(0);

	return stats->ScaleStats(mp, CDouble(2.0) /*factor*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSplit::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalSplit::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " -- Delete Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrDelete);
	os << "], Insert Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrInsert);
	os << "], ";
	m_pcrCtid->OsPrint(os);
	os << ", ";
	m_pcrSegmentId->OsPrint(os);
	os << ", ";
	m_pcrAction->OsPrint(os);

	return os;
}

// EOF
