//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CParseHandlerScalarPartBound.h
//
//	@doc:
//		SAX parse handler class for parsing scalar part bound
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarScalarPartBound_H
#define SPQDXL_CParseHandlerScalarScalarPartBound_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarPartBound
//
//	@doc:
//		Parse handler class for parsing scalar part bound
//
//---------------------------------------------------------------------------
class CParseHandlerScalarPartBound : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarPartBound(const CParseHandlerScalarPartBound &);

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
	CParseHandlerScalarPartBound(CMemoryPool *mp,
								 CParseHandlerManager *parse_handler_mgr,
								 CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarScalarPartBound_H

// EOF
