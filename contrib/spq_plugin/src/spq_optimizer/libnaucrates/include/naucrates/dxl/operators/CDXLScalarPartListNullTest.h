//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	Class for representing DXL Part list null test expressions
//	These expressions indicate whether the list values of a part
//	contain NULL value or not
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarPartListNullTest_H
#define SPQDXL_CDXLScalarPartListNullTest_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"

namespace spqdxl
{
class CDXLScalarPartListNullTest : public CDXLScalar
{
private:
	// partitioning level
	ULONG m_partitioning_level;

	// Null Test type (true for 'is null', false for 'is not null')
	BOOL m_is_null;

	// private copy ctor
	CDXLScalarPartListNullTest(const CDXLScalarPartListNullTest &);

public:
	// ctor
	CDXLScalarPartListNullTest(CMemoryPool *mp, ULONG partitioning_level,
							   BOOL is_null);

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// partitioning level
	ULONG GetPartitioningLevel() const;

	// Null Test type (true for 'is null', false for 'is not null')
	BOOL IsNull() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLScalarPartListNullTest *Cast(CDXLOperator *dxl_op);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarPartListNullTest_H

// EOF
