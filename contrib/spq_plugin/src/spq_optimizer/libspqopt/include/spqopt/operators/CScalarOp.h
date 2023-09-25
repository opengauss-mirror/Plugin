//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarOp.h
//
//	@doc:
//		Base class for all scalar operations such as arithmetic and string
//		evaluations
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarOp_H
#define SPQOPT_CScalarOp_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CScalarOp
//
//	@doc:
//		general scalar operation such as arithmetic and string evaluations
//
//---------------------------------------------------------------------------
class CScalarOp : public CScalar
{
private:
	// metadata id in the catalog
	IMDId *m_mdid_op;

	// return type id or NULL if it can be inferred from the metadata
	IMDId *m_return_type_mdid;

	// scalar operator name
	const CWStringConst *m_pstrOp;

	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;

	// is operator return type BOOL?
	BOOL m_fBoolReturnType;

	// is operator commutative
	BOOL m_fCommutative;

	// private copy ctor
	CScalarOp(const CScalarOp &);

public:
	// ctor
	CScalarOp(CMemoryPool *mp, IMDId *mdid_op, IMDId *return_type_mdid,
			  const CWStringConst *pstrOp);

	// dtor
	virtual ~CScalarOp()
	{
		m_mdid_op->Release();
		CRefCount::SafeRelease(m_return_type_mdid);
		SPQOS_DELETE(m_pstrOp);
	}


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarOp;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarOp";
	}

	// accessor to the return type field
	IMDId *GetReturnTypeMdId() const;

	// the type of the scalar expression
	virtual IMDId *MdidType() const;

	// operator specific hash function
	ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	BOOL FInputOrderSensitive() const;

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
	static CScalarOp *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarOp == pop->Eopid());

		return reinterpret_cast<CScalarOp *>(pop);
	}

	// helper function
	static BOOL FCommutative(const IMDId *pcmdidOtherOp);

	// boolean expression evaluation
	virtual EBoolEvalResult Eber(ULongPtrArray *pdrspqulChildren) const;

	// name of the scalar operator
	const CWStringConst *Pstr() const;

	// metadata id
	IMDId *MdIdOp() const;

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CScalarOp

}  // namespace spqopt

#endif	// !SPQOPT_CScalarOp_H

// EOF
