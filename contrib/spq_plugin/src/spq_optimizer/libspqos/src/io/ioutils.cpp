//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		ioutils.cpp
//
//	@doc:
//		Implementation of I/O utilities
//---------------------------------------------------------------------------

#include "spqos/io/ioutils.h"

#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CFSimulator.h"
#include "spqos/error/CLogger.h"
#include "spqos/string/CStringStatic.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/task/CTaskContext.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		ioutils::CheckState
//
//	@doc:
//		Check state of file or directory
//
//---------------------------------------------------------------------------
void
spqos::ioutils::CheckState(const CHAR *file_path, SFileStat *file_state)
{
	SPQOS_ASSERT(NULL != file_path);
	SPQOS_ASSERT(NULL != file_state);

	// reset file state
	(void) clib::Memset(file_state, 0, sizeof(*file_state));

	INT res = -1;

	// check to simulate I/O error
	SPQOS_CHECK_SIM_IO_ERR(&res, stat(file_path, file_state));

	if (0 != res)
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::FStat
//
//	@doc:
//		Check state of file or directory by file descriptor
//
//---------------------------------------------------------------------------
void
spqos::ioutils::CheckStateUsingFileDescriptor(const INT file_descriptor,
											 SFileStat *file_state)
{
	SPQOS_ASSERT(NULL != file_state);

	// reset file state
	(void) clib::Memset(file_state, 0, sizeof(*file_state));

	INT res = -1;

	// check to simulate I/O error
	SPQOS_CHECK_SIM_IO_ERR(&res, fstat(file_descriptor, file_state));

	if (0 != res)
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::PathExists
//
//	@doc:
//		Check if path is mapped to an accessible file or directory
//
//---------------------------------------------------------------------------
BOOL
spqos::ioutils::PathExists(const CHAR *file_path)
{
	SPQOS_ASSERT(NULL != file_path);

	SFileStat fs;

	INT res = stat(file_path, &fs);

	return (0 == res);
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::IsDir
//
//	@doc:
//		Check if path is directory
//
//---------------------------------------------------------------------------
BOOL
spqos::ioutils::IsDir(const CHAR *file_path)
{
	SPQOS_ASSERT(NULL != file_path);

	SFileStat fs;
	CheckState(file_path, &fs);

	return S_ISDIR(fs.st_mode);
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::IsFile
//
//	@doc:
//		Check if path is file
//
//---------------------------------------------------------------------------
BOOL
spqos::ioutils::IsFile(const CHAR *file_path)
{
	SPQOS_ASSERT(NULL != file_path);

	SFileStat fs;
	CheckState(file_path, &fs);

	return S_ISREG(fs.st_mode);
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::FileSize
//
//	@doc:
//		Get file size by file path
//
//---------------------------------------------------------------------------
ULLONG
spqos::ioutils::FileSize(const CHAR *file_path)
{
	SPQOS_ASSERT(NULL != file_path);
	SPQOS_ASSERT(IsFile(file_path));

	SFileStat fs;
	CheckState(file_path, &fs);

	return fs.st_size;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::FileSize
//
//	@doc:
//		Get file size by file descriptor
//
//---------------------------------------------------------------------------
ULLONG
spqos::ioutils::FileSize(const INT file_descriptor)
{
	SFileStat fs;
	CheckStateUsingFileDescriptor(file_descriptor, &fs);

	return fs.st_size;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::CheckFilePermissions
//
//	@doc:
//		Check permissions
//
//---------------------------------------------------------------------------
BOOL
spqos::ioutils::CheckFilePermissions(const CHAR *file_path,
									ULONG permission_bits)
{
	SPQOS_ASSERT(NULL != file_path);

	SFileStat fs;
	CheckState(file_path, &fs);

	return (permission_bits == (fs.st_mode & permission_bits));
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::CreateDir
//
//	@doc:
//		Create directory with specific permissions
//
//---------------------------------------------------------------------------
void
spqos::ioutils::CreateDir(const CHAR *file_path, ULONG permission_bits)
{
	SPQOS_ASSERT(NULL != file_path);

	INT res = -1;

	// check to simulate I/O error
	SPQOS_CHECK_SIM_IO_ERR(&res, mkdir(file_path, (MODE_T) permission_bits));

	if (0 != res)
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::RemoveDir
//
//	@doc:
//		Delete directory
//
//---------------------------------------------------------------------------
void
spqos::ioutils::RemoveDir(const CHAR *file_path)
{
	SPQOS_ASSERT(NULL != file_path);
	SPQOS_ASSERT(IsDir(file_path));

	INT res = -1;

	// delete existing directory and check to simulate I/O error
	SPQOS_CHECK_SIM_IO_ERR(&res, rmdir(file_path));

	if (0 != res)
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::Move
//
//	@doc:
//		Move file from old path to new path;
//		any file currently mapped to new path is deleted
//
//---------------------------------------------------------------------------
void
spqos::ioutils::Move(const CHAR *old_path, const CHAR *szNew)
{
	SPQOS_ASSERT(NULL != old_path);
	SPQOS_ASSERT(NULL != szNew);
	SPQOS_ASSERT(IsFile(old_path));

	// delete any existing file with the new path
	if (PathExists(szNew))
	{
		Unlink(szNew);
	}

	INT res = -1;

	// rename file and check to simulate I/O error
	SPQOS_CHECK_SIM_IO_ERR(&res, rename(old_path, szNew));

	if (0 != res)
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::Unlink
//
//	@doc:
//		Delete file
//
//---------------------------------------------------------------------------
void
spqos::ioutils::Unlink(const CHAR *file_path)
{
	SPQOS_ASSERT(NULL != file_path);

	// delete existing file
	(void) unlink(file_path);
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::OpenFile
//
//	@doc:
//		Open a file;
//		It shall establish the connection between a file
//		and a file descriptor
//
//---------------------------------------------------------------------------
INT
spqos::ioutils::OpenFile(const CHAR *file_path, INT mode, INT permission_bits)
{
	SPQOS_ASSERT(NULL != file_path);

	INT res = open(file_path, mode, permission_bits);

	SPQOS_ASSERT((0 <= res) || (EINVAL != errno));

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::CloseFile
//
//	@doc:
//		Close a file descriptor
//
//---------------------------------------------------------------------------
INT
spqos::ioutils::CloseFile(INT file_descriptor)
{
	INT res = close(file_descriptor);

	SPQOS_ASSERT(0 == res || EBADF != errno);

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::GetFileState
//
//	@doc:
//		Get file status
//
//---------------------------------------------------------------------------
INT
spqos::ioutils::GetFileState(INT file_descriptor, SFileStat *file_state)
{
	SPQOS_ASSERT(NULL != file_state);

	INT res = fstat(file_descriptor, file_state);

	SPQOS_ASSERT(0 == res || EBADF != errno);

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::Write
//
//	@doc:
//		Write to a file descriptor
//
//---------------------------------------------------------------------------
INT_PTR
spqos::ioutils::Write(INT file_descriptor, const void *buffer,
					 const ULONG_PTR ulpCount)
{
	SPQOS_ASSERT(NULL != buffer);
	SPQOS_ASSERT(0 < ulpCount);
	SPQOS_ASSERT(ULONG_PTR_MAX / 2 > ulpCount);

	SSIZE_T res = write(file_descriptor, buffer, ulpCount);

	SPQOS_ASSERT((0 <= res) || EBADF != errno);

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::Read
//
//	@doc:
//		Read from a file descriptor
//
//---------------------------------------------------------------------------
INT_PTR
spqos::ioutils::Read(INT file_descriptor, void *buffer, const ULONG_PTR ulpCount)
{
	SPQOS_ASSERT(NULL != buffer);
	SPQOS_ASSERT(0 < ulpCount);
	SPQOS_ASSERT(ULONG_PTR_MAX / 2 > ulpCount);

	SSIZE_T res = read(file_descriptor, buffer, ulpCount);

	SPQOS_ASSERT((0 <= res) || EBADF != errno);

	return res;
}

//---------------------------------------------------------------------------
//	@function:
//		ioutils::CreateTempDir
//
//	@doc:
//		Create a unique temporary directory
//
//---------------------------------------------------------------------------
void
spqos::ioutils::CreateTempDir(CHAR *dir_path)
{
	SPQOS_ASSERT(NULL != dir_path);

#ifdef SPQOS_DEBUG
	const SIZE_T ulNumOfCmp = 6;

	SIZE_T size = clib::Strlen(dir_path);

	SPQOS_ASSERT(size > ulNumOfCmp);

	SPQOS_ASSERT(0 == clib::Memcmp("XXXXXX", dir_path + (size - ulNumOfCmp),
								  ulNumOfCmp));
#endif	// SPQOS_DEBUG

	CHAR *szRes = NULL;


	// check to simulate I/O error
	SPQOS_CHECK_SIM_IO_ERR(&szRes, mkdtemp(dir_path));

	if (NULL == szRes)
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIOError, errno);
	}

	return;
}


#ifdef SPQOS_FPSIMULATOR


//---------------------------------------------------------------------------
//	@function:
//		FSimulateIOErrorInternal
//
//	@doc:
//		Inject I/O exception
//
//---------------------------------------------------------------------------
static BOOL
FSimulateIOErrorInternal(INT error_no, const CHAR *file, ULONG line_num)
{
	BOOL fRes = false;

	ITask *ptsk = ITask::Self();
	if (NULL != ptsk && ptsk->IsTraceSet(EtraceSimulateIOError) &&
		CFSimulator::FSim()->NewStack(CException::ExmaSystem,
									  CException::ExmiIOError) &&
		!SPQOS_MATCH_EX(ptsk->GetErrCtxt()->GetException(),
					   CException::ExmaSystem, CException::ExmiIOError))
	{
		// disable simulation temporarily to log injection
		CAutoTraceFlag(EtraceSimulateIOError, false);

		SPQOS_TRACE_FORMAT_ERR("Simulating I/O error at %s:%d", file, line_num);

		errno = error_no;

		if (ptsk->GetErrCtxt()->IsPending())
		{
			ptsk->GetErrCtxt()->Reset();
		}

		// inject I/O error
		fRes = true;
	}

	return fRes;
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::SimulateIOError
//
//	@doc:
//		Inject I/O exception for functions
//		whose returned value type is INT
//
//---------------------------------------------------------------------------
BOOL
spqos::ioutils::SimulateIOError(INT *return_value, INT error_no,
							   const CHAR *file, ULONG line_num)
{
	SPQOS_ASSERT(NULL != return_value);

	*return_value = -1;

	return FSimulateIOErrorInternal(error_no, file, line_num);
}


//---------------------------------------------------------------------------
//	@function:
//		ioutils::SimulateIOError
//
//	@doc:
//		Inject I/O exception for functions
//		whose returned value type is CHAR*
//
//---------------------------------------------------------------------------
BOOL
spqos::ioutils::SimulateIOError(CHAR **return_value, INT error_no,
							   const CHAR *file, ULONG line_num)
{
	SPQOS_ASSERT(NULL != return_value);

	*return_value = NULL;

	return FSimulateIOErrorInternal(error_no, file, line_num);
}
#endif	// SPQOS_FPSIMULATOR

// EOF
