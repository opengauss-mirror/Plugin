//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMappingVarColId.h
//
//	@doc:
//		Abstract base class of VAR to DXL mapping during scalar operator translation.
//		If we need a scalar translator during PlStmt->DXL or Query->DXL translation
//		we implement a variable mapping for PlStmt or Query respectively that
//		is derived from this interface.
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CMappingVarColId_H
#define SPQDXL_CMappingVarColId_H


#include "spqos/common/CHashMap.h"
#include "spqos/common/CHashMapIter.h"

#include "spq_optimizer_util/translate/SPQDBAttInfo.h"
#include "spq_optimizer_util/translate/SPQDBAttOptCol.h"
#include "naucrates/dxl/CIdGenerator.h"
#include "naucrates/dxl/operators/dxlops.h"

//fwd decl
struct Var;
struct List;

namespace spqmd
{
class IMDIndex;
}

namespace spqdxl
{
// physical operator types used in planned statements
enum EPlStmtPhysicalOpType
{
	EpspotTblScan,
	EpspotHashjoin,
	EpspotNLJoin,
	EpspotMergeJoin,
	EpspotMotion,
	EpspotLimit,
	EpspotAgg,
	EpspotWindow,
	EpspotSort,
	EpspotSubqueryScan,
	EpspotAppend,
	EpspotResult,
	EpspotMaterialize,
	EpspotSharedScan,
	EpspotIndexScan,
	EpspotIndexOnlyScan,
	EpspotNone
};

//---------------------------------------------------------------------------
//	@class:
//		CMappingVarColId
//
//	@doc:
//		Class providing the interface for VAR to DXL mapping during scalar
//		operator translation that are used to generate the CDXLNode from a
//		variable reference in a PlStmt or Query
//
//---------------------------------------------------------------------------
class CMappingVarColId
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// hash map structure to store spqdb att -> opt col information
	typedef CHashMap<SPQDBAttInfo, SPQDBAttOptCol, HashSPQDBAttInfo,
					 EqualSPQDBAttInfo, CleanupRelease, CleanupRelease>
		SPQDBAttOptColHashMap;

	// iterator
	typedef CHashMapIter<SPQDBAttInfo, SPQDBAttOptCol, HashSPQDBAttInfo,
						 EqualSPQDBAttInfo, CleanupRelease, CleanupRelease>
		SPQDBAttOptColHashMapIter;

	// map from spqdb att to optimizer col
	SPQDBAttOptColHashMap *m_spqdb_att_opt_col_mapping;

	// insert mapping entry
	void Insert(ULONG, ULONG, INT, ULONG, CWStringBase *str);

	// no copy constructor
	CMappingVarColId(const CMappingVarColId &);

	// helper function to access mapping
	const SPQDBAttOptCol *GetSPQDBAttOptColMapping(
		ULONG current_query_level, const Var *var,
		EPlStmtPhysicalOpType plstmt_physical_op_type) const;

public:
	// ctor
	explicit CMappingVarColId(CMemoryPool *);

	// dtor
	virtual ~CMappingVarColId()
	{
		m_spqdb_att_opt_col_mapping->Release();
	}

	// given a spqdb attribute, return a column name in optimizer world
	virtual const CWStringBase *GetOptColName(
		ULONG current_query_level, const Var *var,
		EPlStmtPhysicalOpType plstmt_physical_op_type) const;

	// given a spqdb attribute, return column id
	virtual ULONG GetColId(ULONG current_query_level, const Var *var,
						   EPlStmtPhysicalOpType plstmt_physical_op_type) const;

	// load up mapping information from an index
	void LoadIndexColumns(ULONG query_level, ULONG RTE_index,
						  const IMDIndex *index,
						  const CDXLTableDescr *table_descr);

	// load up mapping information from table descriptor
	void LoadTblColumns(ULONG query_level, ULONG RTE_index,
						const CDXLTableDescr *table_descr);

	// load up column id mapping information from the array of column descriptors
	void LoadColumns(ULONG query_level, ULONG RTE_index,
					 const CDXLColDescrArray *column_descrs);

	// load up mapping information from derived table columns
	void LoadDerivedTblColumns(ULONG query_level, ULONG RTE_index,
							   const CDXLNodeArray *derived_columns_dxl,
							   List *target_list);

	// load information from CTE columns
	void LoadCTEColumns(ULONG query_level, ULONG RTE_index,
						const ULongPtrArray *pdrspqulCTE, List *target_list);

	// load up mapping information from scalar projection list
	void LoadProjectElements(ULONG query_level, ULONG RTE_index,
							 const CDXLNode *project_list_dxlnode);

	// load up mapping information from list of column names
	void Load(ULONG query_level, ULONG RTE_index, CIdGenerator *id_generator,
			  List *col_names);

	// create a deep copy
	CMappingVarColId *CopyMapColId(CMemoryPool *mp) const;

	// create a deep copy
	CMappingVarColId *CopyMapColId(ULONG query_level) const;

	// create a copy of the mapping replacing old col ids with new ones
	CMappingVarColId *CopyRemapColId(CMemoryPool *mp, ULongPtrArray *old_colids,
									 ULongPtrArray *new_colids) const;
};
}  // namespace spqdxl

#endif	//SPQDXL_CMappingVarColId_H

// EOF
