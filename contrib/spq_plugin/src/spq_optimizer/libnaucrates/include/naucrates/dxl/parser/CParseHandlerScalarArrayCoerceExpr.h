//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Inc.
//
//	@filename:
//		CParseHandlerScalarArrayCoerceExpr.h
//
//	@doc:
//
//		SAX parse handler class for parsing ArrayCoerceExpr operator.
//
//	@owner:
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarArrayCoerceExpr_H
#define SPQDXL_CParseHandlerScalarArrayCoerceExpr_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarArrayCoerceExpr.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarArrayCoerceExpr
//
//	@doc:
//		Parse handler for parsing ArrayCoerceExpr operator
//
//---------------------------------------------------------------------------
class CParseHandlerScalarArrayCoerceExpr : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarArrayCoerceExpr(
		const CParseHandlerScalarArrayCoerceExpr &);

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
	CParseHandlerScalarArrayCoerceExpr(CMemoryPool *mp,
									   CParseHandlerManager *parse_handler_mgr,
									   CParseHandlerBase *parse_handler_root);

	virtual ~CParseHandlerScalarArrayCoerceExpr(){};
};

}  // namespace spqdxl
#endif	// SPQDXL_CParseHandlerScalarArrayCoerceExpr_H

//EOF
