//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLPhysicalBitmapTableScan.h
//
//	@doc:
//		Class for representing DXL bitmap table scan operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalBitmapTableScan_H
#define SPQDXL_CDXLPhysicalBitmapTableScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalAbstractBitmapScan.h"

namespace spqdxl
{
using namespace spqos;

// fwd declarations
class CDXLTableDescr;
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalBitmapTableScan
//
//	@doc:
//		Class for representing DXL bitmap table scan operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalBitmapTableScan : public CDXLPhysicalAbstractBitmapScan
{
private:
	// private copy ctor
	CDXLPhysicalBitmapTableScan(const CDXLPhysicalBitmapTableScan &);

public:
	// ctors
	CDXLPhysicalBitmapTableScan(CMemoryPool *mp, CDXLTableDescr *table_descr)
		: CDXLPhysicalAbstractBitmapScan(mp, table_descr)
	{
	}

	// dtor
	virtual ~CDXLPhysicalBitmapTableScan()
	{
	}

	// operator type
	virtual Edxlopid
	GetDXLOperator() const
	{
		return EdxlopPhysicalBitmapTableScan;
	}

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalBitmapTableScan *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalBitmapTableScan == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalBitmapTableScan *>(dxl_op);
	}

};	// class CDXLPhysicalBitmapTableScan
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLPhysicalBitmapTableScan_H

// EOF
