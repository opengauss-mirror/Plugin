//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDIndexSPQDB.h
//
//	@doc:
//		Implementation of indexes in the metadata cache
//---------------------------------------------------------------------------



#ifndef SPQMD_CMDIndexSPQDB_H
#define SPQMD_CMDIndexSPQDB_H

#include "spqos/base.h"

#include "naucrates/md/IMDIndex.h"

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

// fwd decl
class IMDPartConstraint;

//---------------------------------------------------------------------------
//	@class:
//		CMDIndexSPQDB
//
//	@doc:
//		Class for indexes in the metadata cache
//
//---------------------------------------------------------------------------
class CMDIndexSPQDB : public IMDIndex
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// index mdid
	IMDId *m_mdid;

	// table name
	CMDName *m_mdname;

	// is the index clustered
	BOOL m_clustered;

	// index type
	EmdindexType m_index_type;

	// index physical type represents the format structure on disk. Indexes on
	// partitioned tables are stored as "logical" indexes in m_index_type. A
	// "logical" index may be different than the physical index.
	EmdindexType m_index_physical_type;

	// type of items returned by index
	IMDId *m_mdid_item_type;

	// index key columns
	ULongPtrArray *m_index_key_cols_array;

	// included columns
	ULongPtrArray *m_included_cols_array;

	// operator families for each index key
	IMdIdArray *m_mdid_opfamilies_array;

	// partition constraint
	IMDPartConstraint *m_mdpart_constraint;

	// DXL for object
	const CWStringDynamic *m_dxl_str;

	// private copy ctor
	CMDIndexSPQDB(const CMDIndexSPQDB &);

public:
	// ctor
	CMDIndexSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
				 BOOL is_clustered, EmdindexType index_type,
				 EmdindexType index_physical_type, IMDId *mdid_item_type,
				 ULongPtrArray *index_key_cols_array,
				 ULongPtrArray *included_cols_array,
				 IMdIdArray *mdid_opfamilies_array,
				 IMDPartConstraint *mdpart_constraint);

	// dtor
	virtual ~CMDIndexSPQDB();

	// index mdid
	virtual IMDId *MDId() const;

	// index name
	virtual CMDName Mdname() const;

	// is the index clustered
	virtual BOOL IsClustered() const;

	// index type
	virtual EmdindexType IndexType() const;

	// index physical type
	virtual EmdindexType IndexPhysicalType() const;

	// number of keys
	virtual ULONG Keys() const;

	// return the n-th key column
	virtual ULONG KeyAt(ULONG pos) const;

	// return the position of the key column
	virtual ULONG GetKeyPos(ULONG column) const;

	// number of included columns
	virtual ULONG IncludedCols() const;

	// return the n-th included column
	virtual ULONG IncludedColAt(ULONG pos) const;

	// return the position of the included column
	virtual ULONG GetIncludedColPos(ULONG column) const;

	// part constraint
	virtual IMDPartConstraint *MDPartConstraint() const;

	// DXL string for index
	virtual const CWStringDynamic *
	GetStrRepr() const
	{
		return m_dxl_str;
	}

	// serialize MD index in DXL format given a serializer object
	virtual void Serialize(spqdxl::CXMLSerializer *) const;

	// type id of items returned by the index
	virtual IMDId *GetIndexRetItemTypeMdid() const;

	// check if given scalar comparison can be used with the index key
	// at the specified position
	virtual BOOL IsCompatible(const IMDScalarOp *md_scalar_op,
							  ULONG key_pos) const;

#ifdef SPQOS_DEBUG
	// debug print of the MD index
	virtual void DebugPrint(IOstream &os) const;
#endif
};
}  // namespace spqmd

#endif	// !SPQMD_CMDIndexSPQDB_H

// EOF
