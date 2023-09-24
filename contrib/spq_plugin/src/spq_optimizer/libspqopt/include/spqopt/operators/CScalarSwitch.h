//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarSwitch.h
//
//	@doc:
//		Scalar switch operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSwitch_H
#define SPQOPT_CScalarSwitch_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSwitch
//
//	@doc:
//		Scalar switch operator. This corresponds to SQL case statments in the form
//		(case expr when expr1 then ret1 when expr2 then ret2 ... else retdef end)
//		The switch operator is represented as follows:
//		Switch
//		|-- expr1
//		|-- SwitchCase
//		|	|-- expr1
//		|	+-- ret1
//		|-- SwitchCase
//		|	|-- expr2
//		|	+-- ret2
//		:
//		+-- retdef
//
//---------------------------------------------------------------------------
class CScalarSwitch : public CScalar
{
private:
	// return type
	IMDId *m_mdid_type;

	// is operator return type BOOL?
	BOOL m_fBoolReturnType;

	// private copy ctor
	CScalarSwitch(const CScalarSwitch &);

public:
	// ctor
	CScalarSwitch(CMemoryPool *mp, IMDId *mdid_type);

	// dtor
	virtual ~CScalarSwitch();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarSwitch;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarSwitch";
	}

	// the type of the scalar expression
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
		return EberNullOnAllNullChildren(pdrspqulChildren);
	}

	// conversion function
	static CScalarSwitch *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSwitch == pop->Eopid());

		return dynamic_cast<CScalarSwitch *>(pop);
	}

};	// class CScalarSwitch

}  // namespace spqopt


#endif	// !SPQOPT_CScalarSwitch_H

// EOF
