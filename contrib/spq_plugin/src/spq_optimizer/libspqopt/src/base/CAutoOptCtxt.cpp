//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CAutoOptCtxt.cpp
//
//	@doc:
//		Implementation of auto opt context
//---------------------------------------------------------------------------

#include "spqopt/base/CAutoOptCtxt.h"

#include "spqos/base.h"

#include "spqopt/cost/ICostModel.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/optimizer/COptimizerConfig.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CAutoOptCtxt::CAutoOptCtxt
//
//	@doc:
//		ctor
//		Create and install default optimizer context
//
//---------------------------------------------------------------------------
CAutoOptCtxt::CAutoOptCtxt(CMemoryPool *mp, CMDAccessor *md_accessor,
						   IConstExprEvaluator *pceeval,
						   COptimizerConfig *optimizer_config)
{
	if (NULL == optimizer_config)
	{
		// create default statistics configuration
		optimizer_config = COptimizerConfig::PoconfDefault(mp);
	}
	if (NULL == pceeval)
	{
		// use the default constant expression evaluator which cannot evaluate any expression
		pceeval = SPQOS_NEW(mp) CConstExprEvaluatorDefault();
	}

	COptCtxt *poctxt =
		COptCtxt::PoctxtCreate(mp, md_accessor, pceeval, optimizer_config);
	ITask::Self()->GetTls().Store(poctxt);
}

//---------------------------------------------------------------------------
//	@function:
//		CAutoOptCtxt::CAutoOptCtxt
//
//	@doc:
//		ctor
//		Create and install default optimizer context with the given cost model
//
//---------------------------------------------------------------------------
CAutoOptCtxt::CAutoOptCtxt(CMemoryPool *mp, CMDAccessor *md_accessor,
						   IConstExprEvaluator *pceeval, ICostModel *pcm)
{
	SPQOS_ASSERT(NULL != pcm);

	// create default statistics configuration
	COptimizerConfig *optimizer_config =
		COptimizerConfig::PoconfDefault(mp, pcm);

	if (NULL == pceeval)
	{
		// use the default constant expression evaluator which cannot evaluate any expression
		pceeval = SPQOS_NEW(mp) CConstExprEvaluatorDefault();
	}

	COptCtxt *poctxt =
		COptCtxt::PoctxtCreate(mp, md_accessor, pceeval, optimizer_config);
	ITask::Self()->GetTls().Store(poctxt);
}

//---------------------------------------------------------------------------
//	@function:
//		CAutoOptCtxt::~CAutoOptCtxt
//
//	@doc:
//		dtor
//		uninstall optimizer context and destroy
//
//---------------------------------------------------------------------------
CAutoOptCtxt::~CAutoOptCtxt()
{
	CTaskLocalStorageObject *ptlsobj =
		ITask::Self()->GetTls().Get(CTaskLocalStorage::EtlsidxOptCtxt);
	ITask::Self()->GetTls().Remove(ptlsobj);

	SPQOS_DELETE(ptlsobj);
}

// EOF
