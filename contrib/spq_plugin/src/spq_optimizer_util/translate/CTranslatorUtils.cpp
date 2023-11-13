//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CTranslatorUtils.cpp
//
//	@doc:
//		Implementation of the utility methods for translating SPQDB's
//		Query / PlannedStmt into DXL Tree
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "postgres.h"

#include "access/sysattr.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_statistic.h"
#include "catalog/pg_trigger.h"
#include "catalog/pg_type.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "optimizer/planmem_walker.h"
#include "utils/guc.h"
#include "utils/rel.h"

#define SPQDB_NEXTVAL 1574
#define SPQDB_CURRVAL 1575
#define SPQDB_SETVAL 1576

#include "spqos/base.h"
#include "spqos/common/CAutoTimer.h"
#include "spqos/common/CBitSetIter.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CUtils.h"
#include "spq_optimizer_util/spq_wrappers.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spq_optimizer_util/translate/CDXLTranslateContext.h"
#include "spq_optimizer_util/translate/CTranslatorRelcacheToDXL.h"
#include "spq_optimizer_util/translate/CTranslatorScalarToDXL.h"
#include "spq_optimizer_util/translate/CTranslatorUtils.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/dxl/operators/CDXLColDescr.h"
#include "naucrates/dxl/operators/CDXLDatumBool.h"
#include "naucrates/dxl/operators/CDXLDatumInt2.h"
#include "naucrates/dxl/operators/CDXLDatumInt4.h"
#include "naucrates/dxl/operators/CDXLDatumInt8.h"
#include "naucrates/dxl/operators/CDXLDatumOid.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLSpoolInfo.h"
#include "naucrates/dxl/xml/dxltokens.h"
#include "naucrates/md/CMDIdColStats.h"
#include "naucrates/md/CMDIdRelStats.h"
#include "naucrates/md/CMDTypeGenericSPQDB.h"
#include "naucrates/md/IMDAggregate.h"
#include "naucrates/md/IMDIndex.h"
#include "naucrates/md/IMDRelation.h"
#include "naucrates/md/IMDTrigger.h"
#include "naucrates/md/IMDTypeBool.h"
#include "naucrates/md/IMDTypeInt2.h"
#include "naucrates/md/IMDTypeInt4.h"
#include "naucrates/md/IMDTypeInt8.h"
#include "naucrates/md/IMDTypeOid.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqos;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetIndexDescr
//
//	@doc:
//		Create a DXL index descriptor from an index MD id
//
//---------------------------------------------------------------------------
CDXLIndexDescr *
CTranslatorUtils::GetIndexDescr(CMemoryPool *mp, CMDAccessor *md_accessor,
								IMDId *mdid)
{
	const IMDIndex *index = md_accessor->RetrieveIndex(mdid);
	const CWStringConst *index_name = index->Mdname().GetMDName();
	CMDName *index_mdname = SPQOS_NEW(mp) CMDName(mp, index_name);

	return SPQOS_NEW(mp) CDXLIndexDescr(mp, mdid, index_mdname);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetTableDescr
//
//	@doc:
//		Create a DXL table descriptor from a SPQDB range table entry
//
//---------------------------------------------------------------------------
CDXLTableDescr *
CTranslatorUtils::GetTableDescr(CMemoryPool *mp, CMDAccessor *md_accessor,
								CIdGenerator *id_generator,
								const RangeTblEntry *rte,
								BOOL *is_distributed_table	// output
)
{
	// generate an MDId for the table desc.
	OID rel_oid = rte->relid;

	if (!SPQOS_FTRACE(EopttraceEnableExternalPartitionedTables) &&
		spqdb::HasExternalPartition(rel_oid))
	{
		// fall back to the planner for queries with partition tables that has an external table in one of its leaf
		// partitions.
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
				   SPQOS_WSZ_LIT("Query over external partitions"));
	}

	CMDIdSPQDB *mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, rel_oid);

	const IMDRelation *rel = md_accessor->RetrieveRel(mdid);

	// look up table name
	const CWStringConst *tablename = rel->Mdname().GetMDName();
	CMDName *table_mdname = SPQOS_NEW(mp) CMDName(mp, tablename);

	CDXLTableDescr *table_descr =
		SPQOS_NEW(mp) CDXLTableDescr(mp, mdid, table_mdname, rte->checkAsUser);

	const ULONG len = rel->ColumnCount();

	IMDRelation::Ereldistrpolicy distribution_policy =
		rel->GetRelDistribution();

	if (NULL != is_distributed_table &&
		(IMDRelation::EreldistrHash == distribution_policy ||
		 IMDRelation::EreldistrRandom == distribution_policy ||
		 IMDRelation::EreldistrReplicated == distribution_policy))
	{
		*is_distributed_table = true;
	}
	else if (!u_sess->attr.attr_spq.spq_optimizer_enable_master_only_queries &&
			 (IMDRelation::EreldistrMasterOnly == distribution_policy))
	{
		// fall back to the planner for queries on master-only table if they are disabled with Orca. This is due to
		// the fact that catalog tables (master-only) are not analyzed often and will result in Orca producing
		// inferior plans.

		SPQOS_THROW_EXCEPTION(
			spqdxl::ExmaDXL,							 // major
			spqdxl::ExmiQuery2DXLUnsupportedFeature,	 // minor
			CException::
				ExsevDebug1,  // ulSeverityLevel mapped to SPQDB severity level
			SPQOS_WSZ_LIT("Queries on master-only tables"));
	}

	// add columns from md cache relation object to table descriptor
	for (ULONG ul = 0; ul < len; ul++)
	{
		const IMDColumn *md_col = rel->GetMdCol(ul);
		if (md_col->IsDropped())
		{
			continue;
		}

		CMDName *col = SPQOS_NEW(mp) CMDName(mp, md_col->Mdname().GetMDName());
		CMDIdSPQDB *col_type = CMDIdSPQDB::CastMdid(md_col->MdidType());
		col_type->AddRef();

		// create a column descriptor for the column
		CDXLColDescr *dxl_col_descr = SPQOS_NEW(mp)
			CDXLColDescr(mp, col, id_generator->next_id(), md_col->AttrNum(),
						 col_type, md_col->TypeModifier(), /* type_modifier */
						 false,							   /* fColDropped */
						 md_col->Length());
		table_descr->AddColumnDescr(dxl_col_descr);
	}

	return table_descr;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsSirvFunc
//
//	@doc:
//		Check if the given function is a SIRV (single row volatile) that reads
//		or modifies SQL data
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsSirvFunc(CMemoryPool *mp, CMDAccessor *md_accessor,
							 OID func_oid)
{
	// we exempt the following 3 functions to avoid falling back to the planner
	// for DML on tables with sequences. The same exemption is also in the planner
	if (SPQDB_NEXTVAL == func_oid || SPQDB_CURRVAL == func_oid ||
		SPQDB_SETVAL == func_oid)
	{
		return false;
	}

	CMDIdSPQDB *mdid_func =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, func_oid);
	const IMDFunction *func = md_accessor->RetrieveFunc(mdid_func);

	BOOL is_sirv = (!func->ReturnsSet() &&
					IMDFunction::EfsVolatile == func->GetFuncStability());

	mdid_func->Release();

	return is_sirv;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::HasSubquery
//
//	@doc:
//		Check if the given tree contains a subquery
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::HasSubquery(Node *node)
{
	List *unsupported_list = ListMake1Int(T_SubLink);
	INT unsupported = spqdb::FindNodes(node, unsupported_list);
	spqdb::SPQDBFree(unsupported_list);

	return (0 <= unsupported);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::ConvertToCDXLLogicalTVF
//
//	@doc:
//		Create a DXL logical TVF from a SPQDB range table entry
//
//---------------------------------------------------------------------------
CDXLLogicalTVF *
CTranslatorUtils::ConvertToCDXLLogicalTVF(CMemoryPool *mp,
										  CMDAccessor *md_accessor,
										  CIdGenerator *id_generator,
										  const RangeTblEntry *rte)
{
	// TODO SPQ
	/*
	 * SPQDB_94_MERGE_FIXME: RangeTblEntry for functions can now contain multiple function calls.
	 * ORCA isn't prepared for that yet. See upstream commit 784e762e88.
	 */
	/*if (list_length(rte->functions) != 1)
	{
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
				   SPQOS_WSZ_LIT("Multi-argument UNNEST() or TABLE()"));
	}*/
	/*
	 * SPQDB_94_MERGE_FIXME: Does WITH ORDINALITY work? It was new in 9.4. Add a check here,
	 * if it doesn't, or remove this comment if it does.
	 */

	//RangeTblFunction *rtfunc = (RangeTblFunction *) linitial(rte->functions);
	RangeTblFunction *rtfunc = nullptr;

	// TVF evaluates to const, return const DXL node
	if (IsA(rtfunc->funcexpr, Const))
	{
		Const *constExpr = (Const *) rtfunc->funcexpr;

		CMDIdSPQDB *mdid_return_type =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, constExpr->consttype);

		const IMDType *type = md_accessor->RetrieveType(mdid_return_type);
		CDXLColDescrArray *column_descrs = GetColumnDescriptorsFromComposite(
			mp, md_accessor, id_generator, type);

		CMDName *func_name =
			CDXLUtils::CreateMDNameFromCharArray(mp, rte->eref->aliasname);

		// if TVF evaluates to const, pass invalid key as funcid
		CDXLLogicalTVF *tvf_dxl = SPQOS_NEW(mp)
			CDXLLogicalTVF(mp, SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, 0),
						   mdid_return_type, func_name, column_descrs);

		return tvf_dxl;
	}

	FuncExpr *funcexpr = (FuncExpr *) rtfunc->funcexpr;
	// In the planner, scalar functions that are volatile (SIRV) or read or modify SQL
	// data get patched into an InitPlan. This is not supported in the optimizer
	if (IsSirvFunc(mp, md_accessor, funcexpr->funcid))
	{
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
				   SPQOS_WSZ_LIT("SIRV functions"));
	}
	// get function id
	CMDIdSPQDB *mdid_func =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, funcexpr->funcid);
	CMDIdSPQDB *mdid_return_type =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, funcexpr->funcresulttype);
	const IMDType *type = md_accessor->RetrieveType(mdid_return_type);

	// get function from MDcache
	const IMDFunction *func = md_accessor->RetrieveFunc(mdid_func);

	IMdIdArray *out_arg_types = func->OutputArgTypesMdidArray();

	CDXLColDescrArray *column_descrs = NULL;

	if (NULL != rtfunc->funccoltypes)
	{
		// function returns record - use col names and types from query
		column_descrs = GetColumnDescriptorsFromRecord(
			mp, id_generator, rte->eref->colnames, rtfunc->funccoltypes,
			rtfunc->funccoltypmods);
	}
	else if (type->IsComposite() && IMDId::IsValid(type->GetBaseRelMdid()))
	{
		// function returns a "table" type or a user defined type
		column_descrs = GetColumnDescriptorsFromComposite(mp, md_accessor,
														  id_generator, type);
	}
	else if (NULL != out_arg_types)
	{
		// function returns record - but output col types are defined in catalog
		out_arg_types->AddRef();
		if (ContainsPolymorphicTypes(out_arg_types))
		{
			// resolve polymorphic types (anyelement/anyarray) using the
			// argument types from the query
			List *arg_types = spqdb::GetFuncArgTypes(funcexpr->funcid);
			IMdIdArray *resolved_types =
				ResolvePolymorphicTypes(mp, out_arg_types, arg_types, funcexpr);
			out_arg_types->Release();
			out_arg_types = resolved_types;
		}

		column_descrs = GetColumnDescriptorsFromRecord(
			mp, id_generator, rte->eref->colnames, out_arg_types);
		out_arg_types->Release();
	}
	else
	{
		// function returns base type
		CMDName func_mdname = func->Mdname();
		// table valued functions don't describe the returned column type modifiers, hence the -1
		column_descrs =
			GetColumnDescriptorsFromBase(mp, id_generator, mdid_return_type,
										 default_type_modifier, &func_mdname);
	}

	CMDName *pmdfuncname = SPQOS_NEW(mp) CMDName(mp, func->Mdname().GetMDName());

	CDXLLogicalTVF *tvf_dxl = SPQOS_NEW(mp) CDXLLogicalTVF(
		mp, mdid_func, mdid_return_type, pmdfuncname, column_descrs);

	return tvf_dxl;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::ResolvePolymorphicTypes
//
//	@doc:
//		Resolve polymorphic types in the given array of type ids, replacing
//		them with the actual types obtained from the query
//
//---------------------------------------------------------------------------
IMdIdArray *
CTranslatorUtils::ResolvePolymorphicTypes(CMemoryPool *mp,
										  IMdIdArray *return_arg_mdids,
										  List *input_arg_types,
										  FuncExpr *funcexpr)
{
	ULONG arg_index = 0;

	const ULONG num_arg_types = spqdb::ListLength(input_arg_types);
	const ULONG num_args_from_query = spqdb::ListLength(funcexpr->args);
	const ULONG num_return_args = return_arg_mdids->Size();
	const ULONG num_args = std::min(num_arg_types, num_args_from_query);
	const ULONG total_args = num_args + num_return_args;

	OID arg_types[total_args];
	char arg_modes[total_args];

	// copy the first 'num_args' function argument types
	ListCell *arg_type = NULL;
	ForEach(arg_type, input_arg_types)
	{
		if (arg_index >= num_args)
		{
			break;
		}
		arg_types[arg_index] = lfirst_oid(arg_type);
		arg_modes[arg_index++] = PROARGMODE_IN;
	}

	// copy function return types
	for (ULONG ul = 0; ul < num_return_args; ul++)
	{
		IMDId *mdid = (*return_arg_mdids)[ul];
		arg_types[arg_index] = CMDIdSPQDB::CastMdid(mdid)->Oid();
		arg_modes[arg_index++] = PROARGMODE_TABLE;
	}

	if (!spqdb::ResolvePolymorphicArgType(total_args, arg_types, arg_modes,
										 funcexpr))
	{
		SPQOS_RAISE(
			spqdxl::ExmaDXL, spqdxl::ExmiDXLUnrecognizedType,
			SPQOS_WSZ_LIT(
				"could not determine actual argument/return type for polymorphic function"));
	}

	// generate a new array of mdids based on the resolved return types
	IMdIdArray *resolved_types = SPQOS_NEW(mp) IMdIdArray(mp);

	// get the resolved return types
	for (ULONG ul = num_args; ul < total_args; ul++)
	{
		IMDId *resolved_mdid = NULL;
		resolved_mdid =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, arg_types[ul]);
		resolved_types->Append(resolved_mdid);
	}

	return resolved_types;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::ContainsPolymorphicTypes
//
//	@doc:
//		Check if the given mdid array contains any of the polymorphic
//		types (ANYELEMENT, ANYARRAY, ANYENUM, ANYNONARRAY)
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::ContainsPolymorphicTypes(IMdIdArray *mdid_array)
{
	SPQOS_ASSERT(NULL != mdid_array);
	const ULONG len = mdid_array->Size();
	for (ULONG ul = 0; ul < len; ul++)
	{
		IMDId *mdid_type = (*mdid_array)[ul];
		if (IsPolymorphicType(CMDIdSPQDB::CastMdid(mdid_type)->Oid()))
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnDescriptorsFromRecord
//
//	@doc:
//		Get column descriptors from a record type
//
//---------------------------------------------------------------------------
CDXLColDescrArray *
CTranslatorUtils::GetColumnDescriptorsFromRecord(CMemoryPool *mp,
												 CIdGenerator *id_generator,
												 List *col_names,
												 List *col_types,
												 List *col_type_modifiers)
{
	ListCell *col_name = NULL;
	ListCell *col_type = NULL;
	ListCell *col_type_modifier = NULL;

	ULONG ul = 0;
	CDXLColDescrArray *column_descrs = SPQOS_NEW(mp) CDXLColDescrArray(mp);

	ForThree(col_name, col_names, col_type, col_types, col_type_modifier,
			 col_type_modifiers)
	{
		Value *value = (Value *) lfirst(col_name);
		Oid coltype = lfirst_oid(col_type);
		INT type_modifier = lfirst_int(col_type_modifier);

		CHAR *col_name_char_array = strVal(value);
		CWStringDynamic *column_name =
			CDXLUtils::CreateDynamicStringFromCharArray(mp,
														col_name_char_array);
		CMDName *col_mdname = SPQOS_NEW(mp) CMDName(mp, column_name);
		SPQOS_DELETE(column_name);

		IMDId *col_type = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, coltype);

		CDXLColDescr *dxl_col_descr = SPQOS_NEW(mp) CDXLColDescr(
			mp, col_mdname, id_generator->next_id(), INT(ul + 1) /* attno */,
			col_type, type_modifier, false /* fColDropped */
		);
		column_descrs->Append(dxl_col_descr);
		ul++;
	}

	return column_descrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnDescriptorsFromRecord
//
//	@doc:
//		Get column descriptors from a record type
//
//---------------------------------------------------------------------------
CDXLColDescrArray *
CTranslatorUtils::GetColumnDescriptorsFromRecord(CMemoryPool *mp,
												 CIdGenerator *id_generator,
												 List *col_names,
												 IMdIdArray *out_arg_types)
{
	SPQOS_ASSERT(out_arg_types->Size() == (ULONG) spqdb::ListLength(col_names));
	ListCell *col_name = NULL;

	ULONG ul = 0;
	CDXLColDescrArray *column_descrs = SPQOS_NEW(mp) CDXLColDescrArray(mp);

	ForEach(col_name, col_names)
	{
		Value *value = (Value *) lfirst(col_name);

		CHAR *col_name_char_array = strVal(value);
		CWStringDynamic *column_name =
			CDXLUtils::CreateDynamicStringFromCharArray(mp,
														col_name_char_array);
		CMDName *col_mdname = SPQOS_NEW(mp) CMDName(mp, column_name);
		SPQOS_DELETE(column_name);

		IMDId *col_type = (*out_arg_types)[ul];
		col_type->AddRef();

		// This function is only called to construct column descriptors for table-valued functions
		// which won't have type modifiers for columns of the returned table
		CDXLColDescr *dxl_col_descr = SPQOS_NEW(mp) CDXLColDescr(
			mp, col_mdname, id_generator->next_id(), INT(ul + 1) /* attno */,
			col_type, default_type_modifier, false /* fColDropped */
		);
		column_descrs->Append(dxl_col_descr);
		ul++;
	}

	return column_descrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnDescriptorsFromBase
//
//	@doc:
//		Get column descriptor from a base type
//
//---------------------------------------------------------------------------
CDXLColDescrArray *
CTranslatorUtils::GetColumnDescriptorsFromBase(CMemoryPool *mp,
											   CIdGenerator *id_generator,
											   IMDId *mdid_return_type,
											   INT type_modifier,
											   CMDName *pmdName)
{
	CDXLColDescrArray *column_descrs = SPQOS_NEW(mp) CDXLColDescrArray(mp);

	mdid_return_type->AddRef();
	CMDName *col_mdname = SPQOS_NEW(mp) CMDName(mp, pmdName->GetMDName());

	CDXLColDescr *dxl_col_descr = SPQOS_NEW(mp) CDXLColDescr(
		mp, col_mdname, id_generator->next_id(), INT(1) /* attno */,
		mdid_return_type, type_modifier, /* type_modifier */
		false							 /* fColDropped */
	);

	column_descrs->Append(dxl_col_descr);

	return column_descrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnDescriptorsFromComposite
//
//	@doc:
//		Get column descriptors from a composite type
//
//---------------------------------------------------------------------------
CDXLColDescrArray *
CTranslatorUtils::GetColumnDescriptorsFromComposite(CMemoryPool *mp,
													CMDAccessor *md_accessor,
													CIdGenerator *id_generator,
													const IMDType *type)
{
	CMDColumnArray *col_ptr_arr = ExpandCompositeType(mp, md_accessor, type);

	CDXLColDescrArray *column_descrs = SPQOS_NEW(mp) CDXLColDescrArray(mp);

	for (ULONG ul = 0; ul < col_ptr_arr->Size(); ul++)
	{
		IMDColumn *md_col = (*col_ptr_arr)[ul];

		CMDName *col_mdname =
			SPQOS_NEW(mp) CMDName(mp, md_col->Mdname().GetMDName());
		IMDId *col_type = md_col->MdidType();

		col_type->AddRef();
		CDXLColDescr *dxl_col_descr = SPQOS_NEW(mp) CDXLColDescr(
			mp, col_mdname, id_generator->next_id(), INT(ul + 1) /* attno */,
			col_type, md_col->TypeModifier(), /* type_modifier */
			false							  /* fColDropped */
		);
		column_descrs->Append(dxl_col_descr);
	}

	col_ptr_arr->Release();

	return column_descrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::ExpandCompositeType
//
//	@doc:
//		Expand a composite type into an array of IMDColumns
//
//---------------------------------------------------------------------------
CMDColumnArray *
CTranslatorUtils::ExpandCompositeType(CMemoryPool *mp, CMDAccessor *md_accessor,
									  const IMDType *type)
{
	SPQOS_ASSERT(NULL != type);
	SPQOS_ASSERT(type->IsComposite());

	IMDId *rel_mdid = type->GetBaseRelMdid();
	const IMDRelation *rel = md_accessor->RetrieveRel(rel_mdid);
	SPQOS_ASSERT(NULL != rel);

	CMDColumnArray *pdrgPmdcol = SPQOS_NEW(mp) CMDColumnArray(mp);

	for (ULONG ul = 0; ul < rel->ColumnCount(); ul++)
	{
		CMDColumn *md_col = (CMDColumn *) rel->GetMdCol(ul);

		if (!md_col->IsSystemColumn())
		{
			md_col->AddRef();
			pdrgPmdcol->Append(md_col);
		}
	}

	return pdrgPmdcol;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::ConvertToDXLJoinType
//
//	@doc:
//		Translates the join type from its SPQDB representation into the DXL one
//
//---------------------------------------------------------------------------
EdxlJoinType
CTranslatorUtils::ConvertToDXLJoinType(JoinType jt)
{
	EdxlJoinType join_type = EdxljtSentinel;

	switch (jt)
	{
		case JOIN_INNER:
			join_type = EdxljtInner;
			break;

		case JOIN_LEFT:
			join_type = EdxljtLeft;
			break;

		case JOIN_FULL:
			join_type = EdxljtFull;
			break;

		case JOIN_RIGHT:
			join_type = EdxljtRight;
			break;

		case JOIN_SEMI:
			join_type = EdxljtIn;
			break;

		case JOIN_ANTI:
			join_type = EdxljtLeftAntiSemijoin;
			break;

		case JOIN_LASJ_NOTIN:
			join_type = EdxljtLeftAntiSemijoinNotIn;
			break;

		default:
			SPQOS_ASSERT(!"Unrecognized join type");
	}

	SPQOS_ASSERT(EdxljtSentinel > join_type);

	return join_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::ConvertToDXLIndexScanDirection
//
//	@doc:
//		Translates the DXL index scan direction from SPQDB representation
//
//---------------------------------------------------------------------------
EdxlIndexScanDirection
CTranslatorUtils::ConvertToDXLIndexScanDirection(ScanDirection sd)
{
	EdxlIndexScanDirection idx_scan_direction = EdxlisdSentinel;

	switch (sd)
	{
		case BackwardScanDirection:
			idx_scan_direction = EdxlisdBackward;
			break;

		case ForwardScanDirection:
			idx_scan_direction = EdxlisdForward;
			break;

		case NoMovementScanDirection:
			idx_scan_direction = EdxlisdNoMovement;
			break;

		default:
			SPQOS_ASSERT(!"Unrecognized index scan direction");
	}

	SPQOS_ASSERT(EdxlisdSentinel > idx_scan_direction);

	return idx_scan_direction;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnDescrAt
//
//	@doc:
//		Find the n-th col descr entry
//
//---------------------------------------------------------------------------
const CDXLColDescr *
CTranslatorUtils::GetColumnDescrAt(const CDXLTableDescr *table_descr, ULONG pos)
{
	SPQOS_ASSERT(0 != pos);
	SPQOS_ASSERT(pos < table_descr->Arity());

	return table_descr->GetColumnDescrAt(pos);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetSystemColName
//
//	@doc:
//		Return the name for the system attribute with the given attribute number.
//
//---------------------------------------------------------------------------
const CWStringConst *
CTranslatorUtils::GetSystemColName(AttrNumber attno)
{
	SPQOS_ASSERT(FirstLowInvalidHeapAttributeNumber < attno && 0 > attno);

	switch (attno)
	{
		case SelfItemPointerAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenCtidColName);

		case ObjectIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenOidColName);

		case MinTransactionIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenXminColName);

		case MinCommandIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenCminColName);

		case MaxTransactionIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenXmaxColName);

		case MaxCommandIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenCmaxColName);

		case TableOidAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenTableOidColName);

		case XC_NodeIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenXCNidAttribute);

		case BucketIdAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenBidAttribute);

		case UidAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenUidAttribute);
		case RootSelfItemPointerAttributeNumber:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRootCtidColName);

		default:
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiPlStmt2DXLConversion,
					   SPQOS_WSZ_LIT("Invalid attribute number"));
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetSystemColType
//
//	@doc:
//		Return the type id for the system attribute with the given attribute number.
//
//---------------------------------------------------------------------------
CMDIdSPQDB *
CTranslatorUtils::GetSystemColType(CMemoryPool *mp, AttrNumber attno)
{
	SPQOS_ASSERT(FirstLowInvalidHeapAttributeNumber < attno && 0 > attno);

	switch (attno)
	{
		case SelfItemPointerAttributeNumber:
			// tid type
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_TID);

		case ObjectIdAttributeNumber:
		case TableOidAttributeNumber:
			// OID type
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID);

		case MinTransactionIdAttributeNumber:
		case MaxTransactionIdAttributeNumber:
			// xid type
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_XID);

		case MinCommandIdAttributeNumber:
		case MaxCommandIdAttributeNumber:
			// cid type
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_CID);
		case RootSelfItemPointerAttributeNumber:
			// tid type
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_TID);
		case XC_NodeIdAttributeNumber:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_NID);
		case BucketIdAttributeNumber:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BID);
		case UidAttributeNumber:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_UID);

		default:
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiPlStmt2DXLConversion,
					   SPQOS_WSZ_LIT("Invalid attribute number"));
			return NULL;
	}
}


// Returns the length for the system column with given attno number
const ULONG
CTranslatorUtils::GetSystemColLength(AttrNumber attno)
{
	SPQOS_ASSERT(FirstLowInvalidHeapAttributeNumber < attno && 0 > attno);

	switch (attno)
	{
		case SelfItemPointerAttributeNumber:
			// tid type
			return 6;

		case ObjectIdAttributeNumber:
		case TableOidAttributeNumber:
			// OID type

		case MinTransactionIdAttributeNumber:
		case MaxTransactionIdAttributeNumber:
			// xid type

		case MinCommandIdAttributeNumber:
		case MaxCommandIdAttributeNumber:
			// cid type

		case XC_NodeIdAttributeNumber:
			// int4
			return 4;

		case BucketIdAttributeNumber:
			return 2;
		case UidAttributeNumber:
			return 8;
		case RootSelfItemPointerAttributeNumber:
			// tid type
			return 6;
		default:
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiPlStmt2DXLConversion,
					   SPQOS_WSZ_LIT("Invalid attribute number"));
			return spqos::ulong_max;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetScanDirection
//
//	@doc:
//		Return the SPQDB specific scan direction from its corresponding DXL
//		representation
//
//---------------------------------------------------------------------------
ScanDirection
CTranslatorUtils::GetScanDirection(EdxlIndexScanDirection idx_scan_direction)
{
	if (EdxlisdBackward == idx_scan_direction)
	{
		return BackwardScanDirection;
	}

	if (EdxlisdForward == idx_scan_direction)
	{
		return ForwardScanDirection;
	}

	return NoMovementScanDirection;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::OidCmpOperator
//
//	@doc:
//		Extract comparison operator from an OpExpr, ScalarArrayOpExpr or RowCompareExpr
//
//---------------------------------------------------------------------------
OID
CTranslatorUtils::OidCmpOperator(Expr *expr)
{
	SPQOS_ASSERT(IsA(expr, OpExpr) || IsA(expr, ScalarArrayOpExpr) ||
				IsA(expr, RowCompareExpr));

	switch (expr->type)
	{
		case T_OpExpr:
			return ((OpExpr *) expr)->opno;

		case T_ScalarArrayOpExpr:
			return ((ScalarArrayOpExpr *) expr)->opno;

		case T_RowCompareExpr:
			return LInitialOID(((RowCompareExpr *) expr)->opnos);

		default:
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiPlStmt2DXLConversion,
					   SPQOS_WSZ_LIT("Unsupported comparison"));
			return InvalidOid;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetOpFamilyForIndexQual
//
//	@doc:
//		Extract comparison operator family for the given index column
//
//---------------------------------------------------------------------------
OID
CTranslatorUtils::GetOpFamilyForIndexQual(INT attno, OID index_oid)
{
	Relation rel = spqdb::GetRelation(index_oid);
	SPQOS_ASSERT(NULL != rel);
	SPQOS_ASSERT(attno <= rel->rd_index->indnatts);

	OID op_family_oid = rel->rd_opfamily[attno - 1];
	spqdb::CloseRelation(rel);

	return op_family_oid;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetSetOpType
//
//	@doc:
//		Return the DXL representation of the set operation
//
//---------------------------------------------------------------------------
EdxlSetOpType
CTranslatorUtils::GetSetOpType(SetOperation setop, BOOL is_all)
{
	if (SETOP_UNION == setop && is_all)
	{
		return EdxlsetopUnionAll;
	}

	if (SETOP_INTERSECT == setop && is_all)
	{
		return EdxlsetopIntersectAll;
	}

	if (SETOP_EXCEPT == setop && is_all)
	{
		return EdxlsetopDifferenceAll;
	}

	if (SETOP_UNION == setop)
	{
		return EdxlsetopUnion;
	}

	if (SETOP_INTERSECT == setop)
	{
		return EdxlsetopIntersect;
	}

	if (SETOP_EXCEPT == setop)
	{
		return EdxlsetopDifference;
	}

	SPQOS_ASSERT(!"Unrecognized set operator type");
	return EdxlsetopSentinel;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetGroupingColidArray
//
//	@doc:
//		Construct a dynamic array of column ids for the given set of grouping
// 		col attnos
//
//---------------------------------------------------------------------------
ULongPtrArray *
CTranslatorUtils::GetGroupingColidArray(
	CMemoryPool *mp, CBitSet *group_by_cols,
	IntToUlongMap *sort_group_cols_to_colid_map)
{
	ULongPtrArray *colids = SPQOS_NEW(mp) ULongPtrArray(mp);

	if (NULL != group_by_cols)
	{
		CBitSetIter bsi(*group_by_cols);

		while (bsi.Advance())
		{
			const ULONG colid =
				GetColId(bsi.Bit(), sort_group_cols_to_colid_map);
			colids->Append(SPQOS_NEW(mp) ULONG(colid));
		}
	}

	return colids;
}
CBitSetArray *CTranslatorUtils::CreateGroupingSetsForSets(CMemoryPool *mp, const GroupingSet *grouping_set,
    ULONG num_cols, CBitSet *group_cols, UlongToUlongMap *group_col_pos)
{
    SPQOS_ASSERT(nullptr != grouping_set);
    SPQOS_ASSERT(grouping_set->kind == GROUPING_SET_SETS);
    CBitSetArray *col_attnos_arr = SPQOS_NEW(mp)CBitSetArray(mp);

    ListCell *cell = nullptr;
    ForEach(cell, grouping_set->content)
    {
        Node *n = (Node *)lfirst(cell);
        SPQOS_ASSERT(IsA(n, GroupingSet));
        GroupingSet *gs_current = (GroupingSet *)n;

        CBitSet *bset = nullptr;
        switch (gs_current->kind) {
            case GROUPING_SET_EMPTY:
                bset = SPQOS_NEW(mp)CBitSet(mp, num_cols);
                break;
            case GROUPING_SET_SIMPLE:
                bset = CreateAttnoSetForGroupingSet(mp, gs_current->content, num_cols, group_col_pos, group_cols);
                break;
            default:
                SPQOS_RAISE(ExmaDXL, ExmiQuery2DXLUnsupportedFeature, SPQOS_WSZ_LIT("nested grouping set"));
        }
        col_attnos_arr->Append(bset);
    }
    return col_attnos_arr;
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnAttnosForGroupBy
//
//	@doc:
//		Construct a dynamic array of sets of column attnos corresponding to the
// 		group by clause
//
//---------------------------------------------------------------------------
CBitSetArray *
CTranslatorUtils::GetColumnAttnosForGroupBy(
	CMemoryPool *mp, List *group_clause_list, List *grouping_set_list, ULONG num_cols,
	UlongToUlongMap *group_col_pos,	// mapping of grouping col positions to SortGroupRef ids
	CBitSet *group_cols	 // existing uniqueue grouping columns
)
{
	SPQOS_ASSERT(NULL != group_clause_list);
	SPQOS_ASSERT(0 < spqdb::ListLength(group_clause_list));
	SPQOS_ASSERT(NULL != group_col_pos);
 
	Node *node = (Node *) LInitial(group_clause_list);
	if (NULL == node || IsA(node, SortGroupClause))
	{
		// simple group by
		CBitSet *col_attnos = CreateAttnoSetForGroupingSet(
			mp, group_clause_list, num_cols, group_col_pos, group_cols);
		CBitSetArray *col_attnos_arr = SPQOS_NEW(mp) CBitSetArray(mp);
		col_attnos_arr->Append(col_attnos);
		return col_attnos_arr;
	}
 
	/*if (!IsA(node, GroupingClause))
	{
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
				   SPQOS_WSZ_LIT("Group by clause"));
	}*/
 
	const ULONG num_group_clauses = spqdb::ListLength(group_clause_list);
	SPQOS_ASSERT(0 < num_group_clauses);
	if (1 < num_group_clauses)
	{
		// multiple grouping sets
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
				   SPQOS_WSZ_LIT("Multiple grouping sets specifications"));
	}
 
	/*GroupingClause *grouping_clause = (GroupingClause *) node;
	if (GROUPINGTYPE_ROLLUP == grouping_clause->groupType)
	{
		return CreateGroupingSetsForRollup(mp, grouping_clause, num_cols,
										   group_col_pos, group_cols);
	}
 
	if (GROUPINGTYPE_CUBE == grouping_clause->groupType)
	{
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
				   SPQOS_WSZ_LIT("Cube"));
	}*/
 
	CBitSetArray *col_attnos_arr = SPQOS_NEW(mp) CBitSetArray(mp);
 
	//ListCell *grouping_set = NULL;
	/*ForEach(grouping_set, grouping_clause->groupsets)
	{
		Node *grouping_set_node = (Node *) lfirst(grouping_set);
 
		CBitSet *bset = NULL;
		if (IsA(grouping_set_node, SortGroupClause))
		{
			// grouping set contains a single grouping column
			bset = SPQOS_NEW(mp) CBitSet(mp, num_cols);
			ULONG sort_group_ref =
				((SortGroupClause *) grouping_set_node)->tleSortGroupRef;
			bset->ExchangeSet(sort_group_ref);
			UpdateGrpColMapping(mp, group_col_pos, group_cols, sort_group_ref);
		}
		else if (IsA(grouping_set_node, GroupingClause))
		{
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
					   SPQOS_WSZ_LIT("Multiple grouping sets specifications"));
		}
		else
		{
			// grouping set contains a list of columns
			SPQOS_ASSERT(IsA(grouping_set_node, List));
 
			List *grouping_set_list = (List *) grouping_set_node;
			bset = CreateAttnoSetForGroupingSet(mp, grouping_set_list, num_cols,
												group_col_pos, group_cols);
		}
		col_attnos_arr->Append(bset);
	}*/
 
	return col_attnos_arr;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateGroupingSetsForRollup
//
//	@doc:
//		Construct a dynamic array of sets of column attnos for a rollup
//
//---------------------------------------------------------------------------
CBitSetArray *
CTranslatorUtils::CreateGroupingSetsForRollup(
	CMemoryPool *mp, GroupingSet *grouping_set, ULONG num_cols,
	UlongToUlongMap *
		group_col_pos,	// mapping of grouping col positions to SortGroupRef ids,
	CBitSet *group_cols	 // existing grouping columns
)
{
    SPQOS_ASSERT(nullptr != grouping_set);
    SPQOS_ASSERT(grouping_set->kind == GROUPING_SET_ROLLUP);
    CBitSetArray *col_attnos_arr = SPQOS_NEW(mp)CBitSetArray(mp);
    ListCell *lc = nullptr;
    CBitSet *current_result = SPQOS_NEW(mp)CBitSet(mp);
    ForEach(lc, grouping_set->content)
    {
        GroupingSet *gs_current = (GroupingSet *)lfirst(lc);
        SPQOS_ASSERT(gs_current->kind == GROUPING_SET_SIMPLE);

        CBitSet *bset = CreateAttnoSetForGroupingSet(mp, gs_current->content, num_cols, group_col_pos, group_cols);
        current_result->Union(bset);
        bset->Release();
        col_attnos_arr->Append(SPQOS_NEW(mp)CBitSet(mp, *current_result));
    }
    // add an empty set
    col_attnos_arr->Append(SPQOS_NEW(mp)CBitSet(mp));
    current_result->Release();
    return col_attnos_arr;
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateAttnoSetForGroupingSet
//
//	@doc:
//		Construct a set of column attnos corresponding to a grouping set
//
//---------------------------------------------------------------------------
CBitSet *
CTranslatorUtils::CreateAttnoSetForGroupingSet(
	CMemoryPool *mp, List *group_elems, ULONG num_cols,
	UlongToUlongMap *
		group_col_pos,	// mapping of grouping col positions to SortGroupRef ids,
	CBitSet *group_cols	 // existing grouping columns
)
{
	SPQOS_ASSERT(NULL != group_elems);
	SPQOS_ASSERT(0 < spqdb::ListLength(group_elems));

	CBitSet *bs = SPQOS_NEW(mp) CBitSet(mp, num_cols);

	ListCell *lc = NULL;
	ForEach(lc, group_elems)
	{
		Node *elem_node = (Node *) lfirst(lc);

		if (NULL == elem_node)
		{
			continue;
		}

		if (!IsA(elem_node, SortGroupClause))
		{
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature,
					   SPQOS_WSZ_LIT(
						   "Mixing grouping sets with simple group by lists"));
		}

		ULONG sort_group_ref = ((SortGroupClause *) elem_node)->tleSortGroupRef;
		bs->ExchangeSet(sort_group_ref);

		UpdateGrpColMapping(mp, group_col_pos, group_cols, sort_group_ref);
	}

	return bs;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GenerateColIds
//
//	@doc:
//		Construct an array of DXL column identifiers for a target list
//
//---------------------------------------------------------------------------
ULongPtrArray *
CTranslatorUtils::GenerateColIds(
	CMemoryPool *mp, List *target_list, IMdIdArray *input_mdid_arr,
	ULongPtrArray *input_colids,
	BOOL *
		is_outer_ref,  // array of flags indicating if input columns are outer references
	CIdGenerator *colid_generator)
{
	SPQOS_ASSERT(NULL != target_list);
	SPQOS_ASSERT(NULL != input_mdid_arr);
	SPQOS_ASSERT(NULL != input_colids);
	SPQOS_ASSERT(NULL != is_outer_ref);
	SPQOS_ASSERT(NULL != colid_generator);

	SPQOS_ASSERT(input_mdid_arr->Size() == input_colids->Size());

	ULONG col_pos = 0;
	ListCell *target_entry_cell = NULL;
	ULongPtrArray *colid_array = SPQOS_NEW(mp) ULongPtrArray(mp);

	ForEach(target_entry_cell, target_list)
	{
		TargetEntry *target_entry = (TargetEntry *) lfirst(target_entry_cell);
		SPQOS_ASSERT(NULL != target_entry->expr);

		OID expr_type_oid = spqdb::ExprType((Node *) target_entry->expr);
		if (!target_entry->resjunk)
		{
			ULONG colid = spqos::ulong_max;
			IMDId *mdid = (*input_mdid_arr)[col_pos];
			if (CMDIdSPQDB::CastMdid(mdid)->Oid() != expr_type_oid ||
				is_outer_ref[col_pos])
			{
				// generate a new column when:
				//  (1) the type of input column does not match that of the output column, or
				//  (2) input column is an outer reference
				colid = colid_generator->next_id();
			}
			else
			{
				// use the column identifier of the input
				colid = *(*input_colids)[col_pos];
			}
			SPQOS_ASSERT(spqos::ulong_max != colid);

			colid_array->Append(SPQOS_NEW(mp) ULONG(colid));

			col_pos++;
		}
	}

	return colid_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::FixUnknownTypeConstant
//
//	@doc:
//		If the query has constant of unknown type, then return a copy of the
//		query with all constants of unknown type being coerced to the common data type
//		of the output target list; otherwise return the original query
//---------------------------------------------------------------------------
Query *
CTranslatorUtils::FixUnknownTypeConstant(Query *old_query,
										 List *output_target_list)
{
	SPQOS_ASSERT(NULL != old_query);
	SPQOS_ASSERT(NULL != output_target_list);

	Query *new_query = NULL;

	ULONG pos = 0;
	ULONG col_pos = 0;
	ListCell *target_entry_cell = NULL;
	ForEach(target_entry_cell, old_query->targetList)
	{
		TargetEntry *old_target_entry =
			(TargetEntry *) lfirst(target_entry_cell);
		SPQOS_ASSERT(NULL != old_target_entry->expr);

		if (!old_target_entry->resjunk)
		{
			if (IsA(old_target_entry->expr, Const) &&
				(SPQDB_UNKNOWN ==
				 spqdb::ExprType((Node *) old_target_entry->expr)))
			{
				if (NULL == new_query)
				{
					new_query = (Query *) spqdb::CopyObject(
						const_cast<Query *>(old_query));
				}

				TargetEntry *new_target_entry =
					(TargetEntry *) spqdb::ListNth(new_query->targetList, pos);
				SPQOS_ASSERT(old_target_entry->resno == new_target_entry->resno);
				// implicitly cast the unknown constants to the target data type
				OID target_type_oid =
					GetTargetListReturnTypeOid(output_target_list, col_pos);
				SPQOS_ASSERT(InvalidOid != target_type_oid);
				Node *old_node = (Node *) new_target_entry->expr;
				new_target_entry->expr = (Expr *) spqdb::CoerceToCommonType(
					NULL, /* pstate */
					(Node *) old_node, target_type_oid,
					"UNION/INTERSECT/EXCEPT");

				spqdb::SPQDBFree(old_node);
			}
			col_pos++;
		}

		pos++;
	}

	if (NULL == new_query)
	{
		return old_query;
	}

	spqdb::SPQDBFree(old_query);

	return new_query;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetTargetListReturnTypeOid
//
//	@doc:
//		Return the type of the nth non-resjunked target list entry
//
//---------------------------------------------------------------------------
OID
CTranslatorUtils::GetTargetListReturnTypeOid(List *target_list, ULONG col_pos)
{
	ULONG col_idx = 0;
	ListCell *target_entry_cell = NULL;

	ForEach(target_entry_cell, target_list)
	{
		TargetEntry *target_entry = (TargetEntry *) lfirst(target_entry_cell);
		SPQOS_ASSERT(NULL != target_entry->expr);

		if (!target_entry->resjunk)
		{
			if (col_idx == col_pos)
			{
				return spqdb::ExprType((Node *) target_entry->expr);
			}

			col_idx++;
		}
	}

	return InvalidOid;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetDXLColumnDescrArray
//
//	@doc:
//		Construct an array of DXL column descriptors for a target list using the
// 		column ids in the given array
//
//---------------------------------------------------------------------------
CDXLColDescrArray *
CTranslatorUtils::GetDXLColumnDescrArray(CMemoryPool *mp, List *target_list,
										 ULongPtrArray *colids,
										 BOOL keep_res_junked)
{
	SPQOS_ASSERT(NULL != target_list);
	SPQOS_ASSERT(NULL != colids);

	ListCell *target_entry_cell = NULL;
	CDXLColDescrArray *dxl_col_descrs = SPQOS_NEW(mp) CDXLColDescrArray(mp);
	ULONG ul = 0;
	ForEach(target_entry_cell, target_list)
	{
		TargetEntry *target_entry = (TargetEntry *) lfirst(target_entry_cell);

		if (target_entry->resjunk && !keep_res_junked)
		{
			continue;
		}

		ULONG colid = *(*colids)[ul];
		CDXLColDescr *dxl_col_descr =
			GetColumnDescrAt(mp, target_entry, colid, ul + 1 /*pos*/);
		dxl_col_descrs->Append(dxl_col_descr);
		ul++;
	}

	SPQOS_ASSERT(dxl_col_descrs->Size() == colids->Size());

	return dxl_col_descrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetPosInTargetList
//
//	@doc:
//		Return the positions of the target list entries included in the output
//		target list
//---------------------------------------------------------------------------
ULongPtrArray *
CTranslatorUtils::GetPosInTargetList(CMemoryPool *mp, List *target_list,
									 BOOL keep_res_junked)
{
	SPQOS_ASSERT(NULL != target_list);

	ListCell *target_entry_cell = NULL;
	ULongPtrArray *positions = SPQOS_NEW(mp) ULongPtrArray(mp);
	ULONG ul = 0;
	ForEach(target_entry_cell, target_list)
	{
		TargetEntry *target_entry = (TargetEntry *) lfirst(target_entry_cell);

		if (target_entry->resjunk && !keep_res_junked)
		{
			continue;
		}

		positions->Append(SPQOS_NEW(mp) ULONG(ul));
		ul++;
	}

	return positions;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColumnDescrAt
//
//	@doc:
//		Construct a column descriptor from the given target entry and column
//		identifier
//---------------------------------------------------------------------------
CDXLColDescr *
CTranslatorUtils::GetColumnDescrAt(CMemoryPool *mp, TargetEntry *target_entry,
								   ULONG colid, ULONG pos)
{
	SPQOS_ASSERT(NULL != target_entry);
	SPQOS_ASSERT(spqos::ulong_max != colid);

	CMDName *mdname = NULL;
	if (NULL == target_entry->resname)
	{
		CWStringConst unnamed_col(SPQOS_WSZ_LIT("?column?"));
		mdname = SPQOS_NEW(mp) CMDName(mp, &unnamed_col);
	}
	else
	{
		CWStringDynamic *alias = CDXLUtils::CreateDynamicStringFromCharArray(
			mp, target_entry->resname);
		mdname = SPQOS_NEW(mp) CMDName(mp, alias);
		// CName constructor copies string
		SPQOS_DELETE(alias);
	}

	// create a column descriptor
	OID type_oid = spqdb::ExprType((Node *) target_entry->expr);
	INT type_modifier = spqdb::ExprTypeMod((Node *) target_entry->expr);
	CMDIdSPQDB *col_type = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, type_oid);
	CDXLColDescr *dxl_col_descr =
		SPQOS_NEW(mp) CDXLColDescr(mp, mdname, colid, pos,  /* attno */
								  col_type, type_modifier, /* type_modifier */
								  false					   /* fColDropped */
		);

	return dxl_col_descr;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateDummyProjectElem
//
//	@doc:
//		Create a dummy project element to rename the input column identifier
//---------------------------------------------------------------------------
CDXLNode *
CTranslatorUtils::CreateDummyProjectElem(CMemoryPool *mp, ULONG colid_input,
										 ULONG colid_output,
										 CDXLColDescr *dxl_col_descr)
{
	CMDIdSPQDB *original_mdid = CMDIdSPQDB::CastMdid(dxl_col_descr->MdidType());
	CMDIdSPQDB *copy_mdid = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidGeneral, original_mdid->Oid(),
				  original_mdid->VersionMajor(), original_mdid->VersionMinor());

	// create a column reference for the scalar identifier to be casted
	CMDName *mdname =
		SPQOS_NEW(mp) CMDName(mp, dxl_col_descr->MdName()->GetMDName());
	CDXLColRef *dxl_colref = SPQOS_NEW(mp) CDXLColRef(
		mp, mdname, colid_input, copy_mdid, dxl_col_descr->TypeModifier());
	CDXLScalarIdent *dxl_scalar_ident =
		SPQOS_NEW(mp) CDXLScalarIdent(mp, dxl_colref);

	CDXLNode *dxl_project_element = SPQOS_NEW(mp) CDXLNode(
		mp,
		SPQOS_NEW(mp) CDXLScalarProjElem(
			mp, colid_output,
			SPQOS_NEW(mp) CMDName(mp, dxl_col_descr->MdName()->GetMDName())),
		SPQOS_NEW(mp) CDXLNode(mp, dxl_scalar_ident));

	return dxl_project_element;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetOutputColIdsArray
//
//	@doc:
//		Construct an array of colids for the given target list
//
//---------------------------------------------------------------------------
ULongPtrArray *
CTranslatorUtils::GetOutputColIdsArray(CMemoryPool *mp, List *target_list,
									   IntToUlongMap *attno_to_colid_map)
{
	SPQOS_ASSERT(NULL != target_list);
	SPQOS_ASSERT(NULL != attno_to_colid_map);

	ULongPtrArray *colids = SPQOS_NEW(mp) ULongPtrArray(mp);

	ListCell *target_entry_cell = NULL;
	ForEach(target_entry_cell, target_list)
	{
		TargetEntry *target_entry = (TargetEntry *) lfirst(target_entry_cell);
		ULONG resno = (ULONG) target_entry->resno;
		INT attno = (INT) target_entry->resno;
		const ULONG *ul = attno_to_colid_map->Find(&attno);

		if (NULL == ul)
		{
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLAttributeNotFound,
					   resno);
		}

		colids->Append(SPQOS_NEW(mp) ULONG(*ul));
	}

	return colids;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColId
//
//	@doc:
//		Return the corresponding ColId for the given index into the target list
//
//---------------------------------------------------------------------------
ULONG
CTranslatorUtils::GetColId(INT index, IntToUlongMap *colid_map)
{
	SPQOS_ASSERT(0 < index);

	const ULONG *ul = colid_map->Find(&index);

	if (NULL == ul)
	{
		SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLAttributeNotFound,
				   index);
	}

	return *ul;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetColId
//
//	@doc:
//		Return the corresponding ColId for the given varno, varattno and querylevel
//
//---------------------------------------------------------------------------
ULONG
CTranslatorUtils::GetColId(ULONG query_level, INT varno, INT var_attno,
						   IMDId *mdid, CMappingVarColId *var_colid_mapping)
{
	OID oid = CMDIdSPQDB::CastMdid(mdid)->Oid();
	Var *var = spqdb::MakeVar(varno, var_attno, oid, -1, 0);
	ULONG colid = var_colid_mapping->GetColId(query_level, var, EpspotNone);
	spqdb::SPQDBFree(var);

	return colid;
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetWindowSpecTargetEntry
//
//	@doc:
//		Extract a matching target entry that is a window spec
//
//---------------------------------------------------------------------------
TargetEntry *
CTranslatorUtils::GetWindowSpecTargetEntry(Node *node, List *window_clause_list,
										   List *target_list)
{
	SPQOS_ASSERT(NULL != node);
	List *target_list_subset =
		spqdb::FindMatchingMembersInTargetList(node, target_list);

	ListCell *target_entry_cell = NULL;
	ForEach(target_entry_cell, target_list_subset)
	{
		TargetEntry *cur_target_entry =
			(TargetEntry *) lfirst(target_entry_cell);
		if (IsReferencedInWindowSpec(cur_target_entry, window_clause_list))
		{
			spqdb::SPQDBFree(target_list_subset);
			return cur_target_entry;
		}
	}

	if (NIL != target_list_subset)
	{
		spqdb::SPQDBFree(target_list_subset);
	}
	return NULL;
}


//---------------------------------------------------------------------------
// CTranslatorUtils::IsReferencedInWindowSpec
// Check if the TargetEntry is a used in a window specification
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsReferencedInWindowSpec(const TargetEntry *target_entry,
										   List *window_clause_list)
{
	ListCell *window_clause_cell;
	ForEach(window_clause_cell, window_clause_list)
	{
		WindowClause *window_clause =
			(WindowClause *) lfirst(window_clause_cell);
		if (IsSortingColumn(target_entry, window_clause->orderClause) ||
			IsSortingColumn(target_entry, window_clause->partitionClause))
		{
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateDXLProjElemFromInt8Const
//
//	@doc:
// 		Construct a scalar const value expression for the given BIGINT value
//
//---------------------------------------------------------------------------
CDXLNode *
CTranslatorUtils::CreateDXLProjElemFromInt8Const(CMemoryPool *mp,
												 CMDAccessor *md_accessor,
												 INT val)
{
	SPQOS_ASSERT(NULL != mp);
	const IMDTypeInt8 *md_type_int8 = md_accessor->PtMDType<IMDTypeInt8>();
	md_type_int8->MDId()->AddRef();

	CDXLDatumInt8 *datum_dxl = SPQOS_NEW(mp)
		CDXLDatumInt8(mp, md_type_int8->MDId(), false /*fConstNull*/, val);

	CDXLScalarConstValue *dxl_scalar_const =
		SPQOS_NEW(mp) CDXLScalarConstValue(mp, datum_dxl);

	return SPQOS_NEW(mp) CDXLNode(mp, dxl_scalar_const);
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsSortingColumn
//
//	@doc:
//		Check if the TargetEntry is a sorting column
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsSortingColumn(const TargetEntry *target_entry,
								  List *sort_clause_list)
{
	ListCell *sort_clause_cell = NULL;
	ForEach(sort_clause_cell, sort_clause_list)
	{
		Node *sort_clause = (Node *) lfirst(sort_clause_cell);
		if (IsA(sort_clause, SortGroupClause) &&
			target_entry->ressortgroupref ==
				((SortGroupClause *) sort_clause)->tleSortGroupRef)
		{
			return true;
		}
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetGroupingColumnTargetEntry
//
//	@doc:
//		Extract a matching target entry that is a grouping column
//---------------------------------------------------------------------------
TargetEntry *
CTranslatorUtils::GetGroupingColumnTargetEntry(Node *node, List *group_clause,
											   List *target_list)
{
	SPQOS_ASSERT(NULL != node);
	List *target_list_subset =
		spqdb::FindMatchingMembersInTargetList(node, target_list);

	ListCell *target_entry_cell = NULL;
	ForEach(target_entry_cell, target_list_subset)
	{
		TargetEntry *next_target_entry =
			(TargetEntry *) lfirst(target_entry_cell);
		if (IsGroupingColumn(next_target_entry, group_clause))
		{
			spqdb::SPQDBFree(target_list_subset);
			return next_target_entry;
		}
	}

	if (NIL != target_list_subset)
	{
		spqdb::SPQDBFree(target_list_subset);
	}
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsGroupingColumn
//
//	@doc:
//		Check if the expression has a matching target entry that is a grouping column
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsGroupingColumn(Node *node, List *group_clause,
								   List *target_list)
{
	SPQOS_ASSERT(NULL != node);

	TargetEntry *grouping_col_target_etnry =
		GetGroupingColumnTargetEntry(node, group_clause, target_list);

	return (NULL != grouping_col_target_etnry);
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsGroupingColumn
//
//	@doc:
//		Check if the TargetEntry is a grouping column
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsGroupingColumn(const TargetEntry *target_entry,
								   List *group_clause)
{
	ListCell *group_clause_cell = NULL;
	ForEach(group_clause_cell, group_clause)
	{
		Node *group_clause_node = (Node *) lfirst(group_clause_cell);

		if (NULL == group_clause_node)
		{
			continue;
		}

		if (IsA(group_clause_node, SortGroupClause) &&
			IsGroupingColumn(target_entry,
							 (SortGroupClause *) group_clause_node))
		{
			return true;
		}
		// TODO SPQ no groupsets struct
		/*if (IsA(group_clause_node, GroupingSet))
		{
			GroupingSet *grouping_clause =
				(GroupingSet *) group_clause_node;

			ListCell *grouping_set = NULL;
			ForEach(grouping_set, grouping_clause->groupsets)
			{
				Node *grouping_set_node = (Node *) lfirst(grouping_set);

				if (IsA(grouping_set_node, SortGroupClause) &&
					IsGroupingColumn(target_entry,
									 ((SortGroupClause *) grouping_set_node)))
				{
					return true;
				}

				if (IsA(grouping_set_node, List) &&
					IsGroupingColumn(target_entry, (List *) grouping_set_node))
				{
					return true;
				}
			}
		}*/
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsGroupingColumn
//
//	@doc:
//		Check if the TargetEntry is a grouping column
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsGroupingColumn(const TargetEntry *target_entry,
								   const SortGroupClause *grouping_clause)
{
	SPQOS_ASSERT(NULL != grouping_clause);

	return (target_entry->ressortgroupref == grouping_clause->tleSortGroupRef);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::PlAttnosFromColids
//
//	@doc:
//		Translate an array of colids to a list of attribute numbers using
//		the mappings in the provided context
//---------------------------------------------------------------------------
List *
CTranslatorUtils::ConvertColidToAttnos(ULongPtrArray *colids,
									   CDXLTranslateContext *translate_ctxt)
{
	SPQOS_ASSERT(NULL != colids);
	SPQOS_ASSERT(NULL != translate_ctxt);

	List *result = NIL;

	const ULONG length = colids->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		ULONG colid = *((*colids)[ul]);
		const TargetEntry *target_entry = translate_ctxt->GetTargetEntry(colid);
		SPQOS_ASSERT(NULL != target_entry);
		result = spqdb::LAppendInt(result, target_entry->resno);
	}

	return result;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetLongFromStr
//
//	@doc:
//		Parses a long integer value from a string
//
//---------------------------------------------------------------------------
LINT
CTranslatorUtils::GetLongFromStr(const CWStringBase *wcstr)
{
	CHAR *str = CreateMultiByteCharStringFromWCString(wcstr->GetBuffer());
	CHAR *end = NULL;
	return spqos::clib::Strtol(str, &end, 10);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetIntFromStr
//
//	@doc:
//		Parses an integer value from a string
//
//---------------------------------------------------------------------------
INT
CTranslatorUtils::GetIntFromStr(const CWStringBase *wcstr)
{
	return (INT) GetLongFromStr(wcstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateMultiByteCharStringFromWCString
//
//	@doc:
//		Converts a wide character string into a character array
//
//---------------------------------------------------------------------------
CHAR *
CTranslatorUtils::CreateMultiByteCharStringFromWCString(const WCHAR *wcstr)
{
	SPQOS_ASSERT(NULL != wcstr);

	ULONG max_len = SPQOS_WSZ_LENGTH(wcstr) * SPQOS_SIZEOF(WCHAR) + 1;
	CHAR *str = (CHAR *) spqdb::SPQDBAlloc(max_len);
#ifdef SPQOS_DEBUG
	LINT li = (INT)
#endif
		clib::Wcstombs(str, const_cast<WCHAR *>(wcstr), max_len);
	SPQOS_ASSERT(0 <= li);

	str[max_len - 1] = '\0';

	return str;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::MakeNewToOldColMapping
//
//	@doc:
//		Create a mapping from old columns to the corresponding new column
//
//---------------------------------------------------------------------------
UlongToUlongMap *
CTranslatorUtils::MakeNewToOldColMapping(CMemoryPool *mp,
										 ULongPtrArray *old_colids,
										 ULongPtrArray *new_colids)
{
	SPQOS_ASSERT(NULL != old_colids);
	SPQOS_ASSERT(NULL != new_colids);
	SPQOS_ASSERT(new_colids->Size() == old_colids->Size());

	UlongToUlongMap *old_new_col_mapping = SPQOS_NEW(mp) UlongToUlongMap(mp);
	const ULONG num_cols = old_colids->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		ULONG old_colid = *((*old_colids)[ul]);
		ULONG new_colid = *((*new_colids)[ul]);
#ifdef SPQOS_DEBUG
		BOOL result =
#endif	// SPQOS_DEBUG
			old_new_col_mapping->Insert(SPQOS_NEW(mp) ULONG(old_colid),
										SPQOS_NEW(mp) ULONG(new_colid));
		SPQOS_ASSERT(result);
	}

	return old_new_col_mapping;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsDuplicateSensitiveMotion
//
//	@doc:
//		Is this a motion sensitive to duplicates
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsDuplicateSensitiveMotion(CDXLPhysicalMotion *dxl_motion)
{
	Edxlopid dxl_opid = dxl_motion->GetDXLOperator();

	if (EdxlopPhysicalMotionRedistribute == dxl_opid)
	{
		return CDXLPhysicalRedistributeMotion::Cast(dxl_motion)
			->IsDuplicateSensitive();
	}

	if (EdxlopPhysicalMotionRandom == dxl_opid)
	{
		return CDXLPhysicalRandomMotion::Cast(dxl_motion)
			->IsDuplicateSensitive();
	}

	// other motion operators are not sensitive to duplicates
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::HasProjElem
//
//	@doc:
//		Check whether the given project list has a project element of the given
//		operator type
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::HasProjElem(CDXLNode *project_list_dxlnode, Edxlopid dxl_opid)
{
	SPQOS_ASSERT(NULL != project_list_dxlnode);
	SPQOS_ASSERT(EdxlopScalarProjectList ==
				project_list_dxlnode->GetOperator()->GetDXLOperator());
	SPQOS_ASSERT(EdxlopSentinel > dxl_opid);

	const ULONG arity = project_list_dxlnode->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CDXLNode *dxl_project_element = (*project_list_dxlnode)[ul];
		SPQOS_ASSERT(EdxlopScalarProjectElem ==
					dxl_project_element->GetOperator()->GetDXLOperator());

		CDXLNode *dxl_child_node = (*dxl_project_element)[0];
		if (dxl_opid == dxl_child_node->GetOperator()->GetDXLOperator())
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateDXLProjElemConstNULL
//
//	@doc:
//		Create a DXL project element node with a Const NULL of type provided
//		by the column descriptor.
//
//---------------------------------------------------------------------------
CDXLNode *
CTranslatorUtils::CreateDXLProjElemConstNULL(CMemoryPool *mp,
											 CMDAccessor *md_accessor,
											 CIdGenerator *pidgtorCol,
											 const IMDColumn *md_col)
{
	SPQOS_ASSERT(NULL != md_col);
	SPQOS_ASSERT(!md_col->IsSystemColumn());

	const WCHAR *col_name = md_col->Mdname().GetMDName()->GetBuffer();
	ULONG colid = pidgtorCol->next_id();
	CDXLNode *dxl_project_element = CreateDXLProjElemConstNULL(
		mp, md_accessor, md_col->MdidType(), colid, col_name);

	return dxl_project_element;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateDXLProjElemConstNULL
//
//	@doc:
//		Create a DXL project element node with a Const NULL expression
//---------------------------------------------------------------------------
CDXLNode *
CTranslatorUtils::CreateDXLProjElemConstNULL(CMemoryPool *mp,
											 CMDAccessor *md_accessor,
											 IMDId *mdid, ULONG colid,
											 const WCHAR *col_name)
{
	CHAR *column_name =
		CDXLUtils::CreateMultiByteCharStringFromWCString(mp, col_name);
	CDXLNode *dxl_project_element =
		CreateDXLProjElemConstNULL(mp, md_accessor, mdid, colid, column_name);

	SPQOS_DELETE_ARRAY(column_name);

	return dxl_project_element;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CreateDXLProjElemConstNULL
//
//	@doc:
//		Create a DXL project element node with a Const NULL expression
//---------------------------------------------------------------------------
CDXLNode *
CTranslatorUtils::CreateDXLProjElemConstNULL(CMemoryPool *mp,
											 CMDAccessor *md_accessor,
											 IMDId *mdid, ULONG colid,
											 CHAR *alias_name)
{
	// get the id and alias for the proj elem
	CMDName *alias_mdname = NULL;

	if (NULL == alias_name)
	{
		CWStringConst unnamed_col(SPQOS_WSZ_LIT("?column?"));
		alias_mdname = SPQOS_NEW(mp) CMDName(mp, &unnamed_col);
	}
	else
	{
		CWStringDynamic *alias =
			CDXLUtils::CreateDynamicStringFromCharArray(mp, alias_name);
		alias_mdname = SPQOS_NEW(mp) CMDName(mp, alias);
		SPQOS_DELETE(alias);
	}

	mdid->AddRef();
	CDXLDatum *datum_dxl = NULL;
	if (mdid->Equals(&CMDIdSPQDB::m_mdid_int2))
	{
		datum_dxl = SPQOS_NEW(mp)
			CDXLDatumInt2(mp, mdid, true /*fConstNull*/, 0 /*value*/);
	}
	else if (mdid->Equals(&CMDIdSPQDB::m_mdid_int4))
	{
		datum_dxl = SPQOS_NEW(mp)
			CDXLDatumInt4(mp, mdid, true /*fConstNull*/, 0 /*value*/);
	}
	else if (mdid->Equals(&CMDIdSPQDB::m_mdid_int8))
	{
		datum_dxl = SPQOS_NEW(mp)
			CDXLDatumInt8(mp, mdid, true /*fConstNull*/, 0 /*value*/);
	}
	else if (mdid->Equals(&CMDIdSPQDB::m_mdid_bool))
	{
		datum_dxl = SPQOS_NEW(mp)
			CDXLDatumBool(mp, mdid, true /*fConstNull*/, 0 /*value*/);
	}
	else if (mdid->Equals(&CMDIdSPQDB::m_mdid_oid))
	{
		datum_dxl = SPQOS_NEW(mp)
			CDXLDatumOid(mp, mdid, true /*fConstNull*/, 0 /*value*/);
	}
	else
	{
		const IMDType *md_type = md_accessor->RetrieveType(mdid);
		datum_dxl = CMDTypeGenericSPQDB::CreateDXLDatumVal(
			mp, mdid, md_type, default_type_modifier, true /*fConstNull*/,
			NULL,						   /*pba */
			0 /*length*/, 0 /*l_value*/, 0 /*dValue*/
		);
	}

	CDXLNode *dxl_const_node = SPQOS_NEW(mp)
		CDXLNode(mp, SPQOS_NEW(mp) CDXLScalarConstValue(mp, datum_dxl));

	return SPQOS_NEW(mp)
		CDXLNode(mp, SPQOS_NEW(mp) CDXLScalarProjElem(mp, colid, alias_mdname),
				 dxl_const_node);
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::CheckRTEPermissions
//
//	@doc:
//		Check permissions on range table
//
//---------------------------------------------------------------------------
void
CTranslatorUtils::CheckRTEPermissions(List *range_table_list)
{
	spqdb::CheckRTPermissions(range_table_list);
}


//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::UpdateGrpColMapping
//
//	@doc:
//		Update grouping columns permission mappings
//
//---------------------------------------------------------------------------
void
CTranslatorUtils::UpdateGrpColMapping(CMemoryPool *mp,
									  UlongToUlongMap *group_col_pos,
									  CBitSet *group_cols, ULONG sort_group_ref)
{
	SPQOS_ASSERT(NULL != group_col_pos);
	SPQOS_ASSERT(NULL != group_cols);

	if (!group_cols->Get(sort_group_ref))
	{
		ULONG num_unique_grouping_cols = group_cols->Size();
		group_col_pos->Insert(SPQOS_NEW(mp) ULONG(num_unique_grouping_cols),
							  SPQOS_NEW(mp) ULONG(sort_group_ref));
		(void) group_cols->ExchangeSet(sort_group_ref);
	}
}


//---------------------------------------------------------------------------
//      @function:
//              CTranslatorUtils::MarkOuterRefs
//
//      @doc:
//		check if given column ids are outer refs in the tree rooted by
//		given node
//---------------------------------------------------------------------------
void
CTranslatorUtils::MarkOuterRefs(
	ULONG *colids,	// array of column ids to be checked
	BOOL *
		is_outer_ref,  // array of outer ref indicators, initially all set to true by caller
	ULONG num_columns,	// number of columns
	CDXLNode *dxlnode)
{
	SPQOS_ASSERT(NULL != colids);
	SPQOS_ASSERT(NULL != is_outer_ref);
	SPQOS_ASSERT(NULL != dxlnode);

	const CDXLOperator *dxl_op = dxlnode->GetOperator();
	for (ULONG ulCol = 0; ulCol < num_columns; ulCol++)
	{
		ULONG colid = colids[ulCol];
		if (is_outer_ref[ulCol] && dxl_op->IsColDefined(colid))
		{
			// column is defined by operator, reset outer reference flag
			is_outer_ref[ulCol] = false;
		}
	}

	// recursively process children
	const ULONG arity = dxlnode->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		MarkOuterRefs(colids, is_outer_ref, num_columns, (*dxlnode)[ul]);
	}
}

//---------------------------------------------------------------------------
//      @function:
//              CTranslatorUtils::MapDXLSubplanToSublinkType
//
//      @doc:
//              Map DXL Subplan type to SPQDB SubLinkType
//
//---------------------------------------------------------------------------
SubLinkType
CTranslatorUtils::MapDXLSubplanToSublinkType(EdxlSubPlanType dxl_subplan_type)
{
	SPQOS_ASSERT(EdxlSubPlanTypeSentinel > dxl_subplan_type);
	ULONG mapping[][2] = {{EdxlSubPlanTypeScalar, EXPR_SUBLINK},
						  {EdxlSubPlanTypeExists, EXISTS_SUBLINK},
						  {EdxlSubPlanTypeNotExists, NOT_EXISTS_SUBLINK},
						  {EdxlSubPlanTypeAny, ANY_SUBLINK},
						  {EdxlSubPlanTypeAll, ALL_SUBLINK}};

	const ULONG arity = SPQOS_ARRAY_SIZE(mapping);
	SubLinkType slink = EXPR_SUBLINK;
	BOOL found = false;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		ULONG *elem = mapping[ul];
		if ((ULONG) dxl_subplan_type == elem[0])
		{
			slink = (SubLinkType) elem[1];
			found = true;
			break;
		}
	}

	SPQOS_ASSERT(found && "Invalid SubPlanType");

	return slink;
}


//---------------------------------------------------------------------------
//      @function:
//              CTranslatorUtils::MapSublinkTypeToDXLSubplan
//
//      @doc:
//              Map SPQDB SubLinkType to DXL subplan type
//
//---------------------------------------------------------------------------
EdxlSubPlanType
CTranslatorUtils::MapSublinkTypeToDXLSubplan(SubLinkType slink)
{
	ULONG mapping[][2] = {{EXPR_SUBLINK, EdxlSubPlanTypeScalar},
						  {EXISTS_SUBLINK, EdxlSubPlanTypeExists},
						  {NOT_EXISTS_SUBLINK, EdxlSubPlanTypeNotExists},
						  {ANY_SUBLINK, EdxlSubPlanTypeAny},
						  {ALL_SUBLINK, EdxlSubPlanTypeAll}};

	const ULONG arity = SPQOS_ARRAY_SIZE(mapping);
	EdxlSubPlanType dxl_subplan_type = EdxlSubPlanTypeScalar;
	BOOL found = false;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		ULONG *elem = mapping[ul];
		if ((ULONG) slink == elem[0])
		{
			dxl_subplan_type = (EdxlSubPlanType) elem[1];
			found = true;
			break;
		}
	}

	SPQOS_ASSERT(found && "Invalid SubLinkType");

	return dxl_subplan_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::RelHasTriggers
//
//	@doc:
//		Check whether there are triggers for the given operation on
//		the given relation
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::RelHasTriggers(CMemoryPool *mp, CMDAccessor *md_accessor,
								 const IMDRelation *rel,
								 const EdxlDmlType dml_type_dxl)
{
	const ULONG num_triggers = rel->TriggerCount();
	for (ULONG ul = 0; ul < num_triggers; ul++)
	{
		if (IsApplicableTrigger(md_accessor, rel->TriggerMDidAt(ul),
								dml_type_dxl))
		{
			return true;
		}
	}

	// if table is partitioned, check for triggers on child partitions as well
	INT type = 0;
	if (Edxldmlinsert == dml_type_dxl)
	{
		type = TRIGGER_TYPE_INSERT;
	}
	else if (Edxldmldelete == dml_type_dxl)
	{
		type = TRIGGER_TYPE_DELETE;
	}
	else
	{
		SPQOS_ASSERT(Edxldmlupdate == dml_type_dxl);
		type = TRIGGER_TYPE_UPDATE;
	}

	OID rel_oid = CMDIdSPQDB::CastMdid(rel->MDId())->Oid();
	return spqdb::ChildPartHasTriggers(rel_oid, type);
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::IsApplicableTrigger
//
//	@doc:
//		Check whether the given trigger is applicable to the given DML operation
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsApplicableTrigger(CMDAccessor *md_accessor,
									  IMDId *trigger_mdid,
									  const EdxlDmlType dml_type_dxl)
{
	const IMDTrigger *trigger = md_accessor->RetrieveTrigger(trigger_mdid);
	if (!trigger->IsEnabled())
	{
		return false;
	}

	return ((Edxldmlinsert == dml_type_dxl && trigger->IsInsert()) ||
			(Edxldmldelete == dml_type_dxl && trigger->IsDelete()) ||
			(Edxldmlupdate == dml_type_dxl && trigger->IsUpdate()));
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::RelHasConstraints
//
//	@doc:
//		Check whether there are constraints for the given relation
//
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::RelHasConstraints(const IMDRelation *rel)
{
	if (0 < rel->CheckConstraintCount())
	{
		return true;
	}

	const ULONG num_cols = rel->ColumnCount();

	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		const IMDColumn *md_col = rel->GetMdCol(ul);
		if (!md_col->IsSystemColumn() && !md_col->IsNullable())
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetAssertErrorMsgs
//
//	@doc:
//		Construct a list of error messages from a list of assert constraints
//
//---------------------------------------------------------------------------
List *
CTranslatorUtils::GetAssertErrorMsgs(CDXLNode *assert_constraint_list)
{
	SPQOS_ASSERT(NULL != assert_constraint_list);
	SPQOS_ASSERT(EdxlopScalarAssertConstraintList ==
				assert_constraint_list->GetOperator()->GetDXLOperator());

	List *error_msgs_list = NIL;
	const ULONG num_constraints = assert_constraint_list->Arity();

	for (ULONG ul = 0; ul < num_constraints; ul++)
	{
		CDXLNode *dxl_constraint_node = (*assert_constraint_list)[ul];
		CDXLScalarAssertConstraint *dxl_constraint_op =
			CDXLScalarAssertConstraint::Cast(
				dxl_constraint_node->GetOperator());
		CWStringBase *error_msg = dxl_constraint_op->GetErrorMsgStr();
		error_msgs_list = spqdb::LAppend(
			error_msgs_list,
			spqdb::MakeStringValue(
				CreateMultiByteCharStringFromWCString(error_msg->GetBuffer())));
	}

	return error_msgs_list;
}

//---------------------------------------------------------------------------
//	@function:
//		CTranslatorUtils::GetNumNonSystemColumns
//
//	@doc:
//		Return the count of non-system columns in the relation
//
//---------------------------------------------------------------------------
ULONG
CTranslatorUtils::GetNumNonSystemColumns(const IMDRelation *rel)
{
	SPQOS_ASSERT(NULL != rel);

	ULONG num_non_system_cols = 0;

	const ULONG num_cols = rel->ColumnCount();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		const IMDColumn *md_col = rel->GetMdCol(ul);

		if (!md_col->IsSystemColumn())
		{
			num_non_system_cols++;
		}
	}

	return num_non_system_cols;
}

EdxlAggrefKind
CTranslatorUtils::GetAggKind(CHAR aggkind)
{
	switch (aggkind)
	{
		case 'n':
		{
			return EdxlaggkindNormal;
		}
		case 'o':
		{
			return EdxlaggkindOrderedSet;
		}
		case 'h':
		{
			return EdxlaggkindHypothetical;
		}
		default:
		{
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiExpr2DXLAttributeNotFound,
					   SPQOS_WSZ_LIT("Unknown aggkind value"));
		}
	}
}

CHAR
CTranslatorUtils::GetAggKind(EdxlAggrefKind aggkind)
{
	switch (aggkind)
	{
		case EdxlaggkindNormal:
		{
			return 'n';
		}
		case EdxlaggkindOrderedSet:
		{
			return 'o';
		}
		case EdxlaggkindHypothetical:
		{
			return 'h';
		}
		default:
		{
			SPQOS_RAISE(spqdxl::ExmaDXL, spqdxl::ExmiDXL2ExprAttributeNotFound,
					   SPQOS_WSZ_LIT("Unknown aggkind value"));
		}
	}
}

//---------------------------------------------------------------------------
// CTranslatorUtils::IsCompositeConst
// Check if const func returns composite type
//---------------------------------------------------------------------------
BOOL
CTranslatorUtils::IsCompositeConst(CMemoryPool *mp, CMDAccessor *md_accessor,
								   const RangeTblFunction *rtfunc)
{
	if (!IsA(rtfunc->funcexpr, Const))
		return false;

	Const *constExpr = (Const *) rtfunc->funcexpr;

	CMDIdSPQDB *mdid_return_type =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, constExpr->consttype);

	const IMDType *type = md_accessor->RetrieveType(mdid_return_type);

	mdid_return_type->Release();

	return type->IsComposite();
}

// EOF
