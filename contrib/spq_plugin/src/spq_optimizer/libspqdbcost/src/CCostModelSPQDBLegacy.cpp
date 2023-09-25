//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CCostModelSPQDBLegacy.cpp
//
//	@doc:
//		Implementation of SPQDB's legacy cost model
//---------------------------------------------------------------------------

#include "spqdbcost/CCostModelSPQDBLegacy.h"

#include "spqopt/base/COrderSpec.h"
#include "spqopt/base/CWindowFrame.h"
#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPhysicalSequenceProject.h"

using namespace spqos;
using namespace spqdbcost;


// initialization of cost functions
const CCostModelSPQDBLegacy::SCostMapping CCostModelSPQDBLegacy::m_rgcm[] = {
	{COperator::EopPhysicalTableScan, CostScan},
	{COperator::EopPhysicalDynamicTableScan, CostScan},
	{COperator::EopPhysicalExternalScan, CostScan},

	{COperator::EopPhysicalIndexScan, CostIndexScan},
	{COperator::EopPhysicalDynamicIndexScan, CostIndexScan},
	{COperator::EopPhysicalIndexOnlyScan, CostIndexScan},
	{COperator::EopPhysicalBitmapTableScan, CostBitmapTableScan},
	{COperator::EopPhysicalDynamicBitmapTableScan, CostBitmapTableScan},

	{COperator::EopPhysicalSequenceProject, CostSequenceProject},

	{COperator::EopPhysicalCTEProducer, CostCTEProducer},
	{COperator::EopPhysicalCTEConsumer, CostCTEConsumer},
	{COperator::EopPhysicalConstTableGet, CostConstTableGet},
	{COperator::EopPhysicalDML, CostDML},

	{COperator::EopPhysicalHashAgg, CostHashAgg},
	{COperator::EopPhysicalHashAggDeduplicate, CostHashAgg},
	{COperator::EopPhysicalScalarAgg, CostScalarAgg},
	{COperator::EopPhysicalStreamAgg, CostStreamAgg},
	{COperator::EopPhysicalStreamAggDeduplicate, CostStreamAgg},

	{COperator::EopPhysicalSequence, CostSequence},

	{COperator::EopPhysicalSort, CostSort},

	{COperator::EopPhysicalTVF, CostTVF},

	{COperator::EopPhysicalSerialUnionAll, CostUnionAll},

	{COperator::EopPhysicalInnerHashJoin, CostHashJoin},
	{COperator::EopPhysicalLeftSemiHashJoin, CostHashJoin},
	{COperator::EopPhysicalLeftAntiSemiHashJoin, CostHashJoin},
	{COperator::EopPhysicalLeftAntiSemiHashJoinNotIn, CostHashJoin},
	{COperator::EopPhysicalLeftOuterHashJoin, CostHashJoin},
	{COperator::EopPhysicalRightOuterHashJoin, CostHashJoin},

	{COperator::EopPhysicalInnerIndexNLJoin, CostIndexNLJoin},
	{COperator::EopPhysicalLeftOuterIndexNLJoin, CostIndexNLJoin},

	{COperator::EopPhysicalMotionGather, CostMotion},
	{COperator::EopPhysicalMotionBroadcast, CostMotion},
	{COperator::EopPhysicalMotionHashDistribute, CostMotion},
	{COperator::EopPhysicalMotionRandom, CostMotion},
	{COperator::EopPhysicalMotionRoutedDistribute, CostMotion},

	{COperator::EopPhysicalInnerNLJoin, CostNLJoin},
	{COperator::EopPhysicalLeftSemiNLJoin, CostNLJoin},
	{COperator::EopPhysicalLeftAntiSemiNLJoin, CostNLJoin},
	{COperator::EopPhysicalLeftAntiSemiNLJoinNotIn, CostNLJoin},
	{COperator::EopPhysicalLeftOuterNLJoin, CostNLJoin},
	{COperator::EopPhysicalCorrelatedInnerNLJoin, CostNLJoin},
	{COperator::EopPhysicalCorrelatedLeftOuterNLJoin, CostNLJoin},
	{COperator::EopPhysicalCorrelatedLeftSemiNLJoin, CostNLJoin},
	{COperator::EopPhysicalCorrelatedInLeftSemiNLJoin, CostNLJoin},
	{COperator::EopPhysicalCorrelatedLeftAntiSemiNLJoin, CostNLJoin},
	{COperator::EopPhysicalCorrelatedNotInLeftAntiSemiNLJoin, CostNLJoin},
};

//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CCostModelSPQDBLegacy
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CCostModelSPQDBLegacy::CCostModelSPQDBLegacy(CMemoryPool *mp, ULONG ulSegments,
										   ICostModelParamsArray *pdrspqcp)
	: m_mp(mp), m_num_of_segments(ulSegments)
{
	SPQOS_ASSERT(0 < ulSegments);

	m_cost_model_params = SPQOS_NEW(mp) CCostModelParamsSPQDBLegacy(mp);
	SetParams(pdrspqcp);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::DRowsPerHost
//
//	@doc:
//		Return number of rows per host
//
//---------------------------------------------------------------------------
CDouble
CCostModelSPQDBLegacy::DRowsPerHost(CDouble dRowsTotal) const
{
	return dRowsTotal / m_num_of_segments;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::~CCostModelSPQDBLegacy
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CCostModelSPQDBLegacy::~CCostModelSPQDBLegacy()
{
	m_cost_model_params->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostTupleProcessing
//
//	@doc:
//		Cost of tuple processing
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostTupleProcessing(DOUBLE rows, DOUBLE width,
										  ICostModelParams *pcp)
{
	SPQOS_ASSERT(NULL != pcp);

	CDouble dTupProcBandwidth =
		pcp->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	return CCost((rows * width) / dTupProcBandwidth);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostUnary
//
//	@doc:
//		Helper function to return cost of a plan rooted by unary operator
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostUnary(DOUBLE rows, DOUBLE width, DOUBLE num_rebinds,
								DOUBLE *pdcostChildren, ICostModelParams *pcp)
{
	SPQOS_ASSERT(NULL != pcp);

	CCost costLocal =
		CCost(num_rebinds * CostTupleProcessing(rows, width, pcp).Get());
	CCost costChild = CostSum(pdcostChildren, 1 /*size*/);

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostSpooling
//
//	@doc:
//		Helper function to compute spooling cost
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostSpooling(DOUBLE rows, DOUBLE width,
								   DOUBLE num_rebinds, DOUBLE *pdcostChildren,
								   ICostModelParams *pcp)
{
	SPQOS_ASSERT(NULL != pcp);

	return CostUnary(rows, width, num_rebinds, pdcostChildren, pcp);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostRedistribute
//
//	@doc:
//		Cost of various redistribute motion operators
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostRedistribute(DOUBLE rows, DOUBLE width,
									   ICostModelParams *pcp)
{
	SPQOS_ASSERT(NULL != pcp);

	CDouble dNetBandwidth =
		pcp->PcpLookup(CCostModelParamsSPQDBLegacy::EcpNetBandwidth)->Get();
	SPQOS_ASSERT(0 < dNetBandwidth);

	return CCost(rows * width / dNetBandwidth);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::FUnary
//
//	@doc:
//		Check if given operator is unary
//
//---------------------------------------------------------------------------
BOOL
CCostModelSPQDBLegacy::FUnary(COperator::EOperatorId op_id)
{
	return COperator::EopPhysicalAssert == op_id ||
		   COperator::EopPhysicalComputeScalar == op_id ||
		   COperator::EopPhysicalFilter == op_id ||
		   COperator::EopPhysicalLimit == op_id ||
		   COperator::EopPhysicalPartitionSelector == op_id ||
		   COperator::EopPhysicalPartitionSelectorDML == op_id ||
		   COperator::EopPhysicalRowTrigger == op_id ||
		   COperator::EopPhysicalSplit == op_id ||
		   COperator::EopPhysicalSpool == op_id;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::Cost
//
//	@doc:
//		Add up array of costs
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostSum(DOUBLE *pdCost, ULONG size)
{
	SPQOS_ASSERT(NULL != pdCost);

	DOUBLE res = 1.0;
	for (ULONG ul = 0; ul < size; ul++)
	{
		res = res + pdCost[ul];
	}

	return CCost(res);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostCTEProducer
//
//	@doc:
//		Cost of CTE producer
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostCTEProducer(CMemoryPool *,  // mp
									  CExpressionHandle &exprhdl,
									  const CCostModelSPQDBLegacy *pcmspqdb,
									  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalCTEProducer == exprhdl.Pop()->Eopid());

	CCost cost = CostUnary(pci->Rows(), pci->Width(), pci->NumRebinds(),
						   pci->PdCost(), pcmspqdb->GetCostModelParams());

	// In SPQDB, the child of a ShareInputScan representing the producer can
	// only be a materialize or sort. Here, we check if a materialize node
	// needs to be added during DXL->PlStmt translation

	COperator *popChild = exprhdl.Pop(0 /*child_index*/);
	if (NULL == popChild)
	{
		// child operator is not known, this could happen when computing cost bound
		return cost;
	}
	COperator::EOperatorId op_id = popChild->Eopid();
	if (COperator::EopPhysicalSpool != op_id &&
		COperator::EopPhysicalSort != op_id)
	{
		// no materialize needed
		return cost;
	}

	// a materialize (spool) node is added during DXL->PlStmt translation,
	// we need to add the cost of writing the tuples to disk
	CCost costSpooling =
		CostSpooling(pci->Rows(), pci->Width(), pci->NumRebinds(),
					 pci->PdCost(), pcmspqdb->GetCostModelParams());

	return cost + costSpooling;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostCTEConsumer
//
//	@doc:
//		Cost of CTE consumer
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostCTEConsumer(CMemoryPool *,  // mp
									  CExpressionHandle &
#ifdef SPQOS_DEBUG
										  exprhdl
#endif	// SPQOS_DEBUG
									  ,
									  const CCostModelSPQDBLegacy *pcmspqdb,
									  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalCTEConsumer == exprhdl.Pop()->Eopid());

	CDouble dSeqIOBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpSeqIOBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dSeqIOBandwidth);

	return CCost(pci->NumRebinds() * pci->Rows() * pci->Width() /
				 dSeqIOBandwidth);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostConstTableGet
//
//	@doc:
//		Cost of const table get
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostConstTableGet(CMemoryPool *,	// mp
										CExpressionHandle &
#ifdef SPQOS_DEBUG
											exprhdl
#endif	// SPQOS_DEBUG
										,
										const CCostModelSPQDBLegacy *pcmspqdb,
										const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalConstTableGet == exprhdl.Pop()->Eopid());

	return CCost(pci->NumRebinds() *
				 CostTupleProcessing(pci->Rows(), pci->Width(),
									 pcmspqdb->GetCostModelParams())
					 .Get());
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostDML
//
//	@doc:
//		Cost of DML
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostDML(CMemoryPool *,  // mp
							  CExpressionHandle &
#ifdef SPQOS_DEBUG
								  exprhdl
#endif	// SPQOS_DEBUG
							  ,
							  const CCostModelSPQDBLegacy *pcmspqdb,
							  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalDML == exprhdl.Pop()->Eopid());

	CDouble dTupUpdateBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupUpdateBandwith)
			->Get();
	SPQOS_ASSERT(0 < dTupUpdateBandwidth);

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];

	CCost costLocal = CCost(pci->NumRebinds() * (num_rows_outer * dWidthOuter) /
							dTupUpdateBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostScalarAgg
//
//	@doc:
//		Cost of scalar agg
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostScalarAgg(CMemoryPool *,	// mp
									CExpressionHandle &
#ifdef SPQOS_DEBUG
										exprhdl
#endif	// SPQOS_DEBUG
									,
									const CCostModelSPQDBLegacy *pcmspqdb,
									const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalScalarAgg == exprhdl.Pop()->Eopid());

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];

	// local cost is the cost of processing 1 tuple (size of output) +
	// cost of processing N tuples (size of input)
	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	CCost costLocal =
		CCost(pci->NumRebinds() *
			  (1.0 * pci->Width() + num_rows_outer * dWidthOuter) /
			  dTupProcBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostStreamAgg
//
//	@doc:
//		Cost of stream agg
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostStreamAgg(CMemoryPool *,	// mp
									CExpressionHandle &
#ifdef SPQOS_DEBUG
										exprhdl
#endif	// SPQOS_DEBUG
									,
									const CCostModelSPQDBLegacy *pcmspqdb,
									const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);

#ifdef SPQOS_DEBUG
	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
	SPQOS_ASSERT(COperator::EopPhysicalStreamAgg == op_id ||
				COperator::EopPhysicalStreamAggDeduplicate == op_id);
#endif	// SPQOS_DEBUG

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];

	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	CCost costLocal =
		CCost(pci->NumRebinds() *
			  (pci->Rows() * pci->Width() + num_rows_outer * dWidthOuter) /
			  dTupProcBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostSequence
//
//	@doc:
//		Cost of sequence
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostSequence(CMemoryPool *,  // mp
								   CExpressionHandle &
#ifdef SPQOS_DEBUG
									   exprhdl
#endif	// SPQOS_DEBUG
								   ,
								   const CCostModelSPQDBLegacy *pcmspqdb,
								   const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalSequence == exprhdl.Pop()->Eopid());

	CCost costLocal = CCost(pci->NumRebinds() *
							CostTupleProcessing(pci->Rows(), pci->Width(),
												pcmspqdb->GetCostModelParams())
								.Get());
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostSort
//
//	@doc:
//		Cost of sort
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostSort(CMemoryPool *,  // mp
							   CExpressionHandle &
#ifdef SPQOS_DEBUG
								   exprhdl
#endif	// SPQOS_DEBUG
							   ,
							   const CCostModelSPQDBLegacy *pcmspqdb,
							   const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalSort == exprhdl.Pop()->Eopid());

	// log operation below
	CDouble rows = CDouble(std::max(1.0, pci->Rows()));
	CDouble num_rebinds = CDouble(pci->NumRebinds());
	CDouble width = CDouble(pci->Width());
	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	CCost costLocal = CCost(
		num_rebinds * (rows * rows.Log2() * width * width / dTupProcBandwidth));
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostTVF
//
//	@doc:
//		Cost of table valued function
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostTVF(CMemoryPool *,  // mp
							  CExpressionHandle &
#ifdef SPQOS_DEBUG
								  exprhdl
#endif	// SPQOS_DEBUG
							  ,
							  const CCostModelSPQDBLegacy *pcmspqdb,
							  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalTVF == exprhdl.Pop()->Eopid());

	return CCost(pci->NumRebinds() *
				 CostTupleProcessing(pci->Rows(), pci->Width(),
									 pcmspqdb->GetCostModelParams())
					 .Get());
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostUnionAll
//
//	@doc:
//		Cost of UnionAll
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostUnionAll(CMemoryPool *,  // mp
								   CExpressionHandle &
#ifdef SPQOS_DEBUG
									   exprhdl
#endif	// SPQOS_DEBUG
								   ,
								   const CCostModelSPQDBLegacy *pcmspqdb,
								   const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalSerialUnionAll == exprhdl.Pop()->Eopid());

	CCost costLocal = CCost(pci->NumRebinds() *
							CostTupleProcessing(pci->Rows(), pci->Width(),
												pcmspqdb->GetCostModelParams())
								.Get());
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostHashAgg
//
//	@doc:
//		Cost of hash agg
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostHashAgg(CMemoryPool *,  // mp
								  CExpressionHandle &
#ifdef SPQOS_DEBUG
									  exprhdl
#endif	// SPQOS_DEBUG
								  ,
								  const CCostModelSPQDBLegacy *pcmspqdb,
								  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);

#ifdef SPQOS_DEBUG
	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
	SPQOS_ASSERT(COperator::EopPhysicalHashAgg == op_id ||
				COperator::EopPhysicalHashAggDeduplicate == op_id);
#endif	// SPQOS_DEBUG

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];

	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	CDouble dHashFactor =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpHashFactor)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	CCost costLocal = CCost(pci->NumRebinds() *
							(pci->Rows() * pci->Width() +
							 dHashFactor * num_rows_outer * dWidthOuter) /
							dTupProcBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostHashJoin
//
//	@doc:
//		Cost of hash join
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostHashJoin(CMemoryPool *,  // mp
								   CExpressionHandle &
#ifdef SPQOS_DEBUG
									   exprhdl
#endif	// SPQOS_DEBUG
								   ,
								   const CCostModelSPQDBLegacy *pcmspqdb,
								   const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
#ifdef SPQOS_DEBUG
	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
	SPQOS_ASSERT(COperator::EopPhysicalInnerHashJoin == op_id ||
				COperator::EopPhysicalLeftSemiHashJoin == op_id ||
				COperator::EopPhysicalLeftAntiSemiHashJoin == op_id ||
				COperator::EopPhysicalLeftAntiSemiHashJoinNotIn == op_id ||
				COperator::EopPhysicalLeftOuterHashJoin == op_id ||
				COperator::EopPhysicalRightOuterHashJoin == op_id);
#endif	// SPQOS_DEBUG

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];
	DOUBLE dRowsInner = pci->PdRows()[1];
	DOUBLE dWidthInner = pci->GetWidth()[1];

	CDouble dHashFactor =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpHashFactor)
			->Get();
	CDouble dHashJoinFactor =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpHJFactor)
			->Get();
	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	CCost costLocal =
		CCost(pci->NumRebinds() *
			  (num_rows_outer * dWidthOuter +
			   dRowsInner * dWidthInner * dHashFactor * dHashJoinFactor) /
			  dTupProcBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costChild + costLocal;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostIndexNLJoin
//
//	@doc:
//		Cost of inner or outer index-nljoin
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostIndexNLJoin(CMemoryPool *,  // mp
									  CExpressionHandle &
#ifdef SPQOS_DEBUG
										  exprhdl
#endif	// SPQOS_DEBUG
									  ,
									  const CCostModelSPQDBLegacy *pcmspqdb,
									  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(
		COperator::EopPhysicalInnerIndexNLJoin == exprhdl.Pop()->Eopid() ||
		COperator::EopPhysicalLeftOuterIndexNLJoin == exprhdl.Pop()->Eopid());

	CCost costLocal = CCost(pci->NumRebinds() *
							CostTupleProcessing(pci->Rows(), pci->Width(),
												pcmspqdb->GetCostModelParams())
								.Get());
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostNLJoin
//
//	@doc:
//		Cost of nljoin
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostNLJoin(CMemoryPool *,	 // mp
								 CExpressionHandle &
#ifdef SPQOS_DEBUG
									 exprhdl
#endif	// SPQOS_DEBUG
								 ,
								 const CCostModelSPQDBLegacy *pcmspqdb,
								 const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(CUtils::FNLJoin(exprhdl.Pop()));

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];
	DOUBLE dRowsInner = pci->PdRows()[1];
	DOUBLE dWidthInner = pci->GetWidth()[1];

	CDouble dNLJFactor =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpNLJFactor)
			->Get();
	CDouble dNLJOuterFactor =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpNLJOuterFactor)
			->Get();
	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);
	SPQOS_ASSERT(0 < dNLJOuterFactor);
	SPQOS_ASSERT(0 < dNLJFactor);

	CCost costLocal = CCost(pci->NumRebinds() *
							(num_rows_outer * dWidthOuter * dRowsInner *
							 dWidthInner * dNLJOuterFactor) /
							dTupProcBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	CCost costTotal = CCost(costLocal + costChild);

	// amplify NLJ cost based on NLJ factor
	return CCost(costTotal * dNLJFactor);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostMotion
//
//	@doc:
//		Cost of motion
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostMotion(CMemoryPool *,	 // mp
								 CExpressionHandle &exprhdl,
								 const CCostModelSPQDBLegacy *pcmspqdb,
								 const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);

	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
	SPQOS_ASSERT(COperator::EopPhysicalMotionGather == op_id ||
				COperator::EopPhysicalMotionBroadcast == op_id ||
				COperator::EopPhysicalMotionHashDistribute == op_id ||
				COperator::EopPhysicalMotionRandom == op_id ||
				COperator::EopPhysicalMotionRoutedDistribute == op_id);

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];

	CCost costLocal(0);
	if (COperator::EopPhysicalMotionBroadcast == op_id)
	{
		// broadcast cost is amplified by the number of segments
		CDouble dNetBandwidth =
			pcmspqdb->GetCostModelParams()
				->PcpLookup(CCostModelParamsSPQDBLegacy::EcpNetBandwidth)
				->Get();
		SPQOS_ASSERT(0 < dNetBandwidth);

		costLocal = CCost(pci->NumRebinds() * num_rows_outer * dWidthOuter *
						  pcmspqdb->UlHosts() / dNetBandwidth);
	}
	else
	{
		costLocal = CCost(pci->NumRebinds() *
						  CostRedistribute(num_rows_outer, dWidthOuter,
										   pcmspqdb->GetCostModelParams())
							  .Get());
	}
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostSequenceProject
//
//	@doc:
//		Cost of sequence project
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostSequenceProject(CMemoryPool *,  // mp
										  CExpressionHandle &exprhdl,
										  const CCostModelSPQDBLegacy *pcmspqdb,
										  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(COperator::EopPhysicalSequenceProject ==
				exprhdl.Pop()->Eopid());

	DOUBLE num_rows_outer = pci->PdRows()[0];
	DOUBLE dWidthOuter = pci->GetWidth()[0];

	ULONG ulSortCols = 0;
	COrderSpecArray *pdrspqos =
		CPhysicalSequenceProject::PopConvert(exprhdl.Pop())->Pdrspqos();
	const ULONG ulOrderSpecs = pdrspqos->Size();
	for (ULONG ul = 0; ul < ulOrderSpecs; ul++)
	{
		COrderSpec *pos = (*pdrspqos)[ul];
		ulSortCols += pos->UlSortColumns();
	}

	CDouble dTupProcBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpTupProcBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dTupProcBandwidth);

	// we process (sorted window of) input tuples to compute window function values
	CCost costLocal =
		CCost(pci->NumRebinds() * (ulSortCols * num_rows_outer * dWidthOuter) /
			  dTupProcBandwidth);
	CCost costChild = CostSum(pci->PdCost(), pci->ChildCount());

	return costLocal + costChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostIndexScan
//
//	@doc:
//		Cost of index scan
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostIndexScan(CMemoryPool *,	// mp
									CExpressionHandle &exprhdl,
									const CCostModelSPQDBLegacy *pcmspqdb,
									const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);

	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
	SPQOS_ASSERT(COperator::EopPhysicalIndexScan == op_id ||
				COperator::EopPhysicalIndexOnlyScan == op_id ||
				COperator::EopPhysicalDynamicIndexScan == op_id);

	CDouble dRandomIOBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpRandomIOBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dRandomIOBandwidth);

	switch (op_id)
	{
		case COperator::EopPhysicalDynamicIndexScan:
		case COperator::EopPhysicalIndexScan:
		case COperator::EopPhysicalIndexOnlyScan:
			return CCost(pci->NumRebinds() * (pci->Rows() * pci->Width()) /
						 dRandomIOBandwidth);

		default:
			SPQOS_ASSERT(!"invalid index scan");
			return CCost(0);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostBitmapTableScan
//
//	@doc:
//		Cost of bitmap table scan
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostBitmapTableScan(CMemoryPool *,  // mp,
										  CExpressionHandle &
#ifdef SPQOS_DEBUG
											  exprhdl
#endif	// SPQOS_DEBUG
										  ,
										  const CCostModelSPQDBLegacy *pcmspqdb,
										  const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);
	SPQOS_ASSERT(
		COperator::EopPhysicalBitmapTableScan == exprhdl.Pop()->Eopid() ||
		COperator::EopPhysicalDynamicBitmapTableScan == exprhdl.Pop()->Eopid());

	CDouble dSeqIOBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpSeqIOBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dSeqIOBandwidth);

	// TODO: ; 2014-04-11; compute the real cost here
	return CCost(pci->NumRebinds() * (pci->Rows() * pci->Width()) /
				 dSeqIOBandwidth * 0.5);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::CostScan
//
//	@doc:
//		Cost of scan
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::CostScan(CMemoryPool *,  // mp
							   CExpressionHandle &exprhdl,
							   const CCostModelSPQDBLegacy *pcmspqdb,
							   const SCostingInfo *pci)
{
	SPQOS_ASSERT(NULL != pcmspqdb);
	SPQOS_ASSERT(NULL != pci);

	COperator *pop = exprhdl.Pop();
	COperator::EOperatorId op_id = pop->Eopid();
	SPQOS_ASSERT(COperator::EopPhysicalTableScan == op_id ||
				COperator::EopPhysicalDynamicTableScan == op_id ||
				COperator::EopPhysicalExternalScan == op_id);

	CDouble dSeqIOBandwidth =
		pcmspqdb->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDBLegacy::EcpSeqIOBandwidth)
			->Get();
	SPQOS_ASSERT(0 < dSeqIOBandwidth);

	switch (op_id)
	{
		case COperator::EopPhysicalTableScan:
		case COperator::EopPhysicalDynamicTableScan:
		case COperator::EopPhysicalExternalScan:
			return CCost(pci->NumRebinds() * (pci->Rows() * pci->Width()) /
						 dSeqIOBandwidth);

		default:
			SPQOS_ASSERT(!"invalid index scan");
			return CCost(0);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelSPQDBLegacy::Cost
//
//	@doc:
//		Main driver
//
//---------------------------------------------------------------------------
CCost
CCostModelSPQDBLegacy::Cost(
	CExpressionHandle &exprhdl,	 // handle gives access to expression properties
	const SCostingInfo *pci) const
{
	SPQOS_ASSERT(NULL != pci);

	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
	if (FUnary(op_id))
	{
		return CostUnary(pci->Rows(), pci->Width(), pci->NumRebinds(),
						 pci->PdCost(), m_cost_model_params);
	}

	FnCost *pfnc = NULL;
	const ULONG size = SPQOS_ARRAY_SIZE(m_rgcm);

	// find the cost function corresponding to the given operator
	for (ULONG ul = 0; pfnc == NULL && ul < size; ul++)
	{
		if (op_id == m_rgcm[ul].m_eopid)
		{
			pfnc = m_rgcm[ul].m_pfnc;
		}
	}
	SPQOS_ASSERT(NULL != pfnc);

	return pfnc(m_mp, exprhdl, this, pci);
}

// EOF
