//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarLimitCount.h
//
//	@doc:
//		Class for representing DXL scalar LimitCount (Limit Count Node is only used by CDXLPhysicalLimit
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarLimitCount_H
#define SPQDXL_CDXLScalarLimitCount_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"


namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarLimitCount
//
//	@doc:
//		Class for representing DXL scalar LimitCount
//
//---------------------------------------------------------------------------
class CDXLScalarLimitCount : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarLimitCount(const CDXLScalarLimitCount &);

public:
	// ctor/dtor
	explicit CDXLScalarLimitCount(CMemoryPool *mp);

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the DXL operator
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// conversion function
	static CDXLScalarLimitCount *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarLimitCount == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarLimitCount *>(dxl_op);
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
#endif	// !SPQDXL_CDXLScalarLimitCount_H

// EOF
