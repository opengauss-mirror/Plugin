#include "postgres.h"
#include "storage/lock/lwlock.h"
#include "storage/shmem.h"
#include "utils/guc.h"
#include "commands/variable.h"
#include "knl/knl_session.h"

#include "orafce.h"
#include "builtins.h"
#include "pipe.h"

#define NOT_INITIALIZED		NULL

/*  default value */
uint32 orafce_index;

void set_extension_index(uint32 index) {
    orafce_index = index;
}

void init_session_vars(void) {
    RepallocSessionVarsArrayIfNecessary();
    orafce_session_context* orafce_psc = (orafce_session_context*)MemoryContextAllocZero(u_sess->self_mem_cxt, sizeof(orafce_session_context));
    u_sess->attr.attr_common.extension_session_vars_array[orafce_index] = orafce_psc;
    orafce_psc->nls_date_format = NULL;
    orafce_psc->orafce_timezone = NULL;
    orafce_psc->orafce_varchar2_null_safe_concat = false;
/* pipe.c */
    orafce_psc->output_buffer = NULL;
    orafce_psc->input_buffer = NULL;
    orafce_psc->pipe_sid = 0;
    orafce_psc->pipes = NULL;
    orafce_psc->shmem_lockid = NOT_INITIALIZED;
/* putline.c */
    orafce_psc->is_server_output = false;
    orafce_psc->buffer = NULL;
    orafce_psc->buffer_size = 0;
    orafce_psc->buffer_len = 0;
    orafce_psc->buffer_get = 0;
/* convert.c */
    orafce_psc->orafce_Utf8ToServerConvProc = NULL;
/* datefce.c */
    orafce_psc->mru_weekdays = NULL;
/* file.c */
    orafce_psc->slotid = 0;
/* others.c */
    orafce_psc->lc_collate_cache = NULL;
    orafce_psc->multiplication = 1;
    orafce_psc->def_locale = NULL;
/* plvdate.c */
    orafce_psc->use_easter = true;
    orafce_psc->use_great_friday = true;
    orafce_psc->include_start = true;
    orafce_psc->country_id = -1;
    orafce_psc->holidays_c = 0;
    orafce_psc->exceptions_c = 0;
/* plvlex.c */
    orafce_psc->__node = NULL;
    orafce_psc->__result = NULL;
/* sqlparse.y */
    orafce_psc->scanbuf = NULL;
    orafce_psc->scanbuflen = 0;
/* sqlscan.l */
    orafce_psc->xcdepth = 0;    /* depth of nesting in slash-star comments */
    orafce_psc->dolqstart = NULL;      /* current $foo$ quote start string */
    orafce_psc->extended_string = false;
/* plvsubst.c */
    orafce_psc->c_subst = NULL;
/* random.c */
    orafce_psc->seed = 0;
/* alert.c */
    orafce_psc->alert_events = NULL;
    orafce_psc->alert_locks = NULL;
    orafce_psc->alert_session_lock = NULL;
    orafce_psc->sensitivity = 250.0;
/* shmmc.c */
    orafce_psc->list_c = NULL;
    orafce_psc->list = NULL;
    orafce_psc->max_size = 0;
}

orafce_session_context* get_session_context() {
    if (u_sess->attr.attr_common.extension_session_vars_array[orafce_index] == NULL) {
        init_session_vars();
    }
    return (orafce_session_context*)u_sess->attr.attr_common.extension_session_vars_array[orafce_index];
}

void
_PG_init(void)
{

#if PG_VERSION_NUM < 90600

	RequestAddinLWLocks(1);

#endif

	RequestAddinShmemSpace(SHMEMMSGSZ);

	/* Define custom GUC variables. */
	DefineCustomStringVariable("orafce.nls_date_format",
									"Emulate oracle's date output behaviour.",
									NULL,
									&(get_session_context()->nls_date_format),
									NULL,
									PGC_USERSET,
									0,
									NULL,
									NULL, NULL);

	DefineCustomStringVariable("orafce.timezone",
									"Specify timezone used for sysdate function.",
									NULL,
									&(get_session_context()->orafce_timezone),
									"GMT",
									PGC_USERSET,
									0,
									check_timezone, NULL, NULL);

	DefineCustomBoolVariable("orafce.varchar2_null_safe_concat",
									"Specify timezone used for sysdate function.",
									NULL,
									&(get_session_context()->orafce_varchar2_null_safe_concat),
									false,
									PGC_USERSET,
									0,
									NULL, NULL, NULL);

	EmitWarningsOnPlaceholders("orafce");
}
