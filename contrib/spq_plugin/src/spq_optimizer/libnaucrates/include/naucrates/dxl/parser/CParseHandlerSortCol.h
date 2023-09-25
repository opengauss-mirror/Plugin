//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerSortCol.h
//
//	@doc:
//		SAX parse handler class for parsing a sorting column in the sorting
//		column list of sort and motion nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerSortCol_H
#define SPQDXL_CParseHandlerSortCol_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerSortCol
//
//	@doc:
//		SAX parse handler class for parsing a sorting column in the sorting
//		column list of sort and motion nodes.
//
//---------------------------------------------------------------------------
class CParseHandlerSortCol : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerSortCol(const CParseHandlerSortCol &);

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
	CParseHandlerSortCol(CMemoryPool *mp,
						 CParseHandlerManager *parse_handler_mgr,
						 CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerSortCol_H

// EOF
