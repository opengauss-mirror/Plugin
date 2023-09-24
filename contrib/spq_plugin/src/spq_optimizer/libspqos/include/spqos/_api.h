/*---------------------------------------------------------------------------
 *	Greenplum Database
 *	Copyright (c) 2004-2015 Pivotal Software, Inc.
 *
 *	@filename:
 *		_api.h
 *
 *	@doc:
 *		SPQOS wrapper interface for SPQDB.
 *
 *	@owner:
 *
 *	@test:
 *
 *
 *---------------------------------------------------------------------------*/
#ifndef SPQOS_api_H
#define SPQOS_api_H

#ifndef USE_CMAKE
#include "pg_config.h"
#endif
#include "spqos/base.h"

#ifdef __cplusplus

// lookup given exception type in the given array
spqos::BOOL FoundException(spqos::CException &exc, const spqos::ULONG *exceptions,
						  spqos::ULONG size);

// Check if given exception is an unexpected reason for failing to
// produce a plan
spqos::BOOL IsLoggableFailure(spqos::CException &exc);

// check if given exception should error out
spqos::BOOL ShouldErrorOut(spqos::CException &exc);


extern "C" {
#include <stddef.h>
#endif /* __cplusplus */

/*
 * struct with configuration parameters for task execution;
 * this needs to be in sync with the corresponding structure in optserver.h
 */
struct spqos_exec_params
{
	void *(*func)(void *); /* task function */
	void *arg;			   /* task argument */
	void *result;		   /* task result */
	void *stack_start;	   /* start of current thread's stack */
	char *error_buffer;	   /* buffer used to store error messages */
	int error_buffer_size; /* size of error message buffer */
	bool *abort_requested; /* flag indicating if abort is requested */
};

/* struct containing initialization parameters for spqos */
struct spqos_init_params
{
	bool (*abort_requested)(void); /* callback to report abort requests */
};

/* initialize SPQOS memory pool, worker pool and message repository */
void spqos_init(struct spqos_init_params *params);

/*
 * execute function as a SPQOS task using current thread;
 * return 0 for successful completion, 1 for error
 */
int spqos_exec(spqos_exec_params *params);

/* shutdown SPQOS memory pool, worker pool and message repository */
void spqos_terminate(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SPQOS_api_H */

// EOF
