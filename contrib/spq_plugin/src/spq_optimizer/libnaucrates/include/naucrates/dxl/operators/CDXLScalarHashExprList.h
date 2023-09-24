//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarHashExprList.h
//
//	@doc:
//		Class for representing hash expressions list in DXL Redistribute motion nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarHashExprList_H
#define SPQDXL_CDXLScalarHashExprList_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarHashExprList
//
//	@doc:
//		Hash expressions list in Redistribute motion nodes
//
//---------------------------------------------------------------------------
class CDXLScalarHashExprList : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarHashExprList(CDXLScalarHashExprList &);

public:
	// ctor/dtor
	explicit CDXLScalarHashExprList(CMemoryPool *mp);

	virtual ~CDXLScalarHashExprList(){};

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarHashExprList *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarHashExprList == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarHashExprList *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		SPQOS_ASSERT(!"Invalid function call on a container operator");
		return false;
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure
	void AssertValid(const CDXLNode *node, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarHashExprList_H

// EOF
