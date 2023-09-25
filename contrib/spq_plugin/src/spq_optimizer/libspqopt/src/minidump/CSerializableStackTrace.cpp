//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSerializableStackTrace.cpp
//
//	@doc:
//		Serializable stack trace object
//---------------------------------------------------------------------------

#include "spqopt/minidump/CSerializableStackTrace.h"

#include "spqos/base.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/task/CTask.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CDXLSections.h"

#define SPQOPT_MINIDUMP_BUF_SIZE (1024 * 4)

using namespace spqos;
using namespace spqopt;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CSerializableStackTrace::CSerializableStackTrace
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CSerializableStackTrace::CSerializableStackTrace() : CSerializable()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CSerializableStackTrace::~CSerializableStackTrace
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CSerializableStackTrace::~CSerializableStackTrace()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CSerializableStackTrace::Serialize
//
//	@doc:
//		Serialize contents into provided stream
//
//---------------------------------------------------------------------------
void
CSerializableStackTrace::Serialize(COstream &oos)
{
	if (!ITask::Self()->HasPendingExceptions())
	{
		// no pending exception: no need to serialize stack trace
		return;
	}
	WCHAR wszStackBuffer[SPQOPT_MINIDUMP_BUF_SIZE];
	CWStringStatic str(wszStackBuffer, SPQOS_ARRAY_SIZE(wszStackBuffer));

	str.AppendFormat(CDXLSections::m_wszStackTraceHeader);

	CErrorContext *perrctxt = CTask::Self()->ConvertErrCtxt();
	perrctxt->GetStackDescriptor()->AppendTrace(&str);

	str.AppendFormat(CDXLSections::m_wszStackTraceFooter);

	oos << wszStackBuffer;
}

// EOF
