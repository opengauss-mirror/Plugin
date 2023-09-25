//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software, Inc.
//
//	Template Class for Inner / Left Outer Index Apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementIndexApply_H
#define SPQOPT_CXformImplementIndexApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalIndexApply.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalInnerIndexNLJoin.h"
#include "spqopt/operators/CPhysicalLeftOuterIndexNLJoin.h"
#include "spqopt/operators/CPhysicalNLJoin.h"
#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

class CXformImplementIndexApply : public CXformImplementation
{
private:
	// private copy ctor
	CXformImplementIndexApply(const CXformImplementIndexApply &);

public:
	// ctor
	explicit CXformImplementIndexApply(CMemoryPool *mp)
		:  // pattern
		  CXformImplementation(SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalIndexApply(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // outer child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // inner child
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
			  ))
	{
	}

	// dtor
	virtual ~CXformImplementIndexApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementIndexApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementIndexApply";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &exprhdl) const
	{
		if (exprhdl.DeriveHasSubquery(2))
		{
			return ExfpNone;
		}
		return ExfpHigh;
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
		CLogicalIndexApply *indexApply =
			CLogicalIndexApply::PopConvert(pexpr->Pop());

		// extract components
		CExpression *pexprOuter = (*pexpr)[0];
		CExpression *pexprInner = (*pexpr)[1];
		CExpression *pexprScalar = (*pexpr)[2];
		CColRefArray *colref_array = indexApply->PdrgPcrOuterRefs();
		colref_array->AddRef();

		// addref all components
		pexprOuter->AddRef();
		pexprInner->AddRef();
		pexprScalar->AddRef();

		// assemble physical operator
		CPhysicalNLJoin *pop = NULL;

		if (CLogicalIndexApply::PopConvert(pexpr->Pop())->FouterJoin())
			pop = SPQOS_NEW(mp) CPhysicalLeftOuterIndexNLJoin(
				mp, colref_array, indexApply->OrigJoinPred());
		else
			pop = SPQOS_NEW(mp) CPhysicalInnerIndexNLJoin(
				mp, colref_array, indexApply->OrigJoinPred());

		CExpression *pexprResult = SPQOS_NEW(mp)
			CExpression(mp, pop, pexprOuter, pexprInner, pexprScalar);

		// add alternative to results
		pxfres->Add(pexprResult);
	}

};	// class CXformImplementIndexApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementIndexApply_H

// EOF
