//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartDefault.cpp
//
//	@doc:
//		Implementation of DXL Part Default expression
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarPartDefault.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqopt;
using namespace spqmd;
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartDefault::CDXLScalarPartDefault
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDXLScalarPartDefault::CDXLScalarPartDefault(CMemoryPool *mp,
											 ULONG partitioning_level)
	: CDXLScalar(mp), m_partitioning_level(partitioning_level)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartDefault::GetDXLOperator
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLScalarPartDefault::GetDXLOperator() const
{
	return EdxlopScalarPartDefault;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartDefault::GetOpNameStr
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarPartDefault::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenScalarPartDefault);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartDefault::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLScalarPartDefault::SerializeToDXL(CXMLSerializer *xml_serializer,
									  const CDXLNode *	// dxlnode
) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenPartLevel),
								 m_partitioning_level);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartDefault::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLScalarPartDefault::AssertValid(const CDXLNode *dxlnode,
								   BOOL	 // validate_children
) const
{
	SPQOS_ASSERT(0 == dxlnode->Arity());
}
#endif	// SPQOS_DEBUG

// EOF
