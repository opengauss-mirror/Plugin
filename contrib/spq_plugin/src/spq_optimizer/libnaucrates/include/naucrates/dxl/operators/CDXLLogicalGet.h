//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLLogicalGet.h
//
//	@doc:
//		Class for representing DXL logical get operators
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLLogicalGet_H
#define SPQDXL_CDXLLogicalGet_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogical.h"
#include "naucrates/dxl/operators/CDXLTableDescr.h"


namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLLogicalGet
//
//	@doc:
//		Class for representing DXL logical get operators
//
//---------------------------------------------------------------------------
class CDXLLogicalGet : public CDXLLogical
{
private:
	// table descriptor for the scanned table
	CDXLTableDescr *m_dxl_table_descr;

	// private copy ctor
	CDXLLogicalGet(CDXLLogicalGet &);

public:
	// ctor
	CDXLLogicalGet(CMemoryPool *mp, CDXLTableDescr *table_descr);

	// dtor
	virtual ~CDXLLogicalGet();

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;
	CDXLTableDescr *GetDXLTableDescr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// check if given column is defined by operator
	virtual BOOL IsColDefined(ULONG colid) const;

	// conversion function
	static CDXLLogicalGet *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopLogicalGet == dxl_op->GetDXLOperator() ||
					EdxlopLogicalExternalGet == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLLogicalGet *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLLogicalGet_H

// EOF
