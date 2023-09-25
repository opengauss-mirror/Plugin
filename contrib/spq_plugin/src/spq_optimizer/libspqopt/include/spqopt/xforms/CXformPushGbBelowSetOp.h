//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformPushGbBelowSetOp.h
//
//	@doc:
//		Push grouping below set operation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformPushGbBelowSetOp_H
#define SPQOPT_CXformPushGbBelowSetOp_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformPushGbBelowSetOp
//
//	@doc:
//		Push grouping below set operation
//
//---------------------------------------------------------------------------
template <class TSetOp>
class CXformPushGbBelowSetOp : public CXformExploration
{
private:
	// private copy ctor
	CXformPushGbBelowSetOp(const CXformPushGbBelowSetOp &);

public:
	// ctor
	explicit CXformPushGbBelowSetOp(CMemoryPool *mp)
		: CXformExploration(
			  // pattern
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
				  SPQOS_NEW(mp) CExpression	// left child is a set operation
				  (mp, SPQOS_NEW(mp) TSetOp(mp),
				   SPQOS_NEW(mp)
					   CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))),
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp)
							  CPatternTree(mp))	 // project list of group-by
				  ))
	{
	}

	// dtor
	virtual ~CXformPushGbBelowSetOp()
	{
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &exprhdl) const
	{
		CLogicalGbAgg *popGbAgg = CLogicalGbAgg::PopConvert(exprhdl.Pop());
		if (popGbAgg->FGlobal())
		{
			return ExfpHigh;
		}

		return ExfpNone;
	}

	// actual transform
	virtual void
	Transform(CXformContext *pxfctxt, CXformResult *pxfres,
			  CExpression *pexpr) const
	{
		SPQOS_ASSERT(NULL != pxfctxt);
		SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
		SPQOS_ASSERT(FCheckPattern(pexpr));

		CMemoryPool *mp = pxfctxt->Pmp();
		COptimizerConfig *optconfig =
			COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();

		CExpression *pexprSetOp = (*pexpr)[0];
		if (pexprSetOp->Arity() >
			optconfig->GetHint()->UlPushGroupByBelowSetopThreshold())
		{
			// bail-out if set op has many children
			return;
		}
		CExpression *pexprPrjList = (*pexpr)[1];
		if (0 < pexprPrjList->Arity())
		{
			// bail-out if group-by has any aggregate functions
			return;
		}

		CLogicalGbAgg *popGbAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
		CLogicalSetOp *popSetOp = CLogicalSetOp::PopConvert(pexprSetOp->Pop());

		CColRefArray *pdrspqcrGb = popGbAgg->Pdrspqcr();
		CColRefArray *pdrspqcrOutput = popSetOp->PdrspqcrOutput();
		CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrOutput);
		CColRef2dArray *pdrspqdrspqcrInput = popSetOp->PdrspqdrspqcrInput();
		CExpressionArray *pdrspqexprNewChildren =
			SPQOS_NEW(mp) CExpressionArray(mp);
		CColRef2dArray *pdrspqdrspqcrNewInput = SPQOS_NEW(mp) CColRef2dArray(mp);
		const ULONG arity = pexprSetOp->Arity();

		BOOL fNewChild = false;

		for (ULONG ulChild = 0; ulChild < arity; ulChild++)
		{
			CExpression *pexprChild = (*pexprSetOp)[ulChild];
			CColRefArray *pdrspqcrChild = (*pdrspqdrspqcrInput)[ulChild];
			CColRefSet *pcrsChild = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrChild);

			CColRefArray *pdrspqcrChildGb = NULL;
			if (!pcrsChild->Equals(pcrsOutput))
			{
				// use column mapping in SetOp to set child grouping colums
				UlongToColRefMap *colref_mapping =
					CUtils::PhmulcrMapping(mp, pdrspqcrOutput, pdrspqcrChild);
				pdrspqcrChildGb = CUtils::PdrspqcrRemap(
					mp, pdrspqcrGb, colref_mapping, true /*must_exist*/);
				colref_mapping->Release();
			}
			else
			{
				// use grouping columns directly as child grouping colums
				pdrspqcrGb->AddRef();
				pdrspqcrChildGb = pdrspqcrGb;
			}

			pexprChild->AddRef();
			pcrsChild->Release();

			// if child of setop is already an Agg with the same grouping columns
			// that we want to use, there is no need to add another agg on top of it
			COperator *popChild = pexprChild->Pop();
			if (COperator::EopLogicalGbAgg == popChild->Eopid())
			{
				CLogicalGbAgg *popGbAgg = CLogicalGbAgg::PopConvert(popChild);
				if (CColRef::Equals(popGbAgg->Pdrspqcr(), pdrspqcrChildGb))
				{
					pdrspqexprNewChildren->Append(pexprChild);
					pdrspqdrspqcrNewInput->Append(pdrspqcrChildGb);

					continue;
				}
			}

			fNewChild = true;
			pexprPrjList->AddRef();
			CExpression *pexprChildGb = CUtils::PexprLogicalGbAggGlobal(
				mp, pdrspqcrChildGb, pexprChild, pexprPrjList);
			pdrspqexprNewChildren->Append(pexprChildGb);

			pdrspqcrChildGb->AddRef();
			pdrspqdrspqcrNewInput->Append(pdrspqcrChildGb);
		}

		pcrsOutput->Release();

		if (!fNewChild)
		{
			// all children of the union were already Aggs with the same grouping
			// columns that we would have created. No new alternative expressions
			pdrspqdrspqcrNewInput->Release();
			pdrspqexprNewChildren->Release();

			return;
		}

		pdrspqcrGb->AddRef();
		TSetOp *popSetOpNew =
			SPQOS_NEW(mp) TSetOp(mp, pdrspqcrGb, pdrspqdrspqcrNewInput);
		CExpression *pexprNewSetOp =
			SPQOS_NEW(mp) CExpression(mp, popSetOpNew, pdrspqexprNewChildren);

		popGbAgg->AddRef();
		pexprPrjList->AddRef();
		CExpression *pexprResult =
			SPQOS_NEW(mp) CExpression(mp, popGbAgg, pexprNewSetOp, pexprPrjList);

		pxfres->Add(pexprResult);
	}

};	// class CXformPushGbBelowSetOp

}  // namespace spqopt

#endif	// !SPQOPT_CXformPushGbBelowSetOp_H

// EOF
