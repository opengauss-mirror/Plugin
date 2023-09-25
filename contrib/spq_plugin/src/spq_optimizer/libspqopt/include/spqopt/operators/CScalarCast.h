//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarCast.h
//
//	@doc:
//		Scalar casting operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarCast_H
#define SPQOPT_CScalarCast_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarCast
//
//	@doc:
//		Scalar casting operator
//
//---------------------------------------------------------------------------
class CScalarCast : public CScalar
{
private:
	// return type metadata id in the catalog
	IMDId *m_return_type_mdid;

	// function to be used for casting
	IMDId *m_func_mdid;

	// whether or not this cast is binary coercible
	BOOL m_is_binary_coercible;

	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;

	// is operator's return type BOOL?
	BOOL m_fBoolReturnType;

	// private copy ctor
	CScalarCast(const CScalarCast &);

public:
	// ctor
	CScalarCast(CMemoryPool *mp, IMDId *return_type_mdid, IMDId *mdid_func,
				BOOL is_binary_coercible);

	// dtor
	virtual ~CScalarCast()
	{
		m_func_mdid->Release();
		m_return_type_mdid->Release();
	}


	// ident accessors

	// the type of the scalar expression
	virtual IMDId *
	MdidType() const
	{
		return m_return_type_mdid;
	}

	// func that casts
	IMDId *
	FuncMdId() const
	{
		return m_func_mdid;
	}

	virtual EOperatorId
	Eopid() const
	{
		return EopScalarCast;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarCast";
	}

	// match function
	virtual BOOL Matches(COperator *) const;

	// sensitivity to order of inputs
	virtual BOOL
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

	// whether or not this cast is binary coercible
	BOOL
	IsBinaryCoercible() const
	{
		return m_is_binary_coercible;
	}

	// boolean expression evaluation
	virtual EBoolEvalResult
	Eber(ULongPtrArray *pdrspqulChildren) const
	{
		return EberNullOnAllNullChildren(pdrspqulChildren);
	}

	// conversion function
	static CScalarCast *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarCast == pop->Eopid());

		return dynamic_cast<CScalarCast *>(pop);
	}

};	// class CScalarCast

}  // namespace spqopt


#endif	// !SPQOPT_CScalarCast_H

// EOF
