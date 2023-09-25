//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CSubqueryHandlerTest.h
//
//	@doc:
//		Tests for converting subquery expressions into Apply trees
//---------------------------------------------------------------------------
#ifndef SPQOPT_CSubqueryHandlerTest_H
#define SPQOPT_CSubqueryHandlerTest_H

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
//		CSubqueryHandlerTest
//
//	@doc:
//		Tests for converting subquery expressions into Apply trees
//
//---------------------------------------------------------------------------
class CSubqueryHandlerTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulSubqueryHandlerMinidumpTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Subquery2Apply();
	static SPQOS_RESULT EresUnittest_Subquery2OrTree();
	static SPQOS_RESULT EresUnittest_Subquery2AndTree();
	static SPQOS_RESULT EresUnittest_SubqueryWithDisjunction();
	static SPQOS_RESULT EresUnittest_RunMinidumpTests();

};	// class CSubqueryHandlerTest
}  // namespace spqopt


#endif	// !SPQOPT_CSubqueryHandlerTest_H

// EOF
