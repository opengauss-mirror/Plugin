//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumTest.h
//
//	@doc:
//		Test for datum classes
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CDatumTest_H
#define SPQNAUCRATES_CDatumTest_H

#include "spqos/base.h"

#include "naucrates/base/IDatum.h"

namespace spqnaucrates
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDatumTest
//
//	@doc:
//		Static unit tests for datum
//
//---------------------------------------------------------------------------
class CDatumTest
{
private:
	// create an oid datum
	static IDatum *CreateOidDatum(CMemoryPool *mp, BOOL is_null);

	// create an int2 datum
	static IDatum *CreateInt2Datum(CMemoryPool *mp, BOOL is_null);

	// create an int4 datum
	static IDatum *CreateInt4Datum(CMemoryPool *mp, BOOL is_null);

	// create an int8 datum
	static IDatum *CreateInt8Datum(CMemoryPool *mp, BOOL is_null);

	// create a bool datum
	static IDatum *CreateBoolDatum(CMemoryPool *mp, BOOL is_null);

	// create a generic datum
	static IDatum *CreateGenericDatum(CMemoryPool *mp, BOOL is_null);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_Basics();

	static SPQOS_RESULT StatsComparisonDoubleLessThan();

	static SPQOS_RESULT StatsComparisonDoubleEqualWithinEpsilon();

	static SPQOS_RESULT StatsComparisonIntLessThan();

	static SPQOS_RESULT StatsComparisonIntEqual();

};	// class CDatumTest
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CDatumTest_H


// EOF
