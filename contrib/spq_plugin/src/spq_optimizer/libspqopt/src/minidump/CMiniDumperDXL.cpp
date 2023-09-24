//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumperDXL.cpp
//
//	@doc:
//		Implementation of DXL-specific minidump handler
//---------------------------------------------------------------------------

#include "spqopt/minidump/CMiniDumperDXL.h"

#include "spqos/base.h"
#include "spqos/string/CWStringBase.h"
#include "spqos/task/CTask.h"
#include "spqos/task/CWorker.h"

#include "naucrates/dxl/xml/CDXLSections.h"

using namespace spqos;
using namespace spqdxl;
using namespace spqopt;

// size of temporary buffer for expanding XML entry header
#define SPQOPT_THREAD_HEADER_SIZE 128

//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXL::CMiniDumperDXL
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMiniDumperDXL::CMiniDumperDXL(CMemoryPool *mp) : CMiniDumper(mp)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXL::~CMiniDumperDXL
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMiniDumperDXL::~CMiniDumperDXL()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXL::SerializeHeader
//
//	@doc:
//		Serialize minidump header
//
//---------------------------------------------------------------------------
void
CMiniDumperDXL::SerializeHeader()
{
	*m_oos << CDXLSections::m_wszDocumentHeader;
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXL::SerializeFooter
//
//	@doc:
//		Serialize minidump footer
//
//---------------------------------------------------------------------------
void
CMiniDumperDXL::SerializeFooter()
{
	*m_oos << CDXLSections::m_wszDocumentFooter;
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXL::SerializeEntryHeader
//
//	@doc:
//		Serialize entry header
//
//---------------------------------------------------------------------------
void
CMiniDumperDXL::SerializeEntryHeader()
{
	WCHAR wszBuffer[SPQOPT_THREAD_HEADER_SIZE];

	CWStringStatic str(wszBuffer, SPQOS_ARRAY_SIZE(wszBuffer));
	str.AppendFormat(CDXLSections::m_wszThreadHeaderTemplate,
					 0	// thread id
	);

	*m_oos << str.GetBuffer();
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXL::SerializeEntryFooter
//
//	@doc:
//		Serialize entry footer
//
//---------------------------------------------------------------------------
void
CMiniDumperDXL::SerializeEntryFooter()
{
	*m_oos << CDXLSections::m_wszThreadFooter;
}

// EOF
