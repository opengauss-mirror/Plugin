//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalDynamicTableScan.h
//
//	@doc:
//		Dynamic Table scan operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalDynamicTableScan_H
#define SPQOPT_CPhysicalDynamicTableScan_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalDynamicScan.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalDynamicTableScan
//
//	@doc:
//		Dynamic Table scan operator
//
//---------------------------------------------------------------------------
class CPhysicalDynamicTableScan : public CPhysicalDynamicScan
{
private:
	// private copy ctor
	CPhysicalDynamicTableScan(const CPhysicalDynamicTableScan &);

public:
	// ctors
	CPhysicalDynamicTableScan(CMemoryPool *mp, BOOL is_partial,
							  const CName *pname, CTableDescriptor *ptabdesc,
							  ULONG ulOriginOpId, ULONG scan_id,
							  CColRefArray *colref_array,
							  CColRef2dArray *pdrspqdrspqcrParts,
							  ULONG ulSecondaryScanId,
							  CPartConstraint *ppartcnstr,
							  CPartConstraint *ppartcnstrRel);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalDynamicTableScan;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalDynamicTableScan";
	}

	// match function
	virtual BOOL Matches(COperator *) const;

	// statistics derivation during costing
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CReqdPropPlan *prpplan,
									  IStatisticsArray *stats_ctxt) const;

	// conversion function
	static CPhysicalDynamicTableScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalDynamicTableScan == pop->Eopid());

		return dynamic_cast<CPhysicalDynamicTableScan *>(pop);
	}

};	// class CPhysicalDynamicTableScan

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalDynamicTableScan_H

// EOF
