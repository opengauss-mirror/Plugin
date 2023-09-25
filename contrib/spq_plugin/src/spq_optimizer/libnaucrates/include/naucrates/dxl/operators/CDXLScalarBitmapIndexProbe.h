//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarBitmapIndexProbe.h
//
//	@doc:
//		Class for representing DXL bitmap index probe operators
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarBitmapIndexProbe_H
#define SPQDXL_CDXLScalarBitmapIndexProbe_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"

namespace spqdxl
{
using namespace spqos;

// fwd declarations
class CDXLIndexDescr;
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarBitmapIndexProbe
//
//	@doc:
//		Class for representing DXL bitmap index probe operators
//
//---------------------------------------------------------------------------
class CDXLScalarBitmapIndexProbe : public CDXLScalar
{
private:
	// index descriptor associated with the scanned table
	CDXLIndexDescr *m_dxl_index_descr;

	// disable copy ctor
	CDXLScalarBitmapIndexProbe(CDXLScalarBitmapIndexProbe &);

public:
	// ctor
	CDXLScalarBitmapIndexProbe(CMemoryPool *mp,
							   CDXLIndexDescr *dxl_index_descr);

	//dtor
	virtual ~CDXLScalarBitmapIndexProbe();

	// operator type
	virtual Edxlopid
	GetDXLOperator() const
	{
		return EdxlopScalarBitmapIndexProbe;
	}

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// index descriptor
	virtual const CDXLIndexDescr *
	GetDXLIndexDescr() const
	{
		return m_dxl_index_descr;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		return false;
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLScalarBitmapIndexProbe *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarBitmapIndexProbe == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarBitmapIndexProbe *>(dxl_op);
	}

};	// class CDXLScalarBitmapIndexProbe
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarBitmapIndexProbe_H

// EOF
