//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLLogicalJoin.h
//
//	@doc:
//		Class for representing DXL logical Join operators
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CDXLLogicalJoin_H
#define SPQDXL_CDXLLogicalJoin_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogical.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLLogicalJoin
//
//	@doc:
//		Class for representing DXL logical Join operators
//
//---------------------------------------------------------------------------
class CDXLLogicalJoin : public CDXLLogical
{
private:
	// private copy ctor
	CDXLLogicalJoin(CDXLLogicalJoin &);

	// join type (inner, outer, ...)
	EdxlJoinType m_join_type;

public:
	// ctor/dtor
	CDXLLogicalJoin(CMemoryPool *, EdxlJoinType);

	// accessors
	Edxlopid GetDXLOperator() const;

	const CWStringConst *GetOpNameStr() const;

	// join type
	EdxlJoinType GetJoinType() const;

	const CWStringConst *GetJoinTypeNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLLogicalJoin *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopLogicalJoin == dxl_op->GetDXLOperator());
		return dynamic_cast<CDXLLogicalJoin *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLLogicalJoin_H

// EOF
