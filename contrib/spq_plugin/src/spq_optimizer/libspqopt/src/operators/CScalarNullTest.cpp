//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarNullTest.cpp
//
//	@doc:
//		Implementation of scalar null test operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarNullTest.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;


//---------------------------------------------------------------------------
//	@function:
//		CScalarNullTest::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarNullTest::Matches(COperator *pop) const
{
	return pop->Eopid() == Eopid();
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarNullTest::MdidType
//
//	@doc:
//		Expression type
//
//---------------------------------------------------------------------------
IMDId *
CScalarNullTest::MdidType() const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	return md_accessor->PtMDType<IMDTypeBool>()->MDId();
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarNullTest::Eber
//
//	@doc:
//		Perform boolean expression evaluation
//
//---------------------------------------------------------------------------
CScalar::EBoolEvalResult
CScalarNullTest::Eber(ULongPtrArray *pdrspqulChildren) const
{
	SPQOS_ASSERT(NULL != pdrspqulChildren);
	SPQOS_ASSERT(1 == pdrspqulChildren->Size());

	EBoolEvalResult eber = (EBoolEvalResult) * ((*pdrspqulChildren)[0]);
	switch (eber)
	{
		case EberNull:
			return EberTrue;

		case EberFalse:
		case EberTrue:
			return EberFalse;

		case EberNotTrue:
		default:
			return EberAny;
	}
}


// EOF
