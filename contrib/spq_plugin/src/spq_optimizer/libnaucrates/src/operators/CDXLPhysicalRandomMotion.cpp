//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLPhysicalRandomMotion.cpp
//
//	@doc:
//		Implementation of DXL physical random motion operator
//---------------------------------------------------------------------------


#include "naucrates/dxl/operators/CDXLPhysicalRandomMotion.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalRandomMotion::CDXLPhysicalRandomMotion
//
//	@doc:
//		Constructor
//
//---------------------------------------------------------------------------
CDXLPhysicalRandomMotion::CDXLPhysicalRandomMotion(CMemoryPool *mp,
												   BOOL is_duplicate_sensitive)
	: CDXLPhysicalMotion(mp), m_is_duplicate_sensitive(is_duplicate_sensitive)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalRandomMotion::GetDXLOperator
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLPhysicalRandomMotion::GetDXLOperator() const
{
	return EdxlopPhysicalMotionRandom;
}


//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalRandomMotion::GetOpNameStr
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLPhysicalRandomMotion::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenPhysicalRandomMotion);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalRandomMotion::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLPhysicalRandomMotion::SerializeToDXL(CXMLSerializer *xml_serializer,
										 const CDXLNode *dxlnode) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);

	SerializeSegmentInfoToDXL(xml_serializer);

	if (m_is_duplicate_sensitive)
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenDuplicateSensitive), true);
	}

	// serialize properties
	dxlnode->SerializePropertiesToDXL(xml_serializer);

	// serialize children
	dxlnode->SerializeChildrenToDXL(xml_serializer);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalRandomMotion::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLPhysicalRandomMotion::AssertValid(const CDXLNode *dxlnode,
									  BOOL validate_children) const
{
	// assert proj list and filter are valid
	CDXLPhysical::AssertValid(dxlnode, validate_children);

	SPQOS_ASSERT(m_input_segids_array != NULL);
	SPQOS_ASSERT(0 < m_input_segids_array->Size());
	SPQOS_ASSERT(m_output_segids_array != NULL);
	SPQOS_ASSERT(0 < m_output_segids_array->Size());

	SPQOS_ASSERT(EdxlrandommIndexSentinel == dxlnode->Arity());

	CDXLNode *child_dxlnode = (*dxlnode)[EdxlrandommIndexChild];
	SPQOS_ASSERT(EdxloptypePhysical ==
				child_dxlnode->GetOperator()->GetDXLOperatorType());

	if (validate_children)
	{
		child_dxlnode->GetOperator()->AssertValid(child_dxlnode,
												  validate_children);
	}
}
#endif	// SPQOS_DEBUG

// EOF
