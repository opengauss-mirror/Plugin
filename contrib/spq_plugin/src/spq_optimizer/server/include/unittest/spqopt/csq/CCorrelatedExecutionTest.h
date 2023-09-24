//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCorrelatedExecutionTest.h
//
//	@doc:
//		Test for correlated subqueries
//---------------------------------------------------------------------------
#ifndef SPQOPT_CCorrelatedExecutionTest_H
#define SPQOPT_CCorrelatedExecutionTest_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/CPrintPrefix.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/COperator.h"

// forward declarations
namespace spqdxl
{
typedef CDynamicPtrArray<INT, CleanupDelete> IntPtrArray;
}

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CCorrelatedExecutionTest
//
//	@doc:
//		Tests for converting Apply expressions into NL expressions
//
//---------------------------------------------------------------------------
class CCorrelatedExecutionTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_RunAllPositiveTests();

};	// class CCorrelatedExecutionTest
}  // namespace spqopt


#endif	// !SPQOPT_CCorrelatedExecutionTest_H

// EOF
