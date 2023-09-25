//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2012 EMC Corp.
//
//	@filename:
//		CXformUtils.h
//
//	@doc:
//		Utility functions for xforms
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformUtils_H
#define SPQOPT_CXformUtils_H

#include "spqos/base.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CColRef.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalDML.h"
#include "spqopt/operators/CLogicalDynamicIndexGet.h"
#include "spqopt/operators/CLogicalIndexGet.h"
#include "spqopt/operators/CLogicalJoin.h"
#include "spqopt/operators/CPhysicalJoin.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/xforms/CXform.h"
#include "naucrates/md/IMDIndex.h"
#include "naucrates/md/IMDScalarOp.h"

namespace spqopt
{
using namespace spqos;

// forward declarations
class CGroupExpression;
class CColRefSet;
class CExpression;
class CLogical;
class CLogicalDynamicGet;
class CPartConstraint;
class CTableDescriptor;

// structure describing a candidate for a partial dynamic index scan
struct SPartDynamicIndexGetInfo
{
	// md index
	const IMDIndex *m_pmdindex;

	// part constraint
	CPartConstraint *m_part_constraint;

	// index predicate expressions
	CExpressionArray *m_pdrspqexprIndex;

	// residual expressions
	CExpressionArray *m_pdrspqexprResidual;

	// ctor
	SPartDynamicIndexGetInfo(const IMDIndex *pmdindex,
							 CPartConstraint *ppartcnstr,
							 CExpressionArray *pdrspqexprIndex,
							 CExpressionArray *pdrspqexprResidual)
		: m_pmdindex(pmdindex),
		  m_part_constraint(ppartcnstr),
		  m_pdrspqexprIndex(pdrspqexprIndex),
		  m_pdrspqexprResidual(pdrspqexprResidual)
	{
		SPQOS_ASSERT(NULL != ppartcnstr);
	}

	// dtor
	~SPartDynamicIndexGetInfo()
	{
		m_part_constraint->Release();
		CRefCount::SafeRelease(m_pdrspqexprIndex);
		CRefCount::SafeRelease(m_pdrspqexprResidual);
	}
};

// arrays over partial dynamic index get candidates
typedef CDynamicPtrArray<SPartDynamicIndexGetInfo, CleanupDelete>
	SPartDynamicIndexGetInfoArray;
typedef CDynamicPtrArray<SPartDynamicIndexGetInfoArray, CleanupRelease>
	SPartDynamicIndexGetInfoArrays;

// map of expression to array of expressions
typedef CHashMap<CExpression, CExpressionArray, CExpression::HashValue,
				 CUtils::Equals, CleanupRelease<CExpression>,
				 CleanupRelease<CExpressionArray> >
	ExprToExprArrayMap;

// iterator of map of expression to array of expressions
typedef CHashMapIter<CExpression, CExpressionArray, CExpression::HashValue,
					 CUtils::Equals, CleanupRelease<CExpression>,
					 CleanupRelease<CExpressionArray> >
	ExprToExprArrayMapIter;

// array of array of expressions
typedef CDynamicPtrArray<CExpressionArray, CleanupRelease> CExpressionArrays;

//---------------------------------------------------------------------------
//	@class:
//		CXformUtils
//
//	@doc:
//		Utility functions for xforms
//
//---------------------------------------------------------------------------
class CXformUtils
{
private:
	// enum marking the index column types
	enum EIndexCols
	{
		EicKey,
		EicIncluded
	};

	typedef CLogical *(*PDynamicIndexOpConstructor)(
		CMemoryPool *mp, const IMDIndex *pmdindex, CTableDescriptor *ptabdesc,
		ULONG ulOriginOpId, CName *pname, ULONG ulPartIndex,
		CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrPart,
		ULONG ulSecondaryPartIndexId, CPartConstraint *ppartcnstr,
		CPartConstraint *ppartcnstrRel);

	typedef CLogical *(*PStaticIndexOpConstructor)(
		CMemoryPool *mp, const IMDIndex *pmdindex, CTableDescriptor *ptabdesc,
		ULONG ulOriginOpId, CName *pname, CColRefArray *pdrspqcrOutput);

	typedef CExpression *(PRewrittenIndexPath)(CMemoryPool *mp,
											   CExpression *pexprIndexCond,
											   CExpression *pexprResidualCond,
											   const IMDIndex *pmdindex,
											   CTableDescriptor *ptabdesc,
											   COperator *popLogical);

	// private copy ctor
	CXformUtils(const CXformUtils &);

	// create a logical assert for the not nullable columns of the given table
	// on top of the given child expression
	static CExpression *PexprAssertNotNull(CMemoryPool *mp,
										   CExpression *pexprChild,
										   CTableDescriptor *ptabdesc,
										   CColRefArray *colref_array);

	// create a logical assert for the check constraints on the given table
	static CExpression *PexprAssertCheckConstraints(CMemoryPool *mp,
													CExpression *pexprChild,
													CTableDescriptor *ptabdesc,
													CColRefArray *colref_array);

	// add a min(col) project element to the given expression list for
	// each column in the given column array
	static void AddMinAggs(CMemoryPool *mp, CMDAccessor *md_accessor,
						   CColumnFactory *col_factory,
						   CColRefArray *colref_array,
						   ColRefToColRefMap *phmcrcr,
						   CExpressionArray *pdrspqexpr,
						   CColRefArray **ppdrspqcrNew);

	// check if all columns support MIN aggregate
	static BOOL FSupportsMinAgg(CColRefArray *colref_array);

	// helper for extracting foreign key
	static CColRefSet *PcrsFKey(CMemoryPool *mp, CExpressionArray *pdrspqexpr,
								CColRefSet *prcsOutput, CColRefSet *pcrsKey);

	// return the set of columns from the given array of columns which appear
	// in the index included / key columns
	static CColRefSet *PcrsIndexColumns(CMemoryPool *mp,
										CColRefArray *colref_array,
										const IMDIndex *pmdindex,
										const IMDRelation *pmdrel,
										EIndexCols eic);

	// return the ordered array of columns from the given array of columns which appear
	// in the index included / key columns
	static CColRefArray *PdrspqcrIndexColumns(CMemoryPool *mp,
											 CColRefArray *colref_array,
											 const IMDIndex *pmdindex,
											 const IMDRelation *pmdrel,
											 EIndexCols eic);

	// lookup join keys in scalar child group
	static void LookupJoinKeys(CMemoryPool *mp, CExpression *pexpr,
							   CExpressionArray **ppdrspqexprOuter,
							   CExpressionArray **ppdrspqexprInner,
							   IMdIdArray **join_opfamilies);

	// cache join keys on scalar child group
	static void CacheJoinKeys(CExpression *pexpr,
							  CExpressionArray *pdrspqexprOuter,
							  CExpressionArray *pdrspqexprInner,
							  IMdIdArray *join_opfamilies);

	// helper to extract equality from a given expression
	static BOOL FExtractEquality(
		CExpression *pexpr,
		CExpression **
			ppexprEquality,	 // output: extracted equality expression, set to NULL if extraction failed
		CExpression **
			ppexprOther	 // output: sibling of equality expression, set to NULL if extraction failed
	);

	// check if given xform id is in the given array of xforms
	static BOOL FXformInArray(CXform::EXformId exfid,
							  CXform::EXformId rgXforms[], ULONG ulXforms);

#ifdef SPQOS_DEBUG
	// check whether the given join type is swapable
	static BOOL FSwapableJoinType(COperator::EOperatorId op_id);
#endif	// SPQOS_DEBUG

	// helper function for adding hash join alternative
	template <class T>
	static void AddHashOrMergeJoinAlternative(CMemoryPool *mp,
											  CExpression *pexprJoin,
											  CExpressionArray *pdrspqexprOuter,
											  CExpressionArray *pdrspqexprInner,
											  IMdIdArray *join_opfamilies,
											  CXformResult *pxfres,
											  BOOL is_hash_join_null_aware);

	// helper for transforming SubqueryAll into aggregate subquery
	static void SubqueryAllToAgg(
		CMemoryPool *mp, CExpression *pexprSubquery,
		CExpression *
			*ppexprNewSubquery,	 // output argument for new scalar subquery
		CExpression *
			*ppexprNewScalar  // output argument for new scalar expression
	);

	// helper for transforming SubqueryAny into aggregate subquery
	static void SubqueryAnyToAgg(
		CMemoryPool *mp, CExpression *pexprSubquery,
		CExpression *
			*ppexprNewSubquery,	 // output argument for new scalar subquery
		CExpression *
			*ppexprNewScalar  // output argument for new scalar expression
	);

	// create the Gb operator to be pushed below a join
	static CLogicalGbAgg *PopGbAggPushableBelowJoin(CMemoryPool *mp,
													CLogicalGbAgg *popGbAggOld,
													CColRefSet *prcsOutput,
													CColRefSet *pcrsGrpCols);

	// check if the preconditions for pushing down Group by through join are satisfied
	static BOOL FCanPushGbAggBelowJoin(CColRefSet *pcrsGrpCols,
									   CColRefSet *pcrsJoinOuterChildOutput,
									   CColRefSet *pcrsJoinScalarUsedFromOuter,
									   CColRefSet *pcrsGrpByOutput,
									   CColRefSet *pcrsGrpByUsed,
									   CColRefSet *pcrsFKey);

	// construct an expression representing a new access path using the given functors for
	// operator constructors and rewritten access path
	static CExpression *PexprBuildIndexPlan(
		CMemoryPool *mp, CMDAccessor *md_accessor, CExpression *pexprGet,
		ULONG ulOriginOpId, CExpressionArray *pdrspqexprConds,
		CColRefSet *pcrsReqd, CColRefSet *pcrsScalarExpr,
		CColRefSet *outer_refs, const IMDIndex *pmdindex,
		const IMDRelation *pmdrel, BOOL fAllowPartialIndex,
		CPartConstraint *ppcForPartialIndexes,
		IMDIndex::EmdindexType emdindtype, PDynamicIndexOpConstructor pdiopc,
		PStaticIndexOpConstructor psiopc, PRewrittenIndexPath prip);

	// create a dynamic operator for a btree index plan
	static CLogical *
	PopDynamicBtreeIndexOpConstructor(
		CMemoryPool *mp, const IMDIndex *pmdindex, CTableDescriptor *ptabdesc,
		ULONG ulOriginOpId, CName *pname, ULONG ulPartIndex,
		CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrPart,
		ULONG ulSecondaryPartIndexId, CPartConstraint *ppartcnstr,
		CPartConstraint *ppartcnstrRel)
	{
		return SPQOS_NEW(mp) CLogicalDynamicIndexGet(
			mp, pmdindex, ptabdesc, ulOriginOpId, pname, ulPartIndex,
			pdrspqcrOutput, pdrspqdrspqcrPart, ulSecondaryPartIndexId, ppartcnstr,
			ppartcnstrRel);
	}

	//	create a static operator for a btree index plan
	static CLogical *
	PopStaticBtreeIndexOpConstructor(CMemoryPool *mp, const IMDIndex *pmdindex,
									 CTableDescriptor *ptabdesc,
									 ULONG ulOriginOpId, CName *pname,
									 CColRefArray *pdrspqcrOutput)
	{
		return SPQOS_NEW(mp) CLogicalIndexGet(
			mp, pmdindex, ptabdesc, ulOriginOpId, pname, pdrspqcrOutput);
	}

	//	produce an expression representing a new btree index path
	static CExpression *
	PexprRewrittenBtreeIndexPath(CMemoryPool *mp, CExpression *pexprIndexCond,
								 CExpression *pexprResidualCond,
								 const IMDIndex *,	  // pmdindex
								 CTableDescriptor *,  // ptabdesc
								 COperator *popLogical)
	{
		// create the expression containing the logical index get operator
		return CUtils::PexprSafeSelect(
			mp, SPQOS_NEW(mp) CExpression(mp, popLogical, pexprIndexCond),
			pexprResidualCond);
	}

	// create a candidate dynamic get scan to suplement the partial index scans
	static SPartDynamicIndexGetInfo *PpartdigDynamicGet(
		CMemoryPool *mp, CExpressionArray *pdrspqexprScalar,
		CPartConstraint *ppartcnstrCovered, CPartConstraint *ppartcnstrRel);

	// returns true iff the given expression is a Not operator whose child is a
	// scalar identifier
	static BOOL FNotIdent(CExpression *pexpr);

	// creates a condition of the form col = value, where col is the given column
	static CExpression *PexprEqualityOnBoolColumn(CMemoryPool *mp,
												  CMDAccessor *md_accessor,
												  BOOL fNegated,
												  CColRef *colref);

	// construct a bitmap index path expression for the given predicate
	// out of the children of the given expression
	static CExpression *PexprBitmapLookupWithPredicateBreakDown(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpression *pexprOriginalPred, CExpression *pexprPred,
		CTableDescriptor *ptabdesc, const IMDRelation *pmdrel,
		CColRefArray *pdrspqcrOutput, CColRefSet *outer_refs,
		CColRefSet *pcrsReqd, CExpression **ppexprRecheck,
		CExpression **ppexprResidual);

	// compute the residual predicate for a bitmap table scan
	static void ComputeBitmapTableScanResidualPredicate(
		CMemoryPool *mp, BOOL fConjunction, CExpression *pexprOriginalPred,
		CExpression **ppexprResidual, CExpressionArray *pdrspqexprResidualNew);

	// construct a bitmap index path expression for the given predicate coming
	// from a condition without outer references
	static CExpression *PexprBitmapSelectBestIndex(
		CMemoryPool *mp, CMDAccessor *md_accessor, CExpression *pexprPred,
		CTableDescriptor *ptabdesc, const IMDRelation *pmdrel,
		CColRefArray *pdrspqcrOutput, CColRefSet *pcrsReqd,
		CColRefSet *pcrsOuterRefs, CExpression **ppexprRecheck,
		CExpression **ppexprResidual, BOOL alsoConsiderBTreeIndexes);

	// iterate over given hash map and return array of arrays of project elements sorted by the column id of the first entries
	static CExpressionArrays *PdrspqdrspqexprSortedPrjElemsArray(
		CMemoryPool *mp, ExprToExprArrayMap *phmexprdrspqexpr);

	// comparator used in sorting arrays of project elements based on the column id of the first entry
	static INT ICmpPrjElemsArr(const void *pvFst, const void *pvSnd);

public:
	// helper function for implementation xforms on binary operators
	// with predicates (e.g. joins)
	template <class T>
	static void TransformImplementBinaryOp(CXformContext *pxfctxt,
										   CXformResult *pxfres,
										   CExpression *pexpr);

	// helper function for implementation of hash joins
	template <class T>
	static void ImplementHashJoin(CXformContext *pxfctxt, CXformResult *pxfres,
								  CExpression *pexpr);

	// helper function for implementation of merge joins
	template <class T>
	static void ImplementMergeJoin(CXformContext *pxfctxt, CXformResult *pxfres,
								   CExpression *pexpr);

	// helper function for implementation of nested loops joins
	template <class T>
	static void ImplementNLJoin(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr);

	// helper for removing IsNotFalse join predicate for SPQDB anti-semi hash join
	static BOOL FProcessSPQDBAntiSemiHashJoin(CMemoryPool *mp,
											 CExpression *pexpr,
											 CExpression **ppexprResult);

	// check the applicability of logical join to physical join xform
	static CXform::EXformPromise ExfpLogicalJoin2PhysicalJoin(
		CExpressionHandle &exprhdl);

	// check the applicability of semi join to cross product
	static CXform::EXformPromise ExfpSemiJoin2CrossProduct(
		CExpressionHandle &exprhdl);

	// check the applicability of N-ary join expansion
	static CXform::EXformPromise ExfpExpandJoinOrder(CExpressionHandle &exprhdl,
													 const CXform *xform);

	// extract foreign key
	static CColRefSet *PcrsFKey(CMemoryPool *mp, CExpression *pexprOuter,
								CExpression *pexprInner,
								CExpression *pexprScalar);

	// compute a swap of the two given joins
	static CExpression *PexprSwapJoins(CMemoryPool *mp,
									   CExpression *pexprTopJoin,
									   CExpression *pexprBottomJoin);

	// push a Gb, optionally with a having clause below the child join
	static CExpression *PexprPushGbBelowJoin(CMemoryPool *mp,
											 CExpression *pexpr);

	// check if the the array of aligned input columns are of the same type
	static BOOL FSameDatatype(CColRef2dArray *pdrspqdrspqcrInput);

	// helper function to separate subquery predicates in a top Select node
	static CExpression *PexprSeparateSubqueryPreds(CMemoryPool *mp,
												   CExpression *pexpr);

	// helper for creating inverse predicate for unnesting subquery ALL
	static CExpression *PexprInversePred(CMemoryPool *mp,
										 CExpression *pexprSubquery);

	// helper for creating a null indicator expression
	static CExpression *PexprNullIndicator(CMemoryPool *mp, CExpression *pexpr);

	// helper for creating a logical DML on top of a project
	static CExpression *PexprLogicalDMLOverProject(
		CMemoryPool *mp, CExpression *pexprChild,
		CLogicalDML::EDMLOperator edmlop, CTableDescriptor *ptabdesc,
		CColRefArray *colref_array, CColRef *pcrCtid, CColRef *pcrSegmentId);

	// check whether there are any BEFORE or AFTER triggers on the
	// given table that match the given DML operation
	static BOOL FTriggersExist(CLogicalDML::EDMLOperator edmlop,
							   CTableDescriptor *ptabdesc, BOOL fBefore);

	// does the given trigger type match the given logical DML type
	static BOOL FTriggerApplies(CLogicalDML::EDMLOperator edmlop,
								const IMDTrigger *pmdtrigger);

	// construct a trigger expression on top of the given expression
	static CExpression *PexprRowTrigger(CMemoryPool *mp,
										CExpression *pexprChild,
										CLogicalDML::EDMLOperator edmlop,
										IMDId *rel_mdid, BOOL fBefore,
										CColRefArray *colref_array);

	// construct a trigger expression on top of the given expression
	static CExpression *PexprRowTrigger(CMemoryPool *mp,
										CExpression *pexprChild,
										CLogicalDML::EDMLOperator edmlop,
										IMDId *rel_mdid, BOOL fBefore,
										CColRefArray *pdrspqcrOld,
										CColRefArray *pdrspqcrNew);

	// construct a logical partition selector for the given table descriptor on top
	// of the given child expression. The partition selection filters use columns
	// from the given column array
	static CExpression *PexprLogicalPartitionSelector(
		CMemoryPool *mp, CTableDescriptor *ptabdesc, CColRefArray *colref_array,
		CExpression *pexprChild);

	// return partition filter expressions given a table
	// descriptor and the given column references
	static CExpressionArray *PdrspqexprPartEqFilters(
		CMemoryPool *mp, CTableDescriptor *ptabdesc,
		CColRefArray *pdrspqcrSource);

	// helper for creating Agg expression equivalent to quantified subquery
	static void QuantifiedToAgg(CMemoryPool *mp, CExpression *pexprSubquery,
								CExpression **ppexprNewSubquery,
								CExpression **ppexprNewScalar);

	// helper for creating Agg expression equivalent to existential subquery
	static void ExistentialToAgg(CMemoryPool *mp, CExpression *pexprSubquery,
								 CExpression **ppexprNewSubquery,
								 CExpression **ppexprNewScalar);

	// create a logical assert for the check constraints on the given table
	static CExpression *PexprAssertConstraints(CMemoryPool *mp,
											   CExpression *pexprChild,
											   CTableDescriptor *ptabdesc,
											   CColRefArray *colref_array);

	// create a logical assert for checking cardinality of update values
	static CExpression *PexprAssertUpdateCardinality(CMemoryPool *mp,
													 CExpression *pexprDMLChild,
													 CExpression *pexprDML,
													 CColRef *pcrCtid,
													 CColRef *pcrSegmentId);

	// return true if stats derivation is needed for this xform
	static BOOL FDeriveStatsBeforeXform(CXform *pxform);

	// return true if xform is a subquery decorrelation xform
	static BOOL FSubqueryDecorrelation(CXform *pxform);

	// return true if xform is a subquery unnesting xform
	static BOOL FSubqueryUnnesting(CXform *pxform);

	// return true if xform should be applied to the next binding
	static BOOL FApplyToNextBinding(CXform *pxform,
									CExpression *pexprLastBinding);

	// return a formatted error message for the given exception
	static CWStringConst *PstrErrorMessage(CMemoryPool *mp, ULONG major,
										   ULONG minor, ...);

	// return the array of key columns from the given array of columns which appear
	// in the index key columns
	static CColRefArray *PdrspqcrIndexKeys(CMemoryPool *mp,
										  CColRefArray *colref_array,
										  const IMDIndex *pmdindex,
										  const IMDRelation *pmdrel);

	// return the set of key columns from the given array of columns which appear
	// in the index key columns
	static CColRefSet *PcrsIndexKeys(CMemoryPool *mp,
									 CColRefArray *colref_array,
									 const IMDIndex *pmdindex,
									 const IMDRelation *pmdrel);

	// return the set of key columns from the given array of columns which appear
	// in the index included columns
	static CColRefSet *PcrsIndexIncludedCols(CMemoryPool *mp,
											 CColRefArray *colref_array,
											 const IMDIndex *pmdindex,
											 const IMDRelation *pmdrel);

	// check if an index is applicable given the required, output and scalar
	// expression columns
	static BOOL FIndexApplicable(
		CMemoryPool *mp, const IMDIndex *pmdindex, const IMDRelation *pmdrel,
		CColRefArray *pdrspqcrOutput, CColRefSet *pcrsReqd,
		CColRefSet *pcrsScalar, IMDIndex::EmdindexType emdindtype,
		IMDIndex::EmdindexType altindtype = IMDIndex::EmdindSentinel);

	// check whether a CTE should be inlined
	static BOOL FInlinableCTE(ULONG ulCTEId);

	// return the column reference of the n-th project element
	static CColRef *PcrProjectElement(CExpression *pexpr,
									  ULONG ulIdxProjElement);

	// create an expression with "row_number" window function
	static CExpression *PexprRowNumber(CMemoryPool *mp);

	// create a logical sequence project with a "row_number" window function
	static CExpression *PexprWindowWithRowNumber(CMemoryPool *mp,
												 CExpression *pexprWindowChild,
												 CColRefArray *pdrspqcrInput);

	// generate a logical Assert expression that errors out when more than one row is generated
	static CExpression *PexprAssertOneRow(CMemoryPool *mp,
										  CExpression *pexprChild);

	// helper for adding CTE producer to global CTE info structure
	static CExpression *PexprAddCTEProducer(CMemoryPool *mp, ULONG ulCTEId,
											CColRefArray *colref_array,
											CExpression *pexpr);

	// does transformation generate an Apply expression
	static BOOL
	FGenerateApply(CXform::EXformId exfid)
	{
		return CXform::ExfSelect2Apply == exfid ||
			   CXform::ExfProject2Apply == exfid ||
			   CXform::ExfGbAgg2Apply == exfid ||
			   CXform::ExfSubqJoin2Apply == exfid ||
			   CXform::ExfSubqNAryJoin2Apply == exfid ||
			   CXform::ExfSequenceProject2Apply == exfid;
	}

	// helper for creating IndexGet/DynamicIndexGet expression
	static CExpression *
	PexprLogicalIndexGet(CMemoryPool *mp, CMDAccessor *md_accessor,
						 CExpression *pexprGet, ULONG ulOriginOpId,
						 CExpressionArray *pdrspqexprConds, CColRefSet *pcrsReqd,
						 CColRefSet *pcrsScalarExpr, CColRefSet *outer_refs,
						 const IMDIndex *pmdindex, const IMDRelation *pmdrel,
						 BOOL fAllowPartialIndex,
						 CPartConstraint *ppcartcnstrIndex)
	{
		return PexprBuildIndexPlan(
			mp, md_accessor, pexprGet, ulOriginOpId, pdrspqexprConds, pcrsReqd,
			pcrsScalarExpr, outer_refs, pmdindex, pmdrel, fAllowPartialIndex,
			ppcartcnstrIndex, IMDIndex::EmdindBtree,
			PopDynamicBtreeIndexOpConstructor, PopStaticBtreeIndexOpConstructor,
			PexprRewrittenBtreeIndexPath);
	}

	// helper for creating bitmap bool op expressions
	static CExpression *PexprScalarBitmapBoolOp(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpression *pexprOriginalPred, CExpressionArray *pdrspqexpr,
		CTableDescriptor *ptabdesc, const IMDRelation *pmdrel,
		CColRefArray *pdrspqcrOutput, CColRefSet *outer_refs,
		CColRefSet *pcrsReqd, BOOL fConjunction, CExpression **ppexprRecheck,
		CExpression **ppexprResidual, BOOL isAPartialPredicate);

	// construct a bitmap bool op given the left and right bitmap access
	// path expressions
	static CExpression *PexprBitmapBoolOp(CMemoryPool *mp,
										  IMDId *pmdidBitmapType,
										  CExpression *pexprLeft,
										  CExpression *pexprRight,
										  BOOL fConjunction);

	// given an array of predicate expressions, construct a bitmap access path
	// expression for each predicate and accumulate it in the pdrspqexprBitmap array
	static void CreateBitmapIndexProbeOps(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpression *pexprOriginalPred, CExpressionArray *pdrspqexprPreds,
		CTableDescriptor *ptabdesc, const IMDRelation *pmdrel,
		CColRefArray *pdrspqcrOutput, CColRefSet *outer_refs,
		CColRefSet *pcrsReqd, BOOL fConjunction,
		CExpressionArray *pdrspqexprBitmap, CExpressionArray *pdrspqexprRecheck,
		CExpressionArray *pdrspqexprResidual, BOOL isAPartialPredicate);

	static void CreateBitmapIndexProbesWithOrWithoutPredBreakdown(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpression *pexprOriginalPred, CExpression *pexprPred,
		CTableDescriptor *ptabdesc, const IMDRelation *pmdrel,
		CColRefArray *pdrspqcrOutput, CColRefSet *outer_refs,
		CColRefSet *pcrsReqd, CExpression **pexprBitmapResult,
		CExpression **pexprRecheckResult,
		CExpressionArray *pdrspqexprResidualResult, BOOL isAPartialPredicate);

	// check if expression has any scalar node with ambiguous return type
	static BOOL FHasAmbiguousType(CExpression *pexpr, CMDAccessor *md_accessor);

	// construct a Bitmap(Dynamic)TableGet over BitmapBoolOp for the given
	// logical operator if bitmap indexes exist
	static CExpression *PexprBitmapTableGet(CMemoryPool *mp, CLogical *popGet,
											ULONG ulOriginOpId,
											CTableDescriptor *ptabdesc,
											CExpression *pexprScalar,
											CColRefSet *outer_refs,
											CColRefSet *pcrsReqd);

	// transform a Select over a (dynamic) table get into a bitmap table scan
	// over bitmap bool op
	static CExpression *PexprSelect2BitmapBoolOp(CMemoryPool *mp,
												 CExpression *pexpr);

	// compute a disjunction of two part constraints
	static CPartConstraint *PpartcnstrDisjunction(
		CMemoryPool *mp, CPartConstraint *ppartcnstrOld,
		CPartConstraint *ppartcnstrNew);

	// find a set of partial index combinations
	static SPartDynamicIndexGetInfoArrays *PdrspqdrspqpartdigCandidates(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpressionArray *pdrspqexprScalar, CColRef2dArray *pdrspqdrspqcrPartKey,
		const IMDRelation *pmdrel, CPartConstraint *ppartcnstrRel,
		CColRefArray *pdrspqcrOutput, CColRefSet *pcrsReqd,
		CColRefSet *pcrsScalarExpr,
		CColRefSet *
			pcrsAcceptedOuterRefs  // set of columns to be considered for index apply
	);

	// compute the newly covered part constraint based on the old covered part
	// constraint and the given part constraint
	static CPartConstraint *PpartcnstrUpdateCovered(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CExpressionArray *pdrspqexprScalar, CPartConstraint *ppartcnstrCovered,
		CPartConstraint *ppartcnstr, CColRefArray *pdrspqcrOutput,
		CExpressionArray *pdrspqexprIndex, CExpressionArray *pdrspqexprResidual,
		const IMDRelation *pmdrel, const IMDIndex *pmdindex,
		CColRefSet *pcrsAcceptedOuterRefs);

	// remap the expression from the old columns to the new ones
	static CExpression *PexprRemapColumns(CMemoryPool *mp,
										  CExpression *pexprScalar,
										  CColRefArray *pdrspqcrA,
										  CColRefArray *pdrspqcrRemappedA,
										  CColRefArray *pdrspqcrB,
										  CColRefArray *pdrspqcrRemappedB);

	// construct a partial dynamic index get
	static CExpression *PexprPartialDynamicIndexGet(
		CMemoryPool *mp, CLogicalDynamicGet *popGet, ULONG ulOriginOpId,
		CExpressionArray *pdrspqexprIndex, CExpressionArray *pdrspqexprResidual,
		CColRefArray *pdrspqcrDIG, const IMDIndex *pmdindex,
		const IMDRelation *pmdrel, CPartConstraint *ppartcnstr,
		CColRefSet *
			pcrsAcceptedOuterRefs,	// set of columns to be considered for index apply
		CColRefArray *pdrspqcrOuter, CColRefArray *pdrspqcrNewOuter);

	// create a new CTE consumer for the given CTE id
	static CExpression *PexprCTEConsumer(CMemoryPool *mp, ULONG ulCTEId,
										 CColRefArray *pdrspqcrConsumer);

	// return a new array containing the columns from the given column array 'colref_array'
	// at the positions indicated by the given ULONG array 'pdrspqulIndexesOfRefs'
	// e.g., colref_array = {col1, col2, col3}, pdrspqulIndexesOfRefs = {2, 1}
	// the result will be {col3, col2}
	static CColRefArray *PdrspqcrReorderedSubsequence(
		CMemoryPool *mp, CColRefArray *colref_array,
		ULongPtrArray *pdrspqulIndexesOfRefs);

	// check if given xform is an Agg splitting xform
	static BOOL FSplitAggXform(CXform::EXformId exfid);

	// check if given xform is an Agg CXformSplitDQA
	static BOOL FAggGenBySplitDQAXform(CExpression *pexprAgg);

	// check if given expression is a multi-stage Agg based on origin xform
	static BOOL FMultiStageAgg(CExpression *pexprAgg);

	static BOOL FLocalAggCreatedByEagerAggXform(CExpression *pexprAgg);

	// check if expression handle is attached to a Join with a predicate that uses columns from only one child
	static BOOL FJoinPredOnSingleChild(CMemoryPool *mp,
									   CExpressionHandle &exprhdl);

	// add a redundant SELECT node on top of Dynamic (Bitmap) IndexGet to be able to use index
	// predicate in partition elimination
	static CExpression *PexprRedundantSelectForDynamicIndex(CMemoryPool *mp,
															CExpression *pexpr);

	// convert an Agg window function into regular Agg
	static CExpression *PexprWinFuncAgg2ScalarAgg(CMemoryPool *mp,
												  CExpression *pexprWinFunc);

	// create a map from the argument of each Distinct Agg to the array of project elements that define Distinct Aggs on the same argument
	static void MapPrjElemsWithDistinctAggs(
		CMemoryPool *mp, CExpression *pexprPrjList,
		ExprToExprArrayMap **pphmexprdrspqexpr, ULONG *pulDifferentDQAs);

	// convert GbAgg with distinct aggregates to a join
	static CExpression *PexprGbAggOnCTEConsumer2Join(CMemoryPool *mp,
													 CExpression *pexprGbAgg);

	// combine the individual bitmap access paths to form a bitmap bool op expression
	static void JoinBitmapIndexProbes(CMemoryPool *pmp,
									  CExpressionArray *pdrspqexprBitmapOld,
									  CExpressionArray *pdrspqexprRecheckOld,
									  BOOL fConjunction,
									  CExpression **ppexprBitmap,
									  CExpression **ppexprRecheck);

	static CExpression *AddALinearStackOfUnaryExpressions(
		CMemoryPool *mp, CExpression *lowerPartOfExpr, CExpression *topOfStack,
		CExpression *exclusiveBottomOfStack);

};	// class CXformUtils


//---------------------------------------------------------------------------
//	@function:
//		CXformUtils::TransformImplementBinaryOp
//
//	@doc:
//		Helper function for implementation xforms on binary operators
//		with predicates (e.g. joins)
//
//---------------------------------------------------------------------------
template <class T>
void
CXformUtils::TransformImplementBinaryOp(CXformContext *pxfctxt,
										CXformResult *pxfres,
										CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pexpr);

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	// addref all children
	pexprLeft->AddRef();
	pexprRight->AddRef();
	pexprScalar->AddRef();

	// assemble physical operator
	CExpression *pexprBinary = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) T(mp), pexprLeft, pexprRight, pexprScalar);

#ifdef SPQOS_DEBUG
	COperator::EOperatorId op_id = pexprBinary->Pop()->Eopid();
#endif	// SPQOS_DEBUG
	SPQOS_ASSERT(COperator::EopPhysicalInnerNLJoin == op_id ||
				COperator::EopPhysicalLeftOuterNLJoin == op_id ||
				COperator::EopPhysicalLeftSemiNLJoin == op_id ||
				COperator::EopPhysicalLeftAntiSemiNLJoin == op_id ||
				COperator::EopPhysicalLeftAntiSemiNLJoinNotIn == op_id);

	// add alternative to results
	pxfres->Add(pexprBinary);
}

template <class T>
void
CXformUtils::AddHashOrMergeJoinAlternative(
	CMemoryPool *mp, CExpression *pexprJoin, CExpressionArray *pdrspqexprOuter,
	CExpressionArray *pdrspqexprInner, IMdIdArray *opfamilies,
	CXformResult *pxfres, BOOL is_hash_join_null_aware)
{
	SPQOS_ASSERT(CUtils::FLogicalJoin(pexprJoin->Pop()));
	SPQOS_ASSERT(3 == pexprJoin->Arity());
	SPQOS_ASSERT(NULL != pdrspqexprOuter);
	SPQOS_ASSERT(NULL != pdrspqexprInner);
	SPQOS_ASSERT(NULL != pxfres);

	for (ULONG ul = 0; ul < 3; ul++)
	{
		(*pexprJoin)[ul]->AddRef();
	}
	CLogicalJoin *popLogicalJoin = CLogicalJoin::PopConvert(pexprJoin->Pop());
	T *op =
		SPQOS_NEW(mp) T(mp, pdrspqexprOuter, pdrspqexprInner, opfamilies,
					   is_hash_join_null_aware, popLogicalJoin->OriginXform());
	CExpression *pexprResult = SPQOS_NEW(mp)
		CExpression(mp, op, (*pexprJoin)[0], (*pexprJoin)[1], (*pexprJoin)[2]);
	pxfres->Add(pexprResult);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformUtils::ImplementHashJoin
//
//	@doc:
//		Helper function for implementation of hash joins
//
//---------------------------------------------------------------------------
template <class T>
void
CXformUtils::ImplementHashJoin(CXformContext *pxfctxt, CXformResult *pxfres,
							   CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pxfctxt);

	// if there are outer references, then we cannot build a hash join
	if (CUtils::HasOuterRefs(pexpr))
	{
		return;
	}

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpressionArray *pdrspqexprOuter = NULL;
	CExpressionArray *pdrspqexprInner = NULL;
	IMdIdArray *join_opfamilies = NULL;

	// check if we have already computed hash join keys for the scalar child
	LookupJoinKeys(mp, pexpr, &pdrspqexprOuter, &pdrspqexprInner,
				   &join_opfamilies);

	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];
	CExpressionArray *pdrspqexpr =
		CCastUtils::PdrspqexprCastEquality(mp, pexprScalar);
	ULONG ulPreds = pdrspqexpr->Size();
	BOOL is_hash_join_null_aware = false;

	if (NULL != pdrspqexprOuter)
	{
		SPQOS_ASSERT(NULL != pdrspqexprInner);
		if (0 == pdrspqexprOuter->Size())
		{
			SPQOS_ASSERT(0 == pdrspqexprInner->Size());

			// we failed before to find hash join keys for scalar child,
			// no reason to try to do the same again
			pdrspqexprOuter->Release();
			pdrspqexprInner->Release();
			CRefCount::SafeRelease(join_opfamilies);
		}
		else
		{
			// we have computed hash join keys on scalar child before, reuse them

			for (ULONG ul = 0; ul < ulPreds; ul++)
			{
				CExpression *pexprPred = (*pdrspqexpr)[ul];
				if (!is_hash_join_null_aware)
				{
					is_hash_join_null_aware = CPredicateUtils::FINDF(pexprPred);
				}
			}
			AddHashOrMergeJoinAlternative<T>(mp, pexpr, pdrspqexprOuter,
											 pdrspqexprInner, join_opfamilies,
											 pxfres, is_hash_join_null_aware);
		}

		pdrspqexpr->Release();
		return;
	}

	// split the predicate into arrays of conjuncts based on if they are
	// output from inner or outer child
	pdrspqexprOuter = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprInner = SPQOS_NEW(mp) CExpressionArray(mp);

	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		join_opfamilies = SPQOS_NEW(mp) IMdIdArray(mp);
	}

	for (ULONG ul = 0; ul < ulPreds; ul++)
	{
		CExpression *pexprPred = (*pdrspqexpr)[ul];
		if (CPhysicalJoin::FHashJoinCompatible(pexprPred, pexprOuter,
											   pexprInner))
		{
			if (!is_hash_join_null_aware)
			{
				is_hash_join_null_aware = CPredicateUtils::FINDF(pexprPred);
			}

			CExpression *pexprPredInner;
			CExpression *pexprPredOuter;
			IMDId *mdid_scop;
			CPhysicalJoin::AlignJoinKeyOuterInner(pexprPred, pexprOuter,
												  pexprInner, &pexprPredOuter,
												  &pexprPredInner, &mdid_scop);

			pexprPredInner->AddRef();
			pexprPredOuter->AddRef();
			pdrspqexprOuter->Append(pexprPredOuter);
			pdrspqexprInner->Append(pexprPredInner);

			if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
			{
				CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();
				IMDId *hash_opfamily =
					mda->RetrieveScOp(mdid_scop)->HashOpfamilyMdid();
				SPQOS_ASSERT(NULL != hash_opfamily && hash_opfamily->IsValid());
				hash_opfamily->AddRef();
				join_opfamilies->Append(hash_opfamily);
			}
		}
	}
	SPQOS_ASSERT(pdrspqexprInner->Size() == pdrspqexprOuter->Size());
	SPQOS_ASSERT_IMP(SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution),
					pdrspqexprInner->Size() == join_opfamilies->Size());

	// construct new HashJoin expression using explicit casting, if needed
	pexpr->Pop()->AddRef();
	pexprOuter->AddRef();
	pexprInner->AddRef();
	CExpression *pexprResult = SPQOS_NEW(mp)
		CExpression(mp, pexpr->Pop(), pexprOuter, pexprInner,
					CPredicateUtils::PexprConjunction(mp, pdrspqexpr));

	// cache hash join keys on scalar child group
	CacheJoinKeys(pexprResult, pdrspqexprOuter, pdrspqexprInner, join_opfamilies);

	// Add an alternative only if we found at least one hash-joinable predicate
	if (0 != pdrspqexprOuter->Size())
	{
		AddHashOrMergeJoinAlternative<T>(mp, pexprResult, pdrspqexprOuter,
										 pdrspqexprInner, join_opfamilies,
										 pxfres, is_hash_join_null_aware);
	}
	else
	{
		// clean up
		pdrspqexprOuter->Release();
		pdrspqexprInner->Release();
		CRefCount::SafeRelease(join_opfamilies);
	}

	pexprResult->Release();
}

template <class T>
void
CXformUtils::ImplementMergeJoin(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pxfctxt);

	// if there are outer references, then we cannot build a merge join
	if (CUtils::HasOuterRefs(pexpr))
	{
		return;
	}

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpressionArray *pdrspqexprOuter = NULL;
	CExpressionArray *pdrspqexprInner = NULL;
	IMdIdArray *join_opfamilies = NULL;

	// check if we have already computed join keys for the scalar child
	LookupJoinKeys(mp, pexpr, &pdrspqexprOuter, &pdrspqexprInner,
				   &join_opfamilies);
	if (NULL != pdrspqexprOuter)
	{
		SPQOS_ASSERT(NULL != pdrspqexprInner);
		if (0 == pdrspqexprOuter->Size())
		{
			SPQOS_ASSERT(0 == pdrspqexprInner->Size());

			// we failed before to find join keys for scalar child,
			// no reason to try to do the same again
			pdrspqexprOuter->Release();
			pdrspqexprInner->Release();
			CRefCount::SafeRelease(join_opfamilies);
		}
		else
		{
			// we have computed join keys on scalar child before, reuse them
			AddHashOrMergeJoinAlternative<T>(
				mp, pexpr, pdrspqexprOuter, pdrspqexprInner, join_opfamilies,
				pxfres, true /*is_hash_join_null_aware*/);
		}

		return;
	}

	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	// split the predicate into arrays of conjuncts based on if they are
	// output from inner or outer child
	pdrspqexprOuter = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprInner = SPQOS_NEW(mp) CExpressionArray(mp);

	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		join_opfamilies = SPQOS_NEW(mp) IMdIdArray(mp);
	}

	CExpressionArray *pdrspqexpr =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);
	ULONG ulPreds = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < ulPreds; ul++)
	{
		CExpression *pexprPred = (*pdrspqexpr)[ul];
		if (CPhysicalJoin::FMergeJoinCompatible(pexprPred, pexprOuter,
												pexprInner))
		{
			CExpression *pexprPredInner;
			CExpression *pexprPredOuter;
			IMDId *mdid_scop;
			CPhysicalJoin::AlignJoinKeyOuterInner(pexprPred, pexprOuter,
												  pexprInner, &pexprPredOuter,
												  &pexprPredInner, &mdid_scop);

			pexprPredInner->AddRef();
			pexprPredOuter->AddRef();
			pdrspqexprOuter->Append(pexprPredOuter);
			pdrspqexprInner->Append(pexprPredInner);

			if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
			{
				CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();
				IMDId *hash_opfamily =
					mda->RetrieveScOp(mdid_scop)->HashOpfamilyMdid();
				SPQOS_ASSERT(NULL != hash_opfamily && hash_opfamily->IsValid());
				hash_opfamily->AddRef();
				join_opfamilies->Append(hash_opfamily);
			}
		}
		else
		{
			// In case of FULL merge joins, all the merge clauses must be merge
			// compatible or we cannot create a merge join.
			pdrspqexpr->Release();
			pdrspqexprOuter->Release();
			pdrspqexprInner->Release();
			CRefCount::SafeRelease(join_opfamilies);
			return;
		}
	}
	SPQOS_ASSERT(pdrspqexprInner->Size() == pdrspqexprOuter->Size());
	SPQOS_ASSERT_IMP(SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution),
					pdrspqexprInner->Size() == join_opfamilies->Size());

	// construct new MergeJoin expression using explicit casting, if needed
	pexpr->Pop()->AddRef();
	pexprOuter->AddRef();
	pexprInner->AddRef();
	CExpression *pexprResult = SPQOS_NEW(mp)
		CExpression(mp, pexpr->Pop(), pexprOuter, pexprInner,
					CPredicateUtils::PexprConjunction(mp, pdrspqexpr));

	// cache hash join keys on scalar child group
	CacheJoinKeys(pexprResult, pdrspqexprOuter, pdrspqexprInner, join_opfamilies);

	// Add an alternative only if we found at least one merge-joinable predicate
	if (0 != pdrspqexprOuter->Size())
	{
		AddHashOrMergeJoinAlternative<T>(
			mp, pexprResult, pdrspqexprOuter, pdrspqexprInner, join_opfamilies,
			pxfres, true /*is_hash_join_null_aware*/);
	}
	else
	{
		// clean up
		pdrspqexprOuter->Release();
		pdrspqexprInner->Release();
	}
	CRefCount::SafeRelease(join_opfamilies);

	pexprResult->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CXformUtils::ImplementNLJoin
//
//	@doc:
//		Helper function for implementation of nested loops joins
//
//---------------------------------------------------------------------------
template <class T>
void
CXformUtils::ImplementNLJoin(CXformContext *pxfctxt, CXformResult *pxfres,
							 CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pxfctxt);

	CMemoryPool *mp = pxfctxt->Pmp();

	CColRefArray *pdrspqcrOuter = SPQOS_NEW(mp) CColRefArray(mp);
	CColRefArray *pdrspqcrInner = SPQOS_NEW(mp) CColRefArray(mp);

	TransformImplementBinaryOp<T>(pxfctxt, pxfres, pexpr);

	// clean up
	pdrspqcrOuter->Release();
	pdrspqcrInner->Release();
}
}  // namespace spqopt

#endif	// !SPQOPT_CXformUtils_H

// EOF
