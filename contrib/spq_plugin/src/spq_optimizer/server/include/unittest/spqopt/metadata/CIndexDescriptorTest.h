//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CIndexDescriptorTest.h
//
//	@doc:
//      Test for CTableDescriptor
//---------------------------------------------------------------------------
#ifndef SPQOPT_CIndexDescriptorTest_H
#define SPQOPT_CIndexDescriptorTest_H

#include "spqos/base.h"

#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CIndexDescriptorTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CIndexDescriptorTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
};	// class CIndexDescriptorTest
}  // namespace spqopt

#endif	// !SPQOPT_CIndexDescriptorTest_H

// EOF
