//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPredicateUtilsTest.h
//
//	@doc:
//		Test for predicate utilities
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPredicateUtilsTest_H
#define SPQOPT_CPredicateUtilsTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPredicateUtilsTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CPredicateUtilsTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Conjunctions();
	static SPQOS_RESULT EresUnittest_Disjunctions();
	static SPQOS_RESULT EresUnittest_PlainEqualities();
	static SPQOS_RESULT EresUnittest_Implication();


};	// class CPredicateUtilsTest
}  // namespace spqopt

#endif	// !SPQOPT_CPredicateUtilsTest_H

// EOF
