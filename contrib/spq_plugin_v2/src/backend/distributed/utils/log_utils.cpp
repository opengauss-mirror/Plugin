/*-------------------------------------------------------------------------
 *
 * log_utils.c
 *	  Utilities regarding logs
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#ifdef DISABLE_OG_COMMENTS
#include "common/cryptohash.h"
#include "common/sha2.h"
#endif
#include "utils/builtins.h"
#include "utils/guc.h"

#include "pg_version_constants.h"

#include "distributed/errormessage.h"
#include "distributed/log_utils.h"

/*
 * IsLoggableLevel returns true if either of client or server log guc is configured to
 * log the given log level.
 * In postgres, log can be configured differently for clients and servers.
 */
bool IsLoggableLevel(int logLevel)
{
    return log_min_messages <= logLevel || client_min_messages <= logLevel;
}
