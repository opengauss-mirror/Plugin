//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDCache.cpp
//
//	@doc:
//		 Function implementation of CMDCache
//---------------------------------------------------------------------------

#include "knl/knl_session.h"

#include "spqopt/mdcache/CMDCache.h"

#include "spqos/task/CAutoTraceFlag.h"

using namespace spqos;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CMDCache::Init
//
//	@doc:
//		Initializes global instance
//
//---------------------------------------------------------------------------
void
CMDCache::Init()
{
	SPQOS_ASSERT(NULL == u_sess->spq_cxt.m_pcache && "Metadata cache was already created");

	u_sess->spq_cxt.m_pcache = CCacheFactory::CreateCache<IMDCacheObject *, CMDKey *>(
		true /*fUnique*/, u_sess->spq_cxt.m_ullCacheQuota, CMDKey::UlHashMDKey,
		CMDKey::FEqualMDKey);
}

BOOL
CMDCache::FInitialized()
{
	return (nullptr != u_sess->spq_cxt.m_pcache);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCache::Shutdown
//
//	@doc:
//		Cleans up the underlying cache
//
//---------------------------------------------------------------------------
void
CMDCache::Shutdown()
{
	SPQOS_DELETE(u_sess->spq_cxt.m_pcache);
	u_sess->spq_cxt.m_pcache = NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDCache::SetCacheQuota
//
//	@doc:
//		Set the maximum size of the cache
//
//---------------------------------------------------------------------------
void
CMDCache::SetCacheQuota(ULLONG ullCacheQuota)
{
	SPQOS_ASSERT(NULL != u_sess->spq_cxt.m_pcache && "Metadata cache was not created");
	u_sess->spq_cxt.m_ullCacheQuota = ullCacheQuota;
	u_sess->spq_cxt.m_pcache->SetCacheQuota(ullCacheQuota);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCache::ULLGetCacheQuota
//
//	@doc:
//		Get the maximum size of the cache
//
//---------------------------------------------------------------------------
ULLONG
CMDCache::ULLGetCacheQuota()
{
	// make sure that the CMDCache's saved quota is reflected in the underlying CCache
	SPQOS_ASSERT_IMP(NULL != u_sess->spq_cxt.m_pcache,
					u_sess->spq_cxt.m_pcache->GetCacheQuota() == u_sess->spq_cxt.m_ullCacheQuota);
	return u_sess->spq_cxt.m_ullCacheQuota;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCache::ULLGetCacheEvictionCounter
//
//	@doc:
// 		Get the number of times we evicted entries from this cache
//
//---------------------------------------------------------------------------
ULLONG
CMDCache::ULLGetCacheEvictionCounter()
{
	// make sure that we already initialized our underlying CCache
	SPQOS_ASSERT(NULL != u_sess->spq_cxt.m_pcache);

	return u_sess->spq_cxt.m_pcache->GetEvictionCounter();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCache::Reset
//
//	@doc:
//		Reset metadata cache
//
//---------------------------------------------------------------------------
void
CMDCache::Reset()
{
	CAutoTraceFlag atf1(EtraceSimulateOOM, false);
	CAutoTraceFlag atf2(EtraceSimulateAbort, false);
	CAutoTraceFlag atf3(EtraceSimulateIOError, false);
	CAutoTraceFlag atf4(EtraceSimulateNetError, false);

	Shutdown();
	Init();
}
CMDAccessor::MDCache *
CMDCache::Pcache()
{
	return u_sess->spq_cxt.m_pcache;
}
// EOF
