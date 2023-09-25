//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		COptimizationJobsTest.cpp
//
//	@doc:
//		Test for optimization jobs
//---------------------------------------------------------------------------
#include "unittest/spqopt/search/COptimizationJobsTest.h"

#include "spqos/error/CAutoTrace.h"

#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupExploration.h"
#include "spqopt/search/CJobGroupExpressionExploration.h"
#include "spqopt/search/CJobGroupExpressionImplementation.h"
#include "spqopt/search/CJobGroupExpressionOptimization.h"
#include "spqopt/search/CJobGroupImplementation.h"
#include "spqopt/search/CJobGroupOptimization.h"
#include "spqopt/search/CJobTransformation.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"
#include "spqopt/xforms/CXformFactory.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


//---------------------------------------------------------------------------
//	@function:
//		COptimizationJobsTest::EresUnittest
//
//	@doc:
//		Unittest for optimization jobs
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COptimizationJobsTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(COptimizationJobsTest::EresUnittest_StateMachine),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizationJobsTest::EresUnittest_StateMachine
//
//	@doc:
//		Test of optimization jobs stat machine
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COptimizationJobsTest::EresUnittest_StateMachine()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	{
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));
		CEngine eng(mp);

		// generate  join expression
		CExpression *pexpr =
			CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(mp);

		// generate query context
		CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexpr);

		// Initialize engine
		eng.Init(pqc, NULL /*search_stage_array*/);

		CGroup *pgroup = eng.PgroupRoot();
		pqc->Prpp()->AddRef();
		COptimizationContext *poc = SPQOS_NEW(mp) COptimizationContext(
			mp, pgroup, pqc->Prpp(),
			SPQOS_NEW(mp) CReqdPropRelational(SPQOS_NEW(mp) CColRefSet(mp)),
			SPQOS_NEW(mp) IStatisticsArray(mp),
			0  // ulSearchStageIndex
		);

		// optimize query
		CJobFactory jf(mp, 1000 /*ulJobs*/);
		CScheduler sched(mp, 1000 /*ulJobs*/);
		CSchedulerContext sc;
		sc.Init(mp, &jf, &sched, &eng);
		CJob *pj = jf.PjCreate(CJob::EjtGroupOptimization);
		CJobGroupOptimization *pjgo = CJobGroupOptimization::PjConvert(pj);
		pjgo->Init(pgroup, NULL /*pgexprOrigin*/, poc);
		sched.Add(pjgo, NULL /*pjParent*/);
		CScheduler::Run(&sc);

#ifdef SPQOS_DEBUG
		{
			CAutoTrace at(mp);
			at.Os() << std::endl << "GROUP OPTIMIZATION:" << std::endl;
			(void) pjgo->OsPrint(at.Os());

			// dumping state graph
			at.Os() << std::endl;
			(void) pjgo->OsDiagramToGraphviz(
				mp, at.Os(), SPQOS_WSZ_LIT("GroupOptimizationJob"));

			CJobGroupOptimization::EState *pestate = NULL;
			ULONG size = 0;
			pjgo->Unreachable(mp, &pestate, &size);
			SPQOS_ASSERT(size == 1 &&
						pestate[0] == CJobGroupOptimization::estInitialized);

			SPQOS_DELETE_ARRAY(pestate);
		}

		CGroupExpression *pgexprLogical = NULL;
		CGroupExpression *pgexprPhysical = NULL;
		{
			CGroupProxy spq(pgroup);
			pgexprLogical = spq.PgexprNextLogical(NULL /*pgexpr*/);
			SPQOS_ASSERT(NULL != pgexprLogical);

			pgexprPhysical = spq.PgexprSkipLogical(NULL /*pgexpr*/);
			SPQOS_ASSERT(NULL != pgexprPhysical);
		}

		{
			CAutoTrace at(mp);
			CJobGroupImplementation jgi;
			jgi.Init(pgroup);
			at.Os() << std::endl << "GROUP IMPLEMENTATION:" << std::endl;
			(void) jgi.OsPrint(at.Os());

			// dumping state graph
			at.Os() << std::endl;
			(void) jgi.OsDiagramToGraphviz(
				mp, at.Os(), SPQOS_WSZ_LIT("GroupImplementationJob"));

			CJobGroupImplementation::EState *pestate = NULL;
			ULONG size = 0;
			jgi.Unreachable(mp, &pestate, &size);
			SPQOS_ASSERT(size == 1 &&
						pestate[0] == CJobGroupImplementation::estInitialized);

			SPQOS_DELETE_ARRAY(pestate);
		}

		{
			CAutoTrace at(mp);
			CJobGroupExploration jge;
			jge.Init(pgroup);
			at.Os() << std::endl << "GROUP EXPLORATION:" << std::endl;
			(void) jge.OsPrint(at.Os());

			// dumping state graph
			at.Os() << std::endl;
			(void) jge.OsDiagramToGraphviz(mp, at.Os(),
										   SPQOS_WSZ_LIT("GroupExplorationJob"));

			CJobGroupExploration::EState *pestate = NULL;
			ULONG size = 0;
			jge.Unreachable(mp, &pestate, &size);
			SPQOS_ASSERT(size == 1 &&
						pestate[0] == CJobGroupExploration::estInitialized);

			SPQOS_DELETE_ARRAY(pestate);
		}

		{
			CAutoTrace at(mp);
			CJobGroupExpressionOptimization jgeo;
			jgeo.Init(pgexprPhysical, poc, 0 /*ulOptReq*/);
			at.Os() << std::endl
					<< "GROUP EXPRESSION OPTIMIZATION:" << std::endl;
			(void) jgeo.OsPrint(at.Os());

			// dumping state graph
			at.Os() << std::endl;
			(void) jgeo.OsDiagramToGraphviz(
				mp, at.Os(), SPQOS_WSZ_LIT("GroupExpressionOptimizationJob"));

			CJobGroupExpressionOptimization::EState *pestate = NULL;
			ULONG size = 0;
			jgeo.Unreachable(mp, &pestate, &size);
			SPQOS_ASSERT(size == 1 &&
						pestate[0] ==
							CJobGroupExpressionOptimization::estInitialized);

			SPQOS_DELETE_ARRAY(pestate);
		}

		{
			CAutoTrace at(mp);
			CJobGroupExpressionImplementation jgei;
			jgei.Init(pgexprLogical);
			at.Os() << std::endl
					<< "GROUP EXPRESSION IMPLEMENTATION:" << std::endl;
			(void) jgei.OsPrint(at.Os());

			// dumping state graph
			at.Os() << std::endl;
			(void) jgei.OsDiagramToGraphviz(
				mp, at.Os(), SPQOS_WSZ_LIT("GroupExpressionImplementationJob"));

			CJobGroupExpressionImplementation::EState *pestate = NULL;
			ULONG size = 0;
			jgei.Unreachable(mp, &pestate, &size);
			SPQOS_ASSERT(size == 1 &&
						pestate[0] ==
							CJobGroupExpressionImplementation::estInitialized);

			SPQOS_DELETE_ARRAY(pestate);
		}

		{
			CAutoTrace at(mp);
			CJobGroupExpressionExploration jgee;
			jgee.Init(pgexprLogical);
			at.Os() << std::endl
					<< "GROUP EXPRESSION EXPLORATION:" << std::endl;
			(void) jgee.OsPrint(at.Os());

			// dumping state graph
			at.Os() << std::endl;
			(void) jgee.OsDiagramToGraphviz(
				mp, at.Os(), SPQOS_WSZ_LIT("GroupExpressionExplorationJob"));

			CJobGroupExpressionExploration::EState *pestate = NULL;
			ULONG size = 0;
			jgee.Unreachable(mp, &pestate, &size);
			SPQOS_ASSERT(size == 1 &&
						pestate[0] ==
							CJobGroupExpressionExploration::estInitialized);

			SPQOS_DELETE_ARRAY(pestate);
		}

		{
			CAutoTrace at(mp);
			CXformSet *xform_set =
				CLogical::PopConvert(pgexprLogical->Pop())->PxfsCandidates(mp);

			CXformSetIter xsi(*(xform_set));
			while (xsi.Advance())
			{
				CXform *pxform = CXformFactory::Pxff()->Pxf(xsi.TBit());
				CJobTransformation jt;
				jt.Init(pgexprLogical, pxform);
				at.Os() << std::endl
						<< "GROUP EXPRESSION TRANSFORMATION:" << std::endl;
				(void) jt.OsPrint(at.Os());

				// dumping state graph
				at.Os() << std::endl;
				(void) jt.OsDiagramToGraphviz(
					mp, at.Os(), SPQOS_WSZ_LIT("TransformationJob"));

				CJobTransformation::EState *pestate = NULL;
				ULONG size = 0;
				jt.Unreachable(mp, &pestate, &size);
				SPQOS_ASSERT(size == 1 &&
							pestate[0] == CJobTransformation::estInitialized);

				SPQOS_DELETE_ARRAY(pestate);
			}

			xform_set->Release();
		}
#endif	// SPQOS_DEBUG

		pexpr->Release();
		poc->Release();
		SPQOS_DELETE(pqc);
	}

	return SPQOS_OK;
}


// EOF
