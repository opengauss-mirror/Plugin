//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CScalarArrayRefIndexList.h
//
//	@doc:
//		Class for scalar arrayref index list
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarArrayRefIndexList_H
#define SPQOPT_CScalarArrayRefIndexList_H

#include "spqos/base.h"

#include "spqopt/operators/CScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CScalarArrayRefIndexList
//
//	@doc:
//		Scalar arrayref index list
//
//---------------------------------------------------------------------------
class CScalarArrayRefIndexList : public CScalar
{
public:
	enum EIndexListType
	{
		EiltLower,	// lower index
		EiltUpper,	// upper index
		EiltSentinel
	};

private:
	// index list type
	EIndexListType m_eilt;

	// private copy ctor
	CScalarArrayRefIndexList(const CScalarArrayRefIndexList &);

public:
	// ctor
	CScalarArrayRefIndexList(CMemoryPool *mp, EIndexListType eilt);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarArrayRefIndexList;
	}

	// operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarArrayRefIndexList";
	}

	// index list type
	EIndexListType
	Eilt() const
	{
		return m_eilt;
	}

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

	// type of expression's result
	virtual IMDId *
	MdidType() const
	{
		SPQOS_ASSERT(
			!"Invalid function call: CScalarArrayRefIndexList::MdidType()");
		return NULL;
	}

	// conversion function
	static CScalarArrayRefIndexList *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarArrayRefIndexList == pop->Eopid());

		return dynamic_cast<CScalarArrayRefIndexList *>(pop);
	}

};	// class CScalarArrayRefIndexList
}  // namespace spqopt

#endif	// !SPQOPT_CScalarArrayRefIndexList_H

// EOF
