//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLLogicalCTEAnchor.h
//
//	@doc:
//		Class for representing DXL logical CTE anchors
//---------------------------------------------------------------------------
#ifndef SPQDXL_CDXLLogicalCTEAnchor_H
#define SPQDXL_CDXLLogicalCTEAnchor_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogical.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLLogicalCTEAnchor
//
//	@doc:
//		Class for representing DXL logical CTE producers
//
//---------------------------------------------------------------------------
class CDXLLogicalCTEAnchor : public CDXLLogical
{
private:
	// cte id
	ULONG m_id;

	// private copy ctor
	CDXLLogicalCTEAnchor(CDXLLogicalCTEAnchor &);

public:
	// ctor
	CDXLLogicalCTEAnchor(CMemoryPool *mp, ULONG id);

	// operator type
	Edxlopid GetDXLOperator() const;

	// operator name
	const CWStringConst *GetOpNameStr() const;

	// cte identifier
	ULONG
	Id() const
	{
		return m_id;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;


#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLLogicalCTEAnchor *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopLogicalCTEAnchor == dxl_op->GetDXLOperator());
		return dynamic_cast<CDXLLogicalCTEAnchor *>(dxl_op);
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLLogicalCTEAnchor_H

// EOF
