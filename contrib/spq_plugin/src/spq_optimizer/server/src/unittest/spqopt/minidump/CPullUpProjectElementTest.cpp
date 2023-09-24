//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "unittest/spqopt/minidump/CPullUpProjectElementTest.h"

#include "unittest/spqopt/CTestUtils.h"

namespace spqopt
{
using namespace spqos;

SPQOS_RESULT
CPullUpProjectElementTest::EresUnittest()
{
	ULONG ulTestCounter = 0;
	const CHAR *rgszFileNames[] = {
		"../data/dxl/minidump/SubqueryNoPullUpTableValueFunction.mdp",
	};

	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszFileNames, &ulTestCounter, SPQOS_ARRAY_SIZE(rgszFileNames), true,
		true);
}
}  // namespace spqopt
