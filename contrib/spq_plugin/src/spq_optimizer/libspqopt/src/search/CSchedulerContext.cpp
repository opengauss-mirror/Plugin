//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008-2011 Greenplum, Inc.
//
//	@filename:
//		CSchedulerContext.cpp
//
//	@doc:
//		Implementation of optimizer job scheduler
//---------------------------------------------------------------------------

#include "spqopt/search/CSchedulerContext.h"

#include "spqos/base.h"
#include "spqos/memory/CMemoryPoolManager.h"

#include "spqopt/engine/CEngine.h"
#include "spqopt/search/CScheduler.h"


using namespace spqos;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CSchedulerContext::CSchedulerContext
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CSchedulerContext::CSchedulerContext()
	: m_pmpGlobal(NULL), m_pmpLocal(NULL), m_psched(NULL), m_fInit(false)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CSchedulerContext::~CSchedulerContext
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CSchedulerContext::~CSchedulerContext()
{
	SPQOS_ASSERT_IMP(FInit(), NULL != GetGlobalMemoryPool());
	SPQOS_ASSERT_IMP(FInit(), NULL != PmpLocal());
	SPQOS_ASSERT_IMP(FInit(), NULL != Psched());

	// release local memory pool
	if (FInit())
	{
		CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(PmpLocal());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CSchedulerContext::Init
//
//	@doc:
//		Initialize scheduling context
//
//---------------------------------------------------------------------------
void
CSchedulerContext::Init(CMemoryPool *pmpGlobal, CJobFactory *pjf,
						CScheduler *psched, CEngine *peng)
{
	SPQOS_ASSERT(NULL != pmpGlobal);
	SPQOS_ASSERT(NULL != pjf);
	SPQOS_ASSERT(NULL != psched);
	SPQOS_ASSERT(NULL != peng);

	SPQOS_ASSERT(!FInit() && "Scheduling context is already initialized");

	m_pmpLocal = CMemoryPoolManager::GetMemoryPoolMgr()->CreateMemoryPool();

	m_pmpGlobal = pmpGlobal;
	m_pjf = pjf;
	m_psched = psched;
	m_peng = peng;
	m_fInit = true;
}


// EOF
