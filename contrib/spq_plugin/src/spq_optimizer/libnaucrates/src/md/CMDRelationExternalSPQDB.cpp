//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDRelationExternalSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing MD cache external relations
//---------------------------------------------------------------------------

#include "naucrates/md/CMDRelationExternalSPQDB.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::CMDRelationExternalSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDRelationExternalSPQDB::CMDRelationExternalSPQDB(
	CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
	Ereldistrpolicy rel_distr_policy, CMDColumnArray *mdcol_array,
	ULongPtrArray *distr_col_array, IMdIdArray *distr_opfamilies,
	BOOL convert_hash_to_random, ULongPtr2dArray *keyset_array,
	CMDIndexInfoArray *md_index_info_array, IMdIdArray *mdid_triggers_array,
	IMdIdArray *mdid_check_constraint_array,
	IMDPartConstraint *mdpart_constraint, INT reject_limit,
	BOOL is_reject_limit_in_rows, IMDId *mdid_fmt_err_table)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_rel_distr_policy(rel_distr_policy),
	  m_md_col_array(mdcol_array),
	  m_dropped_cols(0),
	  m_distr_col_array(distr_col_array),
	  m_distr_opfamilies(distr_opfamilies),
	  m_convert_hash_to_random(convert_hash_to_random),
	  m_keyset_array(keyset_array),
	  m_mdindex_info_array(md_index_info_array),
	  m_mdid_trigger_array(mdid_triggers_array),
	  m_mdid_check_constraint_array(mdid_check_constraint_array),
	  m_mdpart_constraint(mdpart_constraint),
	  m_reject_limit(reject_limit),
	  m_is_rej_limit_in_rows(is_reject_limit_in_rows),
	  m_mdid_fmt_err_table(mdid_fmt_err_table),
	  m_system_columns(0),
	  m_colpos_nondrop_colpos_map(NULL),
	  m_attrno_nondrop_col_pos_map(NULL),
	  m_nondrop_col_pos_array(NULL)
{
	SPQOS_ASSERT(mdid->IsValid());
	SPQOS_ASSERT(NULL != mdcol_array);
	SPQOS_ASSERT(NULL != md_index_info_array);
	SPQOS_ASSERT(NULL != mdid_triggers_array);
	SPQOS_ASSERT(NULL != mdid_check_constraint_array);
	SPQOS_ASSERT_IMP(
		convert_hash_to_random,
		IMDRelation::EreldistrHash == rel_distr_policy &&
			"Converting hash distributed table to random only possible for hash distributed tables");
	SPQOS_ASSERT(NULL == distr_opfamilies ||
				distr_opfamilies->Size() == m_distr_col_array->Size());

	m_colpos_nondrop_colpos_map = SPQOS_NEW(m_mp) UlongToUlongMap(m_mp);
	m_attrno_nondrop_col_pos_map = SPQOS_NEW(m_mp) IntToUlongMap(m_mp);
	m_nondrop_col_pos_array = SPQOS_NEW(m_mp) ULongPtrArray(m_mp);
	m_col_width_array = SPQOS_NEW(mp) CDoubleArray(mp);

	ULONG ulPosNonDropped = 0;
	const ULONG arity = mdcol_array->Size();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		IMDColumn *pmdcol = (*mdcol_array)[ul];

		BOOL isSystemCol = pmdcol->IsSystemColumn();
		if (isSystemCol)
		{
			m_system_columns++;
		}

		if (pmdcol->IsDropped())
		{
			m_dropped_cols++;
		}
		else
		{
			if (!isSystemCol)
			{
				m_nondrop_col_pos_array->Append(SPQOS_NEW(m_mp) ULONG(ul));
			}

			(void) m_colpos_nondrop_colpos_map->Insert(
				SPQOS_NEW(m_mp) ULONG(ul),
				SPQOS_NEW(m_mp) ULONG(ulPosNonDropped));
			ulPosNonDropped++;
		}

		(void) m_attrno_nondrop_col_pos_map->Insert(
			SPQOS_NEW(m_mp) INT(pmdcol->AttrNum()), SPQOS_NEW(m_mp) ULONG(ul));
		m_col_width_array->Append(SPQOS_NEW(mp) CDouble(pmdcol->Length()));
	}
	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::~CMDRelationExternalSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDRelationExternalSPQDB::~CMDRelationExternalSPQDB()
{
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
	m_mdid->Release();
	m_md_col_array->Release();
	CRefCount::SafeRelease(m_distr_col_array);
	CRefCount::SafeRelease(m_distr_opfamilies);
	CRefCount::SafeRelease(m_keyset_array);
	m_mdindex_info_array->Release();
	m_mdid_trigger_array->Release();
	m_col_width_array->Release();
	m_mdid_check_constraint_array->Release();
	CRefCount::SafeRelease(m_mdid_fmt_err_table);

	CRefCount::SafeRelease(m_mdpart_constraint);
	CRefCount::SafeRelease(m_colpos_nondrop_colpos_map);
	CRefCount::SafeRelease(m_attrno_nondrop_col_pos_map);
	CRefCount::SafeRelease(m_nondrop_col_pos_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this relation
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationExternalSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::Mdname
//
//	@doc:
//		Returns the name of this relation
//
//---------------------------------------------------------------------------
CMDName
CMDRelationExternalSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::GetRelDistribution
//
//	@doc:
//		Returns the distribution policy for this relation
//
//---------------------------------------------------------------------------
IMDRelation::Ereldistrpolicy
CMDRelationExternalSPQDB::GetRelDistribution() const
{
	return m_rel_distr_policy;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::ColumnCount
//
//	@doc:
//		Returns the number of columns of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::ColumnCount() const
{
	SPQOS_ASSERT(NULL != m_md_col_array);

	return m_md_col_array->Size();
}

// Return the width of a column with regards to the position
DOUBLE
CMDRelationExternalSPQDB::ColWidth(ULONG pos) const
{
	return (*m_col_width_array)[pos]->Get();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::HasDroppedColumns
//
//	@doc:
//		Does relation have dropped columns
//
//---------------------------------------------------------------------------
BOOL
CMDRelationExternalSPQDB::HasDroppedColumns() const
{
	return 0 < m_dropped_cols;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::NonDroppedColsCount
//
//	@doc:
//		Number of non-dropped columns
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::NonDroppedColsCount() const
{
	return ColumnCount() - m_dropped_cols;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::SystemColumnsCount
//
//	@doc:
//		Returns the number of system columns of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::SystemColumnsCount() const
{
	return m_system_columns;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::NonDroppedColsArray
//
//	@doc:
//		Returns the original positions of all the non-dropped columns
//
//---------------------------------------------------------------------------
ULongPtrArray *
CMDRelationExternalSPQDB::NonDroppedColsArray() const
{
	return m_nondrop_col_pos_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::NonDroppedColAt
//
//	@doc:
//		Return the absolute position of the given attribute position excluding
//		dropped columns
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::NonDroppedColAt(ULONG pos) const
{
	SPQOS_ASSERT(pos <= ColumnCount());

	if (!HasDroppedColumns())
	{
		return pos;
	}

	ULONG *pul = m_colpos_nondrop_colpos_map->Find(&pos);

	SPQOS_ASSERT(NULL != pul);
	return *pul;
}

IMDId *
CMDRelationExternalSPQDB::GetDistrOpfamilyAt(ULONG pos) const
{
	if (m_distr_opfamilies == NULL)
	{
		SPQOS_RAISE(CException::ExmaInvalid, CException::ExmiInvalid,
				   SPQOS_WSZ_LIT("GetDistrOpfamilyAt() returning NULL."));
	}

	SPQOS_ASSERT(pos < m_distr_opfamilies->Size());
	return (*m_distr_opfamilies)[pos];
}
//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::GetPosFromAttno
//
//	@doc:
//		Return the position of a column in the metadata object given the
//		attribute number in the system catalog
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::GetPosFromAttno(INT attno) const
{
	ULONG *pul = m_attrno_nondrop_col_pos_map->Find(&attno);
	SPQOS_ASSERT(NULL != pul);

	return *pul;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::ConvertHashToRandom
//
//	@doc:
//		Return true if a hash distributed table needs to be considered as random during planning
//---------------------------------------------------------------------------
BOOL
CMDRelationExternalSPQDB::ConvertHashToRandom() const
{
	return m_convert_hash_to_random;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::RejectLimit
//
//	@doc:
//		Reject limit
//
//---------------------------------------------------------------------------
INT
CMDRelationExternalSPQDB::RejectLimit() const
{
	return m_reject_limit;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::IsRejectLimitInRows
//
//	@doc:
//		Is the reject limit in rows?
//
//---------------------------------------------------------------------------
BOOL
CMDRelationExternalSPQDB::IsRejectLimitInRows() const
{
	return m_is_rej_limit_in_rows;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::GetFormatErrTableMdid
//
//	@doc:
//		Format error table mdid
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationExternalSPQDB::GetFormatErrTableMdid() const
{
	return m_mdid_fmt_err_table;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::KeySetCount
//
//	@doc:
//		Returns the number of key sets
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::KeySetCount() const
{
	return (m_keyset_array == NULL) ? 0 : m_keyset_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::KeySetAt
//
//	@doc:
//		Returns the key set at the specified position
//
//---------------------------------------------------------------------------
const ULongPtrArray *
CMDRelationExternalSPQDB::KeySetAt(ULONG pos) const
{
	SPQOS_ASSERT(NULL != m_keyset_array);

	return (*m_keyset_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::DistrColumnCount
//
//	@doc:
//		Returns the number of columns in the distribution column list of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::DistrColumnCount() const
{
	return (m_distr_col_array == NULL) ? 0 : m_distr_col_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::IndexCount
//
//	@doc:
//		Returns the number of indices of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::IndexCount() const
{
	return m_mdindex_info_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::TriggerCount
//
//	@doc:
//		Returns the number of triggers of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::TriggerCount() const
{
	return m_mdid_trigger_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::GetMdCol
//
//	@doc:
//		Returns the column at the specified position
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationExternalSPQDB::GetMdCol(ULONG pos) const
{
	SPQOS_ASSERT(pos < m_md_col_array->Size());

	return (*m_md_col_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::GetDistrColAt
//
//	@doc:
//		Returns the distribution column at the specified position in the distribution column list
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationExternalSPQDB::GetDistrColAt(ULONG pos) const
{
	SPQOS_ASSERT(pos < m_distr_col_array->Size());

	ULONG ulDistrKeyPos = (*(*m_distr_col_array)[pos]);
	return GetMdCol(ulDistrKeyPos);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::IndexMDidAt
//
//	@doc:
//		Returns the id of the index at the specified position of the index array
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationExternalSPQDB::IndexMDidAt(ULONG pos) const
{
	return (*m_mdindex_info_array)[pos]->MDId();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::TriggerMDidAt
//
//	@doc:
//		Returns the id of the trigger at the specified position of the trigger array
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationExternalSPQDB::TriggerMDidAt(ULONG pos) const
{
	return (*m_mdid_trigger_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::CheckConstraintCount
//
//	@doc:
//		Returns the number of check constraints on this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationExternalSPQDB::CheckConstraintCount() const
{
	return m_mdid_check_constraint_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::CheckConstraintMDidAt
//
//	@doc:
//		Returns the id of the check constraint at the specified position of
//		the check constraint array
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationExternalSPQDB::CheckConstraintMDidAt(ULONG pos) const
{
	return (*m_mdid_check_constraint_array)[pos];
}

IMDPartConstraint *
CMDRelationExternalSPQDB::MDPartConstraint() const
{
	return m_mdpart_constraint;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDRelationExternalSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenRelationExternal));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrPolicy),
		GetDistrPolicyStr(m_rel_distr_policy));

	if (EreldistrHash == m_rel_distr_policy)
	{
		SPQOS_ASSERT(NULL != m_distr_col_array);

		// serialize distribution columns
		CWStringDynamic *pstrDistrColumns =
			ColumnsToStr(m_mp, m_distr_col_array);
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenDistrColumns),
			pstrDistrColumns);
		SPQOS_DELETE(pstrDistrColumns);
	}

	// serialize key sets
	if (m_keyset_array != NULL && 0 < m_keyset_array->Size())
	{
		CWStringDynamic *pstrKeys = CDXLUtils::Serialize(m_mp, m_keyset_array);
		xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenKeys),
									 pstrKeys);
		SPQOS_DELETE(pstrKeys);
	}

	if (0 <= m_reject_limit)
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenExtRelRejLimit),
			m_reject_limit);
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenExtRelRejLimitInRows),
			m_is_rej_limit_in_rows);
	}

	if (NULL != m_mdid_fmt_err_table)
	{
		m_mdid_fmt_err_table->Serialize(
			xml_serializer,
			CDXLTokens::GetDXLTokenStr(EdxltokenExtRelFmtErrRel));
	}

	if (m_convert_hash_to_random)
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenConvertHashToRandom),
			m_convert_hash_to_random);
	}

	// serialize columns
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenColumns));
	for (ULONG ul = 0; ul < m_md_col_array->Size(); ul++)
	{
		CMDColumn *pmdcol = (*m_md_col_array)[ul];
		pmdcol->Serialize(xml_serializer);
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenColumns));

	// serialize index infos
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexInfoList));
	const ULONG ulIndexes = m_mdindex_info_array->Size();
	for (ULONG ul = 0; ul < ulIndexes; ul++)
	{
		CMDIndexInfo *pmdIndexInfo = (*m_mdindex_info_array)[ul];
		pmdIndexInfo->Serialize(xml_serializer);

		SPQOS_CHECK_ABORT;
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexInfoList));

	// serialize trigger information
	SerializeMDIdList(xml_serializer, m_mdid_trigger_array,
					  CDXLTokens::GetDXLTokenStr(EdxltokenTriggers),
					  CDXLTokens::GetDXLTokenStr(EdxltokenTrigger));

	// serialize check constraint information
	SerializeMDIdList(xml_serializer, m_mdid_check_constraint_array,
					  CDXLTokens::GetDXLTokenStr(EdxltokenCheckConstraints),
					  CDXLTokens::GetDXLTokenStr(EdxltokenCheckConstraint));

	// serialize operator class information, if present
	if (EreldistrHash == m_rel_distr_policy && NULL != m_distr_opfamilies)
	{
		SerializeMDIdList(
			xml_serializer, m_distr_opfamilies,
			CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpfamilies),
			CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpfamily));
	}

	// serialize part constraint
	if (NULL != m_mdpart_constraint)
	{
		m_mdpart_constraint->Serialize(xml_serializer);
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenRelationExternal));
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDRelationExternalSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDRelationExternalSPQDB::DebugPrint(IOstream &os) const
{
	os << "External Relation id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Relation name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;

	os << "Distribution policy: "
	   << GetDistrPolicyStr(m_rel_distr_policy)->GetBuffer() << std::endl;

	os << "Relation columns: " << std::endl;
	const ULONG ulColumns = ColumnCount();
	for (ULONG ul = 0; ul < ulColumns; ul++)
	{
		const IMDColumn *pimdcol = GetMdCol(ul);
		pimdcol->DebugPrint(os);
	}
	os << std::endl;

	os << "Distributed by: ";
	const ULONG ulDistrColumns = DistrColumnCount();
	for (ULONG ul = 0; ul < ulDistrColumns; ul++)
	{
		if (0 < ul)
		{
			os << ", ";
		}

		const IMDColumn *pimdcolDistrKey = GetDistrColAt(ul);
		os << (pimdcolDistrKey->Mdname()).GetMDName()->GetBuffer();
	}

	os << std::endl;

	os << "Index Info: ";
	const ULONG ulIndexes = m_mdindex_info_array->Size();
	for (ULONG ul = 0; ul < ulIndexes; ul++)
	{
		CMDIndexInfo *pmdIndexInfo = (*m_mdindex_info_array)[ul];
		pmdIndexInfo->DebugPrint(os);
	}

	os << "Triggers: ";
	CDXLUtils::DebugPrintMDIdArray(os, m_mdid_trigger_array);

	os << "Check Constraint: ";
	CDXLUtils::DebugPrintMDIdArray(os, m_mdid_check_constraint_array);

	os << "Reject limit: " << m_reject_limit;
	if (m_is_rej_limit_in_rows)
	{
		os << " Rows" << std::endl;
	}
	else
	{
		os << " Percent" << std::endl;
	}

	if (NULL != GetFormatErrTableMdid())
	{
		os << "Format Error Table: ";
		GetFormatErrTableMdid()->OsPrint(os);
		os << std::endl;
	}
}

#endif	// SPQOS_DEBUG

// EOF
