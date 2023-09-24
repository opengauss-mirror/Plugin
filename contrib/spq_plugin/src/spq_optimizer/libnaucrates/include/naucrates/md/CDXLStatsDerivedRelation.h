//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLStatsDerivedRelation.h
//
//	@doc:
//		Class representing DXL derived relation statistics
//---------------------------------------------------------------------------

#ifndef SPQMD_CDXLStatsDerivedRelation_H
#define SPQMD_CDXLStatsDerivedRelation_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"

#include "naucrates/md/CDXLStatsDerivedColumn.h"

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
//		CDXLStatsDerivedRelation
//
//	@doc:
//		Class representing DXL derived relation statistics
//
//---------------------------------------------------------------------------
class CDXLStatsDerivedRelation : public CRefCount
{
private:
	// number of rows
	CDouble m_rows;

	// flag to indicate if input relation is empty
	BOOL m_empty;

	// array of derived column statistics
	CDXLStatsDerivedColumnArray *m_dxl_stats_derived_col_array;

	// private copy ctor
	CDXLStatsDerivedRelation(const CDXLStatsDerivedRelation &);

public:
	// ctor
	CDXLStatsDerivedRelation(
		CDouble rows, BOOL is_empty,
		CDXLStatsDerivedColumnArray *dxl_stats_derived_col_array);

	// dtor
	virtual ~CDXLStatsDerivedRelation();

	// number of rows
	CDouble
	Rows() const
	{
		return m_rows;
	}

	// is statistics on an empty input
	virtual BOOL
	IsEmpty() const
	{
		return m_empty;
	}

	// derived column statistics
	const CDXLStatsDerivedColumnArray *GetDXLStatsDerivedColArray() const;

	// serialize bucket in DXL format
	void Serialize(spqdxl::CXMLSerializer *) const;

#ifdef SPQOS_DEBUG
	// debug print of the bucket
	void DebugPrint(IOstream &os) const;
#endif
};

// array of dxl buckets
typedef CDynamicPtrArray<CDXLStatsDerivedRelation, CleanupRelease>
	CDXLStatsDerivedRelationArray;
}  // namespace spqmd

#endif	// !SPQMD_CDXLStatsDerivedRelation_H

// EOF
