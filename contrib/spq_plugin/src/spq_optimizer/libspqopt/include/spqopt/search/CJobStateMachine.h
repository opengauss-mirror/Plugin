//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobStateMachine.h
//
//	@doc:
//		State machine to execute actions and maintain state of
//		optimization jobs;
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJobStateMachine_H
#define SPQOPT_CJobStateMachine_H


#include "spqos/base.h"
#include "spqos/common/CEnumSet.h"
#include "spqos/types.h"

#include "spqopt/base/CStateMachine.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/search/CSchedulerContext.h"

namespace spqopt
{
using namespace spqos;

// prototypes
class CJob;
class CSchedulerContext;

//---------------------------------------------------------------------------
//	@class:
//		CJobStateMachine
//
//	@doc:
//		Optimization job state machine
//
//		The class CJobStateMachine has a templated definition of the SPQOS class
//		CStateMachine. The template allows parameterizing the state machine
//		with states and events enumerations. The final (terminating) state in
//		the state machine is the last entry in the states enumeration.
//
//		Job actions:
//		The class CJobStateMachine also allows setting an action for each
//		state. An action is a function to be executed when the state machine is
//		in a given state. Setting actions happens inside the function
//		CJobStateMachine::SetAction(). This function is called from the Init()
//		function inside each job. The outcome of an action is an event, that
//		may cause the state machine to remain in the same state (self-loop on
//		current state), or move the state machine to a different state.
//
//		Basic job state machine execution:
//		The function that runs the state machine is CJobStateMachine::FRun().
//		This is a simple loop that iteratively calls the action that
//		corresponds to current state. Executing an action returns an event,
//		which is used as input to move the state machine to a new (or same)
//		state. The loop ends when the state machine reaches the final
//		(terminating) state.
//
//		State transitions:
//		Each optimization job inlines the definition of its state machine at
//		the top of its cpp file (see CJobGroupOptimization.cpp for an example).
//		The state machine definition is primarily given by a state transition
//		diagram represented as a square matrix of |states| x |states| entries.
//		Each matrix entry contains an event that causes the transition of state
//		machine from the state corresponding to the current row to the state
//		corresponding to the current column. For example the entry at [row 1,
//		column 2] contains an event e that causes the state machine to
//		transition from state 1 to state 2. When executing  the action defined
//		by state 1, if  the return result is the event e, the state machine
//		advances to state 2. The state transition diagram must be carefully
//		defined to avoid infinite loops that have no possible exists (e.g.,
//		state 1 → state 2 → state 1).
//
//---------------------------------------------------------------------------
template <class TEnumState, TEnumState estSentinel, class TEnumEvent,
		  TEnumEvent eevSentinel>
class CJobStateMachine
{
private:
	// pointer to job action function
	typedef TEnumEvent (*PFuncAction)(CSchedulerContext *psc, CJob *pjOwner);

	// shorthand for state machine
	typedef CStateMachine<TEnumState, estSentinel, TEnumEvent, eevSentinel> SM;

	// array of actions corresponding to states
	PFuncAction m_rgPfuncAction[estSentinel];

	// job state machine
	SM m_sm;

	// hidden copy ctor
	CJobStateMachine(const CJobStateMachine &);

public:
	// ctor
	CJobStateMachine<TEnumState, estSentinel, TEnumEvent, eevSentinel>(){};

	// dtor
	~CJobStateMachine(){};

	// initialize state machine
	void
	Init(const TEnumEvent rgfTransitions[estSentinel][estSentinel]
#ifdef SPQOS_DEBUG
		 ,
		 const WCHAR wszStates[estSentinel][SPQOPT_FSM_NAME_LENGTH],
		 const WCHAR wszEvent[estSentinel][SPQOPT_FSM_NAME_LENGTH]
#endif	// SPQOS_DEBUG
	)
	{
		Reset();

		m_sm.Init(rgfTransitions
#ifdef SPQOS_DEBUG
				  ,
				  wszStates, wszEvent
#endif	// SPQOS_DEBUG
		);
	}

	// match action with state
	void
	SetAction(TEnumState est, PFuncAction pfAction)
	{
		SPQOS_ASSERT(NULL != pfAction);
		SPQOS_ASSERT(NULL == m_rgPfuncAction[est] &&
					"Action has been already set");

		m_rgPfuncAction[est] = pfAction;
	}

	// run the state machine
	BOOL
	FRun(CSchedulerContext *psc, CJob *pjOwner)
	{
		SPQOS_ASSERT(NULL != psc);
		SPQOS_ASSERT(NULL != pjOwner);

		TEnumState estCurrent = estSentinel;
		TEnumState estNext = estSentinel;
		do
		{
			// check if current search stage is timed-out
			if (psc->Peng()->PssCurrent()->FTimedOut())
			{
				// cleanup job state and terminate state machine
				pjOwner->Cleanup();
				return true;
			}

			// find current state
			estCurrent = m_sm.Estate();

			// get the function associated with current state
			PFuncAction pfunc = m_rgPfuncAction[estCurrent];
			SPQOS_ASSERT(NULL != pfunc);

			// execute the function to get an event
			TEnumEvent eev = pfunc(psc, pjOwner);

			// use the event to transition state machine
			estNext = estCurrent;
#ifdef SPQOS_DEBUG
			BOOL fSucceeded =
#endif	// SPQOS_DEBUG
				m_sm.FTransition(eev, estNext);

			SPQOS_ASSERT(fSucceeded);
		} while (estNext != estCurrent && estNext != m_sm.TesFinal());

		return (estNext == m_sm.TesFinal());
	}

	// reset state machine
	void
	Reset()
	{
		m_sm.Reset();

		// initialize actions array
		for (ULONG i = 0; i < estSentinel; i++)
		{
			m_rgPfuncAction[i] = NULL;
		}
	}

#ifdef SPQOS_DEBUG
	// dump history
	IOstream &
	OsHistory(IOstream &os) const
	{
		m_sm.OsHistory(os);
		return os;
	}

	// dump state machine diagram in graphviz format
	IOstream &
	OsDiagramToGraphviz(CMemoryPool *mp, IOstream &os,
						const WCHAR *wszTitle) const
	{
		(void) m_sm.OsDiagramToGraphviz(mp, os, wszTitle);

		return os;
	}

	// compute unreachable states
	void
	Unreachable(CMemoryPool *mp, TEnumState **ppestate, ULONG *pulSize) const
	{
		m_sm.Unreachable(mp, ppestate, pulSize);
	}

#endif	// SPQOS_DEBUG

};	// class CJobStateMachine
}  // namespace spqopt

#endif	// !SPQOPT_CJobStateMachine_H


// EOF
