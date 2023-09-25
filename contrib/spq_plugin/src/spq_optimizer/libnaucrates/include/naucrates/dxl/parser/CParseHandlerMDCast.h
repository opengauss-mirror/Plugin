//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CParseHandlerMDCast.h
//
//	@doc:
//		SAX parse handler class for SPQDB cast metadata
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMDCast_H
#define SPQDXL_CParseHandlerMDCast_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"
#include "naucrates/md/CMDFunctionSPQDB.h"


namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerMDCast
//
//	@doc:
//		Parse handler for SPQDB cast function metadata
//
//---------------------------------------------------------------------------
class CParseHandlerMDCast : public CParseHandlerMetadataObject
{
private:
	// private copy ctor
	CParseHandlerMDCast(const CParseHandlerMDCast &);

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
	CParseHandlerMDCast(CMemoryPool *mp,
						CParseHandlerManager *parse_handler_mgr,
						CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMDCast_H

// EOF
