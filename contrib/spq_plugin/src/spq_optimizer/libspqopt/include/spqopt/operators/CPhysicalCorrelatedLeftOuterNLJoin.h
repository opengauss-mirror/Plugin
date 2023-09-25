//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalCorrelatedLeftOuterNLJoin.h
//
//	@doc:
//		Physical Left Outer NLJ  operator capturing correlated execution
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalCorrelatedLeftOuterNLJoin_H
#define SPQOPT_CPhysicalCorrelatedLeftOuterNLJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalLeftOuterNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalCorrelatedLeftOuterNLJoin
//
//	@doc:
//		Physical left outer NLJ operator capturing correlated execution
//
//---------------------------------------------------------------------------
class CPhysicalCorrelatedLeftOuterNLJoin : public CPhysicalLeftOuterNLJoin
{
private:
	// columns from inner child used in correlated execution
	CColRefArray *m_pdrspqcrInner;

	// origin subquery id
	EOperatorId m_eopidOriginSubq;

	// private copy ctor
	CPhysicalCorrelatedLeftOuterNLJoin(
		const CPhysicalCorrelatedLeftOuterNLJoin &);

public:
	// ctor
	CPhysicalCorrelatedLeftOuterNLJoin(CMemoryPool *mp,
									   CColRefArray *pdrspqcrInner,
									   EOperatorId eopidOriginSubq)
		: CPhysicalLeftOuterNLJoin(mp),
		  m_pdrspqcrInner(pdrspqcrInner),
		  m_eopidOriginSubq(eopidOriginSubq)
	{
		SPQOS_ASSERT(NULL != pdrspqcrInner);

		SetDistrRequests(UlDistrRequestsForCorrelatedJoin());
		SPQOS_ASSERT(0 < UlDistrRequests());
	}

	// dtor
	virtual ~CPhysicalCorrelatedLeftOuterNLJoin()
	{
		m_pdrspqcrInner->Release();
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalCorrelatedLeftOuterNLJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalCorrelatedLeftOuterNLJoin";
	}

	// match function
	virtual BOOL
	Matches(COperator *pop) const
	{
		if (pop->Eopid() == Eopid())
		{
			return m_pdrspqcrInner->Equals(
				CPhysicalCorrelatedLeftOuterNLJoin::PopConvert(pop)
					->PdrgPcrInner());
		}

		return false;
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
				"PdsRequired should not be called for CPhysicalCorrelatedLeftOuterNLJoin"));
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

	// conversion function
	static CPhysicalCorrelatedLeftOuterNLJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalCorrelatedLeftOuterNLJoin == pop->Eopid());

		return dynamic_cast<CPhysicalCorrelatedLeftOuterNLJoin *>(pop);
	}

};	// class CPhysicalCorrelatedLeftOuterNLJoin

}  // namespace spqopt


#endif	// !SPQOPT_CPhysicalCorrelatedLeftOuterNLJoin_H

// EOF
