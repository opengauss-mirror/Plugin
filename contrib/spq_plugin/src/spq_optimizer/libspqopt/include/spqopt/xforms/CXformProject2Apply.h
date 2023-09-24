//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformProject2Apply.h
//
//	@doc:
//		Transform Project to Apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformProject2Apply_H
#define SPQOPT_CXformProject2Apply_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformSubqueryUnnest.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformProject2Apply
//
//	@doc:
//		Transform Project to Apply; this transformation is only applicable
//		to a Project expression with subqueries in its scalar project list
//
//---------------------------------------------------------------------------
class CXformProject2Apply : public CXformSubqueryUnnest
{
private:
	// private copy ctor
	CXformProject2Apply(const CXformProject2Apply &);

public:
	// ctor
	explicit CXformProject2Apply(CMemoryPool *mp);

	// dtor
	virtual ~CXformProject2Apply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfProject2Apply;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformProject2Apply";
	}

};	// class CXformProject2Apply

}  // namespace spqopt

#endif	// !SPQOPT_CXformProject2Apply_H

// EOF
