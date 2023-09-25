//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalSequenceProject.cpp
//
//	@doc:
//		Implementation of physical sequence project operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalSequenceProject.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/base/CWindowFrame.h"
#include "spqopt/cost/ICostModel.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CScalarIdent.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::CPhysicalSequenceProject
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalSequenceProject::CPhysicalSequenceProject(CMemoryPool *mp,
												   CDistributionSpec *pds,
												   COrderSpecArray *pdrspqos,
												   CWindowFrameArray *pdrspqwf)
	: CPhysical(mp),
	  m_pds(pds),
	  m_pdrspqos(pdrspqos),
	  m_pdrspqwf(pdrspqwf),
	  m_pos(NULL),
	  m_pcrsRequiredLocal(NULL)
{
	SPQOS_ASSERT(NULL != pds);
	SPQOS_ASSERT(NULL != pdrspqos);
	SPQOS_ASSERT(NULL != pdrspqwf);
	SPQOS_ASSERT(CDistributionSpec::EdtHashed == pds->Edt() ||
				CDistributionSpec::EdtSingleton == pds->Edt());
	// we don't create LogicalSequenceProject with equivalent hashed distribution specs at this time
	if (CDistributionSpec::EdtHashed == pds->Edt())
	{
		SPQOS_ASSERT(NULL ==
					CDistributionSpecHashed::PdsConvert(pds)->PdshashedEquiv());
	}
	CreateOrderSpec(mp);
	ComputeRequiredLocalColumns(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::CreateOrderSpec
//
//	@doc:
//		Create local order spec that we request relational child to satisfy
//
//---------------------------------------------------------------------------
void
CPhysicalSequenceProject::CreateOrderSpec(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL == m_pos);
	SPQOS_ASSERT(NULL != m_pds);
	SPQOS_ASSERT(NULL != m_pdrspqos);

	m_pos = SPQOS_NEW(mp) COrderSpec(mp);

	// add partition by keys to order spec
	if (CDistributionSpec::EdtHashed == m_pds->Edt())
	{
		CDistributionSpecHashed *pdshashed =
			CDistributionSpecHashed::PdsConvert(m_pds);

		const CExpressionArray *pdrspqexpr = pdshashed->Pdrspqexpr();
		const ULONG size = pdrspqexpr->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			CExpression *pexpr = (*pdrspqexpr)[ul];
			// we assume partition-by keys are always scalar idents
			CScalarIdent *popScId = CScalarIdent::PopConvert(pexpr->Pop());
			const CColRef *colref = popScId->Pcr();

			spqmd::IMDId *mdid =
				colref->RetrieveType()->GetMdidForCmpType(IMDType::EcmptL);
			mdid->AddRef();

			m_pos->Append(mdid, colref, COrderSpec::EntLast);
		}
	}

	if (0 == m_pdrspqos->Size())
	{
		return;
	}

	COrderSpec *posFirst = (*m_pdrspqos)[0];
#ifdef SPQOS_DEBUG
	const ULONG length = m_pdrspqos->Size();
	for (ULONG ul = 1; ul < length; ul++)
	{
		COrderSpec *posCurrent = (*m_pdrspqos)[ul];
		SPQOS_ASSERT(posFirst->FSatisfies(posCurrent) &&
					"first order spec must satisfy all other order specs");
	}
#endif	// SPQOS_DEBUG

	// we assume here that the first order spec in the children array satisfies all other
	// order specs in the array, this happens as part of the initial normalization
	// so we need to add columns only from the first order spec
	const ULONG size = posFirst->UlSortColumns();
	for (ULONG ul = 0; ul < size; ul++)
	{
		const CColRef *colref = posFirst->Pcr(ul);
		spqmd::IMDId *mdid = posFirst->GetMdIdSortOp(ul);
		mdid->AddRef();
		COrderSpec::ENullTreatment ent = posFirst->Ent(ul);
		m_pos->Append(mdid, colref, ent);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::ComputeRequiredLocalColumns
//
//	@doc:
//		Compute local required columns
//
//---------------------------------------------------------------------------
void
CPhysicalSequenceProject::ComputeRequiredLocalColumns(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != m_pos);
	SPQOS_ASSERT(NULL != m_pds);
	SPQOS_ASSERT(NULL != m_pdrspqos);
	SPQOS_ASSERT(NULL != m_pdrspqwf);
	SPQOS_ASSERT(NULL == m_pcrsRequiredLocal);

	m_pcrsRequiredLocal = m_pos->PcrsUsed(mp);
	if (CDistributionSpec::EdtHashed == m_pds->Edt())
	{
		CColRefSet *pcrsHashed =
			CDistributionSpecHashed::PdsConvert(m_pds)->PcrsUsed(mp);
		m_pcrsRequiredLocal->Include(pcrsHashed);
		pcrsHashed->Release();
	}

	// add the columns used in the window frames
	const ULONG size = m_pdrspqwf->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CWindowFrame *pwf = (*m_pdrspqwf)[ul];
		if (NULL != pwf->PexprLeading())
		{
			m_pcrsRequiredLocal->Union(
				pwf->PexprLeading()->DeriveUsedColumns());
		}
		if (NULL != pwf->PexprTrailing())
		{
			m_pcrsRequiredLocal->Union(
				pwf->PexprTrailing()->DeriveUsedColumns());
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::~CPhysicalSequenceProject
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalSequenceProject::~CPhysicalSequenceProject()
{
	m_pds->Release();
	m_pdrspqos->Release();
	m_pdrspqwf->Release();
	m_pos->Release();
	m_pcrsRequiredLocal->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::Matches
//
//	@doc:
//		Match operators
//
//---------------------------------------------------------------------------
BOOL
CPhysicalSequenceProject::Matches(COperator *pop) const
{
	SPQOS_ASSERT(NULL != pop);
	if (Eopid() == pop->Eopid())
	{
		CPhysicalSequenceProject *popPhysicalSequenceProject =
			CPhysicalSequenceProject::PopConvert(pop);
		return m_pds->Matches(popPhysicalSequenceProject->Pds()) &&
			   CWindowFrame::Equals(m_pdrspqwf,
									popPhysicalSequenceProject->Pdrspqwf()) &&
			   COrderSpec::Equals(m_pdrspqos,
								  popPhysicalSequenceProject->Pdrspqos());
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::HashValue
//
//	@doc:
//		Hashing function
//
//---------------------------------------------------------------------------
ULONG
CPhysicalSequenceProject::HashValue() const
{
	ULONG ulHash = 0;
	ulHash = spqos::CombineHashes(ulHash, m_pds->HashValue());
	ulHash = spqos::CombineHashes(
		ulHash, CWindowFrame::HashValue(m_pdrspqwf, 3 /*ulMaxSize*/));
	ulHash = spqos::CombineHashes(
		ulHash, COrderSpec::HashValue(m_pdrspqos, 3 /*ulMaxSize*/));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PcrsRequired
//
//	@doc:
//		Compute required columns of the n-th child;
//		we only compute required columns for the relational child;
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalSequenceProject::PcrsRequired(CMemoryPool *mp,
									   CExpressionHandle &exprhdl,
									   CColRefSet *pcrsRequired,
									   ULONG child_index,
									   CDrvdPropArray *,  // pdrspqdpCtxt
									   ULONG			  // ulOptReq
)
{
	SPQOS_ASSERT(
		0 == child_index &&
		"Required properties can only be computed on the relational child");

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, *m_pcrsRequiredLocal);
	pcrs->Union(pcrsRequired);

	CColRefSet *pcrsOutput =
		PcrsChildReqd(mp, exprhdl, pcrs, child_index, 1 /*ulScalarIndex*/);
	pcrs->Release();

	return pcrsOutput;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalSequenceProject::PosRequired(CMemoryPool *,		// mp
									  CExpressionHandle &,	// exprhdl
									  COrderSpec *,			// posRequired
									  ULONG
#ifdef SPQOS_DEBUG
										  child_index
#endif	// SPQOS_DEBUG
									  ,
									  CDrvdPropArray *,	 // pdrspqdpCtxt
									  ULONG				 // ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	m_pos->AddRef();

	return m_pos;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalSequenceProject::PdsRequired(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CDistributionSpec *pdsRequired,
									  ULONG child_index,
									  CDrvdPropArray *,	 // pdrspqdpCtxt
									  ULONG				 // ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	// if expression has to execute on a single host then we need a gather
	if (exprhdl.NeedsSingletonExecution())
	{
		return PdsRequireSingleton(mp, exprhdl, pdsRequired, child_index);
	}

	// if there are outer references, then we need a broadcast (or a gather)
	if (exprhdl.HasOuterRefs())
	{
		if (CDistributionSpec::EdtSingleton == pdsRequired->Edt() ||
			CDistributionSpec::EdtStrictReplicated == pdsRequired->Edt())
		{
			return PdsPassThru(mp, exprhdl, pdsRequired, child_index);
		}

		return SPQOS_NEW(mp)
			CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);
	}

	// if the window operator has a partition by clause, then always
	// request hashed distribution on the partition column
	if (CDistributionSpec::EdtHashed == m_pds->Edt())
	{
		m_pds->AddRef();
		return m_pds;
	}

	return SPQOS_NEW(mp) CDistributionSpecSingleton();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PrsRequired
//
//	@doc:
//		Compute required rewindability of the n-th child
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalSequenceProject::PrsRequired(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CRewindabilitySpec *prsRequired,
									  ULONG child_index,
									  CDrvdPropArray *,	 // pdrspqdpCtxt
									  ULONG				 // ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	return PrsPassThru(mp, exprhdl, prsRequired, child_index);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalSequenceProject::PppsRequired(CMemoryPool *mp,
									   CExpressionHandle &exprhdl,
									   CPartitionPropagationSpec *pppsRequired,
									   ULONG
#ifdef SPQOS_DEBUG
										   child_index
#endif
									   ,
									   CDrvdPropArray *,  //pdrspqdpCtxt,
									   ULONG			  //ulOptReq
)
{
	SPQOS_ASSERT(0 == child_index);
	SPQOS_ASSERT(NULL != pppsRequired);

	// The logic here is similar to CNormalizer::FPushableThruSeqPrjChild(). We only consider the keys
	// used in the starting hash distribution spec as we do not have the equivalent distribution spec
	CColRefSet *pcrsPartCols;
	if (CDistributionSpec::EdtHashed == Pds()->Edt())
	{
		SPQOS_ASSERT(
			NULL ==
			CDistributionSpecHashed::PdsConvert(Pds())->PdshashedEquiv());
		pcrsPartCols = CUtils::PcrsExtractColumns(
			mp, CDistributionSpecHashed::PdsConvert(Pds())->Pdrspqexpr());
	}
	else
	{
		pcrsPartCols = SPQOS_NEW(mp) CColRefSet(mp);
	}

	CPartitionPropagationSpec *spec =
		CPhysical::PppsRequiredPushThruUnresolvedUnary(
			mp, exprhdl, pppsRequired, CPhysical::EppcAllowed, pcrsPartCols);

	pcrsPartCols->Release();
	return spec;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PcteRequired
//
//	@doc:
//		Compute required CTE map of the n-th child
//
//---------------------------------------------------------------------------
CCTEReq *
CPhysicalSequenceProject::PcteRequired(CMemoryPool *,		 //mp,
									   CExpressionHandle &,	 //exprhdl,
									   CCTEReq *pcter,
									   ULONG
#ifdef SPQOS_DEBUG
										   child_index
#endif
									   ,
									   CDrvdPropArray *,  //pdrspqdpCtxt,
									   ULONG			  //ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);
	return PcterPushThru(pcter);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalSequenceProject::FProvidesReqdCols(CExpressionHandle &exprhdl,
											CColRefSet *pcrsRequired,
											ULONG  // ulOptReq
) const
{
	SPQOS_ASSERT(NULL != pcrsRequired);
	SPQOS_ASSERT(2 == exprhdl.Arity());

	CColRefSet *pcrs = SPQOS_NEW(m_mp) CColRefSet(m_mp);
	// include defined columns by scalar project list
	pcrs->Union(exprhdl.DeriveDefinedColumns(1));

	// include output columns of the relational child
	pcrs->Union(exprhdl.DeriveOutputColumns(0 /*child_index*/));

	BOOL fProvidesCols = pcrs->ContainsAll(pcrsRequired);
	pcrs->Release();

	return fProvidesCols;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalSequenceProject::PosDerive(CMemoryPool *,	// mp
									CExpressionHandle &exprhdl) const
{
	return PosDerivePassThruOuter(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalSequenceProject::PdsDerive(CMemoryPool *mp,
									CExpressionHandle &exprhdl) const
{
	CDistributionSpec *pds = exprhdl.Pdpplan(0 /*child_index*/)->Pds();
	if (CDistributionSpec::EdtStrictReplicated == pds->Edt())
	{
		// Sequence project (i.e. window functions) cannot guarantee replicated
		// data if their windowing clause combined with the function's input column
		// is under specified.
		// If the child was replicated, we can no longer guarantee that
		// property. Therefore we must now dervive tainted replicated.
		return SPQOS_NEW(mp) CDistributionSpecReplicated(
			CDistributionSpec::EdtTaintedReplicated);
	}
	else
	{
		return PdsDerivePassThruOuter(exprhdl);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::PrsDerive
//
//	@doc:
//		Derive rewindability
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalSequenceProject::PrsDerive(CMemoryPool *mp,
									CExpressionHandle &exprhdl) const
{
	return PrsDerivePassThruOuter(mp, exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalSequenceProject::EpetOrder(CExpressionHandle &exprhdl,
									const CEnfdOrder *peo) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	COrderSpec *pos = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pos();
	if (peo->FCompatible(pos))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalSequenceProject::EpetRewindability(CExpressionHandle &exprhdl,
											const CEnfdRewindability *per) const
{
	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	if (per->FCompatible(prs))
	{
		// required distribution is already provided
		return CEnfdProp::EpetUnnecessary;
	}

	// rewindability is enforced on operator's output
	return CEnfdProp::EpetRequired;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSequenceProject::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalSequenceProject::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	(void) m_pds->OsPrint(os);
	os << ", ";
	(void) COrderSpec::OsPrint(os, m_pdrspqos);
	os << ", ";
	(void) CWindowFrame::OsPrint(os, m_pdrspqwf);

	return os << ")";
}


// EOF
