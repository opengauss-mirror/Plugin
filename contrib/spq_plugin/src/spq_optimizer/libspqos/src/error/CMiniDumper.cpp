//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumper.cpp
//
//	@doc:
//		Partial implementation of interface for minidump handler
//---------------------------------------------------------------------------

#include "spqos/error/CMiniDumper.h"

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/string/CWStringConst.h"
#include "spqos/task/CTask.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMiniDumper::CMiniDumper
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMiniDumper::CMiniDumper(CMemoryPool *mp)
	: m_mp(mp), m_initialized(false), m_finalized(false), m_oos(NULL)
{
	SPQOS_ASSERT(NULL != mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumper::~CMiniDumper
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMiniDumper::~CMiniDumper()
{
	if (m_initialized)
	{
		CTask *task = CTask::Self();

		SPQOS_ASSERT(NULL != task);

		task->ConvertErrCtxt()->Unregister(
#ifdef SPQOS_DEBUG
			this
#endif	// SPQOS_DEBUG
		);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumper::Init
//
//	@doc:
//		Initialize
//
//---------------------------------------------------------------------------
void
CMiniDumper::Init(COstream *oos)
{
	SPQOS_ASSERT(!m_initialized);
	SPQOS_ASSERT(!m_finalized);

	CTask *task = CTask::Self();

	SPQOS_ASSERT(NULL != task);

	m_oos = oos;

	task->ConvertErrCtxt()->Register(this);

	m_initialized = true;

	SerializeHeader();
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumper::Finalize
//
//	@doc:
//		Finalize
//
//---------------------------------------------------------------------------
void
CMiniDumper::Finalize()
{
	SPQOS_ASSERT(m_initialized);
	SPQOS_ASSERT(!m_finalized);

	SerializeFooter();

	m_finalized = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CMiniDumper::GetOStream
//
//	@doc:
//		Get stream to serialize to
//
//---------------------------------------------------------------------------
COstream &
CMiniDumper::GetOStream()
{
	SPQOS_ASSERT(m_initialized);

	return *m_oos;
}


// EOF
