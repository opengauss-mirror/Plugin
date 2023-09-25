//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobGroup.h
//
//	@doc:
//		Superclass of group jobs
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJobGroup_H
#define SPQOPT_CJobGroup_H

#include "spqos/base.h"

#include "spqopt/search/CJob.h"

namespace spqopt
{
using namespace spqos;

// prototypes
class CGroup;
class CGroupExpression;

//---------------------------------------------------------------------------
//	@class:
//		CJobGroup
//
//	@doc:
//		Abstract superclass of all group optimization jobs
//
//---------------------------------------------------------------------------
class CJobGroup : public CJob
{
private:
	// private copy ctor
	CJobGroup(const CJobGroup &);

protected:
	// target group
	CGroup *m_pgroup;

	// last scheduled group expression
	CGroupExpression *m_pgexprLastScheduled;

	// ctor
	CJobGroup() : m_pgroup(NULL)
	{
	}

	// dtor
	virtual ~CJobGroup(){};

	// initialize job
	void Init(CGroup *pgroup);

	// get first unscheduled logical expression
	virtual CGroupExpression *PgexprFirstUnschedLogical();

	// get first unscheduled non-logical expression
	virtual CGroupExpression *PgexprFirstUnschedNonLogical();

	// get first unscheduled expression
	virtual CGroupExpression *PgexprFirstUnsched() = 0;

	// schedule jobs for of all new group expressions
	virtual BOOL FScheduleGroupExpressions(CSchedulerContext *psc) = 0;

	// job's function
	virtual BOOL FExecute(CSchedulerContext *psc) = 0;

#ifdef SPQOS_DEBUG

	// print function
	virtual IOstream &OsPrint(IOstream &os) const = 0;

#endif	// SPQOS_DEBUG

};	// class CJobGroup

}  // namespace spqopt

#endif	// !SPQOPT_CJobGroup_H


// EOF
