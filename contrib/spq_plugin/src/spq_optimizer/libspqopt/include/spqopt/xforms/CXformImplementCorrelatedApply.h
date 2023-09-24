//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementCorrelatedApply.h
//
//	@doc:
//		Base class for implementing correlated NLJ
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementCorrelatedApply_H
#define SPQOPT_CXformImplementCorrelatedApply_H

#include "spqos/base.h"

#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/xforms/CXformImplementation.h"
#include "spqopt/xforms/CXformUtils.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementCorrelatedApply
//
//	@doc:
//		Implement correlated Apply
//
//---------------------------------------------------------------------------
template <class TLogicalApply, class TPhysicalJoin>
class CXformImplementCorrelatedApply : public CXformImplementation
{
private:
	// private copy ctor
	CXformImplementCorrelatedApply(const CXformImplementCorrelatedApply &);


public:
	// ctor
	explicit CXformImplementCorrelatedApply(CMemoryPool *mp)
		:  // pattern
		  CXformImplementation(SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) TLogicalApply(mp),
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
			  ))
	{
	}

	// dtor
	virtual ~CXformImplementCorrelatedApply()
	{
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &exprhdl) const
	{
		if (exprhdl.DeriveHasSubquery(2))
		{
			return CXform::ExfpNone;
		}
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

		// extract components
		CExpression *pexprLeft = (*pexpr)[0];
		CExpression *pexprRight = (*pexpr)[1];
		CExpression *pexprScalar = (*pexpr)[2];
		TLogicalApply *popApply = TLogicalApply::PopConvert(pexpr->Pop());
		CColRefArray *colref_array = popApply->PdrgPcrInner();

		colref_array->AddRef();

		// addref all children
		pexprLeft->AddRef();
		pexprRight->AddRef();
		pexprScalar->AddRef();

		// assemble physical operator
		CExpression *pexprPhysicalApply = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp)
				TPhysicalJoin(mp, colref_array, popApply->EopidOriginSubq()),
			pexprLeft, pexprRight, pexprScalar);

		// add alternative to results
		pxfres->Add(pexprPhysicalApply);
	}

};	// class CXformImplementCorrelatedApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementCorrelatedApply_H

// EOF
