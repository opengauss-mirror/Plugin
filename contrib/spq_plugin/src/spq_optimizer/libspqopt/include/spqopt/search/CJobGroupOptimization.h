//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobGroupOptimization.h
//
//	@doc:
//		Optimize group job
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJobGroupOptimization_H
#define SPQOPT_CJobGroupOptimization_H

#include "spqos/base.h"

#include "spqopt/search/CJobGroup.h"
#include "spqopt/search/CJobStateMachine.h"


namespace spqopt
{
using namespace spqos;

// prototypes
class COptimizationContext;


//---------------------------------------------------------------------------
//	@class:
//		CJobGroupOptimization
//
//	@doc:
//		Group optimization job
//
//		Responsible for finding the best plan rooted by an expression in a
//		given group, such that the identified plan satisfies given required
//		plan properties
//
//---------------------------------------------------------------------------
class CJobGroupOptimization : public CJobGroup
{
public:
	// transition events of group optimization
	enum EEvent
	{
		eevImplementing,		   // implementation is in progress
		eevImplemented,			   // implementation is complete
		eevOptimizing,			   // optimization is in progress
		eevOptimizedCurrentLevel,  // optimization of current level is complete
		eevOptimized,			   // optimization is complete

		eevSentinel
	};

	// states of group optimization job
	enum EState
	{
		estInitialized = 0,			  // initial state
		estOptimizingChildren,		  // optimizing group expressions
		estDampingOptimizationLevel,  // damping optimization level
		estCompleted,				  // done optimization

		estSentinel
	};

private:
	// shorthand for job state machine
	typedef CJobStateMachine<EState, estSentinel, EEvent, eevSentinel> JSM;

	// job state machine
	JSM m_jsm;

	// group expression that triggered group optimization
	CGroupExpression *m_pgexprOrigin;

	// optimization context of the job
	COptimizationContext *m_poc;

	// current optimization level of group expressions
	EOptimizationLevel m_eolCurrent;

	// start optimization action
	static EEvent EevtStartOptimization(CSchedulerContext *psc, CJob *pj);

	// optimized child group expressions action
	static EEvent EevtOptimizeChildren(CSchedulerContext *psc, CJob *pj);

	// complete optimization action
	static EEvent EevtCompleteOptimization(CSchedulerContext *psc, CJob *pj);

	// private copy ctor
	CJobGroupOptimization(const CJobGroupOptimization &);

public:
	// ctor
	CJobGroupOptimization();

	// dtor
	virtual ~CJobGroupOptimization();

	// initialize job
	void Init(CGroup *pgroup, CGroupExpression *pgexprOrigin,
			  COptimizationContext *poc);

	// current optimization level accessor
	EOptimizationLevel
	EolCurrent() const
	{
		return m_eolCurrent;
	}

	// damp optimization level of member group expressions
	void
	DampOptimizationLevel()
	{
		m_eolCurrent = CEngine::EolDamp(m_eolCurrent);
	}

	// get first unscheduled expression
	virtual CGroupExpression *
	PgexprFirstUnsched()
	{
		return CJobGroup::PgexprFirstUnschedNonLogical();
	}

	// schedule optimization jobs for of all new group expressions
	virtual BOOL FScheduleGroupExpressions(CSchedulerContext *psc);

	// schedule a new group optimization job
	static void ScheduleJob(CSchedulerContext *psc, CGroup *pgroup,
							CGroupExpression *pgexprOrigin,
							COptimizationContext *poc, CJob *pjParent);

	// job's function
	virtual BOOL FExecute(CSchedulerContext *psc);

#ifdef SPQOS_DEBUG

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

	// dump state machine diagram in graphviz format
	virtual IOstream &
	OsDiagramToGraphviz(CMemoryPool *mp, IOstream &os,
						const WCHAR *wszTitle) const
	{
		(void) m_jsm.OsDiagramToGraphviz(mp, os, wszTitle);

		return os;
	}

	// compute unreachable states
	void
	Unreachable(CMemoryPool *mp, EState **ppestate, ULONG *pulSize) const
	{
		m_jsm.Unreachable(mp, ppestate, pulSize);
	}

#endif	// SPQOS_DEBUG

	// conversion function
	static CJobGroupOptimization *
	PjConvert(CJob *pj)
	{
		SPQOS_ASSERT(NULL != pj);
		SPQOS_ASSERT(EjtGroupOptimization == pj->Ejt());

		return dynamic_cast<CJobGroupOptimization *>(pj);
	}

};	// class CJobGroupOptimization

}  // namespace spqopt

#endif	// !SPQOPT_CJobGroupOptimization_H


// EOF
