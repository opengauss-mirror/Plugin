//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalHashAggDeduplicate.h
//
//	@doc:
//		Hash Aggregate operator for deduplicating join outputs
//---------------------------------------------------------------------------
#ifndef SPQOS_CPhysicalHashAggDeduplicate_H
#define SPQOS_CPhysicalHashAggDeduplicate_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalHashAgg.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalHashAggDeduplicate
//
//	@doc:
//		Hash-based aggregate operator for deduplicating join outputs
//
//---------------------------------------------------------------------------
class CPhysicalHashAggDeduplicate : public CPhysicalHashAgg
{
private:
	// array of keys from the join's child
	CColRefArray *m_pdrspqcrKeys;

	// private copy ctor
	CPhysicalHashAggDeduplicate(const CPhysicalHashAggDeduplicate &);

public:
	// ctor
	CPhysicalHashAggDeduplicate(CMemoryPool *mp, CColRefArray *colref_array,
								CColRefArray *pdrspqcrMinimal,
								COperator::EGbAggType egbaggtype,
								CColRefArray *pdrspqcrKeys,
								BOOL fGeneratesDuplicates, BOOL fMultiStage,
								BOOL isAggFromSplitDQA,
								CLogicalGbAgg::EAggStage aggStage,
								BOOL should_enforce_distribution);

	// dtor
	virtual ~CPhysicalHashAggDeduplicate();


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalHashAggDeduplicate;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalHashAggDeduplicate";
	}

	// array of keys from the join's child
	CColRefArray *
	PdrspqcrKeys() const
	{
		return m_pdrspqcrKeys;
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required output columns of the n-th child
	virtual CColRefSet *
	PcrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
				 CColRefSet *pcrsRequired, ULONG child_index,
				 CDrvdPropArray *,	//pdrspqdpCtxt,
				 ULONG				//ulOptReq
	)
	{
		return PcrsRequiredAgg(mp, exprhdl, pcrsRequired, child_index,
							   m_pdrspqcrKeys);
	}

	// compute required distribution of the n-th child
	virtual CDistributionSpec *
	PdsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
				CDistributionSpec *pdsRequired, ULONG child_index,
				CDrvdPropArray *,  //pdrspqdpCtxt,
				ULONG ulOptReq) const
	{
		return PdsRequiredAgg(mp, exprhdl, pdsRequired, child_index, ulOptReq,
							  m_pdrspqcrKeys, m_pdrspqcrKeys);
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// debug print
	virtual IOstream &OsPrint(IOstream &os) const;

	// conversion function
	static CPhysicalHashAggDeduplicate *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalHashAggDeduplicate == pop->Eopid());

		return reinterpret_cast<CPhysicalHashAggDeduplicate *>(pop);
	}

};	// class CPhysicalHashAggDeduplicate

}  // namespace spqopt


#endif	// !SPQOS_CPhysicalHashAggDeduplicate_H

// EOF
