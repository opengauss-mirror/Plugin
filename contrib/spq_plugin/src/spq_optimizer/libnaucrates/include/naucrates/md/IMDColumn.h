//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		IMDColumn.h
//
//	@doc:
//		Interface class for columns in a metadata cache relation
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDColumn_H
#define SPQMD_IMDColumn_H

#include "spqos/base.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDInterface.h"

namespace spqmd
{
using namespace spqos;

class CMDName;

//---------------------------------------------------------------------------
//	@class:
//		IMDColumn
//
//	@doc:
//		Interface class for columns in a metadata cache relation
//
//---------------------------------------------------------------------------
class IMDColumn : public IMDInterface
{
public:
	// column name
	virtual CMDName Mdname() const = 0;

	// id of attribute type
	virtual IMDId *MdidType() const = 0;

	virtual INT TypeModifier() const = 0;

	// are nulls allowed for this column
	virtual BOOL IsNullable() const = 0;

	// attribute number in the system catalog
	virtual INT AttrNum() const = 0;

	// is this a system column
	virtual BOOL IsSystemColumn() const = 0;

	// is column dropped
	virtual BOOL IsDropped() const = 0;

	// length of the column
	virtual ULONG Length() const = 0;

#ifdef SPQOS_DEBUG
	// debug print of the column
	virtual void DebugPrint(IOstream &os) const = 0;
#endif
};

// IMDColumn array
//	typedef CDynamicPtrArray<IMDColumn, CleanupRelease> CMDColumnArray;

}  // namespace spqmd



#endif	// !SPQMD_IMDColumn_H

// EOF
