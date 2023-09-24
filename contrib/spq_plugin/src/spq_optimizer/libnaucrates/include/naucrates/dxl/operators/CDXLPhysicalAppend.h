//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalAppend.h
//
//	@doc:
//		Class for representing DXL append operators.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLPhysicalAppend_H
#define SPQDXL_CDXLPhysicalAppend_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
// indices of append elements in the children array
enum Edxlappend
{
	EdxlappendIndexProjList = 0,
	EdxlappendIndexFilter,
	EdxlappendIndexFirstChild,
	EdxlappendIndexSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalAppend
//
//	@doc:
//		Class for representing DXL Append operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalAppend : public CDXLPhysical
{
private:
	// is the append node used in an update/delete statement
	BOOL m_used_in_upd_del;

	// TODO:  - Apr 12, 2011; find a better name (and comments) for this variable
	BOOL m_is_zapped;

	// private copy ctor
	CDXLPhysicalAppend(const CDXLPhysicalAppend &);

public:
	// ctor/dtor
	CDXLPhysicalAppend(CMemoryPool *mp, BOOL fIsTarget, BOOL fIsZapped);

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;

	BOOL IsUsedInUpdDel() const;
	BOOL IsZapped() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalAppend *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalAppend == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalAppend *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalAppend_H

// EOF
