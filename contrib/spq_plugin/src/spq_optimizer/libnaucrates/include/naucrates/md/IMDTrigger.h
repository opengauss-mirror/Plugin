//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDTrigger.h
//
//	@doc:
//		Interface for triggers in the metadata cache
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDTrigger_H
#define SPQMD_IMDTrigger_H

#include "spqos/base.h"

#include "naucrates/md/IMDCacheObject.h"

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		IMDTrigger
//
//	@doc:
//		Interface for triggers in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTrigger : public IMDCacheObject
{
public:
	// object type
	virtual Emdtype
	MDType() const
	{
		return EmdtTrigger;
	}

	// does trigger execute on a row-level
	virtual BOOL ExecutesOnRowLevel() const = 0;

	// is this a before trigger
	virtual BOOL IsBefore() const = 0;

	// is this an insert trigger
	virtual BOOL IsInsert() const = 0;

	// is this a delete trigger
	virtual BOOL IsDelete() const = 0;

	// is this an update trigger
	virtual BOOL IsUpdate() const = 0;

	// relation mdid
	virtual IMDId *GetRelMdId() const = 0;

	// function mdid
	virtual IMDId *FuncMdId() const = 0;

	// is trigger enabled
	virtual BOOL IsEnabled() const = 0;
};
}  // namespace spqmd

#endif	// !SPQMD_IMDTrigger_H

// EOF
