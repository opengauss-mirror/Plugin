//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDCache.h
//
//	@doc:
//		Metadata cache.
//---------------------------------------------------------------------------


#ifndef SPQOPT_CMDCache_H
#define SPQOPT_CMDCache_H

#include "spqos/base.h"
#include "spqos/memory/CCache.h"
#include "spqos/memory/CCacheFactory.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDKey.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;


//---------------------------------------------------------------------------
//	@class:
//		CMDCache
//
//	@doc:
//		A wrapper for a generic cache to hide the details of metadata cache
//		creation and encapsulate a singleton cache object
//
//---------------------------------------------------------------------------
class CMDCache
{
private:
	// pointer to the underlying cache
	static CMDAccessor::MDCache *m_pcache;

	// the maximum size of the cache
	static ULLONG m_ullCacheQuota;

	// private ctor
	CMDCache(){};

	// no copy ctor
	CMDCache(const CMDCache &);

	// private dtor
	~CMDCache(){};

public:
	// initialize underlying cache
	static void Init();

	// has cache been initialized?
	static BOOL FInitialized();

	// destroy global instance
	static void Shutdown();

	// set the maximum size of the cache
	static void SetCacheQuota(ULLONG ullCacheQuota);

	// get the maximum size of the cache
	static ULLONG ULLGetCacheQuota();

	// get the number of times we evicted entries from this cache
	static ULLONG ULLGetCacheEvictionCounter();

	// reset global instance
	static void Reset();

	// global accessor
	static CMDAccessor::MDCache *Pcache();

};	// class CMDCache

}  // namespace spqopt

#endif	// !SPQOPT_CMDCache_H

// EOF
