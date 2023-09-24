//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CJobTransformation.cpp
//
//	@doc:
//		Implementation of group expression transformation job
//---------------------------------------------------------------------------

#include "spqopt/search/CJobTransformation.h"

#include "spqopt/engine/CEngine.h"
#include "spqopt/operators/CLogical.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"


using namespace spqopt;

// State transition diagram for transformation job state machine;
//
// +-----------------+
// | estInitialized: |
// | EevtTransform() |
// +-----------------+
//   |
//   | eevCompleted
//   v
// +-----------------+
// |  estCompleted   |
// +-----------------+
//
const CJobTransformation::EEvent
	rgeev[CJobTransformation::estSentinel][CJobTransformation::estSentinel] = {
		{// estInitialized
		 CJobTransformation::eevSentinel, CJobTransformation::eevCompleted},
		{// estCompleted
		 CJobTransformation::eevSentinel, CJobTransformation::eevSentinel},
};

#ifdef SPQOS_DEBUG

// names for states
const WCHAR rgwszStates[CJobTransformation::estSentinel]
					   [SPQOPT_FSM_NAME_LENGTH] = {SPQOS_WSZ_LIT("initialized"),
												  SPQOS_WSZ_LIT("completed")};

// names for events
const WCHAR rgwszEvents[CJobTransformation::eevSentinel]
					   [SPQOPT_FSM_NAME_LENGTH] = {SPQOS_WSZ_LIT("transforming")};

#endif	//SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::CJobTransformation
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobTransformation::CJobTransformation()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::~CJobTransformation
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobTransformation::~CJobTransformation()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobTransformation::Init(CGroupExpression *pgexpr, CXform *pxform)
{
	SPQOS_ASSERT(!FInit());
	SPQOS_ASSERT(NULL != pgexpr);
	SPQOS_ASSERT(NULL != pxform);

	m_pgexpr = pgexpr;
	m_xform = pxform;

	m_jsm.Init(rgeev
#ifdef SPQOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// SPQOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtTransform);

	// mark as initialized
	CJob::SetInit();
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::EevtTransform
//
//	@doc:
//		Apply transformation action
//
//---------------------------------------------------------------------------
CJobTransformation::EEvent
CJobTransformation::EevtTransform(CSchedulerContext *psc, CJob *pjOwner)
{
	// get a job pointer
	CJobTransformation *pjt = PjConvert(pjOwner);
	CMemoryPool *pmpGlobal = psc->GetGlobalMemoryPool();
	CMemoryPool *pmpLocal = psc->PmpLocal();
	CGroupExpression *pgexpr = pjt->m_pgexpr;
	CXform *pxform = pjt->m_xform;

	// insert transformation results to memo
	CXformResult *pxfres = SPQOS_NEW(pmpGlobal) CXformResult(pmpGlobal);
	ULONG ulElapsedTime = 0;
	ULONG ulNumberOfBindings = 0;
	pgexpr->Transform(pmpGlobal, pmpLocal, pxform, pxfres, &ulElapsedTime,
					  &ulNumberOfBindings);
	psc->Peng()->InsertXformResult(pgexpr->Pgroup(), pxfres, pxform->Exfid(),
								   pgexpr, ulElapsedTime, ulNumberOfBindings);
	pxfres->Release();

	return eevCompleted;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::FExecute
//
//	@doc:
//		Main job function
//
//---------------------------------------------------------------------------
BOOL
CJobTransformation::FExecute(CSchedulerContext *psc)
{
	SPQOS_ASSERT(FInit());

	return m_jsm.FRun(psc, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::ScheduleJob
//
//	@doc:
//		Schedule a new transformation job
//
//---------------------------------------------------------------------------
void
CJobTransformation::ScheduleJob(CSchedulerContext *psc,
								CGroupExpression *pgexpr, CXform *pxform,
								CJob *pjParent)
{
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtTransformation);

	// initialize job
	CJobTransformation *pjt = PjConvert(pj);
	pjt->Init(pgexpr, pxform);
	psc->Psched()->Add(pjt, pjParent);
}

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobTransformation::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CJobTransformation::OsPrint(IOstream &os) const
{
	return m_jsm.OsHistory(os);
}

#endif	// SPQOS_DEBUG


// EOF
