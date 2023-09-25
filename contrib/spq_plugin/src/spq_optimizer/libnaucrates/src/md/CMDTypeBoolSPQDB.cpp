//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDTypeBoolSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing the SPQDB bool types in the
//		MD cache
//---------------------------------------------------------------------------

#include "naucrates/md/CMDTypeBoolSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/base/CDatumBoolSPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatumBool.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"
#include "naucrates/md/CMDIdSPQDB.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqnaucrates;

// static member initialization
CWStringConst CMDTypeBoolSPQDB::m_str = CWStringConst(SPQOS_WSZ_LIT("bool"));
CMDName CMDTypeBoolSPQDB::m_mdname(&m_str);

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::CMDTypeBoolSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTypeBoolSPQDB::CMDTypeBoolSPQDB(CMemoryPool *mp) : m_mp(mp)
{
	m_mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_OID);
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		m_distr_opfamily =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_OPFAMILY);
		m_legacy_distr_opfamily = SPQOS_NEW(mp)
			CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_LEGACY_OPFAMILY);
	}
	else
	{
		m_distr_opfamily = NULL;
		m_legacy_distr_opfamily = NULL;
	}
	m_mdid_op_eq = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_EQ_OP);
	m_mdid_op_neq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_NEQ_OP);
	m_mdid_op_lt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_LT_OP);
	m_mdid_op_leq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_LEQ_OP);
	m_mdid_op_gt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_GT_OP);
	m_mdid_op_geq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_GEQ_OP);
	m_mdid_op_cmp =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_COMP_OP);
	m_mdid_type_array =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_ARRAY_TYPE);

	m_mdid_min = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_AGG_MIN);
	m_mdid_max = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_AGG_MAX);
	m_mdid_avg = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_AGG_AVG);
	m_mdid_sum = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_AGG_SUM);
	m_mdid_count =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL_AGG_COUNT);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);

	m_mdid->AddRef();

	SPQOS_ASSERT(SPQDB_BOOL_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());
	m_datum_null = SPQOS_NEW(mp)
		CDatumBoolSPQDB(m_mdid, false /* value */, true /* is_null */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::~CMDTypeBoolSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTypeBoolSPQDB::~CMDTypeBoolSPQDB()
{
	m_mdid->Release();
	CRefCount::SafeRelease(m_distr_opfamily);
	CRefCount::SafeRelease(m_legacy_distr_opfamily);
	m_mdid_op_eq->Release();
	m_mdid_op_neq->Release();
	m_mdid_op_lt->Release();
	m_mdid_op_leq->Release();
	m_mdid_op_gt->Release();
	m_mdid_op_geq->Release();
	m_mdid_op_cmp->Release();
	m_mdid_type_array->Release();

	m_mdid_min->Release();
	m_mdid_max->Release();
	m_mdid_avg->Release();
	m_mdid_sum->Release();
	m_mdid_count->Release();
	m_datum_null->Release();
	SPQOS_DELETE(m_dxl_str);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetMdidForCmpType
//
//	@doc:
//		Return mdid of specified comparison operator type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeBoolSPQDB::GetMdidForCmpType(ECmpType cmp_type) const
{
	switch (cmp_type)
	{
		case EcmptEq:
			return m_mdid_op_eq;
		case EcmptNEq:
			return m_mdid_op_neq;
		case EcmptL:
			return m_mdid_op_lt;
		case EcmptLEq:
			return m_mdid_op_leq;
		case EcmptG:
			return m_mdid_op_gt;
		case EcmptGEq:
			return m_mdid_op_geq;
		default:
			SPQOS_ASSERT(!"Invalid operator type");
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetMdidForAggType
//
//	@doc:
//		Return mdid of specified aggregate type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeBoolSPQDB::GetMdidForAggType(EAggType agg_type) const
{
	switch (agg_type)
	{
		case EaggMin:
			return m_mdid_min;
		case EaggMax:
			return m_mdid_max;
		case EaggAvg:
			return m_mdid_avg;
		case EaggSum:
			return m_mdid_sum;
		case EaggCount:
			return m_mdid_count;
		default:
			SPQOS_ASSERT(!"Invalid aggregate type");
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetDatum
//
//	@doc:
//		Factory function for creating BOOL datums
//
//---------------------------------------------------------------------------
IDatumBool *
CMDTypeBoolSPQDB::CreateBoolDatum(CMemoryPool *mp, BOOL bool_val,
								 BOOL is_null) const
{
	return SPQOS_NEW(mp) CDatumBoolSPQDB(m_mdid->Sysid(), bool_val, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeBoolSPQDB::MDId() const
{
	return m_mdid;
}

IMDId *
CMDTypeBoolSPQDB::GetDistrOpfamilyMdid() const
{
	if (SPQOS_FTRACE(EopttraceUseLegacyOpfamilies))
	{
		return m_legacy_distr_opfamily;
	}
	else
	{
		return m_distr_opfamily;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::Mdname
//
//	@doc:
//		Returns the name of this type
//
//---------------------------------------------------------------------------
CMDName
CMDTypeBoolSPQDB::Mdname() const
{
	return CMDTypeBoolSPQDB::m_mdname;
	;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTypeBoolSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	SPQDBTypeHelper<CMDTypeBoolSPQDB>::Serialize(xml_serializer, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetDatumForDXLConstVal
//
//	@doc:
//		Transformation function to generate bool datum from CDXLScalarConstValue
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeBoolSPQDB::GetDatumForDXLConstVal(
	const CDXLScalarConstValue *dxl_op) const
{
	CDXLDatumBool *dxl_datum =
		CDXLDatumBool::Cast(const_cast<CDXLDatum *>(dxl_op->GetDatumVal()));

	return SPQOS_NEW(m_mp) CDatumBoolSPQDB(m_mdid->Sysid(), dxl_datum->GetValue(),
										 dxl_datum->IsNull());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetDatumForDXLDatum
//
//	@doc:
//		Construct a bool datum from a DXL datum
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeBoolSPQDB::GetDatumForDXLDatum(CMemoryPool *mp,
									 const CDXLDatum *dxl_datum) const
{
	CDXLDatumBool *dxl_datum_bool =
		CDXLDatumBool::Cast(const_cast<CDXLDatum *>(dxl_datum));
	BOOL value = dxl_datum_bool->GetValue();
	BOOL is_null = dxl_datum_bool->IsNull();

	return SPQOS_NEW(mp) CDatumBoolSPQDB(m_mdid->Sysid(), value, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetDatumVal
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeBoolSPQDB::GetDatumVal(CMemoryPool *mp, IDatum *datum) const
{
	CDatumBoolSPQDB *datum_bool = dynamic_cast<CDatumBoolSPQDB *>(datum);
	m_mdid->AddRef();
	return SPQOS_NEW(mp)
		CDXLDatumBool(mp, m_mdid, datum_bool->IsNull(), datum_bool->GetValue());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetDXLOpScConst
//
//	@doc:
// 		Generate a dxl scalar constant from a datum
//
//---------------------------------------------------------------------------
CDXLScalarConstValue *
CMDTypeBoolSPQDB::GetDXLOpScConst(CMemoryPool *mp, IDatum *datum) const
{
	CDatumBoolSPQDB *datum_bool_spqdb = dynamic_cast<CDatumBoolSPQDB *>(datum);

	m_mdid->AddRef();
	CDXLDatumBool *dxl_datum = SPQOS_NEW(mp) CDXLDatumBool(
		mp, m_mdid, datum_bool_spqdb->IsNull(), datum_bool_spqdb->GetValue());

	return SPQOS_NEW(mp) CDXLScalarConstValue(mp, dxl_datum);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::GetDXLDatumNull
//
//	@doc:
// 		Generate dxl datum representing a null value
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeBoolSPQDB::GetDXLDatumNull(CMemoryPool *mp) const
{
	m_mdid->AddRef();

	return SPQOS_NEW(mp) CDXLDatumBool(mp, m_mdid, true /*is_null*/, false);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDTypeBoolSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTypeBoolSPQDB::DebugPrint(IOstream &os) const
{
	SPQDBTypeHelper<CMDTypeBoolSPQDB>::DebugPrint(os, this);
}

#endif	// SPQOS_DEBUG

// EOF
