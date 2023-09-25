//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDRelStats.h
//
//	@doc:
//		Interface for relation stats
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDRelStats_H
#define SPQMD_IMDRelStats_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"

#include "naucrates/md/IMDCacheObject.h"

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		IMDRelStats
//
//	@doc:
//		Interface for relation stats
//
//---------------------------------------------------------------------------
class IMDRelStats : public IMDCacheObject
{
public:
	// object type
	virtual Emdtype
	MDType() const
	{
		return EmdtRelStats;
	}

	// number of rows
	virtual CDouble Rows() const = 0;

	virtual ULONG RelPages() const = 0;

	virtual ULONG RelAllVisible() const = 0;

	// is statistics on an empty input
	virtual BOOL IsEmpty() const = 0;
};
}  // namespace spqmd

#endif	// !SPQMD_IMDRelStats_H

// EOF
