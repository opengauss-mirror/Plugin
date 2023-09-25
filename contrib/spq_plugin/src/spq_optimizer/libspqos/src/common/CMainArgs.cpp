//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMainArgs.cpp
//
//	@doc:
//		Implementation of getopt abstraction
//---------------------------------------------------------------------------

#include "spqos/common/CMainArgs.h"

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CMainArgs::CMainArgs
//
//	@doc:
//		ctor -- saves off all opt params
//
//---------------------------------------------------------------------------
CMainArgs::CMainArgs(ULONG argc, const CHAR **argv, const CHAR *fmt)
	: m_argc(argc),
	  m_argv(argv),
	  m_fmt(fmt),
	  m_optarg(optarg),
	  m_optind(optind),
	  m_optopt(optopt),
	  m_opterr(opterr)
#ifdef SPQOS_Darwin
	  ,
	  m_optreset(optreset)
#endif	// SPQOS_Darwin
{
	// initialize external opt params
	optarg = NULL;
	optind = 1;
	optopt = 1;
	opterr = 1;
#ifdef SPQOS_Darwin
	optreset = 1;
#endif	// SPQOS_Darwin
}


//---------------------------------------------------------------------------
//	@function:
//		CMainArgs::~CMainArgs
//
//	@doc:
//		dtor -- restore previous opt params
//
//---------------------------------------------------------------------------
CMainArgs::~CMainArgs()
{
	optarg = m_optarg;
	optind = m_optind;
	optopt = m_optopt;
	opterr = m_opterr;
#ifdef SPQOS_Darwin
	optreset = m_optreset;
#endif	// SPQOS_Darwin
}


//---------------------------------------------------------------------------
//	@function:
//		CMainArgs::Getopt
//
//	@doc:
//		wraps getopt logic
//
//---------------------------------------------------------------------------
BOOL
CMainArgs::Getopt(CHAR *pch)
{
	SPQOS_ASSERT(NULL != pch);

	INT res = clib::Getopt(m_argc, const_cast<CHAR **>(m_argv), m_fmt);

	if (res != -1)
	{
		*pch = (CHAR) res;
		return true;
	}

	return false;
}

// EOF
