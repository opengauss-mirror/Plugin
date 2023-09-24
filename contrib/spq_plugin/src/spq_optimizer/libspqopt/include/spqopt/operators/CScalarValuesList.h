//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2022 VMware, Inc. or its affiliates.
//
//	@filename:
//		CScalarValuesList.h
//
//	@doc:
//		Class for scalar arrayref index list
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarValuesList_H
#define SPQOPT_CScalarValuesList_H

#include "spqos/base.h"

#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CScalar.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDTypeBool.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CScalarValuesList
//
//	@doc:
//		Scalar arrayref index list
//
//---------------------------------------------------------------------------
class CScalarValuesList : public CScalar
{
	// private copy ctor
	CScalarValuesList(const CScalarValuesList &);

public:
	// ctor
	CScalarValuesList(CMemoryPool *mp);

	// ident accessors
	EOperatorId
	Eopid() const
	{
		return EopScalarValuesList;
	}

	// operator name
	const CHAR *
	SzId() const
	{
		return "CScalarValuesList";
	}

	// match function
	BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	COperator *
	PopCopyWithRemappedColumns(CMemoryPool *,		//mp,
							   UlongToColRefMap *,	//colref_mapping,
							   BOOL					//must_exist
	)
	{
		return PopCopyDefault();
	}

	// type of expression's result
	IMDId *
	MdidType() const
	{
		CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

		return md_accessor->PtMDType<IMDTypeBool>()->MDId();
	}

	// conversion function
	static CScalarValuesList *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarValuesList == pop->Eopid());

		return dynamic_cast<CScalarValuesList *>(pop);
	}

};	// class CScalarValuesList
}  // namespace spqopt

#endif	// !SPQOPT_CScalarValuesList_H

// EOF
