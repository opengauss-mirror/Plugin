//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerStacktrace.h
//
//	@doc:
//		Pass-through parse handler class for stack traces
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerStacktrace_H
#define SPQDXL_CParseHandlerStacktrace_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerBase.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerStacktrace
//
//	@doc:
//		Pass-through parse handler class for stack traces
//
//---------------------------------------------------------------------------
class CParseHandlerStacktrace : public CParseHandlerBase
{
private:
	// private copy ctor
	CParseHandlerStacktrace(const CParseHandlerStacktrace &);

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
	// ctor/dtor
	CParseHandlerStacktrace(CMemoryPool *mp,
							CParseHandlerManager *parse_handler_mgr,
							CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerStacktrace_H

// EOF
