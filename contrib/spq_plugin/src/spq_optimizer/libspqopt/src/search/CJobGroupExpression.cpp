//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CJobGroupExpression.cpp
//
//	@doc:
//		Implementation of group expression job superclass
//---------------------------------------------------------------------------

#include "spqopt/search/CJobGroupExpression.h"

#include "spqopt/operators/CLogical.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupExpressionExploration.h"
#include "spqopt/search/CJobGroupExpressionImplementation.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"
#include "spqopt/xforms/CXformFactory.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpression::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroupExpression::Init(CGroupExpression *pgexpr)
{
	SPQOS_ASSERT(!FInit());
	SPQOS_ASSERT(NULL != pgexpr);
	SPQOS_ASSERT(NULL != pgexpr->Pgroup());
	SPQOS_ASSERT(NULL != pgexpr->Pop());

	m_fChildrenScheduled = false;
	m_fXformsScheduled = false;
	m_pgexpr = pgexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpression::ScheduleTransformations
//
//	@doc:
//		Schedule transformation jobs for the given set of xforms
//
//---------------------------------------------------------------------------
void
CJobGroupExpression::ScheduleTransformations(CSchedulerContext *psc,
											 CXformSet *xform_set)
{
	// iterate on xforms
	CXformSetIter xsi(*(xform_set));
	while (xsi.Advance())
	{
		CXform *pxform = CXformFactory::Pxff()->Pxf(xsi.TBit());
		CJobTransformation::ScheduleJob(psc, m_pgexpr, pxform, this);
	}
}


// EOF
