//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformJoinAssociativity.h
//
//	@doc:
//		Transform left-deep join tree by associativity
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformJoinAssociativity_H
#define SPQOPT_CXformJoinAssociativity_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformJoinAssociativity
//
//	@doc:
//		Associative transformation of left-deep join tree
//
//---------------------------------------------------------------------------
class CXformJoinAssociativity : public CXformExploration
{
private:
	// private copy ctor
	CXformJoinAssociativity(const CXformJoinAssociativity &);

	// helper function for creating the new join predicate
	void CreatePredicates(CMemoryPool *mp, CExpression *pexpr,
						  CExpressionArray *pdrspqexprLower,
						  CExpressionArray *pdrspqexprUpper) const;

public:
	// ctor
	explicit CXformJoinAssociativity(CMemoryPool *mp);

	// dtor
	virtual ~CXformJoinAssociativity()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfJoinAssociativity;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformJoinAssociativity";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformJoinAssociativity

}  // namespace spqopt


#endif	// !SPQOPT_CXformJoinAssociativity_H

// EOF
