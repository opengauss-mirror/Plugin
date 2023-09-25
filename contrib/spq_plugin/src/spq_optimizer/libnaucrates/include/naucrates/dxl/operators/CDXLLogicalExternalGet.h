//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CDXLLogicalExternalGet.h
//
//	@doc:
//		Class for representing DXL logical external get operator, for reading
//		from external tables
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLLogicalExternalGet_H
#define SPQDXL_CDXLLogicalExternalGet_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogicalGet.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLLogicalExternalGet
//
//	@doc:
//		Class for representing DXL logical external get operator
//
//---------------------------------------------------------------------------
class CDXLLogicalExternalGet : public CDXLLogicalGet
{
private:
	// private copy ctor
	CDXLLogicalExternalGet(CDXLLogicalExternalGet &);

public:
	// ctor
	CDXLLogicalExternalGet(CMemoryPool *mp, CDXLTableDescr *table_descr);

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// conversion function
	static CDXLLogicalExternalGet *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopLogicalExternalGet == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLLogicalExternalGet *>(dxl_op);
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLLogicalExternalGet_H

// EOF
