//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalScan.cpp
//
//	@doc:
//		Implementation of base scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalScan.h"

#include "spqos/base.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::CPhysicalScan
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CPhysicalScan::CPhysicalScan(CMemoryPool *mp, const CName *pnameAlias,
							 CTableDescriptor *ptabdesc,
							 CColRefArray *pdrspqcrOutput)
	: CPhysical(mp),
	  m_pnameAlias(pnameAlias),
	  m_ptabdesc(ptabdesc),
	  m_pdrspqcrOutput(pdrspqcrOutput),
	  m_pds(NULL),
	  m_pstatsBaseTable(NULL)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameAlias);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	if (ptabdesc->ConvertHashToRandom())
	{
		// Treating a hash distributed table as random during planning
		m_pds = SPQOS_NEW(m_mp) CDistributionSpecRandom();
	}
	else
	{
		m_pds = CPhysical::PdsCompute(m_mp, ptabdesc, pdrspqcrOutput,
									  NULL /* spq_segment_id */);
	}
	ComputeTableStats(m_mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::~CPhysicalScan
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CPhysicalScan::~CPhysicalScan()
{
	m_ptabdesc->Release();
	m_pdrspqcrOutput->Release();
	m_pds->Release();
	m_pstatsBaseTable->Release();
	SPQOS_DELETE(m_pnameAlias);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::FInputOrderSensitive
//
//	@doc:
//		Not called for leaf operators
//
//---------------------------------------------------------------------------
BOOL
CPhysicalScan::FInputOrderSensitive() const
{
	SPQOS_ASSERT(!"Unexpected function call of FInputOrderSensitive");
	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalScan::FProvidesReqdCols(CExpressionHandle &,  // exprhdl
								 CColRefSet *pcrsRequired,
								 ULONG	// ulOptReq
) const
{
	SPQOS_ASSERT(NULL != pcrsRequired);

	CColRefSet *pcrs = SPQOS_NEW(m_mp) CColRefSet(m_mp);
	pcrs->Include(m_pdrspqcrOutput);

	BOOL result = pcrs->ContainsAll(pcrsRequired);
	pcrs->Release();

	return result;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalScan::EpetOrder(CExpressionHandle &,  // exprhdl
						 const CEnfdOrder *
#ifdef SPQOS_DEBUG
							 peo
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	return CEnfdProp::EpetRequired;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalScan::PdsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	BOOL fIndexOrBitmapScan =
		COperator::EopPhysicalIndexScan == Eopid() ||
        COperator::EopPhysicalShareIndexScan == Eopid() ||  /* SPQ: for shareindexscan */
		COperator::EopPhysicalBitmapTableScan == Eopid() ||
		COperator::EopPhysicalDynamicIndexScan == Eopid() ||
		COperator::EopPhysicalDynamicBitmapTableScan == Eopid();
	if (fIndexOrBitmapScan && CDistributionSpec::EdtHashed == m_pds->Edt() &&
		exprhdl.HasOuterRefs())
	{
		// If index conditions have outer references and the index relation is hashed,
		// check to see if we can derive an equivalent hashed distribution for the
		// outer references. For multi-distribution key tables with an index, it is
		// possible for a spec to have a column from both the inner and the outer
		// table. This is termed "incomplete" and added as an equivalent spec.
		//
		// For example, if we have foo (a, b) distributed by (a,b)
		//                         bar (c, d) distributed by (c, d)
		// with an index on idx_bar_d, if we have the query
		// 		select * from foo join bar on a = c and b = d,
		// it is possible to get a spec of [a, d].
		//
		// An incomplete spec is relevant only when we have an index join on a
		// multi-key distributed table. This is handled either by completing the
		// equivalent spec using Filter predicates above (see
		// CPhysicalFilter::PdsDerive()), or by discarding an incomplete spec at
		// the index join (see CPhysicalJoin::PdsDerive()).
		//
		// This way the equiv spec stays incomplete only as long as it needs to be.

		CExpression *pexprIndexPred = exprhdl.PexprScalarExactChild(
			0 /*child_index*/, true /*error_on_null_return*/);

		CDistributionSpecHashed *pdshashed =
			CDistributionSpecHashed::PdsConvert(m_pds);
		CDistributionSpecHashed *pdshashedEquiv =
			CDistributionSpecHashed::TryToCompleteEquivSpec(
				mp, pdshashed, pexprIndexPred, exprhdl.DeriveOuterReferences());

		if (NULL != pdshashedEquiv)
		{
			CExpressionArray *pdrspqexprHashed = pdshashed->Pdrspqexpr();
			pdrspqexprHashed->AddRef();
			if (NULL != pdshashed->Opfamilies())
			{
				pdshashed->Opfamilies()->AddRef();
			}
			CDistributionSpecHashed *pdshashedResult =
				SPQOS_NEW(mp) CDistributionSpecHashed(
					pdrspqexprHashed, pdshashed->FNullsColocated(),
					pdshashedEquiv, pdshashed->Opfamilies());

			return pdshashedResult;
		}
	}

	m_pds->AddRef();

	return m_pds;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::PpimDeriveFromDynamicScan
//
//	@doc:
//		Derive partition index map from a dynamic scan operator
//
//---------------------------------------------------------------------------
CPartIndexMap *
CPhysicalScan::PpimDeriveFromDynamicScan(CMemoryPool *mp, ULONG part_idx_id,
										 IMDId *rel_mdid,
										 CColRef2dArray *pdrspqdrspqcrPart,
										 ULONG ulSecondaryPartIndexId,
										 CPartConstraint *ppartcnstr,
										 CPartConstraint *ppartcnstrRel,
										 ULONG ulExpectedPropagators)
{
	CPartIndexMap *ppim = SPQOS_NEW(mp) CPartIndexMap(mp);
	UlongToPartConstraintMap *ppartcnstrmap =
		SPQOS_NEW(mp) UlongToPartConstraintMap(mp);

	(void) ppartcnstrmap->Insert(SPQOS_NEW(mp) ULONG(ulSecondaryPartIndexId),
								 ppartcnstr);

	CPartKeysArray *pdrspqpartkeys = SPQOS_NEW(mp) CPartKeysArray(mp);
	pdrspqpartkeys->Append(SPQOS_NEW(mp) CPartKeys(pdrspqdrspqcrPart));

	ppim->Insert(part_idx_id, ppartcnstrmap, CPartIndexMap::EpimConsumer,
				 ulExpectedPropagators, rel_mdid, pdrspqpartkeys, ppartcnstrRel);

	return ppim;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::EpetDistribution
//
//	@doc:
//		Return the enforcing type for distribution property based on this
//		operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalScan::EpetDistribution(CExpressionHandle & /*exprhdl*/,
								const CEnfdDistribution *ped) const
{
	SPQOS_ASSERT(NULL != ped);

	if (ped->FCompatible(m_pds))
	{
		// required distribution will be established by the operator
		return CEnfdProp::EpetUnnecessary;
	}

	// required distribution will be enforced on output
	return CEnfdProp::EpetRequired;
}



//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::ComputeTableStats
//
//	@doc:
//		Compute stats of underlying table
//
//---------------------------------------------------------------------------
void
CPhysicalScan::ComputeTableStats(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL == m_pstatsBaseTable);

	CColRefSet *pcrsHist = SPQOS_NEW(mp) CColRefSet(mp);
	CColRefSet *pcrsWidth = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrOutput);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_pstatsBaseTable =
		md_accessor->Pstats(mp, m_ptabdesc->MDId(), pcrsHist, pcrsWidth);
	SPQOS_ASSERT(NULL != m_pstatsBaseTable);

	pcrsHist->Release();
	pcrsWidth->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScan::PopConvert
//
//	@doc:
//		Conversion function
//
//---------------------------------------------------------------------------
CPhysicalScan *
CPhysicalScan::PopConvert(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(CUtils::FPhysicalScan(pop));

	return dynamic_cast<CPhysicalScan *>(pop);
}


// EOF
