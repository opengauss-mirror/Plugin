//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CFSimulatorTest.h
//
//	@doc:
//      Unit tests for CFSimulator
//---------------------------------------------------------------------------
#ifndef SPQOS_CFSimulatorTest_H
#define SPQOS_CFSimulatorTest_H

#include "spqos/error/CFSimulator.h"

#ifdef SPQOS_FPSIMULATOR

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CFSimulatorTest
//
//	@doc:
//		Unittests for f-simulator
//
//---------------------------------------------------------------------------
class CFSimulatorTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_BasicTracking();

};	// CFSimulatorTest
}  // namespace spqos

#endif	// SPQOS_FPSIMULATOR

#endif	// !SPQOS_CFSimulatorTest_H

// EOF
