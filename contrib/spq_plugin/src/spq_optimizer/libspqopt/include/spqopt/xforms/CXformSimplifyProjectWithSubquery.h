//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSimplifyProjectWithSubquery.h
//
//	@doc:
//		Simplify Project with subquery
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSimplifyProjectWithSubquery_H
#define SPQOPT_CXformSimplifyProjectWithSubquery_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CLogicalProject.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/xforms/CXformSimplifySubquery.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSimplifyProjectWithSubquery
//
//	@doc:
//		Simplify Project with subquery
//
//---------------------------------------------------------------------------
class CXformSimplifyProjectWithSubquery : public CXformSimplifySubquery
{
private:
	// private copy ctor
	CXformSimplifyProjectWithSubquery(
		const CXformSimplifyProjectWithSubquery &);

public:
	// ctor
	explicit CXformSimplifyProjectWithSubquery(CMemoryPool *mp)
		:  // pattern
		  CXformSimplifySubquery(SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalProject(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // project list
			  ))
	{
	}

	// dtor
	virtual ~CXformSimplifyProjectWithSubquery()
	{
	}

	// Compatibility function for simplifying aggregates
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return (CXform::ExfSimplifyProjectWithSubquery != exfid);
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSimplifyProjectWithSubquery;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformSimplifyProjectWithSubquery";
	}

	// is transformation a subquery unnesting (Subquery To Apply) xform?
	virtual BOOL
	FSubqueryUnnesting() const
	{
		return true;
	}

};	// class CXformSimplifyProjectWithSubquery

}  // namespace spqopt

#endif	// !SPQOPT_CXformSimplifyProjectWithSubquery_H

// EOF
