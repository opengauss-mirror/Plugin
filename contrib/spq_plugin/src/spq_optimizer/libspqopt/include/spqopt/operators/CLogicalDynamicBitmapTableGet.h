//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CLogicalDynamicBitmapTableGet.h
//
//	@doc:
//		Logical operator for dynamic table access via bitmap indexes.
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CLogicalDynamicBitmapTableGet_H
#define SPQOPT_CLogicalDynamicBitmapTableGet_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalDynamicGetBase.h"

namespace spqopt
{
// fwd declarations
class CColRefSet;
class CTableDescriptor;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalDynamicBitmapTableGet
//
//	@doc:
//		Logical operator for dynamic table access via bitmap indexes.
//
//---------------------------------------------------------------------------
class CLogicalDynamicBitmapTableGet : public CLogicalDynamicGetBase
{
private:
	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG m_ulOriginOpId;

	// private copy ctor
	CLogicalDynamicBitmapTableGet(const CLogicalDynamicBitmapTableGet &);

public:
	// ctors
	CLogicalDynamicBitmapTableGet(
		CMemoryPool *mp, CTableDescriptor *ptabdesc, ULONG ulOriginOpId,
		const CName *pnameTableAlias, ULONG ulPartIndex,
		CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrPart,
		ULONG ulSecondaryPartIndexId, BOOL is_partial,
		CPartConstraint *ppartcnstr, CPartConstraint *ppartcnstrRel);

	explicit CLogicalDynamicBitmapTableGet(CMemoryPool *mp);

	// dtor
	virtual ~CLogicalDynamicBitmapTableGet();

	// identifier
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalDynamicBitmapTableGet;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalDynamicBitmapTableGet";
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// derive outer references
	virtual CColRefSet *DeriveOuterReferences(CMemoryPool *mp,
											  CExpressionHandle &exprhdl);

	// derive constraint property
	virtual CPropConstraint *DerivePropertyConstraint(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// compute required stat columns of the n-th child
	virtual CColRefSet *
	PcrsStat(CMemoryPool *mp,
			 CExpressionHandle &,  // exprhdl
			 CColRefSet *,		   //pcrsInput
			 ULONG				   // child_index
	) const
	{
		return SPQOS_NEW(mp) CColRefSet(mp);
	}

	// candidate set of xforms
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// derive statistics
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;

	// stat promise
	virtual EStatPromise
	Esp(CExpressionHandle &) const
	{
		return CLogical::EspHigh;
	}

	// origin operator id -- spqos::ulong_max if operator was not generated via a transformation
	ULONG
	UlOriginOpId() const
	{
		return m_ulOriginOpId;
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

	// conversion
	static CLogicalDynamicBitmapTableGet *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalDynamicBitmapTableGet == pop->Eopid());

		return dynamic_cast<CLogicalDynamicBitmapTableGet *>(pop);
	}

};	// class CLogicalDynamicBitmapTableGet
}  // namespace spqopt

#endif	// !SPQOPT_CLogicalDynamicBitmapTableGet_H

// EOF
