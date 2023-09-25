//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLPhysicalIndexOnlyScan.h
//
//	@doc:
//		Class for representing DXL index only scan operators
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalIndexOnlyScan_H
#define SPQDXL_CDXLPhysicalIndexOnlyScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLIndexDescr.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLPhysicalIndexScan.h"
#include "naucrates/dxl/operators/CDXLTableDescr.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalIndexOnlyScan
//
//	@doc:
//		Class for representing DXL index only scan operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalIndexOnlyScan : public CDXLPhysicalIndexScan
{
private:
	// private copy ctor
	CDXLPhysicalIndexOnlyScan(CDXLPhysicalIndexOnlyScan &);

public:
	//ctor
	CDXLPhysicalIndexOnlyScan(CMemoryPool *mp, CDXLTableDescr *table_descr,
							  CDXLIndexDescr *dxl_index_descr,
							  EdxlIndexScanDirection idx_scan_direction);

	//dtor
	virtual ~CDXLPhysicalIndexOnlyScan()
	{
	}

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// conversion function
	static CDXLPhysicalIndexOnlyScan *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalIndexOnlyScan == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalIndexOnlyScan *>(dxl_op);
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalIndexOnlyScan_H

// EOF
