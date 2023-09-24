//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarProjList.h
//
//	@doc:
//		Class for representing DXL projection lists.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarProjList_H
#define SPQDXL_CDXLScalarProjList_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarProjList
//
//	@doc:
//		Projection list in operators.
//
//---------------------------------------------------------------------------
class CDXLScalarProjList : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarProjList(CDXLScalarProjList &);

public:
	// ctor/dtor
	explicit CDXLScalarProjList(CMemoryPool *mp);

	virtual ~CDXLScalarProjList(){};

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// conversion function
	static CDXLScalarProjList *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarProjectList == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarProjList *>(dxl_op);
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
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *dxlnode, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarProjList_H

// EOF
