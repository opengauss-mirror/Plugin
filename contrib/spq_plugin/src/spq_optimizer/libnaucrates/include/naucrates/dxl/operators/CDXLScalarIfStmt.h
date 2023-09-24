//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarIfStmt.h
//
//	@doc:
//
//		Class for representing DXL If Statement (Case Statement is represented as a nested if statement)
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarIfStmt_H
#define SPQDXL_CDXLScalarIfStmt_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarIfStmt
//
//	@doc:
//		Class for representing DXL IF statement
//
//---------------------------------------------------------------------------
class CDXLScalarIfStmt : public CDXLScalar
{
private:
	// catalog MDId of the return type
	IMDId *m_result_type_mdid;

	// private copy ctor
	CDXLScalarIfStmt(const CDXLScalarIfStmt &);

public:
	// ctor
	CDXLScalarIfStmt(CMemoryPool *mp, IMDId *mdid_type);

	//dtor
	virtual ~CDXLScalarIfStmt();

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	IMDId *GetResultTypeMdId() const;

	// DXL Operator ID
	Edxlopid GetDXLOperator() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLScalarIfStmt *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarIfStmt == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarIfStmt *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *node, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLScalarIfStmt_H

// EOF
