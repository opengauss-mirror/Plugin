//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply.h
//
//	@doc:
//		Transform inner join over Select over a partitioned table into
//		a union-all of dynamic index get applies
//
//	@owner:
//		n
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply_H
#define SPQOPT_CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformJoin2IndexApply.h"
#include "spqopt/xforms/CXformJoin2IndexApplyBase.h"

namespace spqopt
{
using namespace spqos;

// fwd declarations
class CExpression;

//---------------------------------------------------------------------------
//	@class:
//		CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply
//
//	@doc:
//		Transform inner join over Select over a partitioned table into a union-all
//		of dynamic index get applies
//
//---------------------------------------------------------------------------
class CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply
	: public CXformJoin2IndexApplyBase<
		  CLogicalInnerJoin, CLogicalIndexApply, CLogicalDynamicGet,
		  true /*fWithSelect*/, true /*is_partial*/, IMDIndex::EmdindBtree>
{
public:
	// ctor
	explicit CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply(
		CMemoryPool *mp)
		: CXformJoin2IndexApplyBase<CLogicalInnerJoin, CLogicalIndexApply,
									CLogicalDynamicGet, true /*fWithSelect*/,
									true /*is_partial*/, IMDIndex::EmdindBtree>(
			  mp)
	{
	}

	// dtor
	virtual ~CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply()
	{
	}

	// compute xform promise for a given expression handle
	virtual CXform::EXformPromise
	Exfp(CExpressionHandle &exprhdl) const
	{
		if (CXform::ExfpNone == CXformJoin2IndexApply::Exfp(exprhdl))
		{
			return CXform::ExfpNone;
		}

		if (exprhdl.DeriveHasPartialIndexes(1))
		{
			return CXform::ExfpHigh;
		}

		return CXform::ExfpNone;
	}

	// ident accessor
	virtual EXformId
	Exfid() const
	{
		return ExfInnerJoinWithInnerSelect2PartialDynamicIndexGetApply;
	}

	// xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply";
	}

	// return true if xform should be applied only once
	virtual BOOL
	IsApplyOnce()
	{
		return true;
	}
};
}  // namespace spqopt

#endif	// !SPQOPT_CXformInnerJoinWithInnerSelect2PartialDynamicIndexGetApply_H

// EOF
