//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CCacheFactory.cpp
//
//	@doc:
//		 Function implementation of CCacheFactory
//---------------------------------------------------------------------------

#include "knl/knl_session.h"

#include "spqos/memory/CCacheFactory.h"

#include "spqos/io/ioutils.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/memory/CCache.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CCacheFactory::CCacheFactory
//
//	@doc:
//		Ctor;
//
//---------------------------------------------------------------------------
CCacheFactory::CCacheFactory(CMemoryPool *mp) : m_mp(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CCacheFactory::~CCacheFactory
//
//	@doc:
//		Ctor;
//
//---------------------------------------------------------------------------
CCacheFactory::~CCacheFactory()
{
    SPQOS_ASSERT(NULL == u_sess->spq_cxt.m_factory &&
                 "Cache factory has not been shut down");
}


//---------------------------------------------------------------------------
//	@function:
//		CCacheFactory::Pmp
//
//	@doc:
//		Returns a pointer to allocated memory pool
//
//---------------------------------------------------------------------------
CMemoryPool *
CCacheFactory::Pmp() const
{
	return m_mp;
}


//---------------------------------------------------------------------------
//	@function:
//		CCacheFactory::Init
//
//	@doc:
//		Initializes global instance
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCacheFactory::Init()
{
	SPQOS_ASSERT(NULL == GetFactory() &&
				"Cache factory was already initialized");

	SPQOS_RESULT res = SPQOS_OK;

	// create cache factory memory pool
	CMemoryPool *mp =
		CMemoryPoolManager::GetMemoryPoolMgr()->CreateMemoryPool();
	SPQOS_TRY
	{
		// create cache factory instance
		u_sess->spq_cxt.m_factory = SPQOS_NEW(mp) CCacheFactory(mp);
	}
	SPQOS_CATCH_EX(ex)
	{
		// destroy memory pool if global instance was not created
		CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(mp);

		u_sess->spq_cxt.m_factory = NULL;

		if (SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM))
		{
			res = SPQOS_OOM;
		}
		else
		{
			res = SPQOS_FAILED;
		}
	}
	SPQOS_CATCH_END;
	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		CCacheFactory::Shutdown
//
//	@doc:
//		Cleans up allocated memory pool
//
//---------------------------------------------------------------------------
void
CCacheFactory::Shutdown()
{
	CCacheFactory *factory = CCacheFactory::GetFactory();

	SPQOS_ASSERT(NULL != factory && "Cache factory has not been initialized");

	CMemoryPool *mp = factory->m_mp;

	// destroy cache factory
	u_sess->spq_cxt.m_factory = NULL;
	SPQOS_DELETE(factory);

	// release allocated memory pool
	CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CCacheFactory::GetFactory
//
//	@doc:
//		get factory from context
//
//---------------------------------------------------------------------------
CCacheFactory *
CCacheFactory::GetFactory()
{
	return u_sess->spq_cxt.m_factory;
}

// EOF
