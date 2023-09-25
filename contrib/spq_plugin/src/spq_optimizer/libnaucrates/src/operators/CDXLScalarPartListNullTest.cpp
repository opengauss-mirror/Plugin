//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//  Implementation of DXL Part list null test expression

#include "naucrates/dxl/operators/CDXLScalarPartListNullTest.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqopt;
using namespace spqmd;
using namespace spqos;
using namespace spqdxl;

// Ctor
CDXLScalarPartListNullTest::CDXLScalarPartListNullTest(CMemoryPool *mp,
													   ULONG partitioning_level,
													   BOOL is_null)
	: CDXLScalar(mp),
	  m_partitioning_level(partitioning_level),
	  m_is_null(is_null)
{
}

// Operator type
Edxlopid
CDXLScalarPartListNullTest::GetDXLOperator() const
{
	return EdxlopScalarPartListNullTest;
}

// Operator name
const CWStringConst *
CDXLScalarPartListNullTest::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenScalarPartListNullTest);
}

// Serialize operator in DXL format
void
CDXLScalarPartListNullTest::SerializeToDXL(CXMLSerializer *xml_serializer,
										   const CDXLNode *	 // dxlnode
) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenPartLevel),
								 m_partitioning_level);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenScalarIsNull), m_is_null);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

// partitioning level
ULONG
CDXLScalarPartListNullTest::GetPartitioningLevel() const
{
	return m_partitioning_level;
}

// Null Test type (true for 'is null', false for 'is not null')
BOOL
CDXLScalarPartListNullTest::IsNull() const
{
	return m_is_null;
}

// does the operator return a boolean result
BOOL
CDXLScalarPartListNullTest::HasBoolResult(CMDAccessor *	 //md_accessor
) const
{
	return true;
}

#ifdef SPQOS_DEBUG
// Checks whether operator node is well-structured
void
CDXLScalarPartListNullTest::AssertValid(const CDXLNode *dxlnode,
										BOOL  // validate_children
) const
{
	SPQOS_ASSERT(0 == dxlnode->Arity());
}
#endif	// SPQOS_DEBUG

// conversion function
CDXLScalarPartListNullTest *
CDXLScalarPartListNullTest::Cast(CDXLOperator *dxl_op)
{
	SPQOS_ASSERT(NULL != dxl_op);
	SPQOS_ASSERT(EdxlopScalarPartListNullTest == dxl_op->GetDXLOperator());

	return dynamic_cast<CDXLScalarPartListNullTest *>(dxl_op);
}

// EOF
