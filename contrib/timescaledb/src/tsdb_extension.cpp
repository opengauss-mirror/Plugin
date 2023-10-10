#include "postgres.h"
#include "compat.h"

#include "commands/cluster.h"
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
#include "utils/evtcache.h"

#include "catalog/storage.h"
#include "tsdb_static.cpp"



bool
IsInParallelMode(void)
{
	return false;
} 

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
	ProjectionPath *pathnode = makeNode(ProjectionPath);

	pathnode->path.pathtype = T_BaseResult;
	pathnode->path.parent = rel;
	/* For now, assume we are above any joins, so no parameterization */
	pathnode->path.param_info = NULL;

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
	if (is_projection_capable_path(subpath))
	{
		/* No separate Result node needed */
		pathnode->dummypp = true;

		/*
		 * Set cost of plan as subpath's cost, adjusted for tlist replacement.
		 */
		pathnode->path.rows = subpath->rows;
		pathnode->path.startup_cost = subpath->startup_cost +
			(target->cost.startup);
		pathnode->path.total_cost = subpath->total_cost +
			(target->cost.startup) +
			(target->cost.per_tuple) * subpath->rows;
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

	SET_LOCKTAG_SPECULATIVE_INSERTION(tag, xid, token);

	Assert(TransactionIdIsValid(xid));
	Assert(token != 0);

	(void) LockAcquire(&tag, ShareLock, false, false);
	LockRelease(&tag, ShareLock, false);
} 

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