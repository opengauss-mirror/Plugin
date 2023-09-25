//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDArrayCoerceCastSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific array coerce
//		casts in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDArrayCoerceCastSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqmd;
using namespace spqdxl;

// ctor
CMDArrayCoerceCastSPQDB::CMDArrayCoerceCastSPQDB(
	CMemoryPool *mp, IMDId *mdid, CMDName *mdname, IMDId *mdid_src,
	IMDId *mdid_dest, BOOL is_binary_coercible, IMDId *mdid_cast_func,
	EmdCoercepathType path_type, INT type_modifier, BOOL is_explicit,
	EdxlCoercionForm dxl_coerce_format, INT location)
	: CMDCastSPQDB(mp, mdid, mdname, mdid_src, mdid_dest, is_binary_coercible,
				  mdid_cast_func, path_type),
	  m_type_modifier(type_modifier),
	  m_is_explicit(is_explicit),
	  m_dxl_coerce_format(dxl_coerce_format),
	  m_location(location)
{
	m_dxl_str = CDXLUtils::SerializeMDObj(mp, this, false /*fSerializeHeader*/,
										  false /*indentation*/);
}

// dtor
CMDArrayCoerceCastSPQDB::~CMDArrayCoerceCastSPQDB()
{
	SPQOS_DELETE(m_dxl_str);
}

// return type modifier
INT
CMDArrayCoerceCastSPQDB::TypeModifier() const
{
	return m_type_modifier;
}

// return is explicit cast
BOOL
CMDArrayCoerceCastSPQDB::IsExplicit() const
{
	return m_is_explicit;
}

// return coercion form
EdxlCoercionForm
CMDArrayCoerceCastSPQDB::GetCoercionForm() const
{
	return m_dxl_coerce_format;
}

// return token location
INT
CMDArrayCoerceCastSPQDB::Location() const
{
	return m_location;
}

// serialize function metadata in DXL format
void
CMDArrayCoerceCastSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBArrayCoerceCast));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));

	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastCoercePathType),
		m_path_type);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastBinaryCoercible),
		m_is_binary_coercible);

	m_mdid_src->Serialize(xml_serializer,
						  CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastSrcType));
	m_mdid_dest->Serialize(
		xml_serializer, CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastDestType));
	m_mdid_cast_func->Serialize(
		xml_serializer, CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastFuncId));

	if (default_type_modifier != TypeModifier())
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenTypeMod), TypeModifier());
	}

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIsExplicit), m_is_explicit);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenCoercionForm),
		(ULONG) m_dxl_coerce_format);
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenLocation),
								 m_location);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBArrayCoerceCast));
}


#ifdef SPQOS_DEBUG

// prints a metadata cache relation to the provided output
void
CMDArrayCoerceCastSPQDB::DebugPrint(IOstream &os) const
{
	CMDCastSPQDB::DebugPrint(os);
	os << ", Result Type Mod: ";
	os << m_type_modifier;
	os << ", isExplicit: ";
	os << m_is_explicit;
	os << ", coercion form: ";
	os << m_dxl_coerce_format;

	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
