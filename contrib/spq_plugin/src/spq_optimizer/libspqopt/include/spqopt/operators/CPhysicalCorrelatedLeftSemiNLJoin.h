//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CPhysicalCorrelatedLeftSemiNLJoin.h
//
//	@doc:
//		Physical Left Semi NLJ operator capturing correlated execution
//		for EXISTS subqueries
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalCorrelatedLeftSemiNLJoin_H
#define SPQOPT_CPhysicalCorrelatedLeftSemiNLJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalLeftSemiNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalCorrelatedLeftSemiNLJoin
//
//	@doc:
//		Physical left semi NLJ operator capturing correlated execution for
//		EXISTS subqueries
//
//---------------------------------------------------------------------------
class CPhysicalCorrelatedLeftSemiNLJoin : public CPhysicalLeftSemiNLJoin
{
private:
	// columns from inner child used in correlated execution
	CColRefArray *m_pdrspqcrInner;

	// origin subquery id
	EOperatorId m_eopidOriginSubq;

	// private copy ctor
	CPhysicalCorrelatedLeftSemiNLJoin(
		const CPhysicalCorrelatedLeftSemiNLJoin &);

public:
	// ctor
	CPhysicalCorrelatedLeftSemiNLJoin(CMemoryPool *mp,
									  CColRefArray *pdrspqcrInner,
									  EOperatorId eopidOriginSubq)
		: CPhysicalLeftSemiNLJoin(mp),
		  m_pdrspqcrInner(pdrspqcrInner),
		  m_eopidOriginSubq(eopidOriginSubq)
	{
		SPQOS_ASSERT(NULL != pdrspqcrInner);

		SetDistrRequests(UlDistrRequestsForCorrelatedJoin());
		SPQOS_ASSERT(0 < UlDistrRequests());
	}

	// dtor
	virtual ~CPhysicalCorrelatedLeftSemiNLJoin()
	{
		m_pdrspqcrInner->Release();
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalCorrelatedLeftSemiNLJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalCorrelatedLeftSemiNLJoin";
	}

	// match function
	virtual BOOL
	Matches(COperator *pop) const
	{
		if (pop->Eopid() == Eopid())
		{
			return m_pdrspqcrInner->Equals(
				CPhysicalCorrelatedLeftSemiNLJoin::PopConvert(pop)
					->PdrgPcrInner());
		}

		return false;
	}

	// distribution matching type
	virtual CEnfdDistribution::EDistributionMatching
	Edm(CReqdPropPlan *,   // prppInput
		ULONG,			   // child_index
		CDrvdPropArray *,  //pdrspqdpCtxt
		ULONG			   // ulOptReq
	)
	{
		return CEnfdDistribution::EdmSatisfy;
	}

	virtual CEnfdDistribution *
	Ped(CMemoryPool *mp, CExpressionHandle &exprhdl, CReqdPropPlan *prppInput,
		ULONG child_index, CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
	{
		return PedCorrelatedJoin(mp, exprhdl, prppInput, child_index,
								 pdrspqdpCtxt, ulOptReq);
	}

	// compute required distribution of the n-th child
	virtual CDistributionSpec *
	PdsRequired(CMemoryPool *,		  // mp
				CExpressionHandle &,  // exprhdl,
				CDistributionSpec *,  // pdsRequired,
				ULONG,				  // child_index,
				CDrvdPropArray *,	  // pdrspqdpCtxt,
				ULONG				  //ulOptReq
	) const
	{
		SPQOS_RAISE(
			CException::ExmaInvalid, CException::ExmiInvalid,
			SPQOS_WSZ_LIT(
				"PdsRequired should not be called for CPhysicalCorrelatedLeftSemiNLJoin"));
		return NULL;
	}

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *
	PrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
				CRewindabilitySpec *prsRequired, ULONG child_index,
				CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq) const
	{
		return PrsRequiredCorrelatedJoin(mp, exprhdl, prsRequired, child_index,
										 pdrspqdpCtxt, ulOptReq);
	}

	// return true if operator is a correlated NL Join
	virtual BOOL
	FCorrelated() const
	{
		return true;
	}

	// return required inner columns
	virtual CColRefArray *
	PdrgPcrInner() const
	{
		return m_pdrspqcrInner;
	}

	// origin subquery id
	EOperatorId
	EopidOriginSubq() const
	{
		return m_eopidOriginSubq;
	}

	// print
	virtual IOstream &
	OsPrint(IOstream &os) const
	{
		os << this->SzId() << "(";
		(void) CUtils::OsPrintDrgPcr(os, m_pdrspqcrInner);
		os << ")";

		return os;
	}

	// conversion function
	static CPhysicalCorrelatedLeftSemiNLJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalCorrelatedLeftSemiNLJoin == pop->Eopid());

		return dynamic_cast<CPhysicalCorrelatedLeftSemiNLJoin *>(pop);
	}

};	// class CPhysicalCorrelatedLeftSemiNLJoin

}  // namespace spqopt


#endif	// !SPQOPT_CPhysicalCorrelatedLeftSemiNLJoin_H

// EOF
