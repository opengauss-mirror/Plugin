//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarDistinctComp.h
//
//	@doc:
//		Class for representing DXL scalar "is distinct from" comparison operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarDistinctComp_H
#define SPQDXL_CDXLScalarDistinctComp_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarComp.h"


namespace spqdxl
{
// indices of scalar distinct comparison elements in the children array
enum Edxlscdistcmp
{
	EdxlscdistcmpIndexLeft = 0,
	EdxlscdistcmpIndexRight,
	EdxlscdistcmpSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarDistinctComp
//
//	@doc:
//		Class for representing DXL scalar "is distinct from" comparison operators
//
//---------------------------------------------------------------------------
class CDXLScalarDistinctComp : public CDXLScalarComp
{
private:
	// private copy ctor
	CDXLScalarDistinctComp(CDXLScalarDistinctComp &);

public:
	// ctor/dtor
	CDXLScalarDistinctComp(CMemoryPool *mp, IMDId *operator_mdid);

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the DXL operator
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarDistinctComp *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarDistinct == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarDistinctComp *>(dxl_op);
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
	void AssertValid(const CDXLNode *node, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarDistinctComp_H


// EOF
