//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 Greenplum, Inc.
//
//	@filename:
//		SPQOptimizer.cpp
//
//	@doc:
//		Entry point to SPQ optimizer
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spq_optimizer_util/SPQOptimizer.h"

#include "spq_optimizer_util/utils/CMemoryPoolPalloc.h"
#include "spq_optimizer_util/utils/CMemoryPoolPallocManager.h"
#include "spq_optimizer_util/utils/COptTasks.h"

// the following headers are needed to reference optimizer library initializers
#include "spqos/_api.h"
#include "spqos/memory/CMemoryPoolManager.h"

#include "spq_optimizer_util/spq_wrappers.h"
#include "spqopt/init.h"
#include "naucrates/exception.h"
#include "naucrates/init.h"
#include "utils/guc.h"
#include "utils/memutils.h"
#include "storage/ipc.h"

bool optimizer_trace_fallback = false;

extern MemoryContext MessageContext;
extern THR_LOCAL bool SPQ_IN_PROCESSING;

void DelCException(CException **exception)
{
	if (exception == NULL || *exception == NULL)
	{
		return;
	}
	delete *exception;
	*exception = NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		SPQOptimizer::PlstmtOptimize
//
//	@doc:
//		Optimize given query using SPQ optimizer
//
//---------------------------------------------------------------------------
PlannedStmt *
SPQOptimizer::SPQOPTOptimizedPlan(
	Query *query,
	bool *
		had_unexpected_failure	// output : set to true if optimizer unexpectedly failed to produce plan
)
{
	SOptContext spqopt_context;
	PlannedStmt *plStmt = NULL;

	*had_unexpected_failure = false;
	CException *exception = NULL;
	SPQ_IN_PROCESSING = true;
	SPQOS_TRY
	{
		plStmt = COptTasks::SPQOPTOptimizedPlan(query, &spqopt_context);
		// clean up context
		spqopt_context.Free(spqopt_context.epinQuery, spqopt_context.epinPlStmt);
	}
	SPQOS_CATCH_EX(ex)
	{
		exception = new CException(ex.Major(), ex.Minor(), ex.Filename(), ex.Line());
	}
	SPQOS_CATCH_END;
	SPQ_IN_PROCESSING = false;
	if (exception == NULL) {
		return plStmt;
	}
		// clone the error message before context free.
		CHAR *serialized_error_msg =
			spqopt_context.CloneErrorMsg(t_thrd.mem_cxt.msg_mem_cxt);
		// clean up context
		spqopt_context.Free(spqopt_context.epinQuery, spqopt_context.epinPlStmt);

		// Special handler for a few common user-facing errors. In particular,
		// we want to use the correct error code for these, in case an application
		// tries to do something smart with them. Also, ERRCODE_INTERNAL_ERROR
		// is handled specially in elog.c, and we don't want that for "normal"
		// application errors.
		if (SPQOS_MATCH_EX((*exception), spqdxl::ExmaDXL,
						  spqdxl::ExmiQuery2DXLNotNullViolation))
		{
			errstart(ERROR, exception->Filename(), exception->Line(), NULL, TEXTDOMAIN);
			DelCException(&exception);
			errfinish(errcode(ERRCODE_NOT_NULL_VIOLATION),
					  errmsg("%s", serialized_error_msg));
		}

		else if (SPQOS_MATCH_EX((*exception), spqdxl::ExmaDXL, spqdxl::ExmiOptimizerError) ||
				 spqopt_context.m_should_error_out)
		{
			Assert(NULL != serialized_error_msg);
			errstart(ERROR, exception->Filename(), exception->Line(), NULL, TEXTDOMAIN);
			DelCException(&exception);
			errfinish(errcode(ERRCODE_INTERNAL_ERROR),
					  errmsg("%s", serialized_error_msg));
		}
		else if (SPQOS_MATCH_EX((*exception), spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError))
		{
			DelCException(&exception);
			PG_RE_THROW();
		}
		else if (SPQOS_MATCH_EX((*exception), spqdxl::ExmaDXL,
							   spqdxl::ExmiNoAvailableMemory))
		{
			errstart(ERROR, exception->Filename(), exception->Line(), NULL, TEXTDOMAIN);
			DelCException(&exception);
			errfinish(errcode(ERRCODE_INTERNAL_ERROR),
					  errmsg("no available memory to allocate string buffer"));
		}
		else if (SPQOS_MATCH_EX((*exception), spqdxl::ExmaDXL,
							   spqdxl::ExmiInvalidComparisonTypeCode))
		{
			errstart(ERROR, exception->Filename(), exception->Line(), NULL, TEXTDOMAIN);
			DelCException(&exception);
			errfinish(
				errcode(ERRCODE_INTERNAL_ERROR),
				errmsg(
					"invalid comparison type code. Valid values are Eq, NEq, LT, LEq, GT, GEq."));
		}

		// Failed to produce a plan, but it wasn't an error that should
		// be propagated to the user. Log the failure if needed, and
		// return without a plan. The caller should fall back to the
		// Postgres planner.

		if (optimizer_trace_fallback)
		{
			errstart(INFO, exception->Filename(), exception->Line(), NULL, TEXTDOMAIN);
			DelCException(&exception);
			errfinish(
				errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				errmsg(
					"SPQORCA failed to produce a plan, falling back to planner"),
				serialized_error_msg ? errdetail("%s", serialized_error_msg)
									 : 0);
		}

		*had_unexpected_failure = spqopt_context.m_is_unexpected_failure;

		if (serialized_error_msg)
			pfree(serialized_error_msg);
		DelCException(&exception);

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		SPQOptimizer::SerializeDXLPlan
//
//	@doc:
//		Serialize planned statement into DXL
//
//---------------------------------------------------------------------------
char *
SPQOptimizer::SerializeDXLPlan(Query *query)
{
	CException *exception = NULL;
	SPQOS_TRY;
	{
		return COptTasks::Optimize(query);
	}
	SPQOS_CATCH_EX(ex);
	{
		exception = new CException(ex.Major(), ex.Minor(), ex.Filename(), ex.Line());
	}
	SPQOS_CATCH_END;
	errstart(ERROR, exception->Filename(), exception->Line(), NULL, TEXTDOMAIN);
	DelCException(&exception);
	errfinish(errcode(ERRCODE_INTERNAL_ERROR),
				  errmsg("optimizer failed to produce plan"));
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		InitSPQOPT()
//
//	@doc:
//		Initialize SPQTOPT and dependent libraries
//
//---------------------------------------------------------------------------
void
SPQOptimizer::InitSPQOPT()
{

    if (u_sess->spq_cxt.spq_opt_initialized) {
        return;
    }

	if (u_sess->attr.attr_spq.spq_optimizer_use_gauss_allocators)
	{
		CMemoryPoolPallocManager::Init();
	}

	struct spqos_init_params params = {spqdb::IsAbortRequested};

	spqos_init(&params);
	spqdxl_init();
	spqopt_init();
    u_sess->spq_cxt.spq_opt_initialized = true;
    on_proc_exit(UnInitSPQOPT, PointerGetDatum(u_sess));

}

//---------------------------------------------------------------------------
//	@function:
//		TerminateSPQOPT()
//
//	@doc:
//		Terminate SPQOPT and dependent libraries
//
//---------------------------------------------------------------------------
void
SPQOptimizer::TerminateSPQOPT()
{
	spqopt_terminate();
	spqdxl_terminate();
	spqos_terminate();
}

//---------------------------------------------------------------------------
//	@function:
//		SPQOPTOptimizedPlan
//
//	@doc:
//		Expose SPQ optimizer API to C files
//
//---------------------------------------------------------------------------
PlannedStmt *
SPQOPTOptimizedPlan(Query *query, bool *had_unexpected_failure)
{
	return SPQOptimizer::SPQOPTOptimizedPlan(query, had_unexpected_failure);
}

//---------------------------------------------------------------------------
//	@function:
//		SerializeDXLPlan
//
//	@doc:
//		Serialize planned statement to DXL
//
//---------------------------------------------------------------------------
char *
SerializeDXLPlan(Query *query)
{
	return SPQOptimizer::SerializeDXLPlan(query);
}

//---------------------------------------------------------------------------
//	@function:
//		InitSPQOPT()
//
//	@doc:
//		Initialize SPQTOPT and dependent libraries
//
//---------------------------------------------------------------------------
void
InitSPQOPT()
{
	CException *exception = NULL;
	SPQOS_TRY
	{
		return SPQOptimizer::InitSPQOPT();
	}
	SPQOS_CATCH_EX(ex)
	{
		exception = new CException(ex.Major(), ex.Minor(), ex.Filename(), ex.Line());
	}
	SPQOS_CATCH_END;
	if (exception != NULL && SPQOS_MATCH_EX((*exception), spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError))
	{
		DelCException(&exception);
		PG_RE_THROW();
	}
	DelCException(&exception);
}

//---------------------------------------------------------------------------
//	@function:
//		TerminateSPQOPT()
//
//	@doc:
//		Terminate SPQOPT and dependent libraries
//
//---------------------------------------------------------------------------
void
TerminateSPQOPT()
{
	CException *exception = NULL;
	SPQOS_TRY
	{
		return SPQOptimizer::TerminateSPQOPT();
	}
	SPQOS_CATCH_EX(ex)
	{
		exception = new CException(ex.Major(), ex.Minor(), ex.Filename(), ex.Line());
	}
	SPQOS_CATCH_END;
	if (exception != NULL && SPQOS_MATCH_EX((*exception), spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError))
	{
		DelCException(&exception);
		PG_RE_THROW();
	}
	DelCException(&exception);
}
void RecordBackTrace()
{
    if (SPQ_IN_PROCESSING == false) {
        ereport(LOG, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("SPQ exit normal tid[%d]", gettid())));
    } else {
        ereport(LOG, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("SPQ exit error tid[%d]", gettid())));
    }
}

void UnInitSPQOPT(int status, Datum arg)
{
    if (status != 0) {
        RecordBackTrace();
    }
    knl_session_context* session_back = u_sess;
    u_sess = (knl_session_context*) DatumGetPointer(arg);
    TerminateSPQOPT();
    u_sess = session_back;
}

// EOF
