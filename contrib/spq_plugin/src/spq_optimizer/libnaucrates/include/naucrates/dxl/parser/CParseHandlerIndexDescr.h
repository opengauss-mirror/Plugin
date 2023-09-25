//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerIndexDescr.h
//
//	@doc:
//		SAX parse handler class for parsing the index descriptor portions
//		of index scan operator nodes
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerIndexDescriptor_H
#define SPQDXL_CParseHandlerIndexDescriptor_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLIndexDescr.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerIndexDescr
//
//	@doc:
//		Parse handler for parsing an index descriptor
//
//---------------------------------------------------------------------------
class CParseHandlerIndexDescr : public CParseHandlerBase
{
private:
	// the index descriptor to construct
	CDXLIndexDescr *m_dxl_index_descr;

	// private copy ctor
	CParseHandlerIndexDescr(const CParseHandlerIndexDescr &);

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
	CParseHandlerIndexDescr(CMemoryPool *, CParseHandlerManager *,
							CParseHandlerBase *);

	//dtor
	~CParseHandlerIndexDescr();

	CDXLIndexDescr *GetDXLIndexDescr();
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerIndexDescriptor_H

// EOF
