//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CIOUtils.cpp
//
//	@doc:
//		Implementation of optimizer I/O utilities
//---------------------------------------------------------------------------

#include "spqopt/base/CIOUtils.h"

#include "spqos/base.h"
#include "spqos/io/COstreamFile.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CWorker.h"

using namespace spqopt;



//---------------------------------------------------------------------------
//	@function:
//		CIOUtils::Dump
//
//	@doc:
//		Dump given string to output file
//
//---------------------------------------------------------------------------
void
CIOUtils::Dump(CHAR *file_name, CHAR *sz)
{
	CAutoSuspendAbort asa;

	const ULONG ulWrPerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	SPQOS_TRY
	{
		CFileWriter fw;
		fw.Open(file_name, ulWrPerms);
		const BYTE *pb = reinterpret_cast<const BYTE *>(sz);
		ULONG_PTR ulpLength = (ULONG_PTR) clib::Strlen(sz);
		fw.Write(pb, ulpLength);
		fw.Close();
	}
	SPQOS_CATCH_EX(ex)
	{
		// ignore exceptions during dumping
		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;
}


// EOF
