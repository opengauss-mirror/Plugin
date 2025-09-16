/*
 * errormessage.c
 *	  Error handling related support functionality.
 *
 * Copyright (c) Citus Data, Inc.
 */

#include "postgres.h"

#include "libpq/sha2.h"
#include "utils/builtins.h"
#include "utils/memutils.h"

#include "distributed/citus_nodes.h"
#include "distributed/errormessage.h"
#include "distributed/log_utils.h"

/*
 * ThrowErrorData --- report an error described by an ErrorData structure
 *
 * This is somewhat like ReThrowError, but it allows elevels besides ERROR,
 * and the boolean flags such as output_to_server are computed via the
 * default rules rather than being copied from the given ErrorData.
 * This is primarily used to re-report errors originally reported by
 * background worker processes and then propagated (with or without
 * modification) to the backend responsible for them.
 */
void ThrowErrorData(ErrorData* edata)
{
    ErrorData* newedata = NULL;

    if (!errstart(edata->elevel, edata->filename, edata->lineno, edata->funcname,
                  edata->domain)) {
        return;
    }

    /* Push the data back into the error context */
    t_thrd.log_cxt.recursion_depth++;
    auto oldMemCtx = MemoryContextSwitchTo(ErrorContext);
    newedata = &t_thrd.log_cxt.errordata[t_thrd.log_cxt.errordata_stack_depth];

    /* Copy the supplied fields to the error stack entry. */
    newedata->message = edata->message ? pstrdup(edata->message) : newedata->message;
    newedata->detail = edata->detail ? pstrdup(edata->detail) : newedata->detail;
    newedata->detail_log =
        edata->detail_log ? pstrdup(edata->detail_log) : newedata->detail_log;
    newedata->hint = edata->hint ? pstrdup(edata->hint) : newedata->hint;
    newedata->context = edata->context ? pstrdup(edata->context) : newedata->context;
    newedata->internalquery =
        edata->internalquery ? pstrdup(edata->internalquery) : newedata->internalquery;
    newedata->backtrace_log =
        edata->backtrace_log ? pstrdup(edata->backtrace_log) : newedata->backtrace_log;
    newedata->cause = edata->cause ? pstrdup(edata->cause) : newedata->cause;
    newedata->action = edata->action ? pstrdup(edata->action) : newedata->action;
    newedata->sqlstate = edata->sqlstate ? pstrdup(edata->sqlstate) : newedata->sqlstate;
    newedata->class_origin =
        edata->class_origin ? pstrdup(edata->class_origin) : newedata->class_origin;
    newedata->subclass_origin = edata->subclass_origin ? pstrdup(edata->subclass_origin)
                                                       : newedata->subclass_origin;
    newedata->cons_catalog =
        edata->cons_catalog ? pstrdup(edata->cons_catalog) : newedata->cons_catalog;
    newedata->cons_schema =
        edata->cons_schema ? pstrdup(edata->cons_schema) : newedata->cons_schema;
    newedata->cons_name =
        edata->cons_name ? pstrdup(edata->cons_name) : newedata->cons_name;
    newedata->catalog_name =
        edata->catalog_name ? pstrdup(edata->catalog_name) : newedata->catalog_name;
    newedata->schema_name =
        edata->schema_name ? pstrdup(edata->schema_name) : newedata->schema_name;
    newedata->table_name =
        edata->table_name ? pstrdup(edata->table_name) : newedata->table_name;
    newedata->column_name =
        edata->column_name ? pstrdup(edata->column_name) : newedata->column_name;
    newedata->cursor_name =
        edata->cursor_name ? pstrdup(edata->cursor_name) : newedata->cursor_name;
    newedata->mysql_errno =
        edata->mysql_errno ? pstrdup(edata->mysql_errno) : newedata->mysql_errno;

    if (edata->sqlerrcode != 0) {
        newedata->sqlerrcode = edata->sqlerrcode;
    }

    newedata->cursorpos = edata->cursorpos;
    newedata->internalpos = edata->internalpos;

    MemoryContextSwitchTo(oldMemCtx);
    t_thrd.log_cxt.recursion_depth--;

    /*
     * And let errfinish() finish up.
     */
    errfinish(0);
}

/*
 * DeferredErrorInternal is a helper function for DeferredError().
 */
DeferredErrorMessage* DeferredErrorMessage::Create(int code, const char* message,
                                                   const char* detail, const char* hint,
                                                   const char* filename, int linenumber,
                                                   const char* functionname)
{
    DeferredErrorMessage* error = CitusMakeNode(DeferredErrorMessage);

    Assert(message != NULL);

    error->code = code;
    error->message = message;
    error->detail = detail;
    error->hint = hint;
    error->filename = filename;
    error->linenumber = linenumber;
    error->functionname = functionname;
    return error;
}

/*
 * RaiseDeferredErrorInternal is a helper function for RaiseDeferredError().
 */
void DeferredErrorMessage::Raise(int elevel)
{
    ErrorData* errorData = static_cast<ErrorData*>(palloc0(sizeof(ErrorData)));

    errorData->sqlerrcode = this->code;
    errorData->elevel = elevel;
    errorData->message = pstrdup(this->message);
    if (this->detail) {
        errorData->detail = pstrdup(this->detail);
    }
    if (this->hint) {
        errorData->hint = pstrdup(this->hint);
    }
    errorData->filename = pstrdup(this->filename);
    errorData->lineno = this->linenumber;
    errorData->funcname = const_cast<char*>(this->functionname);
    ThrowErrorData(errorData);
}
