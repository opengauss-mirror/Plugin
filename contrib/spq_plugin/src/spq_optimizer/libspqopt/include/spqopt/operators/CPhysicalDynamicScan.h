//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CPhysicalDynamicScan.h
//
//	@doc:
//		Base class for physical dynamic scan operators
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CPhysicalDynamicScan_H
#define SPQOPT_CPhysicalDynamicScan_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalScan.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;
class CName;
class CPartConstraint;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalDynamicScan
//
//	@doc:
//		Base class for physical dynamic scan operators
//
//---------------------------------------------------------------------------
class CPhysicalDynamicScan : public CPhysicalScan
{
private:
	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG m_ulOriginOpId;

	// true iff it is a partial scan
	BOOL m_is_partial;

	// id of the dynamic scan
	ULONG m_scan_id;

	// partition keys
	CColRef2dArray *m_pdrspqdrspqcrPart;

	// secondary scan id in case of partial scan
	ULONG m_ulSecondaryScanId;

	// dynamic index part constraint
	CPartConstraint *m_part_constraint;

	// relation part constraint
	CPartConstraint *m_ppartcnstrRel;

	// disable copy ctor
	CPhysicalDynamicScan(const CPhysicalDynamicScan &);

public:
	// ctor
	CPhysicalDynamicScan(CMemoryPool *mp, BOOL is_partial,
						 CTableDescriptor *ptabdesc, ULONG ulOriginOpId,
						 const CName *pnameAlias, ULONG scan_id,
						 CColRefArray *pdrspqcrOutput,
						 CColRef2dArray *pdrspqdrspqcrParts,
						 ULONG ulSecondaryScanId, CPartConstraint *ppartcnstr,
						 CPartConstraint *ppartcnstrRel);

	// dtor
	virtual ~CPhysicalDynamicScan();

	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG
	UlOriginOpId() const
	{
		return m_ulOriginOpId;
	}

	// true iff the scan is partial
	BOOL
	IsPartial() const
	{
		return m_is_partial;
	}

	// return scan id
	ULONG
	ScanId() const
	{
		return m_scan_id;
	}

	// partition keys
	CColRef2dArray *
	PdrspqdrspqcrPart() const
	{
		return m_pdrspqdrspqcrPart;
	}

	// secondary scan id
	ULONG
	UlSecondaryScanId() const
	{
		return m_ulSecondaryScanId;
	}

	// dynamic index part constraint
	CPartConstraint *
	Ppartcnstr() const
	{
		return m_part_constraint;
	}

	// relation part constraint
	CPartConstraint *
	PpartcnstrRel() const
	{
		return m_ppartcnstrRel;
	}

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// derive partition index map
	virtual CPartIndexMap *PpimDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CDrvdPropCtxt *pdpctxt) const;

	// return true if operator is dynamic scan
	virtual BOOL
	FDynamicScan() const
	{
		return true;
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

	// conversion function
	static CPhysicalDynamicScan *PopConvert(COperator *pop);
};
}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalDynamicScan_H

// EOF
