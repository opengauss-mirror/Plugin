//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalStreamAgg.cpp
//
//	@doc:
//		Implementation of stream aggregation operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalStreamAgg.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpressionHandle.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::CPhysicalStreamAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalStreamAgg::CPhysicalStreamAgg(
	CMemoryPool *mp, CColRefArray *colref_array, CColRefArray *pdrspqcrMinimal,
	COperator::EGbAggType egbaggtype, BOOL fGeneratesDuplicates,
	CColRefArray *pdrspqcrArgDQA, BOOL fMultiStage, BOOL isAggFromSplitDQA,
	CLogicalGbAgg::EAggStage aggStage, BOOL should_enforce_distribution)
	: CPhysicalAgg(mp, colref_array, pdrspqcrMinimal, egbaggtype,
				   fGeneratesDuplicates, pdrspqcrArgDQA, fMultiStage,
				   isAggFromSplitDQA, aggStage, should_enforce_distribution),
	  m_pos(NULL)
{
	SPQOS_ASSERT(NULL != m_pdrspqcrMinimal);
	m_pcrsMinimalGrpCols = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrMinimal);
	InitOrderSpec(mp, m_pdrspqcrMinimal);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::InitOrderSpec
//
//	@doc:
//		Initialize the order spec using the given array of columns
//
//---------------------------------------------------------------------------
void
CPhysicalStreamAgg::InitOrderSpec(CMemoryPool *mp, CColRefArray *pdrspqcrOrder)
{
	SPQOS_ASSERT(NULL != pdrspqcrOrder);

	CRefCount::SafeRelease(m_pos);
	m_pos = SPQOS_NEW(mp) COrderSpec(mp);
	const ULONG size = pdrspqcrOrder->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRef *colref = (*pdrspqcrOrder)[ul];

		// TODO: 12/21/2011 - ; this seems broken: a colref must not embed
		// a pointer to a cached object
		spqmd::IMDId *mdid =
			colref->RetrieveType()->GetMdidForCmpType(IMDType::EcmptL);
		mdid->AddRef();

		m_pos->Append(mdid, colref, COrderSpec::EntLast);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::~CPhysicalStreamAgg
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalStreamAgg::~CPhysicalStreamAgg()
{
	m_pcrsMinimalGrpCols->Release();
	m_pos->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::PosCovering
//
//	@doc:
//		Construct order spec on grouping column so that it covers required
//		order spec, the function returns NULL if no covering order spec
//		can be created
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalStreamAgg::PosCovering(CMemoryPool *mp, COrderSpec *posRequired,
								CColRefArray *pdrspqcrGrp) const
{
	SPQOS_ASSERT(NULL != posRequired);

	if (0 == posRequired->UlSortColumns())
	{
		// required order must be non-empty
		return NULL;
	}

	// create a set of required sort columns
	CColRefSet *pcrsReqd = posRequired->PcrsUsed(mp);

	COrderSpec *pos = NULL;

	CColRefSet *pcrsGrpCols = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrGrp);
	if (pcrsGrpCols->ContainsAll(pcrsReqd))
	{
		// required order columns are included in grouping columns, we can
		// construct a covering order spec
		pos = SPQOS_NEW(mp) COrderSpec(mp);

		// extract order expressions from required order
		const ULONG ulReqdSortCols = posRequired->UlSortColumns();
		for (ULONG ul = 0; ul < ulReqdSortCols; ul++)
		{
			CColRef *colref = const_cast<CColRef *>(posRequired->Pcr(ul));
			IMDId *mdid = posRequired->GetMdIdSortOp(ul);
			COrderSpec::ENullTreatment ent = posRequired->Ent(ul);
			mdid->AddRef();
			pos->Append(mdid, colref, ent);
		}

		// augment order with remaining grouping columns
		const ULONG size = pdrspqcrGrp->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			CColRef *colref = (*pdrspqcrGrp)[ul];
			if (!pcrsReqd->FMember(colref))
			{
				IMDId *mdid =
					colref->RetrieveType()->GetMdidForCmpType(IMDType::EcmptL);
				mdid->AddRef();
				pos->Append(mdid, colref, COrderSpec::EntLast);
			}
		}
	}
	pcrsGrpCols->Release();
	pcrsReqd->Release();

	return pos;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::PosRequiredStreamAgg
//
//	@doc:
//		Compute required sort columns of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalStreamAgg::PosRequiredStreamAgg(CMemoryPool *mp,
										 CExpressionHandle &exprhdl,
										 COrderSpec *posRequired,
										 ULONG
#ifdef SPQOS_DEBUG
											 child_index
#endif	// SPQOS_DEBUG
										 ,
										 CColRefArray *pdrspqcrGrp) const
{
	SPQOS_ASSERT(0 == child_index);

	COrderSpec *pos = PosCovering(mp, posRequired, pdrspqcrGrp);
	if (NULL == pos)
	{
		// failed to find a covering order spec, use local order spec
		m_pos->AddRef();
		pos = m_pos;
	}

	// extract sort columns from order spec
	CColRefSet *pcrs = pos->PcrsUsed(mp);

	// get key collection of the relational child
	CKeyCollection *pkc = exprhdl.DeriveKeyCollection(0);

	if (NULL != pkc && pkc->FKey(pcrs, false /*fExactMatch*/))
	{
		CColRefSet *pcrsReqd = posRequired->PcrsUsed(m_mp);
		BOOL fUsesDefinedCols = FUnaryUsesDefinedColumns(pcrsReqd, exprhdl);
		pcrsReqd->Release();

		if (!fUsesDefinedCols)
		{
			// we are grouping on child's key,
			// stream agg does not need to sort child and we can pass through input spec
			pos->Release();
			posRequired->AddRef();
			pos = posRequired;
		}
	}
	pcrs->Release();

	return pos;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalStreamAgg::PosDerive(CMemoryPool *,  // mp
							  CExpressionHandle &exprhdl) const
{
	return PosDerivePassThruOuter(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalStreamAgg::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalStreamAgg::EpetOrder(CExpressionHandle &exprhdl,
							  const CEnfdOrder *peo) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	// get the order delivered by the stream agg node
	COrderSpec *pos = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pos();
	if (peo->FCompatible(pos))
	{
		// required order will be established by the stream agg operator
		return CEnfdProp::EpetUnnecessary;
	}

	// required order will be enforced on limit's output
	return CEnfdProp::EpetRequired;
}

// EOF
