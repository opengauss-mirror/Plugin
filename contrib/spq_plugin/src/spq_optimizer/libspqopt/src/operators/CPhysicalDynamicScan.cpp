//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CPhysicalDynamicScan.cpp
//
//	@doc:
//		Base class for physical dynamic scan operators
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalDynamicScan.h"

#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CPartConstraint.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicScan::CPhysicalDynamicScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalDynamicScan::CPhysicalDynamicScan(
	CMemoryPool *mp, BOOL is_partial, CTableDescriptor *ptabdesc,
	ULONG ulOriginOpId, const CName *pnameAlias, ULONG scan_id,
	CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrParts,
	ULONG ulSecondaryScanId, CPartConstraint *ppartcnstr,
	CPartConstraint *ppartcnstrRel)
	: CPhysicalScan(mp, pnameAlias, ptabdesc, pdrspqcrOutput),
	  m_ulOriginOpId(ulOriginOpId),
	  m_is_partial(is_partial),
	  m_scan_id(scan_id),
	  m_pdrspqdrspqcrPart(pdrspqdrspqcrParts),
	  m_ulSecondaryScanId(ulSecondaryScanId),
	  m_part_constraint(ppartcnstr),
	  m_ppartcnstrRel(ppartcnstrRel)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcrParts);
	SPQOS_ASSERT(0 < pdrspqdrspqcrParts->Size());
	SPQOS_ASSERT(NULL != ppartcnstr);
	SPQOS_ASSERT(NULL != ppartcnstrRel);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicScan::~CPhysicalDynamicScan
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalDynamicScan::~CPhysicalDynamicScan()
{
	m_pdrspqdrspqcrPart->Release();
	m_part_constraint->Release();
	m_ppartcnstrRel->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicScan::HashValue
//
//	@doc:
//		Combine part index, pointer for table descriptor, Eop and output columns
//
//---------------------------------------------------------------------------
ULONG
CPhysicalDynamicScan::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(spqos::HashValue(&m_scan_id),
							m_ptabdesc->MDId()->HashValue()));
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicScan::PpimDerive
//
//	@doc:
//		Derive partition index map
//
//---------------------------------------------------------------------------
CPartIndexMap *
CPhysicalDynamicScan::PpimDerive(CMemoryPool *mp,
								 CExpressionHandle &,  //exprhdl
								 CDrvdPropCtxt *pdpctxt) const
{
	SPQOS_ASSERT(NULL != pdpctxt);
	IMDId *mdid = m_ptabdesc->MDId();
	mdid->AddRef();
	m_pdrspqdrspqcrPart->AddRef();
	m_part_constraint->AddRef();
	m_ppartcnstrRel->AddRef();
	ULONG ulExpectedPartitionSelectors =
		CDrvdPropCtxtPlan::PdpctxtplanConvert(pdpctxt)
			->UlExpectedPartitionSelectors();

	return PpimDeriveFromDynamicScan(
		mp, m_scan_id, mdid, m_pdrspqdrspqcrPart, m_ulSecondaryScanId,
		m_part_constraint, m_ppartcnstrRel, ulExpectedPartitionSelectors);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicScan::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalDynamicScan::OsPrint(IOstream &os) const
{
	os << SzId() << " ";

	// alias of table as referenced in the query
	m_pnameAlias->OsPrint(os);

	// actual name of table in catalog and columns
	os << " (";
	m_ptabdesc->Name().OsPrint(os);
	os << "), Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "] Scan Id: " << m_scan_id << "." << m_ulSecondaryScanId;

	if (!m_part_constraint->IsConstraintUnbounded())
	{
		os << ", ";
		m_part_constraint->OsPrint(os);
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicScan::PopConvert
//
//	@doc:
//		conversion function
//
//---------------------------------------------------------------------------
CPhysicalDynamicScan *
CPhysicalDynamicScan::PopConvert(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(CUtils::FPhysicalScan(pop) &&
				CPhysicalScan::PopConvert(pop)->FDynamicScan());

	return dynamic_cast<CPhysicalDynamicScan *>(pop);
}


// EOF
