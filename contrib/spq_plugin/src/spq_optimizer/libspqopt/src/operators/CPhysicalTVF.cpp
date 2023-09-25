//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp
//
//	@filename:
//		CPhysicalTVF.cpp
//
//	@doc:
//		Implementation of table-valued functions
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalTVF.h"

#include "spqos/base.h"

#include "spqopt/base/CCTEMap.h"
#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CDistributionSpecUniversal.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::CPhysicalTVF
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalTVF::CPhysicalTVF(CMemoryPool *mp, IMDId *mdid_func,
						   IMDId *mdid_return_type, CWStringConst *str,
						   CColumnDescriptorArray *pdrspqcoldesc,
						   CColRefSet *pcrsOutput)
	: CPhysical(mp),
	  m_func_mdid(mdid_func),
	  m_return_type_mdid(mdid_return_type),
	  m_pstr(str),
	  m_pdrspqcoldesc(pdrspqcoldesc),
	  m_pcrsOutput(pcrsOutput)
{
	SPQOS_ASSERT(m_return_type_mdid->IsValid());
	SPQOS_ASSERT(NULL != m_pstr);
	SPQOS_ASSERT(NULL != m_pdrspqcoldesc);
	SPQOS_ASSERT(NULL != m_pcrsOutput);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	if (m_func_mdid->IsValid())
		m_pmdfunc = md_accessor->RetrieveFunc(m_func_mdid);
	else
		m_pmdfunc = NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::~CPhysicalTVF
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalTVF::~CPhysicalTVF()
{
	m_func_mdid->Release();
	m_return_type_mdid->Release();
	m_pdrspqcoldesc->Release();
	m_pcrsOutput->Release();
	SPQOS_DELETE(m_pstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CPhysicalTVF::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CPhysicalTVF *popTVF = CPhysicalTVF::PopConvert(pop);

		return m_func_mdid->Equals(popTVF->FuncMdId()) &&
			   m_return_type_mdid->Equals(popTVF->ReturnTypeMdId()) &&
			   m_pdrspqcoldesc == popTVF->Pdrspqcoldesc() &&
			   m_pcrsOutput->Equals(popTVF->DeriveOutputColumns());
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::FInputOrderSensitive
//
//	@doc:
//		Sensitivity to input order
//
//---------------------------------------------------------------------------
BOOL
CPhysicalTVF::FInputOrderSensitive() const
{
	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PcrsRequired
//
//	@doc:
//		Compute required columns of the n-th child;
//		we only compute required columns for the relational child;
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalTVF::PcrsRequired(CMemoryPool *,		 // mp,
						   CExpressionHandle &,	 // exprhdl,
						   CColRefSet *,		 // pcrsRequired,
						   ULONG,				 // child_index,
						   CDrvdPropArray *,	 // pdrspqdpCtxt
						   ULONG				 // ulOptReq
)
{
	SPQOS_ASSERT(!"CPhysicalTVF has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalTVF::PosRequired(CMemoryPool *,		// mp,
						  CExpressionHandle &,	// exprhdl,
						  COrderSpec *,			// posRequired,
						  ULONG,				// child_index,
						  CDrvdPropArray *,		// pdrspqdpCtxt
						  ULONG					// ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalTVF has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalTVF::PdsRequired(CMemoryPool *,		// mp,
						  CExpressionHandle &,	// exprhdl,
						  CDistributionSpec *,	// pdsRequired,
						  ULONG,				//child_index
						  CDrvdPropArray *,		// pdrspqdpCtxt
						  ULONG					// ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalTVF has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PrsRequired
//
//	@doc:
//		Compute required rewindability of the n-th child
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalTVF::PrsRequired(CMemoryPool *,		 // mp,
						  CExpressionHandle &,	 // exprhdl,
						  CRewindabilitySpec *,	 // prsRequired,
						  ULONG,				 // child_index,
						  CDrvdPropArray *,		 // pdrspqdpCtxt
						  ULONG					 // ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalTVF has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PcteRequired
//
//	@doc:
//		Compute required CTE map of the n-th child
//
//---------------------------------------------------------------------------
CCTEReq *
CPhysicalTVF::PcteRequired(CMemoryPool *,		 //mp,
						   CExpressionHandle &,	 //exprhdl,
						   CCTEReq *,			 //pcter,
						   ULONG,				 //child_index,
						   CDrvdPropArray *,	 //pdrspqdpCtxt,
						   ULONG				 //ulOptReq
) const
{
	SPQOS_ASSERT(!"CPhysicalTVF has no relational children");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalTVF::FProvidesReqdCols(CExpressionHandle &,  // exprhdl,
								CColRefSet *pcrsRequired,
								ULONG  // ulOptReq
) const
{
	SPQOS_ASSERT(NULL != pcrsRequired);

	return m_pcrsOutput->ContainsAll(pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalTVF::PosDerive(CMemoryPool *mp,
						CExpressionHandle &	 // exprhdl
) const
{
	return SPQOS_NEW(mp) COrderSpec(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalTVF::PdsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	if (exprhdl.NeedsSingletonExecution())
	{
		return SPQOS_NEW(mp) CDistributionSpecSingleton();
	}

	return SPQOS_NEW(mp) CDistributionSpecUniversal();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PrsDerive
//
//	@doc:
//		Derive rewindability
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalTVF::PrsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	// TODO: shardikar; If the executor materializes the function results, aren't
	// volatile functions rewindable? Or should they be rescannable and
	// re-executed every time?
	if (IMDFunction::EfsVolatile == exprhdl.DeriveFunctionProperties()->Efs())
	{
		return SPQOS_NEW(mp)
			CRewindabilitySpec(CRewindabilitySpec::ErtRescannable,
							   CRewindabilitySpec::EmhtNoMotion);
	}

	// TVF scan materializes the results of its execution, and so is rewindable.
	return SPQOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtRewindable,
										   CRewindabilitySpec::EmhtNoMotion);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::PcmDerive
//
//	@doc:
//		Common case of combining cte maps of all logical children
//
//---------------------------------------------------------------------------
CCTEMap *
CPhysicalTVF::PcmDerive(CMemoryPool *mp,
						CExpressionHandle &	 //exprhdl
) const
{
	return SPQOS_NEW(mp) CCTEMap(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalTVF::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalTVF::EpetOrder(CExpressionHandle &,  // exprhdl
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
//		CPhysicalTVF::EpetRewindability
//
//	@doc:
//		Return the enforcing type for rewindability property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalTVF::EpetRewindability(CExpressionHandle &exprhdl,
								const CEnfdRewindability *per) const
{
	// get rewindability delivered by the TVF node
	CRewindabilitySpec *prs = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Prs();
	if (per->FCompatible(prs))
	{
		// required distribution is already provided
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}

// EOF
