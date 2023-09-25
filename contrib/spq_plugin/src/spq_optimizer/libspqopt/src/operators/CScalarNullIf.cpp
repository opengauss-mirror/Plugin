//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarNullIf.cpp
//
//	@doc:
//		Implementation of scalar NullIf operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarNullIf.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarNullIf::CScalarNullIf
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarNullIf::CScalarNullIf(CMemoryPool *mp, IMDId *mdid_op, IMDId *mdid_type)
	: CScalar(mp),
	  m_mdid_op(mdid_op),
	  m_mdid_type(mdid_type),
	  m_returns_null_on_null_input(false),
	  m_fBoolReturnType(false)
{
	SPQOS_ASSERT(mdid_op->IsValid());
	SPQOS_ASSERT(mdid_type->IsValid());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_returns_null_on_null_input =
		CMDAccessorUtils::FScalarOpReturnsNullOnNullInput(md_accessor,
														  m_mdid_op);
	m_fBoolReturnType = CMDAccessorUtils::FBoolType(md_accessor, m_mdid_type);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarNullIf::~CScalarNullIf
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CScalarNullIf::~CScalarNullIf()
{
	m_mdid_op->Release();
	m_mdid_type->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarNullIf::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		return type id
//
//---------------------------------------------------------------------------
ULONG
CScalarNullIf::HashValue() const
{
	return spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(m_mdid_op->HashValue(), m_mdid_type->HashValue()));
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarNullIf::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarNullIf::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CScalarNullIf *popScNullIf = CScalarNullIf::PopConvert(pop);

	// match if operators and return types are identical
	return m_mdid_op->Equals(popScNullIf->MdIdOp()) &&
		   m_mdid_type->Equals(popScNullIf->MdidType());
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarNullIf::Eber
//
//	@doc:
//		Perform boolean expression evaluation
//
//---------------------------------------------------------------------------
CScalar::EBoolEvalResult
CScalarNullIf::Eber(ULongPtrArray *pdrspqulChildren) const
{
	if (m_returns_null_on_null_input)
	{
		return EberNullOnAnyNullChild(pdrspqulChildren);
	}

	return EberAny;
}



// EOF
