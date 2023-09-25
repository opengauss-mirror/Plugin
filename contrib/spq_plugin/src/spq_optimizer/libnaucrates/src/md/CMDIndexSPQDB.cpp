//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDIndexSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing metadata indexes
//---------------------------------------------------------------------------


#include "naucrates/md/CMDIndexSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/exception.h"
#include "naucrates/md/CMDRelationSPQDB.h"
#include "naucrates/md/IMDPartConstraint.h"
#include "naucrates/md/IMDScalarOp.h"

using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::CMDIndexSPQDB
//
//	@doc:
//		Constructor
//
//---------------------------------------------------------------------------
CMDIndexSPQDB::CMDIndexSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
						   BOOL is_clustered, IMDIndex::EmdindexType index_type,
						   IMDIndex::EmdindexType index_physical_type,
						   IMDId *mdid_item_type,
						   ULongPtrArray *index_key_cols_array,
						   ULongPtrArray *included_cols_array,
						   IMdIdArray *mdid_opfamilies_array,
						   IMDPartConstraint *mdpart_constraint)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_clustered(is_clustered),
	  m_index_type(index_type),
	  m_index_physical_type(index_physical_type),
	  m_mdid_item_type(mdid_item_type),
	  m_index_key_cols_array(index_key_cols_array),
	  m_included_cols_array(included_cols_array),
	  m_mdid_opfamilies_array(mdid_opfamilies_array),
	  m_mdpart_constraint(mdpart_constraint)
{
	SPQOS_ASSERT(mdid->IsValid());
	SPQOS_ASSERT(IMDIndex::EmdindSentinel > index_type);
	SPQOS_ASSERT(NULL != index_key_cols_array);
	SPQOS_ASSERT(0 < index_key_cols_array->Size());
	SPQOS_ASSERT(NULL != included_cols_array);
	SPQOS_ASSERT_IMP(NULL != mdid_item_type,
					IMDIndex::EmdindBitmap == index_type ||
						IMDIndex::EmdindBtree == index_type ||
						IMDIndex::EmdindGist == index_type ||
						IMDIndex::EmdindGin == index_type);
	SPQOS_ASSERT_IMP(IMDIndex::EmdindBitmap == index_type,
					NULL != mdid_item_type && mdid_item_type->IsValid());
	SPQOS_ASSERT(NULL != mdid_opfamilies_array);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::~CMDIndexSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDIndexSPQDB::~CMDIndexSPQDB()
{
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
	m_mdid->Release();
	CRefCount::SafeRelease(m_mdid_item_type);
	m_index_key_cols_array->Release();
	m_included_cols_array->Release();
	m_mdid_opfamilies_array->Release();
	CRefCount::SafeRelease(m_mdpart_constraint);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this index
//
//---------------------------------------------------------------------------
IMDId *
CMDIndexSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::Mdname
//
//	@doc:
//		Returns the name of this index
//
//---------------------------------------------------------------------------
CMDName
CMDIndexSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::IsClustered
//
//	@doc:
//		Is the index clustered
//
//---------------------------------------------------------------------------
BOOL
CMDIndexSPQDB::IsClustered() const
{
	return m_clustered;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::IndexType
//
//	@doc:
//		Index type
//
//---------------------------------------------------------------------------
IMDIndex::EmdindexType
CMDIndexSPQDB::IndexType() const
{
	return m_index_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::IndexPhysicalType
//
//	@doc:
//		Index physical type
//
//---------------------------------------------------------------------------
IMDIndex::EmdindexType
CMDIndexSPQDB::IndexPhysicalType() const
{
	return m_index_physical_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::Keys
//
//	@doc:
//		Returns the number of index keys
//
//---------------------------------------------------------------------------
ULONG
CMDIndexSPQDB::Keys() const
{
	return m_index_key_cols_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::KeyAt
//
//	@doc:
//		Returns the n-th key column
//
//---------------------------------------------------------------------------
ULONG
CMDIndexSPQDB::KeyAt(ULONG pos) const
{
	return *((*m_index_key_cols_array)[pos]);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::GetKeyPos
//
//	@doc:
//		Return the position of the key column in the index
//
//---------------------------------------------------------------------------
ULONG
CMDIndexSPQDB::GetKeyPos(ULONG column) const
{
	const ULONG size = Keys();

	for (ULONG ul = 0; ul < size; ul++)
	{
		if (KeyAt(ul) == column)
		{
			return ul;
		}
	}

	return spqos::ulong_max;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::IncludedCols
//
//	@doc:
//		Returns the number of included columns
//
//---------------------------------------------------------------------------
ULONG
CMDIndexSPQDB::IncludedCols() const
{
	return m_included_cols_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::IncludedColAt
//
//	@doc:
//		Returns the n-th included column
//
//---------------------------------------------------------------------------
ULONG
CMDIndexSPQDB::IncludedColAt(ULONG pos) const
{
	return *((*m_included_cols_array)[pos]);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::GetIncludedColPos
//
//	@doc:
//		Return the position of the included column in the index
//
//---------------------------------------------------------------------------
ULONG
CMDIndexSPQDB::GetIncludedColPos(ULONG column) const
{
	const ULONG size = IncludedCols();

	for (ULONG ul = 0; ul < size; ul++)
	{
		if (IncludedColAt(ul) == column)
		{
			return ul;
		}
	}

	SPQOS_ASSERT("Column not found in Index's included columns");

	return spqos::ulong_max;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::MDPartConstraint
//
//	@doc:
//		Return the part constraint
//
//---------------------------------------------------------------------------
IMDPartConstraint *
CMDIndexSPQDB::MDPartConstraint() const
{
	return m_mdpart_constraint;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::Serialize
//
//	@doc:
//		Serialize MD index in DXL format
//
//---------------------------------------------------------------------------
void
CMDIndexSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndex));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexClustered), m_clustered);

	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenIndexType),
								 GetDXLStr(m_index_type));
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexPhysicalType),
		GetDXLStr(m_index_physical_type));
	if (NULL != m_mdid_item_type)
	{
		m_mdid_item_type->Serialize(
			xml_serializer, CDXLTokens::GetDXLTokenStr(EdxltokenIndexItemType));
	}

	// serialize index keys
	CWStringDynamic *index_key_cols_str =
		CDXLUtils::Serialize(m_mp, m_index_key_cols_array);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexKeyCols), index_key_cols_str);
	SPQOS_DELETE(index_key_cols_str);

	CWStringDynamic *available_cols_str =
		CDXLUtils::Serialize(m_mp, m_included_cols_array);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexIncludedCols),
		available_cols_str);
	SPQOS_DELETE(available_cols_str);

	// serialize operator class information
	SerializeMDIdList(xml_serializer, m_mdid_opfamilies_array,
					  CDXLTokens::GetDXLTokenStr(EdxltokenOpfamilies),
					  CDXLTokens::GetDXLTokenStr(EdxltokenOpfamily));

	if (NULL != m_mdpart_constraint)
	{
		m_mdpart_constraint->Serialize(xml_serializer);
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndex));
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::DebugPrint
//
//	@doc:
//		Prints a MD index to the provided output
//
//---------------------------------------------------------------------------
void
CMDIndexSPQDB::DebugPrint(IOstream &os) const
{
	os << "Index id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Index name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;
	os << "Index type: " << GetDXLStr(m_index_type)->GetBuffer() << std::endl;
	os << "Physical Index type: "
	   << GetDXLStr(m_index_physical_type)->GetBuffer() << std::endl;

	os << "Index keys: ";
	for (ULONG ul = 0; ul < Keys(); ul++)
	{
		ULONG ulKey = KeyAt(ul);
		if (ul > 0)
		{
			os << ", ";
		}
		os << ulKey;
	}
	os << std::endl;

	os << "Included Columns: ";
	for (ULONG ul = 0; ul < IncludedCols(); ul++)
	{
		ULONG ulKey = IncludedColAt(ul);
		if (ul > 0)
		{
			os << ", ";
		}
		os << ulKey;
	}
	os << std::endl;
}

#endif	// SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::MdidType
//
//	@doc:
//		Type of items returned by the index
//
//---------------------------------------------------------------------------
IMDId *
CMDIndexSPQDB::GetIndexRetItemTypeMdid() const
{
	return m_mdid_item_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIndexSPQDB::IsCompatible
//
//	@doc:
//		Check if given scalar comparison can be used with the index key
// 		at the specified position
//
//---------------------------------------------------------------------------
BOOL
CMDIndexSPQDB::IsCompatible(const IMDScalarOp *md_scalar_op, ULONG key_pos) const
{
	SPQOS_ASSERT(NULL != md_scalar_op);
	SPQOS_ASSERT(key_pos < m_mdid_opfamilies_array->Size());

	// check if the index opfamily for the key at the given position is one of
	// the families the scalar comparison belongs to
	const IMDId *mdid_opfamily = (*m_mdid_opfamilies_array)[key_pos];

	const ULONG opfamilies_count = md_scalar_op->OpfamiliesCount();

	for (ULONG ul = 0; ul < opfamilies_count; ul++)
	{
		if (mdid_opfamily->Equals(md_scalar_op->OpfamilyMdidAt(ul)))
		{
			return true;
		}
	}

	return false;
}

// EOF
