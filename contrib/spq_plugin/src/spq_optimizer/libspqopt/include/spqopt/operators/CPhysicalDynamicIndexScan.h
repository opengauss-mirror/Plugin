//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp
//
//	@filename:
//		CPhysicalDynamicIndexScan.h
//
//	@doc:
//		Physical dynamic index scan operators on partitioned tables
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalDynamicIndexScan_H
#define SPQOPT_CPhysicalDynamicIndexScan_H

#include "spqos/base.h"

#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/operators/CPhysicalDynamicScan.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;
class CIndexDescriptor;
class CName;
class CPartConstraint;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalDynamicIndexScan
//
//	@doc:
//		Physical dynamic index scan operators for partitioned tables
//
//---------------------------------------------------------------------------
class CPhysicalDynamicIndexScan : public CPhysicalDynamicScan
{
private:
	// index descriptor
	CIndexDescriptor *m_pindexdesc;

	// order
	COrderSpec *m_pos;

	// private copy ctor
	CPhysicalDynamicIndexScan(const CPhysicalDynamicIndexScan &);

public:
	// ctors
	CPhysicalDynamicIndexScan(CMemoryPool *mp, BOOL is_partial,
							  CIndexDescriptor *pindexdesc,
							  CTableDescriptor *ptabdesc, ULONG ulOriginOpId,
							  const CName *pnameAlias,
							  CColRefArray *pdrspqcrOutput, ULONG scan_id,
							  CColRef2dArray *pdrspqdrspqcrPart,
							  ULONG ulSecondaryScanId,
							  CPartConstraint *ppartcnstr,
							  CPartConstraint *ppartcnstrRel, COrderSpec *pos);

	// dtor
	virtual ~CPhysicalDynamicIndexScan();


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalDynamicIndexScan;
	}

	// operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalDynamicIndexScan";
	}

	// index descriptor
	CIndexDescriptor *
	Pindexdesc() const
	{
		return m_pindexdesc;
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// match function
	virtual BOOL Matches(COperator *pop) const;

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive sort order
	virtual COrderSpec *
	PosDerive(CMemoryPool *,	   //mp
			  CExpressionHandle &  //exprhdl
	) const
	{
		m_pos->AddRef();
		return m_pos;
	}

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

	// conversion function
	static CPhysicalDynamicIndexScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalDynamicIndexScan == pop->Eopid());

		return dynamic_cast<CPhysicalDynamicIndexScan *>(pop);
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

	// statistics derivation during costing
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CReqdPropPlan *prpplan,
									  IStatisticsArray *stats_ctxt) const;

};	// class CPhysicalDynamicIndexScan

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalDynamicIndexScan_H

// EOF
