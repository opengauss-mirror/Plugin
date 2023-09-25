//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CScalarCoerceToDomain.cpp
//
//	@doc:
//		Implementation of scalar CoerceToDomain operators
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarCoerceToDomain.h"

#include "spqos/base.h"

using namespace spqopt;
using namespace spqmd;


//---------------------------------------------------------------------------
//	@function:
//		CScalarCoerceToDomain::CScalarCoerceToDomain
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarCoerceToDomain::CScalarCoerceToDomain(CMemoryPool *mp, IMDId *mdid_type,
											 INT type_modifier,
											 ECoercionForm ecf, INT location)
	: CScalarCoerceBase(mp, mdid_type, type_modifier, ecf, location),
	  m_returns_null_on_null_input(false)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarCoerceToDomain::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarCoerceToDomain::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarCoerceToDomain *popCoerce =
			CScalarCoerceToDomain::PopConvert(pop);

		return popCoerce->MdidType()->Equals(MdidType()) &&
			   popCoerce->TypeModifier() == TypeModifier() &&
			   popCoerce->Ecf() == Ecf() && popCoerce->Location() == Location();
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarCoerceToDomain::Eber
//
//	@doc:
//		Perform boolean expression evaluation
//
//---------------------------------------------------------------------------
CScalar::EBoolEvalResult
CScalarCoerceToDomain::Eber(ULongPtrArray *pdrspqulChildren) const
{
	if (m_returns_null_on_null_input)
	{
		return EberNullOnAnyNullChild(pdrspqulChildren);
	}

	return EberAny;
}


// EOF
