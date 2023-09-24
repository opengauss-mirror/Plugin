//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartBound.cpp
//
//	@doc:
//		Implementation of DXL Part Bound expression
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarPartBound.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqopt;
using namespace spqmd;
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::CDXLScalarPartBound
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDXLScalarPartBound::CDXLScalarPartBound(CMemoryPool *mp,
										 ULONG partitioning_level,
										 IMDId *mdid_type, BOOL is_lower_bound)
	: CDXLScalar(mp),
	  m_partitioning_level(partitioning_level),
	  m_mdid_type(mdid_type),
	  m_is_lower_bound(is_lower_bound)
{
	SPQOS_ASSERT(mdid_type->IsValid());
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::~CDXLScalarPartBound
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDXLScalarPartBound::~CDXLScalarPartBound()
{
	m_mdid_type->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::GetDXLOperator
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLScalarPartBound::GetDXLOperator() const
{
	return EdxlopScalarPartBound;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::GetOpNameStr
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarPartBound::GetOpNameStr() const
{
	return CDXLTokens::GetDXLTokenStr(EdxltokenScalarPartBound);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::HasBoolResult
//
//	@doc:
//		Does the operator return a boolean result
//
//---------------------------------------------------------------------------
BOOL
CDXLScalarPartBound::HasBoolResult(CMDAccessor *md_accessor) const
{
	return (IMDType::EtiBool ==
			md_accessor->RetrieveType(m_mdid_type)->GetDatumType());
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLScalarPartBound::SerializeToDXL(CXMLSerializer *xml_serializer,
									const CDXLNode *  // dxlnode
) const
{
	const CWStringConst *element_name = GetOpNameStr();

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenPartLevel),
								 m_partitioning_level);
	m_mdid_type->Serialize(xml_serializer,
						   CDXLTokens::GetDXLTokenStr(EdxltokenMDType));
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenScalarPartBoundLower),
		m_is_lower_bound);
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartBound::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLScalarPartBound::AssertValid(const CDXLNode *dxlnode,
								 BOOL  // validate_children
) const
{
	SPQOS_ASSERT(0 == dxlnode->Arity());
}
#endif	// SPQOS_DEBUG

// EOF
