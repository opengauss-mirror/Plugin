//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalStreamAggDeduplicate.h
//
//	@doc:
//		Sort-based stream Aggregate operator for deduplicating join outputs
//---------------------------------------------------------------------------
#ifndef SPQOS_CPhysicalStreamAggDeduplicate_H
#define SPQOS_CPhysicalStreamAggDeduplicate_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalStreamAgg.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalStreamAggDeduplicate
//
//	@doc:
//		Sort-based aggregate operator for deduplicating join outputs
//
//---------------------------------------------------------------------------
class CPhysicalStreamAggDeduplicate : public CPhysicalStreamAgg
{
private:
	// array of keys from the join's child
	CColRefArray *m_pdrspqcrKeys;

	// private copy ctor
	CPhysicalStreamAggDeduplicate(const CPhysicalStreamAggDeduplicate &);

public:
	// ctor
	CPhysicalStreamAggDeduplicate(CMemoryPool *mp, CColRefArray *colref_array,
								  CColRefArray *pdrspqcrMinimal,
								  COperator::EGbAggType egbaggtype,
								  CColRefArray *pdrspqcrKeys,
								  BOOL fGeneratesDuplicates, BOOL fMultiStage,
								  BOOL isAggFromSplitDQA,
								  CLogicalGbAgg::EAggStage aggStage,
								  BOOL should_enforce_distribution);

	// dtor
	virtual ~CPhysicalStreamAggDeduplicate();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalStreamAggDeduplicate;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalStreamAggDeduplicate";
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

	// compute required sort columns of the n-th child
	virtual COrderSpec *
	PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
				COrderSpec *posRequired, ULONG child_index,
				CDrvdPropArray *,  //pdrspqdpCtxt,
				ULONG			   //ulOptReq
	) const
	{
		return PosRequiredStreamAgg(mp, exprhdl, posRequired, child_index,
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
	static CPhysicalStreamAggDeduplicate *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalStreamAggDeduplicate == pop->Eopid());

		return reinterpret_cast<CPhysicalStreamAggDeduplicate *>(pop);
	}

};	// class CPhysicalStreamAggDeduplicate

}  // namespace spqopt


#endif	// !SPQOS_CPhysicalStreamAggDeduplicate_H

// EOF
