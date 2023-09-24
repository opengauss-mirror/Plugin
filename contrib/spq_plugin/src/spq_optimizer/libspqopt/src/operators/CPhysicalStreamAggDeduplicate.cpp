//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalStreamAggDeduplicate.cpp
//
//	@doc:
//		Implementation of stream aggregation operator for deduplicating join outputs
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalStreamAggDeduplicate.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGbAgg.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAggDeduplicate::CPhysicalStreamAggDeduplicate
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalStreamAggDeduplicate::CPhysicalStreamAggDeduplicate(
	CMemoryPool *mp, CColRefArray *colref_array, CColRefArray *pdrspqcrMinimal,
	COperator::EGbAggType egbaggtype, CColRefArray *pdrspqcrKeys,
	BOOL fGeneratesDuplicates, BOOL fMultiStage, BOOL isAggFromSplitDQA,
	CLogicalGbAgg::EAggStage aggStage, BOOL should_enforce_distribution)
	: CPhysicalStreamAgg(mp, colref_array, pdrspqcrMinimal, egbaggtype,
						 fGeneratesDuplicates, NULL /*pdrspqcrGbMinusDistinct*/,
						 fMultiStage, isAggFromSplitDQA, aggStage,
						 should_enforce_distribution),
	  m_pdrspqcrKeys(pdrspqcrKeys)
{
	SPQOS_ASSERT(NULL != pdrspqcrKeys);
	InitOrderSpec(mp, m_pdrspqcrKeys);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAggDeduplicate::~CPhysicalStreamAggDeduplicate
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalStreamAggDeduplicate::~CPhysicalStreamAggDeduplicate()
{
	m_pdrspqcrKeys->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAggDeduplicate::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalStreamAggDeduplicate::OsPrint(IOstream &os) const
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
