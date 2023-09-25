//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		ICostModel.cpp
//
//	@doc:
//		Cost model implementation
//---------------------------------------------------------------------------

#include "spqos/base.h"
#include "spqos/string/CWStringConst.h"

#include "spqdbcost/CCostModelSPQDB.h"

using namespace spqopt;
using namespace spqdbcost;

// default number segments for the cost model
#define SPQOPT_DEFAULT_SEGMENT_COUNT 2

//---------------------------------------------------------------------------
//	@function:
//		ICostModel::PcmDefault
//
//	@doc:
//		Create default cost model
//
//---------------------------------------------------------------------------
ICostModel *
ICostModel::PcmDefault(CMemoryPool *mp)
{
	return SPQOS_NEW(mp) CCostModelSPQDB(mp, SPQOPT_DEFAULT_SEGMENT_COUNT);
}


//---------------------------------------------------------------------------
//	@function:
//		ICostModel::SetParams
//
//	@doc:
//		Set cost model params
//
//---------------------------------------------------------------------------
void
ICostModel::SetParams(ICostModelParamsArray *pdrspqcp)
{
	if (NULL == pdrspqcp)
	{
		return;
	}

	// overwrite default values of cost model parameters
	const ULONG size = pdrspqcp->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		ICostModelParams::SCostParam *pcp = (*pdrspqcp)[ul];
		GetCostModelParams()->SetParam(pcp->Id(), pcp->Get(),
									   pcp->GetLowerBoundVal(),
									   pcp->GetUpperBoundVal());
	}
}


// EOF
