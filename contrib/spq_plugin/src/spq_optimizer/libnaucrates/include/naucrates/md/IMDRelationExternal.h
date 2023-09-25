//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		IMDRelationExternal.h
//
//	@doc:
//		Interface for external relations in the metadata cache
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDRelationExternal_H
#define SPQMD_IMDRelationExternal_H

#include "spqos/base.h"

#include "naucrates/md/IMDRelation.h"

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		IMDRelationExternal
//
//	@doc:
//		Interface for external relations in the metadata cache
//
//---------------------------------------------------------------------------
class IMDRelationExternal : public IMDRelation
{
public:
	// storage type
	virtual Erelstoragetype
	RetrieveRelStorageType() const
	{
		return ErelstorageExternal;
	}

	// is this a temp relation
	virtual BOOL
	IsTemporary() const
	{
		return false;
	}

	// is this a partitioned table
	virtual BOOL
	IsPartitioned() const
	{
		return false;
	}

	// return true if a hash distributed table needs to be considered as random
	virtual BOOL ConvertHashToRandom() const = 0;

	// does this table have oids
	virtual BOOL
	HasOids() const
	{
		return false;
	}

	// number of partition columns
	virtual ULONG
	PartColumnCount() const
	{
		return 0;
	}

	// number of partitions
	virtual ULONG
	PartitionCount() const
	{
		return 0;
	}

	// retrieve the partition column at the given position
	virtual const IMDColumn *PartColAt(ULONG /*pos*/) const
	{
		SPQOS_ASSERT(!"External tables have no partition columns");
		return NULL;
	}

	// retrieve list of partition types
	virtual CharPtrArray *
	GetPartitionTypes() const
	{
		SPQOS_ASSERT(!"External tables have no partition types");
		return NULL;
	}

	// retrieve the partition type at the given position
	virtual CHAR PartTypeAtLevel(ULONG /*pos*/) const
	{
		SPQOS_ASSERT(!"External tables have no partition types");
		return (CHAR) 0;
	}

	// reject limit
	virtual INT RejectLimit() const = 0;

	// reject limit in rows?
	virtual BOOL IsRejectLimitInRows() const = 0;

	// format error table mdid
	virtual IMDId *GetFormatErrTableMdid() const = 0;
};
}  // namespace spqmd

#endif	// !SPQMD_IMDRelationExternal_H

// EOF
