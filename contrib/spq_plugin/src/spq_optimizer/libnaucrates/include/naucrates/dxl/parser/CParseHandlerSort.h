//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerSort.h
//
//	@doc:
//		SAX parse handler class for parsing sort operator nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerSort_H
#define SPQDXL_CParseHandlerSort_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalSort.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerSort
//
//	@doc:
//		Parse handler for sort operators
//
//---------------------------------------------------------------------------
class CParseHandlerSort : public CParseHandlerPhysicalOp
{
private:
	// the sort operator
	CDXLPhysicalSort *m_dxl_op;

	// private copy ctor
	CParseHandlerSort(const CParseHandlerSort &);

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
	CParseHandlerSort(CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
					  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerSort_H

// EOF
