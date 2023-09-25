//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLScalarDMLAction.h
//
//	@doc:
//		Class for representing DXL DML action expressions
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarDMLAction_H
#define SPQDXL_CDXLScalarDMLAction_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarDMLAction
//
//	@doc:
//		Class for representing DXL DML action expressions
//
//---------------------------------------------------------------------------
class CDXLScalarDMLAction : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarDMLAction(const CDXLScalarDMLAction &);

public:
	// ctor/dtor
	explicit CDXLScalarDMLAction(CMemoryPool *mp);

	virtual ~CDXLScalarDMLAction()
	{
	}

	// ident accessors
	Edxlopid GetDXLOperator() const;

	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarDMLAction *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarDMLAction == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarDMLAction *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *node, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarDMLAction_H

// EOF
