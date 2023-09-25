//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalStreamAgg.h
//
//	@doc:
//		Sort-based stream Aggregate operator
//---------------------------------------------------------------------------
#ifndef SPQOS_CPhysicalStreamAgg_H
#define SPQOS_CPhysicalStreamAgg_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalAgg.h"

namespace spqopt
{
// fwd declaration
class CDistributionSpec;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalStreamAgg
//
//	@doc:
//		Sort-based aggregate operator
//
//---------------------------------------------------------------------------
class CPhysicalStreamAgg : public CPhysicalAgg
{
private:
	// private copy ctor
	CPhysicalStreamAgg(const CPhysicalStreamAgg &);

	// local order spec
	COrderSpec *m_pos;

	// set representation of minimal grouping columns
	CColRefSet *m_pcrsMinimalGrpCols;

	// construct order spec on grouping column so that it covers required order spec
	COrderSpec *PosCovering(CMemoryPool *mp, COrderSpec *posRequired,
							CColRefArray *pdrspqcrGrp) const;

protected:
	// compute required sort columns of the n-th child
	COrderSpec *PosRequiredStreamAgg(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 COrderSpec *posRequired, ULONG child_index,
									 CColRefArray *pdrspqcrGrp) const;

	// initialize the order spec using the given array of columns
	void InitOrderSpec(CMemoryPool *mp, CColRefArray *pdrspqcrOrder);

public:
	// ctor
	CPhysicalStreamAgg(
		CMemoryPool *mp, CColRefArray *colref_array,
		CColRefArray *pdrspqcrMinimal,  // minimal grouping columns based on FD's
		COperator::EGbAggType egbaggtype, BOOL fGeneratesDuplicates,
		CColRefArray *pdrspqcrArgDQA, BOOL fMultiStage, BOOL isAggFromSplitDQA,
		CLogicalGbAgg::EAggStage aggStage,
		BOOL should_enforce_distribution = true
		// should_enforce_distribution should be set to false if
		// 'local' and 'global' splits don't need to have different
		// distributions. This flag is set to false if the local
		// aggregate has been created by CXformEagerAgg.
	);

	// dtor
	virtual ~CPhysicalStreamAgg();


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalStreamAgg;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalStreamAgg";
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required sort columns of the n-th child
	virtual COrderSpec *
	PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
				COrderSpec *posRequired, ULONG child_index,
				CDrvdPropArray *,  //pdrspqdpCtxt,
				ULONG			   //ulOptReq
	) const
	{
		return PosRequiredStreamAgg(mp, exprhdl, posRequired, child_index,
									m_pdrspqcrMinimal);
	}

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive sort order
	virtual COrderSpec *PosDerive(CMemoryPool *mp,
								  CExpressionHandle &exprhdl) const;

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CPhysicalStreamAgg *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalStreamAgg == pop->Eopid() ||
					EopPhysicalStreamAggDeduplicate == pop->Eopid());

		return reinterpret_cast<CPhysicalStreamAgg *>(pop);
	}

};	// class CPhysicalStreamAgg

}  // namespace spqopt


#endif	// !SPQOS_CPhysicalStreamAgg_H

// EOF
