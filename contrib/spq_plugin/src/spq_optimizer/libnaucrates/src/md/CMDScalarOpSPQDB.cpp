//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDScalarOpSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific scalar ops
//		in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDScalarOpSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::CMDScalarOpSPQDB
//
//	@doc:
//		Constructs a metadata scalar op
//
//---------------------------------------------------------------------------
CMDScalarOpSPQDB::CMDScalarOpSPQDB(
	CMemoryPool *mp, IMDId *mdid, CMDName *mdname, IMDId *mdid_type_left,
	IMDId *mdid_type_right, IMDId *result_type_mdid, IMDId *mdid_func,
	IMDId *mdid_commute_opr, IMDId *m_mdid_inverse_opr,
	IMDType::ECmpType cmp_type, BOOL returns_null_on_null_input,
	IMdIdArray *mdid_opfamilies_array, IMDId *mdid_hash_opfamily,
	IMDId *mdid_legacy_hash_opfamily, BOOL is_ndv_preserving)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_mdid_type_left(mdid_type_left),
	  m_mdid_type_right(mdid_type_right),
	  m_mdid_type_result(result_type_mdid),
	  m_func_mdid(mdid_func),
	  m_mdid_commute_opr(mdid_commute_opr),
	  m_mdid_inverse_opr(m_mdid_inverse_opr),
	  m_comparision_type(cmp_type),
	  m_returns_null_on_null_input(returns_null_on_null_input),
	  m_mdid_opfamilies_array(mdid_opfamilies_array),
	  m_mdid_hash_opfamily(mdid_hash_opfamily),
	  m_mdid_legacy_hash_opfamily(mdid_legacy_hash_opfamily),
	  m_is_ndv_preserving(is_ndv_preserving)
{
	SPQOS_ASSERT(NULL != mdid_opfamilies_array);
	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::~CMDScalarOpSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDScalarOpSPQDB::~CMDScalarOpSPQDB()
{
	m_mdid->Release();
	m_mdid_type_result->Release();
	m_func_mdid->Release();

	CRefCount::SafeRelease(m_mdid_type_left);
	CRefCount::SafeRelease(m_mdid_type_right);
	CRefCount::SafeRelease(m_mdid_commute_opr);
	CRefCount::SafeRelease(m_mdid_inverse_opr);
	CRefCount::SafeRelease(m_mdid_hash_opfamily);
	CRefCount::SafeRelease(m_mdid_legacy_hash_opfamily);

	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
	m_mdid_opfamilies_array->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::MDId
//
//	@doc:
//		Operator id
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::Mdname
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
CMDName
CMDScalarOpSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::GetLeftMdid
//
//	@doc:
//		Type id of left operand
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::GetLeftMdid() const
{
	return m_mdid_type_left;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::GetRightMdid
//
//	@doc:
//		Type id of right operand
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::GetRightMdid() const
{
	return m_mdid_type_right;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::GetResultTypeMdid
//
//	@doc:
//		Type id of result
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::GetResultTypeMdid() const
{
	return m_mdid_type_result;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::FuncMdId
//
//	@doc:
//		Id of function which implements the operator
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::FuncMdId() const
{
	return m_func_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::GetCommuteOpMdid
//
//	@doc:
//		Id of commute operator
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::GetCommuteOpMdid() const
{
	return m_mdid_commute_opr;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::GetInverseOpMdid
//
//	@doc:
//		Id of inverse operator
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::GetInverseOpMdid() const
{
	return m_mdid_inverse_opr;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::IsEqualityOp
//
//	@doc:
//		Is this an equality operator
//
//---------------------------------------------------------------------------
BOOL
CMDScalarOpSPQDB::IsEqualityOp() const
{
	return IMDType::EcmptEq == m_comparision_type;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::ReturnsNullOnNullInput
//
//	@doc:
//		Does operator return NULL when all inputs are NULL?
//		STRICT implies NULL-returning, but the opposite is not always true,
//		the implementation in SPQDB returns what STRICT property states
//
//---------------------------------------------------------------------------
BOOL
CMDScalarOpSPQDB::ReturnsNullOnNullInput() const
{
	return m_returns_null_on_null_input;
}


BOOL
CMDScalarOpSPQDB::IsNDVPreserving() const
{
	return m_is_ndv_preserving;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::ParseCmpType
//
//	@doc:
//		Comparison type
//
//---------------------------------------------------------------------------
IMDType::ECmpType
CMDScalarOpSPQDB::ParseCmpType() const
{
	return m_comparision_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::Serialize
//
//	@doc:
//		Serialize scalar op metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDScalarOpSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBScalarOp));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBScalarOpCmpType),
		IMDType::GetCmpTypeStr(m_comparision_type));
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenReturnsNullOnNullInput),
		m_returns_null_on_null_input);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenIsNDVPreserving),
		m_is_ndv_preserving);

	Edxltoken dxl_token_array[8] = {EdxltokenSPQDBScalarOpLeftTypeId,
									EdxltokenSPQDBScalarOpRightTypeId,
									EdxltokenSPQDBScalarOpResultTypeId,
									EdxltokenSPQDBScalarOpFuncId,
									EdxltokenSPQDBScalarOpCommOpId,
									EdxltokenSPQDBScalarOpInverseOpId,
									EdxltokenSPQDBScalarOpHashOpfamily,
									EdxltokenSPQDBScalarOpLegacyHashOpfamily};

	IMDId *mdid_array[8] = {m_mdid_type_left,	  m_mdid_type_right,
							m_mdid_type_result,	  m_func_mdid,
							m_mdid_commute_opr,	  m_mdid_inverse_opr,
							m_mdid_hash_opfamily, m_mdid_legacy_hash_opfamily};

	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(dxl_token_array); ul++)
	{
		SerializeMDIdAsElem(xml_serializer,
							CDXLTokens::GetDXLTokenStr(dxl_token_array[ul]),
							mdid_array[ul]);

		SPQOS_CHECK_ABORT;
	}

	// serialize opfamilies information
	if (0 < m_mdid_opfamilies_array->Size())
	{
		SerializeMDIdList(xml_serializer, m_mdid_opfamilies_array,
						  CDXLTokens::GetDXLTokenStr(EdxltokenOpfamilies),
						  CDXLTokens::GetDXLTokenStr(EdxltokenOpfamily));
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBScalarOp));
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::OpfamiliesCount
//
//	@doc:
//		Number of opfamilies this operator belongs to
//
//---------------------------------------------------------------------------
ULONG
CMDScalarOpSPQDB::OpfamiliesCount() const
{
	return m_mdid_opfamilies_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::OpfamilyMdidAt
//
//	@doc:
//		Operator family at given position
//
//---------------------------------------------------------------------------
IMDId *
CMDScalarOpSPQDB::OpfamilyMdidAt(ULONG pos) const
{
	SPQOS_ASSERT(pos < m_mdid_opfamilies_array->Size());

	return (*m_mdid_opfamilies_array)[pos];
}

// compatible hash opfamily
IMDId *
CMDScalarOpSPQDB::HashOpfamilyMdid() const
{
	if (SPQOS_FTRACE(EopttraceUseLegacyOpfamilies))
	{
		return m_mdid_legacy_hash_opfamily;
	}
	else
	{
		return m_mdid_hash_opfamily;
	}
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDScalarOpSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDScalarOpSPQDB::DebugPrint(IOstream &os) const
{
	os << "Operator id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Operator name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;

	os << "Left operand type id: ";
	GetLeftMdid()->OsPrint(os);
	os << std::endl;

	os << "Right operand type id: ";
	GetRightMdid()->OsPrint(os);
	os << std::endl;

	os << "Result type id: ";
	GetResultTypeMdid()->OsPrint(os);
	os << std::endl;

	os << "Operator func id: ";
	FuncMdId()->OsPrint(os);
	os << std::endl;

	os << "Commute operator id: ";
	GetCommuteOpMdid()->OsPrint(os);
	os << std::endl;

	os << "Inverse operator id: ";
	GetInverseOpMdid()->OsPrint(os);
	os << std::endl;

	os << std::endl;
}

#endif	// SPQOS_DEBUG


// EOF
