//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPrintPrefix.cpp
//
//	@doc:
//		Implementation of print prefix class
//---------------------------------------------------------------------------

#include "spqopt/base/CPrintPrefix.h"

#include "spqos/base.h"
#include "spqos/task/IWorker.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CPrintPrefix::CPrintPrefix
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CPrintPrefix::CPrintPrefix(const CPrintPrefix *ppfx, const CHAR *sz)
	: m_ppfx(ppfx), m_sz(sz)
{
	SPQOS_ASSERT(NULL != sz);
}


//---------------------------------------------------------------------------
//	@function:
//		CPrintPrefix::OsPrint
//
//	@doc:
//		print function;
//		recursively traverse the linked list of prefixes and print them
//		in reverse order
//
//---------------------------------------------------------------------------
IOstream &
CPrintPrefix::OsPrint(IOstream &os) const
{
	SPQOS_CHECK_STACK_SIZE;

	if (NULL != m_ppfx)
	{
		(void) m_ppfx->OsPrint(os);
	}

	os << m_sz;
	return os;
}


// EOF
