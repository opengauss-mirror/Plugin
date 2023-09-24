//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CParseHandlerPhysicalDynamicBitmapTableScan.h
//
//	@doc:
//		SAX parse handler class for parsing dynamic bitmap table scan operator nodes
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerPhysicalDynamicBitmapTableScan_H
#define SPQDXL_CParseHandlerPhysicalDynamicBitmapTableScan_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLOperatorFactory.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalAbstractBitmapScan.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerPhysicalDynamicBitmapTableScan
//
//	@doc:
//		Parse handler for parsing dynamic bitmap table scan operator
//
//---------------------------------------------------------------------------
class CParseHandlerPhysicalDynamicBitmapTableScan
	: public CParseHandlerPhysicalAbstractBitmapScan
{
private:
	// private copy ctor
	CParseHandlerPhysicalDynamicBitmapTableScan(
		const CParseHandlerPhysicalDynamicBitmapTableScan &);

	// part index id
	ULONG m_part_index_id;

	// printable partition index id
	ULONG m_part_index_id_printable;

	// process the start of an element
	virtual void StartElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname,		// element's qname
		const Attributes &attr					// element's attributes
	);

	// process the end of an element
	virtual void EndElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname		// element's qname
	);

public:
	// ctor
	CParseHandlerPhysicalDynamicBitmapTableScan(
		CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
		CParseHandlerBase *parse_handler_root)
		: CParseHandlerPhysicalAbstractBitmapScan(mp, parse_handler_mgr,
												  parse_handler_root),
		  m_part_index_id(0),
		  m_part_index_id_printable(0)
	{
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerPhysicalDynamicBitmapTableScan_H

// EOF
