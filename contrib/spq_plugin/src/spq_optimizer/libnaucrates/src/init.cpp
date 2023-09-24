//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		init.cpp
//
//	@doc:
//		Implementation of initialization and termination functions for
//		libspqdxl.
//---------------------------------------------------------------------------

#include "knl/knl_session.h"

#include "naucrates/init.h"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLString.hpp>

#include "spqos/memory/CAutoMemoryPool.h"

#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"
#include "naucrates/dxl/xml/dxltokens.h"
#include "naucrates/exception.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//      @function:
//              InitDXL
//
//      @doc:
//				Initialize DXL support; must be called before any library
//				function is called
//
//
//---------------------------------------------------------------------------
void
InitDXL()
{
	if (0 < u_sess->spq_cxt.m_ulpInitDXL)
	{
		// DXL support is already initialized by a previous call
		return;
	}

	SPQOS_ASSERT(NULL != u_sess->spq_cxt.pmpXerces);
	SPQOS_ASSERT(NULL != u_sess->spq_cxt.pmpDXL);

	// setup own memory manager
	u_sess->spq_cxt.dxl_memory_manager = SPQOS_NEW(u_sess->spq_cxt.pmpXerces) CDXLMemoryManager(u_sess->spq_cxt.pmpXerces);

	// initialize Xerces, if this fails library initialization should crash here
	XMLPlatformUtils::Initialize(XMLUni::fgXercescDefaultLocale,  // locale
								 NULL,	// nlsHome: location for message files
								 NULL,	// panicHandler
								 u_sess->spq_cxt.dxl_memory_manager	 // memoryManager
	);

	// initialize DXL tokens
	CDXLTokens::Init(u_sess->spq_cxt.pmpDXL);

	// initialize parse handler mappings
	CParseHandlerFactory::Init(u_sess->spq_cxt.pmpDXL);

	u_sess->spq_cxt.m_ulpInitDXL++;
}


//---------------------------------------------------------------------------
//      @function:
//              ShutdownDXL
//
//      @doc:
//				Shutdown DXL support; called only at library termination
//
//---------------------------------------------------------------------------
void
ShutdownDXL()
{
	if (0 < u_sess->spq_cxt.m_ulpShutdownDXL)
	{
		// DXL support is already shut-down by a previous call
		return;
	}

	SPQOS_ASSERT(NULL != u_sess->spq_cxt.pmpXerces);

	u_sess->spq_cxt.m_ulpShutdownDXL++;

	XMLPlatformUtils::Terminate();

	CDXLTokens::Terminate();

	SPQOS_DELETE(u_sess->spq_cxt.dxl_memory_manager);
	u_sess->spq_cxt.dxl_memory_manager = NULL;
}


//---------------------------------------------------------------------------
//      @function:
//              spqdxl_init
//
//      @doc:
//              Initialize Xerces parser utils
//
//---------------------------------------------------------------------------
void
spqdxl_init()
{
	// create memory pool for Xerces global allocations
	{
		CAutoMemoryPool amp;

		// detach safety
		u_sess->spq_cxt.pmpXerces = amp.Detach();
	}

	// create memory pool for DXL global allocations
	{
		CAutoMemoryPool amp;

		// detach safety
		u_sess->spq_cxt.pmpDXL = amp.Detach();
	}

	// add standard exception messages
	(void) EresExceptionInit(u_sess->spq_cxt.pmpDXL);
}


//---------------------------------------------------------------------------
//      @function:
//              spqdxl_terminate
//
//      @doc:
//              Terminate Xerces parser utils and destroy memory pool
//
//---------------------------------------------------------------------------
void
spqdxl_terminate()
{
#ifdef SPQOS_DEBUG
	ShutdownDXL();

	if (NULL != u_sess->spq_cxt.pmpDXL)
	{
		(CMemoryPoolManager::GetMemoryPoolMgr())->Destroy(u_sess->spq_cxt.pmpDXL);
		u_sess->spq_cxt.pmpDXL = NULL;
	}

	if (NULL != u_sess->spq_cxt.pmpXerces)
	{
		(CMemoryPoolManager::GetMemoryPoolMgr())->Destroy(u_sess->spq_cxt.pmpXerces);
		u_sess->spq_cxt.pmpXerces = NULL;
	}
#endif	// SPQOS_DEBUG
}

// EOF
