//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CIndexDescriptor.cpp
//
//	@doc:
//		Implementation of index description
//---------------------------------------------------------------------------

#include "spqopt/metadata/CIndexDescriptor.h"

#include "spqos/base.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CIndexDescriptor);

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptor::CIndexDescriptor
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CIndexDescriptor::CIndexDescriptor(
	CMemoryPool *mp, IMDId *pmdidIndex, const CName &name,
	CColumnDescriptorArray *pdrgcoldescKeyCols,
	CColumnDescriptorArray *pdrgcoldescIncludedCols, BOOL is_clustered,
	IMDIndex::EmdindexType index_type)
	: m_pmdidIndex(pmdidIndex),
	  m_name(mp, name),
	  m_pdrspqcoldescKeyCols(pdrgcoldescKeyCols),
	  m_pdrspqcoldescIncludedCols(pdrgcoldescIncludedCols),
	  m_clustered(is_clustered),
	  m_index_type(index_type)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(pmdidIndex->IsValid());
	SPQOS_ASSERT(NULL != pdrgcoldescKeyCols);
	SPQOS_ASSERT(NULL != pdrgcoldescIncludedCols);
}

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptor::~CIndexDescriptor
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CIndexDescriptor::~CIndexDescriptor()
{
	m_pmdidIndex->Release();

	m_pdrspqcoldescKeyCols->Release();
	m_pdrspqcoldescIncludedCols->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptor::Keys
//
//	@doc:
//		number of key columns
//
//---------------------------------------------------------------------------
ULONG
CIndexDescriptor::Keys() const
{
	return m_pdrspqcoldescKeyCols->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptor::UlIncludedColumns
//
//	@doc:
//		Number of included columns
//
//---------------------------------------------------------------------------
ULONG
CIndexDescriptor::UlIncludedColumns() const
{
	// array allocated in ctor
	SPQOS_ASSERT(NULL != m_pdrspqcoldescIncludedCols);

	return m_pdrspqcoldescIncludedCols->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptor::Pindexdesc
//
//	@doc:
//		Create the index descriptor from the table descriptor and index
//		information from the catalog
//
//---------------------------------------------------------------------------
CIndexDescriptor *
CIndexDescriptor::Pindexdesc(CMemoryPool *mp, const CTableDescriptor *ptabdesc,
							 const IMDIndex *pmdindex)
{
	CWStringConst strIndexName(mp, pmdindex->Mdname().GetMDName()->GetBuffer());

	CColumnDescriptorArray *pdrspqcoldesc = ptabdesc->Pdrspqcoldesc();

	pmdindex->MDId()->AddRef();

	// array of index column descriptors
	CColumnDescriptorArray *pdrgcoldescKey =
		SPQOS_NEW(mp) CColumnDescriptorArray(mp);

	for (ULONG ul = 0; ul < pmdindex->Keys(); ul++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldesc)[ul];
		pcoldesc->AddRef();
		pdrgcoldescKey->Append(pcoldesc);
	}

	// array of included column descriptors
	CColumnDescriptorArray *pdrgcoldescIncluded =
		SPQOS_NEW(mp) CColumnDescriptorArray(mp);
	for (ULONG ul = 0; ul < pmdindex->IncludedCols(); ul++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldesc)[ul];
		pcoldesc->AddRef();
		pdrgcoldescIncluded->Append(pcoldesc);
	}


	// create the index descriptors
	CIndexDescriptor *pindexdesc = SPQOS_NEW(mp) CIndexDescriptor(
		mp, pmdindex->MDId(), CName(&strIndexName), pdrgcoldescKey,
		pdrgcoldescIncluded, pmdindex->IsClustered(), pmdindex->IndexType());
	return pindexdesc;
}

BOOL
CIndexDescriptor::SupportsIndexOnlyScan() const
{
	return m_index_type == IMDIndex::EmdindBtree;
}

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptor::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CIndexDescriptor::OsPrint(IOstream &os) const
{
	m_name.OsPrint(os);
	os << ": (Keys :";
	CUtils::OsPrintDrgPcoldesc(os, m_pdrspqcoldescKeyCols,
							   m_pdrspqcoldescKeyCols->Size());
	os << "); ";

	os << "(Included Columns :";
	CUtils::OsPrintDrgPcoldesc(os, m_pdrspqcoldescIncludedCols,
							   m_pdrspqcoldescIncludedCols->Size());
	os << ")";

	os << " [ Clustered :";
	if (m_clustered)
	{
		os << "true";
	}
	else
	{
		os << "false";
	}
	os << " ]";
	return os;
}

// EOF
