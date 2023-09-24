//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLPhysicalCTEProducer.cpp
//
//	@doc:
//		Implementation of DXL physical CTE producer operator
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLPhysicalCTEProducer.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/dxl/xml/dxltokens.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalCTEProducer::CDXLPhysicalCTEProducer
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDXLPhysicalCTEProducer::CDXLPhysicalCTEProducer(
	CMemoryPool *mp, ULONG id, ULongPtrArray *output_colids_array)
	: CDXLPhysical(mp), m_id(id), m_output_colids_array(output_colids_array)
{
	SPQOS_ASSERT(NULL != output_colids_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalCTEProducer::~CDXLPhysicalCTEProducer
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDXLPhysicalCTEProducer::~CDXLPhysicalCTEProducer()
{
	m_output_colids_array->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalCTEProducer::GetDXLOperator
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLPhysicalCTEProducer::GetDXLOperator() const
{
	return EdxlopPhysicalCTEProducer;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalCTEProducer::GetOpNameStr
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLPhysicalCTEProducer::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenPhysicalCTEProducer);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalCTEProducer::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLPhysicalCTEProducer::SerializeToDXL(CXMLSerializer *xml_serializer,
										const CDXLNode *dxlnode) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenCTEId),
								 Id());

	CWStringDynamic *pstrColIds =
		CDXLUtils::Serialize(m_mp, m_output_colids_array);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenColumns),
								 pstrColIds);
	SPQOS_DELETE(pstrColIds);

	// serialize properties
	dxlnode->SerializePropertiesToDXL(xml_serializer);

	dxlnode->SerializeChildrenToDXL(xml_serializer);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLPhysicalCTEProducer::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLPhysicalCTEProducer::AssertValid(const CDXLNode *dxlnode,
									 BOOL validate_children) const
{
	SPQOS_ASSERT(2 == dxlnode->Arity());

	CDXLNode *pdxlnPrL = (*dxlnode)[0];
	CDXLNode *child_dxlnode = (*dxlnode)[1];

	SPQOS_ASSERT(EdxlopScalarProjectList ==
				pdxlnPrL->GetOperator()->GetDXLOperator());
	SPQOS_ASSERT(EdxloptypePhysical ==
				child_dxlnode->GetOperator()->GetDXLOperatorType());

	if (validate_children)
	{
		pdxlnPrL->GetOperator()->AssertValid(pdxlnPrL, validate_children);
		child_dxlnode->GetOperator()->AssertValid(child_dxlnode,
												  validate_children);
	}
}
#endif	// SPQOS_DEBUG

// EOF
