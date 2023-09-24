//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CDXLScalarBitmapBoolOp.h
//
//	@doc:
//		Class for representing DXL bitmap boolean operator
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarBitmapBoolOp_H
#define SPQDXL_CDXLScalarBitmapBoolOp_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"


namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarBitmapBoolOp
//
//	@doc:
//		Class for representing DXL bitmap boolean operator
//
//---------------------------------------------------------------------------
class CDXLScalarBitmapBoolOp : public CDXLScalar
{
public:
	// type of bitmap operator
	enum EdxlBitmapBoolOp
	{
		EdxlbitmapAnd,
		EdxlbitmapOr,
		EdxlbitmapSentinel
	};

private:
	// type id
	IMDId *m_mdid_type;

	// operator type
	const EdxlBitmapBoolOp m_bitmap_op_type;

	// private copy ctor
	CDXLScalarBitmapBoolOp(const CDXLScalarBitmapBoolOp &);

public:
	// ctor
	CDXLScalarBitmapBoolOp(CMemoryPool *mp, IMDId *mdid_type,
						   EdxlBitmapBoolOp bitmap_op_type);

	// dtor
	virtual ~CDXLScalarBitmapBoolOp();

	// dxl operator type
	virtual Edxlopid GetDXLOperator() const;

	// bitmap operator type
	EdxlBitmapBoolOp GetDXLBitmapOpType() const;

	// return type
	IMDId *MdidType() const;

	// name of the DXL operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;


#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLScalarBitmapBoolOp *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarBitmapBoolOp == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarBitmapBoolOp *>(dxl_op);
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarBitmapBoolOp_H

// EOF
