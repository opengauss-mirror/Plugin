// #include "tsdb.h"
// #include "tsdb_head.h"
// //first
// HeapScanDesc
// heap_beginscan_catalog(Relation relation, int nkeys, ScanKey key)
// {
// 	Oid			relid = RelationGetRelid(relation);
// 	Snapshot	snapshot = RegisterSnapshot(GetCatalogSnapshot(relid));

// 	return heap_beginscan_internal(relation, snapshot, nkeys, key, NULL,
// 								   true, true, true, false, false, true);
// }
// void
// heap_setscanlimits(HeapScanDesc scan, BlockNumber startBlk, BlockNumber numBlks)
// {
// 	Assert(!scan->rs_inited);	/* else too late to change */
// 	Assert(!scan->rs_syncscan); /* else rs_startblock is significant */

// 	/* Check startBlk is valid (but allow case of zero blocks...) */
// 	Assert(startBlk == 0 || startBlk < scan->rs_nblocks);

// 	scan->rs_startblock = startBlk;
// 	scan->rs_numblocks = numBlks;
// }
// void
// heap_rescan_set_params(HeapScanDesc scan, ScanKey key,
// 					   bool allow_strat, bool allow_sync, bool allow_pagemode)
// {
// 	/* adjust parameters */
// 	scan->rs_allow_strat = allow_strat;
// 	scan->rs_allow_sync = allow_sync;
// 	scan->rs_pageatatime = allow_pagemode && IsMVCCSnapshot(scan->rs_snapshot);
// 	/* ... and rescan */
// 	heap_rescan(scan, key);
// }
// Size
// heap_parallelscan_estimate(Snapshot snapshot)
// {
// 	return add_size(offsetof(ParallelHeapScanDescData, phs_snapshot_data),
// 					EstimateSnapshotSpace(snapshot));
// }
// void
// heap_parallelscan_initialize(ParallelHeapScanDesc target, Relation relation,
// 							 Snapshot snapshot)
// {
// 	target->phs_relid = RelationGetRelid(relation);
// 	target->phs_nblocks = RelationGetNumberOfBlocks(relation);
// 	/* compare phs_syncscan initialization to similar logic in initscan */
// 	target->phs_syncscan = synchronize_seqscans &&
// 		!RelationUsesLocalBuffers(relation) &&
// 		target->phs_nblocks > NBuffers / 4;
// 	SpinLockInit(&target->phs_mutex);
// 	target->phs_cblock = InvalidBlockNumber;
// 	target->phs_startblock = InvalidBlockNumber;
// 	SerializeSnapshot(snapshot, target->phs_snapshot_data);
// }
// HeapScanDesc
// heap_beginscan_parallel(Relation relation, ParallelHeapScanDesc parallel_scan)
// {
// 	Snapshot	snapshot;

// 	Assert(RelationGetRelid(relation) == parallel_scan->phs_relid);
// 	snapshot = RestoreSnapshot(parallel_scan->phs_snapshot_data);
// 	RegisterSnapshot(snapshot);

// 	return heap_beginscan_internal(relation, snapshot, 0, NULL, parallel_scan,
// 								   true, true, true, false, false, true);
// }
// bool
// heap_tuple_needs_eventual_freeze(HeapTupleHeader tuple)
// {
// 	TransactionId xid;

// 	/*
// 	 * If xmin is a normal transaction ID, this tuple is definitely not
// 	 * frozen.
// 	 */
// 	xid = HeapTupleHeaderGetXmin(tuple);
// 	if (TransactionIdIsNormal(xid))
// 		return true;

// 	/*
// 	 * If xmax is a valid xact or multixact, this tuple is also not frozen.
// 	 */
// 	if (tuple->t_infomask & HEAP_XMAX_IS_MULTI)
// 	{
// 		MultiXactId multi;

// 		multi = HeapTupleHeaderGetRawXmax(tuple);
// 		if (MultiXactIdIsValid(multi))
// 			return true;
// 	}
// 	else
// 	{
// 		xid = HeapTupleHeaderGetRawXmax(tuple);
// 		if (TransactionIdIsNormal(xid))
// 			return true;
// 	}

// 	if (tuple->t_infomask & HEAP_MOVED)
// 	{
// 		xid = HeapTupleHeaderGetXvac(tuple);
// 		if (TransactionIdIsNormal(xid))
// 			return true;
// 	}

// 	return false;
// }
// HeapScanDesc
// heap_beginscan(Relation relation, Snapshot snapshot,
// 			   int nkeys, ScanKey key)
// {
// 	return heap_beginscan_internal(relation, snapshot, nkeys, key, NULL,
// 								   true, true, true, false, false, false);
// }
// HeapScanDesc
// heap_beginscan_strat(Relation relation, Snapshot snapshot,
// 					 int nkeys, ScanKey key,
// 					 bool allow_strat, bool allow_sync)
// {
// 	return heap_beginscan_internal(relation, snapshot, nkeys, key, NULL,
// 								   allow_strat, allow_sync, true,
// 								   false, false, false);
// }
// HeapScanDesc
// heap_beginscan_bm(Relation relation, Snapshot snapshot,
// 				  int nkeys, ScanKey key)
// {
// 	return heap_beginscan_internal(relation, snapshot, nkeys, key, NULL,
// 								   false, false, true, true, false, false);
// }
// HeapScanDesc
// heap_beginscan_sampling(Relation relation, Snapshot snapshot,
// 						int nkeys, ScanKey key,
// 					  bool allow_strat, bool allow_sync, bool allow_pagemode)
// {
// 	return heap_beginscan_internal(relation, snapshot, nkeys, key, NULL,
// 								   allow_strat, allow_sync, allow_pagemode,
// 								   false, true, false);
// }
// HTSU_Result
// heap_delete(Relation relation, ItemPointer tid,
// 			CommandId cid, Snapshot crosscheck, bool wait,
// 			HeapUpdateFailureData *hufd)
// {
// 	HTSU_Result result;
// 	TransactionId xid = GetCurrentTransactionId();
// 	ItemId		lp;
// 	HeapTupleData tp;
// 	Page		page;
// 	BlockNumber block;
// 	Buffer		buffer;
// 	Buffer		vmbuffer = InvalidBuffer;
// 	TransactionId new_xmax;
// 	uint16		new_infomask,
// 				new_infomask2;
// 	bool		have_tuple_lock = false;
// 	bool		iscombo;
// 	bool		all_visible_cleared = false;
// 	HeapTuple	old_key_tuple = NULL;	/* replica identity of the tuple */
// 	bool		old_key_copied = false;

// 	Assert(ItemPointerIsValid(tid));

// 	/*
// 	 * Forbid this during a parallel operation, lest it allocate a combocid.
// 	 * Other workers might need that combocid for visibility checks, and we
// 	 * have no provision for broadcasting it to them.
// 	 */
// 	if (IsInParallelMode())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_TRANSACTION_STATE),
// 				 errmsg("cannot delete tuples during a parallel operation")));

// 	block = ItemPointerGetBlockNumber(tid);
// 	buffer = ReadBuffer(relation, block);
// 	page = BufferGetPage(buffer);

// 	/*
// 	 * Before locking the buffer, pin the visibility map page if it appears to
// 	 * be necessary.  Since we haven't got the lock yet, someone else might be
// 	 * in the middle of changing this, so we'll need to recheck after we have
// 	 * the lock.
// 	 */
// 	if (PageIsAllVisible(page))
// 		visibilitymap_pin(relation, block, &vmbuffer);

// 	LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

// 	/*
// 	 * If we didn't pin the visibility map page and the page has become all
// 	 * visible while we were busy locking the buffer, we'll have to unlock and
// 	 * re-lock, to avoid holding the buffer lock across an I/O.  That's a bit
// 	 * unfortunate, but hopefully shouldn't happen often.
// 	 */
// 	if (vmbuffer == InvalidBuffer && PageIsAllVisible(page))
// 	{
// 		LockBuffer(buffer, BUFFER_LOCK_UNLOCK);
// 		visibilitymap_pin(relation, block, &vmbuffer);
// 		LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);
// 	}

// 	lp = PageGetItemId(page, ItemPointerGetOffsetNumber(tid));
// 	Assert(ItemIdIsNormal(lp));

// 	tp.t_tableOid = RelationGetRelid(relation);
// 	tp.t_data = (HeapTupleHeader) PageGetItem(page, lp);
// 	tp.t_len = ItemIdGetLength(lp);
// 	tp.t_self = *tid;

// l1:
// 	result = HeapTupleSatisfiesUpdate(&tp, cid, buffer);

// 	if (result == HeapTupleInvisible)
// 	{
// 		UnlockReleaseBuffer(buffer);
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("attempted to delete invisible tuple")));
// 	}
// 	else if (result == HeapTupleBeingUpdated && wait)
// 	{
// 		TransactionId xwait;
// 		uint16		infomask;

// 		/* must copy state data before unlocking buffer */
// 		xwait = HeapTupleHeaderGetRawXmax(tp.t_data);
// 		infomask = tp.t_data->t_infomask;

// 		/*
// 		 * Sleep until concurrent transaction ends -- except when there's a
// 		 * single locker and it's our own transaction.  Note we don't care
// 		 * which lock mode the locker has, because we need the strongest one.
// 		 *
// 		 * Before sleeping, we need to acquire tuple lock to establish our
// 		 * priority for the tuple (see heap_lock_tuple).  LockTuple will
// 		 * release us when we are next-in-line for the tuple.
// 		 *
// 		 * If we are forced to "start over" below, we keep the tuple lock;
// 		 * this arranges that we stay at the head of the line while rechecking
// 		 * tuple state.
// 		 */
// 		if (infomask & HEAP_XMAX_IS_MULTI)
// 		{
// 			/* wait for multixact */
// 			if (DoesMultiXactIdConflict((MultiXactId) xwait, infomask,
// 										LockTupleExclusive))
// 			{
// 				LockBuffer(buffer, BUFFER_LOCK_UNLOCK);

// 				/* acquire tuple lock, if necessary */
// 				heap_acquire_tuplock(relation, &(tp.t_self), LockTupleExclusive,
// 									 LockWaitBlock, &have_tuple_lock);

// 				/* wait for multixact */
// 				MultiXactIdWait((MultiXactId) xwait, MultiXactStatusUpdate, infomask,
// 								relation, &(tp.t_self), XLTW_Delete,
// 								NULL);
// 				LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

// 				/*
// 				 * If xwait had just locked the tuple then some other xact
// 				 * could update this tuple before we get to this point.  Check
// 				 * for xmax change, and start over if so.
// 				 */
// 				if (xmax_infomask_changed(tp.t_data->t_infomask, infomask) ||
// 					!TransactionIdEquals(HeapTupleHeaderGetRawXmax(tp.t_data),
// 										 xwait))
// 					goto l1;
// 			}

// 			/*
// 			 * You might think the multixact is necessarily done here, but not
// 			 * so: it could have surviving members, namely our own xact or
// 			 * other subxacts of this backend.  It is legal for us to delete
// 			 * the tuple in either case, however (the latter case is
// 			 * essentially a situation of upgrading our former shared lock to
// 			 * exclusive).  We don't bother changing the on-disk hint bits
// 			 * since we are about to overwrite the xmax altogether.
// 			 */
// 		}
// 		else if (!TransactionIdIsCurrentTransactionId(xwait))
// 		{
// 			/*
// 			 * Wait for regular transaction to end; but first, acquire tuple
// 			 * lock.
// 			 */
// 			LockBuffer(buffer, BUFFER_LOCK_UNLOCK);
// 			heap_acquire_tuplock(relation, &(tp.t_self), LockTupleExclusive,
// 								 LockWaitBlock, &have_tuple_lock);
// 			XactLockTableWait(xwait, relation, &(tp.t_self), XLTW_Delete);
// 			LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

// 			/*
// 			 * xwait is done, but if xwait had just locked the tuple then some
// 			 * other xact could update this tuple before we get to this point.
// 			 * Check for xmax change, and start over if so.
// 			 */
// 			if (xmax_infomask_changed(tp.t_data->t_infomask, infomask) ||
// 				!TransactionIdEquals(HeapTupleHeaderGetRawXmax(tp.t_data),
// 									 xwait))
// 				goto l1;

// 			/* Otherwise check if it committed or aborted */
// 			UpdateXmaxHintBits(tp.t_data, buffer, xwait);
// 		}

// 		/*
// 		 * We may overwrite if previous xmax aborted, or if it committed but
// 		 * only locked the tuple without updating it.
// 		 */
// 		if ((tp.t_data->t_infomask & HEAP_XMAX_INVALID) ||
// 			HEAP_XMAX_IS_LOCKED_ONLY(tp.t_data->t_infomask) ||
// 			HeapTupleHeaderIsOnlyLocked(tp.t_data))
// 			result = HeapTupleMayBeUpdated;
// 		else
// 			result = HeapTupleUpdated;
// 	}

// 	if (crosscheck != InvalidSnapshot && result == HeapTupleMayBeUpdated)
// 	{
// 		/* Perform additional check for transaction-snapshot mode RI updates */
// 		if (!HeapTupleSatisfiesVisibility(&tp, crosscheck, buffer))
// 			result = HeapTupleUpdated;
// 	}

// 	if (result != HeapTupleMayBeUpdated)
// 	{
// 		Assert(result == HeapTupleSelfUpdated ||
// 			   result == HeapTupleUpdated ||
// 			   result == HeapTupleBeingUpdated);
// 		Assert(!(tp.t_data->t_infomask & HEAP_XMAX_INVALID));
// 		hufd->ctid = tp.t_data->t_ctid;
// 		hufd->xmax = HeapTupleHeaderGetUpdateXid(tp.t_data);
// 		if (result == HeapTupleSelfUpdated)
// 			hufd->cmax = HeapTupleHeaderGetCmax(tp.t_data);
// 		else
// 			hufd->cmax = InvalidCommandId;
// 		UnlockReleaseBuffer(buffer);
// 		if (have_tuple_lock)
// 			UnlockTupleTuplock(relation, &(tp.t_self), LockTupleExclusive);
// 		if (vmbuffer != InvalidBuffer)
// 			ReleaseBuffer(vmbuffer);
// 		return result;
// 	}

// 	/*
// 	 * We're about to do the actual delete -- check for conflict first, to
// 	 * avoid possibly having to roll back work we've just done.
// 	 *
// 	 * This is safe without a recheck as long as there is no possibility of
// 	 * another process scanning the page between this check and the delete
// 	 * being visible to the scan (i.e., an exclusive buffer content lock is
// 	 * continuously held from this point until the tuple delete is visible).
// 	 */
// 	CheckForSerializableConflictIn(relation, &tp, buffer);

// 	/* replace cid with a combo cid if necessary */
// 	HeapTupleHeaderAdjustCmax(tp.t_data, &cid, &iscombo);

// 	/*
// 	 * Compute replica identity tuple before entering the critical section so
// 	 * we don't PANIC upon a memory allocation failure.
// 	 */
// 	old_key_tuple = ExtractReplicaIdentity(relation, &tp, true, &old_key_copied);

// 	/*
// 	 * If this is the first possibly-multixact-able operation in the current
// 	 * transaction, set my per-backend OldestMemberMXactId setting. We can be
// 	 * certain that the transaction will never become a member of any older
// 	 * MultiXactIds than that.  (We have to do this even if we end up just
// 	 * using our own TransactionId below, since some other backend could
// 	 * incorporate our XID into a MultiXact immediately afterwards.)
// 	 */
// 	MultiXactIdSetOldestMember();

// 	compute_new_xmax_infomask(HeapTupleHeaderGetRawXmax(tp.t_data),
// 							  tp.t_data->t_infomask, tp.t_data->t_infomask2,
// 							  xid, LockTupleExclusive, true,
// 							  &new_xmax, &new_infomask, &new_infomask2);

// 	START_CRIT_SECTION();

// 	/*
// 	 * If this transaction commits, the tuple will become DEAD sooner or
// 	 * later.  Set flag that this page is a candidate for pruning once our xid
// 	 * falls below the OldestXmin horizon.  If the transaction finally aborts,
// 	 * the subsequent page pruning will be a no-op and the hint will be
// 	 * cleared.
// 	 */
// 	PageSetPrunable(page, xid);

// 	if (PageIsAllVisible(page))
// 	{
// 		all_visible_cleared = true;
// 		PageClearAllVisible(page);
// 		visibilitymap_clear(relation, BufferGetBlockNumber(buffer),
// 							vmbuffer, VISIBILITYMAP_VALID_BITS);
// 	}

// 	/* store transaction information of xact deleting the tuple */
// 	tp.t_data->t_infomask &= ~(HEAP_XMAX_BITS | HEAP_MOVED);
// 	tp.t_data->t_infomask2 &= ~HEAP_KEYS_UPDATED;
// 	tp.t_data->t_infomask |= new_infomask;
// 	tp.t_data->t_infomask2 |= new_infomask2;
// 	HeapTupleHeaderClearHotUpdated(tp.t_data);
// 	HeapTupleHeaderSetXmax(tp.t_data, new_xmax);
// 	HeapTupleHeaderSetCmax(tp.t_data, cid, iscombo);
// 	/* Make sure there is no forward chain link in t_ctid */
// 	tp.t_data->t_ctid = tp.t_self;

// 	MarkBufferDirty(buffer);

// 	/*
// 	 * XLOG stuff
// 	 *
// 	 * NB: heap_abort_speculative() uses the same xlog record and replay
// 	 * routines.
// 	 */
// 	if (RelationNeedsWAL(relation))
// 	{
// 		xl_heap_delete xlrec;
// 		XLogRecPtr	recptr;

// 		/* For logical decode we need combocids to properly decode the catalog */
// 		if (RelationIsAccessibleInLogicalDecoding(relation))
// 			log_heap_new_cid(relation, &tp);

// 		xlrec.flags = all_visible_cleared ? XLH_DELETE_ALL_VISIBLE_CLEARED : 0;
// 		xlrec.infobits_set = compute_infobits(tp.t_data->t_infomask,
// 											  tp.t_data->t_infomask2);
// 		xlrec.offnum = ItemPointerGetOffsetNumber(&tp.t_self);
// 		xlrec.xmax = new_xmax;

// 		if (old_key_tuple != NULL)
// 		{
// 			if (relation->rd_rel->relreplident == REPLICA_IDENTITY_FULL)
// 				xlrec.flags |= XLH_DELETE_CONTAINS_OLD_TUPLE;
// 			else
// 				xlrec.flags |= XLH_DELETE_CONTAINS_OLD_KEY;
// 		}

// 		XLogBeginInsert();
// 		XLogRegisterData((char *) &xlrec, SizeOfHeapDelete);

// 		XLogRegisterBuffer(0, buffer, REGBUF_STANDARD);

// 		/*
// 		 * Log replica identity of the deleted tuple if there is one
// 		 */
// 		if (old_key_tuple != NULL)
// 		{
// 			xl_heap_header xlhdr;

// 			xlhdr.t_infomask2 = old_key_tuple->t_data->t_infomask2;
// 			xlhdr.t_infomask = old_key_tuple->t_data->t_infomask;
// 			xlhdr.t_hoff = old_key_tuple->t_data->t_hoff;

// 			XLogRegisterData((char *) &xlhdr, SizeOfHeapHeader);
// 			XLogRegisterData((char *) old_key_tuple->t_data
// 							 + SizeofHeapTupleHeader,
// 							 old_key_tuple->t_len
// 							 - SizeofHeapTupleHeader);
// 		}

// 		/* filtering by origin on a row level is much more efficient */
// 		XLogIncludeOrigin();

// 		recptr = XLogInsert(RM_HEAP_ID, XLOG_HEAP_DELETE);

// 		PageSetLSN(page, recptr);
// 	}

// 	END_CRIT_SECTION();

// 	LockBuffer(buffer, BUFFER_LOCK_UNLOCK);

// 	if (vmbuffer != InvalidBuffer)
// 		ReleaseBuffer(vmbuffer);

// 	/*
// 	 * If the tuple has toasted out-of-line attributes, we need to delete
// 	 * those items too.  We have to do this before releasing the buffer
// 	 * because we need to look at the contents of the tuple, but it's OK to
// 	 * release the content lock on the buffer first.
// 	 */
// 	if (relation->rd_rel->relkind != RELKIND_RELATION &&
// 		relation->rd_rel->relkind != RELKIND_MATVIEW)
// 	{
// 		/* toast table entries should never be recursively toasted */
// 		Assert(!HeapTupleHasExternal(&tp));
// 	}
// 	else if (HeapTupleHasExternal(&tp))
// 		toast_delete(relation, &tp, false);

// 	/*
// 	 * Mark tuple for invalidation from system caches at next command
// 	 * boundary. We have to do this before releasing the buffer because we
// 	 * need to look at the contents of the tuple.
// 	 */
// 	CacheInvalidateHeapTuple(relation, &tp, NULL);

// 	/* Now we can release the buffer */
// 	ReleaseBuffer(buffer);

// 	/*
// 	 * Release the lmgr tuple lock, if we had it.
// 	 */
// 	if (have_tuple_lock)
// 		UnlockTupleTuplock(relation, &(tp.t_self), LockTupleExclusive);

// 	pgstat_count_heap_delete(relation);

// 	if (old_key_tuple != NULL && old_key_copied)
// 		heap_freetuple(old_key_tuple);

// 	return HeapTupleMayBeUpdated;
// }
// HTSU_Result
// heap_update(Relation relation, ItemPointer otid, HeapTuple newtup,
// 			CommandId cid, Snapshot crosscheck, bool wait,
// 			HeapUpdateFailureData *hufd, LockTupleMode *lockmode)
// {
// 	HTSU_Result result;
// 	TransactionId xid = GetCurrentTransactionId();
// 	Bitmapset  *hot_attrs;
// 	Bitmapset  *key_attrs;
// 	Bitmapset  *id_attrs;
// 	ItemId		lp;
// 	HeapTupleData oldtup;
// 	HeapTuple	heaptup;
// 	HeapTuple	old_key_tuple = NULL;
// 	bool		old_key_copied = false;
// 	Page		page;
// 	BlockNumber block;
// 	MultiXactStatus mxact_status;
// 	Buffer		buffer,
// 				newbuf,
// 				vmbuffer = InvalidBuffer,
// 				vmbuffer_new = InvalidBuffer;
// 	bool		need_toast;
// 	Size		newtupsize,
// 				pagefree;
// 	bool		have_tuple_lock = false;
// 	bool		iscombo;
// 	bool		satisfies_hot;
// 	bool		satisfies_key;
// 	bool		satisfies_id;
// 	bool		use_hot_update = false;
// 	bool		key_intact;
// 	bool		all_visible_cleared = false;
// 	bool		all_visible_cleared_new = false;
// 	bool		checked_lockers;
// 	bool		locker_remains;
// 	TransactionId xmax_new_tuple,
// 				xmax_old_tuple;
// 	uint16		infomask_old_tuple,
// 				infomask2_old_tuple,
// 				infomask_new_tuple,
// 				infomask2_new_tuple;

// 	Assert(ItemPointerIsValid(otid));

// 	/*
// 	 * Forbid this during a parallel operation, lest it allocate a combocid.
// 	 * Other workers might need that combocid for visibility checks, and we
// 	 * have no provision for broadcasting it to them.
// 	 */
// 	if (IsInParallelMode())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_TRANSACTION_STATE),
// 				 errmsg("cannot update tuples during a parallel operation")));

// 	/*
// 	 * Fetch the list of attributes to be checked for HOT update.  This is
// 	 * wasted effort if we fail to update or have to put the new tuple on a
// 	 * different page.  But we must compute the list before obtaining buffer
// 	 * lock --- in the worst case, if we are doing an update on one of the
// 	 * relevant system catalogs, we could deadlock if we try to fetch the list
// 	 * later.  In any case, the relcache caches the data so this is usually
// 	 * pretty cheap.
// 	 *
// 	 * Note that we get a copy here, so we need not worry about relcache flush
// 	 * happening midway through.
// 	 */
// 	hot_attrs = RelationGetIndexAttrBitmap(relation, INDEX_ATTR_BITMAP_ALL);
// 	key_attrs = RelationGetIndexAttrBitmap(relation, INDEX_ATTR_BITMAP_KEY);
// 	id_attrs = RelationGetIndexAttrBitmap(relation,
// 										  INDEX_ATTR_BITMAP_IDENTITY_KEY);

// 	block = ItemPointerGetBlockNumber(otid);
// 	buffer = ReadBuffer(relation, block);
// 	page = BufferGetPage(buffer);

// 	/*
// 	 * Before locking the buffer, pin the visibility map page if it appears to
// 	 * be necessary.  Since we haven't got the lock yet, someone else might be
// 	 * in the middle of changing this, so we'll need to recheck after we have
// 	 * the lock.
// 	 */
// 	if (PageIsAllVisible(page))
// 		visibilitymap_pin(relation, block, &vmbuffer);

// 	LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

// 	lp = PageGetItemId(page, ItemPointerGetOffsetNumber(otid));
// 	Assert(ItemIdIsNormal(lp));

// 	/*
// 	 * Fill in enough data in oldtup for HeapSatisfiesHOTandKeyUpdate to work
// 	 * properly.
// 	 */
// 	oldtup.t_tableOid = RelationGetRelid(relation);
// 	oldtup.t_data = (HeapTupleHeader) PageGetItem(page, lp);
// 	oldtup.t_len = ItemIdGetLength(lp);
// 	oldtup.t_self = *otid;

// 	/* the new tuple is ready, except for this: */
// 	newtup->t_tableOid = RelationGetRelid(relation);

// 	/* Fill in OID for newtup */
// 	if (relation->rd_rel->relhasoids)
// 	{
// #ifdef NOT_USED
// 		/* this is redundant with an Assert in HeapTupleSetOid */
// 		Assert(newtup->t_data->t_infomask & HEAP_HASOID);
// #endif
// 		HeapTupleSetOid(newtup, HeapTupleGetOid(&oldtup));
// 	}
// 	else
// 	{
// 		/* check there is not space for an OID */
// 		Assert(!(newtup->t_data->t_infomask & HEAP_HASOID));
// 	}

// 	/*
// 	 * If we're not updating any "key" column, we can grab a weaker lock type.
// 	 * This allows for more concurrency when we are running simultaneously
// 	 * with foreign key checks.
// 	 *
// 	 * Note that if a column gets detoasted while executing the update, but
// 	 * the value ends up being the same, this test will fail and we will use
// 	 * the stronger lock.  This is acceptable; the important case to optimize
// 	 * is updates that don't manipulate key columns, not those that
// 	 * serendipitiously arrive at the same key values.
// 	 */
// 	HeapSatisfiesHOTandKeyUpdate(relation, hot_attrs, key_attrs, id_attrs,
// 								 &satisfies_hot, &satisfies_key,
// 								 &satisfies_id, &oldtup, newtup);
// 	if (satisfies_key)
// 	{
// 		*lockmode = LockTupleNoKeyExclusive;
// 		mxact_status = MultiXactStatusNoKeyUpdate;
// 		key_intact = true;

// 		/*
// 		 * If this is the first possibly-multixact-able operation in the
// 		 * current transaction, set my per-backend OldestMemberMXactId
// 		 * setting. We can be certain that the transaction will never become a
// 		 * member of any older MultiXactIds than that.  (We have to do this
// 		 * even if we end up just using our own TransactionId below, since
// 		 * some other backend could incorporate our XID into a MultiXact
// 		 * immediately afterwards.)
// 		 */
// 		MultiXactIdSetOldestMember();
// 	}
// 	else
// 	{
// 		*lockmode = LockTupleExclusive;
// 		mxact_status = MultiXactStatusUpdate;
// 		key_intact = false;
// 	}

// 	/*
// 	 * Note: beyond this point, use oldtup not otid to refer to old tuple.
// 	 * otid may very well point at newtup->t_self, which we will overwrite
// 	 * with the new tuple's location, so there's great risk of confusion if we
// 	 * use otid anymore.
// 	 */

// l2:
// 	checked_lockers = false;
// 	locker_remains = false;
// 	result = HeapTupleSatisfiesUpdate(&oldtup, cid, buffer);

// 	/* see below about the "no wait" case */
// 	Assert(result != HeapTupleBeingUpdated || wait);

// 	if (result == HeapTupleInvisible)
// 	{
// 		UnlockReleaseBuffer(buffer);
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("attempted to update invisible tuple")));
// 	}
// 	else if (result == HeapTupleBeingUpdated && wait)
// 	{
// 		TransactionId xwait;
// 		uint16		infomask;
// 		bool		can_continue = false;

// 		/*
// 		 * XXX note that we don't consider the "no wait" case here.  This
// 		 * isn't a problem currently because no caller uses that case, but it
// 		 * should be fixed if such a caller is introduced.  It wasn't a
// 		 * problem previously because this code would always wait, but now
// 		 * that some tuple locks do not conflict with one of the lock modes we
// 		 * use, it is possible that this case is interesting to handle
// 		 * specially.
// 		 *
// 		 * This may cause failures with third-party code that calls
// 		 * heap_update directly.
// 		 */

// 		/* must copy state data before unlocking buffer */
// 		xwait = HeapTupleHeaderGetRawXmax(oldtup.t_data);
// 		infomask = oldtup.t_data->t_infomask;

// 		/*
// 		 * Now we have to do something about the existing locker.  If it's a
// 		 * multi, sleep on it; we might be awakened before it is completely
// 		 * gone (or even not sleep at all in some cases); we need to preserve
// 		 * it as locker, unless it is gone completely.
// 		 *
// 		 * If it's not a multi, we need to check for sleeping conditions
// 		 * before actually going to sleep.  If the update doesn't conflict
// 		 * with the locks, we just continue without sleeping (but making sure
// 		 * it is preserved).
// 		 *
// 		 * Before sleeping, we need to acquire tuple lock to establish our
// 		 * priority for the tuple (see heap_lock_tuple).  LockTuple will
// 		 * release us when we are next-in-line for the tuple.  Note we must
// 		 * not acquire the tuple lock until we're sure we're going to sleep;
// 		 * otherwise we're open for race conditions with other transactions
// 		 * holding the tuple lock which sleep on us.
// 		 *
// 		 * If we are forced to "start over" below, we keep the tuple lock;
// 		 * this arranges that we stay at the head of the line while rechecking
// 		 * tuple state.
// 		 */
// 		if (infomask & HEAP_XMAX_IS_MULTI)
// 		{
// 			TransactionId update_xact;
// 			int			remain;

// 			if (DoesMultiXactIdConflict((MultiXactId) xwait, infomask,
// 										*lockmode))
// 			{
// 				LockBuffer(buffer, BUFFER_LOCK_UNLOCK);

// 				/* acquire tuple lock, if necessary */
// 				heap_acquire_tuplock(relation, &(oldtup.t_self), *lockmode,
// 									 LockWaitBlock, &have_tuple_lock);

// 				/* wait for multixact */
// 				MultiXactIdWait((MultiXactId) xwait, mxact_status, infomask,
// 								relation, &oldtup.t_self, XLTW_Update,
// 								&remain);
// 				checked_lockers = true;
// 				locker_remains = remain != 0;
// 				LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

// 				/*
// 				 * If xwait had just locked the tuple then some other xact
// 				 * could update this tuple before we get to this point.  Check
// 				 * for xmax change, and start over if so.
// 				 */
// 				if (xmax_infomask_changed(oldtup.t_data->t_infomask,
// 										  infomask) ||
// 				!TransactionIdEquals(HeapTupleHeaderGetRawXmax(oldtup.t_data),
// 									 xwait))
// 					goto l2;
// 			}

// 			/*
// 			 * Note that the multixact may not be done by now.  It could have
// 			 * surviving members; our own xact or other subxacts of this
// 			 * backend, and also any other concurrent transaction that locked
// 			 * the tuple with KeyShare if we only got TupleLockUpdate.  If
// 			 * this is the case, we have to be careful to mark the updated
// 			 * tuple with the surviving members in Xmax.
// 			 *
// 			 * Note that there could have been another update in the
// 			 * MultiXact. In that case, we need to check whether it committed
// 			 * or aborted. If it aborted we are safe to update it again;
// 			 * otherwise there is an update conflict, and we have to return
// 			 * HeapTupleUpdated below.
// 			 *
// 			 * In the LockTupleExclusive case, we still need to preserve the
// 			 * surviving members: those would include the tuple locks we had
// 			 * before this one, which are important to keep in case this
// 			 * subxact aborts.
// 			 */
// 			if (!HEAP_XMAX_IS_LOCKED_ONLY(oldtup.t_data->t_infomask))
// 				update_xact = HeapTupleGetUpdateXid(oldtup.t_data);
// 			else
// 				update_xact = InvalidTransactionId;

// 			/*
// 			 * There was no UPDATE in the MultiXact; or it aborted. No
// 			 * TransactionIdIsInProgress() call needed here, since we called
// 			 * MultiXactIdWait() above.
// 			 */
// 			if (!TransactionIdIsValid(update_xact) ||
// 				TransactionIdDidAbort(update_xact))
// 				can_continue = true;
// 		}
// 		else if (TransactionIdIsCurrentTransactionId(xwait))
// 		{
// 			/*
// 			 * The only locker is ourselves; we can avoid grabbing the tuple
// 			 * lock here, but must preserve our locking information.
// 			 */
// 			checked_lockers = true;
// 			locker_remains = true;
// 			can_continue = true;
// 		}
// 		else if (HEAP_XMAX_IS_KEYSHR_LOCKED(infomask) && key_intact)
// 		{
// 			/*
// 			 * If it's just a key-share locker, and we're not changing the key
// 			 * columns, we don't need to wait for it to end; but we need to
// 			 * preserve it as locker.
// 			 */
// 			checked_lockers = true;
// 			locker_remains = true;
// 			can_continue = true;
// 		}
// 		else
// 		{
// 			/*
// 			 * Wait for regular transaction to end; but first, acquire tuple
// 			 * lock.
// 			 */
// 			LockBuffer(buffer, BUFFER_LOCK_UNLOCK);
// 			heap_acquire_tuplock(relation, &(oldtup.t_self), *lockmode,
// 								 LockWaitBlock, &have_tuple_lock);
// 			XactLockTableWait(xwait, relation, &oldtup.t_self,
// 							  XLTW_Update);
// 			checked_lockers = true;
// 			LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

// 			/*
// 			 * xwait is done, but if xwait had just locked the tuple then some
// 			 * other xact could update this tuple before we get to this point.
// 			 * Check for xmax change, and start over if so.
// 			 */
// 			if (xmax_infomask_changed(oldtup.t_data->t_infomask, infomask) ||
// 				!TransactionIdEquals(xwait,
// 								   HeapTupleHeaderGetRawXmax(oldtup.t_data)))
// 				goto l2;

// 			/* Otherwise check if it committed or aborted */
// 			UpdateXmaxHintBits(oldtup.t_data, buffer, xwait);
// 			if (oldtup.t_data->t_infomask & HEAP_XMAX_INVALID)
// 				can_continue = true;
// 		}

// 		result = can_continue ? HeapTupleMayBeUpdated : HeapTupleUpdated;
// 	}

// 	if (crosscheck != InvalidSnapshot && result == HeapTupleMayBeUpdated)
// 	{
// 		/* Perform additional check for transaction-snapshot mode RI updates */
// 		if (!HeapTupleSatisfiesVisibility(&oldtup, crosscheck, buffer))
// 			result = HeapTupleUpdated;
// 	}

// 	if (result != HeapTupleMayBeUpdated)
// 	{
// 		Assert(result == HeapTupleSelfUpdated ||
// 			   result == HeapTupleUpdated ||
// 			   result == HeapTupleBeingUpdated);
// 		Assert(!(oldtup.t_data->t_infomask & HEAP_XMAX_INVALID));
// 		hufd->ctid = oldtup.t_data->t_ctid;
// 		hufd->xmax = HeapTupleHeaderGetUpdateXid(oldtup.t_data);
// 		if (result == HeapTupleSelfUpdated)
// 			hufd->cmax = HeapTupleHeaderGetCmax(oldtup.t_data);
// 		else
// 			hufd->cmax = InvalidCommandId;
// 		UnlockReleaseBuffer(buffer);
// 		if (have_tuple_lock)
// 			UnlockTupleTuplock(relation, &(oldtup.t_self), *lockmode);
// 		if (vmbuffer != InvalidBuffer)
// 			ReleaseBuffer(vmbuffer);
// 		bms_free(hot_attrs);
// 		bms_free(key_attrs);
// 		bms_free(id_attrs);
// 		return result;
// 	}

// 	/*
// 	 * If we didn't pin the visibility map page and the page has become all
// 	 * visible while we were busy locking the buffer, or during some
// 	 * subsequent window during which we had it unlocked, we'll have to unlock
// 	 * and re-lock, to avoid holding the buffer lock across an I/O.  That's a
// 	 * bit unfortunate, especially since we'll now have to recheck whether the
// 	 * tuple has been locked or updated under us, but hopefully it won't
// 	 * happen very often.
// 	 */
// 	if (vmbuffer == InvalidBuffer && PageIsAllVisible(page))
// 	{
// 		LockBuffer(buffer, BUFFER_LOCK_UNLOCK);
// 		visibilitymap_pin(relation, block, &vmbuffer);
// 		LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);
// 		goto l2;
// 	}

// 	/* Fill in transaction status data */

// 	/*
// 	 * If the tuple we're updating is locked, we need to preserve the locking
// 	 * info in the old tuple's Xmax.  Prepare a new Xmax value for this.
// 	 */
// 	compute_new_xmax_infomask(HeapTupleHeaderGetRawXmax(oldtup.t_data),
// 							  oldtup.t_data->t_infomask,
// 							  oldtup.t_data->t_infomask2,
// 							  xid, *lockmode, true,
// 							  &xmax_old_tuple, &infomask_old_tuple,
// 							  &infomask2_old_tuple);

// 	/*
// 	 * And also prepare an Xmax value for the new copy of the tuple.  If there
// 	 * was no xmax previously, or there was one but all lockers are now gone,
// 	 * then use InvalidXid; otherwise, get the xmax from the old tuple.  (In
// 	 * rare cases that might also be InvalidXid and yet not have the
// 	 * HEAP_XMAX_INVALID bit set; that's fine.)
// 	 */
// 	if ((oldtup.t_data->t_infomask & HEAP_XMAX_INVALID) ||
// 		HEAP_LOCKED_UPGRADED(oldtup.t_data->t_infomask) ||
// 		(checked_lockers && !locker_remains))
// 		xmax_new_tuple = InvalidTransactionId;
// 	else
// 		xmax_new_tuple = HeapTupleHeaderGetRawXmax(oldtup.t_data);

// 	if (!TransactionIdIsValid(xmax_new_tuple))
// 	{
// 		infomask_new_tuple = HEAP_XMAX_INVALID;
// 		infomask2_new_tuple = 0;
// 	}
// 	else
// 	{
// 		/*
// 		 * If we found a valid Xmax for the new tuple, then the infomask bits
// 		 * to use on the new tuple depend on what was there on the old one.
// 		 * Note that since we're doing an update, the only possibility is that
// 		 * the lockers had FOR KEY SHARE lock.
// 		 */
// 		if (oldtup.t_data->t_infomask & HEAP_XMAX_IS_MULTI)
// 		{
// 			GetMultiXactIdHintBits(xmax_new_tuple, &infomask_new_tuple,
// 								   &infomask2_new_tuple);
// 		}
// 		else
// 		{
// 			infomask_new_tuple = HEAP_XMAX_KEYSHR_LOCK | HEAP_XMAX_LOCK_ONLY;
// 			infomask2_new_tuple = 0;
// 		}
// 	}

// 	/*
// 	 * Prepare the new tuple with the appropriate initial values of Xmin and
// 	 * Xmax, as well as initial infomask bits as computed above.
// 	 */
// 	newtup->t_data->t_infomask &= ~(HEAP_XACT_MASK);
// 	newtup->t_data->t_infomask2 &= ~(HEAP2_XACT_MASK);
// 	HeapTupleHeaderSetXmin(newtup->t_data, xid);
// 	HeapTupleHeaderSetCmin(newtup->t_data, cid);
// 	newtup->t_data->t_infomask |= HEAP_UPDATED | infomask_new_tuple;
// 	newtup->t_data->t_infomask2 |= infomask2_new_tuple;
// 	HeapTupleHeaderSetXmax(newtup->t_data, xmax_new_tuple);

// 	/*
// 	 * Replace cid with a combo cid if necessary.  Note that we already put
// 	 * the plain cid into the new tuple.
// 	 */
// 	HeapTupleHeaderAdjustCmax(oldtup.t_data, &cid, &iscombo);

// 	/*
// 	 * If the toaster needs to be activated, OR if the new tuple will not fit
// 	 * on the same page as the old, then we need to release the content lock
// 	 * (but not the pin!) on the old tuple's buffer while we are off doing
// 	 * TOAST and/or table-file-extension work.  We must mark the old tuple to
// 	 * show that it's locked, else other processes may try to update it
// 	 * themselves.
// 	 *
// 	 * We need to invoke the toaster if there are already any out-of-line
// 	 * toasted values present, or if the new tuple is over-threshold.
// 	 */
// 	if (relation->rd_rel->relkind != RELKIND_RELATION &&
// 		relation->rd_rel->relkind != RELKIND_MATVIEW)
// 	{
// 		/* toast table entries should never be recursively toasted */
// 		Assert(!HeapTupleHasExternal(&oldtup));
// 		Assert(!HeapTupleHasExternal(newtup));
// 		need_toast = false;
// 	}
// 	else
// 		need_toast = (HeapTupleHasExternal(&oldtup) ||
// 					  HeapTupleHasExternal(newtup) ||
// 					  newtup->t_len > TOAST_TUPLE_THRESHOLD);

// 	pagefree = PageGetHeapFreeSpace(page);

// 	newtupsize = MAXALIGN(newtup->t_len);

// 	if (need_toast || newtupsize > pagefree)
// 	{
// 		TransactionId xmax_lock_old_tuple;
// 		uint16		infomask_lock_old_tuple,
// 					infomask2_lock_old_tuple;
// 		bool		cleared_all_frozen = false;

// 		/*
// 		 * To prevent concurrent sessions from updating the tuple, we have to
// 		 * temporarily mark it locked, while we release the lock.
// 		 *
// 		 * To satisfy the rule that any xid potentially appearing in a buffer
// 		 * written out to disk, we unfortunately have to WAL log this
// 		 * temporary modification.  We can reuse xl_heap_lock for this
// 		 * purpose.  If we crash/error before following through with the
// 		 * actual update, xmax will be of an aborted transaction, allowing
// 		 * other sessions to proceed.
// 		 */

// 		/*
// 		 * Compute xmax / infomask appropriate for locking the tuple. This has
// 		 * to be done separately from the lock, because the potentially
// 		 * created multixact would otherwise be wrong.
// 		 */
// 		compute_new_xmax_infomask(HeapTupleHeaderGetRawXmax(oldtup.t_data),
// 								  oldtup.t_data->t_infomask,
// 								  oldtup.t_data->t_infomask2,
// 								  xid, *lockmode, false,
// 							  &xmax_lock_old_tuple, &infomask_lock_old_tuple,
// 								  &infomask2_lock_old_tuple);

// 		Assert(HEAP_XMAX_IS_LOCKED_ONLY(infomask_lock_old_tuple));

// 		START_CRIT_SECTION();

// 		/* Clear obsolete visibility flags ... */
// 		oldtup.t_data->t_infomask &= ~(HEAP_XMAX_BITS | HEAP_MOVED);
// 		oldtup.t_data->t_infomask2 &= ~HEAP_KEYS_UPDATED;
// 		HeapTupleClearHotUpdated(&oldtup);
// 		/* ... and store info about transaction updating this tuple */
// 		Assert(TransactionIdIsValid(xmax_lock_old_tuple));
// 		HeapTupleHeaderSetXmax(oldtup.t_data, xmax_lock_old_tuple);
// 		oldtup.t_data->t_infomask |= infomask_lock_old_tuple;
// 		oldtup.t_data->t_infomask2 |= infomask2_lock_old_tuple;
// 		HeapTupleHeaderSetCmax(oldtup.t_data, cid, iscombo);

// 		/* temporarily make it look not-updated, but locked */
// 		oldtup.t_data->t_ctid = oldtup.t_self;

// 		/*
// 		 * Clear all-frozen bit on visibility map if needed. We could
// 		 * immediately reset ALL_VISIBLE, but given that the WAL logging
// 		 * overhead would be unchanged, that doesn't seem necessarily
// 		 * worthwhile.
// 		 */
// 		if (PageIsAllVisible(BufferGetPage(buffer)) &&
// 			visibilitymap_clear(relation, block, vmbuffer,
// 								VISIBILITYMAP_ALL_FROZEN))
// 			cleared_all_frozen = true;

// 		MarkBufferDirty(buffer);

// 		if (RelationNeedsWAL(relation))
// 		{
// 			xl_heap_lock xlrec;
// 			XLogRecPtr	recptr;

// 			XLogBeginInsert();
// 			XLogRegisterBuffer(0, buffer, REGBUF_STANDARD);

// 			xlrec.offnum = ItemPointerGetOffsetNumber(&oldtup.t_self);
// 			xlrec.locking_xid = xmax_lock_old_tuple;
// 			xlrec.infobits_set = compute_infobits(oldtup.t_data->t_infomask,
// 												  oldtup.t_data->t_infomask2);
// 			xlrec.flags =
// 				cleared_all_frozen ? XLH_LOCK_ALL_FROZEN_CLEARED : 0;
// 			XLogRegisterData((char *) &xlrec, SizeOfHeapLock);
// 			recptr = XLogInsert(RM_HEAP_ID, XLOG_HEAP_LOCK);
// 			PageSetLSN(page, recptr);
// 		}

// 		END_CRIT_SECTION();

// 		LockBuffer(buffer, BUFFER_LOCK_UNLOCK);

// 		/*
// 		 * Let the toaster do its thing, if needed.
// 		 *
// 		 * Note: below this point, heaptup is the data we actually intend to
// 		 * store into the relation; newtup is the caller's original untoasted
// 		 * data.
// 		 */
// 		if (need_toast)
// 		{
// 			/* Note we always use WAL and FSM during updates */
// 			heaptup = toast_insert_or_update(relation, newtup, &oldtup, 0);
// 			newtupsize = MAXALIGN(heaptup->t_len);
// 		}
// 		else
// 			heaptup = newtup;

// 		/*
// 		 * Now, do we need a new page for the tuple, or not?  This is a bit
// 		 * tricky since someone else could have added tuples to the page while
// 		 * we weren't looking.  We have to recheck the available space after
// 		 * reacquiring the buffer lock.  But don't bother to do that if the
// 		 * former amount of free space is still not enough; it's unlikely
// 		 * there's more free now than before.
// 		 *
// 		 * What's more, if we need to get a new page, we will need to acquire
// 		 * buffer locks on both old and new pages.  To avoid deadlock against
// 		 * some other backend trying to get the same two locks in the other
// 		 * order, we must be consistent about the order we get the locks in.
// 		 * We use the rule "lock the lower-numbered page of the relation
// 		 * first".  To implement this, we must do RelationGetBufferForTuple
// 		 * while not holding the lock on the old page, and we must rely on it
// 		 * to get the locks on both pages in the correct order.
// 		 */
// 		if (newtupsize > pagefree)
// 		{
// 			/* Assume there's no chance to put heaptup on same page. */
// 			newbuf = RelationGetBufferForTuple(relation, heaptup->t_len,
// 											   buffer, 0, NULL,
// 											   &vmbuffer_new, &vmbuffer);
// 		}
// 		else
// 		{
// 			/* Re-acquire the lock on the old tuple's page. */
// 			LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);
// 			/* Re-check using the up-to-date free space */
// 			pagefree = PageGetHeapFreeSpace(page);
// 			if (newtupsize > pagefree)
// 			{
// 				/*
// 				 * Rats, it doesn't fit anymore.  We must now unlock and
// 				 * relock to avoid deadlock.  Fortunately, this path should
// 				 * seldom be taken.
// 				 */
// 				LockBuffer(buffer, BUFFER_LOCK_UNLOCK);
// 				newbuf = RelationGetBufferForTuple(relation, heaptup->t_len,
// 												   buffer, 0, NULL,
// 												   &vmbuffer_new, &vmbuffer);
// 			}
// 			else
// 			{
// 				/* OK, it fits here, so we're done. */
// 				newbuf = buffer;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		/* No TOAST work needed, and it'll fit on same page */
// 		newbuf = buffer;
// 		heaptup = newtup;
// 	}

// 	/*
// 	 * We're about to do the actual update -- check for conflict first, to
// 	 * avoid possibly having to roll back work we've just done.
// 	 *
// 	 * This is safe without a recheck as long as there is no possibility of
// 	 * another process scanning the pages between this check and the update
// 	 * being visible to the scan (i.e., exclusive buffer content lock(s) are
// 	 * continuously held from this point until the tuple update is visible).
// 	 *
// 	 * For the new tuple the only check needed is at the relation level, but
// 	 * since both tuples are in the same relation and the check for oldtup
// 	 * will include checking the relation level, there is no benefit to a
// 	 * separate check for the new tuple.
// 	 */
// 	CheckForSerializableConflictIn(relation, &oldtup, buffer);

// 	/*
// 	 * At this point newbuf and buffer are both pinned and locked, and newbuf
// 	 * has enough space for the new tuple.  If they are the same buffer, only
// 	 * one pin is held.
// 	 */

// 	if (newbuf == buffer)
// 	{
// 		/*
// 		 * Since the new tuple is going into the same page, we might be able
// 		 * to do a HOT update.  Check if any of the index columns have been
// 		 * changed.  If not, then HOT update is possible.
// 		 */
// 		if (satisfies_hot)
// 			use_hot_update = true;
// 	}
// 	else
// 	{
// 		/* Set a hint that the old page could use prune/defrag */
// 		PageSetFull(page);
// 	}

// 	/*
// 	 * Compute replica identity tuple before entering the critical section so
// 	 * we don't PANIC upon a memory allocation failure.
// 	 * ExtractReplicaIdentity() will return NULL if nothing needs to be
// 	 * logged.
// 	 */
// 	old_key_tuple = ExtractReplicaIdentity(relation, &oldtup, !satisfies_id, &old_key_copied);

// 	/* NO EREPORT(ERROR) from here till changes are logged */
// 	START_CRIT_SECTION();

// 	/*
// 	 * If this transaction commits, the old tuple will become DEAD sooner or
// 	 * later.  Set flag that this page is a candidate for pruning once our xid
// 	 * falls below the OldestXmin horizon.  If the transaction finally aborts,
// 	 * the subsequent page pruning will be a no-op and the hint will be
// 	 * cleared.
// 	 *
// 	 * XXX Should we set hint on newbuf as well?  If the transaction aborts,
// 	 * there would be a prunable tuple in the newbuf; but for now we choose
// 	 * not to optimize for aborts.  Note that heap_xlog_update must be kept in
// 	 * sync if this decision changes.
// 	 */
// 	PageSetPrunable(page, xid);

// 	if (use_hot_update)
// 	{
// 		/* Mark the old tuple as HOT-updated */
// 		HeapTupleSetHotUpdated(&oldtup);
// 		/* And mark the new tuple as heap-only */
// 		HeapTupleSetHeapOnly(heaptup);
// 		/* Mark the caller's copy too, in case different from heaptup */
// 		HeapTupleSetHeapOnly(newtup);
// 	}
// 	else
// 	{
// 		/* Make sure tuples are correctly marked as not-HOT */
// 		HeapTupleClearHotUpdated(&oldtup);
// 		HeapTupleClearHeapOnly(heaptup);
// 		HeapTupleClearHeapOnly(newtup);
// 	}

// 	RelationPutHeapTuple(relation, newbuf, heaptup, false);		/* insert new tuple */


// 	/* Clear obsolete visibility flags, possibly set by ourselves above... */
// 	oldtup.t_data->t_infomask &= ~(HEAP_XMAX_BITS | HEAP_MOVED);
// 	oldtup.t_data->t_infomask2 &= ~HEAP_KEYS_UPDATED;
// 	/* ... and store info about transaction updating this tuple */
// 	Assert(TransactionIdIsValid(xmax_old_tuple));
// 	HeapTupleHeaderSetXmax(oldtup.t_data, xmax_old_tuple);
// 	oldtup.t_data->t_infomask |= infomask_old_tuple;
// 	oldtup.t_data->t_infomask2 |= infomask2_old_tuple;
// 	HeapTupleHeaderSetCmax(oldtup.t_data, cid, iscombo);

// 	/* record address of new tuple in t_ctid of old one */
// 	oldtup.t_data->t_ctid = heaptup->t_self;

// 	/* clear PD_ALL_VISIBLE flags, reset all visibilitymap bits */
// 	if (PageIsAllVisible(BufferGetPage(buffer)))
// 	{
// 		all_visible_cleared = true;
// 		PageClearAllVisible(BufferGetPage(buffer));
// 		visibilitymap_clear(relation, BufferGetBlockNumber(buffer),
// 							vmbuffer, VISIBILITYMAP_VALID_BITS);
// 	}
// 	if (newbuf != buffer && PageIsAllVisible(BufferGetPage(newbuf)))
// 	{
// 		all_visible_cleared_new = true;
// 		PageClearAllVisible(BufferGetPage(newbuf));
// 		visibilitymap_clear(relation, BufferGetBlockNumber(newbuf),
// 							vmbuffer_new, VISIBILITYMAP_VALID_BITS);
// 	}

// 	if (newbuf != buffer)
// 		MarkBufferDirty(newbuf);
// 	MarkBufferDirty(buffer);

// 	/* XLOG stuff */
// 	if (RelationNeedsWAL(relation))
// 	{
// 		XLogRecPtr	recptr;

// 		/*
// 		 * For logical decoding we need combocids to properly decode the
// 		 * catalog.
// 		 */
// 		if (RelationIsAccessibleInLogicalDecoding(relation))
// 		{
// 			log_heap_new_cid(relation, &oldtup);
// 			log_heap_new_cid(relation, heaptup);
// 		}

// 		recptr = log_heap_update(relation, buffer,
// 								 newbuf, &oldtup, heaptup,
// 								 old_key_tuple,
// 								 all_visible_cleared,
// 								 all_visible_cleared_new);
// 		if (newbuf != buffer)
// 		{
// 			PageSetLSN(BufferGetPage(newbuf), recptr);
// 		}
// 		PageSetLSN(BufferGetPage(buffer), recptr);
// 	}

// 	END_CRIT_SECTION();

// 	if (newbuf != buffer)
// 		LockBuffer(newbuf, BUFFER_LOCK_UNLOCK);
// 	LockBuffer(buffer, BUFFER_LOCK_UNLOCK);

// 	/*
// 	 * Mark old tuple for invalidation from system caches at next command
// 	 * boundary, and mark the new tuple for invalidation in case we abort. We
// 	 * have to do this before releasing the buffer because oldtup is in the
// 	 * buffer.  (heaptup is all in local memory, but it's necessary to process
// 	 * both tuple versions in one call to inval.c so we can avoid redundant
// 	 * sinval messages.)
// 	 */
// 	CacheInvalidateHeapTuple(relation, &oldtup, heaptup);

// 	/* Now we can release the buffer(s) */
// 	if (newbuf != buffer)
// 		ReleaseBuffer(newbuf);
// 	ReleaseBuffer(buffer);
// 	if (BufferIsValid(vmbuffer_new))
// 		ReleaseBuffer(vmbuffer_new);
// 	if (BufferIsValid(vmbuffer))
// 		ReleaseBuffer(vmbuffer);

// 	/*
// 	 * Release the lmgr tuple lock, if we had it.
// 	 */
// 	if (have_tuple_lock)
// 		UnlockTupleTuplock(relation, &(oldtup.t_self), *lockmode);

// 	pgstat_count_heap_update(relation, use_hot_update);

// 	/*
// 	 * If heaptup is a private copy, release it.  Don't forget to copy t_self
// 	 * back to the caller's image, too.
// 	 */
// 	if (heaptup != newtup)
// 	{
// 		newtup->t_self = heaptup->t_self;
// 		heap_freetuple(heaptup);
// 	}

// 	if (old_key_tuple != NULL && old_key_copied)
// 		heap_freetuple(old_key_tuple);

// 	bms_free(hot_attrs);
// 	bms_free(key_attrs);
// 	bms_free(id_attrs);

// 	return HeapTupleMayBeUpdated;
// }
// HTSU_Result
// heap_lock_tuple(Relation relation, HeapTuple tuple,
// 				CommandId cid, LockTupleMode mode, LockWaitPolicy wait_policy,
// 				bool follow_updates,
// 				Buffer *buffer, HeapUpdateFailureData *hufd)
// {
// 	HTSU_Result result;
// 	ItemPointer tid = &(tuple->t_self);
// 	ItemId		lp;
// 	Page		page;
// 	Buffer		vmbuffer = InvalidBuffer;
// 	BlockNumber block;
// 	TransactionId xid,
// 				xmax;
// 	uint16		old_infomask,
// 				new_infomask,
// 				new_infomask2;
// 	bool		first_time = true;
// 	bool		have_tuple_lock = false;
// 	bool		cleared_all_frozen = false;

// 	*buffer = ReadBuffer(relation, ItemPointerGetBlockNumber(tid));
// 	block = ItemPointerGetBlockNumber(tid);

// 	/*
// 	 * Before locking the buffer, pin the visibility map page if it appears to
// 	 * be necessary.  Since we haven't got the lock yet, someone else might be
// 	 * in the middle of changing this, so we'll need to recheck after we have
// 	 * the lock.
// 	 */
// 	if (PageIsAllVisible(BufferGetPage(*buffer)))
// 		visibilitymap_pin(relation, block, &vmbuffer);

// 	LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);

// 	page = BufferGetPage(*buffer);
// 	lp = PageGetItemId(page, ItemPointerGetOffsetNumber(tid));
// 	Assert(ItemIdIsNormal(lp));

// 	tuple->t_data = (HeapTupleHeader) PageGetItem(page, lp);
// 	tuple->t_len = ItemIdGetLength(lp);
// 	tuple->t_tableOid = RelationGetRelid(relation);

// l3:
// 	result = HeapTupleSatisfiesUpdate(tuple, cid, *buffer);

// 	if (result == HeapTupleInvisible)
// 	{
// 		/*
// 		 * This is possible, but only when locking a tuple for ON CONFLICT
// 		 * UPDATE.  We return this value here rather than throwing an error in
// 		 * order to give that case the opportunity to throw a more specific
// 		 * error.
// 		 */
// 		result = HeapTupleInvisible;
// 		goto out_locked;
// 	}
// 	else if (result == HeapTupleBeingUpdated || result == HeapTupleUpdated)
// 	{
// 		TransactionId xwait;
// 		uint16		infomask;
// 		uint16		infomask2;
// 		bool		require_sleep;
// 		ItemPointerData t_ctid;

// 		/* must copy state data before unlocking buffer */
// 		xwait = HeapTupleHeaderGetRawXmax(tuple->t_data);
// 		infomask = tuple->t_data->t_infomask;
// 		infomask2 = tuple->t_data->t_infomask2;
// 		ItemPointerCopy(&tuple->t_data->t_ctid, &t_ctid);

// 		LockBuffer(*buffer, BUFFER_LOCK_UNLOCK);

// 		/*
// 		 * If any subtransaction of the current top transaction already holds
// 		 * a lock as strong as or stronger than what we're requesting, we
// 		 * effectively hold the desired lock already.  We *must* succeed
// 		 * without trying to take the tuple lock, else we will deadlock
// 		 * against anyone wanting to acquire a stronger lock.
// 		 *
// 		 * Note we only do this the first time we loop on the HTSU result;
// 		 * there is no point in testing in subsequent passes, because
// 		 * evidently our own transaction cannot have acquired a new lock after
// 		 * the first time we checked.
// 		 */
// 		if (first_time)
// 		{
// 			first_time = false;

// 			if (infomask & HEAP_XMAX_IS_MULTI)
// 			{
// 				int			i;
// 				int			nmembers;
// 				MultiXactMember *members;

// 				/*
// 				 * We don't need to allow old multixacts here; if that had
// 				 * been the case, HeapTupleSatisfiesUpdate would have returned
// 				 * MayBeUpdated and we wouldn't be here.
// 				 */
// 				nmembers =
// 					GetMultiXactIdMembers(xwait, &members, false,
// 										  HEAP_XMAX_IS_LOCKED_ONLY(infomask));

// 				for (i = 0; i < nmembers; i++)
// 				{
// 					/* only consider members of our own transaction */
// 					if (!TransactionIdIsCurrentTransactionId(members[i].xid))
// 						continue;

// 					if (TUPLOCK_from_mxstatus(members[i].status) >= mode)
// 					{
// 						pfree(members);
// 						result = HeapTupleMayBeUpdated;
// 						goto out_unlocked;
// 					}
// 				}

// 				if (members)
// 					pfree(members);
// 			}
// 			else if (TransactionIdIsCurrentTransactionId(xwait))
// 			{
// 				switch (mode)
// 				{
// 					case LockTupleKeyShare:
// 						Assert(HEAP_XMAX_IS_KEYSHR_LOCKED(infomask) ||
// 							   HEAP_XMAX_IS_SHR_LOCKED(infomask) ||
// 							   HEAP_XMAX_IS_EXCL_LOCKED(infomask));
// 						result = HeapTupleMayBeUpdated;
// 						goto out_unlocked;
// 					case LockTupleShare:
// 						if (HEAP_XMAX_IS_SHR_LOCKED(infomask) ||
// 							HEAP_XMAX_IS_EXCL_LOCKED(infomask))
// 						{
// 							result = HeapTupleMayBeUpdated;
// 							goto out_unlocked;
// 						}
// 						break;
// 					case LockTupleNoKeyExclusive:
// 						if (HEAP_XMAX_IS_EXCL_LOCKED(infomask))
// 						{
// 							result = HeapTupleMayBeUpdated;
// 							goto out_unlocked;
// 						}
// 						break;
// 					case LockTupleExclusive:
// 						if (HEAP_XMAX_IS_EXCL_LOCKED(infomask) &&
// 							infomask2 & HEAP_KEYS_UPDATED)
// 						{
// 							result = HeapTupleMayBeUpdated;
// 							goto out_unlocked;
// 						}
// 						break;
// 				}
// 			}
// 		}

// 		/*
// 		 * Initially assume that we will have to wait for the locking
// 		 * transaction(s) to finish.  We check various cases below in which
// 		 * this can be turned off.
// 		 */
// 		require_sleep = true;
// 		if (mode == LockTupleKeyShare)
// 		{
// 			/*
// 			 * If we're requesting KeyShare, and there's no update present, we
// 			 * don't need to wait.  Even if there is an update, we can still
// 			 * continue if the key hasn't been modified.
// 			 *
// 			 * However, if there are updates, we need to walk the update chain
// 			 * to mark future versions of the row as locked, too.  That way,
// 			 * if somebody deletes that future version, we're protected
// 			 * against the key going away.  This locking of future versions
// 			 * could block momentarily, if a concurrent transaction is
// 			 * deleting a key; or it could return a value to the effect that
// 			 * the transaction deleting the key has already committed.  So we
// 			 * do this before re-locking the buffer; otherwise this would be
// 			 * prone to deadlocks.
// 			 *
// 			 * Note that the TID we're locking was grabbed before we unlocked
// 			 * the buffer.  For it to change while we're not looking, the
// 			 * other properties we're testing for below after re-locking the
// 			 * buffer would also change, in which case we would restart this
// 			 * loop above.
// 			 */
// 			if (!(infomask2 & HEAP_KEYS_UPDATED))
// 			{
// 				bool		updated;

// 				updated = !HEAP_XMAX_IS_LOCKED_ONLY(infomask);

// 				/*
// 				 * If there are updates, follow the update chain; bail out if
// 				 * that cannot be done.
// 				 */
// 				if (follow_updates && updated)
// 				{
// 					HTSU_Result res;

// 					res = heap_lock_updated_tuple(relation, tuple, &t_ctid,
// 												  GetCurrentTransactionId(),
// 												  mode);
// 					if (res != HeapTupleMayBeUpdated)
// 					{
// 						result = res;
// 						/* recovery code expects to have buffer lock held */
// 						LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 						goto failed;
// 					}
// 				}

// 				LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);

// 				/*
// 				 * Make sure it's still an appropriate lock, else start over.
// 				 * Also, if it wasn't updated before we released the lock, but
// 				 * is updated now, we start over too; the reason is that we
// 				 * now need to follow the update chain to lock the new
// 				 * versions.
// 				 */
// 				if (!HeapTupleHeaderIsOnlyLocked(tuple->t_data) &&
// 					((tuple->t_data->t_infomask2 & HEAP_KEYS_UPDATED) ||
// 					 !updated))
// 					goto l3;

// 				/* Things look okay, so we can skip sleeping */
// 				require_sleep = false;

// 				/*
// 				 * Note we allow Xmax to change here; other updaters/lockers
// 				 * could have modified it before we grabbed the buffer lock.
// 				 * However, this is not a problem, because with the recheck we
// 				 * just did we ensure that they still don't conflict with the
// 				 * lock we want.
// 				 */
// 			}
// 		}
// 		else if (mode == LockTupleShare)
// 		{
// 			/*
// 			 * If we're requesting Share, we can similarly avoid sleeping if
// 			 * there's no update and no exclusive lock present.
// 			 */
// 			if (HEAP_XMAX_IS_LOCKED_ONLY(infomask) &&
// 				!HEAP_XMAX_IS_EXCL_LOCKED(infomask))
// 			{
// 				LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);

// 				/*
// 				 * Make sure it's still an appropriate lock, else start over.
// 				 * See above about allowing xmax to change.
// 				 */
// 				if (!HEAP_XMAX_IS_LOCKED_ONLY(tuple->t_data->t_infomask) ||
// 					HEAP_XMAX_IS_EXCL_LOCKED(tuple->t_data->t_infomask))
// 					goto l3;
// 				require_sleep = false;
// 			}
// 		}
// 		else if (mode == LockTupleNoKeyExclusive)
// 		{
// 			/*
// 			 * If we're requesting NoKeyExclusive, we might also be able to
// 			 * avoid sleeping; just ensure that there no conflicting lock
// 			 * already acquired.
// 			 */
// 			if (infomask & HEAP_XMAX_IS_MULTI)
// 			{
// 				if (!DoesMultiXactIdConflict((MultiXactId) xwait, infomask,
// 											 mode))
// 				{
// 					/*
// 					 * No conflict, but if the xmax changed under us in the
// 					 * meantime, start over.
// 					 */
// 					LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 					if (xmax_infomask_changed(tuple->t_data->t_infomask, infomask) ||
// 						!TransactionIdEquals(HeapTupleHeaderGetRawXmax(tuple->t_data),
// 											 xwait))
// 						goto l3;

// 					/* otherwise, we're good */
// 					require_sleep = false;
// 				}
// 			}
// 			else if (HEAP_XMAX_IS_KEYSHR_LOCKED(infomask))
// 			{
// 				LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);

// 				/* if the xmax changed in the meantime, start over */
// 				if (xmax_infomask_changed(tuple->t_data->t_infomask, infomask) ||
// 					!TransactionIdEquals(
// 									HeapTupleHeaderGetRawXmax(tuple->t_data),
// 										 xwait))
// 					goto l3;
// 				/* otherwise, we're good */
// 				require_sleep = false;
// 			}
// 		}

// 		/*
// 		 * As a check independent from those above, we can also avoid sleeping
// 		 * if the current transaction is the sole locker of the tuple.  Note
// 		 * that the strength of the lock already held is irrelevant; this is
// 		 * not about recording the lock in Xmax (which will be done regardless
// 		 * of this optimization, below).  Also, note that the cases where we
// 		 * hold a lock stronger than we are requesting are already handled
// 		 * above by not doing anything.
// 		 *
// 		 * Note we only deal with the non-multixact case here; MultiXactIdWait
// 		 * is well equipped to deal with this situation on its own.
// 		 */
// 		if (require_sleep && !(infomask & HEAP_XMAX_IS_MULTI) &&
// 			TransactionIdIsCurrentTransactionId(xwait))
// 		{
// 			/* ... but if the xmax changed in the meantime, start over */
// 			LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 			if (xmax_infomask_changed(tuple->t_data->t_infomask, infomask) ||
// 				!TransactionIdEquals(HeapTupleHeaderGetRawXmax(tuple->t_data),
// 									 xwait))
// 				goto l3;
// 			Assert(HEAP_XMAX_IS_LOCKED_ONLY(tuple->t_data->t_infomask));
// 			require_sleep = false;
// 		}

// 		/*
// 		 * Time to sleep on the other transaction/multixact, if necessary.
// 		 *
// 		 * If the other transaction is an update that's already committed,
// 		 * then sleeping cannot possibly do any good: if we're required to
// 		 * sleep, get out to raise an error instead.
// 		 *
// 		 * By here, we either have already acquired the buffer exclusive lock,
// 		 * or we must wait for the locking transaction or multixact; so below
// 		 * we ensure that we grab buffer lock after the sleep.
// 		 */
// 		if (require_sleep && result == HeapTupleUpdated)
// 		{
// 			LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 			goto failed;
// 		}
// 		else if (require_sleep)
// 		{
// 			/*
// 			 * Acquire tuple lock to establish our priority for the tuple, or
// 			 * die trying.  LockTuple will release us when we are next-in-line
// 			 * for the tuple.  We must do this even if we are share-locking.
// 			 *
// 			 * If we are forced to "start over" below, we keep the tuple lock;
// 			 * this arranges that we stay at the head of the line while
// 			 * rechecking tuple state.
// 			 */
// 			if (!heap_acquire_tuplock(relation, tid, mode, wait_policy,
// 									  &have_tuple_lock))
// 			{
// 				/*
// 				 * This can only happen if wait_policy is Skip and the lock
// 				 * couldn't be obtained.
// 				 */
// 				result = HeapTupleWouldBlock;
// 				/* recovery code expects to have buffer lock held */
// 				LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 				goto failed;
// 			}

// 			if (infomask & HEAP_XMAX_IS_MULTI)
// 			{
// 				MultiXactStatus status = get_mxact_status_for_lock(mode, false);

// 				/* We only ever lock tuples, never update them */
// 				if (status >= MultiXactStatusNoKeyUpdate)
// 					elog(ERROR, "invalid lock mode in heap_lock_tuple");

// 				/* wait for multixact to end, or die trying  */
// 				switch (wait_policy)
// 				{
// 					case LockWaitBlock:
// 						MultiXactIdWait((MultiXactId) xwait, status, infomask,
// 								  relation, &tuple->t_self, XLTW_Lock, NULL);
// 						break;
// 					case LockWaitSkip:
// 						if (!ConditionalMultiXactIdWait((MultiXactId) xwait,
// 												  status, infomask, relation,
// 														NULL))
// 						{
// 							result = HeapTupleWouldBlock;
// 							/* recovery code expects to have buffer lock held */
// 							LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 							goto failed;
// 						}
// 						break;
// 					case LockWaitError:
// 						if (!ConditionalMultiXactIdWait((MultiXactId) xwait,
// 												  status, infomask, relation,
// 														NULL))
// 							ereport(ERROR,
// 									(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 									 errmsg("could not obtain lock on row in relation \"%s\"",
// 										RelationGetRelationName(relation))));

// 						break;
// 				}

// 				/*
// 				 * Of course, the multixact might not be done here: if we're
// 				 * requesting a light lock mode, other transactions with light
// 				 * locks could still be alive, as well as locks owned by our
// 				 * own xact or other subxacts of this backend.  We need to
// 				 * preserve the surviving MultiXact members.  Note that it
// 				 * isn't absolutely necessary in the latter case, but doing so
// 				 * is simpler.
// 				 */
// 			}
// 			else
// 			{
// 				/* wait for regular transaction to end, or die trying */
// 				switch (wait_policy)
// 				{
// 					case LockWaitBlock:
// 						XactLockTableWait(xwait, relation, &tuple->t_self,
// 										  XLTW_Lock);
// 						break;
// 					case LockWaitSkip:
// 						if (!ConditionalXactLockTableWait(xwait))
// 						{
// 							result = HeapTupleWouldBlock;
// 							/* recovery code expects to have buffer lock held */
// 							LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 							goto failed;
// 						}
// 						break;
// 					case LockWaitError:
// 						if (!ConditionalXactLockTableWait(xwait))
// 							ereport(ERROR,
// 									(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 									 errmsg("could not obtain lock on row in relation \"%s\"",
// 										RelationGetRelationName(relation))));
// 						break;
// 				}
// 			}

// 			/* if there are updates, follow the update chain */
// 			if (follow_updates && !HEAP_XMAX_IS_LOCKED_ONLY(infomask))
// 			{
// 				HTSU_Result res;

// 				res = heap_lock_updated_tuple(relation, tuple, &t_ctid,
// 											  GetCurrentTransactionId(),
// 											  mode);
// 				if (res != HeapTupleMayBeUpdated)
// 				{
// 					result = res;
// 					/* recovery code expects to have buffer lock held */
// 					LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 					goto failed;
// 				}
// 			}

// 			LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);

// 			/*
// 			 * xwait is done, but if xwait had just locked the tuple then some
// 			 * other xact could update this tuple before we get to this point.
// 			 * Check for xmax change, and start over if so.
// 			 */
// 			if (xmax_infomask_changed(tuple->t_data->t_infomask, infomask) ||
// 				!TransactionIdEquals(HeapTupleHeaderGetRawXmax(tuple->t_data),
// 									 xwait))
// 				goto l3;

// 			if (!(infomask & HEAP_XMAX_IS_MULTI))
// 			{
// 				/*
// 				 * Otherwise check if it committed or aborted.  Note we cannot
// 				 * be here if the tuple was only locked by somebody who didn't
// 				 * conflict with us; that would have been handled above.  So
// 				 * that transaction must necessarily be gone by now.  But
// 				 * don't check for this in the multixact case, because some
// 				 * locker transactions might still be running.
// 				 */
// 				UpdateXmaxHintBits(tuple->t_data, *buffer, xwait);
// 			}
// 		}

// 		/* By here, we're certain that we hold buffer exclusive lock again */

// 		/*
// 		 * We may lock if previous xmax aborted, or if it committed but only
// 		 * locked the tuple without updating it; or if we didn't have to wait
// 		 * at all for whatever reason.
// 		 */
// 		if (!require_sleep ||
// 			(tuple->t_data->t_infomask & HEAP_XMAX_INVALID) ||
// 			HEAP_XMAX_IS_LOCKED_ONLY(tuple->t_data->t_infomask) ||
// 			HeapTupleHeaderIsOnlyLocked(tuple->t_data))
// 			result = HeapTupleMayBeUpdated;
// 		else
// 			result = HeapTupleUpdated;
// 	}

// failed:
// 	if (result != HeapTupleMayBeUpdated)
// 	{
// 		Assert(result == HeapTupleSelfUpdated || result == HeapTupleUpdated ||
// 			   result == HeapTupleWouldBlock);
// 		Assert(!(tuple->t_data->t_infomask & HEAP_XMAX_INVALID));
// 		hufd->ctid = tuple->t_data->t_ctid;
// 		hufd->xmax = HeapTupleHeaderGetUpdateXid(tuple->t_data);
// 		if (result == HeapTupleSelfUpdated)
// 			hufd->cmax = HeapTupleHeaderGetCmax(tuple->t_data);
// 		else
// 			hufd->cmax = InvalidCommandId;
// 		goto out_locked;
// 	}

// 	/*
// 	 * If we didn't pin the visibility map page and the page has become all
// 	 * visible while we were busy locking the buffer, or during some
// 	 * subsequent window during which we had it unlocked, we'll have to unlock
// 	 * and re-lock, to avoid holding the buffer lock across I/O.  That's a bit
// 	 * unfortunate, especially since we'll now have to recheck whether the
// 	 * tuple has been locked or updated under us, but hopefully it won't
// 	 * happen very often.
// 	 */
// 	if (vmbuffer == InvalidBuffer && PageIsAllVisible(page))
// 	{
// 		LockBuffer(*buffer, BUFFER_LOCK_UNLOCK);
// 		visibilitymap_pin(relation, block, &vmbuffer);
// 		LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 		goto l3;
// 	}

// 	xmax = HeapTupleHeaderGetRawXmax(tuple->t_data);
// 	old_infomask = tuple->t_data->t_infomask;

// 	/*
// 	 * If this is the first possibly-multixact-able operation in the current
// 	 * transaction, set my per-backend OldestMemberMXactId setting. We can be
// 	 * certain that the transaction will never become a member of any older
// 	 * MultiXactIds than that.  (We have to do this even if we end up just
// 	 * using our own TransactionId below, since some other backend could
// 	 * incorporate our XID into a MultiXact immediately afterwards.)
// 	 */
// 	MultiXactIdSetOldestMember();

// 	/*
// 	 * Compute the new xmax and infomask to store into the tuple.  Note we do
// 	 * not modify the tuple just yet, because that would leave it in the wrong
// 	 * state if multixact.c elogs.
// 	 */
// 	compute_new_xmax_infomask(xmax, old_infomask, tuple->t_data->t_infomask2,
// 							  GetCurrentTransactionId(), mode, false,
// 							  &xid, &new_infomask, &new_infomask2);

// 	START_CRIT_SECTION();

// 	/*
// 	 * Store transaction information of xact locking the tuple.
// 	 *
// 	 * Note: Cmax is meaningless in this context, so don't set it; this avoids
// 	 * possibly generating a useless combo CID.  Moreover, if we're locking a
// 	 * previously updated tuple, it's important to preserve the Cmax.
// 	 *
// 	 * Also reset the HOT UPDATE bit, but only if there's no update; otherwise
// 	 * we would break the HOT chain.
// 	 */
// 	tuple->t_data->t_infomask &= ~HEAP_XMAX_BITS;
// 	tuple->t_data->t_infomask2 &= ~HEAP_KEYS_UPDATED;
// 	tuple->t_data->t_infomask |= new_infomask;
// 	tuple->t_data->t_infomask2 |= new_infomask2;
// 	if (HEAP_XMAX_IS_LOCKED_ONLY(new_infomask))
// 		HeapTupleHeaderClearHotUpdated(tuple->t_data);
// 	HeapTupleHeaderSetXmax(tuple->t_data, xid);

// 	/*
// 	 * Make sure there is no forward chain link in t_ctid.  Note that in the
// 	 * cases where the tuple has been updated, we must not overwrite t_ctid,
// 	 * because it was set by the updater.  Moreover, if the tuple has been
// 	 * updated, we need to follow the update chain to lock the new versions of
// 	 * the tuple as well.
// 	 */
// 	if (HEAP_XMAX_IS_LOCKED_ONLY(new_infomask))
// 		tuple->t_data->t_ctid = *tid;

// 	/* Clear only the all-frozen bit on visibility map if needed */
// 	if (PageIsAllVisible(page) &&
// 		visibilitymap_clear(relation, block, vmbuffer,
// 							VISIBILITYMAP_ALL_FROZEN))
// 		cleared_all_frozen = true;


// 	MarkBufferDirty(*buffer);

// 	/*
// 	 * XLOG stuff.  You might think that we don't need an XLOG record because
// 	 * there is no state change worth restoring after a crash.  You would be
// 	 * wrong however: we have just written either a TransactionId or a
// 	 * MultiXactId that may never have been seen on disk before, and we need
// 	 * to make sure that there are XLOG entries covering those ID numbers.
// 	 * Else the same IDs might be re-used after a crash, which would be
// 	 * disastrous if this page made it to disk before the crash.  Essentially
// 	 * we have to enforce the WAL log-before-data rule even in this case.
// 	 * (Also, in a PITR log-shipping or 2PC environment, we have to have XLOG
// 	 * entries for everything anyway.)
// 	 */
// 	if (RelationNeedsWAL(relation))
// 	{
// 		xl_heap_lock xlrec;
// 		XLogRecPtr	recptr;

// 		XLogBeginInsert();
// 		XLogRegisterBuffer(0, *buffer, REGBUF_STANDARD);

// 		xlrec.offnum = ItemPointerGetOffsetNumber(&tuple->t_self);
// 		xlrec.locking_xid = xid;
// 		xlrec.infobits_set = compute_infobits(new_infomask,
// 											  tuple->t_data->t_infomask2);
// 		xlrec.flags = cleared_all_frozen ? XLH_LOCK_ALL_FROZEN_CLEARED : 0;
// 		XLogRegisterData((char *) &xlrec, SizeOfHeapLock);

// 		/* we don't decode row locks atm, so no need to log the origin */

// 		recptr = XLogInsert(RM_HEAP_ID, XLOG_HEAP_LOCK);

// 		PageSetLSN(page, recptr);
// 	}

// 	END_CRIT_SECTION();

// 	result = HeapTupleMayBeUpdated;

// out_locked:
// 	LockBuffer(*buffer, BUFFER_LOCK_UNLOCK);

// out_unlocked:
// 	if (BufferIsValid(vmbuffer))
// 		ReleaseBuffer(vmbuffer);

// 	/*
// 	 * Don't update the visibility map here. Locking a tuple doesn't change
// 	 * visibility info.
// 	 */

// 	/*
// 	 * Now that we have successfully marked the tuple as locked, we can
// 	 * release the lmgr tuple lock, if we had it.
// 	 */
// 	if (have_tuple_lock)
// 		UnlockTupleTuplock(relation, tid, mode);

// 	return result;
// }
// LOCKMODE
// AlterTableGetRelOptionsLockLevel(List *defList)
// {
// 	LOCKMODE	lockmode = NoLock;
// 	ListCell   *cell;

// 	if (defList == NIL)
// 		return AccessExclusiveLock;

// 	if (need_initialization)
// 		initialize_reloptions();

// 	foreach(cell, defList)
// 	{
// 		DefElem    *def = (DefElem *) lfirst(cell);
// 		int			i;

// 		for (i = 0; relOpts[i]; i++)
// 		{
// 			if (pg_strncasecmp(relOpts[i]->name,
// 							   def->defname,
// 							   relOpts[i]->namelen + 1) == 0)
// 			{
// 				if (lockmode < relOpts[i]->lockmode)
// 					lockmode = relOpts[i]->lockmode;
// 			}
// 		}
// 	}

// 	return lockmode;
// }
// void
// TupleDescCopyEntry(TupleDesc dst, AttrNumber dstAttno,
// 				   TupleDesc src, AttrNumber srcAttno)
// {
// 	/*
// 	 * sanity checks
// 	 */
// 	AssertArg(PointerIsValid(src));
// 	AssertArg(PointerIsValid(dst));
// 	AssertArg(srcAttno >= 1);
// 	AssertArg(srcAttno <= src->natts);
// 	AssertArg(dstAttno >= 1);
// 	AssertArg(dstAttno <= dst->natts);

// 	memcpy(dst->attrs[dstAttno - 1], src->attrs[srcAttno - 1],
// 		   ATTRIBUTE_FIXED_PART_SIZE);

// 	/*
// 	 * Aside from updating the attno, we'd better reset attcacheoff.
// 	 *
// 	 * XXX Actually, to be entirely safe we'd need to reset the attcacheoff of
// 	 * all following columns in dst as well.  Current usage scenarios don't
// 	 * require that though, because all following columns will get initialized
// 	 * by other uses of this function or TupleDescInitEntry.  So we cheat a
// 	 * bit to avoid a useless O(N^2) penalty.
// 	 */
// 	dst->attrs[dstAttno - 1]->attnum = dstAttno;
// 	dst->attrs[dstAttno - 1]->attcacheoff = -1;

// 	/* since we're not copying constraints or defaults, clear these */
// 	dst->attrs[dstAttno - 1]->attnotnull = false;
// 	dst->attrs[dstAttno - 1]->atthasdef = false;
// }
// Size
// EstimateTransactionStateSpace(void)
// {
// 	TransactionState s;
// 	Size		nxids = 6;		/* iso level, deferrable, top & current XID,
// 								 * command counter, XID count */

// 	for (s = CurrentTransactionState; s != NULL; s = s->parent)
// 	{
// 		if (TransactionIdIsValid(s->transactionId))
// 			nxids = add_size(nxids, 1);
// 		nxids = add_size(nxids, s->nChildXids);
// 	}

// 	nxids = add_size(nxids, nParallelCurrentXids);
// 	return mul_size(nxids, sizeof(TransactionId));
// }
// void
// SerializeTransactionState(Size maxsize, char *start_address)
// {
// 	TransactionState s;
// 	Size		nxids = 0;
// 	Size		i = 0;
// 	Size		c = 0;
// 	TransactionId *workspace;
// 	TransactionId *result = (TransactionId *) start_address;

// 	result[c++] = (TransactionId) XactIsoLevel;
// 	result[c++] = (TransactionId) XactDeferrable;
// 	result[c++] = XactTopTransactionId;
// 	result[c++] = CurrentTransactionState->transactionId;
// 	result[c++] = (TransactionId) currentCommandId;
// 	Assert(maxsize >= c * sizeof(TransactionId));

// 	/*
// 	 * If we're running in a parallel worker and launching a parallel worker
// 	 * of our own, we can just pass along the information that was passed to
// 	 * us.
// 	 */
// 	if (nParallelCurrentXids > 0)
// 	{
// 		result[c++] = nParallelCurrentXids;
// 		Assert(maxsize >= (nParallelCurrentXids + c) * sizeof(TransactionId));
// 		memcpy(&result[c], ParallelCurrentXids,
// 			   nParallelCurrentXids * sizeof(TransactionId));
// 		return;
// 	}

// 	/*
// 	 * OK, we need to generate a sorted list of XIDs that our workers should
// 	 * view as current.  First, figure out how many there are.
// 	 */
// 	for (s = CurrentTransactionState; s != NULL; s = s->parent)
// 	{
// 		if (TransactionIdIsValid(s->transactionId))
// 			nxids = add_size(nxids, 1);
// 		nxids = add_size(nxids, s->nChildXids);
// 	}
// 	Assert((c + 1 + nxids) * sizeof(TransactionId) <= maxsize);

// 	/* Copy them to our scratch space. */
// 	workspace = palloc(nxids * sizeof(TransactionId));
// 	for (s = CurrentTransactionState; s != NULL; s = s->parent)
// 	{
// 		if (TransactionIdIsValid(s->transactionId))
// 			workspace[i++] = s->transactionId;
// 		memcpy(&workspace[i], s->childXids,
// 			   s->nChildXids * sizeof(TransactionId));
// 		i += s->nChildXids;
// 	}
// 	Assert(i == nxids);

// 	/* Sort them. */
// 	qsort(workspace, nxids, sizeof(TransactionId), xidComparator);

// 	/* Copy data into output area. */
// 	result[c++] = (TransactionId) nxids;
// 	memcpy(&result[c], workspace, nxids * sizeof(TransactionId));
// }
// void
// StartParallelWorkerTransaction(char *tstatespace)
// {
// 	TransactionId *tstate = (TransactionId *) tstatespace;

// 	Assert(CurrentTransactionState->blockState == TBLOCK_DEFAULT);
// 	StartTransaction();

// 	XactIsoLevel = (int) tstate[0];
// 	XactDeferrable = (bool) tstate[1];
// 	XactTopTransactionId = tstate[2];
// 	CurrentTransactionState->transactionId = tstate[3];
// 	currentCommandId = tstate[4];
// 	nParallelCurrentXids = (int) tstate[5];
// 	ParallelCurrentXids = &tstate[6];

// 	CurrentTransactionState->blockState = TBLOCK_PARALLEL_INPROGRESS;
// }
// XLogRecPtr
// XactLogCommitRecord(TimestampTz commit_time,
// 					int nsubxacts, TransactionId *subxacts,
// 					int nrels, RelFileNode *rels,
// 					int nmsgs, SharedInvalidationMessage *msgs,
// 					bool relcacheInval, bool forceSync,
// 					TransactionId twophase_xid)
// {
// 	xl_xact_commit xlrec;
// 	xl_xact_xinfo xl_xinfo;
// 	xl_xact_dbinfo xl_dbinfo;
// 	xl_xact_subxacts xl_subxacts;
// 	xl_xact_relfilenodes xl_relfilenodes;
// 	xl_xact_invals xl_invals;
// 	xl_xact_twophase xl_twophase;
// 	xl_xact_origin xl_origin;

// 	uint8		info;

// 	Assert(CritSectionCount > 0);

// 	xl_xinfo.xinfo = 0;

// 	/* decide between a plain and 2pc commit */
// 	if (!TransactionIdIsValid(twophase_xid))
// 		info = XLOG_XACT_COMMIT;
// 	else
// 		info = XLOG_XACT_COMMIT_PREPARED;

// 	/* First figure out and collect all the information needed */

// 	xlrec.xact_time = commit_time;

// 	if (relcacheInval)
// 		xl_xinfo.xinfo |= XACT_COMPLETION_UPDATE_RELCACHE_FILE;
// 	if (forceSyncCommit)
// 		xl_xinfo.xinfo |= XACT_COMPLETION_FORCE_SYNC_COMMIT;

// 	/*
// 	 * Check if the caller would like to ask standbys for immediate feedback
// 	 * once this commit is applied.
// 	 */
// 	if (synchronous_commit >= SYNCHRONOUS_COMMIT_REMOTE_APPLY)
// 		xl_xinfo.xinfo |= XACT_COMPLETION_APPLY_FEEDBACK;

// 	/*
// 	 * Relcache invalidations requires information about the current database
// 	 * and so does logical decoding.
// 	 */
// 	if (nmsgs > 0 || XLogLogicalInfoActive())
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_DBINFO;
// 		xl_dbinfo.dbId = MyDatabaseId;
// 		xl_dbinfo.tsId = MyDatabaseTableSpace;
// 	}

// 	if (nsubxacts > 0)
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_SUBXACTS;
// 		xl_subxacts.nsubxacts = nsubxacts;
// 	}

// 	if (nrels > 0)
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_RELFILENODES;
// 		xl_relfilenodes.nrels = nrels;
// 	}

// 	if (nmsgs > 0)
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_INVALS;
// 		xl_invals.nmsgs = nmsgs;
// 	}

// 	if (TransactionIdIsValid(twophase_xid))
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_TWOPHASE;
// 		xl_twophase.xid = twophase_xid;
// 	}

// 	/* dump transaction origin information */
// 	if (replorigin_session_origin != InvalidRepOriginId)
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_ORIGIN;

// 		xl_origin.origin_lsn = replorigin_session_origin_lsn;
// 		xl_origin.origin_timestamp = replorigin_session_origin_timestamp;
// 	}

// 	if (xl_xinfo.xinfo != 0)
// 		info |= XLOG_XACT_HAS_INFO;

// 	/* Then include all the collected data into the commit record. */

// 	XLogBeginInsert();

// 	XLogRegisterData((char *) (&xlrec), sizeof(xl_xact_commit));

// 	if (xl_xinfo.xinfo != 0)
// 		XLogRegisterData((char *) (&xl_xinfo.xinfo), sizeof(xl_xinfo.xinfo));

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_DBINFO)
// 		XLogRegisterData((char *) (&xl_dbinfo), sizeof(xl_dbinfo));

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_SUBXACTS)
// 	{
// 		XLogRegisterData((char *) (&xl_subxacts),
// 						 MinSizeOfXactSubxacts);
// 		XLogRegisterData((char *) subxacts,
// 						 nsubxacts * sizeof(TransactionId));
// 	}

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_RELFILENODES)
// 	{
// 		XLogRegisterData((char *) (&xl_relfilenodes),
// 						 MinSizeOfXactRelfilenodes);
// 		XLogRegisterData((char *) rels,
// 						 nrels * sizeof(RelFileNode));
// 	}

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_INVALS)
// 	{
// 		XLogRegisterData((char *) (&xl_invals), MinSizeOfXactInvals);
// 		XLogRegisterData((char *) msgs,
// 						 nmsgs * sizeof(SharedInvalidationMessage));
// 	}

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_TWOPHASE)
// 		XLogRegisterData((char *) (&xl_twophase), sizeof(xl_xact_twophase));

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_ORIGIN)
// 		XLogRegisterData((char *) (&xl_origin), sizeof(xl_xact_origin));

// 	/* we allow filtering by xacts */
// 	XLogIncludeOrigin();

// 	return XLogInsert(RM_XACT_ID, info);
// }
// XLogRecPtr
// XactLogAbortRecord(TimestampTz abort_time,
// 				   int nsubxacts, TransactionId *subxacts,
// 				   int nrels, RelFileNode *rels,
// 				   TransactionId twophase_xid)
// {
// 	xl_xact_abort xlrec;
// 	xl_xact_xinfo xl_xinfo;
// 	xl_xact_subxacts xl_subxacts;
// 	xl_xact_relfilenodes xl_relfilenodes;
// 	xl_xact_twophase xl_twophase;

// 	uint8		info;

// 	Assert(CritSectionCount > 0);

// 	xl_xinfo.xinfo = 0;

// 	/* decide between a plain and 2pc abort */
// 	if (!TransactionIdIsValid(twophase_xid))
// 		info = XLOG_XACT_ABORT;
// 	else
// 		info = XLOG_XACT_ABORT_PREPARED;


// 	/* First figure out and collect all the information needed */

// 	xlrec.xact_time = abort_time;

// 	if (nsubxacts > 0)
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_SUBXACTS;
// 		xl_subxacts.nsubxacts = nsubxacts;
// 	}

// 	if (nrels > 0)
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_RELFILENODES;
// 		xl_relfilenodes.nrels = nrels;
// 	}

// 	if (TransactionIdIsValid(twophase_xid))
// 	{
// 		xl_xinfo.xinfo |= XACT_XINFO_HAS_TWOPHASE;
// 		xl_twophase.xid = twophase_xid;
// 	}

// 	if (xl_xinfo.xinfo != 0)
// 		info |= XLOG_XACT_HAS_INFO;

// 	/* Then include all the collected data into the abort record. */

// 	XLogBeginInsert();

// 	XLogRegisterData((char *) (&xlrec), MinSizeOfXactAbort);

// 	if (xl_xinfo.xinfo != 0)
// 		XLogRegisterData((char *) (&xl_xinfo), sizeof(xl_xinfo));

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_SUBXACTS)
// 	{
// 		XLogRegisterData((char *) (&xl_subxacts),
// 						 MinSizeOfXactSubxacts);
// 		XLogRegisterData((char *) subxacts,
// 						 nsubxacts * sizeof(TransactionId));
// 	}

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_RELFILENODES)
// 	{
// 		XLogRegisterData((char *) (&xl_relfilenodes),
// 						 MinSizeOfXactRelfilenodes);
// 		XLogRegisterData((char *) rels,
// 						 nrels * sizeof(RelFileNode));
// 	}

// 	if (xl_xinfo.xinfo & XACT_XINFO_HAS_TWOPHASE)
// 		XLogRegisterData((char *) (&xl_twophase), sizeof(xl_xact_twophase));

// 	return XLogInsert(RM_XACT_ID, info);
// }
// const char *
// xact_identify(uint8 info)
// {
// 	const char *id = NULL;

// 	switch (info & XLOG_XACT_OPMASK)
// 	{
// 		case XLOG_XACT_COMMIT:
// 			id = "COMMIT";
// 			break;
// 		case XLOG_XACT_PREPARE:
// 			id = "PREPARE";
// 			break;
// 		case XLOG_XACT_ABORT:
// 			id = "ABORT";
// 			break;
// 		case XLOG_XACT_COMMIT_PREPARED:
// 			id = "COMMIT_PREPARED";
// 			break;
// 		case XLOG_XACT_ABORT_PREPARED:
// 			id = "ABORT_PREPARED";
// 			break;
// 		case XLOG_XACT_ASSIGNMENT:
// 			id = "ASSIGNMENT";
// 			break;
// 	}

// 	return id;
// }
// void
// ParseCommitRecord(uint8 info, xl_xact_commit *xlrec, xl_xact_parsed_commit *parsed)
// {
// 	char	   *data = ((char *) xlrec) + MinSizeOfXactCommit;

// 	memset(parsed, 0, sizeof(*parsed));

// 	parsed->xinfo = 0;			/* default, if no XLOG_XACT_HAS_INFO is
// 								 * present */

// 	parsed->xact_time = xlrec->xact_time;

// 	if (info & XLOG_XACT_HAS_INFO)
// 	{
// 		xl_xact_xinfo *xl_xinfo = (xl_xact_xinfo *) data;

// 		parsed->xinfo = xl_xinfo->xinfo;

// 		data += sizeof(xl_xact_xinfo);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_DBINFO)
// 	{
// 		xl_xact_dbinfo *xl_dbinfo = (xl_xact_dbinfo *) data;

// 		parsed->dbId = xl_dbinfo->dbId;
// 		parsed->tsId = xl_dbinfo->tsId;

// 		data += sizeof(xl_xact_dbinfo);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_SUBXACTS)
// 	{
// 		xl_xact_subxacts *xl_subxacts = (xl_xact_subxacts *) data;

// 		parsed->nsubxacts = xl_subxacts->nsubxacts;
// 		parsed->subxacts = xl_subxacts->subxacts;

// 		data += MinSizeOfXactSubxacts;
// 		data += parsed->nsubxacts * sizeof(TransactionId);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_RELFILENODES)
// 	{
// 		xl_xact_relfilenodes *xl_relfilenodes = (xl_xact_relfilenodes *) data;

// 		parsed->nrels = xl_relfilenodes->nrels;
// 		parsed->xnodes = xl_relfilenodes->xnodes;

// 		data += MinSizeOfXactRelfilenodes;
// 		data += xl_relfilenodes->nrels * sizeof(RelFileNode);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_INVALS)
// 	{
// 		xl_xact_invals *xl_invals = (xl_xact_invals *) data;

// 		parsed->nmsgs = xl_invals->nmsgs;
// 		parsed->msgs = xl_invals->msgs;

// 		data += MinSizeOfXactInvals;
// 		data += xl_invals->nmsgs * sizeof(SharedInvalidationMessage);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_TWOPHASE)
// 	{
// 		xl_xact_twophase *xl_twophase = (xl_xact_twophase *) data;

// 		parsed->twophase_xid = xl_twophase->xid;

// 		data += sizeof(xl_xact_twophase);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_ORIGIN)
// 	{
// 		xl_xact_origin xl_origin;

// 		/* we're only guaranteed 4 byte alignment, so copy onto stack */
// 		memcpy(&xl_origin, data, sizeof(xl_origin));

// 		parsed->origin_lsn = xl_origin.origin_lsn;
// 		parsed->origin_timestamp = xl_origin.origin_timestamp;

// 		data += sizeof(xl_xact_origin);
// 	}
// }
// void
// ParseAbortRecord(uint8 info, xl_xact_abort *xlrec, xl_xact_parsed_abort *parsed)
// {
// 	char	   *data = ((char *) xlrec) + MinSizeOfXactAbort;

// 	memset(parsed, 0, sizeof(*parsed));

// 	parsed->xinfo = 0;			/* default, if no XLOG_XACT_HAS_INFO is
// 								 * present */

// 	parsed->xact_time = xlrec->xact_time;

// 	if (info & XLOG_XACT_HAS_INFO)
// 	{
// 		xl_xact_xinfo *xl_xinfo = (xl_xact_xinfo *) data;

// 		parsed->xinfo = xl_xinfo->xinfo;

// 		data += sizeof(xl_xact_xinfo);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_SUBXACTS)
// 	{
// 		xl_xact_subxacts *xl_subxacts = (xl_xact_subxacts *) data;

// 		parsed->nsubxacts = xl_subxacts->nsubxacts;
// 		parsed->subxacts = xl_subxacts->subxacts;

// 		data += MinSizeOfXactSubxacts;
// 		data += parsed->nsubxacts * sizeof(TransactionId);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_RELFILENODES)
// 	{
// 		xl_xact_relfilenodes *xl_relfilenodes = (xl_xact_relfilenodes *) data;

// 		parsed->nrels = xl_relfilenodes->nrels;
// 		parsed->xnodes = xl_relfilenodes->xnodes;

// 		data += MinSizeOfXactRelfilenodes;
// 		data += xl_relfilenodes->nrels * sizeof(RelFileNode);
// 	}

// 	if (parsed->xinfo & XACT_XINFO_HAS_TWOPHASE)
// 	{
// 		xl_xact_twophase *xl_twophase = (xl_xact_twophase *) data;

// 		parsed->twophase_xid = xl_twophase->xid;

// 		data += sizeof(xl_xact_twophase);
// 	}
// }
// void
// EnterParallelMode(void)
// {
// 	TransactionState s = CurrentTransactionState;

// 	Assert(s->parallelModeLevel >= 0);

// 	++s->parallelModeLevel;
// }
// void
// ExitParallelMode(void)
// {
// 	TransactionState s = CurrentTransactionState;

// 	Assert(s->parallelModeLevel > 0);
// 	Assert(s->parallelModeLevel > 1 || !ParallelContextActive());

// 	--s->parallelModeLevel;
// }
// bool
// IsInParallelMode(void)
// {
// 	return CurrentTransactionState->parallelModeLevel != 0;
// }
// FuncCandidateList
// FuncnameGetCandidates(List *names, int nargs, List *argnames,
// 					  bool expand_variadic, bool expand_defaults,
// 					  bool missing_ok)
// {
// 	FuncCandidateList resultList = NULL;
// 	bool		any_special = false;
// 	char	   *schemaname;
// 	char	   *funcname;
// 	Oid			namespaceId;
// 	CatCList   *catlist;
// 	int			i;

// 	/* check for caller error */
// 	Assert(nargs >= 0 || !(expand_variadic | expand_defaults));

// 	/* deconstruct the name list */
// 	DeconstructQualifiedName(names, &schemaname, &funcname);

// 	if (schemaname)
// 	{
// 		/* use exact schema given */
// 		namespaceId = LookupExplicitNamespace(schemaname, missing_ok);
// 		if (!OidIsValid(namespaceId))
// 			return NULL;
// 	}
// 	else
// 	{
// 		/* flag to indicate we need namespace search */
// 		namespaceId = InvalidOid;
// 		recomputeNamespacePath();
// 	}

// 	/* Search syscache by name only */
// 	catlist = SearchSysCacheList1(PROCNAMEARGSNSP, CStringGetDatum(funcname));

// 	for (i = 0; i < catlist->n_members; i++)
// 	{
// 		HeapTuple	proctup = &catlist->members[i]->tuple;
// 		Form_pg_proc procform = (Form_pg_proc) GETSTRUCT(proctup);
// 		int			pronargs = procform->pronargs;
// 		int			effective_nargs;
// 		int			pathpos = 0;
// 		bool		variadic;
// 		bool		use_defaults;
// 		Oid			va_elem_type;
// 		int		   *argnumbers = NULL;
// 		FuncCandidateList newResult;

// 		if (OidIsValid(namespaceId))
// 		{
// 			/* Consider only procs in specified namespace */
// 			if (procform->pronamespace != namespaceId)
// 				continue;
// 		}
// 		else
// 		{
// 			/*
// 			 * Consider only procs that are in the search path and are not in
// 			 * the temp namespace.
// 			 */
// 			ListCell   *nsp;

// 			foreach(nsp, activeSearchPath)
// 			{
// 				if (procform->pronamespace == lfirst_oid(nsp) &&
// 					procform->pronamespace != myTempNamespace)
// 					break;
// 				pathpos++;
// 			}
// 			if (nsp == NULL)
// 				continue;		/* proc is not in search path */
// 		}

// 		if (argnames != NIL)
// 		{
// 			/*
// 			 * Call uses named or mixed notation
// 			 *
// 			 * Named or mixed notation can match a variadic function only if
// 			 * expand_variadic is off; otherwise there is no way to match the
// 			 * presumed-nameless parameters expanded from the variadic array.
// 			 */
// 			if (OidIsValid(procform->provariadic) && expand_variadic)
// 				continue;
// 			va_elem_type = InvalidOid;
// 			variadic = false;

// 			/*
// 			 * Check argument count.
// 			 */
// 			Assert(nargs >= 0); /* -1 not supported with argnames */

// 			if (pronargs > nargs && expand_defaults)
// 			{
// 				/* Ignore if not enough default expressions */
// 				if (nargs + procform->pronargdefaults < pronargs)
// 					continue;
// 				use_defaults = true;
// 			}
// 			else
// 				use_defaults = false;

// 			/* Ignore if it doesn't match requested argument count */
// 			if (pronargs != nargs && !use_defaults)
// 				continue;

// 			/* Check for argument name match, generate positional mapping */
// 			if (!MatchNamedCall(proctup, nargs, argnames,
// 								&argnumbers))
// 				continue;

// 			/* Named argument matching is always "special" */
// 			any_special = true;
// 		}
// 		else
// 		{
// 			/*
// 			 * Call uses positional notation
// 			 *
// 			 * Check if function is variadic, and get variadic element type if
// 			 * so.  If expand_variadic is false, we should just ignore
// 			 * variadic-ness.
// 			 */
// 			if (pronargs <= nargs && expand_variadic)
// 			{
// 				va_elem_type = procform->provariadic;
// 				variadic = OidIsValid(va_elem_type);
// 				any_special |= variadic;
// 			}
// 			else
// 			{
// 				va_elem_type = InvalidOid;
// 				variadic = false;
// 			}

// 			/*
// 			 * Check if function can match by using parameter defaults.
// 			 */
// 			if (pronargs > nargs && expand_defaults)
// 			{
// 				/* Ignore if not enough default expressions */
// 				if (nargs + procform->pronargdefaults < pronargs)
// 					continue;
// 				use_defaults = true;
// 				any_special = true;
// 			}
// 			else
// 				use_defaults = false;

// 			/* Ignore if it doesn't match requested argument count */
// 			if (nargs >= 0 && pronargs != nargs && !variadic && !use_defaults)
// 				continue;
// 		}

// 		/*
// 		 * We must compute the effective argument list so that we can easily
// 		 * compare it to earlier results.  We waste a palloc cycle if it gets
// 		 * masked by an earlier result, but really that's a pretty infrequent
// 		 * case so it's not worth worrying about.
// 		 */
// 		effective_nargs = Max(pronargs, nargs);
// 		newResult = (FuncCandidateList)
// 			palloc(offsetof(struct _FuncCandidateList, args) +
// 				   effective_nargs * sizeof(Oid));
// 		newResult->pathpos = pathpos;
// 		newResult->oid = HeapTupleGetOid(proctup);
// 		newResult->nargs = effective_nargs;
// 		newResult->argnumbers = argnumbers;
// 		if (argnumbers)
// 		{
// 			/* Re-order the argument types into call's logical order */
// 			Oid		   *proargtypes = procform->proargtypes.values;
// 			int			i;

// 			for (i = 0; i < pronargs; i++)
// 				newResult->args[i] = proargtypes[argnumbers[i]];
// 		}
// 		else
// 		{
// 			/* Simple positional case, just copy proargtypes as-is */
// 			memcpy(newResult->args, procform->proargtypes.values,
// 				   pronargs * sizeof(Oid));
// 		}
// 		if (variadic)
// 		{
// 			int			i;

// 			newResult->nvargs = effective_nargs - pronargs + 1;
// 			/* Expand variadic argument into N copies of element type */
// 			for (i = pronargs - 1; i < effective_nargs; i++)
// 				newResult->args[i] = va_elem_type;
// 		}
// 		else
// 			newResult->nvargs = 0;
// 		newResult->ndargs = use_defaults ? pronargs - nargs : 0;

// 		/*
// 		 * Does it have the same arguments as something we already accepted?
// 		 * If so, decide what to do to avoid returning duplicate argument
// 		 * lists.  We can skip this check for the single-namespace case if no
// 		 * special (named, variadic or defaults) match has been made, since
// 		 * then the unique index on pg_proc guarantees all the matches have
// 		 * different argument lists.
// 		 */
// 		if (resultList != NULL &&
// 			(any_special || !OidIsValid(namespaceId)))
// 		{
// 			/*
// 			 * If we have an ordered list from SearchSysCacheList (the normal
// 			 * case), then any conflicting proc must immediately adjoin this
// 			 * one in the list, so we only need to look at the newest result
// 			 * item.  If we have an unordered list, we have to scan the whole
// 			 * result list.  Also, if either the current candidate or any
// 			 * previous candidate is a special match, we can't assume that
// 			 * conflicts are adjacent.
// 			 *
// 			 * We ignore defaulted arguments in deciding what is a match.
// 			 */
// 			FuncCandidateList prevResult;

// 			if (catlist->ordered && !any_special)
// 			{
// 				/* ndargs must be 0 if !any_special */
// 				if (effective_nargs == resultList->nargs &&
// 					memcmp(newResult->args,
// 						   resultList->args,
// 						   effective_nargs * sizeof(Oid)) == 0)
// 					prevResult = resultList;
// 				else
// 					prevResult = NULL;
// 			}
// 			else
// 			{
// 				int			cmp_nargs = newResult->nargs - newResult->ndargs;

// 				for (prevResult = resultList;
// 					 prevResult;
// 					 prevResult = prevResult->next)
// 				{
// 					if (cmp_nargs == prevResult->nargs - prevResult->ndargs &&
// 						memcmp(newResult->args,
// 							   prevResult->args,
// 							   cmp_nargs * sizeof(Oid)) == 0)
// 						break;
// 				}
// 			}

// 			if (prevResult)
// 			{
// 				/*
// 				 * We have a match with a previous result.  Decide which one
// 				 * to keep, or mark it ambiguous if we can't decide.  The
// 				 * logic here is preference > 0 means prefer the old result,
// 				 * preference < 0 means prefer the new, preference = 0 means
// 				 * ambiguous.
// 				 */
// 				int			preference;

// 				if (pathpos != prevResult->pathpos)
// 				{
// 					/*
// 					 * Prefer the one that's earlier in the search path.
// 					 */
// 					preference = pathpos - prevResult->pathpos;
// 				}
// 				else if (variadic && prevResult->nvargs == 0)
// 				{
// 					/*
// 					 * With variadic functions we could have, for example,
// 					 * both foo(numeric) and foo(variadic numeric[]) in the
// 					 * same namespace; if so we prefer the non-variadic match
// 					 * on efficiency grounds.
// 					 */
// 					preference = 1;
// 				}
// 				else if (!variadic && prevResult->nvargs > 0)
// 				{
// 					preference = -1;
// 				}
// 				else
// 				{
// 					/*----------
// 					 * We can't decide.  This can happen with, for example,
// 					 * both foo(numeric, variadic numeric[]) and
// 					 * foo(variadic numeric[]) in the same namespace, or
// 					 * both foo(int) and foo (int, int default something)
// 					 * in the same namespace, or both foo(a int, b text)
// 					 * and foo(b text, a int) in the same namespace.
// 					 *----------
// 					 */
// 					preference = 0;
// 				}

// 				if (preference > 0)
// 				{
// 					/* keep previous result */
// 					pfree(newResult);
// 					continue;
// 				}
// 				else if (preference < 0)
// 				{
// 					/* remove previous result from the list */
// 					if (prevResult == resultList)
// 						resultList = prevResult->next;
// 					else
// 					{
// 						FuncCandidateList prevPrevResult;

// 						for (prevPrevResult = resultList;
// 							 prevPrevResult;
// 							 prevPrevResult = prevPrevResult->next)
// 						{
// 							if (prevResult == prevPrevResult->next)
// 							{
// 								prevPrevResult->next = prevResult->next;
// 								break;
// 							}
// 						}
// 						Assert(prevPrevResult); /* assert we found it */
// 					}
// 					pfree(prevResult);
// 					/* fall through to add newResult to list */
// 				}
// 				else
// 				{
// 					/* mark old result as ambiguous, discard new */
// 					prevResult->oid = InvalidOid;
// 					pfree(newResult);
// 					continue;
// 				}
// 			}
// 		}

// 		/*
// 		 * Okay to add it to result list
// 		 */
// 		newResult->next = resultList;
// 		resultList = newResult;
// 	}

// 	ReleaseSysCacheList(catlist);

// 	return resultList;
// }
// FuncCandidateList
// OpernameGetCandidates(List *names, char oprkind, bool missing_schema_ok)
// {
// 	FuncCandidateList resultList = NULL;
// 	char	   *resultSpace = NULL;
// 	int			nextResult = 0;
// 	char	   *schemaname;
// 	char	   *opername;
// 	Oid			namespaceId;
// 	CatCList   *catlist;
// 	int			i;

// 	/* deconstruct the name list */
// 	DeconstructQualifiedName(names, &schemaname, &opername);

// 	if (schemaname)
// 	{
// 		/* use exact schema given */
// 		namespaceId = LookupExplicitNamespace(schemaname, missing_schema_ok);
// 		if (missing_schema_ok && !OidIsValid(namespaceId))
// 			return NULL;
// 	}
// 	else
// 	{
// 		/* flag to indicate we need namespace search */
// 		namespaceId = InvalidOid;
// 		recomputeNamespacePath();
// 	}

// 	/* Search syscache by name only */
// 	catlist = SearchSysCacheList1(OPERNAMENSP, CStringGetDatum(opername));

// 	/*
// 	 * In typical scenarios, most if not all of the operators found by the
// 	 * catcache search will end up getting returned; and there can be quite a
// 	 * few, for common operator names such as '=' or '+'.  To reduce the time
// 	 * spent in palloc, we allocate the result space as an array large enough
// 	 * to hold all the operators.  The original coding of this routine did a
// 	 * separate palloc for each operator, but profiling revealed that the
// 	 * pallocs used an unreasonably large fraction of parsing time.
// 	 */
// #define SPACE_PER_OP MAXALIGN(offsetof(struct _FuncCandidateList, args) + \
// 							  2 * sizeof(Oid))

// 	if (catlist->n_members > 0)
// 		resultSpace = palloc(catlist->n_members * SPACE_PER_OP);

// 	for (i = 0; i < catlist->n_members; i++)
// 	{
// 		HeapTuple	opertup = &catlist->members[i]->tuple;
// 		Form_pg_operator operform = (Form_pg_operator) GETSTRUCT(opertup);
// 		int			pathpos = 0;
// 		FuncCandidateList newResult;

// 		/* Ignore operators of wrong kind, if specific kind requested */
// 		if (oprkind && operform->oprkind != oprkind)
// 			continue;

// 		if (OidIsValid(namespaceId))
// 		{
// 			/* Consider only opers in specified namespace */
// 			if (operform->oprnamespace != namespaceId)
// 				continue;
// 			/* No need to check args, they must all be different */
// 		}
// 		else
// 		{
// 			/*
// 			 * Consider only opers that are in the search path and are not in
// 			 * the temp namespace.
// 			 */
// 			ListCell   *nsp;

// 			foreach(nsp, activeSearchPath)
// 			{
// 				if (operform->oprnamespace == lfirst_oid(nsp) &&
// 					operform->oprnamespace != myTempNamespace)
// 					break;
// 				pathpos++;
// 			}
// 			if (nsp == NULL)
// 				continue;		/* oper is not in search path */

// 			/*
// 			 * Okay, it's in the search path, but does it have the same
// 			 * arguments as something we already accepted?	If so, keep only
// 			 * the one that appears earlier in the search path.
// 			 *
// 			 * If we have an ordered list from SearchSysCacheList (the normal
// 			 * case), then any conflicting oper must immediately adjoin this
// 			 * one in the list, so we only need to look at the newest result
// 			 * item.  If we have an unordered list, we have to scan the whole
// 			 * result list.
// 			 */
// 			if (resultList)
// 			{
// 				FuncCandidateList prevResult;

// 				if (catlist->ordered)
// 				{
// 					if (operform->oprleft == resultList->args[0] &&
// 						operform->oprright == resultList->args[1])
// 						prevResult = resultList;
// 					else
// 						prevResult = NULL;
// 				}
// 				else
// 				{
// 					for (prevResult = resultList;
// 						 prevResult;
// 						 prevResult = prevResult->next)
// 					{
// 						if (operform->oprleft == prevResult->args[0] &&
// 							operform->oprright == prevResult->args[1])
// 							break;
// 					}
// 				}
// 				if (prevResult)
// 				{
// 					/* We have a match with a previous result */
// 					Assert(pathpos != prevResult->pathpos);
// 					if (pathpos > prevResult->pathpos)
// 						continue;		/* keep previous result */
// 					/* replace previous result */
// 					prevResult->pathpos = pathpos;
// 					prevResult->oid = HeapTupleGetOid(opertup);
// 					continue;	/* args are same, of course */
// 				}
// 			}
// 		}

// 		/*
// 		 * Okay to add it to result list
// 		 */
// 		newResult = (FuncCandidateList) (resultSpace + nextResult);
// 		nextResult += SPACE_PER_OP;

// 		newResult->pathpos = pathpos;
// 		newResult->oid = HeapTupleGetOid(opertup);
// 		newResult->nargs = 2;
// 		newResult->nvargs = 0;
// 		newResult->ndargs = 0;
// 		newResult->argnumbers = NULL;
// 		newResult->args[0] = operform->oprleft;
// 		newResult->args[1] = operform->oprright;
// 		newResult->next = resultList;
// 		resultList = newResult;
// 	}

// 	ReleaseSysCacheList(catlist);

// 	return resultList;
// }
// int
// GetTempNamespaceBackendId(Oid namespaceId)
// {
// 	int			result;
// 	char	   *nspname;

// 	/* See if the namespace name starts with "pg_temp_" or "pg_toast_temp_" */
// 	nspname = get_namespace_name(namespaceId);
// 	if (!nspname)
// 		return InvalidBackendId;	/* no such namespace? */
// 	if (strncmp(nspname, "pg_temp_", 8) == 0)
// 		result = atoi(nspname + 8);
// 	else if (strncmp(nspname, "pg_toast_temp_", 14) == 0)
// 		result = atoi(nspname + 14);
// 	else
// 		result = InvalidBackendId;
// 	pfree(nspname);
// 	return result;
// }
// void
// GetTempNamespaceState(Oid *tempNamespaceId, Oid *tempToastNamespaceId)
// {
// 	/* Return namespace OIDs, or 0 if session has not created temp namespace */
// 	*tempNamespaceId = myTempNamespace;
// 	*tempToastNamespaceId = myTempToastNamespace;
// }
// void
// SetTempNamespaceState(Oid tempNamespaceId, Oid tempToastNamespaceId)
// {
// 	/* Worker should not have created its own namespaces ... */
// 	Assert(myTempNamespace == InvalidOid);
// 	Assert(myTempToastNamespace == InvalidOid);
// 	Assert(myTempNamespaceSubID == InvalidSubTransactionId);

// 	/* Assign same namespace OIDs that leader has */
// 	myTempNamespace = tempNamespaceId;
// 	myTempToastNamespace = tempToastNamespaceId;

// 	/*
// 	 * It's fine to leave myTempNamespaceSubID == InvalidSubTransactionId.
// 	 * Even if the namespace is new so far as the leader is concerned, it's
// 	 * not new to the worker, and we certainly wouldn't want the worker trying
// 	 * to destroy it.
// 	 */

// 	baseSearchPathValid = false;	/* may need to rebuild list */
// }
// void
// AtEOXact_Namespace(bool isCommit, bool parallel)
// {
// 	/*
// 	 * If we abort the transaction in which a temp namespace was selected,
// 	 * we'll have to do any creation or cleanout work over again.  So, just
// 	 * forget the namespace entirely until next time.  On the other hand, if
// 	 * we commit then register an exit callback to clean out the temp tables
// 	 * at backend shutdown.  (We only want to register the callback once per
// 	 * session, so this is a good place to do it.)
// 	 */
// 	if (myTempNamespaceSubID != InvalidSubTransactionId && !parallel)
// 	{
// 		if (isCommit)
// 			before_shmem_exit(RemoveTempRelationsCallback, 0);
// 		else
// 		{
// 			myTempNamespace = InvalidOid;
// 			myTempToastNamespace = InvalidOid;
// 			baseSearchPathValid = false;		/* need to rebuild list */
// 		}
// 		myTempNamespaceSubID = InvalidSubTransactionId;
// 	}

// 	/*
// 	 * Clean up if someone failed to do PopOverrideSearchPath
// 	 */
// 	if (overrideStack)
// 	{
// 		if (isCommit)
// 			elog(WARNING, "leaked override search path");
// 		while (overrideStack)
// 		{
// 			OverrideStackEntry *entry;

// 			entry = (OverrideStackEntry *) linitial(overrideStack);
// 			overrideStack = list_delete_first(overrideStack);
// 			list_free(entry->searchPath);
// 			pfree(entry);
// 		}
// 		/* If not baseSearchPathValid, this is useless but harmless */
// 		activeSearchPath = baseSearchPath;
// 		activeCreationNamespace = baseCreationNamespace;
// 		activeTempCreationPending = baseTempCreationPending;
// 	}
// }
// Oid
// FindDefaultConversionProc(int32 for_encoding, int32 to_encoding)
// {
// 	Oid			proc;
// 	ListCell   *l;

// 	recomputeNamespacePath();

// 	foreach(l, activeSearchPath)
// 	{
// 		Oid			namespaceId = lfirst_oid(l);

// 		if (namespaceId == myTempNamespace)
// 			continue;			/* do not look in temp namespace */

// 		proc = FindDefaultConversion(namespaceId, for_encoding, to_encoding);
// 		if (OidIsValid(proc))
// 			return proc;
// 	}

// 	/* Not found in path */
// 	return InvalidOid;
// }
// ObjectAddress
// get_object_address_rv(ObjectType objtype, RangeVar *rel, List *objname,
// 					  List *objargs, Relation *relp, LOCKMODE lockmode,
// 					  bool missing_ok)
// {
// 	if (rel)
// 	{
// 		objname = lcons(makeString(rel->relname), objname);
// 		if (rel->schemaname)
// 			objname = lcons(makeString(rel->schemaname), objname);
// 		if (rel->catalogname)
// 			objname = lcons(makeString(rel->catalogname), objname);
// 	}

// 	return get_object_address(objtype, objname, objargs,
// 							  relp, lockmode, missing_ok);
// }
// bool
// is_objectclass_supported(Oid class_id)
// {
// 	int			index;

// 	for (index = 0; index < lengthof(ObjectProperty); index++)
// 	{
// 		if (ObjectProperty[index].class_oid == class_id)
// 			return true;
// 	}

// 	return false;
// }
// Oid
// get_object_oid_index(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->oid_index_oid;
// }
// int
// get_object_catcache_oid(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->oid_catcache_id;
// }
// int
// get_object_catcache_name(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->name_catcache_id;
// }
// AttrNumber
// get_object_attnum_name(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->attnum_name;
// }
// AttrNumber
// get_object_attnum_namespace(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->attnum_namespace;
// }

// AttrNumber
// get_object_attnum_owner(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->attnum_owner;
// }

// AttrNumber
// get_object_attnum_acl(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->attnum_acl;
// }

// AclObjectKind
// get_object_aclkind(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->acl_kind;
// }
// bool
// get_object_namensp_unique(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->is_nsp_name_unique;
// }
// HeapTuple
// get_catalog_object_by_oid(Relation catalog, Oid objectId)
// {
// 	HeapTuple	tuple;
// 	Oid			classId = RelationGetRelid(catalog);
// 	int			oidCacheId = get_object_catcache_oid(classId);

// 	if (oidCacheId > 0)
// 	{
// 		tuple = SearchSysCacheCopy1(oidCacheId, ObjectIdGetDatum(objectId));
// 		if (!HeapTupleIsValid(tuple))	/* should not happen */
// 			return NULL;
// 	}
// 	else
// 	{
// 		Oid			oidIndexId = get_object_oid_index(classId);
// 		SysScanDesc scan;
// 		ScanKeyData skey;

// 		Assert(OidIsValid(oidIndexId));

// 		ScanKeyInit(&skey,
// 					ObjectIdAttributeNumber,
// 					BTEqualStrategyNumber, F_OIDEQ,
// 					ObjectIdGetDatum(objectId));

// 		scan = systable_beginscan(catalog, oidIndexId, true,
// 								  NULL, 1, &skey);
// 		tuple = systable_getnext(scan);
// 		if (!HeapTupleIsValid(tuple))
// 		{
// 			systable_endscan(scan);
// 			return NULL;
// 		}
// 		tuple = heap_copytuple(tuple);

// 		systable_endscan(scan);
// 	}

// 	return tuple;
// }
// char *
// getObjectDescription(const ObjectAddress *object)
// {
// 	StringInfoData buffer;

// 	initStringInfo(&buffer);

// 	switch (getObjectClass(object))
// 	{
// 		case OCLASS_CLASS:
// 			getRelationDescription(&buffer, object->objectId);
// 			if (object->objectSubId != 0)
// 				appendStringInfo(&buffer, _(" column %s"),
// 								 get_relid_attribute_name(object->objectId,
// 													   object->objectSubId));
// 			break;

// 		case OCLASS_PROC:
// 			appendStringInfo(&buffer, _("function %s"),
// 							 format_procedure(object->objectId));
// 			break;

// 		case OCLASS_TYPE:
// 			appendStringInfo(&buffer, _("type %s"),
// 							 format_type_be(object->objectId));
// 			break;

// 		case OCLASS_CAST:
// 			{
// 				Relation	castDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc rcscan;
// 				HeapTuple	tup;
// 				Form_pg_cast castForm;

// 				castDesc = heap_open(CastRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				rcscan = systable_beginscan(castDesc, CastOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(rcscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for cast %u",
// 						 object->objectId);

// 				castForm = (Form_pg_cast) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, _("cast from %s to %s"),
// 								 format_type_be(castForm->castsource),
// 								 format_type_be(castForm->casttarget));

// 				systable_endscan(rcscan);
// 				heap_close(castDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_COLLATION:
// 			{
// 				HeapTuple	collTup;
// 				Form_pg_collation coll;

// 				collTup = SearchSysCache1(COLLOID,
// 										  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(collTup))
// 					elog(ERROR, "cache lookup failed for collation %u",
// 						 object->objectId);
// 				coll = (Form_pg_collation) GETSTRUCT(collTup);
// 				appendStringInfo(&buffer, _("collation %s"),
// 								 NameStr(coll->collname));
// 				ReleaseSysCache(collTup);
// 				break;
// 			}

// 		case OCLASS_CONSTRAINT:
// 			{
// 				HeapTuple	conTup;
// 				Form_pg_constraint con;

// 				conTup = SearchSysCache1(CONSTROID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(conTup))
// 					elog(ERROR, "cache lookup failed for constraint %u",
// 						 object->objectId);
// 				con = (Form_pg_constraint) GETSTRUCT(conTup);

// 				if (OidIsValid(con->conrelid))
// 				{
// 					StringInfoData rel;

// 					initStringInfo(&rel);
// 					getRelationDescription(&rel, con->conrelid);
// 					appendStringInfo(&buffer, _("constraint %s on %s"),
// 									 NameStr(con->conname), rel.data);
// 					pfree(rel.data);
// 				}
// 				else
// 				{
// 					appendStringInfo(&buffer, _("constraint %s"),
// 									 NameStr(con->conname));
// 				}

// 				ReleaseSysCache(conTup);
// 				break;
// 			}

// 		case OCLASS_CONVERSION:
// 			{
// 				HeapTuple	conTup;

// 				conTup = SearchSysCache1(CONVOID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(conTup))
// 					elog(ERROR, "cache lookup failed for conversion %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("conversion %s"),
// 				 NameStr(((Form_pg_conversion) GETSTRUCT(conTup))->conname));
// 				ReleaseSysCache(conTup);
// 				break;
// 			}

// 		case OCLASS_DEFAULT:
// 			{
// 				Relation	attrdefDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc adscan;
// 				HeapTuple	tup;
// 				Form_pg_attrdef attrdef;
// 				ObjectAddress colobject;

// 				attrdefDesc = heap_open(AttrDefaultRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				adscan = systable_beginscan(attrdefDesc, AttrDefaultOidIndexId,
// 											true, NULL, 1, skey);

// 				tup = systable_getnext(adscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for attrdef %u",
// 						 object->objectId);

// 				attrdef = (Form_pg_attrdef) GETSTRUCT(tup);

// 				colobject.classId = RelationRelationId;
// 				colobject.objectId = attrdef->adrelid;
// 				colobject.objectSubId = attrdef->adnum;

// 				appendStringInfo(&buffer, _("default for %s"),
// 								 getObjectDescription(&colobject));

// 				systable_endscan(adscan);
// 				heap_close(attrdefDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_LANGUAGE:
// 			appendStringInfo(&buffer, _("language %s"),
// 							 get_language_name(object->objectId, false));
// 			break;

// 		case OCLASS_LARGEOBJECT:
// 			appendStringInfo(&buffer, _("large object %u"),
// 							 object->objectId);
// 			break;

// 		case OCLASS_OPERATOR:
// 			appendStringInfo(&buffer, _("operator %s"),
// 							 format_operator(object->objectId));
// 			break;

// 		case OCLASS_OPCLASS:
// 			{
// 				HeapTuple	opcTup;
// 				Form_pg_opclass opcForm;
// 				HeapTuple	amTup;
// 				Form_pg_am	amForm;
// 				char	   *nspname;

// 				opcTup = SearchSysCache1(CLAOID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(opcTup))
// 					elog(ERROR, "cache lookup failed for opclass %u",
// 						 object->objectId);
// 				opcForm = (Form_pg_opclass) GETSTRUCT(opcTup);

// 				amTup = SearchSysCache1(AMOID,
// 										ObjectIdGetDatum(opcForm->opcmethod));
// 				if (!HeapTupleIsValid(amTup))
// 					elog(ERROR, "cache lookup failed for access method %u",
// 						 opcForm->opcmethod);
// 				amForm = (Form_pg_am) GETSTRUCT(amTup);

// 				/* Qualify the name if not visible in search path */
// 				if (OpclassIsVisible(object->objectId))
// 					nspname = NULL;
// 				else
// 					nspname = get_namespace_name(opcForm->opcnamespace);

// 				appendStringInfo(&buffer, _("operator class %s for access method %s"),
// 								 quote_qualified_identifier(nspname,
// 												  NameStr(opcForm->opcname)),
// 								 NameStr(amForm->amname));

// 				ReleaseSysCache(amTup);
// 				ReleaseSysCache(opcTup);
// 				break;
// 			}

// 		case OCLASS_OPFAMILY:
// 			getOpFamilyDescription(&buffer, object->objectId);
// 			break;

// 		case OCLASS_AMOP:
// 			{
// 				Relation	amopDesc;
// 				HeapTuple	tup;
// 				ScanKeyData skey[1];
// 				SysScanDesc amscan;
// 				Form_pg_amop amopForm;
// 				StringInfoData opfam;

// 				amopDesc = heap_open(AccessMethodOperatorRelationId,
// 									 AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				amscan = systable_beginscan(amopDesc, AccessMethodOperatorOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(amscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for amop entry %u",
// 						 object->objectId);

// 				amopForm = (Form_pg_amop) GETSTRUCT(tup);

// 				initStringInfo(&opfam);
// 				getOpFamilyDescription(&opfam, amopForm->amopfamily);

// 				/*------
// 				   translator: %d is the operator strategy (a number), the
// 				   first two %s's are data type names, the third %s is the
// 				   description of the operator family, and the last %s is the
// 				   textual form of the operator with arguments.  */
// 				appendStringInfo(&buffer, _("operator %d (%s, %s) of %s: %s"),
// 								 amopForm->amopstrategy,
// 								 format_type_be(amopForm->amoplefttype),
// 								 format_type_be(amopForm->amoprighttype),
// 								 opfam.data,
// 								 format_operator(amopForm->amopopr));

// 				pfree(opfam.data);

// 				systable_endscan(amscan);
// 				heap_close(amopDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_AMPROC:
// 			{
// 				Relation	amprocDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc amscan;
// 				HeapTuple	tup;
// 				Form_pg_amproc amprocForm;
// 				StringInfoData opfam;

// 				amprocDesc = heap_open(AccessMethodProcedureRelationId,
// 									   AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				amscan = systable_beginscan(amprocDesc, AccessMethodProcedureOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(amscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for amproc entry %u",
// 						 object->objectId);

// 				amprocForm = (Form_pg_amproc) GETSTRUCT(tup);

// 				initStringInfo(&opfam);
// 				getOpFamilyDescription(&opfam, amprocForm->amprocfamily);

// 				/*------
// 				   translator: %d is the function number, the first two %s's
// 				   are data type names, the third %s is the description of the
// 				   operator family, and the last %s is the textual form of the
// 				   function with arguments.  */
// 				appendStringInfo(&buffer, _("function %d (%s, %s) of %s: %s"),
// 								 amprocForm->amprocnum,
// 								 format_type_be(amprocForm->amproclefttype),
// 								 format_type_be(amprocForm->amprocrighttype),
// 								 opfam.data,
// 								 format_procedure(amprocForm->amproc));

// 				pfree(opfam.data);

// 				systable_endscan(amscan);
// 				heap_close(amprocDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_REWRITE:
// 			{
// 				Relation	ruleDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc rcscan;
// 				HeapTuple	tup;
// 				Form_pg_rewrite rule;

// 				ruleDesc = heap_open(RewriteRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				rcscan = systable_beginscan(ruleDesc, RewriteOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(rcscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for rule %u",
// 						 object->objectId);

// 				rule = (Form_pg_rewrite) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, _("rule %s on "),
// 								 NameStr(rule->rulename));
// 				getRelationDescription(&buffer, rule->ev_class);

// 				systable_endscan(rcscan);
// 				heap_close(ruleDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_TRANSFORM:
// 			{
// 				HeapTuple	trfTup;
// 				Form_pg_transform trfForm;

// 				trfTup = SearchSysCache1(TRFOID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(trfTup))
// 					elog(ERROR, "could not find tuple for transform %u",
// 						 object->objectId);

// 				trfForm = (Form_pg_transform) GETSTRUCT(trfTup);

// 				appendStringInfo(&buffer, _("transform for %s language %s"),
// 								 format_type_be(trfForm->trftype),
// 								 get_language_name(trfForm->trflang, false));

// 				ReleaseSysCache(trfTup);
// 				break;
// 			}

// 		case OCLASS_TRIGGER:
// 			{
// 				Relation	trigDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc tgscan;
// 				HeapTuple	tup;
// 				Form_pg_trigger trig;

// 				trigDesc = heap_open(TriggerRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				tgscan = systable_beginscan(trigDesc, TriggerOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(tgscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for trigger %u",
// 						 object->objectId);

// 				trig = (Form_pg_trigger) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, _("trigger %s on "),
// 								 NameStr(trig->tgname));
// 				getRelationDescription(&buffer, trig->tgrelid);

// 				systable_endscan(tgscan);
// 				heap_close(trigDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_SCHEMA:
// 			{
// 				char	   *nspname;

// 				nspname = get_namespace_name(object->objectId);
// 				if (!nspname)
// 					elog(ERROR, "cache lookup failed for namespace %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("schema %s"), nspname);
// 				break;
// 			}

// 		case OCLASS_TSPARSER:
// 			{
// 				HeapTuple	tup;

// 				tup = SearchSysCache1(TSPARSEROID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search parser %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("text search parser %s"),
// 					 NameStr(((Form_pg_ts_parser) GETSTRUCT(tup))->prsname));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TSDICT:
// 			{
// 				HeapTuple	tup;

// 				tup = SearchSysCache1(TSDICTOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search dictionary %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("text search dictionary %s"),
// 					  NameStr(((Form_pg_ts_dict) GETSTRUCT(tup))->dictname));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TSTEMPLATE:
// 			{
// 				HeapTuple	tup;

// 				tup = SearchSysCache1(TSTEMPLATEOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search template %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("text search template %s"),
// 				  NameStr(((Form_pg_ts_template) GETSTRUCT(tup))->tmplname));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TSCONFIG:
// 			{
// 				HeapTuple	tup;

// 				tup = SearchSysCache1(TSCONFIGOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search configuration %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("text search configuration %s"),
// 					 NameStr(((Form_pg_ts_config) GETSTRUCT(tup))->cfgname));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_ROLE:
// 			{
// 				appendStringInfo(&buffer, _("role %s"),
// 								 GetUserNameFromId(object->objectId, false));
// 				break;
// 			}

// 		case OCLASS_DATABASE:
// 			{
// 				char	   *datname;

// 				datname = get_database_name(object->objectId);
// 				if (!datname)
// 					elog(ERROR, "cache lookup failed for database %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("database %s"), datname);
// 				break;
// 			}

// 		case OCLASS_TBLSPACE:
// 			{
// 				char	   *tblspace;

// 				tblspace = get_tablespace_name(object->objectId);
// 				if (!tblspace)
// 					elog(ERROR, "cache lookup failed for tablespace %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("tablespace %s"), tblspace);
// 				break;
// 			}

// 		case OCLASS_FDW:
// 			{
// 				ForeignDataWrapper *fdw;

// 				fdw = GetForeignDataWrapper(object->objectId);
// 				appendStringInfo(&buffer, _("foreign-data wrapper %s"), fdw->fdwname);
// 				break;
// 			}

// 		case OCLASS_FOREIGN_SERVER:
// 			{
// 				ForeignServer *srv;

// 				srv = GetForeignServer(object->objectId);
// 				appendStringInfo(&buffer, _("server %s"), srv->servername);
// 				break;
// 			}

// 		case OCLASS_USER_MAPPING:
// 			{
// 				HeapTuple	tup;
// 				Oid			useid;
// 				char	   *usename;
// 				Form_pg_user_mapping umform;
// 				ForeignServer *srv;

// 				tup = SearchSysCache1(USERMAPPINGOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for user mapping %u",
// 						 object->objectId);
// 				umform = (Form_pg_user_mapping) GETSTRUCT(tup);
// 				useid = umform->umuser;
// 				srv = GetForeignServer(umform->umserver);

// 				ReleaseSysCache(tup);

// 				if (OidIsValid(useid))
// 					usename = GetUserNameFromId(useid, false);
// 				else
// 					usename = "public";

// 				appendStringInfo(&buffer, _("user mapping for %s on server %s"), usename,
// 								 srv->servername);
// 				break;
// 			}

// 		case OCLASS_DEFACL:
// 			{
// 				Relation	defaclrel;
// 				ScanKeyData skey[1];
// 				SysScanDesc rcscan;
// 				HeapTuple	tup;
// 				Form_pg_default_acl defacl;

// 				defaclrel = heap_open(DefaultAclRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				rcscan = systable_beginscan(defaclrel, DefaultAclOidIndexId,
// 											true, NULL, 1, skey);

// 				tup = systable_getnext(rcscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for default ACL %u",
// 						 object->objectId);

// 				defacl = (Form_pg_default_acl) GETSTRUCT(tup);

// 				switch (defacl->defaclobjtype)
// 				{
// 					case DEFACLOBJ_RELATION:
// 						appendStringInfo(&buffer,
// 										 _("default privileges on new relations belonging to role %s"),
// 							   GetUserNameFromId(defacl->defaclrole, false));
// 						break;
// 					case DEFACLOBJ_SEQUENCE:
// 						appendStringInfo(&buffer,
// 										 _("default privileges on new sequences belonging to role %s"),
// 							   GetUserNameFromId(defacl->defaclrole, false));
// 						break;
// 					case DEFACLOBJ_FUNCTION:
// 						appendStringInfo(&buffer,
// 										 _("default privileges on new functions belonging to role %s"),
// 							   GetUserNameFromId(defacl->defaclrole, false));
// 						break;
// 					case DEFACLOBJ_TYPE:
// 						appendStringInfo(&buffer,
// 										 _("default privileges on new types belonging to role %s"),
// 							   GetUserNameFromId(defacl->defaclrole, false));
// 						break;
// 					default:
// 						/* shouldn't get here */
// 						appendStringInfo(&buffer,
// 								_("default privileges belonging to role %s"),
// 							   GetUserNameFromId(defacl->defaclrole, false));
// 						break;
// 				}

// 				if (OidIsValid(defacl->defaclnamespace))
// 				{
// 					appendStringInfo(&buffer,
// 									 _(" in schema %s"),
// 								get_namespace_name(defacl->defaclnamespace));
// 				}

// 				systable_endscan(rcscan);
// 				heap_close(defaclrel, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_EXTENSION:
// 			{
// 				char	   *extname;

// 				extname = get_extension_name(object->objectId);
// 				if (!extname)
// 					elog(ERROR, "cache lookup failed for extension %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("extension %s"), extname);
// 				break;
// 			}

// 		case OCLASS_EVENT_TRIGGER:
// 			{
// 				HeapTuple	tup;

// 				tup = SearchSysCache1(EVENTTRIGGEROID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for event trigger %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("event trigger %s"),
// 				 NameStr(((Form_pg_event_trigger) GETSTRUCT(tup))->evtname));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_POLICY:
// 			{
// 				Relation	policy_rel;
// 				ScanKeyData skey[1];
// 				SysScanDesc sscan;
// 				HeapTuple	tuple;
// 				Form_pg_policy form_policy;

// 				policy_rel = heap_open(PolicyRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				sscan = systable_beginscan(policy_rel, PolicyOidIndexId,
// 										   true, NULL, 1, skey);

// 				tuple = systable_getnext(sscan);

// 				if (!HeapTupleIsValid(tuple))
// 					elog(ERROR, "cache lookup failed for policy %u",
// 						 object->objectId);

// 				form_policy = (Form_pg_policy) GETSTRUCT(tuple);

// 				appendStringInfo(&buffer, _("policy %s on "),
// 								 NameStr(form_policy->polname));
// 				getRelationDescription(&buffer, form_policy->polrelid);

// 				systable_endscan(sscan);
// 				heap_close(policy_rel, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_AM:
// 			{
// 				HeapTuple	tup;

// 				tup = SearchSysCache1(AMOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for access method %u",
// 						 object->objectId);
// 				appendStringInfo(&buffer, _("access method %s"),
// 							 NameStr(((Form_pg_am) GETSTRUCT(tup))->amname));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		default:
// 			appendStringInfo(&buffer, "unrecognized object %u %u %d",
// 							 object->classId,
// 							 object->objectId,
// 							 object->objectSubId);
// 			break;
// 	}

// 	return buffer.data;
// }
// char *
// getObjectDescriptionOids(Oid classid, Oid objid)
// {
// 	ObjectAddress address;

// 	address.classId = classid;
// 	address.objectId = objid;
// 	address.objectSubId = 0;

// 	return getObjectDescription(&address);
// }
// int
// read_objtype_from_string(const char *objtype)
// {
// 	int			i;

// 	for (i = 0; i < lengthof(ObjectTypeMap); i++)
// 	{
// 		if (strcmp(ObjectTypeMap[i].tm_name, objtype) == 0)
// 			return ObjectTypeMap[i].tm_type;
// 	}
// 	ereport(ERROR,
// 			(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 			 errmsg("unrecognized object type \"%s\"", objtype)));

// 	return -1;					/* keep compiler quiet */
// }
// char *
// getObjectTypeDescription(const ObjectAddress *object)
// {
// 	StringInfoData buffer;

// 	initStringInfo(&buffer);

// 	switch (getObjectClass(object))
// 	{
// 		case OCLASS_CLASS:
// 			getRelationTypeDescription(&buffer, object->objectId,
// 									   object->objectSubId);
// 			break;

// 		case OCLASS_PROC:
// 			getProcedureTypeDescription(&buffer, object->objectId);
// 			break;

// 		case OCLASS_TYPE:
// 			appendStringInfoString(&buffer, "type");
// 			break;

// 		case OCLASS_CAST:
// 			appendStringInfoString(&buffer, "cast");
// 			break;

// 		case OCLASS_COLLATION:
// 			appendStringInfoString(&buffer, "collation");
// 			break;

// 		case OCLASS_CONSTRAINT:
// 			getConstraintTypeDescription(&buffer, object->objectId);
// 			break;

// 		case OCLASS_CONVERSION:
// 			appendStringInfoString(&buffer, "conversion");
// 			break;

// 		case OCLASS_DEFAULT:
// 			appendStringInfoString(&buffer, "default value");
// 			break;

// 		case OCLASS_LANGUAGE:
// 			appendStringInfoString(&buffer, "language");
// 			break;

// 		case OCLASS_LARGEOBJECT:
// 			appendStringInfoString(&buffer, "large object");
// 			break;

// 		case OCLASS_OPERATOR:
// 			appendStringInfoString(&buffer, "operator");
// 			break;

// 		case OCLASS_OPCLASS:
// 			appendStringInfoString(&buffer, "operator class");
// 			break;

// 		case OCLASS_OPFAMILY:
// 			appendStringInfoString(&buffer, "operator family");
// 			break;

// 		case OCLASS_AMOP:
// 			appendStringInfoString(&buffer, "operator of access method");
// 			break;

// 		case OCLASS_AMPROC:
// 			appendStringInfoString(&buffer, "function of access method");
// 			break;

// 		case OCLASS_REWRITE:
// 			appendStringInfoString(&buffer, "rule");
// 			break;

// 		case OCLASS_TRIGGER:
// 			appendStringInfoString(&buffer, "trigger");
// 			break;

// 		case OCLASS_SCHEMA:
// 			appendStringInfoString(&buffer, "schema");
// 			break;

// 		case OCLASS_TSPARSER:
// 			appendStringInfoString(&buffer, "text search parser");
// 			break;

// 		case OCLASS_TSDICT:
// 			appendStringInfoString(&buffer, "text search dictionary");
// 			break;

// 		case OCLASS_TSTEMPLATE:
// 			appendStringInfoString(&buffer, "text search template");
// 			break;

// 		case OCLASS_TSCONFIG:
// 			appendStringInfoString(&buffer, "text search configuration");
// 			break;

// 		case OCLASS_ROLE:
// 			appendStringInfoString(&buffer, "role");
// 			break;

// 		case OCLASS_DATABASE:
// 			appendStringInfoString(&buffer, "database");
// 			break;

// 		case OCLASS_TBLSPACE:
// 			appendStringInfoString(&buffer, "tablespace");
// 			break;

// 		case OCLASS_FDW:
// 			appendStringInfoString(&buffer, "foreign-data wrapper");
// 			break;

// 		case OCLASS_FOREIGN_SERVER:
// 			appendStringInfoString(&buffer, "server");
// 			break;

// 		case OCLASS_USER_MAPPING:
// 			appendStringInfoString(&buffer, "user mapping");
// 			break;

// 		case OCLASS_DEFACL:
// 			appendStringInfoString(&buffer, "default acl");
// 			break;

// 		case OCLASS_EXTENSION:
// 			appendStringInfoString(&buffer, "extension");
// 			break;

// 		case OCLASS_EVENT_TRIGGER:
// 			appendStringInfoString(&buffer, "event trigger");
// 			break;

// 		case OCLASS_POLICY:
// 			appendStringInfoString(&buffer, "policy");
// 			break;

// 		case OCLASS_TRANSFORM:
// 			appendStringInfoString(&buffer, "transform");
// 			break;

// 		case OCLASS_AM:
// 			appendStringInfoString(&buffer, "access method");
// 			break;

// 		default:
// 			appendStringInfo(&buffer, "unrecognized %u", object->classId);
// 			break;
// 	}

// 	return buffer.data;
// }
// char *
// getObjectIdentity(const ObjectAddress *object)
// {
// 	return getObjectIdentityParts(object, NULL, NULL);
// }
// char *
// getObjectIdentityParts(const ObjectAddress *object,
// 					   List **objname, List **objargs)
// {
// 	StringInfoData buffer;

// 	initStringInfo(&buffer);

// 	/*
// 	 * Make sure that both objname and objargs were passed, or none was; and
// 	 * initialize them to empty lists.  For objname this is useless because it
// 	 * will be initialized in all cases inside the switch; but we do it anyway
// 	 * so that we can test below that no branch leaves it unset.
// 	 */
// 	Assert(PointerIsValid(objname) == PointerIsValid(objargs));
// 	if (objname)
// 	{
// 		*objname = NIL;
// 		*objargs = NIL;
// 	}

// 	switch (getObjectClass(object))
// 	{
// 		case OCLASS_CLASS:
// 			getRelationIdentity(&buffer, object->objectId, objname);
// 			if (object->objectSubId != 0)
// 			{
// 				char	   *attr;

// 				attr = get_relid_attribute_name(object->objectId,
// 												object->objectSubId);
// 				appendStringInfo(&buffer, ".%s", quote_identifier(attr));
// 				if (objname)
// 					*objname = lappend(*objname, attr);
// 			}
// 			break;

// 		case OCLASS_PROC:
// 			appendStringInfoString(&buffer,
// 							   format_procedure_qualified(object->objectId));
// 			if (objname)
// 				format_procedure_parts(object->objectId, objname, objargs);
// 			break;

// 		case OCLASS_TYPE:
// 			{
// 				char	   *typeout;

// 				typeout = format_type_be_qualified(object->objectId);
// 				appendStringInfoString(&buffer, typeout);
// 				if (objname)
// 					*objname = list_make1(typeout);
// 			}
// 			break;

// 		case OCLASS_CAST:
// 			{
// 				Relation	castRel;
// 				HeapTuple	tup;
// 				Form_pg_cast castForm;

// 				castRel = heap_open(CastRelationId, AccessShareLock);

// 				tup = get_catalog_object_by_oid(castRel, object->objectId);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for cast %u",
// 						 object->objectId);

// 				castForm = (Form_pg_cast) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, "(%s AS %s)",
// 							  format_type_be_qualified(castForm->castsource),
// 							 format_type_be_qualified(castForm->casttarget));

// 				if (objname)
// 				{
// 					*objname = list_make1(format_type_be_qualified(castForm->castsource));
// 					*objargs = list_make1(format_type_be_qualified(castForm->casttarget));
// 				}

// 				heap_close(castRel, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_COLLATION:
// 			{
// 				HeapTuple	collTup;
// 				Form_pg_collation coll;
// 				char	   *schema;

// 				collTup = SearchSysCache1(COLLOID,
// 										  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(collTup))
// 					elog(ERROR, "cache lookup failed for collation %u",
// 						 object->objectId);
// 				coll = (Form_pg_collation) GETSTRUCT(collTup);
// 				schema = get_namespace_name_or_temp(coll->collnamespace);
// 				appendStringInfoString(&buffer,
// 									   quote_qualified_identifier(schema,
// 												   NameStr(coll->collname)));
// 				if (objname)
// 					*objname = list_make2(schema,
// 										  pstrdup(NameStr(coll->collname)));
// 				ReleaseSysCache(collTup);
// 				break;
// 			}

// 		case OCLASS_CONSTRAINT:
// 			{
// 				HeapTuple	conTup;
// 				Form_pg_constraint con;

// 				conTup = SearchSysCache1(CONSTROID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(conTup))
// 					elog(ERROR, "cache lookup failed for constraint %u",
// 						 object->objectId);
// 				con = (Form_pg_constraint) GETSTRUCT(conTup);

// 				if (OidIsValid(con->conrelid))
// 				{
// 					appendStringInfo(&buffer, "%s on ",
// 									 quote_identifier(NameStr(con->conname)));
// 					getRelationIdentity(&buffer, con->conrelid, objname);
// 					if (objname)
// 						*objname = lappend(*objname, pstrdup(NameStr(con->conname)));
// 				}
// 				else
// 				{
// 					ObjectAddress domain;

// 					Assert(OidIsValid(con->contypid));
// 					domain.classId = TypeRelationId;
// 					domain.objectId = con->contypid;
// 					domain.objectSubId = 0;

// 					appendStringInfo(&buffer, "%s on %s",
// 									 quote_identifier(NameStr(con->conname)),
// 						  getObjectIdentityParts(&domain, objname, objargs));

// 					if (objname)
// 						*objargs = lappend(*objargs, pstrdup(NameStr(con->conname)));
// 				}

// 				ReleaseSysCache(conTup);
// 				break;
// 			}

// 		case OCLASS_CONVERSION:
// 			{
// 				HeapTuple	conTup;
// 				Form_pg_conversion conForm;
// 				char	   *schema;

// 				conTup = SearchSysCache1(CONVOID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(conTup))
// 					elog(ERROR, "cache lookup failed for conversion %u",
// 						 object->objectId);
// 				conForm = (Form_pg_conversion) GETSTRUCT(conTup);
// 				schema = get_namespace_name_or_temp(conForm->connamespace);
// 				appendStringInfoString(&buffer,
// 									   quote_qualified_identifier(schema,
// 												 NameStr(conForm->conname)));
// 				if (objname)
// 					*objname = list_make2(schema,
// 										  pstrdup(NameStr(conForm->conname)));
// 				ReleaseSysCache(conTup);
// 				break;
// 			}

// 		case OCLASS_DEFAULT:
// 			{
// 				Relation	attrdefDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc adscan;

// 				HeapTuple	tup;
// 				Form_pg_attrdef attrdef;
// 				ObjectAddress colobject;

// 				attrdefDesc = heap_open(AttrDefaultRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				adscan = systable_beginscan(attrdefDesc, AttrDefaultOidIndexId,
// 											true, NULL, 1, skey);

// 				tup = systable_getnext(adscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for attrdef %u",
// 						 object->objectId);

// 				attrdef = (Form_pg_attrdef) GETSTRUCT(tup);

// 				colobject.classId = RelationRelationId;
// 				colobject.objectId = attrdef->adrelid;
// 				colobject.objectSubId = attrdef->adnum;

// 				appendStringInfo(&buffer, "for %s",
// 								 getObjectIdentityParts(&colobject,
// 														objname, objargs));

// 				systable_endscan(adscan);
// 				heap_close(attrdefDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_LANGUAGE:
// 			{
// 				HeapTuple	langTup;
// 				Form_pg_language langForm;

// 				langTup = SearchSysCache1(LANGOID,
// 										  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(langTup))
// 					elog(ERROR, "cache lookup failed for language %u",
// 						 object->objectId);
// 				langForm = (Form_pg_language) GETSTRUCT(langTup);
// 				appendStringInfoString(&buffer,
// 							   quote_identifier(NameStr(langForm->lanname)));
// 				if (objname)
// 					*objname = list_make1(pstrdup(NameStr(langForm->lanname)));
// 				ReleaseSysCache(langTup);
// 				break;
// 			}
// 		case OCLASS_LARGEOBJECT:
// 			appendStringInfo(&buffer, "%u",
// 							 object->objectId);
// 			if (objname)
// 				*objname = list_make1(psprintf("%u", object->objectId));
// 			break;

// 		case OCLASS_OPERATOR:
// 			appendStringInfoString(&buffer,
// 								format_operator_qualified(object->objectId));
// 			if (objname)
// 				format_operator_parts(object->objectId, objname, objargs);
// 			break;

// 		case OCLASS_OPCLASS:
// 			{
// 				HeapTuple	opcTup;
// 				Form_pg_opclass opcForm;
// 				HeapTuple	amTup;
// 				Form_pg_am	amForm;
// 				char	   *schema;

// 				opcTup = SearchSysCache1(CLAOID,
// 										 ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(opcTup))
// 					elog(ERROR, "cache lookup failed for opclass %u",
// 						 object->objectId);
// 				opcForm = (Form_pg_opclass) GETSTRUCT(opcTup);
// 				schema = get_namespace_name_or_temp(opcForm->opcnamespace);

// 				amTup = SearchSysCache1(AMOID,
// 										ObjectIdGetDatum(opcForm->opcmethod));
// 				if (!HeapTupleIsValid(amTup))
// 					elog(ERROR, "cache lookup failed for access method %u",
// 						 opcForm->opcmethod);
// 				amForm = (Form_pg_am) GETSTRUCT(amTup);

// 				appendStringInfo(&buffer, "%s USING %s",
// 								 quote_qualified_identifier(schema,
// 												  NameStr(opcForm->opcname)),
// 								 quote_identifier(NameStr(amForm->amname)));
// 				if (objname)
// 					*objname = list_make3(pstrdup(NameStr(amForm->amname)),
// 										  schema,
// 										  pstrdup(NameStr(opcForm->opcname)));

// 				ReleaseSysCache(amTup);
// 				ReleaseSysCache(opcTup);
// 				break;
// 			}

// 		case OCLASS_OPFAMILY:
// 			getOpFamilyIdentity(&buffer, object->objectId, objname);
// 			break;

// 		case OCLASS_AMOP:
// 			{
// 				Relation	amopDesc;
// 				HeapTuple	tup;
// 				ScanKeyData skey[1];
// 				SysScanDesc amscan;
// 				Form_pg_amop amopForm;
// 				StringInfoData opfam;
// 				char	   *ltype;
// 				char	   *rtype;

// 				amopDesc = heap_open(AccessMethodOperatorRelationId,
// 									 AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				amscan = systable_beginscan(amopDesc, AccessMethodOperatorOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(amscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for amop entry %u",
// 						 object->objectId);

// 				amopForm = (Form_pg_amop) GETSTRUCT(tup);

// 				initStringInfo(&opfam);
// 				getOpFamilyIdentity(&opfam, amopForm->amopfamily, objname);

// 				ltype = format_type_be_qualified(amopForm->amoplefttype);
// 				rtype = format_type_be_qualified(amopForm->amoprighttype);

// 				if (objname)
// 				{
// 					*objname = lappend(*objname,
// 									 psprintf("%d", amopForm->amopstrategy));
// 					*objargs = list_make2(ltype, rtype);
// 				}

// 				appendStringInfo(&buffer, "operator %d (%s, %s) of %s",
// 								 amopForm->amopstrategy,
// 								 ltype, rtype, opfam.data);

// 				pfree(opfam.data);

// 				systable_endscan(amscan);
// 				heap_close(amopDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_AMPROC:
// 			{
// 				Relation	amprocDesc;
// 				ScanKeyData skey[1];
// 				SysScanDesc amscan;
// 				HeapTuple	tup;
// 				Form_pg_amproc amprocForm;
// 				StringInfoData opfam;
// 				char	   *ltype;
// 				char	   *rtype;

// 				amprocDesc = heap_open(AccessMethodProcedureRelationId,
// 									   AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				amscan = systable_beginscan(amprocDesc, AccessMethodProcedureOidIndexId, true,
// 											NULL, 1, skey);

// 				tup = systable_getnext(amscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for amproc entry %u",
// 						 object->objectId);

// 				amprocForm = (Form_pg_amproc) GETSTRUCT(tup);

// 				initStringInfo(&opfam);
// 				getOpFamilyIdentity(&opfam, amprocForm->amprocfamily, objname);

// 				ltype = format_type_be_qualified(amprocForm->amproclefttype);
// 				rtype = format_type_be_qualified(amprocForm->amprocrighttype);

// 				if (objname)
// 				{
// 					*objname = lappend(*objname,
// 									   psprintf("%d", amprocForm->amprocnum));
// 					*objargs = list_make2(ltype, rtype);
// 				}

// 				appendStringInfo(&buffer, "function %d (%s, %s) of %s",
// 								 amprocForm->amprocnum,
// 								 ltype, rtype, opfam.data);

// 				pfree(opfam.data);

// 				systable_endscan(amscan);
// 				heap_close(amprocDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_REWRITE:
// 			{
// 				Relation	ruleDesc;
// 				HeapTuple	tup;
// 				Form_pg_rewrite rule;

// 				ruleDesc = heap_open(RewriteRelationId, AccessShareLock);

// 				tup = get_catalog_object_by_oid(ruleDesc, object->objectId);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for rule %u",
// 						 object->objectId);

// 				rule = (Form_pg_rewrite) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, "%s on ",
// 								 quote_identifier(NameStr(rule->rulename)));
// 				getRelationIdentity(&buffer, rule->ev_class, objname);
// 				if (objname)
// 					*objname = lappend(*objname, pstrdup(NameStr(rule->rulename)));

// 				heap_close(ruleDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_TRIGGER:
// 			{
// 				Relation	trigDesc;
// 				HeapTuple	tup;
// 				Form_pg_trigger trig;

// 				trigDesc = heap_open(TriggerRelationId, AccessShareLock);

// 				tup = get_catalog_object_by_oid(trigDesc, object->objectId);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for trigger %u",
// 						 object->objectId);

// 				trig = (Form_pg_trigger) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, "%s on ",
// 								 quote_identifier(NameStr(trig->tgname)));
// 				getRelationIdentity(&buffer, trig->tgrelid, objname);
// 				if (objname)
// 					*objname = lappend(*objname, pstrdup(NameStr(trig->tgname)));

// 				heap_close(trigDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_POLICY:
// 			{
// 				Relation	polDesc;
// 				HeapTuple	tup;
// 				Form_pg_policy policy;

// 				polDesc = heap_open(PolicyRelationId, AccessShareLock);

// 				tup = get_catalog_object_by_oid(polDesc, object->objectId);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for policy %u",
// 						 object->objectId);

// 				policy = (Form_pg_policy) GETSTRUCT(tup);

// 				appendStringInfo(&buffer, "%s on ",
// 								 quote_identifier(NameStr(policy->polname)));
// 				getRelationIdentity(&buffer, policy->polrelid, objname);
// 				if (objname)
// 					*objname = lappend(*objname, pstrdup(NameStr(policy->polname)));

// 				heap_close(polDesc, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_SCHEMA:
// 			{
// 				char	   *nspname;

// 				nspname = get_namespace_name_or_temp(object->objectId);
// 				if (!nspname)
// 					elog(ERROR, "cache lookup failed for namespace %u",
// 						 object->objectId);
// 				appendStringInfoString(&buffer,
// 									   quote_identifier(nspname));
// 				if (objname)
// 					*objname = list_make1(nspname);
// 				break;
// 			}

// 		case OCLASS_TSPARSER:
// 			{
// 				HeapTuple	tup;
// 				Form_pg_ts_parser formParser;
// 				char	   *schema;

// 				tup = SearchSysCache1(TSPARSEROID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search parser %u",
// 						 object->objectId);
// 				formParser = (Form_pg_ts_parser) GETSTRUCT(tup);
// 				schema = get_namespace_name_or_temp(formParser->prsnamespace);
// 				appendStringInfoString(&buffer,
// 									   quote_qualified_identifier(schema,
// 											  NameStr(formParser->prsname)));
// 				if (objname)
// 					*objname = list_make2(schema,
// 									  pstrdup(NameStr(formParser->prsname)));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TSDICT:
// 			{
// 				HeapTuple	tup;
// 				Form_pg_ts_dict formDict;
// 				char	   *schema;

// 				tup = SearchSysCache1(TSDICTOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search dictionary %u",
// 						 object->objectId);
// 				formDict = (Form_pg_ts_dict) GETSTRUCT(tup);
// 				schema = get_namespace_name_or_temp(formDict->dictnamespace);
// 				appendStringInfoString(&buffer,
// 									   quote_qualified_identifier(schema,
// 											   NameStr(formDict->dictname)));
// 				if (objname)
// 					*objname = list_make2(schema,
// 									   pstrdup(NameStr(formDict->dictname)));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TSTEMPLATE:
// 			{
// 				HeapTuple	tup;
// 				Form_pg_ts_template formTmpl;
// 				char	   *schema;

// 				tup = SearchSysCache1(TSTEMPLATEOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search template %u",
// 						 object->objectId);
// 				formTmpl = (Form_pg_ts_template) GETSTRUCT(tup);
// 				schema = get_namespace_name_or_temp(formTmpl->tmplnamespace);
// 				appendStringInfoString(&buffer,
// 									   quote_qualified_identifier(schema,
// 											   NameStr(formTmpl->tmplname)));
// 				if (objname)
// 					*objname = list_make2(schema,
// 									   pstrdup(NameStr(formTmpl->tmplname)));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TSCONFIG:
// 			{
// 				HeapTuple	tup;
// 				Form_pg_ts_config formCfg;
// 				char	   *schema;

// 				tup = SearchSysCache1(TSCONFIGOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for text search configuration %u",
// 						 object->objectId);
// 				formCfg = (Form_pg_ts_config) GETSTRUCT(tup);
// 				schema = get_namespace_name_or_temp(formCfg->cfgnamespace);
// 				appendStringInfoString(&buffer,
// 									   quote_qualified_identifier(schema,
// 												 NameStr(formCfg->cfgname)));
// 				if (objname)
// 					*objname = list_make2(schema,
// 										  pstrdup(NameStr(formCfg->cfgname)));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_ROLE:
// 			{
// 				char	   *username;

// 				username = GetUserNameFromId(object->objectId, false);
// 				if (objname)
// 					*objname = list_make1(username);
// 				appendStringInfoString(&buffer,
// 									   quote_identifier(username));
// 				break;
// 			}

// 		case OCLASS_DATABASE:
// 			{
// 				char	   *datname;

// 				datname = get_database_name(object->objectId);
// 				if (!datname)
// 					elog(ERROR, "cache lookup failed for database %u",
// 						 object->objectId);
// 				if (objname)
// 					*objname = list_make1(datname);
// 				appendStringInfoString(&buffer,
// 									   quote_identifier(datname));
// 				break;
// 			}

// 		case OCLASS_TBLSPACE:
// 			{
// 				char	   *tblspace;

// 				tblspace = get_tablespace_name(object->objectId);
// 				if (!tblspace)
// 					elog(ERROR, "cache lookup failed for tablespace %u",
// 						 object->objectId);
// 				if (objname)
// 					*objname = list_make1(tblspace);
// 				appendStringInfoString(&buffer,
// 									   quote_identifier(tblspace));
// 				break;
// 			}

// 		case OCLASS_FDW:
// 			{
// 				ForeignDataWrapper *fdw;

// 				fdw = GetForeignDataWrapper(object->objectId);
// 				appendStringInfoString(&buffer, quote_identifier(fdw->fdwname));
// 				if (objname)
// 					*objname = list_make1(pstrdup(fdw->fdwname));
// 				break;
// 			}

// 		case OCLASS_FOREIGN_SERVER:
// 			{
// 				ForeignServer *srv;

// 				srv = GetForeignServer(object->objectId);
// 				appendStringInfoString(&buffer,
// 									   quote_identifier(srv->servername));
// 				if (objname)
// 					*objname = list_make1(pstrdup(srv->servername));
// 				break;
// 			}

// 		case OCLASS_USER_MAPPING:
// 			{
// 				HeapTuple	tup;
// 				Oid			useid;
// 				Form_pg_user_mapping umform;
// 				ForeignServer *srv;
// 				const char *usename;

// 				tup = SearchSysCache1(USERMAPPINGOID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for user mapping %u",
// 						 object->objectId);
// 				umform = (Form_pg_user_mapping) GETSTRUCT(tup);
// 				useid = umform->umuser;
// 				srv = GetForeignServer(umform->umserver);

// 				ReleaseSysCache(tup);

// 				if (OidIsValid(useid))
// 					usename = GetUserNameFromId(useid, false);
// 				else
// 					usename = "public";

// 				if (objname)
// 				{
// 					*objname = list_make1(pstrdup(usename));
// 					*objargs = list_make1(pstrdup(srv->servername));
// 				}

// 				appendStringInfo(&buffer, "%s on server %s",
// 								 quote_identifier(usename),
// 								 srv->servername);
// 				break;
// 			}

// 		case OCLASS_DEFACL:
// 			{
// 				Relation	defaclrel;
// 				ScanKeyData skey[1];
// 				SysScanDesc rcscan;
// 				HeapTuple	tup;
// 				Form_pg_default_acl defacl;
// 				char	   *schema;
// 				char	   *username;

// 				defaclrel = heap_open(DefaultAclRelationId, AccessShareLock);

// 				ScanKeyInit(&skey[0],
// 							ObjectIdAttributeNumber,
// 							BTEqualStrategyNumber, F_OIDEQ,
// 							ObjectIdGetDatum(object->objectId));

// 				rcscan = systable_beginscan(defaclrel, DefaultAclOidIndexId,
// 											true, NULL, 1, skey);

// 				tup = systable_getnext(rcscan);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for default ACL %u",
// 						 object->objectId);

// 				defacl = (Form_pg_default_acl) GETSTRUCT(tup);

// 				username = GetUserNameFromId(defacl->defaclrole, false);
// 				appendStringInfo(&buffer,
// 								 "for role %s",
// 								 quote_identifier(username));

// 				if (OidIsValid(defacl->defaclnamespace))
// 				{
// 					schema = get_namespace_name_or_temp(defacl->defaclnamespace);
// 					appendStringInfo(&buffer,
// 									 " in schema %s",
// 									 quote_identifier(schema));
// 				}
// 				else
// 					schema = NULL;

// 				switch (defacl->defaclobjtype)
// 				{
// 					case DEFACLOBJ_RELATION:
// 						appendStringInfoString(&buffer,
// 											   " on tables");
// 						break;
// 					case DEFACLOBJ_SEQUENCE:
// 						appendStringInfoString(&buffer,
// 											   " on sequences");
// 						break;
// 					case DEFACLOBJ_FUNCTION:
// 						appendStringInfoString(&buffer,
// 											   " on functions");
// 						break;
// 					case DEFACLOBJ_TYPE:
// 						appendStringInfoString(&buffer,
// 											   " on types");
// 						break;
// 				}

// 				if (objname)
// 				{
// 					*objname = list_make1(username);
// 					if (schema)
// 						*objname = lappend(*objname, schema);
// 					*objargs = list_make1(psprintf("%c", defacl->defaclobjtype));
// 				}

// 				systable_endscan(rcscan);
// 				heap_close(defaclrel, AccessShareLock);
// 				break;
// 			}

// 		case OCLASS_EXTENSION:
// 			{
// 				char	   *extname;

// 				extname = get_extension_name(object->objectId);
// 				if (!extname)
// 					elog(ERROR, "cache lookup failed for extension %u",
// 						 object->objectId);
// 				appendStringInfoString(&buffer, quote_identifier(extname));
// 				if (objname)
// 					*objname = list_make1(extname);
// 				break;
// 			}

// 		case OCLASS_EVENT_TRIGGER:
// 			{
// 				HeapTuple	tup;
// 				Form_pg_event_trigger trigForm;

// 				/* no objname support here */
// 				if (objname)
// 					*objname = NIL;

// 				tup = SearchSysCache1(EVENTTRIGGEROID,
// 									  ObjectIdGetDatum(object->objectId));
// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "cache lookup failed for event trigger %u",
// 						 object->objectId);
// 				trigForm = (Form_pg_event_trigger) GETSTRUCT(tup);
// 				appendStringInfoString(&buffer,
// 							   quote_identifier(NameStr(trigForm->evtname)));
// 				ReleaseSysCache(tup);
// 				break;
// 			}

// 		case OCLASS_TRANSFORM:
// 			{
// 				Relation	transformDesc;
// 				HeapTuple	tup;
// 				Form_pg_transform transform;
// 				char	   *transformLang;
// 				char	   *transformType;

// 				transformDesc = heap_open(TransformRelationId, AccessShareLock);

// 				tup = get_catalog_object_by_oid(transformDesc, object->objectId);

// 				if (!HeapTupleIsValid(tup))
// 					elog(ERROR, "could not find tuple for transform %u",
// 						 object->objectId);

// 				transform = (Form_pg_transform) GETSTRUCT(tup);

// 				transformType = format_type_be_qualified(transform->trftype);
// 				transformLang = get_language_name(transform->trflang, false);

// 				appendStringInfo(&buffer, "for %s on language %s",
// 								 transformType,
// 								 transformLang);
// 				if (objname)
// 				{
// 					*objname = list_make1(transformType);
// 					*objargs = list_make1(pstrdup(transformLang));
// 				}

// 				heap_close(transformDesc, AccessShareLock);
// 			}
// 			break;

// 		case OCLASS_AM:
// 			{
// 				char	   *amname;

// 				amname = get_am_name(object->objectId);
// 				if (!amname)
// 					elog(ERROR, "cache lookup failed for access method %u",
// 						 object->objectId);
// 				appendStringInfoString(&buffer, quote_identifier(amname));
// 				if (objname)
// 					*objname = list_make1(amname);
// 			}
// 			break;

// 		default:
// 			appendStringInfo(&buffer, "unrecognized object %u %u %d",
// 							 object->classId,
// 							 object->objectId,
// 							 object->objectSubId);
// 			break;
// 	}

// 	/*
// 	 * If a get_object_address representation was requested, make sure we are
// 	 * providing one.  We don't check objargs, because many of the cases above
// 	 * leave it as NIL.
// 	 */
// 	if (objname && *objname == NIL)
// 		elog(ERROR, "requested object address for unsupported object class %d: text result \"%s\"",
// 			 (int) getObjectClass(object), buffer.data);

// 	return buffer.data;
// }
// ArrayType *
// strlist_to_textarray(List *list)
// {
// 	ArrayType  *arr;
// 	Datum	   *datums;
// 	int			j = 0;
// 	ListCell   *cell;
// 	MemoryContext memcxt;
// 	MemoryContext oldcxt;

// 	memcxt = AllocSetContextCreate(CurrentMemoryContext,
// 								   "strlist to array",
// 								   ALLOCSET_DEFAULT_SIZES);
// 	oldcxt = MemoryContextSwitchTo(memcxt);

// 	datums = palloc(sizeof(text *) * list_length(list));
// 	foreach(cell, list)
// 	{
// 		char	   *name = lfirst(cell);

// 		datums[j++] = CStringGetTextDatum(name);
// 	}

// 	MemoryContextSwitchTo(oldcxt);

// 	arr = construct_array(datums, list_length(list),
// 						  TEXTOID, -1, false, 'i');
// 	MemoryContextDelete(memcxt);

// 	return arr;
// }
// Oid
// CreateConstraintEntry(const char *constraintName,
// 					  Oid constraintNamespace,
// 					  char constraintType,
// 					  bool isDeferrable,
// 					  bool isDeferred,
// 					  bool isValidated,
// 					  Oid relId,
// 					  const int16 *constraintKey,
// 					  int constraintNKeys,
// 					  Oid domainId,
// 					  Oid indexRelId,
// 					  Oid foreignRelId,
// 					  const int16 *foreignKey,
// 					  const Oid *pfEqOp,
// 					  const Oid *ppEqOp,
// 					  const Oid *ffEqOp,
// 					  int foreignNKeys,
// 					  char foreignUpdateType,
// 					  char foreignDeleteType,
// 					  char foreignMatchType,
// 					  const Oid *exclOp,
// 					  Node *conExpr,
// 					  const char *conBin,
// 					  const char *conSrc,
// 					  bool conIsLocal,
// 					  int conInhCount,
// 					  bool conNoInherit,
// 					  bool is_internal)
// {
// 	Relation	conDesc;
// 	Oid			conOid;
// 	HeapTuple	tup;
// 	bool		nulls[Natts_pg_constraint];
// 	Datum		values[Natts_pg_constraint];
// 	ArrayType  *conkeyArray;
// 	ArrayType  *confkeyArray;
// 	ArrayType  *conpfeqopArray;
// 	ArrayType  *conppeqopArray;
// 	ArrayType  *conffeqopArray;
// 	ArrayType  *conexclopArray;
// 	NameData	cname;
// 	int			i;
// 	ObjectAddress conobject;

// 	conDesc = heap_open(ConstraintRelationId, RowExclusiveLock);

// 	Assert(constraintName);
// 	namestrcpy(&cname, constraintName);

// 	/*
// 	 * Convert C arrays into Postgres arrays.
// 	 */
// 	if (constraintNKeys > 0)
// 	{
// 		Datum	   *conkey;

// 		conkey = (Datum *) palloc(constraintNKeys * sizeof(Datum));
// 		for (i = 0; i < constraintNKeys; i++)
// 			conkey[i] = Int16GetDatum(constraintKey[i]);
// 		conkeyArray = construct_array(conkey, constraintNKeys,
// 									  INT2OID, 2, true, 's');
// 	}
// 	else
// 		conkeyArray = NULL;

// 	if (foreignNKeys > 0)
// 	{
// 		Datum	   *fkdatums;

// 		fkdatums = (Datum *) palloc(foreignNKeys * sizeof(Datum));
// 		for (i = 0; i < foreignNKeys; i++)
// 			fkdatums[i] = Int16GetDatum(foreignKey[i]);
// 		confkeyArray = construct_array(fkdatums, foreignNKeys,
// 									   INT2OID, 2, true, 's');
// 		for (i = 0; i < foreignNKeys; i++)
// 			fkdatums[i] = ObjectIdGetDatum(pfEqOp[i]);
// 		conpfeqopArray = construct_array(fkdatums, foreignNKeys,
// 										 OIDOID, sizeof(Oid), true, 'i');
// 		for (i = 0; i < foreignNKeys; i++)
// 			fkdatums[i] = ObjectIdGetDatum(ppEqOp[i]);
// 		conppeqopArray = construct_array(fkdatums, foreignNKeys,
// 										 OIDOID, sizeof(Oid), true, 'i');
// 		for (i = 0; i < foreignNKeys; i++)
// 			fkdatums[i] = ObjectIdGetDatum(ffEqOp[i]);
// 		conffeqopArray = construct_array(fkdatums, foreignNKeys,
// 										 OIDOID, sizeof(Oid), true, 'i');
// 	}
// 	else
// 	{
// 		confkeyArray = NULL;
// 		conpfeqopArray = NULL;
// 		conppeqopArray = NULL;
// 		conffeqopArray = NULL;
// 	}

// 	if (exclOp != NULL)
// 	{
// 		Datum	   *opdatums;

// 		opdatums = (Datum *) palloc(constraintNKeys * sizeof(Datum));
// 		for (i = 0; i < constraintNKeys; i++)
// 			opdatums[i] = ObjectIdGetDatum(exclOp[i]);
// 		conexclopArray = construct_array(opdatums, constraintNKeys,
// 										 OIDOID, sizeof(Oid), true, 'i');
// 	}
// 	else
// 		conexclopArray = NULL;

// 	/* initialize nulls and values */
// 	for (i = 0; i < Natts_pg_constraint; i++)
// 	{
// 		nulls[i] = false;
// 		values[i] = (Datum) NULL;
// 	}

// 	values[Anum_pg_constraint_conname - 1] = NameGetDatum(&cname);
// 	values[Anum_pg_constraint_connamespace - 1] = ObjectIdGetDatum(constraintNamespace);
// 	values[Anum_pg_constraint_contype - 1] = CharGetDatum(constraintType);
// 	values[Anum_pg_constraint_condeferrable - 1] = BoolGetDatum(isDeferrable);
// 	values[Anum_pg_constraint_condeferred - 1] = BoolGetDatum(isDeferred);
// 	values[Anum_pg_constraint_convalidated - 1] = BoolGetDatum(isValidated);
// 	values[Anum_pg_constraint_conrelid - 1] = ObjectIdGetDatum(relId);
// 	values[Anum_pg_constraint_contypid - 1] = ObjectIdGetDatum(domainId);
// 	values[Anum_pg_constraint_conindid - 1] = ObjectIdGetDatum(indexRelId);
// 	values[Anum_pg_constraint_confrelid - 1] = ObjectIdGetDatum(foreignRelId);
// 	values[Anum_pg_constraint_confupdtype - 1] = CharGetDatum(foreignUpdateType);
// 	values[Anum_pg_constraint_confdeltype - 1] = CharGetDatum(foreignDeleteType);
// 	values[Anum_pg_constraint_confmatchtype - 1] = CharGetDatum(foreignMatchType);
// 	values[Anum_pg_constraint_conislocal - 1] = BoolGetDatum(conIsLocal);
// 	values[Anum_pg_constraint_coninhcount - 1] = Int32GetDatum(conInhCount);
// 	values[Anum_pg_constraint_connoinherit - 1] = BoolGetDatum(conNoInherit);

// 	if (conkeyArray)
// 		values[Anum_pg_constraint_conkey - 1] = PointerGetDatum(conkeyArray);
// 	else
// 		nulls[Anum_pg_constraint_conkey - 1] = true;

// 	if (confkeyArray)
// 		values[Anum_pg_constraint_confkey - 1] = PointerGetDatum(confkeyArray);
// 	else
// 		nulls[Anum_pg_constraint_confkey - 1] = true;

// 	if (conpfeqopArray)
// 		values[Anum_pg_constraint_conpfeqop - 1] = PointerGetDatum(conpfeqopArray);
// 	else
// 		nulls[Anum_pg_constraint_conpfeqop - 1] = true;

// 	if (conppeqopArray)
// 		values[Anum_pg_constraint_conppeqop - 1] = PointerGetDatum(conppeqopArray);
// 	else
// 		nulls[Anum_pg_constraint_conppeqop - 1] = true;

// 	if (conffeqopArray)
// 		values[Anum_pg_constraint_conffeqop - 1] = PointerGetDatum(conffeqopArray);
// 	else
// 		nulls[Anum_pg_constraint_conffeqop - 1] = true;

// 	if (conexclopArray)
// 		values[Anum_pg_constraint_conexclop - 1] = PointerGetDatum(conexclopArray);
// 	else
// 		nulls[Anum_pg_constraint_conexclop - 1] = true;

// 	/*
// 	 * initialize the binary form of the check constraint.
// 	 */
// 	if (conBin)
// 		values[Anum_pg_constraint_conbin - 1] = CStringGetTextDatum(conBin);
// 	else
// 		nulls[Anum_pg_constraint_conbin - 1] = true;

// 	/*
// 	 * initialize the text form of the check constraint
// 	 */
// 	if (conSrc)
// 		values[Anum_pg_constraint_consrc - 1] = CStringGetTextDatum(conSrc);
// 	else
// 		nulls[Anum_pg_constraint_consrc - 1] = true;

// 	tup = heap_form_tuple(RelationGetDescr(conDesc), values, nulls);

// 	conOid = simple_heap_insert(conDesc, tup);

// 	/* update catalog indexes */
// 	CatalogUpdateIndexes(conDesc, tup);

// 	conobject.classId = ConstraintRelationId;
// 	conobject.objectId = conOid;
// 	conobject.objectSubId = 0;

// 	heap_close(conDesc, RowExclusiveLock);

// 	if (OidIsValid(relId))
// 	{
// 		/*
// 		 * Register auto dependency from constraint to owning relation, or to
// 		 * specific column(s) if any are mentioned.
// 		 */
// 		ObjectAddress relobject;

// 		relobject.classId = RelationRelationId;
// 		relobject.objectId = relId;
// 		if (constraintNKeys > 0)
// 		{
// 			for (i = 0; i < constraintNKeys; i++)
// 			{
// 				relobject.objectSubId = constraintKey[i];

// 				recordDependencyOn(&conobject, &relobject, DEPENDENCY_AUTO);
// 			}
// 		}
// 		else
// 		{
// 			relobject.objectSubId = 0;

// 			recordDependencyOn(&conobject, &relobject, DEPENDENCY_AUTO);
// 		}
// 	}

// 	if (OidIsValid(domainId))
// 	{
// 		/*
// 		 * Register auto dependency from constraint to owning domain
// 		 */
// 		ObjectAddress domobject;

// 		domobject.classId = TypeRelationId;
// 		domobject.objectId = domainId;
// 		domobject.objectSubId = 0;

// 		recordDependencyOn(&conobject, &domobject, DEPENDENCY_AUTO);
// 	}

// 	if (OidIsValid(foreignRelId))
// 	{
// 		/*
// 		 * Register normal dependency from constraint to foreign relation, or
// 		 * to specific column(s) if any are mentioned.
// 		 */
// 		ObjectAddress relobject;

// 		relobject.classId = RelationRelationId;
// 		relobject.objectId = foreignRelId;
// 		if (foreignNKeys > 0)
// 		{
// 			for (i = 0; i < foreignNKeys; i++)
// 			{
// 				relobject.objectSubId = foreignKey[i];

// 				recordDependencyOn(&conobject, &relobject, DEPENDENCY_NORMAL);
// 			}
// 		}
// 		else
// 		{
// 			relobject.objectSubId = 0;

// 			recordDependencyOn(&conobject, &relobject, DEPENDENCY_NORMAL);
// 		}
// 	}

// 	if (OidIsValid(indexRelId) && constraintType == CONSTRAINT_FOREIGN)
// 	{
// 		/*
// 		 * Register normal dependency on the unique index that supports a
// 		 * foreign-key constraint.  (Note: for indexes associated with unique
// 		 * or primary-key constraints, the dependency runs the other way, and
// 		 * is not made here.)
// 		 */
// 		ObjectAddress relobject;

// 		relobject.classId = RelationRelationId;
// 		relobject.objectId = indexRelId;
// 		relobject.objectSubId = 0;

// 		recordDependencyOn(&conobject, &relobject, DEPENDENCY_NORMAL);
// 	}

// 	if (foreignNKeys > 0)
// 	{
// 		/*
// 		 * Register normal dependencies on the equality operators that support
// 		 * a foreign-key constraint.  If the PK and FK types are the same then
// 		 * all three operators for a column are the same; otherwise they are
// 		 * different.
// 		 */
// 		ObjectAddress oprobject;

// 		oprobject.classId = OperatorRelationId;
// 		oprobject.objectSubId = 0;

// 		for (i = 0; i < foreignNKeys; i++)
// 		{
// 			oprobject.objectId = pfEqOp[i];
// 			recordDependencyOn(&conobject, &oprobject, DEPENDENCY_NORMAL);
// 			if (ppEqOp[i] != pfEqOp[i])
// 			{
// 				oprobject.objectId = ppEqOp[i];
// 				recordDependencyOn(&conobject, &oprobject, DEPENDENCY_NORMAL);
// 			}
// 			if (ffEqOp[i] != pfEqOp[i])
// 			{
// 				oprobject.objectId = ffEqOp[i];
// 				recordDependencyOn(&conobject, &oprobject, DEPENDENCY_NORMAL);
// 			}
// 		}
// 	}

// 	/*
// 	 * We don't bother to register dependencies on the exclusion operators of
// 	 * an exclusion constraint.  We assume they are members of the opclass
// 	 * supporting the index, so there's an indirect dependency via that. (This
// 	 * would be pretty dicey for cross-type operators, but exclusion operators
// 	 * can never be cross-type.)
// 	 */

// 	if (conExpr != NULL)
// 	{
// 		/*
// 		 * Register dependencies from constraint to objects mentioned in CHECK
// 		 * expression.
// 		 */
// 		recordDependencyOnSingleRelExpr(&conobject, conExpr, relId,
// 										DEPENDENCY_NORMAL,
// 										DEPENDENCY_NORMAL);
// 	}

// 	/* Post creation hook for new constraint */
// 	InvokeObjectPostCreateHookArg(ConstraintRelationId, conOid, 0,
// 								  is_internal);

// 	return conOid;
// }
// void
// NewRelationCreateToastTable(Oid relOid, Datum reloptions)
// {
// 	CheckAndCreateToastTable(relOid, reloptions, AccessExclusiveLock, false);
// }
// void
// NewHeapCreateToastTable(Oid relOid, Datum reloptions, LOCKMODE lockmode)
// {
// 	CheckAndCreateToastTable(relOid, reloptions, lockmode, false);
// }

// //second
// void
// pull_up_subqueries(PlannerInfo *root)
// {
// 	/* Top level of jointree must always be a FromExpr */
// 	Assert(IsA(root->parse->jointree, FromExpr));
// 	/* Reset flag saying we need a deletion cleanup pass */
// 	root->hasDeletedRTEs = false;
// 	/* Recursion starts with no containing join nor appendrel */
// 	root->parse->jointree = (FromExpr *)
// 		pull_up_subqueries_recurse(root, (Node *) root->parse->jointree,
// 								   NULL, NULL, NULL, false);
// 	/* Apply cleanup phase if necessary */
// 	if (root->hasDeletedRTEs)
// 		root->parse->jointree = (FromExpr *)
// 			pull_up_subqueries_cleanup((Node *) root->parse->jointree);
// 	Assert(IsA(root->parse->jointree, FromExpr));
// }

// Expr *
// canonicalize_qual(Expr *qual)
// {
// 	Expr	   *newqual;

// 	/* Quick exit for empty qual */
// 	if (qual == NULL)
// 		return NULL;

// 	/*
// 	 * Pull up redundant subclauses in OR-of-AND trees.  We do this only
// 	 * within the top-level AND/OR structure; there's no point in looking
// 	 * deeper.  Also remove any NULL constants in the top-level structure.
// 	 */
// 	newqual = find_duplicate_ors(qual);

// 	return newqual;
// }

// void
// expand_security_quals(PlannerInfo *root, List *tlist)
// {
// 	Query	   *parse = root->parse;
// 	int			rt_index;
// 	ListCell   *cell;

// 	/*
// 	 * Process each RTE in the rtable list.
// 	 *
// 	 * We only ever modify entries in place and append to the rtable, so it is
// 	 * safe to use a foreach loop here.
// 	 */
// 	rt_index = 0;
// 	foreach(cell, parse->rtable)
// 	{
// 		bool		targetRelation = false;
// 		RangeTblEntry *rte = (RangeTblEntry *) lfirst(cell);

// 		rt_index++;

// 		if (rte->securityQuals == NIL)
// 			continue;

// 		/*
// 		 * Ignore any RTEs that aren't used in the query (such RTEs may be
// 		 * present for permissions checks).
// 		 */
// 		if (rt_index != parse->resultRelation &&
// 			!rangeTableEntry_used((Node *) parse, rt_index, 0))
// 			continue;

// 		/*
// 		 * If this RTE is the target then we need to make a copy of it before
// 		 * expanding it.  The unexpanded copy will become the new target, and
// 		 * the original RTE will be expanded to become the source of rows to
// 		 * update/delete.
// 		 */
// 		if (rt_index == parse->resultRelation)
// 		{
// 			RangeTblEntry *newrte = copyObject(rte);

// 			/*
// 			 * We need to let expand_security_qual know if this is the target
// 			 * relation, as it has additional work to do in that case.
// 			 *
// 			 * Capture that information here as we're about to replace
// 			 * parse->resultRelation.
// 			 */
// 			targetRelation = true;

// 			parse->rtable = lappend(parse->rtable, newrte);
// 			parse->resultRelation = list_length(parse->rtable);

// 			/*
// 			 * Wipe out any copied security barrier quals on the new target to
// 			 * prevent infinite recursion.
// 			 */
// 			newrte->securityQuals = NIL;

// 			/*
// 			 * There's no need to do permissions checks twice, so wipe out the
// 			 * permissions info for the original RTE (we prefer to keep the
// 			 * bits set on the result RTE).
// 			 */
// 			rte->requiredPerms = 0;
// 			rte->checkAsUser = InvalidOid;
// 			rte->selectedCols = NULL;
// 			rte->insertedCols = NULL;
// 			rte->updatedCols = NULL;

// 			/*
// 			 * For the most part, Vars referencing the original relation
// 			 * should remain as they are, meaning that they pull OLD values
// 			 * from the expanded RTE.  But in the RETURNING list and in any
// 			 * WITH CHECK OPTION quals, we want such Vars to represent NEW
// 			 * values, so change them to reference the new RTE.
// 			 */
// 			ChangeVarNodes((Node *) parse->returningList, rt_index,
// 						   parse->resultRelation, 0);

// 			ChangeVarNodes((Node *) parse->withCheckOptions, rt_index,
// 						   parse->resultRelation, 0);
// 		}

// 		/*
// 		 * Process each security barrier qual in turn, starting with the
// 		 * innermost one (the first in the list) and working outwards.
// 		 *
// 		 * We remove each qual from the list before processing it, so that its
// 		 * variables aren't modified by expand_security_qual.  Also we don't
// 		 * necessarily want the attributes referred to by the qual to be
// 		 * exposed by the newly built subquery.
// 		 */
// 		while (rte->securityQuals != NIL)
// 		{
// 			Node	   *qual = (Node *) linitial(rte->securityQuals);

// 			rte->securityQuals = list_delete_first(rte->securityQuals);

// 			ChangeVarNodes(qual, rt_index, 1, 0);
// 			expand_security_qual(root, tlist, rt_index, rte, qual,
// 								 targetRelation);
// 		}
// 	}
// }

// List *
// preprocess_onconflict_targetlist(List *tlist, int result_relation, List *range_table)
// {
// 	return expand_targetlist(tlist, CMD_UPDATE, result_relation, range_table);
// }

// RelOptInfo *
// plan_set_operations(PlannerInfo *root)
// {
// 	Query	   *parse = root->parse;
// 	SetOperationStmt *topop = (SetOperationStmt *) parse->setOperations;
// 	Node	   *node;
// 	RangeTblEntry *leftmostRTE;
// 	Query	   *leftmostQuery;
// 	RelOptInfo *setop_rel;
// 	Path	   *path;
// 	List	   *top_tlist;

// 	Assert(topop && IsA(topop, SetOperationStmt));

// 	/* check for unsupported stuff */
// 	Assert(parse->jointree->fromlist == NIL);
// 	Assert(parse->jointree->quals == NULL);
// 	Assert(parse->groupClause == NIL);
// 	Assert(parse->havingQual == NULL);
// 	Assert(parse->windowClause == NIL);
// 	Assert(parse->distinctClause == NIL);

// 	/*
// 	 * We'll need to build RelOptInfos for each of the leaf subqueries, which
// 	 * are RTE_SUBQUERY rangetable entries in this Query.  Prepare the index
// 	 * arrays for that.
// 	 */
// 	setup_simple_rel_arrays(root);

// 	/*
// 	 * Find the leftmost component Query.  We need to use its column names for
// 	 * all generated tlists (else SELECT INTO won't work right).
// 	 */
// 	node = topop->larg;
// 	while (node && IsA(node, SetOperationStmt))
// 		node = ((SetOperationStmt *) node)->larg;
// 	Assert(node && IsA(node, RangeTblRef));
// 	leftmostRTE = root->simple_rte_array[((RangeTblRef *) node)->rtindex];
// 	leftmostQuery = leftmostRTE->subquery;
// 	Assert(leftmostQuery != NULL);

// 	/*
// 	 * We return our results in the (SETOP, NULL) upperrel.  For the moment,
// 	 * this is also the parent rel of all Paths in the setop tree; we may well
// 	 * change that in future.
// 	 */
// 	setop_rel = fetch_upper_rel(root, UPPERREL_SETOP, NULL);

// 	/*
// 	 * We don't currently worry about setting setop_rel's consider_parallel
// 	 * flag, nor about allowing FDWs to contribute paths to it.
// 	 */

// 	/*
// 	 * If the topmost node is a recursive union, it needs special processing.
// 	 */
// 	if (root->hasRecursion)
// 	{
// 		path = generate_recursion_path(topop, root,
// 									   leftmostQuery->targetList,
// 									   &top_tlist);
// 	}
// 	else
// 	{
// 		/*
// 		 * Recurse on setOperations tree to generate paths for set ops. The
// 		 * final output path should have just the column types shown as the
// 		 * output from the top-level node, plus possibly resjunk working
// 		 * columns (we can rely on upper-level nodes to deal with that).
// 		 */
// 		path = recurse_set_operations((Node *) topop, root,
// 									  topop->colTypes, topop->colCollations,
// 									  true, -1,
// 									  leftmostQuery->targetList,
// 									  &top_tlist,
// 									  NULL);
// 	}

// 	/* Must return the built tlist into root->processed_tlist. */
// 	root->processed_tlist = top_tlist;

// 	/* Add only the final path to the SETOP upperrel. */
// 	add_path(setop_rel, path);

// 	/* Let extensions possibly add some more paths */
// 	if (create_upper_paths_hook)
// 		(*create_upper_paths_hook) (root, UPPERREL_SETOP,
// 									NULL, setop_rel);

// 	/* Select cheapest path */
// 	set_cheapest(setop_rel);

// 	return setop_rel;
// }

// Node *
// adjust_appendrel_attrs_multilevel(PlannerInfo *root, Node *node,
// 								  RelOptInfo *child_rel)
// {
// 	AppendRelInfo *appinfo = find_childrel_appendrelinfo(root, child_rel);
// 	RelOptInfo *parent_rel = find_base_rel(root, appinfo->parent_relid);

// 	/* If parent is also a child, first recurse to apply its translations */
// 	if (parent_rel->reloptkind == RELOPT_OTHER_MEMBER_REL)
// 		node = adjust_appendrel_attrs_multilevel(root, node, parent_rel);
// 	else
// 		Assert(parent_rel->reloptkind == RELOPT_BASEREL);
// 	/* Now translate for this child */
// 	return adjust_appendrel_attrs(root, node, appinfo);
// }

// Relids
// pull_varnos(Node *node)
// {
// 	pull_varnos_context context;

// 	context.varnos = NULL;
// 	context.sublevels_up = 0;

// 	/*
// 	 * Must be prepared to start with a Query or a bare expression tree; if
// 	 * it's a Query, we don't want to increment sublevels_up.
// 	 */
// 	query_or_expression_tree_walker(node,
// 									pull_varnos_walker,
// 									(void *) &context,
// 									0);

// 	return context.varnos;
// }

// Node *
// ParseFuncOrColumn(ParseState *pstate, List *funcname, List *fargs,
// 				  FuncCall *fn, int location)
// {
// 	bool		is_column = (fn == NULL);
// 	List	   *agg_order = (fn ? fn->agg_order : NIL);
// 	Expr	   *agg_filter = NULL;
// 	bool		agg_within_group = (fn ? fn->agg_within_group : false);
// 	bool		agg_star = (fn ? fn->agg_star : false);
// 	bool		agg_distinct = (fn ? fn->agg_distinct : false);
// 	bool		func_variadic = (fn ? fn->func_variadic : false);
// 	WindowDef  *over = (fn ? fn->over : NULL);
// 	Oid			rettype;
// 	Oid			funcid;
// 	ListCell   *l;
// 	ListCell   *nextl;
// 	Node	   *first_arg = NULL;
// 	int			nargs;
// 	int			nargsplusdefs;
// 	Oid			actual_arg_types[FUNC_MAX_ARGS];
// 	Oid		   *declared_arg_types;
// 	List	   *argnames;
// 	List	   *argdefaults;
// 	Node	   *retval;
// 	bool		retset;
// 	int			nvargs;
// 	Oid			vatype;
// 	FuncDetailCode fdresult;
// 	char		aggkind = 0;
// 	ParseCallbackState pcbstate;

// 	/*
// 	 * If there's an aggregate filter, transform it using transformWhereClause
// 	 */
// 	if (fn && fn->agg_filter != NULL)
// 		agg_filter = (Expr *) transformWhereClause(pstate, fn->agg_filter,
// 												   EXPR_KIND_FILTER,
// 												   "FILTER");

// 	/*
// 	 * Most of the rest of the parser just assumes that functions do not have
// 	 * more than FUNC_MAX_ARGS parameters.  We have to test here to protect
// 	 * against array overruns, etc.  Of course, this may not be a function,
// 	 * but the test doesn't hurt.
// 	 */
// 	if (list_length(fargs) > FUNC_MAX_ARGS)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_TOO_MANY_ARGUMENTS),
// 			 errmsg_plural("cannot pass more than %d argument to a function",
// 						   "cannot pass more than %d arguments to a function",
// 						   FUNC_MAX_ARGS,
// 						   FUNC_MAX_ARGS),
// 				 parser_errposition(pstate, location)));

// 	/*
// 	 * Extract arg type info in preparation for function lookup.
// 	 *
// 	 * If any arguments are Param markers of type VOID, we discard them from
// 	 * the parameter list. This is a hack to allow the JDBC driver to not have
// 	 * to distinguish "input" and "output" parameter symbols while parsing
// 	 * function-call constructs.  Don't do this if dealing with column syntax,
// 	 * nor if we had WITHIN GROUP (because in that case it's critical to keep
// 	 * the argument count unchanged).  We can't use foreach() because we may
// 	 * modify the list ...
// 	 */
// 	nargs = 0;
// 	for (l = list_head(fargs); l != NULL; l = nextl)
// 	{
// 		Node	   *arg = lfirst(l);
// 		Oid			argtype = exprType(arg);

// 		nextl = lnext(l);

// 		if (argtype == VOIDOID && IsA(arg, Param) &&
// 			!is_column && !agg_within_group)
// 		{
// 			fargs = list_delete_ptr(fargs, arg);
// 			continue;
// 		}

// 		actual_arg_types[nargs++] = argtype;
// 	}

// 	/*
// 	 * Check for named arguments; if there are any, build a list of names.
// 	 *
// 	 * We allow mixed notation (some named and some not), but only with all
// 	 * the named parameters after all the unnamed ones.  So the name list
// 	 * corresponds to the last N actual parameters and we don't need any extra
// 	 * bookkeeping to match things up.
// 	 */
// 	argnames = NIL;
// 	foreach(l, fargs)
// 	{
// 		Node	   *arg = lfirst(l);

// 		if (IsA(arg, NamedArgExpr))
// 		{
// 			NamedArgExpr *na = (NamedArgExpr *) arg;
// 			ListCell   *lc;

// 			/* Reject duplicate arg names */
// 			foreach(lc, argnames)
// 			{
// 				if (strcmp(na->name, (char *) lfirst(lc)) == 0)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_SYNTAX_ERROR),
// 						   errmsg("argument name \"%s\" used more than once",
// 								  na->name),
// 							 parser_errposition(pstate, na->location)));
// 			}
// 			argnames = lappend(argnames, na->name);
// 		}
// 		else
// 		{
// 			if (argnames != NIL)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 				  errmsg("positional argument cannot follow named argument"),
// 						 parser_errposition(pstate, exprLocation(arg))));
// 		}
// 	}

// 	if (fargs)
// 	{
// 		first_arg = linitial(fargs);
// 		Assert(first_arg != NULL);
// 	}

// 	/*
// 	 * Check for column projection: if function has one argument, and that
// 	 * argument is of complex type, and function name is not qualified, then
// 	 * the "function call" could be a projection.  We also check that there
// 	 * wasn't any aggregate or variadic decoration, nor an argument name.
// 	 */
// 	if (nargs == 1 && agg_order == NIL && agg_filter == NULL && !agg_star &&
// 		!agg_distinct && over == NULL && !func_variadic && argnames == NIL &&
// 		list_length(funcname) == 1)
// 	{
// 		Oid			argtype = actual_arg_types[0];

// 		if (argtype == RECORDOID || ISCOMPLEX(argtype))
// 		{
// 			retval = ParseComplexProjection(pstate,
// 											strVal(linitial(funcname)),
// 											first_arg,
// 											location);
// 			if (retval)
// 				return retval;

// 			/*
// 			 * If ParseComplexProjection doesn't recognize it as a projection,
// 			 * just press on.
// 			 */
// 		}
// 	}

// 	/*
// 	 * Okay, it's not a column projection, so it must really be a function.
// 	 * func_get_detail looks up the function in the catalogs, does
// 	 * disambiguation for polymorphic functions, handles inheritance, and
// 	 * returns the funcid and type and set or singleton status of the
// 	 * function's return value.  It also returns the true argument types to
// 	 * the function.
// 	 *
// 	 * Note: for a named-notation or variadic function call, the reported
// 	 * "true" types aren't really what is in pg_proc: the types are reordered
// 	 * to match the given argument order of named arguments, and a variadic
// 	 * argument is replaced by a suitable number of copies of its element
// 	 * type.  We'll fix up the variadic case below.  We may also have to deal
// 	 * with default arguments.
// 	 */

// 	setup_parser_errposition_callback(&pcbstate, pstate, location);

// 	fdresult = func_get_detail(funcname, fargs, argnames, nargs,
// 							   actual_arg_types,
// 							   !func_variadic, true,
// 							   &funcid, &rettype, &retset,
// 							   &nvargs, &vatype,
// 							   &declared_arg_types, &argdefaults);

// 	cancel_parser_errposition_callback(&pcbstate);

// 	if (fdresult == FUNCDETAIL_COERCION)
// 	{
// 		/*
// 		 * We interpreted it as a type coercion. coerce_type can handle these
// 		 * cases, so why duplicate code...
// 		 */
// 		return coerce_type(pstate, linitial(fargs),
// 						   actual_arg_types[0], rettype, -1,
// 						   COERCION_EXPLICIT, COERCE_EXPLICIT_CALL, location);
// 	}
// 	else if (fdresult == FUNCDETAIL_NORMAL)
// 	{
// 		/*
// 		 * Normal function found; was there anything indicating it must be an
// 		 * aggregate?
// 		 */
// 		if (agg_star)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 			   errmsg("%s(*) specified, but %s is not an aggregate function",
// 					  NameListToString(funcname),
// 					  NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 		if (agg_distinct)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 			errmsg("DISTINCT specified, but %s is not an aggregate function",
// 				   NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 		if (agg_within_group)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("WITHIN GROUP specified, but %s is not an aggregate function",
// 							NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 		if (agg_order != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 			errmsg("ORDER BY specified, but %s is not an aggregate function",
// 				   NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 		if (agg_filter)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 			  errmsg("FILTER specified, but %s is not an aggregate function",
// 					 NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 		if (over)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("OVER specified, but %s is not a window function nor an aggregate function",
// 							NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 	}
// 	else if (fdresult == FUNCDETAIL_AGGREGATE)
// 	{
// 		/*
// 		 * It's an aggregate; fetch needed info from the pg_aggregate entry.
// 		 */
// 		HeapTuple	tup;
// 		Form_pg_aggregate classForm;
// 		int			catDirectArgs;

// 		tup = SearchSysCache1(AGGFNOID, ObjectIdGetDatum(funcid));
// 		if (!HeapTupleIsValid(tup))		/* should not happen */
// 			elog(ERROR, "cache lookup failed for aggregate %u", funcid);
// 		classForm = (Form_pg_aggregate) GETSTRUCT(tup);
// 		aggkind = classForm->aggkind;
// 		catDirectArgs = classForm->aggnumdirectargs;
// 		ReleaseSysCache(tup);

// 		/* Now check various disallowed cases. */
// 		if (AGGKIND_IS_ORDERED_SET(aggkind))
// 		{
// 			int			numAggregatedArgs;
// 			int			numDirectArgs;

// 			if (!agg_within_group)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 						 errmsg("WITHIN GROUP is required for ordered-set aggregate %s",
// 								NameListToString(funcname)),
// 						 parser_errposition(pstate, location)));
// 			if (over)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("OVER is not supported for ordered-set aggregate %s",
// 						NameListToString(funcname)),
// 						 parser_errposition(pstate, location)));
// 			/* gram.y rejects DISTINCT + WITHIN GROUP */
// 			Assert(!agg_distinct);
// 			/* gram.y rejects VARIADIC + WITHIN GROUP */
// 			Assert(!func_variadic);

// 			/*
// 			 * Since func_get_detail was working with an undifferentiated list
// 			 * of arguments, it might have selected an aggregate that doesn't
// 			 * really match because it requires a different division of direct
// 			 * and aggregated arguments.  Check that the number of direct
// 			 * arguments is actually OK; if not, throw an "undefined function"
// 			 * error, similarly to the case where a misplaced ORDER BY is used
// 			 * in a regular aggregate call.
// 			 */
// 			numAggregatedArgs = list_length(agg_order);
// 			numDirectArgs = nargs - numAggregatedArgs;
// 			Assert(numDirectArgs >= 0);

// 			if (!OidIsValid(vatype))
// 			{
// 				/* Test is simple if aggregate isn't variadic */
// 				if (numDirectArgs != catDirectArgs)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 							 errmsg("function %s does not exist",
// 									func_signature_string(funcname, nargs,
// 														  argnames,
// 														  actual_arg_types)),
// 							 errhint("There is an ordered-set aggregate %s, but it requires %d direct arguments, not %d.",
// 									 NameListToString(funcname),
// 									 catDirectArgs, numDirectArgs),
// 							 parser_errposition(pstate, location)));
// 			}
// 			else
// 			{
// 				/*
// 				 * If it's variadic, we have two cases depending on whether
// 				 * the agg was "... ORDER BY VARIADIC" or "..., VARIADIC ORDER
// 				 * BY VARIADIC".  It's the latter if catDirectArgs equals
// 				 * pronargs; to save a catalog lookup, we reverse-engineer
// 				 * pronargs from the info we got from func_get_detail.
// 				 */
// 				int			pronargs;

// 				pronargs = nargs;
// 				if (nvargs > 1)
// 					pronargs -= nvargs - 1;
// 				if (catDirectArgs < pronargs)
// 				{
// 					/* VARIADIC isn't part of direct args, so still easy */
// 					if (numDirectArgs != catDirectArgs)
// 						ereport(ERROR,
// 								(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 								 errmsg("function %s does not exist",
// 										func_signature_string(funcname, nargs,
// 															  argnames,
// 														  actual_arg_types)),
// 								 errhint("There is an ordered-set aggregate %s, but it requires %d direct arguments, not %d.",
// 										 NameListToString(funcname),
// 										 catDirectArgs, numDirectArgs),
// 								 parser_errposition(pstate, location)));
// 				}
// 				else
// 				{
// 					/*
// 					 * Both direct and aggregated args were declared variadic.
// 					 * For a standard ordered-set aggregate, it's okay as long
// 					 * as there aren't too few direct args.  For a
// 					 * hypothetical-set aggregate, we assume that the
// 					 * hypothetical arguments are those that matched the
// 					 * variadic parameter; there must be just as many of them
// 					 * as there are aggregated arguments.
// 					 */
// 					if (aggkind == AGGKIND_HYPOTHETICAL)
// 					{
// 						if (nvargs != 2 * numAggregatedArgs)
// 							ereport(ERROR,
// 									(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 									 errmsg("function %s does not exist",
// 									   func_signature_string(funcname, nargs,
// 															 argnames,
// 														  actual_arg_types)),
// 									 errhint("To use the hypothetical-set aggregate %s, the number of hypothetical direct arguments (here %d) must match the number of ordering columns (here %d).",
// 											 NameListToString(funcname),
// 							  nvargs - numAggregatedArgs, numAggregatedArgs),
// 									 parser_errposition(pstate, location)));
// 					}
// 					else
// 					{
// 						if (nvargs <= numAggregatedArgs)
// 							ereport(ERROR,
// 									(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 									 errmsg("function %s does not exist",
// 									   func_signature_string(funcname, nargs,
// 															 argnames,
// 														  actual_arg_types)),
// 									 errhint("There is an ordered-set aggregate %s, but it requires at least %d direct arguments.",
// 											 NameListToString(funcname),
// 											 catDirectArgs),
// 									 parser_errposition(pstate, location)));
// 					}
// 				}
// 			}

// 			/* Check type matching of hypothetical arguments */
// 			if (aggkind == AGGKIND_HYPOTHETICAL)
// 				unify_hypothetical_args(pstate, fargs, numAggregatedArgs,
// 										actual_arg_types, declared_arg_types);
// 		}
// 		else
// 		{
// 			/* Normal aggregate, so it can't have WITHIN GROUP */
// 			if (agg_within_group)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 						 errmsg("%s is not an ordered-set aggregate, so it cannot have WITHIN GROUP",
// 								NameListToString(funcname)),
// 						 parser_errposition(pstate, location)));
// 		}
// 	}
// 	else if (fdresult == FUNCDETAIL_WINDOWFUNC)
// 	{
// 		/*
// 		 * True window functions must be called with a window definition.
// 		 */
// 		if (!over)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("window function %s requires an OVER clause",
// 							NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 		/* And, per spec, WITHIN GROUP isn't allowed */
// 		if (agg_within_group)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("window function %s cannot have WITHIN GROUP",
// 							NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));
// 	}
// 	else
// 	{
// 		/*
// 		 * Oops.  Time to die.
// 		 *
// 		 * If we are dealing with the attribute notation rel.function, let the
// 		 * caller handle failure.
// 		 */
// 		if (is_column)
// 			return NULL;

// 		/*
// 		 * Else generate a detailed complaint for a function
// 		 */
// 		if (fdresult == FUNCDETAIL_MULTIPLE)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_AMBIGUOUS_FUNCTION),
// 					 errmsg("function %s is not unique",
// 							func_signature_string(funcname, nargs, argnames,
// 												  actual_arg_types)),
// 					 errhint("Could not choose a best candidate function. "
// 							 "You might need to add explicit type casts."),
// 					 parser_errposition(pstate, location)));
// 		else if (list_length(agg_order) > 1 && !agg_within_group)
// 		{
// 			/* It's agg(x, ORDER BY y,z) ... perhaps misplaced ORDER BY */
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 					 errmsg("function %s does not exist",
// 							func_signature_string(funcname, nargs, argnames,
// 												  actual_arg_types)),
// 					 errhint("No aggregate function matches the given name and argument types. "
// 					  "Perhaps you misplaced ORDER BY; ORDER BY must appear "
// 							 "after all regular arguments of the aggregate."),
// 					 parser_errposition(pstate, location)));
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 					 errmsg("function %s does not exist",
// 							func_signature_string(funcname, nargs, argnames,
// 												  actual_arg_types)),
// 			errhint("No function matches the given name and argument types. "
// 					"You might need to add explicit type casts."),
// 					 parser_errposition(pstate, location)));
// 	}

// 	/*
// 	 * If there are default arguments, we have to include their types in
// 	 * actual_arg_types for the purpose of checking generic type consistency.
// 	 * However, we do NOT put them into the generated parse node, because
// 	 * their actual values might change before the query gets run.  The
// 	 * planner has to insert the up-to-date values at plan time.
// 	 */
// 	nargsplusdefs = nargs;
// 	foreach(l, argdefaults)
// 	{
// 		Node	   *expr = (Node *) lfirst(l);

// 		/* probably shouldn't happen ... */
// 		if (nargsplusdefs >= FUNC_MAX_ARGS)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_TOO_MANY_ARGUMENTS),
// 			 errmsg_plural("cannot pass more than %d argument to a function",
// 						   "cannot pass more than %d arguments to a function",
// 						   FUNC_MAX_ARGS,
// 						   FUNC_MAX_ARGS),
// 					 parser_errposition(pstate, location)));

// 		actual_arg_types[nargsplusdefs++] = exprType(expr);
// 	}

// 	/*
// 	 * enforce consistency with polymorphic argument and return types,
// 	 * possibly adjusting return type or declared_arg_types (which will be
// 	 * used as the cast destination by make_fn_arguments)
// 	 */
// 	rettype = enforce_generic_type_consistency(actual_arg_types,
// 											   declared_arg_types,
// 											   nargsplusdefs,
// 											   rettype,
// 											   false);

// 	/* perform the necessary typecasting of arguments */
// 	make_fn_arguments(pstate, fargs, actual_arg_types, declared_arg_types);

// 	/*
// 	 * If the function isn't actually variadic, forget any VARIADIC decoration
// 	 * on the call.  (Perhaps we should throw an error instead, but
// 	 * historically we've allowed people to write that.)
// 	 */
// 	if (!OidIsValid(vatype))
// 	{
// 		Assert(nvargs == 0);
// 		func_variadic = false;
// 	}

// 	/*
// 	 * If it's a variadic function call, transform the last nvargs arguments
// 	 * into an array --- unless it's an "any" variadic.
// 	 */
// 	if (nvargs > 0 && vatype != ANYOID)
// 	{
// 		ArrayExpr  *newa = makeNode(ArrayExpr);
// 		int			non_var_args = nargs - nvargs;
// 		List	   *vargs;

// 		Assert(non_var_args >= 0);
// 		vargs = list_copy_tail(fargs, non_var_args);
// 		fargs = list_truncate(fargs, non_var_args);

// 		newa->elements = vargs;
// 		/* assume all the variadic arguments were coerced to the same type */
// 		newa->element_typeid = exprType((Node *) linitial(vargs));
// 		newa->array_typeid = get_array_type(newa->element_typeid);
// 		if (!OidIsValid(newa->array_typeid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 					 errmsg("could not find array type for data type %s",
// 							format_type_be(newa->element_typeid)),
// 				  parser_errposition(pstate, exprLocation((Node *) vargs))));
// 		/* array_collid will be set by parse_collate.c */
// 		newa->multidims = false;
// 		newa->location = exprLocation((Node *) vargs);

// 		fargs = lappend(fargs, newa);

// 		/* We could not have had VARIADIC marking before ... */
// 		Assert(!func_variadic);
// 		/* ... but now, it's a VARIADIC call */
// 		func_variadic = true;
// 	}

// 	/*
// 	 * If an "any" variadic is called with explicit VARIADIC marking, insist
// 	 * that the variadic parameter be of some array type.
// 	 */
// 	if (nargs > 0 && vatype == ANYOID && func_variadic)
// 	{
// 		Oid			va_arr_typid = actual_arg_types[nargs - 1];

// 		if (!OidIsValid(get_base_element_type(va_arr_typid)))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_DATATYPE_MISMATCH),
// 					 errmsg("VARIADIC argument must be an array"),
// 					 parser_errposition(pstate,
// 									  exprLocation((Node *) llast(fargs)))));
// 	}

// 	/* build the appropriate output structure */
// 	if (fdresult == FUNCDETAIL_NORMAL)
// 	{
// 		FuncExpr   *funcexpr = makeNode(FuncExpr);

// 		funcexpr->funcid = funcid;
// 		funcexpr->funcresulttype = rettype;
// 		funcexpr->funcretset = retset;
// 		funcexpr->funcvariadic = func_variadic;
// 		funcexpr->funcformat = COERCE_EXPLICIT_CALL;
// 		/* funccollid and inputcollid will be set by parse_collate.c */
// 		funcexpr->args = fargs;
// 		funcexpr->location = location;

// 		retval = (Node *) funcexpr;
// 	}
// 	else if (fdresult == FUNCDETAIL_AGGREGATE && !over)
// 	{
// 		/* aggregate function */
// 		Aggref	   *aggref = makeNode(Aggref);

// 		aggref->aggfnoid = funcid;
// 		aggref->aggtype = rettype;
// 		/* aggcollid and inputcollid will be set by parse_collate.c */
// 		aggref->aggtranstype = InvalidOid;		/* will be set by planner */
// 		/* aggargtypes will be set by transformAggregateCall */
// 		/* aggdirectargs and args will be set by transformAggregateCall */
// 		/* aggorder and aggdistinct will be set by transformAggregateCall */
// 		aggref->aggfilter = agg_filter;
// 		aggref->aggstar = agg_star;
// 		aggref->aggvariadic = func_variadic;
// 		aggref->aggkind = aggkind;
// 		/* agglevelsup will be set by transformAggregateCall */
// 		aggref->aggsplit = AGGSPLIT_SIMPLE;		/* planner might change this */
// 		aggref->location = location;

// 		/*
// 		 * Reject attempt to call a parameterless aggregate without (*)
// 		 * syntax.  This is mere pedantry but some folks insisted ...
// 		 */
// 		if (fargs == NIL && !agg_star && !agg_within_group)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("%s(*) must be used to call a parameterless aggregate function",
// 							NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));

// 		if (retset)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregates cannot return sets"),
// 					 parser_errposition(pstate, location)));

// 		/*
// 		 * We might want to support named arguments later, but disallow it for
// 		 * now.  We'd need to figure out the parsed representation (should the
// 		 * NamedArgExprs go above or below the TargetEntry nodes?) and then
// 		 * teach the planner to reorder the list properly.  Or maybe we could
// 		 * make transformAggregateCall do that?  However, if you'd also like
// 		 * to allow default arguments for aggregates, we'd need to do it in
// 		 * planning to avoid semantic problems.
// 		 */
// 		if (argnames != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("aggregates cannot use named arguments"),
// 					 parser_errposition(pstate, location)));

// 		/* parse_agg.c does additional aggregate-specific processing */
// 		transformAggregateCall(pstate, aggref, fargs, agg_order, agg_distinct);

// 		retval = (Node *) aggref;
// 	}
// 	else
// 	{
// 		/* window function */
// 		WindowFunc *wfunc = makeNode(WindowFunc);

// 		Assert(over);			/* lack of this was checked above */
// 		Assert(!agg_within_group);		/* also checked above */

// 		wfunc->winfnoid = funcid;
// 		wfunc->wintype = rettype;
// 		/* wincollid and inputcollid will be set by parse_collate.c */
// 		wfunc->args = fargs;
// 		/* winref will be set by transformWindowFuncCall */
// 		wfunc->winstar = agg_star;
// 		wfunc->winagg = (fdresult == FUNCDETAIL_AGGREGATE);
// 		wfunc->aggfilter = agg_filter;
// 		wfunc->location = location;

// 		/*
// 		 * agg_star is allowed for aggregate functions but distinct isn't
// 		 */
// 		if (agg_distinct)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				  errmsg("DISTINCT is not implemented for window functions"),
// 					 parser_errposition(pstate, location)));

// 		/*
// 		 * Reject attempt to call a parameterless aggregate without (*)
// 		 * syntax.  This is mere pedantry but some folks insisted ...
// 		 */
// 		if (wfunc->winagg && fargs == NIL && !agg_star)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("%s(*) must be used to call a parameterless aggregate function",
// 							NameListToString(funcname)),
// 					 parser_errposition(pstate, location)));

// 		/*
// 		 * ordered aggs not allowed in windows yet
// 		 */
// 		if (agg_order != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("aggregate ORDER BY is not implemented for window functions"),
// 					 parser_errposition(pstate, location)));

// 		/*
// 		 * FILTER is not yet supported with true window functions
// 		 */
// 		if (!wfunc->winagg && agg_filter)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("FILTER is not implemented for non-aggregate window functions"),
// 					 parser_errposition(pstate, location)));

// 		if (retset)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("window functions cannot return sets"),
// 					 parser_errposition(pstate, location)));

// 		/* parse_agg.c does additional window-func-specific processing */
// 		transformWindowFuncCall(pstate, wfunc, over);

// 		retval = (Node *) wfunc;
// 	}

// 	return retval;
// }

// FuncDetailCode
// func_get_detail(List *funcname,
// 				List *fargs,
// 				List *fargnames,
// 				int nargs,
// 				Oid *argtypes,
// 				bool expand_variadic,
// 				bool expand_defaults,
// 				Oid *funcid,	/* return value */
// 				Oid *rettype,	/* return value */
// 				bool *retset,	/* return value */
// 				int *nvargs,	/* return value */
// 				Oid *vatype,	/* return value */
// 				Oid **true_typeids,		/* return value */
// 				List **argdefaults)		/* optional return value */
// {
// 	FuncCandidateList raw_candidates;
// 	FuncCandidateList best_candidate;

// 	/* Passing NULL for argtypes is no longer allowed */
// 	Assert(argtypes);

// 	/* initialize output arguments to silence compiler warnings */
// 	*funcid = InvalidOid;
// 	*rettype = InvalidOid;
// 	*retset = false;
// 	*nvargs = 0;
// 	*vatype = InvalidOid;
// 	*true_typeids = NULL;
// 	if (argdefaults)
// 		*argdefaults = NIL;

// 	/* Get list of possible candidates from namespace search */
// 	raw_candidates = FuncnameGetCandidates(funcname, nargs, fargnames,
// 										   expand_variadic, expand_defaults,
// 										   false);

// 	/*
// 	 * Quickly check if there is an exact match to the input datatypes (there
// 	 * can be only one)
// 	 */
// 	for (best_candidate = raw_candidates;
// 		 best_candidate != NULL;
// 		 best_candidate = best_candidate->next)
// 	{
// 		if (memcmp(argtypes, best_candidate->args, nargs * sizeof(Oid)) == 0)
// 			break;
// 	}

// 	if (best_candidate == NULL)
// 	{
// 		/*
// 		 * If we didn't find an exact match, next consider the possibility
// 		 * that this is really a type-coercion request: a single-argument
// 		 * function call where the function name is a type name.  If so, and
// 		 * if the coercion path is RELABELTYPE or COERCEVIAIO, then go ahead
// 		 * and treat the "function call" as a coercion.
// 		 *
// 		 * This interpretation needs to be given higher priority than
// 		 * interpretations involving a type coercion followed by a function
// 		 * call, otherwise we can produce surprising results. For example, we
// 		 * want "text(varchar)" to be interpreted as a simple coercion, not as
// 		 * "text(name(varchar))" which the code below this point is entirely
// 		 * capable of selecting.
// 		 *
// 		 * We also treat a coercion of a previously-unknown-type literal
// 		 * constant to a specific type this way.
// 		 *
// 		 * The reason we reject COERCION_PATH_FUNC here is that we expect the
// 		 * cast implementation function to be named after the target type.
// 		 * Thus the function will be found by normal lookup if appropriate.
// 		 *
// 		 * The reason we reject COERCION_PATH_ARRAYCOERCE is mainly that you
// 		 * can't write "foo[] (something)" as a function call.  In theory
// 		 * someone might want to invoke it as "_foo (something)" but we have
// 		 * never supported that historically, so we can insist that people
// 		 * write it as a normal cast instead.
// 		 *
// 		 * We also reject the specific case of COERCEVIAIO for a composite
// 		 * source type and a string-category target type.  This is a case that
// 		 * find_coercion_pathway() allows by default, but experience has shown
// 		 * that it's too commonly invoked by mistake.  So, again, insist that
// 		 * people use cast syntax if they want to do that.
// 		 *
// 		 * NB: it's important that this code does not exceed what coerce_type
// 		 * can do, because the caller will try to apply coerce_type if we
// 		 * return FUNCDETAIL_COERCION.  If we return that result for something
// 		 * coerce_type can't handle, we'll cause infinite recursion between
// 		 * this module and coerce_type!
// 		 */
// 		if (nargs == 1 && fargs != NIL && fargnames == NIL)
// 		{
// 			Oid			targetType = FuncNameAsType(funcname);

// 			if (OidIsValid(targetType))
// 			{
// 				Oid			sourceType = argtypes[0];
// 				Node	   *arg1 = linitial(fargs);
// 				bool		iscoercion;

// 				if (sourceType == UNKNOWNOID && IsA(arg1, Const))
// 				{
// 					/* always treat typename('literal') as coercion */
// 					iscoercion = true;
// 				}
// 				else
// 				{
// 					CoercionPathType cpathtype;
// 					Oid			cfuncid;

// 					cpathtype = find_coercion_pathway(targetType, sourceType,
// 													  COERCION_EXPLICIT,
// 													  &cfuncid);
// 					switch (cpathtype)
// 					{
// 						case COERCION_PATH_RELABELTYPE:
// 							iscoercion = true;
// 							break;
// 						case COERCION_PATH_COERCEVIAIO:
// 							if ((sourceType == RECORDOID ||
// 								 ISCOMPLEX(sourceType)) &&
// 							  TypeCategory(targetType) == TYPCATEGORY_STRING)
// 								iscoercion = false;
// 							else
// 								iscoercion = true;
// 							break;
// 						default:
// 							iscoercion = false;
// 							break;
// 					}
// 				}

// 				if (iscoercion)
// 				{
// 					/* Treat it as a type coercion */
// 					*funcid = InvalidOid;
// 					*rettype = targetType;
// 					*retset = false;
// 					*nvargs = 0;
// 					*vatype = InvalidOid;
// 					*true_typeids = argtypes;
// 					return FUNCDETAIL_COERCION;
// 				}
// 			}
// 		}

// 		/*
// 		 * didn't find an exact match, so now try to match up candidates...
// 		 */
// 		if (raw_candidates != NULL)
// 		{
// 			FuncCandidateList current_candidates;
// 			int			ncandidates;

// 			ncandidates = func_match_argtypes(nargs,
// 											  argtypes,
// 											  raw_candidates,
// 											  &current_candidates);

// 			/* one match only? then run with it... */
// 			if (ncandidates == 1)
// 				best_candidate = current_candidates;

// 			/*
// 			 * multiple candidates? then better decide or throw an error...
// 			 */
// 			else if (ncandidates > 1)
// 			{
// 				best_candidate = func_select_candidate(nargs,
// 													   argtypes,
// 													   current_candidates);

// 				/*
// 				 * If we were able to choose a best candidate, we're done.
// 				 * Otherwise, ambiguous function call.
// 				 */
// 				if (!best_candidate)
// 					return FUNCDETAIL_MULTIPLE;
// 			}
// 		}
// 	}

// 	if (best_candidate)
// 	{
// 		HeapTuple	ftup;
// 		Form_pg_proc pform;
// 		FuncDetailCode result;

// 		/*
// 		 * If processing named args or expanding variadics or defaults, the
// 		 * "best candidate" might represent multiple equivalently good
// 		 * functions; treat this case as ambiguous.
// 		 */
// 		if (!OidIsValid(best_candidate->oid))
// 			return FUNCDETAIL_MULTIPLE;

// 		/*
// 		 * We disallow VARIADIC with named arguments unless the last argument
// 		 * (the one with VARIADIC attached) actually matched the variadic
// 		 * parameter.  This is mere pedantry, really, but some folks insisted.
// 		 */
// 		if (fargnames != NIL && !expand_variadic && nargs > 0 &&
// 			best_candidate->argnumbers[nargs - 1] != nargs - 1)
// 			return FUNCDETAIL_NOTFOUND;

// 		*funcid = best_candidate->oid;
// 		*nvargs = best_candidate->nvargs;
// 		*true_typeids = best_candidate->args;

// 		/*
// 		 * If processing named args, return actual argument positions into
// 		 * NamedArgExpr nodes in the fargs list.  This is a bit ugly but not
// 		 * worth the extra notation needed to do it differently.
// 		 */
// 		if (best_candidate->argnumbers != NULL)
// 		{
// 			int			i = 0;
// 			ListCell   *lc;

// 			foreach(lc, fargs)
// 			{
// 				NamedArgExpr *na = (NamedArgExpr *) lfirst(lc);

// 				if (IsA(na, NamedArgExpr))
// 					na->argnumber = best_candidate->argnumbers[i];
// 				i++;
// 			}
// 		}

// 		ftup = SearchSysCache1(PROCOID,
// 							   ObjectIdGetDatum(best_candidate->oid));
// 		if (!HeapTupleIsValid(ftup))	/* should not happen */
// 			elog(ERROR, "cache lookup failed for function %u",
// 				 best_candidate->oid);
// 		pform = (Form_pg_proc) GETSTRUCT(ftup);
// 		*rettype = pform->prorettype;
// 		*retset = pform->proretset;
// 		*vatype = pform->provariadic;
// 		/* fetch default args if caller wants 'em */
// 		if (argdefaults && best_candidate->ndargs > 0)
// 		{
// 			Datum		proargdefaults;
// 			bool		isnull;
// 			char	   *str;
// 			List	   *defaults;

// 			/* shouldn't happen, FuncnameGetCandidates messed up */
// 			if (best_candidate->ndargs > pform->pronargdefaults)
// 				elog(ERROR, "not enough default arguments");

// 			proargdefaults = SysCacheGetAttr(PROCOID, ftup,
// 											 Anum_pg_proc_proargdefaults,
// 											 &isnull);
// 			Assert(!isnull);
// 			str = TextDatumGetCString(proargdefaults);
// 			defaults = (List *) stringToNode(str);
// 			Assert(IsA(defaults, List));
// 			pfree(str);

// 			/* Delete any unused defaults from the returned list */
// 			if (best_candidate->argnumbers != NULL)
// 			{
// 				/*
// 				 * This is a bit tricky in named notation, since the supplied
// 				 * arguments could replace any subset of the defaults.  We
// 				 * work by making a bitmapset of the argnumbers of defaulted
// 				 * arguments, then scanning the defaults list and selecting
// 				 * the needed items.  (This assumes that defaulted arguments
// 				 * should be supplied in their positional order.)
// 				 */
// 				Bitmapset  *defargnumbers;
// 				int		   *firstdefarg;
// 				List	   *newdefaults;
// 				ListCell   *lc;
// 				int			i;

// 				defargnumbers = NULL;
// 				firstdefarg = &best_candidate->argnumbers[best_candidate->nargs - best_candidate->ndargs];
// 				for (i = 0; i < best_candidate->ndargs; i++)
// 					defargnumbers = bms_add_member(defargnumbers,
// 												   firstdefarg[i]);
// 				newdefaults = NIL;
// 				i = pform->pronargs - pform->pronargdefaults;
// 				foreach(lc, defaults)
// 				{
// 					if (bms_is_member(i, defargnumbers))
// 						newdefaults = lappend(newdefaults, lfirst(lc));
// 					i++;
// 				}
// 				Assert(list_length(newdefaults) == best_candidate->ndargs);
// 				bms_free(defargnumbers);
// 				*argdefaults = newdefaults;
// 			}
// 			else
// 			{
// 				/*
// 				 * Defaults for positional notation are lots easier; just
// 				 * remove any unwanted ones from the front.
// 				 */
// 				int			ndelete;

// 				ndelete = list_length(defaults) - best_candidate->ndargs;
// 				while (ndelete-- > 0)
// 					defaults = list_delete_first(defaults);
// 				*argdefaults = defaults;
// 			}
// 		}
// 		if (pform->proisagg)
// 			result = FUNCDETAIL_AGGREGATE;
// 		else if (pform->proiswindow)
// 			result = FUNCDETAIL_WINDOWFUNC;
// 		else
// 			result = FUNCDETAIL_NORMAL;
// 		ReleaseSysCache(ftup);
// 		return result;
// 	}

// 	return FUNCDETAIL_NOTFOUND;
// }

// List *
// raw_parser(const char *str)
// {
// 	core_yyscan_t yyscanner;
// 	base_yy_extra_type yyextra;
// 	int			yyresult;

// 	/* initialize the flex scanner */
// 	yyscanner = scanner_init(str, &yyextra.core_yy_extra,
// 							 ScanKeywords, NumScanKeywords);

// 	/* base_yylex() only needs this much initialization */
// 	yyextra.have_lookahead = false;

// 	/* initialize the bison parser */
// 	parser_init(&yyextra);

// 	/* Parse! */
// 	yyresult = base_yyparse(yyscanner);

// 	/* Clean up (release memory) */
// 	scanner_finish(yyscanner);

// 	if (yyresult)				/* error */
// 		return NIL;

// 	return yyextra.parsetree;
// }

// bool
// contain_windowfuncs(Node *node)
// {
// 	/*
// 	 * Must be prepared to start with a Query or a bare expression tree; if
// 	 * it's a Query, we don't want to increment sublevels_up.
// 	 */
// 	return query_or_expression_tree_walker(node,
// 										   contain_windowfuncs_walker,
// 										   NULL,
// 										   0);
// }

// Node *
// ReplaceVarsFromTargetList(Node *node,
// 						  int target_varno, int sublevels_up,
// 						  RangeTblEntry *target_rte,
// 						  List *targetlist,
// 						  ReplaceVarsNoMatchOption nomatch_option,
// 						  int nomatch_varno,
// 						  bool *outer_hasSubLinks)
// {
// 	ReplaceVarsFromTargetList_context context;

// 	context.target_rte = target_rte;
// 	context.targetlist = targetlist;
// 	context.nomatch_option = nomatch_option;
// 	context.nomatch_varno = nomatch_varno;

// 	return replace_rte_variables(node, target_varno, sublevels_up,
// 								 ReplaceVarsFromTargetList_callback,
// 								 (void *) &context,
// 								 outer_hasSubLinks);
// }

// void
// XactLockTableWait(TransactionId xid, Relation rel, ItemPointer ctid,
// 				  XLTW_Oper oper)
// {
// 	LOCKTAG		tag;
// 	XactLockTableWaitInfo info;
// 	ErrorContextCallback callback;

// 	/*
// 	 * If an operation is specified, set up our verbose error context
// 	 * callback.
// 	 */
// 	if (oper != XLTW_None)
// 	{
// 		Assert(RelationIsValid(rel));
// 		Assert(ItemPointerIsValid(ctid));

// 		info.rel = rel;
// 		info.ctid = ctid;
// 		info.oper = oper;

// 		callback.callback = XactLockTableWaitErrorCb;
// 		callback.arg = &info;
// 		callback.previous = error_context_stack;
// 		error_context_stack = &callback;
// 	}

// 	for (;;)
// 	{
// 		Assert(TransactionIdIsValid(xid));
// 		Assert(!TransactionIdEquals(xid, GetTopTransactionIdIfAny()));

// 		SET_LOCKTAG_TRANSACTION(tag, xid);

// 		(void) LockAcquire(&tag, ShareLock, false, false);

// 		LockRelease(&tag, ShareLock, false);

// 		if (!TransactionIdIsInProgress(xid))
// 			break;
// 		xid = SubTransGetParent(xid);
// 	}

// 	if (oper != XLTW_None)
// 		error_context_stack = callback.previous;
// }

// bool
// ConditionalXactLockTableWait(TransactionId xid)
// {
// 	LOCKTAG		tag;

// 	for (;;)
// 	{
// 		Assert(TransactionIdIsValid(xid));
// 		Assert(!TransactionIdEquals(xid, GetTopTransactionIdIfAny()));

// 		SET_LOCKTAG_TRANSACTION(tag, xid);

// 		if (LockAcquire(&tag, ShareLock, false, true) == LOCKACQUIRE_NOT_AVAIL)
// 			return false;

// 		LockRelease(&tag, ShareLock, false);

// 		if (!TransactionIdIsInProgress(xid))
// 			break;
// 		xid = SubTransGetParent(xid);
// 	}

// 	return true;
// }

// void
// WaitForLockers(LOCKTAG heaplocktag, LOCKMODE lockmode)
// {
// 	List	   *l;

// 	l = list_make1(&heaplocktag);
// 	WaitForLockersMultiple(l, lockmode);
// 	list_free(l);
// }

// uint32
// SpeculativeInsertionLockAcquire(TransactionId xid)
// {
// 	LOCKTAG		tag;

// 	speculativeInsertionToken++;

// 	/*
// 	 * Check for wrap-around. Zero means no token is held, so don't use that.
// 	 */
// 	if (speculativeInsertionToken == 0)
// 		speculativeInsertionToken = 1;

// 	SET_LOCKTAG_SPECULATIVE_INSERTION(tag, xid, speculativeInsertionToken);

// 	(void) LockAcquire(&tag, ExclusiveLock, false, false);

// 	return speculativeInsertionToken;
// }

// void
// SpeculativeInsertionLockRelease(TransactionId xid)
// {
// 	LOCKTAG		tag;

// 	SET_LOCKTAG_SPECULATIVE_INSERTION(tag, xid, speculativeInsertionToken);

// 	LockRelease(&tag, ExclusiveLock, false);
// }

// void
// SpeculativeInsertionWait(TransactionId xid, uint32 token)
// {
// 	LOCKTAG		tag;

// 	SET_LOCKTAG_SPECULATIVE_INSERTION(tag, xid, token);

// 	Assert(TransactionIdIsValid(xid));
// 	Assert(token != 0);

// 	(void) LockAcquire(&tag, ShareLock, false, false);
// 	LockRelease(&tag, ShareLock, false);
// }

// List *
// pg_parse_query(const char *query_string)
// {
// 	List	   *raw_parsetree_list;

// 	TRACE_POSTGRESQL_QUERY_PARSE_START(query_string);

// 	if (log_parser_stats)
// 		ResetUsage();

// 	raw_parsetree_list = raw_parser(query_string);

// 	if (log_parser_stats)
// 		ShowUsage("PARSER STATISTICS");

// #ifdef COPY_PARSE_PLAN_TREES
// 	/* Optional debugging check: pass raw parsetrees through copyObject() */
// 	{
// 		List	   *new_list = (List *) copyObject(raw_parsetree_list);

// 		/* This checks both copyObject() and the equal() routines... */
// 		if (!equal(new_list, raw_parsetree_list))
// 			elog(WARNING, "copyObject() failed to produce an equal raw parse tree");
// 		else
// 			raw_parsetree_list = new_list;
// 	}
// #endif

// 	TRACE_POSTGRESQL_QUERY_PARSE_DONE(query_string);

// 	return raw_parsetree_list;
// }

// PlannedStmt *
// pg_plan_query(Query *querytree, int cursorOptions, ParamListInfo boundParams)
// {
// 	PlannedStmt *plan;

// 	/* Utility commands have no plans. */
// 	if (querytree->commandType == CMD_UTILITY)
// 		return NULL;

// 	/* Planner must have a snapshot in case it calls user-defined functions. */
// 	Assert(ActiveSnapshotSet());

// 	TRACE_POSTGRESQL_QUERY_PLAN_START();

// 	if (log_planner_stats)
// 		ResetUsage();

// 	/* call the optimizer */
// 	plan = planner(querytree, cursorOptions, boundParams);

// 	if (log_planner_stats)
// 		ShowUsage("PLANNER STATISTICS");

// #ifdef COPY_PARSE_PLAN_TREES
// 	/* Optional debugging check: pass plan output through copyObject() */
// 	{
// 		PlannedStmt *new_plan = (PlannedStmt *) copyObject(plan);

// 		/*
// 		 * equal() currently does not have routines to compare Plan nodes, so
// 		 * don't try to test equality here.  Perhaps fix someday?
// 		 */
// #ifdef NOT_USED
// 		/* This checks both copyObject() and the equal() routines... */
// 		if (!equal(new_plan, plan))
// 			elog(WARNING, "copyObject() failed to produce an equal plan tree");
// 		else
// #endif
// 			plan = new_plan;
// 	}
// #endif

// 	/*
// 	 * Print plan if debugging.
// 	 */
// 	if (Debug_print_plan)
// 		elog_node_display(LOG, "plan", plan, Debug_pretty_print);

// 	TRACE_POSTGRESQL_QUERY_PLAN_DONE();

// 	return plan;
// }

// void
// quickdie(SIGNAL_ARGS)
// {
// 	sigaddset(&BlockSig, SIGQUIT);		/* prevent nested calls */
// 	PG_SETMASK(&BlockSig);

// 	/*
// 	 * Prevent interrupts while exiting; though we just blocked signals that
// 	 * would queue new interrupts, one may have been pending.  We don't want a
// 	 * quickdie() downgraded to a mere query cancel.
// 	 */
// 	HOLD_INTERRUPTS();

// 	/*
// 	 * If we're aborting out of client auth, don't risk trying to send
// 	 * anything to the client; we will likely violate the protocol, not to
// 	 * mention that we may have interrupted the guts of OpenSSL or some
// 	 * authentication library.
// 	 */
// 	if (ClientAuthInProgress && whereToSendOutput == DestRemote)
// 		whereToSendOutput = DestNone;

// 	/*
// 	 * Ideally this should be ereport(FATAL), but then we'd not get control
// 	 * back...
// 	 */
// 	ereport(WARNING,
// 			(errcode(ERRCODE_CRASH_SHUTDOWN),
// 			 errmsg("terminating connection because of crash of another server process"),
// 	errdetail("The postmaster has commanded this server process to roll back"
// 			  " the current transaction and exit, because another"
// 			  " server process exited abnormally and possibly corrupted"
// 			  " shared memory."),
// 			 errhint("In a moment you should be able to reconnect to the"
// 					 " database and repeat your command.")));

// 	/*
// 	 * We DO NOT want to run proc_exit() callbacks -- we're here because
// 	 * shared memory may be corrupted, so we don't want to try to clean up our
// 	 * transaction.  Just nail the windows shut and get out of town.  Now that
// 	 * there's an atexit callback to prevent third-party code from breaking
// 	 * things by calling exit() directly, we have to reset the callbacks
// 	 * explicitly to make this work as intended.
// 	 */
// 	on_exit_reset();

// 	/*
// 	 * Note we do exit(2) not exit(0).  This is to force the postmaster into a
// 	 * system reset cycle if some idiot DBA sends a manual SIGQUIT to a random
// 	 * backend.  This is necessary precisely because we don't clean up our
// 	 * shared memory state.  (The "dead man switch" mechanism in pmsignal.c
// 	 * should ensure the postmaster sees this as a crash, too, but no harm in
// 	 * being doubly sure.)
// 	 */
// 	exit(2);
// }

// void
// ProcessClientReadInterrupt(bool blocked)
// {
// 	int			save_errno = errno;

// 	if (DoingCommandRead)
// 	{
// 		/* Check for general interrupts that arrived while reading */
// 		CHECK_FOR_INTERRUPTS();

// 		/* Process sinval catchup interrupts that happened while reading */
// 		if (catchupInterruptPending)
// 			ProcessCatchupInterrupt();

// 		/* Process sinval catchup interrupts that happened while reading */
// 		if (notifyInterruptPending)
// 			ProcessNotifyInterrupt();
// 	}
// 	else if (ProcDiePending && blocked)
// 	{
// 		/*
// 		 * We're dying. It's safe (and sane) to handle that now.
// 		 */
// 		CHECK_FOR_INTERRUPTS();
// 	}

// 	errno = save_errno;
// }

// void
// ProcessClientWriteInterrupt(bool blocked)
// {
// 	int			save_errno = errno;

// 	/*
// 	 * We only want to process the interrupt here if socket writes are
// 	 * blocking to increase the chance to get an error message to the client.
// 	 * If we're not blocked there'll soon be a CHECK_FOR_INTERRUPTS(). But if
// 	 * we're blocked we'll never get out of that situation if the client has
// 	 * died.
// 	 */
// 	if (ProcDiePending && blocked)
// 	{
// 		/*
// 		 * We're dying. It's safe (and sane) to handle that now. But we don't
// 		 * want to send the client the error message as that a) would possibly
// 		 * block again b) would possibly lead to sending an error message to
// 		 * the client, while we already started to send something else.
// 		 */
// 		if (whereToSendOutput == DestRemote)
// 			whereToSendOutput = DestNone;

// 		CHECK_FOR_INTERRUPTS();
// 	}

// 	errno = save_errno;
// }

// void
// PostgresMain(int argc, char *argv[],
// 			 const char *dbname,
// 			 const char *username)
// {
// 	int			firstchar;
// 	StringInfoData input_message;
// 	sigjmp_buf	local_sigjmp_buf;
// 	volatile bool send_ready_for_query = true;
// 	bool		disable_idle_in_transaction_timeout = false;

// 	/* Initialize startup process environment if necessary. */
// 	if (!IsUnderPostmaster)
// 		InitStandaloneProcess(argv[0]);

// 	SetProcessingMode(InitProcessing);

// 	/*
// 	 * Set default values for command-line options.
// 	 */
// 	if (!IsUnderPostmaster)
// 		InitializeGUCOptions();

// 	/*
// 	 * Parse command-line options.
// 	 */
// 	process_postgres_switches(argc, argv, PGC_POSTMASTER, &dbname);

// 	/* Must have gotten a database name, or have a default (the username) */
// 	if (dbname == NULL)
// 	{
// 		dbname = username;
// 		if (dbname == NULL)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("%s: no database nor user name specified",
// 							progname)));
// 	}

// 	/* Acquire configuration parameters, unless inherited from postmaster */
// 	if (!IsUnderPostmaster)
// 	{
// 		if (!SelectConfigFiles(userDoption, progname))
// 			proc_exit(1);
// 	}

// 	/*
// 	 * Set up signal handlers and masks.
// 	 *
// 	 * Note that postmaster blocked all signals before forking child process,
// 	 * so there is no race condition whereby we might receive a signal before
// 	 * we have set up the handler.
// 	 *
// 	 * Also note: it's best not to use any signals that are SIG_IGNored in the
// 	 * postmaster.  If such a signal arrives before we are able to change the
// 	 * handler to non-SIG_IGN, it'll get dropped.  Instead, make a dummy
// 	 * handler in the postmaster to reserve the signal. (Of course, this isn't
// 	 * an issue for signals that are locally generated, such as SIGALRM and
// 	 * SIGPIPE.)
// 	 */
// 	if (am_walsender)
// 		WalSndSignals();
// 	else
// 	{
// 		pqsignal(SIGHUP, SigHupHandler);		/* set flag to read config
// 												 * file */
// 		pqsignal(SIGINT, StatementCancelHandler);		/* cancel current query */
// 		pqsignal(SIGTERM, die); /* cancel current query and exit */

// 		/*
// 		 * In a standalone backend, SIGQUIT can be generated from the keyboard
// 		 * easily, while SIGTERM cannot, so we make both signals do die()
// 		 * rather than quickdie().
// 		 */
// 		if (IsUnderPostmaster)
// 			pqsignal(SIGQUIT, quickdie);		/* hard crash time */
// 		else
// 			pqsignal(SIGQUIT, die);		/* cancel current query and exit */
// 		InitializeTimeouts();	/* establishes SIGALRM handler */

// 		/*
// 		 * Ignore failure to write to frontend. Note: if frontend closes
// 		 * connection, we will notice it and exit cleanly when control next
// 		 * returns to outer loop.  This seems safer than forcing exit in the
// 		 * midst of output during who-knows-what operation...
// 		 */
// 		pqsignal(SIGPIPE, SIG_IGN);
// 		pqsignal(SIGUSR1, procsignal_sigusr1_handler);
// 		pqsignal(SIGUSR2, SIG_IGN);
// 		pqsignal(SIGFPE, FloatExceptionHandler);

// 		/*
// 		 * Reset some signals that are accepted by postmaster but not by
// 		 * backend
// 		 */
// 		pqsignal(SIGCHLD, SIG_DFL);		/* system() requires this on some
// 										 * platforms */
// 	}

// 	pqinitmask();

// 	if (IsUnderPostmaster)
// 	{
// 		/* We allow SIGQUIT (quickdie) at all times */
// 		sigdelset(&BlockSig, SIGQUIT);
// 	}

// 	PG_SETMASK(&BlockSig);		/* block everything except SIGQUIT */

// 	if (!IsUnderPostmaster)
// 	{
// 		/*
// 		 * Validate we have been given a reasonable-looking DataDir (if under
// 		 * postmaster, assume postmaster did this already).
// 		 */
// 		Assert(DataDir);
// 		ValidatePgVersion(DataDir);

// 		/* Change into DataDir (if under postmaster, was done already) */
// 		ChangeToDataDir();

// 		/*
// 		 * Create lockfile for data directory.
// 		 */
// 		CreateDataDirLockFile(false);

// 		/* Initialize MaxBackends (if under postmaster, was done already) */
// 		InitializeMaxBackends();
// 	}

// 	/* Early initialization */
// 	BaseInit();

// 	/*
// 	 * Create a per-backend PGPROC struct in shared memory, except in the
// 	 * EXEC_BACKEND case where this was done in SubPostmasterMain. We must do
// 	 * this before we can use LWLocks (and in the EXEC_BACKEND case we already
// 	 * had to do some stuff with LWLocks).
// 	 */
// #ifdef EXEC_BACKEND
// 	if (!IsUnderPostmaster)
// 		InitProcess();
// #else
// 	InitProcess();
// #endif

// 	/* We need to allow SIGINT, etc during the initial transaction */
// 	PG_SETMASK(&UnBlockSig);

// 	/*
// 	 * General initialization.
// 	 *
// 	 * NOTE: if you are tempted to add code in this vicinity, consider putting
// 	 * it inside InitPostgres() instead.  In particular, anything that
// 	 * involves database access should be there, not here.
// 	 */
// 	InitPostgres(dbname, InvalidOid, username, InvalidOid, NULL);

// 	/*
// 	 * If the PostmasterContext is still around, recycle the space; we don't
// 	 * need it anymore after InitPostgres completes.  Note this does not trash
// 	 * *MyProcPort, because ConnCreate() allocated that space with malloc()
// 	 * ... else we'd need to copy the Port data first.  Also, subsidiary data
// 	 * such as the username isn't lost either; see ProcessStartupPacket().
// 	 */
// 	if (PostmasterContext)
// 	{
// 		MemoryContextDelete(PostmasterContext);
// 		PostmasterContext = NULL;
// 	}

// 	SetProcessingMode(NormalProcessing);

// 	/*
// 	 * Now all GUC states are fully set up.  Report them to client if
// 	 * appropriate.
// 	 */
// 	BeginReportingGUCOptions();

// 	/*
// 	 * Also set up handler to log session end; we have to wait till now to be
// 	 * sure Log_disconnections has its final value.
// 	 */
// 	if (IsUnderPostmaster && Log_disconnections)
// 		on_proc_exit(log_disconnections, 0);

// 	/* Perform initialization specific to a WAL sender process. */
// 	if (am_walsender)
// 		InitWalSender();

// 	/*
// 	 * process any libraries that should be preloaded at backend start (this
// 	 * likewise can't be done until GUC settings are complete)
// 	 */
// 	process_session_preload_libraries();

// 	/*
// 	 * Send this backend's cancellation info to the frontend.
// 	 */
// 	if (whereToSendOutput == DestRemote &&
// 		PG_PROTOCOL_MAJOR(FrontendProtocol) >= 2)
// 	{
// 		StringInfoData buf;

// 		pq_beginmessage(&buf, 'K');
// 		pq_sendint(&buf, (int32) MyProcPid, sizeof(int32));
// 		pq_sendint(&buf, (int32) MyCancelKey, sizeof(int32));
// 		pq_endmessage(&buf);
// 		/* Need not flush since ReadyForQuery will do it. */
// 	}

// 	/* Welcome banner for standalone case */
// 	if (whereToSendOutput == DestDebug)
// 		printf("\nPostgreSQL stand-alone backend %s\n", PG_VERSION);

// 	/*
// 	 * Create the memory context we will use in the main loop.
// 	 *
// 	 * MessageContext is reset once per iteration of the main loop, ie, upon
// 	 * completion of processing of each command message from the client.
// 	 */
// 	MessageContext = AllocSetContextCreate(TopMemoryContext,
// 										   "MessageContext",
// 										   ALLOCSET_DEFAULT_SIZES);

// 	/*
// 	 * Remember stand-alone backend startup time
// 	 */
// 	if (!IsUnderPostmaster)
// 		PgStartTime = GetCurrentTimestamp();

// 	/*
// 	 * POSTGRES main processing loop begins here
// 	 *
// 	 * If an exception is encountered, processing resumes here so we abort the
// 	 * current transaction and start a new one.
// 	 *
// 	 * You might wonder why this isn't coded as an infinite loop around a
// 	 * PG_TRY construct.  The reason is that this is the bottom of the
// 	 * exception stack, and so with PG_TRY there would be no exception handler
// 	 * in force at all during the CATCH part.  By leaving the outermost setjmp
// 	 * always active, we have at least some chance of recovering from an error
// 	 * during error recovery.  (If we get into an infinite loop thereby, it
// 	 * will soon be stopped by overflow of elog.c's internal state stack.)
// 	 *
// 	 * Note that we use sigsetjmp(..., 1), so that this function's signal mask
// 	 * (to wit, UnBlockSig) will be restored when longjmp'ing to here.  This
// 	 * is essential in case we longjmp'd out of a signal handler on a platform
// 	 * where that leaves the signal blocked.  It's not redundant with the
// 	 * unblock in AbortTransaction() because the latter is only called if we
// 	 * were inside a transaction.
// 	 */

// 	if (sigsetjmp(local_sigjmp_buf, 1) != 0)
// 	{
// 		/*
// 		 * NOTE: if you are tempted to add more code in this if-block,
// 		 * consider the high probability that it should be in
// 		 * AbortTransaction() instead.  The only stuff done directly here
// 		 * should be stuff that is guaranteed to apply *only* for outer-level
// 		 * error recovery, such as adjusting the FE/BE protocol status.
// 		 */

// 		/* Since not using PG_TRY, must reset error stack by hand */
// 		error_context_stack = NULL;

// 		/* Prevent interrupts while cleaning up */
// 		HOLD_INTERRUPTS();

// 		/*
// 		 * Forget any pending QueryCancel request, since we're returning to
// 		 * the idle loop anyway, and cancel any active timeout requests.  (In
// 		 * future we might want to allow some timeout requests to survive, but
// 		 * at minimum it'd be necessary to do reschedule_timeouts(), in case
// 		 * we got here because of a query cancel interrupting the SIGALRM
// 		 * interrupt handler.)	Note in particular that we must clear the
// 		 * statement and lock timeout indicators, to prevent any future plain
// 		 * query cancels from being misreported as timeouts in case we're
// 		 * forgetting a timeout cancel.
// 		 */
// 		disable_all_timeouts(false);
// 		QueryCancelPending = false;		/* second to avoid race condition */

// 		/* Not reading from the client anymore. */
// 		DoingCommandRead = false;

// 		/* Make sure libpq is in a good state */
// 		pq_comm_reset();

// 		/* Report the error to the client and/or server log */
// 		EmitErrorReport();

// 		/*
// 		 * Make sure debug_query_string gets reset before we possibly clobber
// 		 * the storage it points at.
// 		 */
// 		debug_query_string = NULL;

// 		/*
// 		 * Abort the current transaction in order to recover.
// 		 */
// 		AbortCurrentTransaction();

// 		if (am_walsender)
// 			WalSndErrorCleanup();

// 		/*
// 		 * We can't release replication slots inside AbortTransaction() as we
// 		 * need to be able to start and abort transactions while having a slot
// 		 * acquired. But we never need to hold them across top level errors,
// 		 * so releasing here is fine. There's another cleanup in ProcKill()
// 		 * ensuring we'll correctly cleanup on FATAL errors as well.
// 		 */
// 		if (MyReplicationSlot != NULL)
// 			ReplicationSlotRelease();

// 		/*
// 		 * Now return to normal top-level context and clear ErrorContext for
// 		 * next time.
// 		 */
// 		MemoryContextSwitchTo(TopMemoryContext);
// 		FlushErrorState();

// 		/*
// 		 * If we were handling an extended-query-protocol message, initiate
// 		 * skip till next Sync.  This also causes us not to issue
// 		 * ReadyForQuery (until we get Sync).
// 		 */
// 		if (doing_extended_query_message)
// 			ignore_till_sync = true;

// 		/* We don't have a transaction command open anymore */
// 		xact_started = false;

// 		/*
// 		 * If an error occurred while we were reading a message from the
// 		 * client, we have potentially lost track of where the previous
// 		 * message ends and the next one begins.  Even though we have
// 		 * otherwise recovered from the error, we cannot safely read any more
// 		 * messages from the client, so there isn't much we can do with the
// 		 * connection anymore.
// 		 */
// 		if (pq_is_reading_msg())
// 			ereport(FATAL,
// 					(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 					 errmsg("terminating connection because protocol synchronization was lost")));

// 		/* Now we can allow interrupts again */
// 		RESUME_INTERRUPTS();
// 	}

// 	/* We can now handle ereport(ERROR) */
// 	PG_exception_stack = &local_sigjmp_buf;

// 	if (!ignore_till_sync)
// 		send_ready_for_query = true;	/* initially, or after error */

// 	/*
// 	 * Non-error queries loop here.
// 	 */

// 	for (;;)
// 	{
// 		/*
// 		 * At top of loop, reset extended-query-message flag, so that any
// 		 * errors encountered in "idle" state don't provoke skip.
// 		 */
// 		doing_extended_query_message = false;

// 		/*
// 		 * Release storage left over from prior query cycle, and create a new
// 		 * query input buffer in the cleared MessageContext.
// 		 */
// 		MemoryContextSwitchTo(MessageContext);
// 		MemoryContextResetAndDeleteChildren(MessageContext);

// 		initStringInfo(&input_message);

// 		/*
// 		 * Also consider releasing our catalog snapshot if any, so that it's
// 		 * not preventing advance of global xmin while we wait for the client.
// 		 */
// 		InvalidateCatalogSnapshotConditionally();

// 		/*
// 		 * (1) If we've reached idle state, tell the frontend we're ready for
// 		 * a new query.
// 		 *
// 		 * Note: this includes fflush()'ing the last of the prior output.
// 		 *
// 		 * This is also a good time to send collected statistics to the
// 		 * collector, and to update the PS stats display.  We avoid doing
// 		 * those every time through the message loop because it'd slow down
// 		 * processing of batched messages, and because we don't want to report
// 		 * uncommitted updates (that confuses autovacuum).  The notification
// 		 * processor wants a call too, if we are not in a transaction block.
// 		 */
// 		if (send_ready_for_query)
// 		{
// 			if (IsAbortedTransactionBlockState())
// 			{
// 				set_ps_display("idle in transaction (aborted)", false);
// 				pgstat_report_activity(STATE_IDLEINTRANSACTION_ABORTED, NULL);

// 				/* Start the idle-in-transaction timer */
// 				if (IdleInTransactionSessionTimeout > 0)
// 				{
// 					disable_idle_in_transaction_timeout = true;
// 					enable_timeout_after(IDLE_IN_TRANSACTION_SESSION_TIMEOUT,
// 										 IdleInTransactionSessionTimeout);
// 				}
// 			}
// 			else if (IsTransactionOrTransactionBlock())
// 			{
// 				set_ps_display("idle in transaction", false);
// 				pgstat_report_activity(STATE_IDLEINTRANSACTION, NULL);

// 				/* Start the idle-in-transaction timer */
// 				if (IdleInTransactionSessionTimeout > 0)
// 				{
// 					disable_idle_in_transaction_timeout = true;
// 					enable_timeout_after(IDLE_IN_TRANSACTION_SESSION_TIMEOUT,
// 										 IdleInTransactionSessionTimeout);
// 				}
// 			}
// 			else
// 			{
// 				ProcessCompletedNotifies();
// 				pgstat_report_stat(false);

// 				set_ps_display("idle", false);
// 				pgstat_report_activity(STATE_IDLE, NULL);
// 			}

// 			ReadyForQuery(whereToSendOutput);
// 			send_ready_for_query = false;
// 		}

// 		/*
// 		 * (2) Allow asynchronous signals to be executed immediately if they
// 		 * come in while we are waiting for client input. (This must be
// 		 * conditional since we don't want, say, reads on behalf of COPY FROM
// 		 * STDIN doing the same thing.)
// 		 */
// 		DoingCommandRead = true;

// 		/*
// 		 * (3) read a command (loop blocks here)
// 		 */
// 		firstchar = ReadCommand(&input_message);

// 		/*
// 		 * (4) disable async signal conditions again.
// 		 *
// 		 * Query cancel is supposed to be a no-op when there is no query in
// 		 * progress, so if a query cancel arrived while we were idle, just
// 		 * reset QueryCancelPending. ProcessInterrupts() has that effect when
// 		 * it's called when DoingCommandRead is set, so check for interrupts
// 		 * before resetting DoingCommandRead.
// 		 */
// 		CHECK_FOR_INTERRUPTS();
// 		DoingCommandRead = false;

// 		/*
// 		 * (5) turn off the idle-in-transaction timeout
// 		 */
// 		if (disable_idle_in_transaction_timeout)
// 		{
// 			disable_timeout(IDLE_IN_TRANSACTION_SESSION_TIMEOUT, false);
// 			disable_idle_in_transaction_timeout = false;
// 		}

// 		/*
// 		 * (6) check for any other interesting events that happened while we
// 		 * slept.
// 		 */
// 		if (got_SIGHUP)
// 		{
// 			got_SIGHUP = false;
// 			ProcessConfigFile(PGC_SIGHUP);
// 		}

// 		/*
// 		 * (7) process the command.  But ignore it if we're skipping till
// 		 * Sync.
// 		 */
// 		if (ignore_till_sync && firstchar != EOF)
// 			continue;

// 		switch (firstchar)
// 		{
// 			case 'Q':			/* simple query */
// 				{
// 					const char *query_string;

// 					/* Set statement_timestamp() */
// 					SetCurrentStatementStartTimestamp();

// 					query_string = pq_getmsgstring(&input_message);
// 					pq_getmsgend(&input_message);

// 					if (am_walsender)
// 						exec_replication_command(query_string);
// 					else
// 						exec_simple_query(query_string);

// 					send_ready_for_query = true;
// 				}
// 				break;

// 			case 'P':			/* parse */
// 				{
// 					const char *stmt_name;
// 					const char *query_string;
// 					int			numParams;
// 					Oid		   *paramTypes = NULL;

// 					forbidden_in_wal_sender(firstchar);

// 					/* Set statement_timestamp() */
// 					SetCurrentStatementStartTimestamp();

// 					stmt_name = pq_getmsgstring(&input_message);
// 					query_string = pq_getmsgstring(&input_message);
// 					numParams = pq_getmsgint(&input_message, 2);
// 					if (numParams > 0)
// 					{
// 						int			i;

// 						paramTypes = (Oid *) palloc(numParams * sizeof(Oid));
// 						for (i = 0; i < numParams; i++)
// 							paramTypes[i] = pq_getmsgint(&input_message, 4);
// 					}
// 					pq_getmsgend(&input_message);

// 					exec_parse_message(query_string, stmt_name,
// 									   paramTypes, numParams);
// 				}
// 				break;

// 			case 'B':			/* bind */
// 				forbidden_in_wal_sender(firstchar);

// 				/* Set statement_timestamp() */
// 				SetCurrentStatementStartTimestamp();

// 				/*
// 				 * this message is complex enough that it seems best to put
// 				 * the field extraction out-of-line
// 				 */
// 				exec_bind_message(&input_message);
// 				break;

// 			case 'E':			/* execute */
// 				{
// 					const char *portal_name;
// 					int			max_rows;

// 					forbidden_in_wal_sender(firstchar);

// 					/* Set statement_timestamp() */
// 					SetCurrentStatementStartTimestamp();

// 					portal_name = pq_getmsgstring(&input_message);
// 					max_rows = pq_getmsgint(&input_message, 4);
// 					pq_getmsgend(&input_message);

// 					exec_execute_message(portal_name, max_rows);
// 				}
// 				break;

// 			case 'F':			/* fastpath function call */
// 				forbidden_in_wal_sender(firstchar);

// 				/* Set statement_timestamp() */
// 				SetCurrentStatementStartTimestamp();

// 				/* Report query to various monitoring facilities. */
// 				pgstat_report_activity(STATE_FASTPATH, NULL);
// 				set_ps_display("<FASTPATH>", false);

// 				/* start an xact for this function invocation */
// 				start_xact_command();

// 				/*
// 				 * Note: we may at this point be inside an aborted
// 				 * transaction.  We can't throw error for that until we've
// 				 * finished reading the function-call message, so
// 				 * HandleFunctionRequest() must check for it after doing so.
// 				 * Be careful not to do anything that assumes we're inside a
// 				 * valid transaction here.
// 				 */

// 				/* switch back to message context */
// 				MemoryContextSwitchTo(MessageContext);

// 				HandleFunctionRequest(&input_message);

// 				/* commit the function-invocation transaction */
// 				finish_xact_command();

// 				send_ready_for_query = true;
// 				break;

// 			case 'C':			/* close */
// 				{
// 					int			close_type;
// 					const char *close_target;

// 					forbidden_in_wal_sender(firstchar);

// 					close_type = pq_getmsgbyte(&input_message);
// 					close_target = pq_getmsgstring(&input_message);
// 					pq_getmsgend(&input_message);

// 					switch (close_type)
// 					{
// 						case 'S':
// 							if (close_target[0] != '\0')
// 								DropPreparedStatement(close_target, false);
// 							else
// 							{
// 								/* special-case the unnamed statement */
// 								drop_unnamed_stmt();
// 							}
// 							break;
// 						case 'P':
// 							{
// 								Portal		portal;

// 								portal = GetPortalByName(close_target);
// 								if (PortalIsValid(portal))
// 									PortalDrop(portal, false);
// 							}
// 							break;
// 						default:
// 							ereport(ERROR,
// 									(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 								   errmsg("invalid CLOSE message subtype %d",
// 										  close_type)));
// 							break;
// 					}

// 					if (whereToSendOutput == DestRemote)
// 						pq_putemptymessage('3');		/* CloseComplete */
// 				}
// 				break;

// 			case 'D':			/* describe */
// 				{
// 					int			describe_type;
// 					const char *describe_target;

// 					forbidden_in_wal_sender(firstchar);

// 					/* Set statement_timestamp() (needed for xact) */
// 					SetCurrentStatementStartTimestamp();

// 					describe_type = pq_getmsgbyte(&input_message);
// 					describe_target = pq_getmsgstring(&input_message);
// 					pq_getmsgend(&input_message);

// 					switch (describe_type)
// 					{
// 						case 'S':
// 							exec_describe_statement_message(describe_target);
// 							break;
// 						case 'P':
// 							exec_describe_portal_message(describe_target);
// 							break;
// 						default:
// 							ereport(ERROR,
// 									(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 								errmsg("invalid DESCRIBE message subtype %d",
// 									   describe_type)));
// 							break;
// 					}
// 				}
// 				break;

// 			case 'H':			/* flush */
// 				pq_getmsgend(&input_message);
// 				if (whereToSendOutput == DestRemote)
// 					pq_flush();
// 				break;

// 			case 'S':			/* sync */
// 				pq_getmsgend(&input_message);
// 				finish_xact_command();
// 				send_ready_for_query = true;
// 				break;

// 				/*
// 				 * 'X' means that the frontend is closing down the socket. EOF
// 				 * means unexpected loss of frontend connection. Either way,
// 				 * perform normal shutdown.
// 				 */
// 			case 'X':
// 			case EOF:

// 				/*
// 				 * Reset whereToSendOutput to prevent ereport from attempting
// 				 * to send any more messages to client.
// 				 */
// 				if (whereToSendOutput == DestRemote)
// 					whereToSendOutput = DestNone;

// 				/*
// 				 * NOTE: if you are tempted to add more code here, DON'T!
// 				 * Whatever you had in mind to do should be set up as an
// 				 * on_proc_exit or on_shmem_exit callback, instead. Otherwise
// 				 * it will fail to be called during other backend-shutdown
// 				 * scenarios.
// 				 */
// 				proc_exit(0);

// 			case 'd':			/* copy data */
// 			case 'c':			/* copy done */
// 			case 'f':			/* copy fail */

// 				/*
// 				 * Accept but ignore these messages, per protocol spec; we
// 				 * probably got here because a COPY failed, and the frontend
// 				 * is still sending data.
// 				 */
// 				break;

// 			default:
// 				ereport(FATAL,
// 						(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 						 errmsg("invalid frontend message type %d",
// 								firstchar)));
// 		}
// 	}							/* end of input-reading loop */
// }

// Datum
// array_cardinality(PG_FUNCTION_ARGS)
// {
// 	AnyArrayType *v = PG_GETARG_ANY_ARRAY(0);

// 	PG_RETURN_INT32(ArrayGetNItems(AARR_NDIM(v), AARR_DIMS(v)));
// }
// Datum
// array_remove(PG_FUNCTION_ARGS)
// {
// 	ArrayType  *array;
// 	Datum		search = PG_GETARG_DATUM(1);
// 	bool		search_isnull = PG_ARGISNULL(1);

// 	if (PG_ARGISNULL(0))
// 		PG_RETURN_NULL();
// 	array = PG_GETARG_ARRAYTYPE_P(0);

// 	array = array_replace_internal(array,
// 								   search, search_isnull,
// 								   (Datum) 0, true,
// 								   true, PG_GET_COLLATION(),
// 								   fcinfo);
// 	PG_RETURN_ARRAYTYPE_P(array);
// }

// Datum
// array_replace(PG_FUNCTION_ARGS)
// {
// 	ArrayType  *array;
// 	Datum		search = PG_GETARG_DATUM(1);
// 	bool		search_isnull = PG_ARGISNULL(1);
// 	Datum		replace = PG_GETARG_DATUM(2);
// 	bool		replace_isnull = PG_ARGISNULL(2);

// 	if (PG_ARGISNULL(0))
// 		PG_RETURN_NULL();
// 	array = PG_GETARG_ARRAYTYPE_P(0);

// 	array = array_replace_internal(array,
// 								   search, search_isnull,
// 								   replace, replace_isnull,
// 								   false, PG_GET_COLLATION(),
// 								   fcinfo);
// 	PG_RETURN_ARRAYTYPE_P(array);
// }
// Datum
// width_bucket_array(PG_FUNCTION_ARGS)
// {
// 	Datum		operand = PG_GETARG_DATUM(0);
// 	ArrayType  *thresholds = PG_GETARG_ARRAYTYPE_P(1);
// 	Oid			collation = PG_GET_COLLATION();
// 	Oid			element_type = ARR_ELEMTYPE(thresholds);
// 	int			result;

// 	/* Check input */
// 	if (ARR_NDIM(thresholds) > 1)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 				 errmsg("thresholds must be one-dimensional array")));

// 	if (array_contains_nulls(thresholds))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
// 				 errmsg("thresholds array must not contain NULLs")));

// 	/* We have a dedicated implementation for float8 data */
// 	if (element_type == FLOAT8OID)
// 		result = width_bucket_array_float8(operand, thresholds);
// 	else
// 	{
// 		TypeCacheEntry *typentry;

// 		/* Cache information about the input type */
// 		typentry = (TypeCacheEntry *) fcinfo->flinfo->fn_extra;
// 		if (typentry == NULL ||
// 			typentry->type_id != element_type)
// 		{
// 			typentry = lookup_type_cache(element_type,
// 										 TYPECACHE_CMP_PROC_FINFO);
// 			if (!OidIsValid(typentry->cmp_proc_finfo.fn_oid))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 				errmsg("could not identify a comparison function for type %s",
// 					   format_type_be(element_type))));
// 			fcinfo->flinfo->fn_extra = (void *) typentry;
// 		}

// 		/*
// 		 * We have separate implementation paths for fixed- and variable-width
// 		 * types, since indexing the array is a lot cheaper in the first case.
// 		 */
// 		if (typentry->typlen > 0)
// 			result = width_bucket_array_fixed(operand, thresholds,
// 											  collation, typentry);
// 		else
// 			result = width_bucket_array_variable(operand, thresholds,
// 												 collation, typentry);
// 	}

// 	/* Avoid leaking memory when handed toasted input. */
// 	PG_FREE_IF_COPY(thresholds, 1);

// 	PG_RETURN_INT32(result);
// }
// Datum
// array_get_element(Datum arraydatum,
// 				  int nSubscripts,
// 				  int *indx,
// 				  int arraytyplen,
// 				  int elmlen,
// 				  bool elmbyval,
// 				  char elmalign,
// 				  bool *isNull)
// {
// 	int			i,
// 				ndim,
// 			   *dim,
// 			   *lb,
// 				offset,
// 				fixedDim[1],
// 				fixedLb[1];
// 	char	   *arraydataptr,
// 			   *retptr;
// 	bits8	   *arraynullsptr;

// 	if (arraytyplen > 0)
// 	{
// 		/*
// 		 * fixed-length arrays -- these are assumed to be 1-d, 0-based
// 		 */
// 		ndim = 1;
// 		fixedDim[0] = arraytyplen / elmlen;
// 		fixedLb[0] = 0;
// 		dim = fixedDim;
// 		lb = fixedLb;
// 		arraydataptr = (char *) DatumGetPointer(arraydatum);
// 		arraynullsptr = NULL;
// 	}
// 	else if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(arraydatum)))
// 	{
// 		/* expanded array: let's do this in a separate function */
// 		return array_get_element_expanded(arraydatum,
// 										  nSubscripts,
// 										  indx,
// 										  arraytyplen,
// 										  elmlen,
// 										  elmbyval,
// 										  elmalign,
// 										  isNull);
// 	}
// 	else
// 	{
// 		/* detoast array if necessary, producing normal varlena input */
// 		ArrayType  *array = DatumGetArrayTypeP(arraydatum);

// 		ndim = ARR_NDIM(array);
// 		dim = ARR_DIMS(array);
// 		lb = ARR_LBOUND(array);
// 		arraydataptr = ARR_DATA_PTR(array);
// 		arraynullsptr = ARR_NULLBITMAP(array);
// 	}

// 	/*
// 	 * Return NULL for invalid subscript
// 	 */
// 	if (ndim != nSubscripts || ndim <= 0 || ndim > MAXDIM)
// 	{
// 		*isNull = true;
// 		return (Datum) 0;
// 	}
// 	for (i = 0; i < ndim; i++)
// 	{
// 		if (indx[i] < lb[i] || indx[i] >= (dim[i] + lb[i]))
// 		{
// 			*isNull = true;
// 			return (Datum) 0;
// 		}
// 	}

// 	/*
// 	 * Calculate the element number
// 	 */
// 	offset = ArrayGetOffset(nSubscripts, dim, lb, indx);

// 	/*
// 	 * Check for NULL array element
// 	 */
// 	if (array_get_isnull(arraynullsptr, offset))
// 	{
// 		*isNull = true;
// 		return (Datum) 0;
// 	}

// 	/*
// 	 * OK, get the element
// 	 */
// 	*isNull = false;
// 	retptr = array_seek(arraydataptr, 0, arraynullsptr, offset,
// 						elmlen, elmbyval, elmalign);
// 	return ArrayCast(retptr, elmbyval, elmlen);
// }
// Datum
// array_set_element(Datum arraydatum,
// 				  int nSubscripts,
// 				  int *indx,
// 				  Datum dataValue,
// 				  bool isNull,
// 				  int arraytyplen,
// 				  int elmlen,
// 				  bool elmbyval,
// 				  char elmalign)
// {
// 	ArrayType  *array;
// 	ArrayType  *newarray;
// 	int			i,
// 				ndim,
// 				dim[MAXDIM],
// 				lb[MAXDIM],
// 				offset;
// 	char	   *elt_ptr;
// 	bool		newhasnulls;
// 	bits8	   *oldnullbitmap;
// 	int			oldnitems,
// 				newnitems,
// 				olddatasize,
// 				newsize,
// 				olditemlen,
// 				newitemlen,
// 				overheadlen,
// 				oldoverheadlen,
// 				addedbefore,
// 				addedafter,
// 				lenbefore,
// 				lenafter;

// 	if (arraytyplen > 0)
// 	{
// 		/*
// 		 * fixed-length arrays -- these are assumed to be 1-d, 0-based. We
// 		 * cannot extend them, either.
// 		 */
// 		char	   *resultarray;

// 		if (nSubscripts != 1)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 					 errmsg("wrong number of array subscripts")));

// 		if (indx[0] < 0 || indx[0] * elmlen >= arraytyplen)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 					 errmsg("array subscript out of range")));

// 		if (isNull)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
// 					 errmsg("cannot assign null value to an element of a fixed-length array")));

// 		resultarray = (char *) palloc(arraytyplen);
// 		memcpy(resultarray, DatumGetPointer(arraydatum), arraytyplen);
// 		elt_ptr = (char *) resultarray + indx[0] * elmlen;
// 		ArrayCastAndSet(dataValue, elmlen, elmbyval, elmalign, elt_ptr);
// 		return PointerGetDatum(resultarray);
// 	}

// 	if (nSubscripts <= 0 || nSubscripts > MAXDIM)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 				 errmsg("wrong number of array subscripts")));

// 	/* make sure item to be inserted is not toasted */
// 	if (elmlen == -1 && !isNull)
// 		dataValue = PointerGetDatum(PG_DETOAST_DATUM(dataValue));

// 	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(arraydatum)))
// 	{
// 		/* expanded array: let's do this in a separate function */
// 		return array_set_element_expanded(arraydatum,
// 										  nSubscripts,
// 										  indx,
// 										  dataValue,
// 										  isNull,
// 										  arraytyplen,
// 										  elmlen,
// 										  elmbyval,
// 										  elmalign);
// 	}

// 	/* detoast input array if necessary */
// 	array = DatumGetArrayTypeP(arraydatum);

// 	ndim = ARR_NDIM(array);

// 	/*
// 	 * if number of dims is zero, i.e. an empty array, create an array with
// 	 * nSubscripts dimensions, and set the lower bounds to the supplied
// 	 * subscripts
// 	 */
// 	if (ndim == 0)
// 	{
// 		Oid			elmtype = ARR_ELEMTYPE(array);

// 		for (i = 0; i < nSubscripts; i++)
// 		{
// 			dim[i] = 1;
// 			lb[i] = indx[i];
// 		}

// 		return PointerGetDatum(construct_md_array(&dataValue, &isNull,
// 												  nSubscripts, dim, lb,
// 												  elmtype,
// 												elmlen, elmbyval, elmalign));
// 	}

// 	if (ndim != nSubscripts)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 				 errmsg("wrong number of array subscripts")));

// 	/* copy dim/lb since we may modify them */
// 	memcpy(dim, ARR_DIMS(array), ndim * sizeof(int));
// 	memcpy(lb, ARR_LBOUND(array), ndim * sizeof(int));

// 	newhasnulls = (ARR_HASNULL(array) || isNull);
// 	addedbefore = addedafter = 0;

// 	/*
// 	 * Check subscripts
// 	 */
// 	if (ndim == 1)
// 	{
// 		if (indx[0] < lb[0])
// 		{
// 			addedbefore = lb[0] - indx[0];
// 			dim[0] += addedbefore;
// 			lb[0] = indx[0];
// 			if (addedbefore > 1)
// 				newhasnulls = true;		/* will insert nulls */
// 		}
// 		if (indx[0] >= (dim[0] + lb[0]))
// 		{
// 			addedafter = indx[0] - (dim[0] + lb[0]) + 1;
// 			dim[0] += addedafter;
// 			if (addedafter > 1)
// 				newhasnulls = true;		/* will insert nulls */
// 		}
// 	}
// 	else
// 	{
// 		/*
// 		 * XXX currently we do not support extending multi-dimensional arrays
// 		 * during assignment
// 		 */
// 		for (i = 0; i < ndim; i++)
// 		{
// 			if (indx[i] < lb[i] ||
// 				indx[i] >= (dim[i] + lb[i]))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 						 errmsg("array subscript out of range")));
// 		}
// 	}

// 	/*
// 	 * Compute sizes of items and areas to copy
// 	 */
// 	newnitems = ArrayGetNItems(ndim, dim);
// 	if (newhasnulls)
// 		overheadlen = ARR_OVERHEAD_WITHNULLS(ndim, newnitems);
// 	else
// 		overheadlen = ARR_OVERHEAD_NONULLS(ndim);
// 	oldnitems = ArrayGetNItems(ndim, ARR_DIMS(array));
// 	oldnullbitmap = ARR_NULLBITMAP(array);
// 	oldoverheadlen = ARR_DATA_OFFSET(array);
// 	olddatasize = ARR_SIZE(array) - oldoverheadlen;
// 	if (addedbefore)
// 	{
// 		offset = 0;
// 		lenbefore = 0;
// 		olditemlen = 0;
// 		lenafter = olddatasize;
// 	}
// 	else if (addedafter)
// 	{
// 		offset = oldnitems;
// 		lenbefore = olddatasize;
// 		olditemlen = 0;
// 		lenafter = 0;
// 	}
// 	else
// 	{
// 		offset = ArrayGetOffset(nSubscripts, dim, lb, indx);
// 		elt_ptr = array_seek(ARR_DATA_PTR(array), 0, oldnullbitmap, offset,
// 							 elmlen, elmbyval, elmalign);
// 		lenbefore = (int) (elt_ptr - ARR_DATA_PTR(array));
// 		if (array_get_isnull(oldnullbitmap, offset))
// 			olditemlen = 0;
// 		else
// 		{
// 			olditemlen = att_addlength_pointer(0, elmlen, elt_ptr);
// 			olditemlen = att_align_nominal(olditemlen, elmalign);
// 		}
// 		lenafter = (int) (olddatasize - lenbefore - olditemlen);
// 	}

// 	if (isNull)
// 		newitemlen = 0;
// 	else
// 	{
// 		newitemlen = att_addlength_datum(0, elmlen, dataValue);
// 		newitemlen = att_align_nominal(newitemlen, elmalign);
// 	}

// 	newsize = overheadlen + lenbefore + newitemlen + lenafter;

// 	/*
// 	 * OK, create the new array and fill in header/dimensions
// 	 */
// 	newarray = (ArrayType *) palloc0(newsize);
// 	SET_VARSIZE(newarray, newsize);
// 	newarray->ndim = ndim;
// 	newarray->dataoffset = newhasnulls ? overheadlen : 0;
// 	newarray->elemtype = ARR_ELEMTYPE(array);
// 	memcpy(ARR_DIMS(newarray), dim, ndim * sizeof(int));
// 	memcpy(ARR_LBOUND(newarray), lb, ndim * sizeof(int));

// 	/*
// 	 * Fill in data
// 	 */
// 	memcpy((char *) newarray + overheadlen,
// 		   (char *) array + oldoverheadlen,
// 		   lenbefore);
// 	if (!isNull)
// 		ArrayCastAndSet(dataValue, elmlen, elmbyval, elmalign,
// 						(char *) newarray + overheadlen + lenbefore);
// 	memcpy((char *) newarray + overheadlen + lenbefore + newitemlen,
// 		   (char *) array + oldoverheadlen + lenbefore + olditemlen,
// 		   lenafter);

// 	/*
// 	 * Fill in nulls bitmap if needed
// 	 *
// 	 * Note: it's possible we just replaced the last NULL with a non-NULL, and
// 	 * could get rid of the bitmap.  Seems not worth testing for though.
// 	 */
// 	if (newhasnulls)
// 	{
// 		bits8	   *newnullbitmap = ARR_NULLBITMAP(newarray);

// 		/* Zero the bitmap to take care of marking inserted positions null */
// 		MemSet(newnullbitmap, 0, (newnitems + 7) / 8);
// 		/* Fix the inserted value */
// 		if (addedafter)
// 			array_set_isnull(newnullbitmap, newnitems - 1, isNull);
// 		else
// 			array_set_isnull(newnullbitmap, offset, isNull);
// 		/* Fix the copied range(s) */
// 		if (addedbefore)
// 			array_bitmap_copy(newnullbitmap, addedbefore,
// 							  oldnullbitmap, 0,
// 							  oldnitems);
// 		else
// 		{
// 			array_bitmap_copy(newnullbitmap, 0,
// 							  oldnullbitmap, 0,
// 							  offset);
// 			if (addedafter == 0)
// 				array_bitmap_copy(newnullbitmap, offset + 1,
// 								  oldnullbitmap, offset + 1,
// 								  oldnitems - offset - 1);
// 		}
// 	}

// 	return PointerGetDatum(newarray);
// }

// Datum
// array_get_slice(Datum arraydatum,
// 				int nSubscripts,
// 				int *upperIndx,
// 				int *lowerIndx,
// 				bool *upperProvided,
// 				bool *lowerProvided,
// 				int arraytyplen,
// 				int elmlen,
// 				bool elmbyval,
// 				char elmalign)
// {
// 	ArrayType  *array;
// 	ArrayType  *newarray;
// 	int			i,
// 				ndim,
// 			   *dim,
// 			   *lb,
// 			   *newlb;
// 	int			fixedDim[1],
// 				fixedLb[1];
// 	Oid			elemtype;
// 	char	   *arraydataptr;
// 	bits8	   *arraynullsptr;
// 	int32		dataoffset;
// 	int			bytes,
// 				span[MAXDIM];

// 	if (arraytyplen > 0)
// 	{
// 		/*
// 		 * fixed-length arrays -- currently, cannot slice these because parser
// 		 * labels output as being of the fixed-length array type! Code below
// 		 * shows how we could support it if the parser were changed to label
// 		 * output as a suitable varlena array type.
// 		 */
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("slices of fixed-length arrays not implemented")));

// 		/*
// 		 * fixed-length arrays -- these are assumed to be 1-d, 0-based
// 		 *
// 		 * XXX where would we get the correct ELEMTYPE from?
// 		 */
// 		ndim = 1;
// 		fixedDim[0] = arraytyplen / elmlen;
// 		fixedLb[0] = 0;
// 		dim = fixedDim;
// 		lb = fixedLb;
// 		elemtype = InvalidOid;	/* XXX */
// 		arraydataptr = (char *) DatumGetPointer(arraydatum);
// 		arraynullsptr = NULL;
// 	}
// 	else
// 	{
// 		/* detoast input array if necessary */
// 		array = DatumGetArrayTypeP(arraydatum);

// 		ndim = ARR_NDIM(array);
// 		dim = ARR_DIMS(array);
// 		lb = ARR_LBOUND(array);
// 		elemtype = ARR_ELEMTYPE(array);
// 		arraydataptr = ARR_DATA_PTR(array);
// 		arraynullsptr = ARR_NULLBITMAP(array);
// 	}

// 	/*
// 	 * Check provided subscripts.  A slice exceeding the current array limits
// 	 * is silently truncated to the array limits.  If we end up with an empty
// 	 * slice, return an empty array.
// 	 */
// 	if (ndim < nSubscripts || ndim <= 0 || ndim > MAXDIM)
// 		return PointerGetDatum(construct_empty_array(elemtype));

// 	for (i = 0; i < nSubscripts; i++)
// 	{
// 		if (!lowerProvided[i] || lowerIndx[i] < lb[i])
// 			lowerIndx[i] = lb[i];
// 		if (!upperProvided[i] || upperIndx[i] >= (dim[i] + lb[i]))
// 			upperIndx[i] = dim[i] + lb[i] - 1;
// 		if (lowerIndx[i] > upperIndx[i])
// 			return PointerGetDatum(construct_empty_array(elemtype));
// 	}
// 	/* fill any missing subscript positions with full array range */
// 	for (; i < ndim; i++)
// 	{
// 		lowerIndx[i] = lb[i];
// 		upperIndx[i] = dim[i] + lb[i] - 1;
// 		if (lowerIndx[i] > upperIndx[i])
// 			return PointerGetDatum(construct_empty_array(elemtype));
// 	}

// 	mda_get_range(ndim, span, lowerIndx, upperIndx);

// 	bytes = array_slice_size(arraydataptr, arraynullsptr,
// 							 ndim, dim, lb,
// 							 lowerIndx, upperIndx,
// 							 elmlen, elmbyval, elmalign);

// 	/*
// 	 * Currently, we put a null bitmap in the result if the source has one;
// 	 * could be smarter ...
// 	 */
// 	if (arraynullsptr)
// 	{
// 		dataoffset = ARR_OVERHEAD_WITHNULLS(ndim, ArrayGetNItems(ndim, span));
// 		bytes += dataoffset;
// 	}
// 	else
// 	{
// 		dataoffset = 0;			/* marker for no null bitmap */
// 		bytes += ARR_OVERHEAD_NONULLS(ndim);
// 	}

// 	newarray = (ArrayType *) palloc0(bytes);
// 	SET_VARSIZE(newarray, bytes);
// 	newarray->ndim = ndim;
// 	newarray->dataoffset = dataoffset;
// 	newarray->elemtype = elemtype;
// 	memcpy(ARR_DIMS(newarray), span, ndim * sizeof(int));

// 	/*
// 	 * Lower bounds of the new array are set to 1.  Formerly (before 7.3) we
// 	 * copied the given lowerIndx values ... but that seems confusing.
// 	 */
// 	newlb = ARR_LBOUND(newarray);
// 	for (i = 0; i < ndim; i++)
// 		newlb[i] = 1;

// 	array_extract_slice(newarray,
// 						ndim, dim, lb,
// 						arraydataptr, arraynullsptr,
// 						lowerIndx, upperIndx,
// 						elmlen, elmbyval, elmalign);

// 	return PointerGetDatum(newarray);
// }

// Datum
// array_map(FunctionCallInfo fcinfo, Oid retType, ArrayMapState *amstate)
// {
// 	AnyArrayType *v;
// 	ArrayType  *result;
// 	Datum	   *values;
// 	bool	   *nulls;
// 	int		   *dim;
// 	int			ndim;
// 	int			nitems;
// 	int			i;
// 	int32		nbytes = 0;
// 	int32		dataoffset;
// 	bool		hasnulls;
// 	Oid			inpType;
// 	int			inp_typlen;
// 	bool		inp_typbyval;
// 	char		inp_typalign;
// 	int			typlen;
// 	bool		typbyval;
// 	char		typalign;
// 	array_iter	iter;
// 	ArrayMetaState *inp_extra;
// 	ArrayMetaState *ret_extra;

// 	/* Get input array */
// 	if (fcinfo->nargs < 1)
// 		elog(ERROR, "invalid nargs: %d", fcinfo->nargs);
// 	if (PG_ARGISNULL(0))
// 		elog(ERROR, "null input array");
// 	v = PG_GETARG_ANY_ARRAY(0);

// 	inpType = AARR_ELEMTYPE(v);
// 	ndim = AARR_NDIM(v);
// 	dim = AARR_DIMS(v);
// 	nitems = ArrayGetNItems(ndim, dim);

// 	/* Check for empty array */
// 	if (nitems <= 0)
// 	{
// 		/* Return empty array */
// 		PG_RETURN_ARRAYTYPE_P(construct_empty_array(retType));
// 	}

// 	/*
// 	 * We arrange to look up info about input and return element types only
// 	 * once per series of calls, assuming the element type doesn't change
// 	 * underneath us.
// 	 */
// 	inp_extra = &amstate->inp_extra;
// 	ret_extra = &amstate->ret_extra;

// 	if (inp_extra->element_type != inpType)
// 	{
// 		get_typlenbyvalalign(inpType,
// 							 &inp_extra->typlen,
// 							 &inp_extra->typbyval,
// 							 &inp_extra->typalign);
// 		inp_extra->element_type = inpType;
// 	}
// 	inp_typlen = inp_extra->typlen;
// 	inp_typbyval = inp_extra->typbyval;
// 	inp_typalign = inp_extra->typalign;

// 	if (ret_extra->element_type != retType)
// 	{
// 		get_typlenbyvalalign(retType,
// 							 &ret_extra->typlen,
// 							 &ret_extra->typbyval,
// 							 &ret_extra->typalign);
// 		ret_extra->element_type = retType;
// 	}
// 	typlen = ret_extra->typlen;
// 	typbyval = ret_extra->typbyval;
// 	typalign = ret_extra->typalign;

// 	/* Allocate temporary arrays for new values */
// 	values = (Datum *) palloc(nitems * sizeof(Datum));
// 	nulls = (bool *) palloc(nitems * sizeof(bool));

// 	/* Loop over source data */
// 	array_iter_setup(&iter, v);
// 	hasnulls = false;

// 	for (i = 0; i < nitems; i++)
// 	{
// 		bool		callit = true;

// 		/* Get source element, checking for NULL */
// 		fcinfo->arg[0] = array_iter_next(&iter, &fcinfo->argnull[0], i,
// 									 inp_typlen, inp_typbyval, inp_typalign);

// 		/*
// 		 * Apply the given function to source elt and extra args.
// 		 */
// 		if (fcinfo->flinfo->fn_strict)
// 		{
// 			int			j;

// 			for (j = 0; j < fcinfo->nargs; j++)
// 			{
// 				if (fcinfo->argnull[j])
// 				{
// 					callit = false;
// 					break;
// 				}
// 			}
// 		}

// 		if (callit)
// 		{
// 			fcinfo->isnull = false;
// 			values[i] = FunctionCallInvoke(fcinfo);
// 		}
// 		else
// 			fcinfo->isnull = true;

// 		nulls[i] = fcinfo->isnull;
// 		if (fcinfo->isnull)
// 			hasnulls = true;
// 		else
// 		{
// 			/* Ensure data is not toasted */
// 			if (typlen == -1)
// 				values[i] = PointerGetDatum(PG_DETOAST_DATUM(values[i]));
// 			/* Update total result size */
// 			nbytes = att_addlength_datum(nbytes, typlen, values[i]);
// 			nbytes = att_align_nominal(nbytes, typalign);
// 			/* check for overflow of total request */
// 			if (!AllocSizeIsValid(nbytes))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
// 						 errmsg("array size exceeds the maximum allowed (%d)",
// 								(int) MaxAllocSize)));
// 		}
// 	}

// 	/* Allocate and initialize the result array */
// 	if (hasnulls)
// 	{
// 		dataoffset = ARR_OVERHEAD_WITHNULLS(ndim, nitems);
// 		nbytes += dataoffset;
// 	}
// 	else
// 	{
// 		dataoffset = 0;			/* marker for no null bitmap */
// 		nbytes += ARR_OVERHEAD_NONULLS(ndim);
// 	}
// 	result = (ArrayType *) palloc0(nbytes);
// 	SET_VARSIZE(result, nbytes);
// 	result->ndim = ndim;
// 	result->dataoffset = dataoffset;
// 	result->elemtype = retType;
// 	memcpy(ARR_DIMS(result), AARR_DIMS(v), ndim * sizeof(int));
// 	memcpy(ARR_LBOUND(result), AARR_LBOUND(v), ndim * sizeof(int));

// 	/*
// 	 * Note: do not risk trying to pfree the results of the called function
// 	 */
// 	CopyArrayEls(result,
// 				 values, nulls, nitems,
// 				 typlen, typbyval, typalign,
// 				 false);

// 	pfree(values);
// 	pfree(nulls);

// 	PG_RETURN_ARRAYTYPE_P(result);
// }
// ArrayBuildState *
// initArrayResult(Oid element_type, MemoryContext rcontext, bool subcontext)
// {
// 	ArrayBuildState *astate;
// 	MemoryContext arr_context = rcontext;

// 	/* Make a temporary context to hold all the junk */
// 	if (subcontext)
// 		arr_context = AllocSetContextCreate(rcontext,
// 											"accumArrayResult",
// 											ALLOCSET_DEFAULT_SIZES);

// 	astate = (ArrayBuildState *)
// 		MemoryContextAlloc(arr_context, sizeof(ArrayBuildState));
// 	astate->mcontext = arr_context;
// 	astate->private_cxt = subcontext;
// 	astate->alen = (subcontext ? 64 : 8);		/* arbitrary starting array
// 												 * size */
// 	astate->dvalues = (Datum *)
// 		MemoryContextAlloc(arr_context, astate->alen * sizeof(Datum));
// 	astate->dnulls = (bool *)
// 		MemoryContextAlloc(arr_context, astate->alen * sizeof(bool));
// 	astate->nelems = 0;
// 	astate->element_type = element_type;
// 	get_typlenbyvalalign(element_type,
// 						 &astate->typlen,
// 						 &astate->typbyval,
// 						 &astate->typalign);

// 	return astate;
// }
// ArrayBuildStateArr *
// initArrayResultArr(Oid array_type, Oid element_type, MemoryContext rcontext,
// 				   bool subcontext)
// {
// 	ArrayBuildStateArr *astate;
// 	MemoryContext arr_context = rcontext;		/* by default use the parent
// 												 * ctx */

// 	/* Lookup element type, unless element_type already provided */
// 	if (!OidIsValid(element_type))
// 	{
// 		element_type = get_element_type(array_type);

// 		if (!OidIsValid(element_type))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_DATATYPE_MISMATCH),
// 					 errmsg("data type %s is not an array type",
// 							format_type_be(array_type))));
// 	}

// 	/* Make a temporary context to hold all the junk */
// 	if (subcontext)
// 		arr_context = AllocSetContextCreate(rcontext,
// 											"accumArrayResultArr",
// 											ALLOCSET_DEFAULT_SIZES);

// 	/* Note we initialize all fields to zero */
// 	astate = (ArrayBuildStateArr *)
// 		MemoryContextAllocZero(arr_context, sizeof(ArrayBuildStateArr));
// 	astate->mcontext = arr_context;
// 	astate->private_cxt = subcontext;

// 	/* Save relevant datatype information */
// 	astate->array_type = array_type;
// 	astate->element_type = element_type;

// 	return astate;
// }

// ArrayBuildStateArr *
// accumArrayResultArr(ArrayBuildStateArr *astate,
// 					Datum dvalue, bool disnull,
// 					Oid array_type,
// 					MemoryContext rcontext)
// {
// 	ArrayType  *arg;
// 	MemoryContext oldcontext;
// 	int		   *dims,
// 			   *lbs,
// 				ndims,
// 				nitems,
// 				ndatabytes;
// 	char	   *data;
// 	int			i;

// 	/*
// 	 * We disallow accumulating null subarrays.  Another plausible definition
// 	 * is to ignore them, but callers that want that can just skip calling
// 	 * this function.
// 	 */
// 	if (disnull)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
// 				 errmsg("cannot accumulate null arrays")));

// 	/* Detoast input array in caller's context */
// 	arg = DatumGetArrayTypeP(dvalue);

// 	if (astate == NULL)
// 		astate = initArrayResultArr(array_type, InvalidOid, rcontext, true);
// 	else
// 		Assert(astate->array_type == array_type);

// 	oldcontext = MemoryContextSwitchTo(astate->mcontext);

// 	/* Collect this input's dimensions */
// 	ndims = ARR_NDIM(arg);
// 	dims = ARR_DIMS(arg);
// 	lbs = ARR_LBOUND(arg);
// 	data = ARR_DATA_PTR(arg);
// 	nitems = ArrayGetNItems(ndims, dims);
// 	ndatabytes = ARR_SIZE(arg) - ARR_DATA_OFFSET(arg);

// 	if (astate->ndims == 0)
// 	{
// 		/* First input; check/save the dimensionality info */

// 		/* Should we allow empty inputs and just produce an empty output? */
// 		if (ndims == 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 					 errmsg("cannot accumulate empty arrays")));
// 		if (ndims + 1 > MAXDIM)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
// 					 errmsg("number of array dimensions (%d) exceeds the maximum allowed (%d)",
// 							ndims + 1, MAXDIM)));

// 		/*
// 		 * The output array will have n+1 dimensions, with the ones after the
// 		 * first matching the input's dimensions.
// 		 */
// 		astate->ndims = ndims + 1;
// 		astate->dims[0] = 0;
// 		memcpy(&astate->dims[1], dims, ndims * sizeof(int));
// 		astate->lbs[0] = 1;
// 		memcpy(&astate->lbs[1], lbs, ndims * sizeof(int));

// 		/* Allocate at least enough data space for this item */
// 		astate->abytes = 1024;
// 		while (astate->abytes <= ndatabytes)
// 			astate->abytes *= 2;
// 		astate->data = (char *) palloc(astate->abytes);
// 	}
// 	else
// 	{
// 		/* Second or later input: must match first input's dimensionality */
// 		if (astate->ndims != ndims + 1)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 			errmsg("cannot accumulate arrays of different dimensionality")));
// 		for (i = 0; i < ndims; i++)
// 		{
// 			if (astate->dims[i + 1] != dims[i] || astate->lbs[i + 1] != lbs[i])
// 				ereport(ERROR,
// 						(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
// 						 errmsg("cannot accumulate arrays of different dimensionality")));
// 		}

// 		/* Enlarge data space if needed */
// 		if (astate->nbytes + ndatabytes > astate->abytes)
// 		{
// 			astate->abytes = Max(astate->abytes * 2,
// 								 astate->nbytes + ndatabytes);
// 			astate->data = (char *) repalloc(astate->data, astate->abytes);
// 		}
// 	}

// 	/*
// 	 * Copy the data portion of the sub-array.  Note we assume that the
// 	 * advertised data length of the sub-array is properly aligned.  We do not
// 	 * have to worry about detoasting elements since whatever's in the
// 	 * sub-array should be OK already.
// 	 */
// 	memcpy(astate->data + astate->nbytes, data, ndatabytes);
// 	astate->nbytes += ndatabytes;

// 	/* Deal with null bitmap if needed */
// 	if (astate->nullbitmap || ARR_HASNULL(arg))
// 	{
// 		int			newnitems = astate->nitems + nitems;

// 		if (astate->nullbitmap == NULL)
// 		{
// 			/*
// 			 * First input with nulls; we must retrospectively handle any
// 			 * previous inputs by marking all their items non-null.
// 			 */
// 			astate->aitems = 256;
// 			while (astate->aitems <= newnitems)
// 				astate->aitems *= 2;
// 			astate->nullbitmap = (bits8 *) palloc((astate->aitems + 7) / 8);
// 			array_bitmap_copy(astate->nullbitmap, 0,
// 							  NULL, 0,
// 							  astate->nitems);
// 		}
// 		else if (newnitems > astate->aitems)
// 		{
// 			astate->aitems = Max(astate->aitems * 2, newnitems);
// 			astate->nullbitmap = (bits8 *)
// 				repalloc(astate->nullbitmap, (astate->aitems + 7) / 8);
// 		}
// 		array_bitmap_copy(astate->nullbitmap, astate->nitems,
// 						  ARR_NULLBITMAP(arg), 0,
// 						  nitems);
// 	}

// 	astate->nitems += nitems;
// 	astate->dims[0] += 1;

// 	MemoryContextSwitchTo(oldcontext);

// 	/* Release detoasted copy if any */
// 	if ((Pointer) arg != DatumGetPointer(dvalue))
// 		pfree(arg);

// 	return astate;
// }
// Datum
// makeArrayResultArr(ArrayBuildStateArr *astate,
// 				   MemoryContext rcontext,
// 				   bool release)
// {
// 	ArrayType  *result;
// 	MemoryContext oldcontext;

// 	/* Build the final array result in rcontext */
// 	oldcontext = MemoryContextSwitchTo(rcontext);

// 	if (astate->ndims == 0)
// 	{
// 		/* No inputs, return empty array */
// 		result = construct_empty_array(astate->element_type);
// 	}
// 	else
// 	{
// 		int			dataoffset,
// 					nbytes;

// 		/* Compute required space */
// 		nbytes = astate->nbytes;
// 		if (astate->nullbitmap != NULL)
// 		{
// 			dataoffset = ARR_OVERHEAD_WITHNULLS(astate->ndims, astate->nitems);
// 			nbytes += dataoffset;
// 		}
// 		else
// 		{
// 			dataoffset = 0;
// 			nbytes += ARR_OVERHEAD_NONULLS(astate->ndims);
// 		}

// 		result = (ArrayType *) palloc0(nbytes);
// 		SET_VARSIZE(result, nbytes);
// 		result->ndim = astate->ndims;
// 		result->dataoffset = dataoffset;
// 		result->elemtype = astate->element_type;

// 		memcpy(ARR_DIMS(result), astate->dims, astate->ndims * sizeof(int));
// 		memcpy(ARR_LBOUND(result), astate->lbs, astate->ndims * sizeof(int));
// 		memcpy(ARR_DATA_PTR(result), astate->data, astate->nbytes);

// 		if (astate->nullbitmap != NULL)
// 			array_bitmap_copy(ARR_NULLBITMAP(result), 0,
// 							  astate->nullbitmap, 0,
// 							  astate->nitems);
// 	}

// 	MemoryContextSwitchTo(oldcontext);

// 	/* Clean up all the junk */
// 	if (release)
// 	{
// 		Assert(astate->private_cxt);
// 		MemoryContextDelete(astate->mcontext);
// 	}

// 	return PointerGetDatum(result);
// }
// ArrayBuildStateAny *
// initArrayResultAny(Oid input_type, MemoryContext rcontext, bool subcontext)
// {
// 	ArrayBuildStateAny *astate;
// 	Oid			element_type = get_element_type(input_type);

// 	if (OidIsValid(element_type))
// 	{
// 		/* Array case */
// 		ArrayBuildStateArr *arraystate;

// 		arraystate = initArrayResultArr(input_type, InvalidOid, rcontext, subcontext);
// 		astate = (ArrayBuildStateAny *)
// 			MemoryContextAlloc(arraystate->mcontext,
// 							   sizeof(ArrayBuildStateAny));
// 		astate->scalarstate = NULL;
// 		astate->arraystate = arraystate;
// 	}
// 	else
// 	{
// 		/* Scalar case */
// 		ArrayBuildState *scalarstate;

// 		/* Let's just check that we have a type that can be put into arrays */
// 		Assert(OidIsValid(get_array_type(input_type)));

// 		scalarstate = initArrayResult(input_type, rcontext, subcontext);
// 		astate = (ArrayBuildStateAny *)
// 			MemoryContextAlloc(scalarstate->mcontext,
// 							   sizeof(ArrayBuildStateAny));
// 		astate->scalarstate = scalarstate;
// 		astate->arraystate = NULL;
// 	}

// 	return astate;
// }
// ArrayBuildStateAny *
// accumArrayResultAny(ArrayBuildStateAny *astate,
// 					Datum dvalue, bool disnull,
// 					Oid input_type,
// 					MemoryContext rcontext)
// {
// 	if (astate == NULL)
// 		astate = initArrayResultAny(input_type, rcontext, true);

// 	if (astate->scalarstate)
// 		(void) accumArrayResult(astate->scalarstate,
// 								dvalue, disnull,
// 								input_type, rcontext);
// 	else
// 		(void) accumArrayResultArr(astate->arraystate,
// 								   dvalue, disnull,
// 								   input_type, rcontext);

// 	return astate;
// }
// Datum
// makeArrayResultAny(ArrayBuildStateAny *astate,
// 				   MemoryContext rcontext, bool release)
// {
// 	Datum		result;

// 	if (astate->scalarstate)
// 	{
// 		/* Must use makeMdArrayResult to support "release" parameter */
// 		int			ndims;
// 		int			dims[1];
// 		int			lbs[1];

// 		/* If no elements were presented, we want to create an empty array */
// 		ndims = (astate->scalarstate->nelems > 0) ? 1 : 0;
// 		dims[0] = astate->scalarstate->nelems;
// 		lbs[0] = 1;

// 		result = makeMdArrayResult(astate->scalarstate, ndims, dims, lbs,
// 								   rcontext, release);
// 	}
// 	else
// 	{
// 		result = makeArrayResultArr(astate->arraystate,
// 									rcontext, release);
// 	}
// 	return result;
// }
// ArrayIterator
// array_create_iterator(ArrayType *arr, int slice_ndim, ArrayMetaState *mstate)
// {
// 	ArrayIterator iterator = palloc0(sizeof(ArrayIteratorData));

// 	/*
// 	 * Sanity-check inputs --- caller should have got this right already
// 	 */
// 	Assert(PointerIsValid(arr));
// 	if (slice_ndim < 0 || slice_ndim > ARR_NDIM(arr))
// 		elog(ERROR, "invalid arguments to array_create_iterator");

// 	/*
// 	 * Remember basic info about the array and its element type
// 	 */
// 	iterator->arr = arr;
// 	iterator->nullbitmap = ARR_NULLBITMAP(arr);
// 	iterator->nitems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));

// 	if (mstate != NULL)
// 	{
// 		Assert(mstate->element_type == ARR_ELEMTYPE(arr));

// 		iterator->typlen = mstate->typlen;
// 		iterator->typbyval = mstate->typbyval;
// 		iterator->typalign = mstate->typalign;
// 	}
// 	else
// 		get_typlenbyvalalign(ARR_ELEMTYPE(arr),
// 							 &iterator->typlen,
// 							 &iterator->typbyval,
// 							 &iterator->typalign);

// 	/*
// 	 * Remember the slicing parameters.
// 	 */
// 	iterator->slice_ndim = slice_ndim;

// 	if (slice_ndim > 0)
// 	{
// 		/*
// 		 * Get pointers into the array's dims and lbound arrays to represent
// 		 * the dims/lbound arrays of a slice.  These are the same as the
// 		 * rightmost N dimensions of the array.
// 		 */
// 		iterator->slice_dims = ARR_DIMS(arr) + ARR_NDIM(arr) - slice_ndim;
// 		iterator->slice_lbound = ARR_LBOUND(arr) + ARR_NDIM(arr) - slice_ndim;

// 		/*
// 		 * Compute number of elements in a slice.
// 		 */
// 		iterator->slice_len = ArrayGetNItems(slice_ndim,
// 											 iterator->slice_dims);

// 		/*
// 		 * Create workspace for building sub-arrays.
// 		 */
// 		iterator->slice_values = (Datum *)
// 			palloc(iterator->slice_len * sizeof(Datum));
// 		iterator->slice_nulls = (bool *)
// 			palloc(iterator->slice_len * sizeof(bool));
// 	}

// 	/*
// 	 * Initialize our data pointer and linear element number.  These will
// 	 * advance through the array during array_iterate().
// 	 */
// 	iterator->data_ptr = ARR_DATA_PTR(arr);
// 	iterator->current_item = 0;

// 	return iterator;
// }
// Datum
// expand_array(Datum arraydatum, MemoryContext parentcontext,
// 			 ArrayMetaState *metacache)
// {
// 	ArrayType  *array;
// 	ExpandedArrayHeader *eah;
// 	MemoryContext objcxt;
// 	MemoryContext oldcxt;
// 	ArrayMetaState fakecache;

// 	/*
// 	 * Allocate private context for expanded object.  We start by assuming
// 	 * that the array won't be very large; but if it does grow a lot, don't
// 	 * constrain aset.c's large-context behavior.
// 	 */
// 	objcxt = AllocSetContextCreate(parentcontext,
// 								   "expanded array",
// 								   ALLOCSET_START_SMALL_SIZES);

// 	/* Set up expanded array header */
// 	eah = (ExpandedArrayHeader *)
// 		MemoryContextAlloc(objcxt, sizeof(ExpandedArrayHeader));

// 	EOH_init_header(&eah->hdr, &EA_methods, objcxt);
// 	eah->ea_magic = EA_MAGIC;

// 	/* If the source is an expanded array, we may be able to optimize */
// 	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(arraydatum)))
// 	{
// 		ExpandedArrayHeader *oldeah = (ExpandedArrayHeader *) DatumGetEOHP(arraydatum);

// 		Assert(oldeah->ea_magic == EA_MAGIC);

// 		/*
// 		 * Update caller's cache if provided; we don't need it this time, but
// 		 * next call might be for a non-expanded source array.  Furthermore,
// 		 * if the caller didn't provide a cache area, use some local storage
// 		 * to cache anyway, thereby avoiding a catalog lookup in the case
// 		 * where we fall through to the flat-copy code path.
// 		 */
// 		if (metacache == NULL)
// 			metacache = &fakecache;
// 		metacache->element_type = oldeah->element_type;
// 		metacache->typlen = oldeah->typlen;
// 		metacache->typbyval = oldeah->typbyval;
// 		metacache->typalign = oldeah->typalign;

// 		/*
// 		 * If element type is pass-by-value and we have a Datum-array
// 		 * representation, just copy the source's metadata and Datum/isnull
// 		 * arrays.  The original flat array, if present at all, adds no
// 		 * additional information so we need not copy it.
// 		 */
// 		if (oldeah->typbyval && oldeah->dvalues != NULL)
// 		{
// 			copy_byval_expanded_array(eah, oldeah);
// 			/* return a R/W pointer to the expanded array */
// 			return EOHPGetRWDatum(&eah->hdr);
// 		}

// 		/*
// 		 * Otherwise, either we have only a flat representation or the
// 		 * elements are pass-by-reference.  In either case, the best thing
// 		 * seems to be to copy the source as a flat representation and then
// 		 * deconstruct that later if necessary.  For the pass-by-ref case, we
// 		 * could perhaps save some cycles with custom code that generates the
// 		 * deconstructed representation in parallel with copying the values,
// 		 * but it would be a lot of extra code for fairly marginal gain.  So,
// 		 * fall through into the flat-source code path.
// 		 */
// 	}

// 	/*
// 	 * Detoast and copy source array into private context, as a flat array.
// 	 *
// 	 * Note that this coding risks leaking some memory in the private context
// 	 * if we have to fetch data from a TOAST table; however, experimentation
// 	 * says that the leak is minimal.  Doing it this way saves a copy step,
// 	 * which seems worthwhile, especially if the array is large enough to need
// 	 * external storage.
// 	 */
// 	oldcxt = MemoryContextSwitchTo(objcxt);
// 	array = DatumGetArrayTypePCopy(arraydatum);
// 	MemoryContextSwitchTo(oldcxt);

// 	eah->ndims = ARR_NDIM(array);
// 	/* note these pointers point into the fvalue header! */
// 	eah->dims = ARR_DIMS(array);
// 	eah->lbound = ARR_LBOUND(array);

// 	/* Save array's element-type data for possible use later */
// 	eah->element_type = ARR_ELEMTYPE(array);
// 	if (metacache && metacache->element_type == eah->element_type)
// 	{
// 		/* We have a valid cache of representational data */
// 		eah->typlen = metacache->typlen;
// 		eah->typbyval = metacache->typbyval;
// 		eah->typalign = metacache->typalign;
// 	}
// 	else
// 	{
// 		/* No, so look it up */
// 		get_typlenbyvalalign(eah->element_type,
// 							 &eah->typlen,
// 							 &eah->typbyval,
// 							 &eah->typalign);
// 		/* Update cache if provided */
// 		if (metacache)
// 		{
// 			metacache->element_type = eah->element_type;
// 			metacache->typlen = eah->typlen;
// 			metacache->typbyval = eah->typbyval;
// 			metacache->typalign = eah->typalign;
// 		}
// 	}

// 	/* we don't make a deconstructed representation now */
// 	eah->dvalues = NULL;
// 	eah->dnulls = NULL;
// 	eah->dvalueslen = 0;
// 	eah->nelems = 0;
// 	eah->flat_size = 0;

// 	/* remember we have a flat representation */
// 	eah->fvalue = array;
// 	eah->fstartptr = ARR_DATA_PTR(array);
// 	eah->fendptr = ((char *) array) + ARR_SIZE(array);

// 	/* return a R/W pointer to the expanded array */
// 	return EOHPGetRWDatum(&eah->hdr);
// }
// ExpandedArrayHeader *
// DatumGetExpandedArrayX(Datum d, ArrayMetaState *metacache)
// {
// 	/* If it's a writable expanded array already, just return it */
// 	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(d)))
// 	{
// 		ExpandedArrayHeader *eah = (ExpandedArrayHeader *) DatumGetEOHP(d);

// 		Assert(eah->ea_magic == EA_MAGIC);
// 		/* Update cache if provided */
// 		if (metacache)
// 		{
// 			metacache->element_type = eah->element_type;
// 			metacache->typlen = eah->typlen;
// 			metacache->typbyval = eah->typbyval;
// 			metacache->typalign = eah->typalign;
// 		}
// 		return eah;
// 	}

// 	/* Else expand using caller's cache if any */
// 	d = expand_array(d, CurrentMemoryContext, metacache);
// 	return (ExpandedArrayHeader *) DatumGetEOHP(d);
// }
// ExpandedArrayHeader *
// DatumGetExpandedArray(Datum d)
// {
// 	/* If it's a writable expanded array already, just return it */
// 	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(d)))
// 	{
// 		ExpandedArrayHeader *eah = (ExpandedArrayHeader *) DatumGetEOHP(d);

// 		Assert(eah->ea_magic == EA_MAGIC);
// 		return eah;
// 	}

// 	/* Else expand the hard way */
// 	d = expand_array(d, CurrentMemoryContext, NULL);
// 	return (ExpandedArrayHeader *) DatumGetEOHP(d);
// }
// AnyArrayType *
// DatumGetAnyArray(Datum d)
// {
// 	ExpandedArrayHeader *eah;

// 	/*
// 	 * If it's an expanded array (RW or RO), return the header pointer.
// 	 */
// 	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d)))
// 	{
// 		eah = (ExpandedArrayHeader *) DatumGetEOHP(d);
// 		Assert(eah->ea_magic == EA_MAGIC);
// 		return (AnyArrayType *) eah;
// 	}

// 	/* Else do regular detoasting as needed */
// 	return (AnyArrayType *) PG_DETOAST_DATUM(d);
// }
// void
// deconstruct_expanded_array(ExpandedArrayHeader *eah)
// {
// 	if (eah->dvalues == NULL)
// 	{
// 		MemoryContext oldcxt = MemoryContextSwitchTo(eah->hdr.eoh_context);
// 		Datum	   *dvalues;
// 		bool	   *dnulls;
// 		int			nelems;

// 		dnulls = NULL;
// 		deconstruct_array(eah->fvalue,
// 						  eah->element_type,
// 						  eah->typlen, eah->typbyval, eah->typalign,
// 						  &dvalues,
// 						  ARR_HASNULL(eah->fvalue) ? &dnulls : NULL,
// 						  &nelems);

// 		/*
// 		 * Update header only after successful completion of this step.  If
// 		 * deconstruct_array fails partway through, worst consequence is some
// 		 * leaked memory in the object's context.  If the caller fails at a
// 		 * later point, that's fine, since the deconstructed representation is
// 		 * valid anyhow.
// 		 */
// 		eah->dvalues = dvalues;
// 		eah->dnulls = dnulls;
// 		eah->dvalueslen = eah->nelems = nelems;
// 		MemoryContextSwitchTo(oldcxt);
// 	}
// }
// Datum
// array_append(PG_FUNCTION_ARGS)
// {
// 	ExpandedArrayHeader *eah;
// 	Datum		newelem;
// 	bool		isNull;
// 	Datum		result;
// 	int		   *dimv,
// 			   *lb;
// 	int			indx;
// 	ArrayMetaState *my_extra;

// 	eah = fetch_array_arg_replace_nulls(fcinfo, 0);
// 	isNull = PG_ARGISNULL(1);
// 	if (isNull)
// 		newelem = (Datum) 0;
// 	else
// 		newelem = PG_GETARG_DATUM(1);

// 	if (eah->ndims == 1)
// 	{
// 		/* append newelem */
// 		int			ub;

// 		lb = eah->lbound;
// 		dimv = eah->dims;
// 		ub = dimv[0] + lb[0] - 1;
// 		indx = ub + 1;

// 		/* overflow? */
// 		if (indx < ub)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
// 					 errmsg("integer out of range")));
// 	}
// 	else if (eah->ndims == 0)
// 		indx = 1;
// 	else
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATA_EXCEPTION),
// 				 errmsg("argument must be empty or one-dimensional array")));

// 	/* Perform element insertion */
// 	my_extra = (ArrayMetaState *) fcinfo->flinfo->fn_extra;

// 	result = array_set_element(EOHPGetRWDatum(&eah->hdr),
// 							   1, &indx, newelem, isNull,
// 			   -1, my_extra->typlen, my_extra->typbyval, my_extra->typalign);

// 	PG_RETURN_DATUM(result);
// }
// Datum
// array_prepend(PG_FUNCTION_ARGS)
// {
// 	ExpandedArrayHeader *eah;
// 	Datum		newelem;
// 	bool		isNull;
// 	Datum		result;
// 	int		   *lb;
// 	int			indx;
// 	int			lb0;
// 	ArrayMetaState *my_extra;

// 	isNull = PG_ARGISNULL(0);
// 	if (isNull)
// 		newelem = (Datum) 0;
// 	else
// 		newelem = PG_GETARG_DATUM(0);
// 	eah = fetch_array_arg_replace_nulls(fcinfo, 1);

// 	if (eah->ndims == 1)
// 	{
// 		/* prepend newelem */
// 		lb = eah->lbound;
// 		indx = lb[0] - 1;
// 		lb0 = lb[0];

// 		/* overflow? */
// 		if (indx > lb[0])
// 			ereport(ERROR,
// 					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
// 					 errmsg("integer out of range")));
// 	}
// 	else if (eah->ndims == 0)
// 	{
// 		indx = 1;
// 		lb0 = 1;
// 	}
// 	else
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATA_EXCEPTION),
// 				 errmsg("argument must be empty or one-dimensional array")));

// 	/* Perform element insertion */
// 	my_extra = (ArrayMetaState *) fcinfo->flinfo->fn_extra;

// 	result = array_set_element(EOHPGetRWDatum(&eah->hdr),
// 							   1, &indx, newelem, isNull,
// 			   -1, my_extra->typlen, my_extra->typbyval, my_extra->typalign);

// 	/* Readjust result's LB to match the input's, as expected for prepend */
// 	Assert(result == EOHPGetRWDatum(&eah->hdr));
// 	if (eah->ndims == 1)
// 	{
// 		/* This is ok whether we've deconstructed or not */
// 		eah->lbound[0] = lb0;
// 	}

// 	PG_RETURN_DATUM(result);
// }
// Datum
// array_agg_array_transfn(PG_FUNCTION_ARGS)
// {
// 	Oid			arg1_typeid = get_fn_expr_argtype(fcinfo->flinfo, 1);
// 	MemoryContext aggcontext;
// 	ArrayBuildStateArr *state;

// 	if (arg1_typeid == InvalidOid)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("could not determine input data type")));

// 	/*
// 	 * Note: we do not need a run-time check about whether arg1_typeid is a
// 	 * valid array type, because the parser would have verified that while
// 	 * resolving the input/result types of this polymorphic aggregate.
// 	 */

// 	if (!AggCheckCallContext(fcinfo, &aggcontext))
// 	{
// 		/* cannot be called directly because of internal-type argument */
// 		elog(ERROR, "array_agg_array_transfn called in non-aggregate context");
// 	}


// 	if (PG_ARGISNULL(0))
// 		state = initArrayResultArr(arg1_typeid, InvalidOid, aggcontext, false);
// 	else
// 		state = (ArrayBuildStateArr *) PG_GETARG_POINTER(0);

// 	state = accumArrayResultArr(state,
// 								PG_GETARG_DATUM(1),
// 								PG_ARGISNULL(1),
// 								arg1_typeid,
// 								aggcontext);

// 	/*
// 	 * The transition type for array_agg() is declared to be "internal", which
// 	 * is a pass-by-value type the same size as a pointer.  So we can safely
// 	 * pass the ArrayBuildStateArr pointer through nodeAgg.c's machinations.
// 	 */
// 	PG_RETURN_POINTER(state);
// }
// Datum
// array_agg_array_finalfn(PG_FUNCTION_ARGS)
// {
// 	Datum		result;
// 	ArrayBuildStateArr *state;

// 	/* cannot be called directly because of internal-type argument */
// 	Assert(AggCheckCallContext(fcinfo, NULL));

// 	state = PG_ARGISNULL(0) ? NULL : (ArrayBuildStateArr *) PG_GETARG_POINTER(0);

// 	if (state == NULL)
// 		PG_RETURN_NULL();		/* returns null iff no input values */

// 	/*
// 	 * Make the result.  We cannot release the ArrayBuildStateArr because
// 	 * sometimes aggregate final functions are re-executed.  Rather, it is
// 	 * nodeAgg.c's responsibility to reset the aggcontext when it's safe to do
// 	 * so.
// 	 */
// 	result = makeArrayResultArr(state, CurrentMemoryContext, false);

// 	PG_RETURN_DATUM(result);
// }
// Datum
// array_position(PG_FUNCTION_ARGS)
// {
// 	return array_position_common(fcinfo);
// }

// Datum
// array_position_start(PG_FUNCTION_ARGS)
// {
// 	return array_position_common(fcinfo);
// }

// Datum
// array_positions(PG_FUNCTION_ARGS)
// {
// 	ArrayType  *array;
// 	Oid			collation = PG_GET_COLLATION();
// 	Oid			element_type;
// 	Datum		searched_element,
// 				value;
// 	bool		isnull;
// 	int			position;
// 	TypeCacheEntry *typentry;
// 	ArrayMetaState *my_extra;
// 	bool		null_search;
// 	ArrayIterator array_iterator;
// 	ArrayBuildState *astate = NULL;

// 	if (PG_ARGISNULL(0))
// 		PG_RETURN_NULL();

// 	array = PG_GETARG_ARRAYTYPE_P(0);
// 	element_type = ARR_ELEMTYPE(array);

// 	position = (ARR_LBOUND(array))[0] - 1;

// 	/*
// 	 * We refuse to search for elements in multi-dimensional arrays, since we
// 	 * have no good way to report the element's location in the array.
// 	 */
// 	if (ARR_NDIM(array) > 1)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("searching for elements in multidimensional arrays is not supported")));

// 	astate = initArrayResult(INT4OID, CurrentMemoryContext, false);

// 	if (PG_ARGISNULL(1))
// 	{
// 		/* fast return when the array doesn't have nulls */
// 		if (!array_contains_nulls(array))
// 			PG_RETURN_DATUM(makeArrayResult(astate, CurrentMemoryContext));
// 		searched_element = (Datum) 0;
// 		null_search = true;
// 	}
// 	else
// 	{
// 		searched_element = PG_GETARG_DATUM(1);
// 		null_search = false;
// 	}

// 	/*
// 	 * We arrange to look up type info for array_create_iterator only once per
// 	 * series of calls, assuming the element type doesn't change underneath
// 	 * us.
// 	 */
// 	my_extra = (ArrayMetaState *) fcinfo->flinfo->fn_extra;
// 	if (my_extra == NULL)
// 	{
// 		fcinfo->flinfo->fn_extra = MemoryContextAlloc(fcinfo->flinfo->fn_mcxt,
// 													  sizeof(ArrayMetaState));
// 		my_extra = (ArrayMetaState *) fcinfo->flinfo->fn_extra;
// 		my_extra->element_type = ~element_type;
// 	}

// 	if (my_extra->element_type != element_type)
// 	{
// 		get_typlenbyvalalign(element_type,
// 							 &my_extra->typlen,
// 							 &my_extra->typbyval,
// 							 &my_extra->typalign);

// 		typentry = lookup_type_cache(element_type, TYPECACHE_EQ_OPR_FINFO);

// 		if (!OidIsValid(typentry->eq_opr_finfo.fn_oid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 				errmsg("could not identify an equality operator for type %s",
// 					   format_type_be(element_type))));

// 		my_extra->element_type = element_type;
// 		fmgr_info_cxt(typentry->eq_opr_finfo.fn_oid, &my_extra->proc,
// 					  fcinfo->flinfo->fn_mcxt);
// 	}

// 	/*
// 	 * Accumulate each array position iff the element matches the given
// 	 * element.
// 	 */
// 	array_iterator = array_create_iterator(array, 0, my_extra);
// 	while (array_iterate(array_iterator, &value, &isnull))
// 	{
// 		position += 1;

// 		/*
// 		 * Can't look at the array element's value if it's null; but if we
// 		 * search for null, we have a hit.
// 		 */
// 		if (isnull || null_search)
// 		{
// 			if (isnull && null_search)
// 				astate =
// 					accumArrayResult(astate, Int32GetDatum(position), false,
// 									 INT4OID, CurrentMemoryContext);

// 			continue;
// 		}

// 		/* not nulls, so run the operator */
// 		if (DatumGetBool(FunctionCall2Coll(&my_extra->proc, collation,
// 										   searched_element, value)))
// 			astate =
// 				accumArrayResult(astate, Int32GetDatum(position), false,
// 								 INT4OID, CurrentMemoryContext);
// 	}

// 	array_free_iterator(array_iterator);

// 	/* Avoid leaking memory when handed toasted input */
// 	PG_FREE_IF_COPY(array, 0);

// 	PG_RETURN_DATUM(makeArrayResult(astate, CurrentMemoryContext));
// }
// Datum
// datumTransfer(Datum value, bool typByVal, int typLen)
// {
// 	if (!typByVal && typLen == -1 &&
// 		VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(value)))
// 		value = TransferExpandedObject(value, CurrentMemoryContext);
// 	else
// 		value = datumCopy(value, typByVal, typLen);
// 	return value;
// }

// Size
// datumEstimateSpace(Datum value, bool isnull, bool typByVal, int typLen)
// {
// 	Size		sz = sizeof(int);

// 	if (!isnull)
// 	{
// 		/* no need to use add_size, can't overflow */
// 		if (typByVal)
// 			sz += sizeof(Datum);
// 		else if (VARATT_IS_EXTERNAL_EXPANDED(value))
// 		{
// 			ExpandedObjectHeader *eoh = DatumGetEOHP(value);

// 			sz += EOH_get_flat_size(eoh);
// 		}
// 		else
// 			sz += datumGetSize(value, typByVal, typLen);
// 	}

// 	return sz;
// }
// void
// datumSerialize(Datum value, bool isnull, bool typByVal, int typLen,
// 			   char **start_address)
// {
// 	ExpandedObjectHeader *eoh = NULL;
// 	int			header;

// 	/* Write header word. */
// 	if (isnull)
// 		header = -2;
// 	else if (typByVal)
// 		header = -1;
// 	else if (VARATT_IS_EXTERNAL_EXPANDED(value))
// 	{
// 		eoh = DatumGetEOHP(value);
// 		header = EOH_get_flat_size(eoh);
// 	}
// 	else
// 		header = datumGetSize(value, typByVal, typLen);
// 	memcpy(*start_address, &header, sizeof(int));
// 	*start_address += sizeof(int);

// 	/* If not null, write payload bytes. */
// 	if (!isnull)
// 	{
// 		if (typByVal)
// 		{
// 			memcpy(*start_address, &value, sizeof(Datum));
// 			*start_address += sizeof(Datum);
// 		}
// 		else if (eoh)
// 		{
// 			EOH_flatten_into(eoh, (void *) *start_address, header);
// 			*start_address += header;
// 		}
// 		else
// 		{
// 			memcpy(*start_address, DatumGetPointer(value), header);
// 			*start_address += header;
// 		}
// 	}
// }
// Datum
// datumRestore(char **start_address, bool *isnull)
// {
// 	int			header;
// 	void	   *d;

// 	/* Read header word. */
// 	memcpy(&header, *start_address, sizeof(int));
// 	*start_address += sizeof(int);

// 	/* If this datum is NULL, we can stop here. */
// 	if (header == -2)
// 	{
// 		*isnull = true;
// 		return (Datum) 0;
// 	}

// 	/* OK, datum is not null. */
// 	*isnull = false;

// 	/* If this datum is pass-by-value, sizeof(Datum) bytes follow. */
// 	if (header == -1)
// 	{
// 		Datum		val;

// 		memcpy(&val, *start_address, sizeof(Datum));
// 		*start_address += sizeof(Datum);
// 		return val;
// 	}

// 	/* Pass-by-reference case; copy indicated number of bytes. */
// 	Assert(header > 0);
// 	d = palloc(header);
// 	memcpy(d, *start_address, header);
// 	*start_address += header;
// 	return PointerGetDatum(d);
// }

// bool
// ParseConfigFile(const char *config_file, bool strict,
// 				const char *calling_file, int calling_lineno,
// 				int depth, int elevel,
// 				ConfigVariable **head_p,
// 				ConfigVariable **tail_p)
// {
// 	char	   *abs_path;
// 	bool		OK = true;
// 	FILE	   *fp;

// 	/*
// 	 * Reject too-deep include nesting depth.  This is just a safety check to
// 	 * avoid dumping core due to stack overflow if an include file loops back
// 	 * to itself.  The maximum nesting depth is pretty arbitrary.
// 	 */
// 	if (depth > 10)
// 	{
// 		ereport(elevel,
// 				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
// 				 errmsg("could not open configuration file \"%s\": maximum nesting depth exceeded",
// 						config_file)));
// 		record_config_file_error("nesting depth exceeded",
// 								 calling_file, calling_lineno,
// 								 head_p, tail_p);
// 		return false;
// 	}

// 	abs_path = AbsoluteConfigLocation(config_file, calling_file);
// 	fp = AllocateFile(abs_path, "r");
// 	if (!fp)
// 	{
// 		if (strict)
// 		{
// 			ereport(elevel,
// 					(errcode_for_file_access(),
// 					 errmsg("could not open configuration file \"%s\": %m",
// 							abs_path)));
// 			record_config_file_error(psprintf("could not open file \"%s\"",
// 											  abs_path),
// 									 calling_file, calling_lineno,
// 									 head_p, tail_p);
// 			OK = false;
// 		}
// 		else
// 		{
// 			ereport(LOG,
// 					(errmsg("skipping missing configuration file \"%s\"",
// 							abs_path)));
// 		}
// 		goto cleanup;
// 	}

// 	OK = ParseConfigFp(fp, abs_path, depth, elevel, head_p, tail_p);

// cleanup:
// 	if (fp)
// 		FreeFile(fp);
// 	pfree(abs_path);

// 	return OK;
// }

// bool
// ParseConfigDirectory(const char *includedir,
// 					 const char *calling_file, int calling_lineno,
// 					 int depth, int elevel,
// 					 ConfigVariable **head_p,
// 					 ConfigVariable **tail_p)
// {
// 	char	   *directory;
// 	DIR		   *d;
// 	struct dirent *de;
// 	char	  **filenames;
// 	int			num_filenames;
// 	int			size_filenames;
// 	bool		status;

// 	directory = AbsoluteConfigLocation(includedir, calling_file);
// 	d = AllocateDir(directory);
// 	if (d == NULL)
// 	{
// 		ereport(elevel,
// 				(errcode_for_file_access(),
// 				 errmsg("could not open configuration directory \"%s\": %m",
// 						directory)));
// 		record_config_file_error(psprintf("could not open directory \"%s\"",
// 										  directory),
// 								 calling_file, calling_lineno,
// 								 head_p, tail_p);
// 		status = false;
// 		goto cleanup;
// 	}

// 	/*
// 	 * Read the directory and put the filenames in an array, so we can sort
// 	 * them prior to processing the contents.
// 	 */
// 	size_filenames = 32;
// 	filenames = (char **) palloc(size_filenames * sizeof(char *));
// 	num_filenames = 0;

// 	while ((de = ReadDir(d, directory)) != NULL)
// 	{
// 		struct stat st;
// 		char		filename[MAXPGPATH];

// 		/*
// 		 * Only parse files with names ending in ".conf".  Explicitly reject
// 		 * files starting with ".".  This excludes things like "." and "..",
// 		 * as well as typical hidden files, backup files, and editor debris.
// 		 */
// 		if (strlen(de->d_name) < 6)
// 			continue;
// 		if (de->d_name[0] == '.')
// 			continue;
// 		if (strcmp(de->d_name + strlen(de->d_name) - 5, ".conf") != 0)
// 			continue;

// 		join_path_components(filename, directory, de->d_name);
// 		canonicalize_path(filename);
// 		if (stat(filename, &st) == 0)
// 		{
// 			if (!S_ISDIR(st.st_mode))
// 			{
// 				/* Add file to array, increasing its size in blocks of 32 */
// 				if (num_filenames >= size_filenames)
// 				{
// 					size_filenames += 32;
// 					filenames = (char **) repalloc(filenames,
// 											size_filenames * sizeof(char *));
// 				}
// 				filenames[num_filenames] = pstrdup(filename);
// 				num_filenames++;
// 			}
// 		}
// 		else
// 		{
// 			/*
// 			 * stat does not care about permissions, so the most likely reason
// 			 * a file can't be accessed now is if it was removed between the
// 			 * directory listing and now.
// 			 */
// 			ereport(elevel,
// 					(errcode_for_file_access(),
// 					 errmsg("could not stat file \"%s\": %m",
// 							filename)));
// 			record_config_file_error(psprintf("could not stat file \"%s\"",
// 											  filename),
// 									 calling_file, calling_lineno,
// 									 head_p, tail_p);
// 			status = false;
// 			goto cleanup;
// 		}
// 	}

// 	if (num_filenames > 0)
// 	{
// 		int			i;

// 		qsort(filenames, num_filenames, sizeof(char *), pg_qsort_strcmp);
// 		for (i = 0; i < num_filenames; i++)
// 		{
// 			if (!ParseConfigFile(filenames[i], true,
// 								 calling_file, calling_lineno,
// 								 depth, elevel,
// 								 head_p, tail_p))
// 			{
// 				status = false;
// 				goto cleanup;
// 			}
// 		}
// 	}
// 	status = true;

// cleanup:
// 	if (d)
// 		FreeDir(d);
// 	pfree(directory);
// 	return status;
// }

// char *
// GetConfigOptionByName(const char *name, const char **varname, bool missing_ok)
// {
// 	struct config_generic *record;

// 	record = find_option(name, false, ERROR);
// 	if (record == NULL)
// 	{
// 		if (missing_ok)
// 		{
// 			if (varname)
// 				*varname = NULL;
// 			return NULL;
// 		}

// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 			   errmsg("unrecognized configuration parameter \"%s\"", name)));
// 	}

// 	if ((record->flags & GUC_SUPERUSER_ONLY) && !superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to examine \"%s\"", name)));

// 	if (varname)
// 		*varname = record->name;

// 	return _ShowOption(record, true);
// }

// Size
// EstimateGUCStateSpace(void)
// {
// 	Size		size;
// 	int			i;

// 	/* Add space reqd for saving the data size of the guc state */
// 	size = sizeof(Size);

// 	/* Add up the space needed for each GUC variable */
// 	for (i = 0; i < num_guc_variables; i++)
// 		size = add_size(size,
// 						estimate_variable_size(guc_variables[i]));

// 	return size;
// }

// void
// SerializeGUCState(Size maxsize, char *start_address)
// {
// 	char	   *curptr;
// 	Size		actual_size;
// 	Size		bytes_left;
// 	int			i;
// 	int			i_role = -1;

// 	/* Reserve space for saving the actual size of the guc state */
// 	Assert(maxsize > sizeof(actual_size));
// 	curptr = start_address + sizeof(actual_size);
// 	bytes_left = maxsize - sizeof(actual_size);

// 	for (i = 0; i < num_guc_variables; i++)
// 	{
// 		/*
// 		 * It's pretty ugly, but we've got to force "role" to be initialized
// 		 * after "session_authorization"; otherwise, the latter will override
// 		 * the former.
// 		 */
// 		if (strcmp(guc_variables[i]->name, "role") == 0)
// 			i_role = i;
// 		else
// 			serialize_variable(&curptr, &bytes_left, guc_variables[i]);
// 	}
// 	if (i_role >= 0)
// 		serialize_variable(&curptr, &bytes_left, guc_variables[i_role]);

// 	/* Store actual size without assuming alignment of start_address. */
// 	actual_size = maxsize - bytes_left - sizeof(actual_size);
// 	memcpy(start_address, &actual_size, sizeof(actual_size));
// }

// void
// RestoreGUCState(void *gucstate)
// {
// 	char	   *varname,
// 			   *varvalue,
// 			   *varsourcefile;
// 	int			varsourceline;
// 	GucSource	varsource;
// 	GucContext	varscontext;
// 	char	   *srcptr = (char *) gucstate;
// 	char	   *srcend;
// 	Size		len;
// 	int			i;

// 	/* See comment at can_skip_gucvar(). */
// 	for (i = 0; i < num_guc_variables; i++)
// 		if (!can_skip_gucvar(guc_variables[i]))
// 			InitializeOneGUCOption(guc_variables[i]);

// 	/* First item is the length of the subsequent data */
// 	memcpy(&len, gucstate, sizeof(len));

// 	srcptr += sizeof(len);
// 	srcend = srcptr + len;

// 	while (srcptr < srcend)
// 	{
// 		int			result;

// 		varname = read_gucstate(&srcptr, srcend);
// 		varvalue = read_gucstate(&srcptr, srcend);
// 		varsourcefile = read_gucstate(&srcptr, srcend);
// 		if (varsourcefile[0])
// 			read_gucstate_binary(&srcptr, srcend,
// 								 &varsourceline, sizeof(varsourceline));
// 		read_gucstate_binary(&srcptr, srcend,
// 							 &varsource, sizeof(varsource));
// 		read_gucstate_binary(&srcptr, srcend,
// 							 &varscontext, sizeof(varscontext));

// 		result = set_config_option(varname, varvalue, varscontext, varsource,
// 								   GUC_ACTION_SET, true, ERROR, true);
// 		if (result <= 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INTERNAL_ERROR),
// 					 errmsg("parameter \"%s\" could not be set", varname)));
// 		if (varsourcefile[0])
// 			set_config_sourcefile(varname, varsourcefile, varsourceline);
// 	}
// }

// bool
// hash_update_hash_key(HTAB *hashp,
// 					 void *existingEntry,
// 					 const void *newKeyPtr)
// {
// 	HASHELEMENT *existingElement = ELEMENT_FROM_KEY(existingEntry);
// 	HASHHDR    *hctl = hashp->hctl;
// 	uint32		newhashvalue;
// 	Size		keysize;
// 	uint32		bucket;
// 	uint32		newbucket;
// 	long		segment_num;
// 	long		segment_ndx;
// 	HASHSEGMENT segp;
// 	HASHBUCKET	currBucket;
// 	HASHBUCKET *prevBucketPtr;
// 	HASHBUCKET *oldPrevPtr;
// 	HashCompareFunc match;

// #if HASH_STATISTICS
// 	hash_accesses++;
// 	hctl->accesses++;
// #endif

// 	/* disallow updates if frozen */
// 	if (hashp->frozen)
// 		elog(ERROR, "cannot update in frozen hashtable \"%s\"",
// 			 hashp->tabname);

// 	/*
// 	 * Lookup the existing element using its saved hash value.  We need to do
// 	 * this to be able to unlink it from its hash chain, but as a side benefit
// 	 * we can verify the validity of the passed existingEntry pointer.
// 	 */
// 	bucket = calc_bucket(hctl, existingElement->hashvalue);

// 	segment_num = bucket >> hashp->sshift;
// 	segment_ndx = MOD(bucket, hashp->ssize);

// 	segp = hashp->dir[segment_num];

// 	if (segp == NULL)
// 		hash_corrupted(hashp);

// 	prevBucketPtr = &segp[segment_ndx];
// 	currBucket = *prevBucketPtr;

// 	while (currBucket != NULL)
// 	{
// 		if (currBucket == existingElement)
// 			break;
// 		prevBucketPtr = &(currBucket->link);
// 		currBucket = *prevBucketPtr;
// 	}

// 	if (currBucket == NULL)
// 		elog(ERROR, "hash_update_hash_key argument is not in hashtable \"%s\"",
// 			 hashp->tabname);

// 	oldPrevPtr = prevBucketPtr;

// 	/*
// 	 * Now perform the equivalent of a HASH_ENTER operation to locate the hash
// 	 * chain we want to put the entry into.
// 	 */
// 	newhashvalue = hashp->hash(newKeyPtr, hashp->keysize);

// 	newbucket = calc_bucket(hctl, newhashvalue);

// 	segment_num = newbucket >> hashp->sshift;
// 	segment_ndx = MOD(newbucket, hashp->ssize);

// 	segp = hashp->dir[segment_num];

// 	if (segp == NULL)
// 		hash_corrupted(hashp);

// 	prevBucketPtr = &segp[segment_ndx];
// 	currBucket = *prevBucketPtr;

// 	/*
// 	 * Follow collision chain looking for matching key
// 	 */
// 	match = hashp->match;		/* save one fetch in inner loop */
// 	keysize = hashp->keysize;	/* ditto */

// 	while (currBucket != NULL)
// 	{
// 		if (currBucket->hashvalue == newhashvalue &&
// 			match(ELEMENTKEY(currBucket), newKeyPtr, keysize) == 0)
// 			break;
// 		prevBucketPtr = &(currBucket->link);
// 		currBucket = *prevBucketPtr;
// #if HASH_STATISTICS
// 		hash_collisions++;
// 		hctl->collisions++;
// #endif
// 	}

// 	if (currBucket != NULL)
// 		return false;			/* collision with an existing entry */

// 	currBucket = existingElement;

// 	/*
// 	 * If old and new hash values belong to the same bucket, we need not
// 	 * change any chain links, and indeed should not since this simplistic
// 	 * update will corrupt the list if currBucket is the last element.  (We
// 	 * cannot fall out earlier, however, since we need to scan the bucket to
// 	 * check for duplicate keys.)
// 	 */
// 	if (bucket != newbucket)
// 	{
// 		/* OK to remove record from old hash bucket's chain. */
// 		*oldPrevPtr = currBucket->link;

// 		/* link into new hashbucket chain */
// 		*prevBucketPtr = currBucket;
// 		currBucket->link = NULL;
// 	}

// 	/* copy new key into record */
// 	currBucket->hashvalue = newhashvalue;
// 	hashp->keycopy(ELEMENTKEY(currBucket), newKeyPtr, keysize);

// 	/* rest of record is untouched */

// 	return true;
// }

// void
// CacheRegisterSyscacheCallback(int cacheid,
// 							  SyscacheCallbackFunction func,
// 							  Datum arg)
// {
// 	if (syscache_callback_count >= MAX_SYSCACHE_CALLBACKS)
// 		elog(FATAL, "out of syscache_callback_list slots");

// 	syscache_callback_list[syscache_callback_count].id = cacheid;
// 	syscache_callback_list[syscache_callback_count].function = func;
// 	syscache_callback_list[syscache_callback_count].arg = arg;

// 	++syscache_callback_count;
// }

// void
// CacheRegisterRelcacheCallback(RelcacheCallbackFunction func,
// 							  Datum arg)
// {
// 	if (relcache_callback_count >= MAX_RELCACHE_CALLBACKS)
// 		elog(FATAL, "out of relcache_callback_list slots");

// 	relcache_callback_list[relcache_callback_count].function = func;
// 	relcache_callback_list[relcache_callback_count].arg = arg;

// 	++relcache_callback_count;
// }

// void
// CallSyscacheCallbacks(int cacheid, uint32 hashvalue)
// {
// 	int			i;

// 	for (i = 0; i < syscache_callback_count; i++)
// 	{
// 		struct SYSCACHECALLBACK *ccitem = syscache_callback_list + i;

// 		if (ccitem->id == cacheid)
// 			(*ccitem->function) (ccitem->arg, cacheid, hashvalue);
// 	}
// }

// char *
// get_language_name(Oid langoid, bool missing_ok)
// {
// 	HeapTuple	tp;

// 	tp = SearchSysCache1(LANGOID, ObjectIdGetDatum(langoid));
// 	if (HeapTupleIsValid(tp))
// 	{
// 		Form_pg_language lantup = (Form_pg_language) GETSTRUCT(tp);
// 		char	   *result;

// 		result = pstrdup(NameStr(lantup->lanname));
// 		ReleaseSysCache(tp);
// 		return result;
// 	}

// 	if (!missing_ok)
// 		elog(ERROR, "cache lookup failed for language %u",
// 			 langoid);
// 	return NULL;
// }

// Oid
// get_op_rettype(Oid opno)
// {
// 	HeapTuple	tp;

// 	tp = SearchSysCache1(OPEROID, ObjectIdGetDatum(opno));
// 	if (HeapTupleIsValid(tp))
// 	{
// 		Form_pg_operator optup = (Form_pg_operator) GETSTRUCT(tp);
// 		Oid			result;

// 		result = optup->oprresult;
// 		ReleaseSysCache(tp);
// 		return result;
// 	}
// 	else
// 		return InvalidOid;
// }

// char
// func_parallel(Oid funcid)
// {
// 	HeapTuple	tp;
// 	char		result;

// 	tp = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcid));
// 	if (!HeapTupleIsValid(tp))
// 		elog(ERROR, "cache lookup failed for function %u", funcid);

// 	result = ((Form_pg_proc) GETSTRUCT(tp))->proparallel;
// 	ReleaseSysCache(tp);
// 	return result;
// }

// Oid
// get_transform_fromsql(Oid typid, Oid langid, List *trftypes)
// {
// 	HeapTuple	tup;

// 	if (!list_member_oid(trftypes, typid))
// 		return InvalidOid;

// 	tup = SearchSysCache2(TRFTYPELANG, typid, langid);
// 	if (HeapTupleIsValid(tup))
// 	{
// 		Oid			funcid;

// 		funcid = ((Form_pg_transform) GETSTRUCT(tup))->trffromsql;
// 		ReleaseSysCache(tup);
// 		return funcid;
// 	}
// 	else
// 		return InvalidOid;
// }

// Oid
// get_transform_tosql(Oid typid, Oid langid, List *trftypes)
// {
// 	HeapTuple	tup;

// 	if (!list_member_oid(trftypes, typid))
// 		return InvalidOid;

// 	tup = SearchSysCache2(TRFTYPELANG, typid, langid);
// 	if (HeapTupleIsValid(tup))
// 	{
// 		Oid			funcid;

// 		funcid = ((Form_pg_transform) GETSTRUCT(tup))->trftosql;
// 		ReleaseSysCache(tup);
// 		return funcid;
// 	}
// 	else
// 		return InvalidOid;
// }

// Oid
// get_promoted_array_type(Oid typid)
// {
// 	Oid			array_type = get_array_type(typid);

// 	if (OidIsValid(array_type))
// 		return array_type;
// 	if (OidIsValid(get_element_type(typid)))
// 		return typid;
// 	return InvalidOid;
// }

// int32
// get_attavgwidth(Oid relid, AttrNumber attnum)
// {
// 	HeapTuple	tp;
// 	int32		stawidth;

// 	if (get_attavgwidth_hook)
// 	{
// 		stawidth = (*get_attavgwidth_hook) (relid, attnum);
// 		if (stawidth > 0)
// 			return stawidth;
// 	}
// 	tp = SearchSysCache3(STATRELATTINH,
// 						 ObjectIdGetDatum(relid),
// 						 Int16GetDatum(attnum),
// 						 BoolGetDatum(false));
// 	if (HeapTupleIsValid(tp))
// 	{
// 		stawidth = ((Form_pg_statistic) GETSTRUCT(tp))->stawidth;
// 		ReleaseSysCache(tp);
// 		if (stawidth > 0)
// 			return stawidth;
// 	}
// 	return 0;
// }

// char *
// get_namespace_name_or_temp(Oid nspid)
// {
// 	if (isTempNamespace(nspid))
// 		return "pg_temp";
// 	else
// 		return get_namespace_name(nspid);
// }

// void
// MemoryContextResetOnly(MemoryContext context)
// {
// 	AssertArg(MemoryContextIsValid(context));

// 	/* Nothing to do if no pallocs since startup or last reset */
// 	if (!context->isReset)
// 	{
// 		MemoryContextCallResetCallbacks(context);
// 		(*context->methods->reset) (context);
// 		context->isReset = true;
// 		VALGRIND_DESTROY_MEMPOOL(context);
// 		VALGRIND_CREATE_MEMPOOL(context, 0, false);
// 	}
// }

// void
// MemoryContextStatsDetail(MemoryContext context, int max_children)
// {
// 	MemoryContextCounters grand_totals;

// 	memset(&grand_totals, 0, sizeof(grand_totals));

// 	MemoryContextStatsInternal(context, 0, true, max_children, &grand_totals);

// 	fprintf(stderr,
// 	"Grand total: %zu bytes in %zd blocks; %zu free (%zd chunks); %zu used\n",
// 			grand_totals.totalspace, grand_totals.nblocks,
// 			grand_totals.freespace, grand_totals.freechunks,
// 			grand_totals.totalspace - grand_totals.freespace);
// }

// bool
// RelationHasUnloggedIndex(Relation rel)
// {
// 	List	   *indexoidlist;
// 	ListCell   *indexoidscan;
// 	bool		result = false;

// 	indexoidlist = RelationGetIndexList(rel);

// 	foreach(indexoidscan, indexoidlist)
// 	{
// 		Oid			indexoid = lfirst_oid(indexoidscan);
// 		HeapTuple	tp;
// 		Form_pg_class reltup;

// 		tp = SearchSysCache1(RELOID, ObjectIdGetDatum(indexoid));
// 		if (!HeapTupleIsValid(tp))
// 			elog(ERROR, "cache lookup failed for relation %u", indexoid);
// 		reltup = (Form_pg_class) GETSTRUCT(tp);

// 		if (reltup->relpersistence == RELPERSISTENCE_UNLOGGED
// 			|| reltup->relam == HASH_AM_OID)
// 			result = true;

// 		ReleaseSysCache(tp);

// 		if (result == true)
// 			break;
// 	}

// 	list_free(indexoidlist);

// 	return result;
// }

// List *
// RelationGetFKeyList(Relation relation)
// {
// 	List	   *result;
// 	Relation	conrel;
// 	SysScanDesc conscan;
// 	ScanKeyData skey;
// 	HeapTuple	htup;
// 	List	   *oldlist;
// 	MemoryContext oldcxt;

// 	/* Quick exit if we already computed the list. */
// 	if (relation->rd_fkeyvalid)
// 		return relation->rd_fkeylist;

// 	/* Fast path: if it doesn't have any triggers, it can't have FKs */
// 	if (!relation->rd_rel->relhastriggers)
// 		return NIL;

// 	/*
// 	 * We build the list we intend to return (in the caller's context) while
// 	 * doing the scan.  After successfully completing the scan, we copy that
// 	 * list into the relcache entry.  This avoids cache-context memory leakage
// 	 * if we get some sort of error partway through.
// 	 */
// 	result = NIL;

// 	/* Prepare to scan pg_constraint for entries having conrelid = this rel. */
// 	ScanKeyInit(&skey,
// 				Anum_pg_constraint_conrelid,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(RelationGetRelid(relation)));

// 	conrel = heap_open(ConstraintRelationId, AccessShareLock);
// 	conscan = systable_beginscan(conrel, ConstraintRelidIndexId, true,
// 								 NULL, 1, &skey);

// 	while (HeapTupleIsValid(htup = systable_getnext(conscan)))
// 	{
// 		Form_pg_constraint constraint = (Form_pg_constraint) GETSTRUCT(htup);
// 		ForeignKeyCacheInfo *info;
// 		Datum		adatum;
// 		bool		isnull;
// 		ArrayType  *arr;
// 		int			nelem;

// 		/* consider only foreign keys */
// 		if (constraint->contype != CONSTRAINT_FOREIGN)
// 			continue;

// 		info = makeNode(ForeignKeyCacheInfo);
// 		info->conrelid = constraint->conrelid;
// 		info->confrelid = constraint->confrelid;

// 		/* Extract data from conkey field */
// 		adatum = fastgetattr(htup, Anum_pg_constraint_conkey,
// 							 conrel->rd_att, &isnull);
// 		if (isnull)
// 			elog(ERROR, "null conkey for rel %s",
// 				 RelationGetRelationName(relation));

// 		arr = DatumGetArrayTypeP(adatum);		/* ensure not toasted */
// 		nelem = ARR_DIMS(arr)[0];
// 		if (ARR_NDIM(arr) != 1 ||
// 			nelem < 1 ||
// 			nelem > INDEX_MAX_KEYS ||
// 			ARR_HASNULL(arr) ||
// 			ARR_ELEMTYPE(arr) != INT2OID)
// 			elog(ERROR, "conkey is not a 1-D smallint array");

// 		info->nkeys = nelem;
// 		memcpy(info->conkey, ARR_DATA_PTR(arr), nelem * sizeof(AttrNumber));

// 		/* Likewise for confkey */
// 		adatum = fastgetattr(htup, Anum_pg_constraint_confkey,
// 							 conrel->rd_att, &isnull);
// 		if (isnull)
// 			elog(ERROR, "null confkey for rel %s",
// 				 RelationGetRelationName(relation));

// 		arr = DatumGetArrayTypeP(adatum);		/* ensure not toasted */
// 		nelem = ARR_DIMS(arr)[0];
// 		if (ARR_NDIM(arr) != 1 ||
// 			nelem != info->nkeys ||
// 			ARR_HASNULL(arr) ||
// 			ARR_ELEMTYPE(arr) != INT2OID)
// 			elog(ERROR, "confkey is not a 1-D smallint array");

// 		memcpy(info->confkey, ARR_DATA_PTR(arr), nelem * sizeof(AttrNumber));

// 		/* Likewise for conpfeqop */
// 		adatum = fastgetattr(htup, Anum_pg_constraint_conpfeqop,
// 							 conrel->rd_att, &isnull);
// 		if (isnull)
// 			elog(ERROR, "null conpfeqop for rel %s",
// 				 RelationGetRelationName(relation));

// 		arr = DatumGetArrayTypeP(adatum);		/* ensure not toasted */
// 		nelem = ARR_DIMS(arr)[0];
// 		if (ARR_NDIM(arr) != 1 ||
// 			nelem != info->nkeys ||
// 			ARR_HASNULL(arr) ||
// 			ARR_ELEMTYPE(arr) != OIDOID)
// 			elog(ERROR, "conpfeqop is not a 1-D OID array");

// 		memcpy(info->conpfeqop, ARR_DATA_PTR(arr), nelem * sizeof(Oid));

// 		/* Add FK's node to the result list */
// 		result = lappend(result, info);
// 	}

// 	systable_endscan(conscan);
// 	heap_close(conrel, AccessShareLock);

// 	/* Now save a copy of the completed list in the relcache entry. */
// 	oldcxt = MemoryContextSwitchTo(CacheMemoryContext);
// 	oldlist = relation->rd_fkeylist;
// 	relation->rd_fkeylist = copyObject(result);
// 	relation->rd_fkeyvalid = true;
// 	MemoryContextSwitchTo(oldcxt);

// 	/* Don't leak the old list, if there is one */
// 	list_free_deep(oldlist);

// 	return result;
// }

// List *
// RelationGetIndexList(Relation relation)
// {
// 	Relation	indrel;
// 	SysScanDesc indscan;
// 	ScanKeyData skey;
// 	HeapTuple	htup;
// 	List	   *result;
// 	List	   *oldlist;
// 	char		replident = relation->rd_rel->relreplident;
// 	Oid			oidIndex = InvalidOid;
// 	Oid			pkeyIndex = InvalidOid;
// 	Oid			candidateIndex = InvalidOid;
// 	MemoryContext oldcxt;

// 	/* Quick exit if we already computed the list. */
// 	if (relation->rd_indexvalid != 0)
// 		return list_copy(relation->rd_indexlist);

// 	/*
// 	 * We build the list we intend to return (in the caller's context) while
// 	 * doing the scan.  After successfully completing the scan, we copy that
// 	 * list into the relcache entry.  This avoids cache-context memory leakage
// 	 * if we get some sort of error partway through.
// 	 */
// 	result = NIL;
// 	oidIndex = InvalidOid;

// 	/* Prepare to scan pg_index for entries having indrelid = this rel. */
// 	ScanKeyInit(&skey,
// 				Anum_pg_index_indrelid,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(RelationGetRelid(relation)));

// 	indrel = heap_open(IndexRelationId, AccessShareLock);
// 	indscan = systable_beginscan(indrel, IndexIndrelidIndexId, true,
// 								 NULL, 1, &skey);

// 	while (HeapTupleIsValid(htup = systable_getnext(indscan)))
// 	{
// 		Form_pg_index index = (Form_pg_index) GETSTRUCT(htup);
// 		Datum		indclassDatum;
// 		oidvector  *indclass;
// 		bool		isnull;

// 		/*
// 		 * Ignore any indexes that are currently being dropped.  This will
// 		 * prevent them from being searched, inserted into, or considered in
// 		 * HOT-safety decisions.  It's unsafe to touch such an index at all
// 		 * since its catalog entries could disappear at any instant.
// 		 */
// 		if (!IndexIsLive(index))
// 			continue;

// 		/* Add index's OID to result list in the proper order */
// 		result = insert_ordered_oid(result, index->indexrelid);

// 		/*
// 		 * indclass cannot be referenced directly through the C struct,
// 		 * because it comes after the variable-width indkey field.  Must
// 		 * extract the datum the hard way...
// 		 */
// 		indclassDatum = heap_getattr(htup,
// 									 Anum_pg_index_indclass,
// 									 GetPgIndexDescriptor(),
// 									 &isnull);
// 		Assert(!isnull);
// 		indclass = (oidvector *) DatumGetPointer(indclassDatum);

// 		/*
// 		 * Invalid, non-unique, non-immediate or predicate indexes aren't
// 		 * interesting for either oid indexes or replication identity indexes,
// 		 * so don't check them.
// 		 */
// 		if (!IndexIsValid(index) || !index->indisunique ||
// 			!index->indimmediate ||
// 			!heap_attisnull(htup, Anum_pg_index_indpred))
// 			continue;

// 		/* Check to see if is a usable btree index on OID */
// 		if (index->indnatts == 1 &&
// 			index->indkey.values[0] == ObjectIdAttributeNumber &&
// 			indclass->values[0] == OID_BTREE_OPS_OID)
// 			oidIndex = index->indexrelid;

// 		/* remember primary key index if any */
// 		if (index->indisprimary)
// 			pkeyIndex = index->indexrelid;

// 		/* remember explicitly chosen replica index */
// 		if (index->indisreplident)
// 			candidateIndex = index->indexrelid;
// 	}

// 	systable_endscan(indscan);

// 	heap_close(indrel, AccessShareLock);

// 	/* Now save a copy of the completed list in the relcache entry. */
// 	oldcxt = MemoryContextSwitchTo(CacheMemoryContext);
// 	oldlist = relation->rd_indexlist;
// 	relation->rd_indexlist = list_copy(result);
// 	relation->rd_oidindex = oidIndex;
// 	if (replident == REPLICA_IDENTITY_DEFAULT && OidIsValid(pkeyIndex))
// 		relation->rd_replidindex = pkeyIndex;
// 	else if (replident == REPLICA_IDENTITY_INDEX && OidIsValid(candidateIndex))
// 		relation->rd_replidindex = candidateIndex;
// 	else
// 		relation->rd_replidindex = InvalidOid;
// 	relation->rd_indexvalid = 1;
// 	MemoryContextSwitchTo(oldcxt);

// 	/* Don't leak the old list, if there is one */
// 	list_free(oldlist);

// 	return result;
// }
// int
// errtable(Relation rel)
// {
// 	err_generic_string(PG_DIAG_SCHEMA_NAME,
// 					   get_namespace_name(RelationGetNamespace(rel)));
// 	err_generic_string(PG_DIAG_TABLE_NAME, RelationGetRelationName(rel));

// 	return 0;					/* return value does not matter */
// }

// int
// errtablecolname(Relation rel, const char *colname)
// {
// 	errtable(rel);
// 	err_generic_string(PG_DIAG_COLUMN_NAME, colname);

// 	return 0;					/* return value does not matter */
// }

// int
// errtableconstraint(Relation rel, const char *conname)
// {
// 	errtable(rel);
// 	err_generic_string(PG_DIAG_CONSTRAINT_NAME, conname);

// 	return 0;					/* return value does not matter */
// }

// Relation
// RelationBuildLocalRelation(const char *relname,
// 						   Oid relnamespace,
// 						   TupleDesc tupDesc,
// 						   Oid relid,
// 						   Oid relfilenode,
// 						   Oid reltablespace,
// 						   bool shared_relation,
// 						   bool mapped_relation,
// 						   char relpersistence,
// 						   char relkind)
// {
// 	Relation	rel;
// 	MemoryContext oldcxt;
// 	int			natts = tupDesc->natts;
// 	int			i;
// 	bool		has_not_null;
// 	bool		nailit;

// 	AssertArg(natts >= 0);

// 	/*
// 	 * check for creation of a rel that must be nailed in cache.
// 	 *
// 	 * XXX this list had better match the relations specially handled in
// 	 * RelationCacheInitializePhase2/3.
// 	 */
// 	switch (relid)
// 	{
// 		case DatabaseRelationId:
// 		case AuthIdRelationId:
// 		case AuthMemRelationId:
// 		case RelationRelationId:
// 		case AttributeRelationId:
// 		case ProcedureRelationId:
// 		case TypeRelationId:
// 			nailit = true;
// 			break;
// 		default:
// 			nailit = false;
// 			break;
// 	}

// 	/*
// 	 * check that hardwired list of shared rels matches what's in the
// 	 * bootstrap .bki file.  If you get a failure here during initdb, you
// 	 * probably need to fix IsSharedRelation() to match whatever you've done
// 	 * to the set of shared relations.
// 	 */
// 	if (shared_relation != IsSharedRelation(relid))
// 		elog(ERROR, "shared_relation flag for \"%s\" does not match IsSharedRelation(%u)",
// 			 relname, relid);

// 	/* Shared relations had better be mapped, too */
// 	Assert(mapped_relation || !shared_relation);

// 	/*
// 	 * switch to the cache context to create the relcache entry.
// 	 */
// 	if (!CacheMemoryContext)
// 		CreateCacheMemoryContext();

// 	oldcxt = MemoryContextSwitchTo(CacheMemoryContext);

// 	/*
// 	 * allocate a new relation descriptor and fill in basic state fields.
// 	 */
// 	rel = (Relation) palloc0(sizeof(RelationData));

// 	/* make sure relation is marked as having no open file yet */
// 	rel->rd_smgr = NULL;

// 	/* mark it nailed if appropriate */
// 	rel->rd_isnailed = nailit;

// 	rel->rd_refcnt = nailit ? 1 : 0;

// 	/* it's being created in this transaction */
// 	rel->rd_createSubid = GetCurrentSubTransactionId();
// 	rel->rd_newRelfilenodeSubid = InvalidSubTransactionId;

// 	/*
// 	 * create a new tuple descriptor from the one passed in.  We do this
// 	 * partly to copy it into the cache context, and partly because the new
// 	 * relation can't have any defaults or constraints yet; they have to be
// 	 * added in later steps, because they require additions to multiple system
// 	 * catalogs.  We can copy attnotnull constraints here, however.
// 	 */
// 	rel->rd_att = CreateTupleDescCopy(tupDesc);
// 	rel->rd_att->tdrefcount = 1;	/* mark as refcounted */
// 	has_not_null = false;
// 	for (i = 0; i < natts; i++)
// 	{
// 		rel->rd_att->attrs[i]->attnotnull = tupDesc->attrs[i]->attnotnull;
// 		has_not_null |= tupDesc->attrs[i]->attnotnull;
// 	}

// 	if (has_not_null)
// 	{
// 		TupleConstr *constr = (TupleConstr *) palloc0(sizeof(TupleConstr));

// 		constr->has_not_null = true;
// 		rel->rd_att->constr = constr;
// 	}

// 	/*
// 	 * initialize relation tuple form (caller may add/override data later)
// 	 */
// 	rel->rd_rel = (Form_pg_class) palloc0(CLASS_TUPLE_SIZE);

// 	namestrcpy(&rel->rd_rel->relname, relname);
// 	rel->rd_rel->relnamespace = relnamespace;

// 	rel->rd_rel->relkind = relkind;
// 	rel->rd_rel->relhasoids = rel->rd_att->tdhasoid;
// 	rel->rd_rel->relnatts = natts;
// 	rel->rd_rel->reltype = InvalidOid;
// 	/* needed when bootstrapping: */
// 	rel->rd_rel->relowner = BOOTSTRAP_SUPERUSERID;

// 	/* set up persistence and relcache fields dependent on it */
// 	rel->rd_rel->relpersistence = relpersistence;
// 	switch (relpersistence)
// 	{
// 		case RELPERSISTENCE_UNLOGGED:
// 		case RELPERSISTENCE_PERMANENT:
// 			rel->rd_backend = InvalidBackendId;
// 			rel->rd_islocaltemp = false;
// 			break;
// 		case RELPERSISTENCE_TEMP:
// 			Assert(isTempOrTempToastNamespace(relnamespace));
// 			rel->rd_backend = BackendIdForTempRelations();
// 			rel->rd_islocaltemp = true;
// 			break;
// 		default:
// 			elog(ERROR, "invalid relpersistence: %c", relpersistence);
// 			break;
// 	}

// 	/* if it's a materialized view, it's not populated initially */
// 	if (relkind == RELKIND_MATVIEW)
// 		rel->rd_rel->relispopulated = false;
// 	else
// 		rel->rd_rel->relispopulated = true;

// 	/* system relations and non-table objects don't have one */
// 	if (!IsSystemNamespace(relnamespace) &&
// 		(relkind == RELKIND_RELATION || relkind == RELKIND_MATVIEW))
// 		rel->rd_rel->relreplident = REPLICA_IDENTITY_DEFAULT;
// 	else
// 		rel->rd_rel->relreplident = REPLICA_IDENTITY_NOTHING;

// 	/*
// 	 * Insert relation physical and logical identifiers (OIDs) into the right
// 	 * places.  For a mapped relation, we set relfilenode to zero and rely on
// 	 * RelationInitPhysicalAddr to consult the map.
// 	 */
// 	rel->rd_rel->relisshared = shared_relation;

// 	RelationGetRelid(rel) = relid;

// 	for (i = 0; i < natts; i++)
// 		rel->rd_att->attrs[i]->attrelid = relid;

// 	rel->rd_rel->reltablespace = reltablespace;

// 	if (mapped_relation)
// 	{
// 		rel->rd_rel->relfilenode = InvalidOid;
// 		/* Add it to the active mapping information */
// 		RelationMapUpdateMap(relid, relfilenode, shared_relation, true);
// 	}
// 	else
// 		rel->rd_rel->relfilenode = relfilenode;

// 	RelationInitLockInfo(rel);	/* see lmgr.c */

// 	RelationInitPhysicalAddr(rel);

// 	/*
// 	 * Okay to insert into the relcache hash table.
// 	 *
// 	 * Ordinarily, there should certainly not be an existing hash entry for
// 	 * the same OID; but during bootstrap, when we create a "real" relcache
// 	 * entry for one of the bootstrap relations, we'll be overwriting the
// 	 * phony one created with formrdesc.  So allow that to happen for nailed
// 	 * rels.
// 	 */
// 	RelationCacheInsert(rel, nailit);

// 	/*
// 	 * Flag relation as needing eoxact cleanup (to clear rd_createSubid). We
// 	 * can't do this before storing relid in it.
// 	 */
// 	EOXactListAdd(rel);

// 	/*
// 	 * done building relcache entry.
// 	 */
// 	MemoryContextSwitchTo(oldcxt);

// 	/* It's fully valid */
// 	rel->rd_isvalid = true;

// 	/*
// 	 * Caller expects us to pin the returned entry.
// 	 */
// 	RelationIncrementReferenceCount(rel);

// 	return rel;
// }

// void
// RelationSetNewRelfilenode(Relation relation, char persistence,
// 						  TransactionId freezeXid, MultiXactId minmulti)
// {
// 	Oid			newrelfilenode;
// 	RelFileNodeBackend newrnode;
// 	Relation	pg_class;
// 	HeapTuple	tuple;
// 	Form_pg_class classform;

// 	/* Indexes, sequences must have Invalid frozenxid; other rels must not */
// 	Assert((relation->rd_rel->relkind == RELKIND_INDEX ||
// 			relation->rd_rel->relkind == RELKIND_SEQUENCE) ?
// 		   freezeXid == InvalidTransactionId :
// 		   TransactionIdIsNormal(freezeXid));
// 	Assert(TransactionIdIsNormal(freezeXid) == MultiXactIdIsValid(minmulti));

// 	/* Allocate a new relfilenode */
// 	newrelfilenode = GetNewRelFileNode(relation->rd_rel->reltablespace, NULL,
// 									   persistence);

// 	/*
// 	 * Get a writable copy of the pg_class tuple for the given relation.
// 	 */
// 	pg_class = heap_open(RelationRelationId, RowExclusiveLock);

// 	tuple = SearchSysCacheCopy1(RELOID,
// 								ObjectIdGetDatum(RelationGetRelid(relation)));
// 	if (!HeapTupleIsValid(tuple))
// 		elog(ERROR, "could not find tuple for relation %u",
// 			 RelationGetRelid(relation));
// 	classform = (Form_pg_class) GETSTRUCT(tuple);

// 	/*
// 	 * Create storage for the main fork of the new relfilenode.
// 	 *
// 	 * NOTE: any conflict in relfilenode value will be caught here, if
// 	 * GetNewRelFileNode messes up for any reason.
// 	 */
// 	newrnode.node = relation->rd_node;
// 	newrnode.node.relNode = newrelfilenode;
// 	newrnode.backend = relation->rd_backend;
// 	RelationCreateStorage(newrnode.node, persistence);
// 	smgrclosenode(newrnode);

// 	/*
// 	 * Schedule unlinking of the old storage at transaction commit.
// 	 */
// 	RelationDropStorage(relation);

// 	/*
// 	 * Now update the pg_class row.  However, if we're dealing with a mapped
// 	 * index, pg_class.relfilenode doesn't change; instead we have to send the
// 	 * update to the relation mapper.
// 	 */
// 	if (RelationIsMapped(relation))
// 		RelationMapUpdateMap(RelationGetRelid(relation),
// 							 newrelfilenode,
// 							 relation->rd_rel->relisshared,
// 							 false);
// 	else
// 		classform->relfilenode = newrelfilenode;

// 	/* These changes are safe even for a mapped relation */
// 	if (relation->rd_rel->relkind != RELKIND_SEQUENCE)
// 	{
// 		classform->relpages = 0;	/* it's empty until further notice */
// 		classform->reltuples = 0;
// 		classform->relallvisible = 0;
// 	}
// 	classform->relfrozenxid = freezeXid;
// 	classform->relminmxid = minmulti;
// 	classform->relpersistence = persistence;

// 	simple_heap_update(pg_class, &tuple->t_self, tuple);
// 	CatalogUpdateIndexes(pg_class, tuple);

// 	heap_freetuple(tuple);

// 	heap_close(pg_class, RowExclusiveLock);

// 	/*
// 	 * Make the pg_class row change visible, as well as the relation map
// 	 * change if any.  This will cause the relcache entry to get updated, too.
// 	 */
// 	CommandCounterIncrement();

// 	/*
// 	 * Mark the rel as having been given a new relfilenode in the current
// 	 * (sub) transaction.  This is a hint that can be used to optimize later
// 	 * operations on the rel in the same transaction.
// 	 */
// 	relation->rd_newRelfilenodeSubid = GetCurrentSubTransactionId();

// 	/* Flag relation as needing eoxact cleanup (to remove the hint) */
// 	EOXactListAdd(relation);
// }
// int
// check_enable_rls(Oid relid, Oid checkAsUser, bool noError)
// {
// 	Oid			user_id = checkAsUser ? checkAsUser : GetUserId();
// 	HeapTuple	tuple;
// 	Form_pg_class classform;
// 	bool		relrowsecurity;
// 	bool		relforcerowsecurity;
// 	bool		amowner;

// 	/* Nothing to do for built-in relations */
// 	if (relid < (Oid) FirstNormalObjectId)
// 		return RLS_NONE;

// 	/* Fetch relation's relrowsecurity and relforcerowsecurity flags */
// 	tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(relid));
// 	if (!HeapTupleIsValid(tuple))
// 		return RLS_NONE;
// 	classform = (Form_pg_class) GETSTRUCT(tuple);

// 	relrowsecurity = classform->relrowsecurity;
// 	relforcerowsecurity = classform->relforcerowsecurity;

// 	ReleaseSysCache(tuple);

// 	/* Nothing to do if the relation does not have RLS */
// 	if (!relrowsecurity)
// 		return RLS_NONE;

// 	/*
// 	 * BYPASSRLS users always bypass RLS.  Note that superusers are always
// 	 * considered to have BYPASSRLS.
// 	 *
// 	 * Return RLS_NONE_ENV to indicate that this decision depends on the
// 	 * environment (in this case, the user_id).
// 	 */
// 	if (has_bypassrls_privilege(user_id))
// 		return RLS_NONE_ENV;

// 	/*
// 	 * Table owners generally bypass RLS, except if the table has been set (by
// 	 * an owner) to FORCE ROW SECURITY, and this is not a referential
// 	 * integrity check.
// 	 *
// 	 * Return RLS_NONE_ENV to indicate that this decision depends on the
// 	 * environment (in this case, the user_id).
// 	 */
// 	amowner = pg_class_ownercheck(relid, user_id);
// 	if (amowner)
// 	{
// 		/*
// 		 * If FORCE ROW LEVEL SECURITY has been set on the relation then we
// 		 * should return RLS_ENABLED to indicate that RLS should be applied.
// 		 * If not, or if we are in an InNoForceRLSOperation context, we return
// 		 * RLS_NONE_ENV.
// 		 *
// 		 * InNoForceRLSOperation indicates that we should not apply RLS even
// 		 * if the table has FORCE RLS set - IF the current user is the owner.
// 		 * This is specifically to ensure that referential integrity checks
// 		 * are able to still run correctly.
// 		 *
// 		 * This is intentionally only done after we have checked that the user
// 		 * is the table owner, which should always be the case for referential
// 		 * integrity checks.
// 		 */
// 		if (!relforcerowsecurity || InNoForceRLSOperation())
// 			return RLS_NONE_ENV;
// 	}

// 	/*
// 	 * We should apply RLS.  However, the user may turn off the row_security
// 	 * GUC to get a forced error instead.
// 	 */
// 	if (!row_security && !noError)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("query would be affected by row-level security policy for table \"%s\"",
// 						get_rel_name(relid)),
// 				 amowner ? errhint("To disable the policy for the table's owner, use ALTER TABLE NO FORCE ROW LEVEL SECURITY.") : 0));

// 	/* RLS should be fully enabled for this relation. */
// 	return RLS_ENABLED;
// }

// Size
// SnapMgrShmemSize(void)
// {
// 	Size		size;

// 	size = offsetof(OldSnapshotControlData, xid_by_minute);
// 	if (old_snapshot_threshold > 0)
// 		size = add_size(size, mul_size(sizeof(TransactionId),
// 									   OLD_SNAPSHOT_TIME_MAP_ENTRIES));

// 	return size;
// }
// void
// SnapMgrInit(void)
// {
// 	bool		found;

// 	/*
// 	 * Create or attach to the OldSnapshotControlData structure.
// 	 */
// 	oldSnapshotControl = (volatile OldSnapshotControlData *)
// 		ShmemInitStruct("OldSnapshotControlData",
// 						SnapMgrShmemSize(), &found);

// 	if (!found)
// 	{
// 		SpinLockInit(&oldSnapshotControl->mutex_current);
// 		oldSnapshotControl->current_timestamp = 0;
// 		SpinLockInit(&oldSnapshotControl->mutex_latest_xmin);
// 		oldSnapshotControl->latest_xmin = InvalidTransactionId;
// 		oldSnapshotControl->next_map_update = 0;
// 		SpinLockInit(&oldSnapshotControl->mutex_threshold);
// 		oldSnapshotControl->threshold_timestamp = 0;
// 		oldSnapshotControl->threshold_xid = InvalidTransactionId;
// 		oldSnapshotControl->head_offset = 0;
// 		oldSnapshotControl->head_timestamp = 0;
// 		oldSnapshotControl->count_used = 0;
// 	}
// }
// int64
// GetSnapshotCurrentTimestamp(void)
// {
// 	int64		now = GetCurrentIntegerTimestamp();

// 	/*
// 	 * Don't let time move backward; if it hasn't advanced, use the old value.
// 	 */
// 	SpinLockAcquire(&oldSnapshotControl->mutex_current);
// 	if (now <= oldSnapshotControl->current_timestamp)
// 		now = oldSnapshotControl->current_timestamp;
// 	else
// 		oldSnapshotControl->current_timestamp = now;
// 	SpinLockRelease(&oldSnapshotControl->mutex_current);

// 	return now;
// }
// int64
// GetOldSnapshotThresholdTimestamp(void)
// {
// 	int64		threshold_timestamp;

// 	SpinLockAcquire(&oldSnapshotControl->mutex_threshold);
// 	threshold_timestamp = oldSnapshotControl->threshold_timestamp;
// 	SpinLockRelease(&oldSnapshotControl->mutex_threshold);

// 	return threshold_timestamp;
// }

// Snapshot
// GetTransactionSnapshot(void)
// {
// 	/*
// 	 * Return historic snapshot if doing logical decoding. We'll never need a
// 	 * non-historic transaction snapshot in this (sub-)transaction, so there's
// 	 * no need to be careful to set one up for later calls to
// 	 * GetTransactionSnapshot().
// 	 */
// 	if (HistoricSnapshotActive())
// 	{
// 		Assert(!FirstSnapshotSet);
// 		return HistoricSnapshot;
// 	}

// 	/* First call in transaction? */
// 	if (!FirstSnapshotSet)
// 	{
// 		/*
// 		 * Don't allow catalog snapshot to be older than xact snapshot.  Must
// 		 * do this first to allow the empty-heap Assert to succeed.
// 		 */
// 		InvalidateCatalogSnapshot();

// 		Assert(pairingheap_is_empty(&RegisteredSnapshots));
// 		Assert(FirstXactSnapshot == NULL);

// 		if (IsInParallelMode())
// 			elog(ERROR,
// 				 "cannot take query snapshot during a parallel operation");

// 		/*
// 		 * In transaction-snapshot mode, the first snapshot must live until
// 		 * end of xact regardless of what the caller does with it, so we must
// 		 * make a copy of it rather than returning CurrentSnapshotData
// 		 * directly.  Furthermore, if we're running in serializable mode,
// 		 * predicate.c needs to wrap the snapshot fetch in its own processing.
// 		 */
// 		if (IsolationUsesXactSnapshot())
// 		{
// 			/* First, create the snapshot in CurrentSnapshotData */
// 			if (IsolationIsSerializable())
// 				CurrentSnapshot = GetSerializableTransactionSnapshot(&CurrentSnapshotData);
// 			else
// 				CurrentSnapshot = GetSnapshotData(&CurrentSnapshotData);
// 			/* Make a saved copy */
// 			CurrentSnapshot = CopySnapshot(CurrentSnapshot);
// 			FirstXactSnapshot = CurrentSnapshot;
// 			/* Mark it as "registered" in FirstXactSnapshot */
// 			FirstXactSnapshot->regd_count++;
// 			pairingheap_add(&RegisteredSnapshots, &FirstXactSnapshot->ph_node);
// 		}
// 		else
// 			CurrentSnapshot = GetSnapshotData(&CurrentSnapshotData);

// 		FirstSnapshotSet = true;
// 		return CurrentSnapshot;
// 	}

// 	if (IsolationUsesXactSnapshot())
// 		return CurrentSnapshot;

// 	/* Don't allow catalog snapshot to be older than xact snapshot. */
// 	InvalidateCatalogSnapshot();

// 	CurrentSnapshot = GetSnapshotData(&CurrentSnapshotData);

// 	return CurrentSnapshot;
// }
// Snapshot
// GetOldestSnapshot(void)
// {
// 	Snapshot	OldestRegisteredSnapshot = NULL;
// 	XLogRecPtr	RegisteredLSN = InvalidXLogRecPtr;

// 	if (!pairingheap_is_empty(&RegisteredSnapshots))
// 	{
// 		OldestRegisteredSnapshot = pairingheap_container(SnapshotData, ph_node,
// 									pairingheap_first(&RegisteredSnapshots));
// 		RegisteredLSN = OldestRegisteredSnapshot->lsn;
// 	}

// 	if (OldestActiveSnapshot != NULL)
// 	{
// 		XLogRecPtr	ActiveLSN = OldestActiveSnapshot->as_snap->lsn;

// 		if (XLogRecPtrIsInvalid(RegisteredLSN) || RegisteredLSN > ActiveLSN)
// 			return OldestActiveSnapshot->as_snap;
// 	}

// 	return OldestRegisteredSnapshot;
// }

// Snapshot
// GetCatalogSnapshot(Oid relid)
// {
// 	/*
// 	 * Return historic snapshot while we're doing logical decoding, so we can
// 	 * see the appropriate state of the catalog.
// 	 *
// 	 * This is the primary reason for needing to reset the system caches after
// 	 * finishing decoding.
// 	 */
// 	if (HistoricSnapshotActive())
// 		return HistoricSnapshot;

// 	return GetNonHistoricCatalogSnapshot(relid);
// }
// Snapshot
// GetNonHistoricCatalogSnapshot(Oid relid)
// {
// 	/*
// 	 * If the caller is trying to scan a relation that has no syscache, no
// 	 * catcache invalidations will be sent when it is updated.  For a few key
// 	 * relations, snapshot invalidations are sent instead.  If we're trying to
// 	 * scan a relation for which neither catcache nor snapshot invalidations
// 	 * are sent, we must refresh the snapshot every time.
// 	 */
// 	if (CatalogSnapshot &&
// 		!RelationInvalidatesSnapshotsOnly(relid) &&
// 		!RelationHasSysCache(relid))
// 		InvalidateCatalogSnapshot();

// 	if (CatalogSnapshot == NULL)
// 	{
// 		/* Get new snapshot. */
// 		CatalogSnapshot = GetSnapshotData(&CatalogSnapshotData);

// 		/*
// 		 * Make sure the catalog snapshot will be accounted for in decisions
// 		 * about advancing PGXACT->xmin.  We could apply RegisterSnapshot, but
// 		 * that would result in making a physical copy, which is overkill; and
// 		 * it would also create a dependency on some resource owner, which we
// 		 * do not want for reasons explained at the head of this file. Instead
// 		 * just shove the CatalogSnapshot into the pairing heap manually. This
// 		 * has to be reversed in InvalidateCatalogSnapshot, of course.
// 		 *
// 		 * NB: it had better be impossible for this to throw error, since the
// 		 * CatalogSnapshot pointer is already valid.
// 		 */
// 		pairingheap_add(&RegisteredSnapshots, &CatalogSnapshot->ph_node);
// 	}

// 	return CatalogSnapshot;
// }
// void
// InvalidateCatalogSnapshot(void)
// {
// 	if (CatalogSnapshot)
// 	{
// 		pairingheap_remove(&RegisteredSnapshots, &CatalogSnapshot->ph_node);
// 		CatalogSnapshot = NULL;
// 		SnapshotResetXmin();
// 	}
// }

// void
// InvalidateCatalogSnapshotConditionally(void)
// {
// 	if (CatalogSnapshot &&
// 		ActiveSnapshot == NULL &&
// 		pairingheap_is_singular(&RegisteredSnapshots))
// 		InvalidateCatalogSnapshot();
// }

// TransactionId
// TransactionIdLimitedForOldSnapshots(TransactionId recentXmin,
// 									Relation relation)
// {
// 	if (TransactionIdIsNormal(recentXmin)
// 		&& old_snapshot_threshold >= 0
// 		&& RelationAllowsEarlyPruning(relation))
// 	{
// 		int64		ts = GetSnapshotCurrentTimestamp();
// 		TransactionId xlimit = recentXmin;
// 		TransactionId latest_xmin;
// 		int64		update_ts;
// 		bool		same_ts_as_threshold = false;

// 		SpinLockAcquire(&oldSnapshotControl->mutex_latest_xmin);
// 		latest_xmin = oldSnapshotControl->latest_xmin;
// 		update_ts = oldSnapshotControl->next_map_update;
// 		SpinLockRelease(&oldSnapshotControl->mutex_latest_xmin);

// 		/*
// 		 * Zero threshold always overrides to latest xmin, if valid.  Without
// 		 * some heuristic it will find its own snapshot too old on, for
// 		 * example, a simple UPDATE -- which would make it useless for most
// 		 * testing, but there is no principled way to ensure that it doesn't
// 		 * fail in this way.  Use a five-second delay to try to get useful
// 		 * testing behavior, but this may need adjustment.
// 		 */
// 		if (old_snapshot_threshold == 0)
// 		{
// 			if (TransactionIdPrecedes(latest_xmin, MyPgXact->xmin)
// 				&& TransactionIdFollows(latest_xmin, xlimit))
// 				xlimit = latest_xmin;

// 			ts -= 5 * USECS_PER_SEC;
// 			SetOldSnapshotThresholdTimestamp(ts, xlimit);

// 			return xlimit;
// 		}

// 		ts = AlignTimestampToMinuteBoundary(ts)
// 			- (old_snapshot_threshold * USECS_PER_MINUTE);

// 		/* Check for fast exit without LW locking. */
// 		SpinLockAcquire(&oldSnapshotControl->mutex_threshold);
// 		if (ts == oldSnapshotControl->threshold_timestamp)
// 		{
// 			xlimit = oldSnapshotControl->threshold_xid;
// 			same_ts_as_threshold = true;
// 		}
// 		SpinLockRelease(&oldSnapshotControl->mutex_threshold);

// 		if (!same_ts_as_threshold)
// 		{
// 			if (ts == update_ts)
// 			{
// 				xlimit = latest_xmin;
// 				if (NormalTransactionIdFollows(xlimit, recentXmin))
// 					SetOldSnapshotThresholdTimestamp(ts, xlimit);
// 			}
// 			else
// 			{
// 				LWLockAcquire(OldSnapshotTimeMapLock, LW_SHARED);

// 				if (oldSnapshotControl->count_used > 0
// 					&& ts >= oldSnapshotControl->head_timestamp)
// 				{
// 					int			offset;

// 					offset = ((ts - oldSnapshotControl->head_timestamp)
// 							  / USECS_PER_MINUTE);
// 					if (offset > oldSnapshotControl->count_used - 1)
// 						offset = oldSnapshotControl->count_used - 1;
// 					offset = (oldSnapshotControl->head_offset + offset)
// 						% OLD_SNAPSHOT_TIME_MAP_ENTRIES;
// 					xlimit = oldSnapshotControl->xid_by_minute[offset];

// 					if (NormalTransactionIdFollows(xlimit, recentXmin))
// 						SetOldSnapshotThresholdTimestamp(ts, xlimit);
// 				}

// 				LWLockRelease(OldSnapshotTimeMapLock);
// 			}
// 		}

// 		/*
// 		 * Failsafe protection against vacuuming work of active transaction.
// 		 *
// 		 * This is not an assertion because we avoid the spinlock for
// 		 * performance, leaving open the possibility that xlimit could advance
// 		 * and be more current; but it seems prudent to apply this limit.  It
// 		 * might make pruning a tiny bit less aggressive than it could be, but
// 		 * protects against data loss bugs.
// 		 */
// 		if (TransactionIdIsNormal(latest_xmin)
// 			&& TransactionIdPrecedes(latest_xmin, xlimit))
// 			xlimit = latest_xmin;

// 		if (NormalTransactionIdFollows(xlimit, recentXmin))
// 			return xlimit;
// 	}

// 	return recentXmin;
// }
// void
// MaintainOldSnapshotTimeMapping(int64 whenTaken, TransactionId xmin)
// {
// 	int64		ts;
// 	TransactionId latest_xmin;
// 	int64		update_ts;
// 	bool		map_update_required = false;

// 	/* Never call this function when old snapshot checking is disabled. */
// 	Assert(old_snapshot_threshold >= 0);

// 	ts = AlignTimestampToMinuteBoundary(whenTaken);

// 	/*
// 	 * Keep track of the latest xmin seen by any process. Update mapping with
// 	 * a new value when we have crossed a bucket boundary.
// 	 */
// 	SpinLockAcquire(&oldSnapshotControl->mutex_latest_xmin);
// 	latest_xmin = oldSnapshotControl->latest_xmin;
// 	update_ts = oldSnapshotControl->next_map_update;
// 	if (ts > update_ts)
// 	{
// 		oldSnapshotControl->next_map_update = ts;
// 		map_update_required = true;
// 	}
// 	if (TransactionIdFollows(xmin, latest_xmin))
// 		oldSnapshotControl->latest_xmin = xmin;
// 	SpinLockRelease(&oldSnapshotControl->mutex_latest_xmin);

// 	/* We only needed to update the most recent xmin value. */
// 	if (!map_update_required)
// 		return;

// 	/* No further tracking needed for 0 (used for testing). */
// 	if (old_snapshot_threshold == 0)
// 		return;

// 	/*
// 	 * We don't want to do something stupid with unusual values, but we don't
// 	 * want to litter the log with warnings or break otherwise normal
// 	 * processing for this feature; so if something seems unreasonable, just
// 	 * log at DEBUG level and return without doing anything.
// 	 */
// 	if (whenTaken < 0)
// 	{
// 		elog(DEBUG1,
// 		"MaintainOldSnapshotTimeMapping called with negative whenTaken = %ld",
// 			 (long) whenTaken);
// 		return;
// 	}
// 	if (!TransactionIdIsNormal(xmin))
// 	{
// 		elog(DEBUG1,
// 			 "MaintainOldSnapshotTimeMapping called with xmin = %lu",
// 			 (unsigned long) xmin);
// 		return;
// 	}

// 	LWLockAcquire(OldSnapshotTimeMapLock, LW_EXCLUSIVE);

// 	Assert(oldSnapshotControl->head_offset >= 0);
// 	Assert(oldSnapshotControl->head_offset < OLD_SNAPSHOT_TIME_MAP_ENTRIES);
// 	Assert((oldSnapshotControl->head_timestamp % USECS_PER_MINUTE) == 0);
// 	Assert(oldSnapshotControl->count_used >= 0);
// 	Assert(oldSnapshotControl->count_used <= OLD_SNAPSHOT_TIME_MAP_ENTRIES);

// 	if (oldSnapshotControl->count_used == 0)
// 	{
// 		/* set up first entry for empty mapping */
// 		oldSnapshotControl->head_offset = 0;
// 		oldSnapshotControl->head_timestamp = ts;
// 		oldSnapshotControl->count_used = 1;
// 		oldSnapshotControl->xid_by_minute[0] = xmin;
// 	}
// 	else if (ts < oldSnapshotControl->head_timestamp)
// 	{
// 		/* old ts; log it at DEBUG */
// 		LWLockRelease(OldSnapshotTimeMapLock);
// 		elog(DEBUG1,
// 			 "MaintainOldSnapshotTimeMapping called with old whenTaken = %ld",
// 			 (long) whenTaken);
// 		return;
// 	}
// 	else if (ts <= (oldSnapshotControl->head_timestamp +
// 					((oldSnapshotControl->count_used - 1)
// 					 * USECS_PER_MINUTE)))
// 	{
// 		/* existing mapping; advance xid if possible */
// 		int			bucket = (oldSnapshotControl->head_offset
// 							  + ((ts - oldSnapshotControl->head_timestamp)
// 								 / USECS_PER_MINUTE))
// 		% OLD_SNAPSHOT_TIME_MAP_ENTRIES;

// 		if (TransactionIdPrecedes(oldSnapshotControl->xid_by_minute[bucket], xmin))
// 			oldSnapshotControl->xid_by_minute[bucket] = xmin;
// 	}
// 	else
// 	{
// 		/* We need a new bucket, but it might not be the very next one. */
// 		int			advance = ((ts - oldSnapshotControl->head_timestamp)
// 							   / USECS_PER_MINUTE);

// 		oldSnapshotControl->head_timestamp = ts;

// 		if (advance >= OLD_SNAPSHOT_TIME_MAP_ENTRIES)
// 		{
// 			/* Advance is so far that all old data is junk; start over. */
// 			oldSnapshotControl->head_offset = 0;
// 			oldSnapshotControl->count_used = 1;
// 			oldSnapshotControl->xid_by_minute[0] = xmin;
// 		}
// 		else
// 		{
// 			/* Store the new value in one or more buckets. */
// 			int			i;

// 			for (i = 0; i < advance; i++)
// 			{
// 				if (oldSnapshotControl->count_used == OLD_SNAPSHOT_TIME_MAP_ENTRIES)
// 				{
// 					/* Map full and new value replaces old head. */
// 					int			old_head = oldSnapshotControl->head_offset;

// 					if (old_head == (OLD_SNAPSHOT_TIME_MAP_ENTRIES - 1))
// 						oldSnapshotControl->head_offset = 0;
// 					else
// 						oldSnapshotControl->head_offset = old_head + 1;
// 					oldSnapshotControl->xid_by_minute[old_head] = xmin;
// 				}
// 				else
// 				{
// 					/* Extend map to unused entry. */
// 					int			new_tail = (oldSnapshotControl->head_offset
// 											+ oldSnapshotControl->count_used)
// 					% OLD_SNAPSHOT_TIME_MAP_ENTRIES;

// 					oldSnapshotControl->count_used++;
// 					oldSnapshotControl->xid_by_minute[new_tail] = xmin;
// 				}
// 			}
// 		}
// 	}

// 	LWLockRelease(OldSnapshotTimeMapLock);
// }
// char *
// ExportSnapshot(Snapshot snapshot)
// {
// 	TransactionId topXid;
// 	TransactionId *children;
// 	int			nchildren;
// 	int			addTopXid;
// 	StringInfoData buf;
// 	FILE	   *f;
// 	int			i;
// 	MemoryContext oldcxt;
// 	char		path[MAXPGPATH];
// 	char		pathtmp[MAXPGPATH];

// 	/*
// 	 * It's tempting to call RequireTransactionChain here, since it's not very
// 	 * useful to export a snapshot that will disappear immediately afterwards.
// 	 * However, we haven't got enough information to do that, since we don't
// 	 * know if we're at top level or not.  For example, we could be inside a
// 	 * plpgsql function that is going to fire off other transactions via
// 	 * dblink.  Rather than disallow perfectly legitimate usages, don't make a
// 	 * check.
// 	 *
// 	 * Also note that we don't make any restriction on the transaction's
// 	 * isolation level; however, importers must check the level if they are
// 	 * serializable.
// 	 */

// 	/*
// 	 * This will assign a transaction ID if we do not yet have one.
// 	 */
// 	topXid = GetTopTransactionId();

// 	/*
// 	 * We cannot export a snapshot from a subtransaction because there's no
// 	 * easy way for importers to verify that the same subtransaction is still
// 	 * running.
// 	 */
// 	if (IsSubTransaction())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_ACTIVE_SQL_TRANSACTION),
// 				 errmsg("cannot export a snapshot from a subtransaction")));

// 	/*
// 	 * We do however allow previous committed subtransactions to exist.
// 	 * Importers of the snapshot must see them as still running, so get their
// 	 * XIDs to add them to the snapshot.
// 	 */
// 	nchildren = xactGetCommittedChildren(&children);

// 	/*
// 	 * Copy the snapshot into TopTransactionContext, add it to the
// 	 * exportedSnapshots list, and mark it pseudo-registered.  We do this to
// 	 * ensure that the snapshot's xmin is honored for the rest of the
// 	 * transaction.
// 	 */
// 	snapshot = CopySnapshot(snapshot);

// 	oldcxt = MemoryContextSwitchTo(TopTransactionContext);
// 	exportedSnapshots = lappend(exportedSnapshots, snapshot);
// 	MemoryContextSwitchTo(oldcxt);

// 	snapshot->regd_count++;
// 	pairingheap_add(&RegisteredSnapshots, &snapshot->ph_node);

// 	/*
// 	 * Fill buf with a text serialization of the snapshot, plus identification
// 	 * data about this transaction.  The format expected by ImportSnapshot is
// 	 * pretty rigid: each line must be fieldname:value.
// 	 */
// 	initStringInfo(&buf);

// 	appendStringInfo(&buf, "xid:%u\n", topXid);
// 	appendStringInfo(&buf, "dbid:%u\n", MyDatabaseId);
// 	appendStringInfo(&buf, "iso:%d\n", XactIsoLevel);
// 	appendStringInfo(&buf, "ro:%d\n", XactReadOnly);

// 	appendStringInfo(&buf, "xmin:%u\n", snapshot->xmin);
// 	appendStringInfo(&buf, "xmax:%u\n", snapshot->xmax);

// 	/*
// 	 * We must include our own top transaction ID in the top-xid data, since
// 	 * by definition we will still be running when the importing transaction
// 	 * adopts the snapshot, but GetSnapshotData never includes our own XID in
// 	 * the snapshot.  (There must, therefore, be enough room to add it.)
// 	 *
// 	 * However, it could be that our topXid is after the xmax, in which case
// 	 * we shouldn't include it because xip[] members are expected to be before
// 	 * xmax.  (We need not make the same check for subxip[] members, see
// 	 * snapshot.h.)
// 	 */
// 	addTopXid = TransactionIdPrecedes(topXid, snapshot->xmax) ? 1 : 0;
// 	appendStringInfo(&buf, "xcnt:%d\n", snapshot->xcnt + addTopXid);
// 	for (i = 0; i < snapshot->xcnt; i++)
// 		appendStringInfo(&buf, "xip:%u\n", snapshot->xip[i]);
// 	if (addTopXid)
// 		appendStringInfo(&buf, "xip:%u\n", topXid);

// 	/*
// 	 * Similarly, we add our subcommitted child XIDs to the subxid data. Here,
// 	 * we have to cope with possible overflow.
// 	 */
// 	if (snapshot->suboverflowed ||
// 		snapshot->subxcnt + nchildren > GetMaxSnapshotSubxidCount())
// 		appendStringInfoString(&buf, "sof:1\n");
// 	else
// 	{
// 		appendStringInfoString(&buf, "sof:0\n");
// 		appendStringInfo(&buf, "sxcnt:%d\n", snapshot->subxcnt + nchildren);
// 		for (i = 0; i < snapshot->subxcnt; i++)
// 			appendStringInfo(&buf, "sxp:%u\n", snapshot->subxip[i]);
// 		for (i = 0; i < nchildren; i++)
// 			appendStringInfo(&buf, "sxp:%u\n", children[i]);
// 	}
// 	appendStringInfo(&buf, "rec:%u\n", snapshot->takenDuringRecovery);

// 	/*
// 	 * Now write the text representation into a file.  We first write to a
// 	 * ".tmp" filename, and rename to final filename if no error.  This
// 	 * ensures that no other backend can read an incomplete file
// 	 * (ImportSnapshot won't allow it because of its valid-characters check).
// 	 */
// 	XactExportFilePath(pathtmp, topXid, list_length(exportedSnapshots), ".tmp");
// 	if (!(f = AllocateFile(pathtmp, PG_BINARY_W)))
// 		ereport(ERROR,
// 				(errcode_for_file_access(),
// 				 errmsg("could not create file \"%s\": %m", pathtmp)));

// 	if (fwrite(buf.data, buf.len, 1, f) != 1)
// 		ereport(ERROR,
// 				(errcode_for_file_access(),
// 				 errmsg("could not write to file \"%s\": %m", pathtmp)));

// 	/* no fsync() since file need not survive a system crash */

// 	if (FreeFile(f))
// 		ereport(ERROR,
// 				(errcode_for_file_access(),
// 				 errmsg("could not write to file \"%s\": %m", pathtmp)));

// 	/*
// 	 * Now that we have written everything into a .tmp file, rename the file
// 	 * to remove the .tmp suffix.
// 	 */
// 	XactExportFilePath(path, topXid, list_length(exportedSnapshots), "");

// 	if (rename(pathtmp, path) < 0)
// 		ereport(ERROR,
// 				(errcode_for_file_access(),
// 				 errmsg("could not rename file \"%s\" to \"%s\": %m",
// 						pathtmp, path)));

// 	/*
// 	 * The basename of the file is what we return from pg_export_snapshot().
// 	 * It's already in path in a textual format and we know that the path
// 	 * starts with SNAPSHOT_EXPORT_DIR.  Skip over the prefix and the slash
// 	 * and pstrdup it so as not to return the address of a local variable.
// 	 */
// 	return pstrdup(path + strlen(SNAPSHOT_EXPORT_DIR) + 1);
// }
// Size
// EstimateSnapshotSpace(Snapshot snap)
// {
// 	Size		size;

// 	Assert(snap != InvalidSnapshot);
// 	Assert(snap->satisfies == HeapTupleSatisfiesMVCC);

// 	/* We allocate any XID arrays needed in the same palloc block. */
// 	size = add_size(sizeof(SerializedSnapshotData),
// 					mul_size(snap->xcnt, sizeof(TransactionId)));
// 	if (snap->subxcnt > 0 &&
// 		(!snap->suboverflowed || snap->takenDuringRecovery))
// 		size = add_size(size,
// 						mul_size(snap->subxcnt, sizeof(TransactionId)));

// 	return size;
// }

// void
// SerializeSnapshot(Snapshot snapshot, char *start_address)
// {
// 	SerializedSnapshotData serialized_snapshot;

// 	Assert(snapshot->subxcnt >= 0);

// 	/* Copy all required fields */
// 	serialized_snapshot.xmin = snapshot->xmin;
// 	serialized_snapshot.xmax = snapshot->xmax;
// 	serialized_snapshot.xcnt = snapshot->xcnt;
// 	serialized_snapshot.subxcnt = snapshot->subxcnt;
// 	serialized_snapshot.suboverflowed = snapshot->suboverflowed;
// 	serialized_snapshot.takenDuringRecovery = snapshot->takenDuringRecovery;
// 	serialized_snapshot.curcid = snapshot->curcid;
// 	serialized_snapshot.whenTaken = snapshot->whenTaken;
// 	serialized_snapshot.lsn = snapshot->lsn;

// 	/*
// 	 * Ignore the SubXID array if it has overflowed, unless the snapshot was
// 	 * taken during recovey - in that case, top-level XIDs are in subxip as
// 	 * well, and we mustn't lose them.
// 	 */
// 	if (serialized_snapshot.suboverflowed && !snapshot->takenDuringRecovery)
// 		serialized_snapshot.subxcnt = 0;

// 	/* Copy struct to possibly-unaligned buffer */
// 	memcpy(start_address,
// 		   &serialized_snapshot, sizeof(SerializedSnapshotData));

// 	/* Copy XID array */
// 	if (snapshot->xcnt > 0)
// 		memcpy((TransactionId *) (start_address +
// 								  sizeof(SerializedSnapshotData)),
// 			   snapshot->xip, snapshot->xcnt * sizeof(TransactionId));

// 	/*
// 	 * Copy SubXID array. Don't bother to copy it if it had overflowed,
// 	 * though, because it's not used anywhere in that case. Except if it's a
// 	 * snapshot taken during recovery; all the top-level XIDs are in subxip as
// 	 * well in that case, so we mustn't lose them.
// 	 */
// 	if (serialized_snapshot.subxcnt > 0)
// 	{
// 		Size		subxipoff = sizeof(SerializedSnapshotData) +
// 		snapshot->xcnt * sizeof(TransactionId);

// 		memcpy((TransactionId *) (start_address + subxipoff),
// 			   snapshot->subxip, snapshot->subxcnt * sizeof(TransactionId));
// 	}
// }

// Snapshot
// RestoreSnapshot(char *start_address)
// {
// 	SerializedSnapshotData serialized_snapshot;
// 	Size		size;
// 	Snapshot	snapshot;
// 	TransactionId *serialized_xids;

// 	memcpy(&serialized_snapshot, start_address,
// 		   sizeof(SerializedSnapshotData));
// 	serialized_xids = (TransactionId *)
// 		(start_address + sizeof(SerializedSnapshotData));

// 	/* We allocate any XID arrays needed in the same palloc block. */
// 	size = sizeof(SnapshotData)
// 		+ serialized_snapshot.xcnt * sizeof(TransactionId)
// 		+ serialized_snapshot.subxcnt * sizeof(TransactionId);

// 	/* Copy all required fields */
// 	snapshot = (Snapshot) MemoryContextAlloc(TopTransactionContext, size);
// 	snapshot->satisfies = HeapTupleSatisfiesMVCC;
// 	snapshot->xmin = serialized_snapshot.xmin;
// 	snapshot->xmax = serialized_snapshot.xmax;
// 	snapshot->xip = NULL;
// 	snapshot->xcnt = serialized_snapshot.xcnt;
// 	snapshot->subxip = NULL;
// 	snapshot->subxcnt = serialized_snapshot.subxcnt;
// 	snapshot->suboverflowed = serialized_snapshot.suboverflowed;
// 	snapshot->takenDuringRecovery = serialized_snapshot.takenDuringRecovery;
// 	snapshot->curcid = serialized_snapshot.curcid;
// 	snapshot->whenTaken = serialized_snapshot.whenTaken;
// 	snapshot->lsn = serialized_snapshot.lsn;

// 	/* Copy XIDs, if present. */
// 	if (serialized_snapshot.xcnt > 0)
// 	{
// 		snapshot->xip = (TransactionId *) (snapshot + 1);
// 		memcpy(snapshot->xip, serialized_xids,
// 			   serialized_snapshot.xcnt * sizeof(TransactionId));
// 	}

// 	/* Copy SubXIDs, if present. */
// 	if (serialized_snapshot.subxcnt > 0)
// 	{
// 		snapshot->subxip = ((TransactionId *) (snapshot + 1)) +
// 			serialized_snapshot.xcnt;
// 		memcpy(snapshot->subxip, serialized_xids + serialized_snapshot.xcnt,
// 			   serialized_snapshot.subxcnt * sizeof(TransactionId));
// 	}

// 	/* Set the copied flag so that the caller will set refcounts correctly. */
// 	snapshot->regd_count = 0;
// 	snapshot->active_count = 0;
// 	snapshot->copied = true;

// 	return snapshot;
// }
// void
// RestoreTransactionSnapshot(Snapshot snapshot, void *master_pgproc)
// {
// 	SetTransactionSnapshot(snapshot, InvalidTransactionId, master_pgproc);
// }

// bool
// RelationInvalidatesSnapshotsOnly(Oid relid)
// {
// 	switch (relid)
// 	{
// 		case DbRoleSettingRelationId:
// 		case DependRelationId:
// 		case SharedDependRelationId:
// 		case DescriptionRelationId:
// 		case SharedDescriptionRelationId:
// 		case SecLabelRelationId:
// 		case SharedSecLabelRelationId:
// 			return true;
// 		default:
// 			break;
// 	}

// 	return false;
// }
// bool
// RelationHasSysCache(Oid relid)
// {
// 	int			low = 0,
// 				high = SysCacheRelationOidSize - 1;

// 	while (low <= high)
// 	{
// 		int			middle = low + (high - low) / 2;

// 		if (SysCacheRelationOid[middle] == relid)
// 			return true;
// 		if (SysCacheRelationOid[middle] < relid)
// 			low = middle + 1;
// 		else
// 			high = middle - 1;
// 	}

// 	return false;
// }
// bool
// RelationSupportsSysCache(Oid relid)
// {
// 	int			low = 0,
// 				high = SysCacheSupportingRelOidSize - 1;

// 	while (low <= high)
// 	{
// 		int			middle = low + (high - low) / 2;

// 		if (SysCacheSupportingRelOid[middle] == relid)
// 			return true;
// 		if (SysCacheSupportingRelOid[middle] < relid)
// 			low = middle + 1;
// 		else
// 			high = middle - 1;
// 	}

// 	return false;
// }

// Datum
// make_timestamp(PG_FUNCTION_ARGS)
// {
// 	int32		year = PG_GETARG_INT32(0);
// 	int32		month = PG_GETARG_INT32(1);
// 	int32		mday = PG_GETARG_INT32(2);
// 	int32		hour = PG_GETARG_INT32(3);
// 	int32		min = PG_GETARG_INT32(4);
// 	float8		sec = PG_GETARG_FLOAT8(5);
// 	Timestamp	result;

// 	result = make_timestamp_internal(year, month, mday,
// 									 hour, min, sec);

// 	PG_RETURN_TIMESTAMP(result);
// }
// Datum
// make_timestamptz(PG_FUNCTION_ARGS)
// {
// 	int32		year = PG_GETARG_INT32(0);
// 	int32		month = PG_GETARG_INT32(1);
// 	int32		mday = PG_GETARG_INT32(2);
// 	int32		hour = PG_GETARG_INT32(3);
// 	int32		min = PG_GETARG_INT32(4);
// 	float8		sec = PG_GETARG_FLOAT8(5);
// 	Timestamp	result;

// 	result = make_timestamp_internal(year, month, mday,
// 									 hour, min, sec);

// 	PG_RETURN_TIMESTAMPTZ(timestamp2timestamptz(result));
// }
// Datum
// make_timestamptz_at_timezone(PG_FUNCTION_ARGS)
// {
// 	int32		year = PG_GETARG_INT32(0);
// 	int32		month = PG_GETARG_INT32(1);
// 	int32		mday = PG_GETARG_INT32(2);
// 	int32		hour = PG_GETARG_INT32(3);
// 	int32		min = PG_GETARG_INT32(4);
// 	float8		sec = PG_GETARG_FLOAT8(5);
// 	text	   *zone = PG_GETARG_TEXT_PP(6);
// 	TimestampTz result;
// 	Timestamp	timestamp;
// 	struct pg_tm tt;
// 	int			tz;
// 	fsec_t		fsec;

// 	timestamp = make_timestamp_internal(year, month, mday,
// 										hour, min, sec);

// 	if (timestamp2tm(timestamp, NULL, &tt, &fsec, NULL, NULL) != 0)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
// 				 errmsg("timestamp out of range")));

// 	tz = parse_sane_timezone(&tt, zone);

// 	result = dt2local(timestamp, -tz);

// 	if (!IS_VALID_TIMESTAMP(result))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
// 				 errmsg("timestamp out of range")));

// 	PG_RETURN_TIMESTAMPTZ(result);
// }
// Datum
// float8_timestamptz(PG_FUNCTION_ARGS)
// {
// 	float8		seconds = PG_GETARG_FLOAT8(0);
// 	TimestampTz result;

// 	/* Deal with NaN and infinite inputs ... */
// 	if (isnan(seconds))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
// 				 errmsg("timestamp cannot be NaN")));

// 	if (isinf(seconds))
// 	{
// 		if (seconds < 0)
// 			TIMESTAMP_NOBEGIN(result);
// 		else
// 			TIMESTAMP_NOEND(result);
// 	}
// 	else
// 	{
// 		/* Out of range? */
// 		if (seconds <
// 			(float8) SECS_PER_DAY * (DATETIME_MIN_JULIAN - UNIX_EPOCH_JDATE)
// 			|| seconds >=
// 			(float8) SECS_PER_DAY * (TIMESTAMP_END_JULIAN - UNIX_EPOCH_JDATE))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
// 					 errmsg("timestamp out of range: \"%g\"", seconds)));

// 		/* Convert UNIX epoch to Postgres epoch */
// 		seconds -= ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY);

// #ifdef HAVE_INT64_TIMESTAMP
// 		seconds = rint(seconds * USECS_PER_SEC);
// 		result = (int64) seconds;
// #else
// 		result = seconds;
// #endif

// 		/* Recheck in case roundoff produces something just out of range */
// 		if (!IS_VALID_TIMESTAMP(result))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
// 					 errmsg("timestamp out of range: \"%g\"",
// 							PG_GETARG_FLOAT8(0))));
// 	}

// 	PG_RETURN_TIMESTAMP(result);
// }
// Datum
// make_interval(PG_FUNCTION_ARGS)
// {
// 	int32		years = PG_GETARG_INT32(0);
// 	int32		months = PG_GETARG_INT32(1);
// 	int32		weeks = PG_GETARG_INT32(2);
// 	int32		days = PG_GETARG_INT32(3);
// 	int32		hours = PG_GETARG_INT32(4);
// 	int32		mins = PG_GETARG_INT32(5);
// 	double		secs = PG_GETARG_FLOAT8(6);
// 	Interval   *result;

// 	/*
// 	 * Reject out-of-range inputs.  We really ought to check the integer
// 	 * inputs as well, but it's not entirely clear what limits to apply.
// 	 */
// 	if (isinf(secs) || isnan(secs))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
// 				 errmsg("interval out of range")));

// 	result = (Interval *) palloc(sizeof(Interval));
// 	result->month = years * MONTHS_PER_YEAR + months;
// 	result->day = weeks * 7 + days;

// #ifdef HAVE_INT64_TIMESTAMP
// 	secs = rint(secs * USECS_PER_SEC);
// 	result->time = hours * ((int64) SECS_PER_HOUR * USECS_PER_SEC) +
// 		mins * ((int64) SECS_PER_MINUTE * USECS_PER_SEC) +
// 		(int64) secs;
// #else
// 	result->time = hours * (double) SECS_PER_HOUR +
// 		mins * (double) SECS_PER_MINUTE +
// 		secs;
// #endif

// 	PG_RETURN_INTERVAL_P(result);
// }
// Datum
// timestamp_zone_transform(PG_FUNCTION_ARGS)
// {
// 	PG_RETURN_POINTER(NULL);
// }
// Datum
// timestamp_izone_transform(PG_FUNCTION_ARGS)
// {
// 	PG_RETURN_POINTER(NULL);
// }
// Datum
// interval_combine(PG_FUNCTION_ARGS)
// {
// 	ArrayType  *transarray1 = PG_GETARG_ARRAYTYPE_P(0);
// 	ArrayType  *transarray2 = PG_GETARG_ARRAYTYPE_P(1);
// 	Datum	   *transdatums1;
// 	Datum	   *transdatums2;
// 	int			ndatums1;
// 	int			ndatums2;
// 	Interval	sum1,
// 				N1;
// 	Interval	sum2,
// 				N2;

// 	Interval   *newsum;
// 	ArrayType  *result;

// 	deconstruct_array(transarray1,
// 					  INTERVALOID, sizeof(Interval), false, 'd',
// 					  &transdatums1, NULL, &ndatums1);
// 	if (ndatums1 != 2)
// 		elog(ERROR, "expected 2-element interval array");

// 	sum1 = *(DatumGetIntervalP(transdatums1[0]));
// 	N1 = *(DatumGetIntervalP(transdatums1[1]));

// 	deconstruct_array(transarray2,
// 					  INTERVALOID, sizeof(Interval), false, 'd',
// 					  &transdatums2, NULL, &ndatums2);
// 	if (ndatums2 != 2)
// 		elog(ERROR, "expected 2-element interval array");

// 	sum2 = *(DatumGetIntervalP(transdatums2[0]));
// 	N2 = *(DatumGetIntervalP(transdatums2[1]));

// 	newsum = DatumGetIntervalP(DirectFunctionCall2(interval_pl,
// 												   IntervalPGetDatum(&sum1),
// 												   IntervalPGetDatum(&sum2)));
// 	N1.time += N2.time;

// 	transdatums1[0] = IntervalPGetDatum(newsum);
// 	transdatums1[1] = IntervalPGetDatum(&N1);

// 	result = construct_array(transdatums1, 2,
// 							 INTERVALOID, sizeof(Interval), false, 'd');

// 	PG_RETURN_ARRAYTYPE_P(result);
// }
// Datum
// interval_accum_inv(PG_FUNCTION_ARGS)
// {
// 	ArrayType  *transarray = PG_GETARG_ARRAYTYPE_P(0);
// 	Interval   *newval = PG_GETARG_INTERVAL_P(1);
// 	Datum	   *transdatums;
// 	int			ndatums;
// 	Interval	sumX,
// 				N;
// 	Interval   *newsum;
// 	ArrayType  *result;

// 	deconstruct_array(transarray,
// 					  INTERVALOID, sizeof(Interval), false, 'd',
// 					  &transdatums, NULL, &ndatums);
// 	if (ndatums != 2)
// 		elog(ERROR, "expected 2-element interval array");

// 	sumX = *(DatumGetIntervalP(transdatums[0]));
// 	N = *(DatumGetIntervalP(transdatums[1]));

// 	newsum = DatumGetIntervalP(DirectFunctionCall2(interval_mi,
// 												   IntervalPGetDatum(&sumX),
// 												 IntervalPGetDatum(newval)));
// 	N.time -= 1;

// 	transdatums[0] = IntervalPGetDatum(newsum);
// 	transdatums[1] = IntervalPGetDatum(&N);

// 	result = construct_array(transdatums, 2,
// 							 INTERVALOID, sizeof(Interval), false, 'd');

// 	PG_RETURN_ARRAYTYPE_P(result);
// }
// #ifndef HAVE_INT64_TIMESTAMP
// int64
// GetCurrentIntegerTimestamp(void)
// {
// 	int64		result;
// 	struct timeval tp;

// 	gettimeofday(&tp, NULL);

// 	result = (int64) tp.tv_sec -
// 		((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY);

// 	result = (result * USECS_PER_SEC) + tp.tv_usec;

// 	return result;
// }
// #endif
// #ifndef HAVE_INT64_TIMESTAMP
// TimestampTz
// IntegerTimestampToTimestampTz(int64 timestamp)
// {
// 	TimestampTz result;

// 	result = timestamp / USECS_PER_SEC;
// 	result += (timestamp % USECS_PER_SEC) / 1000000.0;

// 	return result;
// }
// #endif
// void
// InitDomainConstraintRef(Oid type_id, DomainConstraintRef *ref,
// 						MemoryContext refctx)
// {
// 	/* Look up the typcache entry --- we assume it survives indefinitely */
// 	ref->tcache = lookup_type_cache(type_id, TYPECACHE_DOMAIN_INFO);
// 	/* For safety, establish the callback before acquiring a refcount */
// 	ref->refctx = refctx;
// 	ref->dcc = NULL;
// 	ref->callback.func = dccref_deletion_callback;
// 	ref->callback.arg = (void *) ref;
// 	MemoryContextRegisterResetCallback(refctx, &ref->callback);
// 	/* Acquire refcount if there are constraints, and set up exported list */
// 	if (ref->tcache->domainData)
// 	{
// 		ref->dcc = ref->tcache->domainData;
// 		ref->dcc->dccRefCount++;
// 		ref->constraints = prep_domain_constraints(ref->dcc->constraints,
// 												   ref->refctx);
// 	}
// 	else
// 		ref->constraints = NIL;
// }
// void
// UpdateDomainConstraintRef(DomainConstraintRef *ref)
// {
// 	TypeCacheEntry *typentry = ref->tcache;

// 	/* Make sure typcache entry's data is up to date */
// 	if ((typentry->flags & TCFLAGS_CHECKED_DOMAIN_CONSTRAINTS) == 0 &&
// 		typentry->typtype == TYPTYPE_DOMAIN)
// 		load_domaintype_info(typentry);

// 	/* Transfer to ref object if there's new info, adjusting refcounts */
// 	if (ref->dcc != typentry->domainData)
// 	{
// 		/* Paranoia --- be sure link is nulled before trying to release */
// 		DomainConstraintCache *dcc = ref->dcc;

// 		if (dcc)
// 		{
// 			/*
// 			 * Note: we just leak the previous list of executable domain
// 			 * constraints.  Alternatively, we could keep those in a child
// 			 * context of ref->refctx and free that context at this point.
// 			 * However, in practice this code path will be taken so seldom
// 			 * that the extra bookkeeping for a child context doesn't seem
// 			 * worthwhile; we'll just allow a leak for the lifespan of refctx.
// 			 */
// 			ref->constraints = NIL;
// 			ref->dcc = NULL;
// 			decr_dcc_refcount(dcc);
// 		}
// 		dcc = typentry->domainData;
// 		if (dcc)
// 		{
// 			ref->dcc = dcc;
// 			dcc->dccRefCount++;
// 			ref->constraints = prep_domain_constraints(dcc->constraints,
// 													   ref->refctx);
// 		}
// 	}
// }
// bool
// DomainHasConstraints(Oid type_id)
// {
// 	TypeCacheEntry *typentry;

// 	/*
// 	 * Note: a side effect is to cause the typcache's domain data to become
// 	 * valid.  This is fine since we'll likely need it soon if there is any.
// 	 */
// 	typentry = lookup_type_cache(type_id, TYPECACHE_DOMAIN_INFO);

// 	return (typentry->domainData != NULL);
// }

// //third
// //cluster.c
// void
// cluster_rel(Oid tableOid, Oid indexOid, bool recheck, bool verbose)
// {
// 	Relation	OldHeap;

// 	/* Check for user-requested abort. */
// 	CHECK_FOR_INTERRUPTS();

// 	/*
// 	 * We grab exclusive access to the target rel and index for the duration
// 	 * of the transaction.  (This is redundant for the single-transaction
// 	 * case, since cluster() already did it.)  The index lock is taken inside
// 	 * check_index_is_clusterable.
// 	 */
// 	OldHeap = try_relation_open(tableOid, AccessExclusiveLock);

// 	/* If the table has gone away, we can skip processing it */
// 	if (!OldHeap)
// 		return;

// 	/*
// 	 * Since we may open a new transaction for each relation, we have to check
// 	 * that the relation still is what we think it is.
// 	 *
// 	 * If this is a single-transaction CLUSTER, we can skip these tests. We
// 	 * *must* skip the one on indisclustered since it would reject an attempt
// 	 * to cluster a not-previously-clustered index.
// 	 */
// 	if (recheck)
// 	{
// 		HeapTuple	tuple;
// 		Form_pg_index indexForm;

// 		/* Check that the user still owns the relation */
// 		if (!pg_class_ownercheck(tableOid, GetUserId()))
// 		{
// 			relation_close(OldHeap, AccessExclusiveLock);
// 			return;
// 		}

// 		/*
// 		 * Silently skip a temp table for a remote session.  Only doing this
// 		 * check in the "recheck" case is appropriate (which currently means
// 		 * somebody is executing a database-wide CLUSTER), because there is
// 		 * another check in cluster() which will stop any attempt to cluster
// 		 * remote temp tables by name.  There is another check in cluster_rel
// 		 * which is redundant, but we leave it for extra safety.
// 		 */
// 		if (RELATION_IS_OTHER_TEMP(OldHeap))
// 		{
// 			relation_close(OldHeap, AccessExclusiveLock);
// 			return;
// 		}

// 		if (OidIsValid(indexOid))
// 		{
// 			/*
// 			 * Check that the index still exists
// 			 */
// 			if (!SearchSysCacheExists1(RELOID, ObjectIdGetDatum(indexOid)))
// 			{
// 				relation_close(OldHeap, AccessExclusiveLock);
// 				return;
// 			}

// 			/*
// 			 * Check that the index is still the one with indisclustered set.
// 			 */
// 			tuple = SearchSysCache1(INDEXRELID, ObjectIdGetDatum(indexOid));
// 			if (!HeapTupleIsValid(tuple))		/* probably can't happen */
// 			{
// 				relation_close(OldHeap, AccessExclusiveLock);
// 				return;
// 			}
// 			indexForm = (Form_pg_index) GETSTRUCT(tuple);
// 			if (!indexForm->indisclustered)
// 			{
// 				ReleaseSysCache(tuple);
// 				relation_close(OldHeap, AccessExclusiveLock);
// 				return;
// 			}
// 			ReleaseSysCache(tuple);
// 		}
// 	}

// 	/*
// 	 * We allow VACUUM FULL, but not CLUSTER, on shared catalogs.  CLUSTER
// 	 * would work in most respects, but the index would only get marked as
// 	 * indisclustered in the current database, leading to unexpected behavior
// 	 * if CLUSTER were later invoked in another database.
// 	 */
// 	if (OidIsValid(indexOid) && OldHeap->rd_rel->relisshared)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("cannot cluster a shared catalog")));

// 	/*
// 	 * Don't process temp tables of other backends ... their local buffer
// 	 * manager is not going to cope.
// 	 */
// 	if (RELATION_IS_OTHER_TEMP(OldHeap))
// 	{
// 		if (OidIsValid(indexOid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 			   errmsg("cannot cluster temporary tables of other sessions")));
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				errmsg("cannot vacuum temporary tables of other sessions")));
// 	}

// 	/*
// 	 * Also check for active uses of the relation in the current transaction,
// 	 * including open scans and pending AFTER trigger events.
// 	 */
// 	CheckTableNotInUse(OldHeap, OidIsValid(indexOid) ? "CLUSTER" : "VACUUM");

// 	/* Check heap and index are valid to cluster on */
// 	if (OidIsValid(indexOid))
// 		check_index_is_clusterable(OldHeap, indexOid, recheck, AccessExclusiveLock);

// 	/*
// 	 * Quietly ignore the request if this is a materialized view which has not
// 	 * been populated from its query. No harm is done because there is no data
// 	 * to deal with, and we don't want to throw an error if this is part of a
// 	 * multi-relation request -- for example, CLUSTER was run on the entire
// 	 * database.
// 	 */
// 	if (OldHeap->rd_rel->relkind == RELKIND_MATVIEW &&
// 		!RelationIsPopulated(OldHeap))
// 	{
// 		relation_close(OldHeap, AccessExclusiveLock);
// 		return;
// 	}

// 	/*
// 	 * All predicate locks on the tuples or pages are about to be made
// 	 * invalid, because we move tuples around.  Promote them to relation
// 	 * locks.  Predicate locks on indexes will be promoted when they are
// 	 * reindexed.
// 	 */
// 	TransferPredicateLocksToHeapRelation(OldHeap);

// 	/* rebuild_relation does all the dirty work */
// 	rebuild_relation(OldHeap, indexOid, verbose);

// 	/* NB: rebuild_relation does heap_close() on OldHeap */
// }
// //end cluster_rel

// void
// check_index_is_clusterable(Relation OldHeap, Oid indexOid, bool recheck, LOCKMODE lockmode)
// {
// 	Relation	OldIndex;

// 	OldIndex = index_open(indexOid, lockmode);

// 	/*
// 	 * Check that index is in fact an index on the given relation
// 	 */
// 	if (OldIndex->rd_index == NULL ||
// 		OldIndex->rd_index->indrelid != RelationGetRelid(OldHeap))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("\"%s\" is not an index for table \"%s\"",
// 						RelationGetRelationName(OldIndex),
// 						RelationGetRelationName(OldHeap))));

// 	/* Index AM must allow clustering */
// 	if (!OldIndex->rd_amroutine->amclusterable)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("cannot cluster on index \"%s\" because access method does not support clustering",
// 						RelationGetRelationName(OldIndex))));

// 	/*
// 	 * Disallow clustering on incomplete indexes (those that might not index
// 	 * every row of the relation).  We could relax this by making a separate
// 	 * seqscan pass over the table to copy the missing rows, but that seems
// 	 * expensive and tedious.
// 	 */
// 	if (!heap_attisnull(OldIndex->rd_indextuple, Anum_pg_index_indpred))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("cannot cluster on partial index \"%s\"",
// 						RelationGetRelationName(OldIndex))));

// 	/*
// 	 * Disallow if index is left over from a failed CREATE INDEX CONCURRENTLY;
// 	 * it might well not contain entries for every heap row, or might not even
// 	 * be internally consistent.  (But note that we don't check indcheckxmin;
// 	 * the worst consequence of following broken HOT chains would be that we
// 	 * might put recently-dead tuples out-of-order in the new table, and there
// 	 * is little harm in that.)
// 	 */
// 	if (!IndexIsValid(OldIndex->rd_index))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("cannot cluster on invalid index \"%s\"",
// 						RelationGetRelationName(OldIndex))));

// 	/* Drop relcache refcnt on OldIndex, but keep lock */
// 	index_close(OldIndex, NoLock);
// }
// //end check_index_is_clusterable

// void
// mark_index_clustered(Relation rel, Oid indexOid, bool is_internal)
// {
// 	HeapTuple	indexTuple;
// 	Form_pg_index indexForm;
// 	Relation	pg_index;
// 	ListCell   *index;

// 	/*
// 	 * If the index is already marked clustered, no need to do anything.
// 	 */
// 	if (OidIsValid(indexOid))
// 	{
// 		indexTuple = SearchSysCache1(INDEXRELID, ObjectIdGetDatum(indexOid));
// 		if (!HeapTupleIsValid(indexTuple))
// 			elog(ERROR, "cache lookup failed for index %u", indexOid);
// 		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

// 		if (indexForm->indisclustered)
// 		{
// 			ReleaseSysCache(indexTuple);
// 			return;
// 		}

// 		ReleaseSysCache(indexTuple);
// 	}

// 	/*
// 	 * Check each index of the relation and set/clear the bit as needed.
// 	 */
// 	pg_index = heap_open(IndexRelationId, RowExclusiveLock);

// 	foreach(index, RelationGetIndexList(rel))
// 	{
// 		Oid			thisIndexOid = lfirst_oid(index);

// 		indexTuple = SearchSysCacheCopy1(INDEXRELID,
// 										 ObjectIdGetDatum(thisIndexOid));
// 		if (!HeapTupleIsValid(indexTuple))
// 			elog(ERROR, "cache lookup failed for index %u", thisIndexOid);
// 		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

// 		/*
// 		 * Unset the bit if set.  We know it's wrong because we checked this
// 		 * earlier.
// 		 */
// 		if (indexForm->indisclustered)
// 		{
// 			indexForm->indisclustered = false;
// 			simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
// 			CatalogUpdateIndexes(pg_index, indexTuple);
// 		}
// 		else if (thisIndexOid == indexOid)
// 		{
// 			/* this was checked earlier, but let's be real sure */
// 			if (!IndexIsValid(indexForm))
// 				elog(ERROR, "cannot cluster on invalid index %u", indexOid);
// 			indexForm->indisclustered = true;
// 			simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
// 			CatalogUpdateIndexes(pg_index, indexTuple);
// 		}

// 		InvokeObjectPostAlterHookArg(IndexRelationId, thisIndexOid, 0,
// 									 InvalidOid, is_internal);

// 		heap_freetuple(indexTuple);
// 	}

// 	heap_close(pg_index, RowExclusiveLock);
// }
// //end mark_index_clustered

// Oid
// make_new_heap(Oid OIDOldHeap, Oid NewTableSpace, char relpersistence,
// 			  LOCKMODE lockmode)
// {
// 	TupleDesc	OldHeapDesc;
// 	char		NewHeapName[NAMEDATALEN];
// 	Oid			OIDNewHeap;
// 	Oid			toastid;
// 	Relation	OldHeap;
// 	HeapTuple	tuple;
// 	Datum		reloptions;
// 	bool		isNull;
// 	Oid			namespaceid;

// 	OldHeap = heap_open(OIDOldHeap, lockmode);
// 	OldHeapDesc = RelationGetDescr(OldHeap);

// 	/*
// 	 * Note that the NewHeap will not receive any of the defaults or
// 	 * constraints associated with the OldHeap; we don't need 'em, and there's
// 	 * no reason to spend cycles inserting them into the catalogs only to
// 	 * delete them.
// 	 */

// 	/*
// 	 * But we do want to use reloptions of the old heap for new heap.
// 	 */
// 	tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(OIDOldHeap));
// 	if (!HeapTupleIsValid(tuple))
// 		elog(ERROR, "cache lookup failed for relation %u", OIDOldHeap);
// 	reloptions = SysCacheGetAttr(RELOID, tuple, Anum_pg_class_reloptions,
// 								 &isNull);
// 	if (isNull)
// 		reloptions = (Datum) 0;

// 	if (relpersistence == RELPERSISTENCE_TEMP)
// 		namespaceid = LookupCreationNamespace("pg_temp");
// 	else
// 		namespaceid = RelationGetNamespace(OldHeap);

// 	/*
// 	 * Create the new heap, using a temporary name in the same namespace as
// 	 * the existing table.  NOTE: there is some risk of collision with user
// 	 * relnames.  Working around this seems more trouble than it's worth; in
// 	 * particular, we can't create the new heap in a different namespace from
// 	 * the old, or we will have problems with the TEMP status of temp tables.
// 	 *
// 	 * Note: the new heap is not a shared relation, even if we are rebuilding
// 	 * a shared rel.  However, we do make the new heap mapped if the source is
// 	 * mapped.  This simplifies swap_relation_files, and is absolutely
// 	 * necessary for rebuilding pg_class, for reasons explained there.
// 	 */
// 	snprintf(NewHeapName, sizeof(NewHeapName), "pg_temp_%u", OIDOldHeap);

// 	OIDNewHeap = heap_create_with_catalog(NewHeapName,
// 										  namespaceid,
// 										  NewTableSpace,
// 										  InvalidOid,
// 										  InvalidOid,
// 										  InvalidOid,
// 										  OldHeap->rd_rel->relowner,
// 										  OldHeapDesc,
// 										  NIL,
// 										  RELKIND_RELATION,
// 										  relpersistence,
// 										  false,
// 										  RelationIsMapped(OldHeap),
// 										  true,
// 										  0,
// 										  ONCOMMIT_NOOP,
// 										  reloptions,
// 										  false,
// 										  true,
// 										  true,
// 										  NULL);
// 	Assert(OIDNewHeap != InvalidOid);

// 	ReleaseSysCache(tuple);

// 	/*
// 	 * Advance command counter so that the newly-created relation's catalog
// 	 * tuples will be visible to heap_open.
// 	 */
// 	CommandCounterIncrement();

// 	/*
// 	 * If necessary, create a TOAST table for the new relation.
// 	 *
// 	 * If the relation doesn't have a TOAST table already, we can't need one
// 	 * for the new relation.  The other way around is possible though: if some
// 	 * wide columns have been dropped, NewHeapCreateToastTable can decide that
// 	 * no TOAST table is needed for the new table.
// 	 *
// 	 * Note that NewHeapCreateToastTable ends with CommandCounterIncrement, so
// 	 * that the TOAST table will be visible for insertion.
// 	 */
// 	toastid = OldHeap->rd_rel->reltoastrelid;
// 	if (OidIsValid(toastid))
// 	{
// 		/* keep the existing toast table's reloptions, if any */
// 		tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(toastid));
// 		if (!HeapTupleIsValid(tuple))
// 			elog(ERROR, "cache lookup failed for relation %u", toastid);
// 		reloptions = SysCacheGetAttr(RELOID, tuple, Anum_pg_class_reloptions,
// 									 &isNull);
// 		if (isNull)
// 			reloptions = (Datum) 0;

// 		NewHeapCreateToastTable(OIDNewHeap, reloptions, lockmode);

// 		ReleaseSysCache(tuple);
// 	}

// 	heap_close(OldHeap, NoLock);

// 	return OIDNewHeap;
// }
// //end make_new_heap

// void
// finish_heap_swap(Oid OIDOldHeap, Oid OIDNewHeap,
// 				 bool is_system_catalog,
// 				 bool swap_toast_by_content,
// 				 bool check_constraints,
// 				 bool is_internal,
// 				 TransactionId frozenXid,
// 				 MultiXactId cutoffMulti,
// 				 char newrelpersistence)
// {
// 	ObjectAddress object;
// 	Oid			mapped_tables[4];
// 	int			reindex_flags;
// 	int			i;

// 	/* Zero out possible results from swapped_relation_files */
// 	memset(mapped_tables, 0, sizeof(mapped_tables));

// 	/*
// 	 * Swap the contents of the heap relations (including any toast tables).
// 	 * Also set old heap's relfrozenxid to frozenXid.
// 	 */
// 	swap_relation_files(OIDOldHeap, OIDNewHeap,
// 						(OIDOldHeap == RelationRelationId),
// 						swap_toast_by_content, is_internal,
// 						frozenXid, cutoffMulti, mapped_tables);

// 	/*
// 	 * If it's a system catalog, queue an sinval message to flush all
// 	 * catcaches on the catalog when we reach CommandCounterIncrement.
// 	 */
// 	if (is_system_catalog)
// 		CacheInvalidateCatalog(OIDOldHeap);

// 	/*
// 	 * Rebuild each index on the relation (but not the toast table, which is
// 	 * all-new at this point).  It is important to do this before the DROP
// 	 * step because if we are processing a system catalog that will be used
// 	 * during DROP, we want to have its indexes available.  There is no
// 	 * advantage to the other order anyway because this is all transactional,
// 	 * so no chance to reclaim disk space before commit.  We do not need a
// 	 * final CommandCounterIncrement() because reindex_relation does it.
// 	 *
// 	 * Note: because index_build is called via reindex_relation, it will never
// 	 * set indcheckxmin true for the indexes.  This is OK even though in some
// 	 * sense we are building new indexes rather than rebuilding existing ones,
// 	 * because the new heap won't contain any HOT chains at all, let alone
// 	 * broken ones, so it can't be necessary to set indcheckxmin.
// 	 */
// 	reindex_flags = REINDEX_REL_SUPPRESS_INDEX_USE;
// 	if (check_constraints)
// 		reindex_flags |= REINDEX_REL_CHECK_CONSTRAINTS;

// 	/*
// 	 * Ensure that the indexes have the same persistence as the parent
// 	 * relation.
// 	 */
// 	if (newrelpersistence == RELPERSISTENCE_UNLOGGED)
// 		reindex_flags |= REINDEX_REL_FORCE_INDEXES_UNLOGGED;
// 	else if (newrelpersistence == RELPERSISTENCE_PERMANENT)
// 		reindex_flags |= REINDEX_REL_FORCE_INDEXES_PERMANENT;

// 	reindex_relation(OIDOldHeap, reindex_flags, 0);

// 	/*
// 	 * If the relation being rebuild is pg_class, swap_relation_files()
// 	 * couldn't update pg_class's own pg_class entry (check comments in
// 	 * swap_relation_files()), thus relfrozenxid was not updated. That's
// 	 * annoying because a potential reason for doing a VACUUM FULL is a
// 	 * imminent or actual anti-wraparound shutdown.  So, now that we can
// 	 * access the new relation using it's indices, update relfrozenxid.
// 	 * pg_class doesn't have a toast relation, so we don't need to update the
// 	 * corresponding toast relation. Not that there's little point moving all
// 	 * relfrozenxid updates here since swap_relation_files() needs to write to
// 	 * pg_class for non-mapped relations anyway.
// 	 */
// 	if (OIDOldHeap == RelationRelationId)
// 	{
// 		Relation	relRelation;
// 		HeapTuple	reltup;
// 		Form_pg_class relform;

// 		relRelation = heap_open(RelationRelationId, RowExclusiveLock);

// 		reltup = SearchSysCacheCopy1(RELOID, ObjectIdGetDatum(OIDOldHeap));
// 		if (!HeapTupleIsValid(reltup))
// 			elog(ERROR, "cache lookup failed for relation %u", OIDOldHeap);
// 		relform = (Form_pg_class) GETSTRUCT(reltup);

// 		relform->relfrozenxid = frozenXid;
// 		relform->relminmxid = cutoffMulti;

// 		simple_heap_update(relRelation, &reltup->t_self, reltup);
// 		CatalogUpdateIndexes(relRelation, reltup);

// 		heap_close(relRelation, RowExclusiveLock);
// 	}

// 	/* Destroy new heap with old filenode */
// 	object.classId = RelationRelationId;
// 	object.objectId = OIDNewHeap;
// 	object.objectSubId = 0;

// 	/*
// 	 * The new relation is local to our transaction and we know nothing
// 	 * depends on it, so DROP_RESTRICT should be OK.
// 	 */
// 	performDeletion(&object, DROP_RESTRICT, PERFORM_DELETION_INTERNAL);

// 	/* performDeletion does CommandCounterIncrement at end */

// 	/*
// 	 * Now we must remove any relation mapping entries that we set up for the
// 	 * transient table, as well as its toast table and toast index if any. If
// 	 * we fail to do this before commit, the relmapper will complain about new
// 	 * permanent map entries being added post-bootstrap.
// 	 */
// 	for (i = 0; OidIsValid(mapped_tables[i]); i++)
// 		RelationMapRemoveMapping(mapped_tables[i]);

// 	/*
// 	 * At this point, everything is kosher except that, if we did toast swap
// 	 * by links, the toast table's name corresponds to the transient table.
// 	 * The name is irrelevant to the backend because it's referenced by OID,
// 	 * but users looking at the catalogs could be confused.  Rename it to
// 	 * prevent this problem.
// 	 *
// 	 * Note no lock required on the relation, because we already hold an
// 	 * exclusive lock on it.
// 	 */
// 	if (!swap_toast_by_content)
// 	{
// 		Relation	newrel;

// 		newrel = heap_open(OIDOldHeap, NoLock);
// 		if (OidIsValid(newrel->rd_rel->reltoastrelid))
// 		{
// 			Oid			toastidx;
// 			char		NewToastName[NAMEDATALEN];

// 			/* Get the associated valid index to be renamed */
// 			toastidx = toast_get_valid_index(newrel->rd_rel->reltoastrelid,
// 											 AccessShareLock);

// 			/* rename the toast table ... */
// 			snprintf(NewToastName, NAMEDATALEN, "pg_toast_%u",
// 					 OIDOldHeap);
// 			RenameRelationInternal(newrel->rd_rel->reltoastrelid,
// 								   NewToastName, true);

// 			/* ... and its valid index too. */
// 			snprintf(NewToastName, NAMEDATALEN, "pg_toast_%u_index",
// 					 OIDOldHeap);

// 			RenameRelationInternal(toastidx,
// 								   NewToastName, true);
// 		}
// 		relation_close(newrel, NoLock);
// 	}
// }
// //end finish_heap_swap



// //dbcommands.c
// ObjectAddress
// RenameDatabase(const char *oldname, const char *newname)
// {
// 	Oid			db_id;
// 	HeapTuple	newtup;
// 	Relation	rel;
// 	int			notherbackends;
// 	int			npreparedxacts;
// 	ObjectAddress address;

// 	/*
// 	 * Look up the target database's OID, and get exclusive lock on it. We
// 	 * need this for the same reasons as DROP DATABASE.
// 	 */
// 	rel = heap_open(DatabaseRelationId, RowExclusiveLock);

// 	if (!get_db_info(oldname, AccessExclusiveLock, &db_id, NULL, NULL,
// 					 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_DATABASE),
// 				 errmsg("database \"%s\" does not exist", oldname)));

// 	/* must be owner */
// 	if (!pg_database_ownercheck(db_id, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_DATABASE,
// 					   oldname);

// 	/* must have createdb rights */
// 	if (!have_createdb_privilege())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("permission denied to rename database")));

// 	/*
// 	 * Make sure the new name doesn't exist.  See notes for same error in
// 	 * CREATE DATABASE.
// 	 */
// 	if (OidIsValid(get_database_oid(newname, true)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_DATABASE),
// 				 errmsg("database \"%s\" already exists", newname)));

// 	/*
// 	 * XXX Client applications probably store the current database somewhere,
// 	 * so renaming it could cause confusion.  On the other hand, there may not
// 	 * be an actual problem besides a little confusion, so think about this
// 	 * and decide.
// 	 */
// 	if (db_id == MyDatabaseId)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("current database cannot be renamed")));

// 	/*
// 	 * Make sure the database does not have active sessions.  This is the same
// 	 * concern as above, but applied to other sessions.
// 	 *
// 	 * As in CREATE DATABASE, check this after other error conditions.
// 	 */
// 	if (CountOtherDBBackends(db_id, &notherbackends, &npreparedxacts))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_IN_USE),
// 				 errmsg("database \"%s\" is being accessed by other users",
// 						oldname),
// 				 errdetail_busy_db(notherbackends, npreparedxacts)));

// 	/* rename */
// 	newtup = SearchSysCacheCopy1(DATABASEOID, ObjectIdGetDatum(db_id));
// 	if (!HeapTupleIsValid(newtup))
// 		elog(ERROR, "cache lookup failed for database %u", db_id);
// 	namestrcpy(&(((Form_pg_database) GETSTRUCT(newtup))->datname), newname);
// 	simple_heap_update(rel, &newtup->t_self, newtup);
// 	CatalogUpdateIndexes(rel, newtup);

// 	InvokeObjectPostAlterHook(DatabaseRelationId, db_id, 0);

// 	ObjectAddressSet(address, DatabaseRelationId, db_id);

// 	/*
// 	 * Close pg_database, but keep lock till commit.
// 	 */
// 	heap_close(rel, NoLock);

// 	return address;
// }
// //end RenameDatabase

// ObjectAddress
// AlterDatabaseOwner(const char *dbname, Oid newOwnerId)
// {
// 	Oid			db_id;
// 	HeapTuple	tuple;
// 	Relation	rel;
// 	ScanKeyData scankey;
// 	SysScanDesc scan;
// 	Form_pg_database datForm;
// 	ObjectAddress address;

// 	/*
// 	 * Get the old tuple.  We don't need a lock on the database per se,
// 	 * because we're not going to do anything that would mess up incoming
// 	 * connections.
// 	 */
// 	rel = heap_open(DatabaseRelationId, RowExclusiveLock);
// 	ScanKeyInit(&scankey,
// 				Anum_pg_database_datname,
// 				BTEqualStrategyNumber, F_NAMEEQ,
// 				NameGetDatum(dbname));
// 	scan = systable_beginscan(rel, DatabaseNameIndexId, true,
// 							  NULL, 1, &scankey);
// 	tuple = systable_getnext(scan);
// 	if (!HeapTupleIsValid(tuple))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_DATABASE),
// 				 errmsg("database \"%s\" does not exist", dbname)));

// 	db_id = HeapTupleGetOid(tuple);
// 	datForm = (Form_pg_database) GETSTRUCT(tuple);

// 	/*
// 	 * If the new owner is the same as the existing owner, consider the
// 	 * command to have succeeded.  This is to be consistent with other
// 	 * objects.
// 	 */
// 	if (datForm->datdba != newOwnerId)
// 	{
// 		Datum		repl_val[Natts_pg_database];
// 		bool		repl_null[Natts_pg_database];
// 		bool		repl_repl[Natts_pg_database];
// 		Acl		   *newAcl;
// 		Datum		aclDatum;
// 		bool		isNull;
// 		HeapTuple	newtuple;

// 		/* Otherwise, must be owner of the existing object */
// 		if (!pg_database_ownercheck(HeapTupleGetOid(tuple), GetUserId()))
// 			aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_DATABASE,
// 						   dbname);

// 		/* Must be able to become new owner */
// 		check_is_member_of_role(GetUserId(), newOwnerId);

// 		/*
// 		 * must have createdb rights
// 		 *
// 		 * NOTE: This is different from other alter-owner checks in that the
// 		 * current user is checked for createdb privileges instead of the
// 		 * destination owner.  This is consistent with the CREATE case for
// 		 * databases.  Because superusers will always have this right, we need
// 		 * no special case for them.
// 		 */
// 		if (!have_createdb_privilege())
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				   errmsg("permission denied to change owner of database")));

// 		memset(repl_null, false, sizeof(repl_null));
// 		memset(repl_repl, false, sizeof(repl_repl));

// 		repl_repl[Anum_pg_database_datdba - 1] = true;
// 		repl_val[Anum_pg_database_datdba - 1] = ObjectIdGetDatum(newOwnerId);

// 		/*
// 		 * Determine the modified ACL for the new owner.  This is only
// 		 * necessary when the ACL is non-null.
// 		 */
// 		aclDatum = heap_getattr(tuple,
// 								Anum_pg_database_datacl,
// 								RelationGetDescr(rel),
// 								&isNull);
// 		if (!isNull)
// 		{
// 			newAcl = aclnewowner(DatumGetAclP(aclDatum),
// 								 datForm->datdba, newOwnerId);
// 			repl_repl[Anum_pg_database_datacl - 1] = true;
// 			repl_val[Anum_pg_database_datacl - 1] = PointerGetDatum(newAcl);
// 		}

// 		newtuple = heap_modify_tuple(tuple, RelationGetDescr(rel), repl_val, repl_null, repl_repl);
// 		simple_heap_update(rel, &newtuple->t_self, newtuple);
// 		CatalogUpdateIndexes(rel, newtuple);

// 		heap_freetuple(newtuple);

// 		/* Update owner dependency reference */
// 		changeDependencyOnOwner(DatabaseRelationId, HeapTupleGetOid(tuple),
// 								newOwnerId);
// 	}

// 	InvokeObjectPostAlterHook(DatabaseRelationId, HeapTupleGetOid(tuple), 0);

// 	ObjectAddressSet(address, DatabaseRelationId, db_id);

// 	systable_endscan(scan);

// 	/* Close pg_database, but keep lock till commit */
// 	heap_close(rel, NoLock);

// 	return address;
// }
// //end AlterDatabaseOwner

// Oid
// createdb(const CreatedbStmt *stmt)
// {
// 	HeapScanDesc scan;
// 	Relation	rel;
// 	Oid			src_dboid;
// 	Oid			src_owner;
// 	int			src_encoding;
// 	char	   *src_collate;
// 	char	   *src_ctype;
// 	bool		src_istemplate;
// 	bool		src_allowconn;
// 	Oid			src_lastsysoid;
// 	TransactionId src_frozenxid;
// 	MultiXactId src_minmxid;
// 	Oid			src_deftablespace;
// 	volatile Oid dst_deftablespace;
// 	Relation	pg_database_rel;
// 	HeapTuple	tuple;
// 	Datum		new_record[Natts_pg_database];
// 	bool		new_record_nulls[Natts_pg_database];
// 	Oid			dboid;
// 	Oid			datdba;
// 	ListCell   *option;
// 	DefElem    *dtablespacename = NULL;
// 	DefElem    *downer = NULL;
// 	DefElem    *dtemplate = NULL;
// 	DefElem    *dencoding = NULL;
// 	DefElem    *dcollate = NULL;
// 	DefElem    *dctype = NULL;
// 	DefElem    *distemplate = NULL;
// 	DefElem    *dallowconnections = NULL;
// 	DefElem    *dconnlimit = NULL;
// 	char	   *dbname = stmt->dbname;
// 	char	   *dbowner = NULL;
// 	const char *dbtemplate = NULL;
// 	char	   *dbcollate = NULL;
// 	char	   *dbctype = NULL;
// 	char	   *canonname;
// 	int			encoding = -1;
// 	bool		dbistemplate = false;
// 	bool		dballowconnections = true;
// 	int			dbconnlimit = -1;
// 	int			notherbackends;
// 	int			npreparedxacts;
// 	createdb_failure_params fparms;

// 	/* Extract options from the statement node tree */
// 	foreach(option, stmt->options)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(option);

// 		if (strcmp(defel->defname, "tablespace") == 0)
// 		{
// 			if (dtablespacename)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dtablespacename = defel;
// 		}
// 		else if (strcmp(defel->defname, "owner") == 0)
// 		{
// 			if (downer)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			downer = defel;
// 		}
// 		else if (strcmp(defel->defname, "template") == 0)
// 		{
// 			if (dtemplate)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dtemplate = defel;
// 		}
// 		else if (strcmp(defel->defname, "encoding") == 0)
// 		{
// 			if (dencoding)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dencoding = defel;
// 		}
// 		else if (strcmp(defel->defname, "lc_collate") == 0)
// 		{
// 			if (dcollate)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dcollate = defel;
// 		}
// 		else if (strcmp(defel->defname, "lc_ctype") == 0)
// 		{
// 			if (dctype)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dctype = defel;
// 		}
// 		else if (strcmp(defel->defname, "is_template") == 0)
// 		{
// 			if (distemplate)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			distemplate = defel;
// 		}
// 		else if (strcmp(defel->defname, "allow_connections") == 0)
// 		{
// 			if (dallowconnections)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dallowconnections = defel;
// 		}
// 		else if (strcmp(defel->defname, "connection_limit") == 0)
// 		{
// 			if (dconnlimit)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dconnlimit = defel;
// 		}
// 		else if (strcmp(defel->defname, "location") == 0)
// 		{
// 			ereport(WARNING,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("LOCATION is not supported anymore"),
// 					 errhint("Consider using tablespaces instead.")));
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("option \"%s\" not recognized", defel->defname)));
// 	}

// 	if (downer && downer->arg)
// 		dbowner = defGetString(downer);
// 	if (dtemplate && dtemplate->arg)
// 		dbtemplate = defGetString(dtemplate);
// 	if (dencoding && dencoding->arg)
// 	{
// 		const char *encoding_name;

// 		if (IsA(dencoding->arg, Integer))
// 		{
// 			encoding = defGetInt32(dencoding);
// 			encoding_name = pg_encoding_to_char(encoding);
// 			if (strcmp(encoding_name, "") == 0 ||
// 				pg_valid_server_encoding(encoding_name) < 0)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_UNDEFINED_OBJECT),
// 						 errmsg("%d is not a valid encoding code",
// 								encoding)));
// 		}
// 		else
// 		{
// 			encoding_name = defGetString(dencoding);
// 			encoding = pg_valid_server_encoding(encoding_name);
// 			if (encoding < 0)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_UNDEFINED_OBJECT),
// 						 errmsg("%s is not a valid encoding name",
// 								encoding_name)));
// 		}
// 	}
// 	if (dcollate && dcollate->arg)
// 		dbcollate = defGetString(dcollate);
// 	if (dctype && dctype->arg)
// 		dbctype = defGetString(dctype);
// 	if (distemplate && distemplate->arg)
// 		dbistemplate = defGetBoolean(distemplate);
// 	if (dallowconnections && dallowconnections->arg)
// 		dballowconnections = defGetBoolean(dallowconnections);
// 	if (dconnlimit && dconnlimit->arg)
// 	{
// 		dbconnlimit = defGetInt32(dconnlimit);
// 		if (dbconnlimit < -1)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("invalid connection limit: %d", dbconnlimit)));
// 	}

// 	/* obtain OID of proposed owner */
// 	if (dbowner)
// 		datdba = get_role_oid(dbowner, false);
// 	else
// 		datdba = GetUserId();

// 	/*
// 	 * To create a database, must have createdb privilege and must be able to
// 	 * become the target role (this does not imply that the target role itself
// 	 * must have createdb privilege).  The latter provision guards against
// 	 * "giveaway" attacks.  Note that a superuser will always have both of
// 	 * these privileges a fortiori.
// 	 */
// 	if (!have_createdb_privilege())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("permission denied to create database")));

// 	check_is_member_of_role(GetUserId(), datdba);

// 	/*
// 	 * Lookup database (template) to be cloned, and obtain share lock on it.
// 	 * ShareLock allows two CREATE DATABASEs to work from the same template
// 	 * concurrently, while ensuring no one is busy dropping it in parallel
// 	 * (which would be Very Bad since we'd likely get an incomplete copy
// 	 * without knowing it).  This also prevents any new connections from being
// 	 * made to the source until we finish copying it, so we can be sure it
// 	 * won't change underneath us.
// 	 */
// 	if (!dbtemplate)
// 		dbtemplate = "template1";		/* Default template database name */

// 	if (!get_db_info(dbtemplate, ShareLock,
// 					 &src_dboid, &src_owner, &src_encoding,
// 					 &src_istemplate, &src_allowconn, &src_lastsysoid,
// 					 &src_frozenxid, &src_minmxid, &src_deftablespace,
// 					 &src_collate, &src_ctype))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_DATABASE),
// 				 errmsg("template database \"%s\" does not exist",
// 						dbtemplate)));

// 	/*
// 	 * Permission check: to copy a DB that's not marked datistemplate, you
// 	 * must be superuser or the owner thereof.
// 	 */
// 	if (!src_istemplate)
// 	{
// 		if (!pg_database_ownercheck(src_dboid, GetUserId()))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 					 errmsg("permission denied to copy database \"%s\"",
// 							dbtemplate)));
// 	}

// 	/* If encoding or locales are defaulted, use source's setting */
// 	if (encoding < 0)
// 		encoding = src_encoding;
// 	if (dbcollate == NULL)
// 		dbcollate = src_collate;
// 	if (dbctype == NULL)
// 		dbctype = src_ctype;

// 	/* Some encodings are client only */
// 	if (!PG_VALID_BE_ENCODING(encoding))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("invalid server encoding %d", encoding)));

// 	/* Check that the chosen locales are valid, and get canonical spellings */
// 	if (!check_locale(LC_COLLATE, dbcollate, &canonname))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("invalid locale name: \"%s\"", dbcollate)));
// 	dbcollate = canonname;
// 	if (!check_locale(LC_CTYPE, dbctype, &canonname))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("invalid locale name: \"%s\"", dbctype)));
// 	dbctype = canonname;

// 	check_encoding_locale_matches(encoding, dbcollate, dbctype);

// 	/*
// 	 * Check that the new encoding and locale settings match the source
// 	 * database.  We insist on this because we simply copy the source data ---
// 	 * any non-ASCII data would be wrongly encoded, and any indexes sorted
// 	 * according to the source locale would be wrong.
// 	 *
// 	 * However, we assume that template0 doesn't contain any non-ASCII data
// 	 * nor any indexes that depend on collation or ctype, so template0 can be
// 	 * used as template for creating a database with any encoding or locale.
// 	 */
// 	if (strcmp(dbtemplate, "template0") != 0)
// 	{
// 		if (encoding != src_encoding)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("new encoding (%s) is incompatible with the encoding of the template database (%s)",
// 							pg_encoding_to_char(encoding),
// 							pg_encoding_to_char(src_encoding)),
// 					 errhint("Use the same encoding as in the template database, or use template0 as template.")));

// 		if (strcmp(dbcollate, src_collate) != 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("new collation (%s) is incompatible with the collation of the template database (%s)",
// 							dbcollate, src_collate),
// 					 errhint("Use the same collation as in the template database, or use template0 as template.")));

// 		if (strcmp(dbctype, src_ctype) != 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("new LC_CTYPE (%s) is incompatible with the LC_CTYPE of the template database (%s)",
// 							dbctype, src_ctype),
// 					 errhint("Use the same LC_CTYPE as in the template database, or use template0 as template.")));
// 	}

// 	/* Resolve default tablespace for new database */
// 	if (dtablespacename && dtablespacename->arg)
// 	{
// 		char	   *tablespacename;
// 		AclResult	aclresult;

// 		tablespacename = defGetString(dtablespacename);
// 		dst_deftablespace = get_tablespace_oid(tablespacename, false);
// 		/* check permissions */
// 		aclresult = pg_tablespace_aclcheck(dst_deftablespace, GetUserId(),
// 										   ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_TABLESPACE,
// 						   tablespacename);

// 		/* pg_global must never be the default tablespace */
// 		if (dst_deftablespace == GLOBALTABLESPACE_OID)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				  errmsg("pg_global cannot be used as default tablespace")));

// 		/*
// 		 * If we are trying to change the default tablespace of the template,
// 		 * we require that the template not have any files in the new default
// 		 * tablespace.  This is necessary because otherwise the copied
// 		 * database would contain pg_class rows that refer to its default
// 		 * tablespace both explicitly (by OID) and implicitly (as zero), which
// 		 * would cause problems.  For example another CREATE DATABASE using
// 		 * the copied database as template, and trying to change its default
// 		 * tablespace again, would yield outright incorrect results (it would
// 		 * improperly move tables to the new default tablespace that should
// 		 * stay in the same tablespace).
// 		 */
// 		if (dst_deftablespace != src_deftablespace)
// 		{
// 			char	   *srcpath;
// 			struct stat st;

// 			srcpath = GetDatabasePath(src_dboid, dst_deftablespace);

// 			if (stat(srcpath, &st) == 0 &&
// 				S_ISDIR(st.st_mode) &&
// 				!directory_is_empty(srcpath))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 						 errmsg("cannot assign new default tablespace \"%s\"",
// 								tablespacename),
// 						 errdetail("There is a conflict because database \"%s\" already has some tables in this tablespace.",
// 								   dbtemplate)));
// 			pfree(srcpath);
// 		}
// 	}
// 	else
// 	{
// 		/* Use template database's default tablespace */
// 		dst_deftablespace = src_deftablespace;
// 		/* Note there is no additional permission check in this path */
// 	}

// 	/*
// 	 * Check for db name conflict.  This is just to give a more friendly error
// 	 * message than "unique index violation".  There's a race condition but
// 	 * we're willing to accept the less friendly message in that case.
// 	 */
// 	if (OidIsValid(get_database_oid(dbname, true)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_DATABASE),
// 				 errmsg("database \"%s\" already exists", dbname)));

// 	/*
// 	 * The source DB can't have any active backends, except this one
// 	 * (exception is to allow CREATE DB while connected to template1).
// 	 * Otherwise we might copy inconsistent data.
// 	 *
// 	 * This should be last among the basic error checks, because it involves
// 	 * potential waiting; we may as well throw an error first if we're gonna
// 	 * throw one.
// 	 */
// 	if (CountOtherDBBackends(src_dboid, &notherbackends, &npreparedxacts))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_IN_USE),
// 			errmsg("source database \"%s\" is being accessed by other users",
// 				   dbtemplate),
// 				 errdetail_busy_db(notherbackends, npreparedxacts)));

// 	/*
// 	 * Select an OID for the new database, checking that it doesn't have a
// 	 * filename conflict with anything already existing in the tablespace
// 	 * directories.
// 	 */
// 	pg_database_rel = heap_open(DatabaseRelationId, RowExclusiveLock);

// 	do
// 	{
// 		dboid = GetNewOid(pg_database_rel);
// 	} while (check_db_file_conflict(dboid));

// 	/*
// 	 * Insert a new tuple into pg_database.  This establishes our ownership of
// 	 * the new database name (anyone else trying to insert the same name will
// 	 * block on the unique index, and fail after we commit).
// 	 */

// 	/* Form tuple */
// 	MemSet(new_record, 0, sizeof(new_record));
// 	MemSet(new_record_nulls, false, sizeof(new_record_nulls));

// 	new_record[Anum_pg_database_datname - 1] =
// 		DirectFunctionCall1(namein, CStringGetDatum(dbname));
// 	new_record[Anum_pg_database_datdba - 1] = ObjectIdGetDatum(datdba);
// 	new_record[Anum_pg_database_encoding - 1] = Int32GetDatum(encoding);
// 	new_record[Anum_pg_database_datcollate - 1] =
// 		DirectFunctionCall1(namein, CStringGetDatum(dbcollate));
// 	new_record[Anum_pg_database_datctype - 1] =
// 		DirectFunctionCall1(namein, CStringGetDatum(dbctype));
// 	new_record[Anum_pg_database_datistemplate - 1] = BoolGetDatum(dbistemplate);
// 	new_record[Anum_pg_database_datallowconn - 1] = BoolGetDatum(dballowconnections);
// 	new_record[Anum_pg_database_datconnlimit - 1] = Int32GetDatum(dbconnlimit);
// 	new_record[Anum_pg_database_datlastsysoid - 1] = ObjectIdGetDatum(src_lastsysoid);
// 	new_record[Anum_pg_database_datfrozenxid - 1] = TransactionIdGetDatum(src_frozenxid);
// 	new_record[Anum_pg_database_datminmxid - 1] = TransactionIdGetDatum(src_minmxid);
// 	new_record[Anum_pg_database_dattablespace - 1] = ObjectIdGetDatum(dst_deftablespace);

// 	/*
// 	 * We deliberately set datacl to default (NULL), rather than copying it
// 	 * from the template database.  Copying it would be a bad idea when the
// 	 * owner is not the same as the template's owner.
// 	 */
// 	new_record_nulls[Anum_pg_database_datacl - 1] = true;

// 	tuple = heap_form_tuple(RelationGetDescr(pg_database_rel),
// 							new_record, new_record_nulls);

// 	HeapTupleSetOid(tuple, dboid);

// 	simple_heap_insert(pg_database_rel, tuple);

// 	/* Update indexes */
// 	CatalogUpdateIndexes(pg_database_rel, tuple);

// 	/*
// 	 * Now generate additional catalog entries associated with the new DB
// 	 */

// 	/* Register owner dependency */
// 	recordDependencyOnOwner(DatabaseRelationId, dboid, datdba);

// 	/* Create pg_shdepend entries for objects within database */
// 	copyTemplateDependencies(src_dboid, dboid);

// 	/* Post creation hook for new database */
// 	InvokeObjectPostCreateHook(DatabaseRelationId, dboid, 0);

// 	/*
// 	 * Force a checkpoint before starting the copy. This will force all dirty
// 	 * buffers, including those of unlogged tables, out to disk, to ensure
// 	 * source database is up-to-date on disk for the copy.
// 	 * FlushDatabaseBuffers() would suffice for that, but we also want to
// 	 * process any pending unlink requests. Otherwise, if a checkpoint
// 	 * happened while we're copying files, a file might be deleted just when
// 	 * we're about to copy it, causing the lstat() call in copydir() to fail
// 	 * with ENOENT.
// 	 */
// 	RequestCheckpoint(CHECKPOINT_IMMEDIATE | CHECKPOINT_FORCE | CHECKPOINT_WAIT
// 					  | CHECKPOINT_FLUSH_ALL);

// 	/*
// 	 * Once we start copying subdirectories, we need to be able to clean 'em
// 	 * up if we fail.  Use an ENSURE block to make sure this happens.  (This
// 	 * is not a 100% solution, because of the possibility of failure during
// 	 * transaction commit after we leave this routine, but it should handle
// 	 * most scenarios.)
// 	 */
// 	fparms.src_dboid = src_dboid;
// 	fparms.dest_dboid = dboid;
// 	PG_ENSURE_ERROR_CLEANUP(createdb_failure_callback,
// 							PointerGetDatum(&fparms));
// 	{
// 		/*
// 		 * Iterate through all tablespaces of the template database, and copy
// 		 * each one to the new database.
// 		 */
// 		rel = heap_open(TableSpaceRelationId, AccessShareLock);
// 		scan = heap_beginscan_catalog(rel, 0, NULL);
// 		while ((tuple = heap_getnext(scan, ForwardScanDirection)) != NULL)
// 		{
// 			Oid			srctablespace = HeapTupleGetOid(tuple);
// 			Oid			dsttablespace;
// 			char	   *srcpath;
// 			char	   *dstpath;
// 			struct stat st;

// 			/* No need to copy global tablespace */
// 			if (srctablespace == GLOBALTABLESPACE_OID)
// 				continue;

// 			srcpath = GetDatabasePath(src_dboid, srctablespace);

// 			if (stat(srcpath, &st) < 0 || !S_ISDIR(st.st_mode) ||
// 				directory_is_empty(srcpath))
// 			{
// 				/* Assume we can ignore it */
// 				pfree(srcpath);
// 				continue;
// 			}

// 			if (srctablespace == src_deftablespace)
// 				dsttablespace = dst_deftablespace;
// 			else
// 				dsttablespace = srctablespace;

// 			dstpath = GetDatabasePath(dboid, dsttablespace);

// 			/*
// 			 * Copy this subdirectory to the new location
// 			 *
// 			 * We don't need to copy subdirectories
// 			 */
// 			copydir(srcpath, dstpath, false);

// 			/* Record the filesystem change in XLOG */
// 			{
// 				xl_dbase_create_rec xlrec;

// 				xlrec.db_id = dboid;
// 				xlrec.tablespace_id = dsttablespace;
// 				xlrec.src_db_id = src_dboid;
// 				xlrec.src_tablespace_id = srctablespace;

// 				XLogBeginInsert();
// 				XLogRegisterData((char *) &xlrec, sizeof(xl_dbase_create_rec));

// 				(void) XLogInsert(RM_DBASE_ID,
// 								  XLOG_DBASE_CREATE | XLR_SPECIAL_REL_UPDATE);
// 			}
// 		}
// 		heap_endscan(scan);
// 		heap_close(rel, AccessShareLock);

// 		/*
// 		 * We force a checkpoint before committing.  This effectively means
// 		 * that committed XLOG_DBASE_CREATE operations will never need to be
// 		 * replayed (at least not in ordinary crash recovery; we still have to
// 		 * make the XLOG entry for the benefit of PITR operations). This
// 		 * avoids two nasty scenarios:
// 		 *
// 		 * #1: When PITR is off, we don't XLOG the contents of newly created
// 		 * indexes; therefore the drop-and-recreate-whole-directory behavior
// 		 * of DBASE_CREATE replay would lose such indexes.
// 		 *
// 		 * #2: Since we have to recopy the source database during DBASE_CREATE
// 		 * replay, we run the risk of copying changes in it that were
// 		 * committed after the original CREATE DATABASE command but before the
// 		 * system crash that led to the replay.  This is at least unexpected
// 		 * and at worst could lead to inconsistencies, eg duplicate table
// 		 * names.
// 		 *
// 		 * (Both of these were real bugs in releases 8.0 through 8.0.3.)
// 		 *
// 		 * In PITR replay, the first of these isn't an issue, and the second
// 		 * is only a risk if the CREATE DATABASE and subsequent template
// 		 * database change both occur while a base backup is being taken.
// 		 * There doesn't seem to be much we can do about that except document
// 		 * it as a limitation.
// 		 *
// 		 * Perhaps if we ever implement CREATE DATABASE in a less cheesy way,
// 		 * we can avoid this.
// 		 */
// 		RequestCheckpoint(CHECKPOINT_IMMEDIATE | CHECKPOINT_FORCE | CHECKPOINT_WAIT);

// 		/*
// 		 * Close pg_database, but keep lock till commit.
// 		 */
// 		heap_close(pg_database_rel, NoLock);

// 		/*
// 		 * Force synchronous commit, thus minimizing the window between
// 		 * creation of the database files and committal of the transaction. If
// 		 * we crash before committing, we'll have a DB that's taking up disk
// 		 * space but is not in pg_database, which is not good.
// 		 */
// 		ForceSyncCommit();
// 	}
// 	PG_END_ENSURE_ERROR_CLEANUP(createdb_failure_callback,
// 								PointerGetDatum(&fparms));

// 	return dboid;
// }
// //end createdb

// Oid
// AlterDatabase(AlterDatabaseStmt *stmt, bool isTopLevel)
// {
// 	Relation	rel;
// 	Oid			dboid;
// 	HeapTuple	tuple,
// 				newtuple;
// 	ScanKeyData scankey;
// 	SysScanDesc scan;
// 	ListCell   *option;
// 	bool		dbistemplate = false;
// 	bool		dballowconnections = true;
// 	int			dbconnlimit = -1;
// 	DefElem    *distemplate = NULL;
// 	DefElem    *dallowconnections = NULL;
// 	DefElem    *dconnlimit = NULL;
// 	DefElem    *dtablespace = NULL;
// 	Datum		new_record[Natts_pg_database];
// 	bool		new_record_nulls[Natts_pg_database];
// 	bool		new_record_repl[Natts_pg_database];

// 	/* Extract options from the statement node tree */
// 	foreach(option, stmt->options)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(option);

// 		if (strcmp(defel->defname, "is_template") == 0)
// 		{
// 			if (distemplate)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			distemplate = defel;
// 		}
// 		else if (strcmp(defel->defname, "allow_connections") == 0)
// 		{
// 			if (dallowconnections)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dallowconnections = defel;
// 		}
// 		else if (strcmp(defel->defname, "connection_limit") == 0)
// 		{
// 			if (dconnlimit)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dconnlimit = defel;
// 		}
// 		else if (strcmp(defel->defname, "tablespace") == 0)
// 		{
// 			if (dtablespace)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			dtablespace = defel;
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("option \"%s\" not recognized", defel->defname)));
// 	}

// 	if (dtablespace)
// 	{
// 		/*
// 		 * While the SET TABLESPACE syntax doesn't allow any other options,
// 		 * somebody could write "WITH TABLESPACE ...".  Forbid any other
// 		 * options from being specified in that case.
// 		 */
// 		if (list_length(stmt->options) != 1)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 			   errmsg("option \"%s\" cannot be specified with other options",
// 					  dtablespace->defname)));
// 		/* this case isn't allowed within a transaction block */
// 		PreventTransactionChain(isTopLevel, "ALTER DATABASE SET TABLESPACE");
// 		movedb(stmt->dbname, defGetString(dtablespace));
// 		return InvalidOid;
// 	}

// 	if (distemplate && distemplate->arg)
// 		dbistemplate = defGetBoolean(distemplate);
// 	if (dallowconnections && dallowconnections->arg)
// 		dballowconnections = defGetBoolean(dallowconnections);
// 	if (dconnlimit && dconnlimit->arg)
// 	{
// 		dbconnlimit = defGetInt32(dconnlimit);
// 		if (dbconnlimit < -1)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("invalid connection limit: %d", dbconnlimit)));
// 	}

// 	/*
// 	 * Get the old tuple.  We don't need a lock on the database per se,
// 	 * because we're not going to do anything that would mess up incoming
// 	 * connections.
// 	 */
// 	rel = heap_open(DatabaseRelationId, RowExclusiveLock);
// 	ScanKeyInit(&scankey,
// 				Anum_pg_database_datname,
// 				BTEqualStrategyNumber, F_NAMEEQ,
// 				NameGetDatum(stmt->dbname));
// 	scan = systable_beginscan(rel, DatabaseNameIndexId, true,
// 							  NULL, 1, &scankey);
// 	tuple = systable_getnext(scan);
// 	if (!HeapTupleIsValid(tuple))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_DATABASE),
// 				 errmsg("database \"%s\" does not exist", stmt->dbname)));

// 	dboid = HeapTupleGetOid(tuple);

// 	if (!pg_database_ownercheck(HeapTupleGetOid(tuple), GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_DATABASE,
// 					   stmt->dbname);

// 	/*
// 	 * In order to avoid getting locked out and having to go through
// 	 * standalone mode, we refuse to disallow connections to the database
// 	 * we're currently connected to.  Lockout can still happen with concurrent
// 	 * sessions but the likeliness of that is not high enough to worry about.
// 	 */
// 	if (!dballowconnections && dboid == MyDatabaseId)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("cannot disallow connections for current database")));

// 	/*
// 	 * Build an updated tuple, perusing the information just obtained
// 	 */
// 	MemSet(new_record, 0, sizeof(new_record));
// 	MemSet(new_record_nulls, false, sizeof(new_record_nulls));
// 	MemSet(new_record_repl, false, sizeof(new_record_repl));

// 	if (distemplate)
// 	{
// 		new_record[Anum_pg_database_datistemplate - 1] = BoolGetDatum(dbistemplate);
// 		new_record_repl[Anum_pg_database_datistemplate - 1] = true;
// 	}
// 	if (dallowconnections)
// 	{
// 		new_record[Anum_pg_database_datallowconn - 1] = BoolGetDatum(dballowconnections);
// 		new_record_repl[Anum_pg_database_datallowconn - 1] = true;
// 	}
// 	if (dconnlimit)
// 	{
// 		new_record[Anum_pg_database_datconnlimit - 1] = Int32GetDatum(dbconnlimit);
// 		new_record_repl[Anum_pg_database_datconnlimit - 1] = true;
// 	}

// 	newtuple = heap_modify_tuple(tuple, RelationGetDescr(rel), new_record,
// 								 new_record_nulls, new_record_repl);
// 	simple_heap_update(rel, &tuple->t_self, newtuple);

// 	/* Update indexes */
// 	CatalogUpdateIndexes(rel, newtuple);

// 	InvokeObjectPostAlterHook(DatabaseRelationId,
// 							  HeapTupleGetOid(newtuple), 0);

// 	systable_endscan(scan);

// 	/* Close pg_database, but keep lock till commit */
// 	heap_close(rel, NoLock);

// 	return dboid;
// }
// //end AlterDatabase

// Oid
// AlterDatabaseSet(AlterDatabaseSetStmt *stmt)
// {
// 	Oid			datid = get_database_oid(stmt->dbname, false);

// 	/*
// 	 * Obtain a lock on the database and make sure it didn't go away in the
// 	 * meantime.
// 	 */
// 	shdepLockAndCheckObject(DatabaseRelationId, datid);

// 	if (!pg_database_ownercheck(datid, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_DATABASE,
// 					   stmt->dbname);

// 	AlterSetting(datid, InvalidOid, stmt->setstmt);

// 	UnlockSharedObject(DatabaseRelationId, datid, 0, AccessShareLock);

// 	return datid;
// }
// //end AlterDatabaseSet

// //defrem.c
// /* commands/operatorcmds.c */
// ObjectAddress
// DefineOperator(List *names, List *parameters)
// {
// 	char	   *oprName;
// 	Oid			oprNamespace;
// 	AclResult	aclresult;
// 	bool		canMerge = false;		/* operator merges */
// 	bool		canHash = false;	/* operator hashes */
// 	List	   *functionName = NIL;		/* function for operator */
// 	TypeName   *typeName1 = NULL;		/* first type name */
// 	TypeName   *typeName2 = NULL;		/* second type name */
// 	Oid			typeId1 = InvalidOid;	/* types converted to OID */
// 	Oid			typeId2 = InvalidOid;
// 	Oid			rettype;
// 	List	   *commutatorName = NIL;	/* optional commutator operator name */
// 	List	   *negatorName = NIL;		/* optional negator operator name */
// 	List	   *restrictionName = NIL;	/* optional restrict. sel. procedure */
// 	List	   *joinName = NIL; /* optional join sel. procedure */
// 	Oid			functionOid;	/* functions converted to OID */
// 	Oid			restrictionOid;
// 	Oid			joinOid;
// 	Oid			typeId[2];		/* to hold left and right arg */
// 	int			nargs;
// 	ListCell   *pl;

// 	/* Convert list of names to a name and namespace */
// 	oprNamespace = QualifiedNameGetCreationNamespace(names, &oprName);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(oprNamespace, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(oprNamespace));

// 	/*
// 	 * loop over the definition list and extract the information we need.
// 	 */
// 	foreach(pl, parameters)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);

// 		if (pg_strcasecmp(defel->defname, "leftarg") == 0)
// 		{
// 			typeName1 = defGetTypeName(defel);
// 			if (typeName1->setof)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					errmsg("SETOF type not allowed for operator argument")));
// 		}
// 		else if (pg_strcasecmp(defel->defname, "rightarg") == 0)
// 		{
// 			typeName2 = defGetTypeName(defel);
// 			if (typeName2->setof)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					errmsg("SETOF type not allowed for operator argument")));
// 		}
// 		else if (pg_strcasecmp(defel->defname, "procedure") == 0)
// 			functionName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "commutator") == 0)
// 			commutatorName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "negator") == 0)
// 			negatorName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "restrict") == 0)
// 			restrictionName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "join") == 0)
// 			joinName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "hashes") == 0)
// 			canHash = defGetBoolean(defel);
// 		else if (pg_strcasecmp(defel->defname, "merges") == 0)
// 			canMerge = defGetBoolean(defel);
// 		/* These obsolete options are taken as meaning canMerge */
// 		else if (pg_strcasecmp(defel->defname, "sort1") == 0)
// 			canMerge = true;
// 		else if (pg_strcasecmp(defel->defname, "sort2") == 0)
// 			canMerge = true;
// 		else if (pg_strcasecmp(defel->defname, "ltcmp") == 0)
// 			canMerge = true;
// 		else if (pg_strcasecmp(defel->defname, "gtcmp") == 0)
// 			canMerge = true;
// 		else
// 		{
// 			/* WARNING, not ERROR, for historical backwards-compatibility */
// 			ereport(WARNING,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("operator attribute \"%s\" not recognized",
// 							defel->defname)));
// 		}
// 	}

// 	/*
// 	 * make sure we have our required definitions
// 	 */
// 	if (functionName == NIL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				 errmsg("operator procedure must be specified")));

// 	/* Transform type names to type OIDs */
// 	if (typeName1)
// 		typeId1 = typenameTypeId(NULL, typeName1);
// 	if (typeName2)
// 		typeId2 = typenameTypeId(NULL, typeName2);

// 	if (!OidIsValid(typeId1) && !OidIsValid(typeId2))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 		   errmsg("at least one of leftarg or rightarg must be specified")));

// 	if (typeName1)
// 	{
// 		aclresult = pg_type_aclcheck(typeId1, GetUserId(), ACL_USAGE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error_type(aclresult, typeId1);
// 	}

// 	if (typeName2)
// 	{
// 		aclresult = pg_type_aclcheck(typeId2, GetUserId(), ACL_USAGE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error_type(aclresult, typeId2);
// 	}

// 	/*
// 	 * Look up the operator's underlying function.
// 	 */
// 	if (!OidIsValid(typeId1))
// 	{
// 		typeId[0] = typeId2;
// 		nargs = 1;
// 	}
// 	else if (!OidIsValid(typeId2))
// 	{
// 		typeId[0] = typeId1;
// 		nargs = 1;
// 	}
// 	else
// 	{
// 		typeId[0] = typeId1;
// 		typeId[1] = typeId2;
// 		nargs = 2;
// 	}
// 	functionOid = LookupFuncName(functionName, nargs, typeId, false);

// 	/*
// 	 * We require EXECUTE rights for the function.  This isn't strictly
// 	 * necessary, since EXECUTE will be checked at any attempted use of the
// 	 * operator, but it seems like a good idea anyway.
// 	 */
// 	aclresult = pg_proc_aclcheck(functionOid, GetUserId(), ACL_EXECUTE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_PROC,
// 					   NameListToString(functionName));

// 	rettype = get_func_rettype(functionOid);
// 	aclresult = pg_type_aclcheck(rettype, GetUserId(), ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error_type(aclresult, rettype);

// 	/*
// 	 * Look up restriction and join estimators if specified
// 	 */
// 	if (restrictionName)
// 		restrictionOid = ValidateRestrictionEstimator(restrictionName);
// 	else
// 		restrictionOid = InvalidOid;
// 	if (joinName)
// 		joinOid = ValidateJoinEstimator(joinName);
// 	else
// 		joinOid = InvalidOid;

// 	/*
// 	 * now have OperatorCreate do all the work..
// 	 */
// 	return
// 		OperatorCreate(oprName, /* operator name */
// 					   oprNamespace,	/* namespace */
// 					   typeId1, /* left type id */
// 					   typeId2, /* right type id */
// 					   functionOid,		/* function for operator */
// 					   commutatorName,	/* optional commutator operator name */
// 					   negatorName,		/* optional negator operator name */
// 					   restrictionOid,	/* optional restrict. sel. procedure */
// 					   joinOid, /* optional join sel. procedure name */
// 					   canMerge,	/* operator merges */
// 					   canHash);	/* operator hashes */
// }
// //end DefineOperator
// ObjectAddress
// AlterOperator(AlterOperatorStmt *stmt)
// {
// 	ObjectAddress address;
// 	Oid			oprId;
// 	Relation	catalog;
// 	HeapTuple	tup;
// 	Form_pg_operator oprForm;
// 	int			i;
// 	ListCell   *pl;
// 	Datum		values[Natts_pg_operator];
// 	bool		nulls[Natts_pg_operator];
// 	bool		replaces[Natts_pg_operator];
// 	List	   *restrictionName = NIL;	/* optional restrict. sel. procedure */
// 	bool		updateRestriction = false;
// 	Oid			restrictionOid;
// 	List	   *joinName = NIL; /* optional join sel. procedure */
// 	bool		updateJoin = false;
// 	Oid			joinOid;

// 	/* Look up the operator */
// 	oprId = LookupOperNameTypeNames(NULL, stmt->opername,
// 									(TypeName *) linitial(stmt->operargs),
// 									(TypeName *) lsecond(stmt->operargs),
// 									false, -1);
// 	catalog = heap_open(OperatorRelationId, RowExclusiveLock);
// 	tup = SearchSysCacheCopy1(OPEROID, ObjectIdGetDatum(oprId));
// 	if (tup == NULL)
// 		elog(ERROR, "cache lookup failed for operator %u", oprId);
// 	oprForm = (Form_pg_operator) GETSTRUCT(tup);

// 	/* Process options */
// 	foreach(pl, stmt->options)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);
// 		List	   *param;

// 		if (defel->arg == NULL)
// 			param = NIL;		/* NONE, removes the function */
// 		else
// 			param = defGetQualifiedName(defel);

// 		if (pg_strcasecmp(defel->defname, "restrict") == 0)
// 		{
// 			restrictionName = param;
// 			updateRestriction = true;
// 		}
// 		else if (pg_strcasecmp(defel->defname, "join") == 0)
// 		{
// 			joinName = param;
// 			updateJoin = true;
// 		}

// 		/*
// 		 * The rest of the options that CREATE accepts cannot be changed.
// 		 * Check for them so that we can give a meaningful error message.
// 		 */
// 		else if (pg_strcasecmp(defel->defname, "leftarg") == 0 ||
// 				 pg_strcasecmp(defel->defname, "rightarg") == 0 ||
// 				 pg_strcasecmp(defel->defname, "procedure") == 0 ||
// 				 pg_strcasecmp(defel->defname, "commutator") == 0 ||
// 				 pg_strcasecmp(defel->defname, "negator") == 0 ||
// 				 pg_strcasecmp(defel->defname, "hashes") == 0 ||
// 				 pg_strcasecmp(defel->defname, "merges") == 0)
// 		{
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("operator attribute \"%s\" cannot be changed",
// 							defel->defname)));
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("operator attribute \"%s\" not recognized",
// 							defel->defname)));
// 	}

// 	/* Check permissions. Must be owner. */
// 	if (!pg_oper_ownercheck(oprId, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_OPER,
// 					   NameStr(oprForm->oprname));

// 	/*
// 	 * Look up restriction and join estimators if specified
// 	 */
// 	if (restrictionName)
// 		restrictionOid = ValidateRestrictionEstimator(restrictionName);
// 	else
// 		restrictionOid = InvalidOid;
// 	if (joinName)
// 		joinOid = ValidateJoinEstimator(joinName);
// 	else
// 		joinOid = InvalidOid;

// 	/* Perform additional checks, like OperatorCreate does */
// 	if (!(OidIsValid(oprForm->oprleft) && OidIsValid(oprForm->oprright)))
// 	{
// 		/* If it's not a binary op, these things mustn't be set: */
// 		if (OidIsValid(joinOid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				 errmsg("only binary operators can have join selectivity")));
// 	}

// 	if (oprForm->oprresult != BOOLOID)
// 	{
// 		if (OidIsValid(restrictionOid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("only boolean operators can have restriction selectivity")));
// 		if (OidIsValid(joinOid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				errmsg("only boolean operators can have join selectivity")));
// 	}

// 	/* Update the tuple */
// 	for (i = 0; i < Natts_pg_operator; ++i)
// 	{
// 		values[i] = (Datum) 0;
// 		replaces[i] = false;
// 		nulls[i] = false;
// 	}
// 	if (updateRestriction)
// 	{
// 		replaces[Anum_pg_operator_oprrest - 1] = true;
// 		values[Anum_pg_operator_oprrest - 1] = restrictionOid;
// 	}
// 	if (updateJoin)
// 	{
// 		replaces[Anum_pg_operator_oprjoin - 1] = true;
// 		values[Anum_pg_operator_oprjoin - 1] = joinOid;
// 	}

// 	tup = heap_modify_tuple(tup, RelationGetDescr(catalog),
// 							values, nulls, replaces);

// 	simple_heap_update(catalog, &tup->t_self, tup);
// 	CatalogUpdateIndexes(catalog, tup);

// 	address = makeOperatorDependencies(tup, true);

// 	InvokeObjectPostAlterHook(OperatorRelationId, oprId, 0);

// 	heap_close(catalog, NoLock);

// 	return address;
// }
// //end

// /* commands/aggregatecmds.c */
// ObjectAddress
// DefineAggregate(List *name, List *args, bool oldstyle, List *parameters,
// 				const char *queryString)
// {
// 	char	   *aggName;
// 	Oid			aggNamespace;
// 	AclResult	aclresult;
// 	char		aggKind = AGGKIND_NORMAL;
// 	List	   *transfuncName = NIL;
// 	List	   *finalfuncName = NIL;
// 	List	   *combinefuncName = NIL;
// 	List	   *serialfuncName = NIL;
// 	List	   *deserialfuncName = NIL;
// 	List	   *mtransfuncName = NIL;
// 	List	   *minvtransfuncName = NIL;
// 	List	   *mfinalfuncName = NIL;
// 	bool		finalfuncExtraArgs = false;
// 	bool		mfinalfuncExtraArgs = false;
// 	List	   *sortoperatorName = NIL;
// 	TypeName   *baseType = NULL;
// 	TypeName   *transType = NULL;
// 	TypeName   *mtransType = NULL;
// 	int32		transSpace = 0;
// 	int32		mtransSpace = 0;
// 	char	   *initval = NULL;
// 	char	   *minitval = NULL;
// 	char	   *parallel = NULL;
// 	int			numArgs;
// 	int			numDirectArgs = 0;
// 	oidvector  *parameterTypes;
// 	ArrayType  *allParameterTypes;
// 	ArrayType  *parameterModes;
// 	ArrayType  *parameterNames;
// 	List	   *parameterDefaults;
// 	Oid			variadicArgType;
// 	Oid			transTypeId;
// 	Oid			mtransTypeId = InvalidOid;
// 	char		transTypeType;
// 	char		mtransTypeType = 0;
// 	char		proparallel = PROPARALLEL_UNSAFE;
// 	ListCell   *pl;

// 	/* Convert list of names to a name and namespace */
// 	aggNamespace = QualifiedNameGetCreationNamespace(name, &aggName);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(aggNamespace, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(aggNamespace));

// 	/* Deconstruct the output of the aggr_args grammar production */
// 	if (!oldstyle)
// 	{
// 		Assert(list_length(args) == 2);
// 		numDirectArgs = intVal(lsecond(args));
// 		if (numDirectArgs >= 0)
// 			aggKind = AGGKIND_ORDERED_SET;
// 		else
// 			numDirectArgs = 0;
// 		args = (List *) linitial(args);
// 	}

// 	/* Examine aggregate's definition clauses */
// 	foreach(pl, parameters)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);

// 		/*
// 		 * sfunc1, stype1, and initcond1 are accepted as obsolete spellings
// 		 * for sfunc, stype, initcond.
// 		 */
// 		if (pg_strcasecmp(defel->defname, "sfunc") == 0)
// 			transfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "sfunc1") == 0)
// 			transfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "finalfunc") == 0)
// 			finalfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "combinefunc") == 0)
// 			combinefuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "serialfunc") == 0)
// 			serialfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "deserialfunc") == 0)
// 			deserialfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "msfunc") == 0)
// 			mtransfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "minvfunc") == 0)
// 			minvtransfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "mfinalfunc") == 0)
// 			mfinalfuncName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "finalfunc_extra") == 0)
// 			finalfuncExtraArgs = defGetBoolean(defel);
// 		else if (pg_strcasecmp(defel->defname, "mfinalfunc_extra") == 0)
// 			mfinalfuncExtraArgs = defGetBoolean(defel);
// 		else if (pg_strcasecmp(defel->defname, "sortop") == 0)
// 			sortoperatorName = defGetQualifiedName(defel);
// 		else if (pg_strcasecmp(defel->defname, "basetype") == 0)
// 			baseType = defGetTypeName(defel);
// 		else if (pg_strcasecmp(defel->defname, "hypothetical") == 0)
// 		{
// 			if (defGetBoolean(defel))
// 			{
// 				if (aggKind == AGGKIND_NORMAL)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 							 errmsg("only ordered-set aggregates can be hypothetical")));
// 				aggKind = AGGKIND_HYPOTHETICAL;
// 			}
// 		}
// 		else if (pg_strcasecmp(defel->defname, "stype") == 0)
// 			transType = defGetTypeName(defel);
// 		else if (pg_strcasecmp(defel->defname, "stype1") == 0)
// 			transType = defGetTypeName(defel);
// 		else if (pg_strcasecmp(defel->defname, "sspace") == 0)
// 			transSpace = defGetInt32(defel);
// 		else if (pg_strcasecmp(defel->defname, "mstype") == 0)
// 			mtransType = defGetTypeName(defel);
// 		else if (pg_strcasecmp(defel->defname, "msspace") == 0)
// 			mtransSpace = defGetInt32(defel);
// 		else if (pg_strcasecmp(defel->defname, "initcond") == 0)
// 			initval = defGetString(defel);
// 		else if (pg_strcasecmp(defel->defname, "initcond1") == 0)
// 			initval = defGetString(defel);
// 		else if (pg_strcasecmp(defel->defname, "minitcond") == 0)
// 			minitval = defGetString(defel);
// 		else if (pg_strcasecmp(defel->defname, "parallel") == 0)
// 			parallel = defGetString(defel);
// 		else
// 			ereport(WARNING,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("aggregate attribute \"%s\" not recognized",
// 							defel->defname)));
// 	}

// 	/*
// 	 * make sure we have our required definitions
// 	 */
// 	if (transType == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				 errmsg("aggregate stype must be specified")));
// 	if (transfuncName == NIL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				 errmsg("aggregate sfunc must be specified")));

// 	/*
// 	 * if mtransType is given, mtransfuncName and minvtransfuncName must be as
// 	 * well; if not, then none of the moving-aggregate options should have
// 	 * been given.
// 	 */
// 	if (mtransType != NULL)
// 	{
// 		if (mtransfuncName == NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate msfunc must be specified when mstype is specified")));
// 		if (minvtransfuncName == NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate minvfunc must be specified when mstype is specified")));
// 	}
// 	else
// 	{
// 		if (mtransfuncName != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 			errmsg("aggregate msfunc must not be specified without mstype")));
// 		if (minvtransfuncName != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate minvfunc must not be specified without mstype")));
// 		if (mfinalfuncName != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate mfinalfunc must not be specified without mstype")));
// 		if (mtransSpace != 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate msspace must not be specified without mstype")));
// 		if (minitval != NULL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate minitcond must not be specified without mstype")));
// 	}

// 	/*
// 	 * look up the aggregate's input datatype(s).
// 	 */
// 	if (oldstyle)
// 	{
// 		/*
// 		 * Old style: use basetype parameter.  This supports aggregates of
// 		 * zero or one input, with input type ANY meaning zero inputs.
// 		 *
// 		 * Historically we allowed the command to look like basetype = 'ANY'
// 		 * so we must do a case-insensitive comparison for the name ANY. Ugh.
// 		 */
// 		Oid			aggArgTypes[1];

// 		if (baseType == NULL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate input type must be specified")));

// 		if (pg_strcasecmp(TypeNameToString(baseType), "ANY") == 0)
// 		{
// 			numArgs = 0;
// 			aggArgTypes[0] = InvalidOid;
// 		}
// 		else
// 		{
// 			numArgs = 1;
// 			aggArgTypes[0] = typenameTypeId(NULL, baseType);
// 		}
// 		parameterTypes = buildoidvector(aggArgTypes, numArgs);
// 		allParameterTypes = NULL;
// 		parameterModes = NULL;
// 		parameterNames = NULL;
// 		parameterDefaults = NIL;
// 		variadicArgType = InvalidOid;
// 	}
// 	else
// 	{
// 		/*
// 		 * New style: args is a list of FunctionParameters (possibly zero of
// 		 * 'em).  We share functioncmds.c's code for processing them.
// 		 */
// 		Oid			requiredResultType;

// 		if (baseType != NULL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("basetype is redundant with aggregate input type specification")));

// 		numArgs = list_length(args);
// 		interpret_function_parameter_list(args,
// 										  InvalidOid,
// 										  true, /* is an aggregate */
// 										  queryString,
// 										  &parameterTypes,
// 										  &allParameterTypes,
// 										  &parameterModes,
// 										  &parameterNames,
// 										  &parameterDefaults,
// 										  &variadicArgType,
// 										  &requiredResultType);
// 		/* Parameter defaults are not currently allowed by the grammar */
// 		Assert(parameterDefaults == NIL);
// 		/* There shouldn't have been any OUT parameters, either */
// 		Assert(requiredResultType == InvalidOid);
// 	}

// 	/*
// 	 * look up the aggregate's transtype.
// 	 *
// 	 * transtype can't be a pseudo-type, since we need to be able to store
// 	 * values of the transtype.  However, we can allow polymorphic transtype
// 	 * in some cases (AggregateCreate will check).  Also, we allow "internal"
// 	 * for functions that want to pass pointers to private data structures;
// 	 * but allow that only to superusers, since you could crash the system (or
// 	 * worse) by connecting up incompatible internal-using functions in an
// 	 * aggregate.
// 	 */
// 	transTypeId = typenameTypeId(NULL, transType);
// 	transTypeType = get_typtype(transTypeId);
// 	if (transTypeType == TYPTYPE_PSEUDO &&
// 		!IsPolymorphicType(transTypeId))
// 	{
// 		if (transTypeId == INTERNALOID && superuser())
// 			 /* okay */ ;
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("aggregate transition data type cannot be %s",
// 							format_type_be(transTypeId))));
// 	}

// 	if (serialfuncName && deserialfuncName)
// 	{
// 		/*
// 		 * Serialization is only needed/allowed for transtype INTERNAL.
// 		 */
// 		if (transTypeId != INTERNALOID)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("serialization functions may be specified only when the aggregate transition data type is %s",
// 							format_type_be(INTERNALOID))));
// 	}
// 	else if (serialfuncName || deserialfuncName)
// 	{
// 		/*
// 		 * Cannot specify one function without the other.
// 		 */
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				 errmsg("must specify both or neither of serialization and deserialization functions")));
// 	}

// 	/*
// 	 * If a moving-aggregate transtype is specified, look that up.  Same
// 	 * restrictions as for transtype.
// 	 */
// 	if (mtransType)
// 	{
// 		mtransTypeId = typenameTypeId(NULL, mtransType);
// 		mtransTypeType = get_typtype(mtransTypeId);
// 		if (mtransTypeType == TYPTYPE_PSEUDO &&
// 			!IsPolymorphicType(mtransTypeId))
// 		{
// 			if (mtransTypeId == INTERNALOID && superuser())
// 				 /* okay */ ;
// 			else
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						 errmsg("aggregate transition data type cannot be %s",
// 								format_type_be(mtransTypeId))));
// 		}
// 	}

// 	/*
// 	 * If we have an initval, and it's not for a pseudotype (particularly a
// 	 * polymorphic type), make sure it's acceptable to the type's input
// 	 * function.  We will store the initval as text, because the input
// 	 * function isn't necessarily immutable (consider "now" for timestamp),
// 	 * and we want to use the runtime not creation-time interpretation of the
// 	 * value.  However, if it's an incorrect value it seems much more
// 	 * user-friendly to complain at CREATE AGGREGATE time.
// 	 */
// 	if (initval && transTypeType != TYPTYPE_PSEUDO)
// 	{
// 		Oid			typinput,
// 					typioparam;

// 		getTypeInputInfo(transTypeId, &typinput, &typioparam);
// 		(void) OidInputFunctionCall(typinput, initval, typioparam, -1);
// 	}

// 	/*
// 	 * Likewise for moving-aggregate initval.
// 	 */
// 	if (minitval && mtransTypeType != TYPTYPE_PSEUDO)
// 	{
// 		Oid			typinput,
// 					typioparam;

// 		getTypeInputInfo(mtransTypeId, &typinput, &typioparam);
// 		(void) OidInputFunctionCall(typinput, minitval, typioparam, -1);
// 	}

// 	if (parallel)
// 	{
// 		if (pg_strcasecmp(parallel, "safe") == 0)
// 			proparallel = PROPARALLEL_SAFE;
// 		else if (pg_strcasecmp(parallel, "restricted") == 0)
// 			proparallel = PROPARALLEL_RESTRICTED;
// 		else if (pg_strcasecmp(parallel, "unsafe") == 0)
// 			proparallel = PROPARALLEL_UNSAFE;
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("parameter \"parallel\" must be SAFE, RESTRICTED, or UNSAFE")));
// 	}

// 	/*
// 	 * Most of the argument-checking is done inside of AggregateCreate
// 	 */
// 	return AggregateCreate(aggName,		/* aggregate name */
// 						   aggNamespace,		/* namespace */
// 						   aggKind,
// 						   numArgs,
// 						   numDirectArgs,
// 						   parameterTypes,
// 						   PointerGetDatum(allParameterTypes),
// 						   PointerGetDatum(parameterModes),
// 						   PointerGetDatum(parameterNames),
// 						   parameterDefaults,
// 						   variadicArgType,
// 						   transfuncName,		/* step function name */
// 						   finalfuncName,		/* final function name */
// 						   combinefuncName,		/* combine function name */
// 						   serialfuncName,		/* serial function name */
// 						   deserialfuncName,	/* deserial function name */
// 						   mtransfuncName,		/* fwd trans function name */
// 						   minvtransfuncName,	/* inv trans function name */
// 						   mfinalfuncName,		/* final function name */
// 						   finalfuncExtraArgs,
// 						   mfinalfuncExtraArgs,
// 						   sortoperatorName,	/* sort operator name */
// 						   transTypeId, /* transition data type */
// 						   transSpace,	/* transition space */
// 						   mtransTypeId,		/* transition data type */
// 						   mtransSpace, /* transition space */
// 						   initval,		/* initial condition */
// 						   minitval,	/* initial condition */
// 						   proparallel);		/* parallel safe? */
// }
// //end DefineAggregate

// /* commands/opclasscmds.c */
// ObjectAddress
// DefineOpClass(CreateOpClassStmt *stmt)
// {
// 	char	   *opcname;		/* name of opclass we're creating */
// 	Oid			amoid,			/* our AM's oid */
// 				typeoid,		/* indexable datatype oid */
// 				storageoid,		/* storage datatype oid, if any */
// 				namespaceoid,	/* namespace to create opclass in */
// 				opfamilyoid,	/* oid of containing opfamily */
// 				opclassoid;		/* oid of opclass we create */
// 	int			maxOpNumber,	/* amstrategies value */
// 				maxProcNumber;	/* amsupport value */
// 	bool		amstorage;		/* amstorage flag */
// 	List	   *operators;		/* OpFamilyMember list for operators */
// 	List	   *procedures;		/* OpFamilyMember list for support procs */
// 	ListCell   *l;
// 	Relation	rel;
// 	HeapTuple	tup;
// 	IndexAmRoutine *amroutine;
// 	Datum		values[Natts_pg_opclass];
// 	bool		nulls[Natts_pg_opclass];
// 	AclResult	aclresult;
// 	NameData	opcName;
// 	ObjectAddress myself,
// 				referenced;

// 	/* Convert list of names to a name and namespace */
// 	namespaceoid = QualifiedNameGetCreationNamespace(stmt->opclassname,
// 													 &opcname);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(namespaceoid, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(namespaceoid));

// 	/* Get necessary info about access method */
// 	tup = SearchSysCache1(AMNAME, CStringGetDatum(stmt->amname));
// 	if (!HeapTupleIsValid(tup))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("access method \"%s\" does not exist",
// 						stmt->amname)));

// 	amoid = HeapTupleGetOid(tup);
// 	amroutine = GetIndexAmRoutineByAmId(amoid, false);
// 	ReleaseSysCache(tup);

// 	maxOpNumber = amroutine->amstrategies;
// 	/* if amstrategies is zero, just enforce that op numbers fit in int16 */
// 	if (maxOpNumber <= 0)
// 		maxOpNumber = SHRT_MAX;
// 	maxProcNumber = amroutine->amsupport;
// 	amstorage = amroutine->amstorage;

// 	/* XXX Should we make any privilege check against the AM? */

// 	/*
// 	 * The question of appropriate permissions for CREATE OPERATOR CLASS is
// 	 * interesting.  Creating an opclass is tantamount to granting public
// 	 * execute access on the functions involved, since the index machinery
// 	 * generally does not check access permission before using the functions.
// 	 * A minimum expectation therefore is that the caller have execute
// 	 * privilege with grant option.  Since we don't have a way to make the
// 	 * opclass go away if the grant option is revoked, we choose instead to
// 	 * require ownership of the functions.  It's also not entirely clear what
// 	 * permissions should be required on the datatype, but ownership seems
// 	 * like a safe choice.
// 	 *
// 	 * Currently, we require superuser privileges to create an opclass. This
// 	 * seems necessary because we have no way to validate that the offered set
// 	 * of operators and functions are consistent with the AM's expectations.
// 	 * It would be nice to provide such a check someday, if it can be done
// 	 * without solving the halting problem :-(
// 	 *
// 	 * XXX re-enable NOT_USED code sections below if you remove this test.
// 	 */
// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to create an operator class")));

// 	/* Look up the datatype */
// 	typeoid = typenameTypeId(NULL, stmt->datatype);

// #ifdef NOT_USED
// 	/* XXX this is unnecessary given the superuser check above */
// 	/* Check we have ownership of the datatype */
// 	if (!pg_type_ownercheck(typeoid, GetUserId()))
// 		aclcheck_error_type(ACLCHECK_NOT_OWNER, typeoid);
// #endif

// 	/*
// 	 * Look up the containing operator family, or create one if FAMILY option
// 	 * was omitted and there's not a match already.
// 	 */
// 	if (stmt->opfamilyname)
// 	{
// 		opfamilyoid = get_opfamily_oid(amoid, stmt->opfamilyname, false);
// 	}
// 	else
// 	{
// 		/* Lookup existing family of same name and namespace */
// 		tup = SearchSysCache3(OPFAMILYAMNAMENSP,
// 							  ObjectIdGetDatum(amoid),
// 							  PointerGetDatum(opcname),
// 							  ObjectIdGetDatum(namespaceoid));
// 		if (HeapTupleIsValid(tup))
// 		{
// 			opfamilyoid = HeapTupleGetOid(tup);

// 			/*
// 			 * XXX given the superuser check above, there's no need for an
// 			 * ownership check here
// 			 */
// 			ReleaseSysCache(tup);
// 		}
// 		else
// 		{
// 			ObjectAddress tmpAddr;

// 			/*
// 			 * Create it ... again no need for more permissions ...
// 			 */
// 			tmpAddr = CreateOpFamily(stmt->amname, opcname,
// 									 namespaceoid, amoid);
// 			opfamilyoid = tmpAddr.objectId;
// 		}
// 	}

// 	operators = NIL;
// 	procedures = NIL;

// 	/* Storage datatype is optional */
// 	storageoid = InvalidOid;

// 	/*
// 	 * Scan the "items" list to obtain additional info.
// 	 */
// 	foreach(l, stmt->items)
// 	{
// 		CreateOpClassItem *item = lfirst(l);
// 		Oid			operOid;
// 		Oid			funcOid;
// 		Oid			sortfamilyOid;
// 		OpFamilyMember *member;

// 		Assert(IsA(item, CreateOpClassItem));
// 		switch (item->itemtype)
// 		{
// 			case OPCLASS_ITEM_OPERATOR:
// 				if (item->number <= 0 || item->number > maxOpNumber)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 							 errmsg("invalid operator number %d,"
// 									" must be between 1 and %d",
// 									item->number, maxOpNumber)));
// 				if (item->args != NIL)
// 				{
// 					TypeName   *typeName1 = (TypeName *) linitial(item->args);
// 					TypeName   *typeName2 = (TypeName *) lsecond(item->args);

// 					operOid = LookupOperNameTypeNames(NULL, item->name,
// 													  typeName1, typeName2,
// 													  false, -1);
// 				}
// 				else
// 				{
// 					/* Default to binary op on input datatype */
// 					operOid = LookupOperName(NULL, item->name,
// 											 typeoid, typeoid,
// 											 false, -1);
// 				}

// 				if (item->order_family)
// 					sortfamilyOid = get_opfamily_oid(BTREE_AM_OID,
// 													 item->order_family,
// 													 false);
// 				else
// 					sortfamilyOid = InvalidOid;

// #ifdef NOT_USED
// 				/* XXX this is unnecessary given the superuser check above */
// 				/* Caller must own operator and its underlying function */
// 				if (!pg_oper_ownercheck(operOid, GetUserId()))
// 					aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_OPER,
// 								   get_opname(operOid));
// 				funcOid = get_opcode(operOid);
// 				if (!pg_proc_ownercheck(funcOid, GetUserId()))
// 					aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_PROC,
// 								   get_func_name(funcOid));
// #endif

// 				/* Save the info */
// 				member = (OpFamilyMember *) palloc0(sizeof(OpFamilyMember));
// 				member->object = operOid;
// 				member->number = item->number;
// 				member->sortfamily = sortfamilyOid;
// 				assignOperTypes(member, amoid, typeoid);
// 				addFamilyMember(&operators, member, false);
// 				break;
// 			case OPCLASS_ITEM_FUNCTION:
// 				if (item->number <= 0 || item->number > maxProcNumber)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 							 errmsg("invalid procedure number %d,"
// 									" must be between 1 and %d",
// 									item->number, maxProcNumber)));
// 				funcOid = LookupFuncNameTypeNames(item->name, item->args,
// 												  false);
// #ifdef NOT_USED
// 				/* XXX this is unnecessary given the superuser check above */
// 				/* Caller must own function */
// 				if (!pg_proc_ownercheck(funcOid, GetUserId()))
// 					aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_PROC,
// 								   get_func_name(funcOid));
// #endif

// 				/* Save the info */
// 				member = (OpFamilyMember *) palloc0(sizeof(OpFamilyMember));
// 				member->object = funcOid;
// 				member->number = item->number;

// 				/* allow overriding of the function's actual arg types */
// 				if (item->class_args)
// 					processTypesSpec(item->class_args,
// 									 &member->lefttype, &member->righttype);

// 				assignProcTypes(member, amoid, typeoid);
// 				addFamilyMember(&procedures, member, true);
// 				break;
// 			case OPCLASS_ITEM_STORAGETYPE:
// 				if (OidIsValid(storageoid))
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 						   errmsg("storage type specified more than once")));
// 				storageoid = typenameTypeId(NULL, item->storedtype);

// #ifdef NOT_USED
// 				/* XXX this is unnecessary given the superuser check above */
// 				/* Check we have ownership of the datatype */
// 				if (!pg_type_ownercheck(storageoid, GetUserId()))
// 					aclcheck_error_type(ACLCHECK_NOT_OWNER, storageoid);
// #endif
// 				break;
// 			default:
// 				elog(ERROR, "unrecognized item type: %d", item->itemtype);
// 				break;
// 		}
// 	}

// 	/*
// 	 * If storagetype is specified, make sure it's legal.
// 	 */
// 	if (OidIsValid(storageoid))
// 	{
// 		/* Just drop the spec if same as column datatype */
// 		if (storageoid == typeoid)
// 			storageoid = InvalidOid;
// 		else if (!amstorage)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("storage type cannot be different from data type for access method \"%s\"",
// 							stmt->amname)));
// 	}

// 	rel = heap_open(OperatorClassRelationId, RowExclusiveLock);

// 	/*
// 	 * Make sure there is no existing opclass of this name (this is just to
// 	 * give a more friendly error message than "duplicate key").
// 	 */
// 	if (SearchSysCacheExists3(CLAAMNAMENSP,
// 							  ObjectIdGetDatum(amoid),
// 							  CStringGetDatum(opcname),
// 							  ObjectIdGetDatum(namespaceoid)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("operator class \"%s\" for access method \"%s\" already exists",
// 						opcname, stmt->amname)));

// 	/*
// 	 * If we are creating a default opclass, check there isn't one already.
// 	 * (Note we do not restrict this test to visible opclasses; this ensures
// 	 * that typcache.c can find unique solutions to its questions.)
// 	 */
// 	if (stmt->isDefault)
// 	{
// 		ScanKeyData skey[1];
// 		SysScanDesc scan;

// 		ScanKeyInit(&skey[0],
// 					Anum_pg_opclass_opcmethod,
// 					BTEqualStrategyNumber, F_OIDEQ,
// 					ObjectIdGetDatum(amoid));

// 		scan = systable_beginscan(rel, OpclassAmNameNspIndexId, true,
// 								  NULL, 1, skey);

// 		while (HeapTupleIsValid(tup = systable_getnext(scan)))
// 		{
// 			Form_pg_opclass opclass = (Form_pg_opclass) GETSTRUCT(tup);

// 			if (opclass->opcintype == typeoid && opclass->opcdefault)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_DUPLICATE_OBJECT),
// 						 errmsg("could not make operator class \"%s\" be default for type %s",
// 								opcname,
// 								TypeNameToString(stmt->datatype)),
// 				   errdetail("Operator class \"%s\" already is the default.",
// 							 NameStr(opclass->opcname))));
// 		}

// 		systable_endscan(scan);
// 	}

// 	/*
// 	 * Okay, let's create the pg_opclass entry.
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_opclass_opcmethod - 1] = ObjectIdGetDatum(amoid);
// 	namestrcpy(&opcName, opcname);
// 	values[Anum_pg_opclass_opcname - 1] = NameGetDatum(&opcName);
// 	values[Anum_pg_opclass_opcnamespace - 1] = ObjectIdGetDatum(namespaceoid);
// 	values[Anum_pg_opclass_opcowner - 1] = ObjectIdGetDatum(GetUserId());
// 	values[Anum_pg_opclass_opcfamily - 1] = ObjectIdGetDatum(opfamilyoid);
// 	values[Anum_pg_opclass_opcintype - 1] = ObjectIdGetDatum(typeoid);
// 	values[Anum_pg_opclass_opcdefault - 1] = BoolGetDatum(stmt->isDefault);
// 	values[Anum_pg_opclass_opckeytype - 1] = ObjectIdGetDatum(storageoid);

// 	tup = heap_form_tuple(rel->rd_att, values, nulls);

// 	opclassoid = simple_heap_insert(rel, tup);

// 	CatalogUpdateIndexes(rel, tup);

// 	heap_freetuple(tup);

// 	/*
// 	 * Now add tuples to pg_amop and pg_amproc tying in the operators and
// 	 * functions.  Dependencies on them are inserted, too.
// 	 */
// 	storeOperators(stmt->opfamilyname, amoid, opfamilyoid,
// 				   opclassoid, operators, false);
// 	storeProcedures(stmt->opfamilyname, amoid, opfamilyoid,
// 					opclassoid, procedures, false);

// 	/* let event triggers know what happened */
// 	EventTriggerCollectCreateOpClass(stmt, opclassoid, operators, procedures);

// 	/*
// 	 * Create dependencies for the opclass proper.  Note: we do not need a
// 	 * dependency link to the AM, because that exists through the opfamily.
// 	 */
// 	myself.classId = OperatorClassRelationId;
// 	myself.objectId = opclassoid;
// 	myself.objectSubId = 0;

// 	/* dependency on namespace */
// 	referenced.classId = NamespaceRelationId;
// 	referenced.objectId = namespaceoid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	/* dependency on opfamily */
// 	referenced.classId = OperatorFamilyRelationId;
// 	referenced.objectId = opfamilyoid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_AUTO);

// 	/* dependency on indexed datatype */
// 	referenced.classId = TypeRelationId;
// 	referenced.objectId = typeoid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	/* dependency on storage datatype */
// 	if (OidIsValid(storageoid))
// 	{
// 		referenced.classId = TypeRelationId;
// 		referenced.objectId = storageoid;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 	}

// 	/* dependency on owner */
// 	recordDependencyOnOwner(OperatorClassRelationId, opclassoid, GetUserId());

// 	/* dependency on extension */
// 	recordDependencyOnCurrentExtension(&myself, false);

// 	/* Post creation hook for new operator class */
// 	InvokeObjectPostCreateHook(OperatorClassRelationId, opclassoid, 0);

// 	heap_close(rel, RowExclusiveLock);

// 	return myself;
// }
// //end DefineOpClass

// ObjectAddress
// DefineOpFamily(CreateOpFamilyStmt *stmt)
// {
// 	char	   *opfname;		/* name of opfamily we're creating */
// 	Oid			amoid,			/* our AM's oid */
// 				namespaceoid;	/* namespace to create opfamily in */
// 	AclResult	aclresult;

// 	/* Convert list of names to a name and namespace */
// 	namespaceoid = QualifiedNameGetCreationNamespace(stmt->opfamilyname,
// 													 &opfname);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(namespaceoid, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(namespaceoid));

// 	/* Get access method OID, throwing an error if it doesn't exist. */
// 	amoid = get_index_am_oid(stmt->amname, false);

// 	/* XXX Should we make any privilege check against the AM? */

// 	/*
// 	 * Currently, we require superuser privileges to create an opfamily. See
// 	 * comments in DefineOpClass.
// 	 */
// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to create an operator family")));

// 	/* Insert pg_opfamily catalog entry */
// 	return CreateOpFamily(stmt->amname, opfname, namespaceoid, amoid);
// }
// //end DefineOpFamily

// Oid
// AlterOpFamily(AlterOpFamilyStmt *stmt)
// {
// 	Oid			amoid,			/* our AM's oid */
// 				opfamilyoid;	/* oid of opfamily */
// 	int			maxOpNumber,	/* amstrategies value */
// 				maxProcNumber;	/* amsupport value */
// 	HeapTuple	tup;
// 	IndexAmRoutine *amroutine;

// 	/* Get necessary info about access method */
// 	tup = SearchSysCache1(AMNAME, CStringGetDatum(stmt->amname));
// 	if (!HeapTupleIsValid(tup))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("access method \"%s\" does not exist",
// 						stmt->amname)));

// 	amoid = HeapTupleGetOid(tup);
// 	amroutine = GetIndexAmRoutineByAmId(amoid, false);
// 	ReleaseSysCache(tup);

// 	maxOpNumber = amroutine->amstrategies;
// 	/* if amstrategies is zero, just enforce that op numbers fit in int16 */
// 	if (maxOpNumber <= 0)
// 		maxOpNumber = SHRT_MAX;
// 	maxProcNumber = amroutine->amsupport;

// 	/* XXX Should we make any privilege check against the AM? */

// 	/* Look up the opfamily */
// 	opfamilyoid = get_opfamily_oid(amoid, stmt->opfamilyname, false);

// 	/*
// 	 * Currently, we require superuser privileges to alter an opfamily.
// 	 *
// 	 * XXX re-enable NOT_USED code sections below if you remove this test.
// 	 */
// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to alter an operator family")));

// 	/*
// 	 * ADD and DROP cases need separate code from here on down.
// 	 */
// 	if (stmt->isDrop)
// 		AlterOpFamilyDrop(stmt, amoid, opfamilyoid,
// 						  maxOpNumber, maxProcNumber, stmt->items);
// 	else
// 		AlterOpFamilyAdd(stmt, amoid, opfamilyoid,
// 						 maxOpNumber, maxProcNumber, stmt->items);

// 	return opfamilyoid;
// }
// //end AlterOpFamily

// void
// IsThereOpClassInNamespace(const char *opcname, Oid opcmethod,
// 						  Oid opcnamespace)
// {
// 	/* make sure the new name doesn't exist */
// 	if (SearchSysCacheExists3(CLAAMNAMENSP,
// 							  ObjectIdGetDatum(opcmethod),
// 							  CStringGetDatum(opcname),
// 							  ObjectIdGetDatum(opcnamespace)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("operator class \"%s\" for access method \"%s\" already exists in schema \"%s\"",
// 						opcname,
// 						get_am_name(opcmethod),
// 						get_namespace_name(opcnamespace))));
// }
// //end

// void
// IsThereOpFamilyInNamespace(const char *opfname, Oid opfmethod,
// 						   Oid opfnamespace)
// {
// 	/* make sure the new name doesn't exist */
// 	if (SearchSysCacheExists3(OPFAMILYAMNAMENSP,
// 							  ObjectIdGetDatum(opfmethod),
// 							  CStringGetDatum(opfname),
// 							  ObjectIdGetDatum(opfnamespace)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("operator family \"%s\" for access method \"%s\" already exists in schema \"%s\"",
// 						opfname,
// 						get_am_name(opfmethod),
// 						get_namespace_name(opfnamespace))));
// }
// //end
// /* commands/tsearchcmds.c */
// ObjectAddress
// DefineTSParser(List *names, List *parameters)
// {
// 	char	   *prsname;
// 	ListCell   *pl;
// 	Relation	prsRel;
// 	HeapTuple	tup;
// 	Datum		values[Natts_pg_ts_parser];
// 	bool		nulls[Natts_pg_ts_parser];
// 	NameData	pname;
// 	Oid			prsOid;
// 	Oid			namespaceoid;
// 	ObjectAddress address;

// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to create text search parsers")));

// 	/* Convert list of names to a name and namespace */
// 	namespaceoid = QualifiedNameGetCreationNamespace(names, &prsname);

// 	/* initialize tuple fields with name/namespace */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	namestrcpy(&pname, prsname);
// 	values[Anum_pg_ts_parser_prsname - 1] = NameGetDatum(&pname);
// 	values[Anum_pg_ts_parser_prsnamespace - 1] = ObjectIdGetDatum(namespaceoid);

// 	/*
// 	 * loop over the definition list and extract the information we need.
// 	 */
// 	foreach(pl, parameters)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);

// 		if (pg_strcasecmp(defel->defname, "start") == 0)
// 		{
// 			values[Anum_pg_ts_parser_prsstart - 1] =
// 				get_ts_parser_func(defel, Anum_pg_ts_parser_prsstart);
// 		}
// 		else if (pg_strcasecmp(defel->defname, "gettoken") == 0)
// 		{
// 			values[Anum_pg_ts_parser_prstoken - 1] =
// 				get_ts_parser_func(defel, Anum_pg_ts_parser_prstoken);
// 		}
// 		else if (pg_strcasecmp(defel->defname, "end") == 0)
// 		{
// 			values[Anum_pg_ts_parser_prsend - 1] =
// 				get_ts_parser_func(defel, Anum_pg_ts_parser_prsend);
// 		}
// 		else if (pg_strcasecmp(defel->defname, "headline") == 0)
// 		{
// 			values[Anum_pg_ts_parser_prsheadline - 1] =
// 				get_ts_parser_func(defel, Anum_pg_ts_parser_prsheadline);
// 		}
// 		else if (pg_strcasecmp(defel->defname, "lextypes") == 0)
// 		{
// 			values[Anum_pg_ts_parser_prslextype - 1] =
// 				get_ts_parser_func(defel, Anum_pg_ts_parser_prslextype);
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 				 errmsg("text search parser parameter \"%s\" not recognized",
// 						defel->defname)));
// 	}

// 	/*
// 	 * Validation
// 	 */
// 	if (!OidIsValid(DatumGetObjectId(values[Anum_pg_ts_parser_prsstart - 1])))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search parser start method is required")));

// 	if (!OidIsValid(DatumGetObjectId(values[Anum_pg_ts_parser_prstoken - 1])))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search parser gettoken method is required")));

// 	if (!OidIsValid(DatumGetObjectId(values[Anum_pg_ts_parser_prsend - 1])))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search parser end method is required")));

// 	if (!OidIsValid(DatumGetObjectId(values[Anum_pg_ts_parser_prslextype - 1])))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search parser lextypes method is required")));

// 	/*
// 	 * Looks good, insert
// 	 */
// 	prsRel = heap_open(TSParserRelationId, RowExclusiveLock);

// 	tup = heap_form_tuple(prsRel->rd_att, values, nulls);

// 	prsOid = simple_heap_insert(prsRel, tup);

// 	CatalogUpdateIndexes(prsRel, tup);

// 	address = makeParserDependencies(tup);

// 	/* Post creation hook for new text search parser */
// 	InvokeObjectPostCreateHook(TSParserRelationId, prsOid, 0);

// 	heap_freetuple(tup);

// 	heap_close(prsRel, RowExclusiveLock);

// 	return address;
// }
// //end DefineTSParser

// ObjectAddress
// DefineTSDictionary(List *names, List *parameters)
// {
// 	ListCell   *pl;
// 	Relation	dictRel;
// 	HeapTuple	tup;
// 	Datum		values[Natts_pg_ts_dict];
// 	bool		nulls[Natts_pg_ts_dict];
// 	NameData	dname;
// 	Oid			templId = InvalidOid;
// 	List	   *dictoptions = NIL;
// 	Oid			dictOid;
// 	Oid			namespaceoid;
// 	AclResult	aclresult;
// 	char	   *dictname;
// 	ObjectAddress address;

// 	/* Convert list of names to a name and namespace */
// 	namespaceoid = QualifiedNameGetCreationNamespace(names, &dictname);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(namespaceoid, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(namespaceoid));

// 	/*
// 	 * loop over the definition list and extract the information we need.
// 	 */
// 	foreach(pl, parameters)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);

// 		if (pg_strcasecmp(defel->defname, "template") == 0)
// 		{
// 			templId = get_ts_template_oid(defGetQualifiedName(defel), false);
// 		}
// 		else
// 		{
// 			/* Assume it's an option for the dictionary itself */
// 			dictoptions = lappend(dictoptions, defel);
// 		}
// 	}

// 	/*
// 	 * Validation
// 	 */
// 	if (!OidIsValid(templId))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search template is required")));

// 	verify_dictoptions(templId, dictoptions);

// 	/*
// 	 * Looks good, insert
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	namestrcpy(&dname, dictname);
// 	values[Anum_pg_ts_dict_dictname - 1] = NameGetDatum(&dname);
// 	values[Anum_pg_ts_dict_dictnamespace - 1] = ObjectIdGetDatum(namespaceoid);
// 	values[Anum_pg_ts_dict_dictowner - 1] = ObjectIdGetDatum(GetUserId());
// 	values[Anum_pg_ts_dict_dicttemplate - 1] = ObjectIdGetDatum(templId);
// 	if (dictoptions)
// 		values[Anum_pg_ts_dict_dictinitoption - 1] =
// 			PointerGetDatum(serialize_deflist(dictoptions));
// 	else
// 		nulls[Anum_pg_ts_dict_dictinitoption - 1] = true;

// 	dictRel = heap_open(TSDictionaryRelationId, RowExclusiveLock);

// 	tup = heap_form_tuple(dictRel->rd_att, values, nulls);

// 	dictOid = simple_heap_insert(dictRel, tup);

// 	CatalogUpdateIndexes(dictRel, tup);

// 	address = makeDictionaryDependencies(tup);

// 	/* Post creation hook for new text search dictionary */
// 	InvokeObjectPostCreateHook(TSDictionaryRelationId, dictOid, 0);

// 	heap_freetuple(tup);

// 	heap_close(dictRel, RowExclusiveLock);

// 	return address;
// }
// //end DefineTSDictionary

// ObjectAddress
// AlterTSDictionary(AlterTSDictionaryStmt *stmt)
// {
// 	HeapTuple	tup,
// 				newtup;
// 	Relation	rel;
// 	Oid			dictId;
// 	ListCell   *pl;
// 	List	   *dictoptions;
// 	Datum		opt;
// 	bool		isnull;
// 	Datum		repl_val[Natts_pg_ts_dict];
// 	bool		repl_null[Natts_pg_ts_dict];
// 	bool		repl_repl[Natts_pg_ts_dict];
// 	ObjectAddress address;

// 	dictId = get_ts_dict_oid(stmt->dictname, false);

// 	rel = heap_open(TSDictionaryRelationId, RowExclusiveLock);

// 	tup = SearchSysCache1(TSDICTOID, ObjectIdGetDatum(dictId));

// 	if (!HeapTupleIsValid(tup))
// 		elog(ERROR, "cache lookup failed for text search dictionary %u",
// 			 dictId);

// 	/* must be owner */
// 	if (!pg_ts_dict_ownercheck(dictId, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_TSDICTIONARY,
// 					   NameListToString(stmt->dictname));

// 	/* deserialize the existing set of options */
// 	opt = SysCacheGetAttr(TSDICTOID, tup,
// 						  Anum_pg_ts_dict_dictinitoption,
// 						  &isnull);
// 	if (isnull)
// 		dictoptions = NIL;
// 	else
// 		dictoptions = deserialize_deflist(opt);

// 	/*
// 	 * Modify the options list as per specified changes
// 	 */
// 	foreach(pl, stmt->options)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);
// 		ListCell   *cell;
// 		ListCell   *prev;
// 		ListCell   *next;

// 		/*
// 		 * Remove any matches ...
// 		 */
// 		prev = NULL;
// 		for (cell = list_head(dictoptions); cell; cell = next)
// 		{
// 			DefElem    *oldel = (DefElem *) lfirst(cell);

// 			next = lnext(cell);
// 			if (pg_strcasecmp(oldel->defname, defel->defname) == 0)
// 				dictoptions = list_delete_cell(dictoptions, cell, prev);
// 			else
// 				prev = cell;
// 		}

// 		/*
// 		 * and add new value if it's got one
// 		 */
// 		if (defel->arg)
// 			dictoptions = lappend(dictoptions, defel);
// 	}

// 	/*
// 	 * Validate
// 	 */
// 	verify_dictoptions(((Form_pg_ts_dict) GETSTRUCT(tup))->dicttemplate,
// 					   dictoptions);

// 	/*
// 	 * Looks good, update
// 	 */
// 	memset(repl_val, 0, sizeof(repl_val));
// 	memset(repl_null, false, sizeof(repl_null));
// 	memset(repl_repl, false, sizeof(repl_repl));

// 	if (dictoptions)
// 		repl_val[Anum_pg_ts_dict_dictinitoption - 1] =
// 			PointerGetDatum(serialize_deflist(dictoptions));
// 	else
// 		repl_null[Anum_pg_ts_dict_dictinitoption - 1] = true;
// 	repl_repl[Anum_pg_ts_dict_dictinitoption - 1] = true;

// 	newtup = heap_modify_tuple(tup, RelationGetDescr(rel),
// 							   repl_val, repl_null, repl_repl);

// 	simple_heap_update(rel, &newtup->t_self, newtup);

// 	CatalogUpdateIndexes(rel, newtup);

// 	InvokeObjectPostAlterHook(TSDictionaryRelationId, dictId, 0);

// 	ObjectAddressSet(address, TSDictionaryRelationId, dictId);

// 	/*
// 	 * NOTE: because we only support altering the options, not the template,
// 	 * there is no need to update dependencies.  This might have to change if
// 	 * the options ever reference inside-the-database objects.
// 	 */

// 	heap_freetuple(newtup);
// 	ReleaseSysCache(tup);

// 	heap_close(rel, RowExclusiveLock);

// 	return address;
// }
// //end 

// ObjectAddress
// DefineTSTemplate(List *names, List *parameters)
// {
// 	ListCell   *pl;
// 	Relation	tmplRel;
// 	HeapTuple	tup;
// 	Datum		values[Natts_pg_ts_template];
// 	bool		nulls[Natts_pg_ts_template];
// 	NameData	dname;
// 	int			i;
// 	Oid			tmplOid;
// 	Oid			namespaceoid;
// 	char	   *tmplname;
// 	ObjectAddress address;

// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 			   errmsg("must be superuser to create text search templates")));

// 	/* Convert list of names to a name and namespace */
// 	namespaceoid = QualifiedNameGetCreationNamespace(names, &tmplname);

// 	for (i = 0; i < Natts_pg_ts_template; i++)
// 	{
// 		nulls[i] = false;
// 		values[i] = ObjectIdGetDatum(InvalidOid);
// 	}

// 	namestrcpy(&dname, tmplname);
// 	values[Anum_pg_ts_template_tmplname - 1] = NameGetDatum(&dname);
// 	values[Anum_pg_ts_template_tmplnamespace - 1] = ObjectIdGetDatum(namespaceoid);

// 	/*
// 	 * loop over the definition list and extract the information we need.
// 	 */
// 	foreach(pl, parameters)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);

// 		if (pg_strcasecmp(defel->defname, "init") == 0)
// 		{
// 			values[Anum_pg_ts_template_tmplinit - 1] =
// 				get_ts_template_func(defel, Anum_pg_ts_template_tmplinit);
// 			nulls[Anum_pg_ts_template_tmplinit - 1] = false;
// 		}
// 		else if (pg_strcasecmp(defel->defname, "lexize") == 0)
// 		{
// 			values[Anum_pg_ts_template_tmpllexize - 1] =
// 				get_ts_template_func(defel, Anum_pg_ts_template_tmpllexize);
// 			nulls[Anum_pg_ts_template_tmpllexize - 1] = false;
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 			   errmsg("text search template parameter \"%s\" not recognized",
// 					  defel->defname)));
// 	}

// 	/*
// 	 * Validation
// 	 */
// 	if (!OidIsValid(DatumGetObjectId(values[Anum_pg_ts_template_tmpllexize - 1])))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search template lexize method is required")));

// 	/*
// 	 * Looks good, insert
// 	 */

// 	tmplRel = heap_open(TSTemplateRelationId, RowExclusiveLock);

// 	tup = heap_form_tuple(tmplRel->rd_att, values, nulls);

// 	tmplOid = simple_heap_insert(tmplRel, tup);

// 	CatalogUpdateIndexes(tmplRel, tup);

// 	address = makeTSTemplateDependencies(tup);

// 	/* Post creation hook for new text search template */
// 	InvokeObjectPostCreateHook(TSTemplateRelationId, tmplOid, 0);

// 	heap_freetuple(tup);

// 	heap_close(tmplRel, RowExclusiveLock);

// 	return address;
// }
// //end

// ObjectAddress
// DefineTSConfiguration(List *names, List *parameters, ObjectAddress *copied)
// {
// 	Relation	cfgRel;
// 	Relation	mapRel = NULL;
// 	HeapTuple	tup;
// 	Datum		values[Natts_pg_ts_config];
// 	bool		nulls[Natts_pg_ts_config];
// 	AclResult	aclresult;
// 	Oid			namespaceoid;
// 	char	   *cfgname;
// 	NameData	cname;
// 	Oid			sourceOid = InvalidOid;
// 	Oid			prsOid = InvalidOid;
// 	Oid			cfgOid;
// 	ListCell   *pl;
// 	ObjectAddress address;

// 	/* Convert list of names to a name and namespace */
// 	namespaceoid = QualifiedNameGetCreationNamespace(names, &cfgname);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(namespaceoid, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(namespaceoid));

// 	/*
// 	 * loop over the definition list and extract the information we need.
// 	 */
// 	foreach(pl, parameters)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(pl);

// 		if (pg_strcasecmp(defel->defname, "parser") == 0)
// 			prsOid = get_ts_parser_oid(defGetQualifiedName(defel), false);
// 		else if (pg_strcasecmp(defel->defname, "copy") == 0)
// 			sourceOid = get_ts_config_oid(defGetQualifiedName(defel), false);
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("text search configuration parameter \"%s\" not recognized",
// 							defel->defname)));
// 	}

// 	if (OidIsValid(sourceOid) && OidIsValid(prsOid))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_SYNTAX_ERROR),
// 				 errmsg("cannot specify both PARSER and COPY options")));

// 	/* make copied tsconfig available to callers */
// 	if (copied && OidIsValid(sourceOid))
// 	{
// 		ObjectAddressSet(*copied,
// 						 TSConfigRelationId,
// 						 sourceOid);
// 	}

// 	/*
// 	 * Look up source config if given.
// 	 */
// 	if (OidIsValid(sourceOid))
// 	{
// 		Form_pg_ts_config cfg;

// 		tup = SearchSysCache1(TSCONFIGOID, ObjectIdGetDatum(sourceOid));
// 		if (!HeapTupleIsValid(tup))
// 			elog(ERROR, "cache lookup failed for text search configuration %u",
// 				 sourceOid);

// 		cfg = (Form_pg_ts_config) GETSTRUCT(tup);

// 		/* use source's parser */
// 		prsOid = cfg->cfgparser;

// 		ReleaseSysCache(tup);
// 	}

// 	/*
// 	 * Validation
// 	 */
// 	if (!OidIsValid(prsOid))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("text search parser is required")));

// 	/*
// 	 * Looks good, build tuple and insert
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	namestrcpy(&cname, cfgname);
// 	values[Anum_pg_ts_config_cfgname - 1] = NameGetDatum(&cname);
// 	values[Anum_pg_ts_config_cfgnamespace - 1] = ObjectIdGetDatum(namespaceoid);
// 	values[Anum_pg_ts_config_cfgowner - 1] = ObjectIdGetDatum(GetUserId());
// 	values[Anum_pg_ts_config_cfgparser - 1] = ObjectIdGetDatum(prsOid);

// 	cfgRel = heap_open(TSConfigRelationId, RowExclusiveLock);

// 	tup = heap_form_tuple(cfgRel->rd_att, values, nulls);

// 	cfgOid = simple_heap_insert(cfgRel, tup);

// 	CatalogUpdateIndexes(cfgRel, tup);

// 	if (OidIsValid(sourceOid))
// 	{
// 		/*
// 		 * Copy token-dicts map from source config
// 		 */
// 		ScanKeyData skey;
// 		SysScanDesc scan;
// 		HeapTuple	maptup;

// 		mapRel = heap_open(TSConfigMapRelationId, RowExclusiveLock);

// 		ScanKeyInit(&skey,
// 					Anum_pg_ts_config_map_mapcfg,
// 					BTEqualStrategyNumber, F_OIDEQ,
// 					ObjectIdGetDatum(sourceOid));

// 		scan = systable_beginscan(mapRel, TSConfigMapIndexId, true,
// 								  NULL, 1, &skey);

// 		while (HeapTupleIsValid((maptup = systable_getnext(scan))))
// 		{
// 			Form_pg_ts_config_map cfgmap = (Form_pg_ts_config_map) GETSTRUCT(maptup);
// 			HeapTuple	newmaptup;
// 			Datum		mapvalues[Natts_pg_ts_config_map];
// 			bool		mapnulls[Natts_pg_ts_config_map];

// 			memset(mapvalues, 0, sizeof(mapvalues));
// 			memset(mapnulls, false, sizeof(mapnulls));

// 			mapvalues[Anum_pg_ts_config_map_mapcfg - 1] = cfgOid;
// 			mapvalues[Anum_pg_ts_config_map_maptokentype - 1] = cfgmap->maptokentype;
// 			mapvalues[Anum_pg_ts_config_map_mapseqno - 1] = cfgmap->mapseqno;
// 			mapvalues[Anum_pg_ts_config_map_mapdict - 1] = cfgmap->mapdict;

// 			newmaptup = heap_form_tuple(mapRel->rd_att, mapvalues, mapnulls);

// 			simple_heap_insert(mapRel, newmaptup);

// 			CatalogUpdateIndexes(mapRel, newmaptup);

// 			heap_freetuple(newmaptup);
// 		}

// 		systable_endscan(scan);
// 	}

// 	address = makeConfigurationDependencies(tup, false, mapRel);

// 	/* Post creation hook for new text search configuration */
// 	InvokeObjectPostCreateHook(TSConfigRelationId, cfgOid, 0);

// 	heap_freetuple(tup);

// 	if (mapRel)
// 		heap_close(mapRel, RowExclusiveLock);
// 	heap_close(cfgRel, RowExclusiveLock);

// 	return address;
// }
// //end

// ObjectAddress
// AlterTSConfiguration(AlterTSConfigurationStmt *stmt)
// {
// 	HeapTuple	tup;
// 	Oid			cfgId;
// 	Relation	relMap;
// 	ObjectAddress address;

// 	/* Find the configuration */
// 	tup = GetTSConfigTuple(stmt->cfgname);
// 	if (!HeapTupleIsValid(tup))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("text search configuration \"%s\" does not exist",
// 						NameListToString(stmt->cfgname))));

// 	cfgId = HeapTupleGetOid(tup);

// 	/* must be owner */
// 	if (!pg_ts_config_ownercheck(HeapTupleGetOid(tup), GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_TSCONFIGURATION,
// 					   NameListToString(stmt->cfgname));

// 	relMap = heap_open(TSConfigMapRelationId, RowExclusiveLock);

// 	/* Add or drop mappings */
// 	if (stmt->dicts)
// 		MakeConfigurationMapping(stmt, tup, relMap);
// 	else if (stmt->tokentype)
// 		DropConfigurationMapping(stmt, tup, relMap);

// 	/* Update dependencies */
// 	makeConfigurationDependencies(tup, true, relMap);

// 	InvokeObjectPostAlterHook(TSConfigRelationId,
// 							  HeapTupleGetOid(tup), 0);

// 	ObjectAddressSet(address, TSConfigRelationId, cfgId);

// 	heap_close(relMap, RowExclusiveLock);

// 	ReleaseSysCache(tup);

// 	return address;
// }
// //end

// /* commands/foreigncmds.c */
// ObjectAddress
// AlterForeignServerOwner(const char *name, Oid newOwnerId)
// {
// 	Oid			servOid;
// 	HeapTuple	tup;
// 	Relation	rel;
// 	ObjectAddress address;

// 	rel = heap_open(ForeignServerRelationId, RowExclusiveLock);

// 	tup = SearchSysCacheCopy1(FOREIGNSERVERNAME, CStringGetDatum(name));

// 	if (!HeapTupleIsValid(tup))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("server \"%s\" does not exist", name)));

// 	servOid = HeapTupleGetOid(tup);

// 	AlterForeignServerOwner_internal(rel, tup, newOwnerId);

// 	ObjectAddressSet(address, ForeignServerRelationId, servOid);

// 	heap_freetuple(tup);

// 	heap_close(rel, RowExclusiveLock);

// 	return address;
// }
// //end

// ObjectAddress
// AlterForeignDataWrapperOwner(const char *name, Oid newOwnerId)
// {
// 	Oid			fdwId;
// 	HeapTuple	tup;
// 	Relation	rel;
// 	ObjectAddress address;

// 	rel = heap_open(ForeignDataWrapperRelationId, RowExclusiveLock);

// 	tup = SearchSysCacheCopy1(FOREIGNDATAWRAPPERNAME, CStringGetDatum(name));

// 	if (!HeapTupleIsValid(tup))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("foreign-data wrapper \"%s\" does not exist", name)));

// 	fdwId = HeapTupleGetOid(tup);

// 	AlterForeignDataWrapperOwner_internal(rel, tup, newOwnerId);

// 	ObjectAddressSet(address, ForeignDataWrapperRelationId, fdwId);

// 	heap_freetuple(tup);

// 	heap_close(rel, RowExclusiveLock);

// 	return address;
// }
// //end

// ObjectAddress
// CreateForeignDataWrapper(CreateFdwStmt *stmt)
// {
// 	Relation	rel;
// 	Datum		values[Natts_pg_foreign_data_wrapper];
// 	bool		nulls[Natts_pg_foreign_data_wrapper];
// 	HeapTuple	tuple;
// 	Oid			fdwId;
// 	bool		handler_given;
// 	bool		validator_given;
// 	Oid			fdwhandler;
// 	Oid			fdwvalidator;
// 	Datum		fdwoptions;
// 	Oid			ownerId;
// 	ObjectAddress myself;
// 	ObjectAddress referenced;

// 	rel = heap_open(ForeignDataWrapperRelationId, RowExclusiveLock);

// 	/* Must be super user */
// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 			errmsg("permission denied to create foreign-data wrapper \"%s\"",
// 				   stmt->fdwname),
// 			errhint("Must be superuser to create a foreign-data wrapper.")));

// 	/* For now the owner cannot be specified on create. Use effective user ID. */
// 	ownerId = GetUserId();

// 	/*
// 	 * Check that there is no other foreign-data wrapper by this name.
// 	 */
// 	if (GetForeignDataWrapperByName(stmt->fdwname, true) != NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("foreign-data wrapper \"%s\" already exists",
// 						stmt->fdwname)));

// 	/*
// 	 * Insert tuple into pg_foreign_data_wrapper.
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_foreign_data_wrapper_fdwname - 1] =
// 		DirectFunctionCall1(namein, CStringGetDatum(stmt->fdwname));
// 	values[Anum_pg_foreign_data_wrapper_fdwowner - 1] = ObjectIdGetDatum(ownerId);

// 	/* Lookup handler and validator functions, if given */
// 	parse_func_options(stmt->func_options,
// 					   &handler_given, &fdwhandler,
// 					   &validator_given, &fdwvalidator);

// 	values[Anum_pg_foreign_data_wrapper_fdwhandler - 1] = ObjectIdGetDatum(fdwhandler);
// 	values[Anum_pg_foreign_data_wrapper_fdwvalidator - 1] = ObjectIdGetDatum(fdwvalidator);

// 	nulls[Anum_pg_foreign_data_wrapper_fdwacl - 1] = true;

// 	fdwoptions = transformGenericOptions(ForeignDataWrapperRelationId,
// 										 PointerGetDatum(NULL),
// 										 stmt->options,
// 										 fdwvalidator);

// 	if (PointerIsValid(DatumGetPointer(fdwoptions)))
// 		values[Anum_pg_foreign_data_wrapper_fdwoptions - 1] = fdwoptions;
// 	else
// 		nulls[Anum_pg_foreign_data_wrapper_fdwoptions - 1] = true;

// 	tuple = heap_form_tuple(rel->rd_att, values, nulls);

// 	fdwId = simple_heap_insert(rel, tuple);
// 	CatalogUpdateIndexes(rel, tuple);

// 	heap_freetuple(tuple);

// 	/* record dependencies */
// 	myself.classId = ForeignDataWrapperRelationId;
// 	myself.objectId = fdwId;
// 	myself.objectSubId = 0;

// 	if (OidIsValid(fdwhandler))
// 	{
// 		referenced.classId = ProcedureRelationId;
// 		referenced.objectId = fdwhandler;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 	}

// 	if (OidIsValid(fdwvalidator))
// 	{
// 		referenced.classId = ProcedureRelationId;
// 		referenced.objectId = fdwvalidator;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 	}

// 	recordDependencyOnOwner(ForeignDataWrapperRelationId, fdwId, ownerId);

// 	/* dependency on extension */
// 	recordDependencyOnCurrentExtension(&myself, false);

// 	/* Post creation hook for new foreign data wrapper */
// 	InvokeObjectPostCreateHook(ForeignDataWrapperRelationId, fdwId, 0);

// 	heap_close(rel, RowExclusiveLock);

// 	return myself;
// }
// //end

// ObjectAddress
// AlterForeignDataWrapper(AlterFdwStmt *stmt)
// {
// 	Relation	rel;
// 	HeapTuple	tp;
// 	Form_pg_foreign_data_wrapper fdwForm;
// 	Datum		repl_val[Natts_pg_foreign_data_wrapper];
// 	bool		repl_null[Natts_pg_foreign_data_wrapper];
// 	bool		repl_repl[Natts_pg_foreign_data_wrapper];
// 	Oid			fdwId;
// 	bool		isnull;
// 	Datum		datum;
// 	bool		handler_given;
// 	bool		validator_given;
// 	Oid			fdwhandler;
// 	Oid			fdwvalidator;
// 	ObjectAddress myself;

// 	rel = heap_open(ForeignDataWrapperRelationId, RowExclusiveLock);

// 	/* Must be super user */
// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 			 errmsg("permission denied to alter foreign-data wrapper \"%s\"",
// 					stmt->fdwname),
// 			 errhint("Must be superuser to alter a foreign-data wrapper.")));

// 	tp = SearchSysCacheCopy1(FOREIGNDATAWRAPPERNAME,
// 							 CStringGetDatum(stmt->fdwname));

// 	if (!HeapTupleIsValid(tp))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 		errmsg("foreign-data wrapper \"%s\" does not exist", stmt->fdwname)));

// 	fdwForm = (Form_pg_foreign_data_wrapper) GETSTRUCT(tp);
// 	fdwId = HeapTupleGetOid(tp);

// 	memset(repl_val, 0, sizeof(repl_val));
// 	memset(repl_null, false, sizeof(repl_null));
// 	memset(repl_repl, false, sizeof(repl_repl));

// 	parse_func_options(stmt->func_options,
// 					   &handler_given, &fdwhandler,
// 					   &validator_given, &fdwvalidator);

// 	if (handler_given)
// 	{
// 		repl_val[Anum_pg_foreign_data_wrapper_fdwhandler - 1] = ObjectIdGetDatum(fdwhandler);
// 		repl_repl[Anum_pg_foreign_data_wrapper_fdwhandler - 1] = true;

// 		/*
// 		 * It could be that the behavior of accessing foreign table changes
// 		 * with the new handler.  Warn about this.
// 		 */
// 		ereport(WARNING,
// 				(errmsg("changing the foreign-data wrapper handler can change behavior of existing foreign tables")));
// 	}

// 	if (validator_given)
// 	{
// 		repl_val[Anum_pg_foreign_data_wrapper_fdwvalidator - 1] = ObjectIdGetDatum(fdwvalidator);
// 		repl_repl[Anum_pg_foreign_data_wrapper_fdwvalidator - 1] = true;

// 		/*
// 		 * It could be that existing options for the FDW or dependent SERVER,
// 		 * USER MAPPING or FOREIGN TABLE objects are no longer valid according
// 		 * to the new validator.  Warn about this.
// 		 */
// 		if (OidIsValid(fdwvalidator))
// 			ereport(WARNING,
// 			 (errmsg("changing the foreign-data wrapper validator can cause "
// 					 "the options for dependent objects to become invalid")));
// 	}
// 	else
// 	{
// 		/*
// 		 * Validator is not changed, but we need it for validating options.
// 		 */
// 		fdwvalidator = fdwForm->fdwvalidator;
// 	}

// 	/*
// 	 * If options specified, validate and update.
// 	 */
// 	if (stmt->options)
// 	{
// 		/* Extract the current options */
// 		datum = SysCacheGetAttr(FOREIGNDATAWRAPPEROID,
// 								tp,
// 								Anum_pg_foreign_data_wrapper_fdwoptions,
// 								&isnull);
// 		if (isnull)
// 			datum = PointerGetDatum(NULL);

// 		/* Transform the options */
// 		datum = transformGenericOptions(ForeignDataWrapperRelationId,
// 										datum,
// 										stmt->options,
// 										fdwvalidator);

// 		if (PointerIsValid(DatumGetPointer(datum)))
// 			repl_val[Anum_pg_foreign_data_wrapper_fdwoptions - 1] = datum;
// 		else
// 			repl_null[Anum_pg_foreign_data_wrapper_fdwoptions - 1] = true;

// 		repl_repl[Anum_pg_foreign_data_wrapper_fdwoptions - 1] = true;
// 	}

// 	/* Everything looks good - update the tuple */
// 	tp = heap_modify_tuple(tp, RelationGetDescr(rel),
// 						   repl_val, repl_null, repl_repl);

// 	simple_heap_update(rel, &tp->t_self, tp);
// 	CatalogUpdateIndexes(rel, tp);

// 	heap_freetuple(tp);

// 	ObjectAddressSet(myself, ForeignDataWrapperRelationId, fdwId);

// 	/* Update function dependencies if we changed them */
// 	if (handler_given || validator_given)
// 	{
// 		ObjectAddress referenced;

// 		/*
// 		 * Flush all existing dependency records of this FDW on functions; we
// 		 * assume there can be none other than the ones we are fixing.
// 		 */
// 		deleteDependencyRecordsForClass(ForeignDataWrapperRelationId,
// 										fdwId,
// 										ProcedureRelationId,
// 										DEPENDENCY_NORMAL);

// 		/* And build new ones. */

// 		if (OidIsValid(fdwhandler))
// 		{
// 			referenced.classId = ProcedureRelationId;
// 			referenced.objectId = fdwhandler;
// 			referenced.objectSubId = 0;
// 			recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 		}

// 		if (OidIsValid(fdwvalidator))
// 		{
// 			referenced.classId = ProcedureRelationId;
// 			referenced.objectId = fdwvalidator;
// 			referenced.objectSubId = 0;
// 			recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 		}
// 	}

// 	InvokeObjectPostAlterHook(ForeignDataWrapperRelationId, fdwId, 0);

// 	heap_close(rel, RowExclusiveLock);

// 	return myself;
// }
// //end

// ObjectAddress
// CreateForeignServer(CreateForeignServerStmt *stmt)
// {
// 	Relation	rel;
// 	Datum		srvoptions;
// 	Datum		values[Natts_pg_foreign_server];
// 	bool		nulls[Natts_pg_foreign_server];
// 	HeapTuple	tuple;
// 	Oid			srvId;
// 	Oid			ownerId;
// 	AclResult	aclresult;
// 	ObjectAddress myself;
// 	ObjectAddress referenced;
// 	ForeignDataWrapper *fdw;

// 	rel = heap_open(ForeignServerRelationId, RowExclusiveLock);

// 	/* For now the owner cannot be specified on create. Use effective user ID. */
// 	ownerId = GetUserId();

// 	/*
// 	 * Check that there is no other foreign server by this name.
// 	 */
// 	if (GetForeignServerByName(stmt->servername, true) != NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("server \"%s\" already exists",
// 						stmt->servername)));

// 	/*
// 	 * Check that the FDW exists and that we have USAGE on it. Also get the
// 	 * actual FDW for option validation etc.
// 	 */
// 	fdw = GetForeignDataWrapperByName(stmt->fdwname, false);

// 	aclresult = pg_foreign_data_wrapper_aclcheck(fdw->fdwid, ownerId, ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_FDW, fdw->fdwname);

// 	/*
// 	 * Insert tuple into pg_foreign_server.
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_foreign_server_srvname - 1] =
// 		DirectFunctionCall1(namein, CStringGetDatum(stmt->servername));
// 	values[Anum_pg_foreign_server_srvowner - 1] = ObjectIdGetDatum(ownerId);
// 	values[Anum_pg_foreign_server_srvfdw - 1] = ObjectIdGetDatum(fdw->fdwid);

// 	/* Add server type if supplied */
// 	if (stmt->servertype)
// 		values[Anum_pg_foreign_server_srvtype - 1] =
// 			CStringGetTextDatum(stmt->servertype);
// 	else
// 		nulls[Anum_pg_foreign_server_srvtype - 1] = true;

// 	/* Add server version if supplied */
// 	if (stmt->version)
// 		values[Anum_pg_foreign_server_srvversion - 1] =
// 			CStringGetTextDatum(stmt->version);
// 	else
// 		nulls[Anum_pg_foreign_server_srvversion - 1] = true;

// 	/* Start with a blank acl */
// 	nulls[Anum_pg_foreign_server_srvacl - 1] = true;

// 	/* Add server options */
// 	srvoptions = transformGenericOptions(ForeignServerRelationId,
// 										 PointerGetDatum(NULL),
// 										 stmt->options,
// 										 fdw->fdwvalidator);

// 	if (PointerIsValid(DatumGetPointer(srvoptions)))
// 		values[Anum_pg_foreign_server_srvoptions - 1] = srvoptions;
// 	else
// 		nulls[Anum_pg_foreign_server_srvoptions - 1] = true;

// 	tuple = heap_form_tuple(rel->rd_att, values, nulls);

// 	srvId = simple_heap_insert(rel, tuple);

// 	CatalogUpdateIndexes(rel, tuple);

// 	heap_freetuple(tuple);

// 	/* record dependencies */
// 	myself.classId = ForeignServerRelationId;
// 	myself.objectId = srvId;
// 	myself.objectSubId = 0;

// 	referenced.classId = ForeignDataWrapperRelationId;
// 	referenced.objectId = fdw->fdwid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	recordDependencyOnOwner(ForeignServerRelationId, srvId, ownerId);

// 	/* dependency on extension */
// 	recordDependencyOnCurrentExtension(&myself, false);

// 	/* Post creation hook for new foreign server */
// 	InvokeObjectPostCreateHook(ForeignServerRelationId, srvId, 0);

// 	heap_close(rel, RowExclusiveLock);

// 	return myself;
// }
// //end

// ObjectAddress
// AlterForeignServer(AlterForeignServerStmt *stmt)
// {
// 	Relation	rel;
// 	HeapTuple	tp;
// 	Datum		repl_val[Natts_pg_foreign_server];
// 	bool		repl_null[Natts_pg_foreign_server];
// 	bool		repl_repl[Natts_pg_foreign_server];
// 	Oid			srvId;
// 	Form_pg_foreign_server srvForm;
// 	ObjectAddress address;

// 	rel = heap_open(ForeignServerRelationId, RowExclusiveLock);

// 	tp = SearchSysCacheCopy1(FOREIGNSERVERNAME,
// 							 CStringGetDatum(stmt->servername));

// 	if (!HeapTupleIsValid(tp))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("server \"%s\" does not exist", stmt->servername)));

// 	srvId = HeapTupleGetOid(tp);
// 	srvForm = (Form_pg_foreign_server) GETSTRUCT(tp);

// 	/*
// 	 * Only owner or a superuser can ALTER a SERVER.
// 	 */
// 	if (!pg_foreign_server_ownercheck(srvId, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_FOREIGN_SERVER,
// 					   stmt->servername);

// 	memset(repl_val, 0, sizeof(repl_val));
// 	memset(repl_null, false, sizeof(repl_null));
// 	memset(repl_repl, false, sizeof(repl_repl));

// 	if (stmt->has_version)
// 	{
// 		/*
// 		 * Change the server VERSION string.
// 		 */
// 		if (stmt->version)
// 			repl_val[Anum_pg_foreign_server_srvversion - 1] =
// 				CStringGetTextDatum(stmt->version);
// 		else
// 			repl_null[Anum_pg_foreign_server_srvversion - 1] = true;

// 		repl_repl[Anum_pg_foreign_server_srvversion - 1] = true;
// 	}

// 	if (stmt->options)
// 	{
// 		ForeignDataWrapper *fdw = GetForeignDataWrapper(srvForm->srvfdw);
// 		Datum		datum;
// 		bool		isnull;

// 		/* Extract the current srvoptions */
// 		datum = SysCacheGetAttr(FOREIGNSERVEROID,
// 								tp,
// 								Anum_pg_foreign_server_srvoptions,
// 								&isnull);
// 		if (isnull)
// 			datum = PointerGetDatum(NULL);

// 		/* Prepare the options array */
// 		datum = transformGenericOptions(ForeignServerRelationId,
// 										datum,
// 										stmt->options,
// 										fdw->fdwvalidator);

// 		if (PointerIsValid(DatumGetPointer(datum)))
// 			repl_val[Anum_pg_foreign_server_srvoptions - 1] = datum;
// 		else
// 			repl_null[Anum_pg_foreign_server_srvoptions - 1] = true;

// 		repl_repl[Anum_pg_foreign_server_srvoptions - 1] = true;
// 	}

// 	/* Everything looks good - update the tuple */
// 	tp = heap_modify_tuple(tp, RelationGetDescr(rel),
// 						   repl_val, repl_null, repl_repl);

// 	simple_heap_update(rel, &tp->t_self, tp);
// 	CatalogUpdateIndexes(rel, tp);

// 	InvokeObjectPostAlterHook(ForeignServerRelationId, srvId, 0);

// 	ObjectAddressSet(address, ForeignServerRelationId, srvId);

// 	heap_freetuple(tp);

// 	heap_close(rel, RowExclusiveLock);

// 	return address;
// }
// //end

// ObjectAddress
// CreateUserMapping(CreateUserMappingStmt *stmt)
// {
// 	Relation	rel;
// 	Datum		useoptions;
// 	Datum		values[Natts_pg_user_mapping];
// 	bool		nulls[Natts_pg_user_mapping];
// 	HeapTuple	tuple;
// 	Oid			useId;
// 	Oid			umId;
// 	ObjectAddress myself;
// 	ObjectAddress referenced;
// 	ForeignServer *srv;
// 	ForeignDataWrapper *fdw;
// 	RoleSpec   *role = (RoleSpec *) stmt->user;

// 	rel = heap_open(UserMappingRelationId, RowExclusiveLock);

// 	if (role->roletype == ROLESPEC_PUBLIC)
// 		useId = ACL_ID_PUBLIC;
// 	else
// 		useId = get_rolespec_oid(stmt->user, false);

// 	/* Check that the server exists. */
// 	srv = GetForeignServerByName(stmt->servername, false);

// 	user_mapping_ddl_aclcheck(useId, srv->serverid, stmt->servername);

// 	/*
// 	 * Check that the user mapping is unique within server.
// 	 */
// 	umId = GetSysCacheOid2(USERMAPPINGUSERSERVER,
// 						   ObjectIdGetDatum(useId),
// 						   ObjectIdGetDatum(srv->serverid));
// 	if (OidIsValid(umId))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("user mapping \"%s\" already exists for server %s",
// 						MappingUserName(useId),
// 						stmt->servername)));

// 	fdw = GetForeignDataWrapper(srv->fdwid);

// 	/*
// 	 * Insert tuple into pg_user_mapping.
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_user_mapping_umuser - 1] = ObjectIdGetDatum(useId);
// 	values[Anum_pg_user_mapping_umserver - 1] = ObjectIdGetDatum(srv->serverid);

// 	/* Add user options */
// 	useoptions = transformGenericOptions(UserMappingRelationId,
// 										 PointerGetDatum(NULL),
// 										 stmt->options,
// 										 fdw->fdwvalidator);

// 	if (PointerIsValid(DatumGetPointer(useoptions)))
// 		values[Anum_pg_user_mapping_umoptions - 1] = useoptions;
// 	else
// 		nulls[Anum_pg_user_mapping_umoptions - 1] = true;

// 	tuple = heap_form_tuple(rel->rd_att, values, nulls);

// 	umId = simple_heap_insert(rel, tuple);

// 	CatalogUpdateIndexes(rel, tuple);

// 	heap_freetuple(tuple);

// 	/* Add dependency on the server */
// 	myself.classId = UserMappingRelationId;
// 	myself.objectId = umId;
// 	myself.objectSubId = 0;

// 	referenced.classId = ForeignServerRelationId;
// 	referenced.objectId = srv->serverid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	if (OidIsValid(useId))
// 	{
// 		/* Record the mapped user dependency */
// 		recordDependencyOnOwner(UserMappingRelationId, umId, useId);
// 	}

// 	/* dependency on extension */
// 	recordDependencyOnCurrentExtension(&myself, false);

// 	/* Post creation hook for new user mapping */
// 	InvokeObjectPostCreateHook(UserMappingRelationId, umId, 0);

// 	heap_close(rel, RowExclusiveLock);

// 	return myself;
// }
// //end

// ObjectAddress
// AlterUserMapping(AlterUserMappingStmt *stmt)
// {
// 	Relation	rel;
// 	HeapTuple	tp;
// 	Datum		repl_val[Natts_pg_user_mapping];
// 	bool		repl_null[Natts_pg_user_mapping];
// 	bool		repl_repl[Natts_pg_user_mapping];
// 	Oid			useId;
// 	Oid			umId;
// 	ForeignServer *srv;
// 	ObjectAddress address;
// 	RoleSpec   *role = (RoleSpec *) stmt->user;

// 	rel = heap_open(UserMappingRelationId, RowExclusiveLock);

// 	if (role->roletype == ROLESPEC_PUBLIC)
// 		useId = ACL_ID_PUBLIC;
// 	else
// 		useId = get_rolespec_oid(stmt->user, false);

// 	srv = GetForeignServerByName(stmt->servername, false);

// 	umId = GetSysCacheOid2(USERMAPPINGUSERSERVER,
// 						   ObjectIdGetDatum(useId),
// 						   ObjectIdGetDatum(srv->serverid));
// 	if (!OidIsValid(umId))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("user mapping \"%s\" does not exist for the server",
// 						MappingUserName(useId))));

// 	user_mapping_ddl_aclcheck(useId, srv->serverid, stmt->servername);

// 	tp = SearchSysCacheCopy1(USERMAPPINGOID, ObjectIdGetDatum(umId));

// 	if (!HeapTupleIsValid(tp))
// 		elog(ERROR, "cache lookup failed for user mapping %u", umId);

// 	memset(repl_val, 0, sizeof(repl_val));
// 	memset(repl_null, false, sizeof(repl_null));
// 	memset(repl_repl, false, sizeof(repl_repl));

// 	if (stmt->options)
// 	{
// 		ForeignDataWrapper *fdw;
// 		Datum		datum;
// 		bool		isnull;

// 		/*
// 		 * Process the options.
// 		 */

// 		fdw = GetForeignDataWrapper(srv->fdwid);

// 		datum = SysCacheGetAttr(USERMAPPINGUSERSERVER,
// 								tp,
// 								Anum_pg_user_mapping_umoptions,
// 								&isnull);
// 		if (isnull)
// 			datum = PointerGetDatum(NULL);

// 		/* Prepare the options array */
// 		datum = transformGenericOptions(UserMappingRelationId,
// 										datum,
// 										stmt->options,
// 										fdw->fdwvalidator);

// 		if (PointerIsValid(DatumGetPointer(datum)))
// 			repl_val[Anum_pg_user_mapping_umoptions - 1] = datum;
// 		else
// 			repl_null[Anum_pg_user_mapping_umoptions - 1] = true;

// 		repl_repl[Anum_pg_user_mapping_umoptions - 1] = true;
// 	}

// 	/* Everything looks good - update the tuple */
// 	tp = heap_modify_tuple(tp, RelationGetDescr(rel),
// 						   repl_val, repl_null, repl_repl);

// 	simple_heap_update(rel, &tp->t_self, tp);
// 	CatalogUpdateIndexes(rel, tp);

// 	ObjectAddressSet(address, UserMappingRelationId, umId);

// 	heap_freetuple(tp);

// 	heap_close(rel, RowExclusiveLock);

// 	return address;
// }
// //end

// Oid
// RemoveUserMapping(DropUserMappingStmt *stmt)
// {
// 	ObjectAddress object;
// 	Oid			useId;
// 	Oid			umId;
// 	ForeignServer *srv;
// 	RoleSpec   *role = (RoleSpec *) stmt->user;

// 	if (role->roletype == ROLESPEC_PUBLIC)
// 		useId = ACL_ID_PUBLIC;
// 	else
// 	{
// 		useId = get_rolespec_oid(stmt->user, stmt->missing_ok);
// 		if (!OidIsValid(useId))
// 		{
// 			/*
// 			 * IF EXISTS specified, role not found and not public. Notice this
// 			 * and leave.
// 			 */
// 			elog(NOTICE, "role \"%s\" does not exist, skipping",
// 				 role->rolename);
// 			return InvalidOid;
// 		}
// 	}

// 	srv = GetForeignServerByName(stmt->servername, true);

// 	if (!srv)
// 	{
// 		if (!stmt->missing_ok)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 					 errmsg("server \"%s\" does not exist",
// 							stmt->servername)));
// 		/* IF EXISTS, just note it */
// 		ereport(NOTICE, (errmsg("server does not exist, skipping")));
// 		return InvalidOid;
// 	}

// 	umId = GetSysCacheOid2(USERMAPPINGUSERSERVER,
// 						   ObjectIdGetDatum(useId),
// 						   ObjectIdGetDatum(srv->serverid));

// 	if (!OidIsValid(umId))
// 	{
// 		if (!stmt->missing_ok)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				  errmsg("user mapping \"%s\" does not exist for the server",
// 						 MappingUserName(useId))));

// 		/* IF EXISTS specified, just note it */
// 		ereport(NOTICE,
// 		(errmsg("user mapping \"%s\" does not exist for the server, skipping",
// 				MappingUserName(useId))));
// 		return InvalidOid;
// 	}

// 	user_mapping_ddl_aclcheck(useId, srv->serverid, srv->servername);

// 	/*
// 	 * Do the deletion
// 	 */
// 	object.classId = UserMappingRelationId;
// 	object.objectId = umId;
// 	object.objectSubId = 0;

// 	performDeletion(&object, DROP_CASCADE, 0);

// 	return umId;
// }
// //end

// void
// ImportForeignSchema(ImportForeignSchemaStmt *stmt)
// {
// 	ForeignServer *server;
// 	ForeignDataWrapper *fdw;
// 	FdwRoutine *fdw_routine;
// 	AclResult	aclresult;
// 	List	   *cmd_list;
// 	ListCell   *lc;

// 	/* Check that the foreign server exists and that we have USAGE on it */
// 	server = GetForeignServerByName(stmt->server_name, false);
// 	aclresult = pg_foreign_server_aclcheck(server->serverid, GetUserId(), ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_FOREIGN_SERVER, server->servername);

// 	/* Check that the schema exists and we have CREATE permissions on it */
// 	(void) LookupCreationNamespace(stmt->local_schema);

// 	/* Get the FDW and check it supports IMPORT */
// 	fdw = GetForeignDataWrapper(server->fdwid);
// 	if (!OidIsValid(fdw->fdwhandler))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("foreign-data wrapper \"%s\" has no handler",
// 						fdw->fdwname)));
// 	fdw_routine = GetFdwRoutine(fdw->fdwhandler);
// 	if (fdw_routine->ImportForeignSchema == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FDW_NO_SCHEMAS),
// 				 errmsg("foreign-data wrapper \"%s\" does not support IMPORT FOREIGN SCHEMA",
// 						fdw->fdwname)));

// 	/* Call FDW to get a list of commands */
// 	cmd_list = fdw_routine->ImportForeignSchema(stmt, server->serverid);

// 	/* Parse and execute each command */
// 	foreach(lc, cmd_list)
// 	{
// 		char	   *cmd = (char *) lfirst(lc);
// 		import_error_callback_arg callback_arg;
// 		ErrorContextCallback sqlerrcontext;
// 		List	   *raw_parsetree_list;
// 		ListCell   *lc2;

// 		/*
// 		 * Setup error traceback support for ereport().  This is so that any
// 		 * error in the generated SQL will be displayed nicely.
// 		 */
// 		callback_arg.tablename = NULL;	/* not known yet */
// 		callback_arg.cmd = cmd;
// 		sqlerrcontext.callback = import_error_callback;
// 		sqlerrcontext.arg = (void *) &callback_arg;
// 		sqlerrcontext.previous = error_context_stack;
// 		error_context_stack = &sqlerrcontext;

// 		/*
// 		 * Parse the SQL string into a list of raw parse trees.
// 		 */
// 		raw_parsetree_list = pg_parse_query(cmd);

// 		/*
// 		 * Process each parse tree (we allow the FDW to put more than one
// 		 * command per string, though this isn't really advised).
// 		 */
// 		foreach(lc2, raw_parsetree_list)
// 		{
// 			CreateForeignTableStmt *cstmt = lfirst(lc2);

// 			/*
// 			 * Because we only allow CreateForeignTableStmt, we can skip parse
// 			 * analysis, rewrite, and planning steps here.
// 			 */
// 			if (!IsA(cstmt, CreateForeignTableStmt))
// 				elog(ERROR,
// 					 "foreign-data wrapper \"%s\" returned incorrect statement type %d",
// 					 fdw->fdwname, (int) nodeTag(cstmt));

// 			/* Ignore commands for tables excluded by filter options */
// 			if (!IsImportableForeignTable(cstmt->base.relation->relname, stmt))
// 				continue;

// 			/* Enable reporting of current table's name on error */
// 			callback_arg.tablename = cstmt->base.relation->relname;

// 			/* Ensure creation schema is the one given in IMPORT statement */
// 			cstmt->base.relation->schemaname = pstrdup(stmt->local_schema);

// 			/* Execute statement */
// 			ProcessUtility((Node *) cstmt,
// 						   cmd,
// 						   PROCESS_UTILITY_SUBCOMMAND, NULL,
// 						   None_Receiver, NULL);

// 			/* Be sure to advance the command counter between subcommands */
// 			CommandCounterIncrement();

// 			callback_arg.tablename = NULL;
// 		}

// 		error_context_stack = sqlerrcontext.previous;
// 	}
// }
// //end

// /* commands/dropcmds.c */
// void
// RemoveObjects(DropStmt *stmt)
// {
// 	ObjectAddresses *objects;
// 	ListCell   *cell1;
// 	ListCell   *cell2 = NULL;

// 	objects = new_object_addresses();

// 	foreach(cell1, stmt->objects)
// 	{
// 		ObjectAddress address;
// 		List	   *objname = lfirst(cell1);
// 		List	   *objargs = NIL;
// 		Relation	relation = NULL;
// 		Oid			namespaceId;

// 		if (stmt->arguments)
// 		{
// 			cell2 = (!cell2 ? list_head(stmt->arguments) : lnext(cell2));
// 			objargs = lfirst(cell2);
// 		}

// 		/* Get an ObjectAddress for the object. */
// 		address = get_object_address(stmt->removeType,
// 									 objname, objargs,
// 									 &relation,
// 									 AccessExclusiveLock,
// 									 stmt->missing_ok);

// 		/*
// 		 * Issue NOTICE if supplied object was not found.  Note this is only
// 		 * relevant in the missing_ok case, because otherwise
// 		 * get_object_address would have thrown an error.
// 		 */
// 		if (!OidIsValid(address.objectId))
// 		{
// 			Assert(stmt->missing_ok);
// 			does_not_exist_skipping(stmt->removeType, objname, objargs);
// 			continue;
// 		}

// 		/*
// 		 * Although COMMENT ON FUNCTION, SECURITY LABEL ON FUNCTION, etc. are
// 		 * happy to operate on an aggregate as on any other function, we have
// 		 * historically not allowed this for DROP FUNCTION.
// 		 */
// 		if (stmt->removeType == OBJECT_FUNCTION)
// 		{
// 			Oid			funcOid = address.objectId;
// 			HeapTuple	tup;

// 			tup = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcOid));
// 			if (!HeapTupleIsValid(tup)) /* should not happen */
// 				elog(ERROR, "cache lookup failed for function %u", funcOid);

// 			if (((Form_pg_proc) GETSTRUCT(tup))->proisagg)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 						 errmsg("\"%s\" is an aggregate function",
// 								NameListToString(objname)),
// 				errhint("Use DROP AGGREGATE to drop aggregate functions.")));

// 			ReleaseSysCache(tup);
// 		}

// 		/* Check permissions. */
// 		namespaceId = get_object_namespace(&address);
// 		if (!OidIsValid(namespaceId) ||
// 			!pg_namespace_ownercheck(namespaceId, GetUserId()))
// 			check_object_ownership(GetUserId(), stmt->removeType, address,
// 								   objname, objargs, relation);

// 		/* Release any relcache reference count, but keep lock until commit. */
// 		if (relation)
// 			heap_close(relation, NoLock);

// 		add_exact_object_address(&address, objects);
// 	}

// 	/* Here we really delete them. */
// 	performMultipleDeletions(objects, stmt->behavior, 0);

// 	free_object_addresses(objects);
// }
// //end

// /* commands/indexcmds.c */
// ObjectAddress
// DefineIndex(Oid relationId,
// 			IndexStmt *stmt,
// 			Oid indexRelationId,
// 			bool is_alter_table,
// 			bool check_rights,
// 			bool skip_build,
// 			bool quiet)
// {
// 	char	   *indexRelationName;
// 	char	   *accessMethodName;
// 	Oid		   *typeObjectId;
// 	Oid		   *collationObjectId;
// 	Oid		   *classObjectId;
// 	Oid			accessMethodId;
// 	Oid			namespaceId;
// 	Oid			tablespaceId;
// 	List	   *indexColNames;
// 	Relation	rel;
// 	Relation	indexRelation;
// 	HeapTuple	tuple;
// 	Form_pg_am	accessMethodForm;
// 	IndexAmRoutine *amRoutine;
// 	bool		amcanorder;
// 	amoptions_function amoptions;
// 	Datum		reloptions;
// 	int16	   *coloptions;
// 	IndexInfo  *indexInfo;
// 	int			numberOfAttributes;
// 	TransactionId limitXmin;
// 	VirtualTransactionId *old_snapshots;
// 	ObjectAddress address;
// 	int			n_old_snapshots;
// 	LockRelId	heaprelid;
// 	LOCKTAG		heaplocktag;
// 	LOCKMODE	lockmode;
// 	Snapshot	snapshot;
// 	int			i;

// 	/*
// 	 * count attributes in index
// 	 */
// 	numberOfAttributes = list_length(stmt->indexParams);
// 	if (numberOfAttributes <= 0)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("must specify at least one column")));
// 	if (numberOfAttributes > INDEX_MAX_KEYS)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_TOO_MANY_COLUMNS),
// 				 errmsg("cannot use more than %d columns in an index",
// 						INDEX_MAX_KEYS)));

// 	/*
// 	 * Only SELECT ... FOR UPDATE/SHARE are allowed while doing a standard
// 	 * index build; but for concurrent builds we allow INSERT/UPDATE/DELETE
// 	 * (but not VACUUM).
// 	 *
// 	 * NB: Caller is responsible for making sure that relationId refers to the
// 	 * relation on which the index should be built; except in bootstrap mode,
// 	 * this will typically require the caller to have already locked the
// 	 * relation.  To avoid lock upgrade hazards, that lock should be at least
// 	 * as strong as the one we take here.
// 	 */
// 	lockmode = stmt->concurrent ? ShareUpdateExclusiveLock : ShareLock;
// 	rel = heap_open(relationId, lockmode);

// 	relationId = RelationGetRelid(rel);
// 	namespaceId = RelationGetNamespace(rel);

// 	if (rel->rd_rel->relkind != RELKIND_RELATION &&
// 		rel->rd_rel->relkind != RELKIND_MATVIEW)
// 	{
// 		if (rel->rd_rel->relkind == RELKIND_FOREIGN_TABLE)

// 			/*
// 			 * Custom error message for FOREIGN TABLE since the term is close
// 			 * to a regular table and can confuse the user.
// 			 */
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("cannot create index on foreign table \"%s\"",
// 							RelationGetRelationName(rel))));
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is not a table or materialized view",
// 							RelationGetRelationName(rel))));
// 	}

// 	/*
// 	 * Don't try to CREATE INDEX on temp tables of other backends.
// 	 */
// 	if (RELATION_IS_OTHER_TEMP(rel))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("cannot create indexes on temporary tables of other sessions")));

// 	/*
// 	 * Verify we (still) have CREATE rights in the rel's namespace.
// 	 * (Presumably we did when the rel was created, but maybe not anymore.)
// 	 * Skip check if caller doesn't want it.  Also skip check if
// 	 * bootstrapping, since permissions machinery may not be working yet.
// 	 */
// 	if (check_rights && !IsBootstrapProcessingMode())
// 	{
// 		AclResult	aclresult;

// 		aclresult = pg_namespace_aclcheck(namespaceId, GetUserId(),
// 										  ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 						   get_namespace_name(namespaceId));
// 	}

// 	/*
// 	 * Select tablespace to use.  If not specified, use default tablespace
// 	 * (which may in turn default to database's default).
// 	 */
// 	if (stmt->tableSpace)
// 	{
// 		tablespaceId = get_tablespace_oid(stmt->tableSpace, false);
// 	}
// 	else
// 	{
// 		tablespaceId = GetDefaultTablespace(rel->rd_rel->relpersistence);
// 		/* note InvalidOid is OK in this case */
// 	}

// 	/* Check tablespace permissions */
// 	if (check_rights &&
// 		OidIsValid(tablespaceId) && tablespaceId != MyDatabaseTableSpace)
// 	{
// 		AclResult	aclresult;

// 		aclresult = pg_tablespace_aclcheck(tablespaceId, GetUserId(),
// 										   ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_TABLESPACE,
// 						   get_tablespace_name(tablespaceId));
// 	}

// 	/*
// 	 * Force shared indexes into the pg_global tablespace.  This is a bit of a
// 	 * hack but seems simpler than marking them in the BKI commands.  On the
// 	 * other hand, if it's not shared, don't allow it to be placed there.
// 	 */
// 	if (rel->rd_rel->relisshared)
// 		tablespaceId = GLOBALTABLESPACE_OID;
// 	else if (tablespaceId == GLOBALTABLESPACE_OID)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("only shared relations can be placed in pg_global tablespace")));

// 	/*
// 	 * Choose the index column names.
// 	 */
// 	indexColNames = ChooseIndexColumnNames(stmt->indexParams);

// 	/*
// 	 * Select name for index if caller didn't specify
// 	 */
// 	indexRelationName = stmt->idxname;
// 	if (indexRelationName == NULL)
// 		indexRelationName = ChooseIndexName(RelationGetRelationName(rel),
// 											namespaceId,
// 											indexColNames,
// 											stmt->excludeOpNames,
// 											stmt->primary,
// 											stmt->isconstraint);

// 	/*
// 	 * look up the access method, verify it can handle the requested features
// 	 */
// 	accessMethodName = stmt->accessMethod;
// 	tuple = SearchSysCache1(AMNAME, PointerGetDatum(accessMethodName));
// 	if (!HeapTupleIsValid(tuple))
// 	{
// 		/*
// 		 * Hack to provide more-or-less-transparent updating of old RTREE
// 		 * indexes to GiST: if RTREE is requested and not found, use GIST.
// 		 */
// 		if (strcmp(accessMethodName, "rtree") == 0)
// 		{
// 			ereport(NOTICE,
// 					(errmsg("substituting access method \"gist\" for obsolete method \"rtree\"")));
// 			accessMethodName = "gist";
// 			tuple = SearchSysCache1(AMNAME, PointerGetDatum(accessMethodName));
// 		}

// 		if (!HeapTupleIsValid(tuple))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 					 errmsg("access method \"%s\" does not exist",
// 							accessMethodName)));
// 	}
// 	accessMethodId = HeapTupleGetOid(tuple);
// 	accessMethodForm = (Form_pg_am) GETSTRUCT(tuple);
// 	amRoutine = GetIndexAmRoutine(accessMethodForm->amhandler);

// 	if (strcmp(accessMethodName, "hash") == 0 &&
// 		RelationNeedsWAL(rel))
// 		ereport(WARNING,
// 				(errmsg("hash indexes are not WAL-logged and their use is discouraged")));

// 	if (stmt->unique && !amRoutine->amcanunique)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 			   errmsg("access method \"%s\" does not support unique indexes",
// 					  accessMethodName)));
// 	if (numberOfAttributes > 1 && !amRoutine->amcanmulticol)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 		  errmsg("access method \"%s\" does not support multicolumn indexes",
// 				 accessMethodName)));
// 	if (stmt->excludeOpNames && amRoutine->amgettuple == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 		errmsg("access method \"%s\" does not support exclusion constraints",
// 			   accessMethodName)));

// 	amcanorder = amRoutine->amcanorder;
// 	amoptions = amRoutine->amoptions;

// 	pfree(amRoutine);
// 	ReleaseSysCache(tuple);

// 	/*
// 	 * Validate predicate, if given
// 	 */
// 	if (stmt->whereClause)
// 		CheckPredicate((Expr *) stmt->whereClause);

// 	/*
// 	 * Parse AM-specific options, convert to text array form, validate.
// 	 */
// 	reloptions = transformRelOptions((Datum) 0, stmt->options,
// 									 NULL, NULL, false, false);

// 	(void) index_reloptions(amoptions, reloptions, true);

// 	/*
// 	 * Prepare arguments for index_create, primarily an IndexInfo structure.
// 	 * Note that ii_Predicate must be in implicit-AND format.
// 	 */
// 	indexInfo = makeNode(IndexInfo);
// 	indexInfo->ii_NumIndexAttrs = numberOfAttributes;
// 	indexInfo->ii_Expressions = NIL;	/* for now */
// 	indexInfo->ii_ExpressionsState = NIL;
// 	indexInfo->ii_Predicate = make_ands_implicit((Expr *) stmt->whereClause);
// 	indexInfo->ii_PredicateState = NIL;
// 	indexInfo->ii_ExclusionOps = NULL;
// 	indexInfo->ii_ExclusionProcs = NULL;
// 	indexInfo->ii_ExclusionStrats = NULL;
// 	indexInfo->ii_Unique = stmt->unique;
// 	/* In a concurrent build, mark it not-ready-for-inserts */
// 	indexInfo->ii_ReadyForInserts = !stmt->concurrent;
// 	indexInfo->ii_Concurrent = stmt->concurrent;
// 	indexInfo->ii_BrokenHotChain = false;

// 	typeObjectId = (Oid *) palloc(numberOfAttributes * sizeof(Oid));
// 	collationObjectId = (Oid *) palloc(numberOfAttributes * sizeof(Oid));
// 	classObjectId = (Oid *) palloc(numberOfAttributes * sizeof(Oid));
// 	coloptions = (int16 *) palloc(numberOfAttributes * sizeof(int16));
// 	ComputeIndexAttrs(indexInfo,
// 					  typeObjectId, collationObjectId, classObjectId,
// 					  coloptions, stmt->indexParams,
// 					  stmt->excludeOpNames, relationId,
// 					  accessMethodName, accessMethodId,
// 					  amcanorder, stmt->isconstraint);

// 	/*
// 	 * Extra checks when creating a PRIMARY KEY index.
// 	 */
// 	if (stmt->primary)
// 		index_check_primary_key(rel, indexInfo, is_alter_table);

// 	/*
// 	 * We disallow indexes on system columns other than OID.  They would not
// 	 * necessarily get updated correctly, and they don't seem useful anyway.
// 	 */
// 	for (i = 0; i < indexInfo->ii_NumIndexAttrs; i++)
// 	{
// 		AttrNumber	attno = indexInfo->ii_KeyAttrNumbers[i];

// 		if (attno < 0 && attno != ObjectIdAttributeNumber)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 			   errmsg("index creation on system columns is not supported")));
// 	}

// 	/*
// 	 * Also check for system columns used in expressions or predicates.
// 	 */
// 	if (indexInfo->ii_Expressions || indexInfo->ii_Predicate)
// 	{
// 		Bitmapset  *indexattrs = NULL;

// 		pull_varattnos((Node *) indexInfo->ii_Expressions, 1, &indexattrs);
// 		pull_varattnos((Node *) indexInfo->ii_Predicate, 1, &indexattrs);

// 		for (i = FirstLowInvalidHeapAttributeNumber + 1; i < 0; i++)
// 		{
// 			if (i != ObjectIdAttributeNumber &&
// 				bms_is_member(i - FirstLowInvalidHeapAttributeNumber,
// 							  indexattrs))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				errmsg("index creation on system columns is not supported")));
// 		}
// 	}

// 	/*
// 	 * Report index creation if appropriate (delay this till after most of the
// 	 * error checks)
// 	 */
// 	if (stmt->isconstraint && !quiet)
// 	{
// 		const char *constraint_type;

// 		if (stmt->primary)
// 			constraint_type = "PRIMARY KEY";
// 		else if (stmt->unique)
// 			constraint_type = "UNIQUE";
// 		else if (stmt->excludeOpNames != NIL)
// 			constraint_type = "EXCLUDE";
// 		else
// 		{
// 			elog(ERROR, "unknown constraint type");
// 			constraint_type = NULL;		/* keep compiler quiet */
// 		}

// 		ereport(DEBUG1,
// 		  (errmsg("%s %s will create implicit index \"%s\" for table \"%s\"",
// 				  is_alter_table ? "ALTER TABLE / ADD" : "CREATE TABLE /",
// 				  constraint_type,
// 				  indexRelationName, RelationGetRelationName(rel))));
// 	}

// 	/*
// 	 * A valid stmt->oldNode implies that we already have a built form of the
// 	 * index.  The caller should also decline any index build.
// 	 */
// 	Assert(!OidIsValid(stmt->oldNode) || (skip_build && !stmt->concurrent));

// 	/*
// 	 * Make the catalog entries for the index, including constraints. Then, if
// 	 * not skip_build || concurrent, actually build the index.
// 	 */
// 	indexRelationId =
// 		index_create(rel, indexRelationName, indexRelationId, stmt->oldNode,
// 					 indexInfo, indexColNames,
// 					 accessMethodId, tablespaceId,
// 					 collationObjectId, classObjectId,
// 					 coloptions, reloptions, stmt->primary,
// 					 stmt->isconstraint, stmt->deferrable, stmt->initdeferred,
// 					 allowSystemTableMods,
// 					 skip_build || stmt->concurrent,
// 					 stmt->concurrent, !check_rights,
// 					 stmt->if_not_exists);

// 	ObjectAddressSet(address, RelationRelationId, indexRelationId);

// 	if (!OidIsValid(indexRelationId))
// 	{
// 		heap_close(rel, NoLock);
// 		return address;
// 	}

// 	/* Add any requested comment */
// 	if (stmt->idxcomment != NULL)
// 		CreateComments(indexRelationId, RelationRelationId, 0,
// 					   stmt->idxcomment);

// 	if (!stmt->concurrent)
// 	{
// 		/* Close the heap and we're done, in the non-concurrent case */
// 		heap_close(rel, NoLock);
// 		return address;
// 	}

// 	/* save lockrelid and locktag for below, then close rel */
// 	heaprelid = rel->rd_lockInfo.lockRelId;
// 	SET_LOCKTAG_RELATION(heaplocktag, heaprelid.dbId, heaprelid.relId);
// 	heap_close(rel, NoLock);

// 	/*
// 	 * For a concurrent build, it's important to make the catalog entries
// 	 * visible to other transactions before we start to build the index. That
// 	 * will prevent them from making incompatible HOT updates.  The new index
// 	 * will be marked not indisready and not indisvalid, so that no one else
// 	 * tries to either insert into it or use it for queries.
// 	 *
// 	 * We must commit our current transaction so that the index becomes
// 	 * visible; then start another.  Note that all the data structures we just
// 	 * built are lost in the commit.  The only data we keep past here are the
// 	 * relation IDs.
// 	 *
// 	 * Before committing, get a session-level lock on the table, to ensure
// 	 * that neither it nor the index can be dropped before we finish. This
// 	 * cannot block, even if someone else is waiting for access, because we
// 	 * already have the same lock within our transaction.
// 	 *
// 	 * Note: we don't currently bother with a session lock on the index,
// 	 * because there are no operations that could change its state while we
// 	 * hold lock on the parent table.  This might need to change later.
// 	 */
// 	LockRelationIdForSession(&heaprelid, ShareUpdateExclusiveLock);

// 	PopActiveSnapshot();
// 	CommitTransactionCommand();
// 	StartTransactionCommand();

// 	/*
// 	 * Phase 2 of concurrent index build (see comments for validate_index()
// 	 * for an overview of how this works)
// 	 *
// 	 * Now we must wait until no running transaction could have the table open
// 	 * with the old list of indexes.  Use ShareLock to consider running
// 	 * transactions that hold locks that permit writing to the table.  Note we
// 	 * do not need to worry about xacts that open the table for writing after
// 	 * this point; they will see the new index when they open it.
// 	 *
// 	 * Note: the reason we use actual lock acquisition here, rather than just
// 	 * checking the ProcArray and sleeping, is that deadlock is possible if
// 	 * one of the transactions in question is blocked trying to acquire an
// 	 * exclusive lock on our table.  The lock code will detect deadlock and
// 	 * error out properly.
// 	 */
// 	WaitForLockers(heaplocktag, ShareLock);

// 	/*
// 	 * At this moment we are sure that there are no transactions with the
// 	 * table open for write that don't have this new index in their list of
// 	 * indexes.  We have waited out all the existing transactions and any new
// 	 * transaction will have the new index in its list, but the index is still
// 	 * marked as "not-ready-for-inserts".  The index is consulted while
// 	 * deciding HOT-safety though.  This arrangement ensures that no new HOT
// 	 * chains can be created where the new tuple and the old tuple in the
// 	 * chain have different index keys.
// 	 *
// 	 * We now take a new snapshot, and build the index using all tuples that
// 	 * are visible in this snapshot.  We can be sure that any HOT updates to
// 	 * these tuples will be compatible with the index, since any updates made
// 	 * by transactions that didn't know about the index are now committed or
// 	 * rolled back.  Thus, each visible tuple is either the end of its
// 	 * HOT-chain or the extension of the chain is HOT-safe for this index.
// 	 */

// 	/* Open and lock the parent heap relation */
// 	rel = heap_openrv(stmt->relation, ShareUpdateExclusiveLock);

// 	/* And the target index relation */
// 	indexRelation = index_open(indexRelationId, RowExclusiveLock);

// 	/* Set ActiveSnapshot since functions in the indexes may need it */
// 	PushActiveSnapshot(GetTransactionSnapshot());

// 	/* We have to re-build the IndexInfo struct, since it was lost in commit */
// 	indexInfo = BuildIndexInfo(indexRelation);
// 	Assert(!indexInfo->ii_ReadyForInserts);
// 	indexInfo->ii_Concurrent = true;
// 	indexInfo->ii_BrokenHotChain = false;

// 	/* Now build the index */
// 	index_build(rel, indexRelation, indexInfo, stmt->primary, false);

// 	/* Close both the relations, but keep the locks */
// 	heap_close(rel, NoLock);
// 	index_close(indexRelation, NoLock);

// 	/*
// 	 * Update the pg_index row to mark the index as ready for inserts. Once we
// 	 * commit this transaction, any new transactions that open the table must
// 	 * insert new entries into the index for insertions and non-HOT updates.
// 	 */
// 	index_set_state_flags(indexRelationId, INDEX_CREATE_SET_READY);

// 	/* we can do away with our snapshot */
// 	PopActiveSnapshot();

// 	/*
// 	 * Commit this transaction to make the indisready update visible.
// 	 */
// 	CommitTransactionCommand();
// 	StartTransactionCommand();

// 	/*
// 	 * Phase 3 of concurrent index build
// 	 *
// 	 * We once again wait until no transaction can have the table open with
// 	 * the index marked as read-only for updates.
// 	 */
// 	WaitForLockers(heaplocktag, ShareLock);

// 	/*
// 	 * Now take the "reference snapshot" that will be used by validate_index()
// 	 * to filter candidate tuples.  Beware!  There might still be snapshots in
// 	 * use that treat some transaction as in-progress that our reference
// 	 * snapshot treats as committed.  If such a recently-committed transaction
// 	 * deleted tuples in the table, we will not include them in the index; yet
// 	 * those transactions which see the deleting one as still-in-progress will
// 	 * expect such tuples to be there once we mark the index as valid.
// 	 *
// 	 * We solve this by waiting for all endangered transactions to exit before
// 	 * we mark the index as valid.
// 	 *
// 	 * We also set ActiveSnapshot to this snap, since functions in indexes may
// 	 * need a snapshot.
// 	 */
// 	snapshot = RegisterSnapshot(GetTransactionSnapshot());
// 	PushActiveSnapshot(snapshot);

// 	/*
// 	 * Scan the index and the heap, insert any missing index entries.
// 	 */
// 	validate_index(relationId, indexRelationId, snapshot);

// 	/*
// 	 * Drop the reference snapshot.  We must do this before waiting out other
// 	 * snapshot holders, else we will deadlock against other processes also
// 	 * doing CREATE INDEX CONCURRENTLY, which would see our snapshot as one
// 	 * they must wait for.  But first, save the snapshot's xmin to use as
// 	 * limitXmin for GetCurrentVirtualXIDs().
// 	 */
// 	limitXmin = snapshot->xmin;

// 	PopActiveSnapshot();
// 	UnregisterSnapshot(snapshot);

// 	/*
// 	 * The index is now valid in the sense that it contains all currently
// 	 * interesting tuples.  But since it might not contain tuples deleted just
// 	 * before the reference snap was taken, we have to wait out any
// 	 * transactions that might have older snapshots.  Obtain a list of VXIDs
// 	 * of such transactions, and wait for them individually.
// 	 *
// 	 * We can exclude any running transactions that have xmin > the xmin of
// 	 * our reference snapshot; their oldest snapshot must be newer than ours.
// 	 * We can also exclude any transactions that have xmin = zero, since they
// 	 * evidently have no live snapshot at all (and any one they might be in
// 	 * process of taking is certainly newer than ours).  Transactions in other
// 	 * DBs can be ignored too, since they'll never even be able to see this
// 	 * index.
// 	 *
// 	 * We can also exclude autovacuum processes and processes running manual
// 	 * lazy VACUUMs, because they won't be fazed by missing index entries
// 	 * either.  (Manual ANALYZEs, however, can't be excluded because they
// 	 * might be within transactions that are going to do arbitrary operations
// 	 * later.)
// 	 *
// 	 * Also, GetCurrentVirtualXIDs never reports our own vxid, so we need not
// 	 * check for that.
// 	 *
// 	 * If a process goes idle-in-transaction with xmin zero, we do not need to
// 	 * wait for it anymore, per the above argument.  We do not have the
// 	 * infrastructure right now to stop waiting if that happens, but we can at
// 	 * least avoid the folly of waiting when it is idle at the time we would
// 	 * begin to wait.  We do this by repeatedly rechecking the output of
// 	 * GetCurrentVirtualXIDs.  If, during any iteration, a particular vxid
// 	 * doesn't show up in the output, we know we can forget about it.
// 	 */
// 	old_snapshots = GetCurrentVirtualXIDs(limitXmin, true, false,
// 										  PROC_IS_AUTOVACUUM | PROC_IN_VACUUM,
// 										  &n_old_snapshots);

// 	for (i = 0; i < n_old_snapshots; i++)
// 	{
// 		if (!VirtualTransactionIdIsValid(old_snapshots[i]))
// 			continue;			/* found uninteresting in previous cycle */

// 		if (i > 0)
// 		{
// 			/* see if anything's changed ... */
// 			VirtualTransactionId *newer_snapshots;
// 			int			n_newer_snapshots;
// 			int			j;
// 			int			k;

// 			newer_snapshots = GetCurrentVirtualXIDs(limitXmin,
// 													true, false,
// 										 PROC_IS_AUTOVACUUM | PROC_IN_VACUUM,
// 													&n_newer_snapshots);
// 			for (j = i; j < n_old_snapshots; j++)
// 			{
// 				if (!VirtualTransactionIdIsValid(old_snapshots[j]))
// 					continue;	/* found uninteresting in previous cycle */
// 				for (k = 0; k < n_newer_snapshots; k++)
// 				{
// 					if (VirtualTransactionIdEquals(old_snapshots[j],
// 												   newer_snapshots[k]))
// 						break;
// 				}
// 				if (k >= n_newer_snapshots)		/* not there anymore */
// 					SetInvalidVirtualTransactionId(old_snapshots[j]);
// 			}
// 			pfree(newer_snapshots);
// 		}

// 		if (VirtualTransactionIdIsValid(old_snapshots[i]))
// 			VirtualXactLock(old_snapshots[i], true);
// 	}

// 	/*
// 	 * Index can now be marked valid -- update its pg_index entry
// 	 */
// 	index_set_state_flags(indexRelationId, INDEX_CREATE_SET_VALID);

// 	/*
// 	 * The pg_index update will cause backends (including this one) to update
// 	 * relcache entries for the index itself, but we should also send a
// 	 * relcache inval on the parent table to force replanning of cached plans.
// 	 * Otherwise existing sessions might fail to use the new index where it
// 	 * would be useful.  (Note that our earlier commits did not create reasons
// 	 * to replan; so relcache flush on the index itself was sufficient.)
// 	 */
// 	CacheInvalidateRelcacheByRelid(heaprelid.relId);

// 	/*
// 	 * Last thing to do is release the session-level lock on the parent table.
// 	 */
// 	UnlockRelationIdForSession(&heaprelid, ShareUpdateExclusiveLock);

// 	return address;
// }
// //end

// Oid
// ReindexIndex(RangeVar *indexRelation, int options)
// {
// 	Oid			indOid;
// 	Oid			heapOid = InvalidOid;
// 	Relation	irel;
// 	char		persistence;

// 	/*
// 	 * Find and lock index, and check permissions on table; use callback to
// 	 * obtain lock on table first, to avoid deadlock hazard.  The lock level
// 	 * used here must match the index lock obtained in reindex_index().
// 	 */
// 	indOid = RangeVarGetRelidExtended(indexRelation, AccessExclusiveLock,
// 									  false, false,
// 									  RangeVarCallbackForReindexIndex,
// 									  (void *) &heapOid);

// 	/*
// 	 * Obtain the current persistence of the existing index.  We already hold
// 	 * lock on the index.
// 	 */
// 	irel = index_open(indOid, NoLock);
// 	persistence = irel->rd_rel->relpersistence;
// 	index_close(irel, NoLock);

// 	reindex_index(indOid, false, persistence, options);

// 	return indOid;
// }
// //end

// Oid
// ReindexTable(RangeVar *relation, int options)
// {
// 	Oid			heapOid;

// 	/* The lock level used here should match reindex_relation(). */
// 	heapOid = RangeVarGetRelidExtended(relation, ShareLock, false, false,
// 									   RangeVarCallbackOwnsTable, NULL);

// 	if (!reindex_relation(heapOid,
// 						  REINDEX_REL_PROCESS_TOAST |
// 						  REINDEX_REL_CHECK_CONSTRAINTS,
// 						  options))
// 		ereport(NOTICE,
// 				(errmsg("table \"%s\" has no indexes",
// 						relation->relname)));

// 	return heapOid;
// }
// //end

// char *
// makeObjectName(const char *name1, const char *name2, const char *label)
// {
// 	char	   *name;
// 	int			overhead = 0;	/* chars needed for label and underscores */
// 	int			availchars;		/* chars available for name(s) */
// 	int			name1chars;		/* chars allocated to name1 */
// 	int			name2chars;		/* chars allocated to name2 */
// 	int			ndx;

// 	name1chars = strlen(name1);
// 	if (name2)
// 	{
// 		name2chars = strlen(name2);
// 		overhead++;				/* allow for separating underscore */
// 	}
// 	else
// 		name2chars = 0;
// 	if (label)
// 		overhead += strlen(label) + 1;

// 	availchars = NAMEDATALEN - 1 - overhead;
// 	Assert(availchars > 0);		/* else caller chose a bad label */

// 	/*
// 	 * If we must truncate,  preferentially truncate the longer name. This
// 	 * logic could be expressed without a loop, but it's simple and obvious as
// 	 * a loop.
// 	 */
// 	while (name1chars + name2chars > availchars)
// 	{
// 		if (name1chars > name2chars)
// 			name1chars--;
// 		else
// 			name2chars--;
// 	}

// 	name1chars = pg_mbcliplen(name1, name1chars, name1chars);
// 	if (name2)
// 		name2chars = pg_mbcliplen(name2, name2chars, name2chars);

// 	/* Now construct the string using the chosen lengths */
// 	name = palloc(name1chars + name2chars + overhead + 1);
// 	memcpy(name, name1, name1chars);
// 	ndx = name1chars;
// 	if (name2)
// 	{
// 		name[ndx++] = '_';
// 		memcpy(name + ndx, name2, name2chars);
// 		ndx += name2chars;
// 	}
// 	if (label)
// 	{
// 		name[ndx++] = '_';
// 		strcpy(name + ndx, label);
// 	}
// 	else
// 		name[ndx] = '\0';

// 	return name;
// }
// //end

// char *
// ChooseRelationName(const char *name1, const char *name2,
// 				   const char *label, Oid namespaceid)
// {
// 	int			pass = 0;
// 	char	   *relname = NULL;
// 	char		modlabel[NAMEDATALEN];

// 	/* try the unmodified label first */
// 	StrNCpy(modlabel, label, sizeof(modlabel));

// 	for (;;)
// 	{
// 		relname = makeObjectName(name1, name2, modlabel);

// 		if (!OidIsValid(get_relname_relid(relname, namespaceid)))
// 			break;

// 		/* found a conflict, so try a new name component */
// 		pfree(relname);
// 		snprintf(modlabel, sizeof(modlabel), "%s%d", label, ++pass);
// 	}

// 	return relname;
// }
// //end
// void
// ReindexMultipleTables(const char *objectName, ReindexObjectType objectKind,
// 					  int options)
// {
// 	Oid			objectOid;
// 	Relation	relationRelation;
// 	HeapScanDesc scan;
// 	ScanKeyData scan_keys[1];
// 	HeapTuple	tuple;
// 	MemoryContext private_context;
// 	MemoryContext old;
// 	List	   *relids = NIL;
// 	ListCell   *l;
// 	int			num_keys;

// 	AssertArg(objectName);
// 	Assert(objectKind == REINDEX_OBJECT_SCHEMA ||
// 		   objectKind == REINDEX_OBJECT_SYSTEM ||
// 		   objectKind == REINDEX_OBJECT_DATABASE);

// 	/*
// 	 * Get OID of object to reindex, being the database currently being used
// 	 * by session for a database or for system catalogs, or the schema defined
// 	 * by caller. At the same time do permission checks that need different
// 	 * processing depending on the object type.
// 	 */
// 	if (objectKind == REINDEX_OBJECT_SCHEMA)
// 	{
// 		objectOid = get_namespace_oid(objectName, false);

// 		if (!pg_namespace_ownercheck(objectOid, GetUserId()))
// 			aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_NAMESPACE,
// 						   objectName);
// 	}
// 	else
// 	{
// 		objectOid = MyDatabaseId;

// 		if (strcmp(objectName, get_database_name(objectOid)) != 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("can only reindex the currently open database")));
// 		if (!pg_database_ownercheck(objectOid, GetUserId()))
// 			aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_DATABASE,
// 						   objectName);
// 	}

// 	/*
// 	 * Create a memory context that will survive forced transaction commits we
// 	 * do below.  Since it is a child of PortalContext, it will go away
// 	 * eventually even if we suffer an error; there's no need for special
// 	 * abort cleanup logic.
// 	 */
// 	private_context = AllocSetContextCreate(PortalContext,
// 											"ReindexMultipleTables",
// 											ALLOCSET_SMALL_SIZES);

// 	/*
// 	 * Define the search keys to find the objects to reindex. For a schema, we
// 	 * select target relations using relnamespace, something not necessary for
// 	 * a database-wide operation.
// 	 */
// 	if (objectKind == REINDEX_OBJECT_SCHEMA)
// 	{
// 		num_keys = 1;
// 		ScanKeyInit(&scan_keys[0],
// 					Anum_pg_class_relnamespace,
// 					BTEqualStrategyNumber, F_OIDEQ,
// 					ObjectIdGetDatum(objectOid));
// 	}
// 	else
// 		num_keys = 0;

// 	/*
// 	 * Scan pg_class to build a list of the relations we need to reindex.
// 	 *
// 	 * We only consider plain relations and materialized views here (toast
// 	 * rels will be processed indirectly by reindex_relation).
// 	 */
// 	relationRelation = heap_open(RelationRelationId, AccessShareLock);
// 	scan = heap_beginscan_catalog(relationRelation, num_keys, scan_keys);
// 	while ((tuple = heap_getnext(scan, ForwardScanDirection)) != NULL)
// 	{
// 		Form_pg_class classtuple = (Form_pg_class) GETSTRUCT(tuple);
// 		Oid			relid = HeapTupleGetOid(tuple);

// 		/*
// 		 * Only regular tables and matviews can have indexes, so ignore any
// 		 * other kind of relation.
// 		 */
// 		if (classtuple->relkind != RELKIND_RELATION &&
// 			classtuple->relkind != RELKIND_MATVIEW)
// 			continue;

// 		/* Skip temp tables of other backends; we can't reindex them at all */
// 		if (classtuple->relpersistence == RELPERSISTENCE_TEMP &&
// 			!isTempNamespace(classtuple->relnamespace))
// 			continue;

// 		/* Check user/system classification, and optionally skip */
// 		if (objectKind == REINDEX_OBJECT_SYSTEM &&
// 			!IsSystemClass(relid, classtuple))
// 			continue;

// 		/* Save the list of relation OIDs in private context */
// 		old = MemoryContextSwitchTo(private_context);

// 		/*
// 		 * We always want to reindex pg_class first if it's selected to be
// 		 * reindexed.  This ensures that if there is any corruption in
// 		 * pg_class' indexes, they will be fixed before we process any other
// 		 * tables.  This is critical because reindexing itself will try to
// 		 * update pg_class.
// 		 */
// 		if (relid == RelationRelationId)
// 			relids = lcons_oid(relid, relids);
// 		else
// 			relids = lappend_oid(relids, relid);

// 		MemoryContextSwitchTo(old);
// 	}
// 	heap_endscan(scan);
// 	heap_close(relationRelation, AccessShareLock);

// 	/* Now reindex each rel in a separate transaction */
// 	PopActiveSnapshot();
// 	CommitTransactionCommand();
// 	foreach(l, relids)
// 	{
// 		Oid			relid = lfirst_oid(l);

// 		StartTransactionCommand();
// 		/* functions in indexes may want a snapshot set */
// 		PushActiveSnapshot(GetTransactionSnapshot());
// 		if (reindex_relation(relid,
// 							 REINDEX_REL_PROCESS_TOAST |
// 							 REINDEX_REL_CHECK_CONSTRAINTS,
// 							 options))

// 			if (options & REINDEXOPT_VERBOSE)
// 				ereport(INFO,
// 						(errmsg("table \"%s.%s\" was reindexed",
// 								get_namespace_name(get_rel_namespace(relid)),
// 								get_rel_name(relid))));
// 		PopActiveSnapshot();
// 		CommitTransactionCommand();
// 	}
// 	StartTransactionCommand();

// 	MemoryContextDelete(private_context);
// }
// //end

// /* commands/functioncmds.c */
// ObjectAddress
// CreateFunction(CreateFunctionStmt *stmt, const char *queryString)
// {
// 	char	   *probin_str;
// 	char	   *prosrc_str;
// 	Oid			prorettype;
// 	bool		returnsSet;
// 	char	   *language;
// 	Oid			languageOid;
// 	Oid			languageValidator;
// 	Node	   *transformDefElem = NULL;
// 	char	   *funcname;
// 	Oid			namespaceId;
// 	AclResult	aclresult;
// 	oidvector  *parameterTypes;
// 	ArrayType  *allParameterTypes;
// 	ArrayType  *parameterModes;
// 	ArrayType  *parameterNames;
// 	List	   *parameterDefaults;
// 	Oid			variadicArgType;
// 	List	   *trftypes_list = NIL;
// 	ArrayType  *trftypes;
// 	Oid			requiredResultType;
// 	bool		isWindowFunc,
// 				isStrict,
// 				security,
// 				isLeakProof;
// 	char		volatility;
// 	ArrayType  *proconfig;
// 	float4		procost;
// 	float4		prorows;
// 	HeapTuple	languageTuple;
// 	Form_pg_language languageStruct;
// 	List	   *as_clause;
// 	char		parallel;

// 	/* Convert list of names to a name and namespace */
// 	namespaceId = QualifiedNameGetCreationNamespace(stmt->funcname,
// 													&funcname);

// 	/* Check we have creation rights in target namespace */
// 	aclresult = pg_namespace_aclcheck(namespaceId, GetUserId(), ACL_CREATE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_NAMESPACE,
// 					   get_namespace_name(namespaceId));

// 	/* default attributes */
// 	isWindowFunc = false;
// 	isStrict = false;
// 	security = false;
// 	isLeakProof = false;
// 	volatility = PROVOLATILE_VOLATILE;
// 	proconfig = NULL;
// 	procost = -1;				/* indicates not set */
// 	prorows = -1;				/* indicates not set */
// 	parallel = PROPARALLEL_UNSAFE;

// 	/* override attributes from explicit list */
// 	compute_attributes_sql_style(stmt->options,
// 								 &as_clause, &language, &transformDefElem,
// 								 &isWindowFunc, &volatility,
// 								 &isStrict, &security, &isLeakProof,
// 								 &proconfig, &procost, &prorows, &parallel);

// 	/* Look up the language and validate permissions */
// 	languageTuple = SearchSysCache1(LANGNAME, PointerGetDatum(language));
// 	if (!HeapTupleIsValid(languageTuple))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("language \"%s\" does not exist", language),
// 				 (PLTemplateExists(language) ?
// 				  errhint("Use CREATE LANGUAGE to load the language into the database.") : 0)));

// 	languageOid = HeapTupleGetOid(languageTuple);
// 	languageStruct = (Form_pg_language) GETSTRUCT(languageTuple);

// 	if (languageStruct->lanpltrusted)
// 	{
// 		/* if trusted language, need USAGE privilege */
// 		AclResult	aclresult;

// 		aclresult = pg_language_aclcheck(languageOid, GetUserId(), ACL_USAGE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_LANGUAGE,
// 						   NameStr(languageStruct->lanname));
// 	}
// 	else
// 	{
// 		/* if untrusted language, must be superuser */
// 		if (!superuser())
// 			aclcheck_error(ACLCHECK_NO_PRIV, ACL_KIND_LANGUAGE,
// 						   NameStr(languageStruct->lanname));
// 	}

// 	languageValidator = languageStruct->lanvalidator;

// 	ReleaseSysCache(languageTuple);

// 	/*
// 	 * Only superuser is allowed to create leakproof functions because
// 	 * leakproof functions can see tuples which have not yet been filtered out
// 	 * by security barrier views or row level security policies.
// 	 */
// 	if (isLeakProof && !superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("only superuser can define a leakproof function")));

// 	if (transformDefElem)
// 	{
// 		ListCell   *lc;

// 		Assert(IsA(transformDefElem, List));

// 		foreach(lc, (List *) transformDefElem)
// 		{
// 			Oid			typeid = typenameTypeId(NULL, lfirst(lc));
// 			Oid			elt = get_base_element_type(typeid);

// 			typeid = elt ? elt : typeid;

// 			get_transform_oid(typeid, languageOid, false);
// 			trftypes_list = lappend_oid(trftypes_list, typeid);
// 		}
// 	}

// 	/*
// 	 * Convert remaining parameters of CREATE to form wanted by
// 	 * ProcedureCreate.
// 	 */
// 	interpret_function_parameter_list(stmt->parameters,
// 									  languageOid,
// 									  false,	/* not an aggregate */
// 									  queryString,
// 									  &parameterTypes,
// 									  &allParameterTypes,
// 									  &parameterModes,
// 									  &parameterNames,
// 									  &parameterDefaults,
// 									  &variadicArgType,
// 									  &requiredResultType);

// 	if (stmt->returnType)
// 	{
// 		/* explicit RETURNS clause */
// 		compute_return_type(stmt->returnType, languageOid,
// 							&prorettype, &returnsSet);
// 		if (OidIsValid(requiredResultType) && prorettype != requiredResultType)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 					 errmsg("function result type must be %s because of OUT parameters",
// 							format_type_be(requiredResultType))));
// 	}
// 	else if (OidIsValid(requiredResultType))
// 	{
// 		/* default RETURNS clause from OUT parameters */
// 		prorettype = requiredResultType;
// 		returnsSet = false;
// 	}
// 	else
// 	{
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				 errmsg("function result type must be specified")));
// 		/* Alternative possibility: default to RETURNS VOID */
// 		prorettype = VOIDOID;
// 		returnsSet = false;
// 	}

// 	if (list_length(trftypes_list) > 0)
// 	{
// 		ListCell   *lc;
// 		Datum	   *arr;
// 		int			i;

// 		arr = palloc(list_length(trftypes_list) * sizeof(Datum));
// 		i = 0;
// 		foreach(lc, trftypes_list)
// 			arr[i++] = ObjectIdGetDatum(lfirst_oid(lc));
// 		trftypes = construct_array(arr, list_length(trftypes_list),
// 								   OIDOID, sizeof(Oid), true, 'i');
// 	}
// 	else
// 	{
// 		/* store SQL NULL instead of empty array */
// 		trftypes = NULL;
// 	}

// 	compute_attributes_with_style(stmt->withClause, &isStrict, &volatility);

// 	interpret_AS_clause(languageOid, language, funcname, as_clause,
// 						&prosrc_str, &probin_str);

// 	/*
// 	 * Set default values for COST and ROWS depending on other parameters;
// 	 * reject ROWS if it's not returnsSet.  NB: pg_dump knows these default
// 	 * values, keep it in sync if you change them.
// 	 */
// 	if (procost < 0)
// 	{
// 		/* SQL and PL-language functions are assumed more expensive */
// 		if (languageOid == INTERNALlanguageId ||
// 			languageOid == ClanguageId)
// 			procost = 1;
// 		else
// 			procost = 100;
// 	}
// 	if (prorows < 0)
// 	{
// 		if (returnsSet)
// 			prorows = 1000;
// 		else
// 			prorows = 0;		/* dummy value if not returnsSet */
// 	}
// 	else if (!returnsSet)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("ROWS is not applicable when function does not return a set")));

// 	/*
// 	 * And now that we have all the parameters, and know we're permitted to do
// 	 * so, go ahead and create the function.
// 	 */
// 	return ProcedureCreate(funcname,
// 						   namespaceId,
// 						   stmt->replace,
// 						   returnsSet,
// 						   prorettype,
// 						   GetUserId(),
// 						   languageOid,
// 						   languageValidator,
// 						   prosrc_str,	/* converted to text later */
// 						   probin_str,	/* converted to text later */
// 						   false,		/* not an aggregate */
// 						   isWindowFunc,
// 						   security,
// 						   isLeakProof,
// 						   isStrict,
// 						   volatility,
// 						   parallel,
// 						   parameterTypes,
// 						   PointerGetDatum(allParameterTypes),
// 						   PointerGetDatum(parameterModes),
// 						   PointerGetDatum(parameterNames),
// 						   parameterDefaults,
// 						   PointerGetDatum(trftypes),
// 						   PointerGetDatum(proconfig),
// 						   procost,
// 						   prorows);
// }
// //end

// ObjectAddress
// AlterFunction(AlterFunctionStmt *stmt)
// {
// 	HeapTuple	tup;
// 	Oid			funcOid;
// 	Form_pg_proc procForm;
// 	Relation	rel;
// 	ListCell   *l;
// 	DefElem    *volatility_item = NULL;
// 	DefElem    *strict_item = NULL;
// 	DefElem    *security_def_item = NULL;
// 	DefElem    *leakproof_item = NULL;
// 	List	   *set_items = NIL;
// 	DefElem    *cost_item = NULL;
// 	DefElem    *rows_item = NULL;
// 	DefElem    *parallel_item = NULL;
// 	ObjectAddress address;

// 	rel = heap_open(ProcedureRelationId, RowExclusiveLock);

// 	funcOid = LookupFuncNameTypeNames(stmt->func->funcname,
// 									  stmt->func->funcargs,
// 									  false);

// 	tup = SearchSysCacheCopy1(PROCOID, ObjectIdGetDatum(funcOid));
// 	if (!HeapTupleIsValid(tup)) /* should not happen */
// 		elog(ERROR, "cache lookup failed for function %u", funcOid);

// 	procForm = (Form_pg_proc) GETSTRUCT(tup);

// 	/* Permission check: must own function */
// 	if (!pg_proc_ownercheck(funcOid, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_PROC,
// 					   NameListToString(stmt->func->funcname));

// 	if (procForm->proisagg)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("\"%s\" is an aggregate function",
// 						NameListToString(stmt->func->funcname))));

// 	/* Examine requested actions. */
// 	foreach(l, stmt->actions)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(l);

// 		if (compute_common_attribute(defel,
// 									 &volatility_item,
// 									 &strict_item,
// 									 &security_def_item,
// 									 &leakproof_item,
// 									 &set_items,
// 									 &cost_item,
// 									 &rows_item,
// 									 &parallel_item) == false)
// 			elog(ERROR, "option \"%s\" not recognized", defel->defname);
// 	}

// 	if (volatility_item)
// 		procForm->provolatile = interpret_func_volatility(volatility_item);
// 	if (strict_item)
// 		procForm->proisstrict = intVal(strict_item->arg);
// 	if (security_def_item)
// 		procForm->prosecdef = intVal(security_def_item->arg);
// 	if (leakproof_item)
// 	{
// 		procForm->proleakproof = intVal(leakproof_item->arg);
// 		if (procForm->proleakproof && !superuser())
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				  errmsg("only superuser can define a leakproof function")));
// 	}
// 	if (cost_item)
// 	{
// 		procForm->procost = defGetNumeric(cost_item);
// 		if (procForm->procost <= 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("COST must be positive")));
// 	}
// 	if (rows_item)
// 	{
// 		procForm->prorows = defGetNumeric(rows_item);
// 		if (procForm->prorows <= 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("ROWS must be positive")));
// 		if (!procForm->proretset)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 					 errmsg("ROWS is not applicable when function does not return a set")));
// 	}
// 	if (set_items)
// 	{
// 		Datum		datum;
// 		bool		isnull;
// 		ArrayType  *a;
// 		Datum		repl_val[Natts_pg_proc];
// 		bool		repl_null[Natts_pg_proc];
// 		bool		repl_repl[Natts_pg_proc];

// 		/* extract existing proconfig setting */
// 		datum = SysCacheGetAttr(PROCOID, tup, Anum_pg_proc_proconfig, &isnull);
// 		a = isnull ? NULL : DatumGetArrayTypeP(datum);

// 		/* update according to each SET or RESET item, left to right */
// 		a = update_proconfig_value(a, set_items);

// 		/* update the tuple */
// 		memset(repl_repl, false, sizeof(repl_repl));
// 		repl_repl[Anum_pg_proc_proconfig - 1] = true;

// 		if (a == NULL)
// 		{
// 			repl_val[Anum_pg_proc_proconfig - 1] = (Datum) 0;
// 			repl_null[Anum_pg_proc_proconfig - 1] = true;
// 		}
// 		else
// 		{
// 			repl_val[Anum_pg_proc_proconfig - 1] = PointerGetDatum(a);
// 			repl_null[Anum_pg_proc_proconfig - 1] = false;
// 		}

// 		tup = heap_modify_tuple(tup, RelationGetDescr(rel),
// 								repl_val, repl_null, repl_repl);
// 	}
// 	if (parallel_item)
// 		procForm->proparallel = interpret_func_parallel(parallel_item);

// 	/* Do the update */
// 	simple_heap_update(rel, &tup->t_self, tup);
// 	CatalogUpdateIndexes(rel, tup);

// 	InvokeObjectPostAlterHook(ProcedureRelationId, funcOid, 0);

// 	ObjectAddressSet(address, ProcedureRelationId, funcOid);

// 	heap_close(rel, NoLock);
// 	heap_freetuple(tup);

// 	return address;
// }
// //end

// ObjectAddress
// CreateCast(CreateCastStmt *stmt)
// {
// 	Oid			sourcetypeid;
// 	Oid			targettypeid;
// 	char		sourcetyptype;
// 	char		targettyptype;
// 	Oid			funcid;
// 	Oid			castid;
// 	int			nargs;
// 	char		castcontext;
// 	char		castmethod;
// 	Relation	relation;
// 	HeapTuple	tuple;
// 	Datum		values[Natts_pg_cast];
// 	bool		nulls[Natts_pg_cast];
// 	ObjectAddress myself,
// 				referenced;
// 	AclResult	aclresult;

// 	sourcetypeid = typenameTypeId(NULL, stmt->sourcetype);
// 	targettypeid = typenameTypeId(NULL, stmt->targettype);
// 	sourcetyptype = get_typtype(sourcetypeid);
// 	targettyptype = get_typtype(targettypeid);

// 	/* No pseudo-types allowed */
// 	if (sourcetyptype == TYPTYPE_PSEUDO)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("source data type %s is a pseudo-type",
// 						TypeNameToString(stmt->sourcetype))));

// 	if (targettyptype == TYPTYPE_PSEUDO)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("target data type %s is a pseudo-type",
// 						TypeNameToString(stmt->targettype))));

// 	/* Permission check */
// 	if (!pg_type_ownercheck(sourcetypeid, GetUserId())
// 		&& !pg_type_ownercheck(targettypeid, GetUserId()))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be owner of type %s or type %s",
// 						format_type_be(sourcetypeid),
// 						format_type_be(targettypeid))));

// 	aclresult = pg_type_aclcheck(sourcetypeid, GetUserId(), ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error_type(aclresult, sourcetypeid);

// 	aclresult = pg_type_aclcheck(targettypeid, GetUserId(), ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error_type(aclresult, targettypeid);

// 	/* Domains are allowed for historical reasons, but we warn */
// 	if (sourcetyptype == TYPTYPE_DOMAIN)
// 		ereport(WARNING,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("cast will be ignored because the source data type is a domain")));

// 	else if (targettyptype == TYPTYPE_DOMAIN)
// 		ereport(WARNING,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("cast will be ignored because the target data type is a domain")));

// 	/* Determine the cast method */
// 	if (stmt->func != NULL)
// 		castmethod = COERCION_METHOD_FUNCTION;
// 	else if (stmt->inout)
// 		castmethod = COERCION_METHOD_INOUT;
// 	else
// 		castmethod = COERCION_METHOD_BINARY;

// 	if (castmethod == COERCION_METHOD_FUNCTION)
// 	{
// 		Form_pg_proc procstruct;

// 		funcid = LookupFuncNameTypeNames(stmt->func->funcname,
// 										 stmt->func->funcargs,
// 										 false);

// 		tuple = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcid));
// 		if (!HeapTupleIsValid(tuple))
// 			elog(ERROR, "cache lookup failed for function %u", funcid);

// 		procstruct = (Form_pg_proc) GETSTRUCT(tuple);
// 		nargs = procstruct->pronargs;
// 		if (nargs < 1 || nargs > 3)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				  errmsg("cast function must take one to three arguments")));
// 		if (!IsBinaryCoercible(sourcetypeid, procstruct->proargtypes.values[0]))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("argument of cast function must match or be binary-coercible from source data type")));
// 		if (nargs > 1 && procstruct->proargtypes.values[1] != INT4OID)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 			errmsg("second argument of cast function must be type integer")));
// 		if (nargs > 2 && procstruct->proargtypes.values[2] != BOOLOID)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 			errmsg("third argument of cast function must be type boolean")));
// 		if (!IsBinaryCoercible(procstruct->prorettype, targettypeid))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("return data type of cast function must match or be binary-coercible to target data type")));

// 		/*
// 		 * Restricting the volatility of a cast function may or may not be a
// 		 * good idea in the abstract, but it definitely breaks many old
// 		 * user-defined types.  Disable this check --- tgl 2/1/03
// 		 */
// #ifdef NOT_USED
// 		if (procstruct->provolatile == PROVOLATILE_VOLATILE)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("cast function must not be volatile")));
// #endif
// 		if (procstruct->proisagg)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				 errmsg("cast function must not be an aggregate function")));
// 		if (procstruct->proiswindow)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("cast function must not be a window function")));
// 		if (procstruct->proretset)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("cast function must not return a set")));

// 		ReleaseSysCache(tuple);
// 	}
// 	else
// 	{
// 		funcid = InvalidOid;
// 		nargs = 0;
// 	}

// 	if (castmethod == COERCION_METHOD_BINARY)
// 	{
// 		int16		typ1len;
// 		int16		typ2len;
// 		bool		typ1byval;
// 		bool		typ2byval;
// 		char		typ1align;
// 		char		typ2align;

// 		/*
// 		 * Must be superuser to create binary-compatible casts, since
// 		 * erroneous casts can easily crash the backend.
// 		 */
// 		if (!superuser())
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 			 errmsg("must be superuser to create a cast WITHOUT FUNCTION")));

// 		/*
// 		 * Also, insist that the types match as to size, alignment, and
// 		 * pass-by-value attributes; this provides at least a crude check that
// 		 * they have similar representations.  A pair of types that fail this
// 		 * test should certainly not be equated.
// 		 */
// 		get_typlenbyvalalign(sourcetypeid, &typ1len, &typ1byval, &typ1align);
// 		get_typlenbyvalalign(targettypeid, &typ2len, &typ2byval, &typ2align);
// 		if (typ1len != typ2len ||
// 			typ1byval != typ2byval ||
// 			typ1align != typ2align)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("source and target data types are not physically compatible")));

// 		/*
// 		 * We know that composite, enum and array types are never binary-
// 		 * compatible with each other.  They all have OIDs embedded in them.
// 		 *
// 		 * Theoretically you could build a user-defined base type that is
// 		 * binary-compatible with a composite, enum, or array type.  But we
// 		 * disallow that too, as in practice such a cast is surely a mistake.
// 		 * You can always work around that by writing a cast function.
// 		 */
// 		if (sourcetyptype == TYPTYPE_COMPOSITE ||
// 			targettyptype == TYPTYPE_COMPOSITE)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 				  errmsg("composite data types are not binary-compatible")));

// 		if (sourcetyptype == TYPTYPE_ENUM ||
// 			targettyptype == TYPTYPE_ENUM)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("enum data types are not binary-compatible")));

// 		if (OidIsValid(get_element_type(sourcetypeid)) ||
// 			OidIsValid(get_element_type(targettypeid)))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("array data types are not binary-compatible")));

// 		/*
// 		 * We also disallow creating binary-compatibility casts involving
// 		 * domains.  Casting from a domain to its base type is already
// 		 * allowed, and casting the other way ought to go through domain
// 		 * coercion to permit constraint checking.  Again, if you're intent on
// 		 * having your own semantics for that, create a no-op cast function.
// 		 *
// 		 * NOTE: if we were to relax this, the above checks for composites
// 		 * etc. would have to be modified to look through domains to their
// 		 * base types.
// 		 */
// 		if (sourcetyptype == TYPTYPE_DOMAIN ||
// 			targettyptype == TYPTYPE_DOMAIN)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("domain data types must not be marked binary-compatible")));
// 	}

// 	/*
// 	 * Allow source and target types to be same only for length coercion
// 	 * functions.  We assume a multi-arg function does length coercion.
// 	 */
// 	if (sourcetypeid == targettypeid && nargs < 2)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 			  errmsg("source data type and target data type are the same")));

// 	/* convert CoercionContext enum to char value for castcontext */
// 	switch (stmt->context)
// 	{
// 		case COERCION_IMPLICIT:
// 			castcontext = COERCION_CODE_IMPLICIT;
// 			break;
// 		case COERCION_ASSIGNMENT:
// 			castcontext = COERCION_CODE_ASSIGNMENT;
// 			break;
// 		case COERCION_EXPLICIT:
// 			castcontext = COERCION_CODE_EXPLICIT;
// 			break;
// 		default:
// 			elog(ERROR, "unrecognized CoercionContext: %d", stmt->context);
// 			castcontext = 0;	/* keep compiler quiet */
// 			break;
// 	}

// 	relation = heap_open(CastRelationId, RowExclusiveLock);

// 	/*
// 	 * Check for duplicate.  This is just to give a friendly error message,
// 	 * the unique index would catch it anyway (so no need to sweat about race
// 	 * conditions).
// 	 */
// 	tuple = SearchSysCache2(CASTSOURCETARGET,
// 							ObjectIdGetDatum(sourcetypeid),
// 							ObjectIdGetDatum(targettypeid));
// 	if (HeapTupleIsValid(tuple))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("cast from type %s to type %s already exists",
// 						format_type_be(sourcetypeid),
// 						format_type_be(targettypeid))));

// 	/* ready to go */
// 	values[Anum_pg_cast_castsource - 1] = ObjectIdGetDatum(sourcetypeid);
// 	values[Anum_pg_cast_casttarget - 1] = ObjectIdGetDatum(targettypeid);
// 	values[Anum_pg_cast_castfunc - 1] = ObjectIdGetDatum(funcid);
// 	values[Anum_pg_cast_castcontext - 1] = CharGetDatum(castcontext);
// 	values[Anum_pg_cast_castmethod - 1] = CharGetDatum(castmethod);

// 	MemSet(nulls, false, sizeof(nulls));

// 	tuple = heap_form_tuple(RelationGetDescr(relation), values, nulls);

// 	castid = simple_heap_insert(relation, tuple);

// 	CatalogUpdateIndexes(relation, tuple);

// 	/* make dependency entries */
// 	myself.classId = CastRelationId;
// 	myself.objectId = castid;
// 	myself.objectSubId = 0;

// 	/* dependency on source type */
// 	referenced.classId = TypeRelationId;
// 	referenced.objectId = sourcetypeid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	/* dependency on target type */
// 	referenced.classId = TypeRelationId;
// 	referenced.objectId = targettypeid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	/* dependency on function */
// 	if (OidIsValid(funcid))
// 	{
// 		referenced.classId = ProcedureRelationId;
// 		referenced.objectId = funcid;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 	}

// 	/* dependency on extension */
// 	recordDependencyOnCurrentExtension(&myself, false);

// 	/* Post creation hook for new cast */
// 	InvokeObjectPostCreateHook(CastRelationId, castid, 0);

// 	heap_freetuple(tuple);

// 	heap_close(relation, RowExclusiveLock);

// 	return myself;
// }
// //end

// void
// ExecuteDoStmt(DoStmt *stmt)
// {
// 	InlineCodeBlock *codeblock = makeNode(InlineCodeBlock);
// 	ListCell   *arg;
// 	DefElem    *as_item = NULL;
// 	DefElem    *language_item = NULL;
// 	char	   *language;
// 	Oid			laninline;
// 	HeapTuple	languageTuple;
// 	Form_pg_language languageStruct;

// 	/* Process options we got from gram.y */
// 	foreach(arg, stmt->args)
// 	{
// 		DefElem    *defel = (DefElem *) lfirst(arg);

// 		if (strcmp(defel->defname, "as") == 0)
// 		{
// 			if (as_item)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			as_item = defel;
// 		}
// 		else if (strcmp(defel->defname, "language") == 0)
// 		{
// 			if (language_item)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_SYNTAX_ERROR),
// 						 errmsg("conflicting or redundant options")));
// 			language_item = defel;
// 		}
// 		else
// 			elog(ERROR, "option \"%s\" not recognized",
// 				 defel->defname);
// 	}

// 	if (as_item)
// 		codeblock->source_text = strVal(as_item->arg);
// 	else
// 		ereport(ERROR,
// 				(errcode(ERRCODE_SYNTAX_ERROR),
// 				 errmsg("no inline code specified")));

// 	/* if LANGUAGE option wasn't specified, use the default */
// 	if (language_item)
// 		language = strVal(language_item->arg);
// 	else
// 		language = "plpgsql";

// 	/* Look up the language and validate permissions */
// 	languageTuple = SearchSysCache1(LANGNAME, PointerGetDatum(language));
// 	if (!HeapTupleIsValid(languageTuple))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("language \"%s\" does not exist", language),
// 				 (PLTemplateExists(language) ?
// 				  errhint("Use CREATE LANGUAGE to load the language into the database.") : 0)));

// 	codeblock->langOid = HeapTupleGetOid(languageTuple);
// 	languageStruct = (Form_pg_language) GETSTRUCT(languageTuple);
// 	codeblock->langIsTrusted = languageStruct->lanpltrusted;

// 	if (languageStruct->lanpltrusted)
// 	{
// 		/* if trusted language, need USAGE privilege */
// 		AclResult	aclresult;

// 		aclresult = pg_language_aclcheck(codeblock->langOid, GetUserId(),
// 										 ACL_USAGE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_LANGUAGE,
// 						   NameStr(languageStruct->lanname));
// 	}
// 	else
// 	{
// 		/* if untrusted language, must be superuser */
// 		if (!superuser())
// 			aclcheck_error(ACLCHECK_NO_PRIV, ACL_KIND_LANGUAGE,
// 						   NameStr(languageStruct->lanname));
// 	}

// 	/* get the handler function's OID */
// 	laninline = languageStruct->laninline;
// 	if (!OidIsValid(laninline))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 			 errmsg("language \"%s\" does not support inline code execution",
// 					NameStr(languageStruct->lanname))));

// 	ReleaseSysCache(languageTuple);

// 	/* execute the inline handler */
// 	OidFunctionCall1(laninline, PointerGetDatum(codeblock));
// }
// //end

// ObjectAddress
// CreateTransform(CreateTransformStmt *stmt)
// {
// 	Oid			typeid;
// 	char		typtype;
// 	Oid			langid;
// 	Oid			fromsqlfuncid;
// 	Oid			tosqlfuncid;
// 	AclResult	aclresult;
// 	Form_pg_proc procstruct;
// 	Datum		values[Natts_pg_transform];
// 	bool		nulls[Natts_pg_transform];
// 	bool		replaces[Natts_pg_transform];
// 	Oid			transformid;
// 	HeapTuple	tuple;
// 	HeapTuple	newtuple;
// 	Relation	relation;
// 	ObjectAddress myself,
// 				referenced;
// 	bool		is_replace;

// 	/*
// 	 * Get the type
// 	 */
// 	typeid = typenameTypeId(NULL, stmt->type_name);
// 	typtype = get_typtype(typeid);

// 	if (typtype == TYPTYPE_PSEUDO)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("data type %s is a pseudo-type",
// 						TypeNameToString(stmt->type_name))));

// 	if (typtype == TYPTYPE_DOMAIN)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("data type %s is a domain",
// 						TypeNameToString(stmt->type_name))));

// 	if (!pg_type_ownercheck(typeid, GetUserId()))
// 		aclcheck_error_type(ACLCHECK_NOT_OWNER, typeid);

// 	aclresult = pg_type_aclcheck(typeid, GetUserId(), ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error_type(aclresult, typeid);

// 	/*
// 	 * Get the language
// 	 */
// 	langid = get_language_oid(stmt->lang, false);

// 	aclresult = pg_language_aclcheck(langid, GetUserId(), ACL_USAGE);
// 	if (aclresult != ACLCHECK_OK)
// 		aclcheck_error(aclresult, ACL_KIND_LANGUAGE, stmt->lang);

// 	/*
// 	 * Get the functions
// 	 */
// 	if (stmt->fromsql)
// 	{
// 		fromsqlfuncid = LookupFuncNameTypeNames(stmt->fromsql->funcname, stmt->fromsql->funcargs, false);

// 		if (!pg_proc_ownercheck(fromsqlfuncid, GetUserId()))
// 			aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_PROC, NameListToString(stmt->fromsql->funcname));

// 		aclresult = pg_proc_aclcheck(fromsqlfuncid, GetUserId(), ACL_EXECUTE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_PROC, NameListToString(stmt->fromsql->funcname));

// 		tuple = SearchSysCache1(PROCOID, ObjectIdGetDatum(fromsqlfuncid));
// 		if (!HeapTupleIsValid(tuple))
// 			elog(ERROR, "cache lookup failed for function %u", fromsqlfuncid);
// 		procstruct = (Form_pg_proc) GETSTRUCT(tuple);
// 		if (procstruct->prorettype != INTERNALOID)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("return data type of FROM SQL function must be \"internal\"")));
// 		check_transform_function(procstruct);
// 		ReleaseSysCache(tuple);
// 	}
// 	else
// 		fromsqlfuncid = InvalidOid;

// 	if (stmt->tosql)
// 	{
// 		tosqlfuncid = LookupFuncNameTypeNames(stmt->tosql->funcname, stmt->tosql->funcargs, false);

// 		if (!pg_proc_ownercheck(tosqlfuncid, GetUserId()))
// 			aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_PROC, NameListToString(stmt->tosql->funcname));

// 		aclresult = pg_proc_aclcheck(tosqlfuncid, GetUserId(), ACL_EXECUTE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_PROC, NameListToString(stmt->tosql->funcname));

// 		tuple = SearchSysCache1(PROCOID, ObjectIdGetDatum(tosqlfuncid));
// 		if (!HeapTupleIsValid(tuple))
// 			elog(ERROR, "cache lookup failed for function %u", tosqlfuncid);
// 		procstruct = (Form_pg_proc) GETSTRUCT(tuple);
// 		if (procstruct->prorettype != typeid)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("return data type of TO SQL function must be the transform data type")));
// 		check_transform_function(procstruct);
// 		ReleaseSysCache(tuple);
// 	}
// 	else
// 		tosqlfuncid = InvalidOid;

// 	/*
// 	 * Ready to go
// 	 */
// 	values[Anum_pg_transform_trftype - 1] = ObjectIdGetDatum(typeid);
// 	values[Anum_pg_transform_trflang - 1] = ObjectIdGetDatum(langid);
// 	values[Anum_pg_transform_trffromsql - 1] = ObjectIdGetDatum(fromsqlfuncid);
// 	values[Anum_pg_transform_trftosql - 1] = ObjectIdGetDatum(tosqlfuncid);

// 	MemSet(nulls, false, sizeof(nulls));

// 	relation = heap_open(TransformRelationId, RowExclusiveLock);

// 	tuple = SearchSysCache2(TRFTYPELANG,
// 							ObjectIdGetDatum(typeid),
// 							ObjectIdGetDatum(langid));
// 	if (HeapTupleIsValid(tuple))
// 	{
// 		if (!stmt->replace)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_DUPLICATE_OBJECT),
// 			   errmsg("transform for type %s language \"%s\" already exists",
// 					  format_type_be(typeid),
// 					  stmt->lang)));

// 		MemSet(replaces, false, sizeof(replaces));
// 		replaces[Anum_pg_transform_trffromsql - 1] = true;
// 		replaces[Anum_pg_transform_trftosql - 1] = true;

// 		newtuple = heap_modify_tuple(tuple, RelationGetDescr(relation), values, nulls, replaces);
// 		simple_heap_update(relation, &newtuple->t_self, newtuple);

// 		transformid = HeapTupleGetOid(tuple);
// 		ReleaseSysCache(tuple);
// 		is_replace = true;
// 	}
// 	else
// 	{
// 		newtuple = heap_form_tuple(RelationGetDescr(relation), values, nulls);
// 		transformid = simple_heap_insert(relation, newtuple);
// 		is_replace = false;
// 	}

// 	CatalogUpdateIndexes(relation, newtuple);

// 	if (is_replace)
// 		deleteDependencyRecordsFor(TransformRelationId, transformid, true);

// 	/* make dependency entries */
// 	myself.classId = TransformRelationId;
// 	myself.objectId = transformid;
// 	myself.objectSubId = 0;

// 	/* dependency on language */
// 	referenced.classId = LanguageRelationId;
// 	referenced.objectId = langid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	/* dependency on type */
// 	referenced.classId = TypeRelationId;
// 	referenced.objectId = typeid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	/* dependencies on functions */
// 	if (OidIsValid(fromsqlfuncid))
// 	{
// 		referenced.classId = ProcedureRelationId;
// 		referenced.objectId = fromsqlfuncid;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 	}
// 	if (OidIsValid(tosqlfuncid))
// 	{
// 		referenced.classId = ProcedureRelationId;
// 		referenced.objectId = tosqlfuncid;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 	}

// 	/* dependency on extension */
// 	recordDependencyOnCurrentExtension(&myself, is_replace);

// 	/* Post creation hook for new transform */
// 	InvokeObjectPostCreateHook(TransformRelationId, transformid, 0);

// 	heap_freetuple(newtuple);

// 	heap_close(relation, RowExclusiveLock);

// 	return myself;
// }
// //end

// void
// DropTransformById(Oid transformOid)
// {
// 	Relation	relation;
// 	ScanKeyData scankey;
// 	SysScanDesc scan;
// 	HeapTuple	tuple;

// 	relation = heap_open(TransformRelationId, RowExclusiveLock);

// 	ScanKeyInit(&scankey,
// 				ObjectIdAttributeNumber,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(transformOid));
// 	scan = systable_beginscan(relation, TransformOidIndexId, true,
// 							  NULL, 1, &scankey);

// 	tuple = systable_getnext(scan);
// 	if (!HeapTupleIsValid(tuple))
// 		elog(ERROR, "could not find tuple for transform %u", transformOid);
// 	simple_heap_delete(relation, &tuple->t_self);

// 	systable_endscan(scan);
// 	heap_close(relation, RowExclusiveLock);
// }
// //end

// void
// IsThereFunctionInNamespace(const char *proname, int pronargs,
// 						   oidvector *proargtypes, Oid nspOid)
// {
// 	/* check for duplicate name (more friendly than unique-index failure) */
// 	if (SearchSysCacheExists3(PROCNAMEARGSNSP,
// 							  CStringGetDatum(proname),
// 							  PointerGetDatum(proargtypes),
// 							  ObjectIdGetDatum(nspOid)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_FUNCTION),
// 				 errmsg("function %s already exists in schema \"%s\"",
// 						funcname_signature_string(proname, pronargs,
// 												  NIL, proargtypes->values),
// 						get_namespace_name(nspOid))));
// }
// //end

// Oid
// get_transform_oid(Oid type_id, Oid lang_id, bool missing_ok)
// {
// 	Oid			oid;

// 	oid = GetSysCacheOid2(TRFTYPELANG,
// 						  ObjectIdGetDatum(type_id),
// 						  ObjectIdGetDatum(lang_id));
// 	if (!OidIsValid(oid) && !missing_ok)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 			   errmsg("transform for type %s language \"%s\" does not exist",
// 					  format_type_be(type_id),
// 					  get_language_name(lang_id, false))));
// 	return oid;
// }
// //end

// void
// interpret_function_parameter_list(List *parameters,
// 								  Oid languageOid,
// 								  bool is_aggregate,
// 								  const char *queryString,
// 								  oidvector **parameterTypes,
// 								  ArrayType **allParameterTypes,
// 								  ArrayType **parameterModes,
// 								  ArrayType **parameterNames,
// 								  List **parameterDefaults,
// 								  Oid *variadicArgType,
// 								  Oid *requiredResultType)
// {
// 	int			parameterCount = list_length(parameters);
// 	Oid		   *inTypes;
// 	int			inCount = 0;
// 	Datum	   *allTypes;
// 	Datum	   *paramModes;
// 	Datum	   *paramNames;
// 	int			outCount = 0;
// 	int			varCount = 0;
// 	bool		have_names = false;
// 	bool		have_defaults = false;
// 	ListCell   *x;
// 	int			i;
// 	ParseState *pstate;

// 	*variadicArgType = InvalidOid;		/* default result */
// 	*requiredResultType = InvalidOid;	/* default result */

// 	inTypes = (Oid *) palloc(parameterCount * sizeof(Oid));
// 	allTypes = (Datum *) palloc(parameterCount * sizeof(Datum));
// 	paramModes = (Datum *) palloc(parameterCount * sizeof(Datum));
// 	paramNames = (Datum *) palloc0(parameterCount * sizeof(Datum));
// 	*parameterDefaults = NIL;

// 	/* may need a pstate for parse analysis of default exprs */
// 	pstate = make_parsestate(NULL);
// 	pstate->p_sourcetext = queryString;

// 	/* Scan the list and extract data into work arrays */
// 	i = 0;
// 	foreach(x, parameters)
// 	{
// 		FunctionParameter *fp = (FunctionParameter *) lfirst(x);
// 		TypeName   *t = fp->argType;
// 		bool		isinput = false;
// 		Oid			toid;
// 		Type		typtup;
// 		AclResult	aclresult;

// 		typtup = LookupTypeName(NULL, t, NULL, false);
// 		if (typtup)
// 		{
// 			if (!((Form_pg_type) GETSTRUCT(typtup))->typisdefined)
// 			{
// 				/* As above, hard error if language is SQL */
// 				if (languageOid == SQLlanguageId)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						   errmsg("SQL function cannot accept shell type %s",
// 								  TypeNameToString(t))));
// 				/* We don't allow creating aggregates on shell types either */
// 				else if (is_aggregate)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 							 errmsg("aggregate cannot accept shell type %s",
// 									TypeNameToString(t))));
// 				else
// 					ereport(NOTICE,
// 							(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 							 errmsg("argument type %s is only a shell",
// 									TypeNameToString(t))));
// 			}
// 			toid = typeTypeId(typtup);
// 			ReleaseSysCache(typtup);
// 		}
// 		else
// 		{
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 					 errmsg("type %s does not exist",
// 							TypeNameToString(t))));
// 			toid = InvalidOid;	/* keep compiler quiet */
// 		}

// 		aclresult = pg_type_aclcheck(toid, GetUserId(), ACL_USAGE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error_type(aclresult, toid);

// 		if (t->setof)
// 		{
// 			if (is_aggregate)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						 errmsg("aggregates cannot accept set arguments")));
// 			else
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						 errmsg("functions cannot accept set arguments")));
// 		}

// 		/* handle input parameters */
// 		if (fp->mode != FUNC_PARAM_OUT && fp->mode != FUNC_PARAM_TABLE)
// 		{
// 			/* other input parameters can't follow a VARIADIC parameter */
// 			if (varCount > 0)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						 errmsg("VARIADIC parameter must be the last input parameter")));
// 			inTypes[inCount++] = toid;
// 			isinput = true;
// 		}

// 		/* handle output parameters */
// 		if (fp->mode != FUNC_PARAM_IN && fp->mode != FUNC_PARAM_VARIADIC)
// 		{
// 			if (outCount == 0)	/* save first output param's type */
// 				*requiredResultType = toid;
// 			outCount++;
// 		}

// 		if (fp->mode == FUNC_PARAM_VARIADIC)
// 		{
// 			*variadicArgType = toid;
// 			varCount++;
// 			/* validate variadic parameter type */
// 			switch (toid)
// 			{
// 				case ANYARRAYOID:
// 				case ANYOID:
// 					/* okay */
// 					break;
// 				default:
// 					if (!OidIsValid(get_element_type(toid)))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 							 errmsg("VARIADIC parameter must be an array")));
// 					break;
// 			}
// 		}

// 		allTypes[i] = ObjectIdGetDatum(toid);

// 		paramModes[i] = CharGetDatum(fp->mode);

// 		if (fp->name && fp->name[0])
// 		{
// 			ListCell   *px;

// 			/*
// 			 * As of Postgres 9.0 we disallow using the same name for two
// 			 * input or two output function parameters.  Depending on the
// 			 * function's language, conflicting input and output names might
// 			 * be bad too, but we leave it to the PL to complain if so.
// 			 */
// 			foreach(px, parameters)
// 			{
// 				FunctionParameter *prevfp = (FunctionParameter *) lfirst(px);

// 				if (prevfp == fp)
// 					break;
// 				/* pure in doesn't conflict with pure out */
// 				if ((fp->mode == FUNC_PARAM_IN ||
// 					 fp->mode == FUNC_PARAM_VARIADIC) &&
// 					(prevfp->mode == FUNC_PARAM_OUT ||
// 					 prevfp->mode == FUNC_PARAM_TABLE))
// 					continue;
// 				if ((prevfp->mode == FUNC_PARAM_IN ||
// 					 prevfp->mode == FUNC_PARAM_VARIADIC) &&
// 					(fp->mode == FUNC_PARAM_OUT ||
// 					 fp->mode == FUNC_PARAM_TABLE))
// 					continue;
// 				if (prevfp->name && prevfp->name[0] &&
// 					strcmp(prevfp->name, fp->name) == 0)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						  errmsg("parameter name \"%s\" used more than once",
// 								 fp->name)));
// 			}

// 			paramNames[i] = CStringGetTextDatum(fp->name);
// 			have_names = true;
// 		}

// 		if (fp->defexpr)
// 		{
// 			Node	   *def;

// 			if (!isinput)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 				   errmsg("only input parameters can have default values")));

// 			def = transformExpr(pstate, fp->defexpr,
// 								EXPR_KIND_FUNCTION_DEFAULT);
// 			def = coerce_to_specific_type(pstate, def, toid, "DEFAULT");
// 			assign_expr_collations(pstate, def);

// 			/*
// 			 * Make sure no variables are referred to (this is probably dead
// 			 * code now that add_missing_from is history).
// 			 */
// 			if (list_length(pstate->p_rtable) != 0 ||
// 				contain_var_clause(def))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
// 						 errmsg("cannot use table references in parameter default value")));

// 			/*
// 			 * transformExpr() should have already rejected subqueries,
// 			 * aggregates, and window functions, based on the EXPR_KIND_ for a
// 			 * default expression.
// 			 *
// 			 * It can't return a set either --- but coerce_to_specific_type
// 			 * already checked that for us.
// 			 *
// 			 * Note: the point of these restrictions is to ensure that an
// 			 * expression that, on its face, hasn't got subplans, aggregates,
// 			 * etc cannot suddenly have them after function default arguments
// 			 * are inserted.
// 			 */

// 			*parameterDefaults = lappend(*parameterDefaults, def);
// 			have_defaults = true;
// 		}
// 		else
// 		{
// 			if (isinput && have_defaults)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
// 						 errmsg("input parameters after one with a default value must also have defaults")));
// 		}

// 		i++;
// 	}

// 	free_parsestate(pstate);

// 	/* Now construct the proper outputs as needed */
// 	*parameterTypes = buildoidvector(inTypes, inCount);

// 	if (outCount > 0 || varCount > 0)
// 	{
// 		*allParameterTypes = construct_array(allTypes, parameterCount, OIDOID,
// 											 sizeof(Oid), true, 'i');
// 		*parameterModes = construct_array(paramModes, parameterCount, CHAROID,
// 										  1, true, 'c');
// 		if (outCount > 1)
// 			*requiredResultType = RECORDOID;
// 		/* otherwise we set requiredResultType correctly above */
// 	}
// 	else
// 	{
// 		*allParameterTypes = NULL;
// 		*parameterModes = NULL;
// 	}

// 	if (have_names)
// 	{
// 		for (i = 0; i < parameterCount; i++)
// 		{
// 			if (paramNames[i] == PointerGetDatum(NULL))
// 				paramNames[i] = CStringGetTextDatum("");
// 		}
// 		*parameterNames = construct_array(paramNames, parameterCount, TEXTOID,
// 										  -1, false, 'i');
// 	}
// 	else
// 		*parameterNames = NULL;
// }
// //end

// /* commands/amcmds.c */
// ObjectAddress
// CreateAccessMethod(CreateAmStmt *stmt)
// {
// 	Relation	rel;
// 	ObjectAddress myself;
// 	ObjectAddress referenced;
// 	Oid			amoid;
// 	Oid			amhandler;
// 	bool		nulls[Natts_pg_am];
// 	Datum		values[Natts_pg_am];
// 	HeapTuple	tup;

// 	rel = heap_open(AccessMethodRelationId, RowExclusiveLock);

// 	/* Must be super user */
// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("permission denied to create access method \"%s\"",
// 						stmt->amname),
// 				 errhint("Must be superuser to create an access method.")));

// 	/* Check if name is used */
// 	amoid = GetSysCacheOid1(AMNAME, CStringGetDatum(stmt->amname));
// 	if (OidIsValid(amoid))
// 	{
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("access method \"%s\" already exists",
// 						stmt->amname)));
// 	}

// 	/*
// 	 * Get the handler function oid, verifying the AM type while at it.
// 	 */
// 	amhandler = lookup_index_am_handler_func(stmt->handler_name, stmt->amtype);

// 	/*
// 	 * Insert tuple into pg_am.
// 	 */
// 	memset(values, 0, sizeof(values));
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_am_amname - 1] =
// 		DirectFunctionCall1(namein, CStringGetDatum(stmt->amname));
// 	values[Anum_pg_am_amhandler - 1] = ObjectIdGetDatum(amhandler);
// 	values[Anum_pg_am_amtype - 1] = CharGetDatum(stmt->amtype);

// 	tup = heap_form_tuple(RelationGetDescr(rel), values, nulls);

// 	amoid = simple_heap_insert(rel, tup);
// 	CatalogUpdateIndexes(rel, tup);
// 	heap_freetuple(tup);

// 	myself.classId = AccessMethodRelationId;
// 	myself.objectId = amoid;
// 	myself.objectSubId = 0;

// 	/* Record dependency on handler function */
// 	referenced.classId = ProcedureRelationId;
// 	referenced.objectId = amhandler;
// 	referenced.objectSubId = 0;

// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	recordDependencyOnCurrentExtension(&myself, false);

// 	heap_close(rel, RowExclusiveLock);

// 	return myself;
// }
// //end

// void
// RemoveAccessMethodById(Oid amOid)
// {
// 	Relation	relation;
// 	HeapTuple	tup;

// 	if (!superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to drop access methods")));

// 	relation = heap_open(AccessMethodRelationId, RowExclusiveLock);

// 	tup = SearchSysCache1(AMOID, ObjectIdGetDatum(amOid));
// 	if (!HeapTupleIsValid(tup))
// 		elog(ERROR, "cache lookup failed for access method %u", amOid);

// 	simple_heap_delete(relation, &tup->t_self);

// 	ReleaseSysCache(tup);

// 	heap_close(relation, RowExclusiveLock);
// }
// //end

// Oid
// get_index_am_oid(const char *amname, bool missing_ok)
// {
// 	return get_am_type_oid(amname, AMTYPE_INDEX, missing_ok);
// }
// //end

// char *
// get_am_name(Oid amOid)
// {
// 	HeapTuple	tup;
// 	char	   *result = NULL;

// 	tup = SearchSysCache1(AMOID, ObjectIdGetDatum(amOid));
// 	if (HeapTupleIsValid(tup))
// 	{
// 		Form_pg_am	amform = (Form_pg_am) GETSTRUCT(tup);

// 		result = pstrdup(NameStr(amform->amname));
// 		ReleaseSysCache(tup);
// 	}
// 	return result;
// }
// //end

// /* support routines in commands/define.c */
// int32
// defGetInt32(DefElem *def)
// {
// 	if (def->arg == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_SYNTAX_ERROR),
// 				 errmsg("%s requires an integer value",
// 						def->defname)));
// 	switch (nodeTag(def->arg))
// 	{
// 		case T_Integer:
// 			return (int32) intVal(def->arg);
// 		default:
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("%s requires an integer value",
// 							def->defname)));
// 	}
// 	return 0;					/* keep compiler quiet */
// }
// //end
// //end defrem.h

// //commands/explain.c
// void
// ExplainQuery(ExplainStmt *stmt, const char *queryString,
// 			 ParamListInfo params, DestReceiver *dest)
// {
// 	ExplainState *es = NewExplainState();
// 	TupOutputState *tstate;
// 	List	   *rewritten;
// 	ListCell   *lc;
// 	bool		timing_set = false;

// 	/* Parse options list. */
// 	foreach(lc, stmt->options)
// 	{
// 		DefElem    *opt = (DefElem *) lfirst(lc);

// 		if (strcmp(opt->defname, "analyze") == 0)
// 			es->analyze = defGetBoolean(opt);
// 		else if (strcmp(opt->defname, "verbose") == 0)
// 			es->verbose = defGetBoolean(opt);
// 		else if (strcmp(opt->defname, "costs") == 0)
// 			es->costs = defGetBoolean(opt);
// 		else if (strcmp(opt->defname, "buffers") == 0)
// 			es->buffers = defGetBoolean(opt);
// 		else if (strcmp(opt->defname, "timing") == 0)
// 		{
// 			timing_set = true;
// 			es->timing = defGetBoolean(opt);
// 		}
// 		else if (strcmp(opt->defname, "format") == 0)
// 		{
// 			char	   *p = defGetString(opt);

// 			if (strcmp(p, "text") == 0)
// 				es->format = EXPLAIN_FORMAT_TEXT;
// 			else if (strcmp(p, "xml") == 0)
// 				es->format = EXPLAIN_FORMAT_XML;
// 			else if (strcmp(p, "json") == 0)
// 				es->format = EXPLAIN_FORMAT_JSON;
// 			else if (strcmp(p, "yaml") == 0)
// 				es->format = EXPLAIN_FORMAT_YAML;
// 			else
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				errmsg("unrecognized value for EXPLAIN option \"%s\": \"%s\"",
// 					   opt->defname, p)));
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_SYNTAX_ERROR),
// 					 errmsg("unrecognized EXPLAIN option \"%s\"",
// 							opt->defname)));
// 	}

// 	if (es->buffers && !es->analyze)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("EXPLAIN option BUFFERS requires ANALYZE")));

// 	/* if the timing was not set explicitly, set default value */
// 	es->timing = (timing_set) ? es->timing : es->analyze;

// 	/* check that timing is used with EXPLAIN ANALYZE */
// 	if (es->timing && !es->analyze)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("EXPLAIN option TIMING requires ANALYZE")));

// 	/* currently, summary option is not exposed to users; just set it */
// 	es->summary = es->analyze;

// 	/*
// 	 * Parse analysis was done already, but we still have to run the rule
// 	 * rewriter.  We do not do AcquireRewriteLocks: we assume the query either
// 	 * came straight from the parser, or suitable locks were acquired by
// 	 * plancache.c.
// 	 *
// 	 * Because the rewriter and planner tend to scribble on the input, we make
// 	 * a preliminary copy of the source querytree.  This prevents problems in
// 	 * the case that the EXPLAIN is in a portal or plpgsql function and is
// 	 * executed repeatedly.  (See also the same hack in DECLARE CURSOR and
// 	 * PREPARE.)  XXX FIXME someday.
// 	 */
// 	Assert(IsA(stmt->query, Query));
// 	rewritten = QueryRewrite((Query *) copyObject(stmt->query));

// 	/* emit opening boilerplate */
// 	ExplainBeginOutput(es);

// 	if (rewritten == NIL)
// 	{
// 		/*
// 		 * In the case of an INSTEAD NOTHING, tell at least that.  But in
// 		 * non-text format, the output is delimited, so this isn't necessary.
// 		 */
// 		if (es->format == EXPLAIN_FORMAT_TEXT)
// 			appendStringInfoString(es->str, "Query rewrites to nothing\n");
// 	}
// 	else
// 	{
// 		ListCell   *l;

// 		/* Explain every plan */
// 		foreach(l, rewritten)
// 		{
// 			ExplainOneQuery((Query *) lfirst(l), NULL, es,
// 							queryString, params);

// 			/* Separate plans with an appropriate separator */
// 			if (lnext(l) != NULL)
// 				ExplainSeparatePlans(es);
// 		}
// 	}

// 	/* emit closing boilerplate */
// 	ExplainEndOutput(es);
// 	Assert(es->indent == 0);

// 	/* output tuples */
// 	tstate = begin_tup_output_tupdesc(dest, ExplainResultDesc(stmt));
// 	if (es->format == EXPLAIN_FORMAT_TEXT)
// 		do_text_output_multiline(tstate, es->str->data);
// 	else
// 		do_text_output_oneline(tstate, es->str->data);
// 	end_tup_output(tstate);

// 	pfree(es->str->data);
// }
// //end
// void
// ExplainOnePlan(PlannedStmt *plannedstmt, IntoClause *into, ExplainState *es,
// 			   const char *queryString, ParamListInfo params,
// 			   const instr_time *planduration)
// {
// 	DestReceiver *dest;
// 	QueryDesc  *queryDesc;
// 	instr_time	starttime;
// 	double		totaltime = 0;
// 	int			eflags;
// 	int			instrument_option = 0;

// 	if (es->analyze && es->timing)
// 		instrument_option |= INSTRUMENT_TIMER;
// 	else if (es->analyze)
// 		instrument_option |= INSTRUMENT_ROWS;

// 	if (es->buffers)
// 		instrument_option |= INSTRUMENT_BUFFERS;

// 	/*
// 	 * We always collect timing for the entire statement, even when node-level
// 	 * timing is off, so we don't look at es->timing here.  (We could skip
// 	 * this if !es->summary, but it's hardly worth the complication.)
// 	 */
// 	INSTR_TIME_SET_CURRENT(starttime);

// 	/*
// 	 * Use a snapshot with an updated command ID to ensure this query sees
// 	 * results of any previously executed queries.
// 	 */
// 	PushCopiedSnapshot(GetActiveSnapshot());
// 	UpdateActiveSnapshotCommandId();

// 	/*
// 	 * Normally we discard the query's output, but if explaining CREATE TABLE
// 	 * AS, we'd better use the appropriate tuple receiver.
// 	 */
// 	if (into)
// 		dest = CreateIntoRelDestReceiver(into);
// 	else
// 		dest = None_Receiver;

// 	/* Create a QueryDesc for the query */
// 	queryDesc = CreateQueryDesc(plannedstmt, queryString,
// 								GetActiveSnapshot(), InvalidSnapshot,
// 								dest, params, instrument_option);

// 	/* Select execution options */
// 	if (es->analyze)
// 		eflags = 0;				/* default run-to-completion flags */
// 	else
// 		eflags = EXEC_FLAG_EXPLAIN_ONLY;
// 	if (into)
// 		eflags |= GetIntoRelEFlags(into);

// 	/* call ExecutorStart to prepare the plan for execution */
// 	ExecutorStart(queryDesc, eflags);

// 	/* Execute the plan for statistics if asked for */
// 	if (es->analyze)
// 	{
// 		ScanDirection dir;

// 		/* EXPLAIN ANALYZE CREATE TABLE AS WITH NO DATA is weird */
// 		if (into && into->skipData)
// 			dir = NoMovementScanDirection;
// 		else
// 			dir = ForwardScanDirection;

// 		/* run the plan */
// 		ExecutorRun(queryDesc, dir, 0L);

// 		/* run cleanup too */
// 		ExecutorFinish(queryDesc);

// 		/* We can't run ExecutorEnd 'till we're done printing the stats... */
// 		totaltime += elapsed_time(&starttime);
// 	}

// 	ExplainOpenGroup("Query", NULL, true, es);

// 	/* Create textual dump of plan tree */
// 	ExplainPrintPlan(es, queryDesc);

// 	if (es->summary && planduration)
// 	{
// 		double		plantime = INSTR_TIME_GET_DOUBLE(*planduration);

// 		if (es->format == EXPLAIN_FORMAT_TEXT)
// 			appendStringInfo(es->str, "Planning time: %.3f ms\n",
// 							 1000.0 * plantime);
// 		else
// 			ExplainPropertyFloat("Planning Time", 1000.0 * plantime, 3, es);
// 	}

// 	/* Print info about runtime of triggers */
// 	if (es->analyze)
// 		ExplainPrintTriggers(es, queryDesc);

// 	/*
// 	 * Close down the query and free resources.  Include time for this in the
// 	 * total execution time (although it should be pretty minimal).
// 	 */
// 	INSTR_TIME_SET_CURRENT(starttime);

// 	ExecutorEnd(queryDesc);

// 	FreeQueryDesc(queryDesc);

// 	PopActiveSnapshot();

// 	/* We need a CCI just in case query expanded to multiple plans */
// 	if (es->analyze)
// 		CommandCounterIncrement();

// 	totaltime += elapsed_time(&starttime);

// 	if (es->summary)
// 	{
// 		if (es->format == EXPLAIN_FORMAT_TEXT)
// 			appendStringInfo(es->str, "Execution time: %.3f ms\n",
// 							 1000.0 * totaltime);
// 		else
// 			ExplainPropertyFloat("Execution Time", 1000.0 * totaltime,
// 								 3, es);
// 	}

// 	ExplainCloseGroup("Query", NULL, true, es);
// }
// //end

// ExplainState *
// NewExplainState(void)
// {
// 	ExplainState *es = (ExplainState *) palloc0(sizeof(ExplainState));

// 	/* Set default options (most fields can be left as zeroes). */
// 	es->costs = true;
// 	/* Prepare output buffer. */
// 	es->str = makeStringInfo();

// 	return es;
// }
// //end

// void
// ExplainPrintTriggers(ExplainState *es, QueryDesc *queryDesc)
// {
// 	ResultRelInfo *rInfo;
// 	bool		show_relname;
// 	int			numrels = queryDesc->estate->es_num_result_relations;
// 	List	   *targrels = queryDesc->estate->es_trig_target_relations;
// 	int			nr;
// 	ListCell   *l;

// 	ExplainOpenGroup("Triggers", "Triggers", false, es);

// 	show_relname = (numrels > 1 || targrels != NIL);
// 	rInfo = queryDesc->estate->es_result_relations;
// 	for (nr = 0; nr < numrels; rInfo++, nr++)
// 		report_triggers(rInfo, show_relname, es);

// 	foreach(l, targrels)
// 	{
// 		rInfo = (ResultRelInfo *) lfirst(l);
// 		report_triggers(rInfo, show_relname, es);
// 	}

// 	ExplainCloseGroup("Triggers", "Triggers", false, es);
// }
// //end
// void
// ExplainPropertyBool(const char *qlabel, bool value, ExplainState *es)
// {
// 	ExplainProperty(qlabel, value ? "true" : "false", true, es);
// }
// //end

// //end explain.c
// // sequence.c
// ObjectAddress
// DefineSequence(CreateSeqStmt *seq)
// {
// 	FormData_pg_sequence new;
// 	List	   *owned_by;
// 	CreateStmt *stmt = makeNode(CreateStmt);
// 	Oid			seqoid;
// 	ObjectAddress address;
// 	Relation	rel;
// 	HeapTuple	tuple;
// 	TupleDesc	tupDesc;
// 	Datum		value[SEQ_COL_LASTCOL];
// 	bool		null[SEQ_COL_LASTCOL];
// 	int			i;
// 	NameData	name;

// 	/* Unlogged sequences are not implemented -- not clear if useful. */
// 	if (seq->sequence->relpersistence == RELPERSISTENCE_UNLOGGED)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("unlogged sequences are not supported")));

// 	/*
// 	 * If if_not_exists was given and a relation with the same name already
// 	 * exists, bail out. (Note: we needn't check this when not if_not_exists,
// 	 * because DefineRelation will complain anyway.)
// 	 */
// 	if (seq->if_not_exists)
// 	{
// 		RangeVarGetAndCheckCreationNamespace(seq->sequence, NoLock, &seqoid);
// 		if (OidIsValid(seqoid))
// 		{
// 			ereport(NOTICE,
// 					(errcode(ERRCODE_DUPLICATE_TABLE),
// 					 errmsg("relation \"%s\" already exists, skipping",
// 							seq->sequence->relname)));
// 			return InvalidObjectAddress;
// 		}
// 	}

// 	/* Check and set all option values */
// 	init_params(seq->options, true, &new, &owned_by);

// 	/*
// 	 * Create relation (and fill value[] and null[] for the tuple)
// 	 */
// 	stmt->tableElts = NIL;
// 	for (i = SEQ_COL_FIRSTCOL; i <= SEQ_COL_LASTCOL; i++)
// 	{
// 		ColumnDef  *coldef = makeNode(ColumnDef);

// 		coldef->inhcount = 0;
// 		coldef->is_local = true;
// 		coldef->is_not_null = true;
// 		coldef->is_from_type = false;
// 		coldef->storage = 0;
// 		coldef->raw_default = NULL;
// 		coldef->cooked_default = NULL;
// 		coldef->collClause = NULL;
// 		coldef->collOid = InvalidOid;
// 		coldef->constraints = NIL;
// 		coldef->location = -1;

// 		null[i - 1] = false;

// 		switch (i)
// 		{
// 			case SEQ_COL_NAME:
// 				coldef->typeName = makeTypeNameFromOid(NAMEOID, -1);
// 				coldef->colname = "sequence_name";
// 				namestrcpy(&name, seq->sequence->relname);
// 				value[i - 1] = NameGetDatum(&name);
// 				break;
// 			case SEQ_COL_LASTVAL:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "last_value";
// 				value[i - 1] = Int64GetDatumFast(new.last_value);
// 				break;
// 			case SEQ_COL_STARTVAL:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "start_value";
// 				value[i - 1] = Int64GetDatumFast(new.start_value);
// 				break;
// 			case SEQ_COL_INCBY:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "increment_by";
// 				value[i - 1] = Int64GetDatumFast(new.increment_by);
// 				break;
// 			case SEQ_COL_MAXVALUE:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "max_value";
// 				value[i - 1] = Int64GetDatumFast(new.max_value);
// 				break;
// 			case SEQ_COL_MINVALUE:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "min_value";
// 				value[i - 1] = Int64GetDatumFast(new.min_value);
// 				break;
// 			case SEQ_COL_CACHE:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "cache_value";
// 				value[i - 1] = Int64GetDatumFast(new.cache_value);
// 				break;
// 			case SEQ_COL_LOG:
// 				coldef->typeName = makeTypeNameFromOid(INT8OID, -1);
// 				coldef->colname = "log_cnt";
// 				value[i - 1] = Int64GetDatum((int64) 0);
// 				break;
// 			case SEQ_COL_CYCLE:
// 				coldef->typeName = makeTypeNameFromOid(BOOLOID, -1);
// 				coldef->colname = "is_cycled";
// 				value[i - 1] = BoolGetDatum(new.is_cycled);
// 				break;
// 			case SEQ_COL_CALLED:
// 				coldef->typeName = makeTypeNameFromOid(BOOLOID, -1);
// 				coldef->colname = "is_called";
// 				value[i - 1] = BoolGetDatum(false);
// 				break;
// 		}
// 		stmt->tableElts = lappend(stmt->tableElts, coldef);
// 	}

// 	stmt->relation = seq->sequence;
// 	stmt->inhRelations = NIL;
// 	stmt->constraints = NIL;
// 	stmt->options = NIL;
// 	stmt->oncommit = ONCOMMIT_NOOP;
// 	stmt->tablespacename = NULL;
// 	stmt->if_not_exists = seq->if_not_exists;

// 	address = DefineRelation(stmt, RELKIND_SEQUENCE, seq->ownerId, NULL);
// 	seqoid = address.objectId;
// 	Assert(seqoid != InvalidOid);

// 	rel = heap_open(seqoid, AccessExclusiveLock);
// 	tupDesc = RelationGetDescr(rel);

// 	/* now initialize the sequence's data */
// 	tuple = heap_form_tuple(tupDesc, value, null);
// 	fill_seq_with_data(rel, tuple);

// 	/* process OWNED BY if given */
// 	if (owned_by)
// 		process_owned_by(rel, owned_by);

// 	heap_close(rel, NoLock);

// 	return address;
// }
// //end

// ObjectAddress
// AlterSequence(AlterSeqStmt *stmt)
// {
// 	Oid			relid;
// 	SeqTable	elm;
// 	Relation	seqrel;
// 	Buffer		buf;
// 	HeapTupleData seqtuple;
// 	Form_pg_sequence seq;
// 	FormData_pg_sequence new;
// 	List	   *owned_by;
// 	ObjectAddress address;

// 	/* Open and lock sequence. */
// 	relid = RangeVarGetRelid(stmt->sequence, AccessShareLock, stmt->missing_ok);
// 	if (relid == InvalidOid)
// 	{
// 		ereport(NOTICE,
// 				(errmsg("relation \"%s\" does not exist, skipping",
// 						stmt->sequence->relname)));
// 		return InvalidObjectAddress;
// 	}

// 	init_sequence(relid, &elm, &seqrel);

// 	/* allow ALTER to sequence owner only */
// 	if (!pg_class_ownercheck(relid, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_CLASS,
// 					   stmt->sequence->relname);

// 	/* lock page' buffer and read tuple into new sequence structure */
// 	seq = read_seq_tuple(elm, seqrel, &buf, &seqtuple);

// 	/* Copy old values of options into workspace */
// 	memcpy(&new, seq, sizeof(FormData_pg_sequence));

// 	/* Check and set new values */
// 	init_params(stmt->options, false, &new, &owned_by);

// 	/* Clear local cache so that we don't think we have cached numbers */
// 	/* Note that we do not change the currval() state */
// 	elm->cached = elm->last;

// 	/* check the comment above nextval_internal()'s equivalent call. */
// 	if (RelationNeedsWAL(seqrel))
// 		GetTopTransactionId();

// 	/* Now okay to update the on-disk tuple */
// 	START_CRIT_SECTION();

// 	memcpy(seq, &new, sizeof(FormData_pg_sequence));

// 	MarkBufferDirty(buf);

// 	/* XLOG stuff */
// 	if (RelationNeedsWAL(seqrel))
// 	{
// 		xl_seq_rec	xlrec;
// 		XLogRecPtr	recptr;
// 		Page		page = BufferGetPage(buf);

// 		XLogBeginInsert();
// 		XLogRegisterBuffer(0, buf, REGBUF_WILL_INIT);

// 		xlrec.node = seqrel->rd_node;
// 		XLogRegisterData((char *) &xlrec, sizeof(xl_seq_rec));

// 		XLogRegisterData((char *) seqtuple.t_data, seqtuple.t_len);

// 		recptr = XLogInsert(RM_SEQ_ID, XLOG_SEQ_LOG);

// 		PageSetLSN(page, recptr);
// 	}

// 	END_CRIT_SECTION();

// 	UnlockReleaseBuffer(buf);

// 	/* process OWNED BY if given */
// 	if (owned_by)
// 		process_owned_by(seqrel, owned_by);

// 	InvokeObjectPostAlterHook(RelationRelationId, relid, 0);

// 	ObjectAddressSet(address, RelationRelationId, relid);

// 	relation_close(seqrel, NoLock);

// 	return address;
// }
// //end
// void
// ResetSequenceCaches(void)
// {
// 	if (seqhashtab)
// 	{
// 		hash_destroy(seqhashtab);
// 		seqhashtab = NULL;
// 	}

// 	last_used_seq = NULL;
// }
// //end

// //tablecmds.c
// ObjectAddress
// DefineRelation(CreateStmt *stmt, char relkind, Oid ownerId,
// 			   ObjectAddress *typaddress)
// {
// 	char		relname[NAMEDATALEN];
// 	Oid			namespaceId;
// 	List	   *schema = stmt->tableElts;
// 	Oid			relationId;
// 	Oid			tablespaceId;
// 	Relation	rel;
// 	TupleDesc	descriptor;
// 	List	   *inheritOids;
// 	List	   *old_constraints;
// 	bool		localHasOids;
// 	int			parentOidCount;
// 	List	   *rawDefaults;
// 	List	   *cookedDefaults;
// 	Datum		reloptions;
// 	ListCell   *listptr;
// 	AttrNumber	attnum;
// 	static char *validnsps[] = HEAP_RELOPT_NAMESPACES;
// 	Oid			ofTypeId;
// 	ObjectAddress address;

// 	/*
// 	 * Truncate relname to appropriate length (probably a waste of time, as
// 	 * parser should have done this already).
// 	 */
// 	StrNCpy(relname, stmt->relation->relname, NAMEDATALEN);

// 	/*
// 	 * Check consistency of arguments
// 	 */
// 	if (stmt->oncommit != ONCOMMIT_NOOP
// 		&& stmt->relation->relpersistence != RELPERSISTENCE_TEMP)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_TABLE_DEFINITION),
// 				 errmsg("ON COMMIT can only be used on temporary tables")));

// 	/*
// 	 * Look up the namespace in which we are supposed to create the relation,
// 	 * check we have permission to create there, lock it against concurrent
// 	 * drop, and mark stmt->relation as RELPERSISTENCE_TEMP if a temporary
// 	 * namespace is selected.
// 	 */
// 	namespaceId =
// 		RangeVarGetAndCheckCreationNamespace(stmt->relation, NoLock, NULL);

// 	/*
// 	 * Security check: disallow creating temp tables from security-restricted
// 	 * code.  This is needed because calling code might not expect untrusted
// 	 * tables to appear in pg_temp at the front of its search path.
// 	 */
// 	if (stmt->relation->relpersistence == RELPERSISTENCE_TEMP
// 		&& InSecurityRestrictedOperation())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("cannot create temporary table within security-restricted operation")));

// 	/*
// 	 * Select tablespace to use.  If not specified, use default tablespace
// 	 * (which may in turn default to database's default).
// 	 */
// 	if (stmt->tablespacename)
// 	{
// 		tablespaceId = get_tablespace_oid(stmt->tablespacename, false);
// 	}
// 	else
// 	{
// 		tablespaceId = GetDefaultTablespace(stmt->relation->relpersistence);
// 		/* note InvalidOid is OK in this case */
// 	}

// 	/* Check permissions except when using database's default */
// 	if (OidIsValid(tablespaceId) && tablespaceId != MyDatabaseTableSpace)
// 	{
// 		AclResult	aclresult;

// 		aclresult = pg_tablespace_aclcheck(tablespaceId, GetUserId(),
// 										   ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_TABLESPACE,
// 						   get_tablespace_name(tablespaceId));
// 	}

// 	/* In all cases disallow placing user relations in pg_global */
// 	if (tablespaceId == GLOBALTABLESPACE_OID)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("only shared relations can be placed in pg_global tablespace")));

// 	/* Identify user ID that will own the table */
// 	if (!OidIsValid(ownerId))
// 		ownerId = GetUserId();

// 	/*
// 	 * Parse and validate reloptions, if any.
// 	 */
// 	reloptions = transformRelOptions((Datum) 0, stmt->options, NULL, validnsps,
// 									 true, false);

// 	if (relkind == RELKIND_VIEW)
// 		(void) view_reloptions(reloptions, true);
// 	else
// 		(void) heap_reloptions(relkind, reloptions, true);

// 	if (stmt->ofTypename)
// 	{
// 		AclResult	aclresult;

// 		ofTypeId = typenameTypeId(NULL, stmt->ofTypename);

// 		aclresult = pg_type_aclcheck(ofTypeId, GetUserId(), ACL_USAGE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error_type(aclresult, ofTypeId);
// 	}
// 	else
// 		ofTypeId = InvalidOid;

// 	/*
// 	 * Look up inheritance ancestors and generate relation schema, including
// 	 * inherited attributes.
// 	 */
// 	schema = MergeAttributes(schema, stmt->inhRelations,
// 							 stmt->relation->relpersistence,
// 							 &inheritOids, &old_constraints, &parentOidCount);

// 	/*
// 	 * Create a tuple descriptor from the relation schema.  Note that this
// 	 * deals with column names, types, and NOT NULL constraints, but not
// 	 * default values or CHECK constraints; we handle those below.
// 	 */
// 	descriptor = BuildDescForRelation(schema);

// 	/*
// 	 * Notice that we allow OIDs here only for plain tables, even though some
// 	 * other relkinds can support them.  This is necessary because the
// 	 * default_with_oids GUC must apply only to plain tables and not any other
// 	 * relkind; doing otherwise would break existing pg_dump files.  We could
// 	 * allow explicit "WITH OIDS" while not allowing default_with_oids to
// 	 * affect other relkinds, but it would complicate interpretOidsOption().
// 	 */
// 	localHasOids = interpretOidsOption(stmt->options,
// 									   (relkind == RELKIND_RELATION));
// 	descriptor->tdhasoid = (localHasOids || parentOidCount > 0);

// 	/*
// 	 * Find columns with default values and prepare for insertion of the
// 	 * defaults.  Pre-cooked (that is, inherited) defaults go into a list of
// 	 * CookedConstraint structs that we'll pass to heap_create_with_catalog,
// 	 * while raw defaults go into a list of RawColumnDefault structs that will
// 	 * be processed by AddRelationNewConstraints.  (We can't deal with raw
// 	 * expressions until we can do transformExpr.)
// 	 *
// 	 * We can set the atthasdef flags now in the tuple descriptor; this just
// 	 * saves StoreAttrDefault from having to do an immediate update of the
// 	 * pg_attribute rows.
// 	 */
// 	rawDefaults = NIL;
// 	cookedDefaults = NIL;
// 	attnum = 0;

// 	foreach(listptr, schema)
// 	{
// 		ColumnDef  *colDef = lfirst(listptr);

// 		attnum++;

// 		if (colDef->raw_default != NULL)
// 		{
// 			RawColumnDefault *rawEnt;

// 			Assert(colDef->cooked_default == NULL);

// 			rawEnt = (RawColumnDefault *) palloc(sizeof(RawColumnDefault));
// 			rawEnt->attnum = attnum;
// 			rawEnt->raw_default = colDef->raw_default;
// 			rawDefaults = lappend(rawDefaults, rawEnt);
// 			descriptor->attrs[attnum - 1]->atthasdef = true;
// 		}
// 		else if (colDef->cooked_default != NULL)
// 		{
// 			CookedConstraint *cooked;

// 			cooked = (CookedConstraint *) palloc(sizeof(CookedConstraint));
// 			cooked->contype = CONSTR_DEFAULT;
// 			cooked->conoid = InvalidOid;		/* until created */
// 			cooked->name = NULL;
// 			cooked->attnum = attnum;
// 			cooked->expr = colDef->cooked_default;
// 			cooked->skip_validation = false;
// 			cooked->is_local = true;	/* not used for defaults */
// 			cooked->inhcount = 0;		/* ditto */
// 			cooked->is_no_inherit = false;
// 			cookedDefaults = lappend(cookedDefaults, cooked);
// 			descriptor->attrs[attnum - 1]->atthasdef = true;
// 		}
// 	}

// 	/*
// 	 * Create the relation.  Inherited defaults and constraints are passed in
// 	 * for immediate handling --- since they don't need parsing, they can be
// 	 * stored immediately.
// 	 */
// 	relationId = heap_create_with_catalog(relname,
// 										  namespaceId,
// 										  tablespaceId,
// 										  InvalidOid,
// 										  InvalidOid,
// 										  ofTypeId,
// 										  ownerId,
// 										  descriptor,
// 										  list_concat(cookedDefaults,
// 													  old_constraints),
// 										  relkind,
// 										  stmt->relation->relpersistence,
// 										  false,
// 										  false,
// 										  localHasOids,
// 										  parentOidCount,
// 										  stmt->oncommit,
// 										  reloptions,
// 										  true,
// 										  allowSystemTableMods,
// 										  false,
// 										  typaddress);

// 	/* Store inheritance information for new rel. */
// 	StoreCatalogInheritance(relationId, inheritOids);

// 	/*
// 	 * We must bump the command counter to make the newly-created relation
// 	 * tuple visible for opening.
// 	 */
// 	CommandCounterIncrement();

// 	/*
// 	 * Open the new relation and acquire exclusive lock on it.  This isn't
// 	 * really necessary for locking out other backends (since they can't see
// 	 * the new rel anyway until we commit), but it keeps the lock manager from
// 	 * complaining about deadlock risks.
// 	 */
// 	rel = relation_open(relationId, AccessExclusiveLock);

// 	/*
// 	 * Now add any newly specified column default values and CHECK constraints
// 	 * to the new relation.  These are passed to us in the form of raw
// 	 * parsetrees; we need to transform them to executable expression trees
// 	 * before they can be added. The most convenient way to do that is to
// 	 * apply the parser's transformExpr routine, but transformExpr doesn't
// 	 * work unless we have a pre-existing relation. So, the transformation has
// 	 * to be postponed to this final step of CREATE TABLE.
// 	 */
// 	if (rawDefaults || stmt->constraints)
// 		AddRelationNewConstraints(rel, rawDefaults, stmt->constraints,
// 								  true, true, false);

// 	ObjectAddressSet(address, RelationRelationId, relationId);

// 	/*
// 	 * Clean up.  We keep lock on new relation (although it shouldn't be
// 	 * visible to anyone else anyway, until commit).
// 	 */
// 	relation_close(rel, NoLock);

// 	return address;
// }
// //end

// void
// RemoveRelations(DropStmt *drop)
// {
// 	ObjectAddresses *objects;
// 	char		relkind;
// 	ListCell   *cell;
// 	int			flags = 0;
// 	LOCKMODE	lockmode = AccessExclusiveLock;

// 	/* DROP CONCURRENTLY uses a weaker lock, and has some restrictions */
// 	if (drop->concurrent)
// 	{
// 		flags |= PERFORM_DELETION_CONCURRENTLY;
// 		lockmode = ShareUpdateExclusiveLock;
// 		Assert(drop->removeType == OBJECT_INDEX);
// 		if (list_length(drop->objects) != 1)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("DROP INDEX CONCURRENTLY does not support dropping multiple objects")));
// 		if (drop->behavior == DROP_CASCADE)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				errmsg("DROP INDEX CONCURRENTLY does not support CASCADE")));
// 	}

// 	/*
// 	 * First we identify all the relations, then we delete them in a single
// 	 * performMultipleDeletions() call.  This is to avoid unwanted DROP
// 	 * RESTRICT errors if one of the relations depends on another.
// 	 */

// 	/* Determine required relkind */
// 	switch (drop->removeType)
// 	{
// 		case OBJECT_TABLE:
// 			relkind = RELKIND_RELATION;
// 			break;

// 		case OBJECT_INDEX:
// 			relkind = RELKIND_INDEX;
// 			break;

// 		case OBJECT_SEQUENCE:
// 			relkind = RELKIND_SEQUENCE;
// 			break;

// 		case OBJECT_VIEW:
// 			relkind = RELKIND_VIEW;
// 			break;

// 		case OBJECT_MATVIEW:
// 			relkind = RELKIND_MATVIEW;
// 			break;

// 		case OBJECT_FOREIGN_TABLE:
// 			relkind = RELKIND_FOREIGN_TABLE;
// 			break;

// 		default:
// 			elog(ERROR, "unrecognized drop object type: %d",
// 				 (int) drop->removeType);
// 			relkind = 0;		/* keep compiler quiet */
// 			break;
// 	}

// 	/* Lock and validate each relation; build a list of object addresses */
// 	objects = new_object_addresses();

// 	foreach(cell, drop->objects)
// 	{
// 		RangeVar   *rel = makeRangeVarFromNameList((List *) lfirst(cell));
// 		Oid			relOid;
// 		ObjectAddress obj;
// 		struct DropRelationCallbackState state;

// 		/*
// 		 * These next few steps are a great deal like relation_openrv, but we
// 		 * don't bother building a relcache entry since we don't need it.
// 		 *
// 		 * Check for shared-cache-inval messages before trying to access the
// 		 * relation.  This is needed to cover the case where the name
// 		 * identifies a rel that has been dropped and recreated since the
// 		 * start of our transaction: if we don't flush the old syscache entry,
// 		 * then we'll latch onto that entry and suffer an error later.
// 		 */
// 		AcceptInvalidationMessages();

// 		/* Look up the appropriate relation using namespace search. */
// 		state.relkind = relkind;
// 		state.heapOid = InvalidOid;
// 		state.concurrent = drop->concurrent;
// 		relOid = RangeVarGetRelidExtended(rel, lockmode, true,
// 										  false,
// 										  RangeVarCallbackForDropRelation,
// 										  (void *) &state);

// 		/* Not there? */
// 		if (!OidIsValid(relOid))
// 		{
// 			DropErrorMsgNonExistent(rel, relkind, drop->missing_ok);
// 			continue;
// 		}

// 		/* OK, we're ready to delete this one */
// 		obj.classId = RelationRelationId;
// 		obj.objectId = relOid;
// 		obj.objectSubId = 0;

// 		add_exact_object_address(&obj, objects);
// 	}

// 	performMultipleDeletions(objects, drop->behavior, flags);

// 	free_object_addresses(objects);
// }
// //end
// Oid
// AlterTableLookupRelation(AlterTableStmt *stmt, LOCKMODE lockmode)
// {
// 	return RangeVarGetRelidExtended(stmt->relation, lockmode, stmt->missing_ok, false,
// 									RangeVarCallbackForAlterRelation,
// 									(void *) stmt);
// }
// //end

// ObjectAddress
// AlterTableNamespace(AlterObjectSchemaStmt *stmt, Oid *oldschema)
// {
// 	Relation	rel;
// 	Oid			relid;
// 	Oid			oldNspOid;
// 	Oid			nspOid;
// 	RangeVar   *newrv;
// 	ObjectAddresses *objsMoved;
// 	ObjectAddress myself;

// 	relid = RangeVarGetRelidExtended(stmt->relation, AccessExclusiveLock,
// 									 stmt->missing_ok, false,
// 									 RangeVarCallbackForAlterRelation,
// 									 (void *) stmt);

// 	if (!OidIsValid(relid))
// 	{
// 		ereport(NOTICE,
// 				(errmsg("relation \"%s\" does not exist, skipping",
// 						stmt->relation->relname)));
// 		return InvalidObjectAddress;
// 	}

// 	rel = relation_open(relid, NoLock);

// 	oldNspOid = RelationGetNamespace(rel);

// 	/* If it's an owned sequence, disallow moving it by itself. */
// 	if (rel->rd_rel->relkind == RELKIND_SEQUENCE)
// 	{
// 		Oid			tableId;
// 		int32		colId;

// 		if (sequenceIsOwned(relid, &tableId, &colId))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("cannot move an owned sequence into another schema"),
// 					 errdetail("Sequence \"%s\" is linked to table \"%s\".",
// 							   RelationGetRelationName(rel),
// 							   get_rel_name(tableId))));
// 	}

// 	/* Get and lock schema OID and check its permissions. */
// 	newrv = makeRangeVar(stmt->newschema, RelationGetRelationName(rel), -1);
// 	nspOid = RangeVarGetAndCheckCreationNamespace(newrv, NoLock, NULL);

// 	/* common checks on switching namespaces */
// 	CheckSetNamespace(oldNspOid, nspOid);

// 	objsMoved = new_object_addresses();
// 	AlterTableNamespaceInternal(rel, oldNspOid, nspOid, objsMoved);
// 	free_object_addresses(objsMoved);

// 	ObjectAddressSet(myself, RelationRelationId, relid);

// 	if (oldschema)
// 		*oldschema = oldNspOid;

// 	/* close rel, but keep lock until commit */
// 	relation_close(rel, NoLock);

// 	return myself;
// }
// //end

// void
// ExecuteTruncate(TruncateStmt *stmt)
// {
// 	List	   *rels = NIL;
// 	List	   *relids = NIL;
// 	List	   *seq_relids = NIL;
// 	EState	   *estate;
// 	ResultRelInfo *resultRelInfos;
// 	ResultRelInfo *resultRelInfo;
// 	SubTransactionId mySubid;
// 	ListCell   *cell;

// 	/*
// 	 * Open, exclusive-lock, and check all the explicitly-specified relations
// 	 */
// 	foreach(cell, stmt->relations)
// 	{
// 		RangeVar   *rv = lfirst(cell);
// 		Relation	rel;
// 		bool		recurse = interpretInhOption(rv->inhOpt);
// 		Oid			myrelid;

// 		rel = heap_openrv(rv, AccessExclusiveLock);
// 		myrelid = RelationGetRelid(rel);
// 		/* don't throw error for "TRUNCATE foo, foo" */
// 		if (list_member_oid(relids, myrelid))
// 		{
// 			heap_close(rel, AccessExclusiveLock);
// 			continue;
// 		}
// 		truncate_check_rel(rel);
// 		rels = lappend(rels, rel);
// 		relids = lappend_oid(relids, myrelid);

// 		if (recurse)
// 		{
// 			ListCell   *child;
// 			List	   *children;

// 			children = find_all_inheritors(myrelid, AccessExclusiveLock, NULL);

// 			foreach(child, children)
// 			{
// 				Oid			childrelid = lfirst_oid(child);

// 				if (list_member_oid(relids, childrelid))
// 					continue;

// 				/* find_all_inheritors already got lock */
// 				rel = heap_open(childrelid, NoLock);
// 				truncate_check_rel(rel);
// 				rels = lappend(rels, rel);
// 				relids = lappend_oid(relids, childrelid);
// 			}
// 		}
// 	}

// 	/*
// 	 * In CASCADE mode, suck in all referencing relations as well.  This
// 	 * requires multiple iterations to find indirectly-dependent relations. At
// 	 * each phase, we need to exclusive-lock new rels before looking for their
// 	 * dependencies, else we might miss something.  Also, we check each rel as
// 	 * soon as we open it, to avoid a faux pas such as holding lock for a long
// 	 * time on a rel we have no permissions for.
// 	 */
// 	if (stmt->behavior == DROP_CASCADE)
// 	{
// 		for (;;)
// 		{
// 			List	   *newrelids;

// 			newrelids = heap_truncate_find_FKs(relids);
// 			if (newrelids == NIL)
// 				break;			/* nothing else to add */

// 			foreach(cell, newrelids)
// 			{
// 				Oid			relid = lfirst_oid(cell);
// 				Relation	rel;

// 				rel = heap_open(relid, AccessExclusiveLock);
// 				ereport(NOTICE,
// 						(errmsg("truncate cascades to table \"%s\"",
// 								RelationGetRelationName(rel))));
// 				truncate_check_rel(rel);
// 				rels = lappend(rels, rel);
// 				relids = lappend_oid(relids, relid);
// 			}
// 		}
// 	}

// 	/*
// 	 * Check foreign key references.  In CASCADE mode, this should be
// 	 * unnecessary since we just pulled in all the references; but as a
// 	 * cross-check, do it anyway if in an Assert-enabled build.
// 	 */
// #ifdef USE_ASSERT_CHECKING
// 	heap_truncate_check_FKs(rels, false);
// #else
// 	if (stmt->behavior == DROP_RESTRICT)
// 		heap_truncate_check_FKs(rels, false);
// #endif

// 	/*
// 	 * If we are asked to restart sequences, find all the sequences, lock them
// 	 * (we need AccessExclusiveLock for ResetSequence), and check permissions.
// 	 * We want to do this early since it's pointless to do all the truncation
// 	 * work only to fail on sequence permissions.
// 	 */
// 	if (stmt->restart_seqs)
// 	{
// 		foreach(cell, rels)
// 		{
// 			Relation	rel = (Relation) lfirst(cell);
// 			List	   *seqlist = getOwnedSequences(RelationGetRelid(rel));
// 			ListCell   *seqcell;

// 			foreach(seqcell, seqlist)
// 			{
// 				Oid			seq_relid = lfirst_oid(seqcell);
// 				Relation	seq_rel;

// 				seq_rel = relation_open(seq_relid, AccessExclusiveLock);

// 				/* This check must match AlterSequence! */
// 				if (!pg_class_ownercheck(seq_relid, GetUserId()))
// 					aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_CLASS,
// 								   RelationGetRelationName(seq_rel));

// 				seq_relids = lappend_oid(seq_relids, seq_relid);

// 				relation_close(seq_rel, NoLock);
// 			}
// 		}
// 	}

// 	/* Prepare to catch AFTER triggers. */
// 	AfterTriggerBeginQuery();

// 	/*
// 	 * To fire triggers, we'll need an EState as well as a ResultRelInfo for
// 	 * each relation.  We don't need to call ExecOpenIndices, though.
// 	 */
// 	estate = CreateExecutorState();
// 	resultRelInfos = (ResultRelInfo *)
// 		palloc(list_length(rels) * sizeof(ResultRelInfo));
// 	resultRelInfo = resultRelInfos;
// 	foreach(cell, rels)
// 	{
// 		Relation	rel = (Relation) lfirst(cell);

// 		InitResultRelInfo(resultRelInfo,
// 						  rel,
// 						  0,	/* dummy rangetable index */
// 						  0);
// 		resultRelInfo++;
// 	}
// 	estate->es_result_relations = resultRelInfos;
// 	estate->es_num_result_relations = list_length(rels);

// 	/*
// 	 * Process all BEFORE STATEMENT TRUNCATE triggers before we begin
// 	 * truncating (this is because one of them might throw an error). Also, if
// 	 * we were to allow them to prevent statement execution, that would need
// 	 * to be handled here.
// 	 */
// 	resultRelInfo = resultRelInfos;
// 	foreach(cell, rels)
// 	{
// 		estate->es_result_relation_info = resultRelInfo;
// 		ExecBSTruncateTriggers(estate, resultRelInfo);
// 		resultRelInfo++;
// 	}

// 	/*
// 	 * OK, truncate each table.
// 	 */
// 	mySubid = GetCurrentSubTransactionId();

// 	foreach(cell, rels)
// 	{
// 		Relation	rel = (Relation) lfirst(cell);

// 		/*
// 		 * Normally, we need a transaction-safe truncation here.  However, if
// 		 * the table was either created in the current (sub)transaction or has
// 		 * a new relfilenode in the current (sub)transaction, then we can just
// 		 * truncate it in-place, because a rollback would cause the whole
// 		 * table or the current physical file to be thrown away anyway.
// 		 */
// 		if (rel->rd_createSubid == mySubid ||
// 			rel->rd_newRelfilenodeSubid == mySubid)
// 		{
// 			/* Immediate, non-rollbackable truncation is OK */
// 			heap_truncate_one_rel(rel);
// 		}
// 		else
// 		{
// 			Oid			heap_relid;
// 			Oid			toast_relid;
// 			MultiXactId minmulti;

// 			/*
// 			 * This effectively deletes all rows in the table, and may be done
// 			 * in a serializable transaction.  In that case we must record a
// 			 * rw-conflict in to this transaction from each transaction
// 			 * holding a predicate lock on the table.
// 			 */
// 			CheckTableForSerializableConflictIn(rel);

// 			minmulti = GetOldestMultiXactId();

// 			/*
// 			 * Need the full transaction-safe pushups.
// 			 *
// 			 * Create a new empty storage file for the relation, and assign it
// 			 * as the relfilenode value. The old storage file is scheduled for
// 			 * deletion at commit.
// 			 */
// 			RelationSetNewRelfilenode(rel, rel->rd_rel->relpersistence,
// 									  RecentXmin, minmulti);
// 			if (rel->rd_rel->relpersistence == RELPERSISTENCE_UNLOGGED)
// 				heap_create_init_fork(rel);

// 			heap_relid = RelationGetRelid(rel);
// 			toast_relid = rel->rd_rel->reltoastrelid;

// 			/*
// 			 * The same for the toast table, if any.
// 			 */
// 			if (OidIsValid(toast_relid))
// 			{
// 				rel = relation_open(toast_relid, AccessExclusiveLock);
// 				RelationSetNewRelfilenode(rel, rel->rd_rel->relpersistence,
// 										  RecentXmin, minmulti);
// 				if (rel->rd_rel->relpersistence == RELPERSISTENCE_UNLOGGED)
// 					heap_create_init_fork(rel);
// 				heap_close(rel, NoLock);
// 			}

// 			/*
// 			 * Reconstruct the indexes to match, and we're done.
// 			 */
// 			reindex_relation(heap_relid, REINDEX_REL_PROCESS_TOAST, 0);
// 		}

// 		pgstat_count_truncate(rel);
// 	}

// 	/*
// 	 * Restart owned sequences if we were asked to.
// 	 */
// 	foreach(cell, seq_relids)
// 	{
// 		Oid			seq_relid = lfirst_oid(cell);

// 		ResetSequence(seq_relid);
// 	}

// 	/*
// 	 * Process all AFTER STATEMENT TRUNCATE triggers.
// 	 */
// 	resultRelInfo = resultRelInfos;
// 	foreach(cell, rels)
// 	{
// 		estate->es_result_relation_info = resultRelInfo;
// 		ExecASTruncateTriggers(estate, resultRelInfo);
// 		resultRelInfo++;
// 	}

// 	/* Handle queued AFTER triggers */
// 	AfterTriggerEndQuery(estate);

// 	/* We can clean up the EState now */
// 	FreeExecutorState(estate);

// 	/* And close the rels (can't do this while EState still holds refs) */
// 	foreach(cell, rels)
// 	{
// 		Relation	rel = (Relation) lfirst(cell);

// 		heap_close(rel, NoLock);
// 	}
// }
// //end
// ObjectAddress
// renameatt(RenameStmt *stmt)
// {
// 	Oid			relid;
// 	AttrNumber	attnum;
// 	ObjectAddress address;

// 	/* lock level taken here should match renameatt_internal */
// 	relid = RangeVarGetRelidExtended(stmt->relation, AccessExclusiveLock,
// 									 stmt->missing_ok, false,
// 									 RangeVarCallbackForRenameAttribute,
// 									 NULL);

// 	if (!OidIsValid(relid))
// 	{
// 		ereport(NOTICE,
// 				(errmsg("relation \"%s\" does not exist, skipping",
// 						stmt->relation->relname)));
// 		return InvalidObjectAddress;
// 	}

// 	attnum =
// 		renameatt_internal(relid,
// 						   stmt->subname,		/* old att name */
// 						   stmt->newname,		/* new att name */
// 						   interpretInhOption(stmt->relation->inhOpt),	/* recursive? */
// 						   false,		/* recursing? */
// 						   0,	/* expected inhcount */
// 						   stmt->behavior);

// 	ObjectAddressSubSet(address, RelationRelationId, relid, attnum);

// 	return address;
// }
// //end
// ObjectAddress
// RenameConstraint(RenameStmt *stmt)
// {
// 	Oid			relid = InvalidOid;
// 	Oid			typid = InvalidOid;

// 	if (stmt->renameType == OBJECT_DOMCONSTRAINT)
// 	{
// 		Relation	rel;
// 		HeapTuple	tup;

// 		typid = typenameTypeId(NULL, makeTypeNameFromNameList(stmt->object));
// 		rel = heap_open(TypeRelationId, RowExclusiveLock);
// 		tup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(typid));
// 		if (!HeapTupleIsValid(tup))
// 			elog(ERROR, "cache lookup failed for type %u", typid);
// 		checkDomainOwner(tup);
// 		ReleaseSysCache(tup);
// 		heap_close(rel, NoLock);
// 	}
// 	else
// 	{
// 		/* lock level taken here should match rename_constraint_internal */
// 		relid = RangeVarGetRelidExtended(stmt->relation, AccessExclusiveLock,
// 										 stmt->missing_ok, false,
// 										 RangeVarCallbackForRenameAttribute,
// 										 NULL);
// 		if (!OidIsValid(relid))
// 		{
// 			ereport(NOTICE,
// 					(errmsg("relation \"%s\" does not exist, skipping",
// 							stmt->relation->relname)));
// 			return InvalidObjectAddress;
// 		}
// 	}

// 	return
// 		rename_constraint_internal(relid, typid,
// 								   stmt->subname,
// 								   stmt->newname,
// 		 stmt->relation ? interpretInhOption(stmt->relation->inhOpt) : false,	/* recursive? */
// 								   false,		/* recursing? */
// 								   0 /* expected inhcount */ );

// }
// //end
// ObjectAddress
// RenameRelation(RenameStmt *stmt)
// {
// 	Oid			relid;
// 	ObjectAddress address;

// 	/*
// 	 * Grab an exclusive lock on the target table, index, sequence, view,
// 	 * materialized view, or foreign table, which we will NOT release until
// 	 * end of transaction.
// 	 *
// 	 * Lock level used here should match RenameRelationInternal, to avoid lock
// 	 * escalation.
// 	 */
// 	relid = RangeVarGetRelidExtended(stmt->relation, AccessExclusiveLock,
// 									 stmt->missing_ok, false,
// 									 RangeVarCallbackForAlterRelation,
// 									 (void *) stmt);

// 	if (!OidIsValid(relid))
// 	{
// 		ereport(NOTICE,
// 				(errmsg("relation \"%s\" does not exist, skipping",
// 						stmt->relation->relname)));
// 		return InvalidObjectAddress;
// 	}

// 	/* Do the work */
// 	RenameRelationInternal(relid, stmt->newname, false);

// 	ObjectAddressSet(address, RelationRelationId, relid);

// 	return address;
// }
// //end

// void
// RenameRelationInternal(Oid myrelid, const char *newrelname, bool is_internal)
// {
// 	Relation	targetrelation;
// 	Relation	relrelation;	/* for RELATION relation */
// 	HeapTuple	reltup;
// 	Form_pg_class relform;
// 	Oid			namespaceId;

// 	/*
// 	 * Grab an exclusive lock on the target table, index, sequence, view,
// 	 * materialized view, or foreign table, which we will NOT release until
// 	 * end of transaction.
// 	 */
// 	targetrelation = relation_open(myrelid, AccessExclusiveLock);
// 	namespaceId = RelationGetNamespace(targetrelation);

// 	/*
// 	 * Find relation's pg_class tuple, and make sure newrelname isn't in use.
// 	 */
// 	relrelation = heap_open(RelationRelationId, RowExclusiveLock);

// 	reltup = SearchSysCacheCopy1(RELOID, ObjectIdGetDatum(myrelid));
// 	if (!HeapTupleIsValid(reltup))		/* shouldn't happen */
// 		elog(ERROR, "cache lookup failed for relation %u", myrelid);
// 	relform = (Form_pg_class) GETSTRUCT(reltup);

// 	if (get_relname_relid(newrelname, namespaceId) != InvalidOid)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_TABLE),
// 				 errmsg("relation \"%s\" already exists",
// 						newrelname)));

// 	/*
// 	 * Update pg_class tuple with new relname.  (Scribbling on reltup is OK
// 	 * because it's a copy...)
// 	 */
// 	namestrcpy(&(relform->relname), newrelname);

// 	simple_heap_update(relrelation, &reltup->t_self, reltup);

// 	/* keep the system catalog indexes current */
// 	CatalogUpdateIndexes(relrelation, reltup);

// 	InvokeObjectPostAlterHookArg(RelationRelationId, myrelid, 0,
// 								 InvalidOid, is_internal);

// 	heap_freetuple(reltup);
// 	heap_close(relrelation, RowExclusiveLock);

// 	/*
// 	 * Also rename the associated type, if any.
// 	 */
// 	if (OidIsValid(targetrelation->rd_rel->reltype))
// 		RenameTypeInternal(targetrelation->rd_rel->reltype,
// 						   newrelname, namespaceId);

// 	/*
// 	 * Also rename the associated constraint, if any.
// 	 */
// 	if (targetrelation->rd_rel->relkind == RELKIND_INDEX)
// 	{
// 		Oid			constraintId = get_index_constraint(myrelid);

// 		if (OidIsValid(constraintId))
// 			RenameConstraintById(constraintId, newrelname);
// 	}

// 	/*
// 	 * Close rel, but keep exclusive lock!
// 	 */
// 	relation_close(targetrelation, NoLock);
// }
// //end

// void
// RangeVarCallbackOwnsTable(const RangeVar *relation,
// 						  Oid relId, Oid oldRelId, void *arg)
// {
// 	char		relkind;

// 	/* Nothing to do if the relation was not found. */
// 	if (!OidIsValid(relId))
// 		return;

// 	/*
// 	 * If the relation does exist, check whether it's an index.  But note that
// 	 * the relation might have been dropped between the time we did the name
// 	 * lookup and now.  In that case, there's nothing to do.
// 	 */
// 	relkind = get_rel_relkind(relId);
// 	if (!relkind)
// 		return;
// 	if (relkind != RELKIND_RELATION && relkind != RELKIND_TOASTVALUE &&
// 		relkind != RELKIND_MATVIEW)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("\"%s\" is not a table or materialized view", relation->relname)));

// 	/* Check permissions */
// 	if (!pg_class_ownercheck(relId, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_CLASS, relation->relname);
// }
// //end
// void
// RangeVarCallbackOwnsRelation(const RangeVar *relation,
// 							 Oid relId, Oid oldRelId, void *arg)
// {
// 	HeapTuple	tuple;

// 	/* Nothing to do if the relation was not found. */
// 	if (!OidIsValid(relId))
// 		return;

// 	tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(relId));
// 	if (!HeapTupleIsValid(tuple))		/* should not happen */
// 		elog(ERROR, "cache lookup failed for relation %u", relId);

// 	if (!pg_class_ownercheck(relId, GetUserId()))
// 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_CLASS,
// 					   relation->relname);

// 	if (!allowSystemTableMods &&
// 		IsSystemClass(relId, (Form_pg_class) GETSTRUCT(tuple)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("permission denied: \"%s\" is a system catalog",
// 						relation->relname)));

// 	ReleaseSysCache(tuple);
// }
// //end
// Oid
// AlterTableMoveAll(AlterTableMoveAllStmt *stmt)
// {
// 	List	   *relations = NIL;
// 	ListCell   *l;
// 	ScanKeyData key[1];
// 	Relation	rel;
// 	HeapScanDesc scan;
// 	HeapTuple	tuple;
// 	Oid			orig_tablespaceoid;
// 	Oid			new_tablespaceoid;
// 	List	   *role_oids = roleSpecsToIds(stmt->roles);

// 	/* Ensure we were not asked to move something we can't */
// 	if (stmt->objtype != OBJECT_TABLE && stmt->objtype != OBJECT_INDEX &&
// 		stmt->objtype != OBJECT_MATVIEW)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("only tables, indexes, and materialized views exist in tablespaces")));

// 	/* Get the orig and new tablespace OIDs */
// 	orig_tablespaceoid = get_tablespace_oid(stmt->orig_tablespacename, false);
// 	new_tablespaceoid = get_tablespace_oid(stmt->new_tablespacename, false);

// 	/* Can't move shared relations in to or out of pg_global */
// 	/* This is also checked by ATExecSetTableSpace, but nice to stop earlier */
// 	if (orig_tablespaceoid == GLOBALTABLESPACE_OID ||
// 		new_tablespaceoid == GLOBALTABLESPACE_OID)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 				 errmsg("cannot move relations in to or out of pg_global tablespace")));

// 	/*
// 	 * Must have CREATE rights on the new tablespace, unless it is the
// 	 * database default tablespace (which all users implicitly have CREATE
// 	 * rights on).
// 	 */
// 	if (OidIsValid(new_tablespaceoid) && new_tablespaceoid != MyDatabaseTableSpace)
// 	{
// 		AclResult	aclresult;

// 		aclresult = pg_tablespace_aclcheck(new_tablespaceoid, GetUserId(),
// 										   ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_TABLESPACE,
// 						   get_tablespace_name(new_tablespaceoid));
// 	}

// 	/*
// 	 * Now that the checks are done, check if we should set either to
// 	 * InvalidOid because it is our database's default tablespace.
// 	 */
// 	if (orig_tablespaceoid == MyDatabaseTableSpace)
// 		orig_tablespaceoid = InvalidOid;

// 	if (new_tablespaceoid == MyDatabaseTableSpace)
// 		new_tablespaceoid = InvalidOid;

// 	/* no-op */
// 	if (orig_tablespaceoid == new_tablespaceoid)
// 		return new_tablespaceoid;

// 	/*
// 	 * Walk the list of objects in the tablespace and move them. This will
// 	 * only find objects in our database, of course.
// 	 */
// 	ScanKeyInit(&key[0],
// 				Anum_pg_class_reltablespace,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(orig_tablespaceoid));

// 	rel = heap_open(RelationRelationId, AccessShareLock);
// 	scan = heap_beginscan_catalog(rel, 1, key);
// 	while ((tuple = heap_getnext(scan, ForwardScanDirection)) != NULL)
// 	{
// 		Oid			relOid = HeapTupleGetOid(tuple);
// 		Form_pg_class relForm;

// 		relForm = (Form_pg_class) GETSTRUCT(tuple);

// 		/*
// 		 * Do not move objects in pg_catalog as part of this, if an admin
// 		 * really wishes to do so, they can issue the individual ALTER
// 		 * commands directly.
// 		 *
// 		 * Also, explicitly avoid any shared tables, temp tables, or TOAST
// 		 * (TOAST will be moved with the main table).
// 		 */
// 		if (IsSystemNamespace(relForm->relnamespace) || relForm->relisshared ||
// 			isAnyTempNamespace(relForm->relnamespace) ||
// 			relForm->relnamespace == PG_TOAST_NAMESPACE)
// 			continue;

// 		/* Only move the object type requested */
// 		if ((stmt->objtype == OBJECT_TABLE &&
// 			 relForm->relkind != RELKIND_RELATION) ||
// 			(stmt->objtype == OBJECT_INDEX &&
// 			 relForm->relkind != RELKIND_INDEX) ||
// 			(stmt->objtype == OBJECT_MATVIEW &&
// 			 relForm->relkind != RELKIND_MATVIEW))
// 			continue;

// 		/* Check if we are only moving objects owned by certain roles */
// 		if (role_oids != NIL && !list_member_oid(role_oids, relForm->relowner))
// 			continue;

// 		/*
// 		 * Handle permissions-checking here since we are locking the tables
// 		 * and also to avoid doing a bunch of work only to fail part-way. Note
// 		 * that permissions will also be checked by AlterTableInternal().
// 		 *
// 		 * Caller must be considered an owner on the table to move it.
// 		 */
// 		if (!pg_class_ownercheck(relOid, GetUserId()))
// 			aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_CLASS,
// 						   NameStr(relForm->relname));

// 		if (stmt->nowait &&
// 			!ConditionalLockRelationOid(relOid, AccessExclusiveLock))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_OBJECT_IN_USE),
// 					 errmsg("aborting because lock on relation \"%s.%s\" is not available",
// 							get_namespace_name(relForm->relnamespace),
// 							NameStr(relForm->relname))));
// 		else
// 			LockRelationOid(relOid, AccessExclusiveLock);

// 		/* Add to our list of objects to move */
// 		relations = lappend_oid(relations, relOid);
// 	}

// 	heap_endscan(scan);
// 	heap_close(rel, AccessShareLock);

// 	if (relations == NIL)
// 		ereport(NOTICE,
// 				(errcode(ERRCODE_NO_DATA_FOUND),
// 				 errmsg("no matching relations in tablespace \"%s\" found",
// 					orig_tablespaceoid == InvalidOid ? "(database default)" :
// 						get_tablespace_name(orig_tablespaceoid))));

// 	/* Everything is locked, loop through and move all of the relations. */
// 	foreach(l, relations)
// 	{
// 		List	   *cmds = NIL;
// 		AlterTableCmd *cmd = makeNode(AlterTableCmd);

// 		cmd->subtype = AT_SetTableSpace;
// 		cmd->name = stmt->new_tablespacename;

// 		cmds = lappend(cmds, cmd);

// 		EventTriggerAlterTableStart((Node *) stmt);
// 		/* OID is set by AlterTableInternal */
// 		AlterTableInternal(lfirst_oid(l), cmds, false);
// 		EventTriggerAlterTableEnd();
// 	}

// 	return new_tablespaceoid;
// }
// //end

// //commands/trigger.c
// ObjectAddress
// CreateTrigger(CreateTrigStmt *stmt, const char *queryString,
// 			  Oid relOid, Oid refRelOid, Oid constraintOid, Oid indexOid,
// 			  bool isInternal)
// {
// 	int16		tgtype;
// 	int			ncolumns;
// 	int16	   *columns;
// 	int2vector *tgattr;
// 	Node	   *whenClause;
// 	List	   *whenRtable;
// 	char	   *qual;
// 	Datum		values[Natts_pg_trigger];
// 	bool		nulls[Natts_pg_trigger];
// 	Relation	rel;
// 	AclResult	aclresult;
// 	Relation	tgrel;
// 	SysScanDesc tgscan;
// 	ScanKeyData key;
// 	Relation	pgrel;
// 	HeapTuple	tuple;
// 	Oid			fargtypes[1];	/* dummy */
// 	Oid			funcoid;
// 	Oid			funcrettype;
// 	Oid			trigoid;
// 	char		internaltrigname[NAMEDATALEN];
// 	char	   *trigname;
// 	Oid			constrrelid = InvalidOid;
// 	ObjectAddress myself,
// 				referenced;

// 	if (OidIsValid(relOid))
// 		rel = heap_open(relOid, ShareRowExclusiveLock);
// 	else
// 		rel = heap_openrv(stmt->relation, ShareRowExclusiveLock);

// 	/*
// 	 * Triggers must be on tables or views, and there are additional
// 	 * relation-type-specific restrictions.
// 	 */
// 	if (rel->rd_rel->relkind == RELKIND_RELATION)
// 	{
// 		/* Tables can't have INSTEAD OF triggers */
// 		if (stmt->timing != TRIGGER_TYPE_BEFORE &&
// 			stmt->timing != TRIGGER_TYPE_AFTER)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a table",
// 							RelationGetRelationName(rel)),
// 					 errdetail("Tables cannot have INSTEAD OF triggers.")));
// 	}
// 	else if (rel->rd_rel->relkind == RELKIND_VIEW)
// 	{
// 		/*
// 		 * Views can have INSTEAD OF triggers (which we check below are
// 		 * row-level), or statement-level BEFORE/AFTER triggers.
// 		 */
// 		if (stmt->timing != TRIGGER_TYPE_INSTEAD && stmt->row)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a view",
// 							RelationGetRelationName(rel)),
// 					 errdetail("Views cannot have row-level BEFORE or AFTER triggers.")));
// 		/* Disallow TRUNCATE triggers on VIEWs */
// 		if (TRIGGER_FOR_TRUNCATE(stmt->events))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a view",
// 							RelationGetRelationName(rel)),
// 					 errdetail("Views cannot have TRUNCATE triggers.")));
// 	}
// 	else if (rel->rd_rel->relkind == RELKIND_FOREIGN_TABLE)
// 	{
// 		if (stmt->timing != TRIGGER_TYPE_BEFORE &&
// 			stmt->timing != TRIGGER_TYPE_AFTER)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a foreign table",
// 							RelationGetRelationName(rel)),
// 			  errdetail("Foreign tables cannot have INSTEAD OF triggers.")));

// 		if (TRIGGER_FOR_TRUNCATE(stmt->events))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a foreign table",
// 							RelationGetRelationName(rel)),
// 				errdetail("Foreign tables cannot have TRUNCATE triggers.")));

// 		if (stmt->isconstraint)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a foreign table",
// 							RelationGetRelationName(rel)),
// 			  errdetail("Foreign tables cannot have constraint triggers.")));
// 	}
// 	else
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("\"%s\" is not a table or view",
// 						RelationGetRelationName(rel))));

// 	if (!allowSystemTableMods && IsSystemRelation(rel))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("permission denied: \"%s\" is a system catalog",
// 						RelationGetRelationName(rel))));

// 	if (stmt->isconstraint)
// 	{
// 		/*
// 		 * We must take a lock on the target relation to protect against
// 		 * concurrent drop.  It's not clear that AccessShareLock is strong
// 		 * enough, but we certainly need at least that much... otherwise, we
// 		 * might end up creating a pg_constraint entry referencing a
// 		 * nonexistent table.
// 		 */
// 		if (OidIsValid(refRelOid))
// 		{
// 			LockRelationOid(refRelOid, AccessShareLock);
// 			constrrelid = refRelOid;
// 		}
// 		else if (stmt->constrrel != NULL)
// 			constrrelid = RangeVarGetRelid(stmt->constrrel, AccessShareLock,
// 										   false);
// 	}

// 	/* permission checks */
// 	if (!isInternal)
// 	{
// 		aclresult = pg_class_aclcheck(RelationGetRelid(rel), GetUserId(),
// 									  ACL_TRIGGER);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_CLASS,
// 						   RelationGetRelationName(rel));

// 		if (OidIsValid(constrrelid))
// 		{
// 			aclresult = pg_class_aclcheck(constrrelid, GetUserId(),
// 										  ACL_TRIGGER);
// 			if (aclresult != ACLCHECK_OK)
// 				aclcheck_error(aclresult, ACL_KIND_CLASS,
// 							   get_rel_name(constrrelid));
// 		}
// 	}

// 	/* Compute tgtype */
// 	TRIGGER_CLEAR_TYPE(tgtype);
// 	if (stmt->row)
// 		TRIGGER_SETT_ROW(tgtype);
// 	tgtype |= stmt->timing;
// 	tgtype |= stmt->events;

// 	/* Disallow ROW-level TRUNCATE triggers */
// 	if (TRIGGER_FOR_ROW(tgtype) && TRIGGER_FOR_TRUNCATE(tgtype))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("TRUNCATE FOR EACH ROW triggers are not supported")));

// 	/* INSTEAD triggers must be row-level, and can't have WHEN or columns */
// 	if (TRIGGER_FOR_INSTEAD(tgtype))
// 	{
// 		if (!TRIGGER_FOR_ROW(tgtype))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("INSTEAD OF triggers must be FOR EACH ROW")));
// 		if (stmt->whenClause)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("INSTEAD OF triggers cannot have WHEN conditions")));
// 		if (stmt->columns != NIL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("INSTEAD OF triggers cannot have column lists")));
// 	}

// 	/*
// 	 * Parse the WHEN clause, if any
// 	 */
// 	if (stmt->whenClause)
// 	{
// 		ParseState *pstate;
// 		RangeTblEntry *rte;
// 		List	   *varList;
// 		ListCell   *lc;

// 		/* Set up a pstate to parse with */
// 		pstate = make_parsestate(NULL);
// 		pstate->p_sourcetext = queryString;

// 		/*
// 		 * Set up RTEs for OLD and NEW references.
// 		 *
// 		 * 'OLD' must always have varno equal to 1 and 'NEW' equal to 2.
// 		 */
// 		rte = addRangeTableEntryForRelation(pstate, rel,
// 											makeAlias("old", NIL),
// 											false, false);
// 		addRTEtoQuery(pstate, rte, false, true, true);
// 		rte = addRangeTableEntryForRelation(pstate, rel,
// 											makeAlias("new", NIL),
// 											false, false);
// 		addRTEtoQuery(pstate, rte, false, true, true);

// 		/* Transform expression.  Copy to be sure we don't modify original */
// 		whenClause = transformWhereClause(pstate,
// 										  copyObject(stmt->whenClause),
// 										  EXPR_KIND_TRIGGER_WHEN,
// 										  "WHEN");
// 		/* we have to fix its collations too */
// 		assign_expr_collations(pstate, whenClause);

// 		/*
// 		 * Check for disallowed references to OLD/NEW.
// 		 *
// 		 * NB: pull_var_clause is okay here only because we don't allow
// 		 * subselects in WHEN clauses; it would fail to examine the contents
// 		 * of subselects.
// 		 */
// 		varList = pull_var_clause(whenClause, 0);
// 		foreach(lc, varList)
// 		{
// 			Var		   *var = (Var *) lfirst(lc);

// 			switch (var->varno)
// 			{
// 				case PRS2_OLD_VARNO:
// 					if (!TRIGGER_FOR_ROW(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg("statement trigger's WHEN condition cannot reference column values"),
// 								 parser_errposition(pstate, var->location)));
// 					if (TRIGGER_FOR_INSERT(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg("INSERT trigger's WHEN condition cannot reference OLD values"),
// 								 parser_errposition(pstate, var->location)));
// 					/* system columns are okay here */
// 					break;
// 				case PRS2_NEW_VARNO:
// 					if (!TRIGGER_FOR_ROW(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg("statement trigger's WHEN condition cannot reference column values"),
// 								 parser_errposition(pstate, var->location)));
// 					if (TRIGGER_FOR_DELETE(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg("DELETE trigger's WHEN condition cannot reference NEW values"),
// 								 parser_errposition(pstate, var->location)));
// 					if (var->varattno < 0 && TRIGGER_FOR_BEFORE(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 								 errmsg("BEFORE trigger's WHEN condition cannot reference NEW system columns"),
// 								 parser_errposition(pstate, var->location)));
// 					break;
// 				default:
// 					/* can't happen without add_missing_from, so just elog */
// 					elog(ERROR, "trigger WHEN condition cannot contain references to other relations");
// 					break;
// 			}
// 		}

// 		/* we'll need the rtable for recordDependencyOnExpr */
// 		whenRtable = pstate->p_rtable;

// 		qual = nodeToString(whenClause);

// 		free_parsestate(pstate);
// 	}
// 	else
// 	{
// 		whenClause = NULL;
// 		whenRtable = NIL;
// 		qual = NULL;
// 	}

// 	/*
// 	 * Find and validate the trigger function.
// 	 */
// 	funcoid = LookupFuncName(stmt->funcname, 0, fargtypes, false);
// 	if (!isInternal)
// 	{
// 		aclresult = pg_proc_aclcheck(funcoid, GetUserId(), ACL_EXECUTE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_PROC,
// 						   NameListToString(stmt->funcname));
// 	}
// 	funcrettype = get_func_rettype(funcoid);
// 	if (funcrettype != TRIGGEROID)
// 	{
// 		/*
// 		 * We allow OPAQUE just so we can load old dump files.  When we see a
// 		 * trigger function declared OPAQUE, change it to TRIGGER.
// 		 */
// 		if (funcrettype == OPAQUEOID)
// 		{
// 			ereport(WARNING,
// 					(errmsg("changing return type of function %s from \"opaque\" to \"trigger\"",
// 							NameListToString(stmt->funcname))));
// 			SetFunctionReturnType(funcoid, TRIGGEROID);
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 					 errmsg("function %s must return type %s",
// 							NameListToString(stmt->funcname), "trigger")));
// 	}

// 	/*
// 	 * If the command is a user-entered CREATE CONSTRAINT TRIGGER command that
// 	 * references one of the built-in RI_FKey trigger functions, assume it is
// 	 * from a dump of a pre-7.3 foreign key constraint, and take steps to
// 	 * convert this legacy representation into a regular foreign key
// 	 * constraint.  Ugly, but necessary for loading old dump files.
// 	 */
// 	if (stmt->isconstraint && !isInternal &&
// 		list_length(stmt->args) >= 6 &&
// 		(list_length(stmt->args) % 2) == 0 &&
// 		RI_FKey_trigger_type(funcoid) != RI_TRIGGER_NONE)
// 	{
// 		/* Keep lock on target rel until end of xact */
// 		heap_close(rel, NoLock);

// 		ConvertTriggerToFK(stmt, funcoid);

// 		return InvalidObjectAddress;
// 	}

// 	/*
// 	 * If it's a user-entered CREATE CONSTRAINT TRIGGER command, make a
// 	 * corresponding pg_constraint entry.
// 	 */
// 	if (stmt->isconstraint && !OidIsValid(constraintOid))
// 	{
// 		/* Internal callers should have made their own constraints */
// 		Assert(!isInternal);
// 		constraintOid = CreateConstraintEntry(stmt->trigname,
// 											  RelationGetNamespace(rel),
// 											  CONSTRAINT_TRIGGER,
// 											  stmt->deferrable,
// 											  stmt->initdeferred,
// 											  true,
// 											  RelationGetRelid(rel),
// 											  NULL,		/* no conkey */
// 											  0,
// 											  InvalidOid,		/* no domain */
// 											  InvalidOid,		/* no index */
// 											  InvalidOid,		/* no foreign key */
// 											  NULL,
// 											  NULL,
// 											  NULL,
// 											  NULL,
// 											  0,
// 											  ' ',
// 											  ' ',
// 											  ' ',
// 											  NULL,		/* no exclusion */
// 											  NULL,		/* no check constraint */
// 											  NULL,
// 											  NULL,
// 											  true,		/* islocal */
// 											  0,		/* inhcount */
// 											  true,		/* isnoinherit */
// 											  isInternal);		/* is_internal */
// 	}

// 	/*
// 	 * Generate the trigger's OID now, so that we can use it in the name if
// 	 * needed.
// 	 */
// 	tgrel = heap_open(TriggerRelationId, RowExclusiveLock);

// 	trigoid = GetNewOid(tgrel);

// 	/*
// 	 * If trigger is internally generated, modify the provided trigger name to
// 	 * ensure uniqueness by appending the trigger OID.  (Callers will usually
// 	 * supply a simple constant trigger name in these cases.)
// 	 */
// 	if (isInternal)
// 	{
// 		snprintf(internaltrigname, sizeof(internaltrigname),
// 				 "%s_%u", stmt->trigname, trigoid);
// 		trigname = internaltrigname;
// 	}
// 	else
// 	{
// 		/* user-defined trigger; use the specified trigger name as-is */
// 		trigname = stmt->trigname;
// 	}

// 	/*
// 	 * Scan pg_trigger for existing triggers on relation.  We do this only to
// 	 * give a nice error message if there's already a trigger of the same
// 	 * name.  (The unique index on tgrelid/tgname would complain anyway.) We
// 	 * can skip this for internally generated triggers, since the name
// 	 * modification above should be sufficient.
// 	 *
// 	 * NOTE that this is cool only because we have ShareRowExclusiveLock on
// 	 * the relation, so the trigger set won't be changing underneath us.
// 	 */
// 	if (!isInternal)
// 	{
// 		ScanKeyInit(&key,
// 					Anum_pg_trigger_tgrelid,
// 					BTEqualStrategyNumber, F_OIDEQ,
// 					ObjectIdGetDatum(RelationGetRelid(rel)));
// 		tgscan = systable_beginscan(tgrel, TriggerRelidNameIndexId, true,
// 									NULL, 1, &key);
// 		while (HeapTupleIsValid(tuple = systable_getnext(tgscan)))
// 		{
// 			Form_pg_trigger pg_trigger = (Form_pg_trigger) GETSTRUCT(tuple);

// 			if (namestrcmp(&(pg_trigger->tgname), trigname) == 0)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				  errmsg("trigger \"%s\" for relation \"%s\" already exists",
// 						 trigname, RelationGetRelationName(rel))));
// 		}
// 		systable_endscan(tgscan);
// 	}

// 	/*
// 	 * Build the new pg_trigger tuple.
// 	 */
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_trigger_tgrelid - 1] = ObjectIdGetDatum(RelationGetRelid(rel));
// 	values[Anum_pg_trigger_tgname - 1] = DirectFunctionCall1(namein,
// 												  CStringGetDatum(trigname));
// 	values[Anum_pg_trigger_tgfoid - 1] = ObjectIdGetDatum(funcoid);
// 	values[Anum_pg_trigger_tgtype - 1] = Int16GetDatum(tgtype);
// 	values[Anum_pg_trigger_tgenabled - 1] = CharGetDatum(TRIGGER_FIRES_ON_ORIGIN);
// 	values[Anum_pg_trigger_tgisinternal - 1] = BoolGetDatum(isInternal);
// 	values[Anum_pg_trigger_tgconstrrelid - 1] = ObjectIdGetDatum(constrrelid);
// 	values[Anum_pg_trigger_tgconstrindid - 1] = ObjectIdGetDatum(indexOid);
// 	values[Anum_pg_trigger_tgconstraint - 1] = ObjectIdGetDatum(constraintOid);
// 	values[Anum_pg_trigger_tgdeferrable - 1] = BoolGetDatum(stmt->deferrable);
// 	values[Anum_pg_trigger_tginitdeferred - 1] = BoolGetDatum(stmt->initdeferred);

// 	if (stmt->args)
// 	{
// 		ListCell   *le;
// 		char	   *args;
// 		int16		nargs = list_length(stmt->args);
// 		int			len = 0;

// 		foreach(le, stmt->args)
// 		{
// 			char	   *ar = strVal(lfirst(le));

// 			len += strlen(ar) + 4;
// 			for (; *ar; ar++)
// 			{
// 				if (*ar == '\\')
// 					len++;
// 			}
// 		}
// 		args = (char *) palloc(len + 1);
// 		args[0] = '\0';
// 		foreach(le, stmt->args)
// 		{
// 			char	   *s = strVal(lfirst(le));
// 			char	   *d = args + strlen(args);

// 			while (*s)
// 			{
// 				if (*s == '\\')
// 					*d++ = '\\';
// 				*d++ = *s++;
// 			}
// 			strcpy(d, "\\000");
// 		}
// 		values[Anum_pg_trigger_tgnargs - 1] = Int16GetDatum(nargs);
// 		values[Anum_pg_trigger_tgargs - 1] = DirectFunctionCall1(byteain,
// 													  CStringGetDatum(args));
// 	}
// 	else
// 	{
// 		values[Anum_pg_trigger_tgnargs - 1] = Int16GetDatum(0);
// 		values[Anum_pg_trigger_tgargs - 1] = DirectFunctionCall1(byteain,
// 														CStringGetDatum(""));
// 	}

// 	/* build column number array if it's a column-specific trigger */
// 	ncolumns = list_length(stmt->columns);
// 	if (ncolumns == 0)
// 		columns = NULL;
// 	else
// 	{
// 		ListCell   *cell;
// 		int			i = 0;

// 		columns = (int16 *) palloc(ncolumns * sizeof(int16));
// 		foreach(cell, stmt->columns)
// 		{
// 			char	   *name = strVal(lfirst(cell));
// 			int16		attnum;
// 			int			j;

// 			/* Lookup column name.  System columns are not allowed */
// 			attnum = attnameAttNum(rel, name, false);
// 			if (attnum == InvalidAttrNumber)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_UNDEFINED_COLUMN),
// 					errmsg("column \"%s\" of relation \"%s\" does not exist",
// 						   name, RelationGetRelationName(rel))));

// 			/* Check for duplicates */
// 			for (j = i - 1; j >= 0; j--)
// 			{
// 				if (columns[j] == attnum)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_DUPLICATE_COLUMN),
// 							 errmsg("column \"%s\" specified more than once",
// 									name)));
// 			}

// 			columns[i++] = attnum;
// 		}
// 	}
// 	tgattr = buildint2vector(columns, ncolumns);
// 	values[Anum_pg_trigger_tgattr - 1] = PointerGetDatum(tgattr);

// 	/* set tgqual if trigger has WHEN clause */
// 	if (qual)
// 		values[Anum_pg_trigger_tgqual - 1] = CStringGetTextDatum(qual);
// 	else
// 		nulls[Anum_pg_trigger_tgqual - 1] = true;

// 	tuple = heap_form_tuple(tgrel->rd_att, values, nulls);

// 	/* force tuple to have the desired OID */
// 	HeapTupleSetOid(tuple, trigoid);

// 	/*
// 	 * Insert tuple into pg_trigger.
// 	 */
// 	simple_heap_insert(tgrel, tuple);

// 	CatalogUpdateIndexes(tgrel, tuple);

// 	heap_freetuple(tuple);
// 	heap_close(tgrel, RowExclusiveLock);

// 	pfree(DatumGetPointer(values[Anum_pg_trigger_tgname - 1]));
// 	pfree(DatumGetPointer(values[Anum_pg_trigger_tgargs - 1]));
// 	pfree(DatumGetPointer(values[Anum_pg_trigger_tgattr - 1]));

// 	/*
// 	 * Update relation's pg_class entry.  Crucial side-effect: other backends
// 	 * (and this one too!) are sent SI message to make them rebuild relcache
// 	 * entries.
// 	 */
// 	pgrel = heap_open(RelationRelationId, RowExclusiveLock);
// 	tuple = SearchSysCacheCopy1(RELOID,
// 								ObjectIdGetDatum(RelationGetRelid(rel)));
// 	if (!HeapTupleIsValid(tuple))
// 		elog(ERROR, "cache lookup failed for relation %u",
// 			 RelationGetRelid(rel));

// 	((Form_pg_class) GETSTRUCT(tuple))->relhastriggers = true;

// 	simple_heap_update(pgrel, &tuple->t_self, tuple);

// 	CatalogUpdateIndexes(pgrel, tuple);

// 	heap_freetuple(tuple);
// 	heap_close(pgrel, RowExclusiveLock);

// 	/*
// 	 * We used to try to update the rel's relcache entry here, but that's
// 	 * fairly pointless since it will happen as a byproduct of the upcoming
// 	 * CommandCounterIncrement...
// 	 */

// 	/*
// 	 * Record dependencies for trigger.  Always place a normal dependency on
// 	 * the function.
// 	 */
// 	myself.classId = TriggerRelationId;
// 	myself.objectId = trigoid;
// 	myself.objectSubId = 0;

// 	referenced.classId = ProcedureRelationId;
// 	referenced.objectId = funcoid;
// 	referenced.objectSubId = 0;
// 	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

// 	if (isInternal && OidIsValid(constraintOid))
// 	{
// 		/*
// 		 * Internally-generated trigger for a constraint, so make it an
// 		 * internal dependency of the constraint.  We can skip depending on
// 		 * the relation(s), as there'll be an indirect dependency via the
// 		 * constraint.
// 		 */
// 		referenced.classId = ConstraintRelationId;
// 		referenced.objectId = constraintOid;
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_INTERNAL);
// 	}
// 	else
// 	{
// 		/*
// 		 * User CREATE TRIGGER, so place dependencies.  We make trigger be
// 		 * auto-dropped if its relation is dropped or if the FK relation is
// 		 * dropped.  (Auto drop is compatible with our pre-7.3 behavior.)
// 		 */
// 		referenced.classId = RelationRelationId;
// 		referenced.objectId = RelationGetRelid(rel);
// 		referenced.objectSubId = 0;
// 		recordDependencyOn(&myself, &referenced, DEPENDENCY_AUTO);
// 		if (OidIsValid(constrrelid))
// 		{
// 			referenced.classId = RelationRelationId;
// 			referenced.objectId = constrrelid;
// 			referenced.objectSubId = 0;
// 			recordDependencyOn(&myself, &referenced, DEPENDENCY_AUTO);
// 		}
// 		/* Not possible to have an index dependency in this case */
// 		Assert(!OidIsValid(indexOid));

// 		/*
// 		 * If it's a user-specified constraint trigger, make the constraint
// 		 * internally dependent on the trigger instead of vice versa.
// 		 */
// 		if (OidIsValid(constraintOid))
// 		{
// 			referenced.classId = ConstraintRelationId;
// 			referenced.objectId = constraintOid;
// 			referenced.objectSubId = 0;
// 			recordDependencyOn(&referenced, &myself, DEPENDENCY_INTERNAL);
// 		}
// 	}

// 	/* If column-specific trigger, add normal dependencies on columns */
// 	if (columns != NULL)
// 	{
// 		int			i;

// 		referenced.classId = RelationRelationId;
// 		referenced.objectId = RelationGetRelid(rel);
// 		for (i = 0; i < ncolumns; i++)
// 		{
// 			referenced.objectSubId = columns[i];
// 			recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 		}
// 	}

// 	/*
// 	 * If it has a WHEN clause, add dependencies on objects mentioned in the
// 	 * expression (eg, functions, as well as any columns used).
// 	 */
// 	if (whenClause != NULL)
// 		recordDependencyOnExpr(&myself, whenClause, whenRtable,
// 							   DEPENDENCY_NORMAL);

// 	/* Post creation hook for new trigger */
// 	InvokeObjectPostCreateHookArg(TriggerRelationId, trigoid, 0,
// 								  isInternal);

// 	/* Keep lock on target rel until end of xact */
// 	heap_close(rel, NoLock);

// 	return myself;
// }
// //end
// ObjectAddress
// renametrig(RenameStmt *stmt)
// {
// 	Oid			tgoid;
// 	Relation	targetrel;
// 	Relation	tgrel;
// 	HeapTuple	tuple;
// 	SysScanDesc tgscan;
// 	ScanKeyData key[2];
// 	Oid			relid;
// 	ObjectAddress address;

// 	/*
// 	 * Look up name, check permissions, and acquire lock (which we will NOT
// 	 * release until end of transaction).
// 	 */
// 	relid = RangeVarGetRelidExtended(stmt->relation, AccessExclusiveLock,
// 									 false, false,
// 									 RangeVarCallbackForRenameTrigger,
// 									 NULL);

// 	/* Have lock already, so just need to build relcache entry. */
// 	targetrel = relation_open(relid, NoLock);

// 	/*
// 	 * Scan pg_trigger twice for existing triggers on relation.  We do this in
// 	 * order to ensure a trigger does not exist with newname (The unique index
// 	 * on tgrelid/tgname would complain anyway) and to ensure a trigger does
// 	 * exist with oldname.
// 	 *
// 	 * NOTE that this is cool only because we have AccessExclusiveLock on the
// 	 * relation, so the trigger set won't be changing underneath us.
// 	 */
// 	tgrel = heap_open(TriggerRelationId, RowExclusiveLock);

// 	/*
// 	 * First pass -- look for name conflict
// 	 */
// 	ScanKeyInit(&key[0],
// 				Anum_pg_trigger_tgrelid,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(relid));
// 	ScanKeyInit(&key[1],
// 				Anum_pg_trigger_tgname,
// 				BTEqualStrategyNumber, F_NAMEEQ,
// 				PointerGetDatum(stmt->newname));
// 	tgscan = systable_beginscan(tgrel, TriggerRelidNameIndexId, true,
// 								NULL, 2, key);
// 	if (HeapTupleIsValid(tuple = systable_getnext(tgscan)))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("trigger \"%s\" for relation \"%s\" already exists",
// 						stmt->newname, RelationGetRelationName(targetrel))));
// 	systable_endscan(tgscan);

// 	/*
// 	 * Second pass -- look for trigger existing with oldname and update
// 	 */
// 	ScanKeyInit(&key[0],
// 				Anum_pg_trigger_tgrelid,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(relid));
// 	ScanKeyInit(&key[1],
// 				Anum_pg_trigger_tgname,
// 				BTEqualStrategyNumber, F_NAMEEQ,
// 				PointerGetDatum(stmt->subname));
// 	tgscan = systable_beginscan(tgrel, TriggerRelidNameIndexId, true,
// 								NULL, 2, key);
// 	if (HeapTupleIsValid(tuple = systable_getnext(tgscan)))
// 	{
// 		tgoid = HeapTupleGetOid(tuple);

// 		/*
// 		 * Update pg_trigger tuple with new tgname.
// 		 */
// 		tuple = heap_copytuple(tuple);	/* need a modifiable copy */

// 		namestrcpy(&((Form_pg_trigger) GETSTRUCT(tuple))->tgname,
// 				   stmt->newname);

// 		simple_heap_update(tgrel, &tuple->t_self, tuple);

// 		/* keep system catalog indexes current */
// 		CatalogUpdateIndexes(tgrel, tuple);

// 		InvokeObjectPostAlterHook(TriggerRelationId,
// 								  HeapTupleGetOid(tuple), 0);

// 		/*
// 		 * Invalidate relation's relcache entry so that other backends (and
// 		 * this one too!) are sent SI message to make them rebuild relcache
// 		 * entries.  (Ideally this should happen automatically...)
// 		 */
// 		CacheInvalidateRelcache(targetrel);
// 	}
// 	else
// 	{
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// 				 errmsg("trigger \"%s\" for table \"%s\" does not exist",
// 						stmt->subname, RelationGetRelationName(targetrel))));
// 	}

// 	ObjectAddressSet(address, TriggerRelationId, tgoid);

// 	systable_endscan(tgscan);

// 	heap_close(tgrel, RowExclusiveLock);

// 	/*
// 	 * Close rel, but keep exclusive lock!
// 	 */
// 	relation_close(targetrel, NoLock);

// 	return address;
// }
// //end
// void
// ExecARInsertTriggers(EState *estate, ResultRelInfo *relinfo,
// 					 HeapTuple trigtuple, List *recheckIndexes)
// {
// 	TriggerDesc *trigdesc = relinfo->ri_TrigDesc;

// 	if (trigdesc && trigdesc->trig_insert_after_row)
// 		AfterTriggerSaveEvent(estate, relinfo, TRIGGER_EVENT_INSERT,
// 							  true, NULL, trigtuple, recheckIndexes, NULL);
// }
// //end

// bool
// ExecBRDeleteTriggers(EState *estate, EPQState *epqstate,
// 					 ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple)
// {
// 	TriggerDesc *trigdesc = relinfo->ri_TrigDesc;
// 	bool		result = true;
// 	TriggerData LocTriggerData;
// 	HeapTuple	trigtuple;
// 	HeapTuple	newtuple;
// 	TupleTableSlot *newSlot;
// 	int			i;

// 	Assert(HeapTupleIsValid(fdw_trigtuple) ^ ItemPointerIsValid(tupleid));
// 	if (fdw_trigtuple == NULL)
// 	{
// 		trigtuple = GetTupleForTrigger(estate, epqstate, relinfo, tupleid,
// 									   LockTupleExclusive, &newSlot);
// 		if (trigtuple == NULL)
// 			return false;
// 	}
// 	else
// 		trigtuple = fdw_trigtuple;

// 	LocTriggerData.type = T_TriggerData;
// 	LocTriggerData.tg_event = TRIGGER_EVENT_DELETE |
// 		TRIGGER_EVENT_ROW |
// 		TRIGGER_EVENT_BEFORE;
// 	LocTriggerData.tg_relation = relinfo->ri_RelationDesc;
// 	LocTriggerData.tg_newtuple = NULL;
// 	LocTriggerData.tg_newtuplebuf = InvalidBuffer;
// 	for (i = 0; i < trigdesc->numtriggers; i++)
// 	{
// 		Trigger    *trigger = &trigdesc->triggers[i];

// 		if (!TRIGGER_TYPE_MATCHES(trigger->tgtype,
// 								  TRIGGER_TYPE_ROW,
// 								  TRIGGER_TYPE_BEFORE,
// 								  TRIGGER_TYPE_DELETE))
// 			continue;
// 		if (!TriggerEnabled(estate, relinfo, trigger, LocTriggerData.tg_event,
// 							NULL, trigtuple, NULL))
// 			continue;

// 		LocTriggerData.tg_trigtuple = trigtuple;
// 		LocTriggerData.tg_trigtuplebuf = InvalidBuffer;
// 		LocTriggerData.tg_trigger = trigger;
// 		newtuple = ExecCallTriggerFunc(&LocTriggerData,
// 									   i,
// 									   relinfo->ri_TrigFunctions,
// 									   relinfo->ri_TrigInstrument,
// 									   GetPerTupleMemoryContext(estate));
// 		if (newtuple == NULL)
// 		{
// 			result = false;		/* tell caller to suppress delete */
// 			break;
// 		}
// 		if (newtuple != trigtuple)
// 			heap_freetuple(newtuple);
// 	}
// 	if (trigtuple != fdw_trigtuple)
// 		heap_freetuple(trigtuple);

// 	return result;
// }
// //end

// void
// ExecARDeleteTriggers(EState *estate, ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple)
// {
// 	TriggerDesc *trigdesc = relinfo->ri_TrigDesc;

// 	if (trigdesc && trigdesc->trig_delete_after_row)
// 	{
// 		HeapTuple	trigtuple;

// 		Assert(HeapTupleIsValid(fdw_trigtuple) ^ ItemPointerIsValid(tupleid));
// 		if (fdw_trigtuple == NULL)
// 			trigtuple = GetTupleForTrigger(estate,
// 										   NULL,
// 										   relinfo,
// 										   tupleid,
// 										   LockTupleExclusive,
// 										   NULL);
// 		else
// 			trigtuple = fdw_trigtuple;

// 		AfterTriggerSaveEvent(estate, relinfo, TRIGGER_EVENT_DELETE,
// 							  true, trigtuple, NULL, NIL, NULL);
// 		if (trigtuple != fdw_trigtuple)
// 			heap_freetuple(trigtuple);
// 	}
// }
// //end

// TupleTableSlot *
// ExecBRUpdateTriggers(EState *estate, EPQState *epqstate,
// 					 ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple,
// 					 TupleTableSlot *slot)
// {
// 	TriggerDesc *trigdesc = relinfo->ri_TrigDesc;
// 	HeapTuple	slottuple = ExecMaterializeSlot(slot);
// 	HeapTuple	newtuple = slottuple;
// 	TriggerData LocTriggerData;
// 	HeapTuple	trigtuple;
// 	HeapTuple	oldtuple;
// 	TupleTableSlot *newSlot;
// 	int			i;
// 	Bitmapset  *updatedCols;
// 	LockTupleMode lockmode;

// 	/* Determine lock mode to use */
// 	lockmode = ExecUpdateLockMode(estate, relinfo);

// 	Assert(HeapTupleIsValid(fdw_trigtuple) ^ ItemPointerIsValid(tupleid));
// 	if (fdw_trigtuple == NULL)
// 	{
// 		/* get a copy of the on-disk tuple we are planning to update */
// 		trigtuple = GetTupleForTrigger(estate, epqstate, relinfo, tupleid,
// 									   lockmode, &newSlot);
// 		if (trigtuple == NULL)
// 			return NULL;		/* cancel the update action */
// 	}
// 	else
// 	{
// 		trigtuple = fdw_trigtuple;
// 		newSlot = NULL;
// 	}

// 	/*
// 	 * In READ COMMITTED isolation level it's possible that target tuple was
// 	 * changed due to concurrent update.  In that case we have a raw subplan
// 	 * output tuple in newSlot, and need to run it through the junk filter to
// 	 * produce an insertable tuple.
// 	 *
// 	 * Caution: more than likely, the passed-in slot is the same as the
// 	 * junkfilter's output slot, so we are clobbering the original value of
// 	 * slottuple by doing the filtering.  This is OK since neither we nor our
// 	 * caller have any more interest in the prior contents of that slot.
// 	 */
// 	if (newSlot != NULL)
// 	{
// 		slot = ExecFilterJunk(relinfo->ri_junkFilter, newSlot);
// 		slottuple = ExecMaterializeSlot(slot);
// 		newtuple = slottuple;
// 	}


// 	LocTriggerData.type = T_TriggerData;
// 	LocTriggerData.tg_event = TRIGGER_EVENT_UPDATE |
// 		TRIGGER_EVENT_ROW |
// 		TRIGGER_EVENT_BEFORE;
// 	LocTriggerData.tg_relation = relinfo->ri_RelationDesc;
// 	updatedCols = GetUpdatedColumns(relinfo, estate);
// 	for (i = 0; i < trigdesc->numtriggers; i++)
// 	{
// 		Trigger    *trigger = &trigdesc->triggers[i];

// 		if (!TRIGGER_TYPE_MATCHES(trigger->tgtype,
// 								  TRIGGER_TYPE_ROW,
// 								  TRIGGER_TYPE_BEFORE,
// 								  TRIGGER_TYPE_UPDATE))
// 			continue;
// 		if (!TriggerEnabled(estate, relinfo, trigger, LocTriggerData.tg_event,
// 							updatedCols, trigtuple, newtuple))
// 			continue;

// 		LocTriggerData.tg_trigtuple = trigtuple;
// 		LocTriggerData.tg_newtuple = oldtuple = newtuple;
// 		LocTriggerData.tg_trigtuplebuf = InvalidBuffer;
// 		LocTriggerData.tg_newtuplebuf = InvalidBuffer;
// 		LocTriggerData.tg_trigger = trigger;
// 		newtuple = ExecCallTriggerFunc(&LocTriggerData,
// 									   i,
// 									   relinfo->ri_TrigFunctions,
// 									   relinfo->ri_TrigInstrument,
// 									   GetPerTupleMemoryContext(estate));
// 		if (oldtuple != newtuple && oldtuple != slottuple)
// 			heap_freetuple(oldtuple);
// 		if (newtuple == NULL)
// 		{
// 			if (trigtuple != fdw_trigtuple)
// 				heap_freetuple(trigtuple);
// 			return NULL;		/* "do nothing" */
// 		}
// 	}
// 	if (trigtuple != fdw_trigtuple)
// 		heap_freetuple(trigtuple);

// 	if (newtuple != slottuple)
// 	{
// 		/*
// 		 * Return the modified tuple using the es_trig_tuple_slot.  We assume
// 		 * the tuple was allocated in per-tuple memory context, and therefore
// 		 * will go away by itself. The tuple table slot should not try to
// 		 * clear it.
// 		 */
// 		TupleTableSlot *newslot = estate->es_trig_tuple_slot;
// 		TupleDesc	tupdesc = RelationGetDescr(relinfo->ri_RelationDesc);

// 		if (newslot->tts_tupleDescriptor != tupdesc)
// 			ExecSetSlotDescriptor(newslot, tupdesc);
// 		ExecStoreTuple(newtuple, newslot, InvalidBuffer, false);
// 		slot = newslot;
// 	}
// 	return slot;
// }
// //end
// void
// ExecARUpdateTriggers(EState *estate, ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple,
// 					 HeapTuple newtuple,
// 					 List *recheckIndexes)
// {
// 	TriggerDesc *trigdesc = relinfo->ri_TrigDesc;

// 	if (trigdesc && trigdesc->trig_update_after_row)
// 	{
// 		HeapTuple	trigtuple;

// 		Assert(HeapTupleIsValid(fdw_trigtuple) ^ ItemPointerIsValid(tupleid));
// 		if (fdw_trigtuple == NULL)
// 			trigtuple = GetTupleForTrigger(estate,
// 										   NULL,
// 										   relinfo,
// 										   tupleid,
// 										   LockTupleExclusive,
// 										   NULL);
// 		else
// 			trigtuple = fdw_trigtuple;

// 		AfterTriggerSaveEvent(estate, relinfo, TRIGGER_EVENT_UPDATE,
// 							  true, trigtuple, newtuple, recheckIndexes,
// 							  GetUpdatedColumns(relinfo, estate));
// 		if (trigtuple != fdw_trigtuple)
// 			heap_freetuple(trigtuple);
// 	}
// }
// //end

// //utils/adt/ri_triggers.c

// bool
// RI_FKey_pk_upd_check_required(Trigger *trigger, Relation pk_rel,
// 							  HeapTuple old_row, HeapTuple new_row)
// {
// 	const RI_ConstraintInfo *riinfo;

// 	/*
// 	 * Get arguments.
// 	 */
// 	riinfo = ri_FetchConstraintInfo(trigger, pk_rel, true);

// 	switch (riinfo->confmatchtype)
// 	{
// 		case FKCONSTR_MATCH_SIMPLE:
// 		case FKCONSTR_MATCH_FULL:

// 			/*
// 			 * If any old key value is NULL, the row could not have been
// 			 * referenced by an FK row, so no check is needed.
// 			 */
// 			if (ri_NullCheck(old_row, riinfo, true) != RI_KEYS_NONE_NULL)
// 				return false;

// 			/* If all old and new key values are equal, no check is needed */
// 			if (ri_KeysEqual(pk_rel, old_row, new_row, riinfo, true))
// 				return false;

// 			/* Else we need to fire the trigger. */
// 			return true;

// 			/* Handle MATCH PARTIAL check. */
// 		case FKCONSTR_MATCH_PARTIAL:
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("MATCH PARTIAL not yet implemented")));
// 			break;

// 		default:
// 			elog(ERROR, "unrecognized confmatchtype: %d",
// 				 riinfo->confmatchtype);
// 			break;
// 	}

// 	/* Never reached */
// 	return false;
// }
// //end

// bool
// RI_FKey_fk_upd_check_required(Trigger *trigger, Relation fk_rel,
// 							  HeapTuple old_row, HeapTuple new_row)
// {
// 	const RI_ConstraintInfo *riinfo;

// 	/*
// 	 * Get arguments.
// 	 */
// 	riinfo = ri_FetchConstraintInfo(trigger, fk_rel, false);

// 	switch (riinfo->confmatchtype)
// 	{
// 		case FKCONSTR_MATCH_SIMPLE:

// 			/*
// 			 * If any new key value is NULL, the row must satisfy the
// 			 * constraint, so no check is needed.
// 			 */
// 			if (ri_NullCheck(new_row, riinfo, false) != RI_KEYS_NONE_NULL)
// 				return false;

// 			/*
// 			 * If the original row was inserted by our own transaction, we
// 			 * must fire the trigger whether or not the keys are equal.  This
// 			 * is because our UPDATE will invalidate the INSERT so that the
// 			 * INSERT RI trigger will not do anything; so we had better do the
// 			 * UPDATE check.  (We could skip this if we knew the INSERT
// 			 * trigger already fired, but there is no easy way to know that.)
// 			 */
// 			if (TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmin(old_row->t_data)))
// 				return true;

// 			/* If all old and new key values are equal, no check is needed */
// 			if (ri_KeysEqual(fk_rel, old_row, new_row, riinfo, false))
// 				return false;

// 			/* Else we need to fire the trigger. */
// 			return true;

// 		case FKCONSTR_MATCH_FULL:

// 			/*
// 			 * If all new key values are NULL, the row must satisfy the
// 			 * constraint, so no check is needed.  On the other hand, if only
// 			 * some of them are NULL, the row must fail the constraint.  We
// 			 * must not throw error here, because the row might get
// 			 * invalidated before the constraint is to be checked, but we
// 			 * should queue the event to apply the check later.
// 			 */
// 			switch (ri_NullCheck(new_row, riinfo, false))
// 			{
// 				case RI_KEYS_ALL_NULL:
// 					return false;
// 				case RI_KEYS_SOME_NULL:
// 					return true;
// 				case RI_KEYS_NONE_NULL:
// 					break;		/* continue with the check */
// 			}

// 			/*
// 			 * If the original row was inserted by our own transaction, we
// 			 * must fire the trigger whether or not the keys are equal.  This
// 			 * is because our UPDATE will invalidate the INSERT so that the
// 			 * INSERT RI trigger will not do anything; so we had better do the
// 			 * UPDATE check.  (We could skip this if we knew the INSERT
// 			 * trigger already fired, but there is no easy way to know that.)
// 			 */
// 			if (TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmin(old_row->t_data)))
// 				return true;

// 			/* If all old and new key values are equal, no check is needed */
// 			if (ri_KeysEqual(fk_rel, old_row, new_row, riinfo, false))
// 				return false;

// 			/* Else we need to fire the trigger. */
// 			return true;

// 			/* Handle MATCH PARTIAL check. */
// 		case FKCONSTR_MATCH_PARTIAL:
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("MATCH PARTIAL not yet implemented")));
// 			break;

// 		default:
// 			elog(ERROR, "unrecognized confmatchtype: %d",
// 				 riinfo->confmatchtype);
// 			break;
// 	}

// 	/* Never reached */
// 	return false;
// }
// //end
// int
// RI_FKey_trigger_type(Oid tgfoid)
// {
// 	switch (tgfoid)
// 	{
// 		case F_RI_FKEY_CASCADE_DEL:
// 		case F_RI_FKEY_CASCADE_UPD:
// 		case F_RI_FKEY_RESTRICT_DEL:
// 		case F_RI_FKEY_RESTRICT_UPD:
// 		case F_RI_FKEY_SETNULL_DEL:
// 		case F_RI_FKEY_SETNULL_UPD:
// 		case F_RI_FKEY_SETDEFAULT_DEL:
// 		case F_RI_FKEY_SETDEFAULT_UPD:
// 		case F_RI_FKEY_NOACTION_DEL:
// 		case F_RI_FKEY_NOACTION_UPD:
// 			return RI_TRIGGER_PK;

// 		case F_RI_FKEY_CHECK_INS:
// 		case F_RI_FKEY_CHECK_UPD:
// 			return RI_TRIGGER_FK;
// 	}

// 	return RI_TRIGGER_NONE;
// }
// //end
// //end trigger.c

// //新文件夹目录
// //executor/executor.h
// /*
//  * prototypes from functions in execAmi.c
//  */
// bool
// ExecSupportsMarkRestore(Path *pathnode)
// {
// 	/*
// 	 * For consistency with the routines above, we do not examine the nodeTag
// 	 * but rather the pathtype, which is the Plan node type the Path would
// 	 * produce.
// 	 */
// 	switch (pathnode->pathtype)
// 	{
// 		case T_IndexScan:
// 		case T_IndexOnlyScan:
// 		case T_Material:
// 		case T_Sort:
// 			return true;

// 		case T_CustomScan:
// 			Assert(IsA(pathnode, CustomPath));
// 			if (((CustomPath *) pathnode)->flags & CUSTOMPATH_SUPPORT_MARK_RESTORE)
// 				return true;
// 			return false;

// 		case T_Result:

// 			/*
// 			 * Result supports mark/restore iff it has a child plan that does.
// 			 *
// 			 * We have to be careful here because there is more than one Path
// 			 * type that can produce a Result plan node.
// 			 */
// 			if (IsA(pathnode, ProjectionPath))
// 				return ExecSupportsMarkRestore(((ProjectionPath *) pathnode)->subpath);
// 			else if (IsA(pathnode, MinMaxAggPath))
// 				return false;	/* childless Result */
// 			else
// 			{
// 				Assert(IsA(pathnode, ResultPath));
// 				return false;	/* childless Result */
// 			}

// 		default:
// 			break;
// 	}

// 	return false;
// }
// //end
// /*
//  * prototypes from functions in execCurrent.c
//  */
// bool
// execCurrentOf(CurrentOfExpr *cexpr,
// 			  ExprContext *econtext,
// 			  Oid table_oid,
// 			  ItemPointer current_tid)
// {
// 	char	   *cursor_name;
// 	char	   *table_name;
// 	Portal		portal;
// 	QueryDesc  *queryDesc;

// 	/* Get the cursor name --- may have to look up a parameter reference */
// 	if (cexpr->cursor_name)
// 		cursor_name = cexpr->cursor_name;
// 	else
// 		cursor_name = fetch_cursor_param_value(econtext, cexpr->cursor_param);

// 	/* Fetch table name for possible use in error messages */
// 	table_name = get_rel_name(table_oid);
// 	if (table_name == NULL)
// 		elog(ERROR, "cache lookup failed for relation %u", table_oid);

// 	/* Find the cursor's portal */
// 	portal = GetPortalByName(cursor_name);
// 	if (!PortalIsValid(portal))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_CURSOR),
// 				 errmsg("cursor \"%s\" does not exist", cursor_name)));

// 	/*
// 	 * We have to watch out for non-SELECT queries as well as held cursors,
// 	 * both of which may have null queryDesc.
// 	 */
// 	if (portal->strategy != PORTAL_ONE_SELECT)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 				 errmsg("cursor \"%s\" is not a SELECT query",
// 						cursor_name)));
// 	queryDesc = PortalGetQueryDesc(portal);
// 	if (queryDesc == NULL || queryDesc->estate == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 				 errmsg("cursor \"%s\" is held from a previous transaction",
// 						cursor_name)));

// 	/*
// 	 * We have two different strategies depending on whether the cursor uses
// 	 * FOR UPDATE/SHARE or not.  The reason for supporting both is that the
// 	 * FOR UPDATE code is able to identify a target table in many cases where
// 	 * the other code can't, while the non-FOR-UPDATE case allows use of WHERE
// 	 * CURRENT OF with an insensitive cursor.
// 	 */
// 	if (queryDesc->estate->es_rowMarks)
// 	{
// 		ExecRowMark *erm;
// 		ListCell   *lc;

// 		/*
// 		 * Here, the query must have exactly one FOR UPDATE/SHARE reference to
// 		 * the target table, and we dig the ctid info out of that.
// 		 */
// 		erm = NULL;
// 		foreach(lc, queryDesc->estate->es_rowMarks)
// 		{
// 			ExecRowMark *thiserm = (ExecRowMark *) lfirst(lc);

// 			if (!RowMarkRequiresRowShareLock(thiserm->markType))
// 				continue;		/* ignore non-FOR UPDATE/SHARE items */

// 			if (thiserm->relid == table_oid)
// 			{
// 				if (erm)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 							 errmsg("cursor \"%s\" has multiple FOR UPDATE/SHARE references to table \"%s\"",
// 									cursor_name, table_name)));
// 				erm = thiserm;
// 			}
// 		}

// 		if (erm == NULL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 					 errmsg("cursor \"%s\" does not have a FOR UPDATE/SHARE reference to table \"%s\"",
// 							cursor_name, table_name)));

// 		/*
// 		 * The cursor must have a current result row: per the SQL spec, it's
// 		 * an error if not.
// 		 */
// 		if (portal->atStart || portal->atEnd)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 					 errmsg("cursor \"%s\" is not positioned on a row",
// 							cursor_name)));

// 		/* Return the currently scanned TID, if there is one */
// 		if (ItemPointerIsValid(&(erm->curCtid)))
// 		{
// 			*current_tid = erm->curCtid;
// 			return true;
// 		}

// 		/*
// 		 * This table didn't produce the cursor's current row; some other
// 		 * inheritance child of the same parent must have.  Signal caller to
// 		 * do nothing on this table.
// 		 */
// 		return false;
// 	}
// 	else
// 	{
// 		ScanState  *scanstate;
// 		bool		lisnull;
// 		Oid tuple_tableoid PG_USED_FOR_ASSERTS_ONLY;
// 		ItemPointer tuple_tid;

// 		/*
// 		 * Without FOR UPDATE, we dig through the cursor's plan to find the
// 		 * scan node.  Fail if it's not there or buried underneath
// 		 * aggregation.
// 		 */
// 		scanstate = search_plan_tree(queryDesc->planstate, table_oid);
// 		if (!scanstate)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 					 errmsg("cursor \"%s\" is not a simply updatable scan of table \"%s\"",
// 							cursor_name, table_name)));

// 		/*
// 		 * The cursor must have a current result row: per the SQL spec, it's
// 		 * an error if not.  We test this at the top level, rather than at the
// 		 * scan node level, because in inheritance cases any one table scan
// 		 * could easily not be on a row. We want to return false, not raise
// 		 * error, if the passed-in table OID is for one of the inactive scans.
// 		 */
// 		if (portal->atStart || portal->atEnd)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_INVALID_CURSOR_STATE),
// 					 errmsg("cursor \"%s\" is not positioned on a row",
// 							cursor_name)));

// 		/* Now OK to return false if we found an inactive scan */
// 		if (TupIsNull(scanstate->ss_ScanTupleSlot))
// 			return false;

// 		/* Use slot_getattr to catch any possible mistakes */
// 		tuple_tableoid =
// 			DatumGetObjectId(slot_getattr(scanstate->ss_ScanTupleSlot,
// 										  TableOidAttributeNumber,
// 										  &lisnull));
// 		Assert(!lisnull);
// 		tuple_tid = (ItemPointer)
// 			DatumGetPointer(slot_getattr(scanstate->ss_ScanTupleSlot,
// 										 SelfItemPointerAttributeNumber,
// 										 &lisnull));
// 		Assert(!lisnull);

// 		Assert(tuple_tableoid == table_oid);

// 		*current_tid = *tuple_tid;

// 		return true;
// 	}
// }
// //end
// /*
//  * prototypes from functions in execGrouping.c
//  */
// TupleHashTable
// BuildTupleHashTable(int numCols, AttrNumber *keyColIdx,
// 					FmgrInfo *eqfunctions,
// 					FmgrInfo *hashfunctions,
// 					long nbuckets, Size entrysize,
// 					MemoryContext tablecxt, MemoryContext tempcxt)
// {
// 	TupleHashTable hashtable;
// 	HASHCTL		hash_ctl;

// 	Assert(nbuckets > 0);
// 	Assert(entrysize >= sizeof(TupleHashEntryData));

// 	/* Limit initial table size request to not more than work_mem */
// 	nbuckets = Min(nbuckets, (long) ((work_mem * 1024L) / entrysize));

// 	hashtable = (TupleHashTable) MemoryContextAlloc(tablecxt,
// 												 sizeof(TupleHashTableData));

// 	hashtable->numCols = numCols;
// 	hashtable->keyColIdx = keyColIdx;
// 	hashtable->tab_hash_funcs = hashfunctions;
// 	hashtable->tab_eq_funcs = eqfunctions;
// 	hashtable->tablecxt = tablecxt;
// 	hashtable->tempcxt = tempcxt;
// 	hashtable->entrysize = entrysize;
// 	hashtable->tableslot = NULL;	/* will be made on first lookup */
// 	hashtable->inputslot = NULL;
// 	hashtable->in_hash_funcs = NULL;
// 	hashtable->cur_eq_funcs = NULL;

// 	MemSet(&hash_ctl, 0, sizeof(hash_ctl));
// 	hash_ctl.keysize = sizeof(TupleHashEntryData);
// 	hash_ctl.entrysize = entrysize;
// 	hash_ctl.hash = TupleHashTableHash;
// 	hash_ctl.match = TupleHashTableMatch;
// 	hash_ctl.hcxt = tablecxt;
// 	hashtable->hashtab = hash_create("TupleHashTable", nbuckets,
// 									 &hash_ctl,
// 					HASH_ELEM | HASH_FUNCTION | HASH_COMPARE | HASH_CONTEXT);

// 	return hashtable;
// }
// //end
// TupleHashEntry
// LookupTupleHashEntry(TupleHashTable hashtable, TupleTableSlot *slot,
// 					 bool *isnew)
// {
// 	TupleHashEntry entry;
// 	MemoryContext oldContext;
// 	TupleHashTable saveCurHT;
// 	TupleHashEntryData dummy;
// 	bool		found;

// 	/* If first time through, clone the input slot to make table slot */
// 	if (hashtable->tableslot == NULL)
// 	{
// 		TupleDesc	tupdesc;

// 		oldContext = MemoryContextSwitchTo(hashtable->tablecxt);

// 		/*
// 		 * We copy the input tuple descriptor just for safety --- we assume
// 		 * all input tuples will have equivalent descriptors.
// 		 */
// 		tupdesc = CreateTupleDescCopy(slot->tts_tupleDescriptor);
// 		hashtable->tableslot = MakeSingleTupleTableSlot(tupdesc);
// 		MemoryContextSwitchTo(oldContext);
// 	}

// 	/* Need to run the hash functions in short-lived context */
// 	oldContext = MemoryContextSwitchTo(hashtable->tempcxt);

// 	/*
// 	 * Set up data needed by hash and match functions
// 	 *
// 	 * We save and restore CurTupleHashTable just in case someone manages to
// 	 * invoke this code re-entrantly.
// 	 */
// 	hashtable->inputslot = slot;
// 	hashtable->in_hash_funcs = hashtable->tab_hash_funcs;
// 	hashtable->cur_eq_funcs = hashtable->tab_eq_funcs;

// 	saveCurHT = CurTupleHashTable;
// 	CurTupleHashTable = hashtable;

// 	/* Search the hash table */
// 	dummy.firstTuple = NULL;	/* flag to reference inputslot */
// 	entry = (TupleHashEntry) hash_search(hashtable->hashtab,
// 										 &dummy,
// 										 isnew ? HASH_ENTER : HASH_FIND,
// 										 &found);

// 	if (isnew)
// 	{
// 		if (found)
// 		{
// 			/* found pre-existing entry */
// 			*isnew = false;
// 		}
// 		else
// 		{
// 			/*
// 			 * created new entry
// 			 *
// 			 * Zero any caller-requested space in the entry.  (This zaps the
// 			 * "key data" dynahash.c copied into the new entry, but we don't
// 			 * care since we're about to overwrite it anyway.)
// 			 */
// 			MemSet(entry, 0, hashtable->entrysize);

// 			/* Copy the first tuple into the table context */
// 			MemoryContextSwitchTo(hashtable->tablecxt);
// 			entry->firstTuple = ExecCopySlotMinimalTuple(slot);

// 			*isnew = true;
// 		}
// 	}

// 	CurTupleHashTable = saveCurHT;

// 	MemoryContextSwitchTo(oldContext);

// 	return entry;
// }
// //end
// /*
//  * prototypes from functions in execJunk.c
//  */
// JunkFilter *
// ExecInitJunkFilter(List *targetList, bool hasoid, TupleTableSlot *slot)
// {
// 	JunkFilter *junkfilter;
// 	TupleDesc	cleanTupType;
// 	int			cleanLength;
// 	AttrNumber *cleanMap;
// 	ListCell   *t;
// 	AttrNumber	cleanResno;

// 	/*
// 	 * Compute the tuple descriptor for the cleaned tuple.
// 	 */
// 	cleanTupType = ExecCleanTypeFromTL(targetList, hasoid);

// 	/*
// 	 * Use the given slot, or make a new slot if we weren't given one.
// 	 */
// 	if (slot)
// 		ExecSetSlotDescriptor(slot, cleanTupType);
// 	else
// 		slot = MakeSingleTupleTableSlot(cleanTupType);

// 	/*
// 	 * Now calculate the mapping between the original tuple's attributes and
// 	 * the "clean" tuple's attributes.
// 	 *
// 	 * The "map" is an array of "cleanLength" attribute numbers, i.e. one
// 	 * entry for every attribute of the "clean" tuple. The value of this entry
// 	 * is the attribute number of the corresponding attribute of the
// 	 * "original" tuple.  (Zero indicates a NULL output attribute, but we do
// 	 * not use that feature in this routine.)
// 	 */
// 	cleanLength = cleanTupType->natts;
// 	if (cleanLength > 0)
// 	{
// 		cleanMap = (AttrNumber *) palloc(cleanLength * sizeof(AttrNumber));
// 		cleanResno = 1;
// 		foreach(t, targetList)
// 		{
// 			TargetEntry *tle = lfirst(t);

// 			if (!tle->resjunk)
// 			{
// 				cleanMap[cleanResno - 1] = tle->resno;
// 				cleanResno++;
// 			}
// 		}
// 	}
// 	else
// 		cleanMap = NULL;

// 	/*
// 	 * Finally create and initialize the JunkFilter struct.
// 	 */
// 	junkfilter = makeNode(JunkFilter);

// 	junkfilter->jf_targetList = targetList;
// 	junkfilter->jf_cleanTupType = cleanTupType;
// 	junkfilter->jf_cleanMap = cleanMap;
// 	junkfilter->jf_resultSlot = slot;

// 	return junkfilter;
// }
// //end
// /*
//  * prototypes from functions in execMain.c
//  */
// void
// ExecutorRun(QueryDesc *queryDesc,
// 			ScanDirection direction, uint64 count)
// {
// 	if (ExecutorRun_hook)
// 		(*ExecutorRun_hook) (queryDesc, direction, count);
// 	else
// 		standard_ExecutorRun(queryDesc, direction, count);
// }
// //end
// void
// standard_ExecutorRun(QueryDesc *queryDesc,
// 					 ScanDirection direction, uint64 count)
// {
// 	EState	   *estate;
// 	CmdType		operation;
// 	DestReceiver *dest;
// 	bool		sendTuples;
// 	MemoryContext oldcontext;

// 	/* sanity checks */
// 	Assert(queryDesc != NULL);

// 	estate = queryDesc->estate;

// 	Assert(estate != NULL);
// 	Assert(!(estate->es_top_eflags & EXEC_FLAG_EXPLAIN_ONLY));

// 	/*
// 	 * Switch into per-query memory context
// 	 */
// 	oldcontext = MemoryContextSwitchTo(estate->es_query_cxt);

// 	/* Allow instrumentation of Executor overall runtime */
// 	if (queryDesc->totaltime)
// 		InstrStartNode(queryDesc->totaltime);

// 	/*
// 	 * extract information from the query descriptor and the query feature.
// 	 */
// 	operation = queryDesc->operation;
// 	dest = queryDesc->dest;

// 	/*
// 	 * startup tuple receiver, if we will be emitting tuples
// 	 */
// 	estate->es_processed = 0;
// 	estate->es_lastoid = InvalidOid;

// 	sendTuples = (operation == CMD_SELECT ||
// 				  queryDesc->plannedstmt->hasReturning);

// 	if (sendTuples)
// 		(*dest->rStartup) (dest, operation, queryDesc->tupDesc);

// 	/*
// 	 * run plan
// 	 */
// 	if (!ScanDirectionIsNoMovement(direction))
// 		ExecutePlan(estate,
// 					queryDesc->planstate,
// 					queryDesc->plannedstmt->parallelModeNeeded,
// 					operation,
// 					sendTuples,
// 					count,
// 					direction,
// 					dest);

// 	/*
// 	 * shutdown tuple receiver, if we started it
// 	 */
// 	if (sendTuples)
// 		(*dest->rShutdown) (dest);

// 	if (queryDesc->totaltime)
// 		InstrStopNode(queryDesc->totaltime, estate->es_processed);

// 	MemoryContextSwitchTo(oldcontext);
// }
// //end
// ExecRowMark *
// ExecFindRowMark(EState *estate, Index rti, bool missing_ok)
// {
// 	ListCell   *lc;

// 	foreach(lc, estate->es_rowMarks)
// 	{
// 		ExecRowMark *erm = (ExecRowMark *) lfirst(lc);

// 		if (erm->rti == rti)
// 			return erm;
// 	}
// 	if (!missing_ok)
// 		elog(ERROR, "failed to find ExecRowMark for rangetable index %u", rti);
// 	return NULL;
// }
// //end

// TupleTableSlot *
// EvalPlanQual(EState *estate, EPQState *epqstate,
// 			 Relation relation, Index rti, int lockmode,
// 			 ItemPointer tid, TransactionId priorXmax)
// {
// 	TupleTableSlot *slot;
// 	HeapTuple	copyTuple;

// 	Assert(rti > 0);

// 	/*
// 	 * Get and lock the updated version of the row; if fail, return NULL.
// 	 */
// 	copyTuple = EvalPlanQualFetch(estate, relation, lockmode, LockWaitBlock,
// 								  tid, priorXmax);

// 	if (copyTuple == NULL)
// 		return NULL;

// 	/*
// 	 * For UPDATE/DELETE we have to return tid of actual row we're executing
// 	 * PQ for.
// 	 */
// 	*tid = copyTuple->t_self;

// 	/*
// 	 * Need to run a recheck subquery.  Initialize or reinitialize EPQ state.
// 	 */
// 	EvalPlanQualBegin(epqstate, estate);

// 	/*
// 	 * Free old test tuple, if any, and store new tuple where relation's scan
// 	 * node will see it
// 	 */
// 	EvalPlanQualSetTuple(epqstate, rti, copyTuple);

// 	/*
// 	 * Fetch any non-locked source rows
// 	 */
// 	EvalPlanQualFetchRowMarks(epqstate);

// 	/*
// 	 * Run the EPQ query.  We assume it will return at most one tuple.
// 	 */
// 	slot = EvalPlanQualNext(epqstate);

// 	/*
// 	 * If we got a tuple, force the slot to materialize the tuple so that it
// 	 * is not dependent on any local state in the EPQ query (in particular,
// 	 * it's highly likely that the slot contains references to any pass-by-ref
// 	 * datums that may be present in copyTuple).  As with the next step, this
// 	 * is to guard against early re-use of the EPQ query.
// 	 */
// 	if (!TupIsNull(slot))
// 		(void) ExecMaterializeSlot(slot);

// 	/*
// 	 * Clear out the test tuple.  This is needed in case the EPQ query is
// 	 * re-used to test a tuple for a different relation.  (Not clear that can
// 	 * really happen, but let's be safe.)
// 	 */
// 	EvalPlanQualSetTuple(epqstate, rti, NULL);

// 	return slot;
// }
// //end
// HeapTuple
// EvalPlanQualFetch(EState *estate, Relation relation, int lockmode,
// 				  LockWaitPolicy wait_policy,
// 				  ItemPointer tid, TransactionId priorXmax)
// {
// 	HeapTuple	copyTuple = NULL;
// 	HeapTupleData tuple;
// 	SnapshotData SnapshotDirty;

// 	/*
// 	 * fetch target tuple
// 	 *
// 	 * Loop here to deal with updated or busy tuples
// 	 */
// 	InitDirtySnapshot(SnapshotDirty);
// 	tuple.t_self = *tid;
// 	for (;;)
// 	{
// 		Buffer		buffer;

// 		if (heap_fetch(relation, &SnapshotDirty, &tuple, &buffer, true, NULL))
// 		{
// 			HTSU_Result test;
// 			HeapUpdateFailureData hufd;

// 			/*
// 			 * If xmin isn't what we're expecting, the slot must have been
// 			 * recycled and reused for an unrelated tuple.  This implies that
// 			 * the latest version of the row was deleted, so we need do
// 			 * nothing.  (Should be safe to examine xmin without getting
// 			 * buffer's content lock.  We assume reading a TransactionId to be
// 			 * atomic, and Xmin never changes in an existing tuple, except to
// 			 * invalid or frozen, and neither of those can match priorXmax.)
// 			 */
// 			if (!TransactionIdEquals(HeapTupleHeaderGetXmin(tuple.t_data),
// 									 priorXmax))
// 			{
// 				ReleaseBuffer(buffer);
// 				return NULL;
// 			}

// 			/* otherwise xmin should not be dirty... */
// 			if (TransactionIdIsValid(SnapshotDirty.xmin))
// 				elog(ERROR, "t_xmin is uncommitted in tuple to be updated");

// 			/*
// 			 * If tuple is being updated by other transaction then we have to
// 			 * wait for its commit/abort, or die trying.
// 			 */
// 			if (TransactionIdIsValid(SnapshotDirty.xmax))
// 			{
// 				ReleaseBuffer(buffer);
// 				switch (wait_policy)
// 				{
// 					case LockWaitBlock:
// 						XactLockTableWait(SnapshotDirty.xmax,
// 										  relation, &tuple.t_self,
// 										  XLTW_FetchUpdated);
// 						break;
// 					case LockWaitSkip:
// 						if (!ConditionalXactLockTableWait(SnapshotDirty.xmax))
// 							return NULL;		/* skip instead of waiting */
// 						break;
// 					case LockWaitError:
// 						if (!ConditionalXactLockTableWait(SnapshotDirty.xmax))
// 							ereport(ERROR,
// 									(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 									 errmsg("could not obtain lock on row in relation \"%s\"",
// 										RelationGetRelationName(relation))));
// 						break;
// 				}
// 				continue;		/* loop back to repeat heap_fetch */
// 			}

// 			/*
// 			 * If tuple was inserted by our own transaction, we have to check
// 			 * cmin against es_output_cid: cmin >= current CID means our
// 			 * command cannot see the tuple, so we should ignore it. Otherwise
// 			 * heap_lock_tuple() will throw an error, and so would any later
// 			 * attempt to update or delete the tuple.  (We need not check cmax
// 			 * because HeapTupleSatisfiesDirty will consider a tuple deleted
// 			 * by our transaction dead, regardless of cmax.) We just checked
// 			 * that priorXmax == xmin, so we can test that variable instead of
// 			 * doing HeapTupleHeaderGetXmin again.
// 			 */
// 			if (TransactionIdIsCurrentTransactionId(priorXmax) &&
// 				HeapTupleHeaderGetCmin(tuple.t_data) >= estate->es_output_cid)
// 			{
// 				ReleaseBuffer(buffer);
// 				return NULL;
// 			}

// 			/*
// 			 * This is a live tuple, so now try to lock it.
// 			 */
// 			test = heap_lock_tuple(relation, &tuple,
// 								   estate->es_output_cid,
// 								   lockmode, wait_policy,
// 								   false, &buffer, &hufd);
// 			/* We now have two pins on the buffer, get rid of one */
// 			ReleaseBuffer(buffer);

// 			switch (test)
// 			{
// 				case HeapTupleSelfUpdated:

// 					/*
// 					 * The target tuple was already updated or deleted by the
// 					 * current command, or by a later command in the current
// 					 * transaction.  We *must* ignore the tuple in the former
// 					 * case, so as to avoid the "Halloween problem" of
// 					 * repeated update attempts.  In the latter case it might
// 					 * be sensible to fetch the updated tuple instead, but
// 					 * doing so would require changing heap_update and
// 					 * heap_delete to not complain about updating "invisible"
// 					 * tuples, which seems pretty scary (heap_lock_tuple will
// 					 * not complain, but few callers expect
// 					 * HeapTupleInvisible, and we're not one of them).  So for
// 					 * now, treat the tuple as deleted and do not process.
// 					 */
// 					ReleaseBuffer(buffer);
// 					return NULL;

// 				case HeapTupleMayBeUpdated:
// 					/* successfully locked */
// 					break;

// 				case HeapTupleUpdated:
// 					ReleaseBuffer(buffer);
// 					if (IsolationUsesXactSnapshot())
// 						ereport(ERROR,
// 								(errcode(ERRCODE_T_R_SERIALIZATION_FAILURE),
// 								 errmsg("could not serialize access due to concurrent update")));

// 					/* Should not encounter speculative tuple on recheck */
// 					Assert(!HeapTupleHeaderIsSpeculative(tuple.t_data));
// 					if (!ItemPointerEquals(&hufd.ctid, &tuple.t_self))
// 					{
// 						/* it was updated, so look at the updated version */
// 						tuple.t_self = hufd.ctid;
// 						/* updated row should have xmin matching this xmax */
// 						priorXmax = hufd.xmax;
// 						continue;
// 					}
// 					/* tuple was deleted, so give up */
// 					return NULL;

// 				case HeapTupleWouldBlock:
// 					ReleaseBuffer(buffer);
// 					return NULL;

// 				case HeapTupleInvisible:
// 					elog(ERROR, "attempted to lock invisible tuple");

// 				default:
// 					ReleaseBuffer(buffer);
// 					elog(ERROR, "unrecognized heap_lock_tuple status: %u",
// 						 test);
// 					return NULL;	/* keep compiler quiet */
// 			}

// 			/*
// 			 * We got tuple - now copy it for use by recheck query.
// 			 */
// 			copyTuple = heap_copytuple(&tuple);
// 			ReleaseBuffer(buffer);
// 			break;
// 		}

// 		/*
// 		 * If the referenced slot was actually empty, the latest version of
// 		 * the row must have been deleted, so we need do nothing.
// 		 */
// 		if (tuple.t_data == NULL)
// 		{
// 			ReleaseBuffer(buffer);
// 			return NULL;
// 		}

// 		/*
// 		 * As above, if xmin isn't what we're expecting, do nothing.
// 		 */
// 		if (!TransactionIdEquals(HeapTupleHeaderGetXmin(tuple.t_data),
// 								 priorXmax))
// 		{
// 			ReleaseBuffer(buffer);
// 			return NULL;
// 		}

// 		/*
// 		 * If we get here, the tuple was found but failed SnapshotDirty.
// 		 * Assuming the xmin is either a committed xact or our own xact (as it
// 		 * certainly should be if we're trying to modify the tuple), this must
// 		 * mean that the row was updated or deleted by either a committed xact
// 		 * or our own xact.  If it was deleted, we can ignore it; if it was
// 		 * updated then chain up to the next version and repeat the whole
// 		 * process.
// 		 *
// 		 * As above, it should be safe to examine xmax and t_ctid without the
// 		 * buffer content lock, because they can't be changing.
// 		 */
// 		if (ItemPointerEquals(&tuple.t_self, &tuple.t_data->t_ctid))
// 		{
// 			/* deleted, so forget about it */
// 			ReleaseBuffer(buffer);
// 			return NULL;
// 		}

// 		/* updated, so look at the updated row */
// 		tuple.t_self = tuple.t_data->t_ctid;
// 		/* updated row should have xmin matching this xmax */
// 		priorXmax = HeapTupleHeaderGetUpdateXid(tuple.t_data);
// 		ReleaseBuffer(buffer);
// 		/* loop back to fetch next in chain */
// 	}

// 	/*
// 	 * Return the copied tuple
// 	 */
// 	return copyTuple;
// }
// //end
// void
// EvalPlanQualSetTuple(EPQState *epqstate, Index rti, HeapTuple tuple)
// {
// 	EState	   *estate = epqstate->estate;

// 	Assert(rti > 0);

// 	/*
// 	 * free old test tuple, if any, and store new tuple where relation's scan
// 	 * node will see it
// 	 */
// 	if (estate->es_epqTuple[rti - 1] != NULL)
// 		heap_freetuple(estate->es_epqTuple[rti - 1]);
// 	estate->es_epqTuple[rti - 1] = tuple;
// 	estate->es_epqTupleSet[rti - 1] = true;
// }
// //end
// HeapTuple
// EvalPlanQualGetTuple(EPQState *epqstate, Index rti)
// {
// 	EState	   *estate = epqstate->estate;

// 	Assert(rti > 0);

// 	return estate->es_epqTuple[rti - 1];
// }
// //end
// void
// EvalPlanQualBegin(EPQState *epqstate, EState *parentestate)
// {
// 	EState	   *estate = epqstate->estate;

// 	if (estate == NULL)
// 	{
// 		/* First time through, so create a child EState */
// 		EvalPlanQualStart(epqstate, parentestate, epqstate->plan);
// 	}
// 	else
// 	{
// 		/*
// 		 * We already have a suitable child EPQ tree, so just reset it.
// 		 */
// 		int			rtsize = list_length(parentestate->es_range_table);
// 		PlanState  *planstate = epqstate->planstate;

// 		MemSet(estate->es_epqScanDone, 0, rtsize * sizeof(bool));

// 		/* Recopy current values of parent parameters */
// 		if (parentestate->es_plannedstmt->nParamExec > 0)
// 		{
// 			int			i = parentestate->es_plannedstmt->nParamExec;

// 			while (--i >= 0)
// 			{
// 				/* copy value if any, but not execPlan link */
// 				estate->es_param_exec_vals[i].value =
// 					parentestate->es_param_exec_vals[i].value;
// 				estate->es_param_exec_vals[i].isnull =
// 					parentestate->es_param_exec_vals[i].isnull;
// 			}
// 		}

// 		/*
// 		 * Mark child plan tree as needing rescan at all scan nodes.  The
// 		 * first ExecProcNode will take care of actually doing the rescan.
// 		 */
// 		planstate->chgParam = bms_add_member(planstate->chgParam,
// 											 epqstate->epqParam);
// 	}
// }
// //end
// void
// ExecWithCheckOptions(WCOKind kind, ResultRelInfo *resultRelInfo,
// 					 TupleTableSlot *slot, EState *estate)
// {
// 	Relation	rel = resultRelInfo->ri_RelationDesc;
// 	TupleDesc	tupdesc = RelationGetDescr(rel);
// 	ExprContext *econtext;
// 	ListCell   *l1,
// 			   *l2;

// 	/*
// 	 * We will use the EState's per-tuple context for evaluating constraint
// 	 * expressions (creating it if it's not already there).
// 	 */
// 	econtext = GetPerTupleExprContext(estate);

// 	/* Arrange for econtext's scan tuple to be the tuple under test */
// 	econtext->ecxt_scantuple = slot;

// 	/* Check each of the constraints */
// 	forboth(l1, resultRelInfo->ri_WithCheckOptions,
// 			l2, resultRelInfo->ri_WithCheckOptionExprs)
// 	{
// 		WithCheckOption *wco = (WithCheckOption *) lfirst(l1);
// 		ExprState  *wcoExpr = (ExprState *) lfirst(l2);

// 		/*
// 		 * Skip any WCOs which are not the kind we are looking for at this
// 		 * time.
// 		 */
// 		if (wco->kind != kind)
// 			continue;

// 		/*
// 		 * WITH CHECK OPTION checks are intended to ensure that the new tuple
// 		 * is visible (in the case of a view) or that it passes the
// 		 * 'with-check' policy (in the case of row security). If the qual
// 		 * evaluates to NULL or FALSE, then the new tuple won't be included in
// 		 * the view or doesn't pass the 'with-check' policy for the table.  We
// 		 * need ExecQual to return FALSE for NULL to handle the view case (the
// 		 * opposite of what we do above for CHECK constraints).
// 		 */
// 		if (!ExecQual((List *) wcoExpr, econtext, false))
// 		{
// 			char	   *val_desc;
// 			Bitmapset  *modifiedCols;
// 			Bitmapset  *insertedCols;
// 			Bitmapset  *updatedCols;

// 			switch (wco->kind)
// 			{
// 					/*
// 					 * For WITH CHECK OPTIONs coming from views, we might be
// 					 * able to provide the details on the row, depending on
// 					 * the permissions on the relation (that is, if the user
// 					 * could view it directly anyway).  For RLS violations, we
// 					 * don't include the data since we don't know if the user
// 					 * should be able to view the tuple as as that depends on
// 					 * the USING policy.
// 					 */
// 				case WCO_VIEW_CHECK:
// 					insertedCols = GetInsertedColumns(resultRelInfo, estate);
// 					updatedCols = GetUpdatedColumns(resultRelInfo, estate);
// 					modifiedCols = bms_union(insertedCols, updatedCols);
// 					val_desc = ExecBuildSlotValueDescription(RelationGetRelid(rel),
// 															 slot,
// 															 tupdesc,
// 															 modifiedCols,
// 															 64);

// 					ereport(ERROR,
// 							(errcode(ERRCODE_WITH_CHECK_OPTION_VIOLATION),
// 					  errmsg("new row violates check option for view \"%s\"",
// 							 wco->relname),
// 							 val_desc ? errdetail("Failing row contains %s.",
// 												  val_desc) : 0));
// 					break;
// 				case WCO_RLS_INSERT_CHECK:
// 				case WCO_RLS_UPDATE_CHECK:
// 					if (wco->polname != NULL)
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 								 errmsg("new row violates row-level security policy \"%s\" for table \"%s\"",
// 										wco->polname, wco->relname)));
// 					else
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 								 errmsg("new row violates row-level security policy for table \"%s\"",
// 										wco->relname)));
// 					break;
// 				case WCO_RLS_CONFLICT_CHECK:
// 					if (wco->polname != NULL)
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 								 errmsg("new row violates row-level security policy \"%s\" (USING expression) for table \"%s\"",
// 										wco->polname, wco->relname)));
// 					else
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 								 errmsg("new row violates row-level security policy (USING expression) for table \"%s\"",
// 										wco->relname)));
// 					break;
// 				default:
// 					elog(ERROR, "unrecognized WCO kind: %u", wco->kind);
// 					break;
// 			}
// 		}
// 	}
// }
// //end
// LockTupleMode
// ExecUpdateLockMode(EState *estate, ResultRelInfo *relinfo)
// {
// 	Bitmapset  *keyCols;
// 	Bitmapset  *updatedCols;

// 	/*
// 	 * Compute lock mode to use.  If columns that are part of the key have not
// 	 * been modified, then we can use a weaker lock, allowing for better
// 	 * concurrency.
// 	 */
// 	updatedCols = GetUpdatedColumns(relinfo, estate);
// 	keyCols = RelationGetIndexAttrBitmap(relinfo->ri_RelationDesc,
// 										 INDEX_ATTR_BITMAP_KEY);

// 	if (bms_overlap(keyCols, updatedCols))
// 		return LockTupleExclusive;

// 	return LockTupleNoKeyExclusive;
// }
// //end

// /*
//  * prototypes from functions in execQual.c
//  */
// Tuplestorestate *
// ExecMakeTableFunctionResult(ExprState *funcexpr,
// 							ExprContext *econtext,
// 							MemoryContext argContext,
// 							TupleDesc expectedDesc,
// 							bool randomAccess)
// {
// 	Tuplestorestate *tupstore = NULL;
// 	TupleDesc	tupdesc = NULL;
// 	Oid			funcrettype;
// 	bool		returnsTuple;
// 	bool		returnsSet = false;
// 	FunctionCallInfoData fcinfo;
// 	PgStat_FunctionCallUsage fcusage;
// 	ReturnSetInfo rsinfo;
// 	HeapTupleData tmptup;
// 	MemoryContext callerContext;
// 	MemoryContext oldcontext;
// 	bool		direct_function_call;
// 	bool		first_time = true;

// 	callerContext = CurrentMemoryContext;

// 	funcrettype = exprType((Node *) funcexpr->expr);

// 	returnsTuple = type_is_rowtype(funcrettype);

// 	/*
// 	 * Prepare a resultinfo node for communication.  We always do this even if
// 	 * not expecting a set result, so that we can pass expectedDesc.  In the
// 	 * generic-expression case, the expression doesn't actually get to see the
// 	 * resultinfo, but set it up anyway because we use some of the fields as
// 	 * our own state variables.
// 	 */
// 	rsinfo.type = T_ReturnSetInfo;
// 	rsinfo.econtext = econtext;
// 	rsinfo.expectedDesc = expectedDesc;
// 	rsinfo.allowedModes = (int) (SFRM_ValuePerCall | SFRM_Materialize | SFRM_Materialize_Preferred);
// 	if (randomAccess)
// 		rsinfo.allowedModes |= (int) SFRM_Materialize_Random;
// 	rsinfo.returnMode = SFRM_ValuePerCall;
// 	/* isDone is filled below */
// 	rsinfo.setResult = NULL;
// 	rsinfo.setDesc = NULL;

// 	/*
// 	 * Normally the passed expression tree will be a FuncExprState, since the
// 	 * grammar only allows a function call at the top level of a table
// 	 * function reference.  However, if the function doesn't return set then
// 	 * the planner might have replaced the function call via constant-folding
// 	 * or inlining.  So if we see any other kind of expression node, execute
// 	 * it via the general ExecEvalExpr() code; the only difference is that we
// 	 * don't get a chance to pass a special ReturnSetInfo to any functions
// 	 * buried in the expression.
// 	 */
// 	if (funcexpr && IsA(funcexpr, FuncExprState) &&
// 		IsA(funcexpr->expr, FuncExpr))
// 	{
// 		FuncExprState *fcache = (FuncExprState *) funcexpr;
// 		ExprDoneCond argDone;

// 		/*
// 		 * This path is similar to ExecMakeFunctionResult.
// 		 */
// 		direct_function_call = true;

// 		/*
// 		 * Initialize function cache if first time through
// 		 */
// 		if (fcache->func.fn_oid == InvalidOid)
// 		{
// 			FuncExpr   *func = (FuncExpr *) fcache->xprstate.expr;

// 			init_fcache(func->funcid, func->inputcollid, fcache,
// 						econtext->ecxt_per_query_memory, false);
// 		}
// 		returnsSet = fcache->func.fn_retset;
// 		InitFunctionCallInfoData(fcinfo, &(fcache->func),
// 								 list_length(fcache->args),
// 								 fcache->fcinfo_data.fncollation,
// 								 NULL, (Node *) &rsinfo);

// 		/*
// 		 * Evaluate the function's argument list.
// 		 *
// 		 * We can't do this in the per-tuple context: the argument values
// 		 * would disappear when we reset that context in the inner loop.  And
// 		 * the caller's CurrentMemoryContext is typically a query-lifespan
// 		 * context, so we don't want to leak memory there.  We require the
// 		 * caller to pass a separate memory context that can be used for this,
// 		 * and can be reset each time through to avoid bloat.
// 		 */
// 		MemoryContextReset(argContext);
// 		oldcontext = MemoryContextSwitchTo(argContext);
// 		argDone = ExecEvalFuncArgs(&fcinfo, fcache->args, econtext);
// 		MemoryContextSwitchTo(oldcontext);

// 		/* We don't allow sets in the arguments of the table function */
// 		if (argDone != ExprSingleResult)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("set-valued function called in context that cannot accept a set")));

// 		/*
// 		 * If function is strict, and there are any NULL arguments, skip
// 		 * calling the function and act like it returned NULL (or an empty
// 		 * set, in the returns-set case).
// 		 */
// 		if (fcache->func.fn_strict)
// 		{
// 			int			i;

// 			for (i = 0; i < fcinfo.nargs; i++)
// 			{
// 				if (fcinfo.argnull[i])
// 					goto no_function_result;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		/* Treat funcexpr as a generic expression */
// 		direct_function_call = false;
// 		InitFunctionCallInfoData(fcinfo, NULL, 0, InvalidOid, NULL, NULL);
// 	}

// 	/*
// 	 * Switch to short-lived context for calling the function or expression.
// 	 */
// 	MemoryContextSwitchTo(econtext->ecxt_per_tuple_memory);

// 	/*
// 	 * Loop to handle the ValuePerCall protocol (which is also the same
// 	 * behavior needed in the generic ExecEvalExpr path).
// 	 */
// 	for (;;)
// 	{
// 		Datum		result;

// 		CHECK_FOR_INTERRUPTS();

// 		/*
// 		 * reset per-tuple memory context before each call of the function or
// 		 * expression. This cleans up any local memory the function may leak
// 		 * when called.
// 		 */
// 		ResetExprContext(econtext);

// 		/* Call the function or expression one time */
// 		if (direct_function_call)
// 		{
// 			pgstat_init_function_usage(&fcinfo, &fcusage);

// 			fcinfo.isnull = false;
// 			rsinfo.isDone = ExprSingleResult;
// 			result = FunctionCallInvoke(&fcinfo);

// 			pgstat_end_function_usage(&fcusage,
// 									  rsinfo.isDone != ExprMultipleResult);
// 		}
// 		else
// 		{
// 			result = ExecEvalExpr(funcexpr, econtext,
// 								  &fcinfo.isnull, &rsinfo.isDone);
// 		}

// 		/* Which protocol does function want to use? */
// 		if (rsinfo.returnMode == SFRM_ValuePerCall)
// 		{
// 			/*
// 			 * Check for end of result set.
// 			 */
// 			if (rsinfo.isDone == ExprEndResult)
// 				break;

// 			/*
// 			 * If first time through, build tuplestore for result.  For a
// 			 * scalar function result type, also make a suitable tupdesc.
// 			 */
// 			if (first_time)
// 			{
// 				oldcontext = MemoryContextSwitchTo(econtext->ecxt_per_query_memory);
// 				tupstore = tuplestore_begin_heap(randomAccess, false, work_mem);
// 				rsinfo.setResult = tupstore;
// 				if (!returnsTuple)
// 				{
// 					tupdesc = CreateTemplateTupleDesc(1, false);
// 					TupleDescInitEntry(tupdesc,
// 									   (AttrNumber) 1,
// 									   "column",
// 									   funcrettype,
// 									   -1,
// 									   0);
// 					rsinfo.setDesc = tupdesc;
// 				}
// 				MemoryContextSwitchTo(oldcontext);
// 			}

// 			/*
// 			 * Store current resultset item.
// 			 */
// 			if (returnsTuple)
// 			{
// 				if (!fcinfo.isnull)
// 				{
// 					HeapTupleHeader td = DatumGetHeapTupleHeader(result);

// 					if (tupdesc == NULL)
// 					{
// 						/*
// 						 * This is the first non-NULL result from the
// 						 * function.  Use the type info embedded in the
// 						 * rowtype Datum to look up the needed tupdesc.  Make
// 						 * a copy for the query.
// 						 */
// 						oldcontext = MemoryContextSwitchTo(econtext->ecxt_per_query_memory);
// 						tupdesc = lookup_rowtype_tupdesc_copy(HeapTupleHeaderGetTypeId(td),
// 											   HeapTupleHeaderGetTypMod(td));
// 						rsinfo.setDesc = tupdesc;
// 						MemoryContextSwitchTo(oldcontext);
// 					}
// 					else
// 					{
// 						/*
// 						 * Verify all later returned rows have same subtype;
// 						 * necessary in case the type is RECORD.
// 						 */
// 						if (HeapTupleHeaderGetTypeId(td) != tupdesc->tdtypeid ||
// 							HeapTupleHeaderGetTypMod(td) != tupdesc->tdtypmod)
// 							ereport(ERROR,
// 									(errcode(ERRCODE_DATATYPE_MISMATCH),
// 									 errmsg("rows returned by function are not all of the same row type")));
// 					}

// 					/*
// 					 * tuplestore_puttuple needs a HeapTuple not a bare
// 					 * HeapTupleHeader, but it doesn't need all the fields.
// 					 */
// 					tmptup.t_len = HeapTupleHeaderGetDatumLength(td);
// 					tmptup.t_data = td;

// 					tuplestore_puttuple(tupstore, &tmptup);
// 				}
// 				else
// 				{
// 					/*
// 					 * NULL result from a tuple-returning function; expand it
// 					 * to a row of all nulls.  We rely on the expectedDesc to
// 					 * form such rows.  (Note: this would be problematic if
// 					 * tuplestore_putvalues saved the tdtypeid/tdtypmod from
// 					 * the provided descriptor, since that might not match
// 					 * what we get from the function itself.  But it doesn't.)
// 					 */
// 					int			natts = expectedDesc->natts;
// 					bool	   *nullflags;

// 					nullflags = (bool *) palloc(natts * sizeof(bool));
// 					memset(nullflags, true, natts * sizeof(bool));
// 					tuplestore_putvalues(tupstore, expectedDesc, NULL, nullflags);
// 				}
// 			}
// 			else
// 			{
// 				/* Scalar-type case: just store the function result */
// 				tuplestore_putvalues(tupstore, tupdesc, &result, &fcinfo.isnull);
// 			}

// 			/*
// 			 * Are we done?
// 			 */
// 			if (rsinfo.isDone != ExprMultipleResult)
// 				break;
// 		}
// 		else if (rsinfo.returnMode == SFRM_Materialize)
// 		{
// 			/* check we're on the same page as the function author */
// 			if (!first_time || rsinfo.isDone != ExprSingleResult)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_E_R_I_E_SRF_PROTOCOL_VIOLATED),
// 						 errmsg("table-function protocol for materialize mode was not followed")));
// 			/* Done evaluating the set result */
// 			break;
// 		}
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_E_R_I_E_SRF_PROTOCOL_VIOLATED),
// 					 errmsg("unrecognized table-function returnMode: %d",
// 							(int) rsinfo.returnMode)));

// 		first_time = false;
// 	}

// no_function_result:

// 	/*
// 	 * If we got nothing from the function (ie, an empty-set or NULL result),
// 	 * we have to create the tuplestore to return, and if it's a
// 	 * non-set-returning function then insert a single all-nulls row.  As
// 	 * above, we depend on the expectedDesc to manufacture the dummy row.
// 	 */
// 	if (rsinfo.setResult == NULL)
// 	{
// 		MemoryContextSwitchTo(econtext->ecxt_per_query_memory);
// 		tupstore = tuplestore_begin_heap(randomAccess, false, work_mem);
// 		rsinfo.setResult = tupstore;
// 		if (!returnsSet)
// 		{
// 			int			natts = expectedDesc->natts;
// 			bool	   *nullflags;

// 			MemoryContextSwitchTo(econtext->ecxt_per_tuple_memory);
// 			nullflags = (bool *) palloc(natts * sizeof(bool));
// 			memset(nullflags, true, natts * sizeof(bool));
// 			tuplestore_putvalues(tupstore, expectedDesc, NULL, nullflags);
// 		}
// 	}

// 	/*
// 	 * If function provided a tupdesc, cross-check it.  We only really need to
// 	 * do this for functions returning RECORD, but might as well do it always.
// 	 */
// 	if (rsinfo.setDesc)
// 	{
// 		tupledesc_match(expectedDesc, rsinfo.setDesc);

// 		/*
// 		 * If it is a dynamically-allocated TupleDesc, free it: it is
// 		 * typically allocated in a per-query context, so we must avoid
// 		 * leaking it across multiple usages.
// 		 */
// 		if (rsinfo.setDesc->tdrefcount == -1)
// 			FreeTupleDesc(rsinfo.setDesc);
// 	}

// 	MemoryContextSwitchTo(callerContext);

// 	/* All done, pass back the tuplestore */
// 	return rsinfo.setResult;
// }

// //end
// /*
//  * prototypes from functions in execTuples.c
//  */
// void
// ExecInitResultTupleSlot(EState *estate, PlanState *planstate)
// {
// 	planstate->ps_ResultTupleSlot = ExecAllocTableSlot(&estate->es_tupleTable);
// }
// //end
// void
// ExecInitScanTupleSlot(EState *estate, ScanState *scanstate)
// {
// 	scanstate->ss_ScanTupleSlot = ExecAllocTableSlot(&estate->es_tupleTable);
// }
// //end
// TupleTableSlot *
// ExecInitExtraTupleSlot(EState *estate)
// {
// 	return ExecAllocTableSlot(&estate->es_tupleTable);
// }
// //end

// TupleDesc
// ExecTypeFromTL(List *targetList, bool hasoid)
// {
// 	return ExecTypeFromTLInternal(targetList, hasoid, false);
// }
// //end
// TupleDesc
// ExecCleanTypeFromTL(List *targetList, bool hasoid)
// {
// 	return ExecTypeFromTLInternal(targetList, hasoid, true);
// }
// //end
// //此函数为上面两个函数引用的函数
// static TupleDesc
// ExecTypeFromTLInternal(List *targetList, bool hasoid, bool skipjunk)
// {
// 	TupleDesc	typeInfo;
// 	ListCell   *l;
// 	int			len;
// 	int			cur_resno = 1;

// 	if (skipjunk)
// 		len = ExecCleanTargetListLength(targetList);
// 	else
// 		len = ExecTargetListLength(targetList);
// 	typeInfo = CreateTemplateTupleDesc(len, hasoid);

// 	foreach(l, targetList)
// 	{
// 		TargetEntry *tle = lfirst(l);

// 		if (skipjunk && tle->resjunk)
// 			continue;
// 		TupleDescInitEntry(typeInfo,
// 						   cur_resno,
// 						   tle->resname,
// 						   exprType((Node *) tle->expr),
// 						   exprTypmod((Node *) tle->expr),
// 						   0);
// 		TupleDescInitEntryCollation(typeInfo,
// 									cur_resno,
// 									exprCollation((Node *) tle->expr));
// 		cur_resno++;
// 	}

// 	return typeInfo;
// }
// //end

// TupleDesc
// ExecTypeFromExprList(List *exprList)
// {
// 	TupleDesc	typeInfo;
// 	ListCell   *lc;
// 	int			cur_resno = 1;

// 	typeInfo = CreateTemplateTupleDesc(list_length(exprList), false);

// 	foreach(lc, exprList)
// 	{
// 		Node	   *e = lfirst(lc);

// 		TupleDescInitEntry(typeInfo,
// 						   cur_resno,
// 						   NULL,
// 						   exprType(e),
// 						   exprTypmod(e),
// 						   0);
// 		TupleDescInitEntryCollation(typeInfo,
// 									cur_resno,
// 									exprCollation(e));
// 		cur_resno++;
// 	}

// 	return typeInfo;
// }
// //end
// void
// do_tup_output(TupOutputState *tstate, Datum *values, bool *isnull)
// {
// 	TupleTableSlot *slot = tstate->slot;
// 	int			natts = slot->tts_tupleDescriptor->natts;

// 	/* make sure the slot is clear */
// 	ExecClearTuple(slot);

// 	/* insert data */
// 	memcpy(slot->tts_values, values, natts * sizeof(Datum));
// 	memcpy(slot->tts_isnull, isnull, natts * sizeof(bool));

// 	/* mark slot as containing a virtual tuple */
// 	ExecStoreVirtualTuple(slot);

// 	/* send the tuple to the receiver */
// 	(void) (*tstate->dest->receiveSlot) (slot, tstate->dest);

// 	/* clean up */
// 	ExecClearTuple(slot);
// }
// //end
// void
// do_text_output_multiline(TupOutputState *tstate, const char *txt)
// {
// 	Datum		values[1];
// 	bool		isnull[1] = {false};

// 	while (*txt)
// 	{
// 		const char *eol;
// 		int			len;

// 		eol = strchr(txt, '\n');
// 		if (eol)
// 		{
// 			len = eol - txt;
// 			eol++;
// 		}
// 		else
// 		{
// 			len = strlen(txt);
// 			eol = txt + len;
// 		}

// 		values[0] = PointerGetDatum(cstring_to_text_with_len(txt, len));
// 		do_tup_output(tstate, values, isnull);
// 		pfree(DatumGetPointer(values[0]));
// 		txt = eol;
// 	}
// }
// //end
// void
// ExecTypeSetColNames(TupleDesc typeInfo, List *namesList)
// {
// 	bool		modified = false;
// 	int			colno = 0;
// 	ListCell   *lc;

// 	foreach(lc, namesList)
// 	{
// 		char	   *cname = strVal(lfirst(lc));
// 		Form_pg_attribute attr;

// 		/* Guard against too-long names list */
// 		if (colno >= typeInfo->natts)
// 			break;
// 		attr = typeInfo->attrs[colno++];

// 		/* Ignore empty aliases (these must be for dropped columns) */
// 		if (cname[0] == '\0')
// 			continue;

// 		/* Change tupdesc only if alias is actually different */
// 		if (strcmp(cname, NameStr(attr->attname)) != 0)
// 		{
// 			namestrcpy(&(attr->attname), cname);
// 			modified = true;
// 		}
// 	}

// 	/* If we modified the tupdesc, it's now a new record type */
// 	if (modified)
// 	{
// 		typeInfo->tdtypeid = RECORDOID;
// 		typeInfo->tdtypmod = -1;
// 	}
// }
// //end

// /*
//  * prototypes from functions in execUtils.c
//  */
// EState *
// CreateExecutorState(void)
// {
// 	EState	   *estate;
// 	MemoryContext qcontext;
// 	MemoryContext oldcontext;

// 	/*
// 	 * Create the per-query context for this Executor run.
// 	 */
// 	qcontext = AllocSetContextCreate(CurrentMemoryContext,
// 									 "ExecutorState",
// 									 ALLOCSET_DEFAULT_SIZES);

// 	/*
// 	 * Make the EState node within the per-query context.  This way, we don't
// 	 * need a separate pfree() operation for it at shutdown.
// 	 */
// 	oldcontext = MemoryContextSwitchTo(qcontext);

// 	estate = makeNode(EState);

// 	/*
// 	 * Initialize all fields of the Executor State structure
// 	 */
// 	estate->es_direction = ForwardScanDirection;
// 	estate->es_snapshot = InvalidSnapshot;		/* caller must initialize this */
// 	estate->es_crosscheck_snapshot = InvalidSnapshot;	/* no crosscheck */
// 	estate->es_range_table = NIL;
// 	estate->es_plannedstmt = NULL;

// 	estate->es_junkFilter = NULL;

// 	estate->es_output_cid = (CommandId) 0;

// 	estate->es_result_relations = NULL;
// 	estate->es_num_result_relations = 0;
// 	estate->es_result_relation_info = NULL;

// 	estate->es_trig_target_relations = NIL;
// 	estate->es_trig_tuple_slot = NULL;
// 	estate->es_trig_oldtup_slot = NULL;
// 	estate->es_trig_newtup_slot = NULL;

// 	estate->es_param_list_info = NULL;
// 	estate->es_param_exec_vals = NULL;

// 	estate->es_query_cxt = qcontext;

// 	estate->es_tupleTable = NIL;

// 	estate->es_rowMarks = NIL;

// 	estate->es_processed = 0;
// 	estate->es_lastoid = InvalidOid;

// 	estate->es_top_eflags = 0;
// 	estate->es_instrument = 0;
// 	estate->es_finished = false;

// 	estate->es_exprcontexts = NIL;

// 	estate->es_subplanstates = NIL;

// 	estate->es_auxmodifytables = NIL;

// 	estate->es_per_tuple_exprcontext = NULL;

// 	estate->es_epqTuple = NULL;
// 	estate->es_epqTupleSet = NULL;
// 	estate->es_epqScanDone = NULL;

// 	/*
// 	 * Return the executor state structure
// 	 */
// 	MemoryContextSwitchTo(oldcontext);

// 	return estate;
// }
// //end
// void
// ExecAssignResultTypeFromTL(PlanState *planstate)
// {
// 	bool		hasoid;
// 	TupleDesc	tupDesc;

// 	if (ExecContextForcesOids(planstate, &hasoid))
// 	{
// 		/* context forces OID choice; hasoid is now set correctly */
// 	}
// 	else
// 	{
// 		/* given free choice, don't leave space for OIDs in result tuples */
// 		hasoid = false;
// 	}

// 	/*
// 	 * ExecTypeFromTL needs the parse-time representation of the tlist, not a
// 	 * list of ExprStates.  This is good because some plan nodes don't bother
// 	 * to set up planstate->targetlist ...
// 	 */
// 	tupDesc = ExecTypeFromTL(planstate->plan->targetlist, hasoid);
// 	ExecAssignResultType(planstate, tupDesc);
// }
// //end
// Relation
// ExecOpenScanRelation(EState *estate, Index scanrelid, int eflags)
// {
// 	Relation	rel;
// 	Oid			reloid;
// 	LOCKMODE	lockmode;

// 	/*
// 	 * Determine the lock type we need.  First, scan to see if target relation
// 	 * is a result relation.  If not, check if it's a FOR UPDATE/FOR SHARE
// 	 * relation.  In either of those cases, we got the lock already.
// 	 */
// 	lockmode = AccessShareLock;
// 	if (ExecRelationIsTargetRelation(estate, scanrelid))
// 		lockmode = NoLock;
// 	else
// 	{
// 		/* Keep this check in sync with InitPlan! */
// 		ExecRowMark *erm = ExecFindRowMark(estate, scanrelid, true);

// 		if (erm != NULL && erm->relation != NULL)
// 			lockmode = NoLock;
// 	}

// 	/* Open the relation and acquire lock as needed */
// 	reloid = getrelid(scanrelid, estate->es_range_table);
// 	rel = heap_open(reloid, lockmode);

// 	/*
// 	 * Complain if we're attempting a scan of an unscannable relation, except
// 	 * when the query won't actually be run.  This is a slightly klugy place
// 	 * to do this, perhaps, but there is no better place.
// 	 */
// 	if ((eflags & (EXEC_FLAG_EXPLAIN_ONLY | EXEC_FLAG_WITH_NO_DATA)) == 0 &&
// 		!RelationIsScannable(rel))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("materialized view \"%s\" has not been populated",
// 						RelationGetRelationName(rel)),
// 				 errhint("Use the REFRESH MATERIALIZED VIEW command.")));

// 	return rel;
// }
// //end
// void
// ExecAssignExprContext(EState *estate, PlanState *planstate)
// {
// 	planstate->ps_ExprContext = CreateExprContext(estate);
// }
// //end
// //此函数为上面一个函数引用的函数
// ExprContext *
// CreateExprContext(EState *estate)
// {
// 	ExprContext *econtext;
// 	MemoryContext oldcontext;

// 	/* Create the ExprContext node within the per-query memory context */
// 	oldcontext = MemoryContextSwitchTo(estate->es_query_cxt);

// 	econtext = makeNode(ExprContext);

// 	/* Initialize fields of ExprContext */
// 	econtext->ecxt_scantuple = NULL;
// 	econtext->ecxt_innertuple = NULL;
// 	econtext->ecxt_outertuple = NULL;

// 	econtext->ecxt_per_query_memory = estate->es_query_cxt;

// 	/*
// 	 * Create working memory for expression evaluation in this context.
// 	 */
// 	econtext->ecxt_per_tuple_memory =
// 		AllocSetContextCreate(estate->es_query_cxt,
// 							  "ExprContext",
// 							  ALLOCSET_DEFAULT_SIZES);

// 	econtext->ecxt_param_exec_vals = estate->es_param_exec_vals;
// 	econtext->ecxt_param_list_info = estate->es_param_list_info;

// 	econtext->ecxt_aggvalues = NULL;
// 	econtext->ecxt_aggnulls = NULL;

// 	econtext->caseValue_datum = (Datum) 0;
// 	econtext->caseValue_isNull = true;

// 	econtext->domainValue_datum = (Datum) 0;
// 	econtext->domainValue_isNull = true;

// 	econtext->ecxt_estate = estate;

// 	econtext->ecxt_callbacks = NULL;

// 	/*
// 	 * Link the ExprContext into the EState to ensure it is shut down when the
// 	 * EState is freed.  Because we use lcons(), shutdowns will occur in
// 	 * reverse order of creation, which may not be essential but can't hurt.
// 	 */
// 	estate->es_exprcontexts = lcons(econtext, estate->es_exprcontexts);

// 	MemoryContextSwitchTo(oldcontext);

// 	return econtext;
// }
// //end
// /*
//  * prototypes from functions in execIndexing.c
//  */
// List *
// ExecInsertIndexTuples(TupleTableSlot *slot,
// 					  ItemPointer tupleid,
// 					  EState *estate,
// 					  bool noDupErr,
// 					  bool *specConflict,
// 					  List *arbiterIndexes)
// {
// 	List	   *result = NIL;
// 	ResultRelInfo *resultRelInfo;
// 	int			i;
// 	int			numIndices;
// 	RelationPtr relationDescs;
// 	Relation	heapRelation;
// 	IndexInfo **indexInfoArray;
// 	ExprContext *econtext;
// 	Datum		values[INDEX_MAX_KEYS];
// 	bool		isnull[INDEX_MAX_KEYS];

// 	/*
// 	 * Get information from the result relation info structure.
// 	 */
// 	resultRelInfo = estate->es_result_relation_info;
// 	numIndices = resultRelInfo->ri_NumIndices;
// 	relationDescs = resultRelInfo->ri_IndexRelationDescs;
// 	indexInfoArray = resultRelInfo->ri_IndexRelationInfo;
// 	heapRelation = resultRelInfo->ri_RelationDesc;

// 	/*
// 	 * We will use the EState's per-tuple context for evaluating predicates
// 	 * and index expressions (creating it if it's not already there).
// 	 */
// 	econtext = GetPerTupleExprContext(estate);

// 	/* Arrange for econtext's scan tuple to be the tuple under test */
// 	econtext->ecxt_scantuple = slot;

// 	/*
// 	 * for each index, form and insert the index tuple
// 	 */
// 	for (i = 0; i < numIndices; i++)
// 	{
// 		Relation	indexRelation = relationDescs[i];
// 		IndexInfo  *indexInfo;
// 		bool		applyNoDupErr;
// 		IndexUniqueCheck checkUnique;
// 		bool		satisfiesConstraint;

// 		if (indexRelation == NULL)
// 			continue;

// 		indexInfo = indexInfoArray[i];

// 		/* If the index is marked as read-only, ignore it */
// 		if (!indexInfo->ii_ReadyForInserts)
// 			continue;

// 		/* Check for partial index */
// 		if (indexInfo->ii_Predicate != NIL)
// 		{
// 			List	   *predicate;

// 			/*
// 			 * If predicate state not set up yet, create it (in the estate's
// 			 * per-query context)
// 			 */
// 			predicate = indexInfo->ii_PredicateState;
// 			if (predicate == NIL)
// 			{
// 				predicate = (List *)
// 					ExecPrepareExpr((Expr *) indexInfo->ii_Predicate,
// 									estate);
// 				indexInfo->ii_PredicateState = predicate;
// 			}

// 			/* Skip this index-update if the predicate isn't satisfied */
// 			if (!ExecQual(predicate, econtext, false))
// 				continue;
// 		}

// 		/*
// 		 * FormIndexDatum fills in its values and isnull parameters with the
// 		 * appropriate values for the column(s) of the index.
// 		 */
// 		FormIndexDatum(indexInfo,
// 					   slot,
// 					   estate,
// 					   values,
// 					   isnull);

// 		/* Check whether to apply noDupErr to this index */
// 		applyNoDupErr = noDupErr &&
// 			(arbiterIndexes == NIL ||
// 			 list_member_oid(arbiterIndexes,
// 							 indexRelation->rd_index->indexrelid));

// 		/*
// 		 * The index AM does the actual insertion, plus uniqueness checking.
// 		 *
// 		 * For an immediate-mode unique index, we just tell the index AM to
// 		 * throw error if not unique.
// 		 *
// 		 * For a deferrable unique index, we tell the index AM to just detect
// 		 * possible non-uniqueness, and we add the index OID to the result
// 		 * list if further checking is needed.
// 		 *
// 		 * For a speculative insertion (used by INSERT ... ON CONFLICT), do
// 		 * the same as for a deferrable unique index.
// 		 */
// 		if (!indexRelation->rd_index->indisunique)
// 			checkUnique = UNIQUE_CHECK_NO;
// 		else if (applyNoDupErr)
// 			checkUnique = UNIQUE_CHECK_PARTIAL;
// 		else if (indexRelation->rd_index->indimmediate)
// 			checkUnique = UNIQUE_CHECK_YES;
// 		else
// 			checkUnique = UNIQUE_CHECK_PARTIAL;

// 		satisfiesConstraint =
// 			index_insert(indexRelation, /* index relation */
// 						 values,	/* array of index Datums */
// 						 isnull,	/* null flags */
// 						 tupleid,		/* tid of heap tuple */
// 						 heapRelation,	/* heap relation */
// 						 checkUnique);	/* type of uniqueness check to do */

// 		/*
// 		 * If the index has an associated exclusion constraint, check that.
// 		 * This is simpler than the process for uniqueness checks since we
// 		 * always insert first and then check.  If the constraint is deferred,
// 		 * we check now anyway, but don't throw error on violation or wait for
// 		 * a conclusive outcome from a concurrent insertion; instead we'll
// 		 * queue a recheck event.  Similarly, noDupErr callers (speculative
// 		 * inserters) will recheck later, and wait for a conclusive outcome
// 		 * then.
// 		 *
// 		 * An index for an exclusion constraint can't also be UNIQUE (not an
// 		 * essential property, we just don't allow it in the grammar), so no
// 		 * need to preserve the prior state of satisfiesConstraint.
// 		 */
// 		if (indexInfo->ii_ExclusionOps != NULL)
// 		{
// 			bool		violationOK;
// 			CEOUC_WAIT_MODE waitMode;

// 			if (applyNoDupErr)
// 			{
// 				violationOK = true;
// 				waitMode = CEOUC_LIVELOCK_PREVENTING_WAIT;
// 			}
// 			else if (!indexRelation->rd_index->indimmediate)
// 			{
// 				violationOK = true;
// 				waitMode = CEOUC_NOWAIT;
// 			}
// 			else
// 			{
// 				violationOK = false;
// 				waitMode = CEOUC_WAIT;
// 			}

// 			satisfiesConstraint =
// 				check_exclusion_or_unique_constraint(heapRelation,
// 													 indexRelation, indexInfo,
// 													 tupleid, values, isnull,
// 													 estate, false,
// 												waitMode, violationOK, NULL);
// 		}

// 		if ((checkUnique == UNIQUE_CHECK_PARTIAL ||
// 			 indexInfo->ii_ExclusionOps != NULL) &&
// 			!satisfiesConstraint)
// 		{
// 			/*
// 			 * The tuple potentially violates the uniqueness or exclusion
// 			 * constraint, so make a note of the index so that we can re-check
// 			 * it later.  Speculative inserters are told if there was a
// 			 * speculative conflict, since that always requires a restart.
// 			 */
// 			result = lappend_oid(result, RelationGetRelid(indexRelation));
// 			if (indexRelation->rd_index->indimmediate && specConflict)
// 				*specConflict = true;
// 		}
// 	}

// 	return result;
// }
// //end
// bool
// ExecCheckIndexConstraints(TupleTableSlot *slot,
// 						  EState *estate, ItemPointer conflictTid,
// 						  List *arbiterIndexes)
// {
// 	ResultRelInfo *resultRelInfo;
// 	int			i;
// 	int			numIndices;
// 	RelationPtr relationDescs;
// 	Relation	heapRelation;
// 	IndexInfo **indexInfoArray;
// 	ExprContext *econtext;
// 	Datum		values[INDEX_MAX_KEYS];
// 	bool		isnull[INDEX_MAX_KEYS];
// 	ItemPointerData invalidItemPtr;
// 	bool		checkedIndex = false;

// 	ItemPointerSetInvalid(conflictTid);
// 	ItemPointerSetInvalid(&invalidItemPtr);

// 	/*
// 	 * Get information from the result relation info structure.
// 	 */
// 	resultRelInfo = estate->es_result_relation_info;
// 	numIndices = resultRelInfo->ri_NumIndices;
// 	relationDescs = resultRelInfo->ri_IndexRelationDescs;
// 	indexInfoArray = resultRelInfo->ri_IndexRelationInfo;
// 	heapRelation = resultRelInfo->ri_RelationDesc;

// 	/*
// 	 * We will use the EState's per-tuple context for evaluating predicates
// 	 * and index expressions (creating it if it's not already there).
// 	 */
// 	econtext = GetPerTupleExprContext(estate);

// 	/* Arrange for econtext's scan tuple to be the tuple under test */
// 	econtext->ecxt_scantuple = slot;

// 	/*
// 	 * For each index, form index tuple and check if it satisfies the
// 	 * constraint.
// 	 */
// 	for (i = 0; i < numIndices; i++)
// 	{
// 		Relation	indexRelation = relationDescs[i];
// 		IndexInfo  *indexInfo;
// 		bool		satisfiesConstraint;

// 		if (indexRelation == NULL)
// 			continue;

// 		indexInfo = indexInfoArray[i];

// 		if (!indexInfo->ii_Unique && !indexInfo->ii_ExclusionOps)
// 			continue;

// 		/* If the index is marked as read-only, ignore it */
// 		if (!indexInfo->ii_ReadyForInserts)
// 			continue;

// 		/* When specific arbiter indexes requested, only examine them */
// 		if (arbiterIndexes != NIL &&
// 			!list_member_oid(arbiterIndexes,
// 							 indexRelation->rd_index->indexrelid))
// 			continue;

// 		if (!indexRelation->rd_index->indimmediate)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 					 errmsg("ON CONFLICT does not support deferrable unique constraints/exclusion constraints as arbiters"),
// 					 errtableconstraint(heapRelation,
// 								   RelationGetRelationName(indexRelation))));

// 		checkedIndex = true;

// 		/* Check for partial index */
// 		if (indexInfo->ii_Predicate != NIL)
// 		{
// 			List	   *predicate;

// 			/*
// 			 * If predicate state not set up yet, create it (in the estate's
// 			 * per-query context)
// 			 */
// 			predicate = indexInfo->ii_PredicateState;
// 			if (predicate == NIL)
// 			{
// 				predicate = (List *)
// 					ExecPrepareExpr((Expr *) indexInfo->ii_Predicate,
// 									estate);
// 				indexInfo->ii_PredicateState = predicate;
// 			}

// 			/* Skip this index-update if the predicate isn't satisfied */
// 			if (!ExecQual(predicate, econtext, false))
// 				continue;
// 		}

// 		/*
// 		 * FormIndexDatum fills in its values and isnull parameters with the
// 		 * appropriate values for the column(s) of the index.
// 		 */
// 		FormIndexDatum(indexInfo,
// 					   slot,
// 					   estate,
// 					   values,
// 					   isnull);

// 		satisfiesConstraint =
// 			check_exclusion_or_unique_constraint(heapRelation, indexRelation,
// 												 indexInfo, &invalidItemPtr,
// 											   values, isnull, estate, false,
// 												 CEOUC_WAIT, true,
// 												 conflictTid);
// 		if (!satisfiesConstraint)
// 			return false;
// 	}

// 	if (arbiterIndexes != NIL && !checkedIndex)
// 		elog(ERROR, "unexpected failure to find arbiter index");

// 	return true;
// }
// //end
// void
// check_exclusion_constraint(Relation heap, Relation index,
// 						   IndexInfo *indexInfo,
// 						   ItemPointer tupleid,
// 						   Datum *values, bool *isnull,
// 						   EState *estate, bool newIndex)
// {
// 	(void) check_exclusion_or_unique_constraint(heap, index, indexInfo, tupleid,
// 												values, isnull,
// 												estate, newIndex,
// 												CEOUC_WAIT, false, NULL);
// }
// //end
// /*
//  * prototypes from functions in execProcnode.c
//  */
// bool
// ExecShutdownNode(PlanState *node)
// {
// 	if (node == NULL)
// 		return false;

// 	switch (nodeTag(node))
// 	{
// 		case T_GatherState:
// 			ExecShutdownGather((GatherState *) node);
// 			break;
// 		default:
// 			break;
// 	}

// 	return planstate_tree_walker(node, ExecShutdownNode, NULL);
// }
// //end 
// //end executor.h

// //executor/tuptable.h

// /* in executor/execTuples.c */
// TupleTableSlot *
// MakeTupleTableSlot(void)
// {
// 	TupleTableSlot *slot = makeNode(TupleTableSlot);

// 	slot->tts_isempty = true;
// 	slot->tts_shouldFree = false;
// 	slot->tts_shouldFreeMin = false;
// 	slot->tts_tuple = NULL;
// 	slot->tts_tupleDescriptor = NULL;
// 	slot->tts_mcxt = CurrentMemoryContext;
// 	slot->tts_buffer = InvalidBuffer;
// 	slot->tts_nvalid = 0;
// 	slot->tts_values = NULL;
// 	slot->tts_isnull = NULL;
// 	slot->tts_mintuple = NULL;

// 	return slot;
// }
// //end
// TupleTableSlot *
// ExecAllocTableSlot(List **tupleTable)
// {
// 	TupleTableSlot *slot = MakeTupleTableSlot();

// 	*tupleTable = lappend(*tupleTable, slot);

// 	return slot;
// }
// //end
// TupleTableSlot *
// MakeSingleTupleTableSlot(TupleDesc tupdesc)
// {
// 	TupleTableSlot *slot = MakeTupleTableSlot();

// 	ExecSetSlotDescriptor(slot, tupdesc);

// 	return slot;
// }
// //end
// TupleTableSlot *
// ExecStoreTuple(HeapTuple tuple,
// 			   TupleTableSlot *slot,
// 			   Buffer buffer,
// 			   bool shouldFree)
// {
// 	/*
// 	 * sanity checks
// 	 */
// 	Assert(tuple != NULL);
// 	Assert(slot != NULL);
// 	Assert(slot->tts_tupleDescriptor != NULL);
// 	/* passing shouldFree=true for a tuple on a disk page is not sane */
// 	Assert(BufferIsValid(buffer) ? (!shouldFree) : true);

// 	/*
// 	 * Free any old physical tuple belonging to the slot.
// 	 */
// 	if (slot->tts_shouldFree)
// 		heap_freetuple(slot->tts_tuple);
// 	if (slot->tts_shouldFreeMin)
// 		heap_free_minimal_tuple(slot->tts_mintuple);

// 	/*
// 	 * Store the new tuple into the specified slot.
// 	 */
// 	slot->tts_isempty = false;
// 	slot->tts_shouldFree = shouldFree;
// 	slot->tts_shouldFreeMin = false;
// 	slot->tts_tuple = tuple;
// 	slot->tts_mintuple = NULL;

// 	/* Mark extracted state invalid */
// 	slot->tts_nvalid = 0;

// 	/*
// 	 * If tuple is on a disk page, keep the page pinned as long as we hold a
// 	 * pointer into it.  We assume the caller already has such a pin.
// 	 *
// 	 * This is coded to optimize the case where the slot previously held a
// 	 * tuple on the same disk page: in that case releasing and re-acquiring
// 	 * the pin is a waste of cycles.  This is a common situation during
// 	 * seqscans, so it's worth troubling over.
// 	 */
// 	if (slot->tts_buffer != buffer)
// 	{
// 		if (BufferIsValid(slot->tts_buffer))
// 			ReleaseBuffer(slot->tts_buffer);
// 		slot->tts_buffer = buffer;
// 		if (BufferIsValid(buffer))
// 			IncrBufferRefCount(buffer);
// 	}

// 	return slot;
// }
// //end
// MinimalTuple
// ExecCopySlotMinimalTuple(TupleTableSlot *slot)
// {
// 	/*
// 	 * sanity checks
// 	 */
// 	Assert(slot != NULL);
// 	Assert(!slot->tts_isempty);

// 	/*
// 	 * If we have a physical tuple then just copy it.  Prefer to copy
// 	 * tts_mintuple since that's a tad cheaper.
// 	 */
// 	if (slot->tts_mintuple)
// 		return heap_copy_minimal_tuple(slot->tts_mintuple);
// 	if (slot->tts_tuple)
// 		return minimal_tuple_from_heap_tuple(slot->tts_tuple);

// 	/*
// 	 * Otherwise we need to build a tuple from the Datum array.
// 	 */
// 	return heap_form_minimal_tuple(slot->tts_tupleDescriptor,
// 								   slot->tts_values,
// 								   slot->tts_isnull);
// }
// //end
// /* in access/common/heaptuple.c */
// Datum
// slot_getattr(TupleTableSlot *slot, int attnum, bool *isnull)
// {
// 	HeapTuple	tuple = slot->tts_tuple;
// 	TupleDesc	tupleDesc = slot->tts_tupleDescriptor;
// 	HeapTupleHeader tup;

// 	/*
// 	 * system attributes are handled by heap_getsysattr
// 	 */
// 	if (attnum <= 0)
// 	{
// 		if (tuple == NULL)		/* internal error */
// 			elog(ERROR, "cannot extract system attribute from virtual tuple");
// 		if (tuple == &(slot->tts_minhdr))		/* internal error */
// 			elog(ERROR, "cannot extract system attribute from minimal tuple");
// 		return heap_getsysattr(tuple, attnum, tupleDesc, isnull);
// 	}

// 	/*
// 	 * fast path if desired attribute already cached
// 	 */
// 	if (attnum <= slot->tts_nvalid)
// 	{
// 		*isnull = slot->tts_isnull[attnum - 1];
// 		return slot->tts_values[attnum - 1];
// 	}

// 	/*
// 	 * return NULL if attnum is out of range according to the tupdesc
// 	 */
// 	if (attnum > tupleDesc->natts)
// 	{
// 		*isnull = true;
// 		return (Datum) 0;
// 	}

// 	/*
// 	 * otherwise we had better have a physical tuple (tts_nvalid should equal
// 	 * natts in all virtual-tuple cases)
// 	 */
// 	if (tuple == NULL)			/* internal error */
// 		elog(ERROR, "cannot extract attribute from empty tuple slot");

// 	/*
// 	 * return NULL if attnum is out of range according to the tuple
// 	 *
// 	 * (We have to check this separately because of various inheritance and
// 	 * table-alteration scenarios: the tuple could be either longer or shorter
// 	 * than the tupdesc.)
// 	 */
// 	tup = tuple->t_data;
// 	if (attnum > HeapTupleHeaderGetNatts(tup))
// 	{
// 		*isnull = true;
// 		return (Datum) 0;
// 	}

// 	/*
// 	 * check if target attribute is null: no point in groveling through tuple
// 	 */
// 	if (HeapTupleHasNulls(tuple) && att_isnull(attnum - 1, tup->t_bits))
// 	{
// 		*isnull = true;
// 		return (Datum) 0;
// 	}

// 	/*
// 	 * If the attribute's column has been dropped, we force a NULL result.
// 	 * This case should not happen in normal use, but it could happen if we
// 	 * are executing a plan cached before the column was dropped.
// 	 */
// 	if (tupleDesc->attrs[attnum - 1]->attisdropped)
// 	{
// 		*isnull = true;
// 		return (Datum) 0;
// 	}

// 	/*
// 	 * Extract the attribute, along with any preceding attributes.
// 	 */
// 	slot_deform_tuple(slot, attnum);

// 	/*
// 	 * The result is acquired from tts_values array.
// 	 */
// 	*isnull = slot->tts_isnull[attnum - 1];
// 	return slot->tts_values[attnum - 1];
// }
// //end

// void
// slot_getallattrs(TupleTableSlot *slot)
// {
// 	int			tdesc_natts = slot->tts_tupleDescriptor->natts;
// 	int			attnum;
// 	HeapTuple	tuple;

// 	/* Quick out if we have 'em all already */
// 	if (slot->tts_nvalid == tdesc_natts)
// 		return;

// 	/*
// 	 * otherwise we had better have a physical tuple (tts_nvalid should equal
// 	 * natts in all virtual-tuple cases)
// 	 */
// 	tuple = slot->tts_tuple;
// 	if (tuple == NULL)			/* internal error */
// 		elog(ERROR, "cannot extract attribute from empty tuple slot");

// 	/*
// 	 * load up any slots available from physical tuple
// 	 */
// 	attnum = HeapTupleHeaderGetNatts(tuple->t_data);
// 	attnum = Min(attnum, tdesc_natts);

// 	slot_deform_tuple(slot, attnum);

// 	/*
// 	 * If tuple doesn't have all the atts indicated by tupleDesc, read the
// 	 * rest as null
// 	 */
// 	for (; attnum < tdesc_natts; attnum++)
// 	{
// 		slot->tts_values[attnum] = (Datum) 0;
// 		slot->tts_isnull[attnum] = true;
// 	}
// 	slot->tts_nvalid = tdesc_natts;
// }
// //end
// void
// slot_getsomeattrs(TupleTableSlot *slot, int attnum)
// {
// 	HeapTuple	tuple;
// 	int			attno;

// 	/* Quick out if we have 'em all already */
// 	if (slot->tts_nvalid >= attnum)
// 		return;

// 	/* Check for caller error */
// 	if (attnum <= 0 || attnum > slot->tts_tupleDescriptor->natts)
// 		elog(ERROR, "invalid attribute number %d", attnum);

// 	/*
// 	 * otherwise we had better have a physical tuple (tts_nvalid should equal
// 	 * natts in all virtual-tuple cases)
// 	 */
// 	tuple = slot->tts_tuple;
// 	if (tuple == NULL)			/* internal error */
// 		elog(ERROR, "cannot extract attribute from empty tuple slot");

// 	/*
// 	 * load up any slots available from physical tuple
// 	 */
// 	attno = HeapTupleHeaderGetNatts(tuple->t_data);
// 	attno = Min(attno, attnum);

// 	slot_deform_tuple(slot, attno);

// 	/*
// 	 * If tuple doesn't have all the atts indicated by tupleDesc, read the
// 	 * rest as null
// 	 */
// 	for (; attno < attnum; attno++)
// 	{
// 		slot->tts_values[attno] = (Datum) 0;
// 		slot->tts_isnull[attno] = true;
// 	}
// 	slot->tts_nvalid = attnum;
// }
// //end
// bool
// slot_attisnull(TupleTableSlot *slot, int attnum)
// {
// 	HeapTuple	tuple = slot->tts_tuple;
// 	TupleDesc	tupleDesc = slot->tts_tupleDescriptor;

// 	/*
// 	 * system attributes are handled by heap_attisnull
// 	 */
// 	if (attnum <= 0)
// 	{
// 		if (tuple == NULL)		/* internal error */
// 			elog(ERROR, "cannot extract system attribute from virtual tuple");
// 		if (tuple == &(slot->tts_minhdr))		/* internal error */
// 			elog(ERROR, "cannot extract system attribute from minimal tuple");
// 		return heap_attisnull(tuple, attnum);
// 	}

// 	/*
// 	 * fast path if desired attribute already cached
// 	 */
// 	if (attnum <= slot->tts_nvalid)
// 		return slot->tts_isnull[attnum - 1];

// 	/*
// 	 * return NULL if attnum is out of range according to the tupdesc
// 	 */
// 	if (attnum > tupleDesc->natts)
// 		return true;

// 	/*
// 	 * otherwise we had better have a physical tuple (tts_nvalid should equal
// 	 * natts in all virtual-tuple cases)
// 	 */
// 	if (tuple == NULL)			/* internal error */
// 		elog(ERROR, "cannot extract attribute from empty tuple slot");

// 	/* and let the tuple tell it */
// 	return heap_attisnull(tuple, attnum);
// }
// //end
// //end tuptable.h

// //fmgr.c
// void
// fmgr_info_cxt(Oid functionId, FmgrInfo *finfo, MemoryContext mcxt)
// {
// 	fmgr_info_cxt_security(functionId, finfo, mcxt, false);
// }
// //end
// void
// fmgr_info_copy(FmgrInfo *dstinfo, FmgrInfo *srcinfo,
// 			   MemoryContext destcxt)
// {
// 	memcpy(dstinfo, srcinfo, sizeof(FmgrInfo));
// 	dstinfo->fn_mcxt = destcxt;
// 	if (dstinfo->fn_addr == fmgr_oldstyle)
// 	{
// 		/* For oldstyle functions we must copy fn_extra */
// 		Oldstyle_fnextra *fnextra;

// 		fnextra = (Oldstyle_fnextra *)
// 			MemoryContextAlloc(destcxt, sizeof(Oldstyle_fnextra));
// 		memcpy(fnextra, srcinfo->fn_extra, sizeof(Oldstyle_fnextra));
// 		dstinfo->fn_extra = (void *) fnextra;
// 	}
// 	else
// 		dstinfo->fn_extra = NULL;
// }
// //end

// struct varlena *
// pg_detoast_datum(struct varlena * datum)
// {
// 	if (VARATT_IS_EXTENDED(datum))
// 		return heap_tuple_untoast_attr(datum);
// 	else
// 		return datum;
// }
// //end
// struct varlena *
// pg_detoast_datum_copy(struct varlena * datum)
// {
// 	if (VARATT_IS_EXTENDED(datum))
// 		return heap_tuple_untoast_attr(datum);
// 	else
// 	{
// 		/* Make a modifiable copy of the varlena object */
// 		Size		len = VARSIZE(datum);
// 		struct varlena *result = (struct varlena *) palloc(len);

// 		memcpy(result, datum, len);
// 		return result;
// 	}
// }
// //end
// struct varlena *
// pg_detoast_datum_slice(struct varlena * datum, int32 first, int32 count)
// {
// 	/* Only get the specified portion from the toast rel */
// 	return heap_tuple_untoast_attr_slice(datum, first, count);
// }
// //end
// struct varlena *
// pg_detoast_datum_packed(struct varlena * datum)
// {
// 	if (VARATT_IS_COMPRESSED(datum) || VARATT_IS_EXTERNAL(datum))
// 		return heap_tuple_untoast_attr(datum);
// 	else
// 		return datum;
// }
// //end
// //end fmgr.c

// //funcapi.c
// TypeFuncClass
// get_expr_result_type(Node *expr,
// 					 Oid *resultTypeId,
// 					 TupleDesc *resultTupleDesc)
// {
// 	TypeFuncClass result;

// 	if (expr && IsA(expr, FuncExpr))
// 		result = internal_get_result_type(((FuncExpr *) expr)->funcid,
// 										  expr,
// 										  NULL,
// 										  resultTypeId,
// 										  resultTupleDesc);
// 	else if (expr && IsA(expr, OpExpr))
// 		result = internal_get_result_type(get_opcode(((OpExpr *) expr)->opno),
// 										  expr,
// 										  NULL,
// 										  resultTypeId,
// 										  resultTupleDesc);
// 	else
// 	{
// 		/* handle as a generic expression; no chance to resolve RECORD */
// 		Oid			typid = exprType(expr);

// 		if (resultTypeId)
// 			*resultTypeId = typid;
// 		if (resultTupleDesc)
// 			*resultTupleDesc = NULL;
// 		result = get_type_func_class(typid);
// 		if (result == TYPEFUNC_COMPOSITE && resultTupleDesc)
// 			*resultTupleDesc = lookup_rowtype_tupdesc_copy(typid, -1);
// 	}

// 	return result;
// }
// //end
// bool
// resolve_polymorphic_argtypes(int numargs, Oid *argtypes, char *argmodes,
// 							 Node *call_expr)
// {
// 	bool		have_anyelement_result = false;
// 	bool		have_anyarray_result = false;
// 	bool		have_anyrange_result = false;
// 	Oid			anyelement_type = InvalidOid;
// 	Oid			anyarray_type = InvalidOid;
// 	Oid			anyrange_type = InvalidOid;
// 	int			inargno;
// 	int			i;

// 	/* First pass: resolve polymorphic inputs, check for outputs */
// 	inargno = 0;
// 	for (i = 0; i < numargs; i++)
// 	{
// 		char		argmode = argmodes ? argmodes[i] : PROARGMODE_IN;

// 		switch (argtypes[i])
// 		{
// 			case ANYELEMENTOID:
// 			case ANYNONARRAYOID:
// 			case ANYENUMOID:
// 				if (argmode == PROARGMODE_OUT || argmode == PROARGMODE_TABLE)
// 					have_anyelement_result = true;
// 				else
// 				{
// 					if (!OidIsValid(anyelement_type))
// 					{
// 						anyelement_type = get_call_expr_argtype(call_expr,
// 																inargno);
// 						if (!OidIsValid(anyelement_type))
// 							return false;
// 					}
// 					argtypes[i] = anyelement_type;
// 				}
// 				break;
// 			case ANYARRAYOID:
// 				if (argmode == PROARGMODE_OUT || argmode == PROARGMODE_TABLE)
// 					have_anyarray_result = true;
// 				else
// 				{
// 					if (!OidIsValid(anyarray_type))
// 					{
// 						anyarray_type = get_call_expr_argtype(call_expr,
// 															  inargno);
// 						if (!OidIsValid(anyarray_type))
// 							return false;
// 					}
// 					argtypes[i] = anyarray_type;
// 				}
// 				break;
// 			case ANYRANGEOID:
// 				if (argmode == PROARGMODE_OUT || argmode == PROARGMODE_TABLE)
// 					have_anyrange_result = true;
// 				else
// 				{
// 					if (!OidIsValid(anyrange_type))
// 					{
// 						anyrange_type = get_call_expr_argtype(call_expr,
// 															  inargno);
// 						if (!OidIsValid(anyrange_type))
// 							return false;
// 					}
// 					argtypes[i] = anyrange_type;
// 				}
// 				break;
// 			default:
// 				break;
// 		}
// 		if (argmode != PROARGMODE_OUT && argmode != PROARGMODE_TABLE)
// 			inargno++;
// 	}

// 	/* Done? */
// 	if (!have_anyelement_result && !have_anyarray_result &&
// 		!have_anyrange_result)
// 		return true;

// 	/* If no input polymorphics, parser messed up */
// 	if (!OidIsValid(anyelement_type) && !OidIsValid(anyarray_type) &&
// 		!OidIsValid(anyrange_type))
// 		return false;

// 	/* If needed, deduce one polymorphic type from others */
// 	if (have_anyelement_result && !OidIsValid(anyelement_type))
// 	{
// 		if (OidIsValid(anyarray_type))
// 			anyelement_type = resolve_generic_type(ANYELEMENTOID,
// 												   anyarray_type,
// 												   ANYARRAYOID);
// 		if (OidIsValid(anyrange_type))
// 		{
// 			Oid			subtype = resolve_generic_type(ANYELEMENTOID,
// 													   anyrange_type,
// 													   ANYRANGEOID);

// 			/* check for inconsistent array and range results */
// 			if (OidIsValid(anyelement_type) && anyelement_type != subtype)
// 				return false;
// 			anyelement_type = subtype;
// 		}
// 	}

// 	if (have_anyarray_result && !OidIsValid(anyarray_type))
// 		anyarray_type = resolve_generic_type(ANYARRAYOID,
// 											 anyelement_type,
// 											 ANYELEMENTOID);

// 	/*
// 	 * We can't deduce a range type from other polymorphic inputs, because
// 	 * there may be multiple range types for the same subtype.
// 	 */
// 	if (have_anyrange_result && !OidIsValid(anyrange_type))
// 		return false;

// 	/* XXX do we need to enforce ANYNONARRAY or ANYENUM here?  I think not */

// 	/* And finally replace the output column types as needed */
// 	for (i = 0; i < numargs; i++)
// 	{
// 		switch (argtypes[i])
// 		{
// 			case ANYELEMENTOID:
// 			case ANYNONARRAYOID:
// 			case ANYENUMOID:
// 				argtypes[i] = anyelement_type;
// 				break;
// 			case ANYARRAYOID:
// 				argtypes[i] = anyarray_type;
// 				break;
// 			case ANYRANGEOID:
// 				argtypes[i] = anyrange_type;
// 				break;
// 			default:
// 				break;
// 		}
// 	}

// 	return true;
// }
// //end
// TupleDesc
// build_function_result_tupdesc_d(Datum proallargtypes,
// 								Datum proargmodes,
// 								Datum proargnames)
// {
// 	TupleDesc	desc;
// 	ArrayType  *arr;
// 	int			numargs;
// 	Oid		   *argtypes;
// 	char	   *argmodes;
// 	Datum	   *argnames = NULL;
// 	Oid		   *outargtypes;
// 	char	  **outargnames;
// 	int			numoutargs;
// 	int			nargnames;
// 	int			i;

// 	/* Can't have output args if columns are null */
// 	if (proallargtypes == PointerGetDatum(NULL) ||
// 		proargmodes == PointerGetDatum(NULL))
// 		return NULL;

// 	/*
// 	 * We expect the arrays to be 1-D arrays of the right types; verify that.
// 	 * For the OID and char arrays, we don't need to use deconstruct_array()
// 	 * since the array data is just going to look like a C array of values.
// 	 */
// 	arr = DatumGetArrayTypeP(proallargtypes);	/* ensure not toasted */
// 	numargs = ARR_DIMS(arr)[0];
// 	if (ARR_NDIM(arr) != 1 ||
// 		numargs < 0 ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != OIDOID)
// 		elog(ERROR, "proallargtypes is not a 1-D Oid array");
// 	argtypes = (Oid *) ARR_DATA_PTR(arr);
// 	arr = DatumGetArrayTypeP(proargmodes);		/* ensure not toasted */
// 	if (ARR_NDIM(arr) != 1 ||
// 		ARR_DIMS(arr)[0] != numargs ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != CHAROID)
// 		elog(ERROR, "proargmodes is not a 1-D char array");
// 	argmodes = (char *) ARR_DATA_PTR(arr);
// 	if (proargnames != PointerGetDatum(NULL))
// 	{
// 		arr = DatumGetArrayTypeP(proargnames);	/* ensure not toasted */
// 		if (ARR_NDIM(arr) != 1 ||
// 			ARR_DIMS(arr)[0] != numargs ||
// 			ARR_HASNULL(arr) ||
// 			ARR_ELEMTYPE(arr) != TEXTOID)
// 			elog(ERROR, "proargnames is not a 1-D text array");
// 		deconstruct_array(arr, TEXTOID, -1, false, 'i',
// 						  &argnames, NULL, &nargnames);
// 		Assert(nargnames == numargs);
// 	}

// 	/* zero elements probably shouldn't happen, but handle it gracefully */
// 	if (numargs <= 0)
// 		return NULL;

// 	/* extract output-argument types and names */
// 	outargtypes = (Oid *) palloc(numargs * sizeof(Oid));
// 	outargnames = (char **) palloc(numargs * sizeof(char *));
// 	numoutargs = 0;
// 	for (i = 0; i < numargs; i++)
// 	{
// 		char	   *pname;

// 		if (argmodes[i] == PROARGMODE_IN ||
// 			argmodes[i] == PROARGMODE_VARIADIC)
// 			continue;
// 		Assert(argmodes[i] == PROARGMODE_OUT ||
// 			   argmodes[i] == PROARGMODE_INOUT ||
// 			   argmodes[i] == PROARGMODE_TABLE);
// 		outargtypes[numoutargs] = argtypes[i];
// 		if (argnames)
// 			pname = TextDatumGetCString(argnames[i]);
// 		else
// 			pname = NULL;
// 		if (pname == NULL || pname[0] == '\0')
// 		{
// 			/* Parameter is not named, so gin up a column name */
// 			pname = psprintf("column%d", numoutargs + 1);
// 		}
// 		outargnames[numoutargs] = pname;
// 		numoutargs++;
// 	}

// 	/*
// 	 * If there is no output argument, or only one, the function does not
// 	 * return tuples.
// 	 */
// 	if (numoutargs < 2)
// 		return NULL;

// 	desc = CreateTemplateTupleDesc(numoutargs, false);
// 	for (i = 0; i < numoutargs; i++)
// 	{
// 		TupleDescInitEntry(desc, i + 1,
// 						   outargnames[i],
// 						   outargtypes[i],
// 						   -1,
// 						   0);
// 	}

// 	return desc;
// }
// //end
// int
// get_func_trftypes(HeapTuple procTup,
// 				  Oid **p_trftypes)
// {
// 	Datum		protrftypes;
// 	ArrayType  *arr;
// 	int			nelems;
// 	bool		isNull;

// 	protrftypes = SysCacheGetAttr(PROCOID, procTup,
// 								  Anum_pg_proc_protrftypes,
// 								  &isNull);
// 	if (!isNull)
// 	{
// 		/*
// 		 * We expect the arrays to be 1-D arrays of the right types; verify
// 		 * that.  For the OID and char arrays, we don't need to use
// 		 * deconstruct_array() since the array data is just going to look like
// 		 * a C array of values.
// 		 */
// 		arr = DatumGetArrayTypeP(protrftypes);	/* ensure not toasted */
// 		nelems = ARR_DIMS(arr)[0];
// 		if (ARR_NDIM(arr) != 1 ||
// 			nelems < 0 ||
// 			ARR_HASNULL(arr) ||
// 			ARR_ELEMTYPE(arr) != OIDOID)
// 			elog(ERROR, "protrftypes is not a 1-D Oid array");
// 		Assert(nelems >= ((Form_pg_proc) GETSTRUCT(procTup))->pronargs);
// 		*p_trftypes = (Oid *) palloc(nelems * sizeof(Oid));
// 		memcpy(*p_trftypes, ARR_DATA_PTR(arr),
// 			   nelems * sizeof(Oid));

// 		return nelems;
// 	}
// 	else
// 		return 0;
// }
// //end

// /* from execTuples.c */
// Datum
// HeapTupleHeaderGetDatum(HeapTupleHeader tuple)
// {
// 	Datum		result;
// 	TupleDesc	tupDesc;

// 	/* No work if there are no external TOAST pointers in the tuple */
// 	if (!HeapTupleHeaderHasExternal(tuple))
// 		return PointerGetDatum(tuple);

// 	/* Use the type data saved by heap_form_tuple to look up the rowtype */
// 	tupDesc = lookup_rowtype_tupdesc(HeapTupleHeaderGetTypeId(tuple),
// 									 HeapTupleHeaderGetTypMod(tuple));

// 	/* And do the flattening */
// 	result = toast_flatten_tuple_to_datum(tuple,
// 										HeapTupleHeaderGetDatumLength(tuple),
// 										  tupDesc);

// 	ReleaseTupleDesc(tupDesc);

// 	return result;
// }
// //end
// //end funcapi.c

// //新文件目录
// // stringinfo.c
// int
// appendStringInfoVA(StringInfo str, const char *fmt, va_list args)
// {
// 	int			avail;
// 	size_t		nprinted;

// 	Assert(str != NULL);

// 	/*
// 	 * If there's hardly any space, don't bother trying, just fail to make the
// 	 * caller enlarge the buffer first.  We have to guess at how much to
// 	 * enlarge, since we're skipping the formatting work.
// 	 */
// 	avail = str->maxlen - str->len;
// 	if (avail < 16)
// 		return 32;

// 	nprinted = pvsnprintf(str->data + str->len, (size_t) avail, fmt, args);

// 	if (nprinted < (size_t) avail)
// 	{
// 		/* Success.  Note nprinted does not include trailing null. */
// 		str->len += (int) nprinted;
// 		return 0;
// 	}

// 	/* Restore the trailing null so that str is unmodified. */
// 	str->data[str->len] = '\0';

// 	/*
// 	 * Return pvsnprintf's estimate of the space needed.  (Although this is
// 	 * given as a size_t, we know it will fit in int because it's not more
// 	 * than MaxAllocSize.)
// 	 */
// 	return (int) nprinted;
// }
// //end
// //end stringinfo.c

// //新文件目录
// //lib/stringinfo.c
// const char *
// pq_getmsgrawstring(StringInfo msg)
// {
// 	char	   *str;
// 	int			slen;

// 	str = &msg->data[msg->cursor];

// 	/*
// 	 * It's safe to use strlen() here because a StringInfo is guaranteed to
// 	 * have a trailing null byte.  But check we found a null inside the
// 	 * message.
// 	 */
// 	slen = strlen(str);
// 	if (msg->cursor + slen >= msg->len)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 				 errmsg("invalid string in message")));
// 	msg->cursor += slen + 1;

// 	return str;
// }
// //end
// void
// pq_sendbyte(StringInfo buf, int byt)
// {
// 	appendStringInfoCharMacro(buf, byt);
// }
// //end
// void
// pq_sendint(StringInfo buf, int i, int b)
// {
// 	unsigned char n8;
// 	uint16		n16;
// 	uint32		n32;

// 	switch (b)
// 	{
// 		case 1:
// 			n8 = (unsigned char) i;
// 			appendBinaryStringInfo(buf, (char *) &n8, 1);
// 			break;
// 		case 2:
// 			n16 = htons((uint16) i);
// 			appendBinaryStringInfo(buf, (char *) &n16, 2);
// 			break;
// 		case 4:
// 			n32 = htonl((uint32) i);
// 			appendBinaryStringInfo(buf, (char *) &n32, 4);
// 			break;
// 		default:
// 			elog(ERROR, "unsupported integer size %d", b);
// 			break;
// 	}
// }
// //end
// void
// pq_sendint64(StringInfo buf, int64 i)
// {
// 	uint32		n32;

// 	/* High order half first, since we're doing MSB-first */
// 	n32 = (uint32) (i >> 32);
// 	n32 = htonl(n32);
// 	appendBinaryStringInfo(buf, (char *) &n32, 4);

// 	/* Now the low order half */
// 	n32 = (uint32) i;
// 	n32 = htonl(n32);
// 	appendBinaryStringInfo(buf, (char *) &n32, 4);
// }
// //end
// //end pqformat.c

// //miscadmin.h
// /* now in utils/init/miscinit.c */
// char *
// GetUserNameFromId(Oid roleid, bool noerr)
// {
// 	HeapTuple	tuple;
// 	char	   *result;

// 	tuple = SearchSysCache1(AUTHOID, ObjectIdGetDatum(roleid));
// 	if (!HeapTupleIsValid(tuple))
// 	{
// 		if (!noerr)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 					 errmsg("invalid role OID: %u", roleid)));
// 		result = NULL;
// 	}
// 	else
// 	{
// 		result = pstrdup(NameStr(((Form_pg_authid) GETSTRUCT(tuple))->rolname));
// 		ReleaseSysCache(tuple);
// 	}
// 	return result;
// }
// //end
// void
// InitializeSessionUserId(const char *rolename, Oid roleid)
// {
// 	HeapTuple	roleTup;
// 	Form_pg_authid rform;
// 	char	   *rname;

// 	/*
// 	 * Don't do scans if we're bootstrapping, none of the system catalogs
// 	 * exist yet, and they should be owned by postgres anyway.
// 	 */
// 	AssertState(!IsBootstrapProcessingMode());

// 	/* call only once */
// 	AssertState(!OidIsValid(AuthenticatedUserId));

// 	if (rolename != NULL)
// 	{
// 		roleTup = SearchSysCache1(AUTHNAME, PointerGetDatum(rolename));
// 		if (!HeapTupleIsValid(roleTup))
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
// 					 errmsg("role \"%s\" does not exist", rolename)));
// 	}
// 	else
// 	{
// 		roleTup = SearchSysCache1(AUTHOID, ObjectIdGetDatum(roleid));
// 		if (!HeapTupleIsValid(roleTup))
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
// 					 errmsg("role with OID %u does not exist", roleid)));
// 	}

// 	rform = (Form_pg_authid) GETSTRUCT(roleTup);
// 	roleid = HeapTupleGetOid(roleTup);
// 	rname = NameStr(rform->rolname);

// 	AuthenticatedUserId = roleid;
// 	AuthenticatedUserIsSuperuser = rform->rolsuper;

// 	/* This sets OuterUserId/CurrentUserId too */
// 	SetSessionUserId(roleid, AuthenticatedUserIsSuperuser);

// 	/* Also mark our PGPROC entry with the authenticated user id */
// 	/* (We assume this is an atomic store so no lock is needed) */
// 	MyProc->roleId = roleid;

// 	/*
// 	 * These next checks are not enforced when in standalone mode, so that
// 	 * there is a way to recover from sillinesses like "UPDATE pg_authid SET
// 	 * rolcanlogin = false;".
// 	 */
// 	if (IsUnderPostmaster)
// 	{
// 		/*
// 		 * Is role allowed to login at all?
// 		 */
// 		if (!rform->rolcanlogin)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
// 					 errmsg("role \"%s\" is not permitted to log in",
// 							rname)));

// 		/*
// 		 * Check connection limit for this role.
// 		 *
// 		 * There is a race condition here --- we create our PGPROC before
// 		 * checking for other PGPROCs.  If two backends did this at about the
// 		 * same time, they might both think they were over the limit, while
// 		 * ideally one should succeed and one fail.  Getting that to work
// 		 * exactly seems more trouble than it is worth, however; instead we
// 		 * just document that the connection limit is approximate.
// 		 */
// 		if (rform->rolconnlimit >= 0 &&
// 			!AuthenticatedUserIsSuperuser &&
// 			CountUserBackends(roleid) > rform->rolconnlimit)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_TOO_MANY_CONNECTIONS),
// 					 errmsg("too many connections for role \"%s\"",
// 							rname)));
// 	}

// 	/* Record username and superuser status as GUC settings too */
// 	SetConfigOption("session_authorization", rname,
// 					PGC_BACKEND, PGC_S_OVERRIDE);
// 	SetConfigOption("is_superuser",
// 					AuthenticatedUserIsSuperuser ? "on" : "off",
// 					PGC_INTERNAL, PGC_S_OVERRIDE);

// 	ReleaseSysCache(roleTup);
// }
// //end
// void
// CreateSocketLockFile(const char *socketfile, bool amPostmaster,
// 					 const char *socketDir)
// {
// 	char		lockfile[MAXPGPATH];

// 	snprintf(lockfile, sizeof(lockfile), "%s.lock", socketfile);
// 	CreateLockFile(lockfile, amPostmaster, socketDir, false, socketfile);
// }
// //end
// void
// InitPostmasterChild(void)
// {
// 	IsUnderPostmaster = true;	/* we are a postmaster subprocess now */

// 	MyProcPid = getpid();		/* reset MyProcPid */

// 	MyStartTime = time(NULL);	/* set our start time in case we call elog */

// 	/*
// 	 * make sure stderr is in binary mode before anything can possibly be
// 	 * written to it, in case it's actually the syslogger pipe, so the pipe
// 	 * chunking protocol isn't disturbed. Non-logpipe data gets translated on
// 	 * redirection (e.g. via pg_ctl -l) anyway.
// 	 */
// #ifdef WIN32
// 	_setmode(fileno(stderr), _O_BINARY);
// #endif

// 	/* We don't want the postmaster's proc_exit() handlers */
// 	on_exit_reset();

// 	/* Initialize process-local latch support */
// 	InitializeLatchSupport();
// 	MyLatch = &LocalLatchData;
// 	InitLatch(MyLatch);

// 	/*
// 	 * If possible, make this process a group leader, so that the postmaster
// 	 * can signal any child processes too. Not all processes will have
// 	 * children, but for consistency we make all postmaster child processes do
// 	 * this.
// 	 */
// #ifdef HAVE_SETSID
// 	if (setsid() < 0)
// 		elog(FATAL, "setsid() failed: %m");
// #endif
// }
// //end
// void
// InitStandaloneProcess(const char *argv0)
// {
// 	Assert(!IsPostmasterEnvironment);

// 	MyProcPid = getpid();		/* reset MyProcPid */

// 	MyStartTime = time(NULL);	/* set our start time in case we call elog */

// 	/* Initialize process-local latch support */
// 	InitializeLatchSupport();
// 	MyLatch = &LocalLatchData;
// 	InitLatch(MyLatch);

// 	/* Compute paths, no postmaster to inherit from */
// 	if (my_exec_path[0] == '\0')
// 	{
// 		if (find_my_exec(argv0, my_exec_path) < 0)
// 			elog(FATAL, "%s: could not locate my own executable path",
// 				 argv0);
// 	}

// 	if (pkglib_path[0] == '\0')
// 		get_pkglib_path(my_exec_path, pkglib_path);
// }
// //end
// void
// SetDatabasePath(const char *path)
// {
// 	/* This should happen only once per process */
// 	Assert(!DatabasePath);
// 	DatabasePath = MemoryContextStrdup(TopMemoryContext, path);
// }
// //end
// bool
// InNoForceRLSOperation(void)
// {
// 	return (SecurityRestrictionContext & SECURITY_NOFORCE_RLS) != 0;
// }
// //end
// void
// SwitchToSharedLatch(void)
// {
// 	Assert(MyLatch == &LocalLatchData);
// 	Assert(MyProc != NULL);

// 	MyLatch = &MyProc->procLatch;

// 	if (FeBeWaitSet)
// 		ModifyWaitEvent(FeBeWaitSet, 1, WL_LATCH_SET, MyLatch);

// 	/*
// 	 * Set the shared latch as the local one might have been set. This
// 	 * shouldn't normally be necessary as code is supposed to check the
// 	 * condition before waiting for the latch, but a bit care can't hurt.
// 	 */
// 	SetLatch(MyLatch);
// }
// //end
// void
// SwitchBackToLocalLatch(void)
// {
// 	Assert(MyLatch != &LocalLatchData);
// 	Assert(MyProc != NULL && MyLatch == &MyProc->procLatch);

// 	MyLatch = &LocalLatchData;

// 	if (FeBeWaitSet)
// 		ModifyWaitEvent(FeBeWaitSet, 1, WL_LATCH_SET, MyLatch);

// 	SetLatch(MyLatch);
// }
// //end

// void
// TouchSocketLockFiles(void)
// {
// 	ListCell   *l;

// 	foreach(l, lock_files)
// 	{
// 		char	   *socketLockFile = (char *) lfirst(l);

// 		/* No need to touch the data directory lock file, we trust */
// 		if (strcmp(socketLockFile, DIRECTORY_LOCK_FILE) == 0)
// 			continue;

// 		/*
// 		 * utime() is POSIX standard, utimes() is a common alternative; if we
// 		 * have neither, fall back to actually reading the file (which only
// 		 * sets the access time not mod time, but that should be enough in
// 		 * most cases).  In all paths, we ignore errors.
// 		 */
// #ifdef HAVE_UTIME
// 		utime(socketLockFile, NULL);
// #else							/* !HAVE_UTIME */
// #ifdef HAVE_UTIMES
// 		utimes(socketLockFile, NULL);
// #else							/* !HAVE_UTIMES */
// 		int			fd;
// 		char		buffer[1];

// 		fd = open(socketLockFile, O_RDONLY | PG_BINARY, 0);
// 		if (fd >= 0)
// 		{
// 			read(fd, buffer, sizeof(buffer));
// 			close(fd);
// 		}
// #endif   /* HAVE_UTIMES */
// #endif   /* HAVE_UTIME */
// 	}
// }
// //end
// bool
// RecheckDataDirLockFile(void)
// {
// 	int			fd;
// 	int			len;
// 	long		file_pid;
// 	char		buffer[BLCKSZ];

// 	fd = open(DIRECTORY_LOCK_FILE, O_RDWR | PG_BINARY, 0);
// 	if (fd < 0)
// 	{
// 		/*
// 		 * There are many foreseeable false-positive error conditions.  For
// 		 * safety, fail only on enumerated clearly-something-is-wrong
// 		 * conditions.
// 		 */
// 		switch (errno)
// 		{
// 			case ENOENT:
// 			case ENOTDIR:
// 				/* disaster */
// 				ereport(LOG,
// 						(errcode_for_file_access(),
// 						 errmsg("could not open file \"%s\": %m",
// 								DIRECTORY_LOCK_FILE)));
// 				return false;
// 			default:
// 				/* non-fatal, at least for now */
// 				ereport(LOG,
// 						(errcode_for_file_access(),
// 				  errmsg("could not open file \"%s\": %m; continuing anyway",
// 						 DIRECTORY_LOCK_FILE)));
// 				return true;
// 		}
// 	}
// 	len = read(fd, buffer, sizeof(buffer) - 1);
// 	if (len < 0)
// 	{
// 		ereport(LOG,
// 				(errcode_for_file_access(),
// 				 errmsg("could not read from file \"%s\": %m",
// 						DIRECTORY_LOCK_FILE)));
// 		close(fd);
// 		return true;			/* treat read failure as nonfatal */
// 	}
// 	buffer[len] = '\0';
// 	close(fd);
// 	file_pid = atol(buffer);
// 	if (file_pid == getpid())
// 		return true;			/* all is well */

// 	/* Trouble: someone's overwritten the lock file */
// 	ereport(LOG,
// 			(errmsg("lock file \"%s\" contains wrong PID: %ld instead of %ld",
// 					DIRECTORY_LOCK_FILE, file_pid, (long) getpid())));
// 	return false;
// }
// //end

// void
// process_session_preload_libraries(void)
// {
// 	load_libraries(session_preload_libraries_string,
// 				   "session_preload_libraries",
// 				   false);
// 	load_libraries(local_preload_libraries_string,
// 				   "local_preload_libraries",
// 				   true);
// }
// //end

// /* in utils/init/postinit.c */
// void
// pg_split_opts(char **argv, int *argcp, const char *optstr)
// {
// 	StringInfoData s;

// 	initStringInfo(&s);

// 	while (*optstr)
// 	{
// 		bool		last_was_escape = false;

// 		resetStringInfo(&s);

// 		/* skip over leading space */
// 		while (isspace((unsigned char) *optstr))
// 			optstr++;

// 		if (*optstr == '\0')
// 			break;

// 		/*
// 		 * Parse a single option, stopping at the first space, unless it's
// 		 * escaped.
// 		 */
// 		while (*optstr)
// 		{
// 			if (isspace((unsigned char) *optstr) && !last_was_escape)
// 				break;

// 			if (!last_was_escape && *optstr == '\\')
// 				last_was_escape = true;
// 			else
// 			{
// 				last_was_escape = false;
// 				appendStringInfoChar(&s, *optstr);
// 			}

// 			optstr++;
// 		}

// 		/* now store the option in the next argv[] position */
// 		argv[(*argcp)++] = pstrdup(s.data);
// 	}

// 	pfree(s.data);
// }
// //end
// void
// InitializeMaxBackends(void)
// {
// 	Assert(MaxBackends == 0);

// 	/* the extra unit accounts for the autovacuum launcher */
// 	MaxBackends = MaxConnections + autovacuum_max_workers + 1 +
// 		max_worker_processes;

// 	/* internal error because the values were all checked previously */
// 	if (MaxBackends > MAX_BACKENDS)
// 		elog(ERROR, "too many backends configured");
// }
// //end

// void
// InitPostgres(const char *in_dbname, Oid dboid, const char *username,
// 			 Oid useroid, char *out_dbname)
// {
// 	bool		bootstrap = IsBootstrapProcessingMode();
// 	bool		am_superuser;
// 	char	   *fullpath;
// 	char		dbname[NAMEDATALEN];

// 	elog(DEBUG3, "InitPostgres");

// 	/*
// 	 * Add my PGPROC struct to the ProcArray.
// 	 *
// 	 * Once I have done this, I am visible to other backends!
// 	 */
// 	InitProcessPhase2();

// 	/*
// 	 * Initialize my entry in the shared-invalidation manager's array of
// 	 * per-backend data.
// 	 *
// 	 * Sets up MyBackendId, a unique backend identifier.
// 	 */
// 	MyBackendId = InvalidBackendId;

// 	SharedInvalBackendInit(false);

// 	if (MyBackendId > MaxBackends || MyBackendId <= 0)
// 		elog(FATAL, "bad backend ID: %d", MyBackendId);

// 	/* Now that we have a BackendId, we can participate in ProcSignal */
// 	ProcSignalInit(MyBackendId);

// 	/*
// 	 * Also set up timeout handlers needed for backend operation.  We need
// 	 * these in every case except bootstrap.
// 	 */
// 	if (!bootstrap)
// 	{
// 		RegisterTimeout(DEADLOCK_TIMEOUT, CheckDeadLockAlert);
// 		RegisterTimeout(STATEMENT_TIMEOUT, StatementTimeoutHandler);
// 		RegisterTimeout(LOCK_TIMEOUT, LockTimeoutHandler);
// 		RegisterTimeout(IDLE_IN_TRANSACTION_SESSION_TIMEOUT,
// 						IdleInTransactionSessionTimeoutHandler);
// 	}

// 	/*
// 	 * bufmgr needs another initialization call too
// 	 */
// 	InitBufferPoolBackend();

// 	/*
// 	 * Initialize local process's access to XLOG.
// 	 */
// 	if (IsUnderPostmaster)
// 	{
// 		/*
// 		 * The postmaster already started the XLOG machinery, but we need to
// 		 * call InitXLOGAccess(), if the system isn't in hot-standby mode.
// 		 * This is handled by calling RecoveryInProgress and ignoring the
// 		 * result.
// 		 */
// 		(void) RecoveryInProgress();
// 	}
// 	else
// 	{
// 		/*
// 		 * We are either a bootstrap process or a standalone backend. Either
// 		 * way, start up the XLOG machinery, and register to have it closed
// 		 * down at exit.
// 		 */
// 		StartupXLOG();
// 		on_shmem_exit(ShutdownXLOG, 0);
// 	}

// 	/*
// 	 * Initialize the relation cache and the system catalog caches.  Note that
// 	 * no catalog access happens here; we only set up the hashtable structure.
// 	 * We must do this before starting a transaction because transaction abort
// 	 * would try to touch these hashtables.
// 	 */
// 	RelationCacheInitialize();
// 	InitCatalogCache();
// 	InitPlanCache();

// 	/* Initialize portal manager */
// 	EnablePortalManager();

// 	/* Initialize stats collection --- must happen before first xact */
// 	if (!bootstrap)
// 		pgstat_initialize();

// 	/*
// 	 * Load relcache entries for the shared system catalogs.  This must create
// 	 * at least entries for pg_database and catalogs used for authentication.
// 	 */
// 	RelationCacheInitializePhase2();

// 	/*
// 	 * Set up process-exit callback to do pre-shutdown cleanup.  This is the
// 	 * first before_shmem_exit callback we register; thus, this will be the
// 	 * last thing we do before low-level modules like the buffer manager begin
// 	 * to close down.  We need to have this in place before we begin our first
// 	 * transaction --- if we fail during the initialization transaction, as is
// 	 * entirely possible, we need the AbortTransaction call to clean up.
// 	 */
// 	before_shmem_exit(ShutdownPostgres, 0);

// 	/* The autovacuum launcher is done here */
// 	if (IsAutoVacuumLauncherProcess())
// 		return;

// 	/*
// 	 * Start a new transaction here before first access to db, and get a
// 	 * snapshot.  We don't have a use for the snapshot itself, but we're
// 	 * interested in the secondary effect that it sets RecentGlobalXmin. (This
// 	 * is critical for anything that reads heap pages, because HOT may decide
// 	 * to prune them even if the process doesn't attempt to modify any
// 	 * tuples.)
// 	 */
// 	if (!bootstrap)
// 	{
// 		/* statement_timestamp must be set for timeouts to work correctly */
// 		SetCurrentStatementStartTimestamp();
// 		StartTransactionCommand();

// 		/*
// 		 * transaction_isolation will have been set to the default by the
// 		 * above.  If the default is "serializable", and we are in hot
// 		 * standby, we will fail if we don't change it to something lower.
// 		 * Fortunately, "read committed" is plenty good enough.
// 		 */
// 		XactIsoLevel = XACT_READ_COMMITTED;

// 		(void) GetTransactionSnapshot();
// 	}

// 	/*
// 	 * Perform client authentication if necessary, then figure out our
// 	 * postgres user ID, and see if we are a superuser.
// 	 *
// 	 * In standalone mode and in autovacuum worker processes, we use a fixed
// 	 * ID, otherwise we figure it out from the authenticated user name.
// 	 */
// 	if (bootstrap || IsAutoVacuumWorkerProcess())
// 	{
// 		InitializeSessionUserIdStandalone();
// 		am_superuser = true;
// 	}
// 	else if (!IsUnderPostmaster)
// 	{
// 		InitializeSessionUserIdStandalone();
// 		am_superuser = true;
// 		if (!ThereIsAtLeastOneRole())
// 			ereport(WARNING,
// 					(errcode(ERRCODE_UNDEFINED_OBJECT),
// 					 errmsg("no roles are defined in this database system"),
// 					 errhint("You should immediately run CREATE USER \"%s\" SUPERUSER;.",
// 							 username != NULL ? username : "postgres")));
// 	}
// 	else if (IsBackgroundWorker)
// 	{
// 		if (username == NULL && !OidIsValid(useroid))
// 		{
// 			InitializeSessionUserIdStandalone();
// 			am_superuser = true;
// 		}
// 		else
// 		{
// 			InitializeSessionUserId(username, useroid);
// 			am_superuser = superuser();
// 		}
// 	}
// 	else
// 	{
// 		/* normal multiuser case */
// 		Assert(MyProcPort != NULL);
// 		PerformAuthentication(MyProcPort);
// 		InitializeSessionUserId(username, useroid);
// 		am_superuser = superuser();
// 	}

// 	/*
// 	 * If we're trying to shut down, only superusers can connect, and new
// 	 * replication connections are not allowed.
// 	 */
// 	if ((!am_superuser || am_walsender) &&
// 		MyProcPort != NULL &&
// 		MyProcPort->canAcceptConnections == CAC_WAITBACKUP)
// 	{
// 		if (am_walsender)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 					 errmsg("new replication connections are not allowed during database shutdown")));
// 		else
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 			errmsg("must be superuser to connect during database shutdown")));
// 	}

// 	/*
// 	 * Binary upgrades only allowed super-user connections
// 	 */
// 	if (IsBinaryUpgrade && !am_superuser)
// 	{
// 		ereport(FATAL,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 			 errmsg("must be superuser to connect in binary upgrade mode")));
// 	}

// 	/*
// 	 * The last few connections slots are reserved for superusers. Although
// 	 * replication connections currently require superuser privileges, we
// 	 * don't allow them to consume the reserved slots, which are intended for
// 	 * interactive use.
// 	 */
// 	if ((!am_superuser || am_walsender) &&
// 		ReservedBackends > 0 &&
// 		!HaveNFreeProcs(ReservedBackends))
// 		ereport(FATAL,
// 				(errcode(ERRCODE_TOO_MANY_CONNECTIONS),
// 				 errmsg("remaining connection slots are reserved for non-replication superuser connections")));

// 	/* Check replication permissions needed for walsender processes. */
// 	if (am_walsender)
// 	{
// 		Assert(!bootstrap);

// 		if (!superuser() && !has_rolreplication(GetUserId()))
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 					 errmsg("must be superuser or replication role to start walsender")));
// 	}

// 	/*
// 	 * If this is a plain walsender only supporting physical replication, we
// 	 * don't want to connect to any particular database. Just finish the
// 	 * backend startup by processing any options from the startup packet, and
// 	 * we're done.
// 	 */
// 	if (am_walsender && !am_db_walsender)
// 	{
// 		/* process any options passed in the startup packet */
// 		if (MyProcPort != NULL)
// 			process_startup_options(MyProcPort, am_superuser);

// 		/* Apply PostAuthDelay as soon as we've read all options */
// 		if (PostAuthDelay > 0)
// 			pg_usleep(PostAuthDelay * 1000000L);

// 		/* initialize client encoding */
// 		InitializeClientEncoding();

// 		/* report this backend in the PgBackendStatus array */
// 		pgstat_bestart();

// 		/* close the transaction we started above */
// 		CommitTransactionCommand();

// 		return;
// 	}

// 	/*
// 	 * Set up the global variables holding database id and default tablespace.
// 	 * But note we won't actually try to touch the database just yet.
// 	 *
// 	 * We take a shortcut in the bootstrap case, otherwise we have to look up
// 	 * the db's entry in pg_database.
// 	 */
// 	if (bootstrap)
// 	{
// 		MyDatabaseId = TemplateDbOid;
// 		MyDatabaseTableSpace = DEFAULTTABLESPACE_OID;
// 	}
// 	else if (in_dbname != NULL)
// 	{
// 		HeapTuple	tuple;
// 		Form_pg_database dbform;

// 		tuple = GetDatabaseTuple(in_dbname);
// 		if (!HeapTupleIsValid(tuple))
// 			ereport(FATAL,
// 					(errcode(ERRCODE_UNDEFINED_DATABASE),
// 					 errmsg("database \"%s\" does not exist", in_dbname)));
// 		dbform = (Form_pg_database) GETSTRUCT(tuple);
// 		MyDatabaseId = HeapTupleGetOid(tuple);
// 		MyDatabaseTableSpace = dbform->dattablespace;
// 		/* take database name from the caller, just for paranoia */
// 		strlcpy(dbname, in_dbname, sizeof(dbname));
// 	}
// 	else if (OidIsValid(dboid))
// 	{
// 		/* caller specified database by OID */
// 		HeapTuple	tuple;
// 		Form_pg_database dbform;

// 		tuple = GetDatabaseTupleByOid(dboid);
// 		if (!HeapTupleIsValid(tuple))
// 			ereport(FATAL,
// 					(errcode(ERRCODE_UNDEFINED_DATABASE),
// 					 errmsg("database %u does not exist", dboid)));
// 		dbform = (Form_pg_database) GETSTRUCT(tuple);
// 		MyDatabaseId = HeapTupleGetOid(tuple);
// 		MyDatabaseTableSpace = dbform->dattablespace;
// 		Assert(MyDatabaseId == dboid);
// 		strlcpy(dbname, NameStr(dbform->datname), sizeof(dbname));
// 		/* pass the database name back to the caller */
// 		if (out_dbname)
// 			strcpy(out_dbname, dbname);
// 	}
// 	else
// 	{
// 		/*
// 		 * If this is a background worker not bound to any particular
// 		 * database, we're done now.  Everything that follows only makes sense
// 		 * if we are bound to a specific database.  We do need to close the
// 		 * transaction we started before returning.
// 		 */
// 		if (!bootstrap)
// 			CommitTransactionCommand();
// 		return;
// 	}

// 	/*
// 	 * Now, take a writer's lock on the database we are trying to connect to.
// 	 * If there is a concurrently running DROP DATABASE on that database, this
// 	 * will block us until it finishes (and has committed its update of
// 	 * pg_database).
// 	 *
// 	 * Note that the lock is not held long, only until the end of this startup
// 	 * transaction.  This is OK since we will advertise our use of the
// 	 * database in the ProcArray before dropping the lock (in fact, that's the
// 	 * next thing to do).  Anyone trying a DROP DATABASE after this point will
// 	 * see us in the array once they have the lock.  Ordering is important for
// 	 * this because we don't want to advertise ourselves as being in this
// 	 * database until we have the lock; otherwise we create what amounts to a
// 	 * deadlock with CountOtherDBBackends().
// 	 *
// 	 * Note: use of RowExclusiveLock here is reasonable because we envision
// 	 * our session as being a concurrent writer of the database.  If we had a
// 	 * way of declaring a session as being guaranteed-read-only, we could use
// 	 * AccessShareLock for such sessions and thereby not conflict against
// 	 * CREATE DATABASE.
// 	 */
// 	if (!bootstrap)
// 		LockSharedObject(DatabaseRelationId, MyDatabaseId, 0,
// 						 RowExclusiveLock);

// 	/*
// 	 * Now we can mark our PGPROC entry with the database ID.
// 	 *
// 	 * We assume this is an atomic store so no lock is needed; though actually
// 	 * things would work fine even if it weren't atomic.  Anyone searching the
// 	 * ProcArray for this database's ID should hold the database lock, so they
// 	 * would not be executing concurrently with this store.  A process looking
// 	 * for another database's ID could in theory see a chance match if it read
// 	 * a partially-updated databaseId value; but as long as all such searches
// 	 * wait and retry, as in CountOtherDBBackends(), they will certainly see
// 	 * the correct value on their next try.
// 	 */
// 	MyProc->databaseId = MyDatabaseId;

// 	/*
// 	 * We established a catalog snapshot while reading pg_authid and/or
// 	 * pg_database; but until we have set up MyDatabaseId, we won't react to
// 	 * incoming sinval messages for unshared catalogs, so we won't realize it
// 	 * if the snapshot has been invalidated.  Assume it's no good anymore.
// 	 */
// 	InvalidateCatalogSnapshot();

// 	/*
// 	 * Recheck pg_database to make sure the target database hasn't gone away.
// 	 * If there was a concurrent DROP DATABASE, this ensures we will die
// 	 * cleanly without creating a mess.
// 	 */
// 	if (!bootstrap)
// 	{
// 		HeapTuple	tuple;

// 		tuple = GetDatabaseTuple(dbname);
// 		if (!HeapTupleIsValid(tuple) ||
// 			MyDatabaseId != HeapTupleGetOid(tuple) ||
// 			MyDatabaseTableSpace != ((Form_pg_database) GETSTRUCT(tuple))->dattablespace)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_UNDEFINED_DATABASE),
// 					 errmsg("database \"%s\" does not exist", dbname),
// 			   errdetail("It seems to have just been dropped or renamed.")));
// 	}

// 	/*
// 	 * Now we should be able to access the database directory safely. Verify
// 	 * it's there and looks reasonable.
// 	 */
// 	fullpath = GetDatabasePath(MyDatabaseId, MyDatabaseTableSpace);

// 	if (!bootstrap)
// 	{
// 		if (access(fullpath, F_OK) == -1)
// 		{
// 			if (errno == ENOENT)
// 				ereport(FATAL,
// 						(errcode(ERRCODE_UNDEFINED_DATABASE),
// 						 errmsg("database \"%s\" does not exist",
// 								dbname),
// 					errdetail("The database subdirectory \"%s\" is missing.",
// 							  fullpath)));
// 			else
// 				ereport(FATAL,
// 						(errcode_for_file_access(),
// 						 errmsg("could not access directory \"%s\": %m",
// 								fullpath)));
// 		}

// 		ValidatePgVersion(fullpath);
// 	}

// 	SetDatabasePath(fullpath);

// 	/*
// 	 * It's now possible to do real access to the system catalogs.
// 	 *
// 	 * Load relcache entries for the system catalogs.  This must create at
// 	 * least the minimum set of "nailed-in" cache entries.
// 	 */
// 	RelationCacheInitializePhase3();

// 	/* set up ACL framework (so CheckMyDatabase can check permissions) */
// 	initialize_acl();

// 	/*
// 	 * Re-read the pg_database row for our database, check permissions and set
// 	 * up database-specific GUC settings.  We can't do this until all the
// 	 * database-access infrastructure is up.  (Also, it wants to know if the
// 	 * user is a superuser, so the above stuff has to happen first.)
// 	 */
// 	if (!bootstrap)
// 		CheckMyDatabase(dbname, am_superuser);

// 	/*
// 	 * Now process any command-line switches and any additional GUC variable
// 	 * settings passed in the startup packet.   We couldn't do this before
// 	 * because we didn't know if client is a superuser.
// 	 */
// 	if (MyProcPort != NULL)
// 		process_startup_options(MyProcPort, am_superuser);

// 	/* Process pg_db_role_setting options */
// 	process_settings(MyDatabaseId, GetSessionUserId());

// 	/* Apply PostAuthDelay as soon as we've read all options */
// 	if (PostAuthDelay > 0)
// 		pg_usleep(PostAuthDelay * 1000000L);

// 	/*
// 	 * Initialize various default states that can't be set up until we've
// 	 * selected the active user and gotten the right GUC settings.
// 	 */

// 	/* set default namespace search path */
// 	InitializeSearchPath();

// 	/* initialize client encoding */
// 	InitializeClientEncoding();

// 	/* report this backend in the PgBackendStatus array */
// 	if (!bootstrap)
// 		pgstat_bestart();

// 	/* close the transaction we started above */
// 	if (!bootstrap)
// 		CommitTransactionCommand();
// }
// //end
// /* in tcop/utility.c */
// void
// PreventCommandIfParallelMode(const char *cmdname)
// {
// 	if (IsInParallelMode())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_TRANSACTION_STATE),
// 		/* translator: %s is name of a SQL command, eg CREATE */
// 				 errmsg("cannot execute %s during a parallel operation",
// 						cmdname)));
// }
// //end

// /* in tcop/postgres.c */
// void
// ProcessInterrupts(void)
// {
// 	/* OK to accept any interrupts now? */
// 	if (InterruptHoldoffCount != 0 || CritSectionCount != 0)
// 		return;
// 	InterruptPending = false;

// 	if (ProcDiePending)
// 	{
// 		ProcDiePending = false;
// 		QueryCancelPending = false;		/* ProcDie trumps QueryCancel */
// 		LockErrorCleanup();
// 		/* As in quickdie, don't risk sending to client during auth */
// 		if (ClientAuthInProgress && whereToSendOutput == DestRemote)
// 			whereToSendOutput = DestNone;
// 		if (ClientAuthInProgress)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_QUERY_CANCELED),
// 					 errmsg("canceling authentication due to timeout")));
// 		else if (IsAutoVacuumWorkerProcess())
// 			ereport(FATAL,
// 					(errcode(ERRCODE_ADMIN_SHUTDOWN),
// 					 errmsg("terminating autovacuum process due to administrator command")));
// 		else if (RecoveryConflictPending && RecoveryConflictRetryable)
// 		{
// 			pgstat_report_recovery_conflict(RecoveryConflictReason);
// 			ereport(FATAL,
// 					(errcode(ERRCODE_T_R_SERIALIZATION_FAILURE),
// 			  errmsg("terminating connection due to conflict with recovery"),
// 					 errdetail_recovery_conflict()));
// 		}
// 		else if (RecoveryConflictPending)
// 		{
// 			/* Currently there is only one non-retryable recovery conflict */
// 			Assert(RecoveryConflictReason == PROCSIG_RECOVERY_CONFLICT_DATABASE);
// 			pgstat_report_recovery_conflict(RecoveryConflictReason);
// 			ereport(FATAL,
// 					(errcode(ERRCODE_DATABASE_DROPPED),
// 			  errmsg("terminating connection due to conflict with recovery"),
// 					 errdetail_recovery_conflict()));
// 		}
// 		else
// 			ereport(FATAL,
// 					(errcode(ERRCODE_ADMIN_SHUTDOWN),
// 			 errmsg("terminating connection due to administrator command")));
// 	}
// 	if (ClientConnectionLost)
// 	{
// 		QueryCancelPending = false;		/* lost connection trumps QueryCancel */
// 		LockErrorCleanup();
// 		/* don't send to client, we already know the connection to be dead. */
// 		whereToSendOutput = DestNone;
// 		ereport(FATAL,
// 				(errcode(ERRCODE_CONNECTION_FAILURE),
// 				 errmsg("connection to client lost")));
// 	}

// 	/*
// 	 * If a recovery conflict happens while we are waiting for input from the
// 	 * client, the client is presumably just sitting idle in a transaction,
// 	 * preventing recovery from making progress.  Terminate the connection to
// 	 * dislodge it.
// 	 */
// 	if (RecoveryConflictPending && DoingCommandRead)
// 	{
// 		QueryCancelPending = false;		/* this trumps QueryCancel */
// 		RecoveryConflictPending = false;
// 		LockErrorCleanup();
// 		pgstat_report_recovery_conflict(RecoveryConflictReason);
// 		ereport(FATAL,
// 				(errcode(ERRCODE_T_R_SERIALIZATION_FAILURE),
// 			  errmsg("terminating connection due to conflict with recovery"),
// 				 errdetail_recovery_conflict(),
// 				 errhint("In a moment you should be able to reconnect to the"
// 						 " database and repeat your command.")));
// 	}

// 	if (QueryCancelPending)
// 	{
// 		bool		lock_timeout_occurred;
// 		bool		stmt_timeout_occurred;

// 		/*
// 		 * Don't allow query cancel interrupts while reading input from the
// 		 * client, because we might lose sync in the FE/BE protocol.  (Die
// 		 * interrupts are OK, because we won't read any further messages from
// 		 * the client in that case.)
// 		 */
// 		if (QueryCancelHoldoffCount != 0)
// 		{
// 			/*
// 			 * Re-arm InterruptPending so that we process the cancel request
// 			 * as soon as we're done reading the message.
// 			 */
// 			InterruptPending = true;
// 			return;
// 		}

// 		QueryCancelPending = false;

// 		/*
// 		 * If LOCK_TIMEOUT and STATEMENT_TIMEOUT indicators are both set, we
// 		 * need to clear both, so always fetch both.
// 		 */
// 		lock_timeout_occurred = get_timeout_indicator(LOCK_TIMEOUT, true);
// 		stmt_timeout_occurred = get_timeout_indicator(STATEMENT_TIMEOUT, true);

// 		/*
// 		 * If both were set, we want to report whichever timeout completed
// 		 * earlier; this ensures consistent behavior if the machine is slow
// 		 * enough that the second timeout triggers before we get here.  A tie
// 		 * is arbitrarily broken in favor of reporting a lock timeout.
// 		 */
// 		if (lock_timeout_occurred && stmt_timeout_occurred &&
// 			get_timeout_finish_time(STATEMENT_TIMEOUT) < get_timeout_finish_time(LOCK_TIMEOUT))
// 			lock_timeout_occurred = false;		/* report stmt timeout */

// 		if (lock_timeout_occurred)
// 		{
// 			LockErrorCleanup();
// 			ereport(ERROR,
// 					(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 					 errmsg("canceling statement due to lock timeout")));
// 		}
// 		if (stmt_timeout_occurred)
// 		{
// 			LockErrorCleanup();
// 			ereport(ERROR,
// 					(errcode(ERRCODE_QUERY_CANCELED),
// 					 errmsg("canceling statement due to statement timeout")));
// 		}
// 		if (IsAutoVacuumWorkerProcess())
// 		{
// 			LockErrorCleanup();
// 			ereport(ERROR,
// 					(errcode(ERRCODE_QUERY_CANCELED),
// 					 errmsg("canceling autovacuum task")));
// 		}
// 		if (RecoveryConflictPending)
// 		{
// 			RecoveryConflictPending = false;
// 			LockErrorCleanup();
// 			pgstat_report_recovery_conflict(RecoveryConflictReason);
// 			ereport(ERROR,
// 					(errcode(ERRCODE_T_R_SERIALIZATION_FAILURE),
// 				 errmsg("canceling statement due to conflict with recovery"),
// 					 errdetail_recovery_conflict()));
// 		}

// 		/*
// 		 * If we are reading a command from the client, just ignore the cancel
// 		 * request --- sending an extra error message won't accomplish
// 		 * anything.  Otherwise, go ahead and throw the error.
// 		 */
// 		if (!DoingCommandRead)
// 		{
// 			LockErrorCleanup();
// 			ereport(ERROR,
// 					(errcode(ERRCODE_QUERY_CANCELED),
// 					 errmsg("canceling statement due to user request")));
// 		}
// 	}

// 	if (IdleInTransactionSessionTimeoutPending)
// 	{
// 		/* Has the timeout setting changed since last we looked? */
// 		if (IdleInTransactionSessionTimeout > 0)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_IDLE_IN_TRANSACTION_SESSION_TIMEOUT),
// 					 errmsg("terminating connection due to idle-in-transaction timeout")));
// 		else
// 			IdleInTransactionSessionTimeoutPending = false;

// 	}

// 	if (ParallelMessagePending)
// 		HandleParallelMessages();
// }
// //end
// //end miscadmin.h

// //nodes/makefuncs.c
// Const *
// makeConst(Oid consttype,
// 		  int32 consttypmod,
// 		  Oid constcollid,
// 		  int constlen,
// 		  Datum constvalue,
// 		  bool constisnull,
// 		  bool constbyval)
// {
// 	Const	   *cnst = makeNode(Const);

// 	/*
// 	 * If it's a varlena value, force it to be in non-expanded (non-toasted)
// 	 * format; this avoids any possible dependency on external values and
// 	 * improves consistency of representation, which is important for equal().
// 	 */
// 	if (!constisnull && constlen == -1)
// 		constvalue = PointerGetDatum(PG_DETOAST_DATUM(constvalue));

// 	cnst->consttype = consttype;
// 	cnst->consttypmod = consttypmod;
// 	cnst->constcollid = constcollid;
// 	cnst->constlen = constlen;
// 	cnst->constvalue = constvalue;
// 	cnst->constisnull = constisnull;
// 	cnst->constbyval = constbyval;
// 	cnst->location = -1;		/* "unknown" */

// 	return cnst;
// }
// //end
// FuncCall *
// makeFuncCall(List *name, List *args, int location)
// {
// 	FuncCall   *n = makeNode(FuncCall);

// 	n->funcname = name;
// 	n->args = args;
// 	n->agg_order = NIL;
// 	n->agg_filter = NULL;
// 	n->agg_within_group = false;
// 	n->agg_star = false;
// 	n->agg_distinct = false;
// 	n->func_variadic = false;
// 	n->over = NULL;
// 	n->location = location;
// 	return n;
// }
// //end
// ColumnDef *
// makeColumnDef(const char *colname, Oid typeOid, int32 typmod, Oid collOid)
// {
// 	ColumnDef  *n = makeNode(ColumnDef);

// 	n->colname = pstrdup(colname);
// 	n->typeName = makeTypeNameFromOid(typeOid, typmod);
// 	n->inhcount = 0;
// 	n->is_local = true;
// 	n->is_not_null = false;
// 	n->is_from_type = false;
// 	n->storage = 0;
// 	n->raw_default = NULL;
// 	n->cooked_default = NULL;
// 	n->collClause = NULL;
// 	n->collOid = collOid;
// 	n->constraints = NIL;
// 	n->fdwoptions = NIL;
// 	n->location = -1;

// 	return n;
// }
// //end
// //end makefuncs.c

// //nodes/nodeFuncs.c
// Node *
// strip_implicit_coercions(Node *node)
// {
// 	if (node == NULL)
// 		return NULL;
// 	if (IsA(node, FuncExpr))
// 	{
// 		FuncExpr   *f = (FuncExpr *) node;

// 		if (f->funcformat == COERCE_IMPLICIT_CAST)
// 			return strip_implicit_coercions(linitial(f->args));
// 	}
// 	else if (IsA(node, RelabelType))
// 	{
// 		RelabelType *r = (RelabelType *) node;

// 		if (r->relabelformat == COERCE_IMPLICIT_CAST)
// 			return strip_implicit_coercions((Node *) r->arg);
// 	}
// 	else if (IsA(node, CoerceViaIO))
// 	{
// 		CoerceViaIO *c = (CoerceViaIO *) node;

// 		if (c->coerceformat == COERCE_IMPLICIT_CAST)
// 			return strip_implicit_coercions((Node *) c->arg);
// 	}
// 	else if (IsA(node, ArrayCoerceExpr))
// 	{
// 		ArrayCoerceExpr *c = (ArrayCoerceExpr *) node;

// 		if (c->coerceformat == COERCE_IMPLICIT_CAST)
// 			return strip_implicit_coercions((Node *) c->arg);
// 	}
// 	else if (IsA(node, ConvertRowtypeExpr))
// 	{
// 		ConvertRowtypeExpr *c = (ConvertRowtypeExpr *) node;

// 		if (c->convertformat == COERCE_IMPLICIT_CAST)
// 			return strip_implicit_coercions((Node *) c->arg);
// 	}
// 	else if (IsA(node, CoerceToDomain))
// 	{
// 		CoerceToDomain *c = (CoerceToDomain *) node;

// 		if (c->coercionformat == COERCE_IMPLICIT_CAST)
// 			return strip_implicit_coercions((Node *) c->arg);
// 	}
// 	return node;
// }
// //end
// void
// fix_opfuncids(Node *node)
// {
// 	/* This tree walk requires no special setup, so away we go... */
// 	fix_opfuncids_walker(node, NULL);
// }
// //end
// void
// set_opfuncid(OpExpr *opexpr)
// {
// 	if (opexpr->opfuncid == InvalidOid)
// 		opexpr->opfuncid = get_opcode(opexpr->opno);
// }
// //end
// void
// set_sa_opfuncid(ScalarArrayOpExpr *opexpr)
// {
// 	if (opexpr->opfuncid == InvalidOid)
// 		opexpr->opfuncid = get_opcode(opexpr->opno);
// }
// //end
// bool
// check_functions_in_node(Node *node, check_function_callback checker,
// 						void *context)
// {
// 	switch (nodeTag(node))
// 	{
// 		case T_Aggref:
// 			{
// 				Aggref	   *expr = (Aggref *) node;

// 				if (checker(expr->aggfnoid, context))
// 					return true;
// 			}
// 			break;
// 		case T_WindowFunc:
// 			{
// 				WindowFunc *expr = (WindowFunc *) node;

// 				if (checker(expr->winfnoid, context))
// 					return true;
// 			}
// 			break;
// 		case T_FuncExpr:
// 			{
// 				FuncExpr   *expr = (FuncExpr *) node;

// 				if (checker(expr->funcid, context))
// 					return true;
// 			}
// 			break;
// 		case T_OpExpr:
// 		case T_DistinctExpr:	/* struct-equivalent to OpExpr */
// 		case T_NullIfExpr:		/* struct-equivalent to OpExpr */
// 			{
// 				OpExpr	   *expr = (OpExpr *) node;

// 				/* Set opfuncid if it wasn't set already */
// 				set_opfuncid(expr);
// 				if (checker(expr->opfuncid, context))
// 					return true;
// 			}
// 			break;
// 		case T_ScalarArrayOpExpr:
// 			{
// 				ScalarArrayOpExpr *expr = (ScalarArrayOpExpr *) node;

// 				set_sa_opfuncid(expr);
// 				if (checker(expr->opfuncid, context))
// 					return true;
// 			}
// 			break;
// 		case T_CoerceViaIO:
// 			{
// 				CoerceViaIO *expr = (CoerceViaIO *) node;
// 				Oid			iofunc;
// 				Oid			typioparam;
// 				bool		typisvarlena;

// 				/* check the result type's input function */
// 				getTypeInputInfo(expr->resulttype,
// 								 &iofunc, &typioparam);
// 				if (checker(iofunc, context))
// 					return true;
// 				/* check the input type's output function */
// 				getTypeOutputInfo(exprType((Node *) expr->arg),
// 								  &iofunc, &typisvarlena);
// 				if (checker(iofunc, context))
// 					return true;
// 			}
// 			break;
// 		case T_ArrayCoerceExpr:
// 			{
// 				ArrayCoerceExpr *expr = (ArrayCoerceExpr *) node;

// 				if (OidIsValid(expr->elemfuncid) &&
// 					checker(expr->elemfuncid, context))
// 					return true;
// 			}
// 			break;
// 		case T_RowCompareExpr:
// 			{
// 				RowCompareExpr *rcexpr = (RowCompareExpr *) node;
// 				ListCell   *opid;

// 				foreach(opid, rcexpr->opnos)
// 				{
// 					Oid			opfuncid = get_opcode(lfirst_oid(opid));

// 					if (checker(opfuncid, context))
// 						return true;
// 				}
// 			}
// 			break;
// 		default:
// 			break;
// 	}
// 	return false;
// }
// //end
// bool
// planstate_tree_walker(PlanState *planstate,
// 					  bool (*walker) (),
// 					  void *context)
// {
// 	Plan	   *plan = planstate->plan;
// 	ListCell   *lc;

// 	/* initPlan-s */
// 	if (planstate_walk_subplans(planstate->initPlan, walker, context))
// 		return true;

// 	/* lefttree */
// 	if (outerPlanState(planstate))
// 	{
// 		if (walker(outerPlanState(planstate), context))
// 			return true;
// 	}

// 	/* righttree */
// 	if (innerPlanState(planstate))
// 	{
// 		if (walker(innerPlanState(planstate), context))
// 			return true;
// 	}

// 	/* special child plans */
// 	switch (nodeTag(plan))
// 	{
// 		case T_ModifyTable:
// 			if (planstate_walk_members(((ModifyTable *) plan)->plans,
// 								  ((ModifyTableState *) planstate)->mt_plans,
// 									   walker, context))
// 				return true;
// 			break;
// 		case T_Append:
// 			if (planstate_walk_members(((Append *) plan)->appendplans,
// 									((AppendState *) planstate)->appendplans,
// 									   walker, context))
// 				return true;
// 			break;
// 		case T_MergeAppend:
// 			if (planstate_walk_members(((MergeAppend *) plan)->mergeplans,
// 								((MergeAppendState *) planstate)->mergeplans,
// 									   walker, context))
// 				return true;
// 			break;
// 		case T_BitmapAnd:
// 			if (planstate_walk_members(((BitmapAnd *) plan)->bitmapplans,
// 								 ((BitmapAndState *) planstate)->bitmapplans,
// 									   walker, context))
// 				return true;
// 			break;
// 		case T_BitmapOr:
// 			if (planstate_walk_members(((BitmapOr *) plan)->bitmapplans,
// 								  ((BitmapOrState *) planstate)->bitmapplans,
// 									   walker, context))
// 				return true;
// 			break;
// 		case T_SubqueryScan:
// 			if (walker(((SubqueryScanState *) planstate)->subplan, context))
// 				return true;
// 			break;
// 		case T_CustomScan:
// 			foreach(lc, ((CustomScanState *) planstate)->custom_ps)
// 			{
// 				if (walker((PlanState *) lfirst(lc), context))
// 					return true;
// 			}
// 			break;
// 		default:
// 			break;
// 	}

// 	/* subPlan-s */
// 	if (planstate_walk_subplans(planstate->subPlan, walker, context))
// 		return true;

// 	return false;
// }
// //end
// //end nodeFuncs.c


// //nodes/readfuncs.h
// /*
//  * prototypes for functions in read.c (the lisp token parser)
//  */
// char *
// pg_strtok(int *length)
// {
// 	char	   *local_str;		/* working pointer to string */
// 	char	   *ret_str;		/* start of token to return */

// 	local_str = pg_strtok_ptr;

// 	while (*local_str == ' ' || *local_str == '\n' || *local_str == '\t')
// 		local_str++;

// 	if (*local_str == '\0')
// 	{
// 		*length = 0;
// 		pg_strtok_ptr = local_str;
// 		return NULL;			/* no more tokens */
// 	}

// 	/*
// 	 * Now pointing at start of next token.
// 	 */
// 	ret_str = local_str;

// 	if (*local_str == '(' || *local_str == ')' ||
// 		*local_str == '{' || *local_str == '}')
// 	{
// 		/* special 1-character token */
// 		local_str++;
// 	}
// 	else
// 	{
// 		/* Normal token, possibly containing backslashes */
// 		while (*local_str != '\0' &&
// 			   *local_str != ' ' && *local_str != '\n' &&
// 			   *local_str != '\t' &&
// 			   *local_str != '(' && *local_str != ')' &&
// 			   *local_str != '{' && *local_str != '}')
// 		{
// 			if (*local_str == '\\' && local_str[1] != '\0')
// 				local_str += 2;
// 			else
// 				local_str++;
// 		}
// 	}

// 	*length = local_str - ret_str;

// 	/* Recognize special case for "empty" token */
// 	if (*length == 2 && ret_str[0] == '<' && ret_str[1] == '>')
// 		*length = 0;

// 	pg_strtok_ptr = local_str;

// 	return ret_str;
// }
// //end
// char *
// debackslash(char *token, int length)
// {
// 	char	   *result = palloc(length + 1);
// 	char	   *ptr = result;

// 	while (length > 0)
// 	{
// 		if (*token == '\\' && length > 1)
// 			token++, length--;
// 		*ptr++ = *token++;
// 		length--;
// 	}
// 	*ptr = '\0';
// 	return result;
// }
// //end
// //end readfuncs.h

// //新文件目录
// //optimizer/clauses.c
// WindowFuncLists *
// find_window_functions(Node *clause, Index maxWinRef)
// {
// 	WindowFuncLists *lists = palloc(sizeof(WindowFuncLists));

// 	lists->numWindowFuncs = 0;
// 	lists->maxWinRef = maxWinRef;
// 	lists->windowFuncs = (List **) palloc0((maxWinRef + 1) * sizeof(List *));
// 	(void) find_window_functions_walker(clause, lists);
// 	return lists;
// }
// //end
// bool
// contain_volatile_functions(Node *clause)
// {
// 	return contain_volatile_functions_walker(clause, NULL);
// }
// //end

// bool
// contain_nonstrict_functions(Node *clause)
// {
// 	return contain_nonstrict_functions_walker(clause, NULL);
// }
// //end
// Node *
// estimate_expression_value(PlannerInfo *root, Node *node)
// {
// 	eval_const_expressions_context context;

// 	context.boundParams = root->glob->boundParams;		/* bound Params */
// 	/* we do not need to mark the plan as depending on inlined functions */
// 	context.root = NULL;
// 	context.active_fns = NIL;	/* nothing being recursively simplified */
// 	context.case_val = NULL;	/* no CASE being examined */
// 	context.estimate = true;	/* unsafe transformations OK */
// 	return eval_const_expressions_mutator(node, &context);
// }
// //end
// void
// get_agg_clause_costs(PlannerInfo *root, Node *clause, AggSplit aggsplit,
// 					 AggClauseCosts *costs)
// {
// 	get_agg_clause_costs_context context;

// 	context.root = root;
// 	context.aggsplit = aggsplit;
// 	context.costs = costs;
// 	(void) get_agg_clause_costs_walker(clause, &context);
// }
// //end
// bool
// contain_volatile_functions_not_nextval(Node *clause)
// {
// 	return contain_volatile_functions_not_nextval_walker(clause, NULL);
// }
// //end
// bool
// has_parallel_hazard(Node *node, bool allow_restricted)
// {
// 	has_parallel_hazard_arg context;

// 	context.allow_restricted = allow_restricted;
// 	return has_parallel_hazard_walker(node, &context);
// }
// //end
// bool
// contain_leaked_vars(Node *clause)
// {
// 	return contain_leaked_vars_walker(clause, NULL);
// }
// //end

// //end clauses.c


// //optimizer/cost.h
// /*
//  * prototypes for costsize.c
//  *	  routines to compute costs and sizes
//  */
// double
// index_pages_fetched(double tuples_fetched, BlockNumber pages,
// 					double index_pages, PlannerInfo *root)
// {
// 	double		pages_fetched;
// 	double		total_pages;
// 	double		T,
// 				b;

// 	/* T is # pages in table, but don't allow it to be zero */
// 	T = (pages > 1) ? (double) pages : 1.0;

// 	/* Compute number of pages assumed to be competing for cache space */
// 	total_pages = root->total_table_pages + index_pages;
// 	total_pages = Max(total_pages, 1.0);
// 	Assert(T <= total_pages);

// 	/* b is pro-rated share of effective_cache_size */
// 	b = (double) effective_cache_size *T / total_pages;

// 	/* force it positive and integral */
// 	if (b <= 1.0)
// 		b = 1.0;
// 	else
// 		b = ceil(b);

// 	/* This part is the Mackert and Lohman formula */
// 	if (T <= b)
// 	{
// 		pages_fetched =
// 			(2.0 * T * tuples_fetched) / (2.0 * T + tuples_fetched);
// 		if (pages_fetched >= T)
// 			pages_fetched = T;
// 		else
// 			pages_fetched = ceil(pages_fetched);
// 	}
// 	else
// 	{
// 		double		lim;

// 		lim = (2.0 * T * b) / (2.0 * T - b);
// 		if (tuples_fetched <= lim)
// 		{
// 			pages_fetched =
// 				(2.0 * T * tuples_fetched) / (2.0 * T + tuples_fetched);
// 		}
// 		else
// 		{
// 			pages_fetched =
// 				b + (tuples_fetched - lim) * (T - b) / T;
// 		}
// 		pages_fetched = ceil(pages_fetched);
// 	}
// 	return pages_fetched;
// }
// //end
// void
// cost_tidscan(Path *path, PlannerInfo *root,
// 			 RelOptInfo *baserel, List *tidquals, ParamPathInfo *param_info)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;
// 	bool		isCurrentOf = false;
// 	QualCost	qpqual_cost;
// 	Cost		cpu_per_tuple;
// 	QualCost	tid_qual_cost;
// 	int			ntuples;
// 	ListCell   *l;
// 	double		spc_random_page_cost;

// 	/* Should only be applied to base relations */
// 	Assert(baserel->relid > 0);
// 	Assert(baserel->rtekind == RTE_RELATION);

// 	/* Mark the path with the correct row estimate */
// 	if (param_info)
// 		path->rows = param_info->ppi_rows;
// 	else
// 		path->rows = baserel->rows;

// 	/* Count how many tuples we expect to retrieve */
// 	ntuples = 0;
// 	foreach(l, tidquals)
// 	{
// 		if (IsA(lfirst(l), ScalarArrayOpExpr))
// 		{
// 			/* Each element of the array yields 1 tuple */
// 			ScalarArrayOpExpr *saop = (ScalarArrayOpExpr *) lfirst(l);
// 			Node	   *arraynode = (Node *) lsecond(saop->args);

// 			ntuples += estimate_array_length(arraynode);
// 		}
// 		else if (IsA(lfirst(l), CurrentOfExpr))
// 		{
// 			/* CURRENT OF yields 1 tuple */
// 			isCurrentOf = true;
// 			ntuples++;
// 		}
// 		else
// 		{
// 			/* It's just CTID = something, count 1 tuple */
// 			ntuples++;
// 		}
// 	}

// 	/*
// 	 * We must force TID scan for WHERE CURRENT OF, because only nodeTidscan.c
// 	 * understands how to do it correctly.  Therefore, honor enable_tidscan
// 	 * only when CURRENT OF isn't present.  Also note that cost_qual_eval
// 	 * counts a CurrentOfExpr as having startup cost disable_cost, which we
// 	 * subtract off here; that's to prevent other plan types such as seqscan
// 	 * from winning.
// 	 */
// 	if (isCurrentOf)
// 	{
// 		Assert(baserel->baserestrictcost.startup >= disable_cost);
// 		startup_cost -= disable_cost;
// 	}
// 	else if (!enable_tidscan)
// 		startup_cost += disable_cost;

// 	/*
// 	 * The TID qual expressions will be computed once, any other baserestrict
// 	 * quals once per retrieved tuple.
// 	 */
// 	cost_qual_eval(&tid_qual_cost, tidquals, root);

// 	/* fetch estimated page cost for tablespace containing table */
// 	get_tablespace_page_costs(baserel->reltablespace,
// 							  &spc_random_page_cost,
// 							  NULL);

// 	/* disk costs --- assume each tuple on a different page */
// 	run_cost += spc_random_page_cost * ntuples;

// 	/* Add scanning CPU costs */
// 	get_restriction_qual_cost(root, baserel, param_info, &qpqual_cost);

// 	/* XXX currently we assume TID quals are a subset of qpquals */
// 	startup_cost += qpqual_cost.startup + tid_qual_cost.per_tuple;
// 	cpu_per_tuple = cpu_tuple_cost + qpqual_cost.per_tuple -
// 		tid_qual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * ntuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->pathtarget->cost.startup;
// 	run_cost += path->pathtarget->cost.per_tuple * path->rows;

// 	path->startup_cost = startup_cost;
// 	path->total_cost = startup_cost + run_cost;
// }
// //end
// void
// cost_subqueryscan(SubqueryScanPath *path, PlannerInfo *root,
// 				  RelOptInfo *baserel, ParamPathInfo *param_info)
// {
// 	Cost		startup_cost;
// 	Cost		run_cost;
// 	QualCost	qpqual_cost;
// 	Cost		cpu_per_tuple;

// 	/* Should only be applied to base relations that are subqueries */
// 	Assert(baserel->relid > 0);
// 	Assert(baserel->rtekind == RTE_SUBQUERY);

// 	/* Mark the path with the correct row estimate */
// 	if (param_info)
// 		path->path.rows = param_info->ppi_rows;
// 	else
// 		path->path.rows = baserel->rows;

// 	/*
// 	 * Cost of path is cost of evaluating the subplan, plus cost of evaluating
// 	 * any restriction clauses and tlist that will be attached to the
// 	 * SubqueryScan node, plus cpu_tuple_cost to account for selection and
// 	 * projection overhead.
// 	 */
// 	path->path.startup_cost = path->subpath->startup_cost;
// 	path->path.total_cost = path->subpath->total_cost;

// 	get_restriction_qual_cost(root, baserel, param_info, &qpqual_cost);

// 	startup_cost = qpqual_cost.startup;
// 	cpu_per_tuple = cpu_tuple_cost + qpqual_cost.per_tuple;
// 	run_cost = cpu_per_tuple * baserel->tuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->path.pathtarget->cost.startup;
// 	run_cost += path->path.pathtarget->cost.per_tuple * path->path.rows;

// 	path->path.startup_cost += startup_cost;
// 	path->path.total_cost += startup_cost + run_cost;
// }
// //end
// void
// cost_functionscan(Path *path, PlannerInfo *root,
// 				  RelOptInfo *baserel, ParamPathInfo *param_info)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;
// 	QualCost	qpqual_cost;
// 	Cost		cpu_per_tuple;
// 	RangeTblEntry *rte;
// 	QualCost	exprcost;

// 	/* Should only be applied to base relations that are functions */
// 	Assert(baserel->relid > 0);
// 	rte = planner_rt_fetch(baserel->relid, root);
// 	Assert(rte->rtekind == RTE_FUNCTION);

// 	/* Mark the path with the correct row estimate */
// 	if (param_info)
// 		path->rows = param_info->ppi_rows;
// 	else
// 		path->rows = baserel->rows;

// 	/*
// 	 * Estimate costs of executing the function expression(s).
// 	 *
// 	 * Currently, nodeFunctionscan.c always executes the functions to
// 	 * completion before returning any rows, and caches the results in a
// 	 * tuplestore.  So the function eval cost is all startup cost, and per-row
// 	 * costs are minimal.
// 	 *
// 	 * XXX in principle we ought to charge tuplestore spill costs if the
// 	 * number of rows is large.  However, given how phony our rowcount
// 	 * estimates for functions tend to be, there's not a lot of point in that
// 	 * refinement right now.
// 	 */
// 	cost_qual_eval_node(&exprcost, (Node *) rte->functions, root);

// 	startup_cost += exprcost.startup + exprcost.per_tuple;

// 	/* Add scanning CPU costs */
// 	get_restriction_qual_cost(root, baserel, param_info, &qpqual_cost);

// 	startup_cost += qpqual_cost.startup;
// 	cpu_per_tuple = cpu_tuple_cost + qpqual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * baserel->tuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->pathtarget->cost.startup;
// 	run_cost += path->pathtarget->cost.per_tuple * path->rows;

// 	path->startup_cost = startup_cost;
// 	path->total_cost = startup_cost + run_cost;
// }
// //end
// void
// cost_valuesscan(Path *path, PlannerInfo *root,
// 				RelOptInfo *baserel, ParamPathInfo *param_info)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;
// 	QualCost	qpqual_cost;
// 	Cost		cpu_per_tuple;

// 	/* Should only be applied to base relations that are values lists */
// 	Assert(baserel->relid > 0);
// 	Assert(baserel->rtekind == RTE_VALUES);

// 	/* Mark the path with the correct row estimate */
// 	if (param_info)
// 		path->rows = param_info->ppi_rows;
// 	else
// 		path->rows = baserel->rows;

// 	/*
// 	 * For now, estimate list evaluation cost at one operator eval per list
// 	 * (probably pretty bogus, but is it worth being smarter?)
// 	 */
// 	cpu_per_tuple = cpu_operator_cost;

// 	/* Add scanning CPU costs */
// 	get_restriction_qual_cost(root, baserel, param_info, &qpqual_cost);

// 	startup_cost += qpqual_cost.startup;
// 	cpu_per_tuple += cpu_tuple_cost + qpqual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * baserel->tuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->pathtarget->cost.startup;
// 	run_cost += path->pathtarget->cost.per_tuple * path->rows;

// 	path->startup_cost = startup_cost;
// 	path->total_cost = startup_cost + run_cost;
// }
// //end
// void
// cost_ctescan(Path *path, PlannerInfo *root,
// 			 RelOptInfo *baserel, ParamPathInfo *param_info)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;
// 	QualCost	qpqual_cost;
// 	Cost		cpu_per_tuple;

// 	/* Should only be applied to base relations that are CTEs */
// 	Assert(baserel->relid > 0);
// 	Assert(baserel->rtekind == RTE_CTE);

// 	/* Mark the path with the correct row estimate */
// 	if (param_info)
// 		path->rows = param_info->ppi_rows;
// 	else
// 		path->rows = baserel->rows;

// 	/* Charge one CPU tuple cost per row for tuplestore manipulation */
// 	cpu_per_tuple = cpu_tuple_cost;

// 	/* Add scanning CPU costs */
// 	get_restriction_qual_cost(root, baserel, param_info, &qpqual_cost);

// 	startup_cost += qpqual_cost.startup;
// 	cpu_per_tuple += cpu_tuple_cost + qpqual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * baserel->tuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->pathtarget->cost.startup;
// 	run_cost += path->pathtarget->cost.per_tuple * path->rows;

// 	path->startup_cost = startup_cost;
// 	path->total_cost = startup_cost + run_cost;
// }
// //end
// void
// cost_recursive_union(Path *runion, Path *nrterm, Path *rterm)
// {
// 	Cost		startup_cost;
// 	Cost		total_cost;
// 	double		total_rows;

// 	/* We probably have decent estimates for the non-recursive term */
// 	startup_cost = nrterm->startup_cost;
// 	total_cost = nrterm->total_cost;
// 	total_rows = nrterm->rows;

// 	/*
// 	 * We arbitrarily assume that about 10 recursive iterations will be
// 	 * needed, and that we've managed to get a good fix on the cost and output
// 	 * size of each one of them.  These are mighty shaky assumptions but it's
// 	 * hard to see how to do better.
// 	 */
// 	total_cost += 10 * rterm->total_cost;
// 	total_rows += 10 * rterm->rows;

// 	/*
// 	 * Also charge cpu_tuple_cost per row to account for the costs of
// 	 * manipulating the tuplestores.  (We don't worry about possible
// 	 * spill-to-disk costs.)
// 	 */
// 	total_cost += cpu_tuple_cost * total_rows;

// 	runion->startup_cost = startup_cost;
// 	runion->total_cost = total_cost;
// 	runion->rows = total_rows;
// 	runion->pathtarget->width = Max(nrterm->pathtarget->width,
// 									rterm->pathtarget->width);
// }
// //end
// void
// cost_sort(Path *path, PlannerInfo *root,
// 		  List *pathkeys, Cost input_cost, double tuples, int width,
// 		  Cost comparison_cost, int sort_mem,
// 		  double limit_tuples)
// {
// 	Cost		startup_cost = input_cost;
// 	Cost		run_cost = 0;
// 	double		input_bytes = relation_byte_size(tuples, width);
// 	double		output_bytes;
// 	double		output_tuples;
// 	long		sort_mem_bytes = sort_mem * 1024L;

// 	if (!enable_sort)
// 		startup_cost += disable_cost;

// 	path->rows = tuples;

// 	/*
// 	 * We want to be sure the cost of a sort is never estimated as zero, even
// 	 * if passed-in tuple count is zero.  Besides, mustn't do log(0)...
// 	 */
// 	if (tuples < 2.0)
// 		tuples = 2.0;

// 	/* Include the default cost-per-comparison */
// 	comparison_cost += 2.0 * cpu_operator_cost;

// 	/* Do we have a useful LIMIT? */
// 	if (limit_tuples > 0 && limit_tuples < tuples)
// 	{
// 		output_tuples = limit_tuples;
// 		output_bytes = relation_byte_size(output_tuples, width);
// 	}
// 	else
// 	{
// 		output_tuples = tuples;
// 		output_bytes = input_bytes;
// 	}

// 	if (output_bytes > sort_mem_bytes)
// 	{
// 		/*
// 		 * We'll have to use a disk-based sort of all the tuples
// 		 */
// 		double		npages = ceil(input_bytes / BLCKSZ);
// 		double		nruns = input_bytes / sort_mem_bytes;
// 		double		mergeorder = tuplesort_merge_order(sort_mem_bytes);
// 		double		log_runs;
// 		double		npageaccesses;

// 		/*
// 		 * CPU costs
// 		 *
// 		 * Assume about N log2 N comparisons
// 		 */
// 		startup_cost += comparison_cost * tuples * LOG2(tuples);

// 		/* Disk costs */

// 		/* Compute logM(r) as log(r) / log(M) */
// 		if (nruns > mergeorder)
// 			log_runs = ceil(log(nruns) / log(mergeorder));
// 		else
// 			log_runs = 1.0;
// 		npageaccesses = 2.0 * npages * log_runs;
// 		/* Assume 3/4ths of accesses are sequential, 1/4th are not */
// 		startup_cost += npageaccesses *
// 			(seq_page_cost * 0.75 + random_page_cost * 0.25);
// 	}
// 	else if (tuples > 2 * output_tuples || input_bytes > sort_mem_bytes)
// 	{
// 		/*
// 		 * We'll use a bounded heap-sort keeping just K tuples in memory, for
// 		 * a total number of tuple comparisons of N log2 K; but the constant
// 		 * factor is a bit higher than for quicksort.  Tweak it so that the
// 		 * cost curve is continuous at the crossover point.
// 		 */
// 		startup_cost += comparison_cost * tuples * LOG2(2.0 * output_tuples);
// 	}
// 	else
// 	{
// 		/* We'll use plain quicksort on all the input tuples */
// 		startup_cost += comparison_cost * tuples * LOG2(tuples);
// 	}

// 	/*
// 	 * Also charge a small amount (arbitrarily set equal to operator cost) per
// 	 * extracted tuple.  We don't charge cpu_tuple_cost because a Sort node
// 	 * doesn't do qual-checking or projection, so it has less overhead than
// 	 * most plan nodes.  Note it's correct to use tuples not output_tuples
// 	 * here --- the upper LIMIT will pro-rate the run cost so we'd be double
// 	 * counting the LIMIT otherwise.
// 	 */
// 	run_cost += cpu_operator_cost * tuples;

// 	path->startup_cost = startup_cost;
// 	path->total_cost = startup_cost + run_cost;
// }
// //end
// void
// cost_agg(Path *path, PlannerInfo *root,
// 		 AggStrategy aggstrategy, const AggClauseCosts *aggcosts,
// 		 int numGroupCols, double numGroups,
// 		 Cost input_startup_cost, Cost input_total_cost,
// 		 double input_tuples)
// {
// 	double		output_tuples;
// 	Cost		startup_cost;
// 	Cost		total_cost;
// 	AggClauseCosts dummy_aggcosts;

// 	/* Use all-zero per-aggregate costs if NULL is passed */
// 	if (aggcosts == NULL)
// 	{
// 		Assert(aggstrategy == AGG_HASHED);
// 		MemSet(&dummy_aggcosts, 0, sizeof(AggClauseCosts));
// 		aggcosts = &dummy_aggcosts;
// 	}

// 	/*
// 	 * The transCost.per_tuple component of aggcosts should be charged once
// 	 * per input tuple, corresponding to the costs of evaluating the aggregate
// 	 * transfns and their input expressions (with any startup cost of course
// 	 * charged but once).  The finalCost component is charged once per output
// 	 * tuple, corresponding to the costs of evaluating the finalfns.
// 	 *
// 	 * If we are grouping, we charge an additional cpu_operator_cost per
// 	 * grouping column per input tuple for grouping comparisons.
// 	 *
// 	 * We will produce a single output tuple if not grouping, and a tuple per
// 	 * group otherwise.  We charge cpu_tuple_cost for each output tuple.
// 	 *
// 	 * Note: in this cost model, AGG_SORTED and AGG_HASHED have exactly the
// 	 * same total CPU cost, but AGG_SORTED has lower startup cost.  If the
// 	 * input path is already sorted appropriately, AGG_SORTED should be
// 	 * preferred (since it has no risk of memory overflow).  This will happen
// 	 * as long as the computed total costs are indeed exactly equal --- but if
// 	 * there's roundoff error we might do the wrong thing.  So be sure that
// 	 * the computations below form the same intermediate values in the same
// 	 * order.
// 	 */
// 	if (aggstrategy == AGG_PLAIN)
// 	{
// 		startup_cost = input_total_cost;
// 		startup_cost += aggcosts->transCost.startup;
// 		startup_cost += aggcosts->transCost.per_tuple * input_tuples;
// 		startup_cost += aggcosts->finalCost;
// 		/* we aren't grouping */
// 		total_cost = startup_cost + cpu_tuple_cost;
// 		output_tuples = 1;
// 	}
// 	else if (aggstrategy == AGG_SORTED)
// 	{
// 		/* Here we are able to deliver output on-the-fly */
// 		startup_cost = input_startup_cost;
// 		total_cost = input_total_cost;
// 		/* calcs phrased this way to match HASHED case, see note above */
// 		total_cost += aggcosts->transCost.startup;
// 		total_cost += aggcosts->transCost.per_tuple * input_tuples;
// 		total_cost += (cpu_operator_cost * numGroupCols) * input_tuples;
// 		total_cost += aggcosts->finalCost * numGroups;
// 		total_cost += cpu_tuple_cost * numGroups;
// 		output_tuples = numGroups;
// 	}
// 	else
// 	{
// 		/* must be AGG_HASHED */
// 		startup_cost = input_total_cost;
// 		if (!enable_hashagg)
// 			startup_cost += disable_cost;
// 		startup_cost += aggcosts->transCost.startup;
// 		startup_cost += aggcosts->transCost.per_tuple * input_tuples;
// 		startup_cost += (cpu_operator_cost * numGroupCols) * input_tuples;
// 		total_cost = startup_cost;
// 		total_cost += aggcosts->finalCost * numGroups;
// 		total_cost += cpu_tuple_cost * numGroups;
// 		output_tuples = numGroups;
// 	}

// 	path->rows = output_tuples;
// 	path->startup_cost = startup_cost;
// 	path->total_cost = total_cost;
// }
// //end
// void
// initial_cost_nestloop(PlannerInfo *root, JoinCostWorkspace *workspace,
// 					  JoinType jointype,
// 					  Path *outer_path, Path *inner_path,
// 					  SpecialJoinInfo *sjinfo,
// 					  SemiAntiJoinFactors *semifactors)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;
// 	double		outer_path_rows = outer_path->rows;
// 	Cost		inner_rescan_start_cost;
// 	Cost		inner_rescan_total_cost;
// 	Cost		inner_run_cost;
// 	Cost		inner_rescan_run_cost;

// 	/* estimate costs to rescan the inner relation */
// 	cost_rescan(root, inner_path,
// 				&inner_rescan_start_cost,
// 				&inner_rescan_total_cost);

// 	/* cost of source data */

// 	/*
// 	 * NOTE: clearly, we must pay both outer and inner paths' startup_cost
// 	 * before we can start returning tuples, so the join's startup cost is
// 	 * their sum.  We'll also pay the inner path's rescan startup cost
// 	 * multiple times.
// 	 */
// 	startup_cost += outer_path->startup_cost + inner_path->startup_cost;
// 	run_cost += outer_path->total_cost - outer_path->startup_cost;
// 	if (outer_path_rows > 1)
// 		run_cost += (outer_path_rows - 1) * inner_rescan_start_cost;

// 	inner_run_cost = inner_path->total_cost - inner_path->startup_cost;
// 	inner_rescan_run_cost = inner_rescan_total_cost - inner_rescan_start_cost;

// 	if (jointype == JOIN_SEMI || jointype == JOIN_ANTI)
// 	{
// 		/*
// 		 * SEMI or ANTI join: executor will stop after first match.
// 		 *
// 		 * Getting decent estimates requires inspection of the join quals,
// 		 * which we choose to postpone to final_cost_nestloop.
// 		 */

// 		/* Save private data for final_cost_nestloop */
// 		workspace->inner_run_cost = inner_run_cost;
// 		workspace->inner_rescan_run_cost = inner_rescan_run_cost;
// 	}
// 	else
// 	{
// 		/* Normal case; we'll scan whole input rel for each outer row */
// 		run_cost += inner_run_cost;
// 		if (outer_path_rows > 1)
// 			run_cost += (outer_path_rows - 1) * inner_rescan_run_cost;
// 	}

// 	/* CPU costs left for later */

// 	/* Public result fields */
// 	workspace->startup_cost = startup_cost;
// 	workspace->total_cost = startup_cost + run_cost;
// 	/* Save private data for final_cost_nestloop */
// 	workspace->run_cost = run_cost;
// }
// //end
// void
// final_cost_nestloop(PlannerInfo *root, NestPath *path,
// 					JoinCostWorkspace *workspace,
// 					SpecialJoinInfo *sjinfo,
// 					SemiAntiJoinFactors *semifactors)
// {
// 	Path	   *outer_path = path->outerjoinpath;
// 	Path	   *inner_path = path->innerjoinpath;
// 	double		outer_path_rows = outer_path->rows;
// 	double		inner_path_rows = inner_path->rows;
// 	Cost		startup_cost = workspace->startup_cost;
// 	Cost		run_cost = workspace->run_cost;
// 	Cost		cpu_per_tuple;
// 	QualCost	restrict_qual_cost;
// 	double		ntuples;

// 	/* Protect some assumptions below that rowcounts aren't zero or NaN */
// 	if (outer_path_rows <= 0 || isnan(outer_path_rows))
// 		outer_path_rows = 1;
// 	if (inner_path_rows <= 0 || isnan(inner_path_rows))
// 		inner_path_rows = 1;

// 	/* Mark the path with the correct row estimate */
// 	if (path->path.param_info)
// 		path->path.rows = path->path.param_info->ppi_rows;
// 	else
// 		path->path.rows = path->path.parent->rows;

// 	/* For partial paths, scale row estimate. */
// 	if (path->path.parallel_workers > 0)
// 	{
// 		double	parallel_divisor = get_parallel_divisor(&path->path);

// 		path->path.rows =
// 			clamp_row_est(path->path.rows / parallel_divisor);
// 	}

// 	/*
// 	 * We could include disable_cost in the preliminary estimate, but that
// 	 * would amount to optimizing for the case where the join method is
// 	 * disabled, which doesn't seem like the way to bet.
// 	 */
// 	if (!enable_nestloop)
// 		startup_cost += disable_cost;

// 	/* cost of inner-relation source data (we already dealt with outer rel) */

// 	if (path->jointype == JOIN_SEMI || path->jointype == JOIN_ANTI)
// 	{
// 		/*
// 		 * SEMI or ANTI join: executor will stop after first match.
// 		 */
// 		Cost		inner_run_cost = workspace->inner_run_cost;
// 		Cost		inner_rescan_run_cost = workspace->inner_rescan_run_cost;
// 		double		outer_matched_rows;
// 		Selectivity inner_scan_frac;

// 		/*
// 		 * For an outer-rel row that has at least one match, we can expect the
// 		 * inner scan to stop after a fraction 1/(match_count+1) of the inner
// 		 * rows, if the matches are evenly distributed.  Since they probably
// 		 * aren't quite evenly distributed, we apply a fuzz factor of 2.0 to
// 		 * that fraction.  (If we used a larger fuzz factor, we'd have to
// 		 * clamp inner_scan_frac to at most 1.0; but since match_count is at
// 		 * least 1, no such clamp is needed now.)
// 		 */
// 		outer_matched_rows = rint(outer_path_rows * semifactors->outer_match_frac);
// 		inner_scan_frac = 2.0 / (semifactors->match_count + 1.0);

// 		/*
// 		 * Compute number of tuples processed (not number emitted!).  First,
// 		 * account for successfully-matched outer rows.
// 		 */
// 		ntuples = outer_matched_rows * inner_path_rows * inner_scan_frac;

// 		/*
// 		 * Now we need to estimate the actual costs of scanning the inner
// 		 * relation, which may be quite a bit less than N times inner_run_cost
// 		 * due to early scan stops.  We consider two cases.  If the inner path
// 		 * is an indexscan using all the joinquals as indexquals, then an
// 		 * unmatched outer row results in an indexscan returning no rows,
// 		 * which is probably quite cheap.  Otherwise, the executor will have
// 		 * to scan the whole inner rel for an unmatched row; not so cheap.
// 		 */
// 		if (has_indexed_join_quals(path))
// 		{
// 			/*
// 			 * Successfully-matched outer rows will only require scanning
// 			 * inner_scan_frac of the inner relation.  In this case, we don't
// 			 * need to charge the full inner_run_cost even when that's more
// 			 * than inner_rescan_run_cost, because we can assume that none of
// 			 * the inner scans ever scan the whole inner relation.  So it's
// 			 * okay to assume that all the inner scan executions can be
// 			 * fractions of the full cost, even if materialization is reducing
// 			 * the rescan cost.  At this writing, it's impossible to get here
// 			 * for a materialized inner scan, so inner_run_cost and
// 			 * inner_rescan_run_cost will be the same anyway; but just in
// 			 * case, use inner_run_cost for the first matched tuple and
// 			 * inner_rescan_run_cost for additional ones.
// 			 */
// 			run_cost += inner_run_cost * inner_scan_frac;
// 			if (outer_matched_rows > 1)
// 				run_cost += (outer_matched_rows - 1) * inner_rescan_run_cost * inner_scan_frac;

// 			/*
// 			 * Add the cost of inner-scan executions for unmatched outer rows.
// 			 * We estimate this as the same cost as returning the first tuple
// 			 * of a nonempty scan.  We consider that these are all rescans,
// 			 * since we used inner_run_cost once already.
// 			 */
// 			run_cost += (outer_path_rows - outer_matched_rows) *
// 				inner_rescan_run_cost / inner_path_rows;

// 			/*
// 			 * We won't be evaluating any quals at all for unmatched rows, so
// 			 * don't add them to ntuples.
// 			 */
// 		}
// 		else
// 		{
// 			/*
// 			 * Here, a complicating factor is that rescans may be cheaper than
// 			 * first scans.  If we never scan all the way to the end of the
// 			 * inner rel, it might be (depending on the plan type) that we'd
// 			 * never pay the whole inner first-scan run cost.  However it is
// 			 * difficult to estimate whether that will happen (and it could
// 			 * not happen if there are any unmatched outer rows!), so be
// 			 * conservative and always charge the whole first-scan cost once.
// 			 */
// 			run_cost += inner_run_cost;

// 			/* Add inner run cost for additional outer tuples having matches */
// 			if (outer_matched_rows > 1)
// 				run_cost += (outer_matched_rows - 1) * inner_rescan_run_cost * inner_scan_frac;

// 			/* Add inner run cost for unmatched outer tuples */
// 			run_cost += (outer_path_rows - outer_matched_rows) *
// 				inner_rescan_run_cost;

// 			/* And count the unmatched join tuples as being processed */
// 			ntuples += (outer_path_rows - outer_matched_rows) *
// 				inner_path_rows;
// 		}
// 	}
// 	else
// 	{
// 		/* Normal-case source costs were included in preliminary estimate */

// 		/* Compute number of tuples processed (not number emitted!) */
// 		ntuples = outer_path_rows * inner_path_rows;
// 	}

// 	/* CPU costs */
// 	cost_qual_eval(&restrict_qual_cost, path->joinrestrictinfo, root);
// 	startup_cost += restrict_qual_cost.startup;
// 	cpu_per_tuple = cpu_tuple_cost + restrict_qual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * ntuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->path.pathtarget->cost.startup;
// 	run_cost += path->path.pathtarget->cost.per_tuple * path->path.rows;

// 	path->path.startup_cost = startup_cost;
// 	path->path.total_cost = startup_cost + run_cost;
// }
// //end
// void
// final_cost_mergejoin(PlannerInfo *root, MergePath *path,
// 					 JoinCostWorkspace *workspace,
// 					 SpecialJoinInfo *sjinfo)
// {
// 	Path	   *outer_path = path->jpath.outerjoinpath;
// 	Path	   *inner_path = path->jpath.innerjoinpath;
// 	double		inner_path_rows = inner_path->rows;
// 	List	   *mergeclauses = path->path_mergeclauses;
// 	List	   *innersortkeys = path->innersortkeys;
// 	Cost		startup_cost = workspace->startup_cost;
// 	Cost		run_cost = workspace->run_cost;
// 	Cost		inner_run_cost = workspace->inner_run_cost;
// 	double		outer_rows = workspace->outer_rows;
// 	double		inner_rows = workspace->inner_rows;
// 	double		outer_skip_rows = workspace->outer_skip_rows;
// 	double		inner_skip_rows = workspace->inner_skip_rows;
// 	Cost		cpu_per_tuple,
// 				bare_inner_cost,
// 				mat_inner_cost;
// 	QualCost	merge_qual_cost;
// 	QualCost	qp_qual_cost;
// 	double		mergejointuples,
// 				rescannedtuples;
// 	double		rescanratio;

// 	/* Protect some assumptions below that rowcounts aren't zero or NaN */
// 	if (inner_path_rows <= 0 || isnan(inner_path_rows))
// 		inner_path_rows = 1;

// 	/* Mark the path with the correct row estimate */
// 	if (path->jpath.path.param_info)
// 		path->jpath.path.rows = path->jpath.path.param_info->ppi_rows;
// 	else
// 		path->jpath.path.rows = path->jpath.path.parent->rows;

// 	/* For partial paths, scale row estimate. */
// 	if (path->jpath.path.parallel_workers > 0)
// 	{
// 		double	parallel_divisor = get_parallel_divisor(&path->jpath.path);

// 		path->jpath.path.rows =
// 			clamp_row_est(path->jpath.path.rows / parallel_divisor);
// 	}

// 	/*
// 	 * We could include disable_cost in the preliminary estimate, but that
// 	 * would amount to optimizing for the case where the join method is
// 	 * disabled, which doesn't seem like the way to bet.
// 	 */
// 	if (!enable_mergejoin)
// 		startup_cost += disable_cost;

// 	/*
// 	 * Compute cost of the mergequals and qpquals (other restriction clauses)
// 	 * separately.
// 	 */
// 	cost_qual_eval(&merge_qual_cost, mergeclauses, root);
// 	cost_qual_eval(&qp_qual_cost, path->jpath.joinrestrictinfo, root);
// 	qp_qual_cost.startup -= merge_qual_cost.startup;
// 	qp_qual_cost.per_tuple -= merge_qual_cost.per_tuple;

// 	/*
// 	 * Get approx # tuples passing the mergequals.  We use approx_tuple_count
// 	 * here because we need an estimate done with JOIN_INNER semantics.
// 	 */
// 	mergejointuples = approx_tuple_count(root, &path->jpath, mergeclauses);

// 	/*
// 	 * When there are equal merge keys in the outer relation, the mergejoin
// 	 * must rescan any matching tuples in the inner relation. This means
// 	 * re-fetching inner tuples; we have to estimate how often that happens.
// 	 *
// 	 * For regular inner and outer joins, the number of re-fetches can be
// 	 * estimated approximately as size of merge join output minus size of
// 	 * inner relation. Assume that the distinct key values are 1, 2, ..., and
// 	 * denote the number of values of each key in the outer relation as m1,
// 	 * m2, ...; in the inner relation, n1, n2, ...  Then we have
// 	 *
// 	 * size of join = m1 * n1 + m2 * n2 + ...
// 	 *
// 	 * number of rescanned tuples = (m1 - 1) * n1 + (m2 - 1) * n2 + ... = m1 *
// 	 * n1 + m2 * n2 + ... - (n1 + n2 + ...) = size of join - size of inner
// 	 * relation
// 	 *
// 	 * This equation works correctly for outer tuples having no inner match
// 	 * (nk = 0), but not for inner tuples having no outer match (mk = 0); we
// 	 * are effectively subtracting those from the number of rescanned tuples,
// 	 * when we should not.  Can we do better without expensive selectivity
// 	 * computations?
// 	 *
// 	 * The whole issue is moot if we are working from a unique-ified outer
// 	 * input.
// 	 */
// 	if (IsA(outer_path, UniquePath))
// 		rescannedtuples = 0;
// 	else
// 	{
// 		rescannedtuples = mergejointuples - inner_path_rows;
// 		/* Must clamp because of possible underestimate */
// 		if (rescannedtuples < 0)
// 			rescannedtuples = 0;
// 	}
// 	/* We'll inflate various costs this much to account for rescanning */
// 	rescanratio = 1.0 + (rescannedtuples / inner_path_rows);

// 	/*
// 	 * Decide whether we want to materialize the inner input to shield it from
// 	 * mark/restore and performing re-fetches.  Our cost model for regular
// 	 * re-fetches is that a re-fetch costs the same as an original fetch,
// 	 * which is probably an overestimate; but on the other hand we ignore the
// 	 * bookkeeping costs of mark/restore.  Not clear if it's worth developing
// 	 * a more refined model.  So we just need to inflate the inner run cost by
// 	 * rescanratio.
// 	 */
// 	bare_inner_cost = inner_run_cost * rescanratio;

// 	/*
// 	 * When we interpose a Material node the re-fetch cost is assumed to be
// 	 * just cpu_operator_cost per tuple, independently of the underlying
// 	 * plan's cost; and we charge an extra cpu_operator_cost per original
// 	 * fetch as well.  Note that we're assuming the materialize node will
// 	 * never spill to disk, since it only has to remember tuples back to the
// 	 * last mark.  (If there are a huge number of duplicates, our other cost
// 	 * factors will make the path so expensive that it probably won't get
// 	 * chosen anyway.)	So we don't use cost_rescan here.
// 	 *
// 	 * Note: keep this estimate in sync with create_mergejoin_plan's labeling
// 	 * of the generated Material node.
// 	 */
// 	mat_inner_cost = inner_run_cost +
// 		cpu_operator_cost * inner_path_rows * rescanratio;

// 	/*
// 	 * Prefer materializing if it looks cheaper, unless the user has asked to
// 	 * suppress materialization.
// 	 */
// 	if (enable_material && mat_inner_cost < bare_inner_cost)
// 		path->materialize_inner = true;

// 	/*
// 	 * Even if materializing doesn't look cheaper, we *must* do it if the
// 	 * inner path is to be used directly (without sorting) and it doesn't
// 	 * support mark/restore.
// 	 *
// 	 * Since the inner side must be ordered, and only Sorts and IndexScans can
// 	 * create order to begin with, and they both support mark/restore, you
// 	 * might think there's no problem --- but you'd be wrong.  Nestloop and
// 	 * merge joins can *preserve* the order of their inputs, so they can be
// 	 * selected as the input of a mergejoin, and they don't support
// 	 * mark/restore at present.
// 	 *
// 	 * We don't test the value of enable_material here, because
// 	 * materialization is required for correctness in this case, and turning
// 	 * it off does not entitle us to deliver an invalid plan.
// 	 */
// 	else if (innersortkeys == NIL &&
// 			 !ExecSupportsMarkRestore(inner_path))
// 		path->materialize_inner = true;

// 	/*
// 	 * Also, force materializing if the inner path is to be sorted and the
// 	 * sort is expected to spill to disk.  This is because the final merge
// 	 * pass can be done on-the-fly if it doesn't have to support mark/restore.
// 	 * We don't try to adjust the cost estimates for this consideration,
// 	 * though.
// 	 *
// 	 * Since materialization is a performance optimization in this case,
// 	 * rather than necessary for correctness, we skip it if enable_material is
// 	 * off.
// 	 */
// 	else if (enable_material && innersortkeys != NIL &&
// 			 relation_byte_size(inner_path_rows,
// 								inner_path->pathtarget->width) >
// 			 (work_mem * 1024L))
// 		path->materialize_inner = true;
// 	else
// 		path->materialize_inner = false;

// 	/* Charge the right incremental cost for the chosen case */
// 	if (path->materialize_inner)
// 		run_cost += mat_inner_cost;
// 	else
// 		run_cost += bare_inner_cost;

// 	/* CPU costs */

// 	/*
// 	 * The number of tuple comparisons needed is approximately number of outer
// 	 * rows plus number of inner rows plus number of rescanned tuples (can we
// 	 * refine this?).  At each one, we need to evaluate the mergejoin quals.
// 	 */
// 	startup_cost += merge_qual_cost.startup;
// 	startup_cost += merge_qual_cost.per_tuple *
// 		(outer_skip_rows + inner_skip_rows * rescanratio);
// 	run_cost += merge_qual_cost.per_tuple *
// 		((outer_rows - outer_skip_rows) +
// 		 (inner_rows - inner_skip_rows) * rescanratio);

// 	/*
// 	 * For each tuple that gets through the mergejoin proper, we charge
// 	 * cpu_tuple_cost plus the cost of evaluating additional restriction
// 	 * clauses that are to be applied at the join.  (This is pessimistic since
// 	 * not all of the quals may get evaluated at each tuple.)
// 	 *
// 	 * Note: we could adjust for SEMI/ANTI joins skipping some qual
// 	 * evaluations here, but it's probably not worth the trouble.
// 	 */
// 	startup_cost += qp_qual_cost.startup;
// 	cpu_per_tuple = cpu_tuple_cost + qp_qual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * mergejointuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->jpath.path.pathtarget->cost.startup;
// 	run_cost += path->jpath.path.pathtarget->cost.per_tuple * path->jpath.path.rows;

// 	path->jpath.path.startup_cost = startup_cost;
// 	path->jpath.path.total_cost = startup_cost + run_cost;
// }
// //end
// void
// initial_cost_hashjoin(PlannerInfo *root, JoinCostWorkspace *workspace,
// 					  JoinType jointype,
// 					  List *hashclauses,
// 					  Path *outer_path, Path *inner_path,
// 					  SpecialJoinInfo *sjinfo,
// 					  SemiAntiJoinFactors *semifactors)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;
// 	double		outer_path_rows = outer_path->rows;
// 	double		inner_path_rows = inner_path->rows;
// 	int			num_hashclauses = list_length(hashclauses);
// 	int			numbuckets;
// 	int			numbatches;
// 	int			num_skew_mcvs;

// 	/* cost of source data */
// 	startup_cost += outer_path->startup_cost;
// 	run_cost += outer_path->total_cost - outer_path->startup_cost;
// 	startup_cost += inner_path->total_cost;

// 	/*
// 	 * Cost of computing hash function: must do it once per input tuple. We
// 	 * charge one cpu_operator_cost for each column's hash function.  Also,
// 	 * tack on one cpu_tuple_cost per inner row, to model the costs of
// 	 * inserting the row into the hashtable.
// 	 *
// 	 * XXX when a hashclause is more complex than a single operator, we really
// 	 * should charge the extra eval costs of the left or right side, as
// 	 * appropriate, here.  This seems more work than it's worth at the moment.
// 	 */
// 	startup_cost += (cpu_operator_cost * num_hashclauses + cpu_tuple_cost)
// 		* inner_path_rows;
// 	run_cost += cpu_operator_cost * num_hashclauses * outer_path_rows;

// 	/*
// 	 * Get hash table size that executor would use for inner relation.
// 	 *
// 	 * XXX for the moment, always assume that skew optimization will be
// 	 * performed.  As long as SKEW_WORK_MEM_PERCENT is small, it's not worth
// 	 * trying to determine that for sure.
// 	 *
// 	 * XXX at some point it might be interesting to try to account for skew
// 	 * optimization in the cost estimate, but for now, we don't.
// 	 */
// 	ExecChooseHashTableSize(inner_path_rows,
// 							inner_path->pathtarget->width,
// 							true,		/* useskew */
// 							&numbuckets,
// 							&numbatches,
// 							&num_skew_mcvs);

// 	/*
// 	 * If inner relation is too big then we will need to "batch" the join,
// 	 * which implies writing and reading most of the tuples to disk an extra
// 	 * time.  Charge seq_page_cost per page, since the I/O should be nice and
// 	 * sequential.  Writing the inner rel counts as startup cost, all the rest
// 	 * as run cost.
// 	 */
// 	if (numbatches > 1)
// 	{
// 		double		outerpages = page_size(outer_path_rows,
// 										   outer_path->pathtarget->width);
// 		double		innerpages = page_size(inner_path_rows,
// 										   inner_path->pathtarget->width);

// 		startup_cost += seq_page_cost * innerpages;
// 		run_cost += seq_page_cost * (innerpages + 2 * outerpages);
// 	}

// 	/* CPU costs left for later */

// 	/* Public result fields */
// 	workspace->startup_cost = startup_cost;
// 	workspace->total_cost = startup_cost + run_cost;
// 	/* Save private data for final_cost_hashjoin */
// 	workspace->run_cost = run_cost;
// 	workspace->numbuckets = numbuckets;
// 	workspace->numbatches = numbatches;
// }
// //end
// void
// final_cost_hashjoin(PlannerInfo *root, HashPath *path,
// 					JoinCostWorkspace *workspace,
// 					SpecialJoinInfo *sjinfo,
// 					SemiAntiJoinFactors *semifactors)
// {
// 	Path	   *outer_path = path->jpath.outerjoinpath;
// 	Path	   *inner_path = path->jpath.innerjoinpath;
// 	double		outer_path_rows = outer_path->rows;
// 	double		inner_path_rows = inner_path->rows;
// 	List	   *hashclauses = path->path_hashclauses;
// 	Cost		startup_cost = workspace->startup_cost;
// 	Cost		run_cost = workspace->run_cost;
// 	int			numbuckets = workspace->numbuckets;
// 	int			numbatches = workspace->numbatches;
// 	Cost		cpu_per_tuple;
// 	QualCost	hash_qual_cost;
// 	QualCost	qp_qual_cost;
// 	double		hashjointuples;
// 	double		virtualbuckets;
// 	Selectivity innerbucketsize;
// 	ListCell   *hcl;

// 	/* Mark the path with the correct row estimate */
// 	if (path->jpath.path.param_info)
// 		path->jpath.path.rows = path->jpath.path.param_info->ppi_rows;
// 	else
// 		path->jpath.path.rows = path->jpath.path.parent->rows;

// 	/* For partial paths, scale row estimate. */
// 	if (path->jpath.path.parallel_workers > 0)
// 	{
// 		double	parallel_divisor = get_parallel_divisor(&path->jpath.path);

// 		path->jpath.path.rows =
// 			clamp_row_est(path->jpath.path.rows / parallel_divisor);
// 	}

// 	/*
// 	 * We could include disable_cost in the preliminary estimate, but that
// 	 * would amount to optimizing for the case where the join method is
// 	 * disabled, which doesn't seem like the way to bet.
// 	 */
// 	if (!enable_hashjoin)
// 		startup_cost += disable_cost;

// 	/* mark the path with estimated # of batches */
// 	path->num_batches = numbatches;

// 	/* and compute the number of "virtual" buckets in the whole join */
// 	virtualbuckets = (double) numbuckets *(double) numbatches;

// 	/*
// 	 * Determine bucketsize fraction for inner relation.  We use the smallest
// 	 * bucketsize estimated for any individual hashclause; this is undoubtedly
// 	 * conservative.
// 	 *
// 	 * BUT: if inner relation has been unique-ified, we can assume it's good
// 	 * for hashing.  This is important both because it's the right answer, and
// 	 * because we avoid contaminating the cache with a value that's wrong for
// 	 * non-unique-ified paths.
// 	 */
// 	if (IsA(inner_path, UniquePath))
// 		innerbucketsize = 1.0 / virtualbuckets;
// 	else
// 	{
// 		innerbucketsize = 1.0;
// 		foreach(hcl, hashclauses)
// 		{
// 			RestrictInfo *restrictinfo = (RestrictInfo *) lfirst(hcl);
// 			Selectivity thisbucketsize;

// 			Assert(IsA(restrictinfo, RestrictInfo));

// 			/*
// 			 * First we have to figure out which side of the hashjoin clause
// 			 * is the inner side.
// 			 *
// 			 * Since we tend to visit the same clauses over and over when
// 			 * planning a large query, we cache the bucketsize estimate in the
// 			 * RestrictInfo node to avoid repeated lookups of statistics.
// 			 */
// 			if (bms_is_subset(restrictinfo->right_relids,
// 							  inner_path->parent->relids))
// 			{
// 				/* righthand side is inner */
// 				thisbucketsize = restrictinfo->right_bucketsize;
// 				if (thisbucketsize < 0)
// 				{
// 					/* not cached yet */
// 					thisbucketsize =
// 						estimate_hash_bucketsize(root,
// 										   get_rightop(restrictinfo->clause),
// 												 virtualbuckets);
// 					restrictinfo->right_bucketsize = thisbucketsize;
// 				}
// 			}
// 			else
// 			{
// 				Assert(bms_is_subset(restrictinfo->left_relids,
// 									 inner_path->parent->relids));
// 				/* lefthand side is inner */
// 				thisbucketsize = restrictinfo->left_bucketsize;
// 				if (thisbucketsize < 0)
// 				{
// 					/* not cached yet */
// 					thisbucketsize =
// 						estimate_hash_bucketsize(root,
// 											get_leftop(restrictinfo->clause),
// 												 virtualbuckets);
// 					restrictinfo->left_bucketsize = thisbucketsize;
// 				}
// 			}

// 			if (innerbucketsize > thisbucketsize)
// 				innerbucketsize = thisbucketsize;
// 		}
// 	}

// 	/*
// 	 * Compute cost of the hashquals and qpquals (other restriction clauses)
// 	 * separately.
// 	 */
// 	cost_qual_eval(&hash_qual_cost, hashclauses, root);
// 	cost_qual_eval(&qp_qual_cost, path->jpath.joinrestrictinfo, root);
// 	qp_qual_cost.startup -= hash_qual_cost.startup;
// 	qp_qual_cost.per_tuple -= hash_qual_cost.per_tuple;

// 	/* CPU costs */

// 	if (path->jpath.jointype == JOIN_SEMI || path->jpath.jointype == JOIN_ANTI)
// 	{
// 		double		outer_matched_rows;
// 		Selectivity inner_scan_frac;

// 		/*
// 		 * SEMI or ANTI join: executor will stop after first match.
// 		 *
// 		 * For an outer-rel row that has at least one match, we can expect the
// 		 * bucket scan to stop after a fraction 1/(match_count+1) of the
// 		 * bucket's rows, if the matches are evenly distributed.  Since they
// 		 * probably aren't quite evenly distributed, we apply a fuzz factor of
// 		 * 2.0 to that fraction.  (If we used a larger fuzz factor, we'd have
// 		 * to clamp inner_scan_frac to at most 1.0; but since match_count is
// 		 * at least 1, no such clamp is needed now.)
// 		 */
// 		outer_matched_rows = rint(outer_path_rows * semifactors->outer_match_frac);
// 		inner_scan_frac = 2.0 / (semifactors->match_count + 1.0);

// 		startup_cost += hash_qual_cost.startup;
// 		run_cost += hash_qual_cost.per_tuple * outer_matched_rows *
// 			clamp_row_est(inner_path_rows * innerbucketsize * inner_scan_frac) * 0.5;

// 		/*
// 		 * For unmatched outer-rel rows, the picture is quite a lot different.
// 		 * In the first place, there is no reason to assume that these rows
// 		 * preferentially hit heavily-populated buckets; instead assume they
// 		 * are uncorrelated with the inner distribution and so they see an
// 		 * average bucket size of inner_path_rows / virtualbuckets.  In the
// 		 * second place, it seems likely that they will have few if any exact
// 		 * hash-code matches and so very few of the tuples in the bucket will
// 		 * actually require eval of the hash quals.  We don't have any good
// 		 * way to estimate how many will, but for the moment assume that the
// 		 * effective cost per bucket entry is one-tenth what it is for
// 		 * matchable tuples.
// 		 */
// 		run_cost += hash_qual_cost.per_tuple *
// 			(outer_path_rows - outer_matched_rows) *
// 			clamp_row_est(inner_path_rows / virtualbuckets) * 0.05;

// 		/* Get # of tuples that will pass the basic join */
// 		if (path->jpath.jointype == JOIN_SEMI)
// 			hashjointuples = outer_matched_rows;
// 		else
// 			hashjointuples = outer_path_rows - outer_matched_rows;
// 	}
// 	else
// 	{
// 		/*
// 		 * The number of tuple comparisons needed is the number of outer
// 		 * tuples times the typical number of tuples in a hash bucket, which
// 		 * is the inner relation size times its bucketsize fraction.  At each
// 		 * one, we need to evaluate the hashjoin quals.  But actually,
// 		 * charging the full qual eval cost at each tuple is pessimistic,
// 		 * since we don't evaluate the quals unless the hash values match
// 		 * exactly.  For lack of a better idea, halve the cost estimate to
// 		 * allow for that.
// 		 */
// 		startup_cost += hash_qual_cost.startup;
// 		run_cost += hash_qual_cost.per_tuple * outer_path_rows *
// 			clamp_row_est(inner_path_rows * innerbucketsize) * 0.5;

// 		/*
// 		 * Get approx # tuples passing the hashquals.  We use
// 		 * approx_tuple_count here because we need an estimate done with
// 		 * JOIN_INNER semantics.
// 		 */
// 		hashjointuples = approx_tuple_count(root, &path->jpath, hashclauses);
// 	}

// 	/*
// 	 * For each tuple that gets through the hashjoin proper, we charge
// 	 * cpu_tuple_cost plus the cost of evaluating additional restriction
// 	 * clauses that are to be applied at the join.  (This is pessimistic since
// 	 * not all of the quals may get evaluated at each tuple.)
// 	 */
// 	startup_cost += qp_qual_cost.startup;
// 	cpu_per_tuple = cpu_tuple_cost + qp_qual_cost.per_tuple;
// 	run_cost += cpu_per_tuple * hashjointuples;

// 	/* tlist eval costs are paid per output row, not per tuple scanned */
// 	startup_cost += path->jpath.path.pathtarget->cost.startup;
// 	run_cost += path->jpath.path.pathtarget->cost.per_tuple * path->jpath.path.rows;

// 	path->jpath.path.startup_cost = startup_cost;
// 	path->jpath.path.total_cost = startup_cost + run_cost;
// }
// //end
// void
// cost_gather(GatherPath *path, PlannerInfo *root,
// 			RelOptInfo *rel, ParamPathInfo *param_info,
// 			double *rows)
// {
// 	Cost		startup_cost = 0;
// 	Cost		run_cost = 0;

// 	/* Mark the path with the correct row estimate */
// 	if (rows)
// 		path->path.rows = *rows;
// 	else if (param_info)
// 		path->path.rows = param_info->ppi_rows;
// 	else
// 		path->path.rows = rel->rows;

// 	startup_cost = path->subpath->startup_cost;

// 	run_cost = path->subpath->total_cost - path->subpath->startup_cost;

// 	/* Parallel setup and communication cost. */
// 	startup_cost += parallel_setup_cost;
// 	run_cost += parallel_tuple_cost * path->path.rows;

// 	path->path.startup_cost = startup_cost;
// 	path->path.total_cost = (startup_cost + run_cost);
// }
// //end
// //end cost.h

// //optimizer/plancat.c
// void
// estimate_rel_size(Relation rel, int32 *attr_widths,
// 				  BlockNumber *pages, double *tuples, double *allvisfrac)
// {
// 	BlockNumber curpages;
// 	BlockNumber relpages;
// 	double		reltuples;
// 	BlockNumber relallvisible;
// 	double		density;

// 	switch (rel->rd_rel->relkind)
// 	{
// 		case RELKIND_RELATION:
// 		case RELKIND_INDEX:
// 		case RELKIND_MATVIEW:
// 		case RELKIND_TOASTVALUE:
// 			/* it has storage, ok to call the smgr */
// 			curpages = RelationGetNumberOfBlocks(rel);

// 			/*
// 			 * HACK: if the relation has never yet been vacuumed, use a
// 			 * minimum size estimate of 10 pages.  The idea here is to avoid
// 			 * assuming a newly-created table is really small, even if it
// 			 * currently is, because that may not be true once some data gets
// 			 * loaded into it.  Once a vacuum or analyze cycle has been done
// 			 * on it, it's more reasonable to believe the size is somewhat
// 			 * stable.
// 			 *
// 			 * (Note that this is only an issue if the plan gets cached and
// 			 * used again after the table has been filled.  What we're trying
// 			 * to avoid is using a nestloop-type plan on a table that has
// 			 * grown substantially since the plan was made.  Normally,
// 			 * autovacuum/autoanalyze will occur once enough inserts have
// 			 * happened and cause cached-plan invalidation; but that doesn't
// 			 * happen instantaneously, and it won't happen at all for cases
// 			 * such as temporary tables.)
// 			 *
// 			 * We approximate "never vacuumed" by "has relpages = 0", which
// 			 * means this will also fire on genuinely empty relations.  Not
// 			 * great, but fortunately that's a seldom-seen case in the real
// 			 * world, and it shouldn't degrade the quality of the plan too
// 			 * much anyway to err in this direction.
// 			 *
// 			 * There are two exceptions wherein we don't apply this heuristic.
// 			 * One is if the table has inheritance children.  Totally empty
// 			 * parent tables are quite common, so we should be willing to
// 			 * believe that they are empty.  Also, we don't apply the 10-page
// 			 * minimum to indexes.
// 			 */
// 			if (curpages < 10 &&
// 				rel->rd_rel->relpages == 0 &&
// 				!rel->rd_rel->relhassubclass &&
// 				rel->rd_rel->relkind != RELKIND_INDEX)
// 				curpages = 10;

// 			/* report estimated # pages */
// 			*pages = curpages;
// 			/* quick exit if rel is clearly empty */
// 			if (curpages == 0)
// 			{
// 				*tuples = 0;
// 				*allvisfrac = 0;
// 				break;
// 			}
// 			/* coerce values in pg_class to more desirable types */
// 			relpages = (BlockNumber) rel->rd_rel->relpages;
// 			reltuples = (double) rel->rd_rel->reltuples;
// 			relallvisible = (BlockNumber) rel->rd_rel->relallvisible;

// 			/*
// 			 * If it's an index, discount the metapage while estimating the
// 			 * number of tuples.  This is a kluge because it assumes more than
// 			 * it ought to about index structure.  Currently it's OK for
// 			 * btree, hash, and GIN indexes but suspect for GiST indexes.
// 			 */
// 			if (rel->rd_rel->relkind == RELKIND_INDEX &&
// 				relpages > 0)
// 			{
// 				curpages--;
// 				relpages--;
// 			}

// 			/* estimate number of tuples from previous tuple density */
// 			if (relpages > 0)
// 				density = reltuples / (double) relpages;
// 			else
// 			{
// 				/*
// 				 * When we have no data because the relation was truncated,
// 				 * estimate tuple width from attribute datatypes.  We assume
// 				 * here that the pages are completely full, which is OK for
// 				 * tables (since they've presumably not been VACUUMed yet) but
// 				 * is probably an overestimate for indexes.  Fortunately
// 				 * get_relation_info() can clamp the overestimate to the
// 				 * parent table's size.
// 				 *
// 				 * Note: this code intentionally disregards alignment
// 				 * considerations, because (a) that would be gilding the lily
// 				 * considering how crude the estimate is, and (b) it creates
// 				 * platform dependencies in the default plans which are kind
// 				 * of a headache for regression testing.
// 				 */
// 				int32		tuple_width;

// 				tuple_width = get_rel_data_width(rel, attr_widths);
// 				tuple_width += MAXALIGN(SizeofHeapTupleHeader);
// 				tuple_width += sizeof(ItemIdData);
// 				/* note: integer division is intentional here */
// 				density = (BLCKSZ - SizeOfPageHeaderData) / tuple_width;
// 			}
// 			*tuples = rint(density * (double) curpages);

// 			/*
// 			 * We use relallvisible as-is, rather than scaling it up like we
// 			 * do for the pages and tuples counts, on the theory that any
// 			 * pages added since the last VACUUM are most likely not marked
// 			 * all-visible.  But costsize.c wants it converted to a fraction.
// 			 */
// 			if (relallvisible == 0 || curpages <= 0)
// 				*allvisfrac = 0;
// 			else if ((double) relallvisible >= curpages)
// 				*allvisfrac = 1;
// 			else
// 				*allvisfrac = (double) relallvisible / curpages;
// 			break;
// 		case RELKIND_SEQUENCE:
// 			/* Sequences always have a known size */
// 			*pages = 1;
// 			*tuples = 1;
// 			*allvisfrac = 0;
// 			break;
// 		case RELKIND_FOREIGN_TABLE:
// 			/* Just use whatever's in pg_class */
// 			*pages = rel->rd_rel->relpages;
// 			*tuples = rel->rd_rel->reltuples;
// 			*allvisfrac = 0;
// 			break;
// 		default:
// 			/* else it has no disk storage; probably shouldn't get here? */
// 			*pages = 0;
// 			*tuples = 0;
// 			*allvisfrac = 0;
// 			break;
// 	}
// }
// //end
// int32
// get_relation_data_width(Oid relid, int32 *attr_widths)
// {
// 	int32		result;
// 	Relation	relation;

// 	/* As above, assume relation is already locked */
// 	relation = heap_open(relid, NoLock);

// 	result = get_rel_data_width(relation, attr_widths);

// 	heap_close(relation, NoLock);

// 	return result;
// }
// //end
// List *
// infer_arbiter_indexes(PlannerInfo *root)
// {
// 	OnConflictExpr *onconflict = root->parse->onConflict;

// 	/* Iteration state */
// 	Relation	relation;
// 	Oid			relationObjectId;
// 	Oid			indexOidFromConstraint = InvalidOid;
// 	List	   *indexList;
// 	ListCell   *l;

// 	/* Normalized inference attributes and inference expressions: */
// 	Bitmapset  *inferAttrs = NULL;
// 	List	   *inferElems = NIL;

// 	/* Results */
// 	List	   *results = NIL;

// 	/*
// 	 * Quickly return NIL for ON CONFLICT DO NOTHING without an inference
// 	 * specification or named constraint.  ON CONFLICT DO UPDATE statements
// 	 * must always provide one or the other (but parser ought to have caught
// 	 * that already).
// 	 */
// 	if (onconflict->arbiterElems == NIL &&
// 		onconflict->constraint == InvalidOid)
// 		return NIL;

// 	/*
// 	 * We need not lock the relation since it was already locked, either by
// 	 * the rewriter or when expand_inherited_rtentry() added it to the query's
// 	 * rangetable.
// 	 */
// 	relationObjectId = rt_fetch(root->parse->resultRelation,
// 								root->parse->rtable)->relid;

// 	relation = heap_open(relationObjectId, NoLock);

// 	/*
// 	 * Build normalized/BMS representation of plain indexed attributes, as
// 	 * well as a separate list of expression items.  This simplifies matching
// 	 * the cataloged definition of indexes.
// 	 */
// 	foreach(l, onconflict->arbiterElems)
// 	{
// 		InferenceElem *elem = (InferenceElem *) lfirst(l);
// 		Var		   *var;
// 		int			attno;

// 		if (!IsA(elem->expr, Var))
// 		{
// 			/* If not a plain Var, just shove it in inferElems for now */
// 			inferElems = lappend(inferElems, elem->expr);
// 			continue;
// 		}

// 		var = (Var *) elem->expr;
// 		attno = var->varattno;

// 		if (attno == 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("whole row unique index inference specifications are not supported")));

// 		inferAttrs = bms_add_member(inferAttrs,
// 								 attno - FirstLowInvalidHeapAttributeNumber);
// 	}

// 	/*
// 	 * Lookup named constraint's index.  This is not immediately returned
// 	 * because some additional sanity checks are required.
// 	 */
// 	if (onconflict->constraint != InvalidOid)
// 	{
// 		indexOidFromConstraint = get_constraint_index(onconflict->constraint);

// 		if (indexOidFromConstraint == InvalidOid)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("constraint in ON CONFLICT clause has no associated index")));
// 	}

// 	/*
// 	 * Using that representation, iterate through the list of indexes on the
// 	 * target relation to try and find a match
// 	 */
// 	indexList = RelationGetIndexList(relation);

// 	foreach(l, indexList)
// 	{
// 		Oid			indexoid = lfirst_oid(l);
// 		Relation	idxRel;
// 		Form_pg_index idxForm;
// 		Bitmapset  *indexedAttrs;
// 		List	   *idxExprs;
// 		List	   *predExprs;
// 		AttrNumber	natt;
// 		ListCell   *el;

// 		/*
// 		 * Extract info from the relation descriptor for the index.  We know
// 		 * that this is a target, so get lock type it is known will ultimately
// 		 * be required by the executor.
// 		 *
// 		 * Let executor complain about !indimmediate case directly, because
// 		 * enforcement needs to occur there anyway when an inference clause is
// 		 * omitted.
// 		 */
// 		idxRel = index_open(indexoid, RowExclusiveLock);
// 		idxForm = idxRel->rd_index;

// 		if (!IndexIsValid(idxForm))
// 			goto next;

// 		/*
// 		 * Note that we do not perform a check against indcheckxmin (like e.g.
// 		 * get_relation_info()) here to eliminate candidates, because
// 		 * uniqueness checking only cares about the most recently committed
// 		 * tuple versions.
// 		 */

// 		/*
// 		 * Look for match on "ON constraint_name" variant, which may not be
// 		 * unique constraint.  This can only be a constraint name.
// 		 */
// 		if (indexOidFromConstraint == idxForm->indexrelid)
// 		{
// 			if (!idxForm->indisunique && onconflict->action == ONCONFLICT_UPDATE)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 						 errmsg("ON CONFLICT DO UPDATE not supported with exclusion constraints")));

// 			results = lappend_oid(results, idxForm->indexrelid);
// 			list_free(indexList);
// 			index_close(idxRel, NoLock);
// 			heap_close(relation, NoLock);
// 			return results;
// 		}
// 		else if (indexOidFromConstraint != InvalidOid)
// 		{
// 			/* No point in further work for index in named constraint case */
// 			goto next;
// 		}

// 		/*
// 		 * Only considering conventional inference at this point (not named
// 		 * constraints), so index under consideration can be immediately
// 		 * skipped if it's not unique
// 		 */
// 		if (!idxForm->indisunique)
// 			goto next;

// 		/* Build BMS representation of plain (non expression) index attrs */
// 		indexedAttrs = NULL;
// 		for (natt = 0; natt < idxForm->indnatts; natt++)
// 		{
// 			int			attno = idxRel->rd_index->indkey.values[natt];

// 			if (attno != 0)
// 				indexedAttrs = bms_add_member(indexedAttrs,
// 								 attno - FirstLowInvalidHeapAttributeNumber);
// 		}

// 		/* Non-expression attributes (if any) must match */
// 		if (!bms_equal(indexedAttrs, inferAttrs))
// 			goto next;

// 		/* Expression attributes (if any) must match */
// 		idxExprs = RelationGetIndexExpressions(idxRel);
// 		foreach(el, onconflict->arbiterElems)
// 		{
// 			InferenceElem *elem = (InferenceElem *) lfirst(el);

// 			/*
// 			 * Ensure that collation/opclass aspects of inference expression
// 			 * element match.  Even though this loop is primarily concerned
// 			 * with matching expressions, it is a convenient point to check
// 			 * this for both expressions and ordinary (non-expression)
// 			 * attributes appearing as inference elements.
// 			 */
// 			if (!infer_collation_opclass_match(elem, idxRel, idxExprs))
// 				goto next;

// 			/*
// 			 * Plain Vars don't factor into count of expression elements, and
// 			 * the question of whether or not they satisfy the index
// 			 * definition has already been considered (they must).
// 			 */
// 			if (IsA(elem->expr, Var))
// 				continue;

// 			/*
// 			 * Might as well avoid redundant check in the rare cases where
// 			 * infer_collation_opclass_match() is required to do real work.
// 			 * Otherwise, check that element expression appears in cataloged
// 			 * index definition.
// 			 */
// 			if (elem->infercollid != InvalidOid ||
// 				elem->inferopclass != InvalidOid ||
// 				list_member(idxExprs, elem->expr))
// 				continue;

// 			goto next;
// 		}

// 		/*
// 		 * Now that all inference elements were matched, ensure that the
// 		 * expression elements from inference clause are not missing any
// 		 * cataloged expressions.  This does the right thing when unique
// 		 * indexes redundantly repeat the same attribute, or if attributes
// 		 * redundantly appear multiple times within an inference clause.
// 		 */
// 		if (list_difference(idxExprs, inferElems) != NIL)
// 			goto next;

// 		/*
// 		 * If it's a partial index, its predicate must be implied by the ON
// 		 * CONFLICT's WHERE clause.
// 		 */
// 		predExprs = RelationGetIndexPredicate(idxRel);

// 		if (!predicate_implied_by(predExprs, (List *) onconflict->arbiterWhere))
// 			goto next;

// 		results = lappend_oid(results, idxForm->indexrelid);
// next:
// 		index_close(idxRel, NoLock);
// 	}

// 	list_free(indexList);
// 	heap_close(relation, NoLock);

// 	if (results == NIL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
// 				 errmsg("there is no unique or exclusion constraint matching the ON CONFLICT specification")));

// 	return results;
// }
// //end
// bool
// has_row_triggers(PlannerInfo *root, Index rti, CmdType event)
// {
// 	RangeTblEntry *rte = planner_rt_fetch(rti, root);
// 	Relation	relation;
// 	TriggerDesc *trigDesc;
// 	bool		result = false;

// 	/* Assume we already have adequate lock */
// 	relation = heap_open(rte->relid, NoLock);

// 	trigDesc = relation->trigdesc;
// 	switch (event)
// 	{
// 		case CMD_INSERT:
// 			if (trigDesc &&
// 				(trigDesc->trig_insert_after_row ||
// 				 trigDesc->trig_insert_before_row))
// 				result = true;
// 			break;
// 		case CMD_UPDATE:
// 			if (trigDesc &&
// 				(trigDesc->trig_update_after_row ||
// 				 trigDesc->trig_update_before_row))
// 				result = true;
// 			break;
// 		case CMD_DELETE:
// 			if (trigDesc &&
// 				(trigDesc->trig_delete_after_row ||
// 				 trigDesc->trig_delete_before_row))
// 				result = true;
// 			break;
// 		default:
// 			elog(ERROR, "unrecognized CmdType: %d", (int) event);
// 			break;
// 	}

// 	heap_close(relation, NoLock);
// 	return result;
// }
// //end
// //end plancat.c

// //optimizer/planner.c
// PlannerInfo *
// subquery_planner(PlannerGlobal *glob, Query *parse,
// 				 PlannerInfo *parent_root,
// 				 bool hasRecursion, double tuple_fraction)
// {
// 	PlannerInfo *root;
// 	List	   *newWithCheckOptions;
// 	List	   *newHaving;
// 	bool		hasOuterJoins;
// 	RelOptInfo *final_rel;
// 	ListCell   *l;

// 	/* Create a PlannerInfo data structure for this subquery */
// 	root = makeNode(PlannerInfo);
// 	root->parse = parse;
// 	root->glob = glob;
// 	root->query_level = parent_root ? parent_root->query_level + 1 : 1;
// 	root->parent_root = parent_root;
// 	root->plan_params = NIL;
// 	root->outer_params = NULL;
// 	root->planner_cxt = CurrentMemoryContext;
// 	root->init_plans = NIL;
// 	root->cte_plan_ids = NIL;
// 	root->multiexpr_params = NIL;
// 	root->eq_classes = NIL;
// 	root->append_rel_list = NIL;
// 	root->rowMarks = NIL;
// 	memset(root->upper_rels, 0, sizeof(root->upper_rels));
// 	memset(root->upper_targets, 0, sizeof(root->upper_targets));
// 	root->processed_tlist = NIL;
// 	root->grouping_map = NULL;
// 	root->minmax_aggs = NIL;
// 	root->hasInheritedTarget = false;
// 	root->hasRecursion = hasRecursion;
// 	if (hasRecursion)
// 		root->wt_param_id = SS_assign_special_param(root);
// 	else
// 		root->wt_param_id = -1;
// 	root->non_recursive_path = NULL;

// 	/*
// 	 * If there is a WITH list, process each WITH query and build an initplan
// 	 * SubPlan structure for it.
// 	 */
// 	if (parse->cteList)
// 		SS_process_ctes(root);

// 	/*
// 	 * Look for ANY and EXISTS SubLinks in WHERE and JOIN/ON clauses, and try
// 	 * to transform them into joins.  Note that this step does not descend
// 	 * into subqueries; if we pull up any subqueries below, their SubLinks are
// 	 * processed just before pulling them up.
// 	 */
// 	if (parse->hasSubLinks)
// 		pull_up_sublinks(root);

// 	/*
// 	 * Scan the rangetable for set-returning functions, and inline them if
// 	 * possible (producing subqueries that might get pulled up next).
// 	 * Recursion issues here are handled in the same way as for SubLinks.
// 	 */
// 	inline_set_returning_functions(root);

// 	/*
// 	 * Check to see if any subqueries in the jointree can be merged into this
// 	 * query.
// 	 */
// 	pull_up_subqueries(root);

// 	/*
// 	 * If this is a simple UNION ALL query, flatten it into an appendrel. We
// 	 * do this now because it requires applying pull_up_subqueries to the leaf
// 	 * queries of the UNION ALL, which weren't touched above because they
// 	 * weren't referenced by the jointree (they will be after we do this).
// 	 */
// 	if (parse->setOperations)
// 		flatten_simple_union_all(root);

// 	/*
// 	 * Detect whether any rangetable entries are RTE_JOIN kind; if not, we can
// 	 * avoid the expense of doing flatten_join_alias_vars().  Also check for
// 	 * outer joins --- if none, we can skip reduce_outer_joins().  And check
// 	 * for LATERAL RTEs, too.  This must be done after we have done
// 	 * pull_up_subqueries(), of course.
// 	 */
// 	root->hasJoinRTEs = false;
// 	root->hasLateralRTEs = false;
// 	hasOuterJoins = false;
// 	foreach(l, parse->rtable)
// 	{
// 		RangeTblEntry *rte = (RangeTblEntry *) lfirst(l);

// 		if (rte->rtekind == RTE_JOIN)
// 		{
// 			root->hasJoinRTEs = true;
// 			if (IS_OUTER_JOIN(rte->jointype))
// 				hasOuterJoins = true;
// 		}
// 		if (rte->lateral)
// 			root->hasLateralRTEs = true;
// 	}

// 	/*
// 	 * Preprocess RowMark information.  We need to do this after subquery
// 	 * pullup (so that all non-inherited RTEs are present) and before
// 	 * inheritance expansion (so that the info is available for
// 	 * expand_inherited_tables to examine and modify).
// 	 */
// 	preprocess_rowmarks(root);

// 	/*
// 	 * Expand any rangetable entries that are inheritance sets into "append
// 	 * relations".  This can add entries to the rangetable, but they must be
// 	 * plain base relations not joins, so it's OK (and marginally more
// 	 * efficient) to do it after checking for join RTEs.  We must do it after
// 	 * pulling up subqueries, else we'd fail to handle inherited tables in
// 	 * subqueries.
// 	 */
// 	expand_inherited_tables(root);

// 	/*
// 	 * Set hasHavingQual to remember if HAVING clause is present.  Needed
// 	 * because preprocess_expression will reduce a constant-true condition to
// 	 * an empty qual list ... but "HAVING TRUE" is not a semantic no-op.
// 	 */
// 	root->hasHavingQual = (parse->havingQual != NULL);

// 	/* Clear this flag; might get set in distribute_qual_to_rels */
// 	root->hasPseudoConstantQuals = false;

// 	/*
// 	 * Do expression preprocessing on targetlist and quals, as well as other
// 	 * random expressions in the querytree.  Note that we do not need to
// 	 * handle sort/group expressions explicitly, because they are actually
// 	 * part of the targetlist.
// 	 */
// 	parse->targetList = (List *)
// 		preprocess_expression(root, (Node *) parse->targetList,
// 							  EXPRKIND_TARGET);

// 	newWithCheckOptions = NIL;
// 	foreach(l, parse->withCheckOptions)
// 	{
// 		WithCheckOption *wco = (WithCheckOption *) lfirst(l);

// 		wco->qual = preprocess_expression(root, wco->qual,
// 										  EXPRKIND_QUAL);
// 		if (wco->qual != NULL)
// 			newWithCheckOptions = lappend(newWithCheckOptions, wco);
// 	}
// 	parse->withCheckOptions = newWithCheckOptions;

// 	parse->returningList = (List *)
// 		preprocess_expression(root, (Node *) parse->returningList,
// 							  EXPRKIND_TARGET);

// 	preprocess_qual_conditions(root, (Node *) parse->jointree);

// 	parse->havingQual = preprocess_expression(root, parse->havingQual,
// 											  EXPRKIND_QUAL);

// 	foreach(l, parse->windowClause)
// 	{
// 		WindowClause *wc = (WindowClause *) lfirst(l);

// 		/* partitionClause/orderClause are sort/group expressions */
// 		wc->startOffset = preprocess_expression(root, wc->startOffset,
// 												EXPRKIND_LIMIT);
// 		wc->endOffset = preprocess_expression(root, wc->endOffset,
// 											  EXPRKIND_LIMIT);
// 	}

// 	parse->limitOffset = preprocess_expression(root, parse->limitOffset,
// 											   EXPRKIND_LIMIT);
// 	parse->limitCount = preprocess_expression(root, parse->limitCount,
// 											  EXPRKIND_LIMIT);

// 	if (parse->onConflict)
// 	{
// 		parse->onConflict->arbiterElems = (List *)
// 			preprocess_expression(root,
// 								  (Node *) parse->onConflict->arbiterElems,
// 								  EXPRKIND_ARBITER_ELEM);
// 		parse->onConflict->arbiterWhere =
// 			preprocess_expression(root,
// 								  parse->onConflict->arbiterWhere,
// 								  EXPRKIND_QUAL);
// 		parse->onConflict->onConflictSet = (List *)
// 			preprocess_expression(root,
// 								  (Node *) parse->onConflict->onConflictSet,
// 								  EXPRKIND_TARGET);
// 		parse->onConflict->onConflictWhere =
// 			preprocess_expression(root,
// 								  parse->onConflict->onConflictWhere,
// 								  EXPRKIND_QUAL);
// 		/* exclRelTlist contains only Vars, so no preprocessing needed */
// 	}

// 	root->append_rel_list = (List *)
// 		preprocess_expression(root, (Node *) root->append_rel_list,
// 							  EXPRKIND_APPINFO);

// 	/* Also need to preprocess expressions within RTEs */
// 	foreach(l, parse->rtable)
// 	{
// 		RangeTblEntry *rte = (RangeTblEntry *) lfirst(l);
// 		int			kind;

// 		if (rte->rtekind == RTE_RELATION)
// 		{
// 			if (rte->tablesample)
// 				rte->tablesample = (TableSampleClause *)
// 					preprocess_expression(root,
// 										  (Node *) rte->tablesample,
// 										  EXPRKIND_TABLESAMPLE);
// 		}
// 		else if (rte->rtekind == RTE_SUBQUERY)
// 		{
// 			/*
// 			 * We don't want to do all preprocessing yet on the subquery's
// 			 * expressions, since that will happen when we plan it.  But if it
// 			 * contains any join aliases of our level, those have to get
// 			 * expanded now, because planning of the subquery won't do it.
// 			 * That's only possible if the subquery is LATERAL.
// 			 */
// 			if (rte->lateral && root->hasJoinRTEs)
// 				rte->subquery = (Query *)
// 					flatten_join_alias_vars(root, (Node *) rte->subquery);
// 		}
// 		else if (rte->rtekind == RTE_FUNCTION)
// 		{
// 			/* Preprocess the function expression(s) fully */
// 			kind = rte->lateral ? EXPRKIND_RTFUNC_LATERAL : EXPRKIND_RTFUNC;
// 			rte->functions = (List *) preprocess_expression(root, (Node *) rte->functions, kind);
// 		}
// 		else if (rte->rtekind == RTE_VALUES)
// 		{
// 			/* Preprocess the values lists fully */
// 			kind = rte->lateral ? EXPRKIND_VALUES_LATERAL : EXPRKIND_VALUES;
// 			rte->values_lists = (List *)
// 				preprocess_expression(root, (Node *) rte->values_lists, kind);
// 		}
// 	}

// 	/*
// 	 * In some cases we may want to transfer a HAVING clause into WHERE. We
// 	 * cannot do so if the HAVING clause contains aggregates (obviously) or
// 	 * volatile functions (since a HAVING clause is supposed to be executed
// 	 * only once per group).  We also can't do this if there are any nonempty
// 	 * grouping sets; moving such a clause into WHERE would potentially change
// 	 * the results, if any referenced column isn't present in all the grouping
// 	 * sets.  (If there are only empty grouping sets, then the HAVING clause
// 	 * must be degenerate as discussed below.)
// 	 *
// 	 * Also, it may be that the clause is so expensive to execute that we're
// 	 * better off doing it only once per group, despite the loss of
// 	 * selectivity.  This is hard to estimate short of doing the entire
// 	 * planning process twice, so we use a heuristic: clauses containing
// 	 * subplans are left in HAVING.  Otherwise, we move or copy the HAVING
// 	 * clause into WHERE, in hopes of eliminating tuples before aggregation
// 	 * instead of after.
// 	 *
// 	 * If the query has explicit grouping then we can simply move such a
// 	 * clause into WHERE; any group that fails the clause will not be in the
// 	 * output because none of its tuples will reach the grouping or
// 	 * aggregation stage.  Otherwise we must have a degenerate (variable-free)
// 	 * HAVING clause, which we put in WHERE so that query_planner() can use it
// 	 * in a gating Result node, but also keep in HAVING to ensure that we
// 	 * don't emit a bogus aggregated row. (This could be done better, but it
// 	 * seems not worth optimizing.)
// 	 *
// 	 * Note that both havingQual and parse->jointree->quals are in
// 	 * implicitly-ANDed-list form at this point, even though they are declared
// 	 * as Node *.
// 	 */
// 	newHaving = NIL;
// 	foreach(l, (List *) parse->havingQual)
// 	{
// 		Node	   *havingclause = (Node *) lfirst(l);

// 		if ((parse->groupClause && parse->groupingSets) ||
// 			contain_agg_clause(havingclause) ||
// 			contain_volatile_functions(havingclause) ||
// 			contain_subplans(havingclause))
// 		{
// 			/* keep it in HAVING */
// 			newHaving = lappend(newHaving, havingclause);
// 		}
// 		else if (parse->groupClause && !parse->groupingSets)
// 		{
// 			/* move it to WHERE */
// 			parse->jointree->quals = (Node *)
// 				lappend((List *) parse->jointree->quals, havingclause);
// 		}
// 		else
// 		{
// 			/* put a copy in WHERE, keep it in HAVING */
// 			parse->jointree->quals = (Node *)
// 				lappend((List *) parse->jointree->quals,
// 						copyObject(havingclause));
// 			newHaving = lappend(newHaving, havingclause);
// 		}
// 	}
// 	parse->havingQual = (Node *) newHaving;

// 	/* Remove any redundant GROUP BY columns */
// 	remove_useless_groupby_columns(root);

// 	/*
// 	 * If we have any outer joins, try to reduce them to plain inner joins.
// 	 * This step is most easily done after we've done expression
// 	 * preprocessing.
// 	 */
// 	if (hasOuterJoins)
// 		reduce_outer_joins(root);

// 	/*
// 	 * Do the main planning.  If we have an inherited target relation, that
// 	 * needs special processing, else go straight to grouping_planner.
// 	 */
// 	if (parse->resultRelation &&
// 		rt_fetch(parse->resultRelation, parse->rtable)->inh)
// 		inheritance_planner(root);
// 	else
// 		grouping_planner(root, false, tuple_fraction);

// 	/*
// 	 * Capture the set of outer-level param IDs we have access to, for use in
// 	 * extParam/allParam calculations later.
// 	 */
// 	SS_identify_outer_params(root);

// 	/*
// 	 * If any initPlans were created in this query level, adjust the surviving
// 	 * Paths' costs and parallel-safety flags to account for them.  The
// 	 * initPlans won't actually get attached to the plan tree till
// 	 * create_plan() runs, but we must include their effects now.
// 	 */
// 	final_rel = fetch_upper_rel(root, UPPERREL_FINAL, NULL);
// 	SS_charge_for_initplans(root, final_rel);

// 	/*
// 	 * Make sure we've identified the cheapest Path for the final rel.  (By
// 	 * doing this here not in grouping_planner, we include initPlan costs in
// 	 * the decision, though it's unlikely that will change anything.)
// 	 */
// 	set_cheapest(final_rel);

// 	return root;
// }
// //end
// RowMarkType
// select_rowmark_type(RangeTblEntry *rte, LockClauseStrength strength)
// {
// 	if (rte->rtekind != RTE_RELATION)
// 	{
// 		/* If it's not a table at all, use ROW_MARK_COPY */
// 		return ROW_MARK_COPY;
// 	}
// 	else if (rte->relkind == RELKIND_FOREIGN_TABLE)
// 	{
// 		/* Let the FDW select the rowmark type, if it wants to */
// 		FdwRoutine *fdwroutine = GetFdwRoutineByRelId(rte->relid);

// 		if (fdwroutine->GetForeignRowMarkType != NULL)
// 			return fdwroutine->GetForeignRowMarkType(rte, strength);
// 		/* Otherwise, use ROW_MARK_COPY by default */
// 		return ROW_MARK_COPY;
// 	}
// 	else
// 	{
// 		/* Regular table, apply the appropriate lock type */
// 		switch (strength)
// 		{
// 			case LCS_NONE:

// 				/*
// 				 * We don't need a tuple lock, only the ability to re-fetch
// 				 * the row.  Regular tables support ROW_MARK_REFERENCE, but if
// 				 * this RTE has security barrier quals, it will be turned into
// 				 * a subquery during planning, so use ROW_MARK_COPY.
// 				 *
// 				 * This is only necessary for LCS_NONE, since real tuple locks
// 				 * on an RTE with security barrier quals are supported by
// 				 * pushing the lock down into the subquery --- see
// 				 * expand_security_qual.
// 				 */
// 				if (rte->securityQuals != NIL)
// 					return ROW_MARK_COPY;
// 				return ROW_MARK_REFERENCE;
// 				break;
// 			case LCS_FORKEYSHARE:
// 				return ROW_MARK_KEYSHARE;
// 				break;
// 			case LCS_FORSHARE:
// 				return ROW_MARK_SHARE;
// 				break;
// 			case LCS_FORNOKEYUPDATE:
// 				return ROW_MARK_NOKEYEXCLUSIVE;
// 				break;
// 			case LCS_FORUPDATE:
// 				return ROW_MARK_EXCLUSIVE;
// 				break;
// 		}
// 		elog(ERROR, "unrecognized LockClauseStrength %d", (int) strength);
// 		return ROW_MARK_EXCLUSIVE;		/* keep compiler quiet */
// 	}
// }
// //end
// void
// mark_partial_aggref(Aggref *agg, AggSplit aggsplit)
// {
// 	/* aggtranstype should be computed by this point */
// 	Assert(OidIsValid(agg->aggtranstype));
// 	/* ... but aggsplit should still be as the parser left it */
// 	Assert(agg->aggsplit == AGGSPLIT_SIMPLE);

// 	/* Mark the Aggref with the intended partial-aggregation mode */
// 	agg->aggsplit = aggsplit;

// 	/*
// 	 * Adjust result type if needed.  Normally, a partial aggregate returns
// 	 * the aggregate's transition type; but if that's INTERNAL and we're
// 	 * serializing, it returns BYTEA instead.
// 	 */
// 	if (DO_AGGSPLIT_SKIPFINAL(aggsplit))
// 	{
// 		if (agg->aggtranstype == INTERNALOID && DO_AGGSPLIT_SERIALIZE(aggsplit))
// 			agg->aggtype = BYTEAOID;
// 		else
// 			agg->aggtype = agg->aggtranstype;
// 	}
// }
// //end
// Path *
// get_cheapest_fractional_path(RelOptInfo *rel, double tuple_fraction)
// {
// 	Path	   *best_path = rel->cheapest_total_path;
// 	ListCell   *l;

// 	/* If all tuples will be retrieved, just return the cheapest-total path */
// 	if (tuple_fraction <= 0.0)
// 		return best_path;

// 	/* Convert absolute # of tuples to a fraction; no need to clamp to 0..1 */
// 	if (tuple_fraction >= 1.0 && best_path->rows > 0)
// 		tuple_fraction /= best_path->rows;

// 	foreach(l, rel->pathlist)
// 	{
// 		Path	   *path = (Path *) lfirst(l);

// 		if (path == rel->cheapest_total_path ||
// 		 compare_fractional_path_costs(best_path, path, tuple_fraction) <= 0)
// 			continue;

// 		best_path = path;
// 	}

// 	return best_path;
// }
// //end
// //end planner.c
