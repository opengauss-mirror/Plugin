//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLPhysicalSequence.h
//
//	@doc:
//		Class for representing DXL physical sequence operators
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalSequence_H
#define SPQDXL_CDXLPhysicalSequence_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"
#include "naucrates/dxl/operators/CDXLSpoolInfo.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalSequence
//
//	@doc:
//		Class for representing DXL physical sequence operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalSequence : public CDXLPhysical
{
private:
	// private copy ctor
	CDXLPhysicalSequence(CDXLPhysicalSequence &);

public:
	// ctor
	CDXLPhysicalSequence(CMemoryPool *mp);

	// dtor
	virtual ~CDXLPhysicalSequence();

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalSequence *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalSequence == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalSequence *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalSequence_H

// EOF
