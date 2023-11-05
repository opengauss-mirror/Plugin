//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		dxltokens.cpp
//
//	@doc:
//		DXL token constants in the ORCA CWStringConst and Xerces XMLCh format.
//		Constants are initialized in the CDXLTokens::Init function
//		invoked during initialization of the library (init.cpp),
//		and destroyed in CDXLTokens::Terminate when the library is unloaded
//
//---------------------------------------------------------------------------

#include "knl/knl_session.h"

#include "naucrates/dxl/xml/dxltokens.h"

#include "naucrates/dxl/xml/CDXLMemoryManager.h"

using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLTokens::Init
//
//	@doc:
//		Initialize the constants representing the DXL tokens.
//
//---------------------------------------------------------------------------
void
CDXLTokens::Init(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL == u_sess->spq_cxt.m_dxl_memory_manager);
	SPQOS_ASSERT(NULL == u_sess->spq_cxt.m_mp);

	u_sess->spq_cxt.m_mp = mp;

	u_sess->spq_cxt.m_dxl_memory_manager = SPQOS_NEW(u_sess->spq_cxt.m_mp) CDXLMemoryManager(u_sess->spq_cxt.m_mp);

	SWszMapElem rgStrMap[] = {
		{EdxltokenDXLMessage, SPQOS_WSZ_LIT("DXLMessage")},
		{EdxltokenComment, SPQOS_WSZ_LIT("Comment")},
		{EdxltokenPlan, SPQOS_WSZ_LIT("Plan")},
		{EdxltokenPlanId, SPQOS_WSZ_LIT("Id")},
		{EdxltokenPlanSpaceSize, SPQOS_WSZ_LIT("SpaceSize")},
		{EdxltokenSamplePlans, SPQOS_WSZ_LIT("SamplePlans")},
		{EdxltokenSamplePlan, SPQOS_WSZ_LIT("SamplePlan")},
		{EdxltokenCostDistr, SPQOS_WSZ_LIT("CostDistribution")},
		{EdxltokenRelativeCost, SPQOS_WSZ_LIT("RelativeCost")},
		{EdxltokenX, SPQOS_WSZ_LIT("X")},
		{EdxltokenY, SPQOS_WSZ_LIT("Y")},

		{EdxltokenOptimizerConfig, SPQOS_WSZ_LIT("OptimizerConfig")},
		{EdxltokenEnumeratorConfig, SPQOS_WSZ_LIT("EnumeratorConfig")},
		{EdxltokenStatisticsConfig, SPQOS_WSZ_LIT("StatisticsConfig")},
		{EdxltokenDampingFactorFilter, SPQOS_WSZ_LIT("DampingFactorFilter")},
		{EdxltokenDampingFactorJoin, SPQOS_WSZ_LIT("DampingFactorJoin")},
		{EdxltokenDampingFactorGroupBy, SPQOS_WSZ_LIT("DampingFactorGroupBy")},
		{EdxltokenMaxStatsBuckets, SPQOS_WSZ_LIT("MaxStatsBuckets")},
		{EdxltokenCTEConfig, SPQOS_WSZ_LIT("CTEConfig")},
		{EdxltokenCTEInliningCutoff, SPQOS_WSZ_LIT("CTEInliningCutoff")},
		{EdxltokenCostModelConfig, SPQOS_WSZ_LIT("CostModelConfig")},
		{EdxltokenCostModelType, SPQOS_WSZ_LIT("CostModelType")},
		{EdxltokenSegmentsForCosting, SPQOS_WSZ_LIT("SegmentsForCosting")},
		{EdxltokenHint, SPQOS_WSZ_LIT("Hint")},
		{EdxltokenJoinArityForAssociativityCommutativity,
		 SPQOS_WSZ_LIT("JoinArityForAssociativityCommutativity")},
		{EdxltokenArrayExpansionThreshold,
		 SPQOS_WSZ_LIT("ArrayExpansionThreshold")},
		{EdxltokenJoinOrderDPThreshold,
		 SPQOS_WSZ_LIT("JoinOrderDynamicProgThreshold")},
		{EdxltokenBroadcastThreshold, SPQOS_WSZ_LIT("BroadcastThreshold")},
		{EdxltokenEnforceConstraintsOnDML,
		 SPQOS_WSZ_LIT("EnforceConstraintsOnDML")},
		{EdxltokenPushGroupByBelowSetopThreshold,
		 SPQOS_WSZ_LIT("PushGroupByBelowSetopThreshold")},
		{EdxltokenXformBindThreshold, SPQOS_WSZ_LIT("XformBindThreshold")},
		{EdxltokenSkewFactor, SPQOS_WSZ_LIT("SkewFactor")},
		{EdxltokenWindowOids, SPQOS_WSZ_LIT("WindowOids")},
		{EdxltokenOidRowNumber, SPQOS_WSZ_LIT("RowNumber")},
		{EdxltokenOidRank, SPQOS_WSZ_LIT("Rank")},

		{EdxltokenPlanSamples, SPQOS_WSZ_LIT("PlanSamples")},

		{EdxltokenMetadata, SPQOS_WSZ_LIT("Metadata")},
		{EdxltokenTraceFlags, SPQOS_WSZ_LIT("TraceFlags")},
		{EdxltokenMDRequest, SPQOS_WSZ_LIT("MDRequest")},

		{EdxltokenSysids, SPQOS_WSZ_LIT("SystemIds")},
		{EdxltokenSysid, SPQOS_WSZ_LIT("SystemId")},

		{EdxltokenThread, SPQOS_WSZ_LIT("Thread")},

		{EdxltokenPhysical, SPQOS_WSZ_LIT("OpPhysical")},

		{EdxltokenPhysicalTableScan, SPQOS_WSZ_LIT("TableScan")},
        /* SPQ: for ShareScan */
        {EdxltokenPhysicalTableShareScan, SPQOS_WSZ_LIT("TableShareScan")},
		{EdxltokenPhysicalBitmapTableScan, SPQOS_WSZ_LIT("BitmapTableScan")},
		{EdxltokenPhysicalDynamicBitmapTableScan,
		 SPQOS_WSZ_LIT("DynamicBitmapTableScan")},
		{EdxltokenPhysicalExternalScan, SPQOS_WSZ_LIT("ExternalScan")},
		{EdxltokenPhysicalIndexScan, SPQOS_WSZ_LIT("IndexScan")},
		{EdxltokenPhysicalIndexOnlyScan, SPQOS_WSZ_LIT("IndexOnlyScan")},
        /* SPQ: for shareindexscan */
        {EdxltokenPhysicalShareIndexScan, SPQOS_WSZ_LIT("ShareIndexScan")},
		{EdxltokenScalarBitmapIndexProbe, SPQOS_WSZ_LIT("BitmapIndexProbe")},
		{EdxltokenPhysicalHashJoin, SPQOS_WSZ_LIT("HashJoin")},
		{EdxltokenPhysicalNLJoin, SPQOS_WSZ_LIT("NestedLoopJoin")},
		{EdxltokenPhysicalNLJoinIndex, SPQOS_WSZ_LIT("IndexNestedLoopJoin")},
		{EdxltokenPhysicalMergeJoin, SPQOS_WSZ_LIT("MergeJoin")},
		{EdxltokenPhysicalGatherMotion, SPQOS_WSZ_LIT("GatherMotion")},
		{EdxltokenPhysicalBroadcastMotion, SPQOS_WSZ_LIT("BroadcastMotion")},
		{EdxltokenPhysicalRedistributeMotion,
		 SPQOS_WSZ_LIT("RedistributeMotion")},
		{EdxltokenPhysicalRoutedDistributeMotion,
		 SPQOS_WSZ_LIT("RoutedDistributeMotion")},
		{EdxltokenPhysicalRandomMotion, SPQOS_WSZ_LIT("RandomMotion")},
		{EdxltokenPhysicalLimit, SPQOS_WSZ_LIT("Limit")},
		{EdxltokenPhysicalSort, SPQOS_WSZ_LIT("Sort")},
		{EdxltokenPhysicalAggregate, SPQOS_WSZ_LIT("Aggregate")},
		{EdxltokenPhysicalSubqueryScan, SPQOS_WSZ_LIT("SubqueryScan")},
		{EdxltokenPhysicalResult, SPQOS_WSZ_LIT("Result")},
		{EdxltokenPhysicalValuesScan, SPQOS_WSZ_LIT("Values")},
		{EdxltokenPhysicalAppend, SPQOS_WSZ_LIT("Append")},
		{EdxltokenPhysicalMaterialize, SPQOS_WSZ_LIT("Materialize")},
		{EdxltokenPhysicalSequence, SPQOS_WSZ_LIT("Sequence")},
		{EdxltokenPhysicalDynamicTableScan, SPQOS_WSZ_LIT("DynamicTableScan")},
		{EdxltokenPhysicalDynamicIndexScan, SPQOS_WSZ_LIT("DynamicIndexScan")},
		{EdxltokenPhysicalTVF, SPQOS_WSZ_LIT("TableValuedFunction")},
		{EdxltokenPhysicalWindow, SPQOS_WSZ_LIT("Window")},
		{EdxltokenPhysicalDMLInsert, SPQOS_WSZ_LIT("DMLInsert")},
		{EdxltokenPhysicalDMLDelete, SPQOS_WSZ_LIT("DMLDelete")},
		{EdxltokenPhysicalDMLUpdate, SPQOS_WSZ_LIT("DMLUpdate")},
		{EdxltokenDirectDispatchInfo, SPQOS_WSZ_LIT("DirectDispatchInfo")},
		{EdxltokenDirectDispatchIsRaw, SPQOS_WSZ_LIT("IsRaw")},
		{EdxltokenDirectDispatchKeyValue, SPQOS_WSZ_LIT("KeyValue")},

		{EdxltokenPhysicalPartitionSelector, SPQOS_WSZ_LIT("PartitionSelector")},
		{EdxltokenPhysicalPartitionSelectorLevels,
		 SPQOS_WSZ_LIT("PartitionLevels")},
		{EdxltokenPhysicalPartitionSelectorScanId, SPQOS_WSZ_LIT("ScanId")},
		{EdxltokenPhysicalSplit, SPQOS_WSZ_LIT("Split")},
		{EdxltokenPhysicalRowTrigger, SPQOS_WSZ_LIT("RowTrigger")},
		{EdxltokenPhysicalAssert, SPQOS_WSZ_LIT("Assert")},
		{EdxltokenPhysicalCTEProducer, SPQOS_WSZ_LIT("CTEProducer")},
		{EdxltokenPhysicalCTEConsumer, SPQOS_WSZ_LIT("CTEConsumer")},

		{EdxltokenErrorCode, SPQOS_WSZ_LIT("ErrorCode")},
		{EdxltokenErrorMessage, SPQOS_WSZ_LIT("ErrorMessage")},

		{EdxltokenOnCommitAction, SPQOS_WSZ_LIT("OnCommitAction")},
		{EdxltokenOnCommitNOOP, SPQOS_WSZ_LIT("NOOP")},
		{EdxltokenOnCommitPreserve, SPQOS_WSZ_LIT("PreserveRows")},
		{EdxltokenOnCommitDelete, SPQOS_WSZ_LIT("DeleteRows")},
		{EdxltokenOnCommitDrop, SPQOS_WSZ_LIT("Drop")},

		{EdxltokenDuplicateSensitive, SPQOS_WSZ_LIT("DuplicateSensitive")},

		{EdxltokenPartIndexId, SPQOS_WSZ_LIT("PartIndexId")},
		{EdxltokenPartIndexIdPrintable, SPQOS_WSZ_LIT("PrintablePartIndexId")},
		{EdxltokenSegmentIdCol, SPQOS_WSZ_LIT("SegmentIdCol")},

		{EdxltokenScalar, SPQOS_WSZ_LIT("Scalar")},
		{EdxltokenScalarExpr, SPQOS_WSZ_LIT("ScalarExpr")},

		{EdxltokenScalarProjList, SPQOS_WSZ_LIT("ProjList")},
		{EdxltokenScalarFilter, SPQOS_WSZ_LIT("Filter")},
		{EdxltokenScalarAggref, SPQOS_WSZ_LIT("AggFunc")},
		{EdxltokenScalarWindowref, SPQOS_WSZ_LIT("WindowFunc")},
		{EdxltokenScalarArrayComp, SPQOS_WSZ_LIT("ArrayComp")},
		{EdxltokenScalarBoolTestIsTrue, SPQOS_WSZ_LIT("IsTrue")},
		{EdxltokenScalarBoolTestIsNotTrue, SPQOS_WSZ_LIT("IsNotTrue")},
		{EdxltokenScalarBoolTestIsFalse, SPQOS_WSZ_LIT("IsFalse")},
		{EdxltokenScalarBoolTestIsNotFalse, SPQOS_WSZ_LIT("IsNotFalse")},
		{EdxltokenScalarBoolTestIsUnknown, SPQOS_WSZ_LIT("IsUnknown")},
		{EdxltokenScalarBoolTestIsNotUnknown, SPQOS_WSZ_LIT("IsNotUnknown")},
		{EdxltokenScalarBoolAnd, SPQOS_WSZ_LIT("And")},
		{EdxltokenScalarBoolOr, SPQOS_WSZ_LIT("Or")},
		{EdxltokenScalarBoolNot, SPQOS_WSZ_LIT("Not")},
		{EdxltokenScalarMin, SPQOS_WSZ_LIT("Minimum")},
		{EdxltokenScalarMax, SPQOS_WSZ_LIT("Maximum")},
		{EdxltokenScalarCoalesce, SPQOS_WSZ_LIT("Coalesce")},
		{EdxltokenScalarComp, SPQOS_WSZ_LIT("Comparison")},
		{EdxltokenScalarConstValue, SPQOS_WSZ_LIT("ConstValue")},
		{EdxltokenScalarDistinctComp, SPQOS_WSZ_LIT("IsDistinctFrom")},
		{EdxltokenScalarFuncExpr, SPQOS_WSZ_LIT("FuncExpr")},
		{EdxltokenScalarIsNull, SPQOS_WSZ_LIT("IsNull")},
		{EdxltokenScalarIsNotNull, SPQOS_WSZ_LIT("IsNotNull")},
		{EdxltokenScalarNullIf, SPQOS_WSZ_LIT("NullIf")},
		{EdxltokenScalarHashCondList, SPQOS_WSZ_LIT("HashCondList")},
		{EdxltokenScalarMergeCondList, SPQOS_WSZ_LIT("MergeCondList")},
		{EdxltokenScalarHashExprList, SPQOS_WSZ_LIT("HashExprList")},
		{EdxltokenScalarHashExpr, SPQOS_WSZ_LIT("HashExpr")},
		{EdxltokenScalarIdent, SPQOS_WSZ_LIT("Ident")},
		{EdxltokenScalarIfStmt, SPQOS_WSZ_LIT("If")},
		{EdxltokenScalarSwitch, SPQOS_WSZ_LIT("Switch")},
		{EdxltokenScalarSwitchCase, SPQOS_WSZ_LIT("SwitchCase")},
		{EdxltokenScalarCaseTest, SPQOS_WSZ_LIT("CaseTest")},
		{EdxltokenScalarSubPlan, SPQOS_WSZ_LIT("SubPlan")},
		{EdxltokenScalarJoinFilter, SPQOS_WSZ_LIT("JoinFilter")},
		{EdxltokenScalarRecheckCondFilter, SPQOS_WSZ_LIT("RecheckCond")},
		{EdxltokenScalarLimitCount, SPQOS_WSZ_LIT("LimitCount")},
		{EdxltokenScalarLimitOffset, SPQOS_WSZ_LIT("LimitOffset")},
		{EdxltokenScalarOneTimeFilter, SPQOS_WSZ_LIT("OneTimeFilter")},
		{EdxltokenScalarOpExpr, SPQOS_WSZ_LIT("OpExpr")},
		{EdxltokenScalarProjElem, SPQOS_WSZ_LIT("ProjElem")},
		{EdxltokenScalarCast, SPQOS_WSZ_LIT("Cast")},
		{EdxltokenScalarCoerceToDomain, SPQOS_WSZ_LIT("CoerceToDomain")},
		{EdxltokenScalarCoerceViaIO, SPQOS_WSZ_LIT("CoerceViaIO")},
		{EdxltokenScalarArrayCoerceExpr, SPQOS_WSZ_LIT("ArrayCoerceExpr")},
		{EdxltokenScalarSortCol, SPQOS_WSZ_LIT("SortingColumn")},
		{EdxltokenScalarSortColList, SPQOS_WSZ_LIT("SortingColumnList")},
		{EdxltokenScalarGroupingColList, SPQOS_WSZ_LIT("GroupingColumns")},
		{EdxltokenScalarSortGroupClause, SPQOS_WSZ_LIT("SortGroupClause")},

		{EdxltokenScalarBitmapAnd, SPQOS_WSZ_LIT("BitmapAnd")},
		{EdxltokenScalarBitmapOr, SPQOS_WSZ_LIT("BitmapOr")},

		{EdxltokenScalarArray, SPQOS_WSZ_LIT("Array")},
		{EdxltokenScalarArrayRef, SPQOS_WSZ_LIT("ArrayRef")},
		{EdxltokenScalarArrayRefIndexList, SPQOS_WSZ_LIT("ArrayIndexList")},
		{EdxltokenScalarArrayRefIndexListBound, SPQOS_WSZ_LIT("Bound")},
		{EdxltokenScalarArrayRefIndexListLower, SPQOS_WSZ_LIT("Lower")},
		{EdxltokenScalarArrayRefIndexListUpper, SPQOS_WSZ_LIT("Upper")},
		{EdxltokenScalarArrayRefExpr, SPQOS_WSZ_LIT("RefExpr")},
		{EdxltokenScalarArrayRefAssignExpr, SPQOS_WSZ_LIT("AssignExpr")},

		{EdxltokenScalarAssertConstraintList,
		 SPQOS_WSZ_LIT("AssertConstraintList")},
		{EdxltokenScalarAssertConstraint, SPQOS_WSZ_LIT("AssertConstraint")},

		{EdxltokenScalarSubquery, SPQOS_WSZ_LIT("ScalarSubquery")},
		{EdxltokenScalarSubqueryAny, SPQOS_WSZ_LIT("SubqueryAny")},
		{EdxltokenScalarSubqueryAll, SPQOS_WSZ_LIT("SubqueryAll")},
		{EdxltokenScalarSubqueryExists, SPQOS_WSZ_LIT("SubqueryExists")},
		{EdxltokenScalarSubqueryNotExists, SPQOS_WSZ_LIT("SubqueryNotExists")},

		{EdxltokenScalarDMLAction, SPQOS_WSZ_LIT("DMLAction")},
		{EdxltokenScalarOpList, SPQOS_WSZ_LIT("ScalarOpList")},

		{EdxltokenScalarSubPlanType, SPQOS_WSZ_LIT("SubPlanType")},
		{EdxltokenScalarSubPlanTypeScalar, SPQOS_WSZ_LIT("ScalarSubPlan")},
		{EdxltokenScalarSubPlanTypeExists, SPQOS_WSZ_LIT("ExistsSubPlan")},
		{EdxltokenScalarSubPlanTypeNotExists, SPQOS_WSZ_LIT("NotExistsSubPlan")},
		{EdxltokenScalarSubPlanTypeAny, SPQOS_WSZ_LIT("AnySubPlan")},
		{EdxltokenScalarSubPlanTypeAll, SPQOS_WSZ_LIT("AllSubPlan")},

		{EdxltokenPartLevelEqFilterList, SPQOS_WSZ_LIT("PartEqFilters")},
		{EdxltokenPartLevelEqFilterElemList, SPQOS_WSZ_LIT("PartEqFilterElems")},
		{EdxltokenPartLevelFilterList, SPQOS_WSZ_LIT("PartFilters")},
		{EdxltokenPartLevel, SPQOS_WSZ_LIT("Level")},
		{EdxltokenScalarPartDefault, SPQOS_WSZ_LIT("DefaultPart")},
		{EdxltokenScalarPartBound, SPQOS_WSZ_LIT("PartBound")},
		{EdxltokenScalarPartBoundLower, SPQOS_WSZ_LIT("LowerBound")},
		{EdxltokenScalarPartBoundInclusion, SPQOS_WSZ_LIT("PartBoundInclusion")},
		{EdxltokenScalarPartBoundOpen, SPQOS_WSZ_LIT("PartBoundOpen")},
		{EdxltokenScalarPartListValues, SPQOS_WSZ_LIT("PartListValues")},
		{EdxltokenScalarPartListNullTest, SPQOS_WSZ_LIT("PartListNullTest")},
		{EdxltokenScalarResidualFilter, SPQOS_WSZ_LIT("ResidualFilter")},
		{EdxltokenScalarPropagationExpr, SPQOS_WSZ_LIT("PropagationExpression")},
		{EdxltokenScalarPrintableFilter, SPQOS_WSZ_LIT("PrintableFilter")},

		{EdxltokenScalarSubPlanParamList, SPQOS_WSZ_LIT("ParamList")},
		{EdxltokenScalarSubPlanParam, SPQOS_WSZ_LIT("Param")},
		{EdxltokenScalarSubPlanTestExpr, SPQOS_WSZ_LIT("TestExpr")},
		{EdxltokenScalarValuesList, SPQOS_WSZ_LIT("ValuesList")},

		{EdxltokenValue, SPQOS_WSZ_LIT("Value")},
		{EdxltokenTypeId, SPQOS_WSZ_LIT("TypeMdid")},
		{EdxltokenTypeIds, SPQOS_WSZ_LIT("TypeMdids")},

		{EdxltokenConstTuple, SPQOS_WSZ_LIT("ConstTuple")},
		{EdxltokenDatum, SPQOS_WSZ_LIT("Datum")},

		{EdxltokenTypeMod, SPQOS_WSZ_LIT("TypeModifier")},
		{EdxltokenCoercionForm, SPQOS_WSZ_LIT("CoercionForm")},
		{EdxltokenLocation, SPQOS_WSZ_LIT("Location")},
		{EdxltokenElementFunc, SPQOS_WSZ_LIT("ElementFunc")},
		{EdxltokenIsExplicit, SPQOS_WSZ_LIT("IsExplicit")},

		{EdxltokenJoinType, SPQOS_WSZ_LIT("JoinType")},
		{EdxltokenJoinInner, SPQOS_WSZ_LIT("Inner")},
		{EdxltokenJoinLeft, SPQOS_WSZ_LIT("Left")},
		{EdxltokenJoinFull, SPQOS_WSZ_LIT("Full")},
		{EdxltokenJoinRight, SPQOS_WSZ_LIT("Right")},
		{EdxltokenJoinIn, SPQOS_WSZ_LIT("In")},
		{EdxltokenJoinLeftAntiSemiJoin, SPQOS_WSZ_LIT("LeftAntiSemiJoin")},
		{EdxltokenJoinLeftAntiSemiJoinNotIn,
		 SPQOS_WSZ_LIT("LeftAntiSemiJoinNotIn")},

		{EdxltokenMergeJoinUniqueOuter, SPQOS_WSZ_LIT("UniqueOuter")},

		{EdxltokenWindowLeadingBoundary, SPQOS_WSZ_LIT("LeadingBoundary")},
		{EdxltokenWindowTrailingBoundary, SPQOS_WSZ_LIT("TrailingBoundary")},
		{EdxltokenWindowBoundaryUnboundedPreceding,
		 SPQOS_WSZ_LIT("UnboundedPreceding")},
		{EdxltokenWindowBoundaryBoundedPreceding,
		 SPQOS_WSZ_LIT("BoundedPreceding")},
		{EdxltokenWindowBoundaryCurrentRow, SPQOS_WSZ_LIT("CurrentRow")},
		{EdxltokenWindowBoundaryUnboundedFollowing,
		 SPQOS_WSZ_LIT("UnboundedFollowing")},
		{EdxltokenWindowBoundaryBoundedFollowing,
		 SPQOS_WSZ_LIT("BoundedFollowing")},
		{EdxltokenWindowBoundaryDelayedBoundedPreceding,
		 SPQOS_WSZ_LIT("DelayedBoundedPreceding")},
		{EdxltokenWindowBoundaryDelayedBoundedFollowing,
		 SPQOS_WSZ_LIT("DelayedBoundedFollowing")},

		{EdxltokenWindowFrameSpec, SPQOS_WSZ_LIT("FrameSpec")},
		{EdxltokenScalarWindowFrameLeadingEdge, SPQOS_WSZ_LIT("LeadingEdge")},
		{EdxltokenScalarWindowFrameTrailingEdge, SPQOS_WSZ_LIT("TrailingEdge")},
		{EdxltokenWindowFSRow, SPQOS_WSZ_LIT("Row")},
		{EdxltokenWindowFSRange, SPQOS_WSZ_LIT("Range")},

		{EdxltokenWindowExclusionStrategy, SPQOS_WSZ_LIT("ExclusionStrategy")},
		{EdxltokenWindowESNone, SPQOS_WSZ_LIT("None")},
		{EdxltokenWindowESNulls, SPQOS_WSZ_LIT("Nulls")},
		{EdxltokenWindowESCurrentRow, SPQOS_WSZ_LIT("CurrentRow")},
		{EdxltokenWindowESGroup, SPQOS_WSZ_LIT("Group")},
		{EdxltokenWindowESTies, SPQOS_WSZ_LIT("Ties")},

		{EdxltokenWindowFrame, SPQOS_WSZ_LIT("WindowFrame")},
		{EdxltokenWindowKeyList, SPQOS_WSZ_LIT("WindowKeyList")},
		{EdxltokenWindowKey, SPQOS_WSZ_LIT("WindowKey")},

		{EdxltokenWindowSpecList, SPQOS_WSZ_LIT("WindowSpecList")},
		{EdxltokenWindowSpec, SPQOS_WSZ_LIT("WindowSpec")},

		{EdxltokenWindowrefOid, SPQOS_WSZ_LIT("Mdid")},
		{EdxltokenWindowrefDistinct, SPQOS_WSZ_LIT("Distinct")},
		{EdxltokenWindowrefStarArg, SPQOS_WSZ_LIT("WindowStarArg")},
		{EdxltokenWindowrefSimpleAgg, SPQOS_WSZ_LIT("WindowSimpleAgg")},
		{EdxltokenWindowrefStrategy, SPQOS_WSZ_LIT("WindowStrategy")},
		{EdxltokenWindowrefStageImmediate, SPQOS_WSZ_LIT("Immediate")},
		{EdxltokenWindowrefStagePreliminary, SPQOS_WSZ_LIT("Preliminary")},
		{EdxltokenWindowrefStageRowKey, SPQOS_WSZ_LIT("RowKey")},
		{EdxltokenWindowrefWinSpecPos, SPQOS_WSZ_LIT("WinSpecPos")},

		{EdxltokenAggStrategy, SPQOS_WSZ_LIT("AggregationStrategy")},
		{EdxltokenAggStrategyPlain, SPQOS_WSZ_LIT("Plain")},
		{EdxltokenAggStrategySorted, SPQOS_WSZ_LIT("Sorted")},
		{EdxltokenAggStrategyHashed, SPQOS_WSZ_LIT("Hashed")},

		{EdxltokenAggStreamSafe, SPQOS_WSZ_LIT("StreamSafe")},

		{EdxltokenAggrefOid, SPQOS_WSZ_LIT("AggMdid")},
		{EdxltokenAggrefDistinct, SPQOS_WSZ_LIT("AggDistinct")},
		{EdxltokenAggrefKind, SPQOS_WSZ_LIT("AggKind")},
		{EdxltokenAggrefGpAggOid, SPQOS_WSZ_LIT("GpAggMdid")},
		{EdxltokenAggrefStage, SPQOS_WSZ_LIT("AggStage")},
		{EdxltokenAggrefLookups, SPQOS_WSZ_LIT("AggLookups")},
		{EdxltokenAggrefStageNormal, SPQOS_WSZ_LIT("Normal")},
		{EdxltokenAggrefStagePartial, SPQOS_WSZ_LIT("Partial")},
		{EdxltokenAggrefStageIntermediate, SPQOS_WSZ_LIT("Intermediate")},
		{EdxltokenAggrefStageFinal, SPQOS_WSZ_LIT("Final")},
		{EdxltokenAggrefKindNormal, SPQOS_WSZ_LIT("n")},
		{EdxltokenAggrefKindOrderedSet, SPQOS_WSZ_LIT("o")},
		{EdxltokenAggrefKindHypothetical, SPQOS_WSZ_LIT("h")},

		{EdxltokenArrayType, SPQOS_WSZ_LIT("ArrayType")},
		{EdxltokenArrayElementType, SPQOS_WSZ_LIT("ElementType")},
		{EdxltokenArrayMultiDim, SPQOS_WSZ_LIT("MultiDimensional")},

		{EdxltokenTableDescr, SPQOS_WSZ_LIT("TableDescriptor")},
		{EdxltokenProperties, SPQOS_WSZ_LIT("Properties")},
		{EdxltokenOutputCols, SPQOS_WSZ_LIT("OutputColumns")},
		{EdxltokenCost, SPQOS_WSZ_LIT("Cost")},
		{EdxltokenStartupCost, SPQOS_WSZ_LIT("StartupCost")},
		{EdxltokenTotalCost, SPQOS_WSZ_LIT("TotalCost")},
		{EdxltokenRows, SPQOS_WSZ_LIT("Rows")},
		{EdxltokenWidth, SPQOS_WSZ_LIT("Width")},
		{EdxltokenRelPages, SPQOS_WSZ_LIT("RelPages")},
		{EdxltokenRelAllVisible, SPQOS_WSZ_LIT("RelAllVisible")},
		{EdxltokenTableName, SPQOS_WSZ_LIT("TableName")},
		{EdxltokenDerivedTableName, SPQOS_WSZ_LIT("DerivedTableName")},
		{EdxltokenExecuteAsUser, SPQOS_WSZ_LIT("ExecuteAsUser")},

		{EdxltokenCTASOptions, SPQOS_WSZ_LIT("CTASOptions")},
		{EdxltokenCTASOption, SPQOS_WSZ_LIT("CTASOption")},

		{EdxltokenColDescr, SPQOS_WSZ_LIT("Column")},
		{EdxltokenColRef, SPQOS_WSZ_LIT("ColRef")},

		{EdxltokenColumns, SPQOS_WSZ_LIT("Columns")},
		{EdxltokenColumn, SPQOS_WSZ_LIT("Column")},
		{EdxltokenColName, SPQOS_WSZ_LIT("ColName")},
		{EdxltokenTagColType, SPQOS_WSZ_LIT("ColType")},
		{EdxltokenColId, SPQOS_WSZ_LIT("ColId")},
		{EdxltokenAttno, SPQOS_WSZ_LIT("Attno")},
		{EdxltokenColDropped, SPQOS_WSZ_LIT("IsDropped")},
		{EdxltokenColWidth, SPQOS_WSZ_LIT("ColWidth")},
		{EdxltokenColNullFreq, SPQOS_WSZ_LIT("NullFreq")},
		{EdxltokenColNdvRemain, SPQOS_WSZ_LIT("NdvRemain")},
		{EdxltokenColFreqRemain, SPQOS_WSZ_LIT("FreqRemain")},
		{EdxltokenColStatsMissing, SPQOS_WSZ_LIT("ColStatsMissing")},

		{EdxltokenCtidColName, SPQOS_WSZ_LIT("ctid")},
		{EdxltokenOidColName, SPQOS_WSZ_LIT("oid")},
		{EdxltokenXminColName, SPQOS_WSZ_LIT("xmin")},
		{EdxltokenCminColName, SPQOS_WSZ_LIT("cmin")},
		{EdxltokenXmaxColName, SPQOS_WSZ_LIT("xmax")},
		{EdxltokenCmaxColName, SPQOS_WSZ_LIT("cmax")},
		{EdxltokenTableOidColName, SPQOS_WSZ_LIT("tableoid")},
		{EdxltokenXCNidAttribute, SPQOS_WSZ_LIT("xc_node_id")},
		{EdxltokenBidAttribute, SPQOS_WSZ_LIT("tablebucketid")},
		{EdxltokenUidAttribute, SPQOS_WSZ_LIT("gs_tuple_uid")},
		{EdxltokenRootCtidColName, SPQOS_WSZ_LIT("_root_ctid")},

		{EdxltokenActionColId, SPQOS_WSZ_LIT("ActionCol")},
		{EdxltokenCtidColId, SPQOS_WSZ_LIT("CtidCol")},
		{EdxltokenGpSegmentIdColId, SPQOS_WSZ_LIT("SegmentIdCol")},
		{EdxltokenTupleOidColId, SPQOS_WSZ_LIT("TupleOidCol")},
		{EdxltokenUpdatePreservesOids, SPQOS_WSZ_LIT("PreserveOids")},

		{EdxltokenInputSegments, SPQOS_WSZ_LIT("InputSegments")},
		{EdxltokenOutputSegments, SPQOS_WSZ_LIT("OutputSegments")},
		{EdxltokenSegment, SPQOS_WSZ_LIT("Segment")},
		{EdxltokenSegId, SPQOS_WSZ_LIT("SegmentId")},

		{EdxltokenGroupingCols, SPQOS_WSZ_LIT("GroupingColumns")},
		{EdxltokenGroupingCol, SPQOS_WSZ_LIT("GroupingColumn")},

		{EdxltokenParamKind, SPQOS_WSZ_LIT("ParamType")},

		{EdxltokenAppendIsTarget, SPQOS_WSZ_LIT("IsTarget")},
		{EdxltokenAppendIsZapped, SPQOS_WSZ_LIT("IsZapped")},

		{EdxltokenOpNo, SPQOS_WSZ_LIT("OperatorMdid")},
		{EdxltokenOpName, SPQOS_WSZ_LIT("OperatorName")},
		{EdxltokenOpType, SPQOS_WSZ_LIT("OperatorType")},
		{EdxltokenOpTypeAny, SPQOS_WSZ_LIT("Any")},
		{EdxltokenOpTypeAll, SPQOS_WSZ_LIT("All")},

		{EdxltokenTypeName, SPQOS_WSZ_LIT("TypeName")},
		{EdxltokenUnknown, SPQOS_WSZ_LIT("Unknown")},

		{EdxltokenFuncId, SPQOS_WSZ_LIT("FuncId")},
		{EdxltokenFuncRetSet, SPQOS_WSZ_LIT("FuncRetSet")},

		{EdxltokenAlias, SPQOS_WSZ_LIT("Alias")},

		{EdxltokenSortOpId, SPQOS_WSZ_LIT("SortOperatorMdid")},
		{EdxltokenSortOpName, SPQOS_WSZ_LIT("SortOperatorName")},
		{EdxltokenSortDiscardDuplicates, SPQOS_WSZ_LIT("SortDiscardDuplicates")},
		{EdxltokenSortNullsFirst, SPQOS_WSZ_LIT("SortNullsFirst")},

		{EdxltokenMaterializeEager, SPQOS_WSZ_LIT("Eager")},

		{EdxltokenSpoolId, SPQOS_WSZ_LIT("SpoolId")},
		{EdxltokenSpoolType, SPQOS_WSZ_LIT("SpoolType")},
		{EdxltokenSpoolMaterialize, SPQOS_WSZ_LIT("Materialize")},
		{EdxltokenSpoolSort, SPQOS_WSZ_LIT("Sort")},
		{EdxltokenSpoolMultiSlice, SPQOS_WSZ_LIT("MultiSliceSpool")},
		{EdxltokenExecutorSliceId, SPQOS_WSZ_LIT("ExecutorSliceId")},
		{EdxltokenConsumerSliceCount, SPQOS_WSZ_LIT("NumberOfConsumersSlices")},

		{EdxltokenComparisonOp, SPQOS_WSZ_LIT("ComparisonOperator")},

		{EdxltokenXMLDocHeader,
		 SPQOS_WSZ_LIT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")},
		{EdxltokenNamespaceAttr, SPQOS_WSZ_LIT("xmlns")},
		{EdxltokenNamespacePrefix, SPQOS_WSZ_LIT("dxl")},
		{EdxltokenNamespaceURI,
		 SPQOS_WSZ_LIT("http://greenplum.com/dxl/2010/12/")},

		{EdxltokenBracketOpenTag, SPQOS_WSZ_LIT("<")},
		{EdxltokenBracketCloseTag, SPQOS_WSZ_LIT(">")},
		{EdxltokenBracketOpenEndTag, SPQOS_WSZ_LIT("</")},
		{EdxltokenBracketCloseSingletonTag, SPQOS_WSZ_LIT("/>")},
		{EdxltokenColon, SPQOS_WSZ_LIT(":")},
		{EdxltokenSemicolon, SPQOS_WSZ_LIT(";")},
		{EdxltokenComma, SPQOS_WSZ_LIT(",")},
		{EdxltokenDot, SPQOS_WSZ_LIT(".")},
		{EdxltokenDotSemicolon, SPQOS_WSZ_LIT(".;")},
		{EdxltokenSpace, SPQOS_WSZ_LIT(" ")},
		{EdxltokenQuote, SPQOS_WSZ_LIT("\"")},
		{EdxltokenEq, SPQOS_WSZ_LIT("=")},
		{EdxltokenIndent, SPQOS_WSZ_LIT("  ")},

		{EdxltokenTrue, SPQOS_WSZ_LIT("true")},
		{EdxltokenFalse, SPQOS_WSZ_LIT("false")},

		{EdxltokenRelation, SPQOS_WSZ_LIT("Relation")},
		{EdxltokenRelationExternal, SPQOS_WSZ_LIT("ExternalRelation")},
		{EdxltokenRelationCTAS, SPQOS_WSZ_LIT("CTASRelation")},
		{EdxltokenName, SPQOS_WSZ_LIT("Name")},
		{EdxltokenSchema, SPQOS_WSZ_LIT("Schema")},
		{EdxltokenTablespace, SPQOS_WSZ_LIT("Tablespace")},
		{EdxltokenOid, SPQOS_WSZ_LIT("Oid")},
		{EdxltokenVersion, SPQOS_WSZ_LIT("Version")},
		{EdxltokenMdid, SPQOS_WSZ_LIT("Mdid")},
		{EdxltokenMDTypeRequest, SPQOS_WSZ_LIT("TypeRequest")},
		{EdxltokenTypeInfo, SPQOS_WSZ_LIT("TypeInfo")},
		{EdxltokenFuncInfo, SPQOS_WSZ_LIT("FuncInfo")},
		{EdxltokenRelationMdid, SPQOS_WSZ_LIT("RelationMdid")},
		{EdxltokenRelationStats, SPQOS_WSZ_LIT("RelationStatistics")},
		{EdxltokenColumnStats, SPQOS_WSZ_LIT("ColumnStatistics")},
		{EdxltokenColumnStatsBucket, SPQOS_WSZ_LIT("StatsBucket")},
		{EdxltokenEmptyRelation, SPQOS_WSZ_LIT("EmptyRelation")},

		{EdxltokenIsNull, SPQOS_WSZ_LIT("IsNull")},
		{EdxltokenLintValue, SPQOS_WSZ_LIT("LintValue")},
		{EdxltokenDoubleValue, SPQOS_WSZ_LIT("DoubleValue")},

		{EdxltokenRelTemporary, SPQOS_WSZ_LIT("IsTemporary")},
		{EdxltokenRelHasOids, SPQOS_WSZ_LIT("HasOids")},

		{EdxltokenEntireRow, SPQOS_WSZ_LIT("*")},

		{EdxltokenRelStorageType, SPQOS_WSZ_LIT("StorageType")},
		{EdxltokenRelStorageHeap, SPQOS_WSZ_LIT("Heap")},
		{EdxltokenRelStorageAppendOnlyCols,
		 SPQOS_WSZ_LIT("AppendOnly, Column-oriented")},
		{EdxltokenRelStorageAppendOnlyRows,
		 SPQOS_WSZ_LIT("AppendOnly, Row-oriented")},
		{EdxltokenRelStorageAppendOnlyParquet,
		 SPQOS_WSZ_LIT("AppendOnly, Parquet")},
		{EdxltokenRelStorageVirtual, SPQOS_WSZ_LIT("Virtual")},
		{EdxltokenRelStorageExternal, SPQOS_WSZ_LIT("External")},
		{EdxltokenRelStorageSentinel, SPQOS_WSZ_LIT("Sentinel")},

		{EdxltokenRelDistrPolicy, SPQOS_WSZ_LIT("DistributionPolicy")},
		{EdxltokenRelDistrMasterOnly, SPQOS_WSZ_LIT("MasterOnly")},
		{EdxltokenRelDistrHash, SPQOS_WSZ_LIT("Hash")},
		{EdxltokenRelDistrRandom, SPQOS_WSZ_LIT("Random")},
		{EdxltokenRelDistrReplicated, SPQOS_WSZ_LIT("Replicated")},
		{EdxltokenConvertHashToRandom, SPQOS_WSZ_LIT("ConvertHashToRandom")},

		{EdxltokenRelDistrOpfamilies, SPQOS_WSZ_LIT("DistrOpfamilies")},
		{EdxltokenRelDistrOpfamily, SPQOS_WSZ_LIT("DistrOpfamily")},

		{EdxltokenRelDistrOpclasses, SPQOS_WSZ_LIT("DistrOpclasses")},
		{EdxltokenRelDistrOpclass, SPQOS_WSZ_LIT("DistrOpclass")},

		{EdxltokenExtRelRejLimit, SPQOS_WSZ_LIT("RejectLimit")},
		{EdxltokenExtRelRejLimitInRows, SPQOS_WSZ_LIT("RejectLimitInRows")},
		{EdxltokenExtRelFmtErrRel, SPQOS_WSZ_LIT("FormatErrorRelId")},

		{EdxltokenKeys, SPQOS_WSZ_LIT("Keys")},
		{EdxltokenDistrColumns, SPQOS_WSZ_LIT("DistributionColumns")},

		{EdxltokenPartKeys, SPQOS_WSZ_LIT("PartitionColumns")},
		{EdxltokenPartTypes, SPQOS_WSZ_LIT("PartitionTypes")},
		{EdxltokenNumLeafPartitions, SPQOS_WSZ_LIT("NumberLeafPartitions")},

		{EdxltokenTypeInt4, SPQOS_WSZ_LIT("Int4")},
		{EdxltokenTypeBool, SPQOS_WSZ_LIT("Bool")},

		{EdxltokenMetadataIdList, SPQOS_WSZ_LIT("MetadataIdList")},
		{EdxltokenMetadataColumns, SPQOS_WSZ_LIT("MetadataColumns")},
		{EdxltokenMetadataColumn, SPQOS_WSZ_LIT("MetadataColumn")},

		{EdxltokenColumnNullable, SPQOS_WSZ_LIT("Nullable")},
		{EdxltokenColumnDefaultValue, SPQOS_WSZ_LIT("DefaultValue")},

		{EdxltokenIndex, SPQOS_WSZ_LIT("Index")},

		{EdxltokenIndexInfoList, SPQOS_WSZ_LIT("IndexInfoList")},
		{EdxltokenIndexInfo, SPQOS_WSZ_LIT("IndexInfo")},

		{EdxltokenIndexKeyCols, SPQOS_WSZ_LIT("KeyColumns")},
		{EdxltokenIndexIncludedCols, SPQOS_WSZ_LIT("IncludedColumns")},
		{EdxltokenIndexClustered, SPQOS_WSZ_LIT("IsClustered")},
		{EdxltokenIndexPartial, SPQOS_WSZ_LIT("IsPartial")},
		{EdxltokenIndexType, SPQOS_WSZ_LIT("IndexType")},
		{EdxltokenIndexPhysicalType, SPQOS_WSZ_LIT("PhysicalIndexType")},
		{EdxltokenIndexTypeBtree, SPQOS_WSZ_LIT("B-tree")},
		{EdxltokenIndexTypeBitmap, SPQOS_WSZ_LIT("Bitmap")},
		{EdxltokenIndexTypeGist, SPQOS_WSZ_LIT("Gist")},
		{EdxltokenIndexTypeGin, SPQOS_WSZ_LIT("Gin")},
		{EdxltokenIndexItemType, SPQOS_WSZ_LIT("IndexItemType")},

		{EdxltokenOpfamily, SPQOS_WSZ_LIT("Opfamily")},
		{EdxltokenOpfamilies, SPQOS_WSZ_LIT("Opfamilies")},

		{EdxltokenRelExternalPartitions, SPQOS_WSZ_LIT("ExternalPartitions")},
		{EdxltokenRelExternalPartition, SPQOS_WSZ_LIT("ExternalPartition")},

		{EdxltokenPartitions, SPQOS_WSZ_LIT("Partitions")},
		{EdxltokenPartition, SPQOS_WSZ_LIT("Partition")},

		{EdxltokenConstraints, SPQOS_WSZ_LIT("Constraints")},
		{EdxltokenConstraint, SPQOS_WSZ_LIT("Constraint")},

		{EdxltokenCheckConstraints, SPQOS_WSZ_LIT("CheckConstraints")},
		{EdxltokenCheckConstraint, SPQOS_WSZ_LIT("CheckConstraint")},

		{EdxltokenPartConstraint, SPQOS_WSZ_LIT("PartConstraint")},
		{EdxltokenPartConstraintExprAbsent, SPQOS_WSZ_LIT("ExprAbsent")},
		{EdxltokenDefaultPartition, SPQOS_WSZ_LIT("DefaultPartition")},
		{EdxltokenPartConstraintUnbounded, SPQOS_WSZ_LIT("Unbounded")},

		{EdxltokenMDType, SPQOS_WSZ_LIT("Type")},
		{EdxltokenMDTypeRedistributable, SPQOS_WSZ_LIT("IsRedistributable")},
		{EdxltokenMDTypeHashable, SPQOS_WSZ_LIT("IsHashable")},
		{EdxltokenMDTypeMergeJoinable, SPQOS_WSZ_LIT("IsMergeJoinable")},
		{EdxltokenMDTypeComposite, SPQOS_WSZ_LIT("IsComposite")},
		{EdxltokenMDTypeIsTextRelated, SPQOS_WSZ_LIT("IsTextRelated")},
		{EdxltokenMDTypeFixedLength, SPQOS_WSZ_LIT("IsFixedLength")},
		{EdxltokenMDTypeLength, SPQOS_WSZ_LIT("Length")},
		{EdxltokenMDTypeByValue, SPQOS_WSZ_LIT("PassByValue")},
		{EdxltokenMDTypeDistrOpfamily, SPQOS_WSZ_LIT("DistrOpfamily")},
		{EdxltokenMDTypeLegacyDistrOpfamily,
		 SPQOS_WSZ_LIT("LegacyDistrOpfamily")},
		{EdxltokenMDTypeEqOp, SPQOS_WSZ_LIT("EqualityOp")},
		{EdxltokenMDTypeNEqOp, SPQOS_WSZ_LIT("InequalityOp")},
		{EdxltokenMDTypeLTOp, SPQOS_WSZ_LIT("LessThanOp")},
		{EdxltokenMDTypeLEqOp, SPQOS_WSZ_LIT("LessThanEqualsOp")},
		{EdxltokenMDTypeGTOp, SPQOS_WSZ_LIT("GreaterThanOp")},
		{EdxltokenMDTypeGEqOp, SPQOS_WSZ_LIT("GreaterThanEqualsOp")},
		{EdxltokenMDTypeCompOp, SPQOS_WSZ_LIT("ComparisonOp")},
		{EdxltokenMDTypeHashOp, SPQOS_WSZ_LIT("HashOp")},
		{EdxltokenMDTypeRelid, SPQOS_WSZ_LIT("BaseRelationMdid")},
		{EdxltokenMDTypeArray, SPQOS_WSZ_LIT("ArrayType")},

		{EdxltokenMDTypeAggMin, SPQOS_WSZ_LIT("MinAgg")},
		{EdxltokenMDTypeAggMax, SPQOS_WSZ_LIT("MaxAgg")},
		{EdxltokenMDTypeAggAvg, SPQOS_WSZ_LIT("AvgAgg")},
		{EdxltokenMDTypeAggSum, SPQOS_WSZ_LIT("SumAgg")},
		{EdxltokenMDTypeAggCount, SPQOS_WSZ_LIT("CountAgg")},

		{EdxltokenSPQDBScalarOp, SPQOS_WSZ_LIT("SPQDBScalarOp")},
		{EdxltokenSPQDBScalarOpLeftTypeId, SPQOS_WSZ_LIT("LeftType")},
		{EdxltokenSPQDBScalarOpRightTypeId, SPQOS_WSZ_LIT("RightType")},
		{EdxltokenSPQDBScalarOpResultTypeId, SPQOS_WSZ_LIT("ResultType")},
		{EdxltokenSPQDBScalarOpFuncId, SPQOS_WSZ_LIT("OpFunc")},
		{EdxltokenSPQDBScalarOpCommOpId, SPQOS_WSZ_LIT("Commutator")},
		{EdxltokenSPQDBScalarOpInverseOpId, SPQOS_WSZ_LIT("InverseOp")},
		{EdxltokenSPQDBScalarOpLTOpId, SPQOS_WSZ_LIT("LessThanMergeOp")},
		{EdxltokenSPQDBScalarOpGTOpId, SPQOS_WSZ_LIT("GreaterThanMergeOp")},
		{EdxltokenSPQDBScalarOpCmpType, SPQOS_WSZ_LIT("ComparisonType")},
		{EdxltokenSPQDBScalarOpHashOpfamily, SPQOS_WSZ_LIT("HashOpfamily")},
		{EdxltokenSPQDBScalarOpLegacyHashOpfamily,
		 SPQOS_WSZ_LIT("LegacyHashOpfamily")},

		{EdxltokenCmpEq, SPQOS_WSZ_LIT("Eq")},
		{EdxltokenCmpNeq, SPQOS_WSZ_LIT("NEq")},
		{EdxltokenCmpLt, SPQOS_WSZ_LIT("LT")},
		{EdxltokenCmpLeq, SPQOS_WSZ_LIT("LEq")},
		{EdxltokenCmpGt, SPQOS_WSZ_LIT("GT")},
		{EdxltokenCmpGeq, SPQOS_WSZ_LIT("GEq")},
		{EdxltokenCmpIDF, SPQOS_WSZ_LIT("IDF")},
		{EdxltokenCmpOther, SPQOS_WSZ_LIT("Other")},

		{EdxltokenReturnsNullOnNullInput,
		 SPQOS_WSZ_LIT("ReturnsNullOnNullInput")},
		{EdxltokenIsNDVPreserving, SPQOS_WSZ_LIT("IsNDVPreserving")},

		{EdxltokenTriggers, SPQOS_WSZ_LIT("Triggers")},
		{EdxltokenTrigger, SPQOS_WSZ_LIT("Trigger")},

		{EdxltokenSPQDBTrigger, SPQOS_WSZ_LIT("SPQDBTrigger")},
		{EdxltokenSPQDBTriggerRow, SPQOS_WSZ_LIT("IsRow")},
		{EdxltokenSPQDBTriggerBefore, SPQOS_WSZ_LIT("IsBefore")},
		{EdxltokenSPQDBTriggerInsert, SPQOS_WSZ_LIT("IsInsert")},
		{EdxltokenSPQDBTriggerDelete, SPQOS_WSZ_LIT("IsDelete")},
		{EdxltokenSPQDBTriggerUpdate, SPQOS_WSZ_LIT("IsUpdate")},
		{EdxltokenSPQDBTriggerEnabled, SPQOS_WSZ_LIT("IsEnabled")},

		{EdxltokenSPQDBFunc, SPQOS_WSZ_LIT("SPQDBFunc")},
		{EdxltokenSPQDBFuncStability, SPQOS_WSZ_LIT("Stability")},
		{EdxltokenSPQDBFuncStable, SPQOS_WSZ_LIT("Stable")},
		{EdxltokenSPQDBFuncImmutable, SPQOS_WSZ_LIT("Immutable")},
		{EdxltokenSPQDBFuncVolatile, SPQOS_WSZ_LIT("Volatile")},
		{EdxltokenSPQDBFuncDataAccess, SPQOS_WSZ_LIT("DataAccess")},
		{EdxltokenSPQDBFuncNoSQL, SPQOS_WSZ_LIT("NoSQL")},
		{EdxltokenSPQDBFuncContainsSQL, SPQOS_WSZ_LIT("ContainsSQL")},
		{EdxltokenSPQDBFuncReadsSQLData, SPQOS_WSZ_LIT("ReadsSQLData")},
		{EdxltokenSPQDBFuncModifiesSQLData, SPQOS_WSZ_LIT("ModifiesSQLData")},
		{EdxltokenSPQDBFuncResultTypeId, SPQOS_WSZ_LIT("ResultType")},
		{EdxltokenSPQDBFuncReturnsSet, SPQOS_WSZ_LIT("ReturnsSet")},
		{EdxltokenSPQDBFuncStrict, SPQOS_WSZ_LIT("IsStrict")},
		{EdxltokenSPQDBFuncNDVPreserving, SPQOS_WSZ_LIT("IsNDVPreserving")},
		{EdxltokenSPQDBFuncIsAllowedForPS, SPQOS_WSZ_LIT("IsAllowedForPS")},

		{EdxltokenSPQDBAgg, SPQOS_WSZ_LIT("SPQDBAgg")},
		{EdxltokenSPQDBIsAggOrdered, SPQOS_WSZ_LIT("IsOrdered")},
		{EdxltokenSPQDBAggResultTypeId, SPQOS_WSZ_LIT("ResultType")},
		{EdxltokenSPQDBAggIntermediateResultTypeId,
		 SPQOS_WSZ_LIT("IntermediateResultType")},
		{EdxltokenSPQDBAggSplittable, SPQOS_WSZ_LIT("IsSplittable")},
		{EdxltokenSPQDBAggHashAggCapable, SPQOS_WSZ_LIT("HashAggCapable")},

		{EdxltokenSPQDBCast, SPQOS_WSZ_LIT("MDCast")},
		{EdxltokenSPQDBCastBinaryCoercible, SPQOS_WSZ_LIT("BinaryCoercible")},
		{EdxltokenSPQDBCastSrcType, SPQOS_WSZ_LIT("SourceTypeId")},
		{EdxltokenSPQDBCastDestType, SPQOS_WSZ_LIT("DestinationTypeId")},
		{EdxltokenSPQDBCastFuncId, SPQOS_WSZ_LIT("CastFuncId")},
		{EdxltokenSPQDBCastCoercePathType, SPQOS_WSZ_LIT("CoercePathType")},
		{EdxltokenSPQDBArrayCoerceCast, SPQOS_WSZ_LIT("ArrayCoerceCast")},

		{EdxltokenSPQDBMDScCmp, SPQOS_WSZ_LIT("MDScalarComparison")},

		{EdxltokenQuery, SPQOS_WSZ_LIT("Query")},
		{EdxltokenQueryOutput, SPQOS_WSZ_LIT("OutputColumns")},
		{EdxltokenLogical, SPQOS_WSZ_LIT("LogicalOp")},
		{EdxltokenLogicalGet, SPQOS_WSZ_LIT("LogicalGet")},
		{EdxltokenLogicalExternalGet, SPQOS_WSZ_LIT("LogicalExternalGet")},
		{EdxltokenLogicalProject, SPQOS_WSZ_LIT("LogicalProject")},
		{EdxltokenLogicalSelect, SPQOS_WSZ_LIT("LogicalSelect")},
		{EdxltokenLogicalJoin, SPQOS_WSZ_LIT("LogicalJoin")},
		{EdxltokenLogicalCTEProducer, SPQOS_WSZ_LIT("LogicalCTEProducer")},
		{EdxltokenLogicalCTEConsumer, SPQOS_WSZ_LIT("LogicalCTEConsumer")},
		{EdxltokenCTEList, SPQOS_WSZ_LIT("CTEList")},
		{EdxltokenLogicalCTEAnchor, SPQOS_WSZ_LIT("LogicalCTEAnchor")},
		{EdxltokenLogicalLimit, SPQOS_WSZ_LIT("LogicalLimit")},
		{EdxltokenLogicalConstTable, SPQOS_WSZ_LIT("LogicalConstTable")},
		{EdxltokenLogicalGrpBy, SPQOS_WSZ_LIT("LogicalGroupBy")},
		{EdxltokenLogicalSetOperation, SPQOS_WSZ_LIT("LogicalSetOperation")},
		{EdxltokenLogicalTVF, SPQOS_WSZ_LIT("LogicalTVF")},
		{EdxltokenLogicalWindow, SPQOS_WSZ_LIT("LogicalWindow")},

		{EdxltokenLogicalInsert, SPQOS_WSZ_LIT("LogicalInsert")},
		{EdxltokenLogicalDelete, SPQOS_WSZ_LIT("LogicalDelete")},
		{EdxltokenLogicalUpdate, SPQOS_WSZ_LIT("LogicalUpdate")},
		{EdxltokenLogicalCTAS, SPQOS_WSZ_LIT("LogicalCTAS")},
		{EdxltokenPhysicalCTAS, SPQOS_WSZ_LIT("PhysicalCTAS")},

		{EdxltokenInsertCols, SPQOS_WSZ_LIT("InsertColumns")},
		{EdxltokenDeleteCols, SPQOS_WSZ_LIT("DeleteColumns")},
		{EdxltokenNewCols, SPQOS_WSZ_LIT("NewColumns")},
		{EdxltokenOldCols, SPQOS_WSZ_LIT("OldColumns")},

		{EdxltokenCTEId, SPQOS_WSZ_LIT("CTEId")},

		{EdxltokenInputCols, SPQOS_WSZ_LIT("InputColumns")},
		{EdxltokenCastAcrossInputs, SPQOS_WSZ_LIT("CastAcrossInputs")},

		{EdxltokenLogicalUnion, SPQOS_WSZ_LIT("Union")},
		{EdxltokenLogicalUnionAll, SPQOS_WSZ_LIT("UnionAll")},
		{EdxltokenLogicalIntersect, SPQOS_WSZ_LIT("Intersect")},
		{EdxltokenLogicalIntersectAll, SPQOS_WSZ_LIT("IntersectAll")},
		{EdxltokenLogicalDifference, SPQOS_WSZ_LIT("Difference")},
		{EdxltokenLogicalDifferenceAll, SPQOS_WSZ_LIT("DifferenceAll")},

		{EdxltokenIndexDescr, SPQOS_WSZ_LIT("IndexDescriptor")},
		{EdxltokenIndexName, SPQOS_WSZ_LIT("IndexName")},
		{EdxltokenScalarIndexCondList, SPQOS_WSZ_LIT("IndexCondList")},
		{EdxltokenIndexScanDirection, SPQOS_WSZ_LIT("IndexScanDirection")},
		{EdxltokenIndexScanDirectionForward, SPQOS_WSZ_LIT("Forward")},
		{EdxltokenIndexScanDirectionBackward, SPQOS_WSZ_LIT("Backward")},
		{EdxltokenIndexScanDirectionNoMovement, SPQOS_WSZ_LIT("NoMovement")},

		{EdxltokenStackTrace, SPQOS_WSZ_LIT("Stacktrace")},

		{EdxltokenStatistics, SPQOS_WSZ_LIT("Statistics")},
		{EdxltokenStatsBaseRelation, SPQOS_WSZ_LIT("BaseRelationStats")},
		{EdxltokenStatsDerivedRelation, SPQOS_WSZ_LIT("DerivedRelationStats")},
		{EdxltokenStatsDerivedColumn, SPQOS_WSZ_LIT("DerivedColumnStats")},
		{EdxltokenStatsBucketLowerBound, SPQOS_WSZ_LIT("LowerBound")},
		{EdxltokenStatsBucketUpperBound, SPQOS_WSZ_LIT("UpperBound")},
		{EdxltokenStatsFrequency, SPQOS_WSZ_LIT("Frequency")},
		{EdxltokenStatsDistinct, SPQOS_WSZ_LIT("DistinctValues")},
		{EdxltokenStatsBoundClosed, SPQOS_WSZ_LIT("Closed")},

		{EdxltokenSearchStrategy, SPQOS_WSZ_LIT("SearchStrategy")},
		{EdxltokenSearchStage, SPQOS_WSZ_LIT("SearchStage")},
		{EdxltokenXform, SPQOS_WSZ_LIT("Xform")},
		{EdxltokenTimeThreshold, SPQOS_WSZ_LIT("TimeThreshold")},
		{EdxltokenCostThreshold, SPQOS_WSZ_LIT("CostThreshold")},

		{EdxltokenCostParams, SPQOS_WSZ_LIT("CostParams")},
		{EdxltokenCostParam, SPQOS_WSZ_LIT("CostParam")},
		{EdxltokenCostParamLowerBound, SPQOS_WSZ_LIT("LowerBound")},
		{EdxltokenCostParamUpperBound, SPQOS_WSZ_LIT("UpperBound")},

		{EdxltokenTopLimitUnderDML, SPQOS_WSZ_LIT("TopLimitUnderDML")},

		{EdxltokenCtasOptionType, SPQOS_WSZ_LIT("CtasOptionType")},
		{EdxltokenVarTypeModList, SPQOS_WSZ_LIT("VarTypeModList")},
		{EdxltokenNLJIndexParamList, SPQOS_WSZ_LIT("NLJIndexParamList")},
		{EdxltokenNLJIndexParam, SPQOS_WSZ_LIT("NLJIndexParam")},
		{EdxltokenNLJIndexOuterRefAsParam, SPQOS_WSZ_LIT("OuterRefAsParam")},
	};

	u_sess->spq_cxt.m_pstrmap = SPQOS_NEW_ARRAY(u_sess->spq_cxt.m_mp, SStrMapElem, EdxltokenSentinel);
	u_sess->spq_cxt.m_pxmlszmap = SPQOS_NEW_ARRAY(u_sess->spq_cxt.m_mp, SXMLStrMapElem, EdxltokenSentinel);

	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(rgStrMap); ul++)
	{
		SWszMapElem mapelem = rgStrMap[ul];

		((SStrMapElem *)u_sess->spq_cxt.m_pstrmap)[mapelem.m_edxlt].m_pstr =
			SPQOS_NEW(u_sess->spq_cxt.m_mp) CWStringConst(u_sess->spq_cxt.m_mp, mapelem.m_wsz);
		((SXMLStrMapElem *)u_sess->spq_cxt.m_pxmlszmap)[mapelem.m_edxlt].m_xmlsz = XmlstrFromWsz(mapelem.m_wsz);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLTokens::Terminate
//
//	@doc:
//		Releases the DXL token constants
//
//---------------------------------------------------------------------------
void
CDXLTokens::Terminate()
{
	SPQOS_DELETE_ARRAY((SStrMapElem *)u_sess->spq_cxt.m_pstrmap);
	SPQOS_DELETE_ARRAY((SXMLStrMapElem *)u_sess->spq_cxt.m_pxmlszmap);
	SPQOS_DELETE(u_sess->spq_cxt.m_dxl_memory_manager);
}


//---------------------------------------------------------------------------
//	@function:
//		CDXLTokens::GetDXLTokenStr
//
//	@doc:
//		Returns the token with the given token id in CWStringConst format
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLTokens::GetDXLTokenStr(Edxltoken token_type)
{
	SPQOS_ASSERT(NULL != u_sess->spq_cxt.m_pstrmap && "Token map not initialized yet");

	const CWStringConst *str = ((SStrMapElem *)u_sess->spq_cxt.m_pstrmap)[token_type].m_pstr;
	SPQOS_ASSERT(NULL != str);

	return str;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLTokens::XmlstrToken
//
//	@doc:
//		Returns the token with the given token id in XMLCh* format
//
//---------------------------------------------------------------------------
const XMLCh *
CDXLTokens::XmlstrToken(Edxltoken token_type)
{
	SPQOS_ASSERT(NULL != u_sess->spq_cxt.m_pxmlszmap && "Token map not initialized yet");

	const XMLCh *xml_val = ((SXMLStrMapElem *)u_sess->spq_cxt.m_pxmlszmap)[token_type].m_xmlsz;
	SPQOS_ASSERT(NULL != xml_val);

	return xml_val;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLTokens::XmlstrFromWsz
//
//	@doc:
//		Creates an XMLCh* string from the specified wide character array in the
//		provided memory pool.
//
//---------------------------------------------------------------------------
XMLCh *
CDXLTokens::XmlstrFromWsz(const WCHAR *wsz)
{
	ULONG length = SPQOS_WSZ_LENGTH(wsz);
	CHAR *sz = SPQOS_NEW_ARRAY(u_sess->spq_cxt.m_mp, CHAR, 1 + length);

#ifdef SPQOS_DEBUG
	LINT iLen =
#endif
		clib::Wcstombs(sz, const_cast<WCHAR *>(wsz), 1 + length);

	SPQOS_ASSERT(0 <= iLen);
	XMLCh *pxmlsz = XMLString::transcode(sz, u_sess->spq_cxt.m_dxl_memory_manager);
	SPQOS_DELETE_ARRAY(sz);
	return pxmlsz;
}

// EOF
