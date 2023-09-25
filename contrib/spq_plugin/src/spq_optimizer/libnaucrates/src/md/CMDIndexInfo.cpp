//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software Inc.
//
//	@filename:
//		CMDIndexInfo.cpp
//
//	@doc:
//		Implementation of the class for representing indexinfo
//---------------------------------------------------------------------------

#include "naucrates/md/CMDIndexInfo.h"

#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;

// ctor
CMDIndexInfo::CMDIndexInfo(IMDId *mdid, BOOL is_partial)
	: m_mdid(mdid), m_is_partial(is_partial)
{
	SPQOS_ASSERT(mdid->IsValid());
}

// dtor
CMDIndexInfo::~CMDIndexInfo()
{
	m_mdid->Release();
}

// returns the metadata id of this index
IMDId *
CMDIndexInfo::MDId() const
{
	return m_mdid;
}

// is the index partial
BOOL
CMDIndexInfo::IsPartial() const
{
	return m_is_partial;
}

// serialize indexinfo in DXL format
void
CMDIndexInfo::Serialize(spqdxl::CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexInfo));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexPartial), m_is_partial);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexInfo));
}

#ifdef SPQOS_DEBUG
// prints a indexinfo to the provided output
void
CMDIndexInfo::DebugPrint(IOstream &os) const
{
	os << "Index id: ";
	MDId()->OsPrint(os);
	os << std::endl;
	os << "Is partial index: " << m_is_partial << std::endl;
}

#endif	// SPQOS_DEBUG
