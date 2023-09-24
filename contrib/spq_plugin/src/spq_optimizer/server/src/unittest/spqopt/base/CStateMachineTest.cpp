//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CStateMachineTest.cpp
//
//	@doc:
//      Test of state machine implmentation;
//
//		Implements functionality of CTestMachine: transitions and names
//		of states and events;
//---------------------------------------------------------------------------

#include "unittest/spqopt/base/CStateMachineTest.h"

#include "spqos/common/CRandom.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/test/CUnittest.h"
#include "spqos/types.h"

#include "spqopt/base/CStateMachine.h"

using namespace spqopt;

// State transition diagram for test machine:
// From every state, event eeX transitions into state esX, e.g. eeOne transitions always to esOne and so on
// All transitions are considered valid.
//
static const CStateMachineTest::EEvents
	rgeevTransitions[CStateMachineTest::esSentinel]
					[CStateMachineTest::esSentinel] = {
						{CStateMachineTest::eeOne, CStateMachineTest::eeTwo,
						 CStateMachineTest::eeSentinel},
						{CStateMachineTest::eeOne, CStateMachineTest::eeTwo,
						 CStateMachineTest::eeSentinel},
						{CStateMachineTest::eeOne, CStateMachineTest::eeTwo,
						 CStateMachineTest::eeSentinel}};

#ifdef SPQOS_DEBUG
// names for events
static const WCHAR
	rgwszEvents[CStateMachineTest::eeSentinel][SPQOPT_FSM_NAME_LENGTH] = {
		SPQOS_WSZ_LIT("event_one"), SPQOS_WSZ_LIT("event_two"),
		SPQOS_WSZ_LIT("event_three")};

// names for states
static const WCHAR
	rgwszStates[CStateMachineTest::esSentinel][SPQOPT_FSM_NAME_LENGTH] = {
		SPQOS_WSZ_LIT("state_one"), SPQOS_WSZ_LIT("state_two"),
		SPQOS_WSZ_LIT("state_three")};
#endif	// SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CStateMachineTest::CTestMachine::CTestMachine
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CStateMachineTest::CTestMachine::CTestMachine()
{
	m_sm.Init(rgeevTransitions
#ifdef SPQOS_DEBUG
			  ,
			  rgwszStates, rgwszEvents
#endif	// SPQOS_DEBUG
	);
}


//---------------------------------------------------------------------------
//	@function:
//		CStateMachineTest::EresUnittest
//
//	@doc:
//		Unittest for state machine
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CStateMachineTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CStateMachineTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CStateMachineTest::EresUnittest_Basics
//
//	@doc:
//		Tests for random mix of transitions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CStateMachineTest::EresUnittest_Basics()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CTestMachine *ptm = SPQOS_NEW(mp) CTestMachine;
	CRandom rand;
	EEvents rgev[] = {eeOne, eeTwo, eeThree};
#ifdef SPQOS_DEBUG
	// states are only used for assertions
	EStates rgst[] = {esOne, esTwo, esThree};
#endif	// SPQOS_DEBUG

	EStates es;
	// go into state One to start with
	(void) ptm->Psm()->FTransition(eeOne, es);
	SPQOS_ASSERT(esOne == es);

	for (ULONG i = 0; i < 100; i++)
	{
		// choose random event
		ULONG ul = rand.Next() % SPQOS_ARRAY_SIZE(rgev);

#ifdef SPQOS_DEBUG
		BOOL fCheck =
#endif	// SPQOS_DEBUG
			ptm->Psm()->FTransition(rgev[ul], es);

		SPQOS_ASSERT_IFF(eeThree != rgev[ul], fCheck);
		SPQOS_ASSERT_IFF(eeThree != rgev[ul], rgst[ul] == es);
	}

#ifdef SPQOS_DEBUG
	CWStringDynamic str(mp);
	COstreamString oss(&str);
	(void) ptm->Psm()->OsHistory(oss);

	// dumping state graph
	(void) ptm->Psm()->OsDiagramToGraphviz(mp, oss,
										   SPQOS_WSZ_LIT("CTestMachine"));

	SPQOS_TRACE(str.GetBuffer());

	SPQOS_ASSERT(!ptm->Psm()->FReachable(mp));
#endif	// SPQOS_DEBUG
	SPQOS_DELETE(ptm);

	return SPQOS_OK;
}


// EOF
