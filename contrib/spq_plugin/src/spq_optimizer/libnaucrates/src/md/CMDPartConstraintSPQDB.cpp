//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDPartConstraintSPQDB.cpp
//
//	@doc:
//		Implementation of part constraints in the MD cache
//---------------------------------------------------------------------------

#include "naucrates/md/CMDPartConstraintSPQDB.h"

#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CMDPartConstraintSPQDB::CMDPartConstraintSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDPartConstraintSPQDB::CMDPartConstraintSPQDB(
	CMemoryPool *mp, ULongPtrArray *level_with_default_part_array,
	BOOL is_unbounded, CDXLNode *dxlnode)
	: m_mp(mp),
	  m_level_with_default_part_array(level_with_default_part_array),
	  m_is_unbounded(is_unbounded),
	  m_dxl_node(dxlnode)
{
	SPQOS_ASSERT(NULL != level_with_default_part_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDPartConstraintSPQDB::~CMDPartConstraintSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDPartConstraintSPQDB::~CMDPartConstraintSPQDB()
{
	if (NULL != m_dxl_node)
		m_dxl_node->Release();
	m_level_with_default_part_array->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDPartConstraintSPQDB::GetPartConstraintExpr
//
//	@doc:
//		Scalar expression of the check constraint
//
//---------------------------------------------------------------------------
CExpression *
CMDPartConstraintSPQDB::GetPartConstraintExpr(CMemoryPool *mp,
											 CMDAccessor *md_accessor,
											 CColRefArray *colref_array) const
{
	SPQOS_ASSERT(NULL != colref_array);

	// translate the DXL representation of the part constraint expression
	CTranslatorDXLToExpr dxltr(mp, md_accessor);
	return dxltr.PexprTranslateScalar(m_dxl_node, colref_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDPartConstraintSPQDB::GetDefaultPartsArray
//
//	@doc:
//		Included default partitions
//
//---------------------------------------------------------------------------
ULongPtrArray *
CMDPartConstraintSPQDB::GetDefaultPartsArray() const
{
	return m_level_with_default_part_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDPartConstraintSPQDB::IsConstraintUnbounded
//
//	@doc:
//		Is constraint unbounded
//
//---------------------------------------------------------------------------
BOOL
CMDPartConstraintSPQDB::IsConstraintUnbounded() const
{
	return m_is_unbounded;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDPartConstraintSPQDB::Serialize
//
//	@doc:
//		Serialize part constraint in DXL format
//
//---------------------------------------------------------------------------
void
CMDPartConstraintSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenPartConstraint));

	// serialize default parts
	CWStringDynamic *default_part_array =
		CDXLUtils::Serialize(m_mp, m_level_with_default_part_array);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenDefaultPartition),
		default_part_array);
	SPQOS_DELETE(default_part_array);

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenPartConstraintUnbounded),
		m_is_unbounded);

	// serialize the scalar expression
	if (NULL != m_dxl_node)
	{
		m_dxl_node->SerializeToDXL(xml_serializer);
	}
	else
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenPartConstraintExprAbsent),
			true);
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenPartConstraint));

	SPQOS_CHECK_ABORT;
}

// EOF
