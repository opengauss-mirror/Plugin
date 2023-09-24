//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerScalarBooleanTest.h
//
//	@doc:
//
//		SAX parse handler class for parsing scalar BooleanTest.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerScalarBooleanTest_H
#define SPQDXL_CParseHandlerScalarBooleanTest_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalarBooleanTest.h"
#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"


namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarBooleanTest
//
//	@doc:
//		Parse handler for parsing a scalar boolean test
//
//---------------------------------------------------------------------------
class CParseHandlerScalarBooleanTest : public CParseHandlerScalarOp
{
private:
	EdxlBooleanTestType m_dxl_boolean_test_type;

	// private copy ctor
	CParseHandlerScalarBooleanTest(const CParseHandlerScalarBooleanTest &);

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

	// parse the boolean test type from the Xerces xml string
	static EdxlBooleanTestType GetDxlBooleanTestType(
		const XMLCh *xmlszBoolType);

public:
	// ctor
	CParseHandlerScalarBooleanTest(CMemoryPool *mp,
								   CParseHandlerManager *parse_handler_mgr,
								   CParseHandlerBase *parse_handler_root);
};

}  // namespace spqdxl
#endif	// SPQDXL_CParseHandlerScalarBooleanTest_H

//EOF
