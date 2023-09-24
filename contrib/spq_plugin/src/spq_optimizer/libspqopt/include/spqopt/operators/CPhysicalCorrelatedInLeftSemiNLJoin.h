//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CPhysicalCorrelatedInLeftSemiNLJoin.h
//
//	@doc:
//		Physical Left Semi NLJ operator capturing correlated execution
//		with IN/ANY semantics
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalCorrelatedInLeftSemiNLJoin_H
#define SPQOPT_CPhysicalCorrelatedInLeftSemiNLJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalLeftSemiNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalCorrelatedInLeftSemiNLJoin
//
//	@doc:
//		Physical left semi NLJ operator capturing correlated execution with
//		ANY/IN semantics
//
//---------------------------------------------------------------------------
class CPhysicalCorrelatedInLeftSemiNLJoin : public CPhysicalLeftSemiNLJoin
{
private:
	// columns from inner child used in correlated execution
	CColRefArray *m_pdrspqcrInner;

	// origin subquery id
	EOperatorId m_eopidOriginSubq;

	// private copy ctor
	CPhysicalCorrelatedInLeftSemiNLJoin(
		const CPhysicalCorrelatedInLeftSemiNLJoin &);

public:
	// ctor
	CPhysicalCorrelatedInLeftSemiNLJoin(CMemoryPool *mp,
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
	virtual ~CPhysicalCorrelatedInLeftSemiNLJoin()
	{
		m_pdrspqcrInner->Release();
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalCorrelatedInLeftSemiNLJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalCorrelatedInLeftSemiNLJoin";
	}

	// match function
	virtual BOOL
	Matches(COperator *pop) const
	{
		if (pop->Eopid() == Eopid())
		{
			return m_pdrspqcrInner->Equals(
				CPhysicalCorrelatedInLeftSemiNLJoin::PopConvert(pop)
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
				"PdsRequired should not be called for CPhysicalCorrelatedInLeftSemiNLJoin"));
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

	// conversion function
	static CPhysicalCorrelatedInLeftSemiNLJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalCorrelatedInLeftSemiNLJoin == pop->Eopid());

		return dynamic_cast<CPhysicalCorrelatedInLeftSemiNLJoin *>(pop);
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

};	// class CPhysicalCorrelatedInLeftSemiNLJoin

}  // namespace spqopt


#endif	// !SPQOPT_CPhysicalCorrelatedInLeftSemiNLJoin_H

// EOF
