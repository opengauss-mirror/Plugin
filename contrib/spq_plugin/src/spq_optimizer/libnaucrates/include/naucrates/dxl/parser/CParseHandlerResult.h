//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerResult.h
//
//	@doc:
//		SAX parse handler class for parsing result operator nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerResult_H
#define SPQDXL_CParseHandlerResult_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalResult.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerResult
//
//	@doc:
//		Parse handler for parsing a result operator
//
//---------------------------------------------------------------------------
class CParseHandlerResult : public CParseHandlerPhysicalOp
{
private:
	// the result operator
	CDXLPhysicalResult *m_dxl_op;

	// private copy ctor
	CParseHandlerResult(const CParseHandlerResult &);

	// set up initial handlers
	void SetupInitialHandlers();

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
	CParseHandlerResult(CMemoryPool *mp,
						CParseHandlerManager *parse_handler_mgr,
						CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerResult_H

// EOF
