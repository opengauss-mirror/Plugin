//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarBoolOp.cpp
//
//	@doc:
//		Implementation of scalar boolean operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarBoolOp.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;

const WCHAR CScalarBoolOp::m_rgwszBool[EboolopSentinel][30] = {
	SPQOS_WSZ_LIT("EboolopAnd"), SPQOS_WSZ_LIT("EboolopOr"),
	SPQOS_WSZ_LIT("EboolopNot")};

//---------------------------------------------------------------------------
//	@function:
//		CScalarBoolOp::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		id of comparison
//
//---------------------------------------------------------------------------
ULONG
CScalarBoolOp::HashValue() const
{
	ULONG ulBoolop = (ULONG) Eboolop();
	return spqos::CombineHashes(COperator::HashValue(),
							   spqos::HashValue<ULONG>(&ulBoolop));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarBoolOp::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarBoolOp::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarBoolOp *popLog = CScalarBoolOp::PopConvert(pop);

		// match if operators are identical
		return Eboolop() == popLog->Eboolop();
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarBoolOp::FCommutative
//
//	@doc:
//		Is boolean operator commutative?
//
//---------------------------------------------------------------------------
BOOL
CScalarBoolOp::FCommutative(EBoolOperator eboolop)
{
	switch (eboolop)
	{
		case EboolopAnd:
		case EboolopOr:
			return true;

		case EboolopNot:

		default:
			return false;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarBoolOp::MdidType
//
//	@doc:
//		Expression type
//
//---------------------------------------------------------------------------
IMDId *
CScalarBoolOp::MdidType() const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	return md_accessor->PtMDType<IMDTypeBool>()->MDId();
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarBoolOp::Eber
//
//	@doc:
//		Perform boolean expression evaluation
//
//---------------------------------------------------------------------------
CScalar::EBoolEvalResult
CScalarBoolOp::Eber(ULongPtrArray *pdrspqulChildren) const
{
	if (EboolopAnd == m_eboolop)
	{
		return EberConjunction(pdrspqulChildren);
	}

	if (EboolopOr == m_eboolop)
	{
		return EberDisjunction(pdrspqulChildren);
	}

	SPQOS_ASSERT(EboolopNot == m_eboolop);
	SPQOS_ASSERT(NULL != pdrspqulChildren);
	SPQOS_ASSERT(1 == pdrspqulChildren->Size());

	EBoolEvalResult eber = (EBoolEvalResult) * ((*pdrspqulChildren)[0]);
	switch (eber)
	{
		case EberTrue:
			return EberFalse;

		case EberFalse:
			return EberTrue;

		case EberNull:
			return EberNull;

		case EberNotTrue:
		default:
			return EberAny;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarBoolOp::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarBoolOp::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << m_rgwszBool[m_eboolop];
	os << ")";

	return os;
}


// EOF
