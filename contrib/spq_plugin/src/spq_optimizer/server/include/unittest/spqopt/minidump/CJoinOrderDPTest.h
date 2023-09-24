//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software
//
//	@filename:
//		CJoinOrderDPTest.h
//
//	@doc:
//		Testing guc for disabling dynamic join order algorithm
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJoinOrderDPTest_H
#define SPQOPT_CJoinOrderDPTest_H

#include "spqos/base.h"

namespace spqopt
{
class CJoinOrderDPTest
{
public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();
};	// class CJoinOrderDPTest
}  // namespace spqopt

#endif	// !SPQOPT_CJoinOrderDPTest_H

// EOF
