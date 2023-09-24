//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CNameTest.cpp
//
//	@doc:
//      Test for CName
//---------------------------------------------------------------------------

#include "unittest/spqopt/metadata/CNameTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/metadata/CName.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CNameTest::EresUnittest
//
//	@doc:
//		Unittest for metadata names
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CNameTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(CNameTest::EresUnittest_Basic),
						SPQOS_UNITTEST_FUNC(CNameTest::EresUnittest_Ownership)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CNameTest::EresUnittest_Basic
//
//	@doc:
//		basic naming, assignment thru copy constructors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CNameTest::EresUnittest_Basic()
{
	CWStringConst strName(SPQOS_WSZ_LIT("nametest"));
	CName name1(&strName);
	CName name2(name1);
	CName name3 = name2;

	SPQOS_ASSERT(name1.Equals(name2));
	SPQOS_ASSERT(name1.Equals(name3));
	SPQOS_ASSERT(name2.Equals(name3));

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CNameTest::EresUnittest_Ownership
//
//	@doc:
//		basic name with deep copies
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CNameTest::EresUnittest_Ownership()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringConst strName(SPQOS_WSZ_LIT("nametest"));
	CName name1(&strName);

	CName name2(mp, name1);
	CName name3(mp, name2);

	SPQOS_ASSERT(name1.Equals(name2));
	SPQOS_ASSERT(name1.Equals(name3));
	SPQOS_ASSERT(name2.Equals(name3));

	return SPQOS_OK;
}


// EOF
