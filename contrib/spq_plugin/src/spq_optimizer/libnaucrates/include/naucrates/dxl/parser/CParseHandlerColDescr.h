//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerColDescr.h
//
//	@doc:
//		SAX parse handler class for parsing the list of column descriptors
//		in a table descriptor node.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerColumnDescriptor_H
#define SPQDXL_CParseHandlerColumnDescriptor_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLColDescr.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"

namespace spqdxl
{
using namespace spqos;


XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerColDescr
//
//	@doc:
//		Parse handler for column descriptor lists
//
//---------------------------------------------------------------------------
class CParseHandlerColDescr : public CParseHandlerBase
{
private:
	// array of column descriptors to build
	CDXLColDescrArray *m_dxl_column_descr_array;

	// current column descriptor being parsed
	CDXLColDescr *m_current_column_descr;

	// private copy ctor
	CParseHandlerColDescr(const CParseHandlerColDescr &);

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
	CParseHandlerColDescr(CMemoryPool *m_mp,
						  CParseHandlerManager *parse_handler_mgr,
						  CParseHandlerBase *parse_handler_base);

	virtual ~CParseHandlerColDescr();

	CDXLColDescrArray *GetDXLColumnDescrArray();
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerColumnDescriptor_H

// EOF
