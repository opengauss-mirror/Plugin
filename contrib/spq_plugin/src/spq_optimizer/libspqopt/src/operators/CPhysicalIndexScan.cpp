//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalIndexScan.cpp
//
//	@doc:
//		Implementation of index scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalIndexScan.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalIndexScan::CPhysicalIndexScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalIndexScan::CPhysicalIndexScan(
	CMemoryPool *mp, CIndexDescriptor *pindexdesc, CTableDescriptor *ptabdesc,
	ULONG ulOriginOpId, const CName *pnameAlias, CColRefArray *pdrspqcrOutput,
	COrderSpec *pos)
	: CPhysicalScan(mp, pnameAlias, ptabdesc, pdrspqcrOutput),
	  m_pindexdesc(pindexdesc),
	  m_ulOriginOpId(ulOriginOpId),
	  m_pos(pos)
{
	SPQOS_ASSERT(NULL != pindexdesc);
	SPQOS_ASSERT(NULL != pos);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalIndexScan::~CPhysicalIndexScan
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalIndexScan::~CPhysicalIndexScan()
{
	m_pindexdesc->Release();
	m_pos->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalIndexScan::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalIndexScan::EpetOrder(CExpressionHandle &,	// exprhdl
							  const CEnfdOrder *peo) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	if (peo->FCompatible(m_pos))
	{
		// required order is already established by the index
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalIndexScan::HashValue
//
//	@doc:
//		Combine pointers for table descriptor, index descriptor and Eop
//
//---------------------------------------------------------------------------
ULONG
CPhysicalIndexScan::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(m_pindexdesc->MDId()->HashValue(),
							spqos::HashPtr<CTableDescriptor>(m_ptabdesc)));
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalIndexScan::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalIndexScan::Matches(COperator *pop) const
{
	return CUtils::FMatchIndex(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalIndexScan::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalIndexScan::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " ";
	// index name
	os << "  Index Name: (";
	m_pindexdesc->Name().OsPrint(os);
	// table name
	os << ")";
	os << ", Table Name: (";
	m_ptabdesc->Name().OsPrint(os);
	os << ")";
	os << ", Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "]";

	return os;
}

// EOF
