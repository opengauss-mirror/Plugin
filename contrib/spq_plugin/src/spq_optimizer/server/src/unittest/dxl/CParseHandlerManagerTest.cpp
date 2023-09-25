//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerManagerTest.cpp
//
//	@doc:
//		Tests parsing DXL documents into DXL trees.
//---------------------------------------------------------------------------

#include "unittest/dxl/CParseHandlerManagerTest.h"

#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "spqos/base.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLPhysicalTableScan.h"
#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/parser/CParseHandlerHashJoin.h"
#include "naucrates/dxl/parser/CParseHandlerManager.h"
#include "naucrates/dxl/parser/CParseHandlerPlan.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerManagerTest::EresUnittest
//
//	@doc:
//		Unittest for activating and deactivating DXL parse handlers
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CParseHandlerManagerTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CParseHandlerManagerTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CParseHandlerManagerTest::EresUnittest_Basic
//
//	@doc:
//		Testing activation and deactivation of parse handlers
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CParseHandlerManagerTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	CMemoryPool *mp = amp.Pmp();

	// create XML reader and a parse handler manager for it
	CDXLMemoryManager *dxl_memory_manager = SPQOS_NEW(mp) CDXLMemoryManager(mp);

	SAX2XMLReader *parser = NULL;
	{
		CAutoTraceFlag atf(EtraceSimulateOOM, false);
		parser = XMLReaderFactory::createXMLReader(dxl_memory_manager);
	}

	CParseHandlerManager *parse_handler_mgr =
		SPQOS_NEW(mp) CParseHandlerManager(dxl_memory_manager, parser);

	// create some parse handlers
	CParseHandlerPlan *pphPlan =
		SPQOS_NEW(mp) CParseHandlerPlan(mp, parse_handler_mgr, NULL);
	CParseHandlerHashJoin *pphHJ =
		SPQOS_NEW(mp) CParseHandlerHashJoin(mp, parse_handler_mgr, pphPlan);

	parse_handler_mgr->ActivateParseHandler(pphPlan);
	SPQOS_ASSERT(pphPlan == parse_handler_mgr->GetCurrentParseHandler());
	SPQOS_ASSERT(pphPlan == parser->getContentHandler());

	parse_handler_mgr->ActivateParseHandler(pphHJ);
	SPQOS_ASSERT(pphHJ == parse_handler_mgr->GetCurrentParseHandler());
	SPQOS_ASSERT(pphHJ == parser->getContentHandler());


	parse_handler_mgr->DeactivateHandler();
	SPQOS_ASSERT(pphPlan == parse_handler_mgr->GetCurrentParseHandler());
	SPQOS_ASSERT(pphPlan == parser->getContentHandler());

	parse_handler_mgr->DeactivateHandler();
	// no more parse handlers
	SPQOS_ASSERT(NULL == parse_handler_mgr->GetCurrentParseHandler());
	SPQOS_ASSERT(NULL == parser->getContentHandler());

	// cleanup
	SPQOS_DELETE(parse_handler_mgr);
	delete parser;
	SPQOS_DELETE(dxl_memory_manager);
	SPQOS_DELETE(pphPlan);
	SPQOS_DELETE(pphHJ);

	return SPQOS_OK;
}



// EOF
