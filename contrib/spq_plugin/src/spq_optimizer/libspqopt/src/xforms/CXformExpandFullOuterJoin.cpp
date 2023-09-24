//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformExpandFullOuterJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExpandFullOuterJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalCTEAnchor.h"
#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalFullOuterJoin.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandFullOuterJoin::CXformExpandFullOuterJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExpandFullOuterJoin::CXformExpandFullOuterJoin(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalFullOuterJoin(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp)),  // outer child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp)),  // inner child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar child
			  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformExpandFullOuterJoin::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformExpandFullOuterJoin::Exfp(CExpressionHandle &	 //exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformExpandFullOuterJoin::Transform
//
//	@doc:
//		Actual transformation
// 		The expression A FOJ B is translated to:
//
//		CTEAnchor(cteA)
//		+-- CTEAnchor(cteB)
//			+--UnionAll
//				|--	LOJ
//				|	|--	CTEConsumer(cteA)
//				|	+--	CTEConsumer(cteB)
//				+--	Project
//					+--	LASJ
//					|	|--	CTEConsumer(cteB)
//					|	+--	CTEConsumer(cteA)
//					+-- (NULLS - same schema of A)
//
//		Also, two CTE producers for cteA and cteB are added to CTE info
//
//---------------------------------------------------------------------------
void
CXformExpandFullOuterJoin::Transform(CXformContext *pxfctxt,
									 CXformResult *pxfres,
									 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpression *pexprA = (*pexpr)[0];
	CExpression *pexprB = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	// 1. create the CTE producers
	const ULONG ulCTEIdA = COptCtxt::PoctxtFromTLS()->Pcteinfo()->next_id();
	CColRefArray *pdrspqcrOutA = pexprA->DeriveOutputColumns()->Pdrspqcr(mp);
	(void) CXformUtils::PexprAddCTEProducer(mp, ulCTEIdA, pdrspqcrOutA, pexprA);

	const ULONG ulCTEIdB = COptCtxt::PoctxtFromTLS()->Pcteinfo()->next_id();
	CColRefArray *pdrspqcrOutB = pexprB->DeriveOutputColumns()->Pdrspqcr(mp);
	(void) CXformUtils::PexprAddCTEProducer(mp, ulCTEIdB, pdrspqcrOutB, pexprB);

	// 2. create the right child (PROJECT over LASJ)
	CColRefArray *pdrspqcrRightA = CUtils::PdrspqcrCopy(mp, pdrspqcrOutA);
	CColRefArray *pdrspqcrRightB = CUtils::PdrspqcrCopy(mp, pdrspqcrOutB);
	CExpression *pexprScalarRight = CXformUtils::PexprRemapColumns(
		mp, pexprScalar, pdrspqcrOutA, pdrspqcrRightA, pdrspqcrOutB,
		pdrspqcrRightB);
	CExpression *pexprLASJ = PexprLogicalJoinOverCTEs(
		mp, EdxljtLeftAntiSemijoin, ulCTEIdB, pdrspqcrRightB, ulCTEIdA,
		pdrspqcrRightA, pexprScalarRight);
	CExpression *pexprProject =
		CUtils::PexprLogicalProjectNulls(mp, pdrspqcrRightA, pexprLASJ);

	// 3. create the left child (LOJ) - this has to use the original output
	//    columns and the original scalar expression
	pexprScalar->AddRef();
	CExpression *pexprLOJ =
		PexprLogicalJoinOverCTEs(mp, EdxljtLeft, ulCTEIdA, pdrspqcrOutA,
								 ulCTEIdB, pdrspqcrOutB, pexprScalar);

	// 4. create the UNION ALL expression

	// output columns of the union are the same as the outputs of the first child (LOJ)
	CColRefArray *pdrspqcrOutput = SPQOS_NEW(mp) CColRefArray(mp);
	pdrspqcrOutput->AppendArray(pdrspqcrOutA);
	pdrspqcrOutput->AppendArray(pdrspqcrOutB);

	// input columns of the union
	CColRef2dArray *pdrgdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);

	// inputs from the first child (LOJ)
	pdrspqcrOutput->AddRef();
	pdrgdrspqcrInput->Append(pdrspqcrOutput);

	// inputs from the second child have to be in the correct order
	// a. add new computed columns from the project only
	CColRefSet *pcrsProjOnly = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsProjOnly->Include(pexprProject->DeriveOutputColumns());
	pcrsProjOnly->Exclude(pdrspqcrRightB);
	CColRefArray *pdrspqcrProj = pcrsProjOnly->Pdrspqcr(mp);
	pcrsProjOnly->Release();
	// b. add columns from the LASJ expression
	pdrspqcrProj->AppendArray(pdrspqcrRightB);

	pdrgdrspqcrInput->Append(pdrspqcrProj);

	CExpression *pexprUnionAll = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalUnionAll(mp, pdrspqcrOutput, pdrgdrspqcrInput),
		pexprLOJ, pexprProject);

	// 5. Add CTE anchor for the B subtree
	CExpression *pexprAnchorB = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEIdB), pexprUnionAll);

	// 6. Add CTE anchor for the A subtree
	CExpression *pexprAnchorA = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEIdA), pexprAnchorB);

	// add alternative to xform result
	pxfres->Add(pexprAnchorA);
}

//---------------------------------------------------------------------------
//	@function:
//		CXformExpandFullOuterJoin::PexprLogicalJoinOverCTEs
//
//	@doc:
//		Construct a join expression of two CTEs using the given CTE ids
// 		and output columns
//
//---------------------------------------------------------------------------
CExpression *
CXformExpandFullOuterJoin::PexprLogicalJoinOverCTEs(
	CMemoryPool *mp, EdxlJoinType edxljointype, ULONG ulLeftCTEId,
	CColRefArray *pdrspqcrLeft, ULONG ulRightCTEId, CColRefArray *pdrspqcrRight,
	CExpression *pexprScalar) const
{
	SPQOS_ASSERT(NULL != pexprScalar);

	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	CCTEInfo *pcteinfo = COptCtxt::PoctxtFromTLS()->Pcteinfo();

	CLogicalCTEConsumer *popConsumerLeft =
		SPQOS_NEW(mp) CLogicalCTEConsumer(mp, ulLeftCTEId, pdrspqcrLeft);
	CExpression *pexprLeft = SPQOS_NEW(mp) CExpression(mp, popConsumerLeft);
	pcteinfo->IncrementConsumers(ulLeftCTEId);

	CLogicalCTEConsumer *popConsumerRight =
		SPQOS_NEW(mp) CLogicalCTEConsumer(mp, ulRightCTEId, pdrspqcrRight);
	CExpression *pexprRight = SPQOS_NEW(mp) CExpression(mp, popConsumerRight);
	pcteinfo->IncrementConsumers(ulRightCTEId);

	pdrspqexprChildren->Append(pexprLeft);
	pdrspqexprChildren->Append(pexprRight);
	pdrspqexprChildren->Append(pexprScalar);

	return CUtils::PexprLogicalJoin(mp, edxljointype, pdrspqexprChildren);
}

// EOF
