//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CParseHandlerScalarBitmapIndexProbe.h
//
//	@doc:
//		SAX parse handler class for parsing bitmap index probe operator nodes
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarBitmapIndexProbe_H
#define SPQDXL_CParseHandlerScalarBitmapIndexProbe_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarBitmapIndexProbe
//
//	@doc:
//		Parse handler for bitmap index probe operator nodes
//
//---------------------------------------------------------------------------
class CParseHandlerScalarBitmapIndexProbe : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarBitmapIndexProbe(
		const CParseHandlerScalarBitmapIndexProbe &);

	// process the start of an element
	virtual void StartElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname,		// element's qname
		const Attributes &attr					// element's attributes
	);

	// process the end of an element
	virtual void EndElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname		// element's qname
	);

public:
	// ctor
	CParseHandlerScalarBitmapIndexProbe(CMemoryPool *mp,
										CParseHandlerManager *parse_handler_mgr,
										CParseHandlerBase *parse_handler_root);
};	// class CParseHandlerScalarBitmapIndexProbe
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarBitmapIndexProbe_H

// EOF
