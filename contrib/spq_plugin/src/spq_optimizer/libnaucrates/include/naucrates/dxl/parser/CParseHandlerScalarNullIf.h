//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerScalarNullIf.h
//
//	@doc:
//		SAX parse handler class for parsing scalar NullIf operator
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarNullIf_H
#define SPQDXL_CParseHandlerScalarNullIf_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarNullIf.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarNullIf
//
//	@doc:
//		Parse handler for parsing scallar NullIf operator
//
//---------------------------------------------------------------------------
class CParseHandlerScalarNullIf : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarNullIf(const CParseHandlerScalarNullIf &);

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
	CParseHandlerScalarNullIf(CMemoryPool *mp,
							  CParseHandlerManager *parse_handler_mgr,
							  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarNullIf_H

// EOF
