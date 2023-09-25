//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarIdent.h
//
//	@doc:
//		Class for representing DXL scalar identifier operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarIdent_H
#define SPQDXL_CDXLScalarIdent_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLColRef.h"
#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarIdent
//
//	@doc:
//		Class for representing DXL scalar identifier operators.
//
//---------------------------------------------------------------------------
class CDXLScalarIdent : public CDXLScalar
{
private:
	// column reference
	CDXLColRef *m_dxl_colref;

	// private copy ctor
	CDXLScalarIdent(CDXLScalarIdent &);

public:
	// ctor/dtor
	CDXLScalarIdent(CMemoryPool *, CDXLColRef *);

	virtual ~CDXLScalarIdent();

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// accessors
	const CDXLColRef *GetDXLColRef() const;

	IMDId *MdidType() const;

	INT TypeModifier() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarIdent *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarIdent == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarIdent *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure
	void AssertValid(const CDXLNode *node, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl



#endif	// !SPQDXL_CDXLScalarIdent_H

// EOF
