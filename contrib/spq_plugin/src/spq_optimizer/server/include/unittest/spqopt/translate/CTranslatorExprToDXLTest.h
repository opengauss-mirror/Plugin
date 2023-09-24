//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CTranslatorExprToDXLTest.h
//
//	@doc:
//		Test for translating CExpression to DXL
//---------------------------------------------------------------------------
#ifndef SPQOPT_CTranslatorExprToDXLTest_H
#define SPQOPT_CTranslatorExprToDXLTest_H

#include "spqos/base.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CTranslatorExprToDXLTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CTranslatorExprToDXLTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_RunTests();
	static SPQOS_RESULT EresUnittest_RunMinidumpTests();

};	// class CTranslatorExprToDXLTest
}  // namespace spqopt

#endif	// !SPQOPT_CTranslatorExprToDXLTest_H

// EOF
