//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarConstValue.h
//
//	@doc:
//		Class for representing DXL Const Value
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarConstValue_H
#define SPQDXL_CDXLScalarConstValue_H

#include "spqos/base.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/dxl/operators/CDXLDatum.h"
#include "naucrates/dxl/operators/CDXLScalar.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarConstValue
//
//	@doc:
//		Class for representing DXL scalar Const value
//
//---------------------------------------------------------------------------
class CDXLScalarConstValue : public CDXLScalar
{
private:
	CDXLDatum *m_dxl_datum;

	// private copy ctor
	CDXLScalarConstValue(const CDXLScalarConstValue &);

public:
	// ctor/dtor
	CDXLScalarConstValue(CMemoryPool *mp, CDXLDatum *dxl_datum);

	virtual ~CDXLScalarConstValue();

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// return the datum value
	const CDXLDatum *
	GetDatumVal() const
	{
		return m_dxl_datum;
	}

	// DXL Operator ID
	Edxlopid GetDXLOperator() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarConstValue *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarConstValue == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarConstValue *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure
	void AssertValid(const CDXLNode *dxlnode, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLScalarConstValue_H

// EOF
