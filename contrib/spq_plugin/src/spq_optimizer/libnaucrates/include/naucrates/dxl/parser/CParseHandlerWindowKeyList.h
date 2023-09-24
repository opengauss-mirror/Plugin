//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerWindowKeyList.h
//
//	@doc:
//		SAX parse handler class for parsing the list of window keys
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerWindowKeyList_H
#define SPQDXL_CParseHandlerWindowKeyList_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLWindowKey.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerWindowKeyList
//
//	@doc:
//		SAX parse handler class for parsing the list of window keys
//
//---------------------------------------------------------------------------
class CParseHandlerWindowKeyList : public CParseHandlerBase
{
private:
	// list of window keys
	CDXLWindowKeyArray *m_dxl_window_key_array;

	// private copy ctor
	CParseHandlerWindowKeyList(const CParseHandlerWindowKeyList &);

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
	CParseHandlerWindowKeyList(CMemoryPool *mp,
							   CParseHandlerManager *parse_handler_mgr,
							   CParseHandlerBase *parse_handler_root);

	// list of window keys
	CDXLWindowKeyArray *
	GetDxlWindowKeyArray() const
	{
		return m_dxl_window_key_array;
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerWindowKeyList_H

// EOF
