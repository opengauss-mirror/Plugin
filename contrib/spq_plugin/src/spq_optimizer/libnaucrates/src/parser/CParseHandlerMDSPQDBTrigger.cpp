//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBTrigger.cpp
//
//	@doc:
//		Implementation of the SAX parse handler class for parsing metadata for
//		SPQDB triggers
//---------------------------------------------------------------------------

#include "naucrates/dxl/parser/CParseHandlerMDSPQDBTrigger.h"

#include "naucrates/dxl/operators/CDXLOperatorFactory.h"
#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/parser/CParseHandlerManager.h"

#define SPQMD_TRIGGER_ROW_BIT 0
#define SPQMD_TRIGGER_BEFORE_BIT 1
#define SPQMD_TRIGGER_INSERT_BIT 2
#define SPQMD_TRIGGER_DELETE_BIT 3
#define SPQMD_TRIGGER_UPDATE_BIT 4
#define SPQMD_TRIGGER_BITMAP_LEN 5

using namespace spqdxl;


XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBTrigger::CParseHandlerMDSPQDBTrigger
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CParseHandlerMDSPQDBTrigger::CParseHandlerMDSPQDBTrigger(
	CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
	CParseHandlerBase *parse_handler_root)
	: CParseHandlerMetadataObject(mp, parse_handler_mgr, parse_handler_root),
	  m_mdid(NULL),
	  m_mdname(NULL),
	  m_rel_mdid(NULL),
	  m_func_mdid(NULL),
	  m_type(0),
	  m_is_enabled(false)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBTrigger::StartElement
//
//	@doc:
//		Invoked by Xerces to process an opening tag
//
//---------------------------------------------------------------------------
void
CParseHandlerMDSPQDBTrigger::StartElement(const XMLCh *const,  // element_uri,
										 const XMLCh *const element_local_name,
										 const XMLCh *const,  // element_qname
										 const Attributes &attrs)
{
	if (0 !=
		XMLString::compareString(CDXLTokens::XmlstrToken(EdxltokenSPQDBTrigger),
								 element_local_name))
	{
		CWStringDynamic *str = CDXLUtils::CreateDynamicStringFromXMLChArray(
			m_parse_handler_mgr->GetDXLMemoryManager(), element_local_name);
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag,
				   str->GetBuffer());
	}

	m_mdid = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
		m_parse_handler_mgr->GetDXLMemoryManager(), attrs, EdxltokenMdid,
		EdxltokenSPQDBTrigger);

	const XMLCh *xml_str_name = CDXLOperatorFactory::ExtractAttrValue(
		attrs, EdxltokenName, EdxltokenSPQDBTrigger);
	CWStringDynamic *str_name = CDXLUtils::CreateDynamicStringFromXMLChArray(
		m_parse_handler_mgr->GetDXLMemoryManager(), xml_str_name);
	m_mdname = SPQOS_NEW(m_mp) CMDName(m_mp, str_name);
	SPQOS_DELETE(str_name);
	SPQOS_ASSERT(m_mdid->IsValid() && NULL != m_mdname);

	m_rel_mdid = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
		m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
		EdxltokenRelationMdid, EdxltokenSPQDBTrigger);
	m_func_mdid = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
		m_parse_handler_mgr->GetDXLMemoryManager(), attrs, EdxltokenFuncId,
		EdxltokenSPQDBTrigger);

	BOOL spqmd_trigger_properties[SPQMD_TRIGGER_BITMAP_LEN];
	spqmd_trigger_properties[SPQMD_TRIGGER_ROW_BIT] =
		CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBTriggerRow, EdxltokenSPQDBTrigger);
	spqmd_trigger_properties[SPQMD_TRIGGER_BEFORE_BIT] =
		CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBTriggerBefore, EdxltokenSPQDBTrigger);
	spqmd_trigger_properties[SPQMD_TRIGGER_INSERT_BIT] =
		CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBTriggerInsert, EdxltokenSPQDBTrigger);
	spqmd_trigger_properties[SPQMD_TRIGGER_DELETE_BIT] =
		CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBTriggerDelete, EdxltokenSPQDBTrigger);
	spqmd_trigger_properties[SPQMD_TRIGGER_UPDATE_BIT] =
		CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBTriggerUpdate, EdxltokenSPQDBTrigger);

	for (ULONG idx = 0; idx < SPQMD_TRIGGER_BITMAP_LEN; idx++)
	{
		// if the current property flag is true then set the corresponding bit
		if (spqmd_trigger_properties[idx])
		{
			m_type |= (1 << idx);
		}
	}

	m_is_enabled = CDXLOperatorFactory::ExtractConvertAttrValueToBool(
		m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
		EdxltokenSPQDBTriggerEnabled, EdxltokenSPQDBTrigger);
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBTrigger::EndElement
//
//	@doc:
//		Invoked by Xerces to process a closing tag
//
//---------------------------------------------------------------------------
void
CParseHandlerMDSPQDBTrigger::EndElement(const XMLCh *const,	// element_uri,
									   const XMLCh *const element_local_name,
									   const XMLCh *const  // element_qname
)
{
	if (0 !=
		XMLString::compareString(CDXLTokens::XmlstrToken(EdxltokenSPQDBTrigger),
								 element_local_name))
	{
		CWStringDynamic *str = CDXLUtils::CreateDynamicStringFromXMLChArray(
			m_parse_handler_mgr->GetDXLMemoryManager(), element_local_name);
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag,
				   str->GetBuffer());
	}

	// construct the MD trigger object
	m_imd_obj = SPQOS_NEW(m_mp) CMDTriggerSPQDB(
		m_mp, m_mdid, m_mdname, m_rel_mdid, m_func_mdid, m_type, m_is_enabled);

	// deactivate handler
	m_parse_handler_mgr->DeactivateHandler();
}

// EOF
