//#include "tsdb.h"
#include "compat.h"

#include "commands/cluster.h"
//#include "gtm/utils/pqsignal.h"
#include "utils.h"

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


#include "catalog/pg_cast.h"
#include "catalog/pg_database.h"
#include "catalog/pg_conversion.h"
#include "catalog/pg_attrdef.h"
#include "catalog/pg_language.h"
#include "catalog/pg_amop.h"
#include "catalog/pg_amproc.h"
#include "catalog/pg_rewrite.h"
#include "catalog/pg_ts_parser.h"
#include "catalog/pg_ts_dict.h"
#include "catalog/pg_ts_template.h"
#include "catalog/pg_authid.h"

#include "catalog/pg_description.h"
//#include "catalog/pg_sequence.h"
//#include "catalog/pg_transform.h"
#include "access/visibilitymap.h"
#include "utils/distribute_test.h"
#include "storage/lmgr.h"
#include "commands/sequence.h"
#include "storage/predicate.h"
#include "commands/vacuum.h"
#include "access/multixact.h"
#include "parser/parse_utilcmd.h"
#include "parser/parse_collate.h"

#include "optimizer/pathnode.h"

#include "catalog/storage.h"
#include "tsdb_static.cpp"
//下面是用到的cpp,后面换到static里面




bool
IsInParallelMode(void)
{
	return CurrentTransactionState->parallelModeLevel != 0;
} 

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
// 			if (TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmin_tsdb(old_row->t_data)))
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
// 			if (TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmin_tsdb(old_row->t_data)))
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

// bool
// contain_volatile_functions_not_nextval(Node *clause)
// {
// 	return contain_volatile_functions_not_nextval_walker(clause, NULL);
// }

// FILE *
// OpenPipeStream(const char *command, const char *mode)
// {
// 	FILE	   *file;

// 	DO_DB(elog(LOG, "OpenPipeStream: Allocated %d (%s)",
// 			   numAllocatedDescs, command));

// 	/* Can we allocate another non-virtual FD? */
// 	if (!ReserveAllocatedDesc())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
// 				 errmsg("exceeded maxAllocatedDescs (%d) while trying to execute command \"%s\"",
// 						maxAllocatedDescs, command)));

// 	/* Close excess kernel FDs. */
// 	ReleaseLruFiles();

// TryAgain:
// 	fflush(stdout);
// 	fflush(stderr);
// 	errno = 0;
// 	if ((file = popen(command, mode)) != NULL)
// 	{
// 		AllocateDesc *desc = &og_knl5->allocatedDescs[numAllocatedDescs];

// 		desc->kind = AllocateDescPipe;
// 		desc->desc.file = file;
// 		desc->create_subid = GetCurrentSubTransactionId();
// 		numAllocatedDescs++;
// 		return desc->desc.file;
// 	}

// 	if (errno == EMFILE || errno == ENFILE)
// 	{
// 		int			save_errno = errno;

// 		ereport(LOG,
// 				(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
// 				 errmsg("out of file descriptors: %m; release and retry")));
// 		errno = 0;
// 		if (ReleaseLruFile())
// 			goto TryAgain;
// 		errno = save_errno;
// 	}

// 	return NULL;
// } 



// Node *
// transformWhereClause(ParseState *pstate, Node *clause,
// 					 ParseExprKind exprKind, const char *constructName)
// {
// 	Node	   *qual;

// 	if (clause == NULL)
// 		return NULL;

// 	qual = transformExpr(pstate, clause, exprKind);

// 	qual = coerce_to_boolean(pstate, qual, constructName);

// 	return qual;
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

void
RunObjectPostCreateHook(Oid classId, Oid objectId, int subId,
						bool is_internal)
{
	ObjectAccessPostCreate pc_arg;

	/* caller should check, but just in case... */
	Assert(object_access_hook != NULL);

	memset(&pc_arg, 0, sizeof(ObjectAccessPostCreate));
	pc_arg.is_internal = is_internal;

	(*object_access_hook) (OAT_POST_CREATE,
						   classId, objectId, subId,
						   (void *) &pc_arg);
} 

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
// 	og_knl6->MyBackendId = InvalidBackendId;

// 	SharedInvalBackendInit(false);

// 	if (og_knl6->MyBackendId >MaxBackends ||og_knl6-> MyBackendId <= 0)
// 		elog(FATAL, "bad backend ID: %d",og_knl6-> MyBackendId);

// 	/* Now that we have a BackendId, we can participate in ProcSignal */
// 	ProcSignalInit(og_knl6->MyBackendId);

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
// 		og_knl8->XactIsoLevel = XACT_READ_COMMITTED;

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
// 		Assert(og_knl7->MyProcPort != NULL);
// 		PerformAuthentication(og_knl7->MyProcPort);
// 		InitializeSessionUserId(username, useroid);
// 		am_superuser = superuser();
// 	}

// 	/*
// 	 * If we're trying to shut down, only superusers can connect, and new
// 	 * replication connections are not allowed.
// 	 */
// 	if ((!am_superuser || am_walsender) &&
// 		og_knl7->MyProcPort != NULL &&
// 		og_knl7->MyProcPort->canAcceptConnections == CAC_WAITBACKUP)
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
// 		if (og_knl7->MyProcPort != NULL)
// 			process_startup_options(og_knl7->MyProcPort, am_superuser);

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
// 	if (og_knl7->MyProcPort != NULL)
// 		process_startup_options(og_knl7->MyProcPort, am_superuser);

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


void
cost_gather(GatherPath *path, PlannerInfo *root,
			RelOptInfo *rel, ParamPathInfo *param_info,
			double *rows)
{
	Cost		startup_cost = 0;
	Cost		run_cost = 0;

	/* Mark the path with the correct row estimate */
	if (rows)
		path->path.rows = *rows;
	else if (param_info)
		path->path.rows = param_info->ppi_rows;
	else
		path->path.rows = rel->rows;

	startup_cost = path->subpath->startup_cost;

	run_cost = path->subpath->total_cost - path->subpath->startup_cost;

	/* Parallel setup and communication cost. */
	startup_cost += parallel_setup_cost;
	run_cost += parallel_tuple_cost * path->path.rows;

	path->path.startup_cost = startup_cost;
	path->path.total_cost = (startup_cost + run_cost);
} 


bool
is_projection_capable_path(Path *path)
{
	/* Most plan types can project, so just list the ones that can't */
	switch (path->pathtype)
	{
		case T_Hash:
		case T_Material:
		case T_Sort:
		case T_Unique:
		case T_SetOp:
		case T_LockRows:
		case T_Limit:
		case T_ModifyTable:
		case T_MergeAppend:
		case T_RecursiveUnion:
			return false;
		case T_Append:

			/*
			 * Append can't project, but if it's being used to represent a
			 * dummy path, claim that it can project.  This prevents us from
			 * converting a rel from dummy to non-dummy status by applying a
			 * projection to its dummy path.
			 */
			return IS_DUMMY_PATH(path);
		default:
			break;
	}
	return true;
} 

ProjectionPath *
create_projection_path(PlannerInfo *root,
					   RelOptInfo *rel,
					   Path *subpath,
					   PathTarget *target)
{
	//tsdb 需要在nodetag里面加入T_ProjectionPath
	ProjectionPath *pathnode = makeNode(ProjectionPath);
	PathTarget *oldtarget = subpath->pathtarget;

	pathnode->path.pathtype = T_BaseResult;
	pathnode->path.parent = rel;
	pathnode->path.pathtarget = target;
	/* For now, assume we are above any joins, so no parameterization */
	pathnode->path.param_info = NULL;
	pathnode->path.parallel_aware = false;
	pathnode->path.parallel_safe = rel->consider_parallel &&
		subpath->parallel_safe &&
		!has_parallel_hazard((Node *) target->exprs, false);
	pathnode->path.parallel_workers = subpath->parallel_workers;
	/* Projection does not change the sort order */
	pathnode->path.pathkeys = subpath->pathkeys;

	pathnode->subpath = subpath;

	/*
	 * We might not need a separate Result node.  If the input plan node type
	 * can project, we can just tell it to project something else.  Or, if it
	 * can't project but the desired target has the same expression list as
	 * what the input will produce anyway, we can still give it the desired
	 * tlist (possibly changing its ressortgroupref labels, but nothing else).
	 * Note: in the latter case, create_projection_plan has to recheck our
	 * conclusion; see comments therein.
	 */
	if (is_projection_capable_path(subpath) ||
		equal(oldtarget->exprs, target->exprs))
	{
		/* No separate Result node needed */
		pathnode->dummypp = true;

		/*
		 * Set cost of plan as subpath's cost, adjusted for tlist replacement.
		 */
		pathnode->path.rows = subpath->rows;
		pathnode->path.startup_cost = subpath->startup_cost +
			(target->cost.startup - oldtarget->cost.startup);
		pathnode->path.total_cost = subpath->total_cost +
			(target->cost.startup - oldtarget->cost.startup) +
			(target->cost.per_tuple - oldtarget->cost.per_tuple) * subpath->rows;
	}
	else
	{
		/* We really do need the Result node */
		pathnode->dummypp = false;

		/*
		 * The Result node's cost is cpu_tuple_cost per row, plus the cost of
		 * evaluating the tlist.  There is no qual to worry about.
		 */
		pathnode->path.rows = subpath->rows;
		pathnode->path.startup_cost = subpath->startup_cost +
			target->cost.startup;
		pathnode->path.total_cost = subpath->total_cost +
			target->cost.startup +
			(u_sess->attr.attr_sql.cpu_tuple_cost + target->cost.per_tuple) * subpath->rows;
	}

	return pathnode;
} 

// //这个og没有
bool
has_parallel_hazard(Node *node, bool allow_restricted)
{
	has_parallel_hazard_arg context;

	context.allow_restricted = allow_restricted;
	return has_parallel_hazard_walker(node, &context);
} 

bool
pg_event_trigger_ownercheck(Oid et_oid, Oid roleid)
{
	HeapTuple	tuple;
	Oid			ownerId;

	/* Superusers bypass all permission checking. */
	if (superuser_arg(roleid))
		return true;

	tuple = SearchSysCache1(EVENTTRIGGEROID, ObjectIdGetDatum(et_oid));
	if (!HeapTupleIsValid(tuple))
		ereport(ERROR,
				(errcode(ERRCODE_UNDEFINED_OBJECT),
				 errmsg("event trigger with OID %u does not exist",
						et_oid)));

	ownerId = ((Form_pg_event_trigger) GETSTRUCT(tuple))->evtowner;

	ReleaseSysCache(tuple);

	return has_privs_of_role(roleid, ownerId);
} 



// bool
// get_object_namensp_unique(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->is_nsp_name_unique;
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


List *
EventCacheLookup(EventTriggerEvent event)
{
	EventTriggerCacheEntry *entry;

	if (EventTriggerCacheState != ETCS_VALID)
		BuildEventTriggerCache();
	entry =(EventTriggerCacheEntry *) hash_search(EventTriggerCache, &event, HASH_FIND, NULL);
	return entry != NULL ? entry->triggerlist : NULL;
} 

// IndexAmRoutine *
// GetIndexAmRoutine(Oid amhandler)
// {
// 	Datum		datum;
// 	IndexAmRoutine *routine;

// 	datum = OidFunctionCall0(amhandler);
// 	routine = (IndexAmRoutine *) DatumGetPointer(datum);

// 	if (routine == NULL || !IsA(routine, IndexAmRoutine))
// 		elog(ERROR, "index access method handler function %u did not return an IndexAmRoutine struct",
// 			 amhandler);

// 	return routine;
// } 
// void
// WaitForLockers(LOCKTAG heaplocktag, LOCKMODE lockmode)
// {
// 	List	   *l;

// 	l = list_make1(&heaplocktag);
// 	WaitForLockersMultiple(l, lockmode);
// 	list_free(l);
// } 

bool
has_bypassrls_privilege(Oid roleid)
{
	bool		result = false;
	HeapTuple	utup;

	/* Superusers bypass all permission checking. */
	if (superuser_arg(roleid))
		return true;

	utup = SearchSysCache1(AUTHOID, ObjectIdGetDatum(roleid));
	if (HeapTupleIsValid(utup))
	{
		//tsdb 需要在Form_pg_authid加入成员rolbypassrls
		result = ((Form_pg_authid) GETSTRUCT(utup))->rolbypassrls;
		ReleaseSysCache(utup);
	}
	return result;
} 

bool
InNoForceRLSOperation(void)
{
	return (u_sess->misc_cxt.SecurityRestrictionContext & SECURITY_NOFORCE_RLS) != 0;
} 

int
pg_qsort_strcmp(const void *a, const void *b)
{
	return strcmp(*(const char *const *) a, *(const char *const *) b);
} 

int
get_aggregate_argtypes(Aggref *aggref, Oid *inputTypes)
{
	int			numArguments = 0;
	ListCell   *lc;

	Assert(list_length(aggref->aggargtypes) <= FUNC_MAX_ARGS);

	foreach(lc, aggref->aggargtypes)
	{
		inputTypes[numArguments++] = lfirst_oid(lc);
	}

	return numArguments;
} 

// void
// vacuum(int options, RangeVar *relation, Oid relid, VacuumParams *params,
// 	   List *va_cols, BufferAccessStrategy bstrategy, bool isTopLevel)
// {
// 	const char *stmttype;
// 	volatile bool in_outer_xact,
// 				use_own_xacts;
// 	List	   *relations;
// 	static bool in_vacuum = false;

// 	Assert(params != NULL);

// 	stmttype = (options & VACOPT_VACUUM) ? "VACUUM" : "ANALYZE";

// 	/*
// 	 * We cannot run VACUUM inside a user transaction block; if we were inside
// 	 * a transaction, then our commit- and start-transaction-command calls
// 	 * would not have the intended effect!	There are numerous other subtle
// 	 * dependencies on this, too.
// 	 *
// 	 * ANALYZE (without VACUUM) can run either way.
// 	 */
// 	if (options & VACOPT_VACUUM)
// 	{
// 		PreventTransactionChain(isTopLevel, stmttype);
// 		in_outer_xact = false;
// 	}
// 	else
// 		in_outer_xact = IsInTransactionChain(isTopLevel);

// 	/*
// 	 * Due to static variables vac_context, anl_context and vac_strategy,
// 	 * vacuum() is not reentrant.  This matters when VACUUM FULL or ANALYZE
// 	 * calls a hostile index expression that itself calls ANALYZE.
// 	 */
// 	if (in_vacuum)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("%s cannot be executed from VACUUM or ANALYZE",
// 						stmttype)));

// 	/*
// 	 * Sanity check DISABLE_PAGE_SKIPPING option.
// 	 */
// 	if ((options & VACOPT_FULL) != 0 &&
// 		(options & VACOPT_DISABLE_PAGE_SKIPPING) != 0)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 				 errmsg("VACUUM option DISABLE_PAGE_SKIPPING cannot be used with FULL")));

// 	/*
// 	 * Send info about dead objects to the statistics collector, unless we are
// 	 * in autovacuum --- autovacuum.c does this for itself.
// 	 */
// 	if ((options & VACOPT_VACUUM) && !IsAutoVacuumWorkerProcess())
// 		pgstat_vacuum_stat();

// 	/*
// 	 * Create special memory context for cross-transaction storage.
// 	 *
// 	 * Since it is a child of PortalContext, it will go away eventually even
// 	 * if we suffer an error; there's no need for special abort cleanup logic.
// 	 */
// 	vac_context = AllocSetContextCreate(PortalContext,
// 										"Vacuum",
// 										ALLOCSET_DEFAULT_SIZES);

// 	/*
// 	 * If caller didn't give us a buffer strategy object, make one in the
// 	 * cross-transaction memory context.
// 	 */
// 	if (bstrategy == NULL)
// 	{
// 		MemoryContext old_context = MemoryContextSwitchTo(vac_context);

// 		bstrategy = GetAccessStrategy(BAS_VACUUM);
// 		MemoryContextSwitchTo(old_context);
// 	}
// 	og_knl10->vac_strategy = bstrategy;

// 	/*
// 	 * Build list of relations to process, unless caller gave us one. (If we
// 	 * build one, we put it in vac_context for safekeeping.)
// 	 */
// 	relations = get_rel_oids(relid, relation);

// 	/*
// 	 * Decide whether we need to start/commit our own transactions.
// 	 *
// 	 * For VACUUM (with or without ANALYZE): always do so, so that we can
// 	 * release locks as soon as possible.  (We could possibly use the outer
// 	 * transaction for a one-table VACUUM, but handling TOAST tables would be
// 	 * problematic.)
// 	 *
// 	 * For ANALYZE (no VACUUM): if inside a transaction block, we cannot
// 	 * start/commit our own transactions.  Also, there's no need to do so if
// 	 * only processing one relation.  For multiple relations when not within a
// 	 * transaction block, and also in an autovacuum worker, use own
// 	 * transactions so we can release locks sooner.
// 	 */
// 	if (options & VACOPT_VACUUM)
// 		use_own_xacts = true;
// 	else
// 	{
// 		Assert(options & VACOPT_ANALYZE);
// 		if (IsAutoVacuumWorkerProcess())
// 			use_own_xacts = true;
// 		else if (in_outer_xact)
// 			use_own_xacts = false;
// 		else if (list_length(relations) > 1)
// 			use_own_xacts = true;
// 		else
// 			use_own_xacts = false;
// 	}

// 	/*
// 	 * vacuum_rel expects to be entered with no transaction active; it will
// 	 * start and commit its own transaction.  But we are called by an SQL
// 	 * command, and so we are executing inside a transaction already. We
// 	 * commit the transaction started in PostgresMain() here, and start
// 	 * another one before exiting to match the commit waiting for us back in
// 	 * PostgresMain().
// 	 */
// 	if (use_own_xacts)
// 	{
// 		Assert(!in_outer_xact);

// 		/* ActiveSnapshot is not set by autovacuum */
// 		if (ActiveSnapshotSet())
// 			PopActiveSnapshot();

// 		/* matches the StartTransaction in PostgresMain() */
// 		CommitTransactionCommand();
// 	}

// 	/* Turn vacuum cost accounting on or off */
// 	PG_TRY();
// 	{
// 		ListCell   *cur;

// 		in_vacuum = true;
// 		VacuumCostActive = (VacuumCostDelay > 0);
// 		VacuumCostBalance = 0;
// 		VacuumPageHit = 0;
// 		VacuumPageMiss = 0;
// 		VacuumPageDirty = 0;

// 		/*
// 		 * Loop to process each selected relation.
// 		 */
// 		foreach(cur, relations)
// 		{
// 			Oid			relid = lfirst_oid(cur);

// 			if (options & VACOPT_VACUUM)
// 			{
// 				if (!vacuum_rel(relid, relation, options, params))
// 					continue;
// 			}

// 			if (options & VACOPT_ANALYZE)
// 			{
// 				/*
// 				 * If using separate xacts, start one for analyze. Otherwise,
// 				 * we can use the outer transaction.
// 				 */
// 				if (use_own_xacts)
// 				{
// 					StartTransactionCommand();
// 					/* functions in indexes may want a snapshot set */
// 					PushActiveSnapshot(GetTransactionSnapshot());
// 				}

// 				analyze_rel(relid, relation, options, params,
// 							va_cols, in_outer_xact, og_knl10->vac_strategy);

// 				if (use_own_xacts)
// 				{
// 					PopActiveSnapshot();
// 					CommitTransactionCommand();
// 				}
// 			}
// 		}
// 	}
// 	PG_CATCH();
// 	{
// 		in_vacuum = false;
// 		VacuumCostActive = false;
// 		PG_RE_THROW();
// 	}
// 	PG_END_TRY();

// 	in_vacuum = false;
// 	VacuumCostActive = false;

// 	/*
// 	 * Finish up processing.
// 	 */
// 	if (use_own_xacts)
// 	{
// 		/* here, we are not in a transaction */

// 		/*
// 		 * This matches the CommitTransaction waiting for us in
// 		 * PostgresMain().
// 		 */
// 		StartTransactionCommand();
// 	}

// 	if ((options & VACOPT_VACUUM) && !IsAutoVacuumWorkerProcess())
// 	{
// 		/*
// 		 * Update pg_database.datfrozenxid, and truncate pg_clog if possible.
// 		 * (autovacuum.c does this for itself.)
// 		 */
// 		vac_update_datfrozenxid();
// 	}

// 	/*
// 	 * Clean up working storage --- note we must do this after
// 	 * StartTransactionCommand, else we might be trying to delete the active
// 	 * context!
// 	 */
// 	MemoryContextDelete(vac_context);
// 	vac_context = NULL;
// } 

// Relation
// heap_create(const char *relname,
// 			Oid relnamespace,
// 			Oid reltablespace,
// 			Oid relid,
// 			Oid relfilenode,
// 			TupleDesc tupDesc,
// 			char relkind,
// 			char relpersistence,
// 			bool shared_relation,
// 			bool mapped_relation,
// 			bool allow_system_table_mods)
// {
// 	bool		create_storage;
// 	Relation	rel;

// 	/* The caller must have provided an OID for the relation. */
// 	Assert(OidIsValid(relid));

// 	/*
// 	 * Don't allow creating relations in pg_catalog directly, even though it
// 	 * is allowed to move user defined relations there. Semantics with search
// 	 * paths including pg_catalog are too confusing for now.
// 	 *
// 	 * But allow creating indexes on relations in pg_catalog even if
// 	 * allow_system_table_mods = off, upper layers already guarantee it's on a
// 	 * user defined relation, not a system one.
// 	 */
// 	if (!allow_system_table_mods &&
// 		((IsSystemNamespace(relnamespace) && relkind != RELKIND_INDEX) ||
// 		 IsToastNamespace(relnamespace)) &&
// 		IsNormalProcessingMode())
// 		ereport(ERROR,
// 				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
// 				 errmsg("permission denied to create \"%s.%s\"",
// 						get_namespace_name(relnamespace), relname),
// 		errdetail("System catalog modifications are currently disallowed.")));

// 	/*
// 	 * Decide if we need storage or not, and handle a couple other special
// 	 * cases for particular relkinds.
// 	 */
// 	switch (relkind)
// 	{
// 		case RELKIND_VIEW:
// 		case RELKIND_COMPOSITE_TYPE:
// 		case RELKIND_FOREIGN_TABLE:
// 			create_storage = false;

// 			/*
// 			 * Force reltablespace to zero if the relation has no physical
// 			 * storage.  This is mainly just for cleanliness' sake.
// 			 */
// 			reltablespace = InvalidOid;
// 			break;
// 		case RELKIND_SEQUENCE:
// 			create_storage = true;

// 			/*
// 			 * Force reltablespace to zero for sequences, since we don't
// 			 * support moving them around into different tablespaces.
// 			 */
// 			reltablespace = InvalidOid;
// 			break;
// 		default:
// 			create_storage = true;
// 			break;
// 	}

// 	/*
// 	 * Unless otherwise requested, the physical ID (relfilenode) is initially
// 	 * the same as the logical ID (OID).  When the caller did specify a
// 	 * relfilenode, it already exists; do not attempt to create it.
// 	 */
// 	if (OidIsValid(relfilenode))
// 		create_storage = false;
// 	else
// 		relfilenode = relid;

// 	/*
// 	 * Never allow a pg_class entry to explicitly specify the database's
// 	 * default tablespace in reltablespace; force it to zero instead. This
// 	 * ensures that if the database is cloned with a different default
// 	 * tablespace, the pg_class entry will still match where CREATE DATABASE
// 	 * will put the physically copied relation.
// 	 *
// 	 * Yes, this is a bit of a hack.
// 	 */
// 	if (reltablespace == MyDatabaseTableSpace)
// 		reltablespace = InvalidOid;

// 	/*
// 	 * build the relcache entry.
// 	 */
// 	rel = RelationBuildLocalRelation(relname,
// 									 relnamespace,
// 									 tupDesc,
// 									 relid,
// 									 relfilenode,
// 									 reltablespace,
// 									 shared_relation,
// 									 mapped_relation,
// 									 relpersistence,
// 									 relkind);

// 	/*
// 	 * Have the storage manager create the relation's disk file, if needed.
// 	 *
// 	 * We only create the main fork here, other forks will be created on
// 	 * demand.
// 	 */
// 	if (create_storage)
// 	{
// 		RelationOpenSmgr(rel);
// 		RelationCreateStorage(rel->rd_node, relpersistence);
// 	}

// 	return rel;
// } 

void
add_new_column_to_pathtarget(PathTarget *target, Expr *expr)
{
	if (!list_member(target->exprs, expr))
		add_column_to_pathtarget(target, expr, 0);
}

void
SpeculativeInsertionWait(TransactionId xid, uint32 token)
{
	LOCKTAG		tag;

	//需要在LockTagType加入成员LOCKTAG_SPECULATIVE_TOKEN 
	SET_LOCKTAG_SPECULATIVE_INSERTION(tag, xid, token);

	Assert(TransactionIdIsValid(xid));
	Assert(token != 0);

	(void) LockAcquire(&tag, ShareLock, false, false);
	LockRelease(&tag, ShareLock, false);
} 

// int
// errtableconstraint(Relation rel, const char *conname)
// {
// 	errtable(rel);
// 	err_generic_string(PG_DIAG_CONSTRAINT_NAME, conname);

// 	return 0;					/* return value does not matter */
// } 

// Relids
// find_childrel_parents(PlannerInfo *root, RelOptInfo *rel)
// {
// 	Relids		result = NULL;

// 	do
// 	{
// 		AppendRelInfo *appinfo = find_childrel_appendrelinfo(root, rel);
// 		Index		prelid = appinfo->parent_relid;

// 		result = bms_add_member(result, prelid);

// 		/* traverse up to the parent rel, loop if it's also a child rel */
// 		rel = find_base_rel(root, prelid);
// 	} while (rel->reloptkind == RELOPT_OTHER_MEMBER_REL);

// 	Assert(rel->reloptkind == RELOPT_BASEREL);

// 	return result;
// } 

size_t
pvsnprintf(char *buf, size_t len, const char *fmt, va_list args)
{
	int			nprinted;

	Assert(len > 0);

	errno = 0;

	/*
	 * Assert check here is to catch buggy vsnprintf that overruns the
	 * specified buffer length.  Solaris 7 in 64-bit mode is an example of a
	 * platform with such a bug.
	 */
#ifdef USE_ASSERT_CHECKING
	buf[len - 1] = '\0';
#endif

	nprinted = vsnprintf(buf, len, fmt, args);

	Assert(buf[len - 1] == '\0');

	/*
	 * If vsnprintf reports an error other than ENOMEM, fail.  The possible
	 * causes of this are not user-facing errors, so elog should be enough.
	 */
	if (nprinted < 0 && errno != 0 && errno != ENOMEM)
	{
#ifndef FRONTEND
		elog(ERROR, "vsnprintf failed: %m");
#else
		fprintf(stderr, "vsnprintf failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
#endif
	}

	/*
	 * Note: some versions of vsnprintf return the number of chars actually
	 * stored, not the total space needed as C99 specifies.  And at least one
	 * returns -1 on failure.  Be conservative about believing whether the
	 * print worked.
	 */
	if (nprinted >= 0 && (size_t) nprinted < len - 1)
	{
		/* Success.  Note nprinted does not include trailing null. */
		return (size_t) nprinted;
	}

	if (nprinted >= 0 && (size_t) nprinted > len)
	{
		/*
		 * This appears to be a C99-compliant vsnprintf, so believe its
		 * estimate of the required space.  (If it's wrong, the logic will
		 * still work, but we may loop multiple times.)  Note that the space
		 * needed should be only nprinted+1 bytes, but we'd better allocate
		 * one more than that so that the test above will succeed next time.
		 *
		 * In the corner case where the required space just barely overflows,
		 * fall through so that we'll error out below (possibly after
		 * looping).
		 */
		if ((size_t) nprinted <= MaxAllocSize - 2)
			return nprinted + 2;
	}

	/*
	 * Buffer overrun, and we don't know how much space is needed.  Estimate
	 * twice the previous buffer size, but not more than MaxAllocSize; if we
	 * are already at MaxAllocSize, choke.  Note we use this palloc-oriented
	 * overflow limit even when in frontend.
	 */
	if (len >= MaxAllocSize)
	{
#ifndef FRONTEND
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("out of memory")));
#else
		fprintf(stderr, _("out of memory\n"));
		exit(EXIT_FAILURE);
#endif
	}

	if (len >= MaxAllocSize / 2)
		return MaxAllocSize;

	return len * 2;
} 

//这个里面有静态函数在og里面,之后再讨论
void
slot_getsomeattrs(TupleTableSlot *slot, int attnum)
{
	HeapTuple	tuple;
	int			attno;

	/* Quick out if we have 'em all already */
	if (slot->tts_nvalid >= attnum)
		return;

	/* Check for caller error */
	if (attnum <= 0 || attnum > slot->tts_tupleDescriptor->natts)
		elog(ERROR, "invalid attribute number %d", attnum);

	/*
	 * otherwise we had better have a physical tuple (tts_nvalid should equal
	 * natts in all virtual-tuple cases)
	 */
	tuple = (HeapTuple)slot->tts_tuple;
	if (tuple == NULL)			/* internal error */
		elog(ERROR, "cannot extract attribute from empty tuple slot");

	/*
	 * load up any slots available from physical tuple
	 */
	//tsdb加了强制类型转化
	attno = HeapTupleHeaderGetNatts(tuple->t_data,(TupleDesc)tuple);
	attno = Min(attno, attnum);

	slot_deform_tuple(slot, attno);

	/*
	 * If tuple doesn't have all the atts indicated by tupleDesc, read the
	 * rest as null
	 */
	for (; attno < attnum; attno++)
	{
		slot->tts_values[attno] = (Datum) 0;
		slot->tts_isnull[attno] = true;
	}
	slot->tts_nvalid = attnum;
} 

// HeapTuple
// index_fetch_heap(IndexScanDesc scan)
// {
// 	ItemPointer tid = &scan->xs_ctup.t_self;
// 	bool		all_dead = false;
// 	bool		got_heap_tuple;

// 	/* We can skip the buffer-switching logic if we're in mid-HOT chain. */
// 	if (!scan->xs_continue_hot)
// 	{
// 		/* Switch to correct buffer if we don't have it already */
// 		Buffer		prev_buf = scan->xs_cbuf;

// 		scan->xs_cbuf = ReleaseAndReadBuffer(scan->xs_cbuf,
// 											 scan->heapRelation,
// 											 ItemPointerGetBlockNumber(tid));

// 		/*
// 		 * Prune page, but only if we weren't already on this page
// 		 */
// 		if (prev_buf != scan->xs_cbuf)
// 			heap_page_prune_opt(scan->heapRelation, scan->xs_cbuf);
// 	}

// 	/* Obtain share-lock on the buffer so we can examine visibility */
// 	LockBuffer(scan->xs_cbuf, BUFFER_LOCK_SHARE);
// 	got_heap_tuple = heap_hot_search_buffer(tid, scan->heapRelation,
// 											scan->xs_cbuf,
// 											scan->xs_snapshot,
// 											&scan->xs_ctup,
// 											&all_dead,
// 											!scan->xs_continue_hot);
// 	LockBuffer(scan->xs_cbuf, BUFFER_LOCK_UNLOCK);

// 	if (got_heap_tuple)
// 	{
// 		/*
// 		 * Only in a non-MVCC snapshot can more than one member of the HOT
// 		 * chain be visible.
// 		 */
// 		scan->xs_continue_hot = !IsMVCCSnapshot(scan->xs_snapshot);
// 		pgstat_count_heap_fetch(scan->indexRelation);
// 		return &scan->xs_ctup;
// 	}

// 	/* We've reached the end of the HOT chain. */
// 	scan->xs_continue_hot = false;

// 	/*
// 	 * If we scanned a whole HOT chain and found only dead tuples, tell index
// 	 * AM to kill its entry for that TID (this will take effect in the next
// 	 * amgettuple call, in index_getnext_tid).  We do not do this when in
// 	 * recovery because it may violate MVCC to do so.  See comments in
// 	 * RelationGetIndexScan().
// 	 */
// 	if (!scan->xactStartedInRecovery)
// 		scan->kill_prior_tuple = all_dead;

// 	return NULL;
// } 

// bool
// HeapTupleHeaderIsOnlyLocked(HeapTupleHeader tuple)
// {
// 	TransactionId xmax;

// 	/* if there's no valid Xmax, then there's obviously no update either */
// 	if (tuple->t_infomask & HEAP_XMAX_INVALID)
// 		return true;

// 	if (tuple->t_infomask & HEAP_XMAX_LOCK_ONLY)
// 		return true;

// 	/* invalid xmax means no update */
// 	if (!TransactionIdIsValid(HeapTupleHeaderGetRawXmax_tsdb(tuple)))
// 		return true;

// 	/*
// 	 * if HEAP_XMAX_LOCK_ONLY is not set and not a multi, then this must
// 	 * necessarily have been updated
// 	 */
// 	if (!(tuple->t_infomask & HEAP_XMAX_IS_MULTI))
// 		return false;

// 	/* ... but if it's a multi, then perhaps the updating Xid aborted. */
// 	xmax = HeapTupleHeaderGetUpdateXid_tsdb(tuple,tuple->t_data);

// 	/* not LOCKED_ONLY, so it has to have an xmax */
// 	Assert(TransactionIdIsValid(xmax));

// 	if (TransactionIdIsCurrentTransactionId(xmax))
// 		return false;
// 	if (TransactionIdIsInProgress(xmax))
// 		return false;
// 	if (TransactionIdDidCommit(xmax))
// 		return false;

// 	/*
// 	 * not current, not in progress, not committed -- must have aborted or
// 	 * crashed
// 	 */
// 	return true;
// }

// bool
// HeapTupleHeaderIsOnlyLocked(HeapTupleHeader tuple)
// {
// 	TransactionId xmax;

// 	/* if there's no valid Xmax, then there's obviously no update either */
// 	if (tuple->t_infomask & HEAP_XMAX_INVALID)
// 		return true;

// 	if (tuple->t_infomask & HEAP_XMAX_LOCK_ONLY)
// 		return true;

// 	/* invalid xmax means no update */
// 	if (!TransactionIdIsValid(HeapTupleHeaderGetRawXmax_tsdb(tuple)))
// 		return true;

// 	/*
// 	 * if HEAP_XMAX_LOCK_ONLY is not set and not a multi, then this must
// 	 * necessarily have been updated
// 	 */
// 	if (!(tuple->t_infomask & HEAP_XMAX_IS_MULTI))
// 		return false;

// 	/* ... but if it's a multi, then perhaps the updating Xid aborted. */
// 	xmax = HeapTupleGetUpdateXid(tuple);

// 	/* not LOCKED_ONLY, so it has to have an xmax */
// 	Assert(TransactionIdIsValid(xmax));

// 	if (TransactionIdIsCurrentTransactionId(xmax))
// 		return false;
// 	if (TransactionIdIsInProgress(xmax))
// 		return false;
// 	if (TransactionIdDidCommit(xmax))
// 		return false;

// 	/*
// 	 * not current, not in progress, not committed -- must have aborted or
// 	 * crashed
// 	 */
// 	return true;
// }

// bytea *
// view_reloptions(Datum reloptions, bool validate)
// {
// 	relopt_value *options;
// 	ViewOptions *vopts;
// 	int			numoptions;
// 	static const relopt_parse_elt tab[] = {
// 		{"security_barrier", RELOPT_TYPE_BOOL,
// 		offsetof(ViewOptions, security_barrier)},
// 		{"check_option", RELOPT_TYPE_STRING,
// 		offsetof(ViewOptions, check_option_offset)}
// 	};

// 	options = parseRelOptions(reloptions, validate, RELOPT_KIND_VIEW, &numoptions);

// 	/* if none set, we're done */
// 	if (numoptions == 0)
// 		return NULL;

// 	vopts =(ViewOptions*) allocateReloptStruct(sizeof(ViewOptions), options, numoptions);

// 	fillRelOptions((void *) vopts, sizeof(ViewOptions), options, numoptions,
// 				   validate, tab, lengthof(tab));

// 	pfree(options);

// 	return (bytea *) vopts;
// }

// bool binary_upgrade_is_next_part_toast_pg_class_oid_valid()
// {
//     if (NULL == u_sess->upg_cxt.binary_upgrade_next_part_toast_pg_class_oid) {
//         return false;
//     }

//     if (u_sess->upg_cxt.binary_upgrade_cur_part_toast_pg_class_oid >=
//         u_sess->upg_cxt.binary_upgrade_max_part_toast_pg_class_oid) {
//         return false;
//     }

//     if (!OidIsValid(u_sess->upg_cxt.binary_upgrade_next_part_toast_pg_class_oid
//                         [u_sess->upg_cxt.binary_upgrade_cur_part_toast_pg_class_oid])) {
//         return false;
//     }

//     return true;
// } 

// //storage/lmgr.h
// // void
// // LockTuple(Relation relation, ItemPointer tid, LOCKMODE lockmode,int tsdb = 0)
// // {
// // 	LOCKTAG		tag;

// // 	SET_LOCKTAG_TUPLE(tag,
// // 					  relation->rd_lockInfo.lockRelId.dbId,
// // 					  relation->rd_lockInfo.lockRelId.relId,
// // 					  ItemPointerGetBlockNumber(tid),
// // 					  ItemPointerGetOffsetNumber(tid));

// // 	(void) LockAcquire(&tag, lockmode, false, false);
// // } 

// bool
// visibilitymap_clear(Relation rel, BlockNumber heapBlk, Buffer buf, uint8 flags)
// {
// 	BlockNumber mapBlock = HEAPBLK_TO_MAPBLOCK(heapBlk);
// 	int			mapByte = HEAPBLK_TO_MAPBYTE(heapBlk);
// 	int			mapOffset = HEAPBLK_TO_OFFSET(heapBlk);
// 	uint8		mask = flags << mapOffset;
// 	char	   *map;
// 	bool		cleared = false;

// 	Assert(flags & VISIBILITYMAP_VALID_BITS);

// #ifdef TRACE_VISIBILITYMAP
// 	elog(DEBUG1, "vm_clear %s %d", RelationGetRelationName(rel), heapBlk);
// #endif

// 	if (!BufferIsValid(buf) || BufferGetBlockNumber(buf) != mapBlock)
// 		elog(ERROR, "wrong buffer passed to visibilitymap_clear");

// 	LockBuffer(buf, BUFFER_LOCK_EXCLUSIVE);
// 	map = PageGetContents(BufferGetPage(buf));

// 	if (map[mapByte] & mask)
// 	{
// 		map[mapByte] &= ~mask;

// 		MarkBufferDirty(buf);
// 		cleared = true;
// 	}

// 	LockBuffer(buf, BUFFER_LOCK_UNLOCK);

// 	return cleared;
// } 
// bool
// dsm_impl_op(dsm_op op, dsm_handle handle, Size request_size,
// 			void **impl_private, void **mapped_address, Size *mapped_size,
// 			int elevel)
// {
// 	Assert(op == DSM_OP_CREATE || op == DSM_OP_RESIZE || request_size == 0);
// 	Assert((op != DSM_OP_CREATE && op != DSM_OP_ATTACH) ||
// 		   (*mapped_address == NULL && *mapped_size == 0));

// 	switch (dynamic_shared_memory_type)
// 	{
// #ifdef USE_DSM_POSIX
// 		case DSM_IMPL_POSIX:
// 			return dsm_impl_posix(op, handle, request_size, impl_private,
// 								  mapped_address, mapped_size, elevel);
// #endif
// #ifdef USE_DSM_SYSV
// 		case DSM_IMPL_SYSV:
// 			return dsm_impl_sysv(op, handle, request_size, impl_private,
// 								 mapped_address, mapped_size, elevel);
// #endif
// #ifdef USE_DSM_WINDOWS
// 		case DSM_IMPL_WINDOWS:
// 			return dsm_impl_windows(op, handle, request_size, impl_private,
// 									mapped_address, mapped_size, elevel);
// #endif
// #ifdef USE_DSM_MMAP
// 		case DSM_IMPL_MMAP:
// 			return dsm_impl_mmap(op, handle, request_size, impl_private,
// 								 mapped_address, mapped_size, elevel);
// #endif
// 		default:
// 			elog(ERROR, "unexpected dynamic shared memory type: %d",
// 				 dynamic_shared_memory_type);
// 			return false;
// 	}
// }
void
ResourceOwnerEnlargeDSMs(ResourceOwner owner)
{
	ResourceArrayEnlarge(&(owner->dsmarr));
}

void
ResourceOwnerRememberDSM(ResourceOwner owner, dsm_segment *seg)
{
	ResourceArrayAdd(&(owner->dsmarr), PointerGetDatum(seg));
} 

void
ResourceOwnerForgetDSM(ResourceOwner owner, dsm_segment *seg)
{
	if (!ResourceArrayRemove(&(owner->dsmarr), PointerGetDatum(seg)))
		elog(ERROR, "dynamic shared memory segment %u is not owned by resource owner %s",
			 dsm_segment_handle(seg), owner->name);
} 

// void
// WaitForLockersMultiple(List *locktags, LOCKMODE lockmode)
// {
// 	List	   *holders = NIL;
// 	ListCell   *lc;

// 	/* Done if no locks to wait for */
// 	if (list_length(locktags) == 0)
// 		return;

// 	/* Collect the transactions we need to wait on */
// 	foreach(lc, locktags)
// 	{
// 		LOCKTAG    *locktag =(LOCKTAG    *) lfirst(lc);

// 		holders = lappend(holders, GetLockConflicts(locktag, lockmode));
// 	}

// 	/*
// 	 * Note: GetLockConflicts() never reports our own xid, hence we need not
// 	 * check for that.  Also, prepared xacts are not reported, which is fine
// 	 * since they certainly aren't going to do anything anymore.
// 	 */

// 	/* Finally wait for each such transaction to complete */
// 	foreach(lc, holders)
// 	{
// 		VirtualTransactionId *lockholders =(VirtualTransactionId *) lfirst(lc);

// 		while (VirtualTransactionIdIsValid(*lockholders))
// 		{
// 			VirtualXactLock(*lockholders, true);
// 			lockholders++;
// 		}
// 	}

// 	list_free_deep(holders);
// }

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

void
CacheRegisterSyscacheCallback(int cacheid,
							  SyscacheCallbackFunction func,
							  Datum arg)
{
	knl_u_inval_context *inval_cxt = &t_thrd.lsc_cxt.lsc->inval_cxt;
	if (inval_cxt->syscache_callback_count >= MAX_SYSCACHE_CALLBACKS)
		elog(FATAL, "out of syscache_callback_list slots");

	inval_cxt->syscache_callback_list[inval_cxt->syscache_callback_count].id = cacheid;
	inval_cxt->syscache_callback_list[inval_cxt->syscache_callback_count].function = func;
	inval_cxt->syscache_callback_list[inval_cxt->syscache_callback_count].arg = arg;

	++inval_cxt->syscache_callback_count;
}

// int
// errtable(Relation rel)
// {
// 	err_generic_string(PG_DIAG_SCHEMA_NAME,
// 					   get_namespace_name(RelationGetNamespace(rel)));
// 	err_generic_string(PG_DIAG_TABLE_NAME, RelationGetRelationName(rel));

// 	return 0;					/* return value does not matter */
// }

// int
// err_generic_string(int field, const char *str)
// {
// 	ErrorData  *edata = &errordata[errordata_stack_depth];

// 	/* we don't bother incrementing recursion_depth */
// 	CHECK_STACK_DEPTH();

// 	switch (field)
// 	{
// 		case PG_DIAG_SCHEMA_NAME:
// 			set_errdata_field(edata->assoc_context, &edata->schema_name, str);
// 			break;
// 		case PG_DIAG_TABLE_NAME:
// 			set_errdata_field(edata->assoc_context, &edata->table_name, str);
// 			break;
// 		case PG_DIAG_COLUMN_NAME:
// 			set_errdata_field(edata->assoc_context, &edata->column_name, str);
// 			break;
// 		case PG_DIAG_DATATYPE_NAME:
// 			set_errdata_field(edata->assoc_context, &edata->datatype_name, str);
// 			break;
// 		case PG_DIAG_CONSTRAINT_NAME:
// 			set_errdata_field(edata->assoc_context, &edata->constraint_name, str);
// 			break;
// 		default:
// 			elog(ERROR, "unsupported ErrorData field id: %d", field);
// 			break;
// 	}

// 	return 0;					/* return value does not matter */
// }

// void
// NewHeapCreateToastTable(Oid relOid, Datum reloptions, LOCKMODE lockmode,
// 						Oid OIDOldToast)
// {
// 	CheckAndCreateToastTable(relOid, reloptions, lockmode, false, OIDOldToast);
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

// int
// MultiXactMemberFreezeThreshold(void)
// {
// 	MultiXactOffset members;
// 	uint32		multixacts;
// 	uint32		victim_multixacts;
// 	double		fraction;

// 	/* If we can't determine member space utilization, assume the worst. */
// 	if (!ReadMultiXactCounts(&multixacts, &members))
// 		return 0;

// 	/* If member space utilization is low, no special action is required. */
// 	if (members <= MULTIXACT_MEMBER_SAFE_THRESHOLD)
// 		return autovacuum_multixact_freeze_max_age;

// 	/*
// 	 * Compute a target for relminmxid advancement.  The number of multixacts
// 	 * we try to eliminate from the system is based on how far we are past
// 	 * MULTIXACT_MEMBER_SAFE_THRESHOLD.
// 	 */
// 	fraction = (double) (members - MULTIXACT_MEMBER_SAFE_THRESHOLD) /
// 		(MULTIXACT_MEMBER_DANGER_THRESHOLD - MULTIXACT_MEMBER_SAFE_THRESHOLD);
// 	victim_multixacts = multixacts * fraction;

// 	/* fraction could be > 1.0, but lowest possible freeze age is zero */
// 	if (victim_multixacts > multixacts)
// 		return 0;
// 	return multixacts - victim_multixacts;
// }

// //commands/vacuum.h
// // void
// // lazy_vacuum_rel(Relation onerel, int options, VacuumParams *params,
// // 				BufferAccessStrategy bstrategy)
// // {
// // 	LVRelStats *vacrelstats;
// // 	Relation   *Irel;
// // 	int			nindexes;
// // 	PGRUsage	ru0;
// // 	TimestampTz starttime = 0;
// // 	long		secs;
// // 	int			usecs;
// // 	double		read_rate,
// // 				write_rate;
// // 	bool		aggressive;		/* should we scan all unfrozen pages? */
// // 	bool		scanned_all_unfrozen;	/* actually scanned all such pages? */
// // 	TransactionId xidFullScanLimit;
// // 	MultiXactId mxactFullScanLimit;
// // 	BlockNumber new_rel_pages;
// // 	double		new_rel_tuples;
// // 	BlockNumber new_rel_allvisible;
// // 	double		new_live_tuples;
// // 	TransactionId new_frozen_xid;
// // 	MultiXactId new_min_multi;

// // 	Assert(params != NULL);

// // 	/* measure elapsed time iff autovacuum logging requires it */
// // 	if (IsAutoVacuumWorkerProcess() && params->log_min_duration >= 0)
// // 	{
// // 		pg_rusage_init(&ru0);
// // 		starttime = GetCurrentTimestamp();
// // 	}

// // 	if (options & VACOPT_VERBOSE)
// // 		elevel = INFO;
// // 	else
// // 		elevel = DEBUG2;

// // 	pgstat_progress_start_command(PROGRESS_COMMAND_VACUUM,
// // 								  RelationGetRelid(onerel));

// // 	vac_strategy = bstrategy;

// // 	vacuum_set_xid_limits(onerel,
// // 						  params->freeze_min_age,
// // 						  params->freeze_table_age,
// // 						  params->multixact_freeze_min_age,
// // 						  params->multixact_freeze_table_age,
// // 						  &OldestXmin, &FreezeLimit, &xidFullScanLimit,
// // 						  &MultiXactCutoff, &mxactFullScanLimit);

// // 	/*
// // 	 * We request an aggressive scan if the table's frozen Xid is now older
// // 	 * than or equal to the requested Xid full-table scan limit; or if the
// // 	 * table's minimum MultiXactId is older than or equal to the requested
// // 	 * mxid full-table scan limit; or if DISABLE_PAGE_SKIPPING was specified.
// // 	 */
// // 	aggressive = TransactionIdPrecedesOrEquals(onerel->rd_rel->relfrozenxid,
// // 											   xidFullScanLimit);
// // 	aggressive |= MultiXactIdPrecedesOrEquals(onerel->rd_rel->relminmxid,
// // 											  mxactFullScanLimit);
// // 	if (options & VACOPT_DISABLE_PAGE_SKIPPING)
// // 		aggressive = true;

// // 	vacrelstats = (LVRelStats *) palloc0(sizeof(LVRelStats));

// // 	vacrelstats->old_rel_pages = onerel->rd_rel->relpages;
// // 	vacrelstats->old_rel_tuples = onerel->rd_rel->reltuples;
// // 	vacrelstats->num_index_scans = 0;
// // 	vacrelstats->pages_removed = 0;
// // 	vacrelstats->lock_waiter_detected = false;

// // 	/* Open all indexes of the relation */
// // 	vac_open_indexes(onerel, RowExclusiveLock, &nindexes, &Irel);
// // 	vacrelstats->hasindex = (nindexes > 0);

// // 	/* Do the vacuuming */
// // 	lazy_scan_heap(onerel, options, vacrelstats, Irel, nindexes, aggressive);

// // 	/* Done with indexes */
// // 	vac_close_indexes(nindexes, Irel, NoLock);

// // 	/*
// // 	 * Compute whether we actually scanned the all unfrozen pages. If we did,
// // 	 * we can adjust relfrozenxid and relminmxid.
// // 	 *
// // 	 * NB: We need to check this before truncating the relation, because that
// // 	 * will change ->rel_pages.
// // 	 */
// // 	if ((vacrelstats->scanned_pages + vacrelstats->frozenskipped_pages)
// // 		< vacrelstats->rel_pages)
// // 	{
// // 		Assert(!aggressive);
// // 		scanned_all_unfrozen = false;
// // 	}
// // 	else
// // 		scanned_all_unfrozen = true;

// // 	/*
// // 	 * Optionally truncate the relation.
// // 	 */
// // 	if (should_attempt_truncation(vacrelstats))
// // 		lazy_truncate_heap(onerel, vacrelstats);

// // 	/* Report that we are now doing final cleanup */
// // 	pgstat_progress_update_param(PROGRESS_VACUUM_PHASE,
// // 								 PROGRESS_VACUUM_PHASE_FINAL_CLEANUP);

// // 	/* Vacuum the Free Space Map */
// // 	FreeSpaceMapVacuum(onerel);

// // 	/*
// // 	 * Update statistics in pg_class.
// // 	 *
// // 	 * A corner case here is that if we scanned no pages at all because every
// // 	 * page is all-visible, we should not update relpages/reltuples, because
// // 	 * we have no new information to contribute.  In particular this keeps us
// // 	 * from replacing relpages=reltuples=0 (which means "unknown tuple
// // 	 * density") with nonzero relpages and reltuples=0 (which means "zero
// // 	 * tuple density") unless there's some actual evidence for the latter.
// // 	 *
// // 	 * It's important that we use tupcount_pages and not scanned_pages for the
// // 	 * check described above; scanned_pages counts pages where we could not
// // 	 * get cleanup lock, and which were processed only for frozenxid purposes.
// // 	 *
// // 	 * We do update relallvisible even in the corner case, since if the table
// // 	 * is all-visible we'd definitely like to know that.  But clamp the value
// // 	 * to be not more than what we're setting relpages to.
// // 	 *
// // 	 * Also, don't change relfrozenxid/relminmxid if we skipped any pages,
// // 	 * since then we don't know for certain that all tuples have a newer xmin.
// // 	 */
// // 	new_rel_pages = vacrelstats->rel_pages;
// // 	new_rel_tuples = vacrelstats->new_rel_tuples;
// // 	if (vacrelstats->tupcount_pages == 0 && new_rel_pages > 0)
// // 	{
// // 		new_rel_pages = vacrelstats->old_rel_pages;
// // 		new_rel_tuples = vacrelstats->old_rel_tuples;
// // 	}

// // 	visibilitymap_count(onerel, &new_rel_allvisible, NULL);
// // 	if (new_rel_allvisible > new_rel_pages)
// // 		new_rel_allvisible = new_rel_pages;

// // 	new_frozen_xid = scanned_all_unfrozen ? FreezeLimit : InvalidTransactionId;
// // 	new_min_multi = scanned_all_unfrozen ? MultiXactCutoff : InvalidMultiXactId;

// // 	vac_update_relstats(onerel,
// // 						new_rel_pages,
// // 						new_rel_tuples,
// // 						new_rel_allvisible,
// // 						vacrelstats->hasindex,
// // 						new_frozen_xid,
// // 						new_min_multi,
// // 						false);

// // 	/* report results to the stats collector, too */
// // 	new_live_tuples = new_rel_tuples - vacrelstats->new_dead_tuples;
// // 	if (new_live_tuples < 0)
// // 		new_live_tuples = 0;	/* just in case */

// // 	pgstat_report_vacuum(RelationGetRelid(onerel),
// // 						 onerel->rd_rel->relisshared,
// // 						 new_live_tuples,
// // 						 vacrelstats->new_dead_tuples);
// // 	pgstat_progress_end_command();

// // 	/* and log the action if appropriate */
// // 	if (IsAutoVacuumWorkerProcess() && params->log_min_duration >= 0)
// // 	{
// // 		TimestampTz endtime = GetCurrentTimestamp();

// // 		if (params->log_min_duration == 0 ||
// // 			TimestampDifferenceExceeds(starttime, endtime,
// // 									   params->log_min_duration))
// // 		{
// // 			StringInfoData buf;

// // 			TimestampDifference(starttime, endtime, &secs, &usecs);

// // 			read_rate = 0;
// // 			write_rate = 0;
// // 			if ((secs > 0) || (usecs > 0))
// // 			{
// // 				read_rate = (double) BLCKSZ *VacuumPageMiss / (1024 * 1024) /
// // 							(secs + usecs / 1000000.0);
// // 				write_rate = (double) BLCKSZ *VacuumPageDirty / (1024 * 1024) /
// // 							(secs + usecs / 1000000.0);
// // 			}

// // 			/*
// // 			 * This is pretty messy, but we split it up so that we can skip
// // 			 * emitting individual parts of the message when not applicable.
// // 			 */
// // 			initStringInfo(&buf);
// // 			appendStringInfo(&buf, _("automatic vacuum of table \"%s.%s.%s\": index scans: %d\n"),
// // 							 get_database_name(MyDatabaseId),
// // 							 get_namespace_name(RelationGetNamespace(onerel)),
// // 							 RelationGetRelationName(onerel),
// // 							 vacrelstats->num_index_scans);
// // 			appendStringInfo(&buf, _("pages: %u removed, %u remain, %u skipped due to pins, %u skipped frozen\n"),
// // 							 vacrelstats->pages_removed,
// // 							 vacrelstats->rel_pages,
// // 							 vacrelstats->pinskipped_pages,
// // 							 vacrelstats->frozenskipped_pages);
// // 			appendStringInfo(&buf,
// // 							 _("tuples: %.0f removed, %.0f remain, %.0f are dead but not yet removable\n"),
// // 							 vacrelstats->tuples_deleted,
// // 							 vacrelstats->new_rel_tuples,
// // 							 vacrelstats->new_dead_tuples);
// // 			appendStringInfo(&buf,
// // 						 _("buffer usage: %d hits, %d misses, %d dirtied\n"),
// // 							 VacuumPageHit,
// // 							 VacuumPageMiss,
// // 							 VacuumPageDirty);
// // 			appendStringInfo(&buf, _("avg read rate: %.3f MB/s, avg write rate: %.3f MB/s\n"),
// // 							 read_rate, write_rate);
// // 			appendStringInfo(&buf, _("system usage: %s"), pg_rusage_show(&ru0));

// // 			ereport(LOG,
// // 					(errmsg_internal("%s", buf.data)));
// // 			pfree(buf.data);
// // 		}
// // 	}
// // }

// bool
// MultiXactIdIsRunning(MultiXactId multi, bool isLockOnly)
// {
// 	MultiXactMember *members;
// 	int			nmembers;
// 	int			i;

// 	debug_elog3(DEBUG2, "IsRunning %u?", multi);

// 	/*
// 	 * "false" here means we assume our callers have checked that the given
// 	 * multi cannot possibly come from a pg_upgraded database.
// 	 */
// 	nmembers = GetMultiXactIdMembers(multi, &members, false, isLockOnly);

// 	if (nmembers <= 0)
// 	{
// 		debug_elog2(DEBUG2, "IsRunning: no members");
// 		return false;
// 	}

// 	/*
// 	 * Checking for myself is cheap compared to looking in shared memory;
// 	 * return true if any live subtransaction of the current top-level
// 	 * transaction is a member.
// 	 *
// 	 * This is not needed for correctness, it's just a fast path.
// 	 */
// 	for (i = 0; i < nmembers; i++)
// 	{
// 		if (TransactionIdIsCurrentTransactionId(members[i].xid))
// 		{
// 			debug_elog3(DEBUG2, "IsRunning: I (%d) am running!", i);
// 			pfree(members);
// 			return true;
// 		}
// 	}

// 	/*
// 	 * This could be made faster by having another entry point in procarray.c,
// 	 * walking the PGPROC array only once for all the members.  But in most
// 	 * cases nmembers should be small enough that it doesn't much matter.
// 	 */
// 	for (i = 0; i < nmembers; i++)
// 	{
// 		if (TransactionIdIsInProgress(members[i].xid))
// 		{
// 			debug_elog4(DEBUG2, "IsRunning: member %d (%u) is running",
// 						i, members[i].xid);
// 			pfree(members);
// 			return true;
// 		}
// 	}

// 	pfree(members);

// 	debug_elog3(DEBUG2, "IsRunning: %u is not running", multi);

// 	return false;
// }

// //这个og参数不对,要留着
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


// //access/multixact.h
// // int
// // GetMultiXactIdMembers(MultiXactId multi, MultiXactMember **members,
// // 					  bool from_pgupgrade, bool onlyLock)
// // {
// // 	int			pageno;
// // 	int			prev_pageno;
// // 	int			entryno;
// // 	int			slotno;
// // 	MultiXactOffset *offptr;
// // 	MultiXactOffset offset;
// // 	int			length;
// // 	int			truelength;
// // 	int			i;
// // 	MultiXactId oldestMXact;
// // 	MultiXactId nextMXact;
// // 	MultiXactId tmpMXact;
// // 	MultiXactOffset nextOffset;
// // 	MultiXactMember *ptr;

// // 	debug_elog3(DEBUG2, "GetMembers: asked for %u", multi);

// // 	if (!MultiXactIdIsValid(multi) || from_pgupgrade)
// // 		return -1;

// // 	/* See if the MultiXactId is in the local cache */
// // 	length = mXactCacheGetById(multi, members);
// // 	if (length >= 0)
// // 	{
// // 		debug_elog3(DEBUG2, "GetMembers: found %s in the cache",
// // 					mxid_to_string(multi, length, *members));
// // 		return length;
// // 	}

// // 	/* Set our OldestVisibleMXactId[] entry if we didn't already */
// // 	MultiXactIdSetOldestVisible();

// // 	/*
// // 	 * If we know the multi is used only for locking and not for updates, then
// // 	 * we can skip checking if the value is older than our oldest visible
// // 	 * multi.  It cannot possibly still be running.
// // 	 */
// // 	if (onlyLock &&
// // 		MultiXactIdPrecedes(multi, OldestVisibleMXactId[MyBackendId]))
// // 	{
// // 		debug_elog2(DEBUG2, "GetMembers: a locker-only multi is too old");
// // 		*members = NULL;
// // 		return -1;
// // 	}

// // 	/*
// // 	 * We check known limits on MultiXact before resorting to the SLRU area.
// // 	 *
// // 	 * An ID older than MultiXactState->oldestMultiXactId cannot possibly be
// // 	 * useful; it has already been removed, or will be removed shortly, by
// // 	 * truncation.  If one is passed, an error is raised.
// // 	 *
// // 	 * Also, an ID >= nextMXact shouldn't ever be seen here; if it is seen, it
// // 	 * implies undetected ID wraparound has occurred.  This raises a hard
// // 	 * error.
// // 	 *
// // 	 * Shared lock is enough here since we aren't modifying any global state.
// // 	 * Acquire it just long enough to grab the current counter values.  We may
// // 	 * need both nextMXact and nextOffset; see below.
// // 	 */
// // 	LWLockAcquire(MultiXactGenLock, LW_SHARED);

// // 	oldestMXact = MultiXactState->oldestMultiXactId;
// // 	nextMXact = MultiXactState->nextMXact;
// // 	nextOffset = MultiXactState->nextOffset;

// // 	LWLockRelease(MultiXactGenLock);

// // 	if (MultiXactIdPrecedes(multi, oldestMXact))
// // 	{
// // 		ereport(ERROR,
// // 				(errcode(ERRCODE_INTERNAL_ERROR),
// // 		 errmsg("MultiXactId %u does no longer exist -- apparent wraparound",
// // 				multi)));
// // 		return -1;
// // 	}

// // 	if (!MultiXactIdPrecedes(multi, nextMXact))
// // 		ereport(ERROR,
// // 				(errcode(ERRCODE_INTERNAL_ERROR),
// // 				 errmsg("MultiXactId %u has not been created yet -- apparent wraparound",
// // 						multi)));

// // 	/*
// // 	 * Find out the offset at which we need to start reading MultiXactMembers
// // 	 * and the number of members in the multixact.  We determine the latter as
// // 	 * the difference between this multixact's starting offset and the next
// // 	 * one's.  However, there are some corner cases to worry about:
// // 	 *
// // 	 * 1. This multixact may be the latest one created, in which case there is
// // 	 * no next one to look at.  In this case the nextOffset value we just
// // 	 * saved is the correct endpoint.
// // 	 *
// // 	 * 2. The next multixact may still be in process of being filled in: that
// // 	 * is, another process may have done GetNewMultiXactId but not yet written
// // 	 * the offset entry for that ID.  In that scenario, it is guaranteed that
// // 	 * the offset entry for that multixact exists (because GetNewMultiXactId
// // 	 * won't release MultiXactGenLock until it does) but contains zero
// // 	 * (because we are careful to pre-zero offset pages). Because
// // 	 * GetNewMultiXactId will never return zero as the starting offset for a
// // 	 * multixact, when we read zero as the next multixact's offset, we know we
// // 	 * have this case.  We sleep for a bit and try again.
// // 	 *
// // 	 * 3. Because GetNewMultiXactId increments offset zero to offset one to
// // 	 * handle case #2, there is an ambiguity near the point of offset
// // 	 * wraparound.  If we see next multixact's offset is one, is that our
// // 	 * multixact's actual endpoint, or did it end at zero with a subsequent
// // 	 * increment?  We handle this using the knowledge that if the zero'th
// // 	 * member slot wasn't filled, it'll contain zero, and zero isn't a valid
// // 	 * transaction ID so it can't be a multixact member.  Therefore, if we
// // 	 * read a zero from the members array, just ignore it.
// // 	 *
// // 	 * This is all pretty messy, but the mess occurs only in infrequent corner
// // 	 * cases, so it seems better than holding the MultiXactGenLock for a long
// // 	 * time on every multixact creation.
// // 	 */
// // retry:
// // 	LWLockAcquire(MultiXactOffsetControlLock, LW_EXCLUSIVE);

// // 	pageno = MultiXactIdToOffsetPage(multi);
// // 	entryno = MultiXactIdToOffsetEntry(multi);

// // 	slotno = SimpleLruReadPage(MultiXactOffsetCtl, pageno, true, multi);
// // 	offptr = (MultiXactOffset *) MultiXactOffsetCtl->shared->page_buffer[slotno];
// // 	offptr += entryno;
// // 	offset = *offptr;

// // 	Assert(offset != 0);

// // 	/*
// // 	 * Use the same increment rule as GetNewMultiXactId(), that is, don't
// // 	 * handle wraparound explicitly until needed.
// // 	 */
// // 	tmpMXact = multi + 1;

// // 	if (nextMXact == tmpMXact)
// // 	{
// // 		/* Corner case 1: there is no next multixact */
// // 		length = nextOffset - offset;
// // 	}
// // 	else
// // 	{
// // 		MultiXactOffset nextMXOffset;

// // 		/* handle wraparound if needed */
// // 		if (tmpMXact < FirstMultiXactId)
// // 			tmpMXact = FirstMultiXactId;

// // 		prev_pageno = pageno;

// // 		pageno = MultiXactIdToOffsetPage(tmpMXact);
// // 		entryno = MultiXactIdToOffsetEntry(tmpMXact);

// // 		if (pageno != prev_pageno)
// // 			slotno = SimpleLruReadPage(MultiXactOffsetCtl, pageno, true, tmpMXact);

// // 		offptr = (MultiXactOffset *) MultiXactOffsetCtl->shared->page_buffer[slotno];
// // 		offptr += entryno;
// // 		nextMXOffset = *offptr;

// // 		if (nextMXOffset == 0)
// // 		{
// // 			/* Corner case 2: next multixact is still being filled in */
// // 			LWLockRelease(MultiXactOffsetControlLock);
// // 			CHECK_FOR_INTERRUPTS();
// // 			pg_usleep(1000L);
// // 			goto retry;
// // 		}

// // 		length = nextMXOffset - offset;
// // 	}

// // 	LWLockRelease(MultiXactOffsetControlLock);

// // 	ptr = (MultiXactMember *) palloc(length * sizeof(MultiXactMember));
// // 	*members = ptr;

// // 	/* Now get the members themselves. */
// // 	LWLockAcquire(MultiXactMemberControlLock, LW_EXCLUSIVE);

// // 	truelength = 0;
// // 	prev_pageno = -1;
// // 	for (i = 0; i < length; i++, offset++)
// // 	{
// // 		TransactionId *xactptr;
// // 		uint32	   *flagsptr;
// // 		int			flagsoff;
// // 		int			bshift;
// // 		int			memberoff;

// // 		pageno = MXOffsetToMemberPage(offset);
// // 		memberoff = MXOffsetToMemberOffset(offset);

// // 		if (pageno != prev_pageno)
// // 		{
// // 			slotno = SimpleLruReadPage(MultiXactMemberCtl, pageno, true, multi);
// // 			prev_pageno = pageno;
// // 		}

// // 		xactptr = (TransactionId *)
// // 			(MultiXactMemberCtl->shared->page_buffer[slotno] + memberoff);

// // 		if (!TransactionIdIsValid(*xactptr))
// // 		{
// // 			/* Corner case 3: we must be looking at unused slot zero */
// // 			Assert(offset == 0);
// // 			continue;
// // 		}

// // 		flagsoff = MXOffsetToFlagsOffset(offset);
// // 		bshift = MXOffsetToFlagsBitShift(offset);
// // 		flagsptr = (uint32 *) (MultiXactMemberCtl->shared->page_buffer[slotno] + flagsoff);

// // 		ptr[truelength].xid = *xactptr;
// // 		ptr[truelength].status = (*flagsptr >> bshift) & MXACT_MEMBER_XACT_BITMASK;
// // 		truelength++;
// // 	}

// // 	LWLockRelease(MultiXactMemberControlLock);

// // 	/*
// // 	 * Copy the result into the local cache.
// // 	 */
// // 	mXactCachePut(multi, truelength, ptr);

// // 	debug_elog3(DEBUG2, "GetMembers: no cache for %s",
// // 				mxid_to_string(multi, truelength, ptr));
// // 	return truelength;
// // }

// void
// index_build(Relation heapRelation,
// 			Relation indexRelation,
// 			IndexInfo *indexInfo,
// 			bool isprimary,
// 			bool isreindex)
// {
// 	IndexBuildResult *stats;
// 	Oid			save_userid;
// 	int			save_sec_context;
// 	int			save_nestlevel;

// 	/*
// 	 * sanity checks
// 	 */
// 	Assert(RelationIsValid(indexRelation));
// 	Assert(PointerIsValid(indexRelation->rd_amroutine));
// 	Assert(PointerIsValid(indexRelation->rd_amroutine->ambuild));
// 	Assert(PointerIsValid(indexRelation->rd_amroutine->ambuildempty));

// 	ereport(DEBUG1,
// 			(errmsg("building index \"%s\" on table \"%s\"",
// 					RelationGetRelationName(indexRelation),
// 					RelationGetRelationName(heapRelation))));

// 	/*
// 	 * Switch to the table owner's userid, so that any index functions are run
// 	 * as that user.  Also lock down security-restricted operations and
// 	 * arrange to make GUC variable changes local to this command.
// 	 */
// 	GetUserIdAndSecContext(&save_userid, &save_sec_context);
// 	SetUserIdAndSecContext(heapRelation->rd_rel->relowner,
// 						   save_sec_context | SECURITY_RESTRICTED_OPERATION);
// 	save_nestlevel = NewGUCNestLevel();

// 	/*
// 	 * Call the access method's build procedure
// 	 */
// 	stats = indexRelation->rd_amroutine->ambuild(heapRelation, indexRelation,
// 												 indexInfo);
// 	Assert(PointerIsValid(stats));

// 	/*
// 	 * If this is an unlogged index, we may need to write out an init fork for
// 	 * it -- but we must first check whether one already exists.  If, for
// 	 * example, an unlogged relation is truncated in the transaction that
// 	 * created it, or truncated twice in a subsequent transaction, the
// 	 * relfilenode won't change, and nothing needs to be done here.
// 	 */
// 	if (indexRelation->rd_rel->relpersistence == RELPERSISTENCE_UNLOGGED &&
// 		!smgrexists(indexRelation->rd_smgr, INIT_FORKNUM))
// 	{
// 		RelationOpenSmgr(indexRelation);
// 		smgrcreate(indexRelation->rd_smgr, INIT_FORKNUM, false);
// 		indexRelation->rd_amroutine->ambuildempty(indexRelation);
// 	}

// 	/*
// 	 * If we found any potentially broken HOT chains, mark the index as not
// 	 * being usable until the current transaction is below the event horizon.
// 	 * See src/backend/access/heap/README.HOT for discussion.  Also set this
// 	 * if early pruning/vacuuming is enabled for the heap relation.  While it
// 	 * might become safe to use the index earlier based on actual cleanup
// 	 * activity and other active transactions, the test for that would be much
// 	 * more complex and would require some form of blocking, so keep it simple
// 	 * and fast by just using the current transaction.
// 	 *
// 	 * However, when reindexing an existing index, we should do nothing here.
// 	 * Any HOT chains that are broken with respect to the index must predate
// 	 * the index's original creation, so there is no need to change the
// 	 * index's usability horizon.  Moreover, we *must not* try to change the
// 	 * index's pg_index entry while reindexing pg_index itself, and this
// 	 * optimization nicely prevents that.  The more complex rules needed for a
// 	 * reindex are handled separately after this function returns.
// 	 *
// 	 * We also need not set indcheckxmin during a concurrent index build,
// 	 * because we won't set indisvalid true until all transactions that care
// 	 * about the broken HOT chains or early pruning/vacuuming are gone.
// 	 *
// 	 * Therefore, this code path can only be taken during non-concurrent
// 	 * CREATE INDEX.  Thus the fact that heap_update will set the pg_index
// 	 * tuple's xmin doesn't matter, because that tuple was created in the
// 	 * current transaction anyway.  That also means we don't need to worry
// 	 * about any concurrent readers of the tuple; no other transaction can see
// 	 * it yet.
// 	 */
// 	if ((indexInfo->ii_BrokenHotChain || EarlyPruningEnabled(heapRelation)) &&
// 		!isreindex &&
// 		!indexInfo->ii_Concurrent)
// 	{
// 		Oid			indexId = RelationGetRelid(indexRelation);
// 		Relation	pg_index;
// 		HeapTuple	indexTuple;
// 		Form_pg_index indexForm;

// 		pg_index = heap_open(IndexRelationId, RowExclusiveLock);

// 		indexTuple = SearchSysCacheCopy1(INDEXRELID,
// 										 ObjectIdGetDatum(indexId));
// 		if (!HeapTupleIsValid(indexTuple))
// 			elog(ERROR, "cache lookup failed for index %u", indexId);
// 		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

// 		/* If it's a new index, indcheckxmin shouldn't be set ... */
// 		Assert(!indexForm->indcheckxmin);

// 		indexForm->indcheckxmin = true;
// 		simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
// 		CatalogUpdateIndexes(pg_index, indexTuple);

// 		heap_freetuple(indexTuple);
// 		heap_close(pg_index, RowExclusiveLock);
// 	}

// 	/*
// 	 * Update heap and index pg_class rows
// 	 */
// 	index_update_stats(heapRelation,
// 					   true,
// 					   isprimary,
// 					   stats->heap_tuples);

// 	index_update_stats(indexRelation,
// 					   false,
// 					   false,
// 					   stats->index_tuples);

// 	/* Make the updated catalog row versions visible */
// 	CommandCounterIncrement();

// 	/*
// 	 * If it's for an exclusion constraint, make a second pass over the heap
// 	 * to verify that the constraint is satisfied.  We must not do this until
// 	 * the index is fully valid.  (Broken HOT chains shouldn't matter, though;
// 	 * see comments for IndexCheckExclusion.)
// 	 */
// 	if (indexInfo->ii_ExclusionOps != NULL)
// 		IndexCheckExclusion(heapRelation, indexRelation, indexInfo);

// 	/* Roll back any GUC changes executed by index functions */
// 	AtEOXact_GUC(false, save_nestlevel);

// 	/* Restore userid and security context */
// 	SetUserIdAndSecContext(save_userid, save_sec_context);
// }

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

// bool
// IsAutoVacuumWorkerProcess(void)
// {
// 	return am_autovacuum_worker;
// } 

// //access/hash.h
// // Datum
// // hash_any(register const unsigned char *k, register int keylen)
// // {
// // 	register uint32 a,
// // 				b,
// // 				c,
// // 				len;

// // 	/* Set up the internal state */
// // 	len = keylen;
// // 	a = b = c = 0x9e3779b9 + len + 3923095;

// // 	/* If the source pointer is word-aligned, we use word-wide fetches */
// // 	if (((uintptr_t) k & UINT32_ALIGN_MASK) == 0)
// // 	{
// // 		/* Code path for aligned source data */
// // 		register const uint32 *ka = (const uint32 *) k;

// // 		/* handle most of the key */
// // 		while (len >= 12)
// // 		{
// // 			a += ka[0];
// // 			b += ka[1];
// // 			c += ka[2];
// // 			mix(a, b, c);
// // 			ka += 3;
// // 			len -= 12;
// // 		}

// // 		/* handle the last 11 bytes */
// // 		k = (const unsigned char *) ka;
// // #ifdef WORDS_BIGENDIAN
// // 		switch (len)
// // 		{
// // 			case 11:
// // 				c += ((uint32) k[10] << 8);
// // 				/* fall through */
// // 			case 10:
// // 				c += ((uint32) k[9] << 16);
// // 				/* fall through */
// // 			case 9:
// // 				c += ((uint32) k[8] << 24);
// // 				/* the lowest byte of c is reserved for the length */
// // 				/* fall through */
// // 			case 8:
// // 				b += ka[1];
// // 				a += ka[0];
// // 				break;
// // 			case 7:
// // 				b += ((uint32) k[6] << 8);
// // 				/* fall through */
// // 			case 6:
// // 				b += ((uint32) k[5] << 16);
// // 				/* fall through */
// // 			case 5:
// // 				b += ((uint32) k[4] << 24);
// // 				/* fall through */
// // 			case 4:
// // 				a += ka[0];
// // 				break;
// // 			case 3:
// // 				a += ((uint32) k[2] << 8);
// // 				/* fall through */
// // 			case 2:
// // 				a += ((uint32) k[1] << 16);
// // 				/* fall through */
// // 			case 1:
// // 				a += ((uint32) k[0] << 24);
// // 				/* case 0: nothing left to add */
// // 		}
// // #else							/* !WORDS_BIGENDIAN */
// // 		switch (len)
// // 		{
// // 			case 11:
// // 				c += ((uint32) k[10] << 24);
// // 				/* fall through */
// // 			case 10:
// // 				c += ((uint32) k[9] << 16);
// // 				/* fall through */
// // 			case 9:
// // 				c += ((uint32) k[8] << 8);
// // 				/* the lowest byte of c is reserved for the length */
// // 				/* fall through */
// // 			case 8:
// // 				b += ka[1];
// // 				a += ka[0];
// // 				break;
// // 			case 7:
// // 				b += ((uint32) k[6] << 16);
// // 				/* fall through */
// // 			case 6:
// // 				b += ((uint32) k[5] << 8);
// // 				/* fall through */
// // 			case 5:
// // 				b += k[4];
// // 				/* fall through */
// // 			case 4:
// // 				a += ka[0];
// // 				break;
// // 			case 3:
// // 				a += ((uint32) k[2] << 16);
// // 				/* fall through */
// // 			case 2:
// // 				a += ((uint32) k[1] << 8);
// // 				/* fall through */
// // 			case 1:
// // 				a += k[0];
// // 				/* case 0: nothing left to add */
// // 		}
// // #endif   /* WORDS_BIGENDIAN */
// // 	}
// // 	else
// // 	{
// // 		/* Code path for non-aligned source data */

// // 		/* handle most of the key */
// // 		while (len >= 12)
// // 		{
// // #ifdef WORDS_BIGENDIAN
// // 			a += (k[3] + ((uint32) k[2] << 8) + ((uint32) k[1] << 16) + ((uint32) k[0] << 24));
// // 			b += (k[7] + ((uint32) k[6] << 8) + ((uint32) k[5] << 16) + ((uint32) k[4] << 24));
// // 			c += (k[11] + ((uint32) k[10] << 8) + ((uint32) k[9] << 16) + ((uint32) k[8] << 24));
// // #else							/* !WORDS_BIGENDIAN */
// // 			a += (k[0] + ((uint32) k[1] << 8) + ((uint32) k[2] << 16) + ((uint32) k[3] << 24));
// // 			b += (k[4] + ((uint32) k[5] << 8) + ((uint32) k[6] << 16) + ((uint32) k[7] << 24));
// // 			c += (k[8] + ((uint32) k[9] << 8) + ((uint32) k[10] << 16) + ((uint32) k[11] << 24));
// // #endif   /* WORDS_BIGENDIAN */
// // 			mix(a, b, c);
// // 			k += 12;
// // 			len -= 12;
// // 		}

// // 		/* handle the last 11 bytes */
// // #ifdef WORDS_BIGENDIAN
// // 		switch (len)			/* all the case statements fall through */
// // 		{
// // 			case 11:
// // 				c += ((uint32) k[10] << 8);
// // 			case 10:
// // 				c += ((uint32) k[9] << 16);
// // 			case 9:
// // 				c += ((uint32) k[8] << 24);
// // 				/* the lowest byte of c is reserved for the length */
// // 			case 8:
// // 				b += k[7];
// // 			case 7:
// // 				b += ((uint32) k[6] << 8);
// // 			case 6:
// // 				b += ((uint32) k[5] << 16);
// // 			case 5:
// // 				b += ((uint32) k[4] << 24);
// // 			case 4:
// // 				a += k[3];
// // 			case 3:
// // 				a += ((uint32) k[2] << 8);
// // 			case 2:
// // 				a += ((uint32) k[1] << 16);
// // 			case 1:
// // 				a += ((uint32) k[0] << 24);
// // 				/* case 0: nothing left to add */
// // 		}
// // #else							/* !WORDS_BIGENDIAN */
// // 		switch (len)			/* all the case statements fall through */
// // 		{
// // 			case 11:
// // 				c += ((uint32) k[10] << 24);
// // 			case 10:
// // 				c += ((uint32) k[9] << 16);
// // 			case 9:
// // 				c += ((uint32) k[8] << 8);
// // 				/* the lowest byte of c is reserved for the length */
// // 			case 8:
// // 				b += ((uint32) k[7] << 24);
// // 			case 7:
// // 				b += ((uint32) k[6] << 16);
// // 			case 6:
// // 				b += ((uint32) k[5] << 8);
// // 			case 5:
// // 				b += k[4];
// // 			case 4:
// // 				a += ((uint32) k[3] << 24);
// // 			case 3:
// // 				a += ((uint32) k[2] << 16);
// // 			case 2:
// // 				a += ((uint32) k[1] << 8);
// // 			case 1:
// // 				a += k[0];
// // 				/* case 0: nothing left to add */
// // 		}
// // #endif   /* WORDS_BIGENDIAN */
// // 	}

// // 	final(a, b, c);

// // 	/* report the result */
// // 	return UInt32GetDatum(c);
// // }

// //optimizer/planner.h
// // bool
// // plan_cluster_use_sort(Oid tableOid, Oid indexOid)
// // {
// // 	PlannerInfo *root;
// // 	Query	   *query;
// // 	PlannerGlobal *glob;
// // 	RangeTblEntry *rte;
// // 	RelOptInfo *rel;
// // 	IndexOptInfo *indexInfo;
// // 	QualCost	indexExprCost;
// // 	Cost		comparisonCost;
// // 	Path	   *seqScanPath;
// // 	Path		seqScanAndSortPath;
// // 	IndexPath  *indexScanPath;
// // 	ListCell   *lc;

// // 	/* We can short-circuit the cost comparison if indexscans are disabled */
// // 	if (!enable_indexscan)
// // 		return true;			/* use sort */

// // 	/* Set up mostly-dummy planner state */
// // 	query = makeNode(Query);
// // 	query->commandType = CMD_SELECT;

// // 	glob = makeNode(PlannerGlobal);

// // 	root = makeNode(PlannerInfo);
// // 	root->parse = query;
// // 	root->glob = glob;
// // 	root->query_level = 1;
// // 	root->planner_cxt = CurrentMemoryContext;
// // 	root->wt_param_id = -1;

// // 	/* Build a minimal RTE for the rel */
// // 	rte = makeNode(RangeTblEntry);
// // 	rte->rtekind = RTE_RELATION;
// // 	rte->relid = tableOid;
// // 	rte->relkind = RELKIND_RELATION;	/* Don't be too picky. */
// // 	rte->lateral = false;
// // 	rte->inh = false;
// // 	rte->inFromCl = true;
// // 	query->rtable = list_make1(rte);

// // 	/* Set up RTE/RelOptInfo arrays */
// // 	setup_simple_rel_arrays(root);

// // 	/* Build RelOptInfo */
// // 	rel = build_simple_rel(root, 1, RELOPT_BASEREL);

// // 	/* Locate IndexOptInfo for the target index */
// // 	indexInfo = NULL;
// // 	foreach(lc, rel->indexlist)
// // 	{
// // 		indexInfo = (IndexOptInfo *) lfirst(lc);
// // 		if (indexInfo->indexoid == indexOid)
// // 			break;
// // 	}

// // 	/*
// // 	 * It's possible that get_relation_info did not generate an IndexOptInfo
// // 	 * for the desired index; this could happen if it's not yet reached its
// // 	 * indcheckxmin usability horizon, or if it's a system index and we're
// // 	 * ignoring system indexes.  In such cases we should tell CLUSTER to not
// // 	 * trust the index contents but use seqscan-and-sort.
// // 	 */
// // 	if (lc == NULL)				/* not in the list? */
// // 		return true;			/* use sort */

// // 	/*
// // 	 * Rather than doing all the pushups that would be needed to use
// // 	 * set_baserel_size_estimates, just do a quick hack for rows and width.
// // 	 */
// // 	rel->rows = rel->tuples;
// // 	rel->reltarget->width = get_relation_data_width(tableOid, NULL);

// // 	root->total_table_pages = rel->pages;

// // 	/*
// // 	 * Determine eval cost of the index expressions, if any.  We need to
// // 	 * charge twice that amount for each tuple comparison that happens during
// // 	 * the sort, since tuplesort.c will have to re-evaluate the index
// // 	 * expressions each time.  (XXX that's pretty inefficient...)
// // 	 */
// // 	cost_qual_eval(&indexExprCost, indexInfo->indexprs, root);
// // 	comparisonCost = 2.0 * (indexExprCost.startup + indexExprCost.per_tuple);

// // 	/* Estimate the cost of seq scan + sort */
// // 	seqScanPath = create_seqscan_path(root, rel, NULL, 0);
// // 	cost_sort(&seqScanAndSortPath, root, NIL,
// // 			  seqScanPath->total_cost, rel->tuples, rel->reltarget->width,
// // 			  comparisonCost, maintenance_work_mem, -1.0);

// // 	/* Estimate the cost of index scan */
// // 	indexScanPath = create_index_path(root, indexInfo,
// // 									  NIL, NIL, NIL, NIL, NIL,
// // 									  ForwardScanDirection, false,
// // 									  NULL, 1.0);

// // 	return (seqScanAndSortPath.total_cost < indexScanPath->path.total_cost);
// // }


// //storage/procarray.h
// // bool
// // TransactionIdIsInProgress(TransactionId xid)
// // {
// // 	static TransactionId *xids = NULL;
// // 	int			nxids = 0;
// // 	ProcArrayStruct *arrayP = procArray;
// // 	TransactionId topxid;
// // 	int			i,
// // 				j;

// // 	/*
// // 	 * Don't bother checking a transaction older than RecentXmin; it could not
// // 	 * possibly still be running.  (Note: in particular, this guarantees that
// // 	 * we reject InvalidTransactionId, FrozenTransactionId, etc as not
// // 	 * running.)
// // 	 */
// // 	if (TransactionIdPrecedes(xid, RecentXmin))
// // 	{
// // 		xc_by_recent_xmin_inc();
// // 		return false;
// // 	}

// // 	/*
// // 	 * We may have just checked the status of this transaction, so if it is
// // 	 * already known to be completed, we can fall out without any access to
// // 	 * shared memory.
// // 	 */
// // 	if (TransactionIdIsKnownCompleted(xid))
// // 	{
// // 		xc_by_known_xact_inc();
// // 		return false;
// // 	}

// // 	/*
// // 	 * Also, we can handle our own transaction (and subtransactions) without
// // 	 * any access to shared memory.
// // 	 */
// // 	if (TransactionIdIsCurrentTransactionId(xid))
// // 	{
// // 		xc_by_my_xact_inc();
// // 		return true;
// // 	}

// // 	/*
// // 	 * If first time through, get workspace to remember main XIDs in. We
// // 	 * malloc it permanently to avoid repeated palloc/pfree overhead.
// // 	 */
// // 	if (xids == NULL)
// // 	{
// // 		/*
// // 		 * In hot standby mode, reserve enough space to hold all xids in the
// // 		 * known-assigned list. If we later finish recovery, we no longer need
// // 		 * the bigger array, but we don't bother to shrink it.
// // 		 */
// // 		int			maxxids = RecoveryInProgress() ? TOTAL_MAX_CACHED_SUBXIDS : arrayP->maxProcs;

// // 		xids = (TransactionId *) malloc(maxxids * sizeof(TransactionId));
// // 		if (xids == NULL)
// // 			ereport(ERROR,
// // 					(errcode(ERRCODE_OUT_OF_MEMORY),
// // 					 errmsg("out of memory")));
// // 	}

// // 	LWLockAcquire(ProcArrayLock, LW_SHARED);

// // 	/*
// // 	 * Now that we have the lock, we can check latestCompletedXid; if the
// // 	 * target Xid is after that, it's surely still running.
// // 	 */
// // 	if (TransactionIdPrecedes(ShmemVariableCache->latestCompletedXid, xid))
// // 	{
// // 		LWLockRelease(ProcArrayLock);
// // 		xc_by_latest_xid_inc();
// // 		return true;
// // 	}

// // 	/* No shortcuts, gotta grovel through the array */
// // 	for (i = 0; i < arrayP->numProcs; i++)
// // 	{
// // 		int			pgprocno = arrayP->pgprocnos[i];
// // 		volatile PGPROC *proc = &allProcs[pgprocno];
// // 		volatile PGXACT *pgxact = &allPgXact[pgprocno];
// // 		TransactionId pxid;

// // 		/* Ignore my own proc --- dealt with it above */
// // 		if (proc == MyProc)
// // 			continue;

// // 		/* Fetch xid just once - see GetNewTransactionId */
// // 		pxid = pgxact->xid;

// // 		if (!TransactionIdIsValid(pxid))
// // 			continue;

// // 		/*
// // 		 * Step 1: check the main Xid
// // 		 */
// // 		if (TransactionIdEquals(pxid, xid))
// // 		{
// // 			LWLockRelease(ProcArrayLock);
// // 			xc_by_main_xid_inc();
// // 			return true;
// // 		}

// // 		/*
// // 		 * We can ignore main Xids that are younger than the target Xid, since
// // 		 * the target could not possibly be their child.
// // 		 */
// // 		if (TransactionIdPrecedes(xid, pxid))
// // 			continue;

// // 		/*
// // 		 * Step 2: check the cached child-Xids arrays
// // 		 */
// // 		for (j = pgxact->nxids - 1; j >= 0; j--)
// // 		{
// // 			/* Fetch xid just once - see GetNewTransactionId */
// // 			TransactionId cxid = proc->subxids.xids[j];

// // 			if (TransactionIdEquals(cxid, xid))
// // 			{
// // 				LWLockRelease(ProcArrayLock);
// // 				xc_by_child_xid_inc();
// // 				return true;
// // 			}
// // 		}

// // 		/*
// // 		 * Save the main Xid for step 4.  We only need to remember main Xids
// // 		 * that have uncached children.  (Note: there is no race condition
// // 		 * here because the overflowed flag cannot be cleared, only set, while
// // 		 * we hold ProcArrayLock.  So we can't miss an Xid that we need to
// // 		 * worry about.)
// // 		 */
// // 		if (pgxact->overflowed)
// // 			xids[nxids++] = pxid;
// // 	}

// // 	/*
// // 	 * Step 3: in hot standby mode, check the known-assigned-xids list.  XIDs
// // 	 * in the list must be treated as running.
// // 	 */
// // 	if (RecoveryInProgress())
// // 	{
// // 		/* none of the PGXACT entries should have XIDs in hot standby mode */
// // 		Assert(nxids == 0);

// // 		if (KnownAssignedXidExists(xid))
// // 		{
// // 			LWLockRelease(ProcArrayLock);
// // 			xc_by_known_assigned_inc();
// // 			return true;
// // 		}

// // 		/*
// // 		 * If the KnownAssignedXids overflowed, we have to check pg_subtrans
// // 		 * too.  Fetch all xids from KnownAssignedXids that are lower than
// // 		 * xid, since if xid is a subtransaction its parent will always have a
// // 		 * lower value.  Note we will collect both main and subXIDs here, but
// // 		 * there's no help for it.
// // 		 */
// // 		if (TransactionIdPrecedesOrEquals(xid, procArray->lastOverflowedXid))
// // 			nxids = KnownAssignedXidsGet(xids, xid);
// // 	}

// // 	LWLockRelease(ProcArrayLock);

// // 	/*
// // 	 * If none of the relevant caches overflowed, we know the Xid is not
// // 	 * running without even looking at pg_subtrans.
// // 	 */
// // 	if (nxids == 0)
// // 	{
// // 		xc_no_overflow_inc();
// // 		return false;
// // 	}

// // 	/*
// // 	 * Step 4: have to check pg_subtrans.
// // 	 *
// // 	 * At this point, we know it's either a subtransaction of one of the Xids
// // 	 * in xids[], or it's not running.  If it's an already-failed
// // 	 * subtransaction, we want to say "not running" even though its parent may
// // 	 * still be running.  So first, check pg_clog to see if it's been aborted.
// // 	 */
// // 	xc_slow_answer_inc();

// // 	if (TransactionIdDidAbort(xid))
// // 		return false;

// // 	/*
// // 	 * It isn't aborted, so check whether the transaction tree it belongs to
// // 	 * is still running (or, more precisely, whether it was running when we
// // 	 * held ProcArrayLock).
// // 	 */
// // 	topxid = SubTransGetTopmostTransaction(xid);
// // 	Assert(TransactionIdIsValid(topxid));
// // 	if (!TransactionIdEquals(topxid, xid))
// // 	{
// // 		for (i = 0; i < nxids; i++)
// // 		{
// // 			if (TransactionIdEquals(xids[i], topxid))
// // 				return true;
// // 		}
// // 	}

// // 	return false;
// // }

// Node *
// build_column_default(Relation rel, int attrno)
// {
// 	TupleDesc	rd_att = rel->rd_att;
// 	Form_pg_attribute att_tup = rd_att->attrs[attrno - 1];
// 	Oid			atttype = att_tup->atttypid;
// 	int32		atttypmod = att_tup->atttypmod;
// 	Node	   *expr = NULL;
// 	Oid			exprtype;

// 	/*
// 	 * Scan to see if relation has a default for this column.
// 	 */
// 	if (rd_att->constr && rd_att->constr->num_defval > 0)
// 	{
// 		AttrDefault *defval = rd_att->constr->defval;
// 		int			ndef = rd_att->constr->num_defval;

// 		while (--ndef >= 0)
// 		{
// 			if (attrno == defval[ndef].adnum)
// 			{
// 				/*
// 				 * Found it, convert string representation to node tree.
// 				 */
// 				expr =(Node*) stringToNode(defval[ndef].adbin);
// 				break;
// 			}
// 		}
// 	}

// 	if (expr == NULL)
// 	{
// 		/*
// 		 * No per-column default, so look for a default for the type itself.
// 		 */
// 		expr = get_typdefault(atttype);
// 	}

// 	if (expr == NULL)
// 		return NULL;			/* No default anywhere */

// 	/*
// 	 * Make sure the value is coerced to the target column type; this will
// 	 * generally be true already, but there seem to be some corner cases
// 	 * involving domain defaults where it might not be true. This should match
// 	 * the parser's processing of non-defaulted expressions --- see
// 	 * transformAssignedExpr().
// 	 */
// 	exprtype = exprType(expr);

// 	expr = coerce_to_target_type(NULL,	/* no UNKNOWN params here */
// 								 expr, exprtype,
// 								 atttype, atttypmod,
// 								 COERCION_ASSIGNMENT,
// 								 COERCE_IMPLICIT_CAST,
// 								 -1);
// 	if (expr == NULL)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATATYPE_MISMATCH),
// 				 errmsg("column \"%s\" is of type %s"
// 						" but default expression is of type %s",
// 						NameStr(att_tup->attname),
// 						format_type_be(atttype),
// 						format_type_be(exprtype)),
// 			   errhint("You will need to rewrite or cast the expression.")));

// 	return expr;
// }
// Expr *
// expression_planner(Expr *expr)
// {
// 	Node	   *result;

// 	/*
// 	 * Convert named-argument function calls, insert default arguments and
// 	 * simplify constant subexprs
// 	 */
// 	result = eval_const_expressions(NULL, (Node *) expr);

// 	/* Fill in opfuncid values if missing */
// 	fix_opfuncids(result);

// 	return (Expr *) result;
// }

// //commands/sequence.h
// // void
// // ResetSequence(Oid seq_relid)
// // {
// // 	Relation	seq_rel;
// // 	SeqTable	elm;
// // 	Form_pg_sequence seq;
// // 	Buffer		buf;
// // 	HeapTupleData seqtuple;
// // 	HeapTuple	tuple;

// // 	/*
// // 	 * Read the old sequence.  This does a bit more work than really
// // 	 * necessary, but it's simple, and we do want to double-check that it's
// // 	 * indeed a sequence.
// // 	 */
// // 	init_sequence(seq_relid, &elm, &seq_rel);
// // 	(void) read_seq_tuple(elm, seq_rel, &buf, &seqtuple);

// // 	/*
// // 	 * Copy the existing sequence tuple.
// // 	 */
// // 	tuple = heap_copytuple(&seqtuple);

// // 	/* Now we're done with the old page */
// // 	UnlockReleaseBuffer(buf);

// // 	/*
// // 	 * Modify the copied tuple to execute the restart (compare the RESTART
// // 	 * action in AlterSequence)
// // 	 */
// // 	seq = (Form_pg_sequence) GETSTRUCT(tuple);
// // 	seq->last_value = seq->start_value;
// // 	seq->is_called = false;
// // 	seq->log_cnt = 0;

// // 	/*
// // 	 * Create a new storage file for the sequence.  We want to keep the
// // 	 * sequence's relfrozenxid at 0, since it won't contain any unfrozen XIDs.
// // 	 * Same with relminmxid, since a sequence will never contain multixacts.
// // 	 */
// // 	RelationSetNewRelfilenode(seq_rel, seq_rel->rd_rel->relpersistence,
// // 							  InvalidTransactionId, InvalidMultiXactId);

// // 	/*
// // 	 * Insert the modified tuple into the new storage file.
// // 	 */
// // 	fill_seq_with_data(seq_rel, tuple);

// // 	/* Clear local cache so that we don't think we have cached numbers */
// // 	/* Note that we do not change the currval() state */
// // 	elm->cached = elm->last;

// // 	relation_close(seq_rel, NoLock);
// // }

// //storage/predicate.h
// // void
// // CheckTableForSerializableConflictIn(Relation relation)
// // {
// // 	HASH_SEQ_STATUS seqstat;
// // 	PREDICATELOCKTARGET *target;
// // 	Oid			dbId;
// // 	Oid			heapId;
// // 	int			i;

// // 	/*
// // 	 * Bail out quickly if there are no serializable transactions running.
// // 	 * It's safe to check this without taking locks because the caller is
// // 	 * holding an ACCESS EXCLUSIVE lock on the relation.  No new locks which
// // 	 * would matter here can be acquired while that is held.
// // 	 */
// // 	if (!TransactionIdIsValid(PredXact->SxactGlobalXmin))
// // 		return;

// // 	if (!SerializationNeededForWrite(relation))
// // 		return;

// // 	/*
// // 	 * We're doing a write which might cause rw-conflicts now or later.
// // 	 * Memorize that fact.
// // 	 */
// // 	MyXactDidWrite = true;

// // 	Assert(relation->rd_index == NULL); /* not an index relation */

// // 	dbId = relation->rd_node.dbNode;
// // 	heapId = relation->rd_id;

// // 	LWLockAcquire(SerializablePredicateLockListLock, LW_EXCLUSIVE);
// // 	for (i = 0; i < NUM_PREDICATELOCK_PARTITIONS; i++)
// // 		LWLockAcquire(PredicateLockHashPartitionLockByIndex(i), LW_SHARED);
// // 	LWLockAcquire(SerializableXactHashLock, LW_EXCLUSIVE);

// // 	/* Scan through target list */
// // 	hash_seq_init(&seqstat, PredicateLockTargetHash);

// // 	while ((target = (PREDICATELOCKTARGET *) hash_seq_search(&seqstat)))
// // 	{
// // 		PREDICATELOCK *predlock;

// // 		/*
// // 		 * Check whether this is a target which needs attention.
// // 		 */
// // 		if (GET_PREDICATELOCKTARGETTAG_RELATION(target->tag) != heapId)
// // 			continue;			/* wrong relation id */
// // 		if (GET_PREDICATELOCKTARGETTAG_DB(target->tag) != dbId)
// // 			continue;			/* wrong database id */

// // 		/*
// // 		 * Loop through locks for this target and flag conflicts.
// // 		 */
// // 		predlock = (PREDICATELOCK *)
// // 			SHMQueueNext(&(target->predicateLocks),
// // 						 &(target->predicateLocks),
// // 						 offsetof(PREDICATELOCK, targetLink));
// // 		while (predlock)
// // 		{
// // 			PREDICATELOCK *nextpredlock;

// // 			nextpredlock = (PREDICATELOCK *)
// // 				SHMQueueNext(&(target->predicateLocks),
// // 							 &(predlock->targetLink),
// // 							 offsetof(PREDICATELOCK, targetLink));

// // 			if (predlock->tag.myXact != og_knl13->MySerializableXact
// // 			  && !RWConflictExists(predlock->tag.myXact, MySerializableXact))
// // 			{
// // 				FlagRWConflict(predlock->tag.myXact, MySerializableXact);
// // 			}

// // 			predlock = nextpredlock;
// // 		}
// // 	}

// // 	/* Release locks in reverse order */
// // 	LWLockRelease(SerializableXactHashLock);
// // 	for (i = NUM_PREDICATELOCK_PARTITIONS - 1; i >= 0; i--)
// // 		LWLockRelease(PredicateLockHashPartitionLockByIndex(i));
// // 	LWLockRelease(SerializablePredicateLockListLock);
// // }	

// //catalog/pg_inherits_fn.h
// // List *
// // find_all_inheritors(Oid parentrelId, LOCKMODE lockmode, List **numparents)
// // {
// // 	List	   *rels_list,
// // 			   *rel_numparents;
// // 	ListCell   *l;

// // 	/*
// // 	 * We build a list starting with the given rel and adding all direct and
// // 	 * indirect children.  We can use a single list as both the record of
// // 	 * already-found rels and the agenda of rels yet to be scanned for more
// // 	 * children.  This is a bit tricky but works because the foreach() macro
// // 	 * doesn't fetch the next list element until the bottom of the loop.
// // 	 */
// // 	rels_list = list_make1_oid(parentrelId);
// // 	rel_numparents = list_make1_int(0);

// // 	foreach(l, rels_list)
// // 	{
// // 		Oid			currentrel = lfirst_oid(l);
// // 		List	   *currentchildren;
// // 		ListCell   *lc;

// // 		/* Get the direct children of this rel */
// // 		currentchildren = find_inheritance_children(currentrel, lockmode);

// // 		/*
// // 		 * Add to the queue only those children not already seen. This avoids
// // 		 * making duplicate entries in case of multiple inheritance paths from
// // 		 * the same parent.  (It'll also keep us from getting into an infinite
// // 		 * loop, though theoretically there can't be any cycles in the
// // 		 * inheritance graph anyway.)
// // 		 */
// // 		foreach(lc, currentchildren)
// // 		{
// // 			Oid			child_oid = lfirst_oid(lc);
// // 			bool		found = false;
// // 			ListCell   *lo;
// // 			ListCell   *li;

// // 			/* if the rel is already there, bump number-of-parents counter */
// // 			forboth(lo, rels_list, li, rel_numparents)
// // 			{
// // 				if (lfirst_oid(lo) == child_oid)
// // 				{
// // 					lfirst_int(li)++;
// // 					found = true;
// // 					break;
// // 				}
// // 			}

// // 			/* if it's not there, add it. expect 1 parent, initially. */
// // 			if (!found)
// // 			{
// // 				rels_list = lappend_oid(rels_list, child_oid);
// // 				rel_numparents = lappend_int(rel_numparents, 1);
// // 			}
// // 		}
// // 	}

// // 	if (numparents)
// // 		*numparents = rel_numparents;
// // 	else
// // 		list_free(rel_numparents);
// // 	return rels_list;
// // }	

// //utils/hsearch.h
// // void *
// // hash_search(HTAB *hashp,
// // 			const void *keyPtr,
// // 			HASHACTION action,
// // 			bool *foundPtr)
// // {
// // 	return hash_search_with_hash_value(hashp,
// // 									   keyPtr,
// // 									   hashp->hash(keyPtr, hashp->keysize),
// // 									   action,
// // 									   foundPtr);
// // }


// int
// pq_getbyte(void)
// {
// 	Assert(PqCommReadingMsg);

// 	while (PqRecvPointer >= PqRecvLength)
// 	{
// 		if (pq_recvbuf())		/* If nothing in buffer, then recv some */
// 			return EOF;			/* Failed to recv data */
// 	}
// 	return (unsigned char) PqRecvBuffer[PqRecvPointer++];
// }


// ssize_t
// secure_read(Port *port, void *ptr, size_t len)
// {
// 	ssize_t		n;
// 	int			waitfor;

// retry:
// #ifdef USE_SSL
// 	waitfor = 0;
// 	if (port->ssl_in_use)
// 	{
// 		n = be_tls_read(port, ptr, len, &waitfor);
// 	}
// 	else
// #endif
// 	{
// 		n = secure_raw_read(port, ptr, len);
// 		waitfor = WL_SOCKET_READABLE;
// 	}

// 	/* In blocking mode, wait until the socket is ready */
// 	if (n < 0 && !port->noblock && (errno == EWOULDBLOCK || errno == EAGAIN))
// 	{
// 		WaitEvent	event;

// 		Assert(waitfor);

// 		ModifyWaitEvent(FeBeWaitSet, 0, waitfor, NULL);

// 		WaitEventSetWait(FeBeWaitSet, -1 /* no timeout */ , &event, 1);

// 		/*
// 		 * If the postmaster has died, it's not safe to continue running,
// 		 * because it is the postmaster's job to kill us if some other backend
// 		 * exists uncleanly.  Moreover, we won't run very well in this state;
// 		 * helper processes like walwriter and the bgwriter will exit, so
// 		 * performance may be poor.  Finally, if we don't exit, pg_ctl will be
// 		 * unable to restart the postmaster without manual intervention, so no
// 		 * new connections can be accepted.  Exiting clears the deck for a
// 		 * postmaster restart.
// 		 *
// 		 * (Note that we only make this check when we would otherwise sleep on
// 		 * our latch.  We might still continue running for a while if the
// 		 * postmaster is killed in mid-query, or even through multiple queries
// 		 * if we never have to wait for read.  We don't want to burn too many
// 		 * cycles checking for this very rare condition, and this should cause
// 		 * us to exit quickly in most cases.)
// 		 */
// 		if (event.events & WL_POSTMASTER_DEATH)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_ADMIN_SHUTDOWN),
// 					 errmsg("terminating connection due to unexpected postmaster exit")));

// 		/* Handle interrupt. */
// 		if (event.events & WL_LATCH_SET)
// 		{
// 			ResetLatch(MyLatch);
// 			ProcessClientReadInterrupt(true);

// 			/*
// 			 * We'll retry the read. Most likely it will return immediately
// 			 * because there's still no data available, and we'll wait for the
// 			 * socket to become ready again.
// 			 */
// 		}
// 		goto retry;
// 	}

// 	/*
// 	 * Process interrupts that happened while (or before) receiving. Note that
// 	 * we signal that we're not blocking, which will prevent some types of
// 	 * interrupts from being processed.
// 	 */
// 	ProcessClientReadInterrupt(false);

// 	return n;
// }

// //parser/parse_collate.h
// // void
// // assign_expr_collations(ParseState *pstate, Node *expr)
// // {
// // 	assign_collations_context context;

// // 	/* initialize context for tree walk */
// // 	context.pstate = pstate;
// // 	context.collation = InvalidOid;
// // 	context.strength = COLLATE_NONE;
// // 	context.location = -1;

// // 	/* and away we go */
// // 	(void) assign_collations_walker(expr, &context);
// // }

// // //parser/parse_relation.h
// // void
// // addRTEtoQuery(ParseState *pstate, RangeTblEntry *rte,
// // 			  bool addToJoinList,
// // 			  bool addToRelNameSpace, bool addToVarNameSpace)
// // {
// // 	if (addToJoinList)
// // 	{
// // 		int			rtindex = RTERangeTablePosn(pstate, rte, NULL);
// // 		RangeTblRef *rtr = makeNode(RangeTblRef);

// // 		rtr->rtindex = rtindex;
// // 		pstate->p_joinlist = lappend(pstate->p_joinlist, rtr);
// // 	}
// // 	if (addToRelNameSpace || addToVarNameSpace)
// // 	{
// // 		ParseNamespaceItem *nsitem;

// // 		nsitem = (ParseNamespaceItem *) palloc(sizeof(ParseNamespaceItem));
// // 		nsitem->p_rte = rte;
// // 		nsitem->p_rel_visible = addToRelNameSpace;
// // 		nsitem->p_cols_visible = addToVarNameSpace;
// // 		nsitem->p_lateral_only = false;
// // 		nsitem->p_lateral_ok = true;
// // 		pstate->p_namespace = lappend(pstate->p_namespace, nsitem);
// // 	}
// // }

// // //parser/parse_relation.h
// // RangeTblEntry *
// // addRangeTableEntryForRelation(ParseState *pstate,
// // 							  Relation rel,
// // 							  Alias *alias,
// // 							  bool inh,
// // 							  bool inFromCl)
// // {
// // 	RangeTblEntry *rte = makeNode(RangeTblEntry);
// // 	char	   *refname = alias ? alias->aliasname : RelationGetRelationName(rel);

// // 	Assert(pstate != NULL);

// // 	rte->rtekind = RTE_RELATION;
// // 	rte->alias = alias;
// // 	rte->relid = RelationGetRelid(rel);
// // 	rte->relkind = rel->rd_rel->relkind;

// // 	/*
// // 	 * Build the list of effective column names using user-supplied aliases
// // 	 * and/or actual column names.
// // 	 */
// // 	rte->eref = makeAlias(refname, NIL);
// // 	buildRelationAliases(rel->rd_att, alias, rte->eref);

// // 	/*
// // 	 * Set flags and access permissions.
// // 	 *
// // 	 * The initial default on access checks is always check-for-READ-access,
// // 	 * which is the right thing for all except target tables.
// // 	 */
// // 	rte->lateral = false;
// // 	rte->inh = inh;
// // 	rte->inFromCl = inFromCl;

// // 	rte->requiredPerms = ACL_SELECT;
// // 	rte->checkAsUser = InvalidOid;		/* not set-uid by default, either */
// // 	rte->selectedCols = NULL;
// // 	rte->insertedCols = NULL;
// // 	rte->updatedCols = NULL;

// // 	/*
// // 	 * Add completed RTE to pstate's range table list, but not to join list
// // 	 * nor namespace --- caller must do that if appropriate.
// // 	 */
// // 	pstate->p_rtable = lappend(pstate->p_rtable, rte);

// // 	return rte;
// // }


// //parser/parse_relation.h
// // int
// // attnameAttNum(Relation rd, const char *attname, bool sysColOK)
// // {
// // 	int			i;

// // 	for (i = 0; i < rd->rd_rel->relnatts; i++)
// // 	{
// // 		Form_pg_attribute att = rd->rd_att->attrs[i];

// // 		if (namestrcmp(&(att->attname), attname) == 0 && !att->attisdropped)
// // 			return i + 1;
// // 	}

// // 	if (sysColOK)
// // 	{
// // 		if ((i = specialAttNum(attname)) != InvalidAttrNumber)
// // 		{
// // 			if (i != ObjectIdAttributeNumber || rd->rd_rel->relhasoids)
// // 				return i;
// // 		}
// // 	}

// // 	/* on failure */
// // 	return InvalidAttrNumber;
// // }


// void
// TransferPredicateLocksToHeapRelation(Relation relation)
// {
// 	DropAllPredicateLocksFromTable(relation, true);
// }

// //bootstrap/bootstrap.h
// // void
// // index_register(Oid heap,
// // 			   Oid ind,
// // 			   IndexInfo *indexInfo)
// // {
// // 	IndexList  *newind;
// // 	MemoryContext oldcxt;

// // 	/*
// // 	 * XXX mao 10/31/92 -- don't gc index reldescs, associated info at
// // 	 * bootstrap time.  we'll declare the indexes now, but want to create them
// // 	 * later.
// // 	 */

// // 	if (nogc == NULL)
// // 		nogc = AllocSetContextCreate(NULL,
// // 									 "BootstrapNoGC",
// // 									 ALLOCSET_DEFAULT_SIZES);

// // 	oldcxt = MemoryContextSwitchTo(nogc);

// // 	newind = (IndexList *) palloc(sizeof(IndexList));
// // 	newind->il_heap = heap;
// // 	newind->il_ind = ind;
// // 	newind->il_info = (IndexInfo *) palloc(sizeof(IndexInfo));

// // 	memcpy(newind->il_info, indexInfo, sizeof(IndexInfo));
// // 	/* expressions will likely be null, but may as well copy it */
// // 	newind->il_info->ii_Expressions = (List *)
// // 		copyObject(indexInfo->ii_Expressions);
// // 	newind->il_info->ii_ExpressionsState = NIL;
// // 	/* predicate will likely be null, but may as well copy it */
// // 	newind->il_info->ii_Predicate = (List *)
// // 		copyObject(indexInfo->ii_Predicate);
// // 	newind->il_info->ii_PredicateState = NIL;
// // 	/* no exclusion constraints at bootstrap time, so no need to copy */
// // 	Assert(indexInfo->ii_ExclusionOps == NULL);
// // 	Assert(indexInfo->ii_ExclusionProcs == NULL);
// // 	Assert(indexInfo->ii_ExclusionStrats == NULL);

// // 	newind->il_next = ILHead;
// // 	ILHead = newind;

// // 	MemoryContextSwitchTo(oldcxt);
// // }
// void
// RenameConstraintById(Oid conId, const char *newname)
// {
// 	Relation	conDesc;
// 	HeapTuple	tuple;
// 	Form_pg_constraint con;

// 	conDesc = heap_open(ConstraintRelationId, RowExclusiveLock);

// 	tuple = SearchSysCacheCopy1(CONSTROID, ObjectIdGetDatum(conId));
// 	if (!HeapTupleIsValid(tuple))
// 		elog(ERROR, "cache lookup failed for constraint %u", conId);
// 	con = (Form_pg_constraint) GETSTRUCT(tuple);

// 	/*
// 	 * We need to check whether the name is already in use --- note that there
// 	 * currently is not a unique index that would catch this.
// 	 */
// 	if (OidIsValid(con->conrelid) &&
// 		ConstraintNameIsUsed(CONSTRAINT_RELATION,
// 							 con->conrelid,
// 							 con->connamespace,
// 							 newname))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 			   errmsg("constraint \"%s\" for relation \"%s\" already exists",
// 					  newname, get_rel_name(con->conrelid))));
// 	if (OidIsValid(con->contypid) &&
// 		ConstraintNameIsUsed(CONSTRAINT_DOMAIN,
// 							 con->contypid,
// 							 con->connamespace,
// 							 newname))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DUPLICATE_OBJECT),
// 				 errmsg("constraint \"%s\" for domain %s already exists",
// 						newname, format_type_be(con->contypid))));

// 	/* OK, do the rename --- tuple is a copy, so OK to scribble on it */
// 	namestrcpy(&(con->conname), newname);

// 	simple_heap_update(conDesc, &tuple->t_self, tuple);

// 	/* update the system catalog indexes */
// 	CatalogUpdateIndexes(conDesc, tuple);

// 	InvokeObjectPostAlterHook(ConstraintRelationId, conId, 0);

// 	heap_freetuple(tuple);
// 	heap_close(conDesc, RowExclusiveLock);
// }
// //catalog/index.h
// // ObjectAddress
// // index_constraint_create(Relation heapRelation,
// // 						Oid indexRelationId,
// // 						IndexInfo *indexInfo,
// // 						const char *constraintName,
// // 						char constraintType,
// // 						bool deferrable,
// // 						bool initdeferred,
// // 						bool mark_as_primary,
// // 						bool update_pgindex,
// // 						bool remove_old_dependencies,
// // 						bool allow_system_table_mods,
// // 						bool is_internal)
// // {
// // 	Oid			namespaceId = RelationGetNamespace(heapRelation);
// // 	ObjectAddress myself,
// // 				referenced;
// // 	Oid			conOid;

// // 	/* constraint creation support doesn't work while bootstrapping */
// // 	Assert(!IsBootstrapProcessingMode());

// // 	/* enforce system-table restriction */
// // 	if (!allow_system_table_mods &&
// // 		IsSystemRelation(heapRelation) &&
// // 		IsNormalProcessingMode())
// // 		ereport(ERROR,
// // 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// // 				 errmsg("user-defined indexes on system catalog tables are not supported")));

// // 	/* primary/unique constraints shouldn't have any expressions */
// // 	if (indexInfo->ii_Expressions &&
// // 		constraintType != CONSTRAINT_EXCLUSION)
// // 		elog(ERROR, "constraints cannot have index expressions");

// // 	/*
// // 	 * If we're manufacturing a constraint for a pre-existing index, we need
// // 	 * to get rid of the existing auto dependencies for the index (the ones
// // 	 * that index_create() would have made instead of calling this function).
// // 	 *
// // 	 * Note: this code would not necessarily do the right thing if the index
// // 	 * has any expressions or predicate, but we'd never be turning such an
// // 	 * index into a UNIQUE or PRIMARY KEY constraint.
// // 	 */
// // 	if (remove_old_dependencies)
// // 		deleteDependencyRecordsForClass(RelationRelationId, indexRelationId,
// // 										RelationRelationId, DEPENDENCY_AUTO);

// // 	/*
// // 	 * Construct a pg_constraint entry.
// // 	 */
// // 	conOid = CreateConstraintEntry(constraintName,
// // 								   namespaceId,
// // 								   constraintType,
// // 								   deferrable,
// // 								   initdeferred,
// // 								   true,
// // 								   RelationGetRelid(heapRelation),
// // 								   indexInfo->ii_KeyAttrNumbers,
// // 								   indexInfo->ii_NumIndexAttrs,
// // 								   InvalidOid,	/* no domain */
// // 								   indexRelationId,		/* index OID */
// // 								   InvalidOid,	/* no foreign key */
// // 								   NULL,
// // 								   NULL,
// // 								   NULL,
// // 								   NULL,
// // 								   0,
// // 								   ' ',
// // 								   ' ',
// // 								   ' ',
// // 								   indexInfo->ii_ExclusionOps,
// // 								   NULL,		/* no check constraint */
// // 								   NULL,
// // 								   NULL,
// // 								   true,		/* islocal */
// // 								   0,	/* inhcount */
// // 								   true,		/* noinherit */
// // 								   is_internal);

// // 	/*
// // 	 * Register the index as internally dependent on the constraint.
// // 	 *
// // 	 * Note that the constraint has a dependency on the table, so we don't
// // 	 * need (or want) any direct dependency from the index to the table.
// // 	 */
// // 	myself.classId = RelationRelationId;
// // 	myself.objectId = indexRelationId;
// // 	myself.objectSubId = 0;

// // 	referenced.classId = ConstraintRelationId;
// // 	referenced.objectId = conOid;
// // 	referenced.objectSubId = 0;

// // 	recordDependencyOn(&myself, &referenced, DEPENDENCY_INTERNAL);

// // 	/*
// // 	 * If the constraint is deferrable, create the deferred uniqueness
// // 	 * checking trigger.  (The trigger will be given an internal dependency on
// // 	 * the constraint by CreateTrigger.)
// // 	 */
// // 	if (deferrable)
// // 	{
// // 		CreateTrigStmt *trigger;

// // 		trigger = makeNode(CreateTrigStmt);
// // 		trigger->trigname = (constraintType == CONSTRAINT_PRIMARY) ?
// // 			"PK_ConstraintTrigger" :
// // 			"Unique_ConstraintTrigger";
// // 		trigger->relation = NULL;
// // 		trigger->funcname = SystemFuncName("unique_key_recheck");
// // 		trigger->args = NIL;
// // 		trigger->row = true;
// // 		trigger->timing = TRIGGER_TYPE_AFTER;
// // 		trigger->events = TRIGGER_TYPE_INSERT | TRIGGER_TYPE_UPDATE;
// // 		trigger->columns = NIL;
// // 		trigger->whenClause = NULL;
// // 		trigger->isconstraint = true;
// // 		trigger->deferrable = true;
// // 		trigger->initdeferred = initdeferred;
// // 		trigger->constrrel = NULL;

// // 		(void) CreateTrigger(trigger, NULL, RelationGetRelid(heapRelation),
// // 							 InvalidOid, conOid, indexRelationId, true,0);
// // 	}

// // 	/*
// // 	 * If needed, mark the table as having a primary key.  We assume it can't
// // 	 * have been so marked already, so no need to clear the flag in the other
// // 	 * case.
// // 	 *
// // 	 * Note: this might better be done by callers.  We do it here to avoid
// // 	 * exposing index_update_stats() globally, but that wouldn't be necessary
// // 	 * if relhaspkey went away.
// // 	 */
// // 	if (mark_as_primary)
// // 		index_update_stats(heapRelation,
// // 						   true,
// // 						   true,
// // 						   -1.0);

// // 	/*
// // 	 * If needed, mark the index as primary and/or deferred in pg_index.
// // 	 *
// // 	 * Note: When making an existing index into a constraint, caller must have
// // 	 * a table lock that prevents concurrent table updates; otherwise, there
// // 	 * is a risk that concurrent readers of the table will miss seeing this
// // 	 * index at all.
// // 	 */
// // 	if (update_pgindex && (mark_as_primary || deferrable))
// // 	{
// // 		Relation	pg_index;
// // 		HeapTuple	indexTuple;
// // 		Form_pg_index indexForm;
// // 		bool		dirty = false;

// // 		pg_index = heap_open(IndexRelationId, RowExclusiveLock);

// // 		indexTuple = SearchSysCacheCopy1(INDEXRELID,
// // 										 ObjectIdGetDatum(indexRelationId));
// // 		if (!HeapTupleIsValid(indexTuple))
// // 			elog(ERROR, "cache lookup failed for index %u", indexRelationId);
// // 		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

// // 		if (mark_as_primary && !indexForm->indisprimary)
// // 		{
// // 			indexForm->indisprimary = true;
// // 			dirty = true;
// // 		}

// // 		if (deferrable && indexForm->indimmediate)
// // 		{
// // 			indexForm->indimmediate = false;
// // 			dirty = true;
// // 		}

// // 		if (dirty)
// // 		{
// // 			simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
// // 			CatalogUpdateIndexes(pg_index, indexTuple);

// // 			InvokeObjectPostAlterHookArg(IndexRelationId, indexRelationId, 0,
// // 										 InvalidOid, is_internal);
// // 		}

// // 		heap_freetuple(indexTuple);
// // 		heap_close(pg_index, RowExclusiveLock);
// // 	}

// // 	return referenced;
// // }

Oid
resolve_aggregate_transtype(Oid aggfuncid,
							Oid aggtranstype,
							Oid *inputTypes,
							int numArguments)
{
	/* resolve actual type of transition state, if polymorphic */
	if (IsPolymorphicType(aggtranstype))
	{
		/* have to fetch the agg's declared input types... */
		Oid		   *declaredArgTypes;
		int			agg_nargs;

		(void) get_func_signature(aggfuncid, &declaredArgTypes, &agg_nargs);

		/*
		 * VARIADIC ANY aggs could have more actual than declared args, but
		 * such extra args can't affect polymorphic type resolution.
		 */
		Assert(agg_nargs <= numArguments);

		aggtranstype = enforce_generic_type_consistency(inputTypes,
														declaredArgTypes,
														agg_nargs,
														aggtranstype,
														false);
		pfree(declaredArgTypes);
	}
	return aggtranstype;
}


// char *
// get_database_name(Oid dbid)
// {
// 	HeapTuple	dbtuple;
// 	char	   *result;

// 	dbtuple = SearchSysCache1(DATABASEOID, ObjectIdGetDatum(dbid));
// 	if (HeapTupleIsValid(dbtuple))
// 	{
// 		result = pstrdup(NameStr(((Form_pg_database) GETSTRUCT(dbtuple))->datname));
// 		ReleaseSysCache(dbtuple);
// 	}
// 	else
// 		result = NULL;

// 	return result;
// }

// //parser/parse_utilcmd.h
// // List *
// // transformCreateSchemaStmt(CreateSchemaStmt *stmt)
// // {
// // 	CreateSchemaStmtContext cxt;
// // 	List	   *result;
// // 	ListCell   *elements;

// // 	cxt.stmtType = "CREATE SCHEMA";
// // 	cxt.schemaname = stmt->schemaname;
// // 	cxt.authrole = (RoleSpec *) stmt->authrole;
// // 	cxt.sequences = NIL;
// // 	cxt.tables = NIL;
// // 	cxt.views = NIL;
// // 	cxt.indexes = NIL;
// // 	cxt.triggers = NIL;
// // 	cxt.grants = NIL;

// // 	/*
// // 	 * Run through each schema element in the schema element list. Separate
// // 	 * statements by type, and do preliminary analysis.
// // 	 */
// // 	foreach(elements, stmt->schemaElts)
// // 	{
// // 		Node	   *element = lfirst(elements);

// // 		switch (nodeTag(element))
// // 		{
// // 			case T_CreateSeqStmt:
// // 				{
// // 					CreateSeqStmt *elp = (CreateSeqStmt *) element;

// // 					setSchemaName(cxt.schemaname, &elp->sequence->schemaname);
// // 					cxt.sequences = lappend(cxt.sequences, element);
// // 				}
// // 				break;

// // 			case T_CreateStmt:
// // 				{
// // 					CreateStmt *elp = (CreateStmt *) element;

// // 					setSchemaName(cxt.schemaname, &elp->relation->schemaname);

// // 					/*
// // 					 * XXX todo: deal with constraints
// // 					 */
// // 					cxt.tables = lappend(cxt.tables, element);
// // 				}
// // 				break;

// // 			case T_ViewStmt:
// // 				{
// // 					ViewStmt   *elp = (ViewStmt *) element;

// // 					setSchemaName(cxt.schemaname, &elp->view->schemaname);

// // 					/*
// // 					 * XXX todo: deal with references between views
// // 					 */
// // 					cxt.views = lappend(cxt.views, element);
// // 				}
// // 				break;

// // 			case T_IndexStmt:
// // 				{
// // 					IndexStmt  *elp = (IndexStmt *) element;

// // 					setSchemaName(cxt.schemaname, &elp->relation->schemaname);
// // 					cxt.indexes = lappend(cxt.indexes, element);
// // 				}
// // 				break;

// // 			case T_CreateTrigStmt:
// // 				{
// // 					CreateTrigStmt *elp = (CreateTrigStmt *) element;

// // 					setSchemaName(cxt.schemaname, &elp->relation->schemaname);
// // 					cxt.triggers = lappend(cxt.triggers, element);
// // 				}
// // 				break;

// // 			case T_GrantStmt:
// // 				cxt.grants = lappend(cxt.grants, element);
// // 				break;

// // 			default:
// // 				elog(ERROR, "unrecognized node type: %d",
// // 					 (int) nodeTag(element));
// // 		}
// // 	}

// // 	result = NIL;
// // 	result = list_concat(result, cxt.sequences);
// // 	result = list_concat(result, cxt.tables);
// // 	result = list_concat(result, cxt.views);
// // 	result = list_concat(result, cxt.indexes);
// // 	result = list_concat(result, cxt.triggers);
// // 	result = list_concat(result, cxt.grants);

// // 	return result;
// // }

// IndexAmRoutine *
// GetIndexAmRoutineByAmId(Oid amoid, bool noerror)
// {
// 	HeapTuple	tuple;
// 	Form_pg_am	amform;
// 	regproc		amhandler;

// 	/* Get handler function OID for the access method */
// 	tuple = SearchSysCache1(AMOID, ObjectIdGetDatum(amoid));
// 	if (!HeapTupleIsValid(tuple))
// 	{
// 		if (noerror)
// 			return NULL;
// 		elog(ERROR, "cache lookup failed for access method %u",
// 			 amoid);
// 	}
// 	amform = (Form_pg_am) GETSTRUCT(tuple);

// 	/* Check if it's an index access method as opposed to some other AM */
// 	if (amform->amtype != AMTYPE_INDEX)
// 	{
// 		if (noerror)
// 		{
// 			ReleaseSysCache(tuple);
// 			return NULL;
// 		}
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("access method \"%s\" is not of type %s",
// 						NameStr(amform->amname), "INDEX")));
// 	}

// 	amhandler = amform->amhandler;

// 	/* Complain if handler OID is invalid */
// 	if (!RegProcedureIsValid(amhandler))
// 	{
// 		if (noerror)
// 		{
// 			ReleaseSysCache(tuple);
// 			return NULL;
// 		}
// 		ereport(ERROR,
// 				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
// 				 errmsg("index access method \"%s\" does not have a handler",
// 						NameStr(amform->amname))));
// 	}

// 	ReleaseSysCache(tuple);

// 	/* And finally, call the handler function to get the API struct. */
// 	return GetIndexAmRoutine(amhandler);
// }

// bytea *
// index_reloptions(amoptions_function amoptions, Datum reloptions, bool validate)
// {
// 	Assert(amoptions != NULL);

// 	/* Assume function is strict */
// 	if (!PointerIsValid(DatumGetPointer(reloptions)))
// 		return NULL;

// 	return amoptions(reloptions, validate);
// }
// // Datum
// // transformRelOptions(Datum oldOptions, List *defList, char *namspace,
// // 					char *validnsps[], bool ignoreOids, bool isReset)
// // {
// // 	Datum		result;
// // 	ArrayBuildState *astate;
// // 	ListCell   *cell;

// // 	/* no change if empty list */
// // 	if (defList == NIL)
// // 		return oldOptions;

// // 	/* We build new array using accumArrayResult */
// // 	astate = NULL;

// // 	/* Copy any oldOptions that aren't to be replaced */
// // 	if (PointerIsValid(DatumGetPointer(oldOptions)))
// // 	{
// // 		ArrayType  *array = DatumGetArrayTypeP(oldOptions);
// // 		Datum	   *oldoptions;
// // 		int			noldoptions;
// // 		int			i;

// // 		deconstruct_array(array, TEXTOID, -1, false, 'i',
// // 						  &oldoptions, NULL, &noldoptions);

// // 		for (i = 0; i < noldoptions; i++)
// // 		{
// // 			text	   *oldoption = DatumGetTextP(oldoptions[i]);
// // 			char	   *text_str = VARDATA(oldoption);
// // 			int			text_len = VARSIZE(oldoption) - VARHDRSZ;

// // 			/* Search for a match in defList */
// // 			foreach(cell, defList)
// // 			{
// // 				DefElem    *def = (DefElem *) lfirst(cell);
// // 				int			kw_len;

// // 				/* ignore if not in the same namespace */
// // 				if (namspace == NULL)
// // 				{
// // 					if (def->defnamespace != NULL)
// // 						continue;
// // 				}
// // 				else if (def->defnamespace == NULL)
// // 					continue;
// // 				else if (pg_strcasecmp(def->defnamespace, namspace) != 0)
// // 					continue;

// // 				kw_len = strlen(def->defname);
// // 				if (text_len > kw_len && text_str[kw_len] == '=' &&
// // 					pg_strncasecmp(text_str, def->defname, kw_len) == 0)
// // 					break;
// // 			}
// // 			if (!cell)
// // 			{
// // 				/* No match, so keep old option */
// // 				astate = accumArrayResult(astate, oldoptions[i],
// // 										  false, TEXTOID,
// // 										  CurrentMemoryContext);
// // 			}
// // 		}
// // 	}

// // 	/*
// // 	 * If CREATE/SET, add new options to array; if RESET, just check that the
// // 	 * user didn't say RESET (option=val).  (Must do this because the grammar
// // 	 * doesn't enforce it.)
// // 	 */
// // 	foreach(cell, defList)
// // 	{
// // 		DefElem    *def = (DefElem *) lfirst(cell);

// // 		if (isReset)
// // 		{
// // 			if (def->arg != NULL)
// // 				ereport(ERROR,
// // 						(errcode(ERRCODE_SYNTAX_ERROR),
// // 					errmsg("RESET must not include values for parameters")));
// // 		}
// // 		else
// // 		{
// // 			text	   *t;
// // 			const char *value;
// // 			Size		len;

// // 			/*
// // 			 * Error out if the namespace is not valid.  A NULL namespace is
// // 			 * always valid.
// // 			 */
// // 			if (def->defnamespace != NULL)
// // 			{
// // 				bool		valid = false;
// // 				int			i;

// // 				if (validnsps)
// // 				{
// // 					for (i = 0; validnsps[i]; i++)
// // 					{
// // 						if (pg_strcasecmp(def->defnamespace,
// // 										  validnsps[i]) == 0)
// // 						{
// // 							valid = true;
// // 							break;
// // 						}
// // 					}
// // 				}

// // 				if (!valid)
// // 					ereport(ERROR,
// // 							(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
// // 							 errmsg("unrecognized parameter namespace \"%s\"",
// // 									def->defnamespace)));
// // 			}

// // 			if (ignoreOids && pg_strcasecmp(def->defname, "oids") == 0)
// // 				continue;

// // 			/* ignore if not in the same namespace */
// // 			if (namspace == NULL)
// // 			{
// // 				if (def->defnamespace != NULL)
// // 					continue;
// // 			}
// // 			else if (def->defnamespace == NULL)
// // 				continue;
// // 			else if (pg_strcasecmp(def->defnamespace, namspace) != 0)
// // 				continue;

// // 			/*
// // 			 * Flatten the DefElem into a text string like "name=arg". If we
// // 			 * have just "name", assume "name=true" is meant.  Note: the
// // 			 * namespace is not output.
// // 			 */
// // 			if (def->arg != NULL)
// // 				value = defGetString(def);
// // 			else
// // 				value = "true";
// // 			len = VARHDRSZ + strlen(def->defname) + 1 + strlen(value);
// // 			/* +1 leaves room for sprintf's trailing null */
// // 			t = (text *) palloc(len + 1);
// // 			SET_VARSIZE(t, len);
// // 			sprintf(VARDATA(t), "%s=%s", def->defname, value);

// // 			astate = accumArrayResult(astate, PointerGetDatum(t),
// // 									  false, TEXTOID,
// // 									  CurrentMemoryContext);
// // 		}
// // 	}

// // 	if (astate)
// // 		result = makeArrayResult(astate, CurrentMemoryContext);
// // 	else
// // 		result = (Datum) 0;

// // 	return result;
// // }

// //commands/tablespace.h
// // Oid
// // get_tablespace_oid(const char *tablespacename, bool missing_ok)
// // {
// // 	Oid			result;
// // 	Relation	rel;
// // 	HeapScanDesc scandesc;
// // 	HeapTuple	tuple;
// // 	ScanKeyData entry[1];

// // 	/*
// // 	 * Search pg_tablespace.  We use a heapscan here even though there is an
// // 	 * index on name, on the theory that pg_tablespace will usually have just
// // 	 * a few entries and so an indexed lookup is a waste of effort.
// // 	 */
// // 	rel = heap_open(TableSpaceRelationId, AccessShareLock);

// // 	ScanKeyInit(&entry[0],
// // 				Anum_pg_tablespace_spcname,
// // 				BTEqualStrategyNumber, F_NAMEEQ,
// // 				CStringGetDatum(tablespacename));
// // 	scandesc = heap_beginscan_catalog(rel, 1, entry);
// // 	tuple = heap_getnext(scandesc, ForwardScanDirection);

// // 	/* We assume that there can be at most one matching tuple */
// // 	if (HeapTupleIsValid(tuple))
// // 		result = HeapTupleGetOid(tuple);
// // 	else
// // 		result = InvalidOid;

// // 	heap_endscan(scandesc);
// // 	heap_close(rel, AccessShareLock);

// // 	if (!OidIsValid(result) && !missing_ok)
// // 		ereport(ERROR,
// // 				(errcode(ERRCODE_UNDEFINED_OBJECT),
// // 				 errmsg("tablespace \"%s\" does not exist",
// // 						tablespacename)));

// // 	return result;
// // }

// //commands/tablespace.h
// // Oid
// // GetDefaultTablespace(char relpersistence)
// // {
// // 	Oid			result;

// // 	/* The temp-table case is handled elsewhere */
// // 	if (relpersistence == RELPERSISTENCE_TEMP)
// // 	{
// // 		PrepareTempTablespaces();
// // 		return GetNextTempTableSpace();
// // 	}

// // 	/* Fast path for default_tablespace == "" */
// // 	if (default_tablespace == NULL || default_tablespace[0] == '\0')
// // 		return InvalidOid;

// // 	/*
// // 	 * It is tempting to cache this lookup for more speed, but then we would
// // 	 * fail to detect the case where the tablespace was dropped since the GUC
// // 	 * variable was set.  Note also that we don't complain if the value fails
// // 	 * to refer to an existing tablespace; we just silently return InvalidOid,
// // 	 * causing the new object to be created in the database's tablespace.
// // 	 */
// // 	result = get_tablespace_oid(default_tablespace, true);

// // 	/*
// // 	 * Allow explicit specification of database's default tablespace in
// // 	 * default_tablespace without triggering permissions checks.
// // 	 */
// // 	if (result == MyDatabaseTableSpace)
// // 		result = InvalidOid;
// // 	return result;
// // }

// //commands/tablespace.h
// // char *
// // get_tablespace_name(Oid spc_oid)
// // {
// // 	char	   *result;
// // 	Relation	rel;
// // 	HeapScanDesc scandesc;
// // 	HeapTuple	tuple;
// // 	ScanKeyData entry[1];

// // 	/*
// // 	 * Search pg_tablespace.  We use a heapscan here even though there is an
// // 	 * index on oid, on the theory that pg_tablespace will usually have just a
// // 	 * few entries and so an indexed lookup is a waste of effort.
// // 	 */
// // 	rel = heap_open(TableSpaceRelationId, AccessShareLock);

// // 	ScanKeyInit(&entry[0],
// // 				ObjectIdAttributeNumber,
// // 				BTEqualStrategyNumber, F_OIDEQ,
// // 				ObjectIdGetDatum(spc_oid));
// // 	scandesc = heap_beginscan_catalog(rel, 1, entry);
// // 	tuple = heap_getnext(scandesc, ForwardScanDirection);

// // 	/* We assume that there can be at most one matching tuple */
// // 	if (HeapTupleIsValid(tuple))
// // 		result = pstrdup(NameStr(((Form_pg_tablespace) GETSTRUCT(tuple))->spcname));
// // 	else
// // 		result = NULL;

// // 	heap_endscan(scandesc);
// // 	heap_close(rel, AccessShareLock);

// // 	return result;
// // }

// //parser/parser.h
// // List *
// // raw_parser(const char *str)
// // {
// // 	core_yyscan_t yyscanner;
// // 	base_yy_extra_type yyextra;
// // 	int			yyresult;

// // 	/* initialize the flex scanner */
// // 	yyscanner = scanner_init(str, &yyextra.core_yy_extra,
// // 							 ScanKeywords, NumScanKeywords);

// // 	/* base_yylex() only needs this much initialization */
// // 	yyextra.have_lookahead = false;

// // 	/* initialize the bison parser */
// // 	parser_init(&yyextra);

// // 	/* Parse! */
// // 	yyresult = base_yyparse(yyscanner);

// // 	/* Clean up (release memory) */
// // 	scanner_finish(yyscanner);

// // 	if (yyresult)				/* error */
// // 		return NIL;

// // 	return yyextra.parsetree;
// // }

// //storage/procarray.h
// // VirtualTransactionId *
// // GetCurrentVirtualXIDs(TransactionId limitXmin, bool excludeXmin0,
// // 					  bool allDbs, int excludeVacuum,
// // 					  int *nvxids)
// // {
// // 	VirtualTransactionId *vxids;
// // 	ProcArrayStruct *arrayP = procArray;
// // 	int			count = 0;
// // 	int			index;

// // 	/* allocate what's certainly enough result space */
// // 	vxids = (VirtualTransactionId *)
// // 		palloc(sizeof(VirtualTransactionId) * arrayP->maxProcs);

// // 	LWLockAcquire(ProcArrayLock, LW_SHARED);

// // 	for (index = 0; index < arrayP->numProcs; index++)
// // 	{
// // 		int			pgprocno = arrayP->pgprocnos[index];
// // 		volatile PGPROC *proc = &allProcs[pgprocno];
// // 		volatile PGXACT *pgxact = &allPgXact[pgprocno];

// // 		if (proc == MyProc)
// // 			continue;

// // 		if (excludeVacuum & pgxact->vacuumFlags)
// // 			continue;

// // 		if (allDbs || proc->databaseId == MyDatabaseId)
// // 		{
// // 			/* Fetch xmin just once - might change on us */
// // 			TransactionId pxmin = pgxact->xmin;

// // 			if (excludeXmin0 && !TransactionIdIsValid(pxmin))
// // 				continue;

// // 			/*
// // 			 * InvalidTransactionId precedes all other XIDs, so a proc that
// // 			 * hasn't set xmin yet will not be rejected by this test.
// // 			 */
// // 			if (!TransactionIdIsValid(limitXmin) ||
// // 				TransactionIdPrecedesOrEquals(pxmin, limitXmin))
// // 			{
// // 				VirtualTransactionId vxid;

// // 				GET_VXID_FROM_PGPROC(vxid, *proc);
// // 				if (VirtualTransactionIdIsValid(vxid))
// // 					vxids[count++] = vxid;
// // 			}
// // 		}
// // 	}

// // 	LWLockRelease(ProcArrayLock);

// // 	*nvxids = count;
// // 	return vxids;
// // }

// void
// CreateComments(Oid oid, Oid classoid, int32 subid, char *comment)
// {
// 	Relation	description;
// 	ScanKeyData skey[3];
// 	SysScanDesc sd;
// 	HeapTuple	oldtuple;
// 	HeapTuple	newtuple = NULL;
// 	Datum		values[Natts_pg_description];
// 	bool		nulls[Natts_pg_description];
// 	bool		replaces[Natts_pg_description];
// 	int			i;

// 	/* Reduce empty-string to NULL case */
// 	if (comment != NULL && strlen(comment) == 0)
// 		comment = NULL;

// 	/* Prepare to form or update a tuple, if necessary */
// 	if (comment != NULL)
// 	{
// 		for (i = 0; i < Natts_pg_description; i++)
// 		{
// 			nulls[i] = false;
// 			replaces[i] = true;
// 		}
// 		values[Anum_pg_description_objoid - 1] = ObjectIdGetDatum(oid);
// 		values[Anum_pg_description_classoid - 1] = ObjectIdGetDatum(classoid);
// 		values[Anum_pg_description_objsubid - 1] = Int32GetDatum(subid);
// 		values[Anum_pg_description_description - 1] = CStringGetTextDatum(comment);
// 	}

// 	/* Use the index to search for a matching old tuple */

// 	ScanKeyInit(&skey[0],
// 				Anum_pg_description_objoid,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(oid));
// 	ScanKeyInit(&skey[1],
// 				Anum_pg_description_classoid,
// 				BTEqualStrategyNumber, F_OIDEQ,
// 				ObjectIdGetDatum(classoid));
// 	ScanKeyInit(&skey[2],
// 				Anum_pg_description_objsubid,
// 				BTEqualStrategyNumber, F_INT4EQ,
// 				Int32GetDatum(subid));

// 	description = heap_open(DescriptionRelationId, RowExclusiveLock);

// 	sd = systable_beginscan(description, DescriptionObjIndexId, true,
// 							NULL, 3, skey);

// 	while ((oldtuple = systable_getnext(sd)) != NULL)
// 	{
// 		/* Found the old tuple, so delete or update it */

// 		if (comment == NULL)
// 			simple_heap_delete(description, &oldtuple->t_self);
// 		else
// 		{
// 			newtuple = heap_modify_tuple(oldtuple, RelationGetDescr(description), values,
// 										 nulls, replaces);
// 			simple_heap_update(description, &oldtuple->t_self, newtuple);
// 		}

// 		break;					/* Assume there can be only one match */
// 	}

// 	systable_endscan(sd);

// 	/* If we didn't find an old tuple, insert a new one */

// 	if (newtuple == NULL && comment != NULL)
// 	{
// 		newtuple = heap_form_tuple(RelationGetDescr(description),
// 								   values, nulls);
// 		simple_heap_insert(description, newtuple);
// 	}

// 	/* Update indexes, if necessary */
// 	if (newtuple != NULL)
// 	{
// 		CatalogUpdateIndexes(description, newtuple);
// 		heap_freetuple(newtuple);
// 	}

// 	/* Done */

// 	heap_close(description, NoLock);
// }

// //参数不一样,不能删
// Node *
// transformExpr(ParseState *pstate, Node *expr, ParseExprKind exprKind)
// {
// 	Node	   *result;
// 	ParseExprKind sv_expr_kind;

// 	/* Save and restore identity of expression type we're parsing */
// 	Assert(exprKind != EXPR_KIND_NONE);
// 	sv_expr_kind = pstate->p_expr_kind;
// 	pstate->p_expr_kind = exprKind;

// 	result = transformExprRecurse(pstate, expr);

// 	pstate->p_expr_kind = sv_expr_kind;

// 	return result;
// }

// CommandId
// HeapTupleHeaderGetCmax(HeapTupleHeader tup)
// {
// 	CommandId	cid = HeapTupleHeaderGetRawCommandId(tup);

// 	Assert(!(tup->t_infomask & HEAP_MOVED));

// 	/*
// 	 * Because GetUpdateXid() performs memory allocations if xmax is a
// 	 * multixact we can't Assert() if we're inside a critical section. This
// 	 * weakens the check, but not using GetCmax() inside one would complicate
// 	 * things too much.
// 	 */
// 	Assert(CritSectionCount > 0 ||
// 	  TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetUpdateXid(tup)));

// 	if (tup->t_infomask & HEAP_COMBOCID)
// 		return GetRealCmax(cid);
// 	else
// 		return cid;
// }
// void
// reindex_index(Oid indexId, bool skip_constraint_checks, char persistence,
// 			  int options)
// {
// 	Relation	iRel,
// 				heapRelation;
// 	Oid			heapId;
// 	IndexInfo  *indexInfo;
// 	volatile bool skipped_constraint = false;
// 	PGRUsage	ru0;

// 	pg_rusage_init(&ru0);

// 	/*
// 	 * Open and lock the parent heap relation.  ShareLock is sufficient since
// 	 * we only need to be sure no schema or data changes are going on.
// 	 */
// 	heapId = IndexGetRelation(indexId, false);
// 	heapRelation = heap_open(heapId, ShareLock);

// 	/*
// 	 * Open the target index relation and get an exclusive lock on it, to
// 	 * ensure that no one else is touching this particular index.
// 	 */
// 	iRel = index_open(indexId, AccessExclusiveLock);

// 	/*
// 	 * Don't allow reindex on temp tables of other backends ... their local
// 	 * buffer manager is not going to cope.
// 	 */
// 	if (RELATION_IS_OTHER_TEMP(iRel))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
// 			   errmsg("cannot reindex temporary tables of other sessions")));

// 	/*
// 	 * Also check for active uses of the index in the current transaction; we
// 	 * don't want to reindex underneath an open indexscan.
// 	 */
// 	CheckTableNotInUse(iRel, "REINDEX INDEX");

// 	/*
// 	 * All predicate locks on the index are about to be made invalid. Promote
// 	 * them to relation locks on the heap.
// 	 */
// 	TransferPredicateLocksToHeapRelation(iRel);

// 	PG_TRY();
// 	{
// 		/* Suppress use of the target index while rebuilding it */
// 		SetReindexProcessing(heapId, indexId);

// 		/* Fetch info needed for index_build */
// 		indexInfo = BuildIndexInfo(iRel);

// 		/* If requested, skip checking uniqueness/exclusion constraints */
// 		if (skip_constraint_checks)
// 		{
// 			if (indexInfo->ii_Unique || indexInfo->ii_ExclusionOps != NULL)
// 				skipped_constraint = true;
// 			indexInfo->ii_Unique = false;
// 			indexInfo->ii_ExclusionOps = NULL;
// 			indexInfo->ii_ExclusionProcs = NULL;
// 			indexInfo->ii_ExclusionStrats = NULL;
// 		}

// 		/* We'll build a new physical relation for the index */
// 		RelationSetNewRelfilenode(iRel, persistence, InvalidTransactionId,
// 								  InvalidMultiXactId);

// 		/* Initialize the index and rebuild */
// 		/* Note: we do not need to re-establish pkey setting */
// 		index_build(heapRelation, iRel, indexInfo, false, true);
// 	}
// 	PG_CATCH();
// 	{
// 		/* Make sure flag gets cleared on error exit */
// 		ResetReindexProcessing();
// 		PG_RE_THROW();
// 	}
// 	PG_END_TRY();
// 	ResetReindexProcessing();
// 	if (!skipped_constraint)
// 	{
// 		Relation	pg_index;
// 		HeapTuple	indexTuple;
// 		Form_pg_index indexForm;
// 		bool		index_bad;
// 		bool		early_pruning_enabled = EarlyPruningEnabled(heapRelation);

// 		pg_index = heap_open(IndexRelationId, RowExclusiveLock);

// 		indexTuple = SearchSysCacheCopy1(INDEXRELID,
// 										 ObjectIdGetDatum(indexId));
// 		if (!HeapTupleIsValid(indexTuple))
// 			elog(ERROR, "cache lookup failed for index %u", indexId);
// 		indexForm = (Form_pg_index) GETSTRUCT(indexTuple);

// 		index_bad = (!indexForm->indisvalid ||
// 					 !indexForm->indisready ||
// 					 !indexForm->indislive);
// 		if (index_bad ||
// 			(indexForm->indcheckxmin && !indexInfo->ii_BrokenHotChain) ||
// 			early_pruning_enabled)
// 		{
// 			if (!indexInfo->ii_BrokenHotChain && !early_pruning_enabled)
// 				indexForm->indcheckxmin = false;
// 			else if (index_bad || early_pruning_enabled)
// 				indexForm->indcheckxmin = true;
// 			indexForm->indisvalid = true;
// 			indexForm->indisready = true;
// 			indexForm->indislive = true;
// 			simple_heap_update(pg_index, &indexTuple->t_self, indexTuple);
// 			CatalogUpdateIndexes(pg_index, indexTuple);

// 			/*
// 			 * Invalidate the relcache for the table, so that after we commit
// 			 * all sessions will refresh the table's index list.  This ensures
// 			 * that if anyone misses seeing the pg_index row during this
// 			 * update, they'll refresh their list before attempting any update
// 			 * on the table.
// 			 */
// 			CacheInvalidateRelcache(heapRelation);
// 		}

// 		heap_close(pg_index, RowExclusiveLock);
// 	}

// 	/* Log what we did */
// 	if (options & REINDEXOPT_VERBOSE)
// 		ereport(INFO,
// 				(errmsg("index \"%s\" was reindexed",
// 						get_rel_name(indexId)),
// 				 errdetail("%s.",
// 						   pg_rusage_show(&ru0))));

// 	/* Close rels, but keep locks */
// 	index_close(iRel, NoLock);
// 	heap_close(heapRelation, NoLock);
// }

// // void
// // RangeVarCallbackOwnsTable(const RangeVar *relation,
// // 						  Oid relId, Oid oldRelId, void *arg)
// // {
// // 	char		relkind;

// // 	/* Nothing to do if the relation was not found. */
// // 	if (!OidIsValid(relId))
// // 		return;

// // 	/*
// // 	 * If the relation does exist, check whether it's an index.  But note that
// // 	 * the relation might have been dropped between the time we did the name
// // 	 * lookup and now.  In that case, there's nothing to do.
// // 	 */
// // 	relkind = get_rel_relkind(relId);
// // 	if (!relkind)
// // 		return;
// // 	if (relkind != RELKIND_RELATION && relkind != RELKIND_TOASTVALUE &&
// // 		relkind != RELKIND_MATVIEW)
// // 		ereport(ERROR,
// // 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// // 				 errmsg("\"%s\" is not a table or materialized view", relation->relname)));

// // 	/* Check permissions */
// // 	if (!pg_class_ownercheck(relId, GetUserId()))
// // 		aclcheck_error(ACLCHECK_NOT_OWNER, ACL_KIND_CLASS, relation->relname);
// // } 

bool
check_functions_in_node(Node *node, check_function_callback checker,
						void *context)
{
	switch (nodeTag(node))
	{
		case T_Aggref:
			{
				Aggref	   *expr = (Aggref *) node;

				if (checker(expr->aggfnoid, context))
					return true;
			}
			break;
		case T_WindowFunc:
			{
				WindowFunc *expr = (WindowFunc *) node;

				if (checker(expr->winfnoid, context))
					return true;
			}
			break;
		case T_FuncExpr:
			{
				FuncExpr   *expr = (FuncExpr *) node;

				if (checker(expr->funcid, context))
					return true;
			}
			break;
		case T_OpExpr:
		case T_DistinctExpr:	/* struct-equivalent to OpExpr */
		case T_NullIfExpr:		/* struct-equivalent to OpExpr */
			{
				OpExpr	   *expr = (OpExpr *) node;

				/* Set opfuncid if it wasn't set already */
				set_opfuncid(expr);
				if (checker(expr->opfuncid, context))
					return true;
			}
			break;
		case T_ScalarArrayOpExpr:
			{
				ScalarArrayOpExpr *expr = (ScalarArrayOpExpr *) node;

				set_sa_opfuncid(expr);
				if (checker(expr->opfuncid, context))
					return true;
			}
			break;
		case T_CoerceViaIO:
			{
				CoerceViaIO *expr = (CoerceViaIO *) node;
				Oid			iofunc;
				Oid			typioparam;
				bool		typisvarlena;

				/* check the result type's input function */
				getTypeInputInfo(expr->resulttype,
								 &iofunc, &typioparam);
				if (checker(iofunc, context))
					return true;
				/* check the input type's output function */
				getTypeOutputInfo(exprType((Node *) expr->arg),
								  &iofunc, &typisvarlena);
				if (checker(iofunc, context))
					return true;
			}
			break;
		case T_ArrayCoerceExpr:
			{
				ArrayCoerceExpr *expr = (ArrayCoerceExpr *) node;

				if (OidIsValid(expr->elemfuncid) &&
					checker(expr->elemfuncid, context))
					return true;
			}
			break;
		case T_RowCompareExpr:
			{
				RowCompareExpr *rcexpr = (RowCompareExpr *) node;
				ListCell   *opid;

				foreach(opid, rcexpr->opnos)
				{
					Oid			opfuncid = get_opcode(lfirst_oid(opid));

					if (checker(opfuncid, context))
						return true;
				}
			}
			break;
		default:
			break;
	}
	return false;
}

// CoercionPathType
// find_coercion_pathway(Oid targetTypeId, Oid sourceTypeId,
// 					  CoercionContext ccontext,
// 					  Oid *funcid)
// {
// 	CoercionPathType result = COERCION_PATH_NONE;
// 	HeapTuple	tuple;

// 	*funcid = InvalidOid;

// 	/* Perhaps the types are domains; if so, look at their base types */
// 	if (OidIsValid(sourceTypeId))
// 		sourceTypeId = getBaseType(sourceTypeId);
// 	if (OidIsValid(targetTypeId))
// 		targetTypeId = getBaseType(targetTypeId);

// 	/* Domains are always coercible to and from their base type */
// 	if (sourceTypeId == targetTypeId)
// 		return COERCION_PATH_RELABELTYPE;

// 	/* Look in pg_cast */
// 	tuple = SearchSysCache2(CASTSOURCETARGET,
// 							ObjectIdGetDatum(sourceTypeId),
// 							ObjectIdGetDatum(targetTypeId));

// 	if (HeapTupleIsValid(tuple))
// 	{
// 		Form_pg_cast castForm = (Form_pg_cast) GETSTRUCT(tuple);
// 		CoercionContext castcontext;

// 		/* convert char value for castcontext to CoercionContext enum */
// 		switch (castForm->castcontext)
// 		{
// 			case COERCION_CODE_IMPLICIT:
// 				castcontext = COERCION_IMPLICIT;
// 				break;
// 			case COERCION_CODE_ASSIGNMENT:
// 				castcontext = COERCION_ASSIGNMENT;
// 				break;
// 			case COERCION_CODE_EXPLICIT:
// 				castcontext = COERCION_EXPLICIT;
// 				break;
// 			default:
// 				elog(ERROR, "unrecognized castcontext: %d",
// 					 (int) castForm->castcontext);
// 				castcontext =(CoercionContext) 0;	/* keep compiler quiet */
// 				break;
// 		}

// 		/* Rely on ordering of enum for correct behavior here */
// 		if (ccontext >= castcontext)
// 		{
// 			switch (castForm->castmethod)
// 			{
// 				case COERCION_METHOD_FUNCTION:
// 					result = COERCION_PATH_FUNC;
// 					*funcid = castForm->castfunc;
// 					break;
// 				case COERCION_METHOD_INOUT:
// 					result = COERCION_PATH_COERCEVIAIO;
// 					break;
// 				case COERCION_METHOD_BINARY:
// 					result = COERCION_PATH_RELABELTYPE;
// 					break;
// 				default:
// 					elog(ERROR, "unrecognized castmethod: %d",
// 						 (int) castForm->castmethod);
// 					break;
// 			}
// 		}

// 		ReleaseSysCache(tuple);
// 	}
// 	else
// 	{
// 		/*
// 		 * If there's no pg_cast entry, perhaps we are dealing with a pair of
// 		 * array types.  If so, and if the element types have a suitable cast,
// 		 * report that we can coerce with an ArrayCoerceExpr.
// 		 *
// 		 * Note that the source type can be a domain over array, but not the
// 		 * target, because ArrayCoerceExpr won't check domain constraints.
// 		 *
// 		 * Hack: disallow coercions to oidvector and int2vector, which
// 		 * otherwise tend to capture coercions that should go to "real" array
// 		 * types.  We want those types to be considered "real" arrays for many
// 		 * purposes, but not this one.  (Also, ArrayCoerceExpr isn't
// 		 * guaranteed to produce an output that meets the restrictions of
// 		 * these datatypes, such as being 1-dimensional.)
// 		 */
// 		if (targetTypeId != OIDVECTOROID && targetTypeId != INT2VECTOROID)
// 		{
// 			Oid			targetElem;
// 			Oid			sourceElem;

// 			if ((targetElem = get_element_type(targetTypeId)) != InvalidOid &&
// 			(sourceElem = get_base_element_type(sourceTypeId)) != InvalidOid)
// 			{
// 				CoercionPathType elempathtype;
// 				Oid			elemfuncid;

// 				elempathtype = find_coercion_pathway(targetElem,
// 													 sourceElem,
// 													 ccontext,
// 													 &elemfuncid);
// 				if (elempathtype != COERCION_PATH_NONE &&
// 					elempathtype != COERCION_PATH_ARRAYCOERCE)
// 				{
// 					*funcid = elemfuncid;
// 					if (elempathtype == COERCION_PATH_COERCEVIAIO)
// 						result = COERCION_PATH_COERCEVIAIO;
// 					else
// 						result = COERCION_PATH_ARRAYCOERCE;
// 				}
// 			}
// 		}

// 		/*
// 		 * If we still haven't found a possibility, consider automatic casting
// 		 * using I/O functions.  We allow assignment casts to string types and
// 		 * explicit casts from string types to be handled this way. (The
// 		 * CoerceViaIO mechanism is a lot more general than that, but this is
// 		 * all we want to allow in the absence of a pg_cast entry.) It would
// 		 * probably be better to insist on explicit casts in both directions,
// 		 * but this is a compromise to preserve something of the pre-8.3
// 		 * behavior that many types had implicit (yipes!) casts to text.
// 		 */
// 		if (result == COERCION_PATH_NONE)
// 		{
// 			if (ccontext >= COERCION_ASSIGNMENT &&
// 				TypeCategory(targetTypeId) == TYPCATEGORY_STRING)
// 				result = COERCION_PATH_COERCEVIAIO;
// 			else if (ccontext >= COERCION_EXPLICIT &&
// 					 TypeCategory(sourceTypeId) == TYPCATEGORY_STRING)
// 				result = COERCION_PATH_COERCEVIAIO;
// 		}
// 	}

// 	return result;
// }
// //parser/parse_coerce.h
// // bool
// // IsBinaryCoercible(Oid srctype, Oid targettype)
// // {
// // 	HeapTuple	tuple;
// // 	Form_pg_cast castForm;
// // 	bool		result;

// // 	/* Fast path if same type */
// // 	if (srctype == targettype)
// // 		return true;

// // 	/* Anything is coercible to ANY or ANYELEMENT */
// // 	if (targettype == ANYOID || targettype == ANYELEMENTOID)
// // 		return true;

// // 	/* If srctype is a domain, reduce to its base type */
// // 	if (OidIsValid(srctype))
// // 		srctype = getBaseType(srctype);

// // 	/* Somewhat-fast path for domain -> base type case */
// // 	if (srctype == targettype)
// // 		return true;

// // 	/* Also accept any array type as coercible to ANYARRAY */
// // 	if (targettype == ANYARRAYOID)
// // 		if (type_is_array(srctype))
// // 			return true;

// // 	/* Also accept any non-array type as coercible to ANYNONARRAY */
// // 	if (targettype == ANYNONARRAYOID)
// // 		if (!type_is_array(srctype))
// // 			return true;

// // 	/* Also accept any enum type as coercible to ANYENUM */
// // 	if (targettype == ANYENUMOID)
// // 		if (type_is_enum(srctype))
// // 			return true;

// // 	/* Also accept any range type as coercible to ANYRANGE */
// // 	if (targettype == ANYRANGEOID)
// // 		if (type_is_range(srctype))
// // 			return true;

// // 	/* Also accept any composite type as coercible to RECORD */
// // 	if (targettype == RECORDOID)
// // 		if (ISCOMPLEX(srctype))
// // 			return true;

// // 	/* Also accept any composite array type as coercible to RECORD[] */
// // 	if (targettype == RECORDARRAYOID)
// // 		if (is_complex_array(srctype))
// // 			return true;

// // 	/* Else look in pg_cast */
// // 	tuple = SearchSysCache2(CASTSOURCETARGET,
// // 							ObjectIdGetDatum(srctype),
// // 							ObjectIdGetDatum(targettype));
// // 	if (!HeapTupleIsValid(tuple))
// // 		return false;			/* no cast */
// // 	castForm = (Form_pg_cast) GETSTRUCT(tuple);

// // 	result = (castForm->castmethod == COERCION_METHOD_BINARY &&
// // 			  castForm->castcontext == COERCION_CODE_IMPLICIT);

// // 	ReleaseSysCache(tuple);

// // 	return result;
// // }

// void
// PrepareTempTablespaces(void)
// {
// 	char	   *rawname;
// 	List	   *namelist;
// 	Oid		   *tblSpcs;
// 	int			numSpcs;
// 	ListCell   *l;

// 	/* No work if already done in current transaction */
// 	if (TempTablespacesAreSet())
// 		return;

// 	/*
// 	 * Can't do catalog access unless within a transaction.  This is just a
// 	 * safety check in case this function is called by low-level code that
// 	 * could conceivably execute outside a transaction.  Note that in such a
// 	 * scenario, fd.c will fall back to using the current database's default
// 	 * tablespace, which should always be OK.
// 	 */
// 	if (!IsTransactionState())
// 		return;

// 	/* Need a modifiable copy of string */
// 	rawname = pstrdup(og_knl15->temp_tablespaces);

// 	/* Parse string into list of identifiers */
// 	if (!SplitIdentifierString(rawname, ',', &namelist))
// 	{
// 		/* syntax error in name list */
// 		SetTempTablespaces(NULL, 0);
// 		pfree(rawname);
// 		list_free(namelist);
// 		return;
// 	}

// 	/* Store tablespace OIDs in an array in TopTransactionContext */
// 	tblSpcs = (Oid *) MemoryContextAlloc(TopTransactionContext,
// 										 list_length(namelist) * sizeof(Oid));
// 	numSpcs = 0;
// 	foreach(l, namelist)
// 	{
// 		char	   *curname = (char *) lfirst(l);
// 		Oid			curoid;
// 		AclResult	aclresult;

// 		/* Allow an empty string (signifying database default) */
// 		if (curname[0] == '\0')
// 		{
// 			tblSpcs[numSpcs++] = InvalidOid;
// 			continue;
// 		}

// 		/* Else verify that name is a valid tablespace name */
// 		curoid = get_tablespace_oid(curname, true);
// 		if (curoid == InvalidOid)
// 		{
// 			/* Skip any bad list elements */
// 			continue;
// 		}

// 		/*
// 		 * Allow explicit specification of database's default tablespace in
// 		 * temp_tablespaces without triggering permissions checks.
// 		 */
// 		if (curoid == MyDatabaseTableSpace)
// 		{
// 			tblSpcs[numSpcs++] = InvalidOid;
// 			continue;
// 		}

// 		/* Check permissions similarly */
// 		aclresult = pg_tablespace_aclcheck(curoid, GetUserId(),
// 										   ACL_CREATE);
// 		if (aclresult != ACLCHECK_OK)
// 			continue;

// 		tblSpcs[numSpcs++] = curoid;
// 	}

// 	SetTempTablespaces(tblSpcs, numSpcs);

// 	pfree(rawname);
// 	list_free(namelist);
// }

// //storage/sinvaladt.h
// // void
// // SharedInvalBackendInit(bool sendOnly)
// // {
// // 	int			index;
// // 	ProcState  *stateP = NULL;
// // 	SISeg	   *segP = 	;

// // 	/*
// // 	 * This can run in parallel with read operations, but not with write
// // 	 * operations, since SIInsertDataEntries relies on lastBackend to set
// // 	 * hasMessages appropriately.
// // 	 */
// // 	LWLockAcquire(SInvalWriteLock, LW_EXCLUSIVE);

// // 	/* Look for a free entry in the procState array */
// // 	for (index = 0; index < segP->lastBackend; index++)
// // 	{
// // 		if (segP->procState[index].procPid == 0)		/* inactive slot? */
// // 		{
// // 			stateP = &segP->procState[index];
// // 			break;
// // 		}
// // 	}

// // 	if (stateP == NULL)
// // 	{
// // 		if (segP->lastBackend < segP->maxBackends)
// // 		{
// // 			stateP = &segP->procState[segP->lastBackend];
// // 			Assert(stateP->procPid == 0);
// // 			segP->lastBackend++;
// // 		}
// // 		else
// // 		{
// // 			/*
// // 			 * out of procState slots: MaxBackends exceeded -- report normally
// // 			 */
// // 			MyBackendId = InvalidBackendId;
// // 			LWLockRelease(SInvalWriteLock);
// // 			ereport(FATAL,
// // 					(errcode(ERRCODE_TOO_MANY_CONNECTIONS),
// // 					 errmsg("sorry, too many clients already")));
// // 		}
// // 	}

// // 	MyBackendId = (stateP - &segP->procState[0]) + 1;

// // 	/* Advertise assigned backend ID in MyProc */
// // 	MyProc->backendId = MyBackendId;

// // 	/* Fetch next local transaction ID into local memory */
// // 	nextLocalTransactionId = stateP->nextLXID;

// // 	/* mark myself active, with all extant messages already read */
// // 	stateP->procPid = MyProcPid;
// // 	stateP->proc = MyProc;
// // 	stateP->nextMsgNum = segP->maxMsgNum;
// // 	stateP->resetState = false;
// // 	stateP->signaled = false;
// // 	stateP->hasMessages = false;
// // 	stateP->sendOnly = sendOnly;

// // 	LWLockRelease(SInvalWriteLock);

// // 	/* register exit routine to mark my entry inactive at exit */
// // 	on_shmem_exit(CleanupInvalidationState, PointerGetDatum(segP));

// // 	elog(DEBUG4, "my backend ID is %d", MyBackendId);
// // }

// Node *
// coerce_to_boolean(ParseState *pstate, Node *node,
// 				  const char *constructName)
// {
// 	Oid			inputTypeId = exprType(node);

// 	if (inputTypeId != BOOLOID)
// 	{
// 		Node	   *newnode;

// 		newnode = coerce_to_target_type(pstate, node, inputTypeId,
// 										BOOLOID, -1,
// 										COERCION_ASSIGNMENT,
// 										COERCE_IMPLICIT_CAST,
// 										-1);
// 		if (newnode == NULL)
// 			ereport(ERROR,
// 					(errcode(ERRCODE_DATATYPE_MISMATCH),
// 			/* translator: first %s is name of a SQL construct, eg WHERE */
// 				   errmsg("argument of %s must be type boolean, not type %s",
// 						  constructName, format_type_be(inputTypeId)),
// 					 parser_errposition(pstate, exprLocation(node))));
// 		node = newnode;
// 	}

// 	if (expression_returns_set(node))
// 		ereport(ERROR,
// 				(errcode(ERRCODE_DATATYPE_MISMATCH),
// 		/* translator: %s is name of a SQL construct, eg WHERE */
// 				 errmsg("argument of %s must not return a set",
// 						constructName),
// 				 parser_errposition(pstate, exprLocation(node))));

// 	return node;
// }
// //parser/parse_relation.h
// // Oid
// // attnumTypeId(Relation rd, int attid)
// // {
// // 	if (attid <= 0)
// // 	{
// // 		Form_pg_attribute sysatt;

// // 		sysatt = SystemAttributeDefinition(attid, rd->rd_rel->relhasoids);
// // 		return sysatt->atttypid;
// // 	}
// // 	if (attid > rd->rd_att->natts)
// // 		elog(ERROR, "invalid attribute number %d", attid);
// // 	return rd->rd_att->attrs[attid - 1]->atttypid;
// // }
// TimeoutId
// RegisterTimeout(TimeoutId id, timeout_handler_proc handler)
// {
// 	Assert(all_timeouts_initialized);

// 	/* There's no need to disable the signal handler here. */

// 	if (id >= USER_TIMEOUT)
// 	{
// 		/* Allocate a user-defined timeout reason */
// 		for (id = USER_TIMEOUT; id < MAX_TIMEOUTS;(TimeoutId)(id+1))
// 			if (all_timeouts[id].timeout_handler == NULL)
// 				break;
// 		if (id >= MAX_TIMEOUTS)
// 			ereport(FATAL,
// 					(errcode(ERRCODE_CONFIGURATION_LIMIT_EXCEEDED),
// 					 errmsg("cannot add more timeout reasons")));
// 	}

// 	Assert(all_timeouts[id].timeout_handler == NULL);

// 	all_timeouts[id].timeout_handler = handler;

// 	return id;
// }

// void
// CheckDeadLockAlert(void)
// {
// 	int			save_errno = errno;

// 	got_deadlock_timeout = true;

// 	/*
// 	 * Have to set the latch again, even if handle_sig_alarm already did. Back
// 	 * then got_deadlock_timeout wasn't yet set... It's unlikely that this
// 	 * ever would be a problem, but setting a set latch again is cheap.
// 	 */
// 	SetLatch(MyLatch);
// 	errno = save_errno;
// }

// //storage/ipc.h
// // void
// // on_shmem_exit(pg_on_exit_callback function, Datum arg)
// // {
// // 	if (on_shmem_exit_index >= MAX_ON_EXITS)
// // 		ereport(FATAL,
// // 				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
// // 				 errmsg_internal("out of on_shmem_exit slots")));

// // 	on_shmem_exit_list[on_shmem_exit_index].function = function;
// // 	on_shmem_exit_list[on_shmem_exit_index].arg = arg;

// // 	++on_shmem_exit_index;

// // 	if (!atexit_callback_setup)
// // 	{
// // 		atexit(atexit_callback);
// // 		atexit_callback_setup = true;
// // 	}
// // }

// //storage/ipc.h
// // void
// // before_shmem_exit(pg_on_exit_callback function, Datum arg)
// // {
// // 	if (before_shmem_exit_index >= MAX_ON_EXITS)
// // 		ereport(FATAL,
// // 				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
// // 				 errmsg_internal("out of before_shmem_exit slots")));

// // 	before_shmem_exit_list[before_shmem_exit_index].function = function;
// // 	before_shmem_exit_list[before_shmem_exit_index].arg = arg;

// // 	++before_shmem_exit_index;

// // 	if (!atexit_callback_setup)
// // 	{
// // 		atexit(atexit_callback);
// // 		atexit_callback_setup = true;
// // 	}
// // }
// bool
// IsAutoVacuumLauncherProcess(void)
// {
// 	return am_autovacuum_launcher;
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

// //backend/utils/init/miscinit.cpp
// // void
// // SetDatabasePath(const char *path)
// // {
// // 	/* This should happen only once per process */
// // 	Assert(!DatabasePath);
// // 	DatabasePath = MemoryContextStrdup(TopMemoryContext, path);
// // }

// int
// get_object_catcache_oid(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->oid_catcache_id;
// }
// Oid
// get_object_oid_index(Oid class_id)
// {
// 	const ObjectPropertyType *prop = get_object_property_data(class_id);

// 	return prop->oid_index_oid;
// }
// char *
// format_procedure_qualified(Oid procedure_oid)
// {
// 	return format_procedure_internal(procedure_oid, true);
// }
// void
// format_procedure_parts(Oid procedure_oid, List **objnames, List **objargs)
// {
// 	HeapTuple	proctup;
// 	Form_pg_proc procform;
// 	int			nargs;
// 	int			i;

// 	proctup = SearchSysCache1(PROCOID, ObjectIdGetDatum(procedure_oid));

// 	if (!HeapTupleIsValid(proctup))
// 		elog(ERROR, "cache lookup failed for procedure with OID %u", procedure_oid);

// 	procform = (Form_pg_proc) GETSTRUCT(proctup);
// 	nargs = procform->pronargs;

// 	*objnames = list_make2(get_namespace_name_or_temp(procform->pronamespace),
// 						   pstrdup(NameStr(procform->proname)));
// 	*objargs = NIL;
// 	for (i = 0; i < nargs; i++)
// 	{
// 		Oid			thisargtype = procform->proargtypes.values[i];

// 		*objargs = lappend(*objargs, format_type_be_qualified(thisargtype));
// 	}

// 	ReleaseSysCache(proctup);
// }
// char *
// format_type_be_qualified(Oid type_oid)
// {
// 	return format_type_internal(type_oid, -1, false, false, true);
// }
// char *
// get_namespace_name_or_temp(Oid nspid)
// {
// 	if (isTempNamespace(nspid))
// 		return "pg_temp";
// 	else
// 		return get_namespace_name(nspid);
// }
// char *
// format_operator_qualified(Oid operator_oid)
// {
// 	return format_operator_internal(operator_oid, true);
// }
// void
// format_operator_parts(Oid operator_oid, List **objnames, List **objargs)
// {
// 	HeapTuple	opertup;
// 	Form_pg_operator oprForm;

// 	opertup = SearchSysCache1(OPEROID, ObjectIdGetDatum(operator_oid));
// 	if (!HeapTupleIsValid(opertup))
// 		elog(ERROR, "cache lookup failed for operator with OID %u",
// 			 operator_oid);

// 	oprForm = (Form_pg_operator) GETSTRUCT(opertup);
// 	*objnames = list_make2(get_namespace_name_or_temp(oprForm->oprnamespace),
// 						   pstrdup(NameStr(oprForm->oprname)));
// 	*objargs = NIL;
// 	if (oprForm->oprleft)
// 		*objargs = lappend(*objargs,
// 						   format_type_be_qualified(oprForm->oprleft));
// 	if (oprForm->oprright)
// 		*objargs = lappend(*objargs,
// 						   format_type_be_qualified(oprForm->oprright));

// 	ReleaseSysCache(opertup);
// }

// char *
// format_type_be_qualified(Oid type_oid)
// {
// 	return format_type_internal(type_oid, -1, false, false, true);
// }
// char *
// get_namespace_name_or_temp(Oid nspid)
// {
// 	if (isTempNamespace(nspid))
// 		return "pg_temp";
// 	else
// 		return get_namespace_name(nspid);
// }
// char *
// format_type_be_qualified(Oid type_oid)
// {
// 	return format_type_internal(type_oid, -1, false, false, true);
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

//catalog/storage.h
// void
// RelationCreateStorage(RelFileNode rnode, char relpersistence)
// {
// 	PendingRelDelete *pending;
// 	SMgrRelation srel;
// 	BackendId	backend;
// 	bool		needs_wal;

// 	switch (relpersistence)
// 	{
// 		case RELPERSISTENCE_TEMP:
// 			backend = BackendIdForTempRelations();
// 			needs_wal = false;
// 			break;
// 		case RELPERSISTENCE_UNLOGGED:
// 			backend = InvalidBackendId;
// 			needs_wal = false;
// 			break;
// 		case RELPERSISTENCE_PERMANENT:
// 			backend = InvalidBackendId;
// 			needs_wal = true;
// 			break;
// 		default:
// 			elog(ERROR, "invalid relpersistence: %c", relpersistence);
// 			return;				/* placate compiler */
// 	}

// 	srel = smgropen(rnode, backend);
// 	smgrcreate(srel, MAIN_FORKNUM, false);

// 	if (needs_wal)
// 		log_smgrcreate(&srel->smgr_rnode.node, MAIN_FORKNUM);

// 	/* Add the relation to the list of stuff to delete at abort */
// 	pending = (PendingRelDelete *)
// 		MemoryContextAlloc(TopMemoryContext, sizeof(PendingRelDelete));
// 	pending->relnode = rnode;
// 	pending->backend = backend;
// 	pending->atCommit = false;	/* delete if abort */
// 	pending->nestLevel = GetCurrentTransactionNestLevel();
// 	pending->next = pendingDeletes;
// 	pendingDeletes = pending;
// }


void
ProcArrayGetReplicationSlotXmin(TransactionId *xmin,
								TransactionId *catalog_xmin)
{
	LWLockAcquire(ProcArrayLock, LW_SHARED);

	if (xmin != NULL)
		*xmin = procArray->replication_slot_xmin;

	if (catalog_xmin != NULL)
		*catalog_xmin = procArray->replication_slot_catalog_xmin;

	LWLockRelease(ProcArrayLock);
}

// char
// func_parallel(Oid funcid)
// {
// 	HeapTuple	tp;
// 	char		result;

// 	tp = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcid));
// 	if (!HeapTupleIsValid(tp))
// 		elog(ERROR, "cache lookup failed for function %u", funcid);

// 	//需要在结构体Form_pg_proc中增加成员proparallel
// 	result = ((Form_pg_proc) GETSTRUCT(tp))->proparallel;
// 	ReleaseSysCache(tp);
// 	return result;
// } 


Relids
find_childrel_parents(PlannerInfo *root, RelOptInfo *rel)
{
	Relids		result = NULL;

	do
	{
		AppendRelInfo *appinfo = find_childrel_appendrelinfo(root, rel);
		Index		prelid = appinfo->parent_relid;

		result = bms_add_member(result, prelid);

		/* traverse up to the parent rel, loop if it's also a child rel */
		rel = find_base_rel(root, prelid);
	} while (rel->reloptkind == RELOPT_OTHER_MEMBER_REL);

	Assert(rel->reloptkind == RELOPT_BASEREL);

	return result;
} 