//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarArrayCmp.h
//
//	@doc:
//		Class for scalar array compare operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarArrayCmp_H
#define SPQOPT_CScalarArrayCmp_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDType.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CScalarArrayCmp
//
//	@doc:
//		Class for scalar array compare operators
//
//---------------------------------------------------------------------------
class CScalarArrayCmp : public CScalar
{
public:
	// type of array comparison
	enum EArrCmpType
	{
		EarrcmpAny,
		EarrcmpAll,
		EarrcmpSentinel
	};

private:
	// compare operator mdid
	IMDId *m_mdid_op;

	// comparison operator name
	const CWStringConst *m_pscOp;

	// array compare type
	EArrCmpType m_earrccmpt;

	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;

	// private copy ctor
	CScalarArrayCmp(const CScalarArrayCmp &);

	// names of array compare types
	static const CHAR m_rgszCmpType[EarrcmpSentinel][10];

public:
	// ctor
	CScalarArrayCmp(CMemoryPool *mp, IMDId *mdid_op,
					const CWStringConst *pstrOp, EArrCmpType earrcmpt);

	// dtor
	virtual ~CScalarArrayCmp()
	{
		m_mdid_op->Release();
		SPQOS_DELETE(m_pscOp);
	}


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarArrayCmp;
	}

	// comparison type
	EArrCmpType
	Earrcmpt() const
	{
		return m_earrccmpt;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarArrayCmp";
	}


	// operator specific hash function
	ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	BOOL
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

	// conversion function
	static CScalarArrayCmp *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarArrayCmp == pop->Eopid());

		return reinterpret_cast<CScalarArrayCmp *>(pop);
	}

	// name of the comparison operator
	const CWStringConst *Pstr() const;

	// operator mdid
	IMDId *MdIdOp() const;

	// the type of the scalar expression
	virtual IMDId *MdidType() const;

	// boolean expression evaluation
	virtual EBoolEvalResult Eber(ULongPtrArray *pdrspqulChildren) const;

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

	// expand array comparison expression into a conjunctive/disjunctive expression
	static CExpression *PexprExpand(CMemoryPool *mp,
									CExpression *pexprArrayCmp);

};	// class CScalarArrayCmp

}  // namespace spqopt

#endif	// !SPQOPT_CScalarArrayCmp_H

// EOF
