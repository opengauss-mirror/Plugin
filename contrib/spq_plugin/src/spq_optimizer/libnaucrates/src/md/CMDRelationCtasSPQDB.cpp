//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CMDRelationCtasSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing MD cache CTAS relations
//---------------------------------------------------------------------------

#include "naucrates/md/CMDRelationCtasSPQDB.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLCtasStorageOptions.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::CMDRelationCtasSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDRelationCtasSPQDB::CMDRelationCtasSPQDB(
	CMemoryPool *mp, IMDId *mdid, CMDName *mdname_schema, CMDName *mdname,
	BOOL fTemporary, BOOL fHasOids, Erelstoragetype rel_storage_type,
	Ereldistrpolicy rel_distr_policy, CMDColumnArray *mdcol_array,
	ULongPtrArray *distr_col_array, IMdIdArray *distr_opfamiles,
	IMdIdArray *distr_opclasses, ULongPtr2dArray *keyset_array,
	CDXLCtasStorageOptions *dxl_ctas_storage_options,
	IntPtrArray *vartypemod_array)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname_schema(mdname_schema),
	  m_mdname(mdname),
	  m_is_temp_table(fTemporary),
	  m_has_oids(fHasOids),
	  m_rel_storage_type(rel_storage_type),
	  m_rel_distr_policy(rel_distr_policy),
	  m_md_col_array(mdcol_array),
	  m_distr_col_array(distr_col_array),
	  m_distr_opfamilies(distr_opfamiles),
	  m_distr_opclasses(distr_opclasses),
	  m_keyset_array(keyset_array),
	  m_system_columns(0),
	  m_nondrop_col_pos_array(NULL),
	  m_dxl_ctas_storage_option(dxl_ctas_storage_options),
	  m_vartypemod_array(vartypemod_array)
{
	SPQOS_ASSERT(mdid->IsValid());
	SPQOS_ASSERT(NULL != mdcol_array);
	SPQOS_ASSERT(NULL != dxl_ctas_storage_options);
	SPQOS_ASSERT(IMDRelation::ErelstorageSentinel > m_rel_storage_type);
	SPQOS_ASSERT(0 == keyset_array->Size());
	SPQOS_ASSERT(NULL != vartypemod_array);

	m_attrno_nondrop_col_pos_map = SPQOS_NEW(m_mp) IntToUlongMap(m_mp);
	m_nondrop_col_pos_array = SPQOS_NEW(m_mp) ULongPtrArray(m_mp);
	m_col_width_array = SPQOS_NEW(mp) CDoubleArray(mp);

	const ULONG arity = mdcol_array->Size();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		IMDColumn *mdcol = (*mdcol_array)[ul];
		SPQOS_ASSERT(!mdcol->IsDropped() &&
					"Cannot create a table with dropped columns");

		BOOL is_system_col = mdcol->IsSystemColumn();
		if (is_system_col)
		{
			m_system_columns++;
		}
		else
		{
			m_nondrop_col_pos_array->Append(SPQOS_NEW(m_mp) ULONG(ul));
		}

		(void) m_attrno_nondrop_col_pos_map->Insert(
			SPQOS_NEW(m_mp) INT(mdcol->AttrNum()), SPQOS_NEW(m_mp) ULONG(ul));

		m_col_width_array->Append(SPQOS_NEW(mp) CDouble(mdcol->Length()));
	}
	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::~CMDRelationCtasSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDRelationCtasSPQDB::~CMDRelationCtasSPQDB()
{
	SPQOS_DELETE(m_mdname_schema);
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
	m_mdid->Release();
	m_md_col_array->Release();
	m_keyset_array->Release();
	m_col_width_array->Release();
	CRefCount::SafeRelease(m_distr_col_array);
	CRefCount::SafeRelease(m_attrno_nondrop_col_pos_map);
	CRefCount::SafeRelease(m_nondrop_col_pos_array);
	m_dxl_ctas_storage_option->Release();
	m_vartypemod_array->Release();
	m_distr_opfamilies->Release();
	m_distr_opclasses->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this relation
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationCtasSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::Mdname
//
//	@doc:
//		Returns the name of this relation
//
//---------------------------------------------------------------------------
CMDName
CMDRelationCtasSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::GetMdNameSchema
//
//	@doc:
//		Returns schema name
//
//---------------------------------------------------------------------------
CMDName *
CMDRelationCtasSPQDB::GetMdNameSchema() const
{
	return m_mdname_schema;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::GetRelDistribution
//
//	@doc:
//		Returns the distribution policy for this relation
//
//---------------------------------------------------------------------------
IMDRelation::Ereldistrpolicy
CMDRelationCtasSPQDB::GetRelDistribution() const
{
	return m_rel_distr_policy;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::ColumnCount
//
//	@doc:
//		Returns the number of columns of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationCtasSPQDB::ColumnCount() const
{
	SPQOS_ASSERT(NULL != m_md_col_array);

	return m_md_col_array->Size();
}

// Return the width of a column with regards to the position
DOUBLE
CMDRelationCtasSPQDB::ColWidth(ULONG pos) const
{
	return (*m_col_width_array)[pos]->Get();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::SystemColumnsCount
//
//	@doc:
//		Returns the number of system columns of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationCtasSPQDB::SystemColumnsCount() const
{
	return m_system_columns;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::GetPosFromAttno
//
//	@doc:
//		Return the position of a column in the metadata object given the
//		attribute number in the system catalog
//---------------------------------------------------------------------------
ULONG
CMDRelationCtasSPQDB::GetPosFromAttno(INT attno) const
{
	ULONG *att_pos = m_attrno_nondrop_col_pos_map->Find(&attno);
	SPQOS_ASSERT(NULL != att_pos);

	return *att_pos;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::DistrColumnCount
//
//	@doc:
//		Returns the number of columns in the distribution column list of this relation
//
//---------------------------------------------------------------------------
ULONG
CMDRelationCtasSPQDB::DistrColumnCount() const
{
	return (m_distr_col_array == NULL) ? 0 : m_distr_col_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::GetMdCol
//
//	@doc:
//		Returns the column at the specified position
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationCtasSPQDB::GetMdCol(ULONG pos) const
{
	SPQOS_ASSERT(pos < m_md_col_array->Size());

	return (*m_md_col_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::GetDistrColAt
//
//	@doc:
//		Returns the distribution column at the specified position in the distribution column list
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationCtasSPQDB::GetDistrColAt(ULONG pos) const
{
	SPQOS_ASSERT(pos < m_distr_col_array->Size());

	ULONG distr_key_pos = (*(*m_distr_col_array)[pos]);
	return GetMdCol(distr_key_pos);
}

IMDId *
CMDRelationCtasSPQDB::GetDistrOpfamilyAt(ULONG pos) const
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
//		CMDRelationCtasSPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDRelationCtasSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenRelationCTAS));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	if (NULL != m_mdname_schema)
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenSchema),
			m_mdname_schema->GetMDName());
	}
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelTemporary), m_is_temp_table);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelHasOids), m_has_oids);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelStorageType),
		IMDRelation::GetStorageTypeStr(m_rel_storage_type));

	// serialize vartypmod list
	CWStringDynamic *var_typemod_list_array =
		CDXLUtils::Serialize(m_mp, m_vartypemod_array);
	SPQOS_ASSERT(NULL != var_typemod_list_array);

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenVarTypeModList),
		var_typemod_list_array);
	SPQOS_DELETE(var_typemod_list_array);

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrPolicy),
		GetDistrPolicyStr(m_rel_distr_policy));

	if (EreldistrHash == m_rel_distr_policy)
	{
		SPQOS_ASSERT(NULL != m_distr_col_array);

		// serialize distribution columns
		CWStringDynamic *distr_col_array =
			ColumnsToStr(m_mp, m_distr_col_array);
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenDistrColumns), distr_col_array);
		SPQOS_DELETE(distr_col_array);
	}

	// serialize columns
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenColumns));
	const ULONG columns = m_md_col_array->Size();
	for (ULONG ul = 0; ul < columns; ul++)
	{
		CMDColumn *mdcol = (*m_md_col_array)[ul];
		mdcol->Serialize(xml_serializer);
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenColumns));

	m_dxl_ctas_storage_option->Serialize(xml_serializer);

	// serialize distribution opfamilies

	SerializeMDIdList(xml_serializer, m_distr_opfamilies,
					  CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpfamilies),
					  CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpfamily));

	SerializeMDIdList(xml_serializer, m_distr_opclasses,
					  CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpclasses),
					  CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpclass));


	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenRelationCTAS));
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDRelationCtasSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDRelationCtasSPQDB::DebugPrint(IOstream &os) const
{
	os << "CTAS Relation id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Relation name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;

	os << "Distribution policy: "
	   << GetDistrPolicyStr(m_rel_distr_policy)->GetBuffer() << std::endl;

	os << "Relation columns: " << std::endl;
	const ULONG total_columns = ColumnCount();
	for (ULONG ul = 0; ul < total_columns; ul++)
	{
		const IMDColumn *mdcol = GetMdCol(ul);
		mdcol->DebugPrint(os);
	}
	os << std::endl;

	os << "Distributed by: ";
	const ULONG distr_col_count = DistrColumnCount();
	for (ULONG ul = 0; ul < distr_col_count; ul++)
	{
		if (0 < ul)
		{
			os << ", ";
		}

		const IMDColumn *mdcol_distr_key = GetDistrColAt(ul);
		os << (mdcol_distr_key->Mdname()).GetMDName()->GetBuffer();
	}

	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
