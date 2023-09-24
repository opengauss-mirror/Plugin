//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerDistinctComp.h
//
//	@doc:
//		SAX parse handler class for parsing distinct comparison nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerDistinctComp_H
#define SPQDXL_CParseHandlerDistinctComp_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarDistinctComp.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerDistinctComp
//
//	@doc:
//		Parse handler for parsing a distinct comparison operator
//
//---------------------------------------------------------------------------
class CParseHandlerDistinctComp : public CParseHandlerScalarOp
{
private:
	// the distinct comparison operator
	CDXLScalarDistinctComp *m_dxl_op;

	// private copy ctor
	CParseHandlerDistinctComp(const CParseHandlerDistinctComp &);

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
	CParseHandlerDistinctComp(CMemoryPool *mp,
							  CParseHandlerManager *parse_handler_mgr,
							  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerDistinctComp_H

// EOF
