//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerScalarCoalesce.h
//
//	@doc:
//
//		SAX parse handler class for parsing coalesce operator
//---------------------------------------------------------------------------
#ifndef SPQDXL_CParseHandlerScalarCoalesce_H
#define SPQDXL_CParseHandlerScalarCoalesce_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarCoalesce.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarCoalesce
//
//	@doc:
//		Parse handler for parsing a coalesce operator
//
//---------------------------------------------------------------------------
class CParseHandlerScalarCoalesce : public CParseHandlerScalarOp
{
private:
	// return type
	IMDId *m_mdid_type;

	// private copy ctor
	CParseHandlerScalarCoalesce(const CParseHandlerScalarCoalesce &);

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
	CParseHandlerScalarCoalesce(CMemoryPool *mp,
								CParseHandlerManager *parse_handler_mgr,
								CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarCoalesce_H

//EOF
