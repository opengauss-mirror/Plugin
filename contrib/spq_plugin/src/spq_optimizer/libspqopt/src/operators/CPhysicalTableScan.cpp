//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPhysicalTableScan.cpp
//
//	@doc:
//		Implementation of basic table scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalTableScan.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTableScan::CPhysicalTableScan
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CPhysicalTableScan::CPhysicalTableScan(CMemoryPool *mp, const CName *pnameAlias,
									   CTableDescriptor *ptabdesc,
									   CColRefArray *pdrspqcrOutput)
	: CPhysicalScan(mp, pnameAlias, ptabdesc, pdrspqcrOutput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTableScan::HashValue
//
//	@doc:
//		Combine pointer for table descriptor and Eop
//
//---------------------------------------------------------------------------
ULONG
CPhysicalTableScan::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTableScan::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalTableScan::Matches(COperator *pop) const
{
	if (Eopid() != pop->Eopid())
	{
		return false;
	}

	CPhysicalTableScan *popTableScan = CPhysicalTableScan::PopConvert(pop);
	return m_ptabdesc->MDId()->Equals(popTableScan->Ptabdesc()->MDId()) &&
		   m_pdrspqcrOutput->Equals(popTableScan->PdrspqcrOutput());
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTableScan::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalTableScan::OsPrint(IOstream &os) const
{
	os << SzId() << " ";

	// alias of table as referenced in the query
	m_pnameAlias->OsPrint(os);

	// actual name of table in catalog and columns
	os << " (";
	m_ptabdesc->Name().OsPrint(os);
	os << ")";

	return os;
}



// EOF
