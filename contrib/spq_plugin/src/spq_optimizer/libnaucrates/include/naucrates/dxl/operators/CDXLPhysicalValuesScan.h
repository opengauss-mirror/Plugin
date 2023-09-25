//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software, Inc.
//
//	@filename:
//		CDXLPhysicalValuesScan.h
//
//	@doc:
//		Class for representing DXL physical Values scan
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLPhysicalValuesScan_H
#define SPQDXL_CDXLPhysicalValuesScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
enum EdxlnVal
{
	EdxlValIndexProjList = 0,
	EdxlValIndexConstStart,
	EdxlValIndexSentinel
};

// class for representing DXL physical Values scan
class CDXLPhysicalValuesScan : public CDXLPhysical
{
private:
	// private copy ctor
	CDXLPhysicalValuesScan(CDXLPhysicalValuesScan &);

public:
	// ctor
	CDXLPhysicalValuesScan(CMemoryPool *mp);

	// dtor
	virtual ~CDXLPhysicalValuesScan();

	// get operator type
	Edxlopid GetDXLOperator() const;

	// get operator name
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalValuesScan *Cast(CDXLOperator *dxl_op);

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLPhysicalValuesScan_H

// EOF
