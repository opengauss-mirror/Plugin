//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalCTEProducer.cpp
//
//	@doc:
//		Implementation of CTE producer operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalCTEProducer.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::CLogicalCTEProducer
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalCTEProducer::CLogicalCTEProducer(CMemoryPool *mp)
	: CLogical(mp), m_id(0), m_pdrspqcr(NULL), m_pcrsOutput(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::CLogicalCTEProducer
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalCTEProducer::CLogicalCTEProducer(CMemoryPool *mp, ULONG id,
										 CColRefArray *colref_array)
	: CLogical(mp), m_id(id), m_pdrspqcr(colref_array)
{
	SPQOS_ASSERT(NULL != colref_array);

	m_pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcr);
	SPQOS_ASSERT(m_pdrspqcr->Size() == m_pcrsOutput->Size());

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::~CLogicalCTEProducer
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalCTEProducer::~CLogicalCTEProducer()
{
	CRefCount::SafeRelease(m_pdrspqcr);
	CRefCount::SafeRelease(m_pcrsOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalCTEProducer::DeriveOutputColumns(CMemoryPool *,		  //mp,
										 CExpressionHandle &  //exprhdl
)
{
	m_pcrsOutput->AddRef();
	return m_pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::DeriveNotNullColumns
//
//	@doc:
//		Derive not nullable output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalCTEProducer::DeriveNotNullColumns(CMemoryPool *mp,
										  CExpressionHandle &exprhdl) const
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcr);
	pcrs->Intersection(exprhdl.DeriveNotNullColumns(0));

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalCTEProducer::DeriveKeyCollection(CMemoryPool *,	 // mp
										 CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalCTEProducer::DeriveMaxCard(CMemoryPool *,  // mp
								   CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

CTableDescriptor *
CLogicalCTEProducer::DeriveTableDescriptor(CMemoryPool *,  // mp
										   CExpressionHandle &exprhdl) const
{
	// pass on table descriptor of first child
	return exprhdl.DeriveTableDescriptor(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalCTEProducer::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalCTEProducer *popCTEProducer = CLogicalCTEProducer::PopConvert(pop);

	return m_id == popCTEProducer->UlCTEId() &&
		   m_pdrspqcr->Equals(popCTEProducer->Pdrspqcr());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalCTEProducer::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(), m_id);
	ulHash = spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcr));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalCTEProducer::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *colref_array =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcr, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalCTEProducer(mp, m_id, colref_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalCTEProducer::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementCTEProducer);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalCTEProducer::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalCTEProducer::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << m_id;
	os << "), Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcr);
	os << "]";

	return os;
}

// EOF
