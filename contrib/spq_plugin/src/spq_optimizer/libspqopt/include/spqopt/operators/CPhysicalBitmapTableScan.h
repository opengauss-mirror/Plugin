//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CPhysicalBitmapTableScan.h
//
//	@doc:
//		Bitmap table scan physical operator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CPhysicalBitmapTableScan_H
#define SPQOPT_CPhysicalBitmapTableScan_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalScan.h"

namespace spqopt
{
// fwd declarations
class CDistributionSpec;
class CTableDescriptor;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalBitmapTableScan
//
//	@doc:
//		Bitmap table scan physical operator
//
//---------------------------------------------------------------------------
class CPhysicalBitmapTableScan : public CPhysicalScan
{
private:
	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG m_ulOriginOpId;

	// disable copy ctor
	CPhysicalBitmapTableScan(const CPhysicalBitmapTableScan &);

public:
	// ctor
	CPhysicalBitmapTableScan(CMemoryPool *mp, CTableDescriptor *ptabdesc,
							 ULONG ulOriginOpId, const CName *pnameTableAlias,
							 CColRefArray *pdrspqcrOutput);

	// dtor
	virtual ~CPhysicalBitmapTableScan()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalBitmapTableScan;
	}

	// operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalBitmapTableScan";
	}

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
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
	virtual BOOL Matches(COperator *pop) const;

	// derive partition index map
	virtual CPartIndexMap *
	PpimDerive(CMemoryPool *mp,
			   CExpressionHandle &,	 // exprhdl
			   CDrvdPropCtxt *		 //pdpctxt
	) const
	{
		return SPQOS_NEW(mp) CPartIndexMap(mp);
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
			!"stats derivation during costing for bitmap table scan is invalid");

		return NULL;
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

	// conversion function
	static CPhysicalBitmapTableScan *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalBitmapTableScan == pop->Eopid());

		return dynamic_cast<CPhysicalBitmapTableScan *>(pop);
	}
};
}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalBitmapTableScan_H

// EOF
