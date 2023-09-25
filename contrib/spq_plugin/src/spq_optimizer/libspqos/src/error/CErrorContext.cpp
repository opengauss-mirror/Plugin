//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CErrorContext.cpp
//
//	@doc:
//		Implements context container for error handling
//---------------------------------------------------------------------------

#include "spqos/error/CErrorContext.h"

#include "spqos/error/CMessageRepository.h"
#include "spqos/error/CMiniDumper.h"
#include "spqos/error/CSerializable.h"
#include "spqos/io/ioutils.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/utils.h"

using namespace spqos;


// logger buffer must be large enough to store error messages
SPQOS_CPL_ASSERT(SPQOS_ERROR_MESSAGE_BUFFER_SIZE <= SPQOS_LOG_ENTRY_BUFFER_SIZE);


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::CErrorContext
//
//	@doc:
//
//---------------------------------------------------------------------------
CErrorContext::CErrorContext(CMiniDumper *mini_dumper_handle)
	: m_exception(CException::m_invalid_exception),
	  m_severity(CException::ExsevError),
	  m_pending(false),
	  m_rethrown(false),
	  m_serializing(false),
	  m_static_buffer(m_error_msg, SPQOS_ARRAY_SIZE(m_error_msg)),
	  m_mini_dumper_handle(mini_dumper_handle)
{
	m_serializable_objects_list.Init(
		SPQOS_OFFSET(CSerializable, m_err_ctxt_link));
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::~CErrorContext
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CErrorContext::~CErrorContext()
{
	SPQOS_ASSERT(!m_pending && "unhandled error pending");
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::Reset
//
//	@doc:
//		Clean up error context
//
//---------------------------------------------------------------------------
void
CErrorContext::Reset()
{
	SPQOS_ASSERT(m_pending);

	m_pending = false;
	m_rethrown = false;
	m_serializing = false;
	m_exception = CException::m_invalid_exception;
	m_static_buffer.Reset();
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::Record
//
//	@doc:
//		Grab error context and save it off and format error message
//
//---------------------------------------------------------------------------
void
CErrorContext::Record(CException &exc, VA_LIST vl)
{
	if (m_serializing)
		return;

#ifdef SPQOS_DEBUG
	if (m_pending)
	{
		// reset pending flag so we can throw from here
		m_pending = false;

		SPQOS_ASSERT(!"Pending error unhandled when raising new error");
	}
#endif	// SPQOS_DEBUG

	m_pending = true;
	m_exception = exc;

	// store stack, skipping current frame
	m_stack_descriptor.BackTrace(1);

	ELocale locale = ITask::Self()->Locale();
	CMessage *msg =
		CMessageRepository::GetMessageRepository()->LookupMessage(exc, locale);
	msg->Format(&m_static_buffer, vl);

	m_severity = msg->GetSeverity();

	if (SPQOS_FTRACE(EtracePrintExceptionOnRaise))
	{
		std::wcerr << SPQOS_WSZ_LIT("Exception: ") << m_static_buffer.GetBuffer()
				   << std::endl;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::AppendErrno
//
//	@doc:
//		Print errno
//
//---------------------------------------------------------------------------
void
CErrorContext::AppendErrnoMsg()
{
	SPQOS_ASSERT(m_pending);
	SPQOS_ASSERT(SPQOS_MATCH_EX(m_exception, CException::ExmaSystem,
							  CException::ExmiIOError) ||
				SPQOS_MATCH_EX(m_exception, CException::ExmaSystem,
							  CException::ExmiNetError));
	SPQOS_ASSERT(0 < errno && "Errno has not been set");

	// get errno description
	clib::Strerror_r(errno, m_system_error_msg,
					 SPQOS_ARRAY_SIZE(m_system_error_msg));

	m_static_buffer.AppendFormat(SPQOS_WSZ_LIT(" (%s)"), m_system_error_msg);
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::CopyPropErrCtxt
//
//	@doc:
//		Copy necessary info for error propagation
//
//---------------------------------------------------------------------------
void
CErrorContext::CopyPropErrCtxt(const IErrorContext *err_ctxt)
{
	SPQOS_ASSERT(!m_pending);

	m_pending = true;

	// copy exception
	m_exception = err_ctxt->GetException();

	// copy error message
	m_static_buffer.Reset();
	m_static_buffer.Append(
		&(reinterpret_cast<const CErrorContext *>(err_ctxt)->m_static_buffer));

	// copy severity
	m_severity = err_ctxt->GetSeverity();
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorContext::Serialize
//
//	@doc:
//		Serialize registered objects
//
//---------------------------------------------------------------------------
void
CErrorContext::Serialize()
{
	if (m_serializing)
		return;

	if (NULL == m_mini_dumper_handle || m_serializable_objects_list.IsEmpty())
	{
		return;
	}

	m_serializing = true;

	// Abort might throw an error, so prevent aborting to
	// avoid recursion.
	CAutoSuspendAbort asa;
	// get mini-dumper's stream to serialize to
	COstream &oos = m_mini_dumper_handle->GetOStream();

	// serialize objects to reserved space
	m_mini_dumper_handle->SerializeEntryHeader();

	for (CSerializable *serializable_obj = m_serializable_objects_list.First();
		 NULL != serializable_obj;
		 serializable_obj = m_serializable_objects_list.Next(serializable_obj))
	{
		serializable_obj->Serialize(oos);
	}

	m_mini_dumper_handle->SerializeEntryFooter();

	m_serializing = false;
}


// EOF
