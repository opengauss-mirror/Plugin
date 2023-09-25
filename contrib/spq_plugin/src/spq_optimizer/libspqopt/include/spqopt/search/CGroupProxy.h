//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CGroupProxy.h
//
//	@doc:
//		Lock mechanism for access to a given group
//---------------------------------------------------------------------------
#ifndef SPQOPT_CGroupProxy_H
#define SPQOPT_CGroupProxy_H

#include "spqos/base.h"

#include "spqopt/search/CGroup.h"

namespace spqopt
{
using namespace spqos;

// forward declarations
class CGroupExpression;
class CDrvdProp;
class COptimizationContext;

//---------------------------------------------------------------------------
//	@class:
//		CGroupProxy
//
//	@doc:
//		Exclusive access to a given group
//
//---------------------------------------------------------------------------
class CGroupProxy
{
private:
	// group we're operating on
	CGroup *m_pgroup;

	// skip group expressions starting from the given expression;
	CGroupExpression *PgexprSkip(CGroupExpression *pgexprStart,
								 BOOL fSkipLogical);

public:
	// ctor
	explicit CGroupProxy(CGroup *pgroup);

	// dtor
	~CGroupProxy();

	// set group id
	void
	SetId(ULONG id)
	{
		m_pgroup->SetId(id);
	}

	// set group state
	void
	SetState(CGroup::EState estNewState)
	{
		m_pgroup->SetState(estNewState);
	}

	// set hash join keys
	void
	SetJoinKeys(CExpressionArray *pdrspqexprOuter,
				CExpressionArray *pdrspqexprInner, IMdIdArray *join_opfamilies)
	{
		m_pgroup->SetJoinKeys(pdrspqexprOuter, pdrspqexprInner, join_opfamilies);
	}

	// insert group expression
	void Insert(CGroupExpression *pgexpr);

	// move duplicate group expression to duplicates list
	void MoveDuplicateGExpr(CGroupExpression *pgexpr);

	// initialize group's properties;
	void InitProperties(CDrvdProp *pdp);

	// initialize group's stat;
	void InitStats(IStatistics *stats);

	// retrieve first group expression
	CGroupExpression *PgexprFirst();

	// retrieve next group expression
	CGroupExpression *PgexprNext(CGroupExpression *pgexpr);

	// get the first non-logical group expression following the given expression
	CGroupExpression *PgexprSkipLogical(CGroupExpression *pgexpr);

	// get the next logical group expression following the given expression
	CGroupExpression *PgexprNextLogical(CGroupExpression *pgexpr);

	// lookup best expression under optimization context
	CGroupExpression *PgexprLookup(COptimizationContext *poc) const;


#ifdef SPQOS_DEBUG
	// is group transition to given state complete?
	BOOL
	FTransitioned(CGroup::EState estate) const
	{
		return (CGroup::estExplored == estate && m_pgroup->FExplored()) ||
			   (CGroup::estImplemented == estate && m_pgroup->FImplemented()) ||
			   (CGroup::estOptimized == estate && m_pgroup->FOptimized());
	}
#endif	// SPQOS_DEBUG

};	// class CGroupProxy

}  // namespace spqopt

#endif	// !SPQOPT_CGroupProxy_H


// EOF
