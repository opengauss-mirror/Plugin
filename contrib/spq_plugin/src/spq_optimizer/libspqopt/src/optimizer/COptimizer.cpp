//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		COptimizer.cpp
//
//	@doc:
//		Optimizer class implementation
//---------------------------------------------------------------------------

#include "spqopt/optimizer/COptimizer.h"

#include <fstream>

#include "spqos/common/CBitSet.h"
#include "spqos/common/CDebugCounter.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/io/CFileDescriptor.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/cost/ICostModel.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/minidump/CMiniDumperDXL.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/minidump/CSerializableMDAccessor.h"
#include "spqopt/minidump/CSerializableOptimizerConfig.h"
#include "spqopt/minidump/CSerializablePlan.h"
#include "spqopt/minidump/CSerializableQuery.h"
#include "spqopt/minidump/CSerializableStackTrace.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "naucrates/base/CDatumGenericSPQDB.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/md/IMDProvider.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqos;
using namespace spqdxl;
using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		COptimizer::PrintQuery
//
//	@doc:
//		Helper function to print query expression
//
//---------------------------------------------------------------------------
void
COptimizer::PrintQuery(CMemoryPool *mp, CExpression *pexprTranslated,
					   CQueryContext *pqc)
{
	CAutoTrace at(mp);
	at.Os() << std::endl
			<< "Algebrized query: " << std::endl
			<< *pexprTranslated;

	CExpressionArray *pdrspqexpr =
		COptCtxt::PoctxtFromTLS()->Pcteinfo()->PdrgPexpr(mp);
	const ULONG ulCTEs = pdrspqexpr->Size();
	if (0 < ulCTEs)
	{
		at.Os() << std::endl << "Common Table Expressions: ";
		for (ULONG ul = 0; ul < ulCTEs; ul++)
		{
			at.Os() << std::endl << *(*pdrspqexpr)[ul];
		}
	}
	pdrspqexpr->Release();

	CExpression *pexprPreprocessed = pqc->Pexpr();
	(void) pexprPreprocessed->PdpDerive();
	at.Os() << std::endl
			<< "Algebrized preprocessed query: " << std::endl
			<< *pexprPreprocessed;
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizer::PrintPlan
//
//	@doc:
//		Helper function to print query plan
//
//---------------------------------------------------------------------------
void
COptimizer::PrintPlan(CMemoryPool *mp, CExpression *pexprPlan)
{
	CAutoTrace at(mp);
	at.Os() << std::endl << "Physical plan: " << std::endl << *pexprPlan;
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizer::DumpSamples
//
//	@doc:
//		Helper function to dump plan samples
//
//---------------------------------------------------------------------------
void
COptimizer::DumpSamples(CMemoryPool *mp, CEnumeratorConfig *pec,
						ULONG ulSessionId, ULONG ulCmdId)
{
	SPQOS_ASSERT(NULL != pec);

	CWStringDynamic *str =
		CDXLUtils::SerializeSamplePlans(mp, pec, true /*indentation*/);
	pec->DumpSamples(str, ulSessionId, ulCmdId);
	SPQOS_DELETE(str);
	SPQOS_CHECK_ABORT;

	pec->FitCostDistribution();
	str = CDXLUtils::SerializeCostDistr(mp, pec, true /*indentation*/);
	pec->DumpCostDistr(str, ulSessionId, ulCmdId);
	SPQOS_DELETE(str);
}

//---------------------------------------------------------------------------
//	@function:
//		COptimizer::DumpQueryOrPlan
//
//	@doc:
//		Print query tree or plan tree
//
//---------------------------------------------------------------------------
void
COptimizer::PrintQueryOrPlan(CMemoryPool *mp, CExpression *pexpr,
							 CQueryContext *pqc)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (NULL != pqc)
	{
		if (SPQOS_FTRACE(EopttracePrintQuery))
		{
			PrintQuery(mp, pexpr, pqc);
		}
	}
	else
	{
#ifdef SPQOS_DEBUG_COUNTERS
		// code for debug counters, extract name of the next query, if applicable
		// and prepare for logging the existing query and initializing state for
		// the next one
		std::string query_name = "";

		// try to recognize statements of the type select 'query name: <some name>' and
		// extract the name as the name for the next query
		if (NULL != pexpr &&
			COperator::EopPhysicalComputeScalar == pexpr->Pop()->Eopid() &&
			COperator::EopPhysicalConstTableGet ==
				(*pexpr)[0]->Pop()->Eopid() &&
			COperator::EopScalarProjectList == (*pexpr)[1]->Pop()->Eopid() &&
			1 == (*pexpr)[1]->Arity())
		{
			// the query is of the form select <const>, now look at the <const>
			COperator *pop = (*((*((*pexpr)[1]))[0]))[0]->Pop();

			if (COperator::EopScalarConst == pop->Eopid())
			{
				CScalarConst *popScalarConst = CScalarConst::PopConvert(pop);
				CDatumGenericSPQDB *datum = dynamic_cast<CDatumGenericSPQDB *>(
					popScalarConst->GetDatum());

				if (NULL != datum)
				{
					const char *select_element_bytes =
						(const char *) datum->GetByteArrayValue();
					ULONG select_element_len =
						clib::Strlen(select_element_bytes);
					const char *query_name_prefix = "query name: ";
					ULONG query_name_prefix_len =
						clib::Strlen(query_name_prefix);

					if (0 == clib::Strncmp((const char *) select_element_bytes,
										   query_name_prefix,
										   query_name_prefix_len))
					{
						// the constant in the select starts with "query_name: "
						for (ULONG i = query_name_prefix_len;
							 i < select_element_len; i++)
						{
							if (select_element_bytes[i] > 0)
							{
								// the query name should contain ASCII characters,
								// we skip all other characters
								query_name.append(1, select_element_bytes[i]);
							}
						}
					}
				}
			}
		}
		CDebugCounter::NextQry(query_name.c_str());
#endif

		if (SPQOS_FTRACE(EopttracePrintPlan))
		{
			PrintPlan(mp, pexpr);
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizer::PdxlnOptimize
//
//	@doc:
//		Optimize given query
//		the function is oblivious of trace flags setting/resetting which
//		must happen at the caller side if needed
//
//---------------------------------------------------------------------------
CDXLNode *
COptimizer::PdxlnOptimize(
	CMemoryPool *mp, CMDAccessor *md_accessor, const CDXLNode *query,
	const CDXLNodeArray *query_output_dxlnode_array,
	const CDXLNodeArray *cte_producers, IConstExprEvaluator *pceeval,
	ULONG ulHosts,	// actual number of data nodes in the system
	ULONG ulSessionId, ULONG ulCmdId, CSearchStageArray *search_stage_array,
	COptimizerConfig *optimizer_config,
	const CHAR *szMinidumpFileName	// name of minidump file to be created
)
{
	SPQOS_ASSERT(NULL != md_accessor);
	SPQOS_ASSERT(NULL != query);
	SPQOS_ASSERT(NULL != query_output_dxlnode_array);
	SPQOS_ASSERT(NULL != optimizer_config);

	BOOL fMinidump = SPQOS_FTRACE(EopttraceMinidump);

	// If minidump was requested, open the minidump file and initialize
	// minidumper. (We create the minidumper object even if we're not
	// dumping, but without the Init-call, it will stay inactive.)
	CMiniDumperDXL mdmp(mp);
	CAutoP<std::wofstream> wosMinidump;
	CAutoP<COstreamBasic> osMinidump;
	if (fMinidump)
	{
		CHAR file_name[SPQOS_FILE_NAME_BUF_SIZE];

		CMinidumperUtils::GenerateMinidumpFileName(
			file_name, SPQOS_FILE_NAME_BUF_SIZE, ulSessionId, ulCmdId,
			szMinidumpFileName);

		// Note: std::wofstream won't throw an error on failure. The stream is merely marked as
		// failed. We could check the state, and avoid the overhead of serializing the
		// minidump if it failed, but it's hardly worth optimizing for an error case.
		wosMinidump = SPQOS_NEW(mp) std::wofstream(file_name);
		osMinidump = SPQOS_NEW(mp) COstreamBasic(wosMinidump.Value());

		mdmp.Init(osMinidump.Value());
	}
	CDXLNode *pdxlnPlan = NULL;
	CErrorHandlerStandard errhdl;
	SPQOS_TRY_HDL(&errhdl)
	{
		CSerializableStackTrace serStack;
		CSerializableOptimizerConfig serOptConfig(mp, optimizer_config);
		CSerializableMDAccessor serMDA(md_accessor);
		CSerializableQuery serQuery(mp, query, query_output_dxlnode_array,
									cte_producers);

		{
			optimizer_config->AddRef();
			if (NULL != pceeval)
			{
				pceeval->AddRef();
			}

			// install opt context in TLS
			CAutoOptCtxt aoc(mp, md_accessor, pceeval, optimizer_config);

			// translate DXL Tree -> Expr Tree
			CTranslatorDXLToExpr dxltr(mp, md_accessor);
			CExpression *pexprTranslated = dxltr.PexprTranslateQuery(
				query, query_output_dxlnode_array, cte_producers);
			SPQOS_CHECK_ABORT;
			spqdxl::ULongPtrArray *pdrspqul = dxltr.PdrspqulOutputColRefs();
			spqmd::CMDNameArray *pdrspqmdname = dxltr.Pdrspqmdname();

			CQueryContext *pqc =
				CQueryContext::PqcGenerate(mp, pexprTranslated, pdrspqul,
										   pdrspqmdname, true /*fDeriveStats*/);
			SPQOS_CHECK_ABORT;

			PrintQueryOrPlan(mp, pexprTranslated, pqc);

			CWStringDynamic strTrace(mp);
			COstreamString oss(&strTrace);

			// if the number of inlinable CTEs is greater than the cutoff, then
			// disable inlining for this query
			if (!SPQOS_FTRACE(EopttraceEnableCTEInlining) ||
				CUtils::UlInlinableCTEs(pexprTranslated) >
					optimizer_config->GetCteConf()->UlCTEInliningCutoff())
			{
				COptCtxt::PoctxtFromTLS()->Pcteinfo()->DisableInlining();
			}

			SPQOS_CHECK_ABORT;
			// optimize logical expression tree into physical expression tree.
			CExpression *pexprPlan = PexprOptimize(mp, pqc, search_stage_array);
			SPQOS_CHECK_ABORT;

			// translate plan into DXL
			pdxlnPlan = CreateDXLNode(mp, md_accessor, pexprPlan,
									  pqc->PdrgPcr(), pdrspqmdname, ulHosts);
			SPQOS_CHECK_ABORT;

			if (fMinidump)
			{
				CSerializablePlan serPlan(
					mp, pdxlnPlan,
					optimizer_config->GetEnumeratorCfg()->GetPlanId(),
					optimizer_config->GetEnumeratorCfg()->GetPlanSpaceSize());
				CMinidumperUtils::Finalize(&mdmp, true /* fSerializeErrCtxt*/);
				SPQOS_CHECK_ABORT;
			}

			if (SPQOS_FTRACE(EopttraceSamplePlans))
			{
				DumpSamples(mp, optimizer_config->GetEnumeratorCfg(),
							ulSessionId, ulCmdId);
				SPQOS_CHECK_ABORT;
			}

			// cleanup
			pexprTranslated->Release();
			pexprPlan->Release();
			SPQOS_DELETE(pqc);
		}
	}
	SPQOS_CATCH_EX(ex)
	{
		if (fMinidump)
		{
			CMinidumperUtils::Finalize(&mdmp, false /* fSerializeErrCtxt*/);
			HandleExceptionAfterFinalizingMinidump(ex);
		}

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	return pdxlnPlan;
}

//---------------------------------------------------------------------------
//	@function:
//		COptimizer::HandleExceptionAfterFinalizingMinidump
//
//	@doc:
//		Handle exception after finalizing minidump
//
//---------------------------------------------------------------------------
void
COptimizer::HandleExceptionAfterFinalizingMinidump(CException &ex)
{
	if (NULL != ITask::Self() && !ITask::Self()->GetErrCtxt()->IsPending())
	{
		// if error context has no pending exception, then minidump creation
		// might have reset the error,
		// in this case we need to raise the original exception
		SPQOS_RAISE(
			ex.Major(), ex.Minor(),
			SPQOS_WSZ_LIT("re-raising exception after finalizing minidump"));
	}

	// otherwise error is still pending, re-throw original exception
	SPQOS_RETHROW(ex);
}

// This function provides an entry point to check for a plan with CTE,
// if both CTEProducer and CTEConsumer are executed on the same locality.
// If it is not the case, the plan is bogus and cannot be executed
// by the executor and an exception is raised.
//
// To be able to enter the recursive logic, the execution locality of root
// is determined before the recursive call.
void
COptimizer::CheckCTEConsistency(CMemoryPool *mp, CExpression *pexpr)
{
	UlongToUlongMap *phmulul = SPQOS_NEW(mp) UlongToUlongMap(mp);
	CDrvdPropPlan *pdpplanChild = CDrvdPropPlan::Pdpplan(pexpr->PdpDerive());
	CDistributionSpec *pdsChild = pdpplanChild->Pds();

	CUtils::EExecLocalityType eelt = CUtils::ExecLocalityType(pdsChild);
	CUtils::ValidateCTEProducerConsumerLocality(mp, pexpr, eelt, phmulul);
	phmulul->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		COptimizer::PexprOptimize
//
//	@doc:
//		Optimize query in given query context
//
//---------------------------------------------------------------------------
CExpression *
COptimizer::PexprOptimize(CMemoryPool *mp, CQueryContext *pqc,
						  CSearchStageArray *search_stage_array)
{
	CEngine eng(mp);
	eng.Init(pqc, search_stage_array);
	eng.Optimize();

	SPQOS_CHECK_ABORT;

	CExpression *pexprPlan = eng.PexprExtractPlan();
	(void) pexprPlan->PrppCompute(mp, pqc->Prpp());

	CheckCTEConsistency(mp, pexprPlan);

	PrintQueryOrPlan(mp, pexprPlan);

	// you can also print alternative plans by calling
	// p eng.DbgPrintExpr(<group #>, <opt context #>)
	// in the debugger, giving parameters based on the memo printout

	SPQOS_CHECK_ABORT;

	return pexprPlan;
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizer::CreateDXLNode
//
//	@doc:
//		Translate an optimizer expression into a DXL tree
//
//---------------------------------------------------------------------------
CDXLNode *
COptimizer::CreateDXLNode(CMemoryPool *mp, CMDAccessor *md_accessor,
						  CExpression *pexpr, CColRefArray *colref_array,
						  CMDNameArray *pdrspqmdname, ULONG ulHosts)
{
	SPQOS_ASSERT(0 < ulHosts);
	IntPtrArray *pdrspqiHosts = SPQOS_NEW(mp) IntPtrArray(mp);

	for (ULONG ul = 0; ul < ulHosts; ul++)
	{
		pdrspqiHosts->Append(SPQOS_NEW(mp) INT(ul));
	}

	CTranslatorExprToDXL ptrexprtodxl(mp, md_accessor, pdrspqiHosts);
	CDXLNode *pdxlnPlan =
		ptrexprtodxl.PdxlnTranslate(pexpr, colref_array, pdrspqmdname);

	return pdxlnPlan;
}

// EOF
