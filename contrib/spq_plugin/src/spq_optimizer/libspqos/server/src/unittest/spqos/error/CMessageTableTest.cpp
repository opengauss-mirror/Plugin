//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMessageTableTest.cpp
//
//	@doc:
//		Tests for CMessageTable
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CMessageTableTest.h"

#include "spqos/assert.h"
#include "spqos/error/CMessageTable.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"


using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMessageTableTest::EresUnittest
//
//	@doc:
//		Function for raising assert exceptions; again, encapsulated in a function
//		to facilitate debugging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageTableTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMessageTableTest::EresUnittest_Basic),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CMessageTableTest::EresUnittest_Basic
//
//	@doc:
//		Create message table and insert all standard messages;
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMessageTableTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CMessageTable *pmt =
		SPQOS_NEW(mp) CMessageTable(mp, SPQOS_MSGTAB_SIZE, ElocEnUS_Utf8);

	// insert all system messages
	for (ULONG ul = 0; ul < CException::ExmiSentinel; ul++)
	{
		CMessage *pmsg = CMessage::GetMessage(ul);
		if (CException::m_invalid_exception != pmsg->m_exception)
		{
			pmt->AddMessage(pmsg);

#ifdef SPQOS_DEBUG
			CMessage *pmsgLookedup = pmt->LookupMessage(pmsg->m_exception);
			SPQOS_ASSERT(pmsg == pmsgLookedup && "Lookup failed");
#endif	// SPQOS_DEBUG
		}
	}

	SPQOS_DELETE(pmt);

	return SPQOS_OK;
}

// EOF
