//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2022 VMware Inc.
//
//	@filename:
//		CGroupTest.h
//
//	@doc:
//		Test for CGroup
//---------------------------------------------------------------------------
#ifndef SPQOPT_CGroupTest_H
#define SPQOPT_CGroupTest_H


#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"

#include "spqopt/operators/CExpression.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CGroupTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CGroupTest
{
public:
	// main driver
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_FResetStatsOnCGroupWithDuplicateGroup();

};	// class CGroupTest
}  // namespace spqopt

#endif	// !SPQOPT_CGroupTest_H


// EOF
