//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementUnionAll.cpp
//
//	@doc:
//		Implementation of union all operator
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementUnionAll.h"

#include "spqos/base.h"

#include "spqopt/exception.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPhysicalUnionAll.h"
#include "spqopt/operators/CPhysicalUnionAllFactory.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementUnionAll::CXformImplementUnionAll
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementUnionAll::CXformImplementUnionAll(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalUnionAll(mp),
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementUnionAll::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementUnionAll::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CLogicalUnionAll *popUnionAll = CLogicalUnionAll::PopConvert(pexpr->Pop());
	CPhysicalUnionAllFactory factory(popUnionAll);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG arity = pexpr->Arity();

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		pexprChild->AddRef();
		pdrspqexpr->Append(pexprChild);
	}

	CPhysicalUnionAll *popPhysicalSerialUnionAll =
		factory.PopPhysicalUnionAll(mp, false);

	// assemble serial union physical operator
	CExpression *pexprSerialUnionAll =
		SPQOS_NEW(mp) CExpression(mp, popPhysicalSerialUnionAll, pdrspqexpr);

	// add serial union alternative to results
	pxfres->Add(pexprSerialUnionAll);

	// parallel union alternative to the result if the GUC is on
	BOOL fParallel = SPQOS_FTRACE(EopttraceEnableParallelAppend);

	if (fParallel)
	{
		CPhysicalUnionAll *popPhysicalParallelUnionAll =
			factory.PopPhysicalUnionAll(mp, true);

		pdrspqexpr->AddRef();

		// assemble physical parallel operator
		CExpression *pexprParallelUnionAll = SPQOS_NEW(mp)
			CExpression(mp, popPhysicalParallelUnionAll, pdrspqexpr);

		// add parallel union alternative to results
		pxfres->Add(pexprParallelUnionAll);
	}
}

// EOF
