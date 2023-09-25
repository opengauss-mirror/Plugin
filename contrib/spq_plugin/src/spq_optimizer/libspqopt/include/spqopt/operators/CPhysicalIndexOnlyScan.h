//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2020 VMware, Inc.
//
//	@filename:
//		CPhysicalIndexOnlyScan.h
//
//	@doc:
//		Base class for physical index only scan operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalIndexOnlyScan_H
#define SPQOPT_CPhysicalIndexOnlyScan_H

#include "spqos/base.h"

#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/operators/CPhysicalScan.h"

namespace spqopt
{
// fwd declarations
class CName;
class CDistributionSpecHashed;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalIndexOnlyScan
//
//	@doc:
//		Base class for physical index only scan operators
//
//---------------------------------------------------------------------------
class CPhysicalIndexOnlyScan : public CPhysicalScan
{
private:
	// index descriptor
	CIndexDescriptor *m_pindexdesc;

	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG m_ulOriginOpId;

	// order
	COrderSpec *m_pos;

	// private copy ctor
	CPhysicalIndexOnlyScan(const CPhysicalIndexOnlyScan &);

public:
	// ctors
	CPhysicalIndexOnlyScan(CMemoryPool *mp, CIndexDescriptor *pindexdesc,
						   CTableDescriptor *ptabdesc, ULONG ulOriginOpId,
						   const CName *pnameAlias, CColRefArray *colref_array,
						   COrderSpec *pos);

	// dtor
	virtual ~CPhysicalIndexOnlyScan();


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalIndexOnlyScan;
	}

	// operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalIndexOnlyScan";
	}

	// table alias name
	const CName &
	NameAlias() const
	{
		return *m_pnameAlias;
	}

	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG
	UlOriginOpId() const
	{
		return m_ulOriginOpId;
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *pop) const;

	// index descriptor
	CIndexDescriptor *
	Pindexdesc() const
	{
		return m_pindexdesc;
	}

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

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

	// derive partition index map
	virtual CPartIndexMap *
	PpimDerive(CMemoryPool *mp,
			   CExpressionHandle &,	 // exprhdl
			   CDrvdPropCtxt *		 //pdpctxt
	) const
	{
		return SPQOS_NEW(mp) CPartIndexMap(mp);
	}

	virtual CRewindabilitySpec *
	PrsDerive(CMemoryPool *mp,
			  CExpressionHandle &  // exprhdl
	) const
	{
		// rewindability of output is always true
		return SPQOS_NEW(mp)
			CRewindabilitySpec(CRewindabilitySpec::ErtMarkRestore,
							   CRewindabilitySpec::EmhtNoMotion);
	}

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

	// conversion function
	static CPhysicalIndexOnlyScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalIndexOnlyScan == pop->Eopid());

		return dynamic_cast<CPhysicalIndexOnlyScan *>(pop);
	}

	// statistics derivation during costing
	virtual IStatistics *
	PstatsDerive(CMemoryPool *,		   // mp
				 CExpressionHandle &,  // exprhdl
				 CReqdPropPlan *,	   // prpplan
				 IStatisticsArray *	   //stats_ctxt
	) const
	{
		SPQOS_ASSERT(
			!"stats derivation during costing for index only scan is invalid");

		return NULL;
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

};	// class CPhysicalIndexOnlyScan

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalIndexOnlyScan_H

// EOF
