//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBFunc.cpp
//
//	@doc:
//		Implementation of the SAX parse handler class for parsing metadata for
//		SPQDB functions.
//---------------------------------------------------------------------------

#include "naucrates/dxl/parser/CParseHandlerMDSPQDBFunc.h"

#include "naucrates/dxl/operators/CDXLOperatorFactory.h"
#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/parser/CParseHandlerManager.h"


using namespace spqdxl;


XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBFunc::CParseHandlerMDSPQDBFunc
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CParseHandlerMDSPQDBFunc::CParseHandlerMDSPQDBFunc(
	CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
	CParseHandlerBase *parse_handler_root)
	: CParseHandlerMetadataObject(mp, parse_handler_mgr, parse_handler_root),
	  m_mdid(NULL),
	  m_mdname(NULL),
	  m_mdid_type_result(NULL),
	  m_mdid_types_array(NULL),
	  m_func_stability(CMDFunctionSPQDB::EfsSentinel)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBFunc::StartElement
//
//	@doc:
//		Invoked by Xerces to process an opening tag
//
//---------------------------------------------------------------------------
void
CParseHandlerMDSPQDBFunc::StartElement(const XMLCh *const,  // element_uri,
									  const XMLCh *const element_local_name,
									  const XMLCh *const,  // element_qname
									  const Attributes &attrs)
{
	if (0 ==
		XMLString::compareString(CDXLTokens::XmlstrToken(EdxltokenSPQDBFunc),
								 element_local_name))
	{
		// parse func name
		const XMLCh *xml_str_func_name = CDXLOperatorFactory::ExtractAttrValue(
			attrs, EdxltokenName, EdxltokenSPQDBFunc);

		CWStringDynamic *str_func_name =
			CDXLUtils::CreateDynamicStringFromXMLChArray(
				m_parse_handler_mgr->GetDXLMemoryManager(), xml_str_func_name);

		// create a copy of the string in the CMDName constructor
		m_mdname = SPQOS_NEW(m_mp) CMDName(m_mp, str_func_name);

		SPQOS_DELETE(str_func_name);

		// parse metadata id info
		m_mdid = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs, EdxltokenMdid,
			EdxltokenSPQDBFunc);

		// parse whether func returns a set
		m_returns_set = CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBFuncReturnsSet, EdxltokenSPQDBFunc);
		// parse whether func is strict
		m_is_strict = CDXLOperatorFactory::ExtractConvertAttrValueToBool(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
			EdxltokenSPQDBFuncStrict, EdxltokenSPQDBFunc);

		// parse whether func is NDV-preserving
		m_is_ndv_preserving =
			CDXLOperatorFactory::ExtractConvertAttrValueToBool(
				m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
				EdxltokenSPQDBFuncNDVPreserving, EdxltokenSPQDBFunc,
				true,  // optional
				false  // default is false
			);

		// parse whether func is a lossy cast allowed for partition selection
		m_is_allowed_for_PS =
			CDXLOperatorFactory::ExtractConvertAttrValueToBool(
				m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
				EdxltokenSPQDBFuncIsAllowedForPS, EdxltokenSPQDBFunc,
				true,  // optional
				false  // default is false
			);

		// parse func stability property
		const XMLCh *xmlszStbl = CDXLOperatorFactory::ExtractAttrValue(
			attrs, EdxltokenSPQDBFuncStability, EdxltokenSPQDBFunc);

		m_func_stability = ParseFuncStability(xmlszStbl);

		// parse func data access property
		const XMLCh *xmlszDataAcc = CDXLOperatorFactory::ExtractAttrValue(
			attrs, EdxltokenSPQDBFuncDataAccess, EdxltokenSPQDBFunc);

		m_func_data_access = ParseFuncDataAccess(xmlszDataAcc);
	}
	else if (0 == XMLString::compareString(
					  CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncResultTypeId),
					  element_local_name))
	{
		// parse result type
		SPQOS_ASSERT(NULL != m_mdname);

		m_mdid_type_result = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
			m_parse_handler_mgr->GetDXLMemoryManager(), attrs, EdxltokenMdid,
			EdxltokenSPQDBFuncResultTypeId);
	}
	else if (0 == XMLString::compareString(
					  CDXLTokens::XmlstrToken(EdxltokenOutputCols),
					  element_local_name))
	{
		// parse output column type
		SPQOS_ASSERT(NULL != m_mdname);
		SPQOS_ASSERT(NULL == m_mdid_types_array);

		const XMLCh *xmlszTypes = CDXLOperatorFactory::ExtractAttrValue(
			attrs, EdxltokenTypeIds, EdxltokenOutputCols);

		m_mdid_types_array = CDXLOperatorFactory::ExtractConvertMdIdsToArray(
			m_parse_handler_mgr->GetDXLMemoryManager(), xmlszTypes,
			EdxltokenTypeIds, EdxltokenOutputCols);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBFunc::EndElement
//
//	@doc:
//		Invoked by Xerces to process a closing tag
//
//---------------------------------------------------------------------------
void
CParseHandlerMDSPQDBFunc::EndElement(const XMLCh *const,	 // element_uri,
									const XMLCh *const element_local_name,
									const XMLCh *const	// element_qname
)
{
	if (0 ==
		XMLString::compareString(CDXLTokens::XmlstrToken(EdxltokenSPQDBFunc),
								 element_local_name))
	{
		// construct the MD func object from its part
		SPQOS_ASSERT(m_mdid->IsValid() && NULL != m_mdname);

		m_imd_obj = SPQOS_NEW(m_mp) CMDFunctionSPQDB(
			m_mp, m_mdid, m_mdname, m_mdid_type_result, m_mdid_types_array,
			m_returns_set, m_func_stability, m_func_data_access, m_is_strict,
			m_is_ndv_preserving, m_is_allowed_for_PS);

		// deactivate handler
		m_parse_handler_mgr->DeactivateHandler();
	}
	else if (0 != XMLString::compareString(
					  CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncResultTypeId),
					  element_local_name) &&
			 0 != XMLString::compareString(
					  CDXLTokens::XmlstrToken(EdxltokenOutputCols),
					  element_local_name))
	{
		CWStringDynamic *str = CDXLUtils::CreateDynamicStringFromXMLChArray(
			m_parse_handler_mgr->GetDXLMemoryManager(), element_local_name);
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag,
				   str->GetBuffer());
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBFunc::ParseFuncStability
//
//	@doc:
//		Parses function stability property from XML string
//
//---------------------------------------------------------------------------
CMDFunctionSPQDB::EFuncStbl
CParseHandlerMDSPQDBFunc::ParseFuncStability(const XMLCh *xml_val)
{
	if (0 == XMLString::compareString(
				 CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncStable), xml_val))
	{
		return CMDFunctionSPQDB::EfsStable;
	}

	if (0 == XMLString::compareString(
				 CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncImmutable), xml_val))
	{
		return CMDFunctionSPQDB::EfsImmutable;
	}

	if (0 == XMLString::compareString(
				 CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncVolatile), xml_val))
	{
		return CMDFunctionSPQDB::EfsVolatile;
	}

	SPQOS_RAISE(
		spqdxl::ExmaDXL, spqdxl::ExmiDXLInvalidAttributeValue,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncStability)->GetBuffer(),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFunc)->GetBuffer());

	return CMDFunctionSPQDB::EfsSentinel;
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBFunc::ParseFuncDataAccess
//
//	@doc:
//		Parses function data access property from XML string
//
//---------------------------------------------------------------------------
CMDFunctionSPQDB::EFuncDataAcc
CParseHandlerMDSPQDBFunc::ParseFuncDataAccess(const XMLCh *xml_val)
{
	if (0 == XMLString::compareString(
				 CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncNoSQL), xml_val))
	{
		return CMDFunctionSPQDB::EfdaNoSQL;
	}

	if (0 ==
		XMLString::compareString(
			CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncContainsSQL), xml_val))
	{
		return CMDFunctionSPQDB::EfdaContainsSQL;
	}

	if (0 ==
		XMLString::compareString(
			CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncReadsSQLData), xml_val))
	{
		return CMDFunctionSPQDB::EfdaReadsSQLData;
	}

	if (0 ==
		XMLString::compareString(
			CDXLTokens::XmlstrToken(EdxltokenSPQDBFuncModifiesSQLData), xml_val))
	{
		return CMDFunctionSPQDB::EfdaModifiesSQLData;
	}

	SPQOS_RAISE(
		spqdxl::ExmaDXL, spqdxl::ExmiDXLInvalidAttributeValue,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncDataAccess)->GetBuffer(),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFunc)->GetBuffer());

	return CMDFunctionSPQDB::EfdaSentinel;
}

// EOF
