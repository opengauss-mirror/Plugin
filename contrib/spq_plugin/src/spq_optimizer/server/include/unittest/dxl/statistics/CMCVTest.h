//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Inc.
//
//	@filename:
//		CMCVTest.h
//
//	@doc:
//		Testing merging most common values (MCV) and histograms
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CMCVTest_H
#define SPQNAUCRATES_CMCVTest_H

#include "spqos/base.h"

namespace spqnaucrates
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMCVTest
//
//	@doc:
//		Static unit testing merging most common values (MCV) and histograms
//
//---------------------------------------------------------------------------
class CMCVTest
{
private:
	// triplet consisting of MCV input file, histogram input file and merged output file
	struct SMergeTestElem
	{
		const CHAR *szInputMCVFile;
		const CHAR *szInputHistFile;
		const CHAR *szMergedFile;
	};

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	// sort MCVs tests
	static SPQOS_RESULT EresUnittest_SortInt4MCVs();

	// merge MCVs and histogram
	static SPQOS_RESULT EresUnittest_MergeHistMCV();

};	// class CMCVTest
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CMCVTest_H


// EOF
