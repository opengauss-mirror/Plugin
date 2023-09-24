//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubqueryExistential.h
//
//	@doc:
//		Parent class for existential subquery operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSubqueryExistential_H
#define SPQOPT_CScalarSubqueryExistential_H

#include "spqos/base.h"

#include "spqopt/operators/CScalar.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSubqueryExistential
//
//	@doc:
//		Parent class for EXISTS/NOT EXISTS subquery operators
//
//---------------------------------------------------------------------------
class CScalarSubqueryExistential : public CScalar
{
private:
	// private copy ctor
	CScalarSubqueryExistential(const CScalarSubqueryExistential &);

public:
	// ctor
	CScalarSubqueryExistential(CMemoryPool *mp);

	// dtor
	virtual ~CScalarSubqueryExistential();

	// return the type of the scalar expression
	virtual IMDId *MdidType() const;

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

	// derive partition consumer info
	virtual CPartInfo *PpartinfoDerive(CMemoryPool *mp,
									   CExpressionHandle &exprhdl) const;

	// conversion function
	static CScalarSubqueryExistential *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSubqueryExists == pop->Eopid() ||
					EopScalarSubqueryNotExists == pop->Eopid());

		return dynamic_cast<CScalarSubqueryExistential *>(pop);
	}

};	// class CScalarSubqueryExistential
}  // namespace spqopt

#endif	// !SPQOPT_CScalarSubqueryExistential_H

// EOF
