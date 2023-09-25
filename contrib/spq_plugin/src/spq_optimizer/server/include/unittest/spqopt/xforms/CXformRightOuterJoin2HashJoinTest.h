//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformRightOuterJoin2HashJoinTest.h
//
//	@doc:
//		Test for right outer join to hash join transform
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformRightOuterJoin2HashJoinTest_H
#define SPQOPT_CXformRightOuterJoin2HashJoinTest_H

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
//		CXformRightOuterJoin2HashJoinTest
//
//	@doc:
//		Tests for logical right outer transform
//
//---------------------------------------------------------------------------
class CXformRightOuterJoin2HashJoinTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Transform();

};	// class CXformRightOuterJoin2HashJoinTest
}  // namespace spqopt


#endif	// !SPQOPT_CXformRightOuterJoin2HashJoinTest_H

// EOF
