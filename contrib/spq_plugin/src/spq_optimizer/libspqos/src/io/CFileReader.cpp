//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CFileReader.cpp
//
//	@doc:
//		Implementation of file handler for raw input
//---------------------------------------------------------------------------

#include "spqos/io/CFileReader.h"

#include "spqos/base.h"
#include "spqos/io/ioutils.h"
#include "spqos/task/IWorker.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::CFileReader
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CFileReader::CFileReader()
	: CFileDescriptor(), m_file_size(0), m_file_read_size(0)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::~CFileReader
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CFileReader::~CFileReader()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::Open
//
//	@doc:
//		Open file for reading
//
//---------------------------------------------------------------------------
void
CFileReader::Open(const CHAR *file_path, const ULONG permission_bits)
{
	SPQOS_ASSERT(NULL != file_path);

	OpenFile(file_path, O_RDONLY, permission_bits);

	m_file_size = ioutils::FileSize(file_path);
}


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::Close
//
//	@doc:
//		Close file after reading
//
//---------------------------------------------------------------------------
void
CFileReader::Close()
{
	CloseFile();
	m_file_size = 0;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::Read
//
//	@doc:
//		Read bytes from file
//
//---------------------------------------------------------------------------
ULONG_PTR
CFileReader::ReadBytesToBuffer(BYTE *read_buffer,
							   const ULONG_PTR file_read_size)
{
	SPQOS_ASSERT(CFileDescriptor::IsFileOpen() &&
				"Attempt to read from invalid file descriptor");
	SPQOS_ASSERT(0 < file_read_size);
	SPQOS_ASSERT(NULL != read_buffer);

	ULONG_PTR bytes_to_read = file_read_size;

	while (0 < bytes_to_read)
	{
		INT_PTR current_byte = -1;

		// read from file and check to simulate I/O error
		SPQOS_CHECK_SIM_IO_ERR(
			&current_byte,
			ioutils::Read(GetFileDescriptor(), read_buffer, bytes_to_read));

		// reach the end of file
		if (0 == current_byte)
		{
			break;
		}

		// check for error
		if (-1 == current_byte)
		{
			// in case an interrupt was received we retry
			if (EINTR == errno)
			{
				SPQOS_CHECK_ABORT;
				continue;
			}

			SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
		}

		bytes_to_read -= current_byte;
		read_buffer += current_byte;
		m_file_read_size += current_byte;
	};

	return file_read_size - bytes_to_read;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::FileSize
//
//	@doc:
//		Get file size
//
//---------------------------------------------------------------------------
ULLONG
CFileReader::FileSize() const
{
	return m_file_size;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileReader::FileReadSize
//
//	@doc:
//		Get file read size
//
//---------------------------------------------------------------------------
ULLONG
CFileReader::FileReadSize() const
{
	return m_file_read_size;
}

// EOF
