//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDistributionSpecTest.h
//
//	@doc:
//		Test for distribution spec
//---------------------------------------------------------------------------
#ifndef SPQOS_CDistributionSpecTest_H
#define SPQOS_CDistributionSpecTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDistributionSpecTest
//
//	@doc:
//		Static unit tests for distribution specs
//
//---------------------------------------------------------------------------
class CDistributionSpecTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Any();
	static SPQOS_RESULT EresUnittest_Singleton();
	static SPQOS_RESULT EresUnittest_Replicated();
	static SPQOS_RESULT EresUnittest_Universal();
	static SPQOS_RESULT EresUnittest_Random();
	static SPQOS_RESULT EresUnittest_Hashed();
#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_NegativeAny();
	static SPQOS_RESULT EresUnittest_NegativeUniversal();
	static SPQOS_RESULT EresUnittest_NegativeRandom();
#endif	// SPQOS_DEBUG

};	// class CDistributionSpecTest
}  // namespace spqopt

#endif	// !SPQOS_CDistributionSpecTest_H


// EOF
