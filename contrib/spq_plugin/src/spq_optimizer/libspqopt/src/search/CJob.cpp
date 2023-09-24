//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJob.cpp
//
//	@doc:
//		Implementation of optimizer job base class
//---------------------------------------------------------------------------

#include "spqopt/search/CJob.h"

#include "spqos/base.h"

#include "spqopt/search/CJobQueue.h"
#include "spqopt/search/CScheduler.h"

using namespace spqopt;
using namespace spqos;

FORCE_GENERATE_DBGSTR(CJob);

//---------------------------------------------------------------------------
//	@function:
//		CJob::Reset
//
//	@doc:
//		Reset job
//
//---------------------------------------------------------------------------
void
CJob::Reset()
{
	m_pjParent = NULL;
	m_pjq = NULL;
	m_ulpRefs = 0;
	m_fInit = false;
#ifdef SPQOS_DEBUG
	m_ejs = EjsInit;
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		CJob::FResumeParent
//
//	@doc:
//		Resume parent jobs after job completion
//
//---------------------------------------------------------------------------
BOOL
CJob::FResumeParent() const
{
	SPQOS_ASSERT(0 == UlpRefs());
	SPQOS_ASSERT(NULL != m_pjParent);
	SPQOS_ASSERT(0 < m_pjParent->UlpRefs());

	// decrement parent's ref counter
	ULONG_PTR ulpRefs = m_pjParent->UlpDecrRefs();

	// check if job should be resumed
	return (1 == ulpRefs);
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJob::OsPrint
//
//	@doc:
//		Print job description
//
//---------------------------------------------------------------------------
IOstream &
CJob::OsPrint(IOstream &os) const
{
	os << "ID=" << Id();

	if (NULL != PjParent())
	{
		os << " parent=" << PjParent()->Id() << std::endl;
	}
	else
	{
		os << " ROOT" << std::endl;
	}
	return os;
}

#endif	// SPQOS_DEBUG

// EOF
