//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDecorrelatorTest.h
//
//	@doc:
//		Tests for decorrelating expression trees
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDecorrelatorTest_H
#define SPQOPT_CDecorrelatorTest_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/CPrintPrefix.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/COperator.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CDecorrelatorTest
//
//	@doc:
//		Tests for decorrelating expressions
//
//---------------------------------------------------------------------------
class CDecorrelatorTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Decorrelate();
	static SPQOS_RESULT EresUnittest_DecorrelateSelect();
	static SPQOS_RESULT EresUnittest_DecorrelateGbAgg();

};	// class CDecorrelatorTest
}  // namespace spqopt


#endif	// !SPQOPT_CDecorrelatorTest_H

// EOF
