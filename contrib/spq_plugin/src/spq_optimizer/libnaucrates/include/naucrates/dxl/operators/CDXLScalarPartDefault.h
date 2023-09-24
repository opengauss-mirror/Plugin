//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartDefault.h
//
//	@doc:
//		Class for representing DXL Part Default expressions
//		These expressions indicate whether a particular part is a default part
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarPartDefault_H
#define SPQDXL_CDXLScalarPartDefault_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarPartDefault
//
//	@doc:
//		Class for representing DXL Part Default expressions
//
//---------------------------------------------------------------------------
class CDXLScalarPartDefault : public CDXLScalar
{
private:
	// partitioning level
	ULONG m_partitioning_level;

	// private copy ctor
	CDXLScalarPartDefault(const CDXLScalarPartDefault &);

public:
	// ctor
	CDXLScalarPartDefault(CMemoryPool *mp, ULONG partitioning_level);

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// partitioning level
	ULONG
	GetPartitioningLevel() const
	{
		return m_partitioning_level;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		return true;
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLScalarPartDefault *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarPartDefault == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarPartDefault *>(dxl_op);
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarPartDefault_H

// EOF
