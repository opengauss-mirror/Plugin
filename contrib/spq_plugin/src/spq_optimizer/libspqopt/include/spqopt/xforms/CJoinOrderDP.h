//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CJoinOrderDP.h
//
//	@doc:
//		Dynamic programming-based join order generation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJoinOrderDP_H
#define SPQOPT_CJoinOrderDP_H

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"
#include "spqos/common/CHashMap.h"
#include "spqos/common/DbgPrintMixin.h"
#include "spqos/io/IOstream.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/xforms/CJoinOrder.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CJoinOrderDP
//
//	@doc:
//		Helper class for creating join orders using dynamic programming
//
//---------------------------------------------------------------------------
class CJoinOrderDP : public CJoinOrder, public spqos::DbgPrintMixin<CJoinOrderDP>
{
private:
	//---------------------------------------------------------------------------
	//	@struct:
	//		SComponentPair
	//
	//	@doc:
	//		Struct to capture a pair of components
	//
	//---------------------------------------------------------------------------
	struct SComponentPair : public CRefCount
	{
		// first component
		CBitSet *m_pbsFst;

		// second component
		CBitSet *m_pbsSnd;

		// ctor
		SComponentPair(CBitSet *pbsFst, CBitSet *pbsSnd);

		// dtor
		~SComponentPair();

		// hashing function
		static ULONG HashValue(const SComponentPair *pcomppair);

		// equality function
		static BOOL Equals(const SComponentPair *pcomppairFst,
						   const SComponentPair *pcomppairSnd);
	};

	// hashing function
	static ULONG
	UlHashBitSet(const CBitSet *pbs)
	{
		SPQOS_ASSERT(NULL != pbs);

		return pbs->HashValue();
	}

	// equality function
	static BOOL
	FEqualBitSet(const CBitSet *pbsFst, const CBitSet *pbsSnd)
	{
		SPQOS_ASSERT(NULL != pbsFst);
		SPQOS_ASSERT(NULL != pbsSnd);

		return pbsFst->Equals(pbsSnd);
	}

	// hash map from component to best join order
	typedef CHashMap<CBitSet, CExpression, UlHashBitSet, FEqualBitSet,
					 CleanupRelease<CBitSet>, CleanupRelease<CExpression> >
		BitSetToExpressionMap;

	// hash map from component pair to connecting edges
	typedef CHashMap<SComponentPair, CExpression, SComponentPair::HashValue,
					 SComponentPair::Equals, CleanupRelease<SComponentPair>,
					 CleanupRelease<CExpression> >
		ComponentPairToExpressionMap;

	// hash map from expression to cost of best join order
	typedef CHashMap<CExpression, CDouble, CExpression::HashValue,
					 CUtils::Equals, CleanupRelease<CExpression>,
					 CleanupDelete<CDouble> >
		ExpressionToCostMap;

	// lookup table for links
	ComponentPairToExpressionMap *m_phmcomplink;

	// dynamic programming table
	BitSetToExpressionMap *m_phmbsexpr;

	// map of expressions to its cost
	ExpressionToCostMap *m_phmexprcost;

	// array of top-k join expression
	CExpressionArray *m_pdrspqexprTopKOrders;

	// dummy expression to used for non-joinable components
	CExpression *m_pexprDummy;

	// build expression linking given components
	CExpression *PexprBuildPred(CBitSet *pbsFst, CBitSet *pbsSnd);

	// lookup best join order for given set
	CExpression *PexprLookup(CBitSet *pbs);

	// extract predicate joining the two given sets
	CExpression *PexprPred(CBitSet *pbsFst, CBitSet *pbsSnd);

	// join expressions in the given two sets
	CExpression *PexprJoin(CBitSet *pbsFst, CBitSet *pbsSnd);

	// join expressions in the given set
	CExpression *PexprJoin(CBitSet *pbs);

	// find best join order for given component using dynamic programming
	CExpression *PexprBestJoinOrderDP(CBitSet *pbs);

	// find best join order for given component
	CExpression *PexprBestJoinOrder(CBitSet *pbs);

	// generate cross product for the given components
	CExpression *PexprCross(CBitSet *pbs);

	// join a covered subset with uncovered subset
	CExpression *PexprJoinCoveredSubsetWithUncoveredSubset(
		CBitSet *pbs, CBitSet *pbsCovered, CBitSet *pbsUncovered);

	// return a subset of the given set covered by one or more edges
	CBitSet *PbsCovered(CBitSet *pbsInput);

	// add given join order to best results
	void AddJoinOrder(CExpression *pexprJoin, CDouble dCost);

	// compute cost of given join expression
	CDouble DCost(CExpression *pexpr);

	// derive stats on given expression
	virtual void DeriveStats(CExpression *pexpr);

	// add expression to cost map
	void InsertExpressionCost(CExpression *pexpr, CDouble dCost,
							  BOOL fValidateInsert);

	// generate all subsets of the given array of elements
	static void GenerateSubsets(CMemoryPool *mp, CBitSet *pbsCurrent,
								ULONG *pulElems, ULONG size, ULONG ulIndex,
								CBitSetArray *pdrspqbsSubsets);

	// driver of subset generation
	static CBitSetArray *PdrspqbsSubsets(CMemoryPool *mp, CBitSet *pbs);

public:
	// ctor
	CJoinOrderDP(CMemoryPool *mp, CExpressionArray *pdrspqexprComponents,
				 CExpressionArray *pdrspqexprConjuncts);

	// dtor
	virtual ~CJoinOrderDP();

	// main handler
	virtual CExpression *PexprExpand();

	// best join orders
	CExpressionArray *
	PdrspqexprTopK() const
	{
		return m_pdrspqexprTopKOrders;
	}

	// print function
	virtual IOstream &OsPrint(IOstream &) const;

	virtual CXform::EXformId
	EOriginXForm() const
	{
		return CXform::ExfExpandNAryJoinDP;
	}


};	// class CJoinOrderDP

}  // namespace spqopt

#endif	// !SPQOPT_CJoinOrderDP_H

// EOF
