//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDFunctionSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific functions
//		in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDFunctionSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqmd;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::CMDFunctionSPQDB
//
//	@doc:
//		Constructs a metadata func
//
//---------------------------------------------------------------------------
CMDFunctionSPQDB::CMDFunctionSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
								 IMDId *result_type_mdid,
								 IMdIdArray *mdid_array, BOOL ReturnsSet,
								 EFuncStbl func_stability,
								 EFuncDataAcc func_data_access, BOOL is_strict,
								 BOOL is_ndv_preserving, BOOL is_allowed_for_PS)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_mdid_type_result(result_type_mdid),
	  m_mdid_types_array(mdid_array),
	  m_returns_set(ReturnsSet),
	  m_func_stability(func_stability),
	  m_func_data_access(func_data_access),
	  m_is_strict(is_strict),
	  m_is_ndv_preserving(is_ndv_preserving),
	  m_is_allowed_for_PS(is_allowed_for_PS)
{
	SPQOS_ASSERT(m_mdid->IsValid());
	SPQOS_ASSERT(EfsSentinel > func_stability);
	SPQOS_ASSERT(EfdaSentinel > func_data_access);

	InitDXLTokenArrays();
	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::~CMDFunctionSPQDB
//
//	@doc:
//		Destructor
//
//---------------------------------------------------------------------------
CMDFunctionSPQDB::~CMDFunctionSPQDB()
{
	m_mdid->Release();
	m_mdid_type_result->Release();
	CRefCount::SafeRelease(m_mdid_types_array);
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::InitDXLTokenArrays
//
//	@doc:
//		Initialize DXL token arrays
//
//---------------------------------------------------------------------------
void
CMDFunctionSPQDB::InitDXLTokenArrays()
{
	// stability
	m_dxl_func_stability_array[EfsImmutable] = EdxltokenSPQDBFuncImmutable;
	m_dxl_func_stability_array[EfsStable] = EdxltokenSPQDBFuncStable;
	m_dxl_func_stability_array[EfsVolatile] = EdxltokenSPQDBFuncVolatile;

	// data access
	m_dxl_data_access_array[EfdaNoSQL] = EdxltokenSPQDBFuncNoSQL;
	m_dxl_data_access_array[EfdaContainsSQL] = EdxltokenSPQDBFuncContainsSQL;
	m_dxl_data_access_array[EfdaReadsSQLData] = EdxltokenSPQDBFuncReadsSQLData;
	m_dxl_data_access_array[EfdaModifiesSQLData] =
		EdxltokenSPQDBFuncModifiesSQLData;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::MDId
//
//	@doc:
//		Func id
//
//---------------------------------------------------------------------------
IMDId *
CMDFunctionSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::Mdname
//
//	@doc:
//		Func name
//
//---------------------------------------------------------------------------
CMDName
CMDFunctionSPQDB::Mdname() const
{
	return *m_mdname;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::GetResultTypeMdid
//
//	@doc:
//		Type id of result
//
//---------------------------------------------------------------------------
IMDId *
CMDFunctionSPQDB::GetResultTypeMdid() const
{
	return m_mdid_type_result;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::OutputArgTypesMdidArray
//
//	@doc:
//		Output argument types
//
//---------------------------------------------------------------------------
IMdIdArray *
CMDFunctionSPQDB::OutputArgTypesMdidArray() const
{
	return m_mdid_types_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::ReturnsSet
//
//	@doc:
//		Returns whether function result is a set
//
//---------------------------------------------------------------------------
BOOL
CMDFunctionSPQDB::ReturnsSet() const
{
	return m_returns_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::GetOutputArgTypeArrayStr
//
//	@doc:
//		Serialize the array of output argument types into a comma-separated string
//
//---------------------------------------------------------------------------
CWStringDynamic *
CMDFunctionSPQDB::GetOutputArgTypeArrayStr() const
{
	SPQOS_ASSERT(NULL != m_mdid_types_array);
	CWStringDynamic *str = SPQOS_NEW(m_mp) CWStringDynamic(m_mp);

	const ULONG len = m_mdid_types_array->Size();
	for (ULONG ul = 0; ul < len; ul++)
	{
		IMDId *mdid = (*m_mdid_types_array)[ul];
		if (ul == len - 1)
		{
			// last element: do not print a comma
			str->AppendFormat(SPQOS_WSZ_LIT("%ls"), mdid->GetBuffer());
		}
		else
		{
			str->AppendFormat(
				SPQOS_WSZ_LIT("%ls%ls"), mdid->GetBuffer(),
				CDXLTokens::GetDXLTokenStr(EdxltokenComma)->GetBuffer());
		}
	}

	return str;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::Serialize
//
//	@doc:
//		Serialize function metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDFunctionSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFunc));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));

	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncReturnsSet), m_returns_set);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncStability),
		GetFuncStabilityStr());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncDataAccess),
		GetFuncDataAccessStr());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncStrict), m_is_strict);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncNDVPreserving),
		m_is_ndv_preserving);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncIsAllowedForPS),
		m_is_allowed_for_PS);

	SerializeMDIdAsElem(
		xml_serializer,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFuncResultTypeId),
		m_mdid_type_result);

	if (NULL != m_mdid_types_array)
	{
		xml_serializer->OpenElement(
			CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
			CDXLTokens::GetDXLTokenStr(EdxltokenOutputCols));

		CWStringDynamic *output_arg_type_array_str = GetOutputArgTypeArrayStr();
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenTypeIds),
			output_arg_type_array_str);
		SPQOS_DELETE(output_arg_type_array_str);

		xml_serializer->CloseElement(
			CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
			CDXLTokens::GetDXLTokenStr(EdxltokenOutputCols));
	}
	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBFunc));
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::GetFuncStabilityStr
//
//	@doc:
//		String representation of function stability
//
//---------------------------------------------------------------------------
const CWStringConst *
CMDFunctionSPQDB::GetFuncStabilityStr() const
{
	if (EfsSentinel > m_func_stability)
	{
		return CDXLTokens::GetDXLTokenStr(
			m_dxl_func_stability_array[m_func_stability]);
	}

	SPQOS_ASSERT(!"Unrecognized function stability setting");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::GetFuncDataAccessStr
//
//	@doc:
//		String representation of function data access
//
//---------------------------------------------------------------------------
const CWStringConst *
CMDFunctionSPQDB::GetFuncDataAccessStr() const
{
	if (EfdaSentinel > m_func_data_access)
	{
		return CDXLTokens::GetDXLTokenStr(
			m_dxl_data_access_array[m_func_data_access]);
	}

	SPQOS_ASSERT(!"Unrecognized function data access setting");
	return NULL;
}

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMDFunctionSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDFunctionSPQDB::DebugPrint(IOstream &os) const
{
	os << "Function id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Function name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;

	os << "Result type id: ";
	GetResultTypeMdid()->OsPrint(os);
	os << std::endl;

	const CWStringConst *return_set_str =
		ReturnsSet() ? CDXLTokens::GetDXLTokenStr(EdxltokenTrue)
					 : CDXLTokens::GetDXLTokenStr(EdxltokenFalse);

	os << "Returns set: " << return_set_str->GetBuffer() << std::endl;

	os << "Function is " << GetFuncStabilityStr()->GetBuffer() << std::endl;

	os << "Data access: " << GetFuncDataAccessStr()->GetBuffer() << std::endl;

	const CWStringConst *is_strict =
		IsStrict() ? CDXLTokens::GetDXLTokenStr(EdxltokenTrue)
				   : CDXLTokens::GetDXLTokenStr(EdxltokenFalse);

	os << "Is strict: " << is_strict->GetBuffer() << std::endl;

	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
