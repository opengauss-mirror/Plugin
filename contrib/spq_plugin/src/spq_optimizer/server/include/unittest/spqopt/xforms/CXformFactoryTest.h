//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformFactoryTest.h
//
//	@doc:
//		Unittests for management of global xform set
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformFactoryTest_H
#define SPQOPT_CXformFactoryTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformFactoryTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CXformFactoryTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CXformFactoryTest

}  // namespace spqopt


#endif	// !SPQOPT_CXformFactoryTest_H

// EOF
