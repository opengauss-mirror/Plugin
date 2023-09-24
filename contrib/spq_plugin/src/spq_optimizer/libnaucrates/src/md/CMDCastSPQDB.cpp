//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDCastSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific casts
//		in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDCastSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqmd;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::CMDCastSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDCastSPQDB::CMDCastSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
						 IMDId *mdid_src, IMDId *mdid_dest,
						 BOOL is_binary_coercible, IMDId *mdid_cast_func,
						 EmdCoercepathType path_type)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_mdid_src(mdid_src),
	  m_mdid_dest(mdid_dest),
	  m_is_binary_coercible(is_binary_coercible),
	  m_mdid_cast_func(mdid_cast_func),
	  m_path_type(path_type)
{
	SPQOS_ASSERT(m_mdid->IsValid());
	SPQOS_ASSERT(m_mdid_src->IsValid());
	SPQOS_ASSERT(m_mdid_dest->IsValid());
	SPQOS_ASSERT_IMP(!is_binary_coercible && m_path_type != EmdtCoerceViaIO,
					m_mdid_cast_func->IsValid());

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::~CMDCastSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDCastSPQDB::~CMDCastSPQDB()
{
	m_mdid->Release();
	m_mdid_src->Release();
	m_mdid_dest->Release();
	CRefCount::SafeRelease(m_mdid_cast_func);
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::MDId
//
//	@doc:
//		Mdid of cast object
//
//---------------------------------------------------------------------------
IMDId *
CMDCastSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::Mdname
//
//	@doc:
//		Func name
//
//---------------------------------------------------------------------------
CMDName
CMDCastSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::MdidSrc
//
//	@doc:
//		Source type id
//
//---------------------------------------------------------------------------
IMDId *
CMDCastSPQDB::MdidSrc() const
{
	return m_mdid_src;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::MdidDest
//
//	@doc:
//		Destination type id
//
//---------------------------------------------------------------------------
IMDId *
CMDCastSPQDB::MdidDest() const
{
	return m_mdid_dest;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::GetCastFuncMdId
//
//	@doc:
//		Cast function id
//
//---------------------------------------------------------------------------
IMDId *
CMDCastSPQDB::GetCastFuncMdId() const
{
	return m_mdid_cast_func;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::IsBinaryCoercible
//
//	@doc:
//		Returns whether this is a cast between binary coercible types, i.e. the
//		types are binary compatible
//
//---------------------------------------------------------------------------
BOOL
CMDCastSPQDB::IsBinaryCoercible() const
{
	return m_is_binary_coercible;
}

// returns coercion path type
IMDCast::EmdCoercepathType
CMDCastSPQDB::GetMDPathType() const
{
	return m_path_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::Serialize
//
//	@doc:
//		Serialize function metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDCastSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCast));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));

	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastBinaryCoercible),
		m_is_binary_coercible);
	m_mdid_src->Serialize(xml_serializer,
						  CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastSrcType));
	m_mdid_dest->Serialize(
		xml_serializer, CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastDestType));
	m_mdid_cast_func->Serialize(
		xml_serializer, CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastFuncId));
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCastCoercePathType),
		m_path_type);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBCast));
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMDCastSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDCastSPQDB::DebugPrint(IOstream &os) const
{
	os << "Cast " << (Mdname()).GetMDName()->GetBuffer() << ": ";
	MdidSrc()->OsPrint(os);
	os << "->";
	MdidDest()->OsPrint(os);
	os << std::endl;

	if (m_is_binary_coercible)
	{
		os << ", binary-coercible";
	}

	if (IMDId::IsValid(m_mdid_cast_func))
	{
		os << ", Cast func id: ";
		GetCastFuncMdId()->OsPrint(os);
	}

	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
