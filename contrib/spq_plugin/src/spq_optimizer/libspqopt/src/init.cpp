//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		init.cpp
//
//	@doc:
//		Implementation of initialization and termination functions for
//		libspqopt.
//---------------------------------------------------------------------------

#include "spqopt/init.h"

#include "spqos/_api.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CWorker.h"

#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/xforms/CXformFactory.h"
#include "naucrates/init.h"
#include "knl/knl_session.h"

using namespace spqos;
using namespace spqopt;


//---------------------------------------------------------------------------
//      @function:
//              spqopt_init
//
//      @doc:
//              Initialize spqopt library. To enable memory allocations
//              via a custom allocator, pass in non-NULL fnAlloc/fnFree
//              allocation/deallocation functions. If either of the parameters
//              are NULL, spqopt with be initialized with the default allocator.
//
//---------------------------------------------------------------------------
void
spqopt_init()
{
	{
		CAutoMemoryPool amp;
        u_sess->spq_cxt.m_xform_mp = amp.Pmp();

		// add standard exception messages
		(void) spqopt::EresExceptionInit(u_sess->spq_cxt.m_xform_mp);

		// detach safety
		(void) amp.Detach();
	}

	if (SPQOS_OK != spqopt::CXformFactory::Init())
	{
		return;
	}
}

//---------------------------------------------------------------------------
//      @function:
//              spqopt_terminate
//
//      @doc:
//              Destroy the memory pool
//
//---------------------------------------------------------------------------
void
spqopt_terminate()
{
	CMDCache::Shutdown();

	CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(u_sess->spq_cxt.m_xform_mp);

	CXformFactory::Pxff()->Shutdown();
}

// EOF
