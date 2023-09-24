//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/operators/CHashedDistributions.h"

#include "spqopt/base/CUtils.h"

using namespace spqopt;
CHashedDistributions::CHashedDistributions(CMemoryPool *mp,
										   CColRefArray *pdrspqcrOutput,
										   CColRef2dArray *pdrspqdrspqcrInput)
	: CDistributionSpecArray(mp)
{
	const ULONG num_cols = pdrspqcrOutput->Size();
	const ULONG arity = pdrspqdrspqcrInput->Size();
	for (ULONG ulChild = 0; ulChild < arity; ulChild++)
	{
		CColRefArray *colref_array = (*pdrspqdrspqcrInput)[ulChild];
		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ulCol = 0; ulCol < num_cols; ulCol++)
		{
			CColRef *colref = (*colref_array)[ulCol];
			CExpression *pexpr = CUtils::PexprScalarIdent(mp, colref);
			pdrspqexpr->Append(pexpr);
		}

		// create a hashed distribution on input columns of the current child
		BOOL fNullsColocated = true;
		CDistributionSpec *pdshashed =
			SPQOS_NEW(mp) CDistributionSpecHashed(pdrspqexpr, fNullsColocated);
		Append(pdshashed);
	}
}
