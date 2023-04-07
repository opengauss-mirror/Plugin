//#include "tsdb.h"
#include "compat.h"

#include "commands/cluster.h"
//#include "gtm/utils/pqsignal.h"
#include "utils.h"


//#include "/home/lhy/og/openGauss-server/src/common/port/qsort.cpp"
#include "access/skey.h"
#include "commands/extension.h"
#include "access/heapam.h"

#include "knl/knl_session.h"
#include "knl/knl_guc/knl_session_attr_sql.h"
#include "knl/knl_guc/knl_instance_attr_common.h"
#include "knl/knl_thread.h"

#include "utils/syscache.h"
#include "commands/copy.h"

#include "catalog/pg_type_fn.h"
#include "catalog/pg_ts_config.h"

#include "utils/dynamic_loader.h"
#include "utils/globalplancore.h"

#include "compat/tableam.h"
#include "utils/guc_tables.h"
#include "utils/array.h"
#include "commands/explain.h"
#include "compression/compression.h"
#include "compression/array.h"
#include "compression/dictionary.h"
#include "compression/gorilla.h"
#include "compression/deltadelta.h"

#include "catalog/pg_cast.h"
#include "catalog/pg_database.h"
#include "catalog/pg_conversion.h"
#include "catalog/pg_attrdef.h"
#include "catalog/pg_language.h"
#include "catalog/pg_amop.h"
#include "catalog/pg_amproc.h"
#include "catalog/pg_rewrite.h"
#include "catalog/pg_aggregate.h"

//#include "catalog/pg_policy.h"
#include "catalog/pg_ts_parser.h"
#include "catalog/pg_ts_dict.h"
#include "catalog/pg_ts_template.h"
#include "catalog/pg_opfamily.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_largeobject_metadata.h"
#include "catalog/pg_foreign_server.h"

#include "tsdb_event_trigger.h"
#include "tsdb_static2.cpp"




// static void
// InvalidateConstraintCacheCallBack(Datum arg, int cacheid, uint32 hashvalue)
// {
// 	dlist_mutable_iter iter;

// 	Assert(ri_constraint_cache != NULL);

// 	/*
// 	 * If the list of currently valid entries gets excessively large, we mark
// 	 * them all invalid so we can empty the list.  This arrangement avoids
// 	 * O(N^2) behavior in situations where a session touches many foreign keys
// 	 * and also does many ALTER TABLEs, such as a restore from pg_dump.
// 	 */
// 	if (ri_constraint_cache_valid_count > 1000)
// 		hashvalue = 0;			/* pretend it's a cache reset */

// 	dlist_foreach_modify(iter, &ri_constraint_cache_valid_list)
// 	{
// 		RI_ConstraintInfo *riinfo = dlist_container(RI_ConstraintInfo,
// 													valid_link, iter.cur);

// 		if (hashvalue == 0 || riinfo->oidHashValue == hashvalue)
// 		{
// 			riinfo->valid = false;
// 			/* Remove invalidated entries from the list, too */
// 			dlist_delete(iter.cur);
// 			ri_constraint_cache_valid_count--;
// 		}
// 	}
// }



// static const RI_ConstraintInfo *
// ri_LoadConstraintInfo(Oid constraintOid)
// {
// 	RI_ConstraintInfo *riinfo;
// 	bool		found;
// 	HeapTuple	tup;
// 	Form_pg_constraint conForm;
// 	Datum		adatum;
// 	bool		isNull;
// 	ArrayType  *arr;
// 	int			numkeys;

// 	/*
// 	 * On the first call initialize the hashtable
// 	 */
// 	if (!ri_constraint_cache)
// 		ri_InitHashTables();

// 	/*
// 	 * Find or create a hash entry.  If we find a valid one, just return it.
// 	 */
// 	riinfo = (RI_ConstraintInfo *) hash_search(ri_constraint_cache,
// 											   (void *) &constraintOid,
// 											   HASH_ENTER, &found);
// 	if (!found)
// 		riinfo->valid = false;
// 	else if (riinfo->valid)
// 		return riinfo;

// 	/*
// 	 * Fetch the pg_constraint row so we can fill in the entry.
// 	 */
// 	tup = SearchSysCache1(CONSTROID, ObjectIdGetDatum(constraintOid));
// 	if (!HeapTupleIsValid(tup)) /* should not happen */
// 		elog(ERROR, "cache lookup failed for constraint %u", constraintOid);
// 	conForm = (Form_pg_constraint) GETSTRUCT(tup);

// 	if (conForm->contype != CONSTRAINT_FOREIGN) /* should not happen */
// 		elog(ERROR, "constraint %u is not a foreign key constraint",
// 			 constraintOid);

// 	/* And extract data */
// 	Assert(riinfo->constraint_id == constraintOid);
// 	riinfo->oidHashValue = GetSysCacheHashValue1(CONSTROID,
// 											ObjectIdGetDatum(constraintOid));
// 	memcpy(&riinfo->conname, &conForm->conname, sizeof(NameData));
// 	riinfo->pk_relid = conForm->confrelid;
// 	riinfo->fk_relid = conForm->conrelid;
// 	riinfo->confupdtype = conForm->confupdtype;
// 	riinfo->confdeltype = conForm->confdeltype;
// 	riinfo->confmatchtype = conForm->confmatchtype;

// 	/*
// 	 * We expect the arrays to be 1-D arrays of the right types; verify that.
// 	 * We don't need to use deconstruct_array() since the array data is just
// 	 * going to look like a C array of values.
// 	 */
// 	adatum = SysCacheGetAttr(CONSTROID, tup,
// 							 Anum_pg_constraint_conkey, &isNull);
// 	if (isNull)
// 		elog(ERROR, "null conkey for constraint %u", constraintOid);
// 	arr = DatumGetArrayTypeP(adatum);	/* ensure not toasted */
// 	if (ARR_NDIM(arr) != 1 ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != INT2OID)
// 		elog(ERROR, "conkey is not a 1-D smallint array");
// 	numkeys = ARR_DIMS(arr)[0];
// 	if (numkeys <= 0 || numkeys > RI_MAX_NUMKEYS)
// 		elog(ERROR, "foreign key constraint cannot have %d columns", numkeys);
// 	riinfo->nkeys = numkeys;
// 	memcpy(riinfo->fk_attnums, ARR_DATA_PTR(arr), numkeys * sizeof(int16));
// 	if ((Pointer) arr != DatumGetPointer(adatum))
// 		pfree(arr);				/* free de-toasted copy, if any */

// 	adatum = SysCacheGetAttr(CONSTROID, tup,
// 							 Anum_pg_constraint_confkey, &isNull);
// 	if (isNull)
// 		elog(ERROR, "null confkey for constraint %u", constraintOid);
// 	arr = DatumGetArrayTypeP(adatum);	/* ensure not toasted */
// 	if (ARR_NDIM(arr) != 1 ||
// 		ARR_DIMS(arr)[0] != numkeys ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != INT2OID)
// 		elog(ERROR, "confkey is not a 1-D smallint array");
// 	memcpy(riinfo->pk_attnums, ARR_DATA_PTR(arr), numkeys * sizeof(int16));
// 	if ((Pointer) arr != DatumGetPointer(adatum))
// 		pfree(arr);				/* free de-toasted copy, if any */

// 	adatum = SysCacheGetAttr(CONSTROID, tup,
// 							 Anum_pg_constraint_conpfeqop, &isNull);
// 	if (isNull)
// 		elog(ERROR, "null conpfeqop for constraint %u", constraintOid);
// 	arr = DatumGetArrayTypeP(adatum);	/* ensure not toasted */
// 	/* see TryReuseForeignKey if you change the test below */
// 	if (ARR_NDIM(arr) != 1 ||
// 		ARR_DIMS(arr)[0] != numkeys ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != OIDOID)
// 		elog(ERROR, "conpfeqop is not a 1-D Oid array");
// 	memcpy(riinfo->pf_eq_oprs, ARR_DATA_PTR(arr), numkeys * sizeof(Oid));
// 	if ((Pointer) arr != DatumGetPointer(adatum))
// 		pfree(arr);				/* free de-toasted copy, if any */

// 	adatum = SysCacheGetAttr(CONSTROID, tup,
// 							 Anum_pg_constraint_conppeqop, &isNull);
// 	if (isNull)
// 		elog(ERROR, "null conppeqop for constraint %u", constraintOid);
// 	arr = DatumGetArrayTypeP(adatum);	/* ensure not toasted */
// 	if (ARR_NDIM(arr) != 1 ||
// 		ARR_DIMS(arr)[0] != numkeys ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != OIDOID)
// 		elog(ERROR, "conppeqop is not a 1-D Oid array");
// 	memcpy(riinfo->pp_eq_oprs, ARR_DATA_PTR(arr), numkeys * sizeof(Oid));
// 	if ((Pointer) arr != DatumGetPointer(adatum))
// 		pfree(arr);				/* free de-toasted copy, if any */

// 	adatum = SysCacheGetAttr(CONSTROID, tup,
// 							 Anum_pg_constraint_conffeqop, &isNull);
// 	if (isNull)
// 		elog(ERROR, "null conffeqop for constraint %u", constraintOid);
// 	arr = DatumGetArrayTypeP(adatum);	/* ensure not toasted */
// 	if (ARR_NDIM(arr) != 1 ||
// 		ARR_DIMS(arr)[0] != numkeys ||
// 		ARR_HASNULL(arr) ||
// 		ARR_ELEMTYPE(arr) != OIDOID)
// 		elog(ERROR, "conffeqop is not a 1-D Oid array");
// 	memcpy(riinfo->ff_eq_oprs, ARR_DATA_PTR(arr), numkeys * sizeof(Oid));
// 	if ((Pointer) arr != DatumGetPointer(adatum))
// 		pfree(arr);				/* free de-toasted copy, if any */

// 	ReleaseSysCache(tup);

// 	/*
// 	 * For efficient processing of invalidation messages below, we keep a
// 	 * doubly-linked list, and a count, of all currently valid entries.
// 	 */
// 	dlist_push_tail(&ri_constraint_cache_valid_list, &riinfo->valid_link);
// 	ri_constraint_cache_valid_count++;

// 	riinfo->valid = true;

// 	return riinfo;
// }
// static bool
// contain_volatile_functions_not_nextval_checker(Oid func_id, void *context)
// {
// 	return (func_id != F_NEXTVAL_OID &&
// 			func_volatile(func_id) == PROVOLATILE_VOLATILE);
// }
// // static void *
// // readtup_alloc(Tuplesortstate *state, int tapenum, Size tuplen)
// // {
// // 	if (state->batchUsed)
// // 	{
// // 		/*
// // 		 * No USEMEM() call, because during final on-the-fly merge accounting
// // 		 * is based on tape-private state. ("Overflow" allocations are
// // 		 * detected as an indication that a new round or preloading is
// // 		 * required. Preloading marks existing contents of tape's batch buffer
// // 		 * for reuse.)
// // 		 */
// // 		return mergebatchalloc(state, tapenum, tuplen);
// // 	}
// // 	else
// // 	{
// // 		char	   *ret;

// // 		/* Batch allocation yet to be performed */
// // 		ret =(char*) MemoryContextAlloc(state->tuplecontext, tuplen);
// // 		USEMEM(state, GetMemoryChunkSpace(ret));
// // 		return ret;
// // 	}
// // }


// // static void
// // make_bounded_heap(Tuplesortstate *state)
// // {
// // 	int tupcount = state->memtupcount;
// //     int i;

// //     Assert(state->status == TSS_INITIAL);
// //     Assert(state->bounded);
// //     Assert(tupcount >= state->bound);
// //     Assert(SERIAL(state));

// //     /* Reverse sort direction so largest entry will be at root */
// //     REVERSEDIRECTION(state);

// //     state->memtupcount = 0; /* make the heap empty */
// //     for (i = 0; i < tupcount; i++) {
// //         if (state->memtupcount < state->bound) {
// //             /* Insert next tuple into heap */
// //             /* Must copy source tuple to avoid possible overwrite */
// //             SortTuple   stup = state->memtuples[i];
// //             tuplesort_heap_insert(state, &stup, 0);
// //         } else {
// //             /*
// //              * The heap is full.  Replace the largest entry with the new
// //              * tuple, or just discard it, if it's larger than anything already
// //              * in the heap.
// //              */
// //             if (COMPARETUP(state, &state->memtuples[i], &state->memtuples[0]) <= 0) {
// //                 free_sort_tuple(state, &state->memtuples[i]);
// //                 CHECK_FOR_INTERRUPTS();
// //             } else {
// //                 tuplesort_heap_replace_top(state, &state->memtuples[i]);
// //             }
// //         }
// //     }

// //     Assert(state->memtupcount == state->bound);
// //     state->status = TSS_BOUNDED;
// // }

// static bool
// Do_MultiXactIdWait(MultiXactId multi, MultiXactStatus status,
// 				   uint16 infomask, bool nowait,
// 				   Relation rel, ItemPointer ctid, XLTW_Oper oper,
// 				   int *remaining)
// {
// 	bool		result = true;
// 	MultiXactMember *members;
// 	int			nmembers;
// 	int			remain = 0;

// 	/* for pre-pg_upgrade tuples, no need to sleep at all */
// 	nmembers = HEAP_LOCKED_UPGRADED(infomask) ? -1 :
// 		GetMultiXactIdMembers(multi, &members, false,
// 							  HEAP_XMAX_IS_LOCKED_ONLY_tsdb(infomask));

// 	if (nmembers >= 0)
// 	{
// 		int			i;

// 		for (i = 0; i < nmembers; i++)
// 		{
// 			TransactionId memxid = members[i].xid;
// 			MultiXactStatus memstatus = members[i].status;

// 			if (TransactionIdIsCurrentTransactionId(memxid))
// 			{
// 				remain++;
// 				continue;
// 			}

// 			if (!DoLockModesConflict(LOCKMODE_from_mxstatus(memstatus),
// 									 LOCKMODE_from_mxstatus(status)))
// 			{
// 				if (remaining && TransactionIdIsInProgress(memxid))
// 					remain++;
// 				continue;
// 			}

// 			/*
// 			 * This member conflicts with our multi, so we have to sleep (or
// 			 * return failure, if asked to avoid waiting.)
// 			 *
// 			 * Note that we don't set up an error context callback ourselves,
// 			 * but instead we pass the info down to XactLockTableWait.  This
// 			 * might seem a bit wasteful because the context is set up and
// 			 * tore down for each member of the multixact, but in reality it
// 			 * should be barely noticeable, and it avoids duplicate code.
// 			 */
// 			if (nowait)
// 			{
// 				result = ConditionalXactLockTableWait(memxid);
// 				if (!result)
// 					break;
// 			}
// 			else
// 				XactLockTableWait(memxid, rel, ctid, oper);
// 		}

// 		pfree(members);
// 	}

// 	if (remaining)
// 		*remaining = remain;

// 	return result;
// }

// static bool
// ConditionalMultiXactIdWait(MultiXactId multi, MultiXactStatus status,
// 						   uint16 infomask, Relation rel, int *remaining)
// {
// 	return Do_MultiXactIdWait(multi, status, infomask, true,
// 							  rel, NULL, XLTW_None, remaining);
// }

// //gausskernel/storage/file/fd.cpp

// // static bool
// // reserveAllocatedDesc(void)
// // {
// // 	AllocateDesc *newDescs;
// // 	int			newMax;

// // 	/* Quick out if array already has a free slot. */
// // 	if (numAllocatedDescs < maxAllocatedDescs)
// // 		return true;

// // 	/*
// // 	 * If the array hasn't yet been created in the current process, initialize
// // 	 * it with FD_MINFREE / 2 elements.  In many scenarios this is as many as
// // 	 * we will ever need, anyway.  We don't want to look at max_safe_fds
// // 	 * immediately because set_max_safe_fds() may not have run yet.
// // 	 */
// // 	if (allocatedDescs == NULL)
// // 	{
// // 		newMax = FD_MINFREE / 2;
// // 		newDescs = (AllocateDesc *) malloc(newMax * sizeof(AllocateDesc));
// // 		/* Out of memory already?  Treat as fatal error. */
// // 		if (newDescs == NULL)
// // 			ereport(ERROR,
// // 					(errcode(ERRCODE_OUT_OF_MEMORY),
// // 					 errmsg("out of memory")));
// // 		allocatedDescs = newDescs;
// // 		maxAllocatedDescs = newMax;
// // 		return true;
// // 	}

// // 	/*
// // 	 * Consider enlarging the array beyond the initial allocation used above.
// // 	 * By the time this happens, max_safe_fds should be known accurately.
// // 	 *
// // 	 * We mustn't let allocated descriptors hog all the available FDs, and in
// // 	 * practice we'd better leave a reasonable number of FDs for VFD use.  So
// // 	 * set the maximum to max_safe_fds / 2.  (This should certainly be at
// // 	 * least as large as the initial size, FD_MINFREE / 2.)
// // 	 */
// // 	newMax = max_safe_fds / 2;
// // 	if (newMax > maxAllocatedDescs)
// // 	{
// // 		newDescs = (AllocateDesc *) realloc(allocatedDescs,
// // 											newMax * sizeof(AllocateDesc));
// // 		/* Treat out-of-memory as a non-fatal error. */
// // 		if (newDescs == NULL)
// // 			return false;
// // 		allocatedDescs = newDescs;
// // 		maxAllocatedDescs = newMax;
// // 		return true;
// // 	}

// // 	/* Can't enlarge allocatedDescs[] any more. */
// // 	return false;
// // }
// static void
// MultiXactIdWait(MultiXactId multi, MultiXactStatus status, uint16 infomask,
// 				Relation rel, ItemPointer ctid, XLTW_Oper oper,
// 				int *remaining)
// {
// 	(void) Do_MultiXactIdWait(multi, status, infomask, false,
// 							  rel, ctid, oper, remaining);
// }


// // static bool
// // needs_toast_table(Relation rel)
// // {
// // 	int32 data_length = 0;
// //     bool maxlength_unknown = false;
// //     bool has_toastable_attrs = false;
// //     TupleDesc tupdesc;
// //     Form_pg_attribute* att = NULL;
// //     int32 tuple_length;
// //     int i;

// //     /*
// //      * The existence of newly added catalog toast tables should be
// //      * explicitly pointed out with predetermined oid during inplace upgrade.
// //      */
// //     if (u_sess->attr.attr_common.IsInplaceUpgrade && rel->rd_id < FirstBootstrapObjectId) {
// //         if (OidIsValid(u_sess->upg_cxt.Inplace_upgrade_next_toast_pg_class_oid))
// //             return true;
// //         else
// //             return false;
// //     }

// //     // column-store relations don't need any toast tables.
// //     if (RelationIsColStore(rel))
// //         return false;

// //     tupdesc = rel->rd_att;
// //     att = tupdesc->attrs;

// //     for (i = 0; i < tupdesc->natts; i++) {
// //         if (att[i]->attisdropped)
// //             continue;
// //         data_length = att_align_nominal(data_length, att[i]->attalign);
// //         if (att[i]->attlen > 0) {
// //             /* Fixed-length types are never toastable */
// //             data_length += att[i]->attlen;
// //         } else {
// //             int32 maxlen = type_maximum_size(att[i]->atttypid, att[i]->atttypmod);
// //             if (maxlen < 0)
// //                 maxlength_unknown = true;
// //             else
// //                 data_length += maxlen;
// //             if (att[i]->attstorage != 'p')
// //                 has_toastable_attrs = true;
// //         }
// //     }
// //     if (!has_toastable_attrs) {
// //         return false; /* nothing to toast? */
// //     }
// //     if (maxlength_unknown) {
// //         return true; /* any unlimited-length attrs? */
// //     }
// //     if (RelationIsUstoreFormat(rel)) { // uheap format
// //         tuple_length = MAXALIGN(offsetof(UHeapDiskTupleData, data) +
// //             BITMAPLEN(tupdesc->natts)) + MAXALIGN(data_length);
// //         return ((unsigned long)tuple_length > UTOAST_TUPLE_THRESHOLD);
// //     } else { // heap
// //         tuple_length = MAXALIGN(offsetof(HeapTupleHeaderData, t_bits) +
// //             BITMAPLEN(tupdesc->natts)) + MAXALIGN(data_length);
// //         return ((unsigned long)tuple_length > TOAST_TUPLE_THRESHOLD);
// //     }
// // }
// static inline int
// ApplySortAbbrevFullComparator(Datum datum1, bool isNull1,
// 							  Datum datum2, bool isNull2,
// 							  SortSupport ssup)
// {
// 	int			compare;

// 	if (isNull1)
// 	{
// 		if (isNull2)
// 			compare = 0;		/* NULL "=" NULL */
// 		else if (ssup->ssup_nulls_first)
// 			compare = -1;		/* NULL "<" NOT_NULL */
// 		else
// 			compare = 1;		/* NULL ">" NOT_NULL */
// 	}
// 	else if (isNull2)
// 	{
// 		if (ssup->ssup_nulls_first)
// 			compare = 1;		/* NOT_NULL ">" NULL */
// 		else
// 			compare = -1;		/* NOT_NULL "<" NULL */
// 	}
// 	else
// 	{
// 		compare = (*ssup->abbrev_full_comparator) (datum1, datum2, ssup);
// 		if (ssup->ssup_reverse)
// 			compare = -compare;
// 	}

// 	return compare;
// }
// static void
// escape_yaml(StringInfo buf, const char *str)
// {
// 	escape_json(buf, str);
// }
// static void
// StoreCatalogInheritance1(Oid relationId, Oid parentOid,
// 						 int16 seqNumber, Relation inhRelation)
// {
// 	TupleDesc	desc = RelationGetDescr(inhRelation);
// 	Datum		values[Natts_pg_inherits];
// 	bool		nulls[Natts_pg_inherits];
// 	ObjectAddress childobject,
// 				parentobject;
// 	HeapTuple	tuple;

// 	/*
// 	 * Make the pg_inherits entry
// 	 */
// 	values[Anum_pg_inherits_inhrelid - 1] = ObjectIdGetDatum(relationId);
// 	values[Anum_pg_inherits_inhparent - 1] = ObjectIdGetDatum(parentOid);
// 	values[Anum_pg_inherits_inhseqno - 1] = Int16GetDatum(seqNumber);

// 	memset(nulls, 0, sizeof(nulls));

// 	tuple = heap_form_tuple(desc, values, nulls);

// 	simple_heap_insert(inhRelation, tuple);

// 	CatalogUpdateIndexes(inhRelation, tuple);

// 	heap_freetuple(tuple);

// 	/*
// 	 * Store a dependency too
// 	 */
// 	parentobject.classId = RelationRelationId;
// 	parentobject.objectId = parentOid;
// 	parentobject.objectSubId = 0;
// 	childobject.classId = RelationRelationId;
// 	childobject.objectId = relationId;
// 	childobject.objectSubId = 0;

// 	recordDependencyOn(&childobject, &parentobject, DEPENDENCY_NORMAL);

// 	/*
// 	 * Post creation hook of this inheritance. Since object_access_hook
// 	 * doesn't take multiple object identifiers, we relay oid of parent
// 	 * relation using auxiliary_id argument.
// 	 */
// 	InvokeObjectPostAlterHookArg(InheritsRelationId,
// 								 relationId, 0,
// 								 parentOid, false);

// 	/*
// 	 * Mark the parent as having subclasses.
// 	 */
// 	SetRelationHasSubclass(parentOid, true);
// }

static void
logical_begin_heap_rewrite(RewriteState state)
{
	HASHCTL		hash_ctl;
	TransactionId logical_xmin;

	/*
	 * We only need to persist these mappings if the rewritten table can be
	 * accessed during logical decoding, if not, we can skip doing any
	 * additional work.
	 */
	state->rs_logical_rewrite =
		RelationIsAccessibleInLogicalDecoding(state->rs_old_rel);

	if (!state->rs_logical_rewrite)
		return;

	ProcArrayGetReplicationSlotXmin(NULL, &logical_xmin);

	/*
	 * If there are no logical slots in progress we don't need to do anything,
	 * there cannot be any remappings for relevant rows yet. The relation's
	 * lock protects us against races.
	 */
	if (logical_xmin == InvalidTransactionId)
	{
		state->rs_logical_rewrite = false;
		return;
	}

	state->rs_logical_xmin = logical_xmin;
	state->rs_begin_lsn = GetXLogInsertRecPtr();
	state->rs_num_rewrite_mappings = 0;

	memset(&hash_ctl, 0, sizeof(hash_ctl));
	hash_ctl.keysize = sizeof(TransactionId);
	hash_ctl.entrysize = sizeof(RewriteMappingFile);
	hash_ctl.hcxt = state->rs_cxt;

	state->rs_logical_mappings =
		hash_create("Logical rewrite mapping",
					128,		/* arbitrary initial size */
					&hash_ctl,
					HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);
}




// static void
// movetup_cluster(void *dest, void *src, unsigned int len)
// {
// 	HeapTuple	tuple;

// 	memmove(dest, src, len);

// 	/* Repoint the HeapTupleData header */
// 	tuple = (HeapTuple) dest;
// 	tuple->t_data = (HeapTupleHeader) ((char *) tuple + HEAPTUPLESIZE);
// }
// // static void
// // puttuple_common(Tuplesortstate *state, SortTuple *tuple)
// // {
// // 	switch (state->status)
// // 	{
// // 		case TSS_INITIAL:

// // 			/*
// // 			 * Save the tuple into the unsorted array.  First, grow the array
// // 			 * as needed.  Note that we try to grow the array when there is
// // 			 * still one free slot remaining --- if we fail, there'll still be
// // 			 * room to store the incoming tuple, and then we'll switch to
// // 			 * tape-based operation.
// // 			 */
// // 			if (state->memtupcount >= state->memtupsize - 1)
// // 			{
// // 				(void) grow_memtuples(state);
// // 				Assert(state->memtupcount < state->memtupsize);
// // 			}
// // 			state->memtuples[state->memtupcount++] = *tuple;

// // 			/*
// // 			 * Check if it's time to switch over to a bounded heapsort. We do
// // 			 * so if the input tuple count exceeds twice the desired tuple
// // 			 * count (this is a heuristic for where heapsort becomes cheaper
// // 			 * than a quicksort), or if we've just filled workMem and have
// // 			 * enough tuples to meet the bound.
// // 			 *
// // 			 * Note that once we enter TSS_BOUNDED state we will always try to
// // 			 * complete the sort that way.  In the worst case, if later input
// // 			 * tuples are larger than earlier ones, this might cause us to
// // 			 * exceed workMem significantly.
// // 			 */
// // 			if (state->bounded &&
// // 				(state->memtupcount > state->bound * 2 ||
// // 				 (state->memtupcount > state->bound && LACKMEM(state))))
// // 			{
// // #ifdef TRACE_SORT
// // 				if (trace_sort)
// // 					elog(LOG, "switching to bounded heapsort at %d tuples: %s",
// // 						 state->memtupcount,
// // 						 pg_rusage_show(&state->ru_start));
// // #endif
// // 				make_bounded_heap(state);
// // 				return;
// // 			}

// // 			/*
// // 			 * Done if we still fit in available memory and have array slots.
// // 			 */
// // 			if (state->memtupcount < state->memtupsize && !LACKMEM(state))
// // 				return;

// // 			/*
// // 			 * Nope; time to switch to tape-based operation.
// // 			 */
// // 			inittapes(state);

// // 			/*
// // 			 * Dump tuples until we are back under the limit.
// // 			 */
// // 			dumptuples(state, false);
// // 			break;

// // 		case TSS_BOUNDED:

// // 			/*
// // 			 * We don't want to grow the array here, so check whether the new
// // 			 * tuple can be discarded before putting it in.  This should be a
// // 			 * good speed optimization, too, since when there are many more
// // 			 * input tuples than the bound, most input tuples can be discarded
// // 			 * with just this one comparison.  Note that because we currently
// // 			 * have the sort direction reversed, we must check for <= not >=.
// // 			 */
// // 			if (COMPARETUP(state, tuple, &state->memtuples[0]) <= 0)
// // 			{
// // 				/* new tuple <= top of the heap, so we can discard it */
// // 				free_sort_tuple(state, tuple);
// // 				CHECK_FOR_INTERRUPTS();
// // 			}
// // 			else
// // 			{
// // 				/* discard top of heap, sift up, insert new tuple */
// // 				free_sort_tuple(state, &state->memtuples[0]);
// // 				tuplesort_heap_siftup(state, false);
// // 				tuplesort_heap_insert(state, tuple, 0, false);
// // 			}
// // 			break;

// // 		case TSS_BUILDRUNS:

// // 			/*
// // 			 * Insert the tuple into the heap, with run number currentRun if
// // 			 * it can go into the current run, else HEAP_RUN_NEXT.  The tuple
// // 			 * can go into the current run if it is >= the first
// // 			 * not-yet-output tuple.  (Actually, it could go into the current
// // 			 * run if it is >= the most recently output tuple ... but that
// // 			 * would require keeping around the tuple we last output, and it's
// // 			 * simplest to let writetup free each tuple as soon as it's
// // 			 * written.)
// // 			 *
// // 			 * Note that this only applies when:
// // 			 *
// // 			 * - currentRun is RUN_FIRST
// // 			 *
// // 			 * - Replacement selection is in use (typically it is never used).
// // 			 *
// // 			 * When these two conditions are not both true, all tuples are
// // 			 * appended indifferently, much like the TSS_INITIAL case.
// // 			 *
// // 			 * There should always be room to store the incoming tuple.
// // 			 */
// // 			Assert(!state->replaceActive || state->memtupcount > 0);
// // 			if (state->replaceActive &&
// // 				COMPARETUP(state, tuple, &state->memtuples[0]) >= 0)
// // 			{
// // 				Assert(state->currentRun == RUN_FIRST);

// // 				/*
// // 				 * Insert tuple into first, fully heapified run.
// // 				 *
// // 				 * Unlike classic replacement selection, which this module was
// // 				 * previously based on, only RUN_FIRST tuples are fully
// // 				 * heapified.  Any second/next run tuples are appended
// // 				 * indifferently.  While HEAP_RUN_NEXT tuples may be sifted
// // 				 * out of the way of first run tuples, COMPARETUP() will never
// // 				 * be called for the run's tuples during sifting (only our
// // 				 * initial COMPARETUP() call is required for the tuple, to
// // 				 * determine that the tuple does not belong in RUN_FIRST).
// // 				 */
// // 				tuplesort_heap_insert(state, tuple, state->currentRun, true);
// // 			}
// // 			else
// // 			{
// // 				/*
// // 				 * Tuple was determined to not belong to heapified RUN_FIRST,
// // 				 * or replacement selection not in play.  Append the tuple to
// // 				 * memtuples indifferently.
// // 				 *
// // 				 * dumptuples() does not trust that the next run's tuples are
// // 				 * heapified.  Anything past the first run will always be
// // 				 * quicksorted even when replacement selection is initially
// // 				 * used.  (When it's never used, every tuple still takes this
// // 				 * path.)
// // 				 */
// // 				tuple->tupindex = HEAP_RUN_NEXT;
// // 				state->memtuples[state->memtupcount++] = *tuple;
// // 			}

// // 			/*
// // 			 * If we are over the memory limit, dump tuples till we're under.
// // 			 */
// // 			dumptuples(state, false);
// // 			break;

// // 		default:
// // 			elog(ERROR, "invalid tuplesort state");
// // 			break;
// // 	}
// // }

static int
toast_open_indexes(Relation toastrel,
				   LOCKMODE lock,
				   Relation **toastidxs,
				   int *num_indexes)
{
	int			i = 0;
	int			res = 0;
	bool		found = false;
	List	   *indexlist;
	ListCell   *lc;

	/* Get index list of the toast relation */
	indexlist = RelationGetIndexList(toastrel);
	Assert(indexlist != NIL);

	*num_indexes = list_length(indexlist);

	/* Open all the index relations */
	*toastidxs = (Relation *) palloc(*num_indexes * sizeof(Relation));
	foreach(lc, indexlist)
		(*toastidxs)[i++] = index_open(lfirst_oid(lc), lock);

	/* Fetch the first valid index in list */
	for (i = 0; i < *num_indexes; i++)
	{
		Relation	toastidx = (*toastidxs)[i];

		if (toastidx->rd_index->indisvalid)
		{
			res = i;
			found = true;
			break;
		}
	}

	/*
	 * Free index list, not necessary anymore as relations are opened and a
	 * valid index has been found.
	 */
	list_free(indexlist);

	/*
	 * The toast relation should have one valid index, so something is going
	 * wrong if there is nothing.
	 */
	if (!found)
		elog(ERROR, "no valid index found for toast relation with Oid %u",
			 RelationGetRelid(toastrel));

	return res;
}
static void
toast_close_indexes(Relation *toastidxs, int num_indexes, LOCKMODE lock)
{
	int			i;

	/* Close relations and clean up things */
	for (i = 0; i < num_indexes; i++)
		index_close(toastidxs[i], lock);
	pfree(toastidxs);
}

// static Node *
// make_agg_arg(Oid argtype, Oid argcollation)
// {
// 	Param	   *argp = makeNode(Param);

// 	argp->paramkind = PARAM_EXEC;
// 	argp->paramid = -1;
// 	argp->paramtype = argtype;
// 	argp->paramtypmod = -1;
// 	argp->paramcollid = argcollation;
// 	argp->location = -1;
// 	return (Node *) argp;
// }

// static double
// relation_byte_size(double tuples, int width)
// {
// 	return tuples * (MAXALIGN(width) + MAXALIGN(SizeofHeapTupleHeader));
// }
// static bool
// ri_KeysEqual(Relation rel, HeapTuple oldtup, HeapTuple newtup,
// 			 const RI_ConstraintInfo *riinfo, bool rel_is_pk)
// {
// 	TupleDesc	tupdesc = RelationGetDescr(rel);
// 	const int16 *attnums;
// 	const Oid  *eq_oprs;
// 	int			i;

// 	if (rel_is_pk)
// 	{
// 		attnums = riinfo->pk_attnums;
// 		eq_oprs = riinfo->pp_eq_oprs;
// 	}
// 	else
// 	{
// 		attnums = riinfo->fk_attnums;
// 		eq_oprs = riinfo->ff_eq_oprs;
// 	}

// 	for (i = 0; i < riinfo->nkeys; i++)
// 	{
// 		Datum		oldvalue;
// 		Datum		newvalue;
// 		bool		isnull;

// 		/*
// 		 * Get one attribute's oldvalue. If it is NULL - they're not equal.
// 		 */
// 		oldvalue = heap_getattr_tsdb(oldtup, attnums[i], tupdesc, &isnull);
// 		if (isnull)
// 			return false;

// 		/*
// 		 * Get one attribute's newvalue. If it is NULL - they're not equal.
// 		 */
// 		newvalue = heap_getattr_tsdb(newtup, attnums[i], tupdesc, &isnull);
// 		if (isnull)
// 			return false;

// 		/*
// 		 * Compare them with the appropriate equality operator.
// 		 */
// 		if (!ri_AttributesEqual(eq_oprs[i], RIAttType(rel, attnums[i]),
// 								oldvalue, newvalue))
// 			return false;
// 	}

// 	return true;
// }
// // static const RI_ConstraintInfo *
// // ri_FetchConstraintInfo(Trigger *trigger, Relation trig_rel, bool rel_is_pk)
// // {
// // 	Oid			constraintOid = trigger->tgconstraint;
// // 	const RI_ConstraintInfo *riinfo;

// // 	/*
// // 	 * Check that the FK constraint's OID is available; it might not be if
// // 	 * we've been invoked via an ordinary trigger or an old-style "constraint
// // 	 * trigger".
// // 	 */
// // 	if (!OidIsValid(constraintOid))
// // 		ereport(ERROR,
// // 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// // 		  errmsg("no pg_constraint entry for trigger \"%s\" on table \"%s\"",
// // 				 trigger->tgname, RelationGetRelationName(trig_rel)),
// // 				 errhint("Remove this referential integrity trigger and its mates, then do ALTER TABLE ADD CONSTRAINT.")));

// // 	/* Find or create a hashtable entry for the constraint */
// // 	riinfo = ri_LoadConstraintInfo(constraintOid);

// // 	/* Do some easy cross-checks against the trigger call data */
// // 	if (rel_is_pk)
// // 	{
// // 		if (riinfo->fk_relid != trigger->tgconstrrelid ||
// // 			riinfo->pk_relid != RelationGetRelid(trig_rel))
// // 			elog(ERROR, "wrong pg_constraint entry for trigger \"%s\" on table \"%s\"",
// // 				 trigger->tgname, RelationGetRelationName(trig_rel));
// // 	}
// // 	else
// // 	{
// // 		if (riinfo->fk_relid != RelationGetRelid(trig_rel) ||
// // 			riinfo->pk_relid != trigger->tgconstrrelid)
// // 			elog(ERROR, "wrong pg_constraint entry for trigger \"%s\" on table \"%s\"",
// // 				 trigger->tgname, RelationGetRelationName(trig_rel));
// // 	}

// // 	return riinfo;
// // }
// // static int
// // ri_NullCheck(HeapTuple tup,
// // 			 const RI_ConstraintInfo *riinfo, bool rel_is_pk)
// // {
// // 	const int16 *attnums;
// // 	int			i;
// // 	bool		allnull = true;
// // 	bool		nonenull = true;

// // 	if (rel_is_pk)
// // 		attnums = riinfo->pk_attnums;
// // 	else
// // 		attnums = riinfo->fk_attnums;

// // 	for (i = 0; i < riinfo->nkeys; i++)
// // 	{
// // 		if (heap_attisnull(tup, attnums[i]))
// // 			nonenull = false;
// // 		else
// // 			allnull = false;
// // 	}

// // 	if (allnull)
// // 		return RI_KEYS_ALL_NULL;

// // 	if (nonenull)
// // 		return RI_KEYS_NONE_NULL;

// // 	return RI_KEYS_SOME_NULL;
// // }
// static bool
// contain_volatile_functions_not_nextval_walker(Node *node, void *context)
// {
// 	if (node == NULL)
// 		return false;
// 	/* Check for volatile functions in node itself */
// 	if (check_functions_in_node(node,
// 							  contain_volatile_functions_not_nextval_checker,
// 								context))
// 		return true;

// 	/*
// 	 * See notes in contain_mutable_functions_walker about why we treat
// 	 * MinMaxExpr, XmlExpr, and CoerceToDomain as immutable.
// 	 */

// 	/* Recurse to check arguments */
// 	if (IsA(node, Query))
// 	{
// 		/* Recurse into subselects */
// 		return query_tree_walker((Query *) node,
// 							   (bool (*)())contain_volatile_functions_not_nextval_walker,
// 								 context, 0);
// 	}
// 	return expression_tree_walker(node,
// 							    (bool (*)())contain_volatile_functions_not_nextval_walker,
// 								  context);
// }



// static void
// CheckAndCreateToastTable(Oid relOid, Datum reloptions, LOCKMODE lockmode, bool check)
// {
// 	Relation	rel;

// 	rel = heap_open(relOid, lockmode);

// 	/* create_toast_table does all the work */
// 	//这个是静态函数 common\backend\catalog\toasting.cpp 
// 	(void) create_toast_table(rel, InvalidOid, InvalidOid, reloptions, lockmode, check);

// 	heap_close(rel, NoLock);
// }

// static void
// shm_mq_detach_callback(dsm_segment *seg, Datum arg)
// {
// 	shm_mq	   *mq = (shm_mq *) DatumGetPointer(arg);

// 	shm_mq_detach(mq);
// }


// static BlockNumber
// heap_parallelscan_nextpage(HeapScanDesc scan)
// {
// 	BlockNumber page = InvalidBlockNumber;
// 	BlockNumber sync_startpage = InvalidBlockNumber;
// 	BlockNumber report_page = InvalidBlockNumber;
// 	ParallelHeapScanDesc parallel_scan;

// 	Assert(scan->rs_parallel);
// 	parallel_scan = scan->rs_parallel;

// retry:
// 	/* Grab the spinlock. */
// 	SpinLockAcquire(&parallel_scan->phs_mutex);

// 	/*
// 	 * If the scan's startblock has not yet been initialized, we must do so
// 	 * now.  If this is not a synchronized scan, we just start at block 0, but
// 	 * if it is a synchronized scan, we must get the starting position from
// 	 * the synchronized scan machinery.  We can't hold the spinlock while
// 	 * doing that, though, so release the spinlock, get the information we
// 	 * need, and retry.  If nobody else has initialized the scan in the
// 	 * meantime, we'll fill in the value we fetched on the second time
// 	 * through.
// 	 */
// 	if (parallel_scan->phs_startblock == InvalidBlockNumber)
// 	{
// 		if (!parallel_scan->phs_syncscan)
// 			parallel_scan->phs_startblock = 0;
// 		else if (sync_startpage != InvalidBlockNumber)
// 			parallel_scan->phs_startblock = sync_startpage;
// 		else
// 		{
// 			SpinLockRelease(&parallel_scan->phs_mutex);
// 			sync_startpage = ss_get_location(scan->rs_rd, scan->rs_nblocks);
// 			goto retry;
// 		}
// 		parallel_scan->phs_cblock = parallel_scan->phs_startblock;
// 	}

// 	/*
// 	 * The current block number is the next one that needs to be scanned,
// 	 * unless it's InvalidBlockNumber already, in which case there are no more
// 	 * blocks to scan.  After remembering the current value, we must advance
// 	 * it so that the next call to this function returns the next block to be
// 	 * scanned.
// 	 */
// 	page = parallel_scan->phs_cblock;
// 	if (page != InvalidBlockNumber)
// 	{
// 		parallel_scan->phs_cblock++;
// 		if (parallel_scan->phs_cblock >= scan->rs_nblocks)
// 			parallel_scan->phs_cblock = 0;
// 		if (parallel_scan->phs_cblock == parallel_scan->phs_startblock)
// 		{
// 			parallel_scan->phs_cblock = InvalidBlockNumber;
// 			report_page = parallel_scan->phs_startblock;
// 		}
// 	}

// 	/* Release the lock. */
// 	SpinLockRelease(&parallel_scan->phs_mutex);

// 	/*
// 	 * Report scan location.  Normally, we report the current page number.
// 	 * When we reach the end of the scan, though, we report the starting page,
// 	 * not the ending page, just so the starting positions for later scans
// 	 * doesn't slew backwards.  We only report the position at the end of the
// 	 * scan once, though: subsequent callers will have report nothing, since
// 	 * they will have page == InvalidBlockNumber.
// 	 */
// 	if (scan->rs_syncscan)
// 	{
// 		if (report_page == InvalidBlockNumber)
// 			report_page = page;
// 		if (report_page != InvalidBlockNumber)
// 			ss_report_location(scan->rs_rd, report_page);
// 	}

// 	return page;
// }

// static inline void
// TestForOldSnapshot(Snapshot snapshot, Relation relation, Page page)
// {
// 	Assert(relation != NULL);

// 	if (old_snapshot_threshold >= 0
// 		&& (snapshot) != NULL
// 		&& ((snapshot)->satisfies ==(SnapshotSatisfiesMethod) HeapTupleSatisfiesMVCC
// 			|| (snapshot)->satisfies == HeapTupleSatisfiesToast)
// 		&& !XLogRecPtrIsInvalid((snapshot)->lsn)
// 		&& PageGetLSN(page) > (snapshot)->lsn)
// 		TestForOldSnapshot_impl(snapshot, relation);
// }

// static Tuplestorestate *
// GetCurrentFDWTuplestore(void)
// {
// 	Tuplestorestate *ret;

// 	ret = afterTriggers.fdw_tuplestores[afterTriggers.query_depth];
// 	if (ret == NULL)
// 	{
// 		MemoryContext oldcxt;
// 		ResourceOwner saveResourceOwner;

// 		/*
// 		 * Make the tuplestore valid until end of transaction.  This is the
// 		 * allocation lifespan of the associated events list, but we really
// 		 * only need it until AfterTriggerEndQuery().
// 		 */
// 		oldcxt = MemoryContextSwitchTo(TopTransactionContext);
// 		saveResourceOwner = CurrentResourceOwner;
// 		PG_TRY();
// 		{
// 			CurrentResourceOwner = TopTransactionResourceOwner;
// 			ret = tuplestore_begin_heap(false, false, work_mem);
// 		}
// 		PG_CATCH();
// 		{
// 			CurrentResourceOwner = saveResourceOwner;
// 			PG_RE_THROW();
// 		}
// 		PG_END_TRY();
// 		CurrentResourceOwner = saveResourceOwner;
// 		MemoryContextSwitchTo(oldcxt);

// 		afterTriggers.fdw_tuplestores[afterTriggers.query_depth] = ret;
// 	}

// 	return ret;
// }

// static bool
// CheckMutability(Expr *expr)
// {
// 	/*
// 	 * First run the expression through the planner.  This has a couple of
// 	 * important consequences.  First, function default arguments will get
// 	 * inserted, which may affect volatility (consider "default now()").
// 	 * Second, inline-able functions will get inlined, which may allow us to
// 	 * conclude that the function is really less volatile than it's marked. As
// 	 * an example, polymorphic functions must be marked with the most volatile
// 	 * behavior that they have for any input type, but once we inline the
// 	 * function we may be able to conclude that it's not so volatile for the
// 	 * particular input type we're dealing with.
// 	 *
// 	 * We assume here that expression_planner() won't scribble on its input.
// 	 */
// 	expr = expression_planner(expr);

// 	/* Now we can search for non-immutable functions */
// 	return contain_mutable_functions((Node *) expr);
// }static void *

// static void *
// internal_load_library(const char *libname)
// {
// 	DynamicFileList *file_scanner;
// 	PGModuleMagicFunction magic_func;
// 	char	   *load_error;
// 	struct stat stat_buf;
// 	PG_init_t	PG_init;

// 	/*
// 	 * Scan the list of loaded FILES to see if the file has been loaded.
// 	 */
// 	for (file_scanner = file_list;
// 		 file_scanner != NULL &&
// 		 strcmp(libname, file_scanner->filename) != 0;
// 		 file_scanner = file_scanner->next)
// 		;

// 	if (file_scanner == NULL)
// 	{
// 		/*
// 		 * Check for same files - different paths (ie, symlink or link)
// 		 */
// 		if (stat(libname, &stat_buf) == -1)
// 			ereport(ERROR,
// 					(errcode_for_file_access(),
// 					 errmsg("could not access file \"%s\": %m",
// 							libname)));

// 		for (file_scanner = file_list;
// 			 file_scanner != NULL &&
// 			 !SAME_INODE(stat_buf, *file_scanner);
// 			 file_scanner = file_scanner->next)
// 			;
// 	}

// 	if (file_scanner == NULL)
// 	{
// 		/*
// 		 * File not loaded yet.
// 		 */
// 		file_scanner = (DynamicFileList *)
// 			malloc(offsetof(DynamicFileList, filename) +strlen(libname) + 1);
// 		if (file_scanner == NULL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_OUT_OF_MEMORY),
// 					 errmsg("out of memory")));

// 		MemSet(file_scanner, 0, offsetof(DynamicFileList, filename));
// 		strcpy(file_scanner->filename, libname);
// 		file_scanner->device = stat_buf.st_dev;
// #ifndef WIN32
// 		file_scanner->inode = stat_buf.st_ino;
// #endif
// 		file_scanner->next = NULL;

// 		file_scanner->handle = pg_dlopen(file_scanner->filename);
// 		if (file_scanner->handle == NULL)
// 		{
// 			load_error = (char *) pg_dlerror();
// 			free((char *) file_scanner);
// 			/* errcode_for_file_access might not be appropriate here? */
// 			ereport(ERROR,
// 					(errcode_for_file_access(),
// 					 errmsg("could not load library \"%s\": %s",
// 							libname, load_error)));
// 		}

// 		/* Check the magic function to determine compatibility */
// 		magic_func = (PGModuleMagicFunction)
// 			pg_dlsym(file_scanner->handle, PG_MAGIC_FUNCTION_NAME_STRING);
// 		if (magic_func)
// 		{
// 			const Pg_magic_struct *magic_data_ptr = (*magic_func) ();

// 			if (magic_data_ptr->len != magic_data.len ||
// 				memcmp(magic_data_ptr, &magic_data, magic_data.len) != 0)
// 			{
// 				/* copy data block before unlinking library */
// 				Pg_magic_struct module_magic_data = *magic_data_ptr;

// 				/* try to unlink library */
// 				pg_dlclose(file_scanner->handle);
// 				free((char *) file_scanner);

// 				/* issue suitable complaint */
// 				incompatible_module_error(libname, &module_magic_data);
// 			}
// 		}
// 		else
// 		{
// 			/* try to unlink library */
// 			pg_dlclose(file_scanner->handle);
// 			free((char *) file_scanner);
// 			/* complain */
// 			ereport(ERROR,
// 				  (errmsg("incompatible library \"%s\": missing magic block",
// 						  libname),
// 				   errhint("Extension libraries are required to use the PG_MODULE_MAGIC macro.")));
// 		}

// 		/*
// 		 * If the library has a _PG_init() function, call it.
// 		 */
// 		PG_init = (PG_init_t) pg_dlsym(file_scanner->handle, "_PG_init");
// 		if (PG_init)
// 			(*PG_init) ();

// 		/* OK to link it into list */
// 		if (file_list == NULL)
// 			file_list = file_scanner;
// 		else
// 			file_tail->next = file_scanner;
// 		file_tail = file_scanner;
// 	}

// 	return file_scanner->handle;
// }


// static void
// CheckPredicate(Expr *predicate)
// {
// 	/*
// 	 * transformExpr() should have already rejected subqueries, aggregates,
// 	 * and window functions, based on the EXPR_KIND_ for a predicate.
// 	 */

// 	/*
// 	 * A predicate using mutable functions is probably wrong, for the same
// 	 * reasons that we don't allow an index expression to use one.
// 	 */
// 	if (CheckMutability(predicate))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 		   errmsg("functions in index predicate must be marked IMMUTABLE")));
// }





static bool
get_agg_clause_costs_walker(Node *node, get_agg_clause_costs_context *context)
{
	if (node == NULL)
		return false;
	if (IsA(node, Aggref))
	{
		Aggref	   *aggref = (Aggref *) node;
		AggClauseCosts *costs = context->costs;
		HeapTuple	aggTuple;
		Form_pg_aggregate aggform;
		Oid			aggtransfn;
		Oid			aggfinalfn;
		Oid			aggcombinefn;
		Oid			aggserialfn;
		Oid			aggdeserialfn;
		Oid			aggtranstype;
		int32		aggtransspace;
		QualCost	argcosts;

		Assert(aggref->agglevelsup == 0);

		/*
		 * Fetch info about aggregate from pg_aggregate.  Note it's correct to
		 * ignore the moving-aggregate variant, since what we're concerned
		 * with here is aggregates not window functions.
		 */
		aggTuple = SearchSysCache1(AGGFNOID,
								   ObjectIdGetDatum(aggref->aggfnoid));
		if (!HeapTupleIsValid(aggTuple))
			elog(ERROR, "cache lookup failed for aggregate %u",
				 aggref->aggfnoid);
		aggform = (Form_pg_aggregate) GETSTRUCT(aggTuple);
		//tsdb 这里要在Form_pg_aggregate中加入一些成员
		aggtransfn = aggform->aggtransfn;
		aggfinalfn = aggform->aggfinalfn;
		aggcombinefn = aggform->aggcombinefn;
		aggserialfn = aggform->aggserialfn;
		aggdeserialfn = aggform->aggdeserialfn;
		aggtranstype = aggform->aggtranstype;
		aggtransspace = aggform->aggtransspace;
		ReleaseSysCache(aggTuple);

		/*
		 * Resolve the possibly-polymorphic aggregate transition type, unless
		 * already done in a previous pass over the expression.
		 */
		if (OidIsValid(aggref->aggtranstype))
			aggtranstype = aggref->aggtranstype;
		else
		{
			Oid			inputTypes[FUNC_MAX_ARGS];
			int			numArguments;

			/* extract argument types (ignoring any ORDER BY expressions) */
			numArguments = get_aggregate_argtypes(aggref, inputTypes);

			/* resolve actual type of transition state, if polymorphic */
			aggtranstype = resolve_aggregate_transtype(aggref->aggfnoid,
													   aggtranstype,
													   inputTypes,
													   numArguments);
			aggref->aggtranstype = aggtranstype;
		}

		/*
		 * Count it, and check for cases requiring ordered input.  Note that
		 * ordered-set aggs always have nonempty aggorder.  Any ordered-input
		 * case also defeats partial aggregation.
		 */
		costs->numAggs++;
		if (aggref->aggorder != NIL || aggref->aggdistinct != NIL)
		{
			costs->numOrderedAggs++;
			//tsdb 这里需要在AggClauseCosts加入成员hasNonPartial,hasNonSerial
			costs->hasNonPartial = true;
		}

		/*
		 * Check whether partial aggregation is feasible, unless we already
		 * found out that we can't do it.
		 */
		if (!costs->hasNonPartial)
		{
			/*
			 * If there is no combine function, then partial aggregation is
			 * not possible.
			 */
			if (!OidIsValid(aggcombinefn))
				costs->hasNonPartial = true;

			/*
			 * If we have any aggs with transtype INTERNAL then we must check
			 * whether they have serialization/deserialization functions; if
			 * not, we can't serialize partial-aggregation results.
			 */
			else if (aggtranstype == INTERNALOID &&
					 (!OidIsValid(aggserialfn) || !OidIsValid(aggdeserialfn)))
				costs->hasNonSerial = true;
		}

		/*
		 * Add the appropriate component function execution costs to
		 * appropriate totals.
		 */
		if (DO_AGGSPLIT_COMBINE(context->aggsplit))
		{
			/* charge for combining previously aggregated states */
			costs->transCost.per_tuple += get_func_cost(aggcombinefn) *u_sess->attr.attr_sql.cpu_operator_cost;
		}
		else
			costs->transCost.per_tuple += get_func_cost(aggtransfn) *u_sess->attr.attr_sql.cpu_operator_cost;
		if (DO_AGGSPLIT_DESERIALIZE(context->aggsplit) &&
			OidIsValid(aggdeserialfn))
			costs->transCost.per_tuple += get_func_cost(aggdeserialfn) *u_sess->attr.attr_sql.cpu_operator_cost;
		if (DO_AGGSPLIT_SERIALIZE(context->aggsplit) &&
			OidIsValid(aggserialfn))
			costs->finalCost += get_func_cost(aggserialfn) *u_sess->attr.attr_sql.cpu_operator_cost;
		if (!DO_AGGSPLIT_SKIPFINAL(context->aggsplit) &&
			OidIsValid(aggfinalfn))
			costs->finalCost += get_func_cost(aggfinalfn) *u_sess->attr.attr_sql.cpu_operator_cost;

		/*
		 * These costs are incurred only by the initial aggregate node, so we
		 * mustn't include them again at upper levels.
		 */
		if (!DO_AGGSPLIT_COMBINE(context->aggsplit))
		{
			/* add the input expressions' cost to per-input-row costs */
			cost_qual_eval_node(&argcosts, (Node *) aggref->args, context->root);
			costs->transCost.startup += argcosts.startup;
			costs->transCost.per_tuple += argcosts.per_tuple;

			/*
			 * Add any filter's cost to per-input-row costs.
			 *
			 * XXX Ideally we should reduce input expression costs according
			 * to filter selectivity, but it's not clear it's worth the
			 * trouble.
			 */
			if (aggref->aggfilter)
			{
				cost_qual_eval_node(&argcosts, (Node *) aggref->aggfilter,
									context->root);
				costs->transCost.startup += argcosts.startup;
				costs->transCost.per_tuple += argcosts.per_tuple;
			}
		}

		/*
		 * If there are direct arguments, treat their evaluation cost like the
		 * cost of the finalfn.
		 */
		if (aggref->aggdirectargs)
		{
			cost_qual_eval_node(&argcosts, (Node *) aggref->aggdirectargs,
								context->root);
			costs->transCost.startup += argcosts.startup;
			costs->finalCost += argcosts.per_tuple;
		}

		/*
		 * If the transition type is pass-by-value then it doesn't add
		 * anything to the required size of the hashtable.  If it is
		 * pass-by-reference then we have to add the estimated size of the
		 * value itself, plus palloc overhead.
		 */
		if (!get_typbyval(aggtranstype))
		{
			int32		avgwidth;

			/* Use average width if aggregate definition gave one */
			if (aggtransspace > 0)
				avgwidth = aggtransspace;
			else if (aggtransfn == F_ARRAY_APPEND)
			{
				/*
				 * If the transition function is array_append(), it'll use an
				 * expanded array as transvalue, which will occupy at least
				 * ALLOCSET_SMALL_INITSIZE and possibly more.  Use that as the
				 * estimate for lack of a better idea.
				 */
				avgwidth = ALLOCSET_SMALL_INITSIZE;
			}
			else
			{
				/*
				 * If transition state is of same type as first aggregated
				 * input, assume it's the same typmod (same width) as well.
				 * This works for cases like MAX/MIN and is probably somewhat
				 * reasonable otherwise.
				 */
				int32		aggtranstypmod = -1;

				if (aggref->args)
				{
					TargetEntry *tle = (TargetEntry *) linitial(aggref->args);

					if (aggtranstype == exprType((Node *) tle->expr))
						aggtranstypmod = exprTypmod((Node *) tle->expr);
				}

				avgwidth = get_typavgwidth(aggtranstype, aggtranstypmod);
			}

			avgwidth = MAXALIGN(avgwidth);
			costs->transitionSpace += avgwidth + 2 * sizeof(void *);
		}
		else if (aggtranstype == INTERNALOID)
		{
			/*
			 * INTERNAL transition type is a special case: although INTERNAL
			 * is pass-by-value, it's almost certainly being used as a pointer
			 * to some large data structure.  The aggregate definition can
			 * provide an estimate of the size.  If it doesn't, then we assume
			 * ALLOCSET_DEFAULT_INITSIZE, which is a good guess if the data is
			 * being kept in a private memory context, as is done by
			 * array_agg() for instance.
			 */
			if (aggtransspace > 0)
				costs->transitionSpace += aggtransspace;
			else
				costs->transitionSpace += ALLOCSET_DEFAULT_INITSIZE;
		}

		/*
		 * We assume that the parser checked that there are no aggregates (of
		 * this level anyway) in the aggregated arguments, direct arguments,
		 * or filter clause.  Hence, we need not recurse into any of them.
		 */
		return false;
	}
	Assert(!IsA(node, SubLink));
	return expression_tree_walker(node,(bool (*)()) get_agg_clause_costs_walker,
								  (void *) context);
}

static bool
SanityCheckBackgroundWorker(BackgroundWorker *worker, int elevel)
{
	/* sanity check for flags */
	if (worker->bgw_flags & BGWORKER_BACKEND_DATABASE_CONNECTION)
	{
		if (!(worker->bgw_flags & BGWORKER_SHMEM_ACCESS))
		{
			ereport(elevel,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("background worker \"%s\": must attach to shared memory in order to request a database connection",
							worker->bgw_name)));
			return false;
		}

		if (worker->bgw_start_time == BgWorkerStart_PostmasterStart)
		{
			ereport(elevel,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("background worker \"%s\": cannot request database access if starting at postmaster start",
							worker->bgw_name)));
			return false;
		}

		/* XXX other checks? */
	}

	if ((worker->bgw_restart_time < 0 &&
		 worker->bgw_restart_time != BGW_NEVER_RESTART) ||
		(worker->bgw_restart_time > USECS_PER_DAY / 1000))
	{
		ereport(elevel,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("background worker \"%s\": invalid restart interval",
						worker->bgw_name)));
		return false;
	}

	return true;
}

// static void
// convert_from_base_unit(int64 base_value, int base_unit,
// 					   int64 *value, const char **unit)
// {
// 	const unit_conversion *table;
// 	int			i;

// 	*unit = NULL;

// 	if (base_unit & GUC_UNIT_MEMORY)
// 		table = memory_unit_conversion_table;
// 	else
// 		table = time_unit_conversion_table;

// 	for (i = 0; *table[i].unit; i++)
// 	{
// 		if (base_unit == table[i].base_unit)
// 		{
// 			/*
// 			 * Accept the first conversion that divides the value evenly. We
// 			 * assume that the conversions for each base unit are ordered from
// 			 * greatest unit to the smallest!
// 			 */
// 			if (table[i].multiplier < 0)
// 			{
// 				*value = base_value * (-table[i].multiplier);
// 				*unit = table[i].unit;
// 				break;
// 			}
// 			else if (base_value % table[i].multiplier == 0)
// 			{
// 				*value = base_value / table[i].multiplier;
// 				*unit = table[i].unit;
// 				break;
// 			}
// 		}
// 	}

// 	Assert(*unit != NULL);
// }

// static char *
// _ShowOption(struct config_generic * record, bool use_units)
// {
// 	char		buffer[256];
// 	const char *val;

// 	switch (record->vartype)
// 	{
// 		case PGC_BOOL:
// 			{
// 				struct config_bool *conf = (struct config_bool *) record;

// 				if (conf->show_hook)
// 					val = (*conf->show_hook) ();
// 				else
// 					val = *conf->variable ? "on" : "off";
// 			}
// 			break;

// 		case PGC_INT:
// 			{
// 				struct config_int *conf = (struct config_int *) record;

// 				if (conf->show_hook)
// 					val = (*conf->show_hook) ();
// 				else
// 				{
// 					/*
// 					 * Use int64 arithmetic to avoid overflows in units
// 					 * conversion.
// 					 */
// 					int64		result = *conf->variable;
// 					const char *unit;

// 					if (use_units && result > 0 && (record->flags & GUC_UNIT))
// 					{
// 						convert_from_base_unit(result, record->flags & GUC_UNIT,
// 											   &result, &unit);
// 					}
// 					else
// 						unit = "";

// 					snprintf(buffer, sizeof(buffer), INT64_FORMAT "%s",
// 							 result, unit);
// 					val = buffer;
// 				}
// 			}
// 			break;

// 		case PGC_REAL:
// 			{
// 				struct config_real *conf = (struct config_real *) record;

// 				if (conf->show_hook)
// 					val = (*conf->show_hook) ();
// 				else
// 				{
// 					snprintf(buffer, sizeof(buffer), "%g",
// 							 *conf->variable);
// 					val = buffer;
// 				}
// 			}
// 			break;

// 		case PGC_STRING:
// 			{
// 				struct config_string *conf = (struct config_string *) record;

// 				if (conf->show_hook)
// 					val = (*conf->show_hook) ();
// 				else if (*conf->variable && **conf->variable)
// 					val = *conf->variable;
// 				else
// 					val = "";
// 			}
// 			break;

// 		case PGC_ENUM:
// 			{
// 				struct config_enum *conf = (struct config_enum *) record;

// 				if (conf->show_hook)
// 					val = (*conf->show_hook) ();
// 				else
// 					val = config_enum_lookup_by_value(conf, *conf->variable);
// 			}
// 			break;

// 		default:
// 			/* just to keep compiler quiet */
// 			val = "???";
// 			break;
// 	}

// 	return pstrdup(val);
// }

// static void
// socket_set_nonblocking(bool nonblocking)
// {
// 	if (MyProcPort == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_CONNECTION_DOES_NOT_EXIST),
// 				 errmsg("there is no client connection")));

// 	MyProcPort->noblock = nonblocking;
// }


// //pg
// void
// pq_copymsgbytes(StringInfo msg, char *buf, int datalen)
// {
// 	if (datalen < 0 || datalen > (msg->len - msg->cursor))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 				 errmsg("insufficient data left in message")));
// 	memcpy(buf, &msg->data[msg->cursor], datalen);
// 	msg->cursor += datalen;
// }

// const char *
// pq_getmsgstring(StringInfo msg)
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

// 	return pg_client_to_server(str, slen);
// }
// int
// pq_getmessage(StringInfo s, int maxlen)
// {
// 	int32		len;

// 	Assert(PqCommReadingMsg);

// 	resetStringInfo(s);

// 	/* Read message length word */
// 	if (pq_getbytes((char *) &len, 4) == EOF)
// 	{
// 		ereport(COMMERROR,
// 				(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 				 errmsg("unexpected EOF within message length word")));
// 		return EOF;
// 	}

// 	len = ntohl(len);

// 	if (len < 4 ||
// 		(maxlen > 0 && len > maxlen))
// 	{
// 		ereport(COMMERROR,
// 				(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 				 errmsg("invalid message length")));
// 		return EOF;
// 	}

// 	len -= 4;					/* discount length itself */

// 	if (len > 0)
// 	{
// 		/*
// 		 * Allocate space for message.  If we run out of room (ridiculously
// 		 * large message), we will elog(ERROR), but we want to discard the
// 		 * message body so as not to lose communication sync.
// 		 */
// 		PG_TRY();
// 		{
// 			enlargeStringInfo(s, len);
// 		}
// 		PG_CATCH();
// 		{
// 			if (pq_discardbytes(len) == EOF)
// 				ereport(COMMERROR,
// 						(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 						 errmsg("incomplete message from client")));

// 			/* we discarded the rest of the message so we're back in sync. */
// 			PqCommReadingMsg = false;
// 			PG_RE_THROW();
// 		}
// 		PG_END_TRY();

// 		/* And grab the message */
// 		if (pq_getbytes(s->data, len) == EOF)
// 		{
// 			ereport(COMMERROR,
// 					(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 					 errmsg("incomplete message from client")));
// 			return EOF;
// 		}
// 		s->len = len;
// 		/* Place a trailing null per StringInfo convention */
// 		s->data[len] = '\0';
// 	}

// 	/* finished reading the message. */
// 	PqCommReadingMsg = false;

// 	return 0;
// }

// int
// pq_getbytes(char *s, size_t len)
// {
// 	size_t		amount;

// 	Assert(PqCommReadingMsg);

// 	while (len > 0)
// 	{
// 		while (PqRecvPointer >= PqRecvLength)
// 		{
// 			if (pq_recvbuf())	/* If nothing in buffer, then recv some */
// 				return EOF;		/* Failed to recv data */
// 		}
// 		amount = PqRecvLength - PqRecvPointer;
// 		if (amount > len)
// 			amount = len;
// 		memcpy(s, PqRecvBuffer + PqRecvPointer, amount);
// 		PqRecvPointer += amount;
// 		s += amount;
// 		len -= amount;
// 	}
// 	return 0;
// }

// void
// pq_putemptymessage(char msgtype)
// {
// 	(void) pq_putmessage(msgtype, NULL, 0);
// }
// void
// pq_startmsgread(void)
// {
// 	/*
// 	 * There shouldn't be a read active already, but let's check just to be
// 	 * sure.
// 	 */
// 	if (PqCommReadingMsg)
// 		ereport(FATAL,
// 				(errcode(ERRCODE_PROTOCOL_VIOLATION),
// 				 errmsg("terminating connection because protocol synchronization was lost")));

// 	PqCommReadingMsg = true;
// }

// void
// pq_beginmessage(StringInfo buf, char msgtype)
// {
// 	initStringInfo(buf);

// 	/*
// 	 * We stash the message type into the buffer's cursor field, expecting
// 	 * that the pq_sendXXX routines won't touch it.  We could alternatively
// 	 * make it the first byte of the buffer contents, but this seems easier.
// 	 */
// 	buf->cursor = msgtype;
// }
// void
// pq_sendbyte(StringInfo buf, int byt)
// {
// 	appendStringInfoCharMacro(buf, byt);
// }
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
// void
// pq_endmessage(StringInfo buf)
// {
// 	/* msgtype was saved in cursor field */
// 	(void) pq_putmessage(buf->cursor, buf->data, buf->len);
// 	/* no need to complain about any failure, since pqcomm.c already did */
// 	pfree(buf->data);
// 	buf->data = NULL;
// }

// static struct config_generic *
// find_option(const char *name, bool create_placeholders, int elevel)
// {
// 	const char **key = &name;
// 	struct config_generic **res;
// 	int			i;

// 	Assert(name);

// 	/*
// 	 * By equating const char ** with struct config_generic *, we are assuming
// 	 * the name field is first in config_generic.
// 	 */
// 	res = (struct config_generic **) bsearch((void *) &key,
// 											 (void *) guc_variables,
// 											 num_guc_variables,
// 											 sizeof(struct config_generic *),
// 											 guc_var_compare);
// 	if (res)
// 		return *res;

// 	/*
// 	 * See if the name is an obsolete name for a variable.  We assume that the
// 	 * set of supported old names is short enough that a brute-force search is
// 	 * the best way.
// 	 */
// 	for (i = 0; map_old_guc_names[i] != NULL; i += 2)
// 	{
// 		if (guc_name_compare(name, map_old_guc_names[i]) == 0)
// 			return find_option(map_old_guc_names[i + 1], false, elevel);
// 	}

// 	if (create_placeholders)
// 	{
// 		/*
// 		 * Check if the name is qualified, and if so, add a placeholder.
// 		 */
// 		if (strchr(name, GUC_QUALIFIER_SEPARATOR) != NULL)
// 			return add_placeholder_variable(name, elevel);
// 	}

// 	/* Unknown name */
// 	return NULL;
// }




// static bool
// filter_event_trigger(const char **tag, EventTriggerCacheItem *item)
// {
// 	/*
// 	 * Filter by session replication role, knowing that we never see disabled
// 	 * items down here.
// 	 */
// 	if (SessionReplicationRole == SESSION_REPLICATION_ROLE_REPLICA)
// 	{
// 		if (item->enabled == TRIGGER_FIRES_ON_ORIGIN)
// 			return false;
// 	}
// 	else
// 	{
// 		if (item->enabled == TRIGGER_FIRES_ON_REPLICA)
// 			return false;
// 	}

// 	/* Filter by tags, if any were specified. */
// 	if (item->ntags != 0 && bsearch(tag, item->tag,
// 									item->ntags, sizeof(char *),
// 									pg_qsort_strcmp) == NULL)
// 		return false;

// 	/* if we reach that point, we're not filtering out this item */
// 	return true;
// }



// static List *
// CopyGetAttnums(TupleDesc tupDesc, Relation rel, List *attnamelist)
// {
// 	List	   *attnums = NIL;

// 	if (attnamelist == NIL)
// 	{
// 		/* Generate default column list */
// 		Form_pg_attribute *attr = tupDesc->attrs;
// 		int			attr_count = tupDesc->natts;
// 		int			i;

// 		for (i = 0; i < attr_count; i++)
// 		{
// 			if (attr[i]->attisdropped)
// 				continue;
// 			attnums = lappend_int(attnums, i + 1);
// 		}
// 	}
// 	else
// 	{
// 		/* Validate the user-supplied list and extract attnums */
// 		ListCell   *l;

// 		foreach(l, attnamelist)
// 		{
// 			char	   *name = strVal(lfirst(l));
// 			int			attnum;
// 			int			i;

// 			/* Lookup column name */
// 			attnum = InvalidAttrNumber;
// 			for (i = 0; i < tupDesc->natts; i++)
// 			{
// 				if (tupDesc->attrs[i]->attisdropped)
// 					continue;
// 				if (namestrcmp(&(tupDesc->attrs[i]->attname), name) == 0)
// 				{
// 					attnum = tupDesc->attrs[i]->attnum;
// 					break;
// 				}
// 			}
// 			if (attnum == InvalidAttrNumber)
// 			{
// 				if (rel != NULL)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_UNDEFINED_COLUMN),
// 					errmsg("column \"%s\" of relation \"%s\" does not exist",
// 						   name, RelationGetRelationName(rel))));
// 				else
// 					ereport(ERROR,
// 							(errcode(ERRCODE_UNDEFINED_COLUMN),
// 							 errmsg("column \"%s\" does not exist",
// 									name)));
// 			}
// 			/* Check for duplicates */
// 			if (list_member_int(attnums, attnum))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_DUPLICATE_COLUMN),
// 						 errmsg("column \"%s\" specified more than once",
// 								name)));
// 			attnums = lappend_int(attnums, attnum);
// 		}
// 	}

// 	return attnums;
// } 

// static MultiXactStatus
// get_mxact_status_for_lock(LockTupleMode mode, bool is_update)
// {
// 	int			retval;

// 	if (is_update)
// 		retval = tupleLockExtraInfo[mode].updstatus;
// 	else
// 		retval = tupleLockExtraInfo[mode].lockstatus;

// 	if (retval == -1)
// 		elog(ERROR, "invalid lock tuple mode %d/%s", mode,
// 			 is_update ? "true" : "false");

// 	return (MultiXactStatus) retval;
// } 


// static void
// getRelationTypeDescription(StringInfo buffer, Oid relid, int32 objectSubId)
// {
// 	HeapTuple	relTup;
// 	Form_pg_class relForm;

// 	relTup = SearchSysCache1(RELOID,
// 							 ObjectIdGetDatum(relid));
// 	if (!HeapTupleIsValid(relTup))
// 		elog(ERROR, "cache lookup failed for relation %u", relid);
// 	relForm = (Form_pg_class) GETSTRUCT(relTup);

// 	switch (relForm->relkind)
// 	{
// 		case RELKIND_RELATION:
// 			appendStringInfoString(buffer, "table");
// 			break;
// 		case RELKIND_INDEX:
// 			appendStringInfoString(buffer, "index");
// 			break;
// 		case RELKIND_SEQUENCE:
// 			appendStringInfoString(buffer, "sequence");
// 			break;
// 		case RELKIND_TOASTVALUE:
// 			appendStringInfoString(buffer, "toast table");
// 			break;
// 		case RELKIND_VIEW:
// 			appendStringInfoString(buffer, "view");
// 			break;
// 		case RELKIND_MATVIEW:
// 			appendStringInfoString(buffer, "materialized view");
// 			break;
// 		case RELKIND_COMPOSITE_TYPE:
// 			appendStringInfoString(buffer, "composite type");
// 			break;
// 		case RELKIND_FOREIGN_TABLE:
// 			appendStringInfoString(buffer, "foreign table");
// 			break;
// 		default:
// 			/* shouldn't get here */
// 			appendStringInfoString(buffer, "relation");
// 			break;
// 	}

// 	if (objectSubId != 0)
// 		appendStringInfoString(buffer, " column");

// 	ReleaseSysCache(relTup);
// }

// static void
// getProcedureTypeDescription(StringInfo buffer, Oid procid)
// {
// 	HeapTuple	procTup;
// 	Form_pg_proc procForm;

// 	procTup = SearchSysCache1(PROCOID,
// 							  ObjectIdGetDatum(procid));
// 	if (!HeapTupleIsValid(procTup))
// 		elog(ERROR, "cache lookup failed for procedure %u", procid);
// 	procForm = (Form_pg_proc) GETSTRUCT(procTup);

// 	if (procForm->proisagg)
// 		appendStringInfoString(buffer, "aggregate");
// 	else
// 		appendStringInfoString(buffer, "function");

// 	ReleaseSysCache(procTup);
// }

// static bool
// vacuum_rel(Oid relid, RangeVar *relation, int options, VacuumParams *params)
// {
// 	LOCKMODE	lmode;
// 	Relation	onerel;
// 	LockRelId	onerelid;
// 	Oid			toast_relid;
// 	Oid			save_userid;
// 	int			save_sec_context;
// 	int			save_nestlevel;

// 	Assert(params != NULL);

// 	/* Begin a transaction for vacuuming this relation */
// 	StartTransactionCommand();

// 	/*
// 	 * Functions in indexes may want a snapshot set.  Also, setting a snapshot
// 	 * ensures that RecentGlobalXmin is kept truly recent.
// 	 */
// 	PushActiveSnapshot(GetTransactionSnapshot());

// 	if (!(options & VACOPT_FULL))
// 	{
// 		/*
// 		 * In lazy vacuum, we can set the PROC_IN_VACUUM flag, which lets
// 		 * other concurrent VACUUMs know that they can ignore this one while
// 		 * determining their OldestXmin.  (The reason we don't set it during a
// 		 * full VACUUM is exactly that we may have to run user-defined
// 		 * functions for functional indexes, and we want to make sure that if
// 		 * they use the snapshot set above, any tuples it requires can't get
// 		 * removed from other tables.  An index function that depends on the
// 		 * contents of other tables is arguably broken, but we won't break it
// 		 * here by violating transaction semantics.)
// 		 *
// 		 * We also set the VACUUM_FOR_WRAPAROUND flag, which is passed down by
// 		 * autovacuum; it's used to avoid canceling a vacuum that was invoked
// 		 * in an emergency.
// 		 *
// 		 * Note: these flags remain set until CommitTransaction or
// 		 * AbortTransaction.  We don't want to clear them until we reset
// 		 * MyPgXact->xid/xmin, else OldestXmin might appear to go backwards,
// 		 * which is probably Not Good.
// 		 */
// 		LWLockAcquire(ProcArrayLock, LW_EXCLUSIVE);
// 		MyPgXact->vacuumFlags |= PROC_IN_VACUUM;
// 		if (params->is_wraparound)
// 			MyPgXact->vacuumFlags |= PROC_VACUUM_FOR_WRAPAROUND;
// 		LWLockRelease(ProcArrayLock);
// 	}

// 	/*
// 	 * Check for user-requested abort.  Note we want this to be inside a
// 	 * transaction, so xact.c doesn't issue useless WARNING.
// 	 */
// 	CHECK_FOR_INTERRUPTS();

// 	/*
// 	 * Determine the type of lock we want --- hard exclusive lock for a FULL
// 	 * vacuum, but just ShareUpdateExclusiveLock for concurrent vacuum. Either
// 	 * way, we can be sure that no other backend is vacuuming the same table.
// 	 */
// 	lmode = (options & VACOPT_FULL) ? AccessExclusiveLock : ShareUpdateExclusiveLock;

// 	/*
// 	 * Open the relation and get the appropriate lock on it.
// 	 *
// 	 * There's a race condition here: the rel may have gone away since the
// 	 * last time we saw it.  If so, we don't need to vacuum it.
// 	 *
// 	 * If we've been asked not to wait for the relation lock, acquire it first
// 	 * in non-blocking mode, before calling try_relation_open().
// 	 */
// 	if (!(options & VACOPT_NOWAIT))
// 		onerel = try_relation_open(relid, lmode);
// 	else if (ConditionalLockRelationOid(relid, lmode))
// 		onerel = try_relation_open(relid, NoLock);
// 	else
// 	{
// 		onerel = NULL;
// 		if (IsAutoVacuumWorkerProcess() && params->log_min_duration >= 0)
// 			ereport(LOG,
// 					(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 				   errmsg("skipping vacuum of \"%s\" --- lock not available",
// 						  relation->relname)));
// 	}

// 	if (!onerel)
// 	{
// 		PopActiveSnapshot();
// 		CommitTransactionCommand();
// 		return false;
// 	}

// 	/*
// 	 * Check permissions.
// 	 *
// 	 * We allow the user to vacuum a table if he is superuser, the table
// 	 * owner, or the database owner (but in the latter case, only if it's not
// 	 * a shared relation).  pg_class_ownercheck includes the superuser case.
// 	 *
// 	 * Note we choose to treat permissions failure as a WARNING and keep
// 	 * trying to vacuum the rest of the DB --- is this appropriate?
// 	 */
// 	if (!(pg_class_ownercheck(RelationGetRelid(onerel), GetUserId()) ||
// 		  (pg_database_ownercheck(MyDatabaseId, GetUserId()) && !onerel->rd_rel->relisshared)))
// 	{
// 		if (onerel->rd_rel->relisshared)
// 			ereport(WARNING,
// 				  (errmsg("skipping \"%s\" --- only superuser can vacuum it",
// 						  RelationGetRelationName(onerel))));
// 		else if (onerel->rd_rel->relnamespace == PG_CATALOG_NAMESPACE)
// 			ereport(WARNING,
// 					(errmsg("skipping \"%s\" --- only superuser or database owner can vacuum it",
// 							RelationGetRelationName(onerel))));
// 		else
// 			ereport(WARNING,
// 					(errmsg("skipping \"%s\" --- only table or database owner can vacuum it",
// 							RelationGetRelationName(onerel))));
// 		relation_close(onerel, lmode);
// 		PopActiveSnapshot();
// 		CommitTransactionCommand();
// 		return false;
// 	}

// 	/*
// 	 * Check that it's a vacuumable relation; we used to do this in
// 	 * get_rel_oids() but seems safer to check after we've locked the
// 	 * relation.
// 	 */
// 	if (onerel->rd_rel->relkind != RELKIND_RELATION &&
// 		onerel->rd_rel->relkind != RELKIND_MATVIEW &&
// 		onerel->rd_rel->relkind != RELKIND_TOASTVALUE)
// 	{
// 		ereport(WARNING,
// 				(errmsg("skipping \"%s\" --- cannot vacuum non-tables or special system tables",
// 						RelationGetRelationName(onerel))));
// 		relation_close(onerel, lmode);
// 		PopActiveSnapshot();
// 		CommitTransactionCommand();
// 		return false;
// 	}

// 	/*
// 	 * Silently ignore tables that are temp tables of other backends ---
// 	 * trying to vacuum these will lead to great unhappiness, since their
// 	 * contents are probably not up-to-date on disk.  (We don't throw a
// 	 * warning here; it would just lead to chatter during a database-wide
// 	 * VACUUM.)
// 	 */
// 	if (RELATION_IS_OTHER_TEMP(onerel))
// 	{
// 		relation_close(onerel, lmode);
// 		PopActiveSnapshot();
// 		CommitTransactionCommand();
// 		return false;
// 	}

// 	/*
// 	 * Get a session-level lock too. This will protect our access to the
// 	 * relation across multiple transactions, so that we can vacuum the
// 	 * relation's TOAST table (if any) secure in the knowledge that no one is
// 	 * deleting the parent relation.
// 	 *
// 	 * NOTE: this cannot block, even if someone else is waiting for access,
// 	 * because the lock manager knows that both lock requests are from the
// 	 * same process.
// 	 */
// 	onerelid = onerel->rd_lockInfo.lockRelId;
// 	LockRelationIdForSession(&onerelid, lmode);

// 	/*
// 	 * Remember the relation's TOAST relation for later, if the caller asked
// 	 * us to process it.  In VACUUM FULL, though, the toast table is
// 	 * automatically rebuilt by cluster_rel so we shouldn't recurse to it.
// 	 */
// 	if (!(options & VACOPT_SKIPTOAST) && !(options & VACOPT_FULL))
// 		toast_relid = onerel->rd_rel->reltoastrelid;
// 	else
// 		toast_relid = InvalidOid;

// 	/*
// 	 * Switch to the table owner's userid, so that any index functions are run
// 	 * as that user.  Also lock down security-restricted operations and
// 	 * arrange to make GUC variable changes local to this command. (This is
// 	 * unnecessary, but harmless, for lazy VACUUM.)
// 	 */
// 	GetUserIdAndSecContext(&save_userid, &save_sec_context);
// 	SetUserIdAndSecContext(onerel->rd_rel->relowner,
// 						   save_sec_context | SECURITY_RESTRICTED_OPERATION);
// 	save_nestlevel = NewGUCNestLevel();

// 	/*
// 	 * Do the actual work --- either FULL or "lazy" vacuum
// 	 */
// 	if (options & VACOPT_FULL)
// 	{
// 		/* close relation before vacuuming, but hold lock until commit */
// 		relation_close(onerel, NoLock);
// 		onerel = NULL;

// 		/* VACUUM FULL is now a variant of CLUSTER; see cluster.c */
// 		cluster_rel(relid, InvalidOid, false,
// 					(options & VACOPT_VERBOSE) != 0);
// 	}
// 	else
// 		lazy_vacuum_rel(onerel, options, params,og_knl10->vac_strategy);

// 	/* Roll back any GUC changes executed by index functions */
// 	AtEOXact_GUC(false, save_nestlevel);

// 	/* Restore userid and security context */
// 	SetUserIdAndSecContext(save_userid, save_sec_context);

// 	/* all done with this class, but hold lock until commit */
// 	if (onerel)
// 		relation_close(onerel, NoLock);

// 	/*
// 	 * Complete the transaction and free all temporary memory used.
// 	 */
// 	PopActiveSnapshot();
// 	CommitTransactionCommand();

// 	/*
// 	 * If the relation has a secondary toast rel, vacuum that too while we
// 	 * still hold the session lock on the master table.  Note however that
// 	 * "analyze" will not get done on the toast table.  This is good, because
// 	 * the toaster always uses hardcoded index access and statistics are
// 	 * totally unimportant for toast relations.
// 	 */
// 	if (toast_relid != InvalidOid)
// 		vacuum_rel(toast_relid, relation, options, params);

// 	/*
// 	 * Now release the session-level lock on the master table.
// 	 */
// 	UnlockRelationIdForSession(&onerelid, lmode);

// 	/* Report that we really did it. */
// 	return true;
// }

static bool
ResourceArrayRemove(ResourceArray *resarr, Datum value)
{
	uint32		i,
				idx,
				lastidx = resarr->lastidx;

	Assert(value != resarr->invalidval);

	/* Search through all items, but try lastidx first. */
	if (RESARRAY_IS_ARRAY(resarr))
	{
		if (lastidx < resarr->nitems &&
			resarr->itemsarr[lastidx] == value)
		{
			resarr->itemsarr[lastidx] = resarr->itemsarr[resarr->nitems - 1];
			resarr->nitems--;
			/* Update lastidx to make reverse-order removals fast. */
			resarr->lastidx = resarr->nitems - 1;
			return true;
		}
		for (i = 0; i < resarr->nitems; i++)
		{
			if (resarr->itemsarr[i] == value)
			{
				resarr->itemsarr[i] = resarr->itemsarr[resarr->nitems - 1];
				resarr->nitems--;
				/* Update lastidx to make reverse-order removals fast. */
				resarr->lastidx = resarr->nitems - 1;
				return true;
			}
		}
	}
	else
	{
		uint32		mask = resarr->capacity - 1;

		if (lastidx < resarr->capacity &&
			resarr->itemsarr[lastidx] == value)
		{
			resarr->itemsarr[lastidx] = resarr->invalidval;
			resarr->nitems--;
			return true;
		}
		Datum tsdb_tem = hash_any((const unsigned char *)&value, sizeof(value));//tsdb 这里原来没有强制类型转化
		idx = DatumGetUInt32(tsdb_tem) & mask;
		for (i = 0; i < resarr->capacity; i++)
		{
			if (resarr->itemsarr[idx] == value)
			{
				resarr->itemsarr[idx] = resarr->invalidval;
				resarr->nitems--;
				return true;
			}
			idx = (idx + 1) & mask;
		}
	}

	return false;
}


static void
ResourceArrayAdd(ResourceArray *resarr, Datum value)
{
	uint32		idx;

	Assert(value != resarr->invalidval);
	Assert(resarr->nitems < resarr->maxitems);

	if (RESARRAY_IS_ARRAY(resarr))
	{
		/* Append to linear array. */
		idx = resarr->nitems;
	}
	else
	{
		/* Insert into first free slot at or after hash location. */
		uint32		mask = resarr->capacity - 1;
		Datum tsdb_tem = hash_any((const unsigned char *)&value, sizeof(value));//tsdb 这里原来没有强制类型转化
		idx = DatumGetUInt32(tsdb_tem) & mask;
		for (;;)
		{
			if (resarr->itemsarr[idx] == resarr->invalidval)
				break;
			idx = (idx + 1) & mask;
		}
	}
	resarr->lastidx = idx;
	resarr->itemsarr[idx] = value;
	resarr->nitems++;
}


static void
ResourceArrayEnlarge(ResourceArray *resarr)
{
	uint32		i,
				oldcap,
				newcap;
	Datum	   *olditemsarr;
	Datum	   *newitemsarr;

	if (resarr->nitems < resarr->maxitems)
		return;					/* no work needed */

	olditemsarr = resarr->itemsarr;
	oldcap = resarr->capacity;

	/* Double the capacity of the array (capacity must stay a power of 2!) */
	newcap = (oldcap > 0) ? oldcap * 2 : RESARRAY_INIT_SIZE;
	newitemsarr = (Datum *) MemoryContextAlloc(TopMemoryContext,
											   newcap * sizeof(Datum));
	for (i = 0; i < newcap; i++)
		newitemsarr[i] = resarr->invalidval;

	/* We assume we can't fail below this point, so OK to scribble on resarr */
	resarr->itemsarr = newitemsarr;
	resarr->capacity = newcap;
	resarr->maxitems = RESARRAY_MAX_ITEMS(newcap);
	resarr->nitems = 0;

	if (olditemsarr != NULL)
	{
		/*
		 * Transfer any pre-existing entries into the new array; they don't
		 * necessarily go where they were before, so this simple logic is the
		 * best way.  Note that if we were managing the set as a simple array,
		 * the entries after nitems are garbage, but that shouldn't matter
		 * because we won't get here unless nitems was equal to oldcap.
		 */
		for (i = 0; i < oldcap; i++)
		{
			if (olditemsarr[i] != resarr->invalidval)
				ResourceArrayAdd(resarr, olditemsarr[i]);
		}

		/* And release old array. */
		pfree(olditemsarr);
	}

	Assert(resarr->nitems < resarr->maxitems);
}

static int
DecodeTextArrayToCString(Datum array, char ***cstringp)
{
	ArrayType  *arr = DatumGetArrayTypeP(array);
	Datum	   *elems;
	char	  **cstring;
	int			i;
	int			nelems;

	if (ARR_NDIM(arr) != 1 || ARR_HASNULL(arr) || ARR_ELEMTYPE(arr) != TEXTOID)
		elog(ERROR, "expected 1-D text array");
	deconstruct_array(arr, TEXTOID, -1, false, 'i', &elems, NULL, &nelems);

	cstring =(char **) palloc(nelems * sizeof(char *));
	for (i = 0; i < nelems; ++i)
		cstring[i] = TextDatumGetCString(elems[i]);

	pfree(elems);
	*cstringp = cstring;
	return nelems;
}

static void
InvalidateEventCacheCallback(Datum arg, int cacheid, uint32 hashvalue)
{
	/*
	 * If the cache isn't valid, then there might be a rebuild in progress, so
	 * we can't immediately blow it away.  But it's advantageous to do this
	 * when possible, so as to immediately free memory.
	 */
	if (EventTriggerCacheState == ETCS_VALID)
	{
		MemoryContextResetAndDeleteChildren(EventTriggerCacheContext);
		EventTriggerCache = NULL;
	}

	/* Mark cache for rebuild. */
	EventTriggerCacheState = ETCS_NEEDS_REBUILD;
}

//在EventCacheLookup 中使用,之后解决报错
static void
BuildEventTriggerCache(void)
{
	HASHCTL		ctl;
	HTAB	   *cache;
	MemoryContext oldcontext;
	Relation	rel;
	Relation	irel;
	SysScanDesc scan;

	if (EventTriggerCacheContext != NULL)
	{
		/*
		 * Free up any memory already allocated in EventTriggerCacheContext.
		 * This can happen either because a previous rebuild failed, or
		 * because an invalidation happened before the rebuild was complete.
		 */
		MemoryContextResetAndDeleteChildren(EventTriggerCacheContext);
	}
	else
	{
		/*
		 * This is our first time attempting to build the cache, so we need to
		 * set up the memory context and register a syscache callback to
		 * capture future invalidation events.
		 */
		if (u_sess->cache_mem_cxt == NULL)
			CreateCacheMemoryContext();
		EventTriggerCacheContext =
			AllocSetContextCreate(u_sess->cache_mem_cxt,
								  "EventTriggerCache",
								  ALLOCSET_DEFAULT_SIZES);
		CacheRegisterThreadSyscacheCallback(EVENTTRIGGEROID,
									  InvalidateEventCacheCallback,
									  (Datum) 0);
	}

	/* Switch to correct memory context. */
	oldcontext = MemoryContextSwitchTo(EventTriggerCacheContext);

	/* Prevent the memory context from being nuked while we're rebuilding. */
	EventTriggerCacheState = ETCS_REBUILD_STARTED;

	/* Create new hash table. */
	MemSet(&ctl, 0, sizeof(ctl));
	ctl.keysize = sizeof(EventTriggerEvent);
	ctl.entrysize = sizeof(EventTriggerCacheEntry);
	ctl.hcxt = EventTriggerCacheContext;
	cache = hash_create("Event Trigger Cache", 32, &ctl,
						HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);

	/*
	 * Prepare to scan pg_event_trigger in name order.
	 */
	rel = relation_open(EventTriggerRelationId, AccessShareLock);
	irel = index_open(EventTriggerNameIndexId, AccessShareLock);
	scan = systable_beginscan_ordered(rel, irel, NULL, 0, NULL);

	/*
	 * Build a cache item for each pg_event_trigger tuple, and append each one
	 * to the appropriate cache entry.
	 */
	for (;;)
	{
		HeapTuple	tup;
		Form_pg_event_trigger form;
		char	   *evtevent;
		EventTriggerEvent event;
		EventTriggerCacheItem *item;
		Datum		evttags;
		bool		evttags_isnull;
		EventTriggerCacheEntry *entry;
		bool		found;

		/* Get next tuple. */
		tup = systable_getnext_ordered(scan, ForwardScanDirection);
		if (!HeapTupleIsValid(tup))
			break;

		/* Skip trigger if disabled. */
		form = (Form_pg_event_trigger) GETSTRUCT(tup);
		if (form->evtenabled == TRIGGER_DISABLED)
			continue;

		/* Decode event name. */
		evtevent = NameStr(form->evtevent);
		if (strcmp(evtevent, "ddl_command_start") == 0)
			event = EVT_DDLCommandStart;
		else if (strcmp(evtevent, "ddl_command_end") == 0)
			event = EVT_DDLCommandEnd;
		else if (strcmp(evtevent, "sql_drop") == 0)
			event = EVT_SQLDrop;
		else if (strcmp(evtevent, "table_rewrite") == 0)
			event = EVT_TableRewrite;
		else
			continue;

		/* Allocate new cache item. */
		item =(EventTriggerCacheItem*) palloc0(sizeof(EventTriggerCacheItem));
		item->fnoid = form->evtfoid;
		item->enabled = form->evtenabled;

		/* Decode and sort tags array. */
		evttags = heap_getattr_tsdb(tup, Anum_pg_event_trigger_evttags,
							   RelationGetDescr(rel), &evttags_isnull);
		if (!evttags_isnull)
		{
			item->ntags = DecodeTextArrayToCString(evttags, &item->tag);
			qsort(item->tag, item->ntags, sizeof(char *), pg_qsort_strcmp);
		}

		/* Add to cache entry. */
		entry =(EventTriggerCacheEntry *) hash_search(cache, &event, HASH_ENTER, &found);
		if (found)
			entry->triggerlist = lappend(entry->triggerlist, item);
		else
			entry->triggerlist = list_make1(item);
	}

	/* Done with pg_event_trigger scan. */
	systable_endscan_ordered(scan);
	index_close(irel, AccessShareLock);
	relation_close(rel, AccessShareLock);

	/* Restore previous memory context. */
	MemoryContextSwitchTo(oldcontext);

	/* Install new cache. */
	EventTriggerCache = cache;

	/*
	 * If the cache has been invalidated since we entered this routine, we
	 * still use and return the cache we just finished constructing, to avoid
	 * infinite loops, but we leave the cache marked stale so that we'll
	 * rebuild it again on next access.  Otherwise, we mark the cache valid.
	 */
	if (EventTriggerCacheState == ETCS_REBUILD_STARTED)
		EventTriggerCacheState = ETCS_VALID;
}

// static uint64
// dsm_control_bytes_needed(uint32 nitems)
// {
// 	return offsetof(dsm_control_header, item)
// 		+ sizeof(dsm_control_item) * (uint64) nitems;
// } 
// static bool
// dsm_control_segment_sane(dsm_control_header *control, Size mapped_size)
// {
// 	if (mapped_size < offsetof(dsm_control_header, item))
// 		return false;			/* Mapped size too short to read header. */
// 	if (control->magic != PG_DYNSHMEM_CONTROL_MAGIC)
// 		return false;			/* Magic number doesn't match. */
// 	if (dsm_control_bytes_needed(control->maxitems) > mapped_size)
// 		return false;			/* Max item count won't fit in map. */
// 	if (control->nitems > control->maxitems)
// 		return false;			/* Overfull. */
// 	return true;
// }


// static void
// dsm_backend_startup(void)
// {
// 	/* If dynamic shared memory is disabled, reject this. */
// 	if (dynamic_shared_memory_type == DSM_IMPL_NONE)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("dynamic shared memory is disabled"),
// 				 errhint("Set dynamic_shared_memory_type to a value other than \"none\".")));

// #ifdef EXEC_BACKEND
// 	{
// 		void	   *control_address = NULL;

// 		/* Attach control segment. */
// 		Assert(dsm_control_handle != 0);
// 		dsm_impl_op(DSM_OP_ATTACH, dsm_control_handle, 0,
// 					&dsm_control_impl_private, &control_address,
// 					&dsm_control_mapped_size, ERROR);
// 		dsm_control =(dsm_control_header *) control_address;
// 		/* If control segment doesn't look sane, something is badly wrong. */
// 		if (!dsm_control_segment_sane(dsm_control, dsm_control_mapped_size))
// 		{
// 			dsm_impl_op(DSM_OP_DETACH, dsm_control_handle, 0,
// 						&dsm_control_impl_private, &control_address,
// 						&dsm_control_mapped_size, WARNING);
// 			ereport(FATAL,
// 					(errcode(ERRCODE_INTERNAL_ERROR),
// 			  errmsg("dynamic shared memory control segment is not valid")));
// 		}
// 	}
// #endif

// 	dsm_init_done = true;
// }

// static dsm_segment *
// dsm_create_descriptor(void)
// {
// 	dsm_segment *seg;

// 	if (og_knl->CurrentResourceOwner)
// 		ResourceOwnerEnlargeDSMs(og_knl->CurrentResourceOwner);

// 	seg =(dsm_segment*) MemoryContextAlloc(TopMemoryContext, sizeof(dsm_segment));
// 	dlist_push_head(&dsm_segment_list, &seg->node);

// 	/* seg->handle must be initialized by the caller */
// 	seg->control_slot = INVALID_CONTROL_SLOT;
// 	seg->impl_private = NULL;
// 	seg->mapped_address = NULL;
// 	seg->mapped_size = 0;

// 	seg->resowner = og_knl->CurrentResourceOwner;
// 	if (og_knl->CurrentResourceOwner)
// 		ResourceOwnerRememberDSM(og_knl->CurrentResourceOwner, seg);

// 	slist_init(&seg->on_detach);

// 	return seg;
// }
// static uint8
// compute_infobits(uint16 infomask, uint16 infomask2)
// {
// 	return
// 		((infomask & HEAP_XMAX_IS_MULTI) != 0 ? XLHL_XMAX_IS_MULTI : 0) |
// 		((infomask & HEAP_XMAX_LOCK_ONLY) != 0 ? XLHL_XMAX_LOCK_ONLY : 0) |
// 		((infomask & HEAP_XMAX_EXCL_LOCK) != 0 ? XLHL_XMAX_EXCL_LOCK : 0) |
// 	/* note we ignore HEAP_XMAX_SHR_LOCK here */
// 		((infomask & HEAP_XMAX_KEYSHR_LOCK) != 0 ? XLHL_XMAX_KEYSHR_LOCK : 0) |
// 		((infomask2 & HEAP_KEYS_UPDATED) != 0 ?
// 		 XLHL_KEYS_UPDATED : 0);
// } 


// static void
// compute_new_xmax_infomask(TransactionId xmax, uint16 old_infomask,
// 						  uint16 old_infomask2, TransactionId add_to_xmax,
// 						  LockTupleMode mode, bool is_update,
// 						  TransactionId *result_xmax, uint16 *result_infomask,
// 						  uint16 *result_infomask2)
// {
// 	TransactionId new_xmax;
// 	uint16		new_infomask,
// 				new_infomask2;

// 	Assert(TransactionIdIsCurrentTransactionId(add_to_xmax));

// l5:
// 	new_infomask = 0;
// 	new_infomask2 = 0;
// 	if (old_infomask & HEAP_XMAX_INVALID)
// 	{
// 		/*
// 		 * No previous locker; we just insert our own TransactionId.
// 		 *
// 		 * Note that it's critical that this case be the first one checked,
// 		 * because there are several blocks below that come back to this one
// 		 * to implement certain optimizations; old_infomask might contain
// 		 * other dirty bits in those cases, but we don't really care.
// 		 */
// 		if (is_update)
// 		{
// 			new_xmax = add_to_xmax;
// 			if (mode == LockTupleExclusive)
// 				new_infomask2 |= HEAP_KEYS_UPDATED;
// 		}
// 		else
// 		{
// 			new_infomask |= HEAP_XMAX_LOCK_ONLY;
// 			switch (mode)
// 			{
// 				case LockTupleKeyShare:
// 					new_xmax = add_to_xmax;
// 					new_infomask |= HEAP_XMAX_KEYSHR_LOCK;
// 					break;
// 				case LockTupleShare:
// 					new_xmax = add_to_xmax;
// 					new_infomask |= HEAP_XMAX_SHR_LOCK;
// 					break;
// 				case LockTupleNoKeyExclusive:
// 					new_xmax = add_to_xmax;
// 					new_infomask |= HEAP_XMAX_EXCL_LOCK;
// 					break;
// 				case LockTupleExclusive:
// 					new_xmax = add_to_xmax;
// 					new_infomask |= HEAP_XMAX_EXCL_LOCK;
// 					new_infomask2 |= HEAP_KEYS_UPDATED;
// 					break;
// 				default:
// 					new_xmax = InvalidTransactionId;	/* silence compiler */
// 					elog(ERROR, "invalid lock mode");
// 			}
// 		}
// 	}
// 	else if (old_infomask & HEAP_XMAX_IS_MULTI)
// 	{
// 		MultiXactStatus new_status;

// 		/*
// 		 * Currently we don't allow XMAX_COMMITTED to be set for multis, so
// 		 * cross-check.
// 		 */
// 		Assert(!(old_infomask & HEAP_XMAX_COMMITTED));

// 		/*
// 		 * A multixact together with LOCK_ONLY set but neither lock bit set
// 		 * (i.e. a pg_upgraded share locked tuple) cannot possibly be running
// 		 * anymore.  This check is critical for databases upgraded by
// 		 * pg_upgrade; both MultiXactIdIsRunning and MultiXactIdExpand assume
// 		 * that such multis are never passed.
// 		 */
// 		if (HEAP_LOCKED_UPGRADED(old_infomask))
// 		{
// 			old_infomask &= ~HEAP_XMAX_IS_MULTI;
// 			old_infomask |= HEAP_XMAX_INVALID;
// 			goto l5;
// 		}

// 		/*
// 		 * If the XMAX is already a MultiXactId, then we need to expand it to
// 		 * include add_to_xmax; but if all the members were lockers and are
// 		 * all gone, we can do away with the IS_MULTI bit and just set
// 		 * add_to_xmax as the only locker/updater.  If all lockers are gone
// 		 * and we have an updater that aborted, we can also do without a
// 		 * multi.
// 		 *
// 		 * The cost of doing GetMultiXactIdMembers would be paid by
// 		 * MultiXactIdExpand if we weren't to do this, so this check is not
// 		 * incurring extra work anyhow.
// 		 */
// 		if (!MultiXactIdIsRunning(xmax, HEAP_XMAX_IS_LOCKED_ONLY_tsdb(old_infomask)))
// 		{
// 			if (HEAP_XMAX_IS_LOCKED_ONLY_tsdb(old_infomask) ||
// 				!TransactionIdDidCommit(MultiXactIdGetUpdateXid(xmax,
// 															  old_infomask)))
// 			{
// 				/*
// 				 * Reset these bits and restart; otherwise fall through to
// 				 * create a new multi below.
// 				 */
// 				old_infomask &= ~HEAP_XMAX_IS_MULTI;
// 				old_infomask |= HEAP_XMAX_INVALID;
// 				goto l5;
// 			}
// 		}

// 		new_status = get_mxact_status_for_lock(mode, is_update);

// 		new_xmax = MultiXactIdExpand((MultiXactId) xmax, add_to_xmax,
// 									 new_status);
// 		GetMultiXactIdHintBits(new_xmax, &new_infomask, &new_infomask2);
// 	}
// 	else if (old_infomask & HEAP_XMAX_COMMITTED)
// 	{
// 		/*
// 		 * It's a committed update, so we need to preserve him as updater of
// 		 * the tuple.
// 		 */
// 		MultiXactStatus status;
// 		MultiXactStatus new_status;

// 		if (old_infomask2 & HEAP_KEYS_UPDATED)
// 			status = MultiXactStatusUpdate;
// 		else
// 			status = MultiXactStatusNoKeyUpdate;

// 		new_status = get_mxact_status_for_lock(mode, is_update);

// 		/*
// 		 * since it's not running, it's obviously impossible for the old
// 		 * updater to be identical to the current one, so we need not check
// 		 * for that case as we do in the block above.
// 		 */
// 		new_xmax = MultiXactIdCreate(xmax, status, add_to_xmax, new_status);
// 		GetMultiXactIdHintBits(new_xmax, &new_infomask, &new_infomask2);
// 	}
// 	else if (TransactionIdIsInProgress(xmax))
// 	{
// 		/*
// 		 * If the XMAX is a valid, in-progress TransactionId, then we need to
// 		 * create a new MultiXactId that includes both the old locker or
// 		 * updater and our own TransactionId.
// 		 */
// 		MultiXactStatus new_status;
// 		MultiXactStatus old_status;
// 		LockTupleMode old_mode;

// 		if (HEAP_XMAX_IS_LOCKED_ONLY_tsdb(old_infomask))
// 		{
// 			if (HEAP_XMAX_IS_KEYSHR_LOCKED(old_infomask))
// 				old_status = MultiXactStatusForKeyShare;
// 			else if (HEAP_XMAX_IS_SHR_LOCKED(old_infomask))
// 				old_status = MultiXactStatusForShare;
// 			else if (HEAP_XMAX_IS_EXCL_LOCKED(old_infomask))
// 			{
// 				if (old_infomask2 & HEAP_KEYS_UPDATED)
// 					old_status = MultiXactStatusForUpdate;
// 				else
// 					old_status = MultiXactStatusForNoKeyUpdate;
// 			}
// 			else
// 			{
// 				/*
// 				 * LOCK_ONLY can be present alone only when a page has been
// 				 * upgraded by pg_upgrade.  But in that case,
// 				 * TransactionIdIsInProgress() should have returned false.  We
// 				 * assume it's no longer locked in this case.
// 				 */
// 				elog(WARNING, "LOCK_ONLY found for Xid in progress %u", xmax);
// 				old_infomask |= HEAP_XMAX_INVALID;
// 				old_infomask &= ~HEAP_XMAX_LOCK_ONLY;
// 				goto l5;
// 			}
// 		}
// 		else
// 		{
// 			/* it's an update, but which kind? */
// 			if (old_infomask2 & HEAP_KEYS_UPDATED)
// 				old_status = MultiXactStatusUpdate;
// 			else
// 				old_status = MultiXactStatusNoKeyUpdate;
// 		}

// 		old_mode =(LockTupleMode) TUPLOCK_from_mxstatus(old_status);

// 		/*
// 		 * If the lock to be acquired is for the same TransactionId as the
// 		 * existing lock, there's an optimization possible: consider only the
// 		 * strongest of both locks as the only one present, and restart.
// 		 */
// 		if (xmax == add_to_xmax)
// 		{
// 			/*
// 			 * Note that it's not possible for the original tuple to be
// 			 * updated: we wouldn't be here because the tuple would have been
// 			 * invisible and we wouldn't try to update it.  As a subtlety,
// 			 * this code can also run when traversing an update chain to lock
// 			 * future versions of a tuple.  But we wouldn't be here either,
// 			 * because the add_to_xmax would be different from the original
// 			 * updater.
// 			 */
// 			Assert(HEAP_XMAX_IS_LOCKED_ONLY_tsdb(old_infomask));

// 			/* acquire the strongest of both */
// 			if (mode < old_mode)
// 				mode = old_mode;
// 			/* mustn't touch is_update */

// 			old_infomask |= HEAP_XMAX_INVALID;
// 			goto l5;
// 		}

// 		/* otherwise, just fall back to creating a new multixact */
// 		new_status = get_mxact_status_for_lock(mode, is_update);
// 		new_xmax = MultiXactIdCreate(xmax, old_status,
// 									 add_to_xmax, new_status);
// 		GetMultiXactIdHintBits(new_xmax, &new_infomask, &new_infomask2);
// 	}
// 	else if (!HEAP_XMAX_IS_LOCKED_ONLY_tsdb(old_infomask) &&
// 			 TransactionIdDidCommit(xmax))
// 	{
// 		/*
// 		 * It's a committed update, so we gotta preserve him as updater of the
// 		 * tuple.
// 		 */
// 		MultiXactStatus status;
// 		MultiXactStatus new_status;

// 		if (old_infomask2 & HEAP_KEYS_UPDATED)
// 			status = MultiXactStatusUpdate;
// 		else
// 			status = MultiXactStatusNoKeyUpdate;

// 		new_status = get_mxact_status_for_lock(mode, is_update);

// 		/*
// 		 * since it's not running, it's obviously impossible for the old
// 		 * updater to be identical to the current one, so we need not check
// 		 * for that case as we do in the block above.
// 		 */
// 		new_xmax = MultiXactIdCreate(xmax, status, add_to_xmax, new_status);
// 		GetMultiXactIdHintBits(new_xmax, &new_infomask, &new_infomask2);
// 	}
// 	else
// 	{
// 		/*
// 		 * Can get here iff the locking/updating transaction was running when
// 		 * the infomask was extracted from the tuple, but finished before
// 		 * TransactionIdIsInProgress got to run.  Deal with it as if there was
// 		 * no locker at all in the first place.
// 		 */
// 		old_infomask |= HEAP_XMAX_INVALID;
// 		goto l5;
// 	}

// 	*result_infomask = new_infomask;
// 	*result_infomask2 = new_infomask2;
// 	*result_xmax = new_xmax;
// }

// static uint64
// shm_mq_get_bytes_written(volatile shm_mq *mq, bool *detached)
// {
// 	uint64		v;

// 	SpinLockAcquire(&mq->mq_mutex);
// 	v = mq->mq_bytes_written;
// 	*detached = mq->mq_detached;
// 	SpinLockRelease(&mq->mq_mutex);

// 	return v;
// } 

// static void
// IdleInTransactionSessionTimeoutHandler(void)
// {
// 	IdleInTransactionSessionTimeoutPending = true;
// 	InterruptPending = true;
// 	SetLatch(MyLatch);
// } 


// static void
// shm_mq_inc_bytes_read(shm_mq *mq, Size n)
// {
// 	PGPROC	   *sender;

// 	/*
// 	 * Separate prior reads of mq_ring from the increment of mq_bytes_read
// 	 * which follows.  This pairs with the full barrier in
// 	 * shm_mq_send_bytes(). We only need a read barrier here because the
// 	 * increment of mq_bytes_read is actually a read followed by a dependent
// 	 * write.
// 	 */
// 	pg_read_barrier();

// 	/*
// 	 * There's no need to use pg_atomic_fetch_add_u64 here, because nobody
// 	 * else can be changing this value.  This method should be cheaper.
// 	 */
// 	pg_atomic_write_u64(&mq->mq_bytes_read,
// 						pg_atomic_read_u64(&mq->mq_bytes_read) + n);

// 	/*
// 	 * We shouldn't have any bytes to read without a sender, so we can read
// 	 * mq_sender here without a lock.  Once it's initialized, it can't change.
// 	 */
// 	sender = mq->mq_sender;
// 	Assert(sender != NULL);
// 	SetLatch(&sender->procLatch);
// }
// static shm_mq_result
// shm_mq_receive_bytes(shm_mq *mq, Size bytes_needed, bool nowait,
// 					 Size *nbytesp, void **datap)
// {
// 	Size		ringsize = mq->mq_ring_size;
// 	uint64		used;
// 	uint64		written;

// 	for (;;)
// 	{
// 		Size		offset;
// 		bool		detached;

// 		/* Get bytes written, so we can compute what's available to read. */
// 		written = shm_mq_get_bytes_written(mq, &detached);
// 		used = written - mq->mq_bytes_read;
// 		Assert(used <= ringsize);
// 		offset = mq->mq_bytes_read % (uint64) ringsize;

// 		/* If we have enough data or buffer has wrapped, we're done. */
// 		if (used >= bytes_needed || offset + used >= ringsize)
// 		{
// 			*nbytesp = Min(used, ringsize - offset);
// 			*datap = &mq->mq_ring[mq->mq_ring_offset + offset];
// 			return SHM_MQ_SUCCESS;
// 		}

// 		/*
// 		 * Fall out before waiting if the queue has been detached.
// 		 *
// 		 * Note that we don't check for this until *after* considering whether
// 		 * the data already available is enough, since the receiver can finish
// 		 * receiving a message stored in the buffer even after the sender has
// 		 * detached.
// 		 */
// 		if (detached)
// 			return SHM_MQ_DETACHED;

// 		/* Skip manipulation of our latch if nowait = true. */
// 		if (nowait)
// 			return SHM_MQ_WOULD_BLOCK;

// 		/*
// 		 * Wait for our latch to be set.  It might already be set for some
// 		 * unrelated reason, but that'll just result in one extra trip through
// 		 * the loop.  It's worth it to avoid resetting the latch at top of
// 		 * loop, because setting an already-set latch is much cheaper than
// 		 * setting one that has been reset.
// 		 */
// 		WaitLatch(MyLatch, WL_LATCH_SET, 0);

// 		/* Reset the latch so we don't spin. */
// 		ResetLatch(MyLatch);

// 		/* An interrupt may have occurred while we were waiting. */
// 		CHECK_FOR_INTERRUPTS();
// 	}
// } 




// static const char* storage_name(char c)
// {
//     switch (c) {
//         case 'p':
//             return "PLAIN";
//         case 'm':
//             return "MAIN";
//         case 'x':
//             return "EXTENDED";
//         case 'e':
//             return "EXTERNAL";
//         default:
//             return "???";
//     }
// } 

// static bool
// heap_acquire_tuplock(Relation relation, ItemPointer tid, LockTupleMode mode,
// 					 LockWaitPolicy wait_policy, bool *have_tuple_lock)
// {
// 	if (*have_tuple_lock)
// 		return true;

// 	switch (wait_policy)
// 	{
// 		case LockWaitBlock:
// 			LockTupleTuplock(relation, tid, mode);
// 			break;

// 		case LockWaitSkip:
// 			if (!ConditionalLockTupleTuplock(relation, tid, mode))
// 				return false;
// 			break;

// 		case LockWaitError:
// 			if (!ConditionalLockTupleTuplock(relation, tid, mode))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 					errmsg("could not obtain lock on row in relation \"%s\"",
// 						   RelationGetRelationName(relation))));
// 			break;
// 	}
// 	*have_tuple_lock = true;

// 	return true;
// }

//这个是og内的静态函数
static bool index_recheck_constraint(
    Relation index, Oid* constr_procs, Datum* existing_values, const bool* existing_isnull, Datum* new_values)
{
    int indnkeyatts = IndexRelationGetNumberOfKeyAttributes(index);
    int i;

    for (i = 0; i < indnkeyatts; i++) {
        /* Assume the exclusion operators are strict */
        if (existing_isnull[i]) {
            return false;
        }

        if (!DatumGetBool(
                OidFunctionCall2Coll(constr_procs[i], index->rd_indcollation[i], existing_values[i], new_values[i]))) {
            return false;
        }
    }

    return true;
} 

static bool
check_exclusion_or_unique_constraint(Relation heap, Relation index,
									 IndexInfo *indexInfo,
									 ItemPointer tupleid,
									 Datum *values, bool *isnull,
									 EState *estate, bool newIndex,
									 CEOUC_WAIT_MODE waitMode,
									 bool violationOK,
									 ItemPointer conflictTid)
{
	Oid		   *constr_procs;
	uint16	   *constr_strats;
	Oid		   *index_collations = index->rd_indcollation;
	int			index_natts = index->rd_index->indnatts;
	IndexScanDesc index_scan;
	HeapTuple	tup;
	ScanKeyData scankeys[INDEX_MAX_KEYS];
	SnapshotData DirtySnapshot;
	int			i;
	bool		conflict;
	bool		found_self;
	ExprContext *econtext;
	TupleTableSlot *existing_slot;
	TupleTableSlot *save_scantuple;

	if (indexInfo->ii_ExclusionOps)
	{
		constr_procs = indexInfo->ii_ExclusionProcs;
		constr_strats = indexInfo->ii_ExclusionStrats;
	}
	else
	{
		constr_procs = indexInfo->ii_UniqueProcs;
		constr_strats = indexInfo->ii_UniqueStrats;
	}

	/*
	 * If any of the input values are NULL, the constraint check is assumed to
	 * pass (i.e., we assume the operators are strict).
	 */
	for (i = 0; i < index_natts; i++)
	{
		if (isnull[i])
			return true;
	}

	/*
	 * Search the tuples that are in the index for any violations, including
	 * tuples that aren't visible yet.
	 */
	InitDirtySnapshot(DirtySnapshot);

	for (i = 0; i < index_natts; i++)
	{
		ScanKeyEntryInitialize(&scankeys[i],
							   0,
							   i + 1,
							   constr_strats[i],
							   InvalidOid,
							   index_collations[i],
							   constr_procs[i],
							   values[i]);
	}

	/*
	 * Need a TupleTableSlot to put existing tuples in.
	 *
	 * To use FormIndexDatum, we have to make the econtext's scantuple point
	 * to this slot.  Be sure to save and restore caller's value for
	 * scantuple.
	 */
	existing_slot = MakeSingleTupleTableSlot(RelationGetDescr(heap));

	econtext = GetPerTupleExprContext(estate);
	save_scantuple = econtext->ecxt_scantuple;
	econtext->ecxt_scantuple = existing_slot;

	/*
	 * May have to restart scan from this point if a potential conflict is
	 * found.
	 */
retry:
	conflict = false;
	found_self = false;
	index_scan = index_beginscan(heap, index, &DirtySnapshot, index_natts, 0);
	index_rescan(index_scan, scankeys, index_natts, NULL, 0);

	while ((tup =(HeapTuple) index_getnext(index_scan,
								ForwardScanDirection)) != NULL
								)
	{
		TransactionId xwait;
		ItemPointerData ctid_wait;
		XLTW_Oper	reason_wait;
		Datum		existing_values[INDEX_MAX_KEYS];
		bool		existing_isnull[INDEX_MAX_KEYS];
		char	   *error_new;
		char	   *error_existing;

		/*
		 * Ignore the entry for the tuple we're trying to check.
		 */
		if (ItemPointerIsValid(tupleid) &&
			ItemPointerEquals(tupleid, &tup->t_self))
		{
			if (found_self)		/* should not happen */
				elog(ERROR, "found self tuple multiple times in index \"%s\"",
					 RelationGetRelationName(index));
			found_self = true;
			continue;
		}

		/*
		 * Extract the index column values and isnull flags from the existing
		 * tuple.
		 */
		ExecStoreTuple(tup, existing_slot, InvalidBuffer, false);
		FormIndexDatum(indexInfo, existing_slot, estate,
					   existing_values, existing_isnull);

		/* If lossy indexscan, must recheck the condition */
		if (index_scan->xs_recheck)
		{
			//这个是静态函数 gausskernel/runtime/executor/execUtils.cpp
			if (!index_recheck_constraint(index,
										  constr_procs,
										  existing_values,
										  existing_isnull,
										  values))
				continue;		/* tuple doesn't actually match, so no
								 * conflict */
		}

		/*
		 * At this point we have either a conflict or a potential conflict.
		 *
		 * If an in-progress transaction is affecting the visibility of this
		 * tuple, we need to wait for it to complete and then recheck (unless
		 * the caller requested not to).  For simplicity we do rechecking by
		 * just restarting the whole scan --- this case probably doesn't
		 * happen often enough to be worth trying harder, and anyway we don't
		 * want to hold any index internal locks while waiting.
		 */
		xwait = TransactionIdIsValid(DirtySnapshot.xmin) ?
			DirtySnapshot.xmin : DirtySnapshot.xmax;

		if (TransactionIdIsValid(xwait) &&
			(waitMode == CEOUC_WAIT ||
			 (waitMode == CEOUC_LIVELOCK_PREVENTING_WAIT &&
			 //SnapshotData需要加入成员speculativeToken
			  DirtySnapshot.speculativeToken &&
			  TransactionIdPrecedes(GetCurrentTransactionId(), xwait))))
		{
			ctid_wait = tup->t_data->t_ctid;
			reason_wait = indexInfo->ii_ExclusionOps ?
				XLTW_RecheckExclusionConstr : XLTW_InsertIndex;
			index_endscan(index_scan);
			if (DirtySnapshot.speculativeToken)
				SpeculativeInsertionWait(DirtySnapshot.xmin,
										 DirtySnapshot.speculativeToken);
			else
				XactLockTableWait(xwait, heap,reason_wait);
				//原为
				//XactLockTableWait(xwait, heap, &ctid_wait, reason_wait);
			goto retry;
		}

		/*
		 * We have a definite conflict (or a potential one, but the caller
		 * didn't want to wait).  Return it to caller, or report it.
		 */
		if (violationOK)
		{
			conflict = true;
			if (conflictTid)
				*conflictTid = tup->t_self;
			break;
		}

		error_new = BuildIndexValueDescription(index, values, isnull);
		error_existing = BuildIndexValueDescription(index, existing_values,
													existing_isnull);
		if (newIndex)
			ereport(ERROR,
					(errcode(ERRCODE_EXCLUSION_VIOLATION),
					 errmsg("could not create exclusion constraint \"%s\"",
							RelationGetRelationName(index)),
					 error_new && error_existing ?
					 errdetail("Key %s conflicts with key %s.",
							   error_new, error_existing) :
					 errdetail("Key conflicts exist.")
					 ));
					 //原本末尾还有一个
					 //errtableconstraint(heap,
					 //					RelationGetRelationName(index))
		else
			ereport(ERROR,
					(errcode(ERRCODE_EXCLUSION_VIOLATION),
					 errmsg("conflicting key value violates exclusion constraint \"%s\"",
							RelationGetRelationName(index)),
					 error_new && error_existing ?
					 errdetail("Key %s conflicts with existing key %s.",
							   error_new, error_existing) :
					 errdetail("Key conflicts with existing key.")
					 ));
	}

	index_endscan(index_scan);

	/*
	 * Ordinarily, at this point the search should have found the originally
	 * inserted tuple (if any), unless we exited the loop early because of
	 * conflict.  However, it is possible to define exclusion constraints for
	 * which that wouldn't be true --- for instance, if the operator is <>. So
	 * we no longer complain if found_self is still false.
	 */

	econtext->ecxt_scantuple = save_scantuple;

	ExecDropSingleTupleTableSlot(existing_slot);

	return !conflict;
}

// static bool
// shm_mq_counterparty_gone(volatile shm_mq *mq, BackgroundWorkerHandle *handle)
// {
// 	bool		detached;
// 	pid_t		pid;

// 	/* Acquire the lock just long enough to check the pointer. */
// 	SpinLockAcquire(&mq->mq_mutex);
// 	detached = mq->mq_detached;
// 	SpinLockRelease(&mq->mq_mutex);

// 	/* If the queue has been detached, counterparty is definitely gone. */
// 	if (detached)
// 		return true;

// 	/* If there's a handle, check worker status. */
// 	if (handle != NULL)
// 	{
// 		BgwHandleStatus status;

// 		/* Check for unexpected worker death. */
// 		status = GetBackgroundWorkerPid(handle, &pid);
// 		if (status != BGWH_STARTED && status != BGWH_NOT_YET_STARTED)
// 		{
// 			/* Mark it detached, just to make it official. */
// 			SpinLockAcquire(&mq->mq_mutex);
// 			mq->mq_detached = true;
// 			SpinLockRelease(&mq->mq_mutex);
// 			return true;
// 		}
// 	}

// 	/* Counterparty is not definitively gone. */
// 	return false;
// }

// static uint64
// shm_mq_get_bytes_read(volatile shm_mq *mq, bool *detached)
// {
// 	uint64		v;

// 	SpinLockAcquire(&mq->mq_mutex);
// 	v = mq->mq_bytes_read;
// 	*detached = mq->mq_detached;
// 	SpinLockRelease(&mq->mq_mutex);

// 	return v;
// }
// PGPROC *
// shm_mq_get_receiver(shm_mq *mq)
// {
// 	volatile shm_mq *vmq = mq;
// 	PGPROC	   *receiver;

// 	SpinLockAcquire(&mq->mq_mutex);
// 	receiver = vmq->mq_receiver;
// 	SpinLockRelease(&mq->mq_mutex);

// 	return receiver;
// }

// static bool
// shm_mq_wait_internal(volatile shm_mq *mq, PGPROC *volatile * ptr,
// 					 BackgroundWorkerHandle *handle)
// {
// 	bool		result = false;

// 	for (;;)
// 	{
// 		BgwHandleStatus status;
// 		pid_t		pid;
// 		bool		detached;

// 		/* Acquire the lock just long enough to check the pointer. */
// 		SpinLockAcquire(&mq->mq_mutex);
// 		detached = mq->mq_detached;
// 		result = (*ptr != NULL);
// 		SpinLockRelease(&mq->mq_mutex);

// 		/* Fail if detached; else succeed if initialized. */
// 		if (detached)
// 		{
// 			result = false;
// 			break;
// 		}
// 		if (result)
// 			break;

// 		if (handle != NULL)
// 		{
// 			/* Check for unexpected worker death. */
// 			status = GetBackgroundWorkerPid(handle, &pid);
// 			if (status != BGWH_STARTED && status != BGWH_NOT_YET_STARTED)
// 			{
// 				result = false;
// 				break;
// 			}
// 		}

// 		/* Wait to be signalled. */
// 		WaitLatch(MyLatch, WL_LATCH_SET, 0);

// 		/* Reset the latch so we don't spin. */
// 		ResetLatch(MyLatch);

// 		/* An interrupt may have occurred while we were waiting. */
// 		CHECK_FOR_INTERRUPTS();
// 	}

// 	return result;
// }
// static void
// shm_mq_inc_bytes_written(volatile shm_mq *mq, Size n)
// {
// 	SpinLockAcquire(&mq->mq_mutex);
// 	mq->mq_bytes_written += n;
// 	SpinLockRelease(&mq->mq_mutex);
// }

// static shm_mq_result
// shm_mq_send_bytes(shm_mq_handle *mqh, Size nbytes, const void *data,
// 				  bool nowait, Size *bytes_written)
// {
// 	shm_mq	   *mq = mqh->mqh_queue;
// 	Size		sent = 0;
// 	uint64		used;
// 	Size		ringsize = mq->mq_ring_size;
// 	Size		available;

// 	while (sent < nbytes)
// 	{
// 		bool		detached;
// 		uint64		rb;

// 		/* Compute number of ring buffer bytes used and available. */
// 		rb = shm_mq_get_bytes_read(mq, &detached);
// 		Assert(mq->mq_bytes_written >= rb);
// 		used = mq->mq_bytes_written - rb;
// 		Assert(used <= ringsize);
// 		available = Min(ringsize - used, nbytes - sent);

// 		/* Bail out if the queue has been detached. */
// 		if (detached)
// 		{
// 			*bytes_written = sent;
// 			return SHM_MQ_DETACHED;
// 		}

// 		if (available == 0 && !mqh->mqh_counterparty_attached)
// 		{
// 			/*
// 			 * The queue is full, so if the receiver isn't yet known to be
// 			 * attached, we must wait for that to happen.
// 			 */
// 			if (nowait)
// 			{
// 				if (shm_mq_counterparty_gone(mq, mqh->mqh_handle))
// 				{
// 					*bytes_written = sent;
// 					return SHM_MQ_DETACHED;
// 				}
// 				if (shm_mq_get_receiver(mq) == NULL)
// 				{
// 					*bytes_written = sent;
// 					return SHM_MQ_WOULD_BLOCK;
// 				}
// 			}
// 			else if (!shm_mq_wait_internal(mq, &mq->mq_receiver,
// 										   mqh->mqh_handle))
// 			{
// 				mq->mq_detached = true;
// 				*bytes_written = sent;
// 				return SHM_MQ_DETACHED;
// 			}
// 			mqh->mqh_counterparty_attached = true;

// 			/*
// 			 * The receiver may have read some data after attaching, so we
// 			 * must not wait without rechecking the queue state.
// 			 */
// 		}
// 		else if (available == 0)
// 		{
// 			shm_mq_result res;

// 			/* Let the receiver know that we need them to read some data. */
// 			res = shm_mq_notify_receiver(mq);
// 			if (res != SHM_MQ_SUCCESS)
// 			{
// 				*bytes_written = sent;
// 				return res;
// 			}

// 			/* Skip manipulation of our latch if nowait = true. */
// 			if (nowait)
// 			{
// 				*bytes_written = sent;
// 				return SHM_MQ_WOULD_BLOCK;
// 			}

// 			/*
// 			 * Wait for our latch to be set.  It might already be set for some
// 			 * unrelated reason, but that'll just result in one extra trip
// 			 * through the loop.  It's worth it to avoid resetting the latch
// 			 * at top of loop, because setting an already-set latch is much
// 			 * cheaper than setting one that has been reset.
// 			 */
// 			WaitLatch(MyLatch, WL_LATCH_SET, 0);

// 			/* Reset the latch so we don't spin. */
// 			ResetLatch(MyLatch);

// 			/* An interrupt may have occurred while we were waiting. */
// 			CHECK_FOR_INTERRUPTS();
// 		}
// 		else
// 		{
// 			Size		offset = mq->mq_bytes_written % (uint64) ringsize;
// 			Size		sendnow = Min(available, ringsize - offset);

// 			/* Write as much data as we can via a single memcpy(). */
// 			memcpy(&mq->mq_ring[mq->mq_ring_offset + offset],
// 				   (char *) data + sent, sendnow);
// 			sent += sendnow;

// 			/*
// 			 * Update count of bytes written, with alignment padding.  Note
// 			 * that this will never actually insert any padding except at the
// 			 * end of a run of bytes, because the buffer size is a multiple of
// 			 * MAXIMUM_ALIGNOF, and each read is as well.
// 			 */
// 			Assert(sent == nbytes || sendnow == MAXALIGN(sendnow));
// 			shm_mq_inc_bytes_written(mq, MAXALIGN(sendnow));

// 			/*
// 			 * For efficiency, we don't set the reader's latch here.  We'll do
// 			 * that only when the buffer fills up or after writing an entire
// 			 * message.
// 			 */
// 		}
// 	}

// 	*bytes_written = sent;
// 	return SHM_MQ_SUCCESS;
// }
// static shm_mq_result
// shm_mq_notify_receiver(volatile shm_mq *mq)
// {
// 	PGPROC	   *receiver;
// 	bool		detached;

// 	SpinLockAcquire(&mq->mq_mutex);
// 	detached = mq->mq_detached;
// 	receiver = mq->mq_receiver;
// 	SpinLockRelease(&mq->mq_mutex);

// 	if (detached)
// 		return SHM_MQ_DETACHED;
// 	if (receiver)
// 		SetLatch(&receiver->procLatch);
// 	return SHM_MQ_SUCCESS;
// }

static void
error_duplicate_filter_variable(const char *defname)
{
	ereport(ERROR,
			(errcode(ERRCODE_SYNTAX_ERROR),
			 errmsg("filter variable \"%s\" specified more than once",
					defname)));
} 
// static void
// EventTriggerInvoke(List *fn_oid_list, EventTriggerData *trigdata)
// {
// 	MemoryContext context;
// 	MemoryContext oldcontext;
// 	ListCell   *lc;
// 	bool		first = true;

// 	/* Guard against stack overflow due to recursive event trigger */
// 	check_stack_depth();

// 	/*
// 	 * Let's evaluate event triggers in their own memory context, so that any
// 	 * leaks get cleaned up promptly.
// 	 */
// 	context = AllocSetContextCreate(CurrentMemoryContext,
// 									"event trigger context",
// 									ALLOCSET_DEFAULT_SIZES);
// 	oldcontext = MemoryContextSwitchTo(context);

// 	/* Call each event trigger. */
// 	foreach(lc, fn_oid_list)
// 	{
// 		Oid			fnoid = lfirst_oid(lc);
// 		FmgrInfo	flinfo;
// 		FunctionCallInfoData fcinfo;
// 		PgStat_FunctionCallUsage fcusage;

// 		elog(DEBUG1, "EventTriggerInvoke %u", fnoid);

// 		/*
// 		 * We want each event trigger to be able to see the results of the
// 		 * previous event trigger's action.  Caller is responsible for any
// 		 * command-counter increment that is needed between the event trigger
// 		 * and anything else in the transaction.
// 		 */
// 		if (first)
// 			first = false;
// 		else
// 			CommandCounterIncrement();

// 		/* Look up the function */
// 		fmgr_info(fnoid, &flinfo);

// 		/* Call the function, passing no arguments but setting a context. */
// 		InitFunctionCallInfoData(fcinfo, &flinfo, 0,
// 								 InvalidOid, (Node *) trigdata, NULL);
// 		pgstat_init_function_usage(&fcinfo, &fcusage);
// 		FunctionCallInvoke(&fcinfo);
// 		pgstat_end_function_usage(&fcusage, true);

// 		/* Reclaim memory. */
// 		MemoryContextReset(context);
// 	}

// 	/* Restore old memory context and delete the temporary one. */
// 	MemoryContextSwitchTo(oldcontext);
// 	MemoryContextDelete(context);
// } 

// static List *
// EventTriggerCommonSetup(Node *parsetree,
// 						EventTriggerEvent event, const char *eventstr,
// 						EventTriggerData *trigdata)
// {
// 	const char *tag;
// 	List	   *cachelist;
// 	ListCell   *lc;
// 	List	   *runlist = NIL;

// 	/*
// 	 * We want the list of command tags for which this procedure is actually
// 	 * invoked to match up exactly with the list that CREATE EVENT TRIGGER
// 	 * accepts.  This debugging cross-check will throw an error if this
// 	 * function is invoked for a command tag that CREATE EVENT TRIGGER won't
// 	 * accept.  (Unfortunately, there doesn't seem to be any simple, automated
// 	 * way to verify that CREATE EVENT TRIGGER doesn't accept extra stuff that
// 	 * never reaches this control point.)
// 	 *
// 	 * If this cross-check fails for you, you probably need to either adjust
// 	 * standard_ProcessUtility() not to invoke event triggers for the command
// 	 * type in question, or you need to adjust check_ddl_tag to accept the
// 	 * relevant command tag.
// 	 */
// #ifdef USE_ASSERT_CHECKING
// 	{
// 		const char *dbgtag;

// 		dbgtag = CreateCommandTag(parsetree);
// 		if (event == EVT_DDLCommandStart ||
// 			event == EVT_DDLCommandEnd ||
// 			event == EVT_SQLDrop)
// 		{
// 			if (check_ddl_tag(dbgtag) != EVENT_TRIGGER_COMMAND_TAG_OK)
// 				elog(ERROR, "unexpected command tag \"%s\"", dbgtag);
// 		}
// 		else if (event == EVT_TableRewrite)
// 		{
// 			if (check_table_rewrite_ddl_tag(dbgtag) != EVENT_TRIGGER_COMMAND_TAG_OK)
// 				elog(ERROR, "unexpected command tag \"%s\"", dbgtag);
// 		}
// 	}
// #endif

// 	/* Use cache to find triggers for this event; fast exit if none. */
// 	cachelist = EventCacheLookup(event);
// 	if (cachelist == NIL)
// 		return NIL;

// 	/* Get the command tag. */
// 	tag = CreateCommandTag(parsetree);

// 	/*
// 	 * Filter list of event triggers by command tag, and copy them into our
// 	 * memory context.  Once we start running the command trigers, or indeed
// 	 * once we do anything at all that touches the catalogs, an invalidation
// 	 * might leave cachelist pointing at garbage, so we must do this before we
// 	 * can do much else.
// 	 */
// 	foreach(lc, cachelist)
// 	{
// 		EventTriggerCacheItem *item =(EventTriggerCacheItem *) lfirst(lc);

// 		if (filter_event_trigger(&tag, item))
// 		{
// 			/* We must plan to fire this trigger. */
// 			runlist = lappend_oid(runlist, item->fnoid);
// 		}
// 	}

// 	/* don't spend any more time on this if no functions to run */
// 	if (runlist == NIL)
// 		return NIL;

// 	trigdata->type = T_EventTriggerData;
// 	trigdata->event = eventstr;
// 	trigdata->parsetree = parsetree;
// 	trigdata->tag = tag;

// 	return runlist;
// } 



// static void
// StatementTimeoutHandler(void)
// {
// 	int			sig = SIGINT;

// 	/*
// 	 * During authentication the timeout is used to deal with
// 	 * authentication_timeout - we want to quit in response to such timeouts.
// 	 */
// 	if (ClientAuthInProgress)
// 		sig = SIGTERM;

// #ifdef HAVE_SETSID
// 	/* try to signal whole process group */
// 	kill(-MyProcPid, sig);
// #endif
// 	kill(MyProcPid, sig);
// }
// static void
// LockTimeoutHandler(void)
// {
// #ifdef HAVE_SETSID
// 	/* try to signal whole process group */
// 	kill(-MyProcPid, SIGINT);
// #endif
// 	kill(MyProcPid, SIGINT);
// }

// static void
// ShutdownPostgres(int code, Datum arg)
// {
// 	/* Make sure we've killed any active transaction */
// 	AbortOutOfAnyTransaction();

// 	/*
// 	 * User locks are not released by transaction end, so be sure to release
// 	 * them explicitly.
// 	 */
// 	LockReleaseAll(USER_LOCKMETHOD, true);
// }
// 	HeapTuple	tuple;
// 	Relation	relation;
// 	SysScanDesc scan;
// 	ScanKeyData key[1];

// 	/*
// 	 * form a scan key
// 	 */
// 	ScanKeyInit(&key[0],
// 				ObjectIdAttributeNumber,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(dboid));

// 	/*
// 	 * Open pg_database and fetch a tuple.  Force heap scan if we haven't yet
// 	 * built the critical shared relcache entries (i.e., we're starting up
// 	 * without a shared relcache cache file).
// 	 */
// 	relation = heap_open(DatabaseRelationId, AccessShareLock);
// 	scan = systable_beginscan(relation, DatabaseOidIndexId,
// 							  criticalSharedRelcachesBuilt,
// 							  NULL,
// 							  1, key);

// 	tuple = systable_getnext(scan);

// 	/* Must copy tuple before releasing buffer */
// 	if (HeapTupleIsValid(tuple))
// 		tuple = heap_copytuple(tuple);

// 	/* all done */
// 	systable_endscan(scan);
// 	heap_close(relation, AccessShareLock);

// 	return tuple;
// }

static Node *
make_agg_arg(Oid argtype, Oid argcollation)
{
	Param	   *argp = makeNode(Param);

	argp->paramkind = PARAM_EXEC;
	argp->paramid = -1;
	argp->paramtype = argtype;
	argp->paramtypmod = -1;
	argp->paramcollid = argcollation;
	argp->location = -1;
	return (Node *) argp;
} 


//tsdb 下面两个是从og里面搬出来的
static void deform_next_attribute(bool& slow, long& off, Form_pg_attribute thisatt, char* tp)
{
    if (!slow && thisatt->attcacheoff >= 0) {
        off = thisatt->attcacheoff;
    } else if (thisatt->attlen == -1) {
        /*
         * We can only cache the offset for a varlena attribute if the
         * offset is already suitably aligned, so that there would be no
         * pad bytes in any case: then the offset will be valid for either
         * an aligned or unaligned value.
         */
        if (!slow && (uintptr_t)(off) == att_align_nominal(off, thisatt->attalign)) {
            thisatt->attcacheoff = off;
        } else {
            off = att_align_pointer(off, thisatt->attalign, -1, tp + off);
            slow = true;
        }
    } else {
        /* not varlena, so safe to use att_align_nominal */
        off = att_align_nominal(off, thisatt->attalign);

        if (!slow) {
            thisatt->attcacheoff = off;
        }
    }
}

static void slot_deform_tuple(TupleTableSlot *slot, uint32 natts)
{
    HeapTuple tuple = (HeapTuple)slot->tts_tuple;
    Assert(tuple->tupTableType == HEAP_TUPLE);
    TupleDesc tupleDesc = slot->tts_tupleDescriptor;
    Datum *values = slot->tts_values;
    bool *isnull = slot->tts_isnull;
    HeapTupleHeader tup = tuple->t_data;
    bool hasnulls = HeapTupleHasNulls(tuple);
    Form_pg_attribute *att = tupleDesc->attrs;
    uint32 attnum;
    char *tp = NULL;         /* ptr to tuple data */
    long off;                /* offset in tuple data */
    bits8 *bp = tup->t_bits; /* ptr to null bitmap in tuple */
    bool slow = false;       /* can we use/set attcacheoff? */
    bool heapToUHeap = tupleDesc->tdTableAmType == TAM_USTORE;
	
    /*
     * Check whether the first call for this tuple, and initialize or restore
     * loop state.
     */
    attnum = slot->tts_nvalid;
    if (attnum == 0) {
        /* Start from the first attribute */
        off = 0;
        slow = false;
    } else {
        /* Restore state from previous execution */
        off = slot->tts_off;
        slow = slot->tts_slow;
    }

    /*
     * Ustore has different alignment rules so we force slow = true here.
     * See the comments in heap_deform_tuple() for more information.
     */
    slow = heapToUHeap ? true : slow;

    tp = (char *)tup + tup->t_hoff;

    for (; attnum < natts; attnum++) {
        Form_pg_attribute thisatt = att[attnum];

        if (hasnulls && att_isnull(attnum, bp)) {
            values[attnum] = (Datum)0;
            isnull[attnum] = true;
            slow = true; /* can't use attcacheoff anymore */
            continue;
        }

        isnull[attnum] = false;

        deform_next_attribute(slow, off, thisatt, tp);

        values[attnum] = fetchatt(thisatt, tp + off);

        off = att_addlength_pointer(off, thisatt->attlen, tp + off);

        if (thisatt->attlen <= 0) {
            slow = true; /* can't use attcacheoff anymore */
        }
    }

    /*
     * Save state for next execution
     */
    slot->tts_nvalid = attnum;
    slot->tts_off = off;
    slot->tts_slow = slow;
}