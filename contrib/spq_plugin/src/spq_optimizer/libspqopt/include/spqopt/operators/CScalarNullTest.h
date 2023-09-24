//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarNullTest.h
//
//	@doc:
//		Scalar null test
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarNullTest_H
#define SPQOPT_CScalarNullTest_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarNullTest
//
//	@doc:
//		Scalar null test operator
//
//---------------------------------------------------------------------------
class CScalarNullTest : public CScalar
{
private:
	// private copy ctor
	CScalarNullTest(const CScalarNullTest &);

public:
	// ctor
	explicit CScalarNullTest(CMemoryPool *mp) : CScalar(mp)
	{
	}

	// dtor
	virtual ~CScalarNullTest()
	{
	}


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarNullTest;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarNullTest";
	}

	// match function
	BOOL Matches(COperator *) const;

	// sensitivity to order of inputs
	BOOL
	FInputOrderSensitive() const
	{
		return false;
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

	// the type of the scalar expression
	virtual IMDId *MdidType() const;

	// boolean expression evaluation
	virtual EBoolEvalResult Eber(ULongPtrArray *pdrspqulChildren) const;

	// conversion function
	static CScalarNullTest *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarNullTest == pop->Eopid());

		return dynamic_cast<CScalarNullTest *>(pop);
	}

};	// class CScalarNullTest

}  // namespace spqopt


#endif	// !SPQOPT_CScalarNullTest_H

// EOF
