//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarArrayComp.h
//
//	@doc:
//		Class for representing DXL scalar array comparison such as in, not in, any, all
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarArrayComp_H
#define SPQDXL_CDXLScalarArrayComp_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarComp.h"


namespace spqdxl
{
using namespace spqos;

enum EdxlArrayCompType
{
	Edxlarraycomptypeany = 0,
	Edxlarraycomptypeall
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarArrayComp
//
//	@doc:
//		Class for representing DXL scalar Array OpExpr
//
//---------------------------------------------------------------------------
class CDXLScalarArrayComp : public CDXLScalarComp
{
private:
	EdxlArrayCompType m_comparison_type;

	// private copy ctor
	CDXLScalarArrayComp(const CDXLScalarArrayComp &);

	const CWStringConst *GetDXLStrArrayCmpType() const;

public:
	// ctor/dtor
	CDXLScalarArrayComp(CMemoryPool *mp, IMDId *mdid_op,
						const CWStringConst *str_opname,
						EdxlArrayCompType comparison_type);

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the DXL operator
	const CWStringConst *GetOpNameStr() const;

	//accessors
	BOOL HasBoolResult() const;
	EdxlArrayCompType GetDXLArrayCmpType() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLScalarArrayComp *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarArrayComp == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarArrayComp *>(dxl_op);
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

#endif	// !SPQDXL_CDXLScalarArrayComp_H

// EOF
