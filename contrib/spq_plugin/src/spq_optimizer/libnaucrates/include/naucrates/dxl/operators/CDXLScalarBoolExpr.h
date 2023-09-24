//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarBoolExpr.h
//
//	@doc:
//		Class for representing DXL BoolExpr
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarBoolExpr_H
#define SPQDXL_CDXLScalarBoolExpr_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"


namespace spqdxl
{
using namespace spqos;

enum EdxlBoolExprType
{
	Edxland,
	Edxlor,
	Edxlnot,
	EdxlBoolExprTypeSentinel
};


//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarBoolExpr
//
//	@doc:
//		Class for representing DXL BoolExpr
//
//---------------------------------------------------------------------------
class CDXLScalarBoolExpr : public CDXLScalar
{
private:
	// operator type
	const EdxlBoolExprType m_bool_type;

	// private copy ctor
	CDXLScalarBoolExpr(const CDXLScalarBoolExpr &);

public:
	// ctor/dtor
	explicit CDXLScalarBoolExpr(CMemoryPool *mp,
								const EdxlBoolExprType bool_type);


	// ident accessors
	Edxlopid GetDXLOperator() const;

	// BoolExpr operator type
	EdxlBoolExprType GetDxlBoolTypeStr() const;

	// name of the DXL operator name
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// conversion function
	static CDXLScalarBoolExpr *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarBoolExpr == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarBoolExpr *>(dxl_op);
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

#endif	// !SPQDXL_CDXLScalarBoolExpr_H

// EOF
