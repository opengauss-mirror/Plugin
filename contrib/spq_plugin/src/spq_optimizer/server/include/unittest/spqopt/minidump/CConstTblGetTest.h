//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CConstTblGetTest_H
#define SPQOPT_CConstTblGetTest_H

#include "spqos/base.h"

namespace spqopt
{
class CConstTblGetTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CConstTblGetTest
}  // namespace spqopt

#endif	// !SPQOPT_CConstTblGetTest_H

// EOF
