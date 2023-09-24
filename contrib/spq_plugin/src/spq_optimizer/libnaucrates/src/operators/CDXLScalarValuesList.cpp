//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software, Inc.
//
//	@filename:
//		CDXLScalarValuesList.cpp
//
//	@doc:
//		Implementation of DXL value list operator
//---------------------------------------------------------------------------


#include "naucrates/dxl/operators/CDXLScalarValuesList.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqdxl;


// constructs a m_bytearray_value list node
CDXLScalarValuesList::CDXLScalarValuesList(CMemoryPool *mp) : CDXLScalar(mp)
{
}

// destructor
CDXLScalarValuesList::~CDXLScalarValuesList()
{
}

// operator type
Edxlopid
CDXLScalarValuesList::GetDXLOperator() const
{
	return EdxlopScalarValuesList;
}

// operator name
const CWStringConst *
CDXLScalarValuesList::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenScalarValuesList);
}

// serialize operator in DXL format
void
CDXLScalarValuesList::SerializeToDXL(CXMLSerializer *xml_serializer,
									 const CDXLNode *dxlnode) const
{
	SPQOS_CHECK_ABORT;

	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	dxlnode->SerializeChildrenToDXL(xml_serializer);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);

	SPQOS_CHECK_ABORT;
}

// serialize operator in DXL format
void
CDXLScalarValuesList::SerializeToDXL(CXMLSerializer *xml_serializer,
									 const CDXLNode *dxlnode,
									 const CHAR *attrname) const
{
	SPQOS_CHECK_ABORT;

	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenParamKind),
								 attrname);
	dxlnode->SerializeChildrenToDXL(xml_serializer);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);

	SPQOS_CHECK_ABORT;
}


// conversion function
CDXLScalarValuesList *
CDXLScalarValuesList::Cast(CDXLOperator *dxl_op)
{
	SPQOS_ASSERT(NULL != dxl_op);
	SPQOS_ASSERT(EdxlopScalarValuesList == dxl_op->GetDXLOperator());

	return dynamic_cast<CDXLScalarValuesList *>(dxl_op);
}

// does the operator return a boolean result
BOOL
CDXLScalarValuesList::HasBoolResult(CMDAccessor *  //md_accessor
) const
{
	return false;
}

#ifdef SPQOS_DEBUG

// checks whether operator node is well-structured
void
CDXLScalarValuesList::AssertValid(const CDXLNode *dxlnode,
								  BOOL validate_children) const
{
	const ULONG arity = dxlnode->Arity();

	for (ULONG idx = 0; idx < arity; ++idx)
	{
		CDXLNode *pdxlnConstVal = (*dxlnode)[idx];
		SPQOS_ASSERT(EdxloptypeScalar ==
					pdxlnConstVal->GetOperator()->GetDXLOperatorType());

		if (validate_children)
		{
			pdxlnConstVal->GetOperator()->AssertValid(pdxlnConstVal,
													  validate_children);
		}
	}
}
#endif	// SPQOS_DEBUG

// EOF
