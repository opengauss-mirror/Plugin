//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CScalarCoerceViaIO.h
//
//	@doc:
//		Scalar CoerceViaIO operator,
//		the operator captures coercing a value from one type to another, by
//		calling the output function of the argument type, and passing the
//		result to the input function of the result type.
//
//	@owner:
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarCoerceViaIO_H
#define SPQOPT_CScalarCoerceViaIO_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarCoerceBase.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarCoerceViaIO
//
//	@doc:
//		Scalar CoerceViaIO operator
//
//---------------------------------------------------------------------------
class CScalarCoerceViaIO : public CScalarCoerceBase
{
private:
	// private copy ctor
	CScalarCoerceViaIO(const CScalarCoerceViaIO &);

public:
	// ctor
	CScalarCoerceViaIO(CMemoryPool *mp, IMDId *mdid_type, INT type_modifier,
					   ECoercionForm dxl_coerce_format, INT location);

	// dtor
	virtual ~CScalarCoerceViaIO()
	{
	}

	virtual EOperatorId
	Eopid() const
	{
		return EopScalarCoerceViaIO;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarCoerceViaIO";
	}

	// match function
	virtual BOOL Matches(COperator *) const;

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return false;
	}

	// conversion function
	static CScalarCoerceViaIO *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarCoerceViaIO == pop->Eopid());

		return dynamic_cast<CScalarCoerceViaIO *>(pop);
	}

};	// class CScalarCoerceViaIO

}  // namespace spqopt


#endif	// !SPQOPT_CScalarCoerceViaIO_H

// EOF
