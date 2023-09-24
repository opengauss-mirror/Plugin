//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CDXLScalarArrayRef.h
//
//	@doc:
//		Class for representing DXL scalar arrayrefs
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarArrayRef_H
#define SPQDXL_CDXLScalarArrayRef_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarArrayRef
//
//	@doc:
//		Class for representing DXL scalar arrayrefs
//
//---------------------------------------------------------------------------
class CDXLScalarArrayRef : public CDXLScalar
{
private:
	// base element type id
	IMDId *m_elem_type_mdid;

	// element type modifier
	INT m_type_modifier;

	// array type id
	IMDId *m_array_type_mdid;

	// return type id
	IMDId *m_return_type_mdid;

	// private copy ctor
	CDXLScalarArrayRef(const CDXLScalarArrayRef &);

public:
	// ctor
	CDXLScalarArrayRef(CMemoryPool *mp, IMDId *elem_type_mdid,
					   INT type_modifier, IMDId *array_type_mdid,
					   IMDId *return_type_mdid);

	// dtor
	virtual ~CDXLScalarArrayRef();

	// ident accessors
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// element type id
	IMDId *
	ElementTypeMDid() const
	{
		return m_elem_type_mdid;
	}

	// element type modifier
	INT TypeModifier() const;

	// array type id
	IMDId *
	ArrayTypeMDid() const
	{
		return m_array_type_mdid;
	}

	// return type id
	IMDId *
	ReturnTypeMDid() const
	{
		return m_return_type_mdid;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLScalarArrayRef *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarArrayRef == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarArrayRef *>(dxl_op);
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarArrayRef_H

// EOF
