//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalSubqueryScan.h
//
//	@doc:
//		Class for representing DXL physical subquery scan (projection) operators.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLPhysicalSubqueryScan_H
#define SPQDXL_CDXLPhysicalSubqueryScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"
#include "naucrates/md/CMDName.h"


namespace spqdxl
{
using namespace spqmd;

// indices of subquery scan elements in the children array
enum Edxlsubqscan
{
	EdxlsubqscanIndexProjList = 0,
	EdxlsubqscanIndexFilter,
	EdxlsubqscanIndexChild,
	EdxlsubqscanIndexSentinel
};
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalSubqueryScan
//
//	@doc:
//		Class for representing DXL physical subquery scan (projection) operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalSubqueryScan : public CDXLPhysical
{
private:
	// name for the subquery scan node (corresponding to name in SPQDB's SubqueryScan)
	CMDName *m_mdname_alias;

	// private copy ctor
	CDXLPhysicalSubqueryScan(CDXLPhysicalSubqueryScan &);

public:
	// ctor/dtor
	CDXLPhysicalSubqueryScan(CMemoryPool *mp, CMDName *mdname);

	virtual ~CDXLPhysicalSubqueryScan();

	// accessors
	Edxlopid GetDXLOperator() const;
	const CWStringConst *GetOpNameStr() const;
	const CMDName *MdName();

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalSubqueryScan *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalSubqueryScan == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalSubqueryScan *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalSubqueryScan_H

// EOF
