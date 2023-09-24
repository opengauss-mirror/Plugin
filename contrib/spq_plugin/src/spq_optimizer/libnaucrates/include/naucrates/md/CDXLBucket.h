//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLBucket.h
//
//	@doc:
//		Class representing buckets in a DXL column stats histogram
//---------------------------------------------------------------------------



#ifndef SPQMD_CDXLBucket_H
#define SPQMD_CDXLBucket_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/common/CDynamicPtrArray.h"

#include "naucrates/dxl/operators/CDXLDatum.h"

namespace spqdxl
{
class CXMLSerializer;
}

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CDXLBucket
//
//	@doc:
//		Class representing a bucket in DXL column stats
//
//---------------------------------------------------------------------------
class CDXLBucket : public CRefCount
{
private:
	// lower bound value for the bucket
	CDXLDatum *m_lower_bound_dxl_datum;

	// max value for the bucket
	CDXLDatum *m_upper_bound_dxl_datum;

	// is lower bound closed (i.e., the boundary point is included in the bucket)
	BOOL m_is_lower_closed;

	// is upper bound closed (i.e., the boundary point is included in the bucket)
	BOOL m_is_upper_closed;

	// frequency
	CDouble m_frequency;

	// distinct values
	CDouble m_distinct;

	// private copy ctor
	CDXLBucket(const CDXLBucket &);

	// serialize the bucket boundary
	void SerializeBoundaryValue(CXMLSerializer *xml_serializer,
								const CWStringConst *elem_str,
								CDXLDatum *dxl_datum,
								BOOL is_bound_closed) const;

public:
	// ctor
	CDXLBucket(CDXLDatum *dxl_datum_lower, CDXLDatum *dxl_datum_upper,
			   BOOL is_lower_closed, BOOL is_upper_closed, CDouble frequency,
			   CDouble distinct);

	// dtor
	virtual ~CDXLBucket();

	// is lower bound closed
	BOOL
	IsLowerClosed() const
	{
		return m_is_lower_closed;
	}

	// is upper bound closed
	BOOL
	IsUpperClosed() const
	{
		return m_is_upper_closed;
	}

	// min value for the bucket
	const CDXLDatum *GetDXLDatumLower() const;

	// max value for the bucket
	const CDXLDatum *GetDXLDatumUpper() const;

	// frequency
	CDouble GetFrequency() const;

	// distinct values
	CDouble GetNumDistinct() const;

	// serialize bucket in DXL format
	void Serialize(spqdxl::CXMLSerializer *) const;

#ifdef SPQOS_DEBUG
	// debug print of the bucket
	void DebugPrint(IOstream &os) const;
#endif
};

// array of dxl buckets
typedef CDynamicPtrArray<CDXLBucket, CleanupRelease> CDXLBucketArray;
}  // namespace spqmd

#endif	// !SPQMD_CDXLBucket_H

// EOF
