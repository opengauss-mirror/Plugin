//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarSwitchCase.h
//
//	@doc:
//		Scalar SwitchCase operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSwitchCase_H
#define SPQOPT_CScalarSwitchCase_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSwitchCase
//
//	@doc:
//		Scalar SwitchCase operator
//
//---------------------------------------------------------------------------
class CScalarSwitchCase : public CScalar
{
private:
	// private copy ctor
	CScalarSwitchCase(const CScalarSwitchCase &);

public:
	// ctor
	explicit CScalarSwitchCase(CMemoryPool *mp);

	// dtor
	virtual ~CScalarSwitchCase()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarSwitchCase;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarSwitchCase";
	}

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *
	PopCopyWithRemappedColumns(CMemoryPool *,		//mp,
							   UlongToColRefMap *,	//colref_mapping,
							   BOOL					//must_exist
	)
	{
		return PopCopyDefault();
	}

	virtual IMDId *
	MdidType() const
	{
		SPQOS_ASSERT(!"Invalid function call: CScalarSwitchCase::MdidType()");
		return NULL;
	}

	// boolean expression evaluation
	virtual EBoolEvalResult
	Eber(ULongPtrArray *pdrspqulChildren) const
	{
		return EberNullOnAllNullChildren(pdrspqulChildren);
	}

	// conversion function
	static CScalarSwitchCase *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSwitchCase == pop->Eopid());

		return dynamic_cast<CScalarSwitchCase *>(pop);
	}

};	// class CScalarSwitchCase

}  // namespace spqopt


#endif	// !SPQOPT_CScalarSwitchCase_H

// EOF
