//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CXformCollapseProject.h
//
//	@doc:
//		Transform that collapses two cascaded project nodes
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformCollapseProject_H
#define SPQOPT_CXformCollapseProject_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformSubqueryUnnest.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformCollapseProject
//
//	@doc:
//		Transform that collapses two cascaded project nodes
//
//---------------------------------------------------------------------------
class CXformCollapseProject : public CXformExploration
{
private:
	// private copy ctor
	CXformCollapseProject(const CXformCollapseProject &);

public:
	// ctor
	explicit CXformCollapseProject(CMemoryPool *mp);

	// dtor
	virtual ~CXformCollapseProject()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfCollapseProject;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformCollapseProject";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *, CXformResult *, CExpression *) const;

};	// class CXformCollapseProject

}  // namespace spqopt

#endif	// !SPQOPT_CXformCollapseProject_H

// EOF
