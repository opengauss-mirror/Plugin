//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerLogicalCTEProducer.h
//
//	@doc:
//		Parse handler for parsing a logical CTE producer operator
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CParseHandlerLogicalCTEProducer_H
#define SPQDXL_CParseHandlerLogicalCTEProducer_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerLogicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerLogicalCTEProducer
//
//	@doc:
//		Parse handler for parsing a logical CTE producer operator
//
//---------------------------------------------------------------------------
class CParseHandlerLogicalCTEProducer : public CParseHandlerLogicalOp
{
private:
	// private copy ctor
	CParseHandlerLogicalCTEProducer(const CParseHandlerLogicalCTEProducer &);

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
	CParseHandlerLogicalCTEProducer(CMemoryPool *mp,
									CParseHandlerManager *parse_handler_mgr,
									CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerLogicalCTEProducer_H

// EOF
