//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	Transform Inner/Outer Join to Index Apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformJoin2IndexApply_H
#define SPQOPT_CXformJoin2IndexApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalApply.h"
#include "spqopt/operators/CLogicalJoin.h"
#include "spqopt/xforms/CXformExploration.h"
#include "spqopt/xforms/CXformUtils.h"

namespace spqopt
{
using namespace spqos;

// fwd declaration
class CLogicalDynamicGet;

class CXformJoin2IndexApply : public CXformExploration
{
private:
	// private copy ctor
	CXformJoin2IndexApply(const CXformJoin2IndexApply &);

	// helper to add IndexApply expression to given xform results container
	// for homogeneous b-tree indexes
	void CreateHomogeneousBtreeIndexApplyAlternatives(
		CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
		CExpression *pexprInner, CExpression *pexprScalar,
		CExpression *origJoinPred, CExpression *nodesToInsertAboveIndexGet,
		CExpression *endOfNodesToInsertAboveIndexGet,
		CTableDescriptor *ptabdescInner, CLogicalDynamicGet *popDynamicGet,
		CColRefSet *pcrsScalarExpr, CColRefSet *outer_refs,
		CColRefSet *pcrsReqd, ULONG ulIndices, CXformResult *pxfres) const;

	// helper to add IndexApply expression to given xform results container
	// for homogeneous b-tree indexes
	void CreateAlternativesForBtreeIndex(
		CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
		CExpression *pexprInner, CExpression *origJoinPred,
		CExpression *nodesToInsertAboveIndexGet,
		CExpression *endOfNodesToInsertAboveIndexGet, CMDAccessor *md_accessor,
		CExpressionArray *pdrspqexprConjuncts, CColRefSet *pcrsScalarExpr,
		CColRefSet *outer_refs, CColRefSet *pcrsReqd, const IMDRelation *pmdrel,
		const IMDIndex *pmdindex, CPartConstraint *ppartcnstrIndex,
		CXformResult *pxfres) const;

	// helper to add IndexApply expression to given xform results container
	// for homogeneous bitmap indexes
	void CreateHomogeneousBitmapIndexApplyAlternatives(
		CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
		CExpression *pexprInner, CExpression *pexprScalar,
		CExpression *origJoinPred, CExpression *nodesToInsertAboveIndexGet,
		CExpression *endOfNodesToInsertAboveIndexGet,
		CTableDescriptor *ptabdescInner, CColRefSet *outer_refs,
		CColRefSet *pcrsReqd, CXformResult *pxfres) const;

	// based on the inner and the scalar expression, it computes scalar expression
	// columns, outer references and required columns
	void ComputeColumnSets(CMemoryPool *mp, CExpression *pexprInner,
						   CExpression *pexprScalar,
						   CColRefSet **ppcrsScalarExpr,
						   CColRefSet **ppcrsOuterRefs,
						   CColRefSet **ppcrsReqd) const;

	// create an index apply plan when applicable
	void CreatePartialIndexApplyPlan(
		CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
		CExpression *pexprScalar, CColRefSet *outer_refs,
		CLogicalDynamicGet *popDynamicGet,
		SPartDynamicIndexGetInfoArray *pdrspqpartdig, const IMDRelation *pmdrel,
		CXformResult *pxfres) const;

	// create an join with a CTE consumer on the inner branch, with the given
	// partition constraint
	CExpression *PexprJoinOverCTEConsumer(
		CMemoryPool *mp, COperator *joinOp, CLogicalDynamicGet *popDynamicGet,
		ULONG ulCTEId, CExpression *pexprScalar,
		CColRefArray *pdrspqcrDynamicGet, CPartConstraint *ppartcnstr,
		CColRefArray *pdrspqcrOuter, CColRefArray *pdrspqcrOuterNew) const;

	// create an index apply with a CTE consumer on the outer branch
	// and a dynamic get on the inner one
	CExpression *PexprIndexApplyOverCTEConsumer(
		CMemoryPool *mp, COperator *joinOp, CLogicalDynamicGet *popDynamicGet,
		CExpressionArray *pdrspqexprIndex, CExpressionArray *pdrspqexprResidual,
		CColRefArray *pdrspqcrIndexGet, const IMDIndex *pmdindex,
		const IMDRelation *pmdrel, BOOL fFirst, ULONG ulCTEId,
		CPartConstraint *ppartcnstr, CColRefSet *outer_refs,
		CColRefArray *pdrspqcrOuter, CColRefArray *pdrspqcrOuterNew,
		CColRefArray *pdrspqcrOuterRefsInScan,
		ULongPtrArray *pdrspqulIndexesOfRefsInScan) const;

	// create a union-all with the given children
	CExpression *PexprConstructUnionAll(CMemoryPool *mp,
										CColRefArray *pdrspqcrLeftSchema,
										CColRefArray *pdrspqcrRightSchema,
										CExpression *pexprLeftChild,
										CExpression *pexprRightChild,
										ULONG scan_id) const;

	//	construct a CTE Anchor over the given UnionAll and adds it to the given
	//	Xform result
	void AddUnionPlanForPartialIndexes(CMemoryPool *mp,
									   CLogicalDynamicGet *popDynamicGet,
									   ULONG ulCTEId, CExpression *pexprUnion,
									   CExpression *pexprScalar,
									   CXformResult *pxfres) const;

protected:
	// is the logical join that is being transformed an outer join?
	BOOL m_fOuterJoin;

	// helper to add IndexApply expression to given xform results container
	// for homogeneous indexes
	virtual void CreateHomogeneousIndexApplyAlternatives(
		CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
		CExpression *pexprInner, CExpression *pexprScalar,
		CExpression *origJoinPred, CExpression *nodesToInsertAboveIndexGet,
		CExpression *endOfNodesToInsertAboveIndexGet,
		CTableDescriptor *PtabdescInner, CLogicalDynamicGet *popDynamicGet,
		CXformResult *pxfres, spqmd::IMDIndex::EmdindexType emdtype) const;

	// helper to add IndexApply expression to given xform results container
	// for partial indexes
	virtual void CreatePartialIndexApplyAlternatives(
		CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
		CExpression *pexprInner, CExpression *pexprScalar,
		CTableDescriptor *PtabdescInner, CLogicalDynamicGet *popDynamicGet,
		CXformResult *pxfres) const;

	// return the new instance of logical join operator
	// being targeted in the current xform rule, caller
	// takes the ownership and responsibility to release
	// the instance.
	virtual CLogicalJoin *PopLogicalJoin(CMemoryPool *mp) const = 0;

	// return the new instance of logical apply operator
	// that it is trying to transform to in the current
	// xform rule, caller takes the ownership and
	// responsibility to release the instance.
	virtual CLogicalApply *PopLogicalApply(CMemoryPool *mp,
										   CColRefArray *pdrspqcrOuterRefs,
										   CExpression *origJoinPred) const = 0;

public:
	// ctor
	explicit CXformJoin2IndexApply(CExpression *pexprPattern)
		: CXformExploration(pexprPattern)
	{
		m_fOuterJoin = (COperator::EopLogicalLeftOuterJoin ==
						pexprPattern->Pop()->Eopid());
	}

	// dtor
	virtual ~CXformJoin2IndexApply()
	{
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

};	// class CXformJoin2IndexApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformJoin2IndexApply_H

// EOF
