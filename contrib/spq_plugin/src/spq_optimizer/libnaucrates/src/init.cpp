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
#include "knl/knl_instance.h"
#include "naucrates/init.h"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLString.hpp>

#include "spqos/memory/CAutoMemoryPool.h"

#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"
#include "naucrates/dxl/xml/dxltokens.h"
#include "naucrates/exception.h"
#include "naucrates/dxl/xml/SPQCDXLMemoryManager.h"

using namespace spqos;
using namespace spqdxl;
spqos::CMemoryPool* global_memory_pool = nullptr;
pthread_mutex_t dxl_init_lock = PTHREAD_MUTEX_INITIALIZER;
bool dxl_global_init_status = false;
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

	// initialize DXL tokens
	CDXLTokens::Init(u_sess->spq_cxt.pmpDXL);

	// initialize parse handler mappings
	CParseHandlerFactory::Init(u_sess->spq_cxt.pmpDXL);

	u_sess->spq_cxt.m_ulpInitDXL++;
}


//---------------------------------------------------------------------------
//      @function:
//              InitDXLManager
//
//      @doc:
//				Initialize DXL Memory Manager; must be called before any library
//				function is called
//
//
//---------------------------------------------------------------------------
void
InitDXLManager()
{
    if (dxl_global_init_status) {
        return ;
    }

    pthread_mutex_lock(&dxl_init_lock);
    if (!dxl_global_init_status) {
        if (SPQOS_OK != spqos::CMemoryPoolManager::DXLInit()) {
            ereport(ERROR, (errmsg("SPQ InitDXLManager error, dxl memory manager init failed.")));
            pthread_mutex_unlock(&dxl_init_lock);
            return;
        }
        global_memory_pool = CMemoryPoolManager::GetDXLMemoryPoolMgr()->CreateMemoryPool();

        // setup own memory manager
        spqdxl::CDXLMemoryManager *dxl_memory_manager =
            SPQOS_NEW(global_memory_pool)
                SPQCDXLMemoryManager(global_memory_pool);
        // initialize Xerces, if this fails library initialization should crash here
        XMLPlatformUtils::Initialize(XMLUni::fgXercescDefaultLocale,  // locale
                                     NULL,    // nlsHome: location for message files
                                     NULL,    // panicHandler
                                     dxl_memory_manager     // memoryManager
        );
        (void) on_exit(ShutdownDXLManager, NULL);
        dxl_global_init_status = true;
    }
    pthread_mutex_unlock(&dxl_init_lock);
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

	CDXLTokens::Terminate();
}

//---------------------------------------------------------------------------
//      @function:
//              ShutdownDXLManager
//
//      @doc:
//
//
//
//---------------------------------------------------------------------------
void
ShutdownDXLManager(int code, void* args)
{
    XMLPlatformUtils::Terminate();
    CMemoryPoolManager::GetDXLMemoryPoolMgr()->ShutdownDXLMgr();
    ::delete global_memory_pool;
    global_memory_pool = nullptr;
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
}

// EOF
