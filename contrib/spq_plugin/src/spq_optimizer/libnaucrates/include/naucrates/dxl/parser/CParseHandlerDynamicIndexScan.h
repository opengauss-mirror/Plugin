//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CParseHandlerDynamicIndexScan.h
//
//	@doc:
//		SAX parse handler class for parsing dynamic index scan operator nodes
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerDynamicIndexScan_H
#define SPQDXL_CParseHandlerDynamicIndexScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalIndexScan.h"
#include "naucrates/dxl/parser/CParseHandlerIndexScan.h"
#include "naucrates/dxl/xml/dxltokens.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerDynamicIndexScan
//
//	@doc:
//		Parse handler for index scan operator nodes
//
//---------------------------------------------------------------------------
class CParseHandlerDynamicIndexScan : public CParseHandlerIndexScan
{
private:
	// part index id
	ULONG m_part_index_id;

	// printable partition index id
	ULONG m_part_index_id_printable;

	// private copy ctor
	CParseHandlerDynamicIndexScan(const CParseHandlerDynamicIndexScan &);

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
	CParseHandlerDynamicIndexScan(CMemoryPool *mp,
								  CParseHandlerManager *parse_handler_mgr,
								  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerDynamicIndexScan_H

// EOF
