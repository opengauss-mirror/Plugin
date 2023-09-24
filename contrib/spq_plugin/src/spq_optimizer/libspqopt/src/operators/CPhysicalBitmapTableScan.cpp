//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CPhysicalBitmapTableScan.cpp
//
//	@doc:
//		Bitmap table scan physical operator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalBitmapTableScan.h"

#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CTableDescriptor.h"

using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalBitmapTableScan::CPhysicalBitmapTableScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalBitmapTableScan::CPhysicalBitmapTableScan(CMemoryPool *mp,
												   CTableDescriptor *ptabdesc,
												   ULONG ulOriginOpId,
												   const CName *pnameTableAlias,
												   CColRefArray *pdrspqcrOutput)
	: CPhysicalScan(mp, pnameTableAlias, ptabdesc, pdrspqcrOutput),
	  m_ulOriginOpId(ulOriginOpId)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalBitmapTableScan::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CPhysicalBitmapTableScan::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalBitmapTableScan::HashValue
//
//	@doc:
//		Match this operator with the given one.
//
//---------------------------------------------------------------------------
BOOL
CPhysicalBitmapTableScan::Matches(COperator *pop) const
{
	return CUtils::FMatchBitmapScan(this, pop);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalBitmapTableScan::OsPrint
//
//	@doc:
//		Debug print of a CPhysicalBitmapTableScan
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalBitmapTableScan::OsPrint(IOstream &os) const
{
	os << SzId() << " ";
	os << ", Table Name: (";
	m_ptabdesc->Name().OsPrint(os);
	os << ")";
	os << ", Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "]";

	return os;
}

// EOF
