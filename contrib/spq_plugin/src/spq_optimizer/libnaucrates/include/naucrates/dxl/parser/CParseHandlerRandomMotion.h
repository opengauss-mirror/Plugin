//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerRandomMotion.h
//
//	@doc:
//		SAX parse handler class for parsing random motion operator nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerRandomMotion_H
#define SPQDXL_CParseHandlerRandomMotion_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalRandomMotion.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerRandomMotion
//
//	@doc:
//		Parse handler for routed motion operators
//
//---------------------------------------------------------------------------
class CParseHandlerRandomMotion : public CParseHandlerPhysicalOp
{
private:
	// motion operator
	CDXLPhysicalRandomMotion *m_dxl_op;

	// private copy ctor
	CParseHandlerRandomMotion(const CParseHandlerRandomMotion &);

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
	CParseHandlerRandomMotion(CMemoryPool *mp,
							  CParseHandlerManager *parse_handler_mgr,
							  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerRandomMotion_H

// EOF
