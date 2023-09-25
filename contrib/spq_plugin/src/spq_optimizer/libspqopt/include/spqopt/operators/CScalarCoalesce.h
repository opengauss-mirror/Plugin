//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarCoalesce.h
//
//	@doc:
//		Scalar coalesce operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarCoalesce_H
#define SPQOPT_CScalarCoalesce_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarCoalesce
//
//	@doc:
//		Scalar coalesce operator
//
//---------------------------------------------------------------------------
class CScalarCoalesce : public CScalar
{
private:
	// return type
	IMDId *m_mdid_type;

	// is operator return type BOOL?
	BOOL m_fBoolReturnType;

	// private copy ctor
	CScalarCoalesce(const CScalarCoalesce &);

public:
	// ctor
	CScalarCoalesce(CMemoryPool *mp, IMDId *mdid_type);

	// dtor
	virtual ~CScalarCoalesce();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarCoalesce;
	}

	// operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarCoalesce";
	}

	// return type
	virtual IMDId *
	MdidType() const
	{
		return m_mdid_type;
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
	virtual EBoolEvalResult
	Eber(ULongPtrArray *pdrspqulChildren) const
	{
		// Coalesce returns the first not-null child,
		// if all children are Null, then Coalesce must return Null
		return EberNullOnAllNullChildren(pdrspqulChildren);
	}

	// conversion function
	static CScalarCoalesce *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarCoalesce == pop->Eopid());

		return dynamic_cast<CScalarCoalesce *>(pop);
	}

};	// class CScalarCoalesce

}  // namespace spqopt

#endif	// !SPQOPT_CScalarCoalesce_H

// EOF
