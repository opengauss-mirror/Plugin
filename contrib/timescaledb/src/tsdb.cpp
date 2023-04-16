//#include "tsdb.h"
#include "compat.h"

#include "commands/cluster.h"
//#include "gtm/utils/pqsignal.h"
#include "utils.h"
#include "plan_agg_bookend.cpp"
#include "access/skey.h"
#include "commands/extension.h"


#include "knl/knl_session.h"
#include "knl/knl_guc/knl_session_attr_sql.h"
#include "knl/knl_guc/knl_instance_attr_common.h"
#include "knl/knl_thread.h"

#include "utils/syscache.h"


#include "catalog/pg_type_fn.h"
#include "catalog/pg_ts_config.h"
#include "catalog/namespace.h"

#include "utils/dynamic_loader.h"
#include "utils/globalplancore.h"

#include "compat/tableam.h"
#include "utils/guc_tables.h"
#include "utils/array.h"
#include "storage/procarray.h"

#include "commands/sequence.h"
#include "parser/parse_utilcmd.h"
#include "parser/parse_collate.h"
#include "parser/parse_relation.h"
#include "storage/predicate.h"
#include "commands/tablecmds.h"

#include "libpq/pqsignal.h"
#include "catalog/pg_authid.h"
#include "commands/dbcommands.h"
#include "nodes/pg_list.h"

#include "access/reloptions.h"
#include "access/visibilitymap.h"
#include "optimizer/restrictinfo.h"
#include "optimizer/prep.h"
#include "optimizer/predtest.h"
#include "utils/acl.h"

#include "plan_agg_bookend.h"//为了实现函数ts_preprocess_first_last_aggregates

#include "tsdb_dsm.h"
#include "tsdb_static.cpp"
//gausskernel/process/threadpool/knl_session.cpp这个文件很重要,knl类的初始化都在里面

#define BUFFER_MAPPING_LWLOCK_OFFSET	NUM_INDIVIDUAL_LWLOCKS 
#define DynamicSharedMemoryControlLock (&t_thrd.shemem_ptr_cxt.mainLWLockArray[34].lock) 
#define LOCK_MANAGER_LWLOCK_OFFSET		\
	(BUFFER_MAPPING_LWLOCK_OFFSET + NUM_BUFFER_PARTITIONS)
#define NUM_FIXED_LWLOCKS \
	(PREDICATELOCK_MANAGER_LWLOCK_OFFSET + NUM_PREDICATELOCK_PARTITIONS)

#define REINDEX_REL_FORCE_INDEXES_PERMANENT 0x10


CopyState BeginCopyFrom(Relation rel, const char *filename,
			  bool is_program, List *attnamelist, List *options)
{
	CopyState c;
	return c;
}
ObjectAddress CreateTrigger(CreateTrigStmt *stmt, const char *queryString,
			  Oid relOid, Oid refRelOid, Oid constraintOid, Oid indexOid,
			  bool isInternal,int tsdb)
{
	ObjectAddress o;
	return o;
}

ObjectAddress DefineRelation(CreateStmt *stmt, char relkind, Oid ownerId,
			   ObjectAddress *typaddress, int tsdb)
{
	ObjectAddress o;
	return o;
}
void heap_endscan(HeapScanDescData*& scan)
{

}
PGFunction load_external_function(char *filename, char *funcname,
					   bool signalNotFound, void **filehandle,int tsdb)
{
	PGFunction p;
	return p;
}
List *make_pathkeys_for_sortclauses(PlannerInfo *root,
							  List *sortclauses,
							  List *tlist)
{
	List * p;
	return p;
}
RestrictInfo *
make_restrictinfo(Expr *clause,
				  bool is_pushed_down,
				  bool outerjoin_delayed,
				  bool pseudoconstant,
				  Relids required_relids,
				  Relids outer_relids,
				  Relids nullable_relids)
{
	RestrictInfo* r;
	return r;
}
void NewRelationCreateToastTable(Oid relOid, Datum reloptions){}
void RegisterCustomScanMethods(const ExtensiblePlanMethods *methods){}
void ExecARInsertTriggers_tsdb(EState *estate,
					 ResultRelInfo *relinfo,
					 HeapTuple trigtuple,
					 List *recheckIndexes)
{}
Datum slot_getattr(TupleTableSlot *slot, int attnum, bool *isnull)
{Datum d; return d;}

HTSU_Result heap_lock_tuple(Relation relation, HeapTuple tuple,
				CommandId cid, LockTupleMode mode, LockWaitPolicy wait_policy,
				bool follow_update,
				Buffer *buffer, HeapUpdateFailureData *hufd)
{HTSU_Result h; return h;}
Param *SS_make_initplan_output_param(PlannerInfo *root,
							  Oid resulttype, int32 resulttypmod,
							  Oid resultcollation)
{Param *p; return p;}
TypeName *typeStringToTypeName(const char *str)
{TypeName *p; return p;}
List* pull_var_clause(Node *node, int flags)
{
	List* l;
	return l;
}


List* set_deparse_context_planstate(List* dpcontext, Node* planstate, List* ancestors)// OG内部函数 commands/explain.h
{
	List* l;
	return l;
}
void before_shmem_exit(pg_on_exit_callback function, Datum arg)//OG内部storage/ipc.h 
{

}


bool reindex_relation(Oid relid, int flags, int options)
{
	return false;
}
ObjectAddress
DefineIndex_tsdb(Oid relationId,
			IndexStmt *stmt,
			Oid indexRelationId,
			bool is_alter_table,
			bool check_rights,
			bool skip_build,
			bool quiet)
			{
				ObjectAddress o;
				return o;
			}
void cluster_rel(Oid tableOid, Oid indexOid, bool recheck,
			bool verbose)
{

}
void ExplainPropertyBool(const char *qlabel, bool value,
					ExplainState *es)
{}
void standard_ProcessUtility(Node *parsetree, const char *queryString,
						ProcessUtilityContext context, ParamListInfo params,
						DestReceiver *dest, char *completionTag)// OG内部有，但是参数不同，先假实现，考虑是否更改TSDB中的实际参数类型
{}
void InitPostgres(const char *in_dbname, Oid dboid, const char *username,
 			 Oid useroid, char *out_dbname)
{}

HeapScanDesc heap_beginscan_catalog(Relation relation, int nkeys,
					   ScanKey key)
{	HeapScanDesc a;
	return a;
}

void cost_sort(Path *path, PlannerInfo *root,
		  List *pathkeys, Cost input_cost, double tuples, int width,
		  Cost comparison_cost, int sort_mem,
		  double limit_tuples)// OG内部有，但是参数不同，先假实现，考虑是否更改TSDB中的实际参数类型
{}
// void slot_getallattrs(TupleTableSlot *slot)







int
namecpy(Name n1, Name n2)
{
	if (!n1 || !n2)
		return -1;
	StrNCpy(NameStr(*n1), NameStr(*n2), NAMEDATALEN);
	return 0;
}



// CopyState
// BeginCopyFrom(Relation rel, const char *filename, bool is_program, List *attnamelist, List *options)
// {
// 	CopyState cstate;
// 	bool pipe = (filename == NULL);
// 	TupleDesc tupDesc;
// 	Form_pg_attribute *attr;
// 	AttrNumber num_phys_attrs, num_defaults;
// 	FmgrInfo *in_functions;
// 	Oid *typioparams;
// 	int attnum;
// 	Oid in_func_oid;
// 	int *defmap;
// 	ExprState **defexprs;
// 	MemoryContext oldcontext;
// 	bool volatile_defexprs;

// 	cstate = BeginCopy(true, rel, NULL, NULL, InvalidOid, attnamelist, options);
// 	oldcontext = MemoryContextSwitchTo(cstate->copycontext);

// 	/* Initialize state variables */
// 	cstate->fe_eof = false;
// 	cstate->eol_type = EOL_UNKNOWN;
// 	cstate->cur_relname = RelationGetRelationName(cstate->rel);
// 	cstate->cur_lineno = 0;
// 	cstate->cur_attname = NULL;
// 	cstate->cur_attval = NULL;

// 	/* Set up variables to avoid per-attribute overhead. */
// 	initStringInfo(&cstate->attribute_buf);
// 	initStringInfo(&cstate->line_buf);
// 	cstate->line_buf_converted = false;
// 	cstate->raw_buf = (char *) palloc(RAW_BUF_SIZE + 1);
// 	cstate->raw_buf_index = cstate->raw_buf_len = 0;

// 	tupDesc = RelationGetDescr(cstate->rel);
// 	attr = tupDesc->attrs;
// 	num_phys_attrs = tupDesc->natts;
// 	num_defaults = 0;
// 	volatile_defexprs = false;

// 	/*
// 	 * Pick up the required catalog information for each attribute in the
// 	 * relation, including the input function, the element type (to pass to
// 	 * the input function), and info about defaults and constraints. (Which
// 	 * input function we use depends on text/binary format choice.)
// 	 */
// 	in_functions = (FmgrInfo *) palloc(num_phys_attrs * sizeof(FmgrInfo));
// 	typioparams = (Oid *) palloc(num_phys_attrs * sizeof(Oid));
// 	defmap = (int *) palloc(num_phys_attrs * sizeof(int));
// 	defexprs = (ExprState **) palloc(num_phys_attrs * sizeof(ExprState *));

// 	for (attnum = 1; attnum <= num_phys_attrs; attnum++)
// 	{
// 		/* We don't need info for dropped attributes */
// 		if (attr[attnum - 1]->attisdropped)
// 			continue;

// 		/* Fetch the input function and typioparam info */
// 		if (cstate->binary)
// 			getTypeBinaryInputInfo(attr[attnum - 1]->atttypid,
// 								   &in_func_oid,
// 								   &typioparams[attnum - 1]);
// 		else
// 			getTypeInputInfo(attr[attnum - 1]->atttypid, &in_func_oid, &typioparams[attnum - 1]);
// 		fmgr_info(in_func_oid, &in_functions[attnum - 1]);

// 		/* Get default info if needed */
// 		if (!list_member_int(cstate->attnumlist, attnum))
// 		{
// 			/* attribute is NOT to be copied from input */
// 			/* use default value if one exists */
// 			Expr *defexpr = (Expr *) build_column_default(cstate->rel, attnum);

// 			if (defexpr != NULL)
// 			{
// 				/* Run the expression through planner */
// 				defexpr = expression_planner(defexpr);

// 				/* Initialize executable expression in copycontext */
// 				defexprs[num_defaults] = ExecInitExpr(defexpr, NULL);
// 				defmap[num_defaults] = attnum - 1;
// 				num_defaults++;

// 				/*
// 				 * If a default expression looks at the table being loaded,
// 				 * then it could give the wrong answer when using
// 				 * multi-insert. Since database access can be dynamic this is
// 				 * hard to test for exactly, so we use the much wider test of
// 				 * whether the default expression is volatile. We allow for
// 				 * the special case of when the default expression is the
// 				 * nextval() of a sequence which in this specific case is
// 				 * known to be safe for use with the multi-insert
// 				 * optimisation. Hence we use this special case function
// 				 * checker rather than the standard check for
// 				 * contain_volatile_functions().
// 				 */
// 				if (!volatile_defexprs)
// 					volatile_defexprs = contain_volatile_functions_not_nextval((Node *) defexpr);
// 			}
// 		}
// 	}

// 	/* We keep those variables in cstate. */
// 	cstate->in_functions = in_functions;
// 	cstate->typioparams = typioparams;
// 	cstate->defmap = defmap;
// 	cstate->defexprs = defexprs;
// 	cstate->volatile_defexprs = volatile_defexprs;
// 	cstate->num_defaults = num_defaults;
// 	cstate->is_program = is_program;

// 	if (pipe)
// 	{
// 		Assert(!is_program); /* the grammar does not allow this */
// 		if (og_knl2->whereToSendOutput == DestRemote)
// 			ReceiveCopyBegin(cstate);
// 		else
// 			cstate->copy_file = stdin;
// 	}
// 	else
// 	{
// 		cstate->filename = pstrdup(filename);

// 		if (cstate->is_program)
// 		{
// 			cstate->copy_file = OpenPipeStream(cstate->filename, PG_BINARY_R);
// 			if (cstate->copy_file == NULL)
// 				ereport(ERROR,
// 						(errcode_for_file_access(),
// 						 errmsg("could not execute command \"%s\": %m", cstate->filename)));
// 		}
// 		else
// 		{
// 			struct stat st;

// 			cstate->copy_file = AllocateFile(cstate->filename, PG_BINARY_R);
// 			if (cstate->copy_file == NULL)
// 				ereport(ERROR,
// 						(errcode_for_file_access(),
// 						 errmsg("could not open file \"%s\" for reading: %m", cstate->filename)));

// 			if (fstat(fileno(cstate->copy_file), &st))
// 				ereport(ERROR,
// 						(errcode_for_file_access(),
// 						 errmsg("could not stat file \"%s\": %m", cstate->filename)));

// 			if (S_ISDIR(st.st_mode))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 						 errmsg("\"%s\" is a directory", cstate->filename)));
// 		}
// 	}

// 	if (!cstate->binary)
// 	{
// 		/* must rely on user to tell us... */
// 		cstate->file_has_oids = cstate->oids;
// 	}
// 	else
// 	{
// 		/* Read and verify binary header */
// 		char readSig[11];
// 		int32 tmp;

// 		/* Signature */
// 		if (CopyGetData(cstate, readSig, 11, 11) != 11 || memcmp(readSig, BinarySignature, 11) != 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_BAD_COPY_FILE_FORMAT),
// 					 errmsg("COPY file signature not recognized")));
// 		/* Flags field */
// 		if (!CopyGetInt32(cstate, &tmp))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_BAD_COPY_FILE_FORMAT),
// 					 errmsg("invalid COPY file header (missing flags)")));
// 		cstate->file_has_oids = (tmp & (1 << 16)) != 0;
// 		tmp &= ~(1 << 16);
// 		if ((tmp >> 16) != 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_BAD_COPY_FILE_FORMAT),
// 					 errmsg("unrecognized critical flags in COPY file header")));
// 		/* Header extension length */
// 		if (!CopyGetInt32(cstate, &tmp) || tmp < 0)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_BAD_COPY_FILE_FORMAT),
// 					 errmsg("invalid COPY file header (missing length)")));
// 		/* Skip extension header, if present */
// 		while (tmp-- > 0)
// 		{
// 			if (CopyGetData(cstate, readSig, 1, 1) != 1)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_BAD_COPY_FILE_FORMAT),
// 						 errmsg("invalid COPY file header (wrong length)")));
// 		}
// 	}

// 	if (cstate->file_has_oids && cstate->binary)
// 	{
// 		getTypeBinaryInputInfo(OIDOID, &in_func_oid, &cstate->oid_typioparam);
// 		fmgr_info(in_func_oid, &cstate->oid_in_function);
// 	}

// 	/* create workspace for CopyReadAttributes results */
// 	if (!cstate->binary)
// 	{
// 		AttrNumber attr_count = list_length(cstate->attnumlist);
// 		int nfields = cstate->file_has_oids ? (attr_count + 1) : attr_count;

// 		cstate->max_fields = nfields;
// 		cstate->raw_fields = (char **) palloc(nfields * sizeof(char *));
// 	}

// 	MemoryContextSwitchTo(oldcontext);

// 	return cstate;
// }

//这个在og里面有
// void
// ExecuteTruncate(TruncateStmt *stmt)
// {
// 	List *rels = NIL;
// 	List *relids = NIL;
// 	List *seq_relids = NIL;
// 	EState *estate;
// 	ResultRelInfo *resultRelInfos;
// 	ResultRelInfo *resultRelInfo;
// 	SubTransactionId mySubid;
// 	ListCell *cell;

// 	/*
// 	 * Open, exclusive-lock, and check all the explicitly-specified relations
// 	 */
// 	foreach (cell, stmt->relations)
// 	{
// 		RangeVar *rv =(RangeVar *) lfirst(cell);
// 		Relation rel;
// 		bool recurse = interpretInhOption(rv->inhOpt);
// 		Oid myrelid;

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
// 			ListCell *child;
// 			List *children;

// 			children = find_all_inheritors(myrelid, AccessExclusiveLock, NULL);

// 			foreach (child, children)
// 			{
// 				Oid childrelid = lfirst_oid(child);

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
// 			List *newrelids;

// 			newrelids = heap_truncate_find_FKs(relids);
// 			if (newrelids == NIL)
// 				break; /* nothing else to add */

// 			foreach (cell, newrelids)
// 			{
// 				Oid relid = lfirst_oid(cell);
// 				Relation rel;

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
// 		foreach (cell, rels)
// 		{
// 			Relation rel = (Relation) lfirst(cell);
// 			List *seqlist = getOwnedSequences(RelationGetRelid(rel));
// 			ListCell *seqcell;

// 			foreach (seqcell, seqlist)
// 			{
// 				Oid seq_relid = lfirst_oid(seqcell);
// 				Relation seq_rel;

// 				seq_rel = relation_open(seq_relid, AccessExclusiveLock);

// 				/* This check must match AlterSequence! */
// 				if (!pg_class_ownercheck(seq_relid, GetUserId()))
// 					aclcheck_error(ACLCHECK_NOT_OWNER,
// 								   ACL_KIND_CLASS,
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
// 	resultRelInfos = (ResultRelInfo *) palloc(list_length(rels) * sizeof(ResultRelInfo));
// 	resultRelInfo = resultRelInfos;
// 	foreach (cell, rels)
// 	{
// 		Relation rel = (Relation) lfirst(cell);

// 		InitResultRelInfo(resultRelInfo,
// 						  rel,
// 						  0, /* dummy rangetable index */
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
// 	foreach (cell, rels)
// 	{
// 		estate->es_result_relation_info = resultRelInfo;
// 		ExecBSTruncateTriggers(estate, resultRelInfo);
// 		resultRelInfo++;
// 	}

// 	/*
// 	 * OK, truncate each table.
// 	 */
// 	mySubid = GetCurrentSubTransactionId();

// 	foreach (cell, rels)
// 	{
// 		Relation rel = (Relation) lfirst(cell);

// 		/*
// 		 * Normally, we need a transaction-safe truncation here.  However, if
// 		 * the table was either created in the current (sub)transaction or has
// 		 * a new relfilenode in the current (sub)transaction, then we can just
// 		 * truncate it in-place, because a rollback would cause the whole
// 		 * table or the current physical file to be thrown away anyway.
// 		 */
// 		if (rel->rd_createSubid == mySubid || rel->rd_newRelfilenodeSubid == mySubid)
// 		{
// 			/* Immediate, non-rollbackable truncation is OK */
// 			heap_truncate_one_rel(rel);
// 		}
// 		else
// 		{
// 			Oid heap_relid;
// 			Oid toast_relid;
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
// 			RelationSetNewRelfilenode(rel, rel->rd_rel->relpersistence, RecentXmin, minmulti);
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
// 				RelationSetNewRelfilenode(rel, rel->rd_rel->relpersistence, RecentXmin, minmulti);
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
// 	foreach (cell, seq_relids)
// 	{
// 		Oid seq_relid = lfirst_oid(cell);

// 		ResetSequence(seq_relid);
// 	}

// 	/*
// 	 * Process all AFTER STATEMENT TRUNCATE triggers.
// 	 */
// 	resultRelInfo = resultRelInfos;
// 	foreach (cell, rels)
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
// 	foreach (cell, rels)
// 	{
// 		Relation rel = (Relation) lfirst(cell);

// 		heap_close(rel, NoLock);
// 	}
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
// 				 errmsg("unrecognized configuration parameter \"%s\"", name)));
// 	}

// 	if ((record->flags & GUC_SUPERUSER_ONLY) && !superuser())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("must be superuser to examine \"%s\"", name)));

// 	if (varname)
// 		*varname = record->name;

// 	return _ShowOption(record, true);
// }


// ObjectAddress
// CreateTrigger(CreateTrigStmt *stmt, const char *queryString, Oid relOid, Oid refRelOid,
// 			  Oid constraintOid, Oid indexOid, bool isInternal,int tsdb = 0)
// {
// 	int16 tgtype;
// 	int ncolumns;
// 	int16 *columns;
// 	int2vector *tgattr;
// 	Node *whenClause;
// 	List *whenRtable;
// 	char *qual;
// 	Datum values[Natts_pg_trigger];
// 	bool nulls[Natts_pg_trigger];
// 	Relation rel;
// 	AclResult aclresult;
// 	Relation tgrel;
// 	SysScanDesc tgscan;
// 	ScanKeyData key;
// 	Relation pgrel;
// 	HeapTuple tuple;
// 	Oid fargtypes[1]; /* dummy */
// 	Oid funcoid;
// 	Oid funcrettype;
// 	Oid trigoid;
// 	char internaltrigname[NAMEDATALEN];
// 	char *trigname;
// 	Oid constrrelid = InvalidOid;
// 	ObjectAddress myself, referenced;

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
// 		if (stmt->timing != TRIGGER_TYPE_BEFORE && stmt->timing != TRIGGER_TYPE_AFTER)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a table", RelationGetRelationName(rel)),
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
// 					 errmsg("\"%s\" is a view", RelationGetRelationName(rel)),
// 					 errdetail("Views cannot have row-level BEFORE or AFTER triggers.")));
// 		/* Disallow TRUNCATE triggers on VIEWs */
// 		if (TRIGGER_FOR_TRUNCATE(stmt->events))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a view", RelationGetRelationName(rel)),
// 					 errdetail("Views cannot have TRUNCATE triggers.")));
// 	}
// 	else if (rel->rd_rel->relkind == RELKIND_FOREIGN_TABLE)
// 	{
// 		if (stmt->timing != TRIGGER_TYPE_BEFORE && stmt->timing != TRIGGER_TYPE_AFTER)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a foreign table", RelationGetRelationName(rel)),
// 					 errdetail("Foreign tables cannot have INSTEAD OF triggers.")));

// 		if (TRIGGER_FOR_TRUNCATE(stmt->events))
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a foreign table", RelationGetRelationName(rel)),
// 					 errdetail("Foreign tables cannot have TRUNCATE triggers.")));

// 		if (stmt->isconstraint)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 					 errmsg("\"%s\" is a foreign table", RelationGetRelationName(rel)),
// 					 errdetail("Foreign tables cannot have constraint triggers.")));
// 	}
// 	else
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("\"%s\" is not a table or view", RelationGetRelationName(rel))));

// 	if (!og_knl3->allowSystemTableMods && IsSystemRelation(rel))
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
// 			constrrelid = RangeVarGetRelid(stmt->constrrel, AccessShareLock, false);
// 	}

// 	/* permission checks */
// 	if (!isInternal)
// 	{
// 		aclresult = pg_class_aclcheck(RelationGetRelid(rel), GetUserId(), ACL_TRIGGER);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_CLASS, RelationGetRelationName(rel));

// 		if (OidIsValid(constrrelid))
// 		{
// 			aclresult = pg_class_aclcheck(constrrelid, GetUserId(), ACL_TRIGGER);
// 			if (aclresult != ACLCHECK_OK)
// 				aclcheck_error(aclresult, ACL_KIND_CLASS, get_rel_name(constrrelid));
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
// 					 errmsg("INSTEAD OF triggers cannot have WHEN conditions")));
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
// 		List *varList;
// 		ListCell *lc;

// 		/* Set up a pstate to parse with */
// 		pstate = make_parsestate(NULL);
// 		pstate->p_sourcetext = queryString;

// 		/*
// 		 * Set up RTEs for OLD and NEW references.
// 		 *
// 		 * 'OLD' must always have varno equal to 1 and 'NEW' equal to 2.
// 		 */
// 		rte = addRangeTableEntryForRelation(pstate, rel, makeAlias("old", NIL), false, false);
// 		addRTEtoQuery(pstate, rte, false, true, true);
// 		rte = addRangeTableEntryForRelation(pstate, rel, makeAlias("new", NIL), false, false);
// 		addRTEtoQuery(pstate, rte, false, true, true);

// 		/* Transform expression.  Copy to be sure we don't modify original */
// 		whenClause = transformWhereClause(pstate,
// 										  (Node *)copyObject(stmt->whenClause),
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
// 		foreach (lc, varList)
// 		{
// 			Var *var = (Var *) lfirst(lc);

// 			switch (var->varno)
// 			{
// 				case PRS2_OLD_VARNO:
// 					if (!TRIGGER_FOR_ROW(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg("statement trigger's WHEN condition cannot reference "
// 										"column values"),
// 								 parser_errposition(pstate, var->location)));
// 					if (TRIGGER_FOR_INSERT(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg(
// 									 "INSERT trigger's WHEN condition cannot reference OLD values"),
// 								 parser_errposition(pstate, var->location)));
// 					/* system columns are okay here */
// 					break;
// 				case PRS2_NEW_VARNO:
// 					if (!TRIGGER_FOR_ROW(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg("statement trigger's WHEN condition cannot reference "
// 										"column values"),
// 								 parser_errposition(pstate, var->location)));
// 					if (TRIGGER_FOR_DELETE(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
// 								 errmsg(
// 									 "DELETE trigger's WHEN condition cannot reference NEW values"),
// 								 parser_errposition(pstate, var->location)));
// 					if (var->varattno < 0 && TRIGGER_FOR_BEFORE(tgtype))
// 						ereport(ERROR,
// 								(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 								 errmsg("BEFORE trigger's WHEN condition cannot reference NEW "
// 										"system columns"),
// 								 parser_errposition(pstate, var->location)));
// 					break;
// 				default:
// 					/* can't happen without add_missing_from, so just elog */
// 					elog(ERROR,
// 						 "trigger WHEN condition cannot contain references to other relations");
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
// 			aclcheck_error(aclresult, ACL_KIND_PROC, NameListToString(stmt->funcname));
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
// 							NameListToString(stmt->funcname),
// 							"trigger")));
// 	}

// 	/*
// 	 * If the command is a user-entered CREATE CONSTRAINT TRIGGER command that
// 	 * references one of the built-in RI_FKey trigger functions, assume it is
// 	 * from a dump of a pre-7.3 foreign key constraint, and take steps to
// 	 * convert this legacy representation into a regular foreign key
// 	 * constraint.  Ugly, but necessary for loading old dump files.
// 	 */
// 	if (stmt->isconstraint && !isInternal && list_length(stmt->args) >= 6 &&
// 		(list_length(stmt->args) % 2) == 0 && RI_FKey_trigger_type(funcoid) != RI_TRIGGER_NONE)
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
// 											  NULL, /* no conkey */
// 											  0,
// 											  InvalidOid, /* no domain */
// 											  InvalidOid, /* no index */
// 											  InvalidOid, /* no foreign key */
// 											  NULL,
// 											  NULL,
// 											  NULL,
// 											  NULL,
// 											  0,
// 											  ' ',
// 											  ' ',
// 											  ' ',
// 											  NULL, /* no exclusion */
// 											  NULL, /* no check constraint */
// 											  NULL,
// 											  NULL,
// 											  true,		   /* islocal */
// 											  0,		   /* inhcount */
// 											  true,		   /* isnoinherit */
// 											  isInternal); /* is_internal */
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
// 		snprintf(internaltrigname, sizeof(internaltrigname), "%s_%u", stmt->trigname, trigoid);
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
// 					BTEqualStrategyNumber,
// 					F_OIDEQ,
// 					ObjectIdGetDatum(RelationGetRelid(rel)));
// 		tgscan = systable_beginscan(tgrel, TriggerRelidNameIndexId, true, NULL, 1, &key);
// 		while (HeapTupleIsValid(tuple = systable_getnext(tgscan)))
// 		{
// 			Form_pg_trigger pg_trigger = (Form_pg_trigger) GETSTRUCT(tuple);

// 			if (namestrcmp(&(pg_trigger->tgname), trigname) == 0)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_DUPLICATE_OBJECT),
// 						 errmsg("trigger \"%s\" for relation \"%s\" already exists",
// 								trigname,
// 								RelationGetRelationName(rel))));
// 		}
// 		systable_endscan(tgscan);
// 	}

// 	/*
// 	 * Build the new pg_trigger tuple.
// 	 */
// 	memset(nulls, false, sizeof(nulls));

// 	values[Anum_pg_trigger_tgrelid - 1] = ObjectIdGetDatum(RelationGetRelid(rel));
// 	values[Anum_pg_trigger_tgname - 1] = DirectFunctionCall1(namein, CStringGetDatum(trigname));
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
// 		ListCell *le;
// 		char *args;
// 		int16 nargs = list_length(stmt->args);
// 		int len = 0;

// 		foreach (le, stmt->args)
// 		{
// 			char *ar = strVal(lfirst(le));

// 			len += strlen(ar) + 4;
// 			for (; *ar; ar++)
// 			{
// 				if (*ar == '\\')
// 					len++;
// 			}
// 		}
// 		args = (char *) palloc(len + 1);
// 		args[0] = '\0';
// 		foreach (le, stmt->args)
// 		{
// 			char *s = strVal(lfirst(le));
// 			char *d = args + strlen(args);

// 			while (*s)
// 			{
// 				if (*s == '\\')
// 					*d++ = '\\';
// 				*d++ = *s++;
// 			}
// 			strcpy(d, "\\000");
// 		}
// 		values[Anum_pg_trigger_tgnargs - 1] = Int16GetDatum(nargs);
// 		values[Anum_pg_trigger_tgargs - 1] = DirectFunctionCall1(byteain, CStringGetDatum(args));
// 	}
// 	else
// 	{
// 		values[Anum_pg_trigger_tgnargs - 1] = Int16GetDatum(0);
// 		values[Anum_pg_trigger_tgargs - 1] = DirectFunctionCall1(byteain, CStringGetDatum(""));
// 	}

// 	/* build column number array if it's a column-specific trigger */
// 	ncolumns = list_length(stmt->columns);
// 	if (ncolumns == 0)
// 		columns = NULL;
// 	else
// 	{
// 		ListCell *cell;
// 		int i = 0;

// 		columns = (int16 *) palloc(ncolumns * sizeof(int16));
// 		foreach (cell, stmt->columns)
// 		{
// 			char *name = strVal(lfirst(cell));
// 			int16 attnum;
// 			int j;

// 			/* Lookup column name.  System columns are not allowed */
// 			attnum = attnameAttNum(rel, name, false);
// 			if (attnum == InvalidAttrNumber)
// 				ereport(ERROR,
// 						(errcode(ERRCODE_UNDEFINED_COLUMN),
// 						 errmsg("column \"%s\" of relation \"%s\" does not exist",
// 								name,
// 								RelationGetRelationName(rel))));

// 			/* Check for duplicates */
// 			for (j = i - 1; j >= 0; j--)
// 			{
// 				if (columns[j] == attnum)
// 					ereport(ERROR,
// 							(errcode(ERRCODE_DUPLICATE_COLUMN),
// 							 errmsg("column \"%s\" specified more than once", name)));
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
// 	tuple = SearchSysCacheCopy1(RELOID, ObjectIdGetDatum(RelationGetRelid(rel)));
// 	if (!HeapTupleIsValid(tuple))
// 		elog(ERROR, "cache lookup failed for relation %u", RelationGetRelid(rel));

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
// 		int i;

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
// 		recordDependencyOnExpr(&myself, whenClause, whenRtable, DEPENDENCY_NORMAL);

// 	/* Post creation hook for new trigger */
// 	InvokeObjectPostCreateHookArg(TriggerRelationId, trigoid, 0, isInternal);

// 	/* Keep lock on target rel until end of xact */
// 	heap_close(rel, NoLock);

// 	return myself;
// }

//在og的nodeFuncs.h
// bool
// query_tree_walker(Query *query, bool (*walker)(Node *,void*), void *context, int flags)
// {
// 	Assert(query != NULL && IsA(query, Query));

// 	if (walker((Node *) query->targetList, context))
// 		return true;
// 	if (walker((Node *) query->withCheckOptions, context))
// 		return true;
// 	if (walker((Node *) query->onConflict, context))
// 		return true;
// 	if (walker((Node *) query->returningList, context))
// 		return true;
// 	if (walker((Node *) query->jointree, context))
// 		return true;
// 	if (walker(query->setOperations, context))
// 		return true;
// 	if (walker(query->havingQual, context))
// 		return true;
// 	if (walker(query->limitOffset, context))
// 		return true;
// 	if (walker(query->limitCount, context))
// 		return true;
// 	if (!(flags & QTW_IGNORE_CTE_SUBQUERIES))
// 	{
// 		if (walker((Node *) query->cteList, context))
// 			return true;
// 	}
// 	if (!(flags & QTW_IGNORE_RANGE_TABLE))
// 	{
// 		if (range_table_walker(query->rtable,(bool (*)()) walker, context, flags))
// 			return true;
// 	}
// 	return false;
// }

//在og的nodes/nodeFuncs.h
// bool
// expression_tree_walker(Node *node,bool (*walker)(void*,void*), void *context)
// {
// 	ListCell *temp;

// 	/*
// 	 * The walker has already visited the current node, and so we need only
// 	 * recurse into any sub-nodes it has.
// 	 *
// 	 * We assume that the walker is not interested in List nodes per se, so
// 	 * when we expect a List we just recurse directly to self without
// 	 * bothering to call the walker.
// 	 */
// 	if (node == NULL)
// 		return false;

// 	/* Guard against stack overflow due to overly complex expressions */
// 	check_stack_depth();

// 	switch (nodeTag(node))
// 	{
// 		case T_Var:
// 		case T_Const:
// 		case T_Param:
// 		case T_CoerceToDomainValue:
// 		case T_CaseTestExpr:
// 		case T_SetToDefault:
// 		case T_CurrentOfExpr:
// 		case T_RangeTblRef:
// 		case T_SortGroupClause:
// 			/* primitive node types with no expression subnodes */
// 			break;
// 		case T_WithCheckOption:
// 			return walker(((WithCheckOption *) node)->qual, context);
// 		case T_Aggref: {
// 			Aggref *expr = (Aggref *) node;

// 			/* recurse directly on List */
// 			if (expression_tree_walker((Node *) expr->aggdirectargs, walker, context))
// 				return true;
// 			if (expression_tree_walker((Node *) expr->args, walker, context))
// 				return true;
// 			if (expression_tree_walker((Node *) expr->aggorder, walker, context))
// 				return true;
// 			if (expression_tree_walker((Node *) expr->aggdistinct, walker, context))
// 				return true;
// 			if (walker((Node *) expr->aggfilter, context))
// 				return true;
// 		}
// 		break;
// 		case T_GroupingFunc: {
// 			GroupingFunc *grouping = (GroupingFunc *) node;

// 			if (expression_tree_walker((Node *) grouping->args, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_WindowFunc: {
// 			WindowFunc *expr = (WindowFunc *) node;

// 			/* recurse directly on List */
// 			if (expression_tree_walker((Node *) expr->args, walker, context))
// 				return true;
// 			if (walker((Node *) expr->aggfilter, context))
// 				return true;
// 		}
// 		break;
// 		case T_ArrayRef: {
// 			ArrayRef *aref = (ArrayRef *) node;

// 			/* recurse directly for upper/lower array index lists */
// 			if (expression_tree_walker((Node *) aref->refupperindexpr, walker, context))
// 				return true;
// 			if (expression_tree_walker((Node *) aref->reflowerindexpr, walker, context))
// 				return true;
// 			/* walker must see the refexpr and refassgnexpr, however */
// 			if (walker(aref->refexpr, context))
// 				return true;
// 			if (walker(aref->refassgnexpr, context))
// 				return true;
// 		}
// 		break;
// 		case T_FuncExpr: {
// 			FuncExpr *expr = (FuncExpr *) node;

// 			if (expression_tree_walker((Node *) expr->args, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_NamedArgExpr:
// 			return walker(((NamedArgExpr *) node)->arg, context);
// 		case T_OpExpr:
// 		case T_DistinctExpr: /* struct-equivalent to OpExpr */
// 		case T_NullIfExpr:	 /* struct-equivalent to OpExpr */
// 		{
// 			OpExpr *expr = (OpExpr *) node;

// 			if (expression_tree_walker((Node *) expr->args, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_ScalarArrayOpExpr: {
// 			ScalarArrayOpExpr *expr = (ScalarArrayOpExpr *) node;

// 			if (expression_tree_walker((Node *) expr->args, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_BoolExpr: {
// 			BoolExpr *expr = (BoolExpr *) node;

// 			if (expression_tree_walker((Node *) expr->args, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_SubLink: {
// 			SubLink *sublink = (SubLink *) node;

// 			if (walker(sublink->testexpr, context))
// 				return true;

// 			/*
// 			 * Also invoke the walker on the sublink's Query node, so it
// 			 * can recurse into the sub-query if it wants to.
// 			 */
// 			return walker(sublink->subselect, context);
// 		}
// 		break;
// 		case T_SubPlan: {
// 			SubPlan *subplan = (SubPlan *) node;

// 			/* recurse into the testexpr, but not into the Plan */
// 			if (walker(subplan->testexpr, context))
// 				return true;
// 			/* also examine args list */
// 			if (expression_tree_walker((Node *) subplan->args, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_AlternativeSubPlan:
// 			return walker(((AlternativeSubPlan *) node)->subplans, context);
// 		case T_FieldSelect:
// 			return walker(((FieldSelect *) node)->arg, context);
// 		case T_FieldStore: {
// 			FieldStore *fstore = (FieldStore *) node;

// 			if (walker(fstore->arg, context))
// 				return true;
// 			if (walker(fstore->newvals, context))
// 				return true;
// 		}
// 		break;
// 		case T_RelabelType:
// 			return walker(((RelabelType *) node)->arg, context);
// 		case T_CoerceViaIO:
// 			return walker(((CoerceViaIO *) node)->arg, context);
// 		case T_ArrayCoerceExpr:
// 			return walker(((ArrayCoerceExpr *) node)->arg, context);
// 		case T_ConvertRowtypeExpr:
// 			return walker(((ConvertRowtypeExpr *) node)->arg, context);
// 		case T_CollateExpr:
// 			return walker(((CollateExpr *) node)->arg, context);
// 		case T_CaseExpr: {
// 			CaseExpr *caseexpr = (CaseExpr *) node;

// 			if (walker(caseexpr->arg, context))
// 				return true;
// 			/* we assume walker doesn't care about CaseWhens, either */
// 			foreach (temp, caseexpr->args)
// 			{
// 				CaseWhen *when = (CaseWhen *) lfirst(temp);

// 				Assert(IsA(when, CaseWhen));
// 				if (walker(when->expr, context))
// 					return true;
// 				if (walker(when->result, context))
// 					return true;
// 			}
// 			if (walker(caseexpr->defresult, context))
// 				return true;
// 		}
// 		break;
// 		case T_ArrayExpr:
// 			return walker(((ArrayExpr *) node)->elements, context);
// 		case T_RowExpr:
// 			/* Assume colnames isn't interesting */
// 			return walker(((RowExpr *) node)->args, context);
// 		case T_RowCompareExpr: {
// 			RowCompareExpr *rcexpr = (RowCompareExpr *) node;

// 			if (walker(rcexpr->largs, context))
// 				return true;
// 			if (walker(rcexpr->rargs, context))
// 				return true;
// 		}
// 		break;
// 		case T_CoalesceExpr:
// 			return walker(((CoalesceExpr *) node)->args, context);
// 		case T_MinMaxExpr:
// 			return walker(((MinMaxExpr *) node)->args, context);
// 		case T_XmlExpr: {
// 			XmlExpr *xexpr = (XmlExpr *) node;

// 			if (walker(xexpr->named_args, context))
// 				return true;
// 			/* we assume walker doesn't care about arg_names */
// 			if (walker(xexpr->args, context))
// 				return true;
// 		}
// 		break;
// 		case T_NullTest:
// 			return walker(((NullTest *) node)->arg, context);
// 		case T_BooleanTest:
// 			return walker(((BooleanTest *) node)->arg, context);
// 		case T_CoerceToDomain:
// 			return walker(((CoerceToDomain *) node)->arg, context);
// 		case T_TargetEntry:
// 			return walker(((TargetEntry *) node)->expr, context);
// 		case T_Query:
// 			/* Do nothing with a sub-Query, per discussion above */
// 			break;
// 		case T_WindowClause: {
// 			WindowClause *wc = (WindowClause *) node;

// 			if (walker(wc->partitionClause, context))
// 				return true;
// 			if (walker(wc->orderClause, context))
// 				return true;
// 			if (walker(wc->startOffset, context))
// 				return true;
// 			if (walker(wc->endOffset, context))
// 				return true;
// 		}
// 		break;
// 		case T_CommonTableExpr: {
// 			CommonTableExpr *cte = (CommonTableExpr *) node;

// 			/*
// 			 * Invoke the walker on the CTE's Query node, so it can
// 			 * recurse into the sub-query if it wants to.
// 			 */
// 			return walker(cte->ctequery, context);
// 		}
// 		break;
// 		case T_List:
// 			foreach (temp, (List *) node)
// 			{
// 				if (walker((Node *) lfirst(temp), context))
// 					return true;
// 			}
// 			break;
// 		case T_FromExpr: {
// 			FromExpr *from = (FromExpr *) node;

// 			if (walker(from->fromlist, context))
// 				return true;
// 			if (walker(from->quals, context))
// 				return true;
// 		}
// 		break;
// 		case T_OnConflictExpr: {
// 			OnConflictExpr *onconflict = (OnConflictExpr *) node;

// 			if (walker((Node *) onconflict->arbiterElems, context))
// 				return true;
// 			if (walker(onconflict->arbiterWhere, context))
// 				return true;
// 			if (walker(onconflict->onConflictSet, context))
// 				return true;
// 			if (walker(onconflict->onConflictWhere, context))
// 				return true;
// 			if (walker(onconflict->exclRelTlist, context))
// 				return true;
// 		}
// 		break;
// 		case T_JoinExpr: {
// 			JoinExpr *join = (JoinExpr *) node;

// 			if (walker(join->larg, context))
// 				return true;
// 			if (walker(join->rarg, context))
// 				return true;
// 			if (walker(join->quals, context))
// 				return true;

// 			/*
// 			 * alias clause, using list are deemed uninteresting.
// 			 */
// 		}
// 		break;
// 		case T_SetOperationStmt: {
// 			SetOperationStmt *setop = (SetOperationStmt *) node;

// 			if (walker(setop->larg, context))
// 				return true;
// 			if (walker(setop->rarg, context))
// 				return true;

// 			/* groupClauses are deemed uninteresting */
// 		}
// 		break;
// 		case T_PlaceHolderVar:
// 			return walker(((PlaceHolderVar *) node)->phexpr, context);
// 		case T_InferenceElem:
// 			return walker(((InferenceElem *) node)->expr, context);
// 		case T_AppendRelInfo: {
// 			AppendRelInfo *appinfo = (AppendRelInfo *) node;

// 			if (expression_tree_walker((Node *) appinfo->translated_vars, walker, context))
// 				return true;
// 		}
// 		break;
// 		case T_PlaceHolderInfo:
// 			return walker(((PlaceHolderInfo *) node)->ph_var, context);
// 		case T_RangeTblFunction:
// 			return walker(((RangeTblFunction *) node)->funcexpr, context);
// 		case T_TableSampleClause: {
// 			TableSampleClause *tsc = (TableSampleClause *) node;

// 			if (expression_tree_walker((Node *) tsc->args, walker, context))
// 				return true;
// 			if (walker((Node *) tsc->repeatable, context))
// 				return true;
// 		}
// 		break;
// 		default:
// 			elog(ERROR, "unrecognized node type: %d", (int) nodeTag(node));
// 			break;
// 	}
// 	return false;
// }

//gausskernel/optimizer/commands/cluster.cpp
// void
// cluster_rel(Oid tableOid, Oid indexOid, bool recheck, bool verbose)
// {
// 	Relation OldHeap;

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
// 		HeapTuple tuple;
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
// 			if (!HeapTupleIsValid(tuple)) /* probably can't happen */
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
// 					 errmsg("cannot cluster temporary tables of other sessions")));
// 		else
// 			ereport(ERROR,
// 					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 					 errmsg("cannot vacuum temporary tables of other sessions")));
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
// 	if (OldHeap->rd_rel->relkind == RELKIND_MATVIEW && !RelationIsPopulated(OldHeap))
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

//重载,参数不同
Oid 
ReindexTable(RangeVar *relation, int options)
{
	Oid heapOid;

	/* The lock level used here should match reindex_relation(). */
	heapOid = RangeVarGetRelidExtended_tsdb(relation,
									   ShareLock,
									   false,
									   false,
									   (RangeVarGetRelidCallback_tsdb)RangeVarCallbackOwnsTable,
									   NULL);

	if (!reindex_relation(heapOid,
						  REINDEX_REL_PROCESS_TOAST | REINDEX_REL_CHECK_CONSTRAINTS,
						  options))
		ereport(NOTICE, (errmsg("table \"%s\" has no indexes", relation->relname)));

	return heapOid;
}

//放入gausskernel\optimizer\plan\subselect.cpp
// Param *
// SS_make_initplan_output_param(PlannerInfo *root, Oid resulttype, int32 resulttypmod,
// 							  Oid resultcollation)
// {
// 	return generate_new_param(root, resulttype, resulttypmod, resultcollation);
// }

void
ExecVacuum(VacuumStmt *vacstmt, bool isTopLevel)
{
	VacuumParams params;

	/* sanity checks on options */
	Assert(vacstmt->options & (VACOPT_VACUUM | VACOPT_ANALYZE));
	Assert((vacstmt->options & VACOPT_VACUUM) ||
		   !(vacstmt->options & (VACOPT_FULL | VACOPT_FREEZE)));
	Assert((vacstmt->options & VACOPT_ANALYZE) || vacstmt->va_cols == NIL);
	Assert(!(vacstmt->options & 64));//tsdb VACOPT_SKIPTOAST是64

	/*
	 * All freeze ages are zero if the FREEZE option is given; otherwise pass
	 * them as -1 which means to use the default values.
	 */
	if (vacstmt->options & VACOPT_FREEZE)
	{
		params.freeze_min_age = 0;
		params.freeze_table_age = 0;
		params.multixact_freeze_min_age = 0;
		params.multixact_freeze_table_age = 0;
	}
	else
	{
		params.freeze_min_age = -1;
		params.freeze_table_age = -1;
		params.multixact_freeze_min_age = -1;
		params.multixact_freeze_table_age = -1;
	}

	/* user-invoked vacuum is never "for wraparound" */
	params.is_wraparound = false;

	/* user-invoked vacuum never uses this parameter */
	params.log_min_duration = -1;

	/* Now go through the common routine */
	vacuum(vacstmt,
		   vacstmt->rely_oid,
		   InvalidOid,
		   NULL,
		   isTopLevel);

    //tsdb 原为这个
    // vacuum(vacstmt->options,
	// 	   vacstmt->relation,
	// 	   InvalidOid,
	// 	   &params,
	// 	   vacstmt->va_cols,
	// 	   NULL,
	// 	   isTopLevel);
}






/*
 * Block/unblock signals in a background worker
 */
void
BackgroundWorkerBlockSignals(void)
{
	PG_SETMASK(&t_thrd.libpq_cxt.BlockSig);
}

void
BackgroundWorkerUnblockSignals(void)
{
	PG_SETMASK(&t_thrd.libpq_cxt.UnBlockSig);
}

/*
 * Connect background worker to a database.
 */
void
BackgroundWorkerInitializeConnection(char *dbname, char *username)
{
	BackgroundWorker *worker = MyBgworkerEntry;

	/* XXX is this the right errcode? */
	if (!(worker->bgw_flags & BGWORKER_BACKEND_DATABASE_CONNECTION))
		ereport(FATAL,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("database connection requirement not indicated during registration")));

	InitPostgres(dbname, InvalidOid, username, InvalidOid, NULL);

	/* it had better not gotten out of "init" mode yet */
	if (!IsInitProcessingMode())
		ereport(ERROR, (errmsg("invalid processing mode in background worker")));
	SetProcessingMode(NormalProcessing);
}

BgwHandleStatus
WaitForBackgroundWorkerStartup(BackgroundWorkerHandle *handle, pid_t *pidp)
{
	  BgwHandleStatus status;
	  int     rc;

	  for (;;)
 
	{
		    pid_t    pid;

		    CHECK_FOR_INTERRUPTS();

		    status = GetBackgroundWorkerPid(handle, &pid);
		    if (status == BGWH_STARTED)
      *pidp = pid;
		    if (status != BGWH_NOT_YET_STARTED)
      break;

		    rc = WaitLatch(&t_thrd.proc->procLatch,
           WL_LATCH_SET | WL_POSTMASTER_DEATH, 0);

		    if (rc & WL_POSTMASTER_DEATH)
   
		{
			      status = BGWH_POSTMASTER_DIED;
			      break;
			   
		}

		    ResetLatch(&t_thrd.proc->procLatch);
		 
	}

	  return status;
}

bool
RegisterDynamicBackgroundWorker(BackgroundWorker *worker,
                BackgroundWorkerHandle **handle)
{
	  int     slotno;
	  bool    success = false;
	  uint64   generation = 0;

	  /*
	  * We can't register dynamic background workers from the postmaster. If
	  * this is a standalone backend, we're the only process and can't start
	  * any more. In a multi-process environment, it might be theoretically
	  * possible, but we don't currently support it due to locking
	  * considerations; see comments on the BackgroundWorkerSlot data
	  * structure.
	  */
  if (!IsUnderPostmaster)
    return false;

	  if (!SanityCheckBackgroundWorker(worker, ERROR))
    return false;

	  LWLockAcquire(BackgroundWorkerLock, LW_EXCLUSIVE);

	  /*
	  * Look for an unused slot. If we find one, grab it.
	  */
  for (slotno = 0; slotno < BackgroundWorkerData->total_slots; ++slotno)
 
	{
		    BackgroundWorkerSlot *slot = &BackgroundWorkerData->slot[slotno];

		    if (!slot->in_use)
   
		{
			      memcpy(&slot->worker, worker, sizeof(BackgroundWorker));
			      slot->pid = InvalidPid;
			  /* indicates not started yet */
      slot->generation++;
			      slot->terminate = false;
			      generation = slot->generation;

			      /*
			      * Make sure postmaster doesn't see the slot as in use before it
			      * sees the new contents.
			      */
      pg_write_barrier();

			      slot->in_use = true;
			      success = true;
			      break;
			   
		}
		 
	}

	  LWLockRelease(BackgroundWorkerLock);

	  /* If we found a slot, tell the postmaster to notice the change. */
  if (success)
    SendPostmasterSignal((PMSignalReason)6);//PMSIGNAL_BACKGROUND_WORKER_CHANGE是6

	  /*
	  * If we found a slot and the user has provided a handle, initialize it.
	  */
  if (success && handle)
 
	{
		    *handle =(BackgroundWorkerHandle*) palloc(sizeof(BackgroundWorkerHandle));
		    (*handle)->slot = slotno;
		    (*handle)->generation = generation;
		 
	}

	  return success;
}





void
BackgroundWorkerInitializeConnectionByOid(Oid dboid, Oid useroid)
{
	  BackgroundWorker *worker = MyBgworkerEntry;

	  /* XXX is this the right errcode? */
  if (!(worker->bgw_flags & BGWORKER_BACKEND_DATABASE_CONNECTION))
    ereport(FATAL,
        (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
        errmsg("database connection requirement not indicated during registration")));

	  InitPostgres(NULL, dboid, NULL, useroid, NULL);

	  /* it had better not gotten out of "init" mode yet */
  if (!IsInitProcessingMode())
    ereport(ERROR,
        (errmsg("invalid processing mode in background worker")));
	  SetProcessingMode(NormalProcessing);
}

//gausskernel/optimizer/util/restrictinfo.cpp
// RestrictInfo *
// make_restrictinfo(Expr *clause,
//          bool is_pushed_down,
//          bool outerjoin_delayed,
//          bool pseudoconstant,
//          Relids required_relids,
//          Relids outer_relids,
//          Relids nullable_relids)
// {
// 	  /*
// 	  * If it's an OR clause, build a modified copy with RestrictInfos inserted
// 	  * above each subclause of the top-level AND/OR structure.
// 	  */
//   if (or_clause((Node *) clause))
//     return (RestrictInfo *) make_sub_restrictinfos(clause,
//                            is_pushed_down,
//                            outerjoin_delayed,
//                            pseudoconstant,
//                            required_relids,
//                            outer_relids,
//                            nullable_relids);

// 	  /* Shouldn't be an AND clause, else AND/OR flattening messed up */
//   Assert(!and_clause((Node *) clause));

// 	  return make_restrictinfo_internal(clause,
//                    NULL,
//                    is_pushed_down,
//                    outerjoin_delayed,
//                    pseudoconstant,
//                    required_relids,
//                    outer_relids,
//                    nullable_relids);
// }

AggPath *
create_agg_path(PlannerInfo *root, RelOptInfo *rel, Path *subpath, PathTarget *target,
				AggStrategy aggstrategy, AggSplit aggsplit, List *groupClause, List *qual,
				const AggClauseCosts *aggcosts, double numGroups)
{
	AggPath *pathnode = makeNode(AggPath);

	pathnode->path.pathtype = T_Agg;
	pathnode->path.parent = rel;
	pathnode->path.pathtarget = target;
	/* For now, assume we are above any joins, so no parameterization */
	pathnode->path.param_info = NULL;
	pathnode->path.parallel_aware = false;
	pathnode->path.parallel_safe = rel->consider_parallel && subpath->parallel_safe;
	pathnode->path.parallel_workers = subpath->parallel_workers;
	if (aggstrategy == AGG_SORTED)
		pathnode->path.pathkeys = subpath->pathkeys; /* preserves order */
	else
		pathnode->path.pathkeys = NIL; /* output is unordered */
	pathnode->subpath = subpath;

	pathnode->aggstrategy = aggstrategy;
	pathnode->aggsplit = aggsplit;
	pathnode->numGroups = numGroups;
	pathnode->groupClause = groupClause;
	pathnode->qual = qual;

	cost_agg(&pathnode->path,
			 root,
			 aggstrategy,
			 aggcosts,
			 list_length(groupClause),
			 numGroups,
			 subpath->startup_cost,
			 subpath->total_cost,
			 subpath->rows);

	/* add tlist eval cost for each output row */
	pathnode->path.startup_cost += target->cost.startup;
	pathnode->path.total_cost +=
		target->cost.startup + target->cost.per_tuple * pathnode->path.rows;

	return pathnode;
}

void
add_partial_path(RelOptInfo *parent_rel, Path *new_path)
{
	  bool    accept_new = true;
	  /* unless we find a superior old path */
  ListCell  *insert_after = NULL;
	 /* where to insert new item */
  ListCell  *p1;
	  ListCell  *p1_prev;
	  ListCell  *p1_next;

	  /* Check for query cancel. */
  CHECK_FOR_INTERRUPTS();

	  /*
	  * As in add_path, throw out any paths which are dominated by the new
	  * path, but throw out the new path if some existing path dominates it.
	  */
  p1_prev = NULL;
	  for (p1 = list_head(parent_rel->partial_pathlist); p1 != NULL;
    p1 = p1_next)
 
	{
		    Path    *old_path = (Path *) lfirst(p1);
		    bool    remove_old = false; /* unless new proves superior */
		    PathKeysComparison keyscmp;

		    p1_next = lnext(p1);

		    /* Compare pathkeys. */
    keyscmp = compare_pathkeys(new_path->pathkeys, old_path->pathkeys);

		    /* Unless pathkeys are incompable, keep just one of the two paths. */
    if (keyscmp != PATHKEYS_DIFFERENT)
   
		{
			      if (new_path->total_cost > old_path->total_cost * STD_FUZZ_FACTOR)
     
			{
				        /* New path costs more; keep it only if pathkeys are better. */
        if (keyscmp != PATHKEYS_BETTER1)
          accept_new = false;
				     
			}
			      else if (old_path->total_cost > new_path->total_cost
          * STD_FUZZ_FACTOR)
     
			{
				        /* Old path costs more; keep it only if pathkeys are better. */
        if (keyscmp != PATHKEYS_BETTER2)
          remove_old = true;
				     
			}
			      else if (keyscmp == PATHKEYS_BETTER1)
     
			{
				        /* Costs are about the same, new path has better pathkeys. */
        remove_old = true;
				     
			}
			      else if (keyscmp == PATHKEYS_BETTER2)
     
			{
				        /* Costs are about the same, old path has better pathkeys. */
        accept_new = false;
				     
			}
			      else if (old_path->total_cost > new_path->total_cost * 1.0000000001)
     
			{
				        /* Pathkeys are the same, and the old path costs more. */
        remove_old = true;
				     
			}
			      else      
			{
				        /*
				        * Pathkeys are the same, and new path isn't materially
				        * cheaper.
				        */
        accept_new = false;
				     
			}
			   
		}

		    /*
		    * Remove current element from partial_pathlist if dominated by new.
		    */
    if (remove_old)
   
		{
			      parent_rel->partial_pathlist =
        list_delete_cell(parent_rel->partial_pathlist, p1, p1_prev);
			      /* we should not see IndexPaths here, so always safe to delete */
      Assert(!IsA(old_path, IndexPath));
			      pfree(old_path);
			      /* p1_prev does not advance */
   
		}
		    else    
		{
			      /* new belongs after this old path if it has cost >= old's */
      if (new_path->total_cost >= old_path->total_cost)
        insert_after = p1;
			      /* p1_prev advances */
      p1_prev = p1;
			   
		}

		    /*
		    * If we found an old path that dominates new_path, we can quit
		    * scanning the partial_pathlist; we will not add new_path, and we
		    * assume new_path cannot dominate any later path.
		    */
    if (!accept_new)
      break;
		 
	}

	  if (accept_new)
 
	{
		    /* Accept the new path: insert it at proper place */
    if (insert_after)
      lappend_cell(parent_rel->partial_pathlist, insert_after, new_path);
		    else       parent_rel->partial_pathlist =
        lcons(new_path, parent_rel->partial_pathlist);
		 
	}
	  else  
	{
		    /* we should not see IndexPaths here, so always safe to delete */
    Assert(!IsA(new_path, IndexPath));
		    /* Reject and recycle the new path */
    pfree(new_path);
		 
	}
}

GatherPath *
create_gather_path(PlannerInfo *root, RelOptInfo *rel, Path *subpath,
         PathTarget *target, Relids required_outer, double *rows)
{
	  GatherPath *pathnode = makeNode(GatherPath);

	  Assert(subpath->parallel_safe);

	  pathnode->path.pathtype = T_Gather;
	  pathnode->path.parent = rel;
	  pathnode->path.pathtarget = target;
	  pathnode->path.param_info = get_baserel_parampathinfo(root, rel,
                             required_outer);
	  pathnode->path.parallel_aware = false;
	  pathnode->path.parallel_safe = false;
	  pathnode->path.parallel_workers = 0;
	  pathnode->path.pathkeys = NIL;
	  /* Gather has unordered result */

  pathnode->subpath = subpath;
	  pathnode->num_workers = subpath->parallel_workers;
	  pathnode->single_copy = false;

	  if (pathnode->num_workers == 0)
 
	{
		    pathnode->path.pathkeys = subpath->pathkeys;
		    pathnode->num_workers = 1;
		    pathnode->single_copy = true;
		 
	}

	  cost_gather(pathnode, root, rel, pathnode->path.param_info, rows);

	  return pathnode;
}

void
get_agg_clause_costs(PlannerInfo *root, Node *clause, AggSplit aggsplit,
          AggClauseCosts *costs)
{
	  get_agg_clause_costs_context context;

	  context.root = root;
	  context.aggsplit = aggsplit;
	  context.costs = costs;
	  (void) get_agg_clause_costs_walker(clause, &context);
}

void
add_path(RelOptInfo *parent_rel, Path *new_path)
{
	  bool    accept_new = true;
	  /* unless we find a superior old path */
  ListCell  *insert_after = NULL;
	 /* where to insert new item */
  List    *new_path_pathkeys;
	  ListCell  *p1;
	  ListCell  *p1_prev;
	  ListCell  *p1_next;

	  /*
	  * This is a convenient place to check for query cancel --- no part of the
	  * planner goes very long without calling add_path().
	  */
  CHECK_FOR_INTERRUPTS();

	  /* Pretend parameterized paths have no pathkeys, per comment above */
  new_path_pathkeys = new_path->param_info ? NIL : new_path->pathkeys;

	  /*
	  * Loop to check proposed new path against old paths. Note it is possible
	  * for more than one old path to be tossed out because new_path dominates
	  * it.
	  *
	  * We can't use foreach here because the loop body may delete the current
	  * list cell.
	  */
  p1_prev = NULL;
	  for (p1 = list_head(parent_rel->pathlist); p1 != NULL; p1 = p1_next)
 
	{
		    Path    *old_path = (Path *) lfirst(p1);
		    bool    remove_old = false; /* unless new proves superior */
		    PathCostComparison costcmp;
		    PathKeysComparison keyscmp;
		    BMS_Comparison outercmp;

		    p1_next = lnext(p1);

		    /*
		    * Do a fuzzy cost comparison with standard fuzziness limit.
		    */
    costcmp = compare_path_costs_fuzzily(new_path, old_path,
                      STD_FUZZ_FACTOR);

		    /*
		    * If the two paths compare differently for startup and total cost,
		    * then we want to keep both, and we can skip comparing pathkeys and
		    * required_outer rels. If they compare the same, proceed with the
		    * other comparisons. Row count is checked last. (We make the tests
		    * in this order because the cost comparison is most likely to turn
		    * out "different", and the pathkeys comparison next most likely. As
		    * explained above, row count very seldom makes a difference, so even
		    * though it's cheap to compare there's not much point in checking it
		    * earlier.)
		    */
    if (costcmp != COSTS_DIFFERENT)
   
		{
			      /* Similarly check to see if either dominates on pathkeys */
      List    *old_path_pathkeys;

			      old_path_pathkeys = old_path->param_info ? NIL : old_path->pathkeys;
			      keyscmp = compare_pathkeys(new_path_pathkeys,
                   old_path_pathkeys);
			      if (keyscmp != PATHKEYS_DIFFERENT)
     
			{
				        switch (costcmp)
       
				{
					          case COSTS_EQUAL:
            outercmp = bms_subset_compare(PATH_REQ_OUTER(new_path),
                         PATH_REQ_OUTER(old_path));
					            if (keyscmp == PATHKEYS_BETTER1)
           
					{
						              if ((outercmp == BMS_EQUAL ||
                outercmp == BMS_SUBSET1) &&
                new_path->rows <= old_path->rows &&
                new_path->parallel_safe >= old_path->parallel_safe)
                remove_old = true;
						  /* new dominates old */
           
					}
					            else if (keyscmp == PATHKEYS_BETTER2)
           
					{
						              if ((outercmp == BMS_EQUAL ||
                outercmp == BMS_SUBSET2) &&
                new_path->rows >= old_path->rows &&
                new_path->parallel_safe <= old_path->parallel_safe)
                accept_new = false;
						  /* old dominates new */
           
					}
					            else  /* keyscmp == PATHKEYS_EQUAL */
           
					{
						              if (outercmp == BMS_EQUAL)
             
						{
							                /*
							                * Same pathkeys and outer rels, and
							fuzzily                 * the same cost, so keep just
							one; to decide                 * which, first check
							parallel-safety, then                 * rows, then do a
							fuzzy cost comparison with                 * very small
							fuzz limit. (We used to do an                 * exact
							cost comparison, but that results in                 *
							annoying platform-specific plan variations               
							 * due to roundoff in the cost estimates.) If            
							    * things are still tied, arbitrarily keep           
							     * only the old path. Notice that we will          
							      * keep only the old path even if the           
							     * less-fuzzy comparison decides the startup         
							       * and total costs compare differently.         
							       */
                if (new_path->parallel_safe >
                  old_path->parallel_safe)
                  remove_old = true;
							/* new dominates old */
                else if (new_path->parallel_safe <
                    old_path->parallel_safe)
                  accept_new = false;	/* old dominates new */
							                else if (new_path->rows <
																	 old_path->rows)
                  remove_old = true;
							/* new dominates old */
                else if (new_path->rows > old_path->rows)
                  accept_new = false; /* old dominates new */
							                else if (
								compare_path_costs_fuzzily(new_path,
                                  old_path,
                       1.0000000001) == COSTS_BETTER1)
                  remove_old = true;
							/* new dominates old */
                else                   accept_new =
								false; /* old equals or
                            * dominates new */
							             
						}
						              else if (outercmp == BMS_SUBSET1 &&
                  new_path->rows <= old_path->rows &&
                  new_path->parallel_safe >= old_path->parallel_safe)
                remove_old = true;
						  /* new dominates old */
              else if (outercmp == BMS_SUBSET2 &&
                  new_path->rows >= old_path->rows &&
                  new_path->parallel_safe <= old_path->parallel_safe)
                accept_new = false;
						  /* old dominates new */
              /* else different parameterizations, keep both */
           
					}
					            break;
					          case COSTS_BETTER1:
            if (keyscmp != PATHKEYS_BETTER2)
           
					{
						              outercmp =
							bms_subset_compare(PATH_REQ_OUTER(new_path),
                         PATH_REQ_OUTER(old_path));
						              if ((outercmp == BMS_EQUAL ||
                outercmp == BMS_SUBSET1) &&
                new_path->rows <= old_path->rows &&
                new_path->parallel_safe >= old_path->parallel_safe)
                remove_old = true;
						  /* new dominates old */
           
					}
					            break;
					          case COSTS_BETTER2:
            if (keyscmp != PATHKEYS_BETTER1)
           
					{
						              outercmp =
							bms_subset_compare(PATH_REQ_OUTER(new_path),
                         PATH_REQ_OUTER(old_path));
						              if ((outercmp == BMS_EQUAL ||
                outercmp == BMS_SUBSET2) &&
                new_path->rows >= old_path->rows &&
                new_path->parallel_safe <= old_path->parallel_safe)
                accept_new = false;
						  /* old dominates new */
           
					}
					            break;
					          case COSTS_DIFFERENT:

            /*
             * can't get here, but keep this case to keep compiler
             * quiet
             */
            break;
					       
				}
				     
			}
			   
		}

		    /*
		    * Remove current element from pathlist if dominated by new.
		    */
    if (remove_old)
   
		{
			      parent_rel->pathlist = list_delete_cell(parent_rel->pathlist,
                          p1, p1_prev);

			      /*
			      * Delete the data pointed-to by the deleted cell, if possible
			      */
      if (!IsA(old_path, IndexPath))
        pfree(old_path);
			      /* p1_prev does not advance */
   
		}
		    else    
		{
			      /* new belongs after this old path if it has cost >= old's */
      if (new_path->total_cost >= old_path->total_cost)
        insert_after = p1;
			      /* p1_prev advances */
      p1_prev = p1;
			   
		}

		    /*
		    * If we found an old path that dominates new_path, we can quit
		    * scanning the pathlist; we will not add new_path, and we assume
		    * new_path cannot dominate any other elements of the pathlist.
		    */
    if (!accept_new)
      break;
		 
	}

	  if (accept_new)
 
	{
		    /* Accept the new path: insert it at proper place in pathlist */
    if (insert_after)
      lappend_cell(parent_rel->pathlist, insert_after, new_path);
		    else       parent_rel->pathlist = lcons(new_path, parent_rel->pathlist);
		 
	}
	  else  
	{
		    /* Reject and recycle the new path */
    if (!IsA(new_path, IndexPath))
      pfree(new_path);
		 
	}
}



Path *
apply_projection_to_path(PlannerInfo *root,
            RelOptInfo *rel,
            Path *path,
            PathTarget *target)
{
	  QualCost  oldcost;

	  /*
	  * If given path can't project, we might need a Result node, so make a
	  * separate ProjectionPath.
	  */
  if (!is_projection_capable_path(path))
    return (Path *) create_projection_path(root, rel, path, target);

	  /*
	  * We can just jam the desired tlist into the existing path, being sure to
	  * update its cost estimates appropriately.
	  */
  oldcost = path->pathtarget->cost;
	  path->pathtarget = target;

	  path->startup_cost += target->cost.startup - oldcost.startup;
	  path->total_cost += target->cost.startup - oldcost.startup +
    (target->cost.per_tuple - oldcost.per_tuple) * path->rows;

	  /*
	  * If the path happens to be a Gather path, we'd like to arrange for the
	  * subpath to return the required target list so that workers can help
	  * project. But if there is something that is not parallel-safe in the
	  * target expressions, then we can't.
	  */
  if (IsA(path, GatherPath) &&
    !has_parallel_hazard((Node *) target->exprs, false))
 
	{
		    GatherPath *gpath = (GatherPath *) path;

		    /*
		    * We always use create_projection_path here, even if the subpath is
		    * projection-capable, so as to avoid modifying the subpath in place.
		    * It seems unlikely at present that there could be any other
		    * references to the subpath, but better safe than sorry.
		    *
		    * Note that we don't change the GatherPath's cost estimates; it might
		    * be appropriate to do so, to reflect the fact that the bulk of the
		    * target evaluation will happen in workers.
		    */
    gpath->subpath = (Path *)
      create_projection_path(root,
                 gpath->subpath->parent,
                 gpath->subpath,
                 target);
		 
	}
	  else if (path->parallel_safe &&
      has_parallel_hazard((Node *) target->exprs, false))
 
	{
		    /*
		    * We're inserting a parallel-restricted target list into a path
		    * currently marked parallel-safe, so we have to mark it as no longer
		    * safe.
		    */
    path->parallel_safe = false;
		 
	}

	  return path;
}

//gausskernel\optimizer\path\pathkeys.cpp
// List *
// make_pathkeys_for_sortclauses(PlannerInfo *root,
//                List *sortclauses,
//                List *tlist)
// {
// 	  List    *pathkeys = NIL;
// 	  ListCell  *l;

// 	  foreach (l, sortclauses)
 
// 	{
// 		    SortGroupClause *sortcl = (SortGroupClause *) lfirst(l);
// 		    Expr    *sortkey;
// 		    PathKey  *pathkey;

// 		    sortkey = (Expr *) get_sortgroupclause_expr(sortcl, tlist);
// 		    Assert(OidIsValid(sortcl->sortop));
// 		    pathkey = make_pathkey_from_sortop(root,
//                      sortkey,
//                      root->nullable_baserels,
//                      sortcl->sortop,
//                      sortcl->nulls_first,
//                      sortcl->tleSortGroupRef,
//                      true);

// 		    /* Canonical form eliminates redundant ordering keys */
//     if (!pathkey_is_redundant(pathkey, pathkeys))
//       pathkeys = lappend(pathkeys, pathkey);
		 
// 	}
// 	  return pathkeys;
// }



RelOptInfo *
fetch_upper_rel(PlannerInfo *root, UpperRelationKind kind, Relids relids)
{
	  RelOptInfo *upperrel;
	  ListCell  *lc;

	  /*
	  * For the moment, our indexing data structure is just a List for each
	  * relation kind. If we ever get so many of one kind that this stops
	  * working well, we can improve it. No code outside this function should
	  * assume anything about how to find a particular upperrel.
	  */

  /* If we already made this upperrel for the query, return it */
  foreach (lc, root->upper_rels[kind])//tsdb 这里要加成员upper_rels
 
	{
		    upperrel = (RelOptInfo *) lfirst(lc);

		    if (bms_equal(upperrel->relids, relids))
      return upperrel;
		 
	}

	  upperrel = makeNode(RelOptInfo);
	  upperrel->reloptkind = (RelOptKind)3;//tsdb RELOPT_UPPER_REL是3
	  upperrel->relids = bms_copy(relids);

	  /* cheap startup cost is interesting iff not all tuples to be retrieved */
      //tsdb RelOptInfo需要加入成员consider_startup,consider_param_startup
      upperrel->consider_startup = (root->tuple_fraction > 0);
	  upperrel->consider_param_startup = false;
	  upperrel->consider_parallel = false;
	   /* might get changed later */
  upperrel->reltarget = create_empty_pathtarget();
	  upperrel->pathlist = NIL;
	  upperrel->cheapest_startup_path = NULL;
	  upperrel->cheapest_total_path = NULL;
	  upperrel->cheapest_unique_path = NULL;
	  upperrel->cheapest_parameterized_paths = NIL;

	  root->upper_rels[kind] = lappend(root->upper_rels[kind], upperrel);

	  return upperrel;
}


MinMaxAggPath *
create_minmaxagg_path(PlannerInfo *root,
           RelOptInfo *rel,
           PathTarget *target,
           List *mmaggregates,
           List *quals)
{
	  MinMaxAggPath *pathnode = makeNode(MinMaxAggPath);
	  Cost    initplan_cost;
	  ListCell  *lc;

	  /* The topmost generated Plan node will be a Result */
  	pathnode->path.pathtype = T_BaseResult;
	  pathnode->path.parent = rel;
	  pathnode->path.pathtarget = target;
	  /* For now, assume we are above any joins, so no parameterization */
  pathnode->path.param_info = NULL;
	  pathnode->path.parallel_aware = false;
	  /* A MinMaxAggPath implies use of subplans, so cannot be parallel-safe */
  pathnode->path.parallel_safe = false;
	  pathnode->path.parallel_workers = 0;
	  /* Result is one unordered row */
  pathnode->path.rows = 1;
	  pathnode->path.pathkeys = NIL;

	  pathnode->mmaggregates = mmaggregates;
	  pathnode->quals = quals;

	  /* Calculate cost of all the initplans ... */
  initplan_cost = 0;
	  foreach (lc, mmaggregates)
 
	{
		    MinMaxAggInfo *mminfo = (MinMaxAggInfo *) lfirst(lc);

		    initplan_cost += mminfo->pathcost;
		 
	}
	

	  /* add tlist eval cost for each output row, plus cpu_tuple_cost */
  pathnode->path.startup_cost = initplan_cost + target->cost.startup;
	  pathnode->path.total_cost = initplan_cost + target->cost.startup +
    target->cost.per_tuple +u_sess->attr.attr_sql.cpu_tuple_cost;

	  return pathnode;
}

PathTarget *
make_pathtarget_from_tlist(List *tlist)
{
	  PathTarget *target = makeNode(PathTarget);
	  int     i;
	  ListCell  *lc;

	  target->sortgrouprefs = (Index *) palloc(list_length(tlist) * sizeof(Index));

	  i = 0;
	  foreach (lc, tlist)
 
	{
		    TargetEntry *tle = (TargetEntry *) lfirst(lc);

		    target->exprs = lappend(target->exprs, tle->expr);
		    target->sortgrouprefs[i] = tle->ressortgroupref;
		    i++;
		 
	}

	  return target;
}

PathTarget *
set_pathtarget_cost_width(PlannerInfo *root, PathTarget *target)
{
	  int32    tuple_width = 0;
	  ListCell  *lc;

	  /* Vars are assumed to have cost zero, but other exprs do not */
  target->cost.startup = 0;
	  target->cost.per_tuple = 0;

	  foreach (lc, target->exprs)
 
	{
		    Node    *node = (Node *) lfirst(lc);

		    if (IsA(node, Var))
   
		{
			      Var    *var = (Var *) node;
			      int32    item_width;

			      /* We should not see any upper-level Vars here */
      Assert(var->varlevelsup == 0);

			      /* Try to get data from RelOptInfo cache */
      if (var->varno < root->simple_rel_array_size)
     
			{
				        RelOptInfo *rel = root->simple_rel_array[var->varno];

				        if (rel != NULL &&
          var->varattno >= rel->min_attr &&
          var->varattno <= rel->max_attr)
       
				{
					          int     ndx = var->varattno - rel->min_attr;

					          if (rel->attr_widths[ndx] > 0)
         
					{
						            tuple_width += rel->attr_widths[ndx];
						            continue;
						         
					}
					       
				}
				     
			}

			      /*
			      * No cached data available, so estimate using just the type info.
			      */
      item_width = get_typavgwidth(var->vartype, var->vartypmod);
			      Assert(item_width > 0);
			      tuple_width += item_width;
			   
		}
		    else    
		{
			      /*
			      * Handle general expressions using type info.
			      */
      int32    item_width;
			      QualCost  cost;

			      item_width = get_typavgwidth(exprType(node), exprTypmod(node));
			      Assert(item_width > 0);
			      tuple_width += item_width;

			      /* Account for cost, too */
      cost_qual_eval_node(&cost, node, root);
			      target->cost.startup += cost.startup;
			      target->cost.per_tuple += cost.per_tuple;
			   
		}
		 
	}

	  Assert(tuple_width >= 0);
	  target->width = tuple_width;

	  return target;
}

void
SS_identify_outer_params(PlannerInfo *root)
{
	  Bitmapset *outer_params;
	  PlannerInfo *proot;
	  ListCell  *l;

	  /*
	  * If no parameters have been assigned anywhere in the tree, we certainly
	  * don't need to do anything here.
	  */
  if (root->glob->nParamExec == 0)
    return;

	  /*
	  * Scan all query levels above this one to see which parameters are due to
	  * be available from them, either because lower query levels have
	  * requested them (via plan_params) or because they will be available from
	  * initPlans of those levels.
	  */
  outer_params = NULL;
	  for (proot = root->parent_root; proot != NULL; proot = proot->parent_root)
 
	{
		    /* Include ordinary Var/PHV/Aggref params */
    foreach (l, proot->plan_params)
   
		{
			      PlannerParamItem *pitem = (PlannerParamItem *) lfirst(l);

			      outer_params = bms_add_member(outer_params, pitem->paramId);
			   
		}
		    /* Include any outputs of outer-level initPlans */
    foreach (l, proot->init_plans)
   
		{
			      SubPlan  *initsubplan = (SubPlan *) lfirst(l);
			      ListCell  *l2;

			      foreach (l2, initsubplan->setParam)
     
			{
				        outer_params = bms_add_member(outer_params, lfirst_int(l2));
				     
			}
			   
		}
		    /* Include worktable ID, if a recursive query is being planned */
    if (proot->wt_param_id >= 0)
      outer_params = bms_add_member(outer_params, proot->wt_param_id);
		 
	}
    //需要在PlannerInfo加入成员outer_params
	  root->outer_params = outer_params;
}

void
SS_charge_for_initplans(PlannerInfo *root, RelOptInfo *final_rel)
{
	  Cost    initplan_cost;
	  ListCell  *lc;

	  /* Nothing to do if no initPlans */
  if (root->init_plans == NIL)
    return;

	  /*
	  * Compute the cost increment just once, since it will be the same for all
	  * Paths. We assume each initPlan gets run once during top plan startup.
	  * This is a conservative overestimate, since in fact an initPlan might be
	  * executed later than plan startup, or even not at all.
	  */
  initplan_cost = 0;
	  foreach (lc, root->init_plans)
 
	{
		    SubPlan  *initsubplan = (SubPlan *) lfirst(lc);

		    initplan_cost += initsubplan->startup_cost + initsubplan->per_call_cost;
		 
	}

	  /*
	  * Now adjust the costs and parallel_safe flags.
	  */
  foreach (lc, final_rel->pathlist)
 
	{
		    Path    *path = (Path *) lfirst(lc);

		    path->startup_cost += initplan_cost;
		    path->total_cost += initplan_cost;
		    path->parallel_safe = false;
		 
	}

	  /* We needn't do set_cheapest() here, caller will do it */
}

//这个函数在og内核中有,是一个静态函数,具体情况之后讨论
PathKey *
make_canonical_pathkey(PlannerInfo *root,
           EquivalenceClass *eclass, Oid opfamily,
           int strategy, bool nulls_first)
{
	  PathKey  *pk;
	  ListCell  *lc;
	  MemoryContext oldcontext;

	  /* The passed eclass might be non-canonical, so chase up to the top */
  while (eclass->ec_merged)
    eclass = eclass->ec_merged;

	  foreach (lc, root->canon_pathkeys)
 
	{
		    pk = (PathKey *) lfirst(lc);
		    if (eclass == pk->pk_eclass &&
      opfamily == pk->pk_opfamily &&
      strategy == pk->pk_strategy &&
      nulls_first == pk->pk_nulls_first)
      return pk;
		 
	}

	  /*
	  * Be sure canonical pathkeys are allocated in the main planning context.
	  * Not an issue in normal planning, but it is for GEQO.
	  */
  oldcontext = MemoryContextSwitchTo(root->planner_cxt);

	  pk = makeNode(PathKey);
	  pk->pk_eclass = eclass;
	  pk->pk_opfamily = opfamily;
	  pk->pk_strategy = strategy;
	  pk->pk_nulls_first = nulls_first;

	  root->canon_pathkeys = lappend(root->canon_pathkeys, pk);

	  MemoryContextSwitchTo(oldcontext);

	  return pk;
}

//放在og的gausskernel/optimizer/path/equivclass.cpp
// EquivalenceClass *
// get_eclass_for_sort_expr_tsdb(PlannerInfo *root,
//             Expr *expr,
//             Relids nullable_relids,
//             List *opfamilies,
//             Oid opcintype,
//             Oid collation,
//             Index sortref,
//             Relids rel,
//             bool create_it)
// {
// 	  Relids   expr_relids;
// 	  EquivalenceClass *newec;
// 	  EquivalenceMember *newem;
// 	  ListCell  *lc1;
// 	  MemoryContext oldcontext;

// 	  /*
// 	  * Ensure the expression exposes the correct type and collation.
// 	  */
//   expr = canonicalize_ec_expression(expr, opcintype, collation);

// 	  /*
// 	  * Get the precise set of nullable relids appearing in the expression.
// 	  */
//   expr_relids = pull_varnos((Node *) expr);
// 	  nullable_relids = bms_intersect(nullable_relids, expr_relids);

// 	  /*
// 	  * Scan through the existing EquivalenceClasses for a match
// 	  */
//   foreach (lc1, root->eq_classes)
 
// 	{
// 		    EquivalenceClass *cur_ec = (EquivalenceClass *) lfirst(lc1);
// 		    ListCell  *lc2;

// 		    /*
// 		    * Never match to a volatile EC, except when we are looking at another
// 		    * reference to the same volatile SortGroupClause.
// 		    */
//     if (cur_ec->ec_has_volatile &&
//       (sortref == 0 || sortref != cur_ec->ec_sortref))
//       continue;

// 		    if (collation != cur_ec->ec_collation)
//       continue;
// 		    if (!equal(opfamilies, cur_ec->ec_opfamilies))
//       continue;

// 		    foreach (lc2, cur_ec->ec_members)
   
// 		{
// 			      EquivalenceMember *cur_em = (EquivalenceMember *) lfirst(lc2);

// 			      /*
// 			      * Ignore child members unless they match the request.
// 			      */
//       if (cur_em->em_is_child &&
//         !bms_equal(cur_em->em_relids, rel))
//         continue;

// 			      /*
// 			      * If below an outer join, don't match constants: they're not as
// 			      * constant as they look.
// 			      */
//       if (cur_ec->ec_below_outer_join &&
//         cur_em->em_is_const)
//         continue;

// 			      if (opcintype == cur_em->em_datatype &&
//         equal(expr, cur_em->em_expr))
//         return cur_ec;
// 			/* Match! */
   
// 		}
		 
// 	}

// 	  /* No match; does caller want a NULL result? */
//   if (!create_it)
//     return NULL;

// 	  /*
// 	  * OK, build a new single-member EC
// 	  *
// 	  * Here, we must be sure that we construct the EC in the right context.
// 	  */
//   oldcontext = MemoryContextSwitchTo(root->planner_cxt);

// 	  newec = makeNode(EquivalenceClass);
// 	  newec->ec_opfamilies = list_copy(opfamilies);
// 	  newec->ec_collation = collation;
// 	  newec->ec_members = NIL;
// 	  newec->ec_sources = NIL;
// 	  newec->ec_derives = NIL;
// 	  newec->ec_relids = NULL;
// 	  newec->ec_has_const = false;
// 	  newec->ec_has_volatile = contain_volatile_functions((Node *) expr);
// 	  newec->ec_below_outer_join = false;
// 	  newec->ec_broken = false;
// 	  newec->ec_sortref = sortref;
// 	  newec->ec_merged = NULL;

// 	  if (newec->ec_has_volatile && sortref == 0) /* should not happen */
//     elog(ERROR, "volatile EquivalenceClass has no sortref");

// 	  newem = add_eq_member(newec,(Expr*) copyObject(expr), expr_relids,
//              nullable_relids, false, opcintype);

// 	  /*
// 	  * add_eq_member doesn't check for volatile functions, set-returning
// 	  * functions, aggregates, or window functions, but such could appear in
// 	  * sort expressions; so we have to check whether its const-marking was
// 	  * correct.
// 	  */
//   if (newec->ec_has_const)
 
// 	{
// 		    if (newec->ec_has_volatile ||
//       expression_returns_set((Node *) expr) ||
//       contain_agg_clause((Node *) expr) ||
//       contain_window_function((Node *) expr))
   
// 		{
// 			      newec->ec_has_const = false;
// 			      newem->em_is_const = false;
			   
// 		}
		 
// 	}

// 	  root->eq_classes = lappend(root->eq_classes, newec);

// 	  MemoryContextSwitchTo(oldcontext);

// 	  return newec;
// }

Oid
get_role_oid_or_public(const char *rolname)
{
	  if (strcmp(rolname, "public") == 0)
    return ACL_ID_PUBLIC;

	  return get_role_oid(rolname, false);
}

// #include "utils/palloc.h"
char *
GetUserNameFromId(Oid roleid, bool noerr)
{
	  HeapTuple  tuple;
	  char    *result;

	  tuple = SearchSysCache1(AUTHOID, ObjectIdGetDatum(roleid));
	  if (!HeapTupleIsValid(tuple))
 
	{
		    if (!noerr)
      ereport(ERROR,
          (errcode(ERRCODE_UNDEFINED_OBJECT),
          errmsg("invalid role OID: %u", roleid)));
		    result = NULL;
		 
	}
	  else  
	{
		    result = pstrdup(NameStr(((Form_pg_authid) GETSTRUCT(tuple))->rolname));
		    ReleaseSysCache(tuple);
		 
	}
	  return result;
}




//2022-10-21,带eventtrigger的,都在timescaledb里面实现



// bool
// trackDroppedObjectsNeeded(void)
// {
// 	/*
// 	 * true if any sql_drop, table_rewrite, ddl_command_end event trigger
// 	 * exists
// 	 */
// 	return list_length(EventCacheLookup(EVT_SQLDrop)) > 0 ||
// 		   list_length(EventCacheLookup(EVT_TableRewrite)) > 0 ||
// 		   list_length(EventCacheLookup(EVT_DDLCommandEnd)) > 0;
// }







Oid
get_rolespec_oid(const Node *node, bool missing_ok)
{
	  RoleSpec  *role;
	  Oid     oid;

	  if (!IsA(node, RoleSpec))
    elog(ERROR, "invalid node type %d", node->type);

	  role = (RoleSpec *) node;
	  switch (role->roletype)
 
	{
		    case ROLESPEC_CSTRING:
      Assert(role->rolename);
		      oid = get_role_oid(role->rolename, missing_ok);
		      break;

		    case ROLESPEC_CURRENT_USER:
      oid = GetUserId();
		      break;

		    case ROLESPEC_SESSION_USER:
      oid = GetSessionUserId();
		      break;

		    case ROLESPEC_PUBLIC:
      ereport(ERROR,
          (errcode(ERRCODE_UNDEFINED_OBJECT),
          errmsg("role \"%s\" does not exist", "public")));
		      oid = InvalidOid;
		 /* make compiler happy */
      break;

		    default:
      elog(ERROR, "unexpected role type %d", role->roletype);
		 
	}

	  return oid;
}
//用og内核部分的这个函数
// Oid
// index_create(Relation heapRelation, const char *indexRelationName, Oid indexRelationId,
// 			 Oid relFileNode, IndexInfo *indexInfo, List *indexColNames, Oid accessMethodObjectId,
// 			 Oid tableSpaceId, Oid *collationObjectId, Oid *classObjectId, int16 *coloptions,
// 			 Datum reloptions, bool isprimary, bool isconstraint, bool deferrable,
// 			 bool initdeferred, bool allow_system_table_mods, bool skip_build, bool concurrent,
// 			 bool is_internal, bool if_not_exists)
// {
// 	Oid heapRelationId = RelationGetRelid(heapRelation);
// 	Relation pg_class;
// 	Relation indexRelation;
// 	TupleDesc indexTupDesc;
// 	bool shared_relation;
// 	bool mapped_relation;
// 	bool is_exclusion;
// 	Oid namespaceId;
// 	int i;
// 	char relpersistence;

// 	is_exclusion = (indexInfo->ii_ExclusionOps != NULL);

// 	pg_class = heap_open(RelationRelationId, RowExclusiveLock);

// 	/*
// 	 * The index will be in the same namespace as its parent table, and is
// 	 * shared across databases if and only if the parent is.  Likewise, it
// 	 * will use the relfilenode map if and only if the parent does; and it
// 	 * inherits the parent's relpersistence.
// 	 */
// 	namespaceId = RelationGetNamespace(heapRelation);
// 	shared_relation = heapRelation->rd_rel->relisshared;
// 	mapped_relation = RelationIsMapped(heapRelation);
// 	relpersistence = heapRelation->rd_rel->relpersistence;

// 	/*
// 	 * check parameters
// 	 */
// 	if (indexInfo->ii_NumIndexAttrs < 1)
// 		elog(ERROR, "must index at least one column");

// 	if (!allow_system_table_mods && IsSystemRelation(heapRelation) && IsNormalProcessingMode())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("user-defined indexes on system catalog tables are not supported")));

// 	/*
// 	 * concurrent index build on a system catalog is unsafe because we tend to
// 	 * release locks before committing in catalogs
// 	 */
// 	if (concurrent && IsSystemRelation(heapRelation))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("concurrent index creation on system catalog tables is not supported")));

// 	/*
// 	 * This case is currently not supported, but there's no way to ask for it
// 	 * in the grammar anyway, so it can't happen.
// 	 */
// 	if (concurrent && is_exclusion)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg_internal(
// 					 "concurrent index creation for exclusion constraints is not supported")));

// 	/*
// 	 * We cannot allow indexing a shared relation after initdb (because
// 	 * there's no way to make the entry in other databases' pg_class).
// 	 */
// 	if (shared_relation && !IsBootstrapProcessingMode())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("shared indexes cannot be created after initdb")));

// 	/*
// 	 * Shared relations must be in pg_global, too (last-ditch check)
// 	 */
// 	if (shared_relation && tableSpaceId != GLOBALTABLESPACE_OID)
// 		elog(ERROR, "shared relations must be placed in pg_global tablespace");

// 	if (get_relname_relid(indexRelationName, namespaceId))
// 	{
// 		if (if_not_exists)
// 		{
// 			ereport(NOTICE,
// 					(errcode(ERRCODE_DUPLICATE_TABLE),
// 					 errmsg("relation \"%s\" already exists, skipping", indexRelationName)));
// 			heap_close(pg_class, RowExclusiveLock);
// 			return InvalidOid;
// 		}

// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_TABLE),
// 				 errmsg("relation \"%s\" already exists", indexRelationName)));
// 	}

// 	/*
// 	 * construct tuple descriptor for index tuples
// 	 */
// 	indexTupDesc = ConstructTupleDescriptor(heapRelation,
// 											indexInfo,
// 											indexColNames,
// 											accessMethodObjectId,
// 											collationObjectId,
// 											classObjectId);

// 	/*
// 	 * Allocate an OID for the index, unless we were told what to use.
// 	 *
// 	 * The OID will be the relfilenode as well, so make sure it doesn't
// 	 * collide with either pg_class OIDs or existing physical files.
// 	 */
// 	if (!OidIsValid(indexRelationId))
// 	{
// 		/* Use binary-upgrade override for pg_class.oid/relfilenode? */
// 		if (IsBinaryUpgrade)
// 		{
// 			if (!OidIsValid(og_knl_session1->binary_upgrade_next_index_pg_class_oid))
// 				ereport(ERROR,
// 						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// 						 errmsg("pg_class index OID value not set when in binary upgrade mode")));

// 			indexRelationId =og_knl_session1-> binary_upgrade_next_index_pg_class_oid;
// 			og_knl_session1->binary_upgrade_next_index_pg_class_oid = InvalidOid;
// 		}
// 		else
// 		{
// 			indexRelationId = GetNewRelFileNode(tableSpaceId, pg_class, relpersistence);
// 		}
// 	}

// 	/*
// 	 * create the index relation's relcache entry and physical disk file. (If
// 	 * we fail further down, it's the smgr's responsibility to remove the disk
// 	 * file again.)
// 	 */
// 	indexRelation = heap_create(indexRelationName,
// 								namespaceId,
// 								tableSpaceId,
// 								indexRelationId,
// 								relFileNode,
// 								indexTupDesc,
// 								RELKIND_INDEX,
// 								relpersistence,
// 								shared_relation,
// 								mapped_relation,
// 								allow_system_table_mods);

// 	Assert(indexRelationId == RelationGetRelid(indexRelation));

// 	/*
// 	 * Obtain exclusive lock on it.  Although no other backends can see it
// 	 * until we commit, this prevents deadlock-risk complaints from lock
// 	 * manager in cases such as CLUSTER.
// 	 */
// 	LockRelation(indexRelation, AccessExclusiveLock);

// 	/*
// 	 * Fill in fields of the index's pg_class entry that are not set correctly
// 	 * by heap_create.
// 	 *
// 	 * XXX should have a cleaner way to create cataloged indexes
// 	 */
// 	indexRelation->rd_rel->relowner = heapRelation->rd_rel->relowner;
// 	indexRelation->rd_rel->relam = accessMethodObjectId;
// 	indexRelation->rd_rel->relhasoids = false;

// 	/*
// 	 * store index's pg_class entry
// 	 */
// 	InsertPgClassTuple(pg_class,
// 					   indexRelation,
// 					   RelationGetRelid(indexRelation),
// 					   (Datum) 0,
// 					   reloptions);

// 	/* done with pg_class */
// 	heap_close(pg_class, RowExclusiveLock);

// 	/*
// 	 * now update the object id's of all the attribute tuple forms in the
// 	 * index relation's tuple descriptor
// 	 */
// 	InitializeAttributeOids(indexRelation, indexInfo->ii_NumIndexAttrs, indexRelationId);

// 	/*
// 	 * append ATTRIBUTE tuples for the index
// 	 */
// 	AppendAttributeTuples(indexRelation, indexInfo->ii_NumIndexAttrs);

// 	/* ----------------
// 	 *	  update pg_index
// 	 *	  (append INDEX tuple)
// 	 *
// 	 *	  Note that this stows away a representation of "predicate".
// 	 *	  (Or, could define a rule to maintain the predicate) --Nels, Feb '92
// 	 * ----------------
// 	 */
// 	UpdateIndexRelation(indexRelationId,
// 						heapRelationId,
// 						indexInfo,
// 						collationObjectId,
// 						classObjectId,
// 						coloptions,
// 						isprimary,
// 						is_exclusion,
// 						!deferrable,
// 						!concurrent);

// 	/*
// 	 * Register constraint and dependencies for the index.
// 	 *
// 	 * If the index is from a CONSTRAINT clause, construct a pg_constraint
// 	 * entry.  The index will be linked to the constraint, which in turn is
// 	 * linked to the table.  If it's not a CONSTRAINT, we need to make a
// 	 * dependency directly on the table.
// 	 *
// 	 * We don't need a dependency on the namespace, because there'll be an
// 	 * indirect dependency via our parent table.
// 	 *
// 	 * During bootstrap we can't register any dependencies, and we don't try
// 	 * to make a constraint either.
// 	 */
// 	if (!IsBootstrapProcessingMode())
// 	{
// 		ObjectAddress myself, referenced;

// 		myself.classId = RelationRelationId;
// 		myself.objectId = indexRelationId;
// 		myself.objectSubId = 0;

// 		if (isconstraint)
// 		{
// 			char constraintType;

// 			if (isprimary)
// 				constraintType = CONSTRAINT_PRIMARY;
// 			else if (indexInfo->ii_Unique)
// 				constraintType = CONSTRAINT_UNIQUE;
// 			else if (is_exclusion)
// 				constraintType = CONSTRAINT_EXCLUSION;
// 			else
// 			{
// 				elog(ERROR, "constraint must be PRIMARY, UNIQUE or EXCLUDE");
// 				constraintType = 0; /* keep compiler quiet */
// 			}

// 			index_constraint_create(heapRelation,
// 									indexRelationId,
// 									indexInfo,
// 									indexRelationName,
// 									constraintType,
// 									deferrable,
// 									initdeferred,
// 									false, /* already marked primary */
// 									false, /* pg_index entry is OK */
// 									false, /* no old dependencies */
// 									allow_system_table_mods,
// 									is_internal);
// 		}
// 		else
// 		{
// 			bool have_simple_col = false;

// 			/* Create auto dependencies on simply-referenced columns */
// 			for (i = 0; i < indexInfo->ii_NumIndexAttrs; i++)
// 			{
// 				if (indexInfo->ii_KeyAttrNumbers[i] != 0)
// 				{
// 					referenced.classId = RelationRelationId;
// 					referenced.objectId = heapRelationId;
// 					referenced.objectSubId = indexInfo->ii_KeyAttrNumbers[i];

// 					recordDependencyOn(&myself, &referenced, DEPENDENCY_AUTO);

// 					have_simple_col = true;
// 				}
// 			}

// 			/*
// 			 * If there are no simply-referenced columns, give the index an
// 			 * auto dependency on the whole table.  In most cases, this will
// 			 * be redundant, but it might not be if the index expressions and
// 			 * predicate contain no Vars or only whole-row Vars.
// 			 */
// 			if (!have_simple_col)
// 			{
// 				referenced.classId = RelationRelationId;
// 				referenced.objectId = heapRelationId;
// 				referenced.objectSubId = 0;

// 				recordDependencyOn(&myself, &referenced, DEPENDENCY_AUTO);
// 			}

// 			/* Non-constraint indexes can't be deferrable */
// 			Assert(!deferrable);
// 			Assert(!initdeferred);
// 		}

// 		/* Store dependency on collations */
// 		/* The default collation is pinned, so don't bother recording it */
// 		for (i = 0; i < indexInfo->ii_NumIndexAttrs; i++)
// 		{
// 			if (OidIsValid(collationObjectId[i]) && collationObjectId[i] != DEFAULT_COLLATION_OID)
// 			{
// 				referenced.classId = CollationRelationId;
// 				referenced.objectId = collationObjectId[i];
// 				referenced.objectSubId = 0;

// 				recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 			}
// 		}

// 		/* Store dependency on operator classes */
// 		for (i = 0; i < indexInfo->ii_NumIndexAttrs; i++)
// 		{
// 			referenced.classId = OperatorClassRelationId;
// 			referenced.objectId = classObjectId[i];
// 			referenced.objectSubId = 0;

// 			recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
// 		}

// 		/* Store dependencies on anything mentioned in index expressions */
// 		if (indexInfo->ii_Expressions)
// 		{
// 			recordDependencyOnSingleRelExpr(&myself,
// 											(Node *) indexInfo->ii_Expressions,
// 											heapRelationId,
// 											DEPENDENCY_NORMAL,
// 											DEPENDENCY_AUTO);
// 		}

// 		/* Store dependencies on anything mentioned in predicate */
// 		if (indexInfo->ii_Predicate)
// 		{
// 			recordDependencyOnSingleRelExpr(&myself,
// 											(Node *) indexInfo->ii_Predicate,
// 											heapRelationId,
// 											DEPENDENCY_NORMAL,
// 											DEPENDENCY_AUTO);
// 		}
// 	}
// 	else
// 	{
// 		/* Bootstrap mode - assert we weren't asked for constraint support */
// 		Assert(!isconstraint);
// 		Assert(!deferrable);
// 		Assert(!initdeferred);
// 	}

// 	/* Post creation hook for new index */
// 	InvokeObjectPostCreateHookArg(RelationRelationId, indexRelationId, 0, is_internal);

// 	/*
// 	 * Advance the command counter so that we can see the newly-entered
// 	 * catalog tuples for the index.
// 	 */
// 	CommandCounterIncrement();

// 	/*
// 	 * In bootstrap mode, we have to fill in the index strategy structure with
// 	 * information from the catalogs.  If we aren't bootstrapping, then the
// 	 * relcache entry has already been rebuilt thanks to sinval update during
// 	 * CommandCounterIncrement.
// 	 */
// 	if (IsBootstrapProcessingMode())
// 		RelationInitIndexAccessInfo(indexRelation);
// 	else
// 		Assert(indexRelation->rd_indexcxt != NULL);

// 	/*
// 	 * If this is bootstrap (initdb) time, then we don't actually fill in the
// 	 * index yet.  We'll be creating more indexes and classes later, so we
// 	 * delay filling them in until just before we're done with bootstrapping.
// 	 * Similarly, if the caller specified skip_build then filling the index is
// 	 * delayed till later (ALTER TABLE can save work in some cases with this).
// 	 * Otherwise, we call the AM routine that constructs the index.
// 	 */
// 	if (IsBootstrapProcessingMode())
// 	{
// 		index_register(heapRelationId, indexRelationId, indexInfo);
// 	}
// 	else if (skip_build)
// 	{
// 		/*
// 		 * Caller is responsible for filling the index later on.  However,
// 		 * we'd better make sure that the heap relation is correctly marked as
// 		 * having an index.
// 		 */
// 		index_update_stats(heapRelation, true, isprimary, -1.0);
// 		/* Make the above update visible */
// 		CommandCounterIncrement();
// 	}
// 	else
// 	{
// 		index_build(heapRelation, indexRelation, indexInfo, isprimary, false);
// 	}

// 	/*
// 	 * Close the index; but we keep the lock that we acquired above until end
// 	 * of transaction.  Closing the heap is caller's responsibility.
// 	 */
// 	index_close(indexRelation, NoLock);

// 	return indexRelationId;
// }

// void
// RenameRelationInternal(Oid myrelid, const char *newrelname, bool is_internal)
// {
// 	Relation targetrelation;
// 	Relation relrelation; /* for RELATION relation */
// 	HeapTuple reltup;
// 	Form_pg_class relform;
// 	Oid namespaceId;

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
// 	if (!HeapTupleIsValid(reltup)) /* shouldn't happen */
// 		elog(ERROR, "cache lookup failed for relation %u", myrelid);
// 	relform = (Form_pg_class) GETSTRUCT(reltup);

// 	if (get_relname_relid(newrelname, namespaceId) != InvalidOid)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_TABLE),
// 				 errmsg("relation \"%s\" already exists", newrelname)));

// 	/*
// 	 * Update pg_class tuple with new relname.  (Scribbling on reltup is OK
// 	 * because it's a copy...)
// 	 */
// 	namestrcpy(&(relform->relname), newrelname);

// 	simple_heap_update(relrelation, &reltup->t_self, reltup);

// 	/* keep the system catalog indexes current */
// 	CatalogUpdateIndexes(relrelation, reltup);

// 	InvokeObjectPostAlterHookArg(RelationRelationId, myrelid, 0, InvalidOid, is_internal);

// 	heap_freetuple(reltup);
// 	heap_close(relrelation, RowExclusiveLock);

// 	/*
// 	 * Also rename the associated type, if any.
// 	 */
// 	if (OidIsValid(targetrelation->rd_rel->reltype))
// 		RenameTypeInternal(targetrelation->rd_rel->reltype, newrelname, namespaceId);

// 	/*
// 	 * Also rename the associated constraint, if any.
// 	 */
// 	if (targetrelation->rd_rel->relkind == RELKIND_INDEX)
// 	{
// 		Oid constraintId = get_index_constraint(myrelid);

// 		if (OidIsValid(constraintId))
// 			RenameConstraintById(constraintId, newrelname);
// 	}

// 	/*
// 	 * Close rel, but keep exclusive lock!
// 	 */
// 	relation_close(targetrelation, NoLock);
// }

// List *
// pull_var_clause(Node *node, int flags)
// {
// 	pull_var_clause_context context;

// 	/* Assert that caller has not specified inconsistent flags */
// 	Assert((flags & (PVC_INCLUDE_AGGREGATES | PVC_RECURSE_AGGREGATES)) !=
// 		   (PVC_INCLUDE_AGGREGATES | PVC_RECURSE_AGGREGATES));
// 	Assert((flags & (PVC_INCLUDE_WINDOWFUNCS | PVC_RECURSE_WINDOWFUNCS)) !=
// 		   (PVC_INCLUDE_WINDOWFUNCS | PVC_RECURSE_WINDOWFUNCS));
// 	Assert((flags & (PVC_INCLUDE_PLACEHOLDERS | PVC_RECURSE_PLACEHOLDERS)) !=
// 		   (PVC_INCLUDE_PLACEHOLDERS | PVC_RECURSE_PLACEHOLDERS));

// 	context.varlist = NIL;
// 	context.flags = flags;

// 	pull_var_clause_walker(node, &context);
// 	return context.varlist;
// }


void
mark_index_clustered(Relation rel, Oid indexOid, bool is_internal)
{
	HeapTuple indexTuple;
	Form_pg_index indexForm;
	Relation pg_index;
	ListCell *index;

	/*
	 * If the index is already marked clustered, no need to do anything.
	 */
	if (OidIsValid(indexOid))
	{
		indexTuple = SearchSysCache1(INDEXRELID, ObjectIdGetDatum(indexOid));
		if (!HeapTupleIsValid(indexTuple))
			elog(ERROR, "cache lookup failed for index %u", indexOid);
		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

		if (indexForm->indisclustered)
		{
			ReleaseSysCache(indexTuple);
			return;
		}

		ReleaseSysCache(indexTuple);
	}

	/*
	 * Check each index of the relation and set/clear the bit as needed.
	 */
	pg_index = heap_open(IndexRelationId, RowExclusiveLock);

	foreach (index, RelationGetIndexList(rel))
	{
		Oid thisIndexOid = lfirst_oid(index);

		indexTuple = SearchSysCacheCopy1(INDEXRELID, ObjectIdGetDatum(thisIndexOid));
		if (!HeapTupleIsValid(indexTuple))
			elog(ERROR, "cache lookup failed for index %u", thisIndexOid);
		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

		/*
		 * Unset the bit if set.  We know it's wrong because we checked this
		 * earlier.
		 */
		if (indexForm->indisclustered)
		{
			indexForm->indisclustered = false;
			simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
			CatalogUpdateIndexes(pg_index, indexTuple);
		}
		else if (thisIndexOid == indexOid)
		{
			/* this was checked earlier, but let's be real sure */
			if (!IndexIsValid(indexForm))
				elog(ERROR, "cannot cluster on invalid index %u", indexOid);
			indexForm->indisclustered = true;
			simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
			CatalogUpdateIndexes(pg_index, indexTuple);
		}

		InvokeObjectPostAlterHookArg(IndexRelationId, thisIndexOid, 0, InvalidOid, is_internal);

		heap_freetuple(indexTuple);
	}

	heap_close(pg_index, RowExclusiveLock);
}



ArrayIterator
array_create_iterator(ArrayType *arr, int slice_ndim, ArrayMetaState *mstate)
{
	ArrayIterator iterator =(ArrayIterator) palloc0(sizeof(ArrayIteratorData*));

	/*
	 * Sanity-check inputs --- caller should have got this right already
	 */
	Assert(PointerIsValid(arr));
	if (slice_ndim < 0 || slice_ndim > ARR_NDIM(arr))
		elog(ERROR, "invalid arguments to array_create_iterator");

	/*
	 * Remember basic info about the array and its element type
	 */
	iterator->arr = arr;
	iterator->nullbitmap = ARR_NULLBITMAP(arr);
	iterator->nitems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));

	if (mstate != NULL)
	{
		Assert(mstate->element_type == ARR_ELEMTYPE(arr));

		iterator->typlen = mstate->typlen;
		iterator->typbyval = mstate->typbyval;
		iterator->typalign = mstate->typalign;
	}
	else
		get_typlenbyvalalign(ARR_ELEMTYPE(arr),
							 &iterator->typlen,
							 &iterator->typbyval,
							 &iterator->typalign);

	/*
	 * Remember the slicing parameters.
	 */
	iterator->slice_ndim = slice_ndim;

	if (slice_ndim > 0)
	{
		/*
		 * Get pointers into the array's dims and lbound arrays to represent
		 * the dims/lbound arrays of a slice.  These are the same as the
		 * rightmost N dimensions of the array.
		 */
		iterator->slice_dims = ARR_DIMS(arr) + ARR_NDIM(arr) - slice_ndim;
		iterator->slice_lbound = ARR_LBOUND(arr) + ARR_NDIM(arr) - slice_ndim;

		/*
		 * Compute number of elements in a slice.
		 */
		iterator->slice_len = ArrayGetNItems(slice_ndim, iterator->slice_dims);

		/*
		 * Create workspace for building sub-arrays.
		 */
		iterator->slice_values = (Datum *) palloc(iterator->slice_len * sizeof(Datum));
		iterator->slice_nulls = (bool *) palloc(iterator->slice_len * sizeof(bool));
	}

	/*
	 * Initialize our data pointer and linear element number.  These will
	 * advance through the array during array_iterate().
	 */
	iterator->data_ptr = ARR_DATA_PTR(arr);
	iterator->current_item = 0;

	return iterator;
}
void
CreateCacheMemoryContext(void)
{
	  /*
  * Purely for paranoia, check that context doesn't exist; caller probably
  * did so already.
  */
 if (u_sess->cache_mem_cxt == nullptr ) {
            u_sess->cache_mem_cxt =
                AllocSetContextCreate(u_sess->top_mem_cxt, "SessionCacheMemoryContext", ALLOCSET_DEFAULT_SIZES);
        }
//   if (!CacheMemoryContext)
//     CacheMemoryContext = AllocSetContextCreate(TopMemoryContext,
//                          "CacheMemoryContext",
//                          ALLOCSET_DEFAULT_SIZES);
}



Oid
RangeVarGetRelidExtended_tsdb(const RangeVar *relation, LOCKMODE lockmode,
            bool missing_ok, bool nowait,
           RangeVarGetRelidCallback_tsdb callback, void *callback_arg,int tsdb)
{
	  uint64   inval_count;
	  Oid     relId;
	  Oid     oldRelId = InvalidOid;
	  bool    retry = false;

	  /*
	  * We check the catalog name and then ignore it.
	  */
  if (relation->catalogname)
 
	{
		    if (strcmp(relation->catalogname, get_database_name(u_sess->proc_cxt.MyDatabaseId)) != 0)
      ereport(ERROR,
          (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
          errmsg("cross-database references are not implemented: \"%s.%s.%s\"",
              relation->catalogname, relation->schemaname,
              relation->relname)));
		 
	}

	  /*
	  * DDL operations can change the results of a name lookup. Since all such
	  * operations will generate invalidation messages, we keep track of
	  * whether any such messages show up while we're performing the operation,
	  * and retry until either (1) no more invalidation messages show up or (2)
	  * the answer doesn't change.
	  *
	  * But if lockmode = NoLock, then we assume that either the caller is OK
	  * with the answer changing under them, or that they already hold some
	  * appropriate lock, and therefore return the first answer we get without
	  * checking for invalidation messages. Also, if the requested lock is
	  * already held, LockRelationOid will not AcceptInvalidationMessages, so
	  * we may fail to notice a change. We could protect against that case by
	  * calling AcceptInvalidationMessages() before beginning this loop, but
	  * that would add a significant amount overhead, so for now we don't.
	  */
  for (;;)
 
	{
		    /*
		    * Remember this value, so that, after looking up the relation name
		    * and locking its OID, we can check whether any invalidation messages
		    * have been processed that might require a do-over.
		    */
    inval_count = u_sess->inval_cxt.SIMCounter;

		    /*
		    * Some non-default relpersistence value may have been specified. The
		    * parser never generates such a RangeVar in simple DML, but it can
		    * happen in contexts such as "CREATE TEMP TABLE foo (f1 int PRIMARY
		    * KEY)". Such a command will generate an added CREATE INDEX
		    * operation, which must be careful to find the temp table, even when
		    * pg_temp is not first in the search path.
		    */
    if (relation->relpersistence == RELPERSISTENCE_TEMP)
   
		{
			      if (!OidIsValid(u_sess->catalog_cxt.myTempNamespace))
        relId = InvalidOid;
			  /* this probably can't happen? */
      else      
			{
				        if (relation->schemaname)
       
				{
					          Oid     namespaceId;

					          namespaceId =
						LookupExplicitNamespace(relation->schemaname, missing_ok);

					          /*
					          * For missing_ok, allow a non-existent schema name to
					          * return InvalidOid.
					          */
          if (namespaceId != u_sess->catalog_cxt.myTempNamespace)
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_TABLE_DEFINITION),
                errmsg("temporary tables cannot specify a schema name")));
					       
				}

				        relId = get_relname_relid(relation->relname, u_sess->catalog_cxt.myTempNamespace);
				     
			}
			   
		}
		    else if (relation->schemaname)
   
		{
			      Oid     namespaceId;

			      /* use exact schema given */
      namespaceId = LookupExplicitNamespace(relation->schemaname, missing_ok);
			      if (missing_ok && !OidIsValid(namespaceId))
        relId = InvalidOid;
			      else         relId =
				get_relname_relid(relation->relname, namespaceId);
			   
		}
		    else    
		{
			      /* search the namespace path */
      relId = RelnameGetRelid(relation->relname);
			   
		}

		    /*
		    * Invoke caller-supplied callback, if any.
		    *
		    * This callback is a good place to check permissions: we haven't
		    * taken the table lock yet (and it's really best to check permissions
		    * before locking anything!), but we've gotten far enough to know what
		    * OID we think we should lock. Of course, concurrent DDL might
		    * change things while we're waiting for the lock, but in that case
		    * the callback will be invoked again for the new OID.
		    */
    if (callback)
      callback(relation, relId, oldRelId, callback_arg);

		    /*
		    * If no lock requested, we assume the caller knows what they're
		    * doing. They should have already acquired a heavyweight lock on
		    * this relation earlier in the processing of this same statement, so
		    * it wouldn't be appropriate to AcceptInvalidationMessages() here, as
		    * that might pull the rug out from under them.
		    */
    if (lockmode == NoLock)
      break;

		    /*
		    * If, upon retry, we get back the same OID we did last time, then the
		    * invalidation messages we processed did not change the final answer.
		    * So we're done.
		    *
		    * If we got a different OID, we've locked the relation that used to
		    * have this name rather than the one that does now. So release the
		    * lock.
		    */
    if (retry)
   
		{
			      if (relId == oldRelId)
        break;
			      if (OidIsValid(oldRelId))
        UnlockRelationOid(oldRelId, lockmode);
			   
		}

		    /*
		    * Lock relation. This will also accept any pending invalidation
		    * messages. If we got back InvalidOid, indicating not found, then
		    * there's nothing to lock, but we accept invalidation messages
		    * anyway, to flush any negative catcache entries that may be
		    * lingering.
		    */
    if (!OidIsValid(relId))
      AcceptInvalidationMessages();
		    else if (!nowait)
      LockRelationOid(relId, lockmode);
		    else if (!ConditionalLockRelationOid(relId, lockmode))
   
		{
			      if (relation->schemaname)
        ereport(ERROR,
            (errcode(ERRCODE_LOCK_NOT_AVAILABLE),
            errmsg("could not obtain lock on relation \"%s.%s\"",
                relation->schemaname, relation->relname)));
			      else
        ereport(ERROR,
            (errcode(ERRCODE_LOCK_NOT_AVAILABLE),
            errmsg("could not obtain lock on relation \"%s\"",
                relation->relname)));
			   
		}

		    /*
		    * If no invalidation message were processed, we're done!
		    */
    if (inval_count == u_sess->inval_cxt.SIMCounter)
      break;

		    /*
		    * Something may have changed. Let's repeat the name lookup, to make
		    * sure this name still references the same relation it did
		    * previously.
		    */
    retry = true;
		    oldRelId = relId;
		 
	}

	  if (!OidIsValid(relId) && !missing_ok)
 
	{
		    if (relation->schemaname)
      ereport(ERROR,
          (errcode(ERRCODE_UNDEFINED_TABLE),
          errmsg(
						  "relation \"%s.%s\" does not exist",
              relation->schemaname, relation->relname)));
		    else       ereport(ERROR,
          (errcode(ERRCODE_UNDEFINED_TABLE),
          errmsg("relation \"%s\" does not exist",
              relation->relname)));
		 
	}
	  return relId;
}

//在og的fmgr.h
// PGFunction
// load_external_function(char *filename, char *funcname, bool signalNotFound, void **filehandle,
// 					   int tsdb = 0)
// {
// 	char *fullname;
// 	void *lib_handle;
// 	PGFunction retval;

// 	/* Expand the possibly-abbreviated filename to an exact path name */
// 	fullname = expand_dynamic_library_name(filename);

// 	/* Load the shared library, unless we already did */
// 	lib_handle = internal_load_library(fullname);

// 	/* Return handle if caller wants it */
// 	if (filehandle)
// 		*filehandle = lib_handle;

// 	/* Look up the function within the library */
// 	retval = (PGFunction) pg_dlsym(lib_handle, funcname);

// 	if (retval == NULL && signalNotFound)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_UNDEFINED_FUNCTION),
// 				 errmsg("could not find function \"%s\" in file \"%s\"", funcname, fullname)));

// 	pfree(fullname);
// 	return retval;
// }

//common\backend\parser\parse_type.cpp 
// TypeName *
// typeStringToTypeName(const char *str)
// {
// 	  StringInfoData buf;
// 	  List    *raw_parsetree_list;
// 	  SelectStmt *stmt;
// 	  ResTarget *restarget;
// 	  TypeCast  *typecast;
// 	  TypeName  *typeName;
// 	  ErrorContextCallback ptserrcontext;

// 	  /* make sure we give useful error for empty input */
//   if (strspn(str, " \t\n\r\f") == strlen(str))
//     goto fail;

// 	  initStringInfo(&buf);
// 	  appendStringInfo(&buf, "SELECT NULL::%s", str);

// 	  /*
// 	  * Setup error traceback support in case of ereport() during parse
// 	  */
//   ptserrcontext.callback = pts_error_callback;
// 	  ptserrcontext.arg = (void *) str;
// 	  ptserrcontext.previous = error_context_stack;
// 	  error_context_stack = &ptserrcontext;

// 	  raw_parsetree_list = raw_parser(buf.data);

// 	  error_context_stack = ptserrcontext.previous;

// 	  /*
// 	  * Make sure we got back exactly what we expected and no more; paranoia is
// 	  * justified since the string might contain anything.
// 	  */
//   if (list_length(raw_parsetree_list) != 1)
//     goto fail;
// 	  stmt = (SelectStmt *) linitial(raw_parsetree_list);
// 	  if (stmt == NULL ||
//     !IsA(stmt, SelectStmt) ||
//     stmt->distinctClause != NIL ||
//     stmt->intoClause != NULL ||
//     stmt->fromClause != NIL ||
//     stmt->whereClause != NULL ||
//     stmt->groupClause != NIL ||
//     stmt->havingClause != NULL ||
//     stmt->windowClause != NIL ||
//     stmt->valuesLists != NIL ||
//     stmt->sortClause != NIL ||
//     stmt->limitOffset != NULL ||
//     stmt->limitCount != NULL ||
//     stmt->lockingClause != NIL ||
//     stmt->withClause != NULL ||
//     stmt->op != SETOP_NONE)
//     goto fail;
// 	  if (list_length(stmt->targetList) != 1)
//     goto fail;
// 	  restarget = (ResTarget *) linitial(stmt->targetList);
// 	  if (restarget == NULL ||
//     !IsA(restarget, ResTarget) ||
//     restarget->name != NULL ||
//     restarget->indirection != NIL)
//     goto fail;
// 	  typecast = (TypeCast *) restarget->val;
// 	  if (typecast == NULL ||
//     !IsA(typecast, TypeCast) ||
//     typecast->arg == NULL ||
//     !IsA(typecast->arg, A_Const))
//     goto fail;

// 	  typeName = typecast->typname;
// 	  if (typeName == NULL ||
//     !IsA(typeName, TypeName))
//     goto fail;
// 	  if (typeName->setof)
//     goto fail;

// 	  pfree(buf.data);

// 	  return typeName;

// fail:
// 	  ereport(ERROR,
//       (errcode(ERRCODE_SYNTAX_ERROR),
//       errmsg("invalid type name \"%s\"", str)));
// 	  return NULL;
// 	       /* keep compiler quiet */
// }



AttrNumber *
convert_tuples_by_name_map(TupleDesc indesc,
             TupleDesc outdesc,
             const char *msg)
{
	  AttrNumber *attrMap;
	  int     n;
	  int     i;

	  n = outdesc->natts;
	  attrMap = (AttrNumber *) palloc0(n * sizeof(AttrNumber));
	  for (i = 0; i < n; i++)
 
	{
		    Form_pg_attribute att = outdesc->attrs[i];
		    char    *attname;
		    Oid     atttypid;
		    int32    atttypmod;
		    int     j;

		    if (att->attisdropped)
      continue;
		     /* attrMap[i] is already 0 */
    attname = NameStr(att->attname);
		    atttypid = att->atttypid;
		    atttypmod = att->atttypmod;
		    for (j = 0; j < indesc->natts; j++)
   
		{
			      att = indesc->attrs[j];
			      if (att->attisdropped)
        continue;
			      if (strcmp(attname, NameStr(att->attname)) == 0)
     
			{
				        /* Found it, check type */
        if (atttypid != att->atttypid || atttypmod != att->atttypmod)
          ereport(
					ERROR,
              (
						errcode(ERRCODE_DATATYPE_MISMATCH),
              errmsg_internal("%s", _(msg)),
              errdetail(
							"Attribute \"%s\" of type %s does not match corresponding attribute of type %s.",
                   attname,
                   format_type_be(outdesc->tdtypeid),
                   format_type_be(indesc->tdtypeid))));
				        attrMap[i] = (AttrNumber) (j + 1);
				        break;
				     
			}
			   
		}
		    if (attrMap[i] == 0)
      ereport(ERROR,
          (errcode(ERRCODE_DATATYPE_MISMATCH),
          errmsg_internal("%s", _(msg)),
          errdetail("Attribute \"%s\" of type %s does not exist in type %s.",
               attname,
               format_type_be(outdesc->tdtypeid),
               format_type_be(indesc->tdtypeid))));
		 
	}

	  return attrMap;
}

int
check_enable_rls(Oid relid, Oid checkAsUser, bool noError)
{
	  Oid     user_id = checkAsUser ? checkAsUser : GetUserId();
	  HeapTuple  tuple;
	  Form_pg_class classform;
	  bool    relrowsecurity;
	  bool    relforcerowsecurity;
	  bool    amowner;

	  /* Nothing to do for built-in relations */
  if (relid < (Oid) FirstNormalObjectId)
    return RLS_NONE;

	  /* Fetch relation's relrowsecurity and relforcerowsecurity flags */
  tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(relid));
	  if (!HeapTupleIsValid(tuple))
    return RLS_NONE;
	  classform = (Form_pg_class) GETSTRUCT(tuple);

	  relrowsecurity = true;
	  relforcerowsecurity = true;

	  ReleaseSysCache(tuple);

	  /* Nothing to do if the relation does not have RLS */
  if (!relrowsecurity)
    return RLS_NONE;

	  /*
	  * BYPASSRLS users always bypass RLS. Note that superusers are always
	  * considered to have BYPASSRLS.
	  *
	  * Return RLS_NONE_ENV to indicate that this decision depends on the
	  * environment (in this case, the user_id).
	  */
  if (has_bypassrls_privilege(user_id))
    return RLS_NONE_ENV;

	  /*
	  * Table owners generally bypass RLS, except if the table has been set (by
	  * an owner) to FORCE ROW SECURITY, and this is not a referential
	  * integrity check.
	  *
	  * Return RLS_NONE_ENV to indicate that this decision depends on the
	  * environment (in this case, the user_id).
	  */
  amowner = pg_class_ownercheck(relid, user_id);
	  if (amowner)
 
	{
		    /*
		    * If FORCE ROW LEVEL SECURITY has been set on the relation then we
		    * should return RLS_ENABLED to indicate that RLS should be applied.
		    * If not, or if we are in an InNoForceRLSOperation context, we return
		    * RLS_NONE_ENV.
		    *
		    * InNoForceRLSOperation indicates that we should not apply RLS even
		    * if the table has FORCE RLS set - IF the current user is the owner.
		    * This is specifically to ensure that referential integrity checks
		    * are able to still run correctly.
		    *
		    * This is intentionally only done after we have checked that the user
		    * is the table owner, which should always be the case for referential
		    * integrity checks.
		    */
    if (!relforcerowsecurity || InNoForceRLSOperation())
      return RLS_NONE_ENV;
		 
	}

	  /*
  * We should apply RLS. However, the user may turn off the row_security
  * GUC to get a forced error instead.
  */
  if (!row_security && !noError)
    ereport(ERROR,
        (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
        errmsg("query would be affected by row-level security policy for table \"%s\"",
            get_rel_name(relid)),
        amowner ? errhint("To disable the policy for the table's owner, use ALTER TABLE NO FORCE ROW LEVEL SECURITY.") : 0));

	  /* RLS should be fully enabled for this relation. */
  return RLS_ENABLED;
}

//gausskernel/optimizer/commands/trigger.cpp
// void
// ExecARInsertTriggers_tsdb(EState *estate, ResultRelInfo *relinfo,
//           HeapTuple trigtuple, List *recheckIndexes)
// {
// 	  TriggerDesc *trigdesc = relinfo->ri_TrigDesc;

// 	  if (trigdesc && trigdesc->trig_insert_after_row)
//     AfterTriggerSaveEvent(estate, relinfo, TRIGGER_EVENT_INSERT,
//                true, NULL, trigtuple, recheckIndexes, NULL);
// }

void
PreventCommandIfParallelMode(const char *cmdname)
{
	  if (IsInParallelMode())
    ereport(
		ERROR,
        (
			errcode(ERRCODE_INVALID_TRANSACTION_STATE),
    /* translator: %s is name of a SQL command, eg CREATE */
        errmsg(
				"cannot execute %s during a parallel operation",
            cmdname)));
}



// Expr *
// ts_transform_cross_datatype_comparison(Expr *clause)
// {
// 	clause =(Expr *) copyObject(clause);
// 	if (IsA(clause, OpExpr) && list_length(castNode(OpExpr, clause)->args) == 2)
// 	{
// 		OpExpr *op = castNode(OpExpr, clause);
// 		Oid left_type = exprType((const Node *)linitial(op->args));
// 		Oid right_type = exprType((const Node *)lsecond(op->args));

// 		if (op->opresulttype != BOOLOID || op->opretset == true)
// 			return clause;

// 		if (!IsA(linitial(op->args), Var) && !IsA(lsecond(op->args), Var))
// 			return clause;

// 		if (DATATYPE_PAIR(left_type, right_type, TIMESTAMPOID, TIMESTAMPTZOID) ||
// 			DATATYPE_PAIR(left_type, right_type, TIMESTAMPTZOID, DATEOID))
// 		{
// 			char *opname = get_opname(op->opno);
// 			Oid source_type, target_type, opno, cast_oid;

// 			/*
// 			 * if Var is on left side we put cast on right side otherwise
// 			 * it will be left
// 			 */
// 			if (IsA(linitial(op->args), Var))
// 			{
// 				source_type = right_type;
// 				target_type = left_type;
// 			}
// 			else
// 			{
// 				source_type = left_type;
// 				target_type = right_type;
// 			}

// 			opno = ts_get_operator(opname, PG_CATALOG_NAMESPACE, target_type, target_type);
// 			cast_oid = ts_get_cast_func(source_type, target_type);

// 			if (OidIsValid(opno) && OidIsValid(cast_oid))
// 			{
// 				Expr *left =(Expr *) linitial(op->args);
// 				Expr *right =(Expr *) lsecond(op->args);

// 				if (source_type == left_type)
// 					left = (Expr *) makeFuncExpr(cast_oid,
// 												 target_type,
// 												 list_make1(left),
// 												 InvalidOid,
// 												 InvalidOid,
// 												 (CoercionForm)0);
// 				else
// 					right = (Expr *) makeFuncExpr(cast_oid,
// 												  target_type,
// 												  list_make1(right),
// 												  InvalidOid,
// 												  InvalidOid,
// 												  (CoercionForm)0);

// 				clause = make_opclause(opno, BOOLOID, false, left, right, InvalidOid, InvalidOid);
// 			}
// 		}
// 	}
// 	return clause;
// }



List *
ExecInsertIndexTuples(TupleTableSlot *slot,
           ItemPointer tupleid,
           EState *estate,
           bool noDupErr,
           bool *specConflict,
           List *arbiterIndexes)
{
	  List    *result = NIL;
	  ResultRelInfo *resultRelInfo;
	  int     i;
	  int     numIndices;
	  RelationPtr relationDescs;
	  Relation  heapRelation;
	  IndexInfo **indexInfoArray;
	  ExprContext *econtext;
	  Datum    values[INDEX_MAX_KEYS];
	  bool    isnull[INDEX_MAX_KEYS];

	  /*
	  * Get information from the result relation info structure.
	  */
  resultRelInfo = estate->es_result_relation_info;
	  numIndices = resultRelInfo->ri_NumIndices;
	  relationDescs = resultRelInfo->ri_IndexRelationDescs;
	  indexInfoArray = resultRelInfo->ri_IndexRelationInfo;
	  heapRelation = resultRelInfo->ri_RelationDesc;

	  /*
	  * We will use the EState's per-tuple context for evaluating predicates
	  * and index expressions (creating it if it's not already there).
	  */
  econtext = GetPerTupleExprContext(estate);

	  /* Arrange for econtext's scan tuple to be the tuple under test */
  econtext->ecxt_scantuple = slot;

	  /*
	  * for each index, form and insert the index tuple
	  */
  for (i = 0; i < numIndices; i++)
 
	{
		    Relation  indexRelation = relationDescs[i];
		    IndexInfo *indexInfo;
		    bool    applyNoDupErr;
		    IndexUniqueCheck checkUnique;
		    bool    satisfiesConstraint;

		    if (indexRelation == NULL)
      continue;

		    indexInfo = indexInfoArray[i];

		    /* If the index is marked as read-only, ignore it */
    if (!indexInfo->ii_ReadyForInserts)
      continue;

		    /* Check for partial index */
    if (indexInfo->ii_Predicate != NIL)
   
		{
			      List    *predicate;

			      /*
			      * If predicate state not set up yet, create it (in the estate's
			      * per-query context)
			      */
      predicate = indexInfo->ii_PredicateState;
			      if (predicate == NIL)
     
			{
				        predicate = (List *)
          ExecPrepareExpr((Expr *) indexInfo->ii_Predicate,
                  estate);
				        indexInfo->ii_PredicateState = predicate;
				     
			}

			      /* Skip this index-update if the predicate isn't satisfied */
      if (!ExecQual(predicate, econtext, false))
        continue;
			   
		}

		    /*
		    * FormIndexDatum fills in its values and isnull parameters with the
		    * appropriate values for the column(s) of the index.
		    */
    FormIndexDatum(indexInfo,
           slot,
           estate,
           values,
           isnull);

		    /* Check whether to apply noDupErr to this index */
    applyNoDupErr = noDupErr &&
      (arbiterIndexes == NIL ||
      list_member_oid(arbiterIndexes,
              indexRelation->rd_index->indexrelid));

		    /*
		    * The index AM does the actual insertion, plus uniqueness checking.
		    *
		    * For an immediate-mode unique index, we just tell the index AM to
		    * throw error if not unique.
		    *
		    * For a deferrable unique index, we tell the index AM to just detect
		    * possible non-uniqueness, and we add the index OID to the result
		    * list if further checking is needed.
		    *
		    * For a speculative insertion (used by INSERT ... ON CONFLICT), do
		    * the same as for a deferrable unique index.
		    */
    if (!indexRelation->rd_index->indisunique)
      checkUnique = UNIQUE_CHECK_NO;
		    else if (applyNoDupErr)
      checkUnique = UNIQUE_CHECK_PARTIAL;
		    else if (indexRelation->rd_index->indimmediate)
      checkUnique = UNIQUE_CHECK_YES;
		    else       checkUnique = UNIQUE_CHECK_PARTIAL;

		    satisfiesConstraint =
      index_insert(indexRelation, /* index relation */
            values,  /* array of index Datums */
            isnull,  /* null flags */
            tupleid,    /* tid of heap tuple */
            heapRelation, /* heap relation */
            checkUnique);
		/* type of uniqueness check to do */

    /*
     * If the index has an associated exclusion constraint, check that.
     * This is simpler than the process for uniqueness checks since we
     * always insert first and then check. If the constraint is deferred,
     * we check now anyway, but don't throw error on violation or wait for
     * a conclusive outcome from a concurrent insertion; instead we'll
     * queue a recheck event. Similarly, noDupErr callers (speculative
     * inserters) will recheck later, and wait for a conclusive outcome
     * then.
     *
     * An index for an exclusion constraint can't also be UNIQUE (not an
     * essential property, we just don't allow it in the grammar), so no
     * need to preserve the prior state of satisfiesConstraint.
     */
    if (indexInfo->ii_ExclusionOps != NULL)
   
		{
			      bool    violationOK;
			      CEOUC_WAIT_MODE waitMode;

			      if (applyNoDupErr)
     
			{
				        violationOK = true;
				        waitMode = CEOUC_LIVELOCK_PREVENTING_WAIT;
				     
			}
			      else if (!indexRelation->rd_index->indimmediate)
     
			{
				        violationOK = true;
				        waitMode = CEOUC_NOWAIT;
				     
			}
			      else      
			{
				        violationOK = false;
				        waitMode = CEOUC_WAIT;
				     
			}

			      satisfiesConstraint =
        check_exclusion_or_unique_constraint(heapRelation,
                          indexRelation, indexInfo,
                          tupleid, values, isnull,
                          estate, false,
                        waitMode, violationOK, NULL);
			   
		}

		    if ((checkUnique == UNIQUE_CHECK_PARTIAL ||
      indexInfo->ii_ExclusionOps != NULL) &&
      !satisfiesConstraint)
   
		{
			      /*
			      * The tuple potentially violates the uniqueness or exclusion
			      * constraint, so make a note of the index so that we can re-check
			      * it later. Speculative inserters are told if there was a
			      * speculative conflict, since that always requires a restart.
			      */
      result = lappend_oid(result, RelationGetRelid(indexRelation));
			      if (indexRelation->rd_index->indimmediate && specConflict)
        *specConflict = true;
			   
		}
		 
	}

	  return result;
}

void
TerminateBackgroundWorker(BackgroundWorkerHandle *handle)
{
	  BackgroundWorkerSlot *slot;
	  bool    signal_postmaster = false;

	  Assert(handle->slot < g_max_worker_processes);
	  slot = &BackgroundWorkerData->slot[handle->slot];

	  /* Set terminate flag in shared memory, unless slot has been reused. */
  LWLockAcquire(BackgroundWorkerLock, LW_EXCLUSIVE);
	  if (handle->generation == slot->generation)
 
	{
		    slot->terminate = true;
		    signal_postmaster = true;
		 
	}
	  LWLockRelease(BackgroundWorkerLock);

	  /* Make sure the postmaster notices the change to shared memory. */
  if (signal_postmaster)
    SendPostmasterSignal((PMSignalReason)6);//tsdb PMSIGNAL_BACKGROUND_WORKER_CHANGE是6
}

BgwHandleStatus
GetBackgroundWorkerPid(BackgroundWorkerHandle *handle, pid_t *pidp)
{
	  BackgroundWorkerSlot *slot;
	  pid_t    pid;

	  Assert(handle->slot < g_max_worker_processes);
	  slot = &BackgroundWorkerData->slot[handle->slot];

	  /*
	  * We could probably arrange to synchronize access to data using memory
	  * barriers only, but for now, let's just keep it simple and grab the
	  * lock. It seems unlikely that there will be enough traffic here to
	  * result in meaningful contention.
	  */
  LWLockAcquire(BackgroundWorkerLock, LW_SHARED);

	  /*
	  * The generation number can't be concurrently changed while we hold the
	  * lock. The pid, which is updated by the postmaster, can change at any
	  * time, but we assume such changes are atomic. So the value we read
	  * won't be garbage, but it might be out of date by the time the caller
	  * examines it (but that's unavoidable anyway).
	  */
  if (handle->generation != slot->generation)
    pid = 0;
	  else     pid = slot->pid;

	  /* All done. */
  LWLockRelease(BackgroundWorkerLock);

	  if (pid == 0)
    return BGWH_STOPPED;
	  else if (pid == InvalidPid)
    return BGWH_NOT_YET_STARTED;
	  *pidp = pid;
	  return BGWH_STARTED;
}

BgwHandleStatus
WaitForBackgroundWorkerShutdown(BackgroundWorkerHandle *handle)
{
	  BgwHandleStatus status;
	  int     rc;

	  for (;;)
 
	{
		    pid_t    pid;

		    CHECK_FOR_INTERRUPTS();

		    status = GetBackgroundWorkerPid(handle, &pid);
		    if (status == BGWH_STOPPED)
      break;

		    rc = WaitLatch(&t_thrd.proc->procLatch,
           WL_LATCH_SET | WL_POSTMASTER_DEATH, 0);

		    if (rc & WL_POSTMASTER_DEATH)
   
		{
			      status = BGWH_POSTMASTER_DIED;
			      break;
			   
		}

		    ResetLatch(&t_thrd.proc->procLatch);
		 
	}

	  return status;
}

//gausskernel\optimizer\commands\explain.cpp 
// void
// ExplainPropertyBool(const char *qlabel, bool value, ExplainState *es)
// {
// 	  ExplainProperty(qlabel, value ? "true" : "false", true, es);
// }

void
CacheRegisterRelcacheCallback(RelcacheCallbackFunction func, Datum arg)
{
	knl_u_inval_context *inval_cxt = &t_thrd.lsc_cxt.lsc->inval_cxt;
	if (inval_cxt->relcache_callback_count >= MAX_RELCACHE_CALLBACKS)
		elog(FATAL, "out of relcache_callback_list slots");

	inval_cxt->relcache_callback_list[inval_cxt->relcache_callback_count].function = func;
	inval_cxt->relcache_callback_list[inval_cxt->relcache_callback_count].arg = arg;

	++inval_cxt->relcache_callback_count;
}





void
get_typlenbyval(Oid typid, int16 *typlen, bool *typbyval)
{
	HeapTuple tp;
	Form_pg_type typtup;

	tp = SearchSysCache1(TYPEOID, ObjectIdGetDatum(typid));
	if (!HeapTupleIsValid(tp))
		elog(ERROR, "cache lookup failed for type %u", typid);
	typtup = (Form_pg_type) GETSTRUCT(tp);
	*typlen = typtup->typlen;
	*typbyval = typtup->typbyval;
	ReleaseSysCache(tp);
}

void
mark_partial_aggref(Aggref *agg, AggSplit aggsplit)
{
	/* aggtranstype should be computed by this point */
	Assert(OidIsValid(agg->aggtranstype));
	/* ... but aggsplit should still be as the parser left it */
	Assert(agg->aggsplit == AGGSPLIT_SIMPLE);

	/* Mark the Aggref with the intended partial-aggregation mode */
	agg->aggsplit = aggsplit;

	/*
	 * Adjust result type if needed.  Normally, a partial aggregate returns
	 * the aggregate's transition type; but if that's INTERNAL and we're
	 * serializing, it returns BYTEA instead.
	 */
	if (DO_AGGSPLIT_SKIPFINAL(aggsplit))
	{
		if (agg->aggtranstype == INTERNALOID && DO_AGGSPLIT_SERIALIZE(aggsplit))
			agg->aggtype = BYTEAOID;
		else
			agg->aggtype = agg->aggtranstype;
	}
}

void
add_new_columns_to_pathtarget(PathTarget *target, List *exprs)
{
	ListCell *lc;

	foreach (lc, exprs)
	{
		Expr *expr = (Expr *) lfirst(lc);

		add_new_column_to_pathtarget(target, expr);
	}
}

void
add_column_to_pathtarget(PathTarget *target, Expr *expr, Index sortgroupref)
{
	/* Updating the exprs list is easy ... */
	target->exprs = lappend(target->exprs, expr);
	/* ... the sortgroupref data, a bit less so */
	if (target->sortgrouprefs)
	{
		int nexprs = list_length(target->exprs);

		/* This might look inefficient, but actually it's usually cheap */
		target->sortgrouprefs = (Index *) repalloc(target->sortgrouprefs, nexprs * sizeof(Index));
		target->sortgrouprefs[nexprs - 1] = sortgroupref;
	}
	else if (sortgroupref)
	{
		/* Adding sortgroupref labeling to a previously unlabeled target */
		int nexprs = list_length(target->exprs);

		target->sortgrouprefs = (Index *) palloc0(nexprs * sizeof(Index));
		target->sortgrouprefs[nexprs - 1] = sortgroupref;
	}
}

SortGroupClause *
get_sortgroupref_clause_noerr(Index sortref, List *clauses)
{
	ListCell *l;

	foreach (l, clauses)
	{
		SortGroupClause *cl = (SortGroupClause *) lfirst(l);

		if (cl->tleSortGroupRef == sortref)
			return cl;
	}

	return NULL;
}

PathTarget *
create_empty_pathtarget(void)
{
	/* This is easy, but we don't want callers to hard-wire this ... */
	return makeNode(PathTarget);
}

void
slot_getallattrs(TupleTableSlot *slot)
{
	slot_getsomeattrs(slot, slot->tts_tupleDescriptor->natts);
}


// HeapTuple
// index_getnext(IndexScanDesc scan, ScanDirection direction, int tsdb = 0)
// {
// 	HeapTuple heapTuple;
// 	ItemPointer tid;

// 	for (;;)
// 	{
// 		if (scan->xs_continue_hot)
// 		{
// 			/*
// 			 * We are resuming scan of a HOT chain after having returned an
// 			 * earlier member.  Must still hold pin on current heap page.
// 			 */
// 			Assert(BufferIsValid(scan->xs_cbuf));
// 			Assert(ItemPointerGetBlockNumber(&scan->xs_ctup.t_self) ==
// 				   BufferGetBlockNumber(scan->xs_cbuf));
// 		}
// 		else
// 		{
// 			/* Time to fetch the next TID from the index */
// 			tid = index_getnext_tid(scan, direction);

// 			/* If we're out of index entries, we're done */
// 			if (tid == NULL)
// 				break;
// 		}

// 		/*
// 		 * Fetch the next (or only) visible heap tuple for this index entry.
// 		 * If we don't find anything, loop around and grab the next TID from
// 		 * the index.
// 		 */
// 		heapTuple = index_fetch_heap(scan);
// 		if (heapTuple != NULL)
// 			return heapTuple;
// 	}

// 	return NULL; /* failure exit */
// }

char *
psprintf(const char *fmt, ...)
{
	size_t len = 128; /* initial assumption about buffer size */

	for (;;)
	{
		char *result;
		va_list args;
		size_t newlen;

		/*
		 * Allocate result buffer.  Note that in frontend this maps to malloc
		 * with exit-on-error.
		 */
		result = (char *) palloc(len);

		/* Try to format the data. */
		va_start(args, fmt);
		newlen = pvsnprintf(result, len, fmt, args);
		va_end(args);

		if (newlen < len)
			return result; /* success */

		/* Release buffer and loop around to try again with larger len. */
		pfree(result);
		len = newlen;
	}
}
// void
// NewRelationCreateToastTable(Oid relOid, Datum reloptions)
// {
// 	CheckAndCreateToastTable(relOid, reloptions, AccessExclusiveLock, false);
// }


//先不重载尝试一下
// ObjectAddress
// DefineRelation(CreateStmt *stmt, char relkind, Oid ownerId, ObjectAddress *typaddress)
// {
// 	char relname[NAMEDATALEN];
// 	Oid namespaceId;
// 	List *schema = stmt->tableElts;
// 	Oid relationId;
// 	Oid tablespaceId;
// 	Relation rel;
// 	TupleDesc descriptor;
// 	List *inheritOids;
// 	List *old_constraints;
// 	bool localHasOids;
// 	int parentOidCount;
// 	List *rawDefaults;
// 	List *cookedDefaults;
// 	Datum reloptions;
// 	ListCell *listptr;
// 	AttrNumber attnum;
// 	static char *validnsps[] = HEAP_RELOPT_NAMESPACES;
// 	Oid ofTypeId;
// 	ObjectAddress address;

// 	/*
// 	 * Truncate relname to appropriate length (probably a waste of time, as
// 	 * parser should have done this already).
// 	 */
// 	StrNCpy(relname, stmt->relation->relname, NAMEDATALEN);

// 	/*
// 	 * Check consistency of arguments
// 	 */
// 	if (stmt->oncommit != ONCOMMIT_NOOP && stmt->relation->relpersistence != RELPERSISTENCE_TEMP)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INVALID_TABLE_DEFINITION),
// 				 errmsg("ON COMMIT can only be used on temporary tables")));

// 	/*
// 	 * Look up the namespace in which we are supposed to create the relation,
// 	 * check we have permission to create there, lock it against concurrent
// 	 * drop, and mark stmt->relation as RELPERSISTENCE_TEMP if a temporary
// 	 * namespace is selected.
// 	 */
// 	namespaceId = RangeVarGetAndCheckCreationNamespace(stmt->relation, NoLock, NULL);

// 	/*
// 	 * Security check: disallow creating temp tables from security-restricted
// 	 * code.  This is needed because calling code might not expect untrusted
// 	 * tables to appear in pg_temp at the front of its search path.
// 	 */
// 	if (stmt->relation->relpersistence == RELPERSISTENCE_TEMP && InSecurityRestrictedOperation())
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
// 		AclResult aclresult;

// 		aclresult = pg_tablespace_aclcheck(tablespaceId, GetUserId(), ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			aclcheck_error(aclresult, ACL_KIND_TABLESPACE, get_tablespace_name(tablespaceId));
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
// 	reloptions = transformRelOptions((Datum) 0, stmt->options, NULL, validnsps, true, false);

// 	if (relkind == RELKIND_VIEW)
// 		(void) view_reloptions(reloptions, true);
// 	else
// 		(void) heap_reloptions(relkind, reloptions, true);

// 	if (stmt->ofTypename)
// 	{
// 		AclResult aclresult;

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
// 	schema = MergeAttributes(schema,
// 							 stmt->inhRelations,
// 							 stmt->relation->relpersistence,
// 							 &inheritOids,
// 							 &old_constraints,
// 							 &parentOidCount);

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
// 	localHasOids = interpretOidsOption(stmt->options, (relkind == RELKIND_RELATION));
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

// 	foreach (listptr, schema)
// 	{
// 		ColumnDef *colDef =(ColumnDef *) lfirst(listptr);

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
// 			cooked->conoid = InvalidOid; /* until created */
// 			cooked->name = NULL;
// 			cooked->attnum = attnum;
// 			cooked->expr = colDef->cooked_default;
// 			cooked->skip_validation = false;
// 			cooked->is_local = true; /* not used for defaults */
// 			cooked->inhcount = 0;	 /* ditto */
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
// 										  list_concat(cookedDefaults, old_constraints),
// 										  relkind,
// 										  stmt->relation->relpersistence,
// 										  false,
// 										  false,
// 										  localHasOids,
// 										  parentOidCount,
// 										  stmt->oncommit,
// 										  reloptions,
// 										  true,
// 										  og_knl3->allowSystemTableMods,
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
// 		AddRelationNewConstraints(rel, rawDefaults, stmt->constraints, true, true, false);

// 	ObjectAddressSet(address, RelationRelationId, relationId);

// 	/*
// 	 * Clean up.  We keep lock on new relation (although it shouldn't be
// 	 * visible to anyone else anyway, until commit).
// 	 */
// 	relation_close(rel, NoLock);

// 	return address;
// }


List *
RelationGetFKeyList(Relation relation)
{
	List *result;
	Relation conrel;
	SysScanDesc conscan;
	ScanKeyData skey;
	HeapTuple htup;
	List *oldlist;
	MemoryContext oldcxt;

	/* Quick exit if we already computed the list. */
    //tsdb 需要在RelationData加入成员rd_fkeyvalid,rd_fkeylist
	// if (relation->rd_fkeyvalid)
	// 	return relation->rd_fkeylist;

	/* Fast path: if it doesn't have any triggers, it can't have FKs */
	if (!relation->rd_rel->relhastriggers)
		return NIL;

	/*
	 * We build the list we intend to return (in the caller's context) while
	 * doing the scan.  After successfully completing the scan, we copy that
	 * list into the relcache entry.  This avoids cache-context memory leakage
	 * if we get some sort of error partway through.
	 */
	result = NIL;

	/* Prepare to scan pg_constraint for entries having conrelid = this rel. */
	ScanKeyInit(&skey,
				Anum_pg_constraint_conrelid,
				BTEqualStrategyNumber,
				F_OIDEQ,
				ObjectIdGetDatum(RelationGetRelid(relation)));

	conrel = heap_open(ConstraintRelationId, AccessShareLock);
	conscan = systable_beginscan(conrel, ConstraintRelidIndexId, true, NULL, 1, &skey);

	while (HeapTupleIsValid(htup = systable_getnext(conscan)))
	{
		Form_pg_constraint constraint = (Form_pg_constraint) GETSTRUCT(htup);
		ForeignKeyCacheInfo *info;
		Datum adatum;
		bool isnull;
		ArrayType *arr;
		int nelem;

		/* consider only foreign keys */
		if (constraint->contype != CONSTRAINT_FOREIGN)
			continue;

		info = makeNode(ForeignKeyCacheInfo);
		info->conrelid = constraint->conrelid;
		info->confrelid = constraint->confrelid;

		/* Extract data from conkey field */
		adatum = fastgetattr(htup, Anum_pg_constraint_conkey, conrel->rd_att, &isnull);
		if (isnull)
			elog(ERROR, "null conkey for rel %s", RelationGetRelationName(relation));

		arr = DatumGetArrayTypeP(adatum); /* ensure not toasted */
		nelem = ARR_DIMS(arr)[0];
		if (ARR_NDIM(arr) != 1 || nelem < 1 || nelem > INDEX_MAX_KEYS || ARR_HASNULL(arr) ||
			ARR_ELEMTYPE(arr) != INT2OID)
			elog(ERROR, "conkey is not a 1-D smallint array");

		info->nkeys = nelem;
		memcpy(info->conkey, ARR_DATA_PTR(arr), nelem * sizeof(AttrNumber));

		/* Likewise for confkey */
		adatum = fastgetattr(htup, Anum_pg_constraint_confkey, conrel->rd_att, &isnull);
		if (isnull)
			elog(ERROR, "null confkey for rel %s", RelationGetRelationName(relation));

		arr = DatumGetArrayTypeP(adatum); /* ensure not toasted */
		nelem = ARR_DIMS(arr)[0];
		if (ARR_NDIM(arr) != 1 || nelem != info->nkeys || ARR_HASNULL(arr) ||
			ARR_ELEMTYPE(arr) != INT2OID)
			elog(ERROR, "confkey is not a 1-D smallint array");

		memcpy(info->confkey, ARR_DATA_PTR(arr), nelem * sizeof(AttrNumber));

		/* Likewise for conpfeqop */
		adatum = fastgetattr(htup, Anum_pg_constraint_conpfeqop, conrel->rd_att, &isnull);
		if (isnull)
			elog(ERROR, "null conpfeqop for rel %s", RelationGetRelationName(relation));

		arr = DatumGetArrayTypeP(adatum); /* ensure not toasted */
		nelem = ARR_DIMS(arr)[0];
		if (ARR_NDIM(arr) != 1 || nelem != info->nkeys || ARR_HASNULL(arr) ||
			ARR_ELEMTYPE(arr) != OIDOID)
			elog(ERROR, "conpfeqop is not a 1-D OID array");

		memcpy(info->conpfeqop, ARR_DATA_PTR(arr), nelem * sizeof(Oid));

		/* Add FK's node to the result list */
		result = lappend(result, info);
	}

	systable_endscan(conscan);
	heap_close(conrel, AccessShareLock);

	/* Now save a copy of the completed list in the relcache entry. */
	oldcxt = MemoryContextSwitchTo(u_sess->cache_mem_cxt);
	oldlist = NIL;//relation->rd_fkeylist;
	// relation->rd_fkeylist =(List*) copyObject(result);
	// relation->rd_fkeyvalid = true;
	MemoryContextSwitchTo(oldcxt);

	/* Don't leak the old list, if there is one */
	list_free_deep(oldlist);

	return result;
}

//access/heapam.h
// HeapScanDesc
// heap_beginscan(Relation relation, Snapshot snapshot, int nkeys, ScanKey key)
// {
// 	return heap_beginscan_internal(relation,
// 								   snapshot,
// 								   nkeys,
// 								   key,
// 								   NULL,
// 								   true,
// 								   true,
// 								   true,
// 								   false,
// 								   false,
// 								   false);
// }

//gausskernel\storage\access\common\heaptuple.cpp 
// Datum
// slot_getattr(TupleTableSlot *slot, int attnum, bool *isnull)
// {
// 	HeapTuple tuple = slot->tts_tuple;
// 	TupleDesc tupleDesc = slot->tts_tupleDescriptor;
// 	HeapTupleHeader tup;

// 	/*
// 	 * system attributes are handled by heap_getsysattr
// 	 */
// 	if (attnum <= 0)
// 	{
// 		if (tuple == NULL) /* internal error */
// 			elog(ERROR, "cannot extract system attribute from virtual tuple");
// 		if (tuple == &(slot->tts_minhdr)) /* internal error */
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
// 	if (tuple == NULL) /* internal error */
// 		elog(ERROR, "cannot extract attribute from empty tuple slot");

// 	/*
// 	 * return NULL if attnum is out of range according to the tuple
// 	 *
// 	 * (We have to check this separately because of various inheritance and
// 	 * table-alteration scenarios: the tuple could be either longer or shorter
// 	 * than the tupdesc.)
// 	 */
// 	tup = tuple->t_data;
// 	if (attnum > HeapTupleHeaderGetNatts_tsdb(tup))
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

//"access/heapam.h"
// HTSU_Result
// heap_lock_tuple(Relation relation, HeapTuple tuple, CommandId cid, LockTupleMode mode,
// 				LockWaitPolicy wait_policy, bool follow_updates, Buffer *buffer,
// 				HeapUpdateFailureData *hufd)
// {
// 	HTSU_Result result;
// 	ItemPointer tid = &(tuple->t_self);
// 	ItemId lp;
// 	Page page;
// 	Buffer vmbuffer = InvalidBuffer;
// 	BlockNumber block;
// 	TransactionId xid, xmax;
// 	uint16 old_infomask, new_infomask, new_infomask2;
// 	bool first_time = true;
// 	bool have_tuple_lock = false;
// 	bool cleared_all_frozen = false;

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
// 	result =(HTSU_Result) HeapTupleSatisfiesUpdate(tuple, cid, *buffer);

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
// 		uint16 infomask;
// 		uint16 infomask2;
// 		bool require_sleep;
// 		ItemPointerData t_ctid;

// 		/* must copy state data before unlocking buffer */
// 		xwait = HeapTupleHeaderGetRawXmax_tsdb(tuple->t_data);
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
// 				int i;
// 				int nmembers;
// 				MultiXactMember *members;

// 				/*
// 				 * We don't need to allow old multixacts here; if that had
// 				 * been the case, HeapTupleSatisfiesUpdate would have returned
// 				 * MayBeUpdated and we wouldn't be here.
// 				 */
// 				nmembers = GetMultiXactIdMembers(xwait,
// 												 &members,
// 												 false,
// 												 HEAP_XMAX_IS_LOCKED_ONLY_tsdb(infomask));

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
// 						if (HEAP_XMAX_IS_SHR_LOCKED(infomask) || HEAP_XMAX_IS_EXCL_LOCKED(infomask))
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
// 						if (HEAP_XMAX_IS_EXCL_LOCKED(infomask) && infomask2 & HEAP_KEYS_UPDATED)
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
// 				bool updated;

// 				updated = !HEAP_XMAX_IS_LOCKED_ONLY_tsdb(infomask);

// 				/*
// 				 * If there are updates, follow the update chain; bail out if
// 				 * that cannot be done.
// 				 */
// 				if (follow_updates && updated)
// 				{
// 					HTSU_Result res;

// 					res = heap_lock_updated_tuple(relation,
// 												  tuple,
// 												  &t_ctid,
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
// 					((tuple->t_data->t_infomask2 & HEAP_KEYS_UPDATED) || !updated))
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
// 			if (HEAP_XMAX_IS_LOCKED_ONLY_tsdb(infomask) && !HEAP_XMAX_IS_EXCL_LOCKED(infomask))
// 			{
// 				LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);

// 				/*
// 				 * Make sure it's still an appropriate lock, else start over.
// 				 * See above about allowing xmax to change.
// 				 */
// 				if (!HEAP_XMAX_IS_LOCKED_ONLY_tsdb(tuple->t_data->t_infomask) ||
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
// 				if (!DoesMultiXactIdConflict((MultiXactId) xwait, infomask, mode))
// 				{
// 					/*
// 					 * No conflict, but if the xmax changed under us in the
// 					 * meantime, start over.
// 					 */
// 					LockBuffer(*buffer, BUFFER_LOCK_EXCLUSIVE);
// 					if (xmax_infomask_changed(tuple->t_data->t_infomask, infomask) ||
// 						!TransactionIdEquals(HeapTupleHeaderGetRawXmax_tsdb(tuple->t_data), xwait))
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
// 					!TransactionIdEquals(HeapTupleHeaderGetRawXmax_tsdb(tuple->t_data), xwait))
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
// 				!TransactionIdEquals(HeapTupleHeaderGetRawXmax_tsdb(tuple->t_data), xwait))
// 				goto l3;
// 			Assert(HEAP_XMAX_IS_LOCKED_ONLY_tsdb(tuple->t_data->t_infomask));
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
// 			if (!heap_acquire_tuplock(relation, tid, mode, wait_policy, &have_tuple_lock))
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
// 						MultiXactIdWait((MultiXactId) xwait,
// 										status,
// 										infomask,
// 										relation,
// 										&tuple->t_self,
// 										XLTW_Lock,
// 										NULL);
// 						break;
// 					case LockWaitSkip:
// 						if (!ConditionalMultiXactIdWait((MultiXactId) xwait,
// 														status,
// 														infomask,
// 														relation,
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
// 														status,
// 														infomask,
// 														relation,
// 														NULL))
// 							ereport(ERROR,
// 									(errcode(ERRCODE_LOCK_NOT_AVAILABLE),
// 									 errmsg("could not obtain lock on row in relation \"%s\"",
// 											RelationGetRelationName(relation))));

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
// 						XactLockTableWait(xwait, relation, &tuple->t_self, XLTW_Lock);
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
// 											RelationGetRelationName(relation))));
// 						break;
// 				}
// 			}

// 			/* if there are updates, follow the update chain */
// 			if (follow_updates && !HEAP_XMAX_IS_LOCKED_ONLY_tsdb(infomask))
// 			{
// 				HTSU_Result res;

// 				res = heap_lock_updated_tuple(relation,
// 											  tuple,
// 											  &t_ctid,
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
// 				!TransactionIdEquals(HeapTupleHeaderGetRawXmax_tsdb(tuple->t_data), xwait))
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
// 		if (!require_sleep || (tuple->t_data->t_infomask & HEAP_XMAX_INVALID) ||
// 			HEAP_XMAX_IS_LOCKED_ONLY_tsdb(tuple->t_data->t_infomask) ||
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
// 		hufd->xmax = HeapTupleHeaderGetUpdateXid_tsdb(tuple,tuple->t_data);
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

// 	xmax = HeapTupleHeaderGetRawXmax_tsdb(tuple->t_data);
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
// 	compute_new_xmax_infomask(xmax,
// 							  old_infomask,
// 							  tuple->t_data->t_infomask2,
// 							  GetCurrentTransactionId(),
// 							  mode,
// 							  false,
// 							  &xid,
// 							  &new_infomask,
// 							  &new_infomask2);

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
// 	if (HEAP_XMAX_IS_LOCKED_ONLY_tsdb(new_infomask))
// 		HeapTupleHeaderClearHotUpdated(tuple->t_data);
// 	HeapTupleHeaderSetXmax(tuple->t_data, xid);

// 	/*
// 	 * Make sure there is no forward chain link in t_ctid.  Note that in the
// 	 * cases where the tuple has been updated, we must not overwrite t_ctid,
// 	 * because it was set by the updater.  Moreover, if the tuple has been
// 	 * updated, we need to follow the update chain to lock the new versions of
// 	 * the tuple as well.
// 	 */
// 	if (HEAP_XMAX_IS_LOCKED_ONLY_tsdb(new_infomask))
// 		tuple->t_data->t_ctid = *tid;

// 	/* Clear only the all-frozen bit on visibility map if needed */
// 	if (PageIsAllVisible(page) &&
// 		visibilitymap_clear(relation, block, vmbuffer, VISIBILITYMAP_ALL_FROZEN))
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
// 		XLogRecPtr recptr;

// 		XLogBeginInsert();
// 		XLogRegisterBuffer(0, *buffer, REGBUF_STANDARD);

// 		xlrec.offnum = ItemPointerGetOffsetNumber(&tuple->t_self);
// 		xlrec.locking_xid = xid;
// 		xlrec.infobits_set = compute_infobits(new_infomask, tuple->t_data->t_infomask2);
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

//access/heapam.h
// void
// heap_endscan(HeapScanDesc scan)
// {
// 	/* Note: no locking manipulations needed */

// 	/*
// 	 * unpin scan buffers
// 	 */
// 	if (BufferIsValid(scan->rs_cbuf))
// 		ReleaseBuffer(scan->rs_cbuf);

// 	/*
// 	 * decrement relation reference count and free scan descriptor storage
// 	 */
// 	RelationDecrementReferenceCount((Relation)scan->rs_rd);

// 	if (scan->rs_key)
// 		pfree(scan->rs_key);

// 	if (scan->rs_strategy != NULL)
// 		FreeAccessStrategy(scan->rs_strategy);

// 	if (scan->rs_temp_snap)
// 		UnregisterSnapshot(scan->rs_snapshot);

// 	pfree(scan);
// }

/*
 * roleSpecsToIds
 *
 * Given a list of RoleSpecs, generate a list of role OIDs in the same order.
 *
 * ROLESPEC_PUBLIC is not allowed.
 */
List *
roleSpecsToIds(List *memberNames)
{
	List *result = NIL;
	ListCell *l;

	foreach (l, memberNames)
	{
		Node *rolespec = (Node *) lfirst(l);
		Oid roleid;

		roleid = get_rolespec_oid(rolespec, false);
		result = lappend_oid(result, roleid);
	}
	return result;
}


NamedLWLockTrancheRequest *NamedLWLockTrancheRequestArray;
LWLockPadded *
GetNamedLWLockTranche(const char *tranche_name)
{
	int lock_pos;
	int i;

	/*
	 * Obtain the position of base address of LWLock belonging to requested
	 * tranche_name in MainLWLockArray.  LWLocks for named tranches are placed
	 * in MainLWLockArray after fixed locks.
	 */
	lock_pos = NUM_FIXED_LWLOCKS;
	for (i = 0; i < NamedLWLockTrancheRequests; i++)
	{
		if (strcmp(NamedLWLockTrancheRequestArray[i].tranche_name, tranche_name) == 0)
			return &t_thrd.shemem_ptr_cxt.mainLWLockArray[lock_pos];

		lock_pos += NamedLWLockTrancheRequestArray[i].num_lwlocks;
	}

	if (i >= NamedLWLockTrancheRequests)
		elog(ERROR, "requested tranche is not registered");

	/* just to keep compiler quiet */
	return NULL;
}

/*
 * RequestNamedLWLockTranche
 *		Request that extra LWLocks be allocated during postmaster
 *		startup.
 *
 * This is only useful for extensions if called from the _PG_init hook
 * of a library that is loaded into the postmaster via
 * shared_preload_libraries.  Once shared memory has been allocated, calls
 * will be ignored.  (We could raise an error, but it seems better to make
 * it a no-op, so that libraries containing such calls can be reloaded if
 * needed.)
 */
void
RequestNamedLWLockTranche(const char *tranche_name, int num_lwlocks)
{
	NamedLWLockTrancheRequest *request;

	if (IsUnderPostmaster || !lock_named_request_allowed)
		return; /* too late */

	if (NamedLWLockTrancheRequestArray == NULL)
	{
		NamedLWLockTrancheRequestsAllocated = 16;
		NamedLWLockTrancheRequestArray =
			(NamedLWLockTrancheRequest *) MemoryContextAlloc(TopMemoryContext,//tsdb TopMemoryContext
															 NamedLWLockTrancheRequestsAllocated *
																 sizeof(NamedLWLockTrancheRequest));
	}

	if (NamedLWLockTrancheRequests >= NamedLWLockTrancheRequestsAllocated)
	{
		int i = NamedLWLockTrancheRequestsAllocated;

		while (i <= NamedLWLockTrancheRequests)
			i *= 2;

		NamedLWLockTrancheRequestArray =
			(NamedLWLockTrancheRequest *) repalloc(NamedLWLockTrancheRequestArray,
												   i * sizeof(NamedLWLockTrancheRequest));
		NamedLWLockTrancheRequestsAllocated = i;
	}

	request = &NamedLWLockTrancheRequestArray[NamedLWLockTrancheRequests];
	Assert(strlen(tranche_name) + 1 < NAMEDATALEN);
	StrNCpy(request->tranche_name, tranche_name, NAMEDATALEN);
	request->num_lwlocks = num_lwlocks;
	NamedLWLockTrancheRequests++;
}




//放在gausskernel/storage/access/heap/heapam.cpp 
// HeapScanDesc
// heap_beginscan_catalog(Relation relation, int nkeys, ScanKey key)
// {
// 	Oid relid = RelationGetRelid(relation);
// 	Snapshot snapshot = RegisterSnapshot(GetCatalogSnapshot(relid));

// 	return heap_beginscan_internal(relation,
// 								   snapshot,
// 								   nkeys,
// 								   key,
// 								   NULL,
// 								   true,
// 								   true,
// 								   true,
// 								   false,
// 								   false,
// 								   true);
// }

void
ApplySetting(Snapshot snapshot, Oid databaseid, Oid roleid, Relation relsetting, GucSource source)
{
	SysScanDesc scan;
	ScanKeyData keys[2];
	HeapTuple tup;

	ScanKeyInit(&keys[0],
				Anum_pg_db_role_setting_setdatabase,
				BTEqualStrategyNumber,
				F_OIDEQ,
				ObjectIdGetDatum(databaseid));
	ScanKeyInit(&keys[1],
				Anum_pg_db_role_setting_setrole,
				BTEqualStrategyNumber,
				F_OIDEQ,
				ObjectIdGetDatum(roleid));

	scan = systable_beginscan(relsetting, DbRoleSettingDatidRolidIndexId, true, snapshot, 2, keys);
	while (HeapTupleIsValid(tup = systable_getnext(scan)))
	{
		bool isnull;
		Datum datum;

		datum = heap_getattr_tsdb(tup,
							 Anum_pg_db_role_setting_setconfig,
							 RelationGetDescr(relsetting),
							 &isnull);
		if (!isnull)
		{
			ArrayType *a = DatumGetArrayTypeP(datum);

			/*
			 * We process all the options at SUSET level.  We assume that the
			 * right to insert an option into pg_db_role_setting was checked
			 * when it was inserted.
			 */
			ProcessGUCArray(a, PGC_SUSET, source, GUC_ACTION_SET);
		}
	}

	systable_endscan(scan);
}

//放在gausskernel/storage/access/heap/heapam.cpp 
//先不重载尝试一下10-22
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

//先不重载,尝试一下
// Oid
// make_new_heap(Oid OIDOldHeap, Oid NewTableSpace, char relpersistence, LOCKMODE lockmode)
// {
// 	TupleDesc OldHeapDesc;
// 	char NewHeapName[NAMEDATALEN];
// 	Oid OIDNewHeap;
// 	Oid toastid;
// 	Relation OldHeap;
// 	HeapTuple tuple;
// 	Datum reloptions;
// 	bool isNull;
// 	Oid namespaceid;

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
// 	reloptions = SysCacheGetAttr(RELOID, tuple, Anum_pg_class_reloptions, &isNull);
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
// 		reloptions = SysCacheGetAttr(RELOID, tuple, Anum_pg_class_reloptions, &isNull);
// 		if (isNull)
// 			reloptions = (Datum) 0;

// 		NewHeapCreateToastTable(OIDNewHeap, reloptions, lockmode);

// 		ReleaseSysCache(tuple);
// 	}

// 	heap_close(OldHeap, NoLock);

// 	return OIDNewHeap;
// }

//不重载,试
// void
// vacuum_set_xid_limits(Relation rel, int freeze_min_age, int freeze_table_age,
// 					  int multixact_freeze_min_age, int multixact_freeze_table_age,
// 					  TransactionId *oldestXmin, TransactionId *freezeLimit,
// 					  TransactionId *xidFullScanLimit, MultiXactId *multiXactCutoff,
// 					  MultiXactId *mxactFullScanLimit)
// {
// 	int freezemin;
// 	int mxid_freezemin;
// 	int effective_multixact_freeze_max_age;
// 	TransactionId limit;
// 	TransactionId safeLimit;
// 	MultiXactId mxactLimit;
// 	MultiXactId safeMxactLimit;

// 	/*
// 	 * We can always ignore processes running lazy vacuum.  This is because we
// 	 * use these values only for deciding which tuples we must keep in the
// 	 * tables.  Since lazy vacuum doesn't write its XID anywhere, it's safe to
// 	 * ignore it.  In theory it could be problematic to ignore lazy vacuums in
// 	 * a full vacuum, but keep in mind that only one vacuum process can be
// 	 * working on a particular table at any time, and that each vacuum is
// 	 * always an independent transaction.
// 	 */
// 	*oldestXmin = TransactionIdLimitedForOldSnapshots(GetOldestXmin(rel, true), rel);

// 	Assert(TransactionIdIsNormal(*oldestXmin));

// 	/*
// 	 * Determine the minimum freeze age to use: as specified by the caller, or
// 	 * vacuum_freeze_min_age, but in any case not more than half
// 	 * autovacuum_freeze_max_age, so that autovacuums to prevent XID
// 	 * wraparound won't occur too frequently.
// 	 */
// 	freezemin = freeze_min_age;
// 	if (freezemin < 0)
// 		freezemin = vacuum_freeze_min_age;
// 	freezemin = Min(freezemin, autovacuum_freeze_max_age / 2);
// 	Assert(freezemin >= 0);

// 	/*
// 	 * Compute the cutoff XID, being careful not to generate a "permanent" XID
// 	 */
// 	limit = *oldestXmin - freezemin;
// 	if (!TransactionIdIsNormal(limit))
// 		limit = FirstNormalTransactionId;

// 	/*
// 	 * If oldestXmin is very far back (in practice, more than
// 	 * autovacuum_freeze_max_age / 2 XIDs old), complain and force a minimum
// 	 * freeze age of zero.
// 	 */
// 	safeLimit = ReadNewTransactionId() - autovacuum_freeze_max_age;
// 	if (!TransactionIdIsNormal(safeLimit))
// 		safeLimit = FirstNormalTransactionId;

// 	if (TransactionIdPrecedes(limit, safeLimit))
// 	{
// 		ereport(WARNING,
// 				(errmsg("oldest xmin is far in the past"),
// 				 errhint("Close open transactions soon to avoid wraparound problems.")));
// 		limit = *oldestXmin;
// 	}

// 	*freezeLimit = limit;

// 	/*
// 	 * Compute the multixact age for which freezing is urgent.  This is
// 	 * normally autovacuum_multixact_freeze_max_age, but may be less if we are
// 	 * short of multixact member space.
// 	 */
// 	effective_multixact_freeze_max_age = MultiXactMemberFreezeThreshold();

// 	/*
// 	 * Determine the minimum multixact freeze age to use: as specified by
// 	 * caller, or vacuum_multixact_freeze_min_age, but in any case not more
// 	 * than half effective_multixact_freeze_max_age, so that autovacuums to
// 	 * prevent MultiXact wraparound won't occur too frequently.
// 	 */
// 	mxid_freezemin = multixact_freeze_min_age;
// 	if (mxid_freezemin < 0)
// 		mxid_freezemin = vacuum_multixact_freeze_min_age;
// 	mxid_freezemin = Min(mxid_freezemin, effective_multixact_freeze_max_age / 2);
// 	Assert(mxid_freezemin >= 0);

// 	/* compute the cutoff multi, being careful to generate a valid value */
// 	mxactLimit = GetOldestMultiXactId() - mxid_freezemin;
// 	if (mxactLimit < FirstMultiXactId)
// 		mxactLimit = FirstMultiXactId;

// 	safeMxactLimit = ReadNextMultiXactId() - effective_multixact_freeze_max_age;
// 	if (safeMxactLimit < FirstMultiXactId)
// 		safeMxactLimit = FirstMultiXactId;

// 	if (MultiXactIdPrecedes(mxactLimit, safeMxactLimit))
// 	{
// 		ereport(WARNING,
// 				(errmsg("oldest multixact is far in the past"),
// 				 errhint("Close open transactions with multixacts soon to avoid wraparound "
// 						 "problems.")));
// 		mxactLimit = safeMxactLimit;
// 	}

// 	*multiXactCutoff = mxactLimit;

// 	if (xidFullScanLimit != NULL)
// 	{
// 		int freezetable;

// 		Assert(mxactFullScanLimit != NULL);

// 		/*
// 		 * Determine the table freeze age to use: as specified by the caller,
// 		 * or vacuum_freeze_table_age, but in any case not more than
// 		 * autovacuum_freeze_max_age * 0.95, so that if you have e.g nightly
// 		 * VACUUM schedule, the nightly VACUUM gets a chance to freeze tuples
// 		 * before anti-wraparound autovacuum is launched.
// 		 */
// 		freezetable = freeze_table_age;
// 		if (freezetable < 0)
// 			freezetable = vacuum_freeze_table_age;
// 		freezetable = Min(freezetable, autovacuum_freeze_max_age * 0.95);
// 		Assert(freezetable >= 0);

// 		/*
// 		 * Compute XID limit causing a full-table vacuum, being careful not to
// 		 * generate a "permanent" XID.
// 		 */
// 		limit = ReadNewTransactionId() - freezetable;
// 		if (!TransactionIdIsNormal(limit))
// 			limit = FirstNormalTransactionId;

// 		*xidFullScanLimit = limit;

// 		/*
// 		 * Similar to the above, determine the table freeze age to use for
// 		 * multixacts: as specified by the caller, or
// 		 * vacuum_multixact_freeze_table_age, but in any case not more than
// 		 * autovacuum_multixact_freeze_table_age * 0.95, so that if you have
// 		 * e.g. nightly VACUUM schedule, the nightly VACUUM gets a chance to
// 		 * freeze multixacts before anti-wraparound autovacuum is launched.
// 		 */
// 		freezetable = multixact_freeze_table_age;
// 		if (freezetable < 0)
// 			freezetable = vacuum_multixact_freeze_table_age;
// 		freezetable = Min(freezetable, effective_multixact_freeze_max_age * 0.95);
// 		Assert(freezetable >= 0);

// 		/*
// 		 * Compute MultiXact limit causing a full-table vacuum, being careful
// 		 * to generate a valid MultiXact value.
// 		 */
// 		mxactLimit = ReadNextMultiXactId() - freezetable;
// 		if (mxactLimit < FirstMultiXactId)
// 			mxactLimit = FirstMultiXactId;

// 		*mxactFullScanLimit = mxactLimit;
// 	}
// 	else
// 	{
// 		Assert(mxactFullScanLimit == NULL);
// 	}
// }


RewriteState
begin_heap_rewrite(Relation old_heap, Relation new_heap, TransactionId oldest_xmin,
				   TransactionId freeze_xid, MultiXactId cutoff_multi, bool use_wal)
{
	RewriteState state;
	MemoryContext rw_cxt;
	MemoryContext old_cxt;
	HASHCTL hash_ctl;

	/*
	 * To ease cleanup, make a separate context that will contain the
	 * RewriteState struct itself plus all subsidiary data.
	 */
	rw_cxt = AllocSetContextCreate(CurrentMemoryContext, "Table rewrite", ALLOCSET_DEFAULT_SIZES);
	old_cxt = MemoryContextSwitchTo(rw_cxt);

	/* Create and fill in the state struct */
	state =(RewriteStateData*) palloc0(sizeof(RewriteStateData));

	state->rs_old_rel = old_heap;
	state->rs_new_rel = new_heap;
	state->rs_buffer = (Page) palloc(BLCKSZ);
	/* new_heap needn't be empty, just locked */
	state->rs_blockno = RelationGetNumberOfBlocks(new_heap);
	state->rs_buffer_valid = false;
	state->rs_use_wal = use_wal;
	state->rs_oldest_xmin = oldest_xmin;
	state->rs_freeze_xid = freeze_xid;
	state->rs_cutoff_multi = cutoff_multi;
	state->rs_cxt = rw_cxt;

	/* Initialize hash tables used to track update chains */
	memset(&hash_ctl, 0, sizeof(hash_ctl));
	hash_ctl.keysize = sizeof(TidHashKey);
	hash_ctl.entrysize = sizeof(UnresolvedTupData);
	hash_ctl.hcxt = state->rs_cxt;

	state->rs_unresolved_tups = hash_create("Rewrite / Unresolved ctids",
											128, /* arbitrary initial size */
											&hash_ctl,
											HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);

	hash_ctl.entrysize = sizeof(OldToNewMappingData);

	state->rs_old_new_tid_map = hash_create("Rewrite / Old to new tid map",
											128, /* arbitrary initial size */
											&hash_ctl,
											HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);

	MemoryContextSwitchTo(old_cxt);

	logical_begin_heap_rewrite(state);

	return state;
}

//common\backend\utils\sort\tuplesort.cpp 
// Tuplesortstate *
// tuplesort_begin_cluster(TupleDesc tupDesc, Relation indexRel, int workMem, bool randomAccess)
// {
// 	Tuplesortstate *state = tuplesort_begin_common(workMem, randomAccess);
// 	ScanKey indexScanKey;
// 	MemoryContext oldcontext;
// 	int i;

// 	Assert(indexRel->rd_rel->relam == BTREE_AM_OID);

// 	oldcontext = MemoryContextSwitchTo(state->sortcontext);

// #ifdef TRACE_SORT
// 	if (trace_sort)
// 		elog(LOG,
// 			 "begin tuple sort: nkeys = %d, workMem = %d, randomAccess = %c",
// 			 RelationGetNumberOfAttributes(indexRel),
// 			 workMem,
// 			 randomAccess ? 't' : 'f');
// #endif

// 	state->nKeys = RelationGetNumberOfAttributes(indexRel);

// 	TRACE_POSTGRESQL_SORT_START(CLUSTER_SORT,
// 								false, /* no unique check */
// 								state->nKeys,
// 								workMem,
// 								randomAccess);

// 	state->comparetup = comparetup_cluster;
// 	state->copytup = copytup_cluster;
// 	state->writetup = writetup_cluster;
// 	state->readtup = readtup_cluster;
// 	state->movetup = movetup_cluster;
// 	state->abbrevNext = 10;

// 	state->indexInfo = BuildIndexInfo(indexRel);

// 	state->tupDesc = tupDesc; /* assume we need not copy tupDesc */

// 	indexScanKey = _bt_mkscankey_nodata(indexRel);

// 	if (state->indexInfo->ii_Expressions != NULL)
// 	{
// 		TupleTableSlot *slot;
// 		ExprContext *econtext;

// 		/*
// 		 * We will need to use FormIndexDatum to evaluate the index
// 		 * expressions.  To do that, we need an EState, as well as a
// 		 * TupleTableSlot to put the table tuples into.  The econtext's
// 		 * scantuple has to point to that slot, too.
// 		 */
// 		state->estate = CreateExecutorState();
// 		slot = MakeSingleTupleTableSlot(tupDesc);
// 		econtext = GetPerTupleExprContext(state->estate);
// 		econtext->ecxt_scantuple = slot;
// 	}

// 	/* Prepare SortSupport data for each column */
// 	state->sortKeys = (SortSupport) palloc0(state->nKeys * sizeof(SortSupportData));

// 	for (i = 0; i < state->nKeys; i++)
// 	{
// 		SortSupport sortKey = state->sortKeys + i;
// 		ScanKey scanKey = indexScanKey + i;
// 		int16 strategy;

// 		sortKey->ssup_cxt = CurrentMemoryContext;
// 		sortKey->ssup_collation = scanKey->sk_collation;
// 		sortKey->ssup_nulls_first = (scanKey->sk_flags & SK_BT_NULLS_FIRST) != 0;
// 		sortKey->ssup_attno = scanKey->sk_attno;
// 		/* Convey if abbreviation optimization is applicable in principle */
// 		sortKey->abbreviate = (i == 0);

// 		AssertState(sortKey->ssup_attno != 0);

// 		strategy =
// 			(scanKey->sk_flags & SK_BT_DESC) != 0 ? BTGreaterStrategyNumber : BTLessStrategyNumber;

// 		PrepareSortSupportFromIndexRel(indexRel, strategy, sortKey);
// 	}

// 	_bt_freeskey(indexScanKey);

// 	MemoryContextSwitchTo(oldcontext);

// 	return state;
// }


//common\backend\utils\sort\tuplesort.cpp
// void
// tuplesort_putheaptuple(Tuplesortstate *state, HeapTuple tup)
// {
// 	MemoryContext oldcontext = MemoryContextSwitchTo(state->sortcontext);
// 	SortTuple stup;

// 	/*
// 	 * Copy the given tuple into memory we control, and decrease availMem.
// 	 * Then call the common code.
// 	 */
// 	COPYTUP(state, &stup, (void *) tup);

// 	puttuple_common(state, &stup);

// 	MemoryContextSwitchTo(oldcontext);
// }

Oid
toast_get_valid_index(Oid toastoid, LOCKMODE lock)
{
	int num_indexes;
	int validIndex;
	Oid validIndexOid;
	Relation *toastidxs;
	Relation toastrel;

	/* Open the toast relation */
	toastrel = heap_open(toastoid, lock);

	/* Look for the valid index of the toast relation */
	validIndex = toast_open_indexes(toastrel, lock, &toastidxs, &num_indexes);
	validIndexOid = RelationGetRelid(toastidxs[validIndex]);

	/* Close the toast relation and all its indexes */
	toast_close_indexes(toastidxs, num_indexes, lock);
	heap_close(toastrel, lock);

	return validIndexOid;
}


bool
IsSystemClass(Oid relid, Form_pg_class reltuple)
{
	return IsToastClass(reltuple) || IsCatalogClass(relid, reltuple);
}

void
build_aggregate_finalfn_expr(Oid *agg_input_types, int num_finalfn_inputs, Oid agg_state_type,
							 Oid agg_result_type, Oid agg_input_collation, Oid finalfn_oid,
							 Expr **finalfnexpr)
{
	List *args;
	int i;

	/*
	 * Build expr tree for final function
	 */
	args = list_make1(make_agg_arg(agg_state_type, agg_input_collation));

	/* finalfn may take additional args, which match agg's input types */
	for (i = 0; i < num_finalfn_inputs - 1; i++)
	{
		args = lappend(args, make_agg_arg(agg_input_types[i], agg_input_collation));
	}

	*finalfnexpr = (Expr *) makeFuncExpr(finalfn_oid,
										 agg_result_type,
										 args,
										 InvalidOid,
										 agg_input_collation,
										 COERCE_EXPLICIT_CALL);
	/* finalfn is currently never treated as variadic */
}

ColumnDef *
makeColumnDef(const char *colname, Oid typeOid, int32 typmod, Oid collOid)
{
	ColumnDef *n = makeNode(ColumnDef);

	n->colname = pstrdup(colname);
	n->typname = makeTypeNameFromOid(typeOid, typmod);
	n->inhcount = 0;
	n->is_local = true;
	n->is_not_null = false;
	n->is_from_type = false;
	n->storage = 0;
	n->raw_default = NULL;
	n->cooked_default = NULL;
	n->collClause = NULL;
	n->collOid = collOid;
	n->constraints = NIL;
	n->fdwoptions = NIL;
	//tsdb n->location = -1;

	return n;
}

///common/backend/catalog/index.cpp 
// bool
// reindex_relation(Oid relid, int flags, int options)
// {
// 	Relation rel;
// 	Oid toast_relid;
// 	List *indexIds;
// 	bool is_pg_class;
// 	bool result;

// 	/*
// 	 * Open and lock the relation.  ShareLock is sufficient since we only need
// 	 * to prevent schema and data changes in it.  The lock level used here
// 	 * should match ReindexTable().
// 	 */
// 	rel = heap_open(relid, ShareLock);

// 	toast_relid = rel->rd_rel->reltoastrelid;

// 	/*
// 	 * Get the list of index OIDs for this relation.  (We trust to the
// 	 * relcache to get this with a sequential scan if ignoring system
// 	 * indexes.)
// 	 */
// 	indexIds = RelationGetIndexList(rel);

// 	/*
// 	 * reindex_index will attempt to update the pg_class rows for the relation
// 	 * and index.  If we are processing pg_class itself, we want to make sure
// 	 * that the updates do not try to insert index entries into indexes we
// 	 * have not processed yet.  (When we are trying to recover from corrupted
// 	 * indexes, that could easily cause a crash.) We can accomplish this
// 	 * because CatalogUpdateIndexes will use the relcache's index list to know
// 	 * which indexes to update. We just force the index list to be only the
// 	 * stuff we've processed.
// 	 *
// 	 * It is okay to not insert entries into the indexes we have not processed
// 	 * yet because all of this is transaction-safe.  If we fail partway
// 	 * through, the updated rows are dead and it doesn't matter whether they
// 	 * have index entries.  Also, a new pg_class index will be created with a
// 	 * correct entry for its own pg_class row because we do
// 	 * RelationSetNewRelfilenode() before we do index_build().
// 	 *
// 	 * Note that we also clear pg_class's rd_oidindex until the loop is done,
// 	 * so that that index can't be accessed either.  This means we cannot
// 	 * safely generate new relation OIDs while in the loop; shouldn't be a
// 	 * problem.
// 	 */
// 	is_pg_class = (RelationGetRelid(rel) == RelationRelationId);

// 	/* Ensure rd_indexattr is valid; see comments for RelationSetIndexList */
// 	if (is_pg_class)
// 		(void) RelationGetIndexAttrBitmap(rel, INDEX_ATTR_BITMAP_ALL);

// 	PG_TRY();
// 	{
// 		List *doneIndexes;
// 		ListCell *indexId;
// 		char persistence;

// 		if (flags & REINDEX_REL_SUPPRESS_INDEX_USE)
// 		{
// 			/* Suppress use of all the indexes until they are rebuilt */
// 			SetReindexPending(indexIds);

// 			/*
// 			 * Make the new heap contents visible --- now things might be
// 			 * inconsistent!
// 			 */
// 			CommandCounterIncrement();
// 		}

// 		/*
// 		 * Compute persistence of indexes: same as that of owning rel, unless
// 		 * caller specified otherwise.
// 		 */
// 		if (flags & REINDEX_REL_FORCE_INDEXES_UNLOGGED)
// 			persistence = RELPERSISTENCE_UNLOGGED;
// 		else if (flags & REINDEX_REL_FORCE_INDEXES_PERMANENT)
// 			persistence = RELPERSISTENCE_PERMANENT;
// 		else
// 			persistence = rel->rd_rel->relpersistence;

// 		/* Reindex all the indexes. */
// 		doneIndexes = NIL;
// 		foreach (indexId, indexIds)
// 		{
// 			Oid indexOid = lfirst_oid(indexId);

// 			if (is_pg_class)
// 				RelationSetIndexList(rel, doneIndexes, InvalidOid);

// 			reindex_index(indexOid, !(flags & REINDEX_REL_CHECK_CONSTRAINTS), persistence, options);

// 			CommandCounterIncrement();

// 			/* Index should no longer be in the pending list */
// 			Assert(!ReindexIsProcessingIndex(indexOid));

// 			if (is_pg_class)
// 				doneIndexes = lappend_oid(doneIndexes, indexOid);
// 		}
// 	}
// 	PG_CATCH();
// 	{
// 		/* Make sure list gets cleared on error exit */
// 		ResetReindexPending();
// 		PG_RE_THROW();
// 	}
// 	PG_END_TRY();
// 	ResetReindexPending();

// 	if (is_pg_class)
// 		RelationSetIndexList(rel, indexIds, ClassOidIndexId);

// 	/*
// 	 * Close rel, but continue to hold the lock.
// 	 */
// 	heap_close(rel, NoLock);

// 	result = (indexIds != NIL);

// 	/*
// 	 * If the relation has a secondary toast rel, reindex that too while we
// 	 * still hold the lock on the master table.
// 	 */
// 	if ((flags & REINDEX_REL_PROCESS_TOAST) && OidIsValid(toast_relid))
// 		result |= reindex_relation(toast_relid, flags, options);

// 	return result;
// }

// 在tsdb中tsl\src\compression\compression.c 
// static Compressor *
// compressor_for_algorithm_and_type(CompressionAlgorithms algorithm, Oid type)
// {
// 	if (algorithm >= _END_COMPRESSION_ALGORITHMS)
// 		elog(ERROR, "invalid compression algorithm %d", algorithm);

// 	return definitions[algorithm].compressor_for_type(type);
// }


void *
MemoryContextAllocExtended(MemoryContext context, Size size, int flags)
{
	void *ret;

	AssertArg(MemoryContextIsValid(context));
	AssertNotInCriticalSection(context);

	if (((flags & MCXT_ALLOC_HUGE) != 0 && !AllocHugeSizeIsValid(size)) ||
		((flags & MCXT_ALLOC_HUGE) == 0 && !AllocSizeIsValid(size)))
		elog(ERROR, "invalid memory alloc request size %zu", size);

	context->isReset = false;

	void *(*alloc) (MemoryContext context, Size size);
	ret = (alloc)(context, size);//这里修改了
	if (ret == NULL)
	{
		if ((flags & MCXT_ALLOC_NO_OOM) == 0)
		{
			MemoryContextStats(TopMemoryContext);
			ereport(ERROR,
					(errcode(ERRCODE_OUT_OF_MEMORY),
					 errmsg("out of memory"),
					 errdetail("Failed on request of size %zu.", size)));
		}
		return NULL;
	}

	VALGRIND_MEMPOOL_ALLOC(context, ret, size);

	if ((flags & MCXT_ALLOC_ZERO) != 0)
		MemSetAligned(ret, 0, size);

	return ret;
}

ArrayBuildStateArr *
initArrayResultArr(Oid array_type, Oid element_type, MemoryContext rcontext, bool subcontext)
{
	ArrayBuildStateArr *astate;
	MemoryContext arr_context = rcontext; /* by default use the parent
										   * ctx */

	/* Lookup element type, unless element_type already provided */
	if (!OidIsValid(element_type))
	{
		element_type = get_element_type(array_type);

		if (!OidIsValid(element_type))
			ereport(ERROR,
					(errcode(ERRCODE_DATATYPE_MISMATCH),
					 errmsg("data type %s is not an array type", format_type_be(array_type))));
	}

	/* Make a temporary context to hold all the junk */
	if (subcontext)
		arr_context =
			AllocSetContextCreate(rcontext, "accumArrayResultArr", ALLOCSET_DEFAULT_SIZES);

	/* Note we initialize all fields to zero */
	astate = (ArrayBuildStateArr *) MemoryContextAllocZero(arr_context, sizeof(ArrayBuildStateArr));
	astate->mcontext = arr_context;
	astate->private_cxt = subcontext;

	/* Save relevant datatype information */
	astate->array_type = array_type;
	astate->element_type = element_type;

	return astate;
}

ArrayBuildState *
initArrayResult(Oid element_type, MemoryContext rcontext, bool subcontext)
{
	ArrayBuildState *astate;
	MemoryContext arr_context = rcontext;

	/* Make a temporary context to hold all the junk */
	if (subcontext)
		arr_context = AllocSetContextCreate(rcontext, "accumArrayResult", ALLOCSET_DEFAULT_SIZES);

	astate = (ArrayBuildState *) MemoryContextAlloc(arr_context, sizeof(ArrayBuildState));
	astate->mcontext = arr_context;
    //这里ArrayBuildState需要加成员private_cxt
	astate->private_cxt = subcontext;
	astate->alen = (subcontext ? 64 : 8); /* arbitrary starting array
										   * size */
	astate->dvalues = (Datum *) MemoryContextAlloc(arr_context, astate->alen * sizeof(Datum));
	astate->dnulls = (bool *) MemoryContextAlloc(arr_context, astate->alen * sizeof(bool));
	astate->nelems = 0;
	astate->element_type = element_type;
	get_typlenbyvalalign(element_type, &astate->typlen, &astate->typbyval, &astate->typalign);

	return astate;
}

ArrayBuildStateArr *
accumArrayResultArr(ArrayBuildStateArr *astate, Datum dvalue, bool disnull, Oid array_type,
					MemoryContext rcontext)
{
	ArrayType *arg;
	MemoryContext oldcontext;
	int *dims, *lbs, ndims, nitems, ndatabytes;
	char *data;
	int i;

	/*
	 * We disallow accumulating null subarrays.  Another plausible definition
	 * is to ignore them, but callers that want that can just skip calling
	 * this function.
	 */
	if (disnull)
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("cannot accumulate null arrays")));

	/* Detoast input array in caller's context */
	arg = DatumGetArrayTypeP(dvalue);

	if (astate == NULL)
		astate = initArrayResultArr(array_type, InvalidOid, rcontext, true);
	else
		Assert(astate->array_type == array_type);

	oldcontext = MemoryContextSwitchTo(astate->mcontext);

	/* Collect this input's dimensions */
	ndims = ARR_NDIM(arg);
	dims = ARR_DIMS(arg);
	lbs = ARR_LBOUND(arg);
	data = ARR_DATA_PTR(arg);
	nitems = ArrayGetNItems(ndims, dims);
	ndatabytes = ARR_SIZE(arg) - ARR_DATA_OFFSET(arg);

	if (astate->ndims == 0)
	{
		/* First input; check/save the dimensionality info */

		/* Should we allow empty inputs and just produce an empty output? */
		if (ndims == 0)
			ereport(ERROR,
					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
					 errmsg("cannot accumulate empty arrays")));
		if (ndims + 1 > MAXDIM)
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("number of array dimensions (%d) exceeds the maximum allowed (%d)",
							ndims + 1,
							MAXDIM)));

		/*
		 * The output array will have n+1 dimensions, with the ones after the
		 * first matching the input's dimensions.
		 */
		astate->ndims = ndims + 1;
		astate->dims[0] = 0;
		memcpy(&astate->dims[1], dims, ndims * sizeof(int));
		astate->lbs[0] = 1;
		memcpy(&astate->lbs[1], lbs, ndims * sizeof(int));

		/* Allocate at least enough data space for this item */
		astate->abytes = 1024;
		while (astate->abytes <= ndatabytes)
			astate->abytes *= 2;
		astate->data = (char *) palloc(astate->abytes);
	}
	else
	{
		/* Second or later input: must match first input's dimensionality */
		if (astate->ndims != ndims + 1)
			ereport(ERROR,
					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
					 errmsg("cannot accumulate arrays of different dimensionality")));
		for (i = 0; i < ndims; i++)
		{
			if (astate->dims[i + 1] != dims[i] || astate->lbs[i + 1] != lbs[i])
				ereport(ERROR,
						(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
						 errmsg("cannot accumulate arrays of different dimensionality")));
		}

		/* Enlarge data space if needed */
		if (astate->nbytes + ndatabytes > astate->abytes)
		{
			astate->abytes = Max(astate->abytes * 2, astate->nbytes + ndatabytes);
			astate->data = (char *) repalloc(astate->data, astate->abytes);
		}
	}

	/*
	 * Copy the data portion of the sub-array.  Note we assume that the
	 * advertised data length of the sub-array is properly aligned.  We do not
	 * have to worry about detoasting elements since whatever's in the
	 * sub-array should be OK already.
	 */
	memcpy(astate->data + astate->nbytes, data, ndatabytes);
	astate->nbytes += ndatabytes;

	/* Deal with null bitmap if needed */
	if (astate->nullbitmap || ARR_HASNULL(arg))
	{
		int newnitems = astate->nitems + nitems;

		if (astate->nullbitmap == NULL)
		{
			/*
			 * First input with nulls; we must retrospectively handle any
			 * previous inputs by marking all their items non-null.
			 */
			astate->aitems = 256;
			while (astate->aitems <= newnitems)
				astate->aitems *= 2;
			astate->nullbitmap = (bits8 *) palloc((astate->aitems + 7) / 8);
			array_bitmap_copy(astate->nullbitmap, 0, NULL, 0, astate->nitems);
		}
		else if (newnitems > astate->aitems)
		{
			astate->aitems = Max(astate->aitems * 2, newnitems);
			astate->nullbitmap = (bits8 *) repalloc(astate->nullbitmap, (astate->aitems + 7) / 8);
		}
		array_bitmap_copy(astate->nullbitmap, astate->nitems, ARR_NULLBITMAP(arg), 0, nitems);
	}

	astate->nitems += nitems;
	astate->dims[0] += 1;

	MemoryContextSwitchTo(oldcontext);

	/* Release detoasted copy if any */
	if ((Pointer) arg != DatumGetPointer(dvalue))
		pfree(arg);

	return astate;
}

Query *
get_view_query(Relation view)
{
	int i;

	Assert(view->rd_rel->relkind == RELKIND_VIEW);

	for (i = 0; i < view->rd_rules->numLocks; i++)
	{
		RewriteRule *rule = view->rd_rules->rules[i];

		if (rule->event == CMD_SELECT)
		{
			/* A _RETURN rule should have only one action */
			if (list_length(rule->actions) != 1)
				elog(ERROR, "invalid _RETURN rule action specification");

			return (Query *) linitial(rule->actions);
		}
	}

	elog(ERROR, "failed to find _RETURN rule for view");
	return NULL; /* keep compiler quiet */
}

Datum
makeArrayResultArr(ArrayBuildStateArr *astate, MemoryContext rcontext, bool release)
{
	ArrayType *result;
	MemoryContext oldcontext;

	/* Build the final array result in rcontext */
	oldcontext = MemoryContextSwitchTo(rcontext);

	if (astate->ndims == 0)
	{
		/* No inputs, return empty array */
		result = construct_empty_array(astate->element_type);
	}
	else
	{
		int dataoffset, nbytes;

		/* Compute required space */
		nbytes = astate->nbytes;
		if (astate->nullbitmap != NULL)
		{
			dataoffset = ARR_OVERHEAD_WITHNULLS(astate->ndims, astate->nitems);
			nbytes += dataoffset;
		}
		else
		{
			dataoffset = 0;
			nbytes += ARR_OVERHEAD_NONULLS(astate->ndims);
		}

		result = (ArrayType *) palloc0(nbytes);
		SET_VARSIZE(result, nbytes);
		result->ndim = astate->ndims;
		result->dataoffset = dataoffset;
		result->elemtype = astate->element_type;

		memcpy(ARR_DIMS(result), astate->dims, astate->ndims * sizeof(int));
		memcpy(ARR_LBOUND(result), astate->lbs, astate->ndims * sizeof(int));
		memcpy(ARR_DATA_PTR(result), astate->data, astate->nbytes);

		if (astate->nullbitmap != NULL)
			array_bitmap_copy(ARR_NULLBITMAP(result), 0, astate->nullbitmap, 0, astate->nitems);
	}

	MemoryContextSwitchTo(oldcontext);

	/* Clean up all the junk */
	if (release)
	{
		Assert(astate->private_cxt);
		MemoryContextDelete(astate->mcontext);
	}

	return PointerGetDatum(result);
}

int
SPI_execute_with_args(const char *src, int nargs, Oid *argtypes, Datum *Values, const char *Nulls,
					  bool read_only, long tcount)
{
	int res;
	_SPI_plan plan;
	ParamListInfo paramLI;

	if (src == NULL || nargs < 0 || tcount < 0)
		return SPI_ERROR_ARGUMENT;

	if (nargs > 0 && (argtypes == NULL || Values == NULL))
		return SPI_ERROR_PARAM;

	res = _SPI_begin_call(true);
	if (res < 0)
		return res;

	memset(&plan, 0, sizeof(_SPI_plan));
	plan.magic = _SPI_PLAN_MAGIC;
	plan.cursor_options = 0;
	plan.nargs = nargs;
	plan.argtypes = argtypes;
	plan.parserSetup = NULL;
	plan.parserSetupArg = NULL;

	paramLI = _SPI_convert_params(nargs, argtypes, Values, Nulls);

	_SPI_prepare_oneshot_plan(src, &plan);

	res = _SPI_execute_plan(&plan,
							paramLI,
							InvalidSnapshot,
							InvalidSnapshot,
							read_only,
							true,
							tcount);

	_SPI_end_call(true);
	return res;
}
bool
scanint8(const char *str, bool errorOK, int64 *result)
{
	const char *ptr = str;
	int64 tmp = 0;
	int sign = 1;

	/*
	 * Do our own scan, rather than relying on sscanf which might be broken
	 * for long long.
	 */

	/* skip leading spaces */
	while (*ptr && isspace((unsigned char) *ptr))
		ptr++;

	/* handle sign */
	if (*ptr == '-')
	{
		ptr++;

		/*
		 * Do an explicit check for INT64_MIN.  Ugly though this is, it's
		 * cleaner than trying to get the loop below to handle it portably.
		 */
		if (strncmp(ptr, "9223372036854775808", 19) == 0)
		{
			tmp = PG_INT64_MIN;
			ptr += 19;
			goto gotdigits;
		}
		sign = -1;
	}
	else if (*ptr == '+')
		ptr++;

	/* require at least one digit */
	if (!isdigit((unsigned char) *ptr))
	{
		if (errorOK)
			return false;
		else
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
					 errmsg("invalid input syntax for integer: \"%s\"", str)));
	}

	/* process digits */
	while (*ptr && isdigit((unsigned char) *ptr))
	{
		int64 newtmp = tmp * 10 + (*ptr++ - '0');

		if ((newtmp / 10) != tmp) /* overflow? */
		{
			if (errorOK)
				return false;
			else
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
						 errmsg("value \"%s\" is out of range for type bigint", str)));
		}
		tmp = newtmp;
	}

gotdigits:

	/* allow trailing whitespace, but not other trailing chars */
	while (*ptr != '\0' && isspace((unsigned char) *ptr))
		ptr++;

	if (*ptr != '\0')
	{
		if (errorOK)
			return false;
		else
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
					 errmsg("invalid input syntax for integer: \"%s\"", str)));
	}

	*result = (sign < 0) ? -tmp : tmp;

	return true;
}

//gausskernel/optimizer/path/costsize.cpp 

// void
// cost_sort(Path *path, PlannerInfo *root, List *pathkeys, Cost input_cost, double tuples, int width,
// 		  Cost comparison_cost, int sort_mem, double limit_tuples)
// {
// 	Cost startup_cost = input_cost;
// 	Cost run_cost = 0;
// 	double input_bytes = relation_byte_size(tuples, width);
// 	double output_bytes;
// 	double output_tuples;
// 	long sort_mem_bytes = sort_mem * 1024L;

// 	if (!og_knl_session4->enable_sort)
// 		startup_cost +=og_knl_session5-> disable_cost;

// 	path->rows = tuples;

// 	/*
// 	 * We want to be sure the cost of a sort is never estimated as zero, even
// 	 * if passed-in tuple count is zero.  Besides, mustn't do log(0)...
// 	 */
// 	if (tuples < 2.0)
// 		tuples = 2.0;

// 	/* Include the default cost-per-comparison */
// 	comparison_cost += 2.0 *og_knl4-> cpu_operator_cost;

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
// 		double npages = ceil(input_bytes / BLCKSZ);
// 		double nruns = input_bytes / sort_mem_bytes;
// 		double mergeorder = tuplesort_merge_order(sort_mem_bytes);
// 		double log_runs;
// 		double npageaccesses;

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
// 		startup_cost += npageaccesses * (og_knl_session4-> seq_page_cost * 0.75 +og_gpc-> random_page_cost * 0.25);
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
// 	run_cost += og_knl4->cpu_operator_cost * tuples;

// 	path->startup_cost = startup_cost;
// 	path->total_cost = startup_cost + run_cost;
// }

void
check_index_predicates(PlannerInfo *root, RelOptInfo *rel)
{
	List *clauselist;
	bool have_partial;
	bool is_target_rel;
	Relids otherrels;
	ListCell *lc;

	/*
	 * Initialize the indrestrictinfo lists to be identical to
	 * baserestrictinfo, and check whether there are any partial indexes.  If
	 * not, this is all we need to do.
	 */
	have_partial = false;
	foreach (lc, rel->indexlist)
	{
		IndexOptInfo *index = (IndexOptInfo *) lfirst(lc);

		index->indrestrictinfo = rel->baserestrictinfo;
		if (index->indpred)
			have_partial = true;
	}
	if (!have_partial)
		return;

	/*
	 * Construct a list of clauses that we can assume true for the purpose of
	 * proving the index(es) usable.  Restriction clauses for the rel are
	 * always usable, and so are any join clauses that are "movable to" this
	 * rel.  Also, we can consider any EC-derivable join clauses (which must
	 * be "movable to" this rel, by definition).
	 */
	clauselist = list_copy(rel->baserestrictinfo);

	/* Scan the rel's join clauses */
	foreach (lc, rel->joininfo)
	{
		RestrictInfo *rinfo = (RestrictInfo *) lfirst(lc);

		/* Check if clause can be moved to this rel */
        //tsdb原来是 join_clause_is_movable_to(rinfo, (Index)rel)
		if (!join_clause_is_movable_to(rinfo, NULL))
			continue;

		clauselist = lappend(clauselist, rinfo);
	}

	/*
	 * Add on any equivalence-derivable join clauses.  Computing the correct
	 * relid sets for generate_join_implied_equalities is slightly tricky
	 * because the rel could be a child rel rather than a true baserel, and in
	 * that case we must remove its parents' relid(s) from all_baserels.
	 */
	if (rel->reloptkind == RELOPT_OTHER_MEMBER_REL)
		otherrels = bms_difference(root->all_baserels, find_childrel_parents(root, rel));
	else
		otherrels = bms_difference(root->all_baserels, rel->relids);

	if (!bms_is_empty(otherrels))
		clauselist = list_concat(clauselist,
								 generate_join_implied_equalities(root,
																  bms_union(rel->relids, otherrels),
																  otherrels,
																  rel));

	/*
	 * Normally we remove quals that are implied by a partial index's
	 * predicate from indrestrictinfo, indicating that they need not be
	 * checked explicitly by an indexscan plan using this index.  However, if
	 * the rel is a target relation of UPDATE/DELETE/SELECT FOR UPDATE, we
	 * cannot remove such quals from the plan, because they need to be in the
	 * plan so that they will be properly rechecked by EvalPlanQual testing.
	 * Some day we might want to remove such quals from the main plan anyway
	 * and pass them through to EvalPlanQual via a side channel; but for now,
	 * we just don't remove implied quals at all for target relations.
	 */
	is_target_rel = (rel->relid == root->parse->resultRelation ||
					 get_plan_rowmark(root->rowMarks, rel->relid) != NULL);

	/*
	 * Now try to prove each index predicate true, and compute the
	 * indrestrictinfo lists for partial indexes.  Note that we compute the
	 * indrestrictinfo list even for non-predOK indexes; this might seem
	 * wasteful, but we may be able to use such indexes in OR clauses, cf
	 * generate_bitmap_or_paths().
	 */
	foreach (lc, rel->indexlist)
	{
		IndexOptInfo *index = (IndexOptInfo *) lfirst(lc);
		ListCell *lcr;

		if (index->indpred == NIL)
			continue; /* ignore non-partial indexes here */

		if (!index->predOK) /* don't repeat work if already proven OK */
			index->predOK = predicate_implied_by(index->indpred, clauselist);

		/* If rel is an update target, leave indrestrictinfo as set above */
		if (is_target_rel)
			continue;

		/* Else compute indrestrictinfo as the non-implied quals */
        //tsdb 需要在IndexOptInfo加入成员indrestrictinfo
		index->indrestrictinfo = NIL;
		foreach (lcr, rel->baserestrictinfo)
		{
			RestrictInfo *rinfo = (RestrictInfo *) lfirst(lcr);

			/* predicate_implied_by() assumes first arg is immutable */
			if (contain_mutable_functions((Node *) rinfo->clause) ||
				!predicate_implied_by(list_make1(rinfo->clause), index->indpred))
				index->indrestrictinfo = lappend(index->indrestrictinfo, rinfo);
		}
	}
}



SortPath *
create_sort_path(PlannerInfo *root, RelOptInfo *rel, Path *subpath, List *pathkeys,
				 double limit_tuples)
{
	//需要在nodetag中加成员T_SortPath
	SortPath *pathnode = makeNode(SortPath);

	pathnode->path.pathtype = T_Sort;
	pathnode->path.parent = rel;
	/* Sort doesn't project, so use source path's pathtarget */
	pathnode->path.pathtarget = subpath->pathtarget;
	/* For now, assume we are above any joins, so no parameterization */
	pathnode->path.param_info = NULL;
	pathnode->path.parallel_aware = false;
	pathnode->path.parallel_safe = rel->consider_parallel && subpath->parallel_safe;
	pathnode->path.parallel_workers = subpath->parallel_workers;
	pathnode->path.pathkeys = pathkeys;

	pathnode->subpath = subpath;

	cost_sort(&pathnode->path,
			  root,
			  pathkeys,
			  subpath->total_cost,
			  subpath->rows,
			  subpath->pathtarget->width,
			  0.0, /* XXX comparison_cost shouldn't be 0? */
			  u_sess->attr.attr_memory.work_mem,
			  limit_tuples);

	return pathnode;
}



//原本在plan_agg_bookend.cpp中,放置重复定义,在这里实现
void
ts_preprocess_first_last_aggregates(PlannerInfo *root, List *tlist)
{
	Query *parse = root->parse;
	FromExpr *jtnode;
	RangeTblRef *rtr;
	RangeTblEntry *rte;
	List *first_last_aggs;
	RelOptInfo *grouped_rel;
	ListCell *lc;
	List *mm_agg_list;
	MinMaxAggPath *minmaxagg_path;

	/* minmax_aggs list should be empty at this point */
	Assert(root->minmax_aggs == NIL);

	/* Nothing to do if query has no aggregates */
	if (!parse->hasAggs)
		return;

	Assert(!parse->setOperations);  /* shouldn't get here if a setop */
	Assert(parse->rowMarks == NIL); /* nor if FOR UPDATE */

	/*
	 * Reject unoptimizable cases.
	 *
	 * We don't handle the case when agg function is in ORDER BY. The reason
	 * being is that we replace Aggref node before sort keys are being
	 * generated.
	 *
	 * We don't handle GROUP BY or windowing, because our current
	 * implementations of grouping require looking at all the rows anyway, and
	 * so there's not much point in optimizing FIRST/LAST.
	 */
	if (parse->groupClause || list_length(parse->groupingSets) > 1 || parse->hasWindowFuncs ||
		contains_first_last_node(parse->sortClause, tlist))
		return;

	/*
	 * Reject if query contains any CTEs; there's no way to build an indexscan
	 * on one so we couldn't succeed here.  (If the CTEs are unreferenced,
	 * that's not true, but it doesn't seem worth expending cycles to check.)
	 */
	if (parse->cteList)
		return;

	/*
	 * We also restrict the query to reference exactly one table, since join
	 * conditions can't be handled reasonably.  (We could perhaps handle a
	 * query containing cartesian-product joins, but it hardly seems worth the
	 * trouble.)  However, the single table could be buried in several levels
	 * of FromExpr due to subqueries.  Note the "single" table could be an
	 * inheritance parent, too, including the case of a UNION ALL subquery
	 * that's been flattened to an appendrel.
	 */
	jtnode = parse->jointree;
	while (IsA(jtnode, FromExpr))
	{
		if (list_length(jtnode->fromlist) != 1)
			return;
		jtnode =(FromExpr *) linitial(jtnode->fromlist);
	}
	if (!IsA(jtnode, RangeTblRef))
		return;
	rtr = (RangeTblRef *) jtnode;
	rte = planner_rt_fetch(rtr->rtindex, root);
	if (rte->rtekind == RTE_RELATION)
		/* ordinary relation, ok */;
	else if (rte->rtekind == RTE_SUBQUERY && rte->inh)
		/* flattened UNION ALL subquery, ok */;
	else
		return;

	/*
	 * Scan the tlist and HAVING qual to find all the aggregates and verify
	 * all are FIRST/LAST aggregates.  Stop as soon as we find one that isn't.
	 */
	first_last_aggs = NIL;
	if (find_first_last_aggs_walker((Node *) tlist, &first_last_aggs))
		return;
	if (find_first_last_aggs_walker(parse->havingQual, &first_last_aggs))
		return;

	/*
	 * OK, there is at least the possibility of performing the optimization.
	 * Build an access path for each aggregate.  If any of the aggregates
	 * prove to be non-indexable, give up; there is no point in optimizing
	 * just some of them.
	 */
	foreach (lc, first_last_aggs)
	{
		FirstLastAggInfo *fl_info = (FirstLastAggInfo *) lfirst(lc);
		MinMaxAggInfo *mminfo = fl_info->m_agg_info;
		Oid eqop;
		bool reverse;

		/*
		 * We'll need the equality operator that goes with the aggregate's
		 * ordering operator.
		 */
		eqop = get_equality_op_for_ordering_op(mminfo->aggsortop, &reverse);
		if (!OidIsValid(eqop)) /* shouldn't happen */
			elog(ERROR,
				 "could not find equality operator for ordering operator %u",
				 mminfo->aggsortop);

		/*
		 * We can use either an ordering that gives NULLS FIRST or one that
		 * gives NULLS LAST; furthermore there's unlikely to be much
		 * performance difference between them, so it doesn't seem worth
		 * costing out both ways if we get a hit on the first one.  NULLS
		 * FIRST is more likely to be available if the operator is a
		 * reverse-sort operator, so try that first if reverse.
		 */
		if (build_first_last_path(root, fl_info, eqop, mminfo->aggsortop, reverse))
			continue;
		if (build_first_last_path(root, fl_info, eqop, mminfo->aggsortop, !reverse))
			continue;

		/* No indexable path for this aggregate, so fail */
		return;
	}

	/*
	 * OK, we can do the query this way. We are using MinMaxAggPath to store
	 * First/Last Agg path since the logic is almost the same. MinMaxAggPath
	 * is used later on by planner so by reusing it we don't need to re-invent
	 * planner.
	 *
	 * Prepare to create a MinMaxAggPath node.
	 *
	 * First, create an output Param node for each agg.  (If we end up not
	 * using the MinMaxAggPath, we'll waste a PARAM_EXEC slot for each agg,
	 * which is not worth worrying about.  We can't wait till create_plan time
	 * to decide whether to make the Param, unfortunately.)
	 */
	mm_agg_list = NIL;
	foreach (lc, first_last_aggs)
	{
		FirstLastAggInfo *fl_info = (FirstLastAggInfo *) lfirst(lc);
		MinMaxAggInfo *mminfo = fl_info->m_agg_info;

		mminfo->param = SS_make_initplan_output_param(root,
													  exprType((Node *) mminfo->target),
													  -1,
													  exprCollation((Node *) mminfo->target));
		mm_agg_list = lcons(mminfo, mm_agg_list);
	}

	/*
	 * Create a MinMaxAggPath node with the appropriate estimated costs and
	 * other needed data, and add it to the UPPERREL_GROUP_AGG upperrel, where
	 * it will compete against the standard aggregate implementation.  (It
	 * will likely always win, but we need not assume that here.)
	 *
	 * Note: grouping_planner won't have created this upperrel yet, but it's
	 * fine for us to create it first.  We will not have inserted the correct
	 * consider_parallel value in it, but MinMaxAggPath paths are currently
	 * never parallel-safe anyway, so that doesn't matter.  Likewise, it
	 * doesn't matter that we haven't filled FDW-related fields in the rel.
	 */
	grouped_rel = fetch_upper_rel(root, UPPERREL_GROUP_AGG, NULL);
	minmaxagg_path = create_minmaxagg_path(root,
										   grouped_rel,
										   create_pathtarget(root, tlist),
										   mm_agg_list,
										   (List *) parse->havingQual);
	/* Let's replace Aggref node since we will use subquery we've generated  */
	replace_aggref_in_tlist(minmaxagg_path);
	add_path(grouped_rel, (Path *) minmaxagg_path);
}