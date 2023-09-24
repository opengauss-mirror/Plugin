//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerScalarFuncExpr.h
//
//	@doc:
//
//		SAX parse handler class for parsing scalar FuncExpr.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarFuncExpr_H
#define SPQDXL_CParseHandlerScalarFuncExpr_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarFuncExpr.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarFuncExpr
//
//	@doc:
//		Parse handler for parsing a scalar func expression
//
//---------------------------------------------------------------------------
class CParseHandlerScalarFuncExpr : public CParseHandlerScalarOp
{
private:
	BOOL m_inside_func_expr;

	// private copy ctor
	CParseHandlerScalarFuncExpr(const CParseHandlerScalarFuncExpr &);

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
	CParseHandlerScalarFuncExpr(CMemoryPool *mp,
								CParseHandlerManager *parse_handler_mgr,
								CParseHandlerBase *parse_handler_root);
};

}  // namespace spqdxl
#endif	// !SPQDXL_CParseHandlerScalarFuncExpr_H

//EOF
