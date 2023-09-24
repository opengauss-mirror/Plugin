//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CLogicalDynamicGetBase.cpp
//
//	@doc:
//		Implementation of dynamic table access base class
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDynamicGetBase.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/statistics/CFilterStatsProcessor.h"
#include "naucrates/statistics/CStatsPredUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::CLogicalDynamicGetBase
//
//	@doc:
//		ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDynamicGetBase::CLogicalDynamicGetBase(CMemoryPool *mp)
	: CLogical(mp),
	  m_pnameAlias(NULL),
	  m_ptabdesc(NULL),
	  m_scan_id(0),
	  m_pdrspqcrOutput(NULL),
	  m_pdrspqdrspqcrPart(NULL),
	  m_ulSecondaryScanId(0),
	  m_is_partial(false),
	  m_part_constraint(NULL),
	  m_ppartcnstrRel(NULL),
	  m_pcrsDist(NULL)
{
	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::CLogicalDynamicGetBase
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalDynamicGetBase::CLogicalDynamicGetBase(
	CMemoryPool *mp, const CName *pnameAlias, CTableDescriptor *ptabdesc,
	ULONG scan_id, CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrPart,
	ULONG ulSecondaryScanId, BOOL is_partial, CPartConstraint *ppartcnstr,
	CPartConstraint *ppartcnstrRel)
	: CLogical(mp),
	  m_pnameAlias(pnameAlias),
	  m_ptabdesc(ptabdesc),
	  m_scan_id(scan_id),
	  m_pdrspqcrOutput(pdrspqcrOutput),
	  m_pdrspqdrspqcrPart(pdrspqdrspqcrPart),
	  m_ulSecondaryScanId(ulSecondaryScanId),
	  m_is_partial(is_partial),
	  m_part_constraint(ppartcnstr),
	  m_ppartcnstrRel(ppartcnstrRel),
	  m_pcrsDist(NULL)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameAlias);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	SPQOS_ASSERT(NULL != pdrspqdrspqcrPart);
	SPQOS_ASSERT(NULL != ppartcnstr);
	SPQOS_ASSERT(NULL != ppartcnstrRel);

	SPQOS_ASSERT_IMP(scan_id != ulSecondaryScanId, NULL != ppartcnstr);
	SPQOS_ASSERT_IMP(is_partial,
					NULL != m_part_constraint->PcnstrCombined() &&
						"Partial scan with unsupported constraint type");

	m_pcrsDist = CLogical::PcrsDist(mp, m_ptabdesc, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::CLogicalDynamicGetBase
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalDynamicGetBase::CLogicalDynamicGetBase(CMemoryPool *mp,
											   const CName *pnameAlias,
											   CTableDescriptor *ptabdesc,
											   ULONG scan_id,
											   CColRefArray *pdrspqcrOutput)
	: CLogical(mp),
	  m_pnameAlias(pnameAlias),
	  m_ptabdesc(ptabdesc),
	  m_scan_id(scan_id),
	  m_pdrspqcrOutput(pdrspqcrOutput),
	  m_ulSecondaryScanId(scan_id),
	  m_is_partial(false),
	  m_part_constraint(NULL),
	  m_ppartcnstrRel(NULL),
	  m_pcrsDist(NULL)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameAlias);

	// generate a default column set for the table descriptor if not passed in
	if (NULL == m_pdrspqcrOutput)
	{
		m_pdrspqcrOutput = PdrspqcrCreateMapping(mp, m_ptabdesc->Pdrspqcoldesc(),
											   UlOpId(), m_ptabdesc->MDId());
	}
	m_pdrspqdrspqcrPart = PdrspqdrspqcrCreatePartCols(mp, m_pdrspqcrOutput,
												  m_ptabdesc->PdrspqulPart());

	// generate a constraint "true"
	UlongToConstraintMap *phmulcnstr = CUtils::PhmulcnstrBoolConstOnPartKeys(
		mp, m_pdrspqdrspqcrPart, true /*value*/);
	CBitSet *pbsDefaultParts = CUtils::PbsAllSet(mp, m_pdrspqdrspqcrPart->Size());
	m_pdrspqdrspqcrPart->AddRef();
	m_part_constraint =
		SPQOS_NEW(mp) CPartConstraint(mp, phmulcnstr, pbsDefaultParts,
									 true /*is_unbounded*/, m_pdrspqdrspqcrPart);
	m_part_constraint->AddRef();
	m_ppartcnstrRel = m_part_constraint;

	m_pcrsDist = CLogical::PcrsDist(mp, m_ptabdesc, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::~CLogicalDynamicGetBase
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CLogicalDynamicGetBase::~CLogicalDynamicGetBase()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcrOutput);
	CRefCount::SafeRelease(m_pdrspqdrspqcrPart);
	CRefCount::SafeRelease(m_part_constraint);
	CRefCount::SafeRelease(m_ppartcnstrRel);
	CRefCount::SafeRelease(m_pcrsDist);

	SPQOS_DELETE(m_pnameAlias);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalDynamicGetBase::DeriveOutputColumns(CMemoryPool *mp,
											CExpressionHandle &	 // exprhdl
)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(m_pdrspqcrOutput);

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalDynamicGetBase::DeriveKeyCollection(CMemoryPool *mp,
											CExpressionHandle &	 // exprhdl
) const
{
	const CBitSetArray *pdrspqbs = m_ptabdesc->PdrspqbsKeys();

	return CLogical::PkcKeysBaseTable(mp, pdrspqbs, m_pdrspqcrOutput);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::DerivePropertyConstraint
//
//	@doc:
//		Derive constraint property
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalDynamicGetBase::DerivePropertyConstraint(CMemoryPool *mp,
												 CExpressionHandle &  // exprhdl
) const
{
	return PpcDeriveConstraintFromTable(mp, m_ptabdesc, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::PpartinfoDerive
//
//	@doc:
//		Derive partition consumer info
//
//---------------------------------------------------------------------------
CPartInfo *
CLogicalDynamicGetBase::DerivePartitionInfo(CMemoryPool *mp,
											CExpressionHandle &	 // exprhdl
) const
{
	IMDId *mdid = m_ptabdesc->MDId();
	mdid->AddRef();
	m_pdrspqdrspqcrPart->AddRef();
	m_ppartcnstrRel->AddRef();

	CPartInfo *ppartinfo = SPQOS_NEW(mp) CPartInfo(mp);
	ppartinfo->AddPartConsumer(mp, m_scan_id, mdid, m_pdrspqdrspqcrPart,
							   m_ppartcnstrRel);

	return ppartinfo;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::SetPartConstraint
//
//	@doc:
//		Set part constraint
//
//---------------------------------------------------------------------------
void
CLogicalDynamicGetBase::SetPartConstraint(CPartConstraint *ppartcnstr)
{
	SPQOS_ASSERT(NULL != ppartcnstr);
	SPQOS_ASSERT(NULL != m_part_constraint);
	SPQOS_ASSERT_IMP(m_is_partial,
					NULL != ppartcnstr->PcnstrCombined() &&
						"Partial scan with unsupported constraint type");

	m_part_constraint->Release();
	m_part_constraint = ppartcnstr;

	m_ppartcnstrRel->Release();
	ppartcnstr->AddRef();
	m_ppartcnstrRel = ppartcnstr;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::SetSecondaryScanId
//
//	@doc:
//		Set secondary scan id
//
//---------------------------------------------------------------------------
void
CLogicalDynamicGetBase::SetSecondaryScanId(ULONG scan_id)
{
	m_ulSecondaryScanId = scan_id;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::SetPartial
//
//	@doc:
//		Set partial to true
//
//---------------------------------------------------------------------------
void
CLogicalDynamicGetBase::SetPartial()
{
	SPQOS_ASSERT(!IsPartial());
	SPQOS_ASSERT(NULL != m_part_constraint->PcnstrCombined() &&
				"Partial scan with unsupported constraint type");
	m_is_partial = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDynamicGetBase::PstatsDeriveFilter
//
//	@doc:
//		Derive stats from base table using filters on partition and/or index columns
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDynamicGetBase::PstatsDeriveFilter(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CExpression *pexprFilter) const
{
	CExpression *pexprFilterNew = NULL;
	CConstraint *pcnstr = m_part_constraint->PcnstrCombined();
	if (m_is_partial && NULL != pcnstr && !pcnstr->IsConstraintUnbounded())
	{
		if (NULL == pexprFilter)
		{
			pexprFilterNew = pcnstr->PexprScalar(mp);
			pexprFilterNew->AddRef();
		}
		else
		{
			pexprFilterNew = CPredicateUtils::PexprConjunction(
				mp, pexprFilter, pcnstr->PexprScalar(mp));
		}
	}
	else if (NULL != pexprFilter)
	{
		pexprFilterNew = pexprFilter;
		pexprFilterNew->AddRef();
	}

	CColRefSet *pcrsStat = SPQOS_NEW(mp) CColRefSet(mp);

	if (NULL != pexprFilterNew)
	{
		pcrsStat->Include(pexprFilterNew->DeriveUsedColumns());
	}

	// requesting statistics on distribution columns to estimate data skew
	if (NULL != m_pcrsDist)
	{
		pcrsStat->Include(m_pcrsDist);
	}


	CStatistics *pstatsFullTable = dynamic_cast<CStatistics *>(
		PstatsBaseTable(mp, exprhdl, m_ptabdesc, pcrsStat));

	pcrsStat->Release();

	if (NULL == pexprFilterNew || pexprFilterNew->DeriveHasSubquery())
	{
		return pstatsFullTable;
	}

	CStatsPred *pred_stats = CStatsPredUtils::ExtractPredStats(
		mp, pexprFilterNew, NULL /*outer_refs*/
	);
	pexprFilterNew->Release();

	IStatistics *result_stats = CFilterStatsProcessor::MakeStatsFilter(
		mp, pstatsFullTable, pred_stats, true /* do_cap_NDVs */);
	pred_stats->Release();
	pstatsFullTable->Release();

	return result_stats;
}

// EOF
