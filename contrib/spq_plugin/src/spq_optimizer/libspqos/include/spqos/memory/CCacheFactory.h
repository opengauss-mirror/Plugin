//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC CORP.
//
//	@filename:
//		CCache.h
//
//	@doc:
//		Definition of cache factory class.
//.
//	@owner:
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef CCACHEFACTORY_H_
#define CCACHEFACTORY_H_

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/memory/CCache.h"

// default initial value of the gclock counter during insertion of an entry
#define CCACHE_GCLOCK_INIT_COUNTER 3

using namespace spqos;

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CCacheFactory
//
//	@doc:
//		Definition of cache factory;
//
//		This class is responsible of creating cache objects and allocating the
//		memory pools they need
//
//---------------------------------------------------------------------------
class CCacheFactory
{
private:

	// memory pool allocated to caches
	CMemoryPool *m_mp;

	// private ctor
	CCacheFactory(CMemoryPool *mp);

	// no copy ctor
	CCacheFactory(const CCacheFactory &);



public:
	// private dtor
	~CCacheFactory();

	// initialize global memory pool
	static SPQOS_RESULT Init();

	// destroy global instance
	void Shutdown();

	// global accessor
    static CCacheFactory *GetFactory();

	// create a cache instance
	template <class T, class K>
	static CCache<T, K> *
	CreateCache(BOOL unique, ULLONG cache_quota,
				typename CCache<T, K>::HashFuncPtr hash_func,
				typename CCache<T, K>::EqualFuncPtr equal_func)
	{
		SPQOS_ASSERT(NULL != GetFactory() &&
					"Cache factory has not been initialized");

		CMemoryPool *mp = GetFactory()->Pmp();
		CCache<T, K> *cache = SPQOS_NEW(mp)
			CCache<T, K>(mp, unique, cache_quota, CCACHE_GCLOCK_INIT_COUNTER,
						 hash_func, equal_func);

		return cache;
	}

	CMemoryPool *Pmp() const;

};	// CCacheFactory
}  // namespace spqos


#endif	// CCACHEFACTORY_H_

// EOF
