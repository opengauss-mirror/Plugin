//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerGatherMotion.h
//
//	@doc:
//		SAX parse handler class for parsing gather motion operator nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerGatherMotion_H
#define SPQDXL_CParseHandlerGatherMotion_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalGatherMotion.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerGatherMotion
//
//	@doc:
//		Parse handler for gather motion operators
//
//---------------------------------------------------------------------------
class CParseHandlerGatherMotion : public CParseHandlerPhysicalOp
{
private:
	// the gather motion operator
	CDXLPhysicalGatherMotion *m_dxl_op;

	// private copy ctor
	CParseHandlerGatherMotion(const CParseHandlerGatherMotion &);

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
	CParseHandlerGatherMotion(CMemoryPool *mp,
							  CParseHandlerManager *parse_handler_mgr,
							  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerGatherMotion_H

// EOF
