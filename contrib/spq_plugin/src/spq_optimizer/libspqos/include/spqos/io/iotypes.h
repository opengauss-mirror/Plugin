//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		iotypes.h
//
//	@doc:
//		IO type definitions for SPQOS;
//---------------------------------------------------------------------------
#ifndef SPQOS_iotypes_H
#define SPQOS_iotypes_H

#include <errno.h>
#include <sys/stat.h>


namespace spqos
{
// file state structure
typedef struct stat SFileStat;
}  // namespace spqos

#endif	// !SPQOS_iotypes_H

// EOF
