/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#include <postgres.h>
#include <pg_config.h>
#include <access/xact.h>
#include <commands/extension.h>
#include <miscadmin.h>
#include <utils/guc.h>
#include <parser/analyze.h>

#include "extension.h"
#include "bgw/launcher_interface.h"
#include "guc.h"
#include "catalog.h"
#include "version.h"
#include "compat.h"
#include "config.h"
#include "license_guc.h"
#include "constraint_aware_append.h"
#include "hypertable_cache.h"


#include "cache.h"
#include "chunk_append/exec.h"


static uint32 tsdb_index;
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif




extern void _hypertable_cache_init(void);
extern void _hypertable_cache_fini(void);

extern void _cache_invalidate_init(void);
extern void _cache_invalidate_fini(void);

extern void _cache_init(void);
extern void _cache_fini(void);

extern void _planner_init(void);
extern void _planner_fini(void);

extern void _process_utility_init(void);
extern void _process_utility_fini(void);

extern void _event_trigger_init(void);
extern void _event_trigger_fini(void);

extern void _conn_plain_init();
extern void _conn_plain_fini();

#ifdef TS_USE_OPENSSL
extern void _conn_ssl_init(); 
extern void _conn_ssl_fini(); 
#endif

extern void _chunk_append_init();

extern void TSDLLEXPORT _PG_init(void);
extern void TSDLLEXPORT _PG_fini(void);

extern "C" void set_extension_index(uint32 index);
extern "C" void init_session_vars(void);

extern tsdb_session_context* get_session_context(bool is_from_PG_init);

TS_FUNCTION_INFO_V1(ts_post_load_init);



void
_PG_init(void)
{
	/*
	 * Check extension_is loaded to catch certain errors such as calls to
	 * functions defined on the wrong extension version
	 */
	ts_extension_check_version(TIMESCALEDB_VERSION_MOD);
	ts_extension_check_server_version();
	ts_bgw_check_loader_api_version();
	
	

	
	if (!tsdb_first_start)
	{
	_cache_init();
	_hypertable_cache_init();
	_constraint_aware_append_init();
	_cache_invalidate_init();
	_planner_init();
	_chunk_append_init();
	_process_utility_init();
	_guc_init();
	_conn_plain_init();
	}	
	//

	

	
	

}

void
_PG_fini(void)
{
	/*
	 * Order of items should be strict reverse order of _PG_init. Please
	 * document any exceptions.
	 */
#ifdef TS_USE_OPENSSL
	_conn_ssl_fini();
#endif
	_conn_plain_fini();
	_guc_fini();
	_process_utility_fini();
	_event_trigger_fini();
	_planner_fini();
	_cache_invalidate_fini();
	_hypertable_cache_fini();
	_cache_fini();
}

TSDLLEXPORT Datum
ts_post_load_init(PG_FUNCTION_ARGS)
{
	/*
	 * Unfortunately, if we load the tsl during _PG_init parallel workers try
	 * to load the tsl before timescale itself, causing link-time errors. To
	 * prevent this we defer loading until here.
	 */
	ts_license_enable_module_loading();

	PG_RETURN_VOID();
}



void set_extension_index(uint32 index) {
    tsdb_index = index;  
}  




tsdb_session_context* get_session_context(bool is_from_PG_init)  
{    
       if (u_sess->attr.attr_common.extension_session_vars_array[tsdb_index] == NULL && !is_from_PG_init) {
        init_session_vars();
    }
    return  (tsdb_session_context*)u_sess->attr.attr_common.extension_session_vars_array[tsdb_index];  
} 

void init_session_vars(void) 
{      
	if (u_sess->attr.attr_common.extension_session_vars_array[tsdb_index]!=NULL)
	return
	
    RepallocSessionVarsArrayIfNecessary(); 
	tsdb_session_context* psc = (tsdb_session_context*)MemoryContextAllocZero(u_sess->self_mem_cxt, sizeof(tsdb_session_context));
	u_sess->attr.attr_common.extension_session_vars_array[tsdb_index] = psc;  
	psc->tsdb_pinned_caches = NIL;
	psc->tsdb_pinned_caches_mctx = NULL;
	psc->tsdb_hypertable_cache_current = NULL;     
	psc->tsdb_planner_hcaches = NIL;
	psc->tsdb_TS_CTE_EXPAND= "ts_expand";
	

	psc->tsdb_chunk_append_plan_methods = {
	.ExtensibleName = "ChunkAppend",
	.CreateExtensiblePlanState = ts_chunk_append_state_create,
	};

	psc->tsdb_telemetry_level_options[0] = { "off", TELEMETRY_OFF, false };
	psc->tsdb_telemetry_level_options[1] = { "basic", TELEMETRY_BASIC, false };
	psc->tsdb_telemetry_level_options[2] = { NULL, 0, false };
	psc->tsdb_on_level = TELEMETRY_BASIC;


	psc->tsdb_expect_chunk_modification = false;
	_constraint_aware_append_init();

	
	if (!ts_extension_is_loaded())
		return;
	

	if (!tsdb_first_start)
	{
	_cache_init();
	_hypertable_cache_init();
	_cache_invalidate_init();

	_planner_init();
	_chunk_append_init();
	_process_utility_init();
	}	

}