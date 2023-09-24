//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarOneTimeFilter.h
//
//	@doc:
//		Class for representing a scalar filter that is executed once inside DXL physical operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarOneTimeFilter_H
#define SPQDXL_CDXLScalarOneTimeFilter_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarFilter.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarOneTimeFilter
//
//	@doc:
//		Class for representing DXL filter operators
//
//---------------------------------------------------------------------------
class CDXLScalarOneTimeFilter : public CDXLScalarFilter
{
private:
	// private copy ctor
	CDXLScalarOneTimeFilter(CDXLScalarOneTimeFilter &);

public:
	// ctor
	explicit CDXLScalarOneTimeFilter(CMemoryPool *mp);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	// conversion function
	static CDXLScalarOneTimeFilter *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarOneTimeFilter == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarOneTimeFilter *>(dxl_op);
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		SPQOS_ASSERT(!"Invalid function call for a container operator");
		return false;
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLScalarOneTimeFilter_H

// EOF
