//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMessageTest.cpp
//
//	@doc:
//		Tests for CMessage
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CMessageTest.h"

#include "spqos/assert.h"
#include "spqos/error/CMessage.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"


using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMessageTest::EresUnittest
//
//	@doc:
//		Function for raising assert exceptions; again, encapsulated in a function
//		to facilitate debugging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMessageTest::EresUnittest_BasicWrapper),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CMessageTest::EresUnittest_BasicWrapper
//
//	@doc:
//		Wrapper around basic test to provide va_list arguments
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageTest::EresUnittest_BasicWrapper()
{
	// create memory pool of 128KB
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	return EresUnittest_Basic(mp,
							  // parameters to Assert message
							  __FILE__, __LINE__, SPQOS_WSZ_LIT("!true"));
}


//---------------------------------------------------------------------------
//	@function:
//		CMessageTest::EresUnittest_Basic
//
//	@doc:
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageTest::EresUnittest_Basic(const void *pv, ...)
{
	const ULONG size = 2048;

	CMemoryPool *mp = (CMemoryPool *) pv;

	// take pre-defined assertion exc message
	CMessage *pmsg = CMessage::GetMessage(CException::ExmiAssert);

	SPQOS_ASSERT(SPQOS_MATCH_EX(pmsg->m_exception, CException::ExmaSystem,
							  CException::ExmiAssert));

	// target buffer for format test
	WCHAR *wsz = SPQOS_NEW_ARRAY(mp, WCHAR, size);
	CWStringStatic wss(wsz, size);

	VA_LIST vl;
	VA_START(vl, pv);

	// manufacture an OOM message (no additional parameters)
	pmsg->Format(&wss, vl);

	VA_END(vl);

	SPQOS_TRACE(wsz);

	SPQOS_DELETE_ARRAY(wsz);

	return SPQOS_OK;
}

// EOF
