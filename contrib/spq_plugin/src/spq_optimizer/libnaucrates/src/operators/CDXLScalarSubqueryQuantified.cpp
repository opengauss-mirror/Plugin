//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC, Corp.
//
//	@filename:
//		CDXLScalarSubqueryQuantified.cpp
//
//	@doc:
//		Implementation of quantified subquery operator
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarSubqueryQuantified.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarSubqueryQuantified::CDXLScalarSubqueryQuantified
//
//	@doc:
//		Constructor
//
//---------------------------------------------------------------------------
CDXLScalarSubqueryQuantified::CDXLScalarSubqueryQuantified(
	CMemoryPool *mp, IMDId *scalar_op_mdid, CMDName *scalar_op_mdname,
	ULONG colid)
	: CDXLScalar(mp),
	  m_scalar_op_mdid(scalar_op_mdid),
	  m_scalar_op_mdname(scalar_op_mdname),
	  m_colid(colid)
{
	SPQOS_ASSERT(scalar_op_mdid->IsValid());
	SPQOS_ASSERT(NULL != scalar_op_mdname);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarSubqueryQuantified::~CDXLScalarSubqueryQuantified
//
//	@doc:
//		Destructor
//
//---------------------------------------------------------------------------
CDXLScalarSubqueryQuantified::~CDXLScalarSubqueryQuantified()
{
	m_scalar_op_mdid->Release();
	SPQOS_DELETE(m_scalar_op_mdname);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarSubqueryQuantified::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLScalarSubqueryQuantified::SerializeToDXL(CXMLSerializer *xml_serializer,
											 const CDXLNode *dxlnode) const
{
	const CWStringConst *element_name = GetOpNameStr();
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);

	// serialize operator id and name
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenOpName),
								 m_scalar_op_mdname->GetMDName());
	m_scalar_op_mdid->Serialize(xml_serializer,
								CDXLTokens::GetDXLTokenStr(EdxltokenOpNo));

	// serialize computed column id
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenColId),
								 m_colid);

	dxlnode->SerializeChildrenToDXL(xml_serializer);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarSubqueryQuantified::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLScalarSubqueryQuantified::AssertValid(const CDXLNode *dxlnode,
										  BOOL validate_children) const
{
	SPQOS_ASSERT(2 == dxlnode->Arity());

	CDXLNode *pdxlnScalarChild = (*dxlnode)[EdxlsqquantifiedIndexScalar];
	CDXLNode *pdxlnRelationalChild =
		(*dxlnode)[EdxlsqquantifiedIndexRelational];

	SPQOS_ASSERT(EdxloptypeScalar ==
				pdxlnScalarChild->GetOperator()->GetDXLOperatorType());
	SPQOS_ASSERT(EdxloptypeLogical ==
				pdxlnRelationalChild->GetOperator()->GetDXLOperatorType());

	dxlnode->AssertValid(validate_children);
}
#endif	// SPQOS_DEBUG

// EOF
