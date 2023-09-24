//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CDXLPhysicalExternalScan.h
//
//	@doc:
//		Class for representing DXL external scan operators
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalExternalScan_H
#define SPQDXL_CDXLPhysicalExternalScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalTableScan.h"
#include "naucrates/dxl/operators/CDXLTableDescr.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalExternalScan
//
//	@doc:
//		Class for representing DXL external scan operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalExternalScan : public CDXLPhysicalTableScan
{
private:
	// private copy ctor
	CDXLPhysicalExternalScan(CDXLPhysicalExternalScan &);

public:
	// ctors
	explicit CDXLPhysicalExternalScan(CMemoryPool *mp);

	CDXLPhysicalExternalScan(CMemoryPool *mp, CDXLTableDescr *table_descr);

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// conversion function
	static CDXLPhysicalExternalScan *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalExternalScan == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalExternalScan *>(dxl_op);
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalExternalScan_H

// EOF
