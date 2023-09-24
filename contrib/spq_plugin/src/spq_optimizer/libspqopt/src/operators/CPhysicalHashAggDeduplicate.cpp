//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalHashAggDeduplicate.cpp
//
//	@doc:
//		Implementation of Hash Aggregate operator for deduplicating join outputs
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalHashAggDeduplicate.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGbAgg.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAggDeduplicate::CPhysicalHashAggDeduplicate
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalHashAggDeduplicate::CPhysicalHashAggDeduplicate(
	CMemoryPool *mp, CColRefArray *colref_array, CColRefArray *pdrspqcrMinimal,
	COperator::EGbAggType egbaggtype, CColRefArray *pdrspqcrKeys,
	BOOL fGeneratesDuplicates, BOOL fMultiStage, BOOL isAggFromSplitDQA,
	CLogicalGbAgg::EAggStage aggStage, BOOL should_enforce_distribution)
	: CPhysicalHashAgg(mp, colref_array, pdrspqcrMinimal, egbaggtype,
					   fGeneratesDuplicates, NULL /*pdrspqcrGbMinusDistinct*/,
					   fMultiStage, isAggFromSplitDQA, aggStage,
					   should_enforce_distribution),
	  m_pdrspqcrKeys(pdrspqcrKeys)
{
	SPQOS_ASSERT(NULL != pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAggDeduplicate::~CPhysicalHashAggDeduplicate
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalHashAggDeduplicate::~CPhysicalHashAggDeduplicate()
{
	m_pdrspqcrKeys->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAggDeduplicate::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalHashAggDeduplicate::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << "( ";
	CLogicalGbAgg::OsPrintGbAggType(os, Egbaggtype());
	os << " )"
	   << " Grp Cols: [";

	CUtils::OsPrintDrgPcr(os, PdrspqcrGroupingCols());
	os << "]"
	   << ", Key Cols:[";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrKeys);
	os << "]";

	os << ", Generates Duplicates :[ " << FGeneratesDuplicates() << " ] ";

	return os;
}

// EOF

// EOF
