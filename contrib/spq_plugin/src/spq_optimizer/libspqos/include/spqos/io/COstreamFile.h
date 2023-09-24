//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COstreamFile.h
//
//	@doc:
//		Output file stream class;
//---------------------------------------------------------------------------
#ifndef SPQOS_COstreamFile_H
#define SPQOS_COstreamFile_H

#include "spqos/io/CFileWriter.h"
#include "spqos/io/COstream.h"
#include "spqos/io/ioutils.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		COstreamFile
//
//	@doc:
//		Implements an output stream writing to a file
//
//---------------------------------------------------------------------------
class COstreamFile : public COstream
{
private:
	// underlying file writer
	CFileWriter m_file_writer;

	// private copy ctor
	COstreamFile(const COstreamFile &);

public:
	// please see comments in COstream.h for an explanation
	using COstream::operator<<;

	// ctor
	COstreamFile(const CHAR *file_path,
				 ULONG permission_bits = S_IRUSR | S_IWUSR);

	// dtor
	virtual ~COstreamFile();

	// implement << operator
	virtual IOstream &operator<<(const WCHAR *);
};

}  // namespace spqos

#endif	// !SPQOS_COstreamFile_H

// EOF
