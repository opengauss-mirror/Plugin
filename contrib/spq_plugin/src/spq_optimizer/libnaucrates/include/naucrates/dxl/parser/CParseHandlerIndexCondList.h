//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerIndexCondList.h
//
//	@doc:
//		SAX parse handler class for parsing the list of index conditions in a
//		index scan node
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerIndexCondList_H
#define SPQDXL_CParseHandlerIndexCondList_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerIndexCondList
//
//	@doc:
//		SAX parse handler class for parsing the list of index conditions in a
//		index scan node
//
//---------------------------------------------------------------------------
class CParseHandlerIndexCondList : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerIndexCondList(const CParseHandlerIndexCondList &);

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
	CParseHandlerIndexCondList(CMemoryPool *mp,
							   CParseHandlerManager *parse_handler_mgr,
							   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerIndexCondList_H

// EOF
