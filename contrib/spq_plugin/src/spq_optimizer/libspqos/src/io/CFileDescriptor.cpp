//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CFileDescriptor.cpp
//
//	@doc:
//		File descriptor abstraction
//---------------------------------------------------------------------------


#include "spqos/io/CFileDescriptor.h"

#include "spqos/base.h"
#include "spqos/io/ioutils.h"
#include "spqos/string/CStringStatic.h"
#include "spqos/task/IWorker.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CFileDescriptor::CFileDescriptor
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CFileDescriptor::CFileDescriptor() : m_file_descriptor(SPQOS_FILE_DESCR_INVALID)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CFileDescriptor::OpenFile
//
//	@doc:
//		Open file descriptor
//
//---------------------------------------------------------------------------
void
CFileDescriptor::OpenFile(const CHAR *file_path, ULONG mode,
						  ULONG permission_bits)
{
	SPQOS_ASSERT(!IsFileOpen());

	BOOL fOpened = false;

	while (!fOpened)
	{
		m_file_descriptor = SPQOS_FILE_DESCR_INVALID;

		// create file with given mode and permissions and check to simulate I/O error
		SPQOS_CHECK_SIM_IO_ERR(
			&m_file_descriptor,
			ioutils::OpenFile(file_path, mode, permission_bits));

		// check for error
		if (SPQOS_FILE_DESCR_INVALID == m_file_descriptor)
		{
			// in case an interrupt was received we retry
			if (EINTR == errno)
			{
				SPQOS_CHECK_ABORT;

				continue;
			}

			SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
		}

		fOpened = true;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CFile::~CFile
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CFileDescriptor::~CFileDescriptor()
{
	if (IsFileOpen())
	{
		CloseFile();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CFile::CloseFile
//
//	@doc:
//		Close file
//
//---------------------------------------------------------------------------
void
CFileDescriptor::CloseFile()
{
	SPQOS_ASSERT(IsFileOpen());

	BOOL fClosed = false;

	while (!fClosed)
	{
		INT res = ioutils::CloseFile(m_file_descriptor);

		// check for error
		if (0 != res)
		{
			SPQOS_ASSERT(EINTR == errno || EIO == errno);

			// in case an interrupt was received we retry
			if (EINTR == errno)
			{
				continue;
			}
		}

		fClosed = true;
	}

	m_file_descriptor = SPQOS_FILE_DESCR_INVALID;
}

// EOF
