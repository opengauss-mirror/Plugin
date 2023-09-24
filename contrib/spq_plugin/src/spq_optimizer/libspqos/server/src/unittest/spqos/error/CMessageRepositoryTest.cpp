//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMessageRepositoryTest.cpp
//
//	@doc:
//		Tests for CMessageTable
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CMessageRepositoryTest.h"

#include "spqos/assert.h"
#include "spqos/base.h"
#include "spqos/error/CMessageRepository.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"


using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMessageRepositoryTest::EresUnittest
//
//	@doc:
//		unit test driver
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageRepositoryTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMessageRepositoryTest::EresUnittest_Basic),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CMessageRepositoryTest::EresUnittest_Basic
//
//	@doc:
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageRepositoryTest::EresUnittest_Basic()
{
#ifdef SPQOS_DEBUG
	// lookup OOM message
	CMessage *pmsg = CMessageRepository::GetMessageRepository()->LookupMessage(
		CException(CException::ExmaSystem, CException::ExmiOOM), ElocEnUS_Utf8);

	SPQOS_ASSERT(SPQOS_MATCH_EX(pmsg->m_exception, CException::ExmaSystem,
							  CException::ExmiOOM));

	SPQOS_ASSERT(pmsg == CMessage::GetMessage(CException::ExmiOOM));

	// attempt looking up OOM message in German -- should return enUS OOM message;
	pmsg = CMessageRepository::GetMessageRepository()->LookupMessage(
		CException(CException::ExmaSystem, CException::ExmiOOM), ElocGeDE_Utf8);

	SPQOS_ASSERT(SPQOS_MATCH_EX(pmsg->m_exception, CException::ExmaSystem,
							  CException::ExmiOOM));

	SPQOS_ASSERT(pmsg == CMessage::GetMessage(CException::ExmiOOM));

	SPQOS_TRY
	{
		// attempt looking up message with invalid exception code
		pmsg = CMessageRepository::GetMessageRepository()->LookupMessage(
			CException(CException::ExmaSystem, 1234567), ElocEnUS_Utf8);
	}
	SPQOS_CATCH_EX(exc)
	{
		SPQOS_ASSERT(
			SPQOS_MATCH_EX(exc, CException::ExmaSystem, CException::ExmiAssert));
		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

#endif	// SPQOS_DEBUG

	return SPQOS_OK;
}

// EOF
