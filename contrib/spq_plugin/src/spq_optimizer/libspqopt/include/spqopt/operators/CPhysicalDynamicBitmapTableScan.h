//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CPhysicalDynamicBitmapTableScan.h
//
//	@doc:
//		Dynamic bitmap table scan physical operator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CPhysicalDynamicBitmapTableScan_H
#define SPQOPT_CPhysicalDynamicBitmapTableScan_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalDynamicScan.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;
class CName;
class CPartConstraint;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalDynamicBitmapTableScan
//
//	@doc:
//		Dynamic bitmap table scan physical operator
//
//---------------------------------------------------------------------------
class CPhysicalDynamicBitmapTableScan : public CPhysicalDynamicScan
{
private:
	// disable copy ctor
	CPhysicalDynamicBitmapTableScan(const CPhysicalDynamicBitmapTableScan &);

public:
	// ctor
	CPhysicalDynamicBitmapTableScan(CMemoryPool *mp, BOOL is_partial,
									CTableDescriptor *ptabdesc,
									ULONG ulOriginOpId, const CName *pnameAlias,
									ULONG scan_id, CColRefArray *pdrspqcrOutput,
									CColRef2dArray *pdrspqdrspqcrParts,
									ULONG ulSecondaryScanId,
									CPartConstraint *ppartcnstr,
									CPartConstraint *ppartcnstrRel);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalDynamicBitmapTableScan;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalDynamicBitmapTableScan";
	}

	// match function
	virtual BOOL Matches(COperator *) const;

	// statistics derivation during costing
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CReqdPropPlan *prpplan,
									  IStatisticsArray *stats_ctxt) const;

	// conversion function
	static CPhysicalDynamicBitmapTableScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalDynamicBitmapTableScan == pop->Eopid());

		return dynamic_cast<CPhysicalDynamicBitmapTableScan *>(pop);
	}
};
}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalDynamicBitmapTableScan_H

// EOF
