//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COstreamFile.cpp
//
//	@doc:
//		Implementation of output file stream class;
//---------------------------------------------------------------------------

#include "spqos/io/COstreamFile.h"

#include "spqos/base.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		COstreamFile::COstreamFile
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
COstreamFile::COstreamFile(const CHAR *file_path, ULONG permission_bits)
{
	m_file_writer.Open(file_path, permission_bits);
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamFile::COstreamFile
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
COstreamFile::~COstreamFile()
{
	m_file_writer.Close();
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamFile::operator<<
//
//	@doc:
//		WCHAR write thru;
//
//---------------------------------------------------------------------------
IOstream &
COstreamFile::operator<<(const WCHAR *wsz)
{
	ULONG_PTR ulpSize = SPQOS_WSZ_LENGTH(wsz) * SPQOS_SIZEOF(WCHAR);
	m_file_writer.Write((const BYTE *) wsz, ulpSize);

	return *this;
}


// EOF
