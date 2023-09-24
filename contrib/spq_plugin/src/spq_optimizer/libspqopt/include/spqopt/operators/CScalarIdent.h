//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CScalarIdent.h
//
//	@doc:
//		Scalar column identifier
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarIdent_H
#define SPQOPT_CScalarIdent_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarIdent
//
//	@doc:
//		scalar identifier operator
//
//---------------------------------------------------------------------------
class CScalarIdent : public CScalar
{
private:
	// column
	const CColRef *m_pcr;

	// private copy ctor
	CScalarIdent(const CScalarIdent &);


public:
	// ctor
	CScalarIdent(CMemoryPool *mp, const CColRef *colref)
		: CScalar(mp), m_pcr(colref)
	{
	}

	// dtor
	virtual ~CScalarIdent()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarIdent;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarIdent";
	}

	// accessor
	const CColRef *
	Pcr() const
	{
		return m_pcr;
	}

	// operator specific hash function
	ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	BOOL FInputOrderSensitive() const;

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);


	// return locally used columns
	virtual CColRefSet *
	PcrsUsed(CMemoryPool *mp,
			 CExpressionHandle &  // exprhdl

	)
	{
		CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
		pcrs->Include(m_pcr);

		return pcrs;
	}

	// conversion function
	static CScalarIdent *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarIdent == pop->Eopid());

		return reinterpret_cast<CScalarIdent *>(pop);
	}

	// the type of the scalar expression
	virtual IMDId *MdidType() const;

	// the type modifier of the scalar expression
	virtual INT TypeModifier() const;

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

	// is the given expression a scalar cast of a scalar identifier
	static BOOL FCastedScId(CExpression *pexpr);

	// is the given expression a scalar cast of given scalar identifier
	static BOOL FCastedScId(CExpression *pexpr, CColRef *colref);

	// is the given expression a scalar func allowed for Partition selection of given scalar identifier
	static BOOL FAllowedFuncScId(CExpression *pexpr);

	// is the given expression a scalar func allowed for Partition selection of given scalar identifier
	static BOOL FAllowedFuncScId(CExpression *pexpr, CColRef *colref);

};	// class CScalarIdent

}  // namespace spqopt


#endif	// !SPQOPT_CScalarIdent_H

// EOF
