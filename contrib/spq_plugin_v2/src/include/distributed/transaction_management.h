/*-------------------------------------------------------------------------
 * transaction_management.h
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef TRANSACTION_MANAGMENT_H
#define TRANSACTION_MANAGMENT_H

#include "access/xact.h"
#include "catalog/objectaddress.h"
#include "lib/ilist.h"
#include "lib/stringinfo.h"
#include "nodes/pg_list.h"
#include "nodes/primnodes.h"
#include "utils/hsearch.h"

/* forward declare, to avoid recursive includes */
struct DistObjectCacheEntry;

/* describes what kind of modifications have occurred in the current transaction */
typedef enum {
    XACT_MODIFICATION_INVALID = 0, /* placeholder initial value */
    XACT_MODIFICATION_NONE,        /* no modifications have taken place */
    XACT_MODIFICATION_DATA,        /* data modifications (DML) have occurred */
    XACT_MODIFICATION_MULTI_SHARD  /* multi-shard modifications have occurred */
} XactModificationType;

/*
 * Enum defining the state of a coordinated (i.e. a transaction potentially
 * spanning several nodes).
 */
typedef enum CoordinatedTransactionState {
    /* no coordinated transaction in progress, no connections established */
    COORD_TRANS_NONE,

    /* no coordinated transaction in progress, but connections established */
    COORD_TRANS_IDLE,

    /* coordinated transaction in progress */
    COORD_TRANS_STARTED,

    /* coordinated transaction prepared on all workers */
    COORD_TRANS_PREPARED,

    /* coordinated transaction committed */
    COORD_TRANS_COMMITTED
} CoordinatedTransactionState;

/* Enumeration to keep track of context within nested sub-transactions */
typedef struct SubXactContext {
    SubTransactionId subId;
    StringInfo setLocalCmds;
    HTAB* propagatedObjects;
} SubXactContext;

/*
 * Function delegated with force_delegation call enforces the distribution argument
 * along with the colocationId. The latter one is equally important to not allow
 * the same partition key value into another distributed table which is not co-located
 * and therefore might be on a different node.
 */
typedef struct AllowedDistributionColumn {
    Const* distributionColumnValue;
    uint32 colocationId;
    bool isActive;

    /* In nested executor, track the level at which value is set */
    int executorLevel;
} AllowedDistributionColumn;

/*
 * BeginXactDeferrableState reflects the value of the DEFERRABLE property
 * in the BEGIN of a transaction block.
 */
typedef enum BeginXactDeferrableState {
    BeginXactDeferrable_NotSet,
    BeginXactDeferrable_Disabled,
    BeginXactDeferrable_Enabled,
} BeginXactDeferrableState;

/*
 * BeginXactReadOnlyState reflects the value of the READ ONLY property
 * in the BEGIN of a transaction block.
 */
typedef enum BeginXactReadOnlyState {
    BeginXactReadOnly_NotSet,
    BeginXactReadOnly_Disabled,
    BeginXactReadOnly_Enabled,
} BeginXactReadOnlyState;

struct SessionTransactionCtx {

    CoordinatedTransactionState CurrentCoordinatedTransactionState{COORD_TRANS_NONE};

    /* number of nested stored procedure call levels we are currently in */
    int StoredProcedureLevel{0};

    /* number of nested DO block levels we are currently in */
    int DoBlockLevel{0};

    /* state needed to keep track of operations used during a transaction */
    XactModificationType XactModificationLevel{XACT_MODIFICATION_NONE};

    /* list of connections that are part of the current coordinated transaction */
    dlist_head InProgressTransactions{};

    /*
     * activeSetStmts keeps track of SET LOCAL statements executed within the current
     * subxact and will be set to NULL when pushing into new subxact or ending top xact.
     */
    StringInfo activeSetStmts{nullptr};

    /*
     * Though a list, we treat this as a stack, pushing on subxact contexts whenever
     * e.g. a SAVEPOINT is executed (though this is actually performed by providing
     * PostgreSQL with a sub-xact callback). At present, the context of a subxact
     * includes
     *  - a subxact identifier,
     *  - any SET LOCAL statements propagated to workers during the sub-transaction,
     *  - all objects propagated to workers during the sub-transaction.
     *
     * To be clear, last item of activeSubXactContexts list corresponds to top of
     * stack.
     */
    List* activeSubXactContexts{NIL};

    /*
     * PropagatedObjectsInTx is a set of objects propagated in the root transaction.
     * We also keep track of objects propagated in sub-transactions in
     * activeSubXactContexts. Any committed sub-transaction would cause the objects, which
     * are propagated during the sub-transaction, to be moved to upper transaction's set.
     * Objects are discarded when the sub-transaction is aborted.
     */
    HTAB* PropagatedObjectsInTx{nullptr};

    /* some pre-allocated memory so we don't need to call malloc() during callbacks */
    MemoryContext CitusXactCallbackContext{nullptr};

    /*
     * Should this coordinated transaction use 2PC? Set by
     * CoordinatedTransactionUse2PC(), e.g. if any modification
     * is issued and us 2PC. But, even if this flag is set,
     * the transaction manager is smart enough to only
     * do 2PC on the remote connections that did a modification.
     *
     * As a variable name ShouldCoordinatedTransactionUse2PC could
     * be improved. We use Use2PCForCoordinatedTransaction() as the
     * public API function, hence couldn't come up with a better name
     * for the underlying variable at the moment.
     */
    bool ShouldCoordinatedTransactionUse2PC{false};

    /*
     * Distribution function argument (along with colocationId) when delegated
     * using forceDelegation flag.
     */
    AllowedDistributionColumn AllowedDistributionColumnValue{};

    /* if true, we should trigger node metadata sync on commit */
    bool NodeMetadataSyncOnCommit{false};

    /*
     * In an explicit BEGIN ...; we keep track of top-level transaction characteristics
     * specified by the user.
     */
    BeginXactReadOnlyState BeginXactReadOnly{BeginXactReadOnly_NotSet};

    BeginXactDeferrableState BeginXactDeferrable{BeginXactDeferrable_NotSet};

    /* did current transaction modify pg_dist_node? */
    bool TransactionModifiedNodeMetadata{false};

    /* Variable to determine if we are in the process of creating citus */
    int CreateCitusTransactionLevel{0};

    void InitializeTransCtx();
};

/*
 * Coordinated transaction management.
 */
extern void UseCoordinatedTransaction(void);
extern bool InCoordinatedTransaction(void);
extern void Use2PCForCoordinatedTransaction(void);
extern bool GetCoordinatedTransactionShouldUse2PC(void);
extern bool IsMultiStatementTransaction(void);
extern void EnsureDistributedTransactionId(void);
extern bool MaybeExecutingUDF(void);

/* functions for tracking the objects propagated in current transaction */
extern void TrackPropagatedObject(const ObjectAddress* objectAddress);
extern void TrackPropagatedTableAndSequences(Oid relationId);
extern void ResetPropagatedObjects(void);
extern bool HasAnyDependencyInPropagatedObjects(const ObjectAddress* objectAddress);

/* initialization function(s) */
extern void InitializeTransactionManagement(void);

/* other functions */
extern List* ActiveSubXactContexts(void);
extern StringInfo BeginAndSetDistributedTransactionIdCommand(void);
extern void TriggerNodeMetadataSyncOnCommit(void);

#endif /*  TRANSACTION_MANAGMENT_H */
