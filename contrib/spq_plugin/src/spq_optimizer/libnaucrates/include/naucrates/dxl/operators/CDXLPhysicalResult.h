//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalResult.h
//
//	@doc:
//		Class for representing DXL physical result operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalResult_H
#define SPQDXL_CDXLPhysicalResult_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
// indices of result elements in the children array
enum Edxlresult
{
	EdxlresultIndexProjList = 0,
	EdxlresultIndexFilter,
	EdxlresultIndexOneTimeFilter,
	EdxlresultIndexChild,
	EdxlresultIndexSentinel
};
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalResult
//
//	@doc:
//		Class for representing DXL result operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalResult : public CDXLPhysical
{
private:
	// private copy ctor
	CDXLPhysicalResult(CDXLPhysicalResult &);

public:
	// ctor/dtor
	explicit CDXLPhysicalResult(CMemoryPool *mp);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalResult *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalResult == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalResult *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalResult_H

// EOF
