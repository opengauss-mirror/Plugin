//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColumnDescriptorTest.h
//
//	@doc:
//		Test for CColumnDescriptor
//---------------------------------------------------------------------------
#ifndef SPQOPT_CColumnDescriptorTest_H
#define SPQOPT_CColumnDescriptorTest_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/metadata/CName.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CColumnDescriptorTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CColumnDescriptorTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CColumnDescriptorTest
}  // namespace spqopt

#endif	// !SPQOPT_CColumnDescriptorTest_H

// EOF
