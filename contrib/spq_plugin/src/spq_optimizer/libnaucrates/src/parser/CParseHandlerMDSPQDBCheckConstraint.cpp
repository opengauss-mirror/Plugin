//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBCheckConstraint.cpp
//
//	@doc:
//		Implementation of the SAX parse handler class for parsing an MD check constraint
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "naucrates/dxl/parser/CParseHandlerMDSPQDBCheckConstraint.h"

#include "naucrates/dxl/operators/CDXLOperatorFactory.h"
#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/parser/CParseHandlerManager.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"
#include "naucrates/md/CMDCheckConstraintSPQDB.h"

using namespace spqdxl;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBCheckConstraint::CParseHandlerMDSPQDBCheckConstraint
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CParseHandlerMDSPQDBCheckConstraint::CParseHandlerMDSPQDBCheckConstraint(
	CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
	CParseHandlerBase *parse_handler_root)
	: CParseHandlerMetadataObject(mp, parse_handler_mgr, parse_handler_root),
	  m_mdid(NULL),
	  m_mdname(NULL),
	  m_rel_mdid(NULL)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBCheckConstraint::StartElement
//
//	@doc:
//		Invoked by Xerces to process an opening tag
//
//---------------------------------------------------------------------------
void
CParseHandlerMDSPQDBCheckConstraint::StartElement(
	const XMLCh *const,	 // element_uri,
	const XMLCh *const element_local_name,
	const XMLCh *const,	 // element_qname,
	const Attributes &attrs)
{
	if (0 != XMLString::compareString(
				 CDXLTokens::XmlstrToken(EdxltokenCheckConstraint),
				 element_local_name))
	{
		CWStringDynamic *str = CDXLUtils::CreateDynamicStringFromXMLChArray(
			m_parse_handler_mgr->GetDXLMemoryManager(), element_local_name);
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag,
				   str->GetBuffer());
	}

	// new md object
	SPQOS_ASSERT(NULL == m_mdid);

	// parse mdid
	m_mdid = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
		m_parse_handler_mgr->GetDXLMemoryManager(), attrs, EdxltokenMdid,
		EdxltokenCheckConstraint);

	// parse check constraint name
	const XMLCh *parsed_column_name = CDXLOperatorFactory::ExtractAttrValue(
		attrs, EdxltokenName, EdxltokenCheckConstraint);
	CWStringDynamic *column_name = CDXLUtils::CreateDynamicStringFromXMLChArray(
		m_parse_handler_mgr->GetDXLMemoryManager(), parsed_column_name);

	// create a copy of the string in the CMDName constructor
	m_mdname = SPQOS_NEW(m_mp) CMDName(m_mp, column_name);
	SPQOS_DELETE(column_name);

	// parse mdid of relation
	m_rel_mdid = CDXLOperatorFactory::ExtractConvertAttrValueToMdId(
		m_parse_handler_mgr->GetDXLMemoryManager(), attrs,
		EdxltokenRelationMdid, EdxltokenCheckConstraint);

	// create and activate the parse handler for the child scalar expression node
	CParseHandlerBase *scalar_expr_handler_base =
		CParseHandlerFactory::GetParseHandler(
			m_mp, CDXLTokens::XmlstrToken(EdxltokenScalar), m_parse_handler_mgr,
			this);
	m_parse_handler_mgr->ActivateParseHandler(scalar_expr_handler_base);

	// store parse handler
	this->Append(scalar_expr_handler_base);
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerMDSPQDBCheckConstraint::EndElement
//
//	@doc:
//		Invoked by Xerces to process a closing tag
//
//---------------------------------------------------------------------------
void
CParseHandlerMDSPQDBCheckConstraint::EndElement(
	const XMLCh *const,	 // element_uri,
	const XMLCh *const element_local_name,
	const XMLCh *const	// element_qname
)
{
	if (0 != XMLString::compareString(
				 CDXLTokens::XmlstrToken(EdxltokenCheckConstraint),
				 element_local_name))
	{
		CWStringDynamic *str = CDXLUtils::CreateDynamicStringFromXMLChArray(
			m_parse_handler_mgr->GetDXLMemoryManager(), element_local_name);
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag,
				   str->GetBuffer());
	}

	// get node for default value expression from child parse handler
	CParseHandlerScalarOp *op_parse_handler =
		dynamic_cast<CParseHandlerScalarOp *>((*this)[0]);

	CDXLNode *dxlnode_scalar_expr = op_parse_handler->CreateDXLNode();
	SPQOS_ASSERT(NULL != dxlnode_scalar_expr);
	dxlnode_scalar_expr->AddRef();

	m_imd_obj = SPQOS_NEW(m_mp) CMDCheckConstraintSPQDB(
		m_mp, m_mdid, m_mdname, m_rel_mdid, dxlnode_scalar_expr);

	// deactivate handler
	m_parse_handler_mgr->DeactivateHandler();
}

// EOF
