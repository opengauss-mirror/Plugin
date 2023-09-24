//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerUtils.h
//
//	@doc:
//		Class providing helper methods for parse handler
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerUtils_H
#define SPQDXL_CParseHandlerUtils_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLOperator.h"
#include "naucrates/dxl/parser/CParseHandlerProperties.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerUtils
//
//	@doc:
//		Class providing helper methods for parse handler
//
//---------------------------------------------------------------------------
class CParseHandlerUtils
{
public:
	// parse and the set operator's costing and statistical properties
	static void SetProperties(CDXLNode *dxlnode,
							  CParseHandlerProperties *prop_parse_handler);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerUtils_H

// EOF
