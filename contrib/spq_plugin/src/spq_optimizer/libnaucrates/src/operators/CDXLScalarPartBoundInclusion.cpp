//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartBoundInclusion.cpp
//
//	@doc:
//		Implementation of DXL Part bound inclusion expression
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarPartBoundInclusion.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqopt;
using namespace spqmd;
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBoundInclusion::CDXLScalarPartBoundInclusion
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDXLScalarPartBoundInclusion::CDXLScalarPartBoundInclusion(
	CMemoryPool *mp, ULONG partitioning_level, BOOL is_lower_bound)
	: CDXLScalar(mp),
	  m_partitioning_level(partitioning_level),
	  m_is_lower_bound(is_lower_bound)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBoundInclusion::GetDXLOperator
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLScalarPartBoundInclusion::GetDXLOperator() const
{
	return EdxlopScalarPartBoundInclusion;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBoundInclusion::GetOpNameStr
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarPartBoundInclusion::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenScalarPartBoundInclusion);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBoundInclusion::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLScalarPartBoundInclusion::SerializeToDXL(CXMLSerializer *xml_serializer,
											 const CDXLNode *  // dxlnode
) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenPartLevel),
								 m_partitioning_level);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenScalarPartBoundLower),
		m_is_lower_bound);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBoundInclusion::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLScalarPartBoundInclusion::AssertValid(const CDXLNode *dxlnode,
										  BOOL	// validate_children
) const
{
	SPQOS_ASSERT(0 == dxlnode->Arity());
}
#endif	// SPQOS_DEBUG

// EOF
