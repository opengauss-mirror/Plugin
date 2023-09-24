//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumperTest.cpp
//
//	@doc:
//		Tests for minidump handler
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CMiniDumperTest.h"

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/string/CWStringStatic.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::EresUnittest
//
//	@doc:
//		Function for testing minidump handler
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMiniDumperTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMiniDumperTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::EresUnittest_Basic
//
//	@doc:
//		Basic test for minidump handler
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMiniDumperTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CMiniDumperStream mdrs(mp);

	CWStringDynamic wstrMinidump(mp);
	COstreamString oss(&wstrMinidump);
	mdrs.Init(&oss);

	SPQOS_TRY
	{
		(void) PvRaise(NULL);
	}
	SPQOS_CATCH_EX(ex)
	{
		mdrs.Finalize();

		SPQOS_RESET_EX;

		SPQOS_TRACE(wstrMinidump.GetBuffer());
	}
	SPQOS_CATCH_END;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::PvRaise
//
//	@doc:
//		Function raising an exception
//
//---------------------------------------------------------------------------
void *
CMiniDumperTest::PvRaise(void *	 // pv
)
{
	// register stack serializer with error context
	CSerializableStack ss;

	clib::USleep(1000);

	// raise exception to trigger minidump
	SPQOS_OOM_CHECK(NULL);

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CMiniDumperStream::CMiniDumperStream
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMiniDumperTest::CMiniDumperStream::CMiniDumperStream(CMemoryPool *mp)
	: CMiniDumper(mp)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CMiniDumperStream::~CMiniDumperStream
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMiniDumperTest::CMiniDumperStream::~CMiniDumperStream()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CMiniDumperStream::UlSerializeHeader
//
//	@doc:
//		Serialize minidump header
//
//---------------------------------------------------------------------------
void
CMiniDumperTest::CMiniDumperStream::SerializeHeader()
{
	*m_oos << "\n<MINIDUMP_TEST>\n";
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CMiniDumperStream::UlSerializeFooter
//
//	@doc:
//		Serialize minidump footer
//
//---------------------------------------------------------------------------
void
CMiniDumperTest::CMiniDumperStream::SerializeFooter()
{
	*m_oos << "</MINIDUMP_TEST>\n";
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CMiniDumperStream::SerializeEntryHeader
//
//	@doc:
//		Serialize entry header
//
//---------------------------------------------------------------------------
void
CMiniDumperTest::CMiniDumperStream::SerializeEntryHeader()
{
	WCHAR wszBuffer[SPQOS_MINIDUMP_BUF_SIZE];
	CWStringStatic wstr(wszBuffer, SPQOS_ARRAY_SIZE(wszBuffer));
	wstr.AppendFormat(SPQOS_WSZ_LIT("<THREAD ID=%d>\n"), 0);

	*m_oos << wstr.GetBuffer();
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CMiniDumperStream::SerializeEntryFooter
//
//	@doc:
//		Serialize entry footer
//
//---------------------------------------------------------------------------
void
CMiniDumperTest::CMiniDumperStream::SerializeEntryFooter()
{
	*m_oos << "</THREAD>\n";
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CSerializableStack::CSerializableStack
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMiniDumperTest::CSerializableStack::CSerializableStack() : CSerializable()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CSerializableStack::~CSerializableStack
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------

CMiniDumperTest::CSerializableStack::~CSerializableStack()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperTest::CSerializableStack::Serialize
//
//	@doc:
//		Serialize object to passed stream
//
//---------------------------------------------------------------------------
void
CMiniDumperTest::CSerializableStack::Serialize(COstream &oos)
{
	WCHAR wszStackBuffer[SPQOS_MINIDUMP_BUF_SIZE];
	CWStringStatic wstr(wszStackBuffer, SPQOS_ARRAY_SIZE(wszStackBuffer));

	wstr.AppendFormat(SPQOS_WSZ_LIT("<STACK_TRACE>\n"));

	CErrorContext *perrctxt = CTask::Self()->ConvertErrCtxt();
	perrctxt->GetStackDescriptor()->AppendTrace(&wstr);

	wstr.AppendFormat(SPQOS_WSZ_LIT("</STACK_TRACE>\n"));

	oos << wstr.GetBuffer();
}


// EOF
