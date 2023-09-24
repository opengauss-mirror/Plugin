//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerScalarLimitOffset.h
//
//	@doc:
//		SAX parse handler class for parsing LimitOffset
//
//---------------------------------------------------------------------------


#ifndef SPQDXL_CParseHandlerScalarLimitOffset_H
#define SPQDXL_CParseHandlerScalarLimitOffset_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarLimitOffset
//
//	@doc:
//		Parse handler for parsing a LIMIT Offset statement
//
//---------------------------------------------------------------------------
class CParseHandlerScalarLimitOffset : public CParseHandlerScalarOp
{
private:
	// private copy ctor
	CParseHandlerScalarLimitOffset(const CParseHandlerScalarLimitOffset &);

	// process the start of an element
	void StartElement(const XMLCh *const element_uri,
					  const XMLCh *const element_local_name,
					  const XMLCh *const element_qname, const Attributes &attr);

	// process the end of an element
	void EndElement(const XMLCh *const element_uri,
					const XMLCh *const element_local_name,
					const XMLCh *const element_qname);

public:
	// ctor
	CParseHandlerScalarLimitOffset(CMemoryPool *mp,
								   CParseHandlerManager *parse_handler_mgr,
								   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl
#endif	// !SPQDXL_CParseHandlerScalarLimitOffset_H

//EOF
