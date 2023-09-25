//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarJoinFilter.h
//
//	@doc:
//		Class for representing a join filter node inside DXL join operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarJoinFilter_H
#define SPQDXL_CDXLScalarJoinFilter_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarFilter.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarJoinFilter
//
//	@doc:
//		Class for representing DXL join condition operators
//
//---------------------------------------------------------------------------
class CDXLScalarJoinFilter : public CDXLScalarFilter
{
private:
	// private copy ctor
	CDXLScalarJoinFilter(CDXLScalarJoinFilter &);

public:
	// ctor/dtor
	explicit CDXLScalarJoinFilter(CMemoryPool *mp);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// conversion function
	static CDXLScalarJoinFilter *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarJoinFilter == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarJoinFilter *>(dxl_op);
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
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *node, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLScalarJoinFilter_H

// EOF
