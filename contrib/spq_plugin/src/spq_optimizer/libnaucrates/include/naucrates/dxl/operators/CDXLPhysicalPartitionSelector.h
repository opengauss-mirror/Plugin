//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLPhysicalPartitionSelector.h
//
//	@doc:
//		Class for representing DXL partition selector
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalPartitionSelector_H
#define SPQDXL_CDXLPhysicalPartitionSelector_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
// indices of partition selector elements in the children array
enum Edxlps
{
	EdxlpsIndexProjList = 0,
	EdxlpsIndexEqFilters,
	EdxlpsIndexFilters,
	EdxlpsIndexResidualFilter,
	EdxlpsIndexPropExpr,
	EdxlpsIndexPrintableFilter,
	EdxlpsIndexChild,
	EdxlpsIndexSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalPartitionSelector
//
//	@doc:
//		Class for representing DXL partition selector
//
//---------------------------------------------------------------------------
class CDXLPhysicalPartitionSelector : public CDXLPhysical
{
private:
	// table id
	IMDId *m_rel_mdid;

	// number of partitioning levels
	ULONG m_num_of_part_levels;

	// scan id
	ULONG m_scan_id;

	// private copy ctor
	CDXLPhysicalPartitionSelector(CDXLPhysicalPartitionSelector &);

public:
	// ctor
	CDXLPhysicalPartitionSelector(CMemoryPool *mp, IMDId *mdid_rel,
								  ULONG num_of_part_levels, ULONG scan_id);

	// dtor
	virtual ~CDXLPhysicalPartitionSelector();

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// table id
	IMDId *
	GetRelMdId() const
	{
		return m_rel_mdid;
	}

	// number of partitioning levels
	ULONG
	GetPartitioningLevel() const
	{
		return m_num_of_part_levels;
	}

	// scan id
	ULONG
	ScanId() const
	{
		return m_scan_id;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLPhysicalPartitionSelector *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalPartitionSelector ==
					dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalPartitionSelector *>(dxl_op);
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalPartitionSelector_H

// EOF
