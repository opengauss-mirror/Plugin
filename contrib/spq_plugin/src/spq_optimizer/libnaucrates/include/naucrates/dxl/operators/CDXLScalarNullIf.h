//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLScalarNullIf.h
//
//	@doc:
//		Class for representing DXL scalar NullIf operator
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarNullIf_H
#define SPQDXL_CDXLScalarNullIf_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarNullIf
//
//	@doc:
//		Class for representing DXL scalar NullIf operator
//
//---------------------------------------------------------------------------
class CDXLScalarNullIf : public CDXLScalar
{
private:
	// operator number
	IMDId *m_mdid_op;

	// return type
	IMDId *m_mdid_type;

	// private copy ctor
	CDXLScalarNullIf(CDXLScalarNullIf &);

public:
	// ctor
	CDXLScalarNullIf(CMemoryPool *mp, IMDId *mdid_op, IMDId *mdid_type);

	// dtor
	virtual ~CDXLScalarNullIf();

	// ident accessors
	virtual Edxlopid GetDXLOperator() const;

	// name of the DXL operator
	const CWStringConst *GetOpNameStr() const;

	// operator id
	virtual IMDId *MdIdOp() const;

	// return type
	virtual IMDId *MdidType() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

	// conversion function
	static CDXLScalarNullIf *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarNullIf == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarNullIf *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarNullIf_H


// EOF
