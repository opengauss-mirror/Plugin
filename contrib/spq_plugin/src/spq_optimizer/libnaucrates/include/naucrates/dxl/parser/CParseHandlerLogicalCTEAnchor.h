//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerLogicalCTEAnchor.h
//
//	@doc:
//		Parse handler for parsing a logical CTE anchor operator
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CParseHandlerLogicalCTEAnchor_H
#define SPQDXL_CParseHandlerLogicalCTEAnchor_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerLogicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerLogicalCTEAnchor
//
//	@doc:
//		Parse handler for parsing a logical CTE anchor operator
//
//---------------------------------------------------------------------------
class CParseHandlerLogicalCTEAnchor : public CParseHandlerLogicalOp
{
private:
	// private copy ctor
	CParseHandlerLogicalCTEAnchor(const CParseHandlerLogicalCTEAnchor &);

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
	CParseHandlerLogicalCTEAnchor(CMemoryPool *mp,
								  CParseHandlerManager *parse_handler_mgr,
								  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerLogicalCTEAnchor_H

// EOF
