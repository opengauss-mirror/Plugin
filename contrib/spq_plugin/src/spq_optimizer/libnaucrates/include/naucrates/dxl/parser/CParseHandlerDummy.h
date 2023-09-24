//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerDummy.h
//
//	@doc:
//		Dummy SAX parse handler used for validation of XML documents.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerDummy_H
#define SPQDXL_CParseHandlerDummy_H

#include <xercesc/sax2/DefaultHandler.hpp>

#include "spqos/base.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

class CDXLMemoryManager;

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerDummy
//
//	@doc:
//		Dummy parse handler for validation of XML documents.
//
//---------------------------------------------------------------------------
class CParseHandlerDummy : public DefaultHandler
{
private:
	// memory manager to use for Xerces allocation
	CDXLMemoryManager *m_dxl_memory_manager;

public:
	// ctor
	explicit CParseHandlerDummy(CDXLMemoryManager *dxl_memory_manager);

	// process a parsing error
	void error(const SAXParseException &sax_parse_ex);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerDummy_H

// EOF
