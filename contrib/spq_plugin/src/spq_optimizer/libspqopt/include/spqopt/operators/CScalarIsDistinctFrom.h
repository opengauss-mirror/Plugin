//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarIsDistinctFrom.h
//
//	@doc:
//		Is distinct from operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarIsDistinctFrom_H
#define SPQOPT_CScalarIsDistinctFrom_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarCmp.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarIsDistinctFrom
//
//	@doc:
//		Is distinct from operator
//
//---------------------------------------------------------------------------
class CScalarIsDistinctFrom : public CScalarCmp
{
private:
	// private copy ctor
	CScalarIsDistinctFrom(const CScalarIsDistinctFrom &);

public:
	// ctor
	CScalarIsDistinctFrom(CMemoryPool *mp, IMDId *mdid_op,
						  const CWStringConst *pstrOp)
		: CScalarCmp(mp, mdid_op, pstrOp, IMDType::EcmptIDF)
	{
		SPQOS_ASSERT(mdid_op->IsValid());
	}

	// dtor
	virtual ~CScalarIsDistinctFrom()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarIsDistinctFrom;
	}

	// boolean expression evaluation
	virtual EBoolEvalResult Eber(ULongPtrArray *pdrspqulChildren) const;

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarIsDistinctFrom";
	}

	virtual BOOL Matches(COperator *pop) const;

	// conversion function
	static CScalarIsDistinctFrom *PopConvert(COperator *pop);

	// get commuted scalar IDF operator
	virtual CScalarIsDistinctFrom *PopCommutedOp(CMemoryPool *mp,
												 COperator *pop);

};	// class CScalarIsDistinctFrom

}  // namespace spqopt

#endif	// !SPQOPT_CScalarIsDistinctFrom_H

// EOF
