//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CMissingStatsTest.h
//
//	@doc:
//		Test for ensuring the expected number of missing stats during optimization
//		is correct.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CMissingStatsTest_H
#define SPQOPT_CMissingStatsTest_H

#include "spqos/base.h"

#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CStatistics.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CMissingStatsTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CMissingStatsTest
{
	struct SMissingStatsTestCase
	{
		// input stats dxl file
		const CHAR *m_szInputFile;

		// expected number of columns with missing statistics
		ULONG m_ulExpectedMissingStats;

	};	// SMissingStatsTestCase

private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulMissingStatsTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CMissingStatsTest
}  // namespace spqopt

#endif	// !SPQOPT_CMissingStatsTest_H

// EOF
