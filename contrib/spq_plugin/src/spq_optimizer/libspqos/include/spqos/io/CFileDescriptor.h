//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CFileDescriptor.h
//
//	@doc:
//		File descriptor abstraction
//---------------------------------------------------------------------------
#ifndef SPQOS_CFileDescriptor_H
#define SPQOS_CFileDescriptor_H

#include "spqos/types.h"

#define SPQOS_FILE_NAME_BUF_SIZE (1024)
#define SPQOS_FILE_DESCR_INVALID (-1)

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CFileDescriptor
//
//	@doc:
//		File handler abstraction;
//
//---------------------------------------------------------------------------

class CFileDescriptor
{
private:
	// file descriptor
	INT m_file_descriptor;

	// no copy ctor
	CFileDescriptor(const CFileDescriptor &);

protected:
	// ctor -- accessible through inheritance only
	CFileDescriptor();

	// dtor -- accessible through inheritance only
	virtual ~CFileDescriptor();

	// get file descriptor
	INT
	GetFileDescriptor() const
	{
		return m_file_descriptor;
	}

	// open file
	void OpenFile(const CHAR *file_path, ULONG mode, ULONG permission_bits);

	// close file
	void CloseFile();

public:
	// check if file is open
	BOOL
	IsFileOpen() const
	{
		return (SPQOS_FILE_DESCR_INVALID != m_file_descriptor);
	}

};	// class CFile
}  // namespace spqos

#endif	// !SPQOS_CFile_H

// EOF
