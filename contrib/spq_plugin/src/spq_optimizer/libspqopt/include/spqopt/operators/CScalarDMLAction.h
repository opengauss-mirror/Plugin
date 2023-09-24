//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarDMLAction.h
//
//	@doc:
//		Scalar DML action operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarDMLAction_H
#define SPQOPT_CScalarDMLAction_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarDMLAction
//
//	@doc:
//		Scalar casting operator
//
//---------------------------------------------------------------------------
class CScalarDMLAction : public CScalar
{
private:
	// private copy ctor
	CScalarDMLAction(const CScalarDMLAction &);

public:
	// dml action specification
	enum EDMLAction
	{
		EdmlactionDelete,
		EdmlactionInsert
	};

	// ctor
	CScalarDMLAction(CMemoryPool *mp) : CScalar(mp)
	{
	}

	// dtor
	virtual ~CScalarDMLAction()
	{
	}
	// ident accessors

	// the type of the scalar expression
	virtual IMDId *MdidType() const;

	virtual EOperatorId
	Eopid() const
	{
		return EopScalarDMLAction;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarDMLAction";
	}

	// match function
	virtual BOOL Matches(COperator *pop) const;

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

	// conversion function
	static CScalarDMLAction *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarDMLAction == pop->Eopid());

		return dynamic_cast<CScalarDMLAction *>(pop);
	}

};	// class CScalarDMLAction
}  // namespace spqopt

#endif	// !SPQOPT_CScalarDMLAction_H

// EOF
