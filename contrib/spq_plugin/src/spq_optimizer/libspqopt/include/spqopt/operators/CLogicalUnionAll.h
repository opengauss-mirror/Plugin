//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalUnionAll.h
//
//	@doc:
//		Logical Union all operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalUnionAll_H
#define SPQOPT_CLogicalUnionAll_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalUnion.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalUnionAll
//
//	@doc:
//		Union all operators
//
//---------------------------------------------------------------------------
class CLogicalUnionAll : public CLogicalUnion
{
private:
	// if this union is needed for partial indexes then store the scan
	// id, otherwise this will be spqos::ulong_max
	ULONG m_ulScanIdPartialIndex;

	// private copy ctor
	CLogicalUnionAll(const CLogicalUnionAll &);

public:
	// ctor
	explicit CLogicalUnionAll(CMemoryPool *mp);

	CLogicalUnionAll(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
					 CColRef2dArray *pdrspqdrspqcrInput,
					 ULONG ulScanIdPartialIndex = spqos::ulong_max);

	// dtor
	virtual ~CLogicalUnionAll();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalUnionAll;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalUnionAll";
	}

	// if this union is needed for partial indexes then return the scan
	// id, otherwise return spqos::ulong_max
	ULONG
	UlScanIdPartialIndex() const
	{
		return m_ulScanIdPartialIndex;
	}

	// is this unionall needed for a partial index
	BOOL
	IsPartialIndex() const
	{
		return (spqos::ulong_max > m_ulScanIdPartialIndex);
	}

	// sensitivity to order of inputs
	BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------

	// derive max card
	virtual CMaxCard DeriveMaxCard(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const;

	// derive key collections
	virtual CKeyCollection *DeriveKeyCollection(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// stat promise
	virtual EStatPromise
	Esp(CExpressionHandle &) const
	{
		return CLogical::EspHigh;
	}

	// derive statistics
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;


	// conversion function
	static CLogicalUnionAll *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalUnionAll == pop->Eopid());

		return reinterpret_cast<CLogicalUnionAll *>(pop);
	}

	// derive statistics based on union all semantics
	static IStatistics *PstatsDeriveUnionAll(CMemoryPool *mp,
											 CExpressionHandle &exprhdl);

};	// class CLogicalUnionAll

}  // namespace spqopt

#endif	// !SPQOPT_CLogicalUnionAll_H

// EOF
