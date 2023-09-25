//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CParseHandlerPhysicalBitmapTableScan.h
//
//	@doc:
//		SAX parse handler class for parsing dynamic bitmap table scan operator nodes
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerPhysicalBitmapTableScan_H
#define SPQDXL_CParseHandlerPhysicalBitmapTableScan_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerPhysicalAbstractBitmapScan.h"

namespace spqdxl
{
using namespace spqos;

// forward declarations
class CDXLPhysicalBitmapTableScan;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerPhysicalBitmapTableScan
//
//	@doc:
//		Parse handler for parsing bitmap table scan operator
//
//---------------------------------------------------------------------------
class CParseHandlerPhysicalBitmapTableScan
	: public CParseHandlerPhysicalAbstractBitmapScan
{
private:
	// private copy ctor
	CParseHandlerPhysicalBitmapTableScan(
		const CParseHandlerPhysicalBitmapTableScan &);

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
	CParseHandlerPhysicalBitmapTableScan(
		CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
		CParseHandlerBase *parse_handler_root)
		: CParseHandlerPhysicalAbstractBitmapScan(mp, parse_handler_mgr,
												  parse_handler_root)
	{
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerPhysicalBitmapTableScan_H

// EOF
