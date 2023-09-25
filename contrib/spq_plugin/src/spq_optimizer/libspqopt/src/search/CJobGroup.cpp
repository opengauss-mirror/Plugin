//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CJobGroup.cpp
//
//	@doc:
//		Implementation of group job superclass
//---------------------------------------------------------------------------

#include "spqopt/search/CJobGroup.h"

#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupExpressionExploration.h"
#include "spqopt/search/CJobGroupExpressionImplementation.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CJobGroup::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroup::Init(CGroup *pgroup)
{
	SPQOS_ASSERT(!FInit());
	SPQOS_ASSERT(NULL != pgroup);

	m_pgroup = pgroup;
	m_pgexprLastScheduled = NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroup::PgexprFirstUnschedNonLogical
//
//	@doc:
//		Get first non-logical group expression with an unscheduled job
//
//---------------------------------------------------------------------------
CGroupExpression *
CJobGroup::PgexprFirstUnschedNonLogical()
{
	CGroupExpression *pgexpr = NULL;
	{
		CGroupProxy spq(m_pgroup);
		if (NULL == m_pgexprLastScheduled)
		{
			// get first group expression
			pgexpr = spq.PgexprSkipLogical(NULL /*pgexpr*/);
		}
		else
		{
			// get group expression next to last scheduled one
			pgexpr = spq.PgexprSkipLogical(m_pgexprLastScheduled);
		}
	}

	return pgexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroup::PgexprFirstUnschedLogical
//
//	@doc:
//		Get first logical group expression with an unscheduled job
//
//---------------------------------------------------------------------------
CGroupExpression *
CJobGroup::PgexprFirstUnschedLogical()
{
	CGroupExpression *pgexpr = NULL;
	{
		CGroupProxy spq(m_pgroup);
		if (NULL == m_pgexprLastScheduled)
		{
			// get first group expression
			pgexpr = spq.PgexprFirst();
		}
		else
		{
			// get group expression next to last scheduled one
			pgexpr = spq.PgexprNext(m_pgexprLastScheduled);
		}
	}

	return pgexpr;
}

// EOF
