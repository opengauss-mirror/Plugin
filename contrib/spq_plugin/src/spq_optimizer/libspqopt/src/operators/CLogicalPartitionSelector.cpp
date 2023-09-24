//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalPartitionSelector.cpp
//
//	@doc:
//		Implementation of Logical partition selector
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalPartitionSelector.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::CLogicalPartitionSelector
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalPartitionSelector::CLogicalPartitionSelector(CMemoryPool *mp)
	: CLogical(mp), m_mdid(NULL), m_pdrspqexprFilters(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::CLogicalPartitionSelector
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalPartitionSelector::CLogicalPartitionSelector(
	CMemoryPool *mp, IMDId *mdid, CExpressionArray *pdrspqexprFilters)
	: CLogical(mp), m_mdid(mdid), m_pdrspqexprFilters(pdrspqexprFilters)
{
	SPQOS_ASSERT(mdid->IsValid());
	SPQOS_ASSERT(NULL != pdrspqexprFilters);
	SPQOS_ASSERT(0 < pdrspqexprFilters->Size());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::~CLogicalPartitionSelector
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalPartitionSelector::~CLogicalPartitionSelector()
{
	CRefCount::SafeRelease(m_mdid);
	CRefCount::SafeRelease(m_pdrspqexprFilters);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::Matches
//
//	@doc:
//		Match operators
//
//---------------------------------------------------------------------------
BOOL
CLogicalPartitionSelector::Matches(COperator *pop) const
{
	if (Eopid() != pop->Eopid())
	{
		return false;
	}

	CLogicalPartitionSelector *popPartSelector =
		CLogicalPartitionSelector::PopConvert(pop);

	return popPartSelector->MDId()->Equals(m_mdid) &&
		   popPartSelector->m_pdrspqexprFilters->Equals(m_pdrspqexprFilters);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::HashValue
//
//	@doc:
//		Hash operator
//
//---------------------------------------------------------------------------
ULONG
CLogicalPartitionSelector::HashValue() const
{
	return spqos::CombineHashes(Eopid(), m_mdid->HashValue());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalPartitionSelector::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL /* must_exist */)
{
	CExpressionArray *pdrspqexpr =
		CUtils::PdrspqexprRemap(mp, m_pdrspqexprFilters, colref_mapping);

	m_mdid->AddRef();

	return SPQOS_NEW(mp) CLogicalPartitionSelector(mp, m_mdid, pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalPartitionSelector::DeriveOutputColumns(CMemoryPool *mp,
											   CExpressionHandle &exprhdl)
{
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);

	pcrsOutput->Union(exprhdl.DeriveOutputColumns(0));

	return pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalPartitionSelector::DeriveMaxCard(CMemoryPool *,	 // mp
										 CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalPartitionSelector::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementPartitionSelector);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalPartitionSelector::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalPartitionSelector::OsPrint(IOstream &os) const
{
	os << SzId() << ", Part Table: ";
	m_mdid->OsPrint(os);

	return os;
}

// EOF
