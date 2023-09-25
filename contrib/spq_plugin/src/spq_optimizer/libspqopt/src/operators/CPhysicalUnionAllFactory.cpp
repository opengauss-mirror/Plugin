//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.


#include "spqopt/operators/CPhysicalUnionAllFactory.h"

#include "spqos/base.h"

#include "spqopt/exception.h"
#include "spqopt/operators/CPhysicalParallelUnionAll.h"
#include "spqopt/operators/CPhysicalSerialUnionAll.h"
#include "spqopt/xforms/CXformUtils.h"

namespace spqopt
{
CPhysicalUnionAllFactory::CPhysicalUnionAllFactory(
	CLogicalUnionAll *popLogicalUnionAll)
	: m_popLogicalUnionAll(popLogicalUnionAll)
{
}

CPhysicalUnionAll *
CPhysicalUnionAllFactory::PopPhysicalUnionAll(CMemoryPool *mp, BOOL fParallel)
{
	CColRefArray *pdrspqcrOutput = m_popLogicalUnionAll->PdrspqcrOutput();
	CColRef2dArray *pdrspqdrspqcrInput = m_popLogicalUnionAll->PdrspqdrspqcrInput();

	// TODO:  May 2nd 2012; support compatible types
	if (!CXformUtils::FSameDatatype(pdrspqdrspqcrInput))
	{
		SPQOS_RAISE(spqopt::ExmaSPQOPT, spqopt::ExmiUnsupportedOp,
				   SPQOS_WSZ_LIT("Union of non-identical types"));
	}

	pdrspqcrOutput->AddRef();
	pdrspqdrspqcrInput->AddRef();

	if (fParallel)
	{
		return SPQOS_NEW(mp) CPhysicalParallelUnionAll(
			mp, pdrspqcrOutput, pdrspqdrspqcrInput,
			m_popLogicalUnionAll->UlScanIdPartialIndex());
	}
	else
	{
		return SPQOS_NEW(mp) CPhysicalSerialUnionAll(
			mp, pdrspqcrOutput, pdrspqdrspqcrInput,
			m_popLogicalUnionAll->UlScanIdPartialIndex());
	}
}

}  // namespace spqopt
