//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalInsert.cpp
//
//	@doc:
//		Implementation of logical Insert operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalInsert.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::CLogicalInsert
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalInsert::CLogicalInsert(CMemoryPool *mp)
	: CLogical(mp), m_ptabdesc(NULL), m_pdrspqcrSource(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::CLogicalInsert
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalInsert::CLogicalInsert(CMemoryPool *mp, CTableDescriptor *ptabdesc,
							   CColRefArray *pdrspqcrSource)
	: CLogical(mp), m_ptabdesc(ptabdesc), m_pdrspqcrSource(pdrspqcrSource)

{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pdrspqcrSource);

	m_pcrsLocalUsed->Include(m_pdrspqcrSource);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::~CLogicalInsert
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalInsert::~CLogicalInsert()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcrSource);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalInsert::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalInsert *popInsert = CLogicalInsert::PopConvert(pop);

	return m_ptabdesc->MDId()->Equals(popInsert->Ptabdesc()->MDId()) &&
		   m_pdrspqcrSource->Equals(popInsert->PdrspqcrSource());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalInsert::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrSource));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalInsert::PopCopyWithRemappedColumns(CMemoryPool *mp,
										   UlongToColRefMap *colref_mapping,
										   BOOL must_exist)
{
	CColRefArray *colref_array =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrSource, colref_mapping, must_exist);
	m_ptabdesc->AddRef();

	return SPQOS_NEW(mp) CLogicalInsert(mp, m_ptabdesc, colref_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalInsert::DeriveOutputColumns(CMemoryPool *mp,
									CExpressionHandle &	 //exprhdl
)
{
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsOutput->Include(m_pdrspqcrSource);
	return pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalInsert::DeriveKeyCollection(CMemoryPool *,	// mp
									CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalInsert::DeriveMaxCard(CMemoryPool *,  // mp
							  CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalInsert::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfInsert2DML);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalInsert::PstatsDerive(CMemoryPool *,	 // mp,
							 CExpressionHandle &exprhdl,
							 IStatisticsArray *	 // not used
) const
{
	return PstatsPassThruOuter(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInsert::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalInsert::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " (";
	m_ptabdesc->Name().OsPrint(os);
	os << "), Source Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrSource);
	os << "]";

	return os;
}

// EOF
