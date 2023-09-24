//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software, Inc.
//
//	@filename:
//		CParseHandlerMDArrayCoerceCast.h
//
//	@doc:
//		SAX parse handler class for SPQDB array coerce cast metadata
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMDArrayCoerceCast_H
#define SPQDXL_CParseHandlerMDArrayCoerceCast_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"
#include "naucrates/md/CMDFunctionSPQDB.h"


namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

// parse handler for SPQDB array coerce cast function metadata
class CParseHandlerMDArrayCoerceCast : public CParseHandlerMetadataObject
{
private:
	// private copy ctor
	CParseHandlerMDArrayCoerceCast(const CParseHandlerMDArrayCoerceCast &);

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
	CParseHandlerMDArrayCoerceCast(CMemoryPool *mp,
								   CParseHandlerManager *parse_handler_mgr,
								   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMDArrayCoerceCast_H

// EOF
