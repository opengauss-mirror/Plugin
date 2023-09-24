//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC, Corp.
//
//	@filename:
//		CDXLScalarSubqueryNotExists.h
//
//	@doc:
//		Class for representing NOT EXISTS subqueries
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarSubqueryNotExists_H
#define SPQDXL_CDXLScalarSubqueryNotExists_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLScalar.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarSubqueryNotExists
//
//	@doc:
//		Class for representing NOT EXISTS subqueries
//
//---------------------------------------------------------------------------
class CDXLScalarSubqueryNotExists : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarSubqueryNotExists(CDXLScalarSubqueryNotExists &);

public:
	// ctor/dtor
	explicit CDXLScalarSubqueryNotExists(CMemoryPool *mp);

	virtual ~CDXLScalarSubqueryNotExists();

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// conversion function
	static CDXLScalarSubqueryNotExists *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarSubqueryNotExists == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarSubqueryNotExists *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		return true;
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *dxlnode, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl


#endif	// !SPQDXL_CDXLScalarSubqueryNotExists_H

// EOF
