//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CLogicalGbAggDeduplicate.cpp
//
//	@doc:
//		Implementation of aggregate operator for deduplicating semijoin outputs
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalGbAggDeduplicate.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::CLogicalGbAggDeduplicate
//
//	@doc:
//		Ctor for xform pattern
//
//---------------------------------------------------------------------------
CLogicalGbAggDeduplicate::CLogicalGbAggDeduplicate(CMemoryPool *mp)
	: CLogicalGbAgg(mp), m_pdrspqcrKeys(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::CLogicalGbAggDeduplicate
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalGbAggDeduplicate::CLogicalGbAggDeduplicate(
	CMemoryPool *mp, CColRefArray *colref_array,
	COperator::EGbAggType egbaggtype, CColRefArray *pdrspqcrKeys)
	: CLogicalGbAgg(mp, colref_array, egbaggtype), m_pdrspqcrKeys(pdrspqcrKeys)
{
	SPQOS_ASSERT(NULL != pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::CLogicalGbAggDeduplicate
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalGbAggDeduplicate::CLogicalGbAggDeduplicate(
	CMemoryPool *mp, CColRefArray *colref_array, CColRefArray *pdrspqcrMinimal,
	COperator::EGbAggType egbaggtype, CColRefArray *pdrspqcrKeys)
	: CLogicalGbAgg(mp, colref_array, pdrspqcrMinimal, egbaggtype),
	  m_pdrspqcrKeys(pdrspqcrKeys)
{
	SPQOS_ASSERT(NULL != pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::~CLogicalGbAggDeduplicate
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalGbAggDeduplicate::~CLogicalGbAggDeduplicate()
{
	// safe release -- to allow for instances used in patterns
	CRefCount::SafeRelease(m_pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalGbAggDeduplicate::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *colref_array =
		CUtils::PdrspqcrRemap(mp, Pdrspqcr(), colref_mapping, must_exist);

	CColRefArray *pdrspqcrMinimal = PdrspqcrMinimal();
	if (NULL != pdrspqcrMinimal)
	{
		pdrspqcrMinimal = CUtils::PdrspqcrRemap(mp, pdrspqcrMinimal,
											  colref_mapping, must_exist);
	}

	CColRefArray *pdrspqcrKeys =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrKeys, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalGbAggDeduplicate(
		mp, colref_array, pdrspqcrMinimal, Egbaggtype(), pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::PcrsStat
//
//	@doc:
//		Compute required stats columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGbAggDeduplicate::PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CColRefSet *pcrsInput,
								   ULONG child_index) const
{
	return PcrsStatGbAgg(mp, exprhdl, pcrsInput, child_index, m_pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalGbAggDeduplicate::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   CUtils::UlHashColArray(Pdrspqcr()));
	ulHash = spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrKeys));

	ULONG ulGbaggtype = (ULONG) Egbaggtype();

	ulHash = spqos::CombineHashes(ulHash, spqos::HashValue<ULONG>(&ulGbaggtype));

	return spqos::CombineHashes(ulHash,
							   spqos::HashValue<BOOL>(&m_fGeneratesDuplicates));
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalGbAggDeduplicate::DeriveKeyCollection(CMemoryPool *mp,
											  CExpressionHandle &  //exprhdl
) const
{
	CKeyCollection *pkc = NULL;

	// Gb produces a key only if it's global
	if (FGlobal())
	{
		// keys from join child are still keys
		m_pdrspqcrKeys->AddRef();
		pkc = SPQOS_NEW(mp) CKeyCollection(mp, m_pdrspqcrKeys);
	}

	return pkc;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalGbAggDeduplicate::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalGbAggDeduplicate *popAgg =
		CLogicalGbAggDeduplicate::PopConvert(pop);

	if (FGeneratesDuplicates() != popAgg->FGeneratesDuplicates())
	{
		return false;
	}

	return popAgg->Egbaggtype() == Egbaggtype() &&
		   Pdrspqcr()->Equals(popAgg->Pdrspqcr()) &&
		   m_pdrspqcrKeys->Equals(popAgg->PdrspqcrKeys()) &&
		   CColRef::Equals(PdrspqcrMinimal(), popAgg->PdrspqcrMinimal());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalGbAggDeduplicate::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfPushGbDedupBelowJoin);
	(void) xform_set->ExchangeSet(CXform::ExfSplitGbAggDedup);
	(void) xform_set->ExchangeSet(CXform::ExfGbAggDedup2HashAggDedup);
	(void) xform_set->ExchangeSet(CXform::ExfGbAggDedup2StreamAggDedup);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalGbAggDeduplicate::PstatsDerive(CMemoryPool *mp,
									   CExpressionHandle &exprhdl,
									   IStatisticsArray *  // not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);
	IStatistics *child_stats = exprhdl.Pstats(0);

	// extract computed columns
	ULongPtrArray *pdrspqulComputedCols = SPQOS_NEW(mp) ULongPtrArray(mp);
	exprhdl.DeriveDefinedColumns(1)->ExtractColIds(mp, pdrspqulComputedCols);

	// construct bitset with keys of join child
	CBitSet *keys = SPQOS_NEW(mp) CBitSet(mp);
	const ULONG ulKeys = m_pdrspqcrKeys->Size();
	for (ULONG ul = 0; ul < ulKeys; ul++)
	{
		CColRef *colref = (*m_pdrspqcrKeys)[ul];
		keys->ExchangeSet(colref->Id());
	}

	IStatistics *stats = CLogicalGbAgg::PstatsDerive(mp, child_stats, Pdrspqcr(),
													 pdrspqulComputedCols, keys);
	keys->Release();
	pdrspqulComputedCols->Release();

	return stats;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAggDeduplicate::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalGbAggDeduplicate::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << "( ";
	OsPrintGbAggType(os, Egbaggtype());
	os << " )";
	os << " Grp Cols: [";
	CUtils::OsPrintDrgPcr(os, Pdrspqcr());
	os << "], Minimal Grp Cols: [";
	if (NULL != PdrspqcrMinimal())
	{
		CUtils::OsPrintDrgPcr(os, PdrspqcrMinimal());
	}
	os << "], Join Child Keys: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrKeys);
	os << "]";
	os << ", Generates Duplicates :[ " << FGeneratesDuplicates() << " ] ";

	return os;
}

// EOF
