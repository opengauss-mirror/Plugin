//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/operators/CStrictHashedDistributions.h"

#include "spqopt/base/CDistributionSpecStrictRandom.h"
#include "spqopt/base/CUtils.h"

using namespace spqopt;

CStrictHashedDistributions::CStrictHashedDistributions(
	CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
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
			if (colref->RetrieveType()->IsRedistributable())
			{
				CExpression *pexpr = CUtils::PexprScalarIdent(mp, colref);
				pdrspqexpr->Append(pexpr);
			}
		}

		CDistributionSpec *pdshashed;
		ULONG ulColumnsToRedistribute = pdrspqexpr->Size();
		if (0 < ulColumnsToRedistribute)
		{
			// create a hashed distribution on input columns of the current child
			BOOL fNullsColocated = true;
			pdshashed = SPQOS_NEW(mp)
				CDistributionSpecStrictHashed(pdrspqexpr, fNullsColocated);
		}
		else
		{
			// None of the input columns are redistributable, but we want to
			// parallelize the relations we are concatenating, so we generate
			// a random redistribution.
			// When given a plan containing a "hash" redistribution on _no_ columns,
			// Some databases actually execute it as if it's a random redistribution.
			// We should not generate such a plan, for clarity and our own sanity

			pdshashed = SPQOS_NEW(mp) CDistributionSpecStrictRandom();
			pdrspqexpr->Release();
		}
		Append(pdshashed);
	}
}
