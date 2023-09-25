//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC, Corp.
//
//	@filename:
//		CDXLScalarSubqueryAll.h
//
//	@doc:
//		Class for representing ALL subqueries
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLScalarSubqueryAll_H
#define SPQDXL_CDXLScalarSubqueryAll_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLScalarSubqueryQuantified.h"
#include "naucrates/md/CMDName.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarSubqueryAll
//
//	@doc:
//		Class for representing ALL subqueries
//
//---------------------------------------------------------------------------
class CDXLScalarSubqueryAll : public CDXLScalarSubqueryQuantified
{
private:
	// private copy ctor
	CDXLScalarSubqueryAll(CDXLScalarSubqueryAll &);

public:
	// ctor
	CDXLScalarSubqueryAll(CMemoryPool *mp, IMDId *scalar_op_mdid,
						  CMDName *mdname, ULONG colid);

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// conversion function
	static CDXLScalarSubqueryAll *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarSubqueryAll == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarSubqueryAll *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		return true;
	}
};
}  // namespace spqdxl


#endif	// !SPQDXL_CDXLScalarSubqueryAll_H

// EOF
