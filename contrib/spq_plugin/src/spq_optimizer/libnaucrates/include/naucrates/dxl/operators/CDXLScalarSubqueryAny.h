//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC, Corp.
//
//	@filename:
//		CDXLScalarSubqueryAny.h
//
//	@doc:
//		Class for representing ANY subqueries
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarSubqueryAny_H
#define SPQDXL_CDXLScalarSubqueryAny_H

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
//		CDXLScalarSubqueryAny
//
//	@doc:
//		Class for representing ANY subqueries
//
//---------------------------------------------------------------------------
class CDXLScalarSubqueryAny : public CDXLScalarSubqueryQuantified
{
private:
	// private copy ctor
	CDXLScalarSubqueryAny(CDXLScalarSubqueryAny &);

public:
	// ctor
	CDXLScalarSubqueryAny(CMemoryPool *mp, IMDId *scalar_op_mdid,
						  CMDName *mdname, ULONG colid);

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// conversion function
	static CDXLScalarSubqueryAny *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarSubqueryAny == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarSubqueryAny *>(dxl_op);
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


#endif	// !SPQDXL_CDXLScalarSubqueryAny_H

// EOF
