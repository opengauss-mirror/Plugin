//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerLimit.h
//
//	@doc:
//		SAX parse handler class for parsing limit operator nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerLimit_H
#define SPQDXL_CParseHandlerLimit_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalLimit.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerLimit
//
//	@doc:
//		Parse handler for limit operator
//
//---------------------------------------------------------------------------
class CParseHandlerLimit : public CParseHandlerPhysicalOp
{
private:
	CDXLPhysicalLimit *m_dxl_op;

	// private copy ctor
	CParseHandlerLimit(const CParseHandlerLimit &);

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
	CParseHandlerLimit(CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
					   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl
#endif	// SPQDXL_CParseHandlerLimit_H

// EOF
