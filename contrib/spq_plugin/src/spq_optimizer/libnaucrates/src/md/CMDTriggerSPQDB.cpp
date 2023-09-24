//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDTriggerSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific triggers
//		in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDTriggerSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqmd;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::CMDTriggerSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTriggerSPQDB::CMDTriggerSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
							   IMDId *rel_mdid, IMDId *mdid_func, INT type,
							   BOOL is_enabled)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_rel_mdid(rel_mdid),
	  m_func_mdid(mdid_func),
	  m_type(type),
	  m_is_enabled(is_enabled)
{
	SPQOS_ASSERT(m_mdid->IsValid());
	SPQOS_ASSERT(m_rel_mdid->IsValid());
	SPQOS_ASSERT(m_func_mdid->IsValid());
	SPQOS_ASSERT(0 <= type);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::~CMDTriggerSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTriggerSPQDB::~CMDTriggerSPQDB()
{
	m_mdid->Release();
	m_rel_mdid->Release();
	m_func_mdid->Release();
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::FRow
//
//	@doc:
//		Does trigger execute on a row-level
//
//---------------------------------------------------------------------------
BOOL
CMDTriggerSPQDB::ExecutesOnRowLevel() const
{
	return (m_type & SPQMD_TRIGGER_ROW);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::IsBefore
//
//	@doc:
//		Is this a before trigger
//
//---------------------------------------------------------------------------
BOOL
CMDTriggerSPQDB::IsBefore() const
{
	return (m_type & SPQMD_TRIGGER_BEFORE);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::Insert
//
//	@doc:
//		Is this an insert trigger
//
//---------------------------------------------------------------------------
BOOL
CMDTriggerSPQDB::IsInsert() const
{
	return (m_type & SPQMD_TRIGGER_INSERT);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::FDelete
//
//	@doc:
//		Is this a delete trigger
//
//---------------------------------------------------------------------------
BOOL
CMDTriggerSPQDB::IsDelete() const
{
	return (m_type & SPQMD_TRIGGER_DELETE);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::FUpdate
//
//	@doc:
//		Is this an update trigger
//
//---------------------------------------------------------------------------
BOOL
CMDTriggerSPQDB::IsUpdate() const
{
	return (m_type & SPQMD_TRIGGER_UPDATE);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::Serialize
//
//	@doc:
//		Serialize trigger metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTriggerSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTrigger));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	m_rel_mdid->Serialize(xml_serializer,
						  CDXLTokens::GetDXLTokenStr(EdxltokenRelationMdid));
	m_func_mdid->Serialize(xml_serializer,
						   CDXLTokens::GetDXLTokenStr(EdxltokenFuncId));

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTriggerRow),
		ExecutesOnRowLevel());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTriggerBefore), IsBefore());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTriggerInsert), IsInsert());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTriggerDelete), IsDelete());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTriggerUpdate), IsUpdate());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTriggerEnabled), m_is_enabled);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBTrigger));
}

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMDTriggerSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTriggerSPQDB::DebugPrint(IOstream &os) const
{
	os << "Trigger id: ";
	m_mdid->OsPrint(os);
	os << std::endl;

	os << "Trigger name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;

	os << "Trigger relation id: ";
	m_rel_mdid->OsPrint(os);
	os << std::endl;

	os << "Trigger function id: ";
	m_func_mdid->OsPrint(os);
	os << std::endl;

	if (ExecutesOnRowLevel())
	{
		os << "Trigger level: Row" << std::endl;
	}
	else
	{
		os << "Trigger level: Table" << std::endl;
	}

	if (IsBefore())
	{
		os << "Trigger timing: Before" << std::endl;
	}
	else
	{
		os << "Trigger timing: After" << std::endl;
	}

	os << "Trigger statement type(s): [ ";
	if (IsInsert())
	{
		os << "Insert ";
	}

	if (IsDelete())
	{
		os << "Delete ";
	}

	if (IsUpdate())
	{
		os << "Update ";
	}
	os << "]" << std::endl;

	if (m_is_enabled)
	{
		os << "Trigger enabled: Yes" << std::endl;
	}
	else
	{
		os << "Trigger enabled: No" << std::endl;
	}
}

#endif	// SPQOS_DEBUG

// EOF
