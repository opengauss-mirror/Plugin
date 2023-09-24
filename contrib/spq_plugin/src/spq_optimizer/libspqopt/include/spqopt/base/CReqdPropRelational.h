//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC CORP.
//
//	@filename:
//		CReqdPropRelational.h
//
//	@doc:
//		Derived required relational properties
//---------------------------------------------------------------------------
#ifndef SPQOPT_CReqdPropRelational_H
#define SPQOPT_CReqdPropRelational_H

#include "spqos/base.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CReqdProp.h"

namespace spqopt
{
using namespace spqos;

// forward declaration
class CExpression;
class CExpressionHandle;
class CColRefSet;

//---------------------------------------------------------------------------
//	@class:
//		CReqdPropRelational
//
//	@doc:
//		Required relational properties container.
//
//---------------------------------------------------------------------------
class CReqdPropRelational : public CReqdProp
{
private:
	// required stat columns
	CColRefSet *m_pcrsStat;

	// predicate on partition key
	CExpression *m_pexprPartPred;

	// private copy ctor
	CReqdPropRelational(const CReqdPropRelational &);

public:
	// default ctor
	CReqdPropRelational();

	// ctor
	explicit CReqdPropRelational(CColRefSet *pcrs);

	// ctor
	CReqdPropRelational(CColRefSet *pcrs, CExpression *pexprPartPred);

	// dtor
	virtual ~CReqdPropRelational();

	// type of properties
	virtual BOOL
	FRelational() const
	{
		SPQOS_ASSERT(!FPlan());
		return true;
	}

	// stat columns accessor
	CColRefSet *
	PcrsStat() const
	{
		return m_pcrsStat;
	}

	// partition predicate accessor
	CExpression *
	PexprPartPred() const
	{
		return m_pexprPartPred;
	}

	// required properties computation function
	virtual void Compute(CMemoryPool *mp, CExpressionHandle &exprhdl,
						 CReqdProp *prpInput, ULONG child_index,
						 CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq);

	// return difference from given properties
	CReqdPropRelational *PrprelDifference(CMemoryPool *mp,
										  CReqdPropRelational *prprel);

	// return true if property container is empty
	BOOL IsEmpty() const;

	// shorthand for conversion
	static CReqdPropRelational *GetReqdRelationalProps(CReqdProp *prp);

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CReqdPropRelational

}  // namespace spqopt


#endif	// !SPQOPT_CReqdPropRelational_H

// EOF
