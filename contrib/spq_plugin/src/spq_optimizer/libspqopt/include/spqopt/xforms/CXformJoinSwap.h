//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformJoinSwap.h
//
//	@doc:
//		Join swap transformation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformJoinSwap_H
#define SPQOPT_CXformJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/xforms/CXformExploration.h"
#include "spqopt/xforms/CXformUtils.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformJoinSwap
//
//	@doc:
//		Join swap transformation
//
//---------------------------------------------------------------------------
template <class TJoinTop, class TJoinBottom>
class CXformJoinSwap : public CXformExploration
{
private:
	// private copy ctor
	CXformJoinSwap(const CXformJoinSwap &);

public:
	// ctor
	explicit CXformJoinSwap(CMemoryPool *mp)
		: CXformExploration(
			  // pattern
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) TJoinTop(mp),
				  SPQOS_NEW(mp) CExpression	// left child is a join tree
				  (mp, SPQOS_NEW(mp) TJoinBottom(mp),
				   SPQOS_NEW(mp) CExpression(
					   mp, SPQOS_NEW(mp) CPatternLeaf(mp)),	// left child
				   SPQOS_NEW(mp) CExpression(
					   mp, SPQOS_NEW(mp) CPatternLeaf(mp)),	// right child
				   SPQOS_NEW(mp) CExpression(
					   mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
				   ),
				  SPQOS_NEW(mp) CExpression	// right child is a pattern leaf
				  (mp, SPQOS_NEW(mp) CPatternLeaf(mp)),
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // top-join predicate
				  ))
	{
	}

	// dtor
	virtual ~CXformJoinSwap()
	{
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &  // exprhdl
	) const
	{
		return CXform::ExfpHigh;
	}

	// actual transform
	void
	Transform(CXformContext *pxfctxt, CXformResult *pxfres,
			  CExpression *pexpr) const
	{
		SPQOS_ASSERT(NULL != pxfctxt);
		SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
		SPQOS_ASSERT(FCheckPattern(pexpr));

		CMemoryPool *mp = pxfctxt->Pmp();

		CExpression *pexprResult =
			CXformUtils::PexprSwapJoins(mp, pexpr, (*pexpr)[0]);
		if (NULL == pexprResult)
		{
			return;
		}

		pxfres->Add(pexprResult);
	}

};	// class CXformJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformJoinSwap_H

// EOF
