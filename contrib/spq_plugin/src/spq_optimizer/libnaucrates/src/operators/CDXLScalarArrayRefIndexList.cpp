//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CDXLScalarArrayRefIndexList.cpp
//
//	@doc:
//		Implementation of DXL arrayref index list
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarArrayRefIndexList.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarArrayRefIndexList::CDXLScalarArrayRefIndexList
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDXLScalarArrayRefIndexList::CDXLScalarArrayRefIndexList(
	CMemoryPool *mp, EIndexListBound index_list_bound)
	: CDXLScalar(mp), m_index_list_bound(index_list_bound)
{
	SPQOS_ASSERT(EilbSentinel > index_list_bound);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarArrayRefIndexList::GetDXLOperator
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLScalarArrayRefIndexList::GetDXLOperator() const
{
	return EdxlopScalarArrayRefIndexList;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarArrayRefIndexList::GetOpNameStr
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarArrayRefIndexList::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenScalarArrayRefIndexList);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarArrayRefIndexList::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLScalarArrayRefIndexList::SerializeToDXL(CXMLSerializer *xml_serializer,
											const CDXLNode *dxlnode) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenScalarArrayRefIndexListBound),
		GetDXLIndexListBoundStr(m_index_list_bound));

	dxlnode->SerializeChildrenToDXL(xml_serializer);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarArrayRefIndexList::PstrIndexListBound
//
//	@doc:
//		String representation of index list bound
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarArrayRefIndexList::GetDXLIndexListBoundStr(
	EIndexListBound index_list_bound)
{
	switch (index_list_bound)
	{
		case EilbLower:
			return CDXLTokens::GetDXLTokenStr(
				EdxltokenScalarArrayRefIndexListLower);

		case EilbUpper:
			return CDXLTokens::GetDXLTokenStr(
				EdxltokenScalarArrayRefIndexListUpper);

		default:
			SPQOS_ASSERT("Invalid array bound");
			return NULL;
	}
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarArrayRefIndexList::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLScalarArrayRefIndexList::AssertValid(const CDXLNode *dxlnode,
										 BOOL validate_children) const
{
	const ULONG arity = dxlnode->Arity();
	for (ULONG ul = 0; ul < arity; ++ul)
	{
		CDXLNode *child_dxlnode = (*dxlnode)[ul];
		SPQOS_ASSERT(EdxloptypeScalar ==
					child_dxlnode->GetOperator()->GetDXLOperatorType());

		if (validate_children)
		{
			child_dxlnode->GetOperator()->AssertValid(child_dxlnode,
													  validate_children);
		}
	}
}
#endif	// SPQOS_DEBUG

// EOF
