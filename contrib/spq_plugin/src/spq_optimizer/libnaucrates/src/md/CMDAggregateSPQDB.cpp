//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDAggregateSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific aggregates
//		in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDAggregateSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqmd;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::CMDAggregateSPQDB
//
//	@doc:
//		Constructs a metadata aggregate
//
//---------------------------------------------------------------------------
CMDAggregateSPQDB::CMDAggregateSPQDB(CMemoryPool *mp, IMDId *mdid,
								   CMDName *mdname, IMDId *result_type_mdid,
								   IMDId *intermediate_result_type_mdid,
								   BOOL fOrdered, BOOL is_splittable,
								   BOOL is_hash_agg_capable)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_mdid_type_result(result_type_mdid),
	  m_mdid_type_intermediate(intermediate_result_type_mdid),
	  m_is_ordered(fOrdered),
	  m_is_splittable(is_splittable),
	  m_hash_agg_capable(is_hash_agg_capable)
{
	SPQOS_ASSERT(mdid->IsValid());

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::~CMDAggregateSPQDB
//
//	@doc:
//		Destructor
//
//---------------------------------------------------------------------------
CMDAggregateSPQDB::~CMDAggregateSPQDB()
{
	m_mdid->Release();
	m_mdid_type_intermediate->Release();
	m_mdid_type_result->Release();
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::MDId
//
//	@doc:
//		Agg id
//
//---------------------------------------------------------------------------
IMDId *
CMDAggregateSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::Mdname
//
//	@doc:
//		Agg name
//
//---------------------------------------------------------------------------
CMDName
CMDAggregateSPQDB::Mdname() const
{
	return *m_mdname;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::GetResultTypeMdid
//
//	@doc:
//		Type id of result
//
//---------------------------------------------------------------------------
IMDId *
CMDAggregateSPQDB::GetResultTypeMdid() const
{
	return m_mdid_type_result;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::GetIntermediateResultTypeMdid
//
//	@doc:
//		Type id of intermediate result
//
//---------------------------------------------------------------------------
IMDId *
CMDAggregateSPQDB::GetIntermediateResultTypeMdid() const
{
	return m_mdid_type_intermediate;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::Serialize
//
//	@doc:
//		Serialize function metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDAggregateSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBAgg));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));

	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	if (m_is_ordered)
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBIsAggOrdered),
			m_is_ordered);
	}

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBAggSplittable),
		m_is_splittable);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBAggHashAggCapable),
		m_hash_agg_capable);

	SerializeMDIdAsElem(
		xml_serializer,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBAggResultTypeId),
		m_mdid_type_result);
	SerializeMDIdAsElem(
		xml_serializer,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBAggIntermediateResultTypeId),
		m_mdid_type_intermediate);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBAgg));
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMDAggregateSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDAggregateSPQDB::DebugPrint(IOstream &os) const
{
	os << "Aggregate id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Aggregate name: " << (Mdname()).GetMDName()->GetBuffer()
	   << std::endl;

	os << "Result type id: ";
	GetResultTypeMdid()->OsPrint(os);
	os << std::endl;

	os << "Intermediate result type id: ";
	GetIntermediateResultTypeMdid()->OsPrint(os);
	os << std::endl;

	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
