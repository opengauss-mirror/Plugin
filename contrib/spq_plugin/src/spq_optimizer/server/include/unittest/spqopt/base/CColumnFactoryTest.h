//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008, 2009 Greenplum, Inc.
//
//	@filename:
//		CColumnFactoryTest.h
//
//	@doc:
//		Test for CColumnFactory
//---------------------------------------------------------------------------
#ifndef SPQOPT_CColumnFactoryTest_H
#define SPQOPT_CColumnFactoryTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CColumnFactoryTest
//
//	@doc:
//		unittests
//
//---------------------------------------------------------------------------
class CColumnFactoryTest
{
public:
	// actual unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
};
}  // namespace spqopt

#endif	// !SPQOPT_CColumnFactoryTest_H

// EOF
