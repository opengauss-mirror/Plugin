//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerCost.h
//
//	@doc:
//		SAX parse handler class for parsing cost estimates of physical operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerCost_H
#define SPQDXL_CParseHandlerCost_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLOperatorCost.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerCost
//
//	@doc:
//		Parse handler for parsing the cost for physical operators
//
//---------------------------------------------------------------------------
class CParseHandlerCost : public CParseHandlerBase
{
private:
	// physical operator cost constructed by the parse handler
	CDXLOperatorCost *m_operator_cost_dxl;

	// private copy ctor
	CParseHandlerCost(const CParseHandlerCost &);

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
	CParseHandlerCost(CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
					  CParseHandlerBase *parse_handler_root);

	~CParseHandlerCost();

	// returns operator cost constructed by the handler
	CDXLOperatorCost *GetDXLOperatorCost();
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerCost_H

// EOF
