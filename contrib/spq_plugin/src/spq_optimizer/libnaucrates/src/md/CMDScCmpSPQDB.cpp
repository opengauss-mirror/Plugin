//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDScCmpSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific comparisons
//		in the MD cache
//---------------------------------------------------------------------------


#include "naucrates/md/CMDScCmpSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqmd;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::CMDScCmpSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDScCmpSPQDB::CMDScCmpSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
						   IMDId *left_mdid, IMDId *right_mdid,
						   IMDType::ECmpType cmp_type, IMDId *mdid_op)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_mdid_left(left_mdid),
	  m_mdid_right(right_mdid),
	  m_comparision_type(cmp_type),
	  m_mdid_op(mdid_op)
{
	SPQOS_ASSERT(m_mdid->IsValid());
	SPQOS_ASSERT(m_mdid_left->IsValid());
	SPQOS_ASSERT(m_mdid_right->IsValid());
	SPQOS_ASSERT(m_mdid_op->IsValid());
	SPQOS_ASSERT(IMDType::EcmptOther != m_comparision_type);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::~CMDScCmpSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDScCmpSPQDB::~CMDScCmpSPQDB()
{
	m_mdid->Release();
	m_mdid_left->Release();
	m_mdid_right->Release();
	m_mdid_op->Release();
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::MDId
//
//	@doc:
//		Mdid of comparison object
//
//---------------------------------------------------------------------------
IMDId *
CMDScCmpSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::Mdname
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
CMDName
CMDScCmpSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::GetLeftMdid
//
//	@doc:
//		Left type id
//
//---------------------------------------------------------------------------
IMDId *
CMDScCmpSPQDB::GetLeftMdid() const
{
	return m_mdid_left;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::GetRightMdid
//
//	@doc:
//		Destination type id
//
//---------------------------------------------------------------------------
IMDId *
CMDScCmpSPQDB::GetRightMdid() const
{
	return m_mdid_right;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::MdIdOp
//
//	@doc:
//		Cast function id
//
//---------------------------------------------------------------------------
IMDId *
CMDScCmpSPQDB::MdIdOp() const
{
	return m_mdid_op;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::IsBinaryCoercible
//
//	@doc:
//		Returns whether this is a cast between binary coercible types, i.e. the
//		types are binary compatible
//
//---------------------------------------------------------------------------
IMDType::ECmpType
CMDScCmpSPQDB::ParseCmpType() const
{
	return m_comparision_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::Serialize
//
//	@doc:
//		Serialize comparison operator metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDScCmpSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBMDScCmp));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));

	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBScalarOpCmpType),
		IMDType::GetCmpTypeStr(m_comparision_type));

	m_mdid_left->Serialize(
		xml_serializer,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBScalarOpLeftTypeId));
	m_mdid_right->Serialize(
		xml_serializer,
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBScalarOpRightTypeId));
	m_mdid_op->Serialize(xml_serializer,
						 CDXLTokens::GetDXLTokenStr(EdxltokenOpNo));

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenSPQDBMDScCmp));

	SPQOS_CHECK_ABORT;
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMDScCmpSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDScCmpSPQDB::DebugPrint(IOstream &os) const
{
	os << "ComparisonOp ";
	GetLeftMdid()->OsPrint(os);
	os << (Mdname()).GetMDName()->GetBuffer() << "(";
	MdIdOp()->OsPrint(os);
	os << ") ";
	GetLeftMdid()->OsPrint(os);


	os << ", type: " << IMDType::GetCmpTypeStr(m_comparision_type);

	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
