//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/base/CDatumSortedSet.h"

#include "spqos/common/CAutoRef.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CScalarConst.h"

using namespace spqopt;

CDatumSortedSet::CDatumSortedSet(CMemoryPool *mp, CExpression *pexprArray,
								 const IComparator *pcomp)
	: IDatumArray(mp), m_fIncludesNull(false)
{
	SPQOS_ASSERT(COperator::EopScalarArray == pexprArray->Pop()->Eopid());

	const ULONG ulArrayExprArity = CUtils::UlScalarArrayArity(pexprArray);
	SPQOS_ASSERT(0 < ulArrayExprArity);

	spqos::CAutoRef<IDatumArray> aprngdatum(SPQOS_NEW(mp) IDatumArray(mp));
	for (ULONG ul = 0; ul < ulArrayExprArity; ul++)
	{
		CScalarConst *popScConst =
			CUtils::PScalarArrayConstChildAt(pexprArray, ul);
		IDatum *datum = popScConst->GetDatum();
		if (datum->IsNull())
		{
			m_fIncludesNull = true;
		}
		else
		{
			datum->AddRef();
			aprngdatum->Append(datum);
		}
	}
	aprngdatum->Sort(&CUtils::IDatumCmp);

	// de-duplicate
	const ULONG ulRangeArrayArity = aprngdatum->Size();
	IDatum *pdatumPrev = (*aprngdatum)[0];
	pdatumPrev->AddRef();
	Append(pdatumPrev);
	for (ULONG ul = 1; ul < ulRangeArrayArity; ul++)
	{
		if (!pcomp->Equals((*aprngdatum)[ul], pdatumPrev))
		{
			pdatumPrev = (*aprngdatum)[ul];
			pdatumPrev->AddRef();
			Append(pdatumPrev);
		}
	}
}

BOOL
CDatumSortedSet::FIncludesNull() const
{
	return m_fIncludesNull;
}
