//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTableDescriptorTest.h
//
//	@doc:
//      Test for CTableDescriptor
//---------------------------------------------------------------------------
#ifndef SPQOPT_CTableDescriptorTest_H
#define SPQOPT_CTableDescriptorTest_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/metadata/CColumnDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CTableDescriptorTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CTableDescriptorTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
};	// class CTableDescriptorTest
}  // namespace spqopt

#endif	// !SPQOPT_CTableDescriptorTest_H

// EOF
