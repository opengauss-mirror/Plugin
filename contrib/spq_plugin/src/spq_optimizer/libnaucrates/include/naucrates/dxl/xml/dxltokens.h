//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		dxltokens.h
//
//	@doc:
//		Constants for the tokens used in the DXL document.
//		Tokens are represented in both in CWStringConst format, and as XMLCh
//		arrays - the native format of the Xerces parser.
//		Constants are dynamically created during loading of the dxl library.
//---------------------------------------------------------------------------
#ifndef SPQDXL_dxltokens_H
#define SPQDXL_dxltokens_H

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

#include "spqos/base.h"
#include "spqos/common/CHashMap.h"
#include "spqos/string/CWStringConst.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//fwd decl
class CDXLMemoryManager;

enum Edxltoken
{
	EdxltokenDXLMessage,

	EdxltokenComment,

	EdxltokenPlan,
	EdxltokenPlanId,
	EdxltokenPlanSpaceSize,
	EdxltokenSamplePlans,
	EdxltokenSamplePlan,
	EdxltokenCostDistr,
	EdxltokenRelativeCost,
	EdxltokenX,
	EdxltokenY,

	EdxltokenOptimizerConfig,
	EdxltokenEnumeratorConfig,
	EdxltokenStatisticsConfig,
	EdxltokenDampingFactorFilter,
	EdxltokenDampingFactorJoin,
	EdxltokenDampingFactorGroupBy,
	EdxltokenCTEConfig,
	EdxltokenCTEInliningCutoff,
	EdxltokenCostModelConfig,
	EdxltokenCostModelType,
	EdxltokenSegmentsForCosting,
	EdxltokenHint,
	EdxltokenJoinArityForAssociativityCommutativity,
	EdxltokenArrayExpansionThreshold,
	EdxltokenJoinOrderDPThreshold,
	EdxltokenBroadcastThreshold,
	EdxltokenEnforceConstraintsOnDML,
	EdxltokenPushGroupByBelowSetopThreshold,
	EdxltokenXformBindThreshold,
	EdxltokenSkewFactor,
	EdxltokenMaxStatsBuckets,
	EdxltokenWindowOids,
	EdxltokenOidRowNumber,
	EdxltokenOidRank,

	EdxltokenPlanSamples,

	EdxltokenMetadata,
	EdxltokenTraceFlags,
	EdxltokenMDRequest,

	EdxltokenSysids,
	EdxltokenSysid,

	EdxltokenThread,

	EdxltokenPhysical,

	EdxltokenPhysicalTableScan,
    EdxltokenPhysicalTableShareScan, /* SPQ: for ShareScan */
	EdxltokenPhysicalBitmapTableScan,
	EdxltokenPhysicalDynamicBitmapTableScan,
	EdxltokenPhysicalExternalScan,
	EdxltokenPhysicalMultiExternalScan,
	EdxltokenPhysicalIndexScan,
	EdxltokenPhysicalIndexOnlyScan,
    EdxltokenPhysicalShareIndexScan, /* SPQ: for shareindexscan */
	EdxltokenPhysicalHashJoin,
	EdxltokenPhysicalNLJoin,
	EdxltokenPhysicalNLJoinIndex,
	EdxltokenPhysicalMergeJoin,
	EdxltokenPhysicalGatherMotion,
	EdxltokenPhysicalBroadcastMotion,
	EdxltokenPhysicalRedistributeMotion,
	EdxltokenPhysicalRoutedDistributeMotion,
	EdxltokenPhysicalRandomMotion,
	EdxltokenPhysicalSort,
	EdxltokenPhysicalLimit,
	EdxltokenPhysicalResult,
	EdxltokenPhysicalSubqueryScan,
	EdxltokenPhysicalAggregate,
	EdxltokenPhysicalAppend,
	EdxltokenPhysicalMaterialize,
	EdxltokenPhysicalSequence,
	EdxltokenPhysicalDynamicTableScan,
	EdxltokenPhysicalDynamicIndexScan,
	EdxltokenPhysicalTVF,
	EdxltokenPhysicalWindow,
	EdxltokenPhysicalPartitionSelector,
	EdxltokenPhysicalPartitionSelectorLevels,
	EdxltokenPhysicalPartitionSelectorScanId,
	EdxltokenPhysicalValuesScan,

	EdxltokenPhysicalCTEProducer,
	EdxltokenPhysicalCTEConsumer,

	EdxltokenDuplicateSensitive,

	EdxltokenPartIndexId,
	EdxltokenPartIndexIdPrintable,
	EdxltokenSegmentIdCol,

	EdxltokenScalar,

	EdxltokenScalarProjList,
	EdxltokenScalarFilter,
	EdxltokenScalarAggref,
	EdxltokenScalarWindowref,
	EdxltokenScalarArrayComp,
	EdxltokenScalarBoolTestIsTrue,
	EdxltokenScalarBoolTestIsNotTrue,
	EdxltokenScalarBoolTestIsFalse,
	EdxltokenScalarBoolTestIsNotFalse,
	EdxltokenScalarBoolTestIsUnknown,
	EdxltokenScalarBoolTestIsNotUnknown,
	EdxltokenScalarBoolAnd,
	EdxltokenScalarBoolOr,
	EdxltokenScalarBoolNot,
	EdxltokenScalarMin,
	EdxltokenScalarMax,
	EdxltokenScalarCaseTest,
	EdxltokenScalarCoalesce,
	EdxltokenScalarComp,
	EdxltokenScalarConstValue,
	EdxltokenScalarDistinctComp,
	EdxltokenScalarFuncExpr,
	EdxltokenScalarIsNull,
	EdxltokenScalarIsNotNull,
	EdxltokenScalarNullIf,
	EdxltokenScalarHashCondList,
	EdxltokenScalarMergeCondList,
	EdxltokenScalarHashExprList,
	EdxltokenScalarHashExpr,
	EdxltokenScalarIdent,
	EdxltokenScalarIfStmt,
	EdxltokenScalarSwitch,
	EdxltokenScalarSwitchCase,
	EdxltokenScalarSubPlan,
	EdxltokenScalarJoinFilter,
	EdxltokenScalarRecheckCondFilter,
	EdxltokenScalarLimitCount,
	EdxltokenScalarLimitOffset,
	EdxltokenScalarOneTimeFilter,
	EdxltokenScalarOpExpr,
	EdxltokenScalarProjElem,
	EdxltokenScalarCast,
	EdxltokenScalarCoerceToDomain,
	EdxltokenScalarCoerceViaIO,
	EdxltokenScalarArrayCoerceExpr,
	EdxltokenScalarSortCol,
	EdxltokenScalarSortColList,
	EdxltokenScalarGroupingColList,
	EdxltokenScalarSortGroupClause,

	EdxltokenScalarBitmapAnd,
	EdxltokenScalarBitmapOr,

	EdxltokenScalarArray,
	EdxltokenScalarArrayRef,
	EdxltokenScalarArrayRefIndexList,
	EdxltokenScalarArrayRefIndexListBound,
	EdxltokenScalarArrayRefIndexListLower,
	EdxltokenScalarArrayRefIndexListUpper,
	EdxltokenScalarArrayRefExpr,
	EdxltokenScalarArrayRefAssignExpr,

	EdxltokenScalarAssertConstraint,
	EdxltokenScalarAssertConstraintList,

	EdxltokenScalarSubPlanType,
	EdxltokenScalarSubPlanTypeScalar,
	EdxltokenScalarSubPlanTypeExists,
	EdxltokenScalarSubPlanTypeNotExists,
	EdxltokenScalarSubPlanTypeAny,
	EdxltokenScalarSubPlanTypeAll,
	EdxltokenScalarSubPlanTestExpr,
	EdxltokenScalarSubPlanParamList,
	EdxltokenScalarSubPlanParam,

	EdxltokenScalarSubquery,
	EdxltokenScalarSubqueryAll,
	EdxltokenScalarSubqueryAny,
	EdxltokenScalarSubqueryExists,
	EdxltokenScalarSubqueryNotExists,

	EdxltokenScalarDMLAction,
	EdxltokenScalarOpList,

	EdxltokenPartLevelEqFilterList,
	EdxltokenPartLevelEqFilterElemList,
	EdxltokenPartLevelFilterList,
	EdxltokenPartLevel,
	EdxltokenScalarPartDefault,
	EdxltokenScalarPartBound,
	EdxltokenScalarPartBoundLower,
	EdxltokenScalarPartBoundInclusion,
	EdxltokenScalarPartBoundOpen,
	EdxltokenScalarPartListValues,
	EdxltokenScalarPartListNullTest,
	EdxltokenScalarResidualFilter,
	EdxltokenScalarPropagationExpr,
	EdxltokenScalarPrintableFilter,
	EdxltokenScalarBitmapIndexProbe,
	EdxltokenScalarValuesList,

	EdxltokenWindowFrame,
	EdxltokenScalarWindowFrameLeadingEdge,
	EdxltokenScalarWindowFrameTrailingEdge,
	EdxltokenWindowKeyList,
	EdxltokenWindowKey,

	EdxltokenWindowSpecList,
	EdxltokenWindowSpec,

	EdxltokenWindowFrameSpec,
	EdxltokenWindowFSRow,
	EdxltokenWindowFSRange,

	EdxltokenWindowLeadingBoundary,
	EdxltokenWindowTrailingBoundary,
	EdxltokenWindowBoundaryUnboundedPreceding,
	EdxltokenWindowBoundaryBoundedPreceding,
	EdxltokenWindowBoundaryCurrentRow,
	EdxltokenWindowBoundaryUnboundedFollowing,
	EdxltokenWindowBoundaryBoundedFollowing,
	EdxltokenWindowBoundaryDelayedBoundedPreceding,
	EdxltokenWindowBoundaryDelayedBoundedFollowing,

	EdxltokenWindowExclusionStrategy,
	EdxltokenWindowESNone,
	EdxltokenWindowESNulls,
	EdxltokenWindowESCurrentRow,
	EdxltokenWindowESGroup,
	EdxltokenWindowESTies,

	EdxltokenWindowrefOid,
	EdxltokenWindowrefDistinct,
	EdxltokenWindowrefStarArg,
	EdxltokenWindowrefSimpleAgg,
	EdxltokenWindowrefStrategy,
	EdxltokenWindowrefStageImmediate,
	EdxltokenWindowrefStagePreliminary,
	EdxltokenWindowrefStageRowKey,
	EdxltokenWindowrefWinSpecPos,

	EdxltokenValue,
	EdxltokenTypeId,
	EdxltokenTypeIds,

	EdxltokenConstTuple,
	EdxltokenDatum,

	// CoerceToDomain and CoerceViaIO and ArrayCoerceExpr related tokens
	EdxltokenTypeMod,
	EdxltokenCoercionForm,
	EdxltokenLocation,
	EdxltokenElementFunc,
	EdxltokenIsExplicit,

	EdxltokenJoinType,
	EdxltokenJoinInner,
	EdxltokenJoinLeft,
	EdxltokenJoinFull,
	EdxltokenJoinRight,
	EdxltokenJoinIn,
	EdxltokenJoinLeftAntiSemiJoin,
	EdxltokenJoinLeftAntiSemiJoinNotIn,

	EdxltokenMergeJoinUniqueOuter,

	EdxltokenAggStrategy,
	EdxltokenAggStrategyPlain,
	EdxltokenAggStrategySorted,
	EdxltokenAggStrategyHashed,
	EdxltokenAggStreamSafe,

	EdxltokenAggrefOid,
	EdxltokenAggrefDistinct,
	EdxltokenAggrefKind,
	EdxltokenAggrefGpAggOid,
	EdxltokenAggrefStage,
	EdxltokenAggrefLookups,
	EdxltokenAggrefStageNormal,
	EdxltokenAggrefStagePartial,
	EdxltokenAggrefStageIntermediate,
	EdxltokenAggrefStageFinal,
	EdxltokenAggrefKindNormal,
	EdxltokenAggrefKindOrderedSet,
	EdxltokenAggrefKindHypothetical,

	EdxltokenArrayType,
	EdxltokenArrayElementType,
	EdxltokenArrayMultiDim,

	EdxltokenTableDescr,
	EdxltokenProperties,
	EdxltokenOutputCols,
	EdxltokenCost,
	EdxltokenStartupCost,
	EdxltokenTotalCost,
	EdxltokenRows,
	EdxltokenWidth,
	EdxltokenRelPages,
	EdxltokenRelAllVisible,
	EdxltokenCTASOptions,
	EdxltokenCTASOption,

	EdxltokenExecuteAsUser,

	EdxltokenAlias,
	EdxltokenTableName,
	EdxltokenDerivedTableName,

	EdxltokenColDescr,
	EdxltokenColRef,

	EdxltokenColumns,
	EdxltokenColumn,
	EdxltokenColName,
	EdxltokenTagColType,
	EdxltokenColId,
	EdxltokenAttno,
	EdxltokenColDropped,
	EdxltokenColWidth,
	EdxltokenColNullFreq,
	EdxltokenColNdvRemain,
	EdxltokenColFreqRemain,
	EdxltokenColStatsMissing,

	EdxltokenCtidColName,
	EdxltokenOidColName,
	EdxltokenXminColName,
	EdxltokenCminColName,
	EdxltokenXmaxColName,
	EdxltokenCmaxColName,
	EdxltokenTableOidColName,
	EdxltokenRootCtidColName,
	EdxltokenXCNidAttribute,
	EdxltokenBidAttribute,
	EdxltokenUidAttribute,

	EdxltokenActionColId,
	EdxltokenCtidColId,
	EdxltokenGpSegmentIdColId,
	EdxltokenTupleOidColId,
	EdxltokenUpdatePreservesOids,

	EdxltokenInputSegments,
	EdxltokenOutputSegments,
	EdxltokenSegment,
	EdxltokenSegId,

	EdxltokenGroupingCols,
	EdxltokenGroupingCol,

	EdxltokenParamKind,

	EdxltokenAppendIsTarget,
	EdxltokenAppendIsZapped,

	EdxltokenOpNo,
	EdxltokenOpName,

	EdxltokenOpType,
	EdxltokenOpTypeAny,
	EdxltokenOpTypeAll,

	EdxltokenTypeName,
	EdxltokenUnknown,

	EdxltokenFuncId,
	EdxltokenFuncRetSet,


	EdxltokenSortOpId,
	EdxltokenSortOpName,
	EdxltokenSortDiscardDuplicates,
	EdxltokenSortNullsFirst,

	EdxltokenMaterializeEager,
	EdxltokenSpoolId,
	EdxltokenSpoolType,
	EdxltokenSpoolMaterialize,
	EdxltokenSpoolSort,
	EdxltokenSpoolMultiSlice,
	EdxltokenExecutorSliceId,
	EdxltokenConsumerSliceCount,

	EdxltokenComparisonOp,

	EdxltokenXMLDocHeader,
	EdxltokenNamespaceAttr,
	EdxltokenNamespacePrefix,
	EdxltokenNamespaceURI,

	EdxltokenBracketOpenTag,
	EdxltokenBracketCloseTag,
	EdxltokenBracketOpenEndTag,
	EdxltokenBracketCloseSingletonTag,
	EdxltokenColon,
	EdxltokenSemicolon,
	EdxltokenComma,
	EdxltokenDot,
	EdxltokenDotSemicolon,
	EdxltokenSpace,
	EdxltokenQuote,
	EdxltokenEq,
	EdxltokenIndent,

	EdxltokenTrue,
	EdxltokenFalse,

	// nest params related constants
	EdxltokenNLJIndexParamList,
	EdxltokenNLJIndexParam,
	EdxltokenNLJIndexOuterRefAsParam,

	// metadata-related constants
	EdxltokenRelation,
	EdxltokenRelationExternal,
	EdxltokenRelationCTAS,
	EdxltokenName,
	EdxltokenSchema,
	EdxltokenTablespace,
	EdxltokenOid,
	EdxltokenVersion,
	EdxltokenMdid,
	EdxltokenMDTypeRequest,
	EdxltokenTypeInfo,
	EdxltokenFuncInfo,
	EdxltokenRelationMdid,
	EdxltokenRelationStats,
	EdxltokenColumnStats,
	EdxltokenColumnStatsBucket,
	EdxltokenEmptyRelation,
	EdxltokenIsNull,
	EdxltokenLintValue,
	EdxltokenDoubleValue,

	EdxltokenRelTemporary,
	EdxltokenRelHasOids,

	EdxltokenRelStorageType,
	EdxltokenRelStorageHeap,
	EdxltokenRelStorageAppendOnlyCols,
	EdxltokenRelStorageAppendOnlyRows,
	EdxltokenRelStorageAppendOnlyParquet,
	EdxltokenRelStorageVirtual,
	EdxltokenRelStorageExternal,
	EdxltokenRelStorageSentinel,

	EdxltokenPartKeys,
	EdxltokenPartTypes,
	EdxltokenNumLeafPartitions,

	EdxltokenRelDistrPolicy,
	EdxltokenRelDistrMasterOnly,
	EdxltokenRelDistrHash,
	EdxltokenRelDistrRandom,
	EdxltokenRelDistrReplicated,
	EdxltokenConvertHashToRandom,

	EdxltokenRelDistrOpfamilies,
	EdxltokenRelDistrOpfamily,

	EdxltokenRelDistrOpclasses,
	EdxltokenRelDistrOpclass,

	EdxltokenRelExternalPartitions,
	EdxltokenRelExternalPartition,

	EdxltokenExtRelRejLimit,
	EdxltokenExtRelRejLimitInRows,
	EdxltokenExtRelFmtErrRel,

	EdxltokenMetadataColumns,
	EdxltokenMetadataColumn,
	EdxltokenColumnNullable,
	EdxltokenColumnDefaultValue,

	EdxltokenKeys,
	EdxltokenDistrColumns,

	EdxltokenIndexKeyCols,
	EdxltokenIndexIncludedCols,
	EdxltokenIndexClustered,
	EdxltokenIndexPartial,
	EdxltokenIndexPhysicalType,
	EdxltokenIndexType,
	EdxltokenIndexTypeBtree,
	EdxltokenIndexTypeBitmap,
	EdxltokenIndexTypeGist,
	EdxltokenIndexTypeGin,
	EdxltokenIndexItemType,

	EdxltokenOpfamily,
	EdxltokenOpfamilies,

	EdxltokenTypeInt4,
	EdxltokenTypeBool,

	EdxltokenMetadataIdList,
	EdxltokenIndexInfoList,
	EdxltokenIndexInfo,

	EdxltokenIndex,
	EdxltokenPartitions,
	EdxltokenPartition,

	EdxltokenConstraints,
	EdxltokenConstraint,

	EdxltokenCheckConstraints,
	EdxltokenCheckConstraint,
	EdxltokenPartConstraint,
	EdxltokenPartConstraintExprAbsent,
	EdxltokenDefaultPartition,
	EdxltokenPartConstraintUnbounded,

	EdxltokenMDType,
	EdxltokenMDTypeRedistributable,
	EdxltokenMDTypeHashable,
	EdxltokenMDTypeMergeJoinable,
	EdxltokenMDTypeComposite,
	EdxltokenMDTypeIsTextRelated,
	EdxltokenMDTypeFixedLength,
	EdxltokenMDTypeLength,
	EdxltokenMDTypeByValue,
	EdxltokenMDTypeDistrOpfamily,
	EdxltokenMDTypeLegacyDistrOpfamily,
	EdxltokenMDTypeEqOp,
	EdxltokenMDTypeNEqOp,
	EdxltokenMDTypeLTOp,
	EdxltokenMDTypeLEqOp,
	EdxltokenMDTypeGTOp,
	EdxltokenMDTypeGEqOp,
	EdxltokenMDTypeCompOp,
	EdxltokenMDTypeHashOp,
	EdxltokenMDTypeArray,
	EdxltokenMDTypeRelid,

	EdxltokenMDTypeAggMin,
	EdxltokenMDTypeAggMax,
	EdxltokenMDTypeAggAvg,
	EdxltokenMDTypeAggSum,
	EdxltokenMDTypeAggCount,

	EdxltokenSPQDBScalarOp,
	EdxltokenSPQDBScalarOpLeftTypeId,
	EdxltokenSPQDBScalarOpRightTypeId,
	EdxltokenSPQDBScalarOpResultTypeId,
	EdxltokenSPQDBScalarOpFuncId,
	EdxltokenSPQDBScalarOpCommOpId,
	EdxltokenSPQDBScalarOpInverseOpId,
	EdxltokenSPQDBScalarOpLTOpId,
	EdxltokenSPQDBScalarOpGTOpId,
	EdxltokenSPQDBScalarOpCmpType,
	EdxltokenSPQDBScalarOpHashOpfamily,
	EdxltokenSPQDBScalarOpLegacyHashOpfamily,

	EdxltokenCmpEq,
	EdxltokenCmpNeq,
	EdxltokenCmpLt,
	EdxltokenCmpLeq,
	EdxltokenCmpGt,
	EdxltokenCmpGeq,
	EdxltokenCmpIDF,
	EdxltokenCmpOther,

	EdxltokenReturnsNullOnNullInput,
	EdxltokenIsNDVPreserving,

	EdxltokenTriggers,
	EdxltokenTrigger,

	EdxltokenSPQDBTrigger,
	EdxltokenSPQDBTriggerRow,
	EdxltokenSPQDBTriggerBefore,
	EdxltokenSPQDBTriggerInsert,
	EdxltokenSPQDBTriggerDelete,
	EdxltokenSPQDBTriggerUpdate,
	EdxltokenSPQDBTriggerEnabled,

	EdxltokenSPQDBFunc,
	EdxltokenSPQDBFuncStability,
	EdxltokenSPQDBFuncStable,
	EdxltokenSPQDBFuncImmutable,
	EdxltokenSPQDBFuncVolatile,
	EdxltokenSPQDBFuncDataAccess,
	EdxltokenSPQDBFuncNoSQL,
	EdxltokenSPQDBFuncContainsSQL,
	EdxltokenSPQDBFuncReadsSQLData,
	EdxltokenSPQDBFuncModifiesSQLData,
	EdxltokenSPQDBFuncResultTypeId,
	EdxltokenSPQDBFuncReturnsSet,
	EdxltokenSPQDBFuncStrict,
	EdxltokenSPQDBFuncNDVPreserving,
	EdxltokenSPQDBFuncIsAllowedForPS,

	EdxltokenSPQDBCast,
	EdxltokenSPQDBCastBinaryCoercible,
	EdxltokenSPQDBCastSrcType,
	EdxltokenSPQDBCastDestType,
	EdxltokenSPQDBCastFuncId,
	EdxltokenSPQDBCastCoercePathType,
	EdxltokenSPQDBArrayCoerceCast,

	EdxltokenSPQDBMDScCmp,

	EdxltokenSPQDBAgg,
	EdxltokenSPQDBIsAggOrdered,
	EdxltokenSPQDBAggResultTypeId,
	EdxltokenSPQDBAggIntermediateResultTypeId,
	EdxltokenSPQDBAggSplittable,
	EdxltokenSPQDBAggHashAggCapable,

	EdxltokenEntireRow,

	EdxltokenScalarExpr,  // top level scalar expression
	EdxltokenQuery,
	EdxltokenQueryOutput,
	EdxltokenLogical,
	EdxltokenLogicalGet,
	EdxltokenLogicalExternalGet,
	EdxltokenLogicalProject,
	EdxltokenLogicalSelect,
	EdxltokenLogicalJoin,
	EdxltokenLogicalCTEProducer,
	EdxltokenLogicalCTEConsumer,
	EdxltokenCTEList,
	EdxltokenLogicalCTEAnchor,
	EdxltokenLogicalLimit,
	EdxltokenLogicalOutput,
	EdxltokenLogicalConstTable,
	EdxltokenLogicalGrpBy,
	EdxltokenLogicalSetOperation,
	EdxltokenLogicalTVF,
	EdxltokenLogicalWindow,

	EdxltokenLogicalInsert,
	EdxltokenLogicalDelete,
	EdxltokenLogicalUpdate,
	EdxltokenLogicalCTAS,
	EdxltokenPhysicalCTAS,
	EdxltokenPhysicalDMLInsert,
	EdxltokenPhysicalDMLDelete,
	EdxltokenPhysicalDMLUpdate,
	EdxltokenDirectDispatchInfo,
	EdxltokenDirectDispatchIsRaw,
	EdxltokenDirectDispatchKeyValue,
	EdxltokenPhysicalSplit,
	EdxltokenPhysicalRowTrigger,
	EdxltokenPhysicalAssert,

	EdxltokenErrorCode,
	EdxltokenErrorMessage,

	EdxltokenOnCommitAction,
	EdxltokenOnCommitNOOP,
	EdxltokenOnCommitPreserve,
	EdxltokenOnCommitDelete,
	EdxltokenOnCommitDrop,

	EdxltokenInsertCols,
	EdxltokenDeleteCols,
	EdxltokenNewCols,
	EdxltokenOldCols,

	EdxltokenCTEId,

	EdxltokenLogicalGrpCols,

	EdxltokenInputCols,
	EdxltokenCastAcrossInputs,

	EdxltokenLogicalUnion,
	EdxltokenLogicalUnionAll,
	EdxltokenLogicalIntersect,
	EdxltokenLogicalIntersectAll,
	EdxltokenLogicalDifference,
	EdxltokenLogicalDifferenceAll,

	EdxltokenIndexDescr,
	EdxltokenIndexName,
	EdxltokenIndexScanDirection,
	EdxltokenIndexScanDirectionForward,
	EdxltokenIndexScanDirectionBackward,
	EdxltokenIndexScanDirectionNoMovement,
	EdxltokenScalarIndexCondList,

	EdxltokenStackTrace,

	EdxltokenStatistics,
	EdxltokenStatsBaseRelation,
	EdxltokenStatsDerivedRelation,
	EdxltokenStatsDerivedColumn,
	EdxltokenStatsBucketLowerBound,
	EdxltokenStatsBucketUpperBound,
	EdxltokenStatsFrequency,
	EdxltokenStatsDistinct,
	EdxltokenStatsBoundClosed,

	// search strategy
	EdxltokenSearchStrategy,
	EdxltokenSearchStage,
	EdxltokenXform,
	EdxltokenTimeThreshold,
	EdxltokenCostThreshold,

	// cost model parameters
	EdxltokenCostParams,
	EdxltokenCostParam,
	EdxltokenCostParamLowerBound,
	EdxltokenCostParamUpperBound,

	EdxltokenTopLimitUnderDML,

	EdxltokenCtasOptionType,
	EdxltokenVarTypeModList,

	EdxltokenSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLTokens
//
//	@doc:
//		DXL tokens.
//
//---------------------------------------------------------------------------
class CDXLTokens
{
private:
	// element for mapping Edxltoken to WCHARs
	struct SWszMapElem
	{
		Edxltoken m_edxlt;
		const WCHAR *m_wsz;
	};

	// element for mapping Edxltoken to CWStringConst
	struct SStrMapElem
	{
		Edxltoken m_edxlt;
		CWStringConst *m_pstr;

		// ctor
		SStrMapElem() : m_edxlt(EdxltokenSentinel), m_pstr(NULL)
		{
		}

		// ctor
		SStrMapElem(Edxltoken edxlt, CWStringConst *str)
			: m_edxlt(edxlt), m_pstr(str)
		{
			SPQOS_ASSERT(edxlt < EdxltokenSentinel);
			SPQOS_ASSERT(str->IsValid());
		}

		//dtor
		~SStrMapElem()
		{
			SPQOS_DELETE(m_pstr);
		}
	};

	// element for mapping Edxltoken to XML string
	struct SXMLStrMapElem
	{
		Edxltoken m_edxlt;
		XMLCh *m_xmlsz;

		// ctor
		SXMLStrMapElem() : m_edxlt(EdxltokenSentinel), m_xmlsz(NULL)
		{
		}

		// ctor
		SXMLStrMapElem(Edxltoken edxlt, XMLCh *xml_val)
			: m_edxlt(edxlt), m_xmlsz(xml_val)
		{
			SPQOS_ASSERT(edxlt < EdxltokenSentinel);
			SPQOS_ASSERT(NULL != xml_val);
		}

		//dtor
		~SXMLStrMapElem()
		{
			SPQOS_DELETE_ARRAY(reinterpret_cast<BYTE *>(m_xmlsz));
		}
	};

	// array maintaining the mapping Edxltoken -> CWStringConst
	static SStrMapElem *m_pstrmap;

	// array maintaining the mapping Edxltoken -> XML string
	static SXMLStrMapElem *m_pxmlszmap;

	// memory pool -- not owned
	static CMemoryPool *m_mp;

	// local dxl memory manager
	static CDXLMemoryManager *m_dxl_memory_manager;

	// create a string in Xerces XMLCh* format
	static XMLCh *XmlstrFromWsz(const WCHAR *wsz);

public:
	// retrieve a token in CWStringConst and XMLCh* format, respectively
	static const CWStringConst *GetDXLTokenStr(Edxltoken token_type);

	static const XMLCh *XmlstrToken(Edxltoken token_type);

	// initialize constants. Must be called before constants are accessed.
	static void Init(CMemoryPool *mp);

	// cleanup tokens
	static void Terminate();
};

}  // namespace spqdxl

#endif	// !SPQDXL_dxltokens_H

// EOF
