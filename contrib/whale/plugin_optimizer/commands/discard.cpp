/* -------------------------------------------------------------------------
 *
 * discard.cpp
 *	  The implementation of the DISCARD command
 *
 * Portions Copyright (c) 2020 Huawei Technologies Co.,Ltd.
 * Copyright (c) 1996-2012, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/gausskernel/optimizer/commands/discard.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include "access/xact.h"
#include "catalog/namespace.h"
#include "plugin_commands/async.h"
#include "plugin_commands/discard.h"
#include "plugin_commands/prepare.h"
#include "utils/guc.h"
#include "utils/portal.h"

static void DiscardAll(bool isTopLevel);

/*
 * DISCARD { ALL | TEMP | PLANS }
 */
void DiscardCommand(DiscardStmt* stmt, bool isTopLevel)
{
    switch (stmt->target) {
        case DISCARD_ALL:
            DiscardAll(isTopLevel);
            break;

        case DISCARD_PLANS:
            ResetPlanCache();
            break;

        case DISCARD_TEMP:
            ResetTempTableNamespace();
            break;

        default:
            ereport(ERROR,
                (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE), errmsg("unrecognized DISCARD target: %d", stmt->target)));
    }
}

static void DiscardAll(bool isTopLevel)
{
    /*
     * Disallow DISCARD ALL in a transaction block. This is arguably
     * inconsistent (we don't make a similar check in the command sequence
     * that DISCARD ALL is equivalent to), but the idea is to catch mistakes:
     * DISCARD ALL inside a transaction block would leave the transaction
     * still uncommitted.
     */
    PreventTransactionChain(isTopLevel, "DISCARD ALL");

    /* Closing portals might run user-defined code, so do that first. */
    PortalHashTableDeleteAll();
    SetPGVariable("session_authorization", NIL, false);
    ResetAllOptions();
    DropAllPreparedStatements();
    Async_UnlistenAll();
    LockReleaseAll(USER_LOCKMETHOD, true);
    ResetPlanCache();
    ResetTempTableNamespace();
}
