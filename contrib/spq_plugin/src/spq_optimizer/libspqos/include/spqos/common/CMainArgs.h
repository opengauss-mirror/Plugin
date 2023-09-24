//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMainArgs.h
//
//	@doc:
//		simple wrapper to pass standard args from main to other routines
//---------------------------------------------------------------------------
#ifndef SPQOS_CMainArgs_H
#define SPQOS_CMainArgs_H

#include "spqos/types.h"


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CMainArgs
//
//	@doc:
//		Main args, following standard convention int, char**
//
//---------------------------------------------------------------------------
class CMainArgs
{
private:
	// number of arguments
	ULONG m_argc;

	// pointer to string array
	const CHAR **m_argv;

	// format string
	const CHAR *m_fmt;

	// saved option params
	CHAR *m_optarg;
	INT m_optind;
	INT m_optopt;
	INT m_opterr;
#ifdef SPQOS_Darwin
	INT m_optreset;
#endif	// SPQOS_Darwin

public:
	// ctor
	CMainArgs(ULONG ulArgs, const CHAR **rgszArgs, const CHAR *szFmt);

	// dtor -- restores option params
	~CMainArgs();

	// getopt functionality
	BOOL Getopt(CHAR *ch);

};	// class CMainArgs
}  // namespace spqos

#endif	// SPQOS_CMainArgs_H

// EOF
