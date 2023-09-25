//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPhysicalTableScan.h
//
//	@doc:
//		Table scan operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalTableScan_H
#define SPQOPT_CPhysicalTableScan_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalScan.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalTableScan
//
//	@doc:
//		Table scan operator
//
//---------------------------------------------------------------------------
class CPhysicalTableScan : public CPhysicalScan
{
private:
	// private copy ctor
	CPhysicalTableScan(const CPhysicalTableScan &);

public:
	// ctors
	explicit CPhysicalTableScan(CMemoryPool *mp);
	CPhysicalTableScan(CMemoryPool *, const CName *, CTableDescriptor *,
					   CColRefArray *);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalTableScan;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalTableScan";
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *) const;

	// derive partition index map
	virtual CPartIndexMap *
	PpimDerive(CMemoryPool *mp,
			   CExpressionHandle &,	 // exprhdl
			   CDrvdPropCtxt *		 //pdpctxt
	) const
	{
		return SPQOS_NEW(mp) CPartIndexMap(mp);
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;


	// conversion function
	static CPhysicalTableScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalTableScan == pop->Eopid() ||
					EopPhysicalExternalScan == pop->Eopid() ||
					EopPhysicalMultiExternalScan == pop->Eopid());

		return reinterpret_cast<CPhysicalTableScan *>(pop);
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
			!"stats derivation during costing for table scan is invalid");

		return NULL;
	}

	virtual CRewindabilitySpec *
	PrsDerive(CMemoryPool *mp,
			  CExpressionHandle &  // exprhdl
	) const
	{
		// mark-restorability of output is always true
		return SPQOS_NEW(mp)
			CRewindabilitySpec(CRewindabilitySpec::ErtMarkRestore,
							   CRewindabilitySpec::EmhtNoMotion);
	}


};	// class CPhysicalTableScan

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalTableScan_H

// EOF
