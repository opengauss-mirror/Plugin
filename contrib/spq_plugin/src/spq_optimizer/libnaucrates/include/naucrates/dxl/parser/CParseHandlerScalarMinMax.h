//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CParseHandlerScalarMinMax.h
//
//	@doc:
//
//		SAX parse handler class for parsing MinMax operator
//---------------------------------------------------------------------------
#ifndef SPQDXL_CParseHandlerScalarMinMax_H
#define SPQDXL_CParseHandlerScalarMinMax_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarMinMax.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarMinMax
//
//	@doc:
//		Parse handler for parsing a MinMax operator
//
//---------------------------------------------------------------------------
class CParseHandlerScalarMinMax : public CParseHandlerScalarOp
{
private:
	// return type
	IMDId *m_mdid_type;

	// min/max type
	CDXLScalarMinMax::EdxlMinMaxType m_min_max_type;

	// private copy ctor
	CParseHandlerScalarMinMax(const CParseHandlerScalarMinMax &);

	// process the start of an element
	void StartElement(const XMLCh *const element_uri,
					  const XMLCh *const element_local_name,
					  const XMLCh *const element_qname, const Attributes &attr);

	// process the end of an element
	void EndElement(const XMLCh *const element_uri,
					const XMLCh *const element_local_name,
					const XMLCh *const element_qname);

	// parse the min/max type from the attribute value
	static CDXLScalarMinMax::EdxlMinMaxType GetMinMaxType(
		const XMLCh *element_local_name);

public:
	// ctor
	CParseHandlerScalarMinMax(CMemoryPool *mp,
							  CParseHandlerManager *parse_handler_mgr,
							  CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerScalarMinMax_H

//EOF
