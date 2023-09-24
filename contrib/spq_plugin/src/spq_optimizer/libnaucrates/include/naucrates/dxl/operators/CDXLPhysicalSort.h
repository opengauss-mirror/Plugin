//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalSort.h
//
//	@doc:
//		Class for representing DXL sort operators.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLPhysicalSort_H
#define SPQDXL_CDXLPhysicalSort_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
// indices of sort node elements in the children array
enum Edxlsort
{
	EdxlsortIndexProjList = 0,
	EdxlsortIndexFilter,
	EdxlsortIndexSortColList,
	EdxlsortIndexLimitCount,
	EdxlsortIndexLimitOffset,
	EdxlsortIndexChild,
	EdxlsortIndexSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalSort
//
//	@doc:
//		Class for representing DXL sort operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalSort : public CDXLPhysical
{
private:
	// private copy ctor
	CDXLPhysicalSort(const CDXLPhysicalSort &);

	// whether sort discards duplicates
	BOOL m_discard_duplicates;


public:
	// ctor/dtor
	CDXLPhysicalSort(CMemoryPool *mp, BOOL discard_duplicates);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;
	BOOL FDiscardDuplicates() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalSort *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalSort == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalSort *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalSort_H

// EOF
