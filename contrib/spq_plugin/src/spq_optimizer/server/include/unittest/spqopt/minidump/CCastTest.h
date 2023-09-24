//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CCastTest_H
#define SPQOPT_CCastTest_H

#include "spqos/base.h"

namespace spqopt
{
class CCastTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CCastTest
}  // namespace spqopt

#endif	// !SPQOPT_CCastTest_H

// EOF
