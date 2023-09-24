//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLColDescr.h
//
//	@doc:
//		Class for representing column descriptors.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLColDescr_H
#define SPQDXL_CDXLColDescr_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDName.h"

namespace spqdxl
{
using namespace spqmd;

// fwd decl
class CXMLSerializer;
class CDXLColDescr;

typedef CDynamicPtrArray<CDXLColDescr, CleanupRelease> CDXLColDescrArray;

//---------------------------------------------------------------------------
//	@class:
//		CDXLColDescr
//
//	@doc:
//		Class for representing column descriptors in DXL operators
//
//---------------------------------------------------------------------------
class CDXLColDescr : public CRefCount
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// name
	CMDName *m_md_name;

	// column id: unique identifier of that instance of the column in the query
	ULONG m_column_id;

	// attribute number in the database (corresponds to varattno in SPQDB)
	INT m_attr_no;

	// mdid of column's type
	IMDId *m_column_mdid_type;

	INT m_type_modifier;

	// is column dropped from the table: needed for correct restoring of attribute numbers in the range table entries
	BOOL m_is_dropped;

	// width of the column, for instance  char(10) column has width 10
	ULONG m_column_width;

	// private copy ctor
	CDXLColDescr(const CDXLColDescr &);

public:
	// ctor
	CDXLColDescr(CMemoryPool *, CMDName *, ULONG column_id, INT attr_no,
				 IMDId *column_mdid_type, INT type_modifier, BOOL is_dropped,
				 ULONG width = spqos::ulong_max);

	//dtor
	~CDXLColDescr();

	// column name
	const CMDName *MdName() const;

	// column identifier
	ULONG Id() const;

	// attribute number of the column in the base table
	INT AttrNum() const;

	// is the column dropped in the base table
	BOOL IsDropped() const;

	// column type
	IMDId *MdidType() const;

	INT TypeModifier() const;

	// column width
	ULONG Width() const;

	void SerializeToDXL(CXMLSerializer *xml_serializer) const;
};

}  // namespace spqdxl



#endif	// !SPQDXL_CDXLColDescr_H

// EOF
