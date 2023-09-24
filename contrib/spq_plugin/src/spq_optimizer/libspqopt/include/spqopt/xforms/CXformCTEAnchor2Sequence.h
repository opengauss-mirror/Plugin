//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformCTEAnchor2Sequence.h
//
//	@doc:
//		Transform logical CTE anchor to logical sequence over CTE producer
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformCTEAnchor2Sequence_H
#define SPQOPT_CXformCTEAnchor2Sequence_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformCTEAnchor2Sequence
//
//	@doc:
//		Transform logical CTE anchor to logical sequence over CTE producer
//
//---------------------------------------------------------------------------
class CXformCTEAnchor2Sequence : public CXformExploration
{
private:
	// private copy ctor
	CXformCTEAnchor2Sequence(const CXformCTEAnchor2Sequence &);

public:
	// ctor
	explicit CXformCTEAnchor2Sequence(CMemoryPool *mp);

	// dtor
	virtual ~CXformCTEAnchor2Sequence()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfCTEAnchor2Sequence;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformCTEAnchor2Sequence";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformCTEAnchor2Sequence
}  // namespace spqopt

#endif	// !SPQOPT_CXformCTEAnchor2Sequence_H

// EOF
