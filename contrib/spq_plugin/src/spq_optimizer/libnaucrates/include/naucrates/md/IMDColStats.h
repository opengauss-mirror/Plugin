//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDColStats.h
//
//	@doc:
//		Interface for column stats
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDColStats_H
#define SPQMD_IMDColStats_H

#include "spqos/base.h"

#include "naucrates/md/CDXLBucket.h"
#include "naucrates/md/IMDCacheObject.h"

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		IMDColStats
//
//	@doc:
//		Interface for column stats
//
//---------------------------------------------------------------------------
class IMDColStats : public IMDCacheObject
{
public:
	// object type
	virtual Emdtype
	MDType() const
	{
		return EmdtColStats;
	}

	// number of buckets
	virtual ULONG Buckets() const = 0;

	// width
	virtual CDouble Width() const = 0;

	// null fraction
	virtual CDouble GetNullFreq() const = 0;

	// ndistinct of remaining tuples
	virtual CDouble GetDistinctRemain() const = 0;

	// frequency of remaining tuples
	virtual CDouble GetFreqRemain() const = 0;

	// is the columns statistics missing in the database
	virtual BOOL IsColStatsMissing() const = 0;

	// get the bucket at the given position
	virtual const CDXLBucket *GetDXLBucketAt(ULONG ul) const = 0;
};
}  // namespace spqmd


#endif	// !SPQMD_IMDColStats_H

// EOF
