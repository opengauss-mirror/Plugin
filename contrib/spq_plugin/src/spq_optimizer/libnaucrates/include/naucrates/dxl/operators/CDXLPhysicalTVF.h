//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLPhysicalTVF.h
//
//	@doc:
//		Class for representing DXL physical table-valued functions
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalTVF_H
#define SPQDXL_CDXLPhysicalTVF_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLColDescr.h"
#include "naucrates/dxl/operators/CDXLPhysical.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalTVF
//
//	@doc:
//		Class for representing DXL physical table-valued functions
//
//---------------------------------------------------------------------------
class CDXLPhysicalTVF : public CDXLPhysical
{
private:
	// function mdid
	IMDId *m_func_mdid;

	// return type
	IMDId *m_return_type_mdid;

	// function name
	CWStringConst *func_name;

	// private copy ctor
	CDXLPhysicalTVF(const CDXLPhysicalTVF &);

public:
	// ctor
	CDXLPhysicalTVF(CMemoryPool *mp, IMDId *mdid_func, IMDId *mdid_return_type,
					CWStringConst *str);

	// dtor
	virtual ~CDXLPhysicalTVF();

	// get operator type
	Edxlopid GetDXLOperator() const;

	// get operator name
	const CWStringConst *GetOpNameStr() const;

	// get function name
	CWStringConst *
	Pstr() const
	{
		return func_name;
	}

	// get function id
	IMDId *
	FuncMdId() const
	{
		return m_func_mdid;
	}

	// get return type
	IMDId *
	ReturnTypeMdId() const
	{
		return m_return_type_mdid;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalTVF *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalTVF == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalTVF *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLPhysicalTVF_H

// EOF
