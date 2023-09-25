//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CIndexDescriptor.h
//
//	@doc:
//		Base class for index descriptor
//---------------------------------------------------------------------------
#ifndef SPQOPT_CIndexDescriptor_H
#define SPQOPT_CIndexDescriptor_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/metadata/CColumnDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDIndex.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CIndexDescriptor
//
//	@doc:
//		Base class for index descriptor
//
//---------------------------------------------------------------------------
class CIndexDescriptor : public CRefCount,
						 public DbgPrintMixin<CIndexDescriptor>
{
private:
	// mdid of the index
	IMDId *m_pmdidIndex;

	// name of index
	CName m_name;

	// array of index key columns
	CColumnDescriptorArray *m_pdrspqcoldescKeyCols;

	// array of index included columns
	CColumnDescriptorArray *m_pdrspqcoldescIncludedCols;

	// clustered index
	BOOL m_clustered;

	// index type
	IMDIndex::EmdindexType m_index_type;

	// private copy ctor
	CIndexDescriptor(const CIndexDescriptor &);

public:
	// ctor
	CIndexDescriptor(CMemoryPool *mp, IMDId *pmdidIndex, const CName &name,
					 CColumnDescriptorArray *pdrgcoldescKeyCols,
					 CColumnDescriptorArray *pdrgcoldescIncludedCols,
					 BOOL is_clustered, IMDIndex::EmdindexType emdindt);

	// dtor
	virtual ~CIndexDescriptor();

	// number of key columns
	ULONG Keys() const;

	// number of included columns
	ULONG UlIncludedColumns() const;

	// index mdid accessor
	IMDId *
	MDId() const
	{
		return m_pmdidIndex;
	}

	// index name
	const CName &
	Name() const
	{
		return m_name;
	}

	// key column descriptors
	CColumnDescriptorArray *
	PdrspqcoldescKey() const
	{
		return m_pdrspqcoldescKeyCols;
	}

	// included column descriptors
	CColumnDescriptorArray *
	PdrspqcoldescIncluded() const
	{
		return m_pdrspqcoldescIncludedCols;
	}

	// is index clustered
	BOOL
	IsClustered() const
	{
		return m_clustered;
	}

	IMDIndex::EmdindexType
	IndexType() const
	{
		return m_index_type;
	}

	BOOL SupportsIndexOnlyScan() const;

	// create an index descriptor
	static CIndexDescriptor *Pindexdesc(CMemoryPool *mp,
										const CTableDescriptor *ptabdesc,
										const IMDIndex *pmdindex);

	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CIndexDescriptor
}  // namespace spqopt

#endif	// !SPQOPT_CIndexDescriptor_H

// EOF
