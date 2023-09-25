//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CRangeTest.h
//
//	@doc:
//		Test for ranges
//---------------------------------------------------------------------------
#ifndef SPQOPT_CRangeTest_H
#define SPQOPT_CRangeTest_H

#include "spqos/base.h"

#include "spqopt/base/CRange.h"

#include "unittest/spqopt/CTestUtils.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CRangeTest
//
//	@doc:
//		Static unit tests for ranges
//
//---------------------------------------------------------------------------
class CRangeTest
{
	typedef IDatum *(*PfPdatum)(CMemoryPool *mp, INT i);

private:
	static SPQOS_RESULT EresInitAndCheckRanges(CMemoryPool *mp, IMDId *mdid,
											  PfPdatum pf);

	static void TestRangeRelationship(CMemoryPool *mp, CRange *prange1,
									  CRange *prange2, CRange *prange3,
									  CRange *prange4, CRange *prange5);

	static void PrintRange(CMemoryPool *mp, CColRef *colref, CRange *prange);

	// int2 datum
	static IDatum *CreateInt2Datum(CMemoryPool *mp, INT i);

	// int4 datum
	static IDatum *CreateInt4Datum(CMemoryPool *mp, INT i);

	// int8 datum
	static IDatum *CreateInt8Datum(CMemoryPool *mp, INT li);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_CRangeInt2();
	static SPQOS_RESULT EresUnittest_CRangeInt4();
	static SPQOS_RESULT EresUnittest_CRangeInt8();
	static SPQOS_RESULT EresUnittest_CRangeFromScalar();

};	// class CRangeTest
}  // namespace spqopt

#endif	// !SPQOPT_CRangeTest_H


// EOF
