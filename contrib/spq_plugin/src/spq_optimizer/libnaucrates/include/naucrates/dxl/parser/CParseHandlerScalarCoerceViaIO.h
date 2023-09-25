//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CParseHandlerScalarCoerceViaIO.h
//
//	@doc:
//
//		SAX parse handler class for parsing CoerceViaIO operator.
//
//	@owner:
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarCoerceViaIO_H
#define SPQDXL_CParseHandlerScalarCoerceViaIO_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarCoerceViaIO.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarCoerceViaIO
//
//	@doc:
//		Parse handler for parsing CoerceViaIO operator
//
//---------------------------------------------------------------------------
class CParseHandlerScalarCoerceViaIO : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarCoerceViaIO(const CParseHandlerScalarCoerceViaIO &);

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
	CParseHandlerScalarCoerceViaIO(CMemoryPool *mp,
								   CParseHandlerManager *parse_handler_mgr,
								   CParseHandlerBase *parse_handler_root);

	virtual ~CParseHandlerScalarCoerceViaIO(){};
};

}  // namespace spqdxl
#endif	// SPQDXL_CParseHandlerScalarCoerceViaIO_H

//EOF
