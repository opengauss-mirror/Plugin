//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarBoolOp.h
//
//	@doc:
//		Base class for all scalar boolean operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarBoolOp_H
#define SPQOPT_CScalarBoolOp_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarBoolOp
//
//	@doc:
//		Scalar boolean operator
//
//---------------------------------------------------------------------------
class CScalarBoolOp : public CScalar
{
public:
	// enum of boolean operators
	enum EBoolOperator
	{
		EboolopAnd,	 // AND
		EboolopOr,	 // OR
		EboolopNot,	 // NOT

		EboolopSentinel
	};

private:
	static const WCHAR m_rgwszBool[EboolopSentinel][30];

	// boolean operator
	EBoolOperator m_eboolop;

	// private copy ctor
	CScalarBoolOp(const CScalarBoolOp &);

public:
	// ctor
	CScalarBoolOp(CMemoryPool *mp, EBoolOperator eboolop)
		: CScalar(mp), m_eboolop(eboolop)
	{
		SPQOS_ASSERT(0 <= eboolop && EboolopSentinel > eboolop);
	}

	// dtor
	virtual ~CScalarBoolOp()
	{
	}


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarBoolOp;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarBoolOp";
	}

	// accessor
	EBoolOperator
	Eboolop() const
	{
		return m_eboolop;
	}

	// operator specific hash function
	ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *) const;

	// sensitivity to order of inputs
	BOOL
	FInputOrderSensitive() const
	{
		return !FCommutative(Eboolop());
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
	static CScalarBoolOp *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarBoolOp == pop->Eopid());

		return reinterpret_cast<CScalarBoolOp *>(pop);
	}

	// boolean expression evaluation
	virtual EBoolEvalResult Eber(ULongPtrArray *pdrspqulChildren) const;

	// decide boolean operator commutativity
	static BOOL FCommutative(EBoolOperator eboolop);

	// the type of the scalar expression
	virtual IMDId *MdidType() const;

	// print
	virtual IOstream &OsPrint(IOstream &os) const;


};	// class CScalarBoolOp

}  // namespace spqopt


#endif	// !SPQOPT_CScalarBoolOp_H

// EOF
