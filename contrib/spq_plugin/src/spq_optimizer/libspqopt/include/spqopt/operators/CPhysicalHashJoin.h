//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalHashJoin.h
//
//	@doc:
//		Base hash join operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalHashJoin_H
#define SPQOPT_CPhysicalHashJoin_H

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPhysicalJoin.h"

namespace spqopt
{
// fwd declarations
class CDistributionSpecHashed;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalHashJoin
//
//	@doc:
//		Base hash join operator
//
//---------------------------------------------------------------------------
class CPhysicalHashJoin : public CPhysicalJoin
{
private:
	// the array of expressions from the outer relation
	// that are extracted from the hashing condition
	CExpressionArray *m_pdrspqexprOuterKeys;

	// the array of expressions from the inner relation
	// that are extracted from the hashing condition
	CExpressionArray *m_pdrspqexprInnerKeys;

	// Hash op families of the operators used in the join conditions
	IMdIdArray *m_hash_opfamilies;

	// if the join condition is null-aware
	// true by default, and false if the join condition doesn't contain
	// any INDF predicates
	BOOL m_is_null_aware;

	// array redistribute request sent to the first hash join child
	CDistributionSpecArray *m_pdrspqdsRedistributeRequests;

	// private copy ctor
	CPhysicalHashJoin(const CPhysicalHashJoin &);

	// compute a distribution matching the distribution delivered by given child
	CDistributionSpec *PdsMatch(CMemoryPool *mp, CDistributionSpec *pds,
								ULONG ulSourceChildIndex) const;

protected:
	// compute required hashed distribution from the n-th child
	CDistributionSpecHashed *PdshashedRequired(CMemoryPool *mp,
											   ULONG child_index,
											   ULONG ulReqIndex) const;
	// create (redistribute, redistribute) optimization request
	CDistributionSpec *PdsRequiredRedistribute(CMemoryPool *mp,
											   CExpressionHandle &exprhdl,
											   CDistributionSpec *pdsInput,
											   ULONG child_index,
											   CDrvdPropArray *pdrspqdpCtxt,
											   ULONG ulOptReq) const;

	// create the set of redistribute requests to send to first hash join child
	void CreateHashRedistributeRequests(CMemoryPool *mp);

private:
	// create (non-singleton, replicate) optimization request
	CDistributionSpec *PdsRequiredReplicate(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CDistributionSpec *pdsInput, ULONG child_index,
		CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq, CReqdPropPlan *prppInput);

	// create a child hashed distribution request based on input hashed distribution,
	// return NULL if no such request can be created
	CDistributionSpecHashed *PdshashedPassThru(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CDistributionSpecHashed *pdshashedInput, ULONG child_index,
		CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq) const;

	// check whether a hash key is nullable
	BOOL FNullableHashKey(ULONG ulKey, CColRefSet *pcrsNotNullInner,
						  BOOL fInner) const;

protected:
	// helper for computing a hashed distribution matching the given distribution
	CDistributionSpecHashed *PdshashedMatching(
		CMemoryPool *mp, CDistributionSpecHashed *pdshashed,
		ULONG ulSourceChild) const;

	// create (singleton, singleton) optimization request
	CDistributionSpec *PdsRequiredSingleton(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CDistributionSpec *pdsInput,
											ULONG child_index,
											CDrvdPropArray *pdrspqdpCtxt) const;

	// check whether the hash keys from one child are nullable
	BOOL FNullableHashKeys(CColRefSet *pcrsNotNullInner, BOOL fInner) const;

	ULONG
	NumDistrReq() const
	{
		return m_pdrspqdsRedistributeRequests->Size();
	}

	// create optimization requests
	virtual void CreateOptRequests(CMemoryPool *mp);

public:
	// ctor
	CPhysicalHashJoin(CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
					  CExpressionArray *pdrspqexprInnerKeys,
					  IMdIdArray *hash_opfamilies = NULL,
					  BOOL is_null_aware = true,
					  CXform::EXformId origin_xform = CXform::ExfSentinel);

	// dtor
	virtual ~CPhysicalHashJoin();

	// inner keys
	const CExpressionArray *
	PdrspqexprInnerKeys() const
	{
		return m_pdrspqexprInnerKeys;
	}

	// outer keys
	const CExpressionArray *
	PdrspqexprOuterKeys() const
	{
		return m_pdrspqexprOuterKeys;
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required sort order of the n-th child
	virtual COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posInput, ULONG child_index,
									CDrvdPropArray *pdrspqdpCtxt,
									ULONG ulOptReq) const;

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CRewindabilitySpec *prsRequired,
											ULONG child_index,
											CDrvdPropArray *pdrspqdpCtxt,
											ULONG ulOptReq) const;

	// compute required distribution of the n-th child
	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const;

	virtual CEnfdDistribution *Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CReqdPropPlan *prppInput, ULONG child_index,
								   CDrvdPropArray *pdrspqdpCtxt,
								   ULONG ulDistrReq);

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive sort order
	virtual COrderSpec *
	PosDerive(CMemoryPool *mp,
			  CExpressionHandle &  // exprhdl
	) const
	{
		// hash join is not order-preserving
		return SPQOS_NEW(mp) COrderSpec(mp);
	}

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;


	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// execution order of children
	virtual EChildExecOrder
	Eceo() const
	{
		// TODO - ; 01/06/2014
		// obtain this property by through MD abstraction layer, similar to scalar properties

		// hash join in SPQDB executes its inner (right) child first,
		// the optimization order of hash join children follows the execution order
		return EceoRightToLeft;
	}

	// conversion function
	static CPhysicalHashJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(CUtils::FHashJoin(pop));

		return dynamic_cast<CPhysicalHashJoin *>(pop);
	}

};	// class CPhysicalHashJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalHashJoin_H

// EOF
