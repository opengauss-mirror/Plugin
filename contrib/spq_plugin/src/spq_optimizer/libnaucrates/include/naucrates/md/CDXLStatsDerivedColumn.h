//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLStatsDerivedColumn.h
//
//	@doc:
//		Class representing DXL derived column statistics
//---------------------------------------------------------------------------

#ifndef SPQMD_CDXLStatsDerivedColumn_H
#define SPQMD_CDXLStatsDerivedColumn_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"

#include "naucrates/md/CDXLBucket.h"

namespace spqdxl
{
class CXMLSerializer;
}

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLStatsDerivedColumn
//
//	@doc:
//		Class representing DXL derived column statistics
//
//---------------------------------------------------------------------------
class CDXLStatsDerivedColumn : public CRefCount
{
private:
	// column identifier
	ULONG m_colid;

	// column width
	CDouble m_width;

	// null fraction
	CDouble m_null_freq;

	// ndistinct of remaining tuples
	CDouble m_distinct_remaining;

	// frequency of remaining tuples
	CDouble m_freq_remaining;

	CDXLBucketArray *m_dxl_stats_bucket_array;

	// private copy ctor
	CDXLStatsDerivedColumn(const CDXLStatsDerivedColumn &);

public:
	// ctor
	CDXLStatsDerivedColumn(ULONG colid, CDouble width, CDouble null_freq,
						   CDouble distinct_remaining, CDouble freq_remaining,
						   CDXLBucketArray *dxl_stats_bucket_array);

	// dtor
	virtual ~CDXLStatsDerivedColumn();

	// column identifier
	ULONG
	GetColId() const
	{
		return m_colid;
	}

	// column width
	CDouble
	Width() const
	{
		return m_width;
	}

	// null fraction of this column
	CDouble
	GetNullFreq() const
	{
		return m_null_freq;
	}

	// ndistinct of remaining tuples
	CDouble
	GetDistinctRemain() const
	{
		return m_distinct_remaining;
	}

	// frequency of remaining tuples
	CDouble
	GetFreqRemain() const
	{
		return m_freq_remaining;
	}

	const CDXLBucketArray *TransformHistogramToDXLBucketArray() const;

	// serialize bucket in DXL format
	void Serialize(spqdxl::CXMLSerializer *) const;

#ifdef SPQOS_DEBUG
	// debug print of the bucket
	void DebugPrint(IOstream &os) const;
#endif
};

// array of dxl buckets
typedef CDynamicPtrArray<CDXLStatsDerivedColumn, CleanupRelease>
	CDXLStatsDerivedColumnArray;
}  // namespace spqmd

#endif	// !SPQMD_CDXLStatsDerivedColumn_H

// EOF
