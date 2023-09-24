//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerScalarIdent.h
//
//	@doc:
//		SAX parse handler class for parsing scalar identifier nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarIdent_H
#define SPQDXL_CParseHandlerScalarIdent_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarIdent.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarIdent
//
//	@doc:
//		Parse handler for parsing a scalar identifier operator
//
//---------------------------------------------------------------------------
class CParseHandlerScalarIdent : public CParseHandlerScalarOp
{
private:
	// the scalar identifier
	CDXLScalarIdent *m_dxl_op;

	// private copy ctor
	CParseHandlerScalarIdent(const CParseHandlerScalarIdent &);

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
	CParseHandlerScalarIdent(CMemoryPool *mp,
							 CParseHandlerManager *parse_handler_mgr,
							 CParseHandlerBase *parse_handler_root);

	virtual ~CParseHandlerScalarIdent();
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarIdent_H

// EOF
