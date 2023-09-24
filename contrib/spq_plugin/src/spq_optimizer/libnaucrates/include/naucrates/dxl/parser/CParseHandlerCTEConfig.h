//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CParseHandlerCTEConfig.h
//
//	@doc:
//		SAX parse handler class for parsing CTE configuration
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerCTEConfig_H
#define SPQDXL_CParseHandlerCTEConfig_H

#include "spqos/base.h"

#include "spqopt/engine/CCTEConfig.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerCTEConfig
//
//	@doc:
//		SAX parse handler class for parsing CTE configuration options
//
//---------------------------------------------------------------------------
class CParseHandlerCTEConfig : public CParseHandlerBase
{
private:
	// CTE configuration
	CCTEConfig *m_cte_conf;

	// private copy ctor
	CParseHandlerCTEConfig(const CParseHandlerCTEConfig &);

	// process the start of an element
	void StartElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname,		// element's qname
		const Attributes &attr					// element's attributes
	);

	// process the end of an element
	void EndElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname		// element's qname
	);

public:
	// ctor
	CParseHandlerCTEConfig(CMemoryPool *mp,
						   CParseHandlerManager *parse_handler_mgr,
						   CParseHandlerBase *parse_handler_root);

	// dtor
	virtual ~CParseHandlerCTEConfig();

	// type of the parse handler
	virtual EDxlParseHandlerType GetParseHandlerType() const;

	// enumerator configuration
	CCTEConfig *GetCteConf() const;
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerCTEConfig_H

// EOF
