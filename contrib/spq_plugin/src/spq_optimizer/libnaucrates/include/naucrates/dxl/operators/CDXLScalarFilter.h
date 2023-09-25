//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarFilter.h
//
//	@doc:
//		Class for representing a scalar filter node inside DXL physical operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarFilter_H
#define SPQDXL_CDXLScalarFilter_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarFilter
//
//	@doc:
//		Class for representing DXL filter operators
//
//---------------------------------------------------------------------------
class CDXLScalarFilter : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarFilter(CDXLScalarFilter &);

public:
	// ctor/dtor
	explicit CDXLScalarFilter(CMemoryPool *mp);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarFilter *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarFilter == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarFilter *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		SPQOS_ASSERT(!"Invalid function call for a container operator");
		return false;
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure
	virtual void AssertValid(const CDXLNode *node,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLScalarFilter_H

// EOF
