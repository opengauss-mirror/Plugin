//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarNullIf.h
//
//	@doc:
//		Scalar NullIf Operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarNullIf_H
#define SPQOPT_CScalarNullIf_H

#include "spqos/base.h"

#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarNullIf
//
//	@doc:
//		Scalar NullIf operator
//
//---------------------------------------------------------------------------
class CScalarNullIf : public CScalar
{
private:
	// operator id
	IMDId *m_mdid_op;

	// return type
	IMDId *m_mdid_type;

	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;

	// is operator return type BOOL?
	BOOL m_fBoolReturnType;

	// private copy ctor
	CScalarNullIf(const CScalarNullIf &);

public:
	// ctor
	CScalarNullIf(CMemoryPool *mp, IMDId *mdid_op, IMDId *mdid_type);

	// dtor
	virtual ~CScalarNullIf();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarNullIf;
	}

	// operator id
	virtual IMDId *
	MdIdOp() const
	{
		return m_mdid_op;
	}

	// return type
	virtual IMDId *
	MdidType() const
	{
		return m_mdid_type;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarNullIf";
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
	virtual COperator *
	PopCopyWithRemappedColumns(CMemoryPool *,		//mp,
							   UlongToColRefMap *,	//colref_mapping,
							   BOOL					//must_exist
	)
	{
		return PopCopyDefault();
	}

	// boolean expression evaluation
	virtual EBoolEvalResult Eber(ULongPtrArray *pdrspqulChildren) const;

	// conversion function
	static CScalarNullIf *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarNullIf == pop->Eopid());

		return reinterpret_cast<CScalarNullIf *>(pop);
	}

};	// class CScalarNullIf

}  // namespace spqopt

#endif	// !SPQOPT_CScalarNullIf_H

// EOF
