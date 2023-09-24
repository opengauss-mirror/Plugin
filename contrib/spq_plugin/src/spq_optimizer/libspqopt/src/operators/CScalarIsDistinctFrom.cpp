//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CScalarIsDistinctFrom.cpp
//
//	@doc:
//		Implementation of scalar IDF comparison operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarIsDistinctFrom.h"

#include "spqopt/base/COptCtxt.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"

using namespace spqopt;
using namespace spqmd;


// conversion function
CScalarIsDistinctFrom *
CScalarIsDistinctFrom::PopConvert(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(EopScalarIsDistinctFrom == pop->Eopid());

	return reinterpret_cast<CScalarIsDistinctFrom *>(pop);
}

// perform boolean expression evaluation
CScalar::EBoolEvalResult
CScalarIsDistinctFrom::Eber(ULongPtrArray *pdrspqulChildren) const
{
	SPQOS_ASSERT(2 == pdrspqulChildren->Size());

	// Is Distinct From(IDF) expression will always evaluate
	// to a true/false/unknown but not a NULL
	EBoolEvalResult firstResult = (EBoolEvalResult) * (*pdrspqulChildren)[0];
	EBoolEvalResult secondResult = (EBoolEvalResult) * (*pdrspqulChildren)[1];

	if (firstResult == EberAny || secondResult == EberAny ||
		firstResult == EberNotTrue || secondResult == EberNotTrue)
	{
		return CScalar::EberAny;
	}
	else if (firstResult != secondResult)
	{
		return CScalar::EberTrue;
	}
	return CScalar::EberFalse;
}

BOOL
CScalarIsDistinctFrom::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarIsDistinctFrom *popIDF = CScalarIsDistinctFrom::PopConvert(pop);

		// match if operator mdids are identical
		return MdIdOp()->Equals(popIDF->MdIdOp());
	}

	return false;
}

// get commuted scalar IDF operator
CScalarIsDistinctFrom *
CScalarIsDistinctFrom::PopCommutedOp(CMemoryPool *mp, COperator *pop)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = PmdidCommuteOp(md_accessor, pop);
	if (NULL != mdid && mdid->IsValid())
	{
		return SPQOS_NEW(mp)
			CScalarIsDistinctFrom(mp, mdid, Pstr(mp, md_accessor, mdid));
	}
	return NULL;
}

// EOF
