//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COrderSpec.h
//
//	@doc:
//		Description of sort order;
//		Can be used as required or derived property;
//---------------------------------------------------------------------------
#ifndef SPQOPT_COrderSpec_H
#define SPQOPT_COrderSpec_H

#include "spqos/base.h"
#include "spqos/common/DbgPrintMixin.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CPropSpec.h"
#include "naucrates/md/IMDId.h"

namespace spqopt
{
// type definition of corresponding dynamic pointer array
class COrderSpec;
typedef CDynamicPtrArray<COrderSpec, CleanupRelease> COrderSpecArray;

using namespace spqos;

// fwd declaration
class CColRefSet;

//---------------------------------------------------------------------------
//	@class:
//		COrderSpec
//
//	@doc:
//		Array of Order Expressions
//
//---------------------------------------------------------------------------
class COrderSpec : public CPropSpec
{
public:
	enum ENullTreatment
	{
		EntAuto,  // default behavior, as implemented by operator

		EntFirst,
		EntLast,

		EntSentinel
	};

private:
	//---------------------------------------------------------------------------
	//	@class:
	//		COrderExpression
	//
	//	@doc:
	//		Spec of sort order component consisting of
	//
	//			1. sort operator's mdid
	//			2. column reference
	//			3. definition of NULL treatment
	//
	//---------------------------------------------------------------------------
	class COrderExpression : public spqos::DbgPrintMixin<COrderExpression>
	{
	private:
		// MD id of sort operator
		spqmd::IMDId *m_mdid;

		// sort column
		const CColRef *m_pcr;

		// null treatment
		ENullTreatment m_ent;

		// private copy ctor
		COrderExpression(const COrderExpression &);

	public:
		// ctor
		COrderExpression(spqmd::IMDId *mdid, const CColRef *colref,
						 ENullTreatment ent);

		// dtor
		virtual ~COrderExpression();

		// accessor of sort operator midid
		spqmd::IMDId *
		GetMdIdSortOp() const
		{
			return m_mdid;
		}

		// accessor of sort column
		const CColRef *
		Pcr() const
		{
			return m_pcr;
		}

		// accessor of null treatment
		ENullTreatment
		Ent() const
		{
			return m_ent;
		}

		// check if order specs match
		BOOL Matches(const COrderExpression *poe) const;

		// print
		IOstream &OsPrint(IOstream &os) const;

	};	// class COrderExpression

	// array of order expressions
	typedef CDynamicPtrArray<COrderExpression, CleanupDelete>
		COrderExpressionArray;


	// memory pool
	CMemoryPool *m_mp;

	// components of order spec
	COrderExpressionArray *m_pdrspqoe;

	// private copy ctor
	COrderSpec(const COrderSpec &);

	// extract columns from order spec into the given column set
	void ExtractCols(CColRefSet *pcrs) const;

public:
	// ctor
	explicit COrderSpec(CMemoryPool *mp);

	// dtor
	virtual ~COrderSpec();

	// number of sort expressions
	ULONG
	UlSortColumns() const
	{
		return m_pdrspqoe->Size();
	}

	// accessor of sort operator of the n-th component
	IMDId *
	GetMdIdSortOp(ULONG ul) const
	{
		COrderExpression *poe = (*m_pdrspqoe)[ul];
		return poe->GetMdIdSortOp();
	}

	// accessor of sort column of the n-th component
	const CColRef *
	Pcr(ULONG ul) const
	{
		COrderExpression *poe = (*m_pdrspqoe)[ul];
		return poe->Pcr();
	}

	// accessor of null treatment of the n-th component
	ENullTreatment
	Ent(ULONG ul) const
	{
		COrderExpression *poe = (*m_pdrspqoe)[ul];
		return poe->Ent();
	}

	// check if order spec has no columns
	BOOL
	IsEmpty() const
	{
		return UlSortColumns() == 0;
	}

	// append new component
	void Append(spqmd::IMDId *mdid, const CColRef *colref, ENullTreatment ent);

	// extract colref set of order columns
	virtual CColRefSet *PcrsUsed(CMemoryPool *mp) const;

	// property type
	virtual EPropSpecType
	Epst() const
	{
		return EpstOrder;
	}

	// check if order specs match
	BOOL Matches(const COrderSpec *pos) const;

	// check if order specs satisfies req'd spec
	BOOL FSatisfies(const COrderSpec *pos) const;

	// append enforcers to dynamic array for the given plan properties
	virtual void AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CReqdPropPlan *prpp,
								 CExpressionArray *pdrspqexpr,
								 CExpression *pexpr);

	// hash function
	virtual ULONG HashValue() const;

	// return a copy of the order spec with remapped columns
	virtual COrderSpec *PosCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// return a copy of the order spec after excluding the given columns
	virtual COrderSpec *PosExcludeColumns(CMemoryPool *mp, CColRefSet *pcrs);

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

	// matching function over order spec arrays
	static BOOL Equals(const COrderSpecArray *pdrspqosFirst,
					   const COrderSpecArray *pdrspqosSecond);

	// combine hash values of a maximum number of entries
	static ULONG HashValue(const COrderSpecArray *pdrspqos, ULONG ulMaxSize);

	// print array of order spec objects
	static IOstream &OsPrint(IOstream &os, const COrderSpecArray *pdrspqos);

	// extract colref set of order columns used by elements of order spec array
	static CColRefSet *GetColRefSet(CMemoryPool *mp, COrderSpecArray *pdrspqos);

	// filter out array of order specs from order expressions using the passed columns
	static COrderSpecArray *PdrspqosExclude(CMemoryPool *mp,
										   COrderSpecArray *pdrspqos,
										   CColRefSet *pcrsToExclude);


};	// class COrderSpec

}  // namespace spqopt

#endif	// !SPQOPT_COrderSpec_H

// EOF
