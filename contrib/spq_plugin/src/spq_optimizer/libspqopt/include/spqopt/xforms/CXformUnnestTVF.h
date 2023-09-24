//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformUnnestTVF.h
//
//	@doc:
//		 Unnest TVF with subquery arguments
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformUnnestTVF_H
#define SPQOPT_CXformUnnestTVF_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformUnnestTVF
//
//	@doc:
//		Unnest TVF with subquery arguments
//
//---------------------------------------------------------------------------
class CXformUnnestTVF : public CXformExploration
{
private:
	// private copy ctor
	CXformUnnestTVF(const CXformUnnestTVF &);

	// helper for mapping subquery function arguments into columns
	static CColRefArray *PdrspqcrSubqueries(CMemoryPool *mp,
										   CExpression *pexprCTEProducer,
										   CExpression *pexprCTEConsumer);

	//	collect subquery arguments and return a Project expression
	static CExpression *PexprProjectSubqueries(CMemoryPool *mp,
											   CExpression *pexprTVF);

public:
	// ctor
	explicit CXformUnnestTVF(CMemoryPool *mp);

	// dtor
	virtual ~CXformUnnestTVF()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfUnnestTVF;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformUnnestTVF";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformUnnestTVF

}  // namespace spqopt

#endif	// !SPQOPT_CXformUnnestTVF_H

// EOF
