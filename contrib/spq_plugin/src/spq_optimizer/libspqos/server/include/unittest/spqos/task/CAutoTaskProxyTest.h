//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CAutoTaskProxyTest.h
//
//	@doc:
//		Test for CAutoTaskProxy
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoTaskProxyTest_H
#define SPQOS_CAutoTaskProxyTest_H

#include "spqos/task/CAutoTaskProxy.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoTaskProxyTest
//
//	@doc:
//		Unit tests for ATP class
//
//---------------------------------------------------------------------------
class CAutoTaskProxyTest
{
public:
	enum EWaitType
	{
		EwtInvalid = 0,

		EwtWait,
		EwtWaitAny,
		EwtTimedWait,
		EwtTimedWaitAny,
		EwtDestroy,

		EwtSentinel
	};

	struct STestThreadDescriptor
	{
		// pthread descriptor
		PTHREAD_T m_pthrdt;

		// thread id
		ULONG id;

		// return value
		BOOL fException;

		// Propagate Exception
		BOOL fPropagateException;

		// CMemoryPool
		CMemoryPool *m_mp;
	};

	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Wait();
	static SPQOS_RESULT EresUnittest_WaitAny();
	static SPQOS_RESULT EresUnittest_TimedWait();
	static SPQOS_RESULT EresUnittest_TimedWaitAny();
	static SPQOS_RESULT EresUnittest_Destroy();
	static SPQOS_RESULT EresUnittest_PropagateCancelError();
	static SPQOS_RESULT EresUnittest_PropagateExecError();
	static SPQOS_RESULT EresUnittest_ExecuteError();
	static SPQOS_RESULT EresUnittest_CheckErrorPropagation();

	// propagate error with/without cancel by specific value
	// need to access the private method of CTask
	static void Unittest_PropagateErrorInternal(void *(*) (void *), BOOL);
	static void *Unittest_CheckExecuteErrorInternal(void *);

	// execute *wait* functions with/without cancel
	static void Unittest_ExecuteWaitFunc(CAutoTaskProxy &, CTask *, BOOL,
										 EWaitType);

	static void *PvUnittest_Short(void *);
	static void *PvUnittest_Long(void *);
	static void *PvUnittest_Infinite(void *);
	static void *PvUnittest_Error(void *);
	static void PvUnittest_CheckAborted(CTask *ptsk);

};	// CAutoTaskProxyTest
}  // namespace spqos

#endif	// !SPQOS_CAutoTaskProxyTest_H

// EOF
