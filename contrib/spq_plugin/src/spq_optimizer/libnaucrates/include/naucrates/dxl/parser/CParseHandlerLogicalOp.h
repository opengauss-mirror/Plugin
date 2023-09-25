//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerLogicalOp.h
//
//	@doc:
//		SAX parse handler class for parsing logical operators.
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerLogicalOp_H
#define SPQDXL_CParseHandlerLogicalOp_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerLogicalOp
//
//	@doc:
//		Parse handler for logical operators
//
//
//---------------------------------------------------------------------------
class CParseHandlerLogicalOp : public CParseHandlerOp
{
private:
	// private copy ctor
	CParseHandlerLogicalOp(const CParseHandlerLogicalOp &);

protected:
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
	// ctor/dtor
	CParseHandlerLogicalOp(CMemoryPool *mp,
						   CParseHandlerManager *parse_handler_mgr,
						   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerLogicalOp_H

// EOF
