//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformPushDownLeftOuterJoin.cpp
//
//	@doc:
//		Implementation of left outer join push down transformation
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformPushDownLeftOuterJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalLeftOuterJoin.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/operators/CPredicateUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformPushDownLeftOuterJoin::CXformPushDownLeftOuterJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformPushDownLeftOuterJoin::CXformPushDownLeftOuterJoin(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp),
			  SPQOS_NEW(mp) CExpression	// outer child is an NAry-Join
			  (mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp),
			   SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp)),
			   SPQOS_NEW(mp) CExpression(
				   mp,
				   SPQOS_NEW(mp) CPatternTree(mp))  // NAry-join predicate tree
			   ),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // inner child is a leaf
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // LOJ predicate tree
			  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformPushDownLeftOuterJoin::Exfp
//
//	@doc:
//		Xform promise
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformPushDownLeftOuterJoin::Exfp(CExpressionHandle &exprhdl) const
{
	CExpression *pexprScalar = exprhdl.PexprScalarRepChild(2);
	if (COperator::EopScalarConst == pexprScalar->Pop()->Eopid())
	{
		return CXform::ExfpNone;
	}
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformPushDownLeftOuterJoin::Transform
//
//	@doc:
//		Transform LOJ whose outer child is an NAry-join to be a child
//		of NAry-join
//
//		Input:
//			LOJ (a=d)
//				|---NAry-Join (a=b) and (b=c)
//				|     |--A
//				|     |--B
//				|     +--C
//				+--D
//
//		Output:
//			  NAry-Join (a=b) and (b=c)
//				|--B
//				|--C
//				+--LOJ (a=d)
//					|--A
//					+--D
//
//---------------------------------------------------------------------------
void
CXformPushDownLeftOuterJoin::Transform(CXformContext *pxfctxt,
									   CXformResult *pxfres,
									   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpression *pexprNAryJoin = (*pexpr)[0];
	CExpression *pexprLOJInnerChild = (*pexpr)[1];
	CExpression *pexprLOJScalarChild = (*pexpr)[2];

	CColRefSet *pcrsLOJUsed = pexprLOJScalarChild->DeriveUsedColumns();
	CExpressionArray *pdrspqexprLOJChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprNAryJoinChildren =
		SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG arity = pexprNAryJoin->Arity();
	CExpression *pexprNAryJoinScalarChild = (*pexprNAryJoin)[arity - 1];
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CExpression *pexprChild = (*pexprNAryJoin)[ul];
		CColRefSet *pcrsOutput = pexprChild->DeriveOutputColumns();
		pexprChild->AddRef();
		if (!pcrsOutput->IsDisjoint(pcrsLOJUsed))
		{
			pdrspqexprLOJChildren->Append(pexprChild);
		}
		else
		{
			pdrspqexprNAryJoinChildren->Append(pexprChild);
		}
	}

	if (pdrspqexprLOJChildren->Size() == 0)
	{
		// cannot create a valid LOJ; bail
		pdrspqexprLOJChildren->Release();
		pdrspqexprNAryJoinChildren->Release();
		return;
	}

	CExpression *pexprLOJOuterChild = (*pdrspqexprLOJChildren)[0];
	if (1 < pdrspqexprLOJChildren->Size())
	{
		// collect all relations needed by LOJ outer side into a cross product,
		// normalization at the end of this function takes care of pushing NAry
		// join predicates down
		pdrspqexprLOJChildren->Append(
			CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/));
		pexprLOJOuterChild = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp), pdrspqexprLOJChildren);

		// reconstruct LOJ children and add only the created child
		pdrspqexprLOJChildren = SPQOS_NEW(mp) CExpressionArray(mp);
		pdrspqexprLOJChildren->Append(pexprLOJOuterChild);
	}

	// continue with rest of LOJ inner and scalar children
	pexprLOJInnerChild->AddRef();
	pdrspqexprLOJChildren->Append(pexprLOJInnerChild);
	pexprLOJScalarChild->AddRef();
	pdrspqexprLOJChildren->Append(pexprLOJScalarChild);

	// build new LOJ
	CExpression *pexprLOJNew = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp), pdrspqexprLOJChildren);

	// add new NAry join children
	pdrspqexprNAryJoinChildren->Append(pexprLOJNew);
	pexprNAryJoinScalarChild->AddRef();
	pdrspqexprNAryJoinChildren->Append(pexprNAryJoinScalarChild);

	if (3 > pdrspqexprNAryJoinChildren->Size())
	{
		// xform must generate a valid NAry-join expression
		// for example, in the following case we end-up with the same input
		// expression, which should be avoided:
		//
		//	Input:
		//
		//    LOJ (a=c) and (b=c)
		//     |--NAry-Join (a=b)
		//     |   |--A
		//     |   +--B
		//     +--C
		//
		//	Output:
		//
		//	  NAry-Join (true)
		//      +--LOJ (a=c) and (b=c)
		//           |--NAry-Join (a=b)
		//           |    |--A
		//           |    +--B
		//           +--C

		pdrspqexprNAryJoinChildren->Release();
		return;
	}

	// create new NAry join
	CExpression *pexprNAryJoinNew = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp), pdrspqexprNAryJoinChildren);

	// normalize resulting expression and add it to xform results
	CExpression *pexprResult =
		CNormalizer::PexprNormalize(mp, pexprNAryJoinNew);
	pexprNAryJoinNew->Release();

	pxfres->Add(pexprResult);
}

// EOF
