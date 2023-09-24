//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLLogicalSelect.h
//
//	@doc:
//		Class for representing DXL logical select operators
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CDXLLogicalSelect_H
#define SPQDXL_CDXLLogicalSelect_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogical.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLLogicalSelect
//
//	@doc:
//		Class for representing DXL Logical Select operators
//
//---------------------------------------------------------------------------
class CDXLLogicalSelect : public CDXLLogical
{
private:
	// private copy ctor
	CDXLLogicalSelect(CDXLLogicalSelect &);

public:
	// ctor/dtor
	explicit CDXLLogicalSelect(CMemoryPool *);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLLogicalSelect *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopLogicalSelect == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLLogicalSelect *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLLogicalSelect_H

// EOF
