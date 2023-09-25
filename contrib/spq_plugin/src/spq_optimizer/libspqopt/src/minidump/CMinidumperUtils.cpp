//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMinidumperUtils.cpp
//
//	@doc:
//		Implementation of minidump utility functions
//---------------------------------------------------------------------------

#include "spqopt/minidump/CMinidumperUtils.h"

#include <fstream>

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/common/CAutoTimer.h"
#include "spqos/common/CBitSet.h"
#include "spqos/common/syslibwrapper.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/io/COstreamFile.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/task/CTask.h"
#include "spqos/task/CWorker.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/cost/ICostModel.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/minidump/CMetadataAccessorFactory.h"
#include "spqopt/minidump/CMiniDumperDXL.h"
#include "spqopt/optimizer/COptimizer.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/parser/CParseHandlerDXL.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqos;
using namespace spqopt;
using namespace spqdxl;
using namespace std;

//---------------------------------------------------------------------------
//	@function:
//		CMinidumperUtils::PdxlmdLoad
//
//	@doc:
//		Load minidump file
//
//---------------------------------------------------------------------------
CDXLMinidump *
CMinidumperUtils::PdxlmdLoad(CMemoryPool *mp, const CHAR *file_name)
{
	CAutoTraceFlag atf1(EtraceSimulateAbort, false);
	CAutoTraceFlag atf2(EtraceSimulateOOM, false);

	{
		CAutoTrace at(mp);
		at.Os() << "parsing DXL File " << file_name;
	}

	CParseHandlerDXL *parse_handler_dxl = CDXLUtils::GetParseHandlerForDXLFile(
		mp, file_name, NULL /*xsd_file_path*/);

	CBitSet *pbs = parse_handler_dxl->Pbs();
	COptimizerConfig *optimizer_config =
		parse_handler_dxl->GetOptimizerConfig();
	CDXLNode *query = parse_handler_dxl->GetQueryDXLRoot();
	CDXLNodeArray *query_output_dxlnode_array =
		parse_handler_dxl->GetOutputColumnsDXLArray();
	CDXLNodeArray *cte_producers = parse_handler_dxl->GetCTEProducerDXLArray();
	IMDCacheObjectArray *mdcache_obj_array =
		parse_handler_dxl->GetMdIdCachedObjArray();
	CSystemIdArray *pdrspqsysid = parse_handler_dxl->GetSysidPtrArray();
	CDXLNode *pdxlnPlan = parse_handler_dxl->PdxlnPlan();
	ULLONG plan_id = parse_handler_dxl->GetPlanId();
	ULLONG plan_space_size = parse_handler_dxl->GetPlanSpaceSize();

	if (NULL != pbs)
	{
		pbs->AddRef();
	}

	if (NULL != optimizer_config)
	{
		optimizer_config->AddRef();
	}

	if (NULL != query)
	{
		query->AddRef();
	}

	if (NULL != query_output_dxlnode_array)
	{
		query_output_dxlnode_array->AddRef();
	}

	if (NULL != cte_producers)
	{
		cte_producers->AddRef();
	}

	if (NULL != mdcache_obj_array)
	{
		mdcache_obj_array->AddRef();
	}

	if (NULL != pdrspqsysid)
	{
		pdrspqsysid->AddRef();
	}

	if (NULL != pdxlnPlan)
	{
		pdxlnPlan->AddRef();
	}

	// cleanup
	SPQOS_DELETE(parse_handler_dxl);

	return SPQOS_NEW(mp) CDXLMinidump(
		pbs, optimizer_config, query, query_output_dxlnode_array, cte_producers,
		pdxlnPlan, mdcache_obj_array, pdrspqsysid, plan_id, plan_space_size);
}


//---------------------------------------------------------------------------
//	@function:
//		CMinidumperUtils::GenerateMinidumpFileName
//
//	@doc:
//		Generate a timestamp-based minidump filename in the provided buffer.
//
//---------------------------------------------------------------------------
void
CMinidumperUtils::GenerateMinidumpFileName(
	CHAR *buf, ULONG length, ULONG ulSessionId, ULONG ulCmdId,
	const CHAR *szMinidumpFileName	// name of minidump file to be created,
									// if NULL, a time-based name is generated
)
{
	if (!spqos::ioutils::PathExists("minidumps"))
	{
		SPQOS_TRY
		{
			// create a minidumps folder
			const ULONG ulWrPerms = S_IRUSR | S_IWUSR | S_IXUSR;
			spqos::ioutils::CreateDir("minidumps", ulWrPerms);
		}
		SPQOS_CATCH_EX(ex)
		{
			std::cerr << "[OPT]: Failed to create minidumps directory";

			// ignore exceptions during the creation of directory
			SPQOS_RESET_EX;
		}
		SPQOS_CATCH_END;
	}

	if (NULL == szMinidumpFileName)
	{
		// generate a time-based file name
		CUtils::GenerateFileName(buf, "minidumps/Minidump", "mdp", length,
								 ulSessionId, ulCmdId);
	}
	else
	{
		// use provided file name
		const CHAR *szPrefix = "minidumps/";
		const ULONG ulPrefixLength = clib::Strlen(szPrefix);
		clib::Strncpy(buf, szPrefix, ulPrefixLength);

		// remove directory path before file name, if any
		ULONG ulNameLength = clib::Strlen(szMinidumpFileName);
		ULONG ulNameStart = ulNameLength - 1;
		while (ulNameStart > 0 && szMinidumpFileName[ulNameStart - 1] != '\\' &&
			   szMinidumpFileName[ulNameStart - 1] != '/')
		{
			ulNameStart--;
		}

		ulNameLength = clib::Strlen(szMinidumpFileName + ulNameStart);
		clib::Strncpy(buf + ulPrefixLength, szMinidumpFileName + ulNameStart,
					  ulNameLength + 1);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CMinidumperUtils::Finalize
//
//	@doc:
//		Finalize minidump and dump to file
//
//---------------------------------------------------------------------------
void
CMinidumperUtils::Finalize(CMiniDumperDXL *pmdmp, BOOL fSerializeErrCtx)
{
	CAutoTraceFlag atf1(EtraceSimulateAbort, false);
	CAutoTraceFlag atf2(EtraceSimulateOOM, false);
	CAutoTraceFlag atf3(EtraceSimulateNetError, false);
	CAutoTraceFlag atf4(EtraceSimulateIOError, false);

	if (fSerializeErrCtx)
	{
		CErrorContext *perrctxt = CTask::Self()->ConvertErrCtxt();
		perrctxt->Serialize();
	}

	pmdmp->Finalize();
}

//---------------------------------------------------------------------------
//	@function:
//		CMinidumperUtils::PdxlnExecuteMinidump
//
//	@doc:
//		Load and execute the minidump in the given file
//
//---------------------------------------------------------------------------
CDXLNode *
CMinidumperUtils::PdxlnExecuteMinidump(CMemoryPool *mp, const CHAR *file_name,
									   ULONG ulSegments, ULONG ulSessionId,
									   ULONG ulCmdId,
									   COptimizerConfig *optimizer_config,
									   IConstExprEvaluator *pceeval)
{
	SPQOS_ASSERT(NULL != file_name);
	SPQOS_ASSERT(NULL != optimizer_config);

	CAutoTimer at("Minidump", true /*fPrint*/);

	// load dump file
	CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(mp, file_name);
	SPQOS_CHECK_ABORT;

	CDXLNode *pdxlnPlan =
		PdxlnExecuteMinidump(mp, pdxlmd, file_name, ulSegments, ulSessionId,
							 ulCmdId, optimizer_config, pceeval);

	// cleanup
	SPQOS_DELETE(pdxlmd);

	return pdxlnPlan;
}


//---------------------------------------------------------------------------
//	@function:
//		CMinidumperUtils::PdxlnExecuteMinidump
//
//	@doc:
//		Execute the given minidump
//
//---------------------------------------------------------------------------
CDXLNode *
CMinidumperUtils::PdxlnExecuteMinidump(CMemoryPool *mp, CDXLMinidump *pdxlmd,
									   const CHAR *file_name, ULONG ulSegments,
									   ULONG ulSessionId, ULONG ulCmdId,
									   COptimizerConfig *optimizer_config,
									   IConstExprEvaluator *pceeval)
{
	SPQOS_ASSERT(NULL != file_name);

	// reset metadata ccache
	CMDCache::Reset();

	CMetadataAccessorFactory factory(mp, pdxlmd, file_name);

	CDXLNode *result = CMinidumperUtils::PdxlnExecuteMinidump(
		mp, factory.Pmda(), pdxlmd, file_name, ulSegments, ulSessionId, ulCmdId,
		optimizer_config, pceeval);

	return result;
}


//---------------------------------------------------------------------------
//	@function:
//		CMinidumperUtils::PdxlnExecuteMinidump
//
//	@doc:
//		Execute the given minidump using the given MD accessor
//
//---------------------------------------------------------------------------
CDXLNode *
CMinidumperUtils::PdxlnExecuteMinidump(
	CMemoryPool *mp, CMDAccessor *md_accessor, CDXLMinidump *pdxlmd,
	const CHAR *file_name, ULONG ulSegments, ULONG ulSessionId, ULONG ulCmdId,
	COptimizerConfig *optimizer_config, IConstExprEvaluator *pceeval)
{
	SPQOS_ASSERT(NULL != md_accessor);
	SPQOS_ASSERT(NULL != pdxlmd->GetQueryDXLRoot() &&
				NULL != pdxlmd->PdrspqdxlnQueryOutput() &&
				NULL != pdxlmd->GetCTEProducerDXLArray() &&
				"No query found in Minidump");
	SPQOS_ASSERT(NULL != pdxlmd->GetMdIdCachedObjArray() &&
				NULL != pdxlmd->GetSysidPtrArray() &&
				"No metadata found in Minidump");
	SPQOS_ASSERT(NULL != optimizer_config);

	CDXLNode *pdxlnPlan = NULL;
	CAutoTimer at("Minidump", true /*fPrint*/);

	SPQOS_CHECK_ABORT;

	// set trace flags
	CBitSet *pbsEnabled = NULL;
	CBitSet *pbsDisabled = NULL;
	SetTraceflags(mp, pdxlmd->Pbs(), &pbsEnabled, &pbsDisabled);

	if (NULL == pceeval)
	{
		// disable constant expression evaluation when running minidump since
		// there no executor to compute the scalar expression
		SPQOS_UNSET_TRACE(EopttraceEnableConstantExpressionEvaluation);
	}

	CErrorHandlerStandard errhdl;
	SPQOS_TRY_HDL(&errhdl)
	{
		pdxlnPlan = COptimizer::PdxlnOptimize(
			mp, md_accessor, pdxlmd->GetQueryDXLRoot(),
			pdxlmd->PdrspqdxlnQueryOutput(), pdxlmd->GetCTEProducerDXLArray(),
			pceeval, ulSegments, ulSessionId, ulCmdId,
			NULL,  // search_stage_array
			optimizer_config, file_name);
	}
	SPQOS_CATCH_EX(ex)
	{
		// reset trace flags
		ResetTraceflags(pbsEnabled, pbsDisabled);

		CRefCount::SafeRelease(pbsEnabled);
		CRefCount::SafeRelease(pbsDisabled);

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	// reset trace flags
	ResetTraceflags(pbsEnabled, pbsDisabled);

	// clean up
	CRefCount::SafeRelease(pbsEnabled);
	CRefCount::SafeRelease(pbsDisabled);

	SPQOS_CHECK_ABORT;

	return pdxlnPlan;
}

// EOF
