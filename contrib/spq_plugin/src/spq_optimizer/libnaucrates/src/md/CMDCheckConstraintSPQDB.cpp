//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDCheckConstraintSPQDB.cpp
//
//	@doc:
//		Implementation of the class representing a SPQDB check constraint
//		in a metadata cache relation
//---------------------------------------------------------------------------

#include "naucrates/md/CMDCheckConstraintSPQDB.h"

#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CMDCheckConstraintSPQDB::CMDCheckConstraintSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDCheckConstraintSPQDB::CMDCheckConstraintSPQDB(CMemoryPool *mp, IMDId *mdid,
											   CMDName *mdname, IMDId *rel_mdid,
											   CDXLNode *dxlnode)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_rel_mdid(rel_mdid),
	  m_dxl_node(dxlnode)
{
	SPQOS_ASSERT(mdid->IsValid());
	SPQOS_ASSERT(rel_mdid->IsValid());
	SPQOS_ASSERT(NULL != mdname);
	SPQOS_ASSERT(NULL != dxlnode);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCheckConstraintSPQDB::~CMDCheckConstraintSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDCheckConstraintSPQDB::~CMDCheckConstraintSPQDB()
{
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
	m_mdid->Release();
	m_rel_mdid->Release();
	m_dxl_node->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCheckConstraintSPQDB::GetCheckConstraintExpr
//
//	@doc:
//		Scalar expression of the check constraint
//
//---------------------------------------------------------------------------
CExpression *
CMDCheckConstraintSPQDB::GetCheckConstraintExpr(CMemoryPool *mp,
											   CMDAccessor *md_accessor,
											   CColRefArray *colref_array) const
{
	SPQOS_ASSERT(NULL != colref_array);

	const IMDRelation *mdrel = md_accessor->RetrieveRel(m_rel_mdid);
#ifdef SPQOS_DEBUG
	const ULONG len = colref_array->Size();
	SPQOS_ASSERT(len > 0);

	const ULONG arity =
		mdrel->NonDroppedColsCount() - mdrel->SystemColumnsCount();
	SPQOS_ASSERT(arity == len);
#endif	// SPQOS_DEBUG

	// translate the DXL representation of the check constraint expression
	CTranslatorDXLToExpr dxltr(mp, md_accessor);
	return dxltr.PexprTranslateScalar(m_dxl_node, colref_array,
									  mdrel->NonDroppedColsArray());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDCheckConstraintSPQDB::Serialize
//
//	@doc:
//		Serialize check constraint in DXL format
//
//---------------------------------------------------------------------------
void
CMDCheckConstraintSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCheckConstraint));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	m_rel_mdid->Serialize(xml_serializer,
						  CDXLTokens::GetDXLTokenStr(EdxltokenRelationMdid));

	// serialize the scalar expression
	m_dxl_node->SerializeToDXL(xml_serializer);

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCheckConstraint));
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDCheckConstraintSPQDB::DebugPrint
//
//	@doc:
//		Prints a MD constraint to the provided output
//
//---------------------------------------------------------------------------
void
CMDCheckConstraintSPQDB::DebugPrint(IOstream &os) const
{
	os << "Constraint Id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Constraint Name: " << (Mdname()).GetMDName()->GetBuffer()
	   << std::endl;

	os << "Relation id: ";
	GetRelMdId()->OsPrint(os);
	os << std::endl;
}

#endif	// SPQOS_DEBUG

// EOF
