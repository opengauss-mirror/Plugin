//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2011 EMC Corp.
//
//	@filename:
//		CGroupProxy.cpp
//
//	@doc:
//		Implementation of proxy object for group access
//---------------------------------------------------------------------------

#include "spqopt/search/CGroupProxy.h"

#include "spqos/base.h"
#include "spqos/common/CSyncHashtableAccessByKey.h"

#include "spqopt/base/CDrvdPropRelational.h"
#include "spqopt/base/COptimizationContext.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CJobGroup.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::CGroupProxy
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CGroupProxy::CGroupProxy(CGroup *pgroup) : m_pgroup(pgroup)
{
	SPQOS_ASSERT(NULL != pgroup);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::~CGroupProxy
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CGroupProxy::~CGroupProxy()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::Insert
//
//	@doc:
//		Insert group expression into group
//
//---------------------------------------------------------------------------
void
CGroupProxy::Insert(CGroupExpression *pgexpr)
{
	pgexpr->Init(m_pgroup, m_pgroup->m_ulGExprs++);

	SPQOS_ASSERT(pgexpr->Pgroup() == m_pgroup);

	m_pgroup->Insert(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::MoveDuplicateGExpr
//
//	@doc:
//		Move duplicate group expression to duplicates list
//
//---------------------------------------------------------------------------
void
CGroupProxy::MoveDuplicateGExpr(CGroupExpression *pgexpr)
{
	SPQOS_ASSERT(pgexpr->Pgroup() == m_pgroup);

#ifdef SPQOS_DEBUG
	ULONG ulGExprsOld =
		m_pgroup->m_listGExprs.Size() + m_pgroup->m_listDupGExprs.Size();
#endif	// SPQOS_DEBUG

	m_pgroup->MoveDuplicateGExpr(pgexpr);
	SPQOS_ASSERT(ulGExprsOld == (m_pgroup->m_listGExprs.Size() +
								m_pgroup->m_listDupGExprs.Size()));
}

//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::InitProperties
//
//	@doc:
//		Initialize group's properties
//
//---------------------------------------------------------------------------
void
CGroupProxy::InitProperties(CDrvdProp *pdp)
{
	m_pgroup->InitProperties(pdp);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::InitStats
//
//	@doc:
//		Initialize group's stats
//
//---------------------------------------------------------------------------
void
CGroupProxy::InitStats(IStatistics *stats)
{
	m_pgroup->InitStats(stats);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::PgexprNext
//
//	@doc:
//		Retrieve next group expression;
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroupProxy::PgexprNext(CGroupExpression *pgexpr)
{
	SPQOS_ASSERT(NULL != pgexpr);
	return m_pgroup->PgexprNext(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::PgexprFirst
//
//	@doc:
//		Retrieve first group expression;
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroupProxy::PgexprFirst()
{
	return m_pgroup->PgexprFirst();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::PgexprSkip
//
//	@doc:
//		Skip group expressions starting from the given expression;
//		the type of group expressions to skip is determined by the passed
//		flag
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroupProxy::PgexprSkip(CGroupExpression *pgexprStart, BOOL fSkipLogical)
{
	CGroupExpression *pgexpr = pgexprStart;
	while (NULL != pgexpr && fSkipLogical == pgexpr->Pop()->FLogical())
	{
		pgexpr = PgexprNext(pgexpr);
	}

	return pgexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::PgexprSkipLogical
//
//	@doc:
//		Retrieve the first non-logical group expression following the given
//		expression;
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroupProxy::PgexprSkipLogical(CGroupExpression *pgexpr)
{
	if (NULL == pgexpr)
	{
		return PgexprSkip(PgexprFirst(), true /*fSkipLogical*/);
	}

	return PgexprSkip(PgexprNext(pgexpr), true /*fSkipLogical*/);
}



//---------------------------------------------------------------------------
//	@function:
//		CGroupProxy::PgexprNextLogical
//
//	@doc:
//		Find the first logical group expression after the given expression
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroupProxy::PgexprNextLogical(CGroupExpression *pgexpr)
{
	SPQOS_ASSERT(!m_pgroup->FScalar());

	if (NULL == pgexpr)
	{
		return PgexprSkip(PgexprFirst(), false /*fSkipLogical*/);
	}

	return PgexprSkip(PgexprNext(pgexpr), false /*fSkipLogical*/);
}


// EOF
