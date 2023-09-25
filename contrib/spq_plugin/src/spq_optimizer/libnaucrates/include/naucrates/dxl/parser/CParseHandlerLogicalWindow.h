//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerLogicalWindow.h
//
//	@doc:
//		Parse handler for parsing a logical window operator
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CParseHandlerLogicalWindow_H
#define SPQDXL_CParseHandlerLogicalWindow_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogicalWindow.h"
#include "naucrates/dxl/operators/CDXLWindowSpec.h"
#include "naucrates/dxl/parser/CParseHandlerLogicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerLogicalWindow
//
//	@doc:
//		Parse handler for parsing a logical window operator
//
//---------------------------------------------------------------------------
class CParseHandlerLogicalWindow : public CParseHandlerLogicalOp
{
private:
	// list of window specification
	CDXLWindowSpecArray *m_window_spec_array;

	// private copy ctor
	CParseHandlerLogicalWindow(const CParseHandlerLogicalWindow &);

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
	CParseHandlerLogicalWindow(CMemoryPool *mp,
							   CParseHandlerManager *parse_handler_mgr,
							   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerLogicalWindow_H

// EOF
