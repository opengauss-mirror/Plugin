//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CCostModelParamsSPQDB.cpp
//
//	@doc:
//		Parameters of SPQDB cost model
//---------------------------------------------------------------------------

#include "spqdbcost/CCostModelParamsSPQDB.h"

#include "spqos/base.h"
#include "spqos/string/CWStringConst.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CCostModelParamsSPQDB);

// sequential i/o bandwidth
const CDouble CCostModelParamsSPQDB::DSeqIOBandwidthVal = 1024.0;

// random i/o bandwidth
const CDouble CCostModelParamsSPQDB::DRandomIOBandwidthVal = 30.0;

// tuple processing bandwidth
const CDouble CCostModelParamsSPQDB::DTupProcBandwidthVal = 512.0;

// output bandwidth
const CDouble CCostModelParamsSPQDB::DOutputBandwidthVal = 256.0;

// scan initialization cost factor
const CDouble CCostModelParamsSPQDB::DInitScanFacorVal = 431.0;

// table scan cost unit
const CDouble CCostModelParamsSPQDB::DTableScanCostUnitVal = 5.50e-07;

// index scan initialization cost factor
const CDouble CCostModelParamsSPQDB::DInitIndexScanFactorVal = 142.0;

// index block cost unit
const CDouble CCostModelParamsSPQDB::DIndexBlockCostUnitVal = 1.27e-06;

// index filtering cost unit
const CDouble CCostModelParamsSPQDB::DIndexFilterCostUnitVal = 1.65e-04;

// index scan cost unit per tuple per width
const CDouble CCostModelParamsSPQDB::DIndexScanTupCostUnitVal = 3.66e-06;

// index scan random IO factor
const CDouble CCostModelParamsSPQDB::DIndexScanTupRandomFactorVal = 6.0;

// filter column cost unit
const CDouble CCostModelParamsSPQDB::DFilterColCostUnitVal = 3.29e-05;

// output tuple cost unit
const CDouble CCostModelParamsSPQDB::DOutputTupCostUnitVal = 1.86e-06;

// sending tuple cost unit in gather motion
const CDouble CCostModelParamsSPQDB::DGatherSendCostUnitVal = 4.58e-06;

// receiving tuple cost unit in gather motion
const CDouble CCostModelParamsSPQDB::DGatherRecvCostUnitVal = 2.20e-06;

// sending tuple cost unit in redistribute motion
const CDouble CCostModelParamsSPQDB::DRedistributeSendCostUnitVal = 2.33e-06;

// receiving tuple cost unit in redistribute motion
const CDouble CCostModelParamsSPQDB::DRedistributeRecvCostUnitVal = 8.0e-07;

// sending tuple cost unit in broadcast motion
const CDouble CCostModelParamsSPQDB::DBroadcastSendCostUnitVal = 4.965e-05;

// receiving tuple cost unit in broadcast motion
const CDouble CCostModelParamsSPQDB::DBroadcastRecvCostUnitVal = 1.35e-06;

// tuple cost unit in No-Op motion
const CDouble CCostModelParamsSPQDB::DNoOpCostUnitVal = 0;

// feeding cost per tuple per column in join operator
const CDouble CCostModelParamsSPQDB::DJoinFeedingTupColumnCostUnitVal = 8.69e-05;

// feeding cost per tuple per width in join operator
const CDouble CCostModelParamsSPQDB::DJoinFeedingTupWidthCostUnitVal = 6.09e-07;

// output cost per tuple in join operator
const CDouble CCostModelParamsSPQDB::DJoinOutputTupCostUnitVal = 3.50e-06;

// memory threshold for hash join spilling (in bytes)
const CDouble CCostModelParamsSPQDB::DHJSpillingMemThresholdVal =
	50 * 1024 * 1024;

// initial cost for building hash table for hash join
const CDouble CCostModelParamsSPQDB::DHJHashTableInitCostFactorVal = 500.0;

// building hash table cost per tuple per column
const CDouble CCostModelParamsSPQDB::DHJHashTableColumnCostUnitVal = 5.0e-05;

// the unit cost to process each tuple with unit width when building a hash table
const CDouble CCostModelParamsSPQDB::DHJHashTableWidthCostUnitVal = 3.0e-06;

// hashing cost per tuple with unit width in hash join
const CDouble CCostModelParamsSPQDB::DHJHashingTupWidthCostUnitVal = 1.97e-05;

// feeding cost per tuple per column in hash join if spilling
const CDouble CCostModelParamsSPQDB::DHJFeedingTupColumnSpillingCostUnitVal =
	1.97e-04;

// feeding cost per tuple with unit width in hash join if spilling
const CDouble CCostModelParamsSPQDB::DHJFeedingTupWidthSpillingCostUnitVal =
	3.0e-06;

// hashing cost per tuple with unit width in hash join if spilling
const CDouble CCostModelParamsSPQDB::DHJHashingTupWidthSpillingCostUnitVal =
	2.30e-05;

// cost for building hash table for per tuple per grouping column in hash aggregate
const CDouble CCostModelParamsSPQDB::DHashAggInputTupColumnCostUnitVal =
	1.20e-04;

// cost for building hash table for per tuple with unit width in hash aggregate
const CDouble CCostModelParamsSPQDB::DHashAggInputTupWidthCostUnitVal = 1.12e-07;

// cost for outputting for per tuple with unit width in hash aggregate
const CDouble CCostModelParamsSPQDB::DHashAggOutputTupWidthCostUnitVal =
	5.61e-07;

// sorting cost per tuple with unit width
const CDouble CCostModelParamsSPQDB::DSortTupWidthCostUnitVal = 5.67e-06;

// cost for processing per tuple with unit width
const CDouble CCostModelParamsSPQDB::DTupDefaultProcCostUnitVal = 1.0e-06;

// cost for materializing per tuple with unit width
const CDouble CCostModelParamsSPQDB::DMaterializeCostUnitVal = 4.68e-06;

// tuple update bandwidth
const CDouble CCostModelParamsSPQDB::DTupUpdateBandwidthVal = 256.0;

// network bandwidth
const CDouble CCostModelParamsSPQDB::DNetBandwidthVal = 1024.0;

// number of segments
const CDouble CCostModelParamsSPQDB::DSegmentsVal = 4.0;

// nlj factor
const CDouble CCostModelParamsSPQDB::DNLJFactorVal = 1.0;

// hj factor
const CDouble CCostModelParamsSPQDB::DHJFactorVal = 2.5;

// hash building factor
const CDouble CCostModelParamsSPQDB::DHashFactorVal = 2.0;

// default cost
const CDouble CCostModelParamsSPQDB::DDefaultCostVal = 100.0;

// largest estimation risk for which we don't penalize index join
const CDouble CCostModelParamsSPQDB::DIndexJoinAllowedRiskThreshold = 3;

// default bitmap IO co-efficient when NDV is larger
const CDouble CCostModelParamsSPQDB::DBitmapIOCostLargeNDV(0.0082);

// default bitmap IO co-efficient when NDV is smaller
const CDouble CCostModelParamsSPQDB::DBitmapIOCostSmallNDV(0.2138);

// default bitmap page cost when NDV is larger
const CDouble CCostModelParamsSPQDB::DBitmapPageCostLargeNDV(83.1651);

// default bitmap page cost when NDV is larger
const CDouble CCostModelParamsSPQDB::DBitmapPageCostSmallNDV(204.3810);

// default bitmap page cost with no assumption about NDV
const CDouble CCostModelParamsSPQDB::DBitmapPageCost(10);

// default threshold of NDV for bitmap costing
const CDouble CCostModelParamsSPQDB::DBitmapNDVThreshold(200);

// cost of a bitmap scan rebind
const CDouble CCostModelParamsSPQDB::DBitmapScanRebindCost(0.06);

// see CCostModelSPQDB::CostHashJoin() for why this is needed
const CDouble CCostModelParamsSPQDB::DPenalizeHJSkewUpperLimit(10.0);

// default scalar func cost
const CDouble CCostModelParamsSPQDB::DScalarFuncCost(1.0e-04);

/* SPQ: for ShareScan */
const CDouble CCostModelParamsSPQDB::DSPQTableShareScanFactor(1.0);

/* SPQ: for shareindexscan */
const CDouble CCostModelParamsSPQDB::DSPQShareIndexScanFactor(1.0);

/* SPQ: for HashJoin InnerCost*/
const CDouble CCostModelParamsSPQDB::DSPQHJInnerCostFactor(1.0);

#define SPQOPT_COSTPARAM_NAME_MAX_LENGTH 80

// parameter names in the same order of param enumeration
const CHAR rgszCostParamNames[CCostModelParamsSPQDB::EcpSentinel]
							 [SPQOPT_COSTPARAM_NAME_MAX_LENGTH] = {
								 "SeqIOBandwidth",
								 "RandomIOBandwidth",
								 "TupProcBandwidth",
								 "OutputBandwidth",
								 "InitScanFacor",
								 "TableScanCostUnit",
								 "InitIndexScanFactor",
								 "IndexBlockCostUnit",
								 "IndexFilterCostUnit",
								 "IndexScanTupCostUnit",
								 "IndexScanTupRandomFactor",
								 "FilterColCostUnit",
								 "OutputTupCostUnit",
								 "GatherSendCostUnit",
								 "GatherRecvCostUnit",
								 "RedistributeSendCostUnit",
								 "RedistributeRecvCostUnit",
								 "BroadcastSendCostUnit",
								 "BroadcastRecvCostUnit",
								 "NoOpCostUnit",
								 "JoinFeedingTupColumnCostUnit",
								 "JoinFeedingTupWidthCostUnit",
								 "JoinOutputTupCostUnit",
								 "HJSpillingMemThreshold",
								 "HJHashTableInitCostFactor",
								 "HJHashTableColumnCostUnit",
								 "HJHashTableWidthCostUnit",
								 "HJHashingTupWidthCostUnit",
								 "HJFeedingTupColumnSpillingCostUnit",
								 "HJFeedingTupWidthSpillingCostUnit",
								 "HJHashingTupWidthSpillingCostUnit",
								 "HashAggInputTupColumnCostUnit",
								 "HashAggInputTupWidthCostUnit",
								 "HashAggOutputTupWidthCostUnit",
								 "SortTupWidthCostUnit",
								 "TupDefaultProcCostUnit",
								 "MaterializeCostUnit",
								 "TupUpdateBandwidth",
								 "NetworkBandwidth",
								 "Segments",
								 "NLJFactor",
								 "HJFactor",
								 "HashFactor",
								 "DefaultCost",
								 "IndexJoinAllowedRiskThreshold",
								 "BitmapIOLargerNDV",
								 "BitmapIOSmallerNDV",
								 "BitmapPageCostLargerNDV",
								 "BitmapPageCostSmallerNDV",
								 "BitmapNDVThreshold",
								 "ScalarFuncCostUnit",
                                 "SPQTableShareScanFactor", /* SPQ: for ShareScan */
                                 "SPQShareIndexScanFactor", /* SPQ: for shareindexscan */
								 "SPQHJInnerCostFactor",  /* SPQ: for HashJoin InnerCost*/
};

//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::CCostModelParamsSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDB::CCostModelParamsSPQDB(CMemoryPool *mp) : m_mp(mp)
{
	SPQOS_ASSERT(NULL != mp);

	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		m_rspqcp[ul] = NULL;
	}

	// populate param array with default param values
	m_rspqcp[EcpSeqIOBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpSeqIOBandwidth, DSeqIOBandwidthVal,
				   DSeqIOBandwidthVal - 128.0, DSeqIOBandwidthVal + 128.0);
	m_rspqcp[EcpRandomIOBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpRandomIOBandwidth, DRandomIOBandwidthVal,
				   DRandomIOBandwidthVal - 8.0, DRandomIOBandwidthVal + 8.0);
	m_rspqcp[EcpTupProcBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpTupProcBandwidth, DTupProcBandwidthVal,
				   DTupProcBandwidthVal - 32.0, DTupProcBandwidthVal + 32.0);
	m_rspqcp[EcpOutputBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpOutputBandwidth, DOutputBandwidthVal,
				   DOutputBandwidthVal - 32.0, DOutputBandwidthVal + 32.0);
	m_rspqcp[EcpInitScanFactor] = SPQOS_NEW(mp)
		SCostParam(EcpInitScanFactor, DInitScanFacorVal,
				   DInitScanFacorVal - 2.0, DInitScanFacorVal + 2.0);
	m_rspqcp[EcpTableScanCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpTableScanCostUnit, DTableScanCostUnitVal,
				   DTableScanCostUnitVal - 1.0, DTableScanCostUnitVal + 1.0);
	m_rspqcp[EcpInitIndexScanFactor] = SPQOS_NEW(mp) SCostParam(
		EcpInitIndexScanFactor, DInitIndexScanFactorVal,
		DInitIndexScanFactorVal - 1.0, DInitIndexScanFactorVal + 1.0);
	m_rspqcp[EcpIndexBlockCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpIndexBlockCostUnit, DIndexBlockCostUnitVal,
				   DIndexBlockCostUnitVal - 1.0, DIndexBlockCostUnitVal + 1.0);
	m_rspqcp[EcpIndexFilterCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpIndexFilterCostUnit, DIndexFilterCostUnitVal,
		DIndexFilterCostUnitVal - 1.0, DIndexFilterCostUnitVal + 1.0);
	m_rspqcp[EcpIndexScanTupCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpIndexScanTupCostUnit, DIndexScanTupCostUnitVal,
		DIndexScanTupCostUnitVal - 1.0, DIndexScanTupCostUnitVal + 1.0);
	m_rspqcp[EcpIndexScanTupRandomFactor] = SPQOS_NEW(mp) SCostParam(
		EcpIndexScanTupRandomFactor, DIndexScanTupRandomFactorVal,
		DIndexScanTupRandomFactorVal - 1.0, DIndexScanTupRandomFactorVal + 1.0);
	m_rspqcp[EcpFilterColCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpFilterColCostUnit, DFilterColCostUnitVal,
				   DFilterColCostUnitVal - 1.0, DFilterColCostUnitVal + 1.0);
	m_rspqcp[EcpOutputTupCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpOutputTupCostUnit, DOutputTupCostUnitVal,
				   DOutputTupCostUnitVal - 1.0, DOutputTupCostUnitVal + 1.0);
	m_rspqcp[EcpGatherSendCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpGatherSendCostUnit, DGatherSendCostUnitVal,
				   DGatherSendCostUnitVal - 0.0, DGatherSendCostUnitVal + 0.0);
	m_rspqcp[EcpGatherRecvCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpGatherRecvCostUnit, DGatherRecvCostUnitVal,
				   DGatherRecvCostUnitVal - 0.0, DGatherRecvCostUnitVal + 0.0);
	m_rspqcp[EcpRedistributeSendCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpRedistributeSendCostUnit, DRedistributeSendCostUnitVal,
		DRedistributeSendCostUnitVal - 0.0, DRedistributeSendCostUnitVal + 0.0);
	m_rspqcp[EcpRedistributeRecvCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpRedistributeRecvCostUnit, DRedistributeRecvCostUnitVal,
		DRedistributeRecvCostUnitVal - 0.0, DRedistributeRecvCostUnitVal + 0.0);
	m_rspqcp[EcpBroadcastSendCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpBroadcastSendCostUnit, DBroadcastSendCostUnitVal,
		DBroadcastSendCostUnitVal - 0.0, DBroadcastSendCostUnitVal + 0.0);
	m_rspqcp[EcpBroadcastRecvCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpBroadcastRecvCostUnit, DBroadcastRecvCostUnitVal,
		DBroadcastRecvCostUnitVal - 0.0, DBroadcastRecvCostUnitVal + 0.0);
	m_rspqcp[EcpNoOpCostUnit] =
		SPQOS_NEW(mp) SCostParam(EcpNoOpCostUnit, DNoOpCostUnitVal,
								DNoOpCostUnitVal - 0.0, DNoOpCostUnitVal + 0.0);
	m_rspqcp[EcpJoinFeedingTupColumnCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpJoinFeedingTupColumnCostUnit, DJoinFeedingTupColumnCostUnitVal,
		DJoinFeedingTupColumnCostUnitVal - 0.0,
		DJoinFeedingTupColumnCostUnitVal + 0.0);
	m_rspqcp[EcpJoinFeedingTupWidthCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpJoinFeedingTupWidthCostUnit, DJoinFeedingTupWidthCostUnitVal,
		DJoinFeedingTupWidthCostUnitVal - 0.0,
		DJoinFeedingTupWidthCostUnitVal + 0.0);
	m_rspqcp[EcpJoinOutputTupCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpJoinOutputTupCostUnit, DJoinOutputTupCostUnitVal,
		DJoinOutputTupCostUnitVal - 0.0, DJoinOutputTupCostUnitVal + 0.0);
	m_rspqcp[EcpHJSpillingMemThreshold] = SPQOS_NEW(mp) SCostParam(
		EcpHJSpillingMemThreshold, DHJSpillingMemThresholdVal,
		DHJSpillingMemThresholdVal - 0.0, DHJSpillingMemThresholdVal + 0.0);
	m_rspqcp[EcpHJHashTableInitCostFactor] = SPQOS_NEW(mp)
		SCostParam(EcpHJHashTableInitCostFactor, DHJHashTableInitCostFactorVal,
				   DHJHashTableInitCostFactorVal - 0.0,
				   DHJHashTableInitCostFactorVal + 0.0);
	m_rspqcp[EcpHJHashTableColumnCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpHJHashTableColumnCostUnit, DHJHashTableColumnCostUnitVal,
				   DHJHashTableColumnCostUnitVal - 0.0,
				   DHJHashTableColumnCostUnitVal + 0.0);
	m_rspqcp[EcpHJHashTableWidthCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpHJHashTableWidthCostUnit, DHJHashTableWidthCostUnitVal,
		DHJHashTableWidthCostUnitVal - 0.0, DHJHashTableWidthCostUnitVal + 0.0);
	m_rspqcp[EcpHJHashingTupWidthCostUnit] = SPQOS_NEW(mp)
		SCostParam(EcpHJHashingTupWidthCostUnit, DHJHashingTupWidthCostUnitVal,
				   DHJHashingTupWidthCostUnitVal - 0.0,
				   DHJHashingTupWidthCostUnitVal + 0.0);
	m_rspqcp[EcpHJFeedingTupColumnSpillingCostUnit] =
		SPQOS_NEW(mp) SCostParam(EcpHJFeedingTupColumnSpillingCostUnit,
								DHJFeedingTupColumnSpillingCostUnitVal,
								DHJFeedingTupColumnSpillingCostUnitVal - 0.0,
								DHJFeedingTupColumnSpillingCostUnitVal + 0.0);
	m_rspqcp[EcpHJFeedingTupWidthSpillingCostUnit] =
		SPQOS_NEW(mp) SCostParam(EcpHJFeedingTupWidthSpillingCostUnit,
								DHJFeedingTupWidthSpillingCostUnitVal,
								DHJFeedingTupWidthSpillingCostUnitVal - 0.0,
								DHJFeedingTupWidthSpillingCostUnitVal + 0.0);
	m_rspqcp[EcpHJHashingTupWidthSpillingCostUnit] =
		SPQOS_NEW(mp) SCostParam(EcpHJHashingTupWidthSpillingCostUnit,
								DHJHashingTupWidthSpillingCostUnitVal,
								DHJHashingTupWidthSpillingCostUnitVal - 0.0,
								DHJHashingTupWidthSpillingCostUnitVal + 0.0);
	m_rspqcp[EcpHashAggInputTupColumnCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpHashAggInputTupColumnCostUnit, DHashAggInputTupColumnCostUnitVal,
		DHashAggInputTupColumnCostUnitVal - 0.0,
		DHashAggInputTupColumnCostUnitVal + 0.0);
	m_rspqcp[EcpHashAggInputTupWidthCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpHashAggInputTupWidthCostUnit, DHashAggInputTupWidthCostUnitVal,
		DHashAggInputTupWidthCostUnitVal - 0.0,
		DHashAggInputTupWidthCostUnitVal + 0.0);
	m_rspqcp[EcpHashAggOutputTupWidthCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpHashAggOutputTupWidthCostUnit, DHashAggOutputTupWidthCostUnitVal,
		DHashAggOutputTupWidthCostUnitVal - 0.0,
		DHashAggOutputTupWidthCostUnitVal + 0.0);
	m_rspqcp[EcpSortTupWidthCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpSortTupWidthCostUnit, DSortTupWidthCostUnitVal,
		DSortTupWidthCostUnitVal - 0.0, DSortTupWidthCostUnitVal + 0.0);
	m_rspqcp[EcpTupDefaultProcCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpTupDefaultProcCostUnit, DTupDefaultProcCostUnitVal,
		DTupDefaultProcCostUnitVal - 0.0, DTupDefaultProcCostUnitVal + 0.0);
	m_rspqcp[EcpMaterializeCostUnit] = SPQOS_NEW(mp) SCostParam(
		EcpMaterializeCostUnit, DMaterializeCostUnitVal,
		DMaterializeCostUnitVal - 0.0, DMaterializeCostUnitVal + 0.0);
	m_rspqcp[EcpTupUpdateBandwith] = SPQOS_NEW(mp) SCostParam(
		EcpTupUpdateBandwith, DTupUpdateBandwidthVal,
		DTupUpdateBandwidthVal - 32.0, DTupUpdateBandwidthVal + 32.0);
	m_rspqcp[EcpNetBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpNetBandwidth, DNetBandwidthVal, DNetBandwidthVal - 128.0,
				   DNetBandwidthVal + 128.0);
	m_rspqcp[EcpSegments] = SPQOS_NEW(mp) SCostParam(
		EcpSegments, DSegmentsVal, DSegmentsVal - 2.0, DSegmentsVal + 2.0);
	m_rspqcp[EcpNLJFactor] = SPQOS_NEW(mp) SCostParam(
		EcpNLJFactor, DNLJFactorVal, DNLJFactorVal - 0.5, DNLJFactorVal + 0.5);
	m_rspqcp[EcpHJFactor] = SPQOS_NEW(mp) SCostParam(
		EcpHJFactor, DHJFactorVal, DHJFactorVal - 1.0, DHJFactorVal + 1.0);
	m_rspqcp[EcpHashFactor] =
		SPQOS_NEW(mp) SCostParam(EcpHashFactor, DHashFactorVal,
								DHashFactorVal - 1.0, DHashFactorVal + 1.0);
	m_rspqcp[EcpDefaultCost] =
		SPQOS_NEW(mp) SCostParam(EcpDefaultCost, DDefaultCostVal,
								DDefaultCostVal - 32.0, DDefaultCostVal + 32.0);
	m_rspqcp[EcpIndexJoinAllowedRiskThreshold] = SPQOS_NEW(mp)
		SCostParam(EcpIndexJoinAllowedRiskThreshold,
				   DIndexJoinAllowedRiskThreshold, 0, spqos::ulong_max);
	m_rspqcp[EcpBitmapIOCostLargeNDV] = SPQOS_NEW(mp) SCostParam(
		EcpBitmapIOCostLargeNDV, DBitmapIOCostLargeNDV,
		DBitmapIOCostLargeNDV - 0.0001, DBitmapIOCostLargeNDV + 0.0001);
	m_rspqcp[EcpBitmapIOCostSmallNDV] = SPQOS_NEW(mp) SCostParam(
		EcpBitmapIOCostSmallNDV, DBitmapIOCostSmallNDV,
		DBitmapIOCostSmallNDV - 0.0001, DBitmapIOCostSmallNDV + 0.0001);
	m_rspqcp[EcpBitmapPageCostLargeNDV] = SPQOS_NEW(mp) SCostParam(
		EcpBitmapPageCostLargeNDV, DBitmapPageCostLargeNDV,
		DBitmapPageCostLargeNDV - 1.0, DBitmapPageCostLargeNDV + 1.0);
	m_rspqcp[EcpBitmapPageCostSmallNDV] = SPQOS_NEW(mp) SCostParam(
		EcpBitmapPageCostSmallNDV, DBitmapPageCostSmallNDV,
		DBitmapPageCostSmallNDV - 1.0, DBitmapPageCostSmallNDV + 1.0);
	m_rspqcp[EcpBitmapPageCost] =
		SPQOS_NEW(mp) SCostParam(EcpBitmapPageCost, DBitmapPageCost,
								DBitmapPageCost - 1.0, DBitmapPageCost + 1.0);
	m_rspqcp[EcpBitmapNDVThreshold] = SPQOS_NEW(mp)
		SCostParam(EcpBitmapNDVThreshold, DBitmapNDVThreshold,
				   DBitmapNDVThreshold - 1.0, DBitmapNDVThreshold + 1.0);
	m_rspqcp[EcpBitmapScanRebindCost] = SPQOS_NEW(mp)
		SCostParam(EcpBitmapScanRebindCost, DBitmapScanRebindCost,
				   DBitmapScanRebindCost - 1.0, DBitmapScanRebindCost + 1.0);
	m_rspqcp[EcpPenalizeHJSkewUpperLimit] = SPQOS_NEW(mp) SCostParam(
		EcpPenalizeHJSkewUpperLimit, DPenalizeHJSkewUpperLimit,
		DPenalizeHJSkewUpperLimit - 1.0, DPenalizeHJSkewUpperLimit + 1.0);

	m_rspqcp[EcpScalarFuncCost] =
		SPQOS_NEW(mp) SCostParam(EcpScalarFuncCost, DScalarFuncCost,
								DScalarFuncCost - 0.0, DScalarFuncCost + 0.0);
    /* SPQ: for ShareScan */
    m_rspqcp[EcpSPQTableShareScanFactor] = SPQOS_NEW(mp) SCostParam(
        EcpSPQTableShareScanFactor, DSPQTableShareScanFactor,
        DSPQTableShareScanFactor - 0.0, DSPQTableShareScanFactor + 0.0);
    /* SPQ: for shareindexscan */
    m_rspqcp[EcpSPQShareIndexScanFactor] = SPQOS_NEW(mp) SCostParam(
        EcpSPQShareIndexScanFactor, DSPQShareIndexScanFactor,
        DSPQShareIndexScanFactor - 0.0, DSPQShareIndexScanFactor + 0.0);
    /* SPQ: for HashJoin OuterWidth*/
    m_rspqcp[EcpHJInnerCostFactor] = SPQOS_NEW(mp) SCostParam(
        EcpHJInnerCostFactor, DSPQHJInnerCostFactor,
        DSPQHJInnerCostFactor - 0.0, DSPQHJInnerCostFactor + 0.0);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::~CCostModelParamsSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDB::~CCostModelParamsSPQDB()
{
	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		SPQOS_DELETE(m_rspqcp[ul]);
		m_rspqcp[ul] = NULL;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::PcpLookup
//
//	@doc:
//		Lookup param by id;
//
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDB::SCostParam *
CCostModelParamsSPQDB::PcpLookup(ULONG id) const
{
	ECostParam ecp = (ECostParam) id;
	SPQOS_ASSERT(EcpSentinel > ecp);

	return m_rspqcp[ecp];
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::PcpLookup
//
//	@doc:
//		Lookup param by name;
//		return NULL if name is not recognized
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDB::SCostParam *
CCostModelParamsSPQDB::PcpLookup(const CHAR *szName) const
{
	SPQOS_ASSERT(NULL != szName);

	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		if (0 == clib::Strcmp(szName, rgszCostParamNames[ul]))
		{
			return PcpLookup((ECostParam) ul);
		}
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::SetParam
//
//	@doc:
//		Set param by id
//
//---------------------------------------------------------------------------
void
CCostModelParamsSPQDB::SetParam(ULONG id, CDouble dVal, CDouble dLowerBound,
							   CDouble dUpperBound)
{
	ECostParam ecp = (ECostParam) id;
	SPQOS_ASSERT(EcpSentinel > ecp);

	SPQOS_DELETE(m_rspqcp[ecp]);
	m_rspqcp[ecp] = NULL;
	m_rspqcp[ecp] =
		SPQOS_NEW(m_mp) SCostParam(ecp, dVal, dLowerBound, dUpperBound);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::SetParam
//
//	@doc:
//		Set param by name
//
//---------------------------------------------------------------------------
void
CCostModelParamsSPQDB::SetParam(const CHAR *szName, CDouble dVal,
							   CDouble dLowerBound, CDouble dUpperBound)
{
	SPQOS_ASSERT(NULL != szName);

	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		if (0 == clib::Strcmp(szName, rgszCostParamNames[ul]))
		{
			SPQOS_DELETE(m_rspqcp[ul]);
			m_rspqcp[ul] = NULL;
			m_rspqcp[ul] =
				SPQOS_NEW(m_mp) SCostParam(ul, dVal, dLowerBound, dUpperBound);

			return;
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDB::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CCostModelParamsSPQDB::OsPrint(IOstream &os) const
{
	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		SCostParam *pcp = PcpLookup((ECostParam) ul);
		os << rgszCostParamNames[ul] << " : " << pcp->Get() << "  ["
		   << pcp->GetLowerBoundVal() << "," << pcp->GetUpperBoundVal() << "]"
		   << std::endl;
	}
	return os;
}

BOOL
CCostModelParamsSPQDB::Equals(ICostModelParams *pcm) const
{
	CCostModelParamsSPQDB *pcmgOther = dynamic_cast<CCostModelParamsSPQDB *>(pcm);
	if (NULL == pcmgOther)
		return false;

	for (ULONG ul = 0U; ul < SPQOS_ARRAY_SIZE(m_rspqcp); ul++)
	{
		if (!m_rspqcp[ul]->Equals(pcmgOther->m_rspqcp[ul]))
			return false;
	}

	return true;
}

const CHAR *
CCostModelParamsSPQDB::SzNameLookup(ULONG id) const
{
	ECostParam ecp = (ECostParam) id;
	SPQOS_ASSERT(EcpSentinel > ecp);
	return rgszCostParamNames[ecp];
}

// EOF
