//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLScalarSwitchCase.h
//
//	@doc:
//
//		Class for representing a single DXL switch case
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarSwitchCase_H
#define SPQDXL_CDXLScalarSwitchCase_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarSwitchCase
//
//	@doc:
//		Class for representing DXL Switch Case
//
//---------------------------------------------------------------------------
class CDXLScalarSwitchCase : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarSwitchCase(const CDXLScalarSwitchCase &);

public:
	// ctor
	explicit CDXLScalarSwitchCase(CMemoryPool *mp);

	//dtor
	virtual ~CDXLScalarSwitchCase()
	{
	}

	// name of the operator
	virtual const CWStringConst *GetOpNameStr() const;

	// DXL Operator ID
	virtual Edxlopid GetDXLOperator() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLScalarSwitchCase *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarSwitchCase == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarSwitchCase *>(dxl_op);
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
	void AssertValid(const CDXLNode *dxlnode, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLScalarSwitchCase_H

// EOF
