//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDXLUtilsTest.h
//
//	@doc:
//		Tests the DXL utility functions
//---------------------------------------------------------------------------


#ifndef SPQOPT_CDXLUtilsTest_H
#define SPQOPT_CDXLUtilsTest_H

#include "spqos/base.h"

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDXLUtilsTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CDXLUtilsTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_SerializeQuery();
	static SPQOS_RESULT EresUnittest_SerializePlan();
	static SPQOS_RESULT EresUnittest_Encoding();

};	// class CDXLUtilsTest
}  // namespace spqdxl

#endif	// !SPQOPT_CDXLUtilsTest_H

// EOF
