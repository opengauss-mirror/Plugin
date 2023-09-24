//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.


#include "unittest/spqopt/minidump/CPhysicalParallelUnionAllTest.h"

#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "naucrates/traceflags/traceflags.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqos;

static ULONG ulCounter = 0;

static const CHAR *rgszFileNames[] = {
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/NoOpMotionUsesOnlyGroupOutputColumns.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/RedundantMotionParallelUnionAll.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/ParallelUnionAllWithNoRedistributableColumns.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/ParallelUnionAllWithSingleNotRedistributableColumn.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/TwoHashedTables.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/ParallelAppend-Select.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/ParallelAppend-Insert.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/ParallelAppend-ConstTable.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/ParallelUnionAllWithNotEqualNumOfDistrColumns.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/FallBackToSerialAppend.mdp",
	"../data/dxl/minidump/CPhysicalParallelUnionAllTest/RandomDistributedChildrenUnhashableColumns.mdp",
};

namespace spqopt
{
SPQOS_RESULT
CPhysicalParallelUnionAllTest::EresUnittest()
{
	BOOL fMatchPlans = true;
	BOOL fTestSpacePruning = true;
	CAutoTraceFlag atfParallelAppend(spqos::EopttraceEnableParallelAppend, true);
	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszFileNames, &ulCounter, SPQOS_ARRAY_SIZE(rgszFileNames), fMatchPlans,
		fTestSpacePruning);
}

}  // namespace spqopt
