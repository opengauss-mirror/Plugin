//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSimplifySelectWithSubquery.h
//
//	@doc:
//		Simplify Select with subquery
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSimplifySelectWithSubquery_H
#define SPQOPT_CXformSimplifySelectWithSubquery_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/xforms/CXformSimplifySubquery.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSimplifySelectWithSubquery
//
//	@doc:
//		Simplify Select with subquery
//
//---------------------------------------------------------------------------
class CXformSimplifySelectWithSubquery : public CXformSimplifySubquery
{
private:
	// private copy ctor
	CXformSimplifySelectWithSubquery(const CXformSimplifySelectWithSubquery &);

public:
	// ctor
	explicit CXformSimplifySelectWithSubquery(CMemoryPool *mp)
		:  // pattern
		  CXformSimplifySubquery(SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
			  ))
	{
	}

	// dtor
	virtual ~CXformSimplifySelectWithSubquery()
	{
	}

	// Compatibility function for simplifying aggregates
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return (CXform::ExfSimplifySelectWithSubquery != exfid);
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSimplifySelectWithSubquery;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformSimplifySelectWithSubquery";
	}

	// is transformation a subquery unnesting (Subquery To Apply) xform?
	virtual BOOL
	FSubqueryUnnesting() const
	{
		return true;
	}

};	// class CXformSimplifySelectWithSubquery

}  // namespace spqopt

#endif	// !SPQOPT_CXformSimplifySelectWithSubquery_H

// EOF
