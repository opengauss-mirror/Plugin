//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		spq_wrappers.cpp
//
//	@doc:
//		Implementation of SPQDB function wrappers. Note that we should never
// 		return directly from inside the PG_TRY() block, in order to restore
//		the long jump stack. That is why we save the return value of the SPQDB
//		function to a local variable and return it after the PG_END_TRY().
//		./README file contains the sources (caches and catalog tables) of metadata
//		requested by the optimizer and retrieved using SPQDB function wrappers. Any
//		change to optimizer's requested metadata should also be recorded in ./README file.
//
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spq_optimizer_util/spq_wrappers.h"
#include "spqplugin.h"

#include "spqos/base.h"
#include "spqos/error/CAutoExceptionStack.h"
#include "spqos/error/CException.h"

#include "spq_optimizer_util/utils/spqdbdefs.h"
#include "naucrates/exception.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_constraint.h"
#include "utils/fmgroids.h"
#include "utils/memutils.h"
#include "utils/numeric.h"
#include "utils/lsyscache.h"
#include "utils/selfuncs.h"
#include "spq/spq_util.h"
#include "spq/spq_hash.h"
#include "optimizer/clauses.h"
#include "access/hash.h"
#include "optimizer/planmem_walker.h"
#include "optimizer/plancat.h"
#include "catalog/pg_aggregate.h"
#include "parser/parse_agg.h"
#include "spqos/error/CAutoExceptionStack.h"
#include "parser/parse_coerce.h"
#include "utils/inval.h"

#define SPQ_WRAP_START                                                             \
    sigjmp_buf local_sigjmp_buf;                                                   \
    spqos::CAutoExceptionStack aes((void **) &t_thrd.log_cxt.PG_exception_stack,   \
                                   (void **) &t_thrd.log_cxt.error_context_stack); \
    if (0 == sigsetjmp(local_sigjmp_buf, 0)) {                                     \
        aes.SetLocalJmp(&local_sigjmp_buf)

#define SPQ_WRAP_END                                                               \
    } else {                                                                       \
        SPQOS_RAISE(spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError);                    \
    }
using namespace spqos;

bool
spqdb::BoolFromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetBool(d);
	}
	SPQ_WRAP_END;
	return false;
}

Datum
spqdb::DatumFromBool(bool b)
{
	SPQ_WRAP_START;
	{
		return BoolGetDatum(b);
	}
	SPQ_WRAP_END;
	return 0;
}

char
spqdb::CharFromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetChar(d);
	}
	SPQ_WRAP_END;
	return '\0';
}

Datum
spqdb::DatumFromChar(char c)
{
	SPQ_WRAP_START;
	{
		return CharGetDatum(c);
	}
	SPQ_WRAP_END;
	return 0;
}

int8
spqdb::Int8FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetInt8(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromInt8(int8 i8)
{
	SPQ_WRAP_START;
	{
		return Int8GetDatum(i8);
	}
	SPQ_WRAP_END;
	return 0;
}

uint8
spqdb::Uint8FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt8(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromUint8(uint8 ui8)
{
	SPQ_WRAP_START;
	{
		return UInt8GetDatum(ui8);
	}
	SPQ_WRAP_END;
	return 0;
}

int16
spqdb::Int16FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetInt16(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromInt16(int16 i16)
{
	SPQ_WRAP_START;
	{
		return Int16GetDatum(i16);
	}
	SPQ_WRAP_END;
	return 0;
}

uint16
spqdb::Uint16FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt16(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromUint16(uint16 ui16)
{
	SPQ_WRAP_START;
	{
		return UInt16GetDatum(ui16);
	}
	SPQ_WRAP_END;
	return 0;
}

int32
spqdb::Int32FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetInt32(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromInt32(int32 i32)
{
	SPQ_WRAP_START;
	{
		return Int32GetDatum(i32);
	}
	SPQ_WRAP_END;
	return 0;
}

uint32
spqdb::lUint32FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt32(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromUint32(uint32 ui32)
{
	SPQ_WRAP_START;
	{
		return UInt32GetDatum(ui32);
	}
	SPQ_WRAP_END;
	return 0;
}

int64
spqdb::Int64FromDatum(Datum d)
{
	Datum d2 = d;
	SPQ_WRAP_START;
	{
		return DatumGetInt64(d2);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromInt64(int64 i64)
{
	int64 ii64 = i64;
	SPQ_WRAP_START;
	{
		return Int64GetDatum(ii64);
	}
	SPQ_WRAP_END;
	return 0;
}

uint64
spqdb::Uint64FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt64(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromUint64(uint64 ui64)
{
	SPQ_WRAP_START;
	{
		return UInt64GetDatum(ui64);
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::OidFromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetObjectId(d);
	}
	SPQ_WRAP_END;
	return 0;
}

void *
spqdb::PointerFromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetPointer(d);
	}
	SPQ_WRAP_END;
	return NULL;
}

float4
spqdb::Float4FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetFloat4(d);
	}
	SPQ_WRAP_END;
	return 0;
}

float8
spqdb::Float8FromDatum(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetFloat8(d);
	}
	SPQ_WRAP_END;
	return 0;
}

Datum
spqdb::DatumFromPointer(const void *p)
{
	SPQ_WRAP_START;
	{
		return PointerGetDatum(p);
	}
	SPQ_WRAP_END;
	return 0;
}


bool
spqdb::AggregateExists(Oid oid)
{
	SPQ_WRAP_START;
	{
		return aggregate_exists(oid);
	}
	SPQ_WRAP_END;
	return false;
}

Bitmapset *
spqdb::BmsAddMember(Bitmapset *a, int x)
{
	SPQ_WRAP_START;
	{
		return bms_add_member(a, x);
	}
	SPQ_WRAP_END;
	return NULL;
}

void *
spqdb::CopyObject(void *from)
{
	SPQ_WRAP_START;
	{
		return copyObject(from);
	}
	SPQ_WRAP_END;
	return NULL;
}

Size
spqdb::DatumSize(Datum value, bool type_by_val, int iTypLen)
{
	SPQ_WRAP_START;
	{
		return datumGetSize(value, type_by_val, iTypLen);
	}
	SPQ_WRAP_END;
	return 0;
}

Node *
spqdb::MutateExpressionTree(Node *node, Node *(*mutator)(), void *context)
{
	SPQ_WRAP_START;
	{
		return expression_tree_mutator(node, (Node* (*)(Node*, void*))mutator, context);
	}
	SPQ_WRAP_END;
	return NULL;
}

bool
spqdb::WalkExpressionTree(Node *node, bool (*walker)(), void *context)
{
	SPQ_WRAP_START;
	{
		return expression_tree_walker(node, walker, context);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::ExprType(Node *expr)
{
	SPQ_WRAP_START;
	{
		return exprType(expr);
	}
	SPQ_WRAP_END;
	return 0;
}

int32
spqdb::ExprTypeMod(Node *expr)
{
	SPQ_WRAP_START;
	{
		return exprTypmod(expr);
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::ExprCollation(Node *expr)
{
	SPQ_WRAP_START;
	{
		if (expr && IsA(expr, List))
		{
			// SPQDB_91_MERGE_FIXME: collation
			List *exprlist = (List *) expr;
			ListCell *lc;

			Oid collation = InvalidOid;
			foreach (lc, exprlist)
			{
				Node *expr = (Node *) lfirst(lc);
				if ((collation = exprCollation(expr)) != InvalidOid)
				{
					break;
				}
			}
			return collation;
		}
		else
		{
			return exprCollation(expr);
		}
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::TypeCollation(Oid type)
{
	SPQ_WRAP_START;
	{
		Oid collation = InvalidOid;
		if (type_is_collatable(type))
		{
			collation = DEFAULT_COLLATION_OID;
		}
		return collation;
	}
	SPQ_WRAP_END;
	return 0;
}


List *
spqdb::ExtractNodesPlan(Plan *pl, int node_tag, bool descend_into_subqueries)
{
	SPQ_WRAP_START;
	{
		return extract_nodes_plan(pl, node_tag, descend_into_subqueries);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::ExtractNodesExpression(Node *node, int node_tag,
							 bool descend_into_subqueries)
{
	SPQ_WRAP_START;
	{
		return extract_nodes_expression(node, node_tag,
										descend_into_subqueries);
	}
	SPQ_WRAP_END;
	return NIL;
}
void
spqdb::FreeAttrStatsSlot(AttStatsSlot *sslot)
{
	SPQ_WRAP_START;
	{
		spq_free_attstatsslot(sslot);
		return;
	}
	SPQ_WRAP_END;
}

// spq partition support
#if 0
bool
spqdb::IsFuncAllowedForPartitionSelection(Oid funcid)
{
	SPQ_WRAP_START;
	switch (funcid) {
            // These are the functions we have allowed as lossy casts for Partition selection.
            // For range partition selection, the logic in ORCA checks on bounds of the partition ranges.
            // Hence these must be increasing functions.
        case F_TIMESTAMP_DATE: // date(timestamp) -> date
        case F_DTOI4:          // int4(float8) -> int4
        case F_FTOI4:          // int4(float4) -> int4
        case F_INT82:          // int2(int8) -> int2
        case F_INT84:          // int4(int8) -> int4
        case F_I4TOI2:         // int2(int4) -> int2
        case F_FTOI8:          // int8(float4) -> int8
        case F_FTOI2:          // int2(float4) -> int2
        case F_FLOAT4_NUMERIC: // numeric(float4) -> numeric
        case F_DTOI8:          // int8(float8) -> int8
        case F_DTOI2:          // int2(float4) -> int2
        case F_DTOF:           // float4(float8) -> float4
        case F_FLOAT8_NUMERIC: // numeric(float8) -> numeric
        case F_NUMERIC_INT8:   // int8(numeric) -> int8
        case F_NUMERIC_INT2:   // int2(numeric) -> int2
        case F_NUMERIC_INT4:   // int4(numeric) -> int4
            return true;
        default:
            return false;
    }
	SPQ_WRAP_END;
}
#endif
bool
spqdb::FuncStrict(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return func_strict(funcid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsFuncNDVPreserving(Oid funcid)
{
	// Given a function oid, return whether it's one of a list of NDV-preserving
	// functions (estimated NDV of output is similar to that of the input)
	switch (funcid)
	{
		// for now, these are the functions we consider for this optimization
		case F_LOWER:
		case F_LTRIM1:
		case F_BTRIM1:
		case F_RTRIM1:
		case F_UPPER:
			return true;
		default:
			return false;
	}
}

char
spqdb::FuncStability(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return func_volatile(funcid);
	}
	SPQ_WRAP_END;
	return '\0';
}

char
spqdb::FuncDataAccess(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return func_data_access(funcid);
	}
	SPQ_WRAP_END;
	return '\0';
}
char
spqdb::FuncExecLocation(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return func_exec_location(funcid);
	}
	SPQ_WRAP_END;
	return '\0';
}

bool
spqdb::FunctionExists(Oid oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return function_exists(oid);
	}
	SPQ_WRAP_END;
	return false;
}
Oid
spqdb::GetAggIntermediateResultType(Oid aggid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_aggregate */
		return get_agg_transtype(aggid);
	}
	SPQ_WRAP_END;
	return 0;
}

int spqdb::GetAggregateArgTypes(Aggref *aggref, Oid *inputTypes)
{
    SPQ_WRAP_START;
    {
        return get_aggregate_argtypes(aggref, inputTypes, FUNC_MAX_ARGS);
    }
    SPQ_WRAP_END;
    return 0;
}
 
Oid spqdb::ResolveAggregateTransType(Oid aggfnoid, Oid aggtranstype, Oid *inputTypes, int numArguments)
{
    SPQ_WRAP_START;
    {
        return resolve_aggregate_transtype(aggfnoid, aggtranstype, inputTypes, numArguments);
    }
    SPQ_WRAP_END;
    return 0;
}

Query *
spqdb::FlattenJoinAliasVar(Query *query, spqos::ULONG query_level)
{
	SPQ_WRAP_START;
	{
		return flatten_join_alias_var_optimizer(query, query_level);
	}
	SPQ_WRAP_END;

	return NULL;
}

bool
spqdb::IsOrderedAgg(Oid aggid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_aggregate */
		return is_agg_ordered(aggid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsAggPartialCapable(Oid aggid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_aggregate */
		return is_agg_partial_capable(aggid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetAggregate(const char *agg, Oid type_oid, int nargs)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_aggregate */
		return get_aggregate(agg, type_oid);
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::GetArrayType(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		return get_array_type(typid);
	}
	SPQ_WRAP_END;
	return 0;
}
bool
spqdb::GetAttrStatsSlot(AttStatsSlot *sslot, HeapTuple statstuple, int reqkind,
					   Oid reqop, int flags)
{
	SPQ_WRAP_START;
	{
		return spq_get_attstatsslot(sslot, statstuple, reqkind, reqop, flags);
	}
	SPQ_WRAP_END;
	return false;
}

HeapTuple
spqdb::GetAttStats(Oid relid, AttrNumber attnum)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_statistic */
		return get_att_stats(relid, attnum);
	}
	SPQ_WRAP_END;
	return NULL;
}

Oid
spqdb::GetCommutatorOp(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		return get_commutator(opno);
	}
	SPQ_WRAP_END;
	return 0;
}

char *
spqdb::GetTriggerName(Oid triggerid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_trigger */
        //spq support trigger
        //return get_trigger_name(triggerid);
        return NULL;
	}
	SPQ_WRAP_END;
	return NULL;
}

Oid
spqdb::GetTriggerRelid(Oid triggerid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_trigger */
        //spq support trigger
		//return get_trigger_relid(triggerid);
		return 0;
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::GetTriggerFuncid(Oid triggerid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_trigger */
        //spq support trigger
		//return get_trigger_funcid(triggerid);
        return 0;
	}
	SPQ_WRAP_END;
	return 0;
}

int32
spqdb::GetTriggerType(Oid triggerid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_trigger */
		return get_trigger_type(triggerid);
	}
	SPQ_WRAP_END;
	return 0;
}

bool
spqdb::IsTriggerEnabled(Oid triggerid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_trigger */
		//return trigger_enabled(triggerid);
		return false;
	}
	SPQ_WRAP_END;
	return false;
}

char *
spqdb::GetCheckConstraintName(Oid check_constraint_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_constraint */
		return get_check_constraint_name(check_constraint_oid);
	}
	SPQ_WRAP_END;
	return NULL;
}
Oid
spqdb::GetCheckConstraintRelid(Oid check_constraint_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_constraint */
		return get_check_constraint_relid(check_constraint_oid);
	}
	SPQ_WRAP_END;
	return 0;
}


Node *
spqdb::PnodeCheckConstraint(Oid check_constraint_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_constraint */
		return get_check_constraint_expr_tree(check_constraint_oid);
	}
	SPQ_WRAP_END;
	return NULL;
}

List *
spqdb::GetCheckConstraintOids(Oid rel_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_constraint */
		return get_check_constraint_oids(rel_oid);
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::GetRelationPartContraints(Oid rel_oid, List **default_levels)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule, pg_constraint */
        //spq partition support
		//return get_relation_part_constraints(rel_oid, default_levels);
		return NULL;
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::GetLeafPartContraints(Oid rel_oid, List **default_levels)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule, pg_constraint */
        //spq partition support
		//return get_leaf_part_constraints(rel_oid, default_levels);
        return NULL;
	}
	SPQ_WRAP_END;
	return NULL;
}

bool
spqdb::HasExternalPartition(Oid oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule */
        //spq partition support
		//return rel_has_external_partition(oid);
        return false;
	}
	SPQ_WRAP_END;
	return false;
}

List *
spqdb::GetExternalPartitions(Oid oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule */
        //spq partition support
		//return rel_get_external_partitions(oid);
        return NIL;
	}
	SPQ_WRAP_END;
	return NIL;
}

bool
spqdb::IsLeafPartition(Oid oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule */
        //spq partition support
		//return rel_is_leaf_partition(oid);
        return false;
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetRootPartition(Oid oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule */
        //spq partition support
		//return rel_partition_get_master(oid);
        return InvalidOid;
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

bool
spqdb::GetCastFunc(Oid src_oid, Oid dest_oid, bool *is_binary_coercible,
				  Oid *cast_fn_oid, CoercionPathType *pathtype)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_cast */
		return get_cast_func(src_oid, dest_oid, is_binary_coercible,
							 cast_fn_oid, pathtype);
	}
	SPQ_WRAP_END;
	return false;
}

unsigned int
spqdb::GetComparisonType(Oid op_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */
		return get_comparison_type(op_oid);
	}
	SPQ_WRAP_END;
	return CmptOther;
}

Oid
spqdb::GetComparisonOperator(Oid left_oid, Oid right_oid, unsigned int cmpt)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */
		return get_comparison_operator(left_oid, right_oid, (SPQCmpType) cmpt);
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

Oid
spqdb::GetEqualityOp(Oid type_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		Oid eq_opr;

		get_sort_group_operators(type_oid, false, true, false, NULL, &eq_opr,
								 NULL, NULL);

		return eq_opr;
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

Oid
spqdb::GetEqualityOpForOrderingOp(Oid opno, bool *reverse)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */
		return get_equality_op_for_ordering_op(opno, reverse);
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

Oid
spqdb::GetOrderingOpForEqualityOp(Oid opno, bool *reverse)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */
		return get_ordering_op_for_equality_op(opno, reverse);
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

char *
spqdb::GetFuncName(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return get_func_name(funcid);
	}
	SPQ_WRAP_END;
	return NULL;
}

List *
spqdb::GetFuncOutputArgTypes(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return get_func_output_arg_types(funcid);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::GetFuncArgTypes(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return get_func_arg_types(funcid);
	}
	SPQ_WRAP_END;
	return NIL;
}

bool
spqdb::GetFuncRetset(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return get_func_retset(funcid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetFuncRetType(Oid funcid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return get_func_rettype(funcid);
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::GetInverseOp(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		return get_negator(opno);
	}
	SPQ_WRAP_END;
	return 0;
}

RegProcedure
spqdb::GetOpFunc(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		return get_opcode(opno);
	}
	SPQ_WRAP_END;
	return 0;
}

char *
spqdb::GetOpName(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		return get_opname(opno);
	}
	SPQ_WRAP_END;
	return NULL;
}

List *
spqdb::GetPartitionAttrs(Oid oid)
{
	SPQ_WRAP_START;
	{
		// return unique partition level attributes
		/* catalog tables: pg_partition */
        // spq partition support
		//return rel_partition_keys_ordered(oid);
		return NIL;
	}
	SPQ_WRAP_END;
	return NIL;
}

void
spqdb::GetOrderedPartKeysAndKinds(Oid oid, List **pkeys, List **pkinds)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition */
		//rel_partition_keys_kinds_ordered(oid, pkeys, pkinds);
	}
	SPQ_WRAP_END;
}
// spq partition support
#if 0
PartitionNode *
spqdb::GetParts(Oid relid, int16 level, Oid parent, bool inctemplate,
			   bool includesubparts)
{
	SPQ_WRAP_START;
	{
		// catalog tables: pg_partition, pg_partition_rule
		return get_parts(relid, level, parent, inctemplate, includesubparts);
	}
	SPQ_WRAP_END;
	return NULL;
}
#endif

List *
spqdb::GetRelationKeys(Oid relid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_constraint */
		//return get_relation_keys(relid);
	}
	SPQ_WRAP_END;
	return NIL;
}

Oid
spqdb::GetTypeRelid(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		return get_typ_typrelid(typid);
	}
	SPQ_WRAP_END;
	return 0;
}

char *
spqdb::GetTypeName(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		return get_type_name(typid);
	}
	SPQ_WRAP_END;
	return NULL;
}

int
spqdb::GetSPQSegmentCount(void)
{
	SPQ_WRAP_START;
	{
		return getSpqsegmentCount();
	}
	SPQ_WRAP_END;
	return 0;
}

bool
spqdb::HeapAttIsNull(HeapTuple tup, int attno)
{
	SPQ_WRAP_START;
	{
		return heap_attisnull(tup, attno, nullptr);
	}
	SPQ_WRAP_END;
	return false;
}

void
spqdb::FreeHeapTuple(HeapTuple htup)
{
	SPQ_WRAP_START;
	{
		heap_freetuple(htup);
		return;
	}
	SPQ_WRAP_END;
}

Oid
spqdb::GetDefaultDistributionOpclassForType(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type, pg_opclass */
		//return cdb_default_distribution_opclass_for_type(typid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetColumnDefOpclassForType(List *opclassName, Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type, pg_opclass */
		//return cdb_get_opclass_for_column_def(opclassName, typid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetDefaultDistributionOpfamilyForType(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type, pg_opclass */
		return spq_default_distribution_opfamily_for_type(typid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetHashProcInOpfamily(Oid opfamily, Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amproc, pg_type, pg_opclass */
		//return cdb_hashproc_in_opfamily(opfamily, typid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::IsLegacyCdbHashFunction(Oid funcid)
{
	SPQ_WRAP_START;
	{
		//return isLegacyCdbHashFunction(funcid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetLegacyCdbHashOpclassForBaseType(Oid typid)
{
	SPQ_WRAP_START;
	{
		//return get_legacy_cdbhash_opclass_for_base_type(typid);
	}
	SPQ_WRAP_END;
	return false;
}

Oid
spqdb::GetOpclassFamily(Oid opclass)
{
	SPQ_WRAP_START;
	{
		return get_opclass_family(opclass);
	}
	SPQ_WRAP_END;
	return false;
}

List *
spqdb::LAppend(List *list, void *datum)
{
	SPQ_WRAP_START;
	{
		return lappend(list, datum);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::LAppendInt(List *list, int iDatum)
{
	SPQ_WRAP_START;
	{
		return lappend_int(list, iDatum);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::LAppendOid(List *list, Oid datum)
{
	SPQ_WRAP_START;
	{
		return lappend_oid(list, datum);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::LPrepend(void *datum, List *list)
{
	SPQ_WRAP_START;
	{
		return lcons(datum, list);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::LPrependInt(int datum, List *list)
{
	SPQ_WRAP_START;
	{
		return lcons_int(datum, list);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::LPrependOid(Oid datum, List *list)
{
	SPQ_WRAP_START;
	{
		return lcons_oid(datum, list);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::ListConcat(List *list1, List *list2)
{
	SPQ_WRAP_START;
	{
		return list_concat(list1, list2);
	}
	SPQ_WRAP_END;
	return NIL;
}

List *
spqdb::ListCopy(List *list)
{
	SPQ_WRAP_START;
	{
		return list_copy(list);
	}
	SPQ_WRAP_END;
	return NIL;
}

ListCell *
spqdb::ListHead(List *l)
{
	SPQ_WRAP_START;
	{
		return list_head(l);
	}
	SPQ_WRAP_END;
	return NULL;
}

ListCell *
spqdb::ListTail(List *l)
{
	SPQ_WRAP_START;
	{
		return list_tail(l);
	}
	SPQ_WRAP_END;
	return NULL;
}

uint32
spqdb::ListLength(List *l)
{
	SPQ_WRAP_START;
	{
		return list_length(l);
	}
	SPQ_WRAP_END;
	return 0;
}

void *
spqdb::ListNth(List *list, int n)
{
	SPQ_WRAP_START;
	{
		return list_nth(list, n);
	}
	SPQ_WRAP_END;
	return NULL;
}

int
spqdb::ListNthInt(List *list, int n)
{
	SPQ_WRAP_START;
	{
		return list_nth_int(list, n);
	}
	SPQ_WRAP_END;
	return 0;
}

Oid
spqdb::ListNthOid(List *list, int n)
{
	SPQ_WRAP_START;
	{
		return list_nth_oid(list, n);
	}
	SPQ_WRAP_END;
	return 0;
}

bool
spqdb::ListMemberOid(List *list, Oid oid)
{
	SPQ_WRAP_START;
	{
		return list_member_oid(list, oid);
	}
	SPQ_WRAP_END;
	return false;
}

void
spqdb::ListFree(List *list)
{
	SPQ_WRAP_START;
	{
		list_free(list);
		return;
	}
	SPQ_WRAP_END;
}

void
spqdb::ListFreeDeep(List *list)
{
	SPQ_WRAP_START;
	{
		list_free_deep(list);
		return;
	}
	SPQ_WRAP_END;
}

bool
spqdb::IsMotionGather(const Motion *motion)
{
	SPQ_WRAP_START;
	{
		//return isMotionGather(motion);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsAppendOnlyPartitionTable(Oid root_oid)
{
	SPQ_WRAP_START;
	{
	    //spq partition support
		//return rel_has_appendonly_partition(root_oid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsMultilevelPartitionUniform(Oid root_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rule, pg_constraint */
        //spq partition support
		//return rel_partitioning_is_uniform(root_oid);
	}
	SPQ_WRAP_END;
	return false;
}

TypeCacheEntry *
spqdb::LookupTypeCache(Oid type_id, int flags)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type, pg_operator, pg_opclass, pg_opfamily, pg_amop */
		return lookup_type_cache(type_id, flags);
	}
	SPQ_WRAP_END;
	return NULL;
}

Value *
spqdb::MakeStringValue(char *str)
{
	SPQ_WRAP_START;
	{
		return makeString(str);
	}
	SPQ_WRAP_END;
	return NULL;
}

Value *
spqdb::MakeIntegerValue(long i)
{
	SPQ_WRAP_START;
	{
		return makeInteger(i);
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::MakeBoolConst(bool value, bool isnull)
{
	SPQ_WRAP_START;
	{
		return makeBoolConst(value, isnull);
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::MakeNULLConst(Oid type_oid)
{
	SPQ_WRAP_START;
	{
		return (Node *) makeNullConst(type_oid, -1 /*consttypmod*/, InvalidOid);
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::MakeSegmentFilterExpr(int segid)
{
	SPQ_WRAP_START;
	{
		//return (Node *) makeSegmentFilterExpr(segid);
	}
	SPQ_WRAP_END;
    return NULL;
}

TargetEntry *
spqdb::MakeTargetEntry(Expr *expr, AttrNumber resno, char *resname, bool resjunk)
{
	SPQ_WRAP_START;
	{
		return makeTargetEntry(expr, resno, resname, resjunk);
	}
	SPQ_WRAP_END;
	return NULL;
}

Var *
spqdb::MakeVar(Index varno, AttrNumber varattno, Oid vartype, int32 vartypmod,
			  Index varlevelsup)
{
	SPQ_WRAP_START;
	{
		// SPQDB_91_MERGE_FIXME: collation
		Oid collation = TypeCollation(vartype);
		return makeVar(varno, varattno, vartype, vartypmod, collation,
					   varlevelsup);
	}
	SPQ_WRAP_END;
	return NULL;
}

void *
spqdb::MemCtxtAllocZeroAligned(MemoryContext context, Size size)
{
	SPQ_WRAP_START;
	{
		return MemoryContextAllocZeroAligned(context, size);
	}
	SPQ_WRAP_END;
	return NULL;
}

void *
spqdb::MemCtxtAllocZero(MemoryContext context, Size size)
{
	SPQ_WRAP_START;
	{
		return MemoryContextAllocZero(context, size);
	}
	SPQ_WRAP_END;
	return NULL;
}

void *
spqdb::MemCtxtRealloc(void *pointer, Size size)
{
	SPQ_WRAP_START;
	{
		return repalloc(pointer, size);
	}
	SPQ_WRAP_END;
	return NULL;
}

char *
spqdb::MemCtxtStrdup(MemoryContext context, const char *string)
{
	SPQ_WRAP_START;
	{
		return MemoryContextStrdup(context, string);
	}
	SPQ_WRAP_END;
	return NULL;
}

// Helper function to throw an error with errcode, message and hint, like you
// would with ereport(...) in the backend. This could be extended for other
// fields, but this is all we need at the moment.
void
spqdb::GpdbEreportImpl(int xerrcode, int severitylevel, const char *xerrmsg,
					  const char *xerrhint, const char *filename, int lineno,
					  const char *funcname)
{
	SPQ_WRAP_START;
	{
		// We cannot use the ereport() macro here, because we want to pass on
		// the caller's filename and line number. This is essentially an
		// expanded version of ereport(). It will be caught by the
		// SPQ_WRAP_END, and propagated up as a C++ exception, to be
		// re-thrown as a Postgres error once we leave the C++ land.
		if (errstart(severitylevel, filename, lineno, funcname, TEXTDOMAIN))
			errfinish(errcode(xerrcode), errmsg("%s", xerrmsg),
					  xerrhint ? errhint("%s", xerrhint) : 0);
	}
	SPQ_WRAP_END;
}

char *
spqdb::NodeToString(void *obj)
{
	SPQ_WRAP_START;
	{
		return nodeToString(obj);
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::StringToNode(char *string)
{
	SPQ_WRAP_START;
	{
		return (Node *) stringToNode(string);
	}
	SPQ_WRAP_END;
	return NULL;
}


Node *
spqdb::GetTypeDefault(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		return get_typdefault(typid);
	}
	SPQ_WRAP_END;
	return NULL;
}


double
spqdb::NumericToDoubleNoOverflow(Numeric num)
{
	SPQ_WRAP_START;
	{
		return numeric_to_double_no_overflow(num);
	}
	SPQ_WRAP_END;
	return 0.0;
}

bool
spqdb::NumericIsNan(Numeric num)
{
	SPQ_WRAP_START;
	{
		return numeric_is_nan(num);
	}
	SPQ_WRAP_END;
	return false;
}

double
spqdb::ConvertTimeValueToScalar(Datum datum, Oid typid)
{
	SPQ_WRAP_START;
	{
		return convert_timevalue_to_scalar(datum, typid);
	}
	SPQ_WRAP_END;
	return 0.0;
}

double
spqdb::ConvertNetworkToScalar(Datum datum, Oid typid)
{
	SPQ_WRAP_START;
	{
		return convert_network_to_scalar(datum, typid);
	}
	SPQ_WRAP_END;
	return 0.0;
}

bool
spqdb::IsOpHashJoinable(Oid opno, Oid inputtype)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		return op_hashjoinable(opno, inputtype);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsOpMergeJoinable(Oid opno, Oid inputtype)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		return op_mergejoinable(opno, inputtype);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsOpStrict(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator, pg_proc */
		return op_strict(opno);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsOpNDVPreserving(Oid opno)
{
#if 0
	switch (opno)
	{
		// for now, we consider only the concatenation op as NDV-preserving
		// (note that we do additional checks later, e.g. col || 'const' is
		// NDV-preserving, while col1 || col2 is not)
		case OIDTextConcatenateOperator:
			return true;
		default:
			return false;
	}
#endif
    return false;
}

void
spqdb::GetOpInputTypes(Oid opno, Oid *lefttype, Oid *righttype)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_operator */
		op_input_types(opno, lefttype, righttype);
		return;
	}
	SPQ_WRAP_END;
}

void *
spqdb::SPQDBAlloc(Size size)
{
	SPQ_WRAP_START;
	{
		return palloc(size);
	}
	SPQ_WRAP_END;
	return NULL;
}

void
spqdb::SPQDBFree(void *ptr)
{
	SPQ_WRAP_START;
	{
		pfree(ptr);
		return;
	}
	SPQ_WRAP_END;
}

bool
spqdb::WalkQueryOrExpressionTree(Node *node, bool (*walker)(), void *context,
								int flags)
{
	SPQ_WRAP_START;
	{
		return query_or_expression_tree_walker(node, walker, context, flags);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::WalkQueryTree(Query *query, bool (*walker) (), void *context, int flags)
{
    SPQ_WRAP_START;
    {
        return query_tree_walker(query, walker, context, flags);
    }
    SPQ_WRAP_END;
    return false;
}

Node *
spqdb::MutateQueryOrExpressionTree(Node *node, Node *(*mutator)(), void *context,
								  int flags)
{
	SPQ_WRAP_START;
	{
		return query_or_expression_tree_mutator(node, (Node* (*)(Node*, void*))mutator, context, flags);
	}
	SPQ_WRAP_END;
	return NULL;
}

Query *
spqdb::MutateQueryTree(Query *query, Node *(*mutator)(), void *context,
					  int flags)
{
	SPQ_WRAP_START;
	{
		return query_tree_mutator(query, (Node* (*)(Node*, void*))mutator, context, flags);
	}
	SPQ_WRAP_END;
	return NULL;
}

bool
spqdb::RelPartIsRoot(Oid relid)
{
	SPQ_WRAP_START;
	{
	    //spq partition support
		//return PART_STATUS_ROOT == rel_part_status(relid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::RelPartIsInterior(Oid relid)
{
	SPQ_WRAP_START;
	{
	    //spq partition support
		//return PART_STATUS_INTERIOR == rel_part_status(relid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::RelPartIsNone(Oid relid)
{
	SPQ_WRAP_START;
	{
	    //spq partition support
		//return PART_STATUS_NONE == rel_part_status(relid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::HasSubclassSlow(Oid rel_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_inherits */
		return has_subclass_slow(rel_oid);
	}
	SPQ_WRAP_END;
	return false;
}


GpPolicy *
spqdb::GetDistributionPolicy(Relation rel)
{
	SPQ_WRAP_START;
	{
		return createRandomPartitionedPolicy(getSpqsegmentCount());
	}
	SPQ_WRAP_END;
	return NULL;
}

spqos::BOOL
spqdb::IsChildPartDistributionMismatched(Relation rel)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_class, pg_inherits */
		//return child_distribution_mismatch(rel);
	}
	SPQ_WRAP_END;
	return false;
}

spqos::BOOL
spqdb::ChildPartHasTriggers(Oid oid, int trigger_type)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_inherits, pg_trigger */
		// TODO SPQ
		// return child_triggers(oid, trigger_type);
	}
	SPQ_WRAP_END;
	return false;
}

void
spqdb::CdbEstimateRelationSize(RelOptInfo *relOptInfo, Relation rel,
							  int32 *attr_widths, BlockNumber *pages,
							  double *tuples, double *allvisfrac)
{
	SPQ_WRAP_START;
	{
		//cdb_estimate_rel_size(relOptInfo, rel, attr_widths, pages, tuples,
							 // allvisfrac);
		return;
	}
	SPQ_WRAP_END;
}

double
spqdb::SpqEstimatePartitionedNumTuples(Relation rel)
{
	SPQ_WRAP_START;
	{
		return spq_estimate_partitioned_numtuples(rel);
	}
	SPQ_WRAP_END;
}

void
spqdb::CloseRelation(Relation rel)
{
	SPQ_WRAP_START;
	{
		RelationClose(rel);
		return;
	}
	SPQ_WRAP_END;
}

List *
spqdb::GetRelationIndexes(Relation relation)
{
	SPQ_WRAP_START;
	{
		if (relation->rd_rel->relhasindex)
		{
			/* catalog tables: from relcache */
			return RelationGetIndexList(relation);
		}
	}
	SPQ_WRAP_END;
	return NIL;
}
#if 0
LogicalIndexes *
spqdb::GetLogicalPartIndexes(Oid oid)
{
	SPQ_WRAP_START;
	{
		// catalog tables: pg_partition, pg_partition_rule, pg_index
		return BuildLogicalIndexInfo(oid);
	}
	SPQ_WRAP_END;
	return NULL;
}

LogicalIndexInfo *
spqdb::GetLogicalIndexInfo(Oid root_oid, Oid index_oid)
{
	SPQ_WRAP_START;
	{
		// catalog tables: pg_index 
		return logicalIndexInfoForIndexOid(root_oid, index_oid);
	}
	SPQ_WRAP_END;
	return NULL;
}

LogicalIndexType
spqdb::GetLogicalIndexType(Oid index_oid)
{
	SPQ_WRAP_START;
	{
		// catalog tables: pg_index 
		return logicalIndexTypeForIndexOid(index_oid);
	}
	SPQ_WRAP_END;
	return INDTYPE_BTREE;
}
#endif

void
spqdb::BuildRelationTriggers(Relation rel)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_trigger */
		RelationBuildTriggers(rel);
		return;
	}
	SPQ_WRAP_END;
}

Relation
spqdb::GetRelation(Oid rel_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: relcache */
		return RelationIdGetRelation(rel_oid);
	}
	SPQ_WRAP_END;
	return NULL;
}
#if 0
ExtTableEntry *
spqdb::GetExternalTableEntry(Oid rel_oid)
{
	SPQ_WRAP_START;
	{
		// catalog tables: pg_exttable 
		return GetExtTableEntry(rel_oid);
	}
	SPQ_WRAP_END;
	return NULL;
}

List *
spqdb::GetExternalScanUriList(ExtTableEntry *ext, bool *ismasteronlyp)
{
	SPQ_WRAP_START;
	{
		return create_external_scan_uri_list(ext, ismasteronlyp);
	}
	SPQ_WRAP_END;
	return NULL;
}
#endif
TargetEntry *
spqdb::FindFirstMatchingMemberInTargetList(Node *node, List *targetlist)
{
	SPQ_WRAP_START;
	{
		return tlist_member(node, targetlist);
	}
	SPQ_WRAP_END;
	return NULL;
}

List *
spqdb::FindMatchingMembersInTargetList(Node *node, List *targetlist)
{
	SPQ_WRAP_START;
	{
		return tlist_members(node, targetlist);
	}
	SPQ_WRAP_END;

	return NIL;
}

bool
spqdb::Equals(void *p1, void *p2)
{
	SPQ_WRAP_START;
	{
		return equal(p1, p2);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsCompositeType(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		return type_is_rowtype(typid);
	}
	SPQ_WRAP_END;
	return false;
}

bool
spqdb::IsTextRelatedType(Oid typid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type */
		char typcategory;
		bool typispreferred;
		get_type_category_preferred(typid, &typcategory, &typispreferred);

		return typcategory == TYPCATEGORY_STRING;
	}
	SPQ_WRAP_END;
	return false;
}


int
spqdb::GetIntFromValue(Node *node)
{
	SPQ_WRAP_START;
	{
		return intVal(node);
	}
	SPQ_WRAP_END;
	return 0;
}

#if 0
Uri *
spqdb::ParseExternalTableUri(const char *uri)
{
	SPQ_WRAP_START;
	{
		return ParseExternalTableUri(uri);
	}
	SPQ_WRAP_END;
	return NULL;
}
// TODO spq need support
CdbComponentDatabases *
spqdb::GetComponentDatabases(void)
{
	SPQ_WRAP_START;
	{
		// catalog tables: spq_segment_config 
		return cdbcomponent_getCdbComponents();
	}
	SPQ_WRAP_END;
	return NULL;
}
#endif

int
spqdb::StrCmpIgnoreCase(const char *s1, const char *s2)
{
	SPQ_WRAP_START;
	{
		return pg_strcasecmp(s1, s2);
	}
	SPQ_WRAP_END;
	return 0;
}

bool *
spqdb::ConstructRandomSegMap(int total_primaries, int total_to_skip)
{
	SPQ_WRAP_START;
	{
		return makeRandomSegMap(total_primaries, total_to_skip);
	}
	SPQ_WRAP_END;
	return NULL;
}

StringInfo
spqdb::MakeStringInfo(void)
{
	SPQ_WRAP_START;
	{
		return makeStringInfo();
	}
	SPQ_WRAP_END;
	return NULL;
}

void
spqdb::AppendStringInfo(StringInfo str, const char *str1, const char *str2)
{
	SPQ_WRAP_START;
	{
		appendStringInfo(str, "%s%s", str1, str2);
		return;
	}
	SPQ_WRAP_END;
}

int
spqdb::FindNodes(Node *node, List *nodeTags)
{
	SPQ_WRAP_START;
	{
		return find_nodes(node, nodeTags);
	}
	SPQ_WRAP_END;
	return -1;
}

int
spqdb::CheckCollation(Node *node)
{
	SPQ_WRAP_START;
	{
		return check_collation(node);
	}
	SPQ_WRAP_END;
	return -1;
}

Node *
spqdb::CoerceToCommonType(ParseState *pstate, Node *node, Oid target_type,
						 const char *context)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_type, pg_cast */
		return coerce_to_common_type(pstate, node, target_type, context);
	}
	SPQ_WRAP_END;
	return NULL;
}

bool
spqdb::ResolvePolymorphicArgType(int numargs, Oid *argtypes, char *argmodes,
								FuncExpr *call_expr)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_proc */
		return resolve_polymorphic_argtypes(numargs, argtypes, argmodes,
											(Node *) call_expr);
	}
	SPQ_WRAP_END;
	return false;
}

// hash a list of const values with SPQDB's hash function
int32
spqdb::CdbHashConstList(List *constants, int num_segments, Oid *hashfuncs)
{
	SPQ_WRAP_START;
	{
		//return cdbhash_const_list(constants, num_segments, hashfuncs);
	}
	SPQ_WRAP_END;
	return 0;
}

unsigned int
spqdb::SpqHashRandomSeg(int num_segments)
{
	SPQ_WRAP_START;
	{
		//return cdbhashrandomseg(num_segments);
	}
	SPQ_WRAP_END;
	return 0;
}

// check permissions on range table
void
spqdb::CheckRTPermissions(List *rtable)
{
	SPQ_WRAP_START;
	{
		ExecCheckRTPerms(rtable, true);
		return;
	}
	SPQ_WRAP_END;
}

// check permissions on range table list
bool spqdb::CheckRTPermissionsWithRet(List *rtable)
{
    SPQ_WRAP_START;
    {
        return ExecCheckRTPerms(rtable, false);
    }
    SPQ_WRAP_END;
}

// get index op family properties
void
spqdb::IndexOpProperties(Oid opno, Oid opfamily, int *strategy, Oid *subtype)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */

		// Only the right type is returned to the caller, the left
		// type is simply ignored.
		Oid lefttype;

		get_op_opfamily_properties(opno, opfamily, false, strategy, &lefttype,
								   subtype);
		return;
	}
	SPQ_WRAP_END;
}

// get oids of opfamilies for the index keys
List *
spqdb::GetIndexOpFamilies(Oid index_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_index */

		// We return the operator families of the index keys.
		return get_index_opfamilies(index_oid);
	}
	SPQ_WRAP_END;

	return NIL;
}

// get oids of families this operator belongs to
List *
spqdb::GetOpFamiliesForScOp(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */

		// We return the operator families this operator
		// belongs to.
		return get_operator_opfamilies(opno);
	}
	SPQ_WRAP_END;

	return NIL;
}

// get the OID of hash equality operator(s) compatible with the given op
Oid
spqdb::GetCompatibleHashOpFamily(Oid opno)
{
	SPQ_WRAP_START;
	{
		return get_compatible_hash_opfamily(opno);
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

// get the OID of hash equality operator(s) compatible with the given op
Oid
spqdb::GetCompatibleLegacyHashOpFamily(Oid opno)
{
	SPQ_WRAP_START;
	{
		return get_compatible_legacy_hash_opfamily(opno);
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

List *
spqdb::GetMergeJoinOpFamilies(Oid opno)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_amop */

		return get_mergejoin_opfamilies(opno);
	}
	SPQ_WRAP_END;
	return NIL;
}


// get the OID of base elementtype for a given typid
// eg.: CREATE DOMAIN text_domain as text;
// SELECT oid, typbasetype from pg_type where typname = 'text_domain';
// oid         | XXXXX  --> Oid for text_domain
// typbasetype | 25     --> Oid for base element ie, TEXT
Oid
spqdb::GetBaseType(Oid typid)
{
	SPQ_WRAP_START;
	{
		return getBaseType(typid);
	}
	SPQ_WRAP_END;
	return InvalidOid;
}

// Evaluates 'expr' and returns the result as an Expr.
// Caller keeps ownership of 'expr' and takes ownership of the result
Expr *
spqdb::EvaluateExpr(Expr *expr, Oid result_type, int32 typmod)
{
	SPQ_WRAP_START;
	{
		// SPQDB_91_MERGE_FIXME: collation
		return evaluate_expr(expr, result_type, typmod, InvalidOid);
	}
	SPQ_WRAP_END;
	return NULL;
}

// interpret the value of "With oids" option from a list of defelems
bool
spqdb::InterpretOidsOption(List *options, bool allowOids)
{
	SPQ_WRAP_START;
	{
		return interpretOidsOption(options);
	}
	SPQ_WRAP_END;
	return false;
}

char *
spqdb::DefGetString(DefElem *defelem)
{
	SPQ_WRAP_START;
	{
		return defGetString(defelem);
	}
	SPQ_WRAP_END;
	return NULL;
}

Expr *
spqdb::TransformArrayConstToArrayExpr(Const *c)
{
	SPQ_WRAP_START;
	{
		return transform_array_Const_to_ArrayExpr(c);
	}
	SPQ_WRAP_END;
	return NULL;
}

Node *
spqdb::EvalConstExpressions(Node *node)
{
	SPQ_WRAP_START;
	{
		return eval_const_expressions(NULL, node);
	}
	SPQ_WRAP_END;
	return NULL;
}

#if 0
SelectedParts *
spqdb::RunStaticPartitionSelection(PartitionSelector *ps)
{
	SPQ_WRAP_START;
	{
		return static_part_selection(ps);
	}
	SPQ_WRAP_END;
	return NULL;
}

FaultInjectorType_e
spqdb::InjectFaultInOptTasks(const char *fault_name)
{
	SPQ_WRAP_START;
	{
		return FaultInjector_InjectFaultIfSet(fault_name, DDLNotSpecified, "",
											  "");
	}
	SPQ_WRAP_END;
	return FaultInjectorTypeNotSpecified;
}
#endif
spqos::ULONG
spqdb::CountLeafPartTables(Oid rel_oid)
{
	SPQ_WRAP_START;
	{
		/* catalog tables: pg_partition, pg_partition_rules */
		//return countLeafPartTables(rel_oid);
		return 0;
	}
	SPQ_WRAP_END;

	return 0;
}



/*
 * To detect changes to catalog tables that require resetting the Metadata
 * Cache, we use the normal PostgreSQL catalog cache invalidation mechanism.
 * We register a callback to a cache on all the catalog tables that contain
 * information that's contained in the ORCA metadata cache.

 * There is no fine-grained mechanism in the metadata cache for invalidating
 * individual entries ATM, so we just blow the whole cache whenever anything
 * changes. The callback simply increments a counter. Whenever we start
 * planning a query, we check the counter to see if it has changed since the
 * last planned query, and reset the whole cache if it has.
 *
 * To make sure we've covered all catalog tables that contain information
 * that's stored in the metadata cache, there are "catalog tables: xxx"
 * comments in all the calls to backend functions in this file. They indicate
 * which catalog tables each function uses. We conservatively assume that
 * anything fetched via the wrapper functions in this file can end up in the
 * metadata cache and hence need to have an invalidation callback registered.
 */
static bool mdcache_invalidation_counter_registered = false;
static int64 mdcache_invalidation_counter = 0;
static int64 last_mdcache_invalidation_counter = 0;

static void
mdsyscache_invalidation_counter_callback(Datum arg, int cacheid,
										 uint32 hashvalue)
{
	mdcache_invalidation_counter++;
}
static void
mdrelcache_invalidation_counter_callback(Datum arg, Oid relid)
{
	mdcache_invalidation_counter++;
}

static void
register_mdcache_invalidation_callbacks(void)
{
	/* These are all the catalog tables that we care about. */
	int metadata_caches[] = {
		AGGFNOID,		  /* pg_aggregate */
		AMOPOPID,		  /* pg_amop */
		CASTSOURCETARGET, /* pg_cast */
		CONSTROID,		  /* pg_constraint */
		OPEROID,		  /* pg_operator */
		OPFAMILYOID,	  /* pg_opfamily */
		//PARTOID,		  /* pg_partition */
		//PARTRULEOID,	  /* pg_partition_rule */
		STATRELATTINH,	  /* pg_statistics */
		TYPEOID,		  /* pg_type */
		PROCOID,		  /* pg_proc */

		/*
		 * lookup_type_cache() will also access pg_opclass, via GetDefaultOpClass(),
		 * but there is no syscache for it. Postgres doesn't seem to worry about
		 * invalidating the type cache on updates to pg_opclass, so we don't
		 * worry about that either.
		 */
		/* pg_opclass */

		/*
		 * Information from the following catalogs are included in the
		 * relcache, and any updates will generate relcache invalidation
		 * event. We'll catch the relcache invalidation event and don't need
		 * to register a catcache callback for them.
		 */
		/* pg_class */
		/* pg_index */
		/* pg_trigger */

		/*
		 * pg_exttable is only updated when a new external table is dropped/created,
		 * which will trigger a relcache invalidation event.
		 */
		/* pg_exttable */

		/*
		 * XXX: no syscache on pg_inherits. Is that OK? For any partitioning
		 * changes, I think there will also be updates on pg_partition and/or
		 * pg_partition_rules.
		 */
		/* pg_inherits */

		/*
		 * We assume that spq_segment_config will not change on the fly in a way that
		 * would affect ORCA
		 */
		/* spq_segment_config */
	};
	unsigned int i;

	for (i = 0; i < lengthof(metadata_caches); i++)
	{
		CacheRegisterSessionSyscacheCallback(metadata_caches[i],
				&mdsyscache_invalidation_counter_callback, (Datum) 0);
	}

	/* also register the relcache callback */
	CacheRegisterSessionRelcacheCallback(&mdrelcache_invalidation_counter_callback, (Datum) 0);
}

// Has there been any catalog changes since last call?
bool
spqdb::MDCacheNeedsReset(void)
{
	SPQ_WRAP_START;
	{
		if (!mdcache_invalidation_counter_registered)
		{
			register_mdcache_invalidation_callbacks();
			mdcache_invalidation_counter_registered = true;
		}
		if (last_mdcache_invalidation_counter == mdcache_invalidation_counter)
			return false;
		else
		{
			last_mdcache_invalidation_counter = mdcache_invalidation_counter;
			return true;
		}
	}
	SPQ_WRAP_END;

	return true;
}

// returns true if a query cancel is requested in SPQDB
bool
spqdb::IsAbortRequested(void)
{
	// No SPQ_WRAP_START/END needed here. We just check these global flags,
	// it cannot throw an ereport().
	//return (QueryCancelPending || ProcDiePending);
	return false;
}

GpPolicy *
spqdb::MakeGpPolicy(GpPolicyType ptype, int nattrs, int numsegments)
{
	SPQ_WRAP_START;
	{
		/*
		 * FIXME_TABLE_EXPAND: it used by ORCA, help...
		 */
		return makeSpqPolicy(ptype, nattrs, numsegments);
	}
	SPQ_WRAP_END;
}

uint32
spqdb::HashChar(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt32(DirectFunctionCall1(hashchar, d));
	}
	SPQ_WRAP_END;
}

uint32
spqdb::HashBpChar(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt32(DirectFunctionCall1(hashbpchar, d));
	}
	SPQ_WRAP_END;
}

uint32
spqdb::HashText(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt32(DirectFunctionCall1(hashtext, d));
	}
	SPQ_WRAP_END;
}

uint32
spqdb::HashName(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt32(DirectFunctionCall1(hashname, d));
	}
	SPQ_WRAP_END;
}

uint32
spqdb::UUIDHash(Datum d)
{
	SPQ_WRAP_START;
	{
		return DatumGetUInt32(DirectFunctionCall1(uuid_hash, d));
	}
	SPQ_WRAP_END;
}

void *
spqdb::SPQDBMemoryContextAlloc(MemoryContext context, Size size)
{
	SPQ_WRAP_START;
	{
		return MemoryContextAlloc(context, size);
	}
	SPQ_WRAP_END;
	return NULL;
}

void
spqdb::SPQDBMemoryContextDelete(MemoryContext context)
{
	SPQ_WRAP_START;
	{
		MemoryContextDelete(context);
	}
	SPQ_WRAP_END;
}

MemoryContext
spqdb::SPQDBAllocSetContextCreate()
{
	SPQ_WRAP_START;
	{
		return AllocSetContextCreate(
			OptimizerMemoryContext, "SPQORCA memory pool",
			ALLOCSET_DEFAULT_MINSIZE, ALLOCSET_DEFAULT_INITSIZE,
			ALLOCSET_DEFAULT_MAXSIZE);
	}
	SPQ_WRAP_END;
	return NULL;
}

// EOF
