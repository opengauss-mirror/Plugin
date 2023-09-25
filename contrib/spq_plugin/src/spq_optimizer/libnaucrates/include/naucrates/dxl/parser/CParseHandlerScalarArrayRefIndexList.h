//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CParseHandlerScalarArrayRefIndexList.h
//
//	@doc:
//		SAX parse handler class for parsing the list of arrayref indexes
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarScalarArrayRefIndexList_H
#define SPQDXL_CParseHandlerScalarScalarArrayRefIndexList_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarArrayRefIndexList
//
//	@doc:
//		Parse handler class for parsing the list of arrayref indexes
//
//---------------------------------------------------------------------------
class CParseHandlerScalarArrayRefIndexList : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarArrayRefIndexList(
		const CParseHandlerScalarArrayRefIndexList &);

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
	CParseHandlerScalarArrayRefIndexList(
		CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
		CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarScalarArrayRefIndexList_H

// EOF
