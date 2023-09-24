//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CLogicalExternalGet.h
//
//	@doc:
//		Logical external get operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalExternalGet_H
#define SPQOPT_CLogicalExternalGet_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGet.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;
class CName;
class CColRefSet;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalExternalGet
//
//	@doc:
//		Logical external get operator
//
//---------------------------------------------------------------------------
class CLogicalExternalGet : public CLogicalGet
{
private:
	// private copy ctor
	CLogicalExternalGet(const CLogicalExternalGet &);

public:
	// ctors
	explicit CLogicalExternalGet(CMemoryPool *mp);

	CLogicalExternalGet(CMemoryPool *mp, const CName *pnameAlias,
						CTableDescriptor *ptabdesc);

	CLogicalExternalGet(CMemoryPool *mp, const CName *pnameAlias,
						CTableDescriptor *ptabdesc,
						CColRefArray *pdrspqcrOutput);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalExternalGet;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalExternalGet";
	}

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	//-------------------------------------------------------------------------------------
	// Required Relational Properties
	//-------------------------------------------------------------------------------------

	// compute required stat columns of the n-th child
	virtual CColRefSet *
	PcrsStat(CMemoryPool *,		   // mp,
			 CExpressionHandle &,  // exprhdl
			 CColRefSet *,		   // pcrsInput
			 ULONG				   // child_index
	) const
	{
		SPQOS_ASSERT(!"CLogicalExternalGet has no children");
		return NULL;
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CLogicalExternalGet *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalExternalGet == pop->Eopid());

		return dynamic_cast<CLogicalExternalGet *>(pop);
	}

};	// class CLogicalExternalGet
}  // namespace spqopt

#endif	// !SPQOPT_CLogicalExternalGet_H

// EOF
