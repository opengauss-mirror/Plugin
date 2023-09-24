//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		COptCtxt.cpp
//
//	@doc:
//		Implementation of optimizer context
//---------------------------------------------------------------------------

#include "spqopt/base/COptCtxt.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDefaultComparator.h"
#include "spqopt/cost/ICostModel.h"
#include "spqopt/eval/IConstExprEvaluator.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;

// value of the first value part id
ULONG COptCtxt::m_ulFirstValidPartId = 1;

//---------------------------------------------------------------------------
//	@function:
//		COptCtxt::COptCtxt
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
COptCtxt::COptCtxt(CMemoryPool *mp, CColumnFactory *col_factory,
				   CMDAccessor *md_accessor, IConstExprEvaluator *pceeval,
				   COptimizerConfig *optimizer_config)
	: CTaskLocalStorageObject(CTaskLocalStorage::EtlsidxOptCtxt),
	  m_mp(mp),
	  m_pcf(col_factory),
	  m_pmda(md_accessor),
	  m_pceeval(pceeval),
	  m_pcomp(SPQOS_NEW(m_mp) CDefaultComparator(pceeval)),
	  m_auPartId(m_ulFirstValidPartId),
	  m_pcteinfo(NULL),
	  m_pdrspqcrSystemCols(NULL),
	  m_optimizer_config(optimizer_config),
	  m_fDMLQuery(false),
	  m_has_master_only_tables(false),
	  m_has_volatile_func(false),
	  m_has_replicated_tables(false)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != col_factory);
	SPQOS_ASSERT(NULL != md_accessor);
	SPQOS_ASSERT(NULL != pceeval);
	SPQOS_ASSERT(NULL != m_pcomp);
	SPQOS_ASSERT(NULL != optimizer_config);
	SPQOS_ASSERT(NULL != optimizer_config->GetCostModel());

	m_pcteinfo = SPQOS_NEW(m_mp) CCTEInfo(m_mp);
	m_cost_model = optimizer_config->GetCostModel();
	m_direct_dispatchable_filters = SPQOS_NEW(mp) CExpressionArray(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		COptCtxt::~COptCtxt
//
//	@doc:
//		dtor
//		Does not de-allocate memory pool!
//
//---------------------------------------------------------------------------
COptCtxt::~COptCtxt()
{
	SPQOS_DELETE(m_pcf);
	SPQOS_DELETE(m_pcomp);
	m_pceeval->Release();
	m_pcteinfo->Release();
	m_optimizer_config->Release();
	CRefCount::SafeRelease(m_pdrspqcrSystemCols);
	CRefCount::SafeRelease(m_direct_dispatchable_filters);
}


//---------------------------------------------------------------------------
//	@function:
//		COptCtxt::PoctxtCreate
//
//	@doc:
//		Factory method for optimizer context
//
//---------------------------------------------------------------------------
COptCtxt *
COptCtxt::PoctxtCreate(CMemoryPool *mp, CMDAccessor *md_accessor,
					   IConstExprEvaluator *pceeval,
					   COptimizerConfig *optimizer_config)
{
	SPQOS_ASSERT(NULL != optimizer_config);

	// CONSIDER:  - 1/5/09; allocate column factory out of given mem pool
	// instead of having it create its own;
	CColumnFactory *col_factory = SPQOS_NEW(mp) CColumnFactory;

	COptCtxt *poctxt = NULL;
	{
		// safe handling of column factory; since it owns a pool that would be
		// leaked if below allocation fails
		CAutoP<CColumnFactory> a_pcf;
		a_pcf = col_factory;
		a_pcf.Value()->Initialize();

		poctxt = SPQOS_NEW(mp)
			COptCtxt(mp, col_factory, md_accessor, pceeval, optimizer_config);

		// detach safety
		(void) a_pcf.Reset();
	}
	return poctxt;
}


//---------------------------------------------------------------------------
//	@function:
//		COptCtxt::FAllEnforcersEnabled
//
//	@doc:
//		Return true if all enforcers are enabled
//
//---------------------------------------------------------------------------
BOOL
COptCtxt::FAllEnforcersEnabled()
{
	BOOL fEnforcerDisabled =
		SPQOS_FTRACE(EopttraceDisableMotions) ||
		SPQOS_FTRACE(EopttraceDisableMotionBroadcast) ||
		SPQOS_FTRACE(EopttraceDisableMotionGather) ||
		SPQOS_FTRACE(EopttraceDisableMotionHashDistribute) ||
		SPQOS_FTRACE(EopttraceDisableMotionRandom) ||
		SPQOS_FTRACE(EopttraceDisableMotionRountedDistribute) ||
		SPQOS_FTRACE(EopttraceDisableSort) ||
		SPQOS_FTRACE(EopttraceDisableSpool) ||
		SPQOS_FTRACE(EopttraceDisablePartPropagation);

	return !fEnforcerDisabled;
}
