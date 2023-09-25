//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalSequence.cpp
//
//	@doc:
//		Implementation of logical sequence operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalSequence.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::CLogicalSequence
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalSequence::CLogicalSequence(CMemoryPool *mp) : CLogical(mp)
{
	SPQOS_ASSERT(NULL != mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalSequence::Matches(COperator *pop) const
{
	return pop->Eopid() == Eopid();
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalSequence::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementSequence);
	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalSequence::DeriveOutputColumns(CMemoryPool *,  // mp
									  CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(1 <= exprhdl.Arity());

	// get output columns of last child
	CColRefSet *pcrs = exprhdl.DeriveOutputColumns(exprhdl.Arity() - 1);
	pcrs->AddRef();

	return pcrs;
}



//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalSequence::DeriveKeyCollection(CMemoryPool *,  // mp
									  CExpressionHandle &exprhdl) const
{
	// return key of last child
	const ULONG arity = exprhdl.Arity();
	return PkcDeriveKeysPassThru(exprhdl, arity - 1 /* ulChild */);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalSequence::DeriveMaxCard(CMemoryPool *,	// mp
								CExpressionHandle &exprhdl) const
{
	// pass on max card of last child
	return exprhdl.DeriveMaxCard(exprhdl.Arity() - 1);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequence::DerivePartitionInfo
//
//	@doc:
//		Derive part consumers
//
//---------------------------------------------------------------------------
CPartInfo *
CLogicalSequence::DerivePartitionInfo(CMemoryPool *mp,
									  CExpressionHandle &exprhdl) const
{
	return PpartinfoDeriveCombine(mp, exprhdl);
}


// EOF
