//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalExternalScan.h
//
//	@doc:
//		External scan operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalExternalScan_H
#define SPQOPT_CPhysicalExternalScan_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalTableScan.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalExternalScan
//
//	@doc:
//		External scan operator
//
//---------------------------------------------------------------------------
class CPhysicalExternalScan : public CPhysicalTableScan
{
private:
	// private copy ctor
	CPhysicalExternalScan(const CPhysicalExternalScan &);

public:
	// ctor
	CPhysicalExternalScan(CMemoryPool *, const CName *, CTableDescriptor *,
						  CColRefArray *);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalExternalScan;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalExternalScan";
	}

	// match function
	virtual BOOL Matches(COperator *) const;

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive rewindability
	virtual CRewindabilitySpec *
	PrsDerive(CMemoryPool *mp,
			  CExpressionHandle &  // exprhdl
	) const
	{
		// external tables are neither rewindable nor rescannable
		return SPQOS_NEW(mp) CRewindabilitySpec(
			CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
	}

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return rewindability property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetRewindability(
		CExpressionHandle &exprhdl, const CEnfdRewindability *per) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CPhysicalExternalScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalExternalScan == pop->Eopid());

		return reinterpret_cast<CPhysicalExternalScan *>(pop);
	}

};	// class CPhysicalExternalScan

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalExternalScan_H

// EOF
