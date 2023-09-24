//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CFSimulatorTestExt.h
//
//	@doc:
//      Extended FS tests
//---------------------------------------------------------------------------
#ifndef SPQOS_CFSimulatorTestExt_H
#define SPQOS_CFSimulatorTestExt_H

#include "spqos/error/CFSimulator.h"

#ifdef SPQOS_FPSIMULATOR

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CFSimulatorTestExt
//
//	@doc:
//		Extended unittests for f-simulator
//
//---------------------------------------------------------------------------
class CFSimulatorTestExt
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_OOM();
	static SPQOS_RESULT EresUnittest_Abort();
	static SPQOS_RESULT EresUnittest_IOError();
	static SPQOS_RESULT EresUnittest_NetError();

	// simulate exceptions of given type
	static SPQOS_RESULT EresUnittest_SimulateException(ULONG major, ULONG minor);

};	// CFSimulatorTestExt
}  // namespace spqos

#endif	// SPQOS_FPSIMULATOR

#endif	// !SPQOS_CFSimulatorTestExt_H

// EOF
