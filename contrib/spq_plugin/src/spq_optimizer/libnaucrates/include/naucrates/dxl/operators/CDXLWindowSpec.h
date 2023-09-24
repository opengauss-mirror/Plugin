//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLWindowSpec.h
//
//	@doc:
//		Class for representing DXL window specification in the DXL
//		representation of the logical query tree
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLWindowSpec_H
#define SPQDXL_CDXLWindowSpec_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLWindowFrame.h"
#include "naucrates/md/CMDName.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLWindowSpec
//
//	@doc:
//		Class for representing DXL window specification in the DXL
//		representation of the logical query tree
//
//---------------------------------------------------------------------------
class CDXLWindowSpec : public CRefCount
{
private:
	// memory pool;
	CMemoryPool *m_mp;

	// partition-by column identifiers
	ULongPtrArray *m_partition_by_colid_array;

	// name of window specification
	CMDName *m_mdname;

	// sorting columns
	CDXLNode *m_sort_col_list_dxlnode;

	// window frame associated with the window key
	CDXLWindowFrame *m_window_frame;

	// private copy ctor
	CDXLWindowSpec(const CDXLWindowSpec &);

public:
	// ctor
	CDXLWindowSpec(CMemoryPool *mp, ULongPtrArray *partition_by_colid_array,
				   CMDName *mdname, CDXLNode *sort_col_list_dxlnode,
				   CDXLWindowFrame *window_frame);

	// dtor
	virtual ~CDXLWindowSpec();

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *) const;

	// set window frame definition
	void SetWindowFrame(CDXLWindowFrame *window_frame);

	// return window frame
	CDXLWindowFrame *
	GetWindowFrame() const
	{
		return m_window_frame;
	}

	// partition-by column identifiers
	ULongPtrArray *
	GetPartitionByColIdArray() const
	{
		return m_partition_by_colid_array;
	}

	// sort columns
	CDXLNode *
	GetSortColListDXL() const
	{
		return m_sort_col_list_dxlnode;
	}

	// window specification name
	CMDName *
	MdName() const
	{
		return m_mdname;
	}
};

typedef CDynamicPtrArray<CDXLWindowSpec, CleanupRelease> CDXLWindowSpecArray;
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLWindowSpec_H

// EOF
