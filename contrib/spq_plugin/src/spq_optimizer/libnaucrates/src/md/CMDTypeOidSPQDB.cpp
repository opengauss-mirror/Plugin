//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDTypeOidSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific OID type in
//		the MD cache
//---------------------------------------------------------------------------

#include "naucrates/md/CMDTypeOidSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/base/CDatumOidSPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatum.h"
#include "naucrates/dxl/operators/CDXLDatumOid.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqnaucrates;

// static member initialization
CWStringConst CMDTypeOidSPQDB::m_str = CWStringConst(SPQOS_WSZ_LIT("oid"));
CMDName CMDTypeOidSPQDB::m_mdname(&m_str);

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::CMDTypeOidSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTypeOidSPQDB::CMDTypeOidSPQDB(CMemoryPool *mp) : m_mp(mp)
{
	m_mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_OID);
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		m_distr_opfamily =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_OPFAMILY);
		m_legacy_distr_opfamily = SPQOS_NEW(mp)
			CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_LEGACY_OPFAMILY);
	}
	else
	{
		m_distr_opfamily = NULL;
		m_legacy_distr_opfamily = NULL;
	}
	m_mdid_op_eq = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_EQ_OP);
	m_mdid_op_neq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_NEQ_OP);
	m_mdid_op_lt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_LT_OP);
	m_mdid_op_leq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_LEQ_OP);
	m_mdid_op_gt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_GT_OP);
	m_mdid_op_geq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_GEQ_OP);
	m_mdid_op_cmp =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_COMP_OP);
	m_mdid_type_array =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_ARRAY_TYPE);

	m_mdid_min = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_AGG_MIN);
	m_mdid_max = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_AGG_MAX);
	m_mdid_avg = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_AGG_AVG);
	m_mdid_sum = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_AGG_SUM);
	m_mdid_count =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID_AGG_COUNT);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);

	SPQOS_ASSERT(SPQDB_OID_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());
	m_mdid->AddRef();
	m_datum_null =
		SPQOS_NEW(mp) CDatumOidSPQDB(m_mdid, 1 /* value */, true /* is_null */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::~CMDTypeOidSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTypeOidSPQDB::~CMDTypeOidSPQDB()
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
//		CMDTypeOidSPQDB::GetDatum
//
//	@doc:
//		Factory function for creating OID datums
//
//---------------------------------------------------------------------------
IDatumOid *
CMDTypeOidSPQDB::CreateOidDatum(CMemoryPool *mp, OID oValue, BOOL is_null) const
{
	return SPQOS_NEW(mp) CDatumOidSPQDB(m_mdid->Sysid(), oValue, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeOidSPQDB::MDId() const
{
	return m_mdid;
}

IMDId *
CMDTypeOidSPQDB::GetDistrOpfamilyMdid() const
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
//		CMDTypeOidSPQDB::Mdname
//
//	@doc:
//		Returns the name of this type
//
//---------------------------------------------------------------------------
CMDName
CMDTypeOidSPQDB::Mdname() const
{
	return CMDTypeOidSPQDB::m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::GetMdidForCmpType
//
//	@doc:
//		Return mdid of specified comparison operator type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeOidSPQDB::GetMdidForCmpType(ECmpType cmp_type) const
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
//		CMDTypeOidSPQDB::GetMdidForAggType
//
//	@doc:
//		Return mdid of specified aggregate type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeOidSPQDB::GetMdidForAggType(EAggType agg_type) const
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
//		CMDTypeOidSPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTypeOidSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	SPQDBTypeHelper<CMDTypeOidSPQDB>::Serialize(xml_serializer, this);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::GetDatumForDXLConstVal
//
//	@doc:
//		Transformation method for generating oid datum from CDXLScalarConstValue
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeOidSPQDB::GetDatumForDXLConstVal(const CDXLScalarConstValue *dxl_op) const
{
	CDXLDatumOid *dxl_datum =
		CDXLDatumOid::Cast(const_cast<CDXLDatum *>(dxl_op->GetDatumVal()));

	return SPQOS_NEW(m_mp) CDatumOidSPQDB(m_mdid->Sysid(), dxl_datum->OidValue(),
										dxl_datum->IsNull());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::GetDatumForDXLDatum
//
//	@doc:
//		Construct an oid datum from a DXL datum
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeOidSPQDB::GetDatumForDXLDatum(CMemoryPool *mp,
									const CDXLDatum *dxl_datum) const
{
	CDXLDatumOid *dxl_datumOid =
		CDXLDatumOid::Cast(const_cast<CDXLDatum *>(dxl_datum));
	OID oid_value = dxl_datumOid->OidValue();
	BOOL is_null = dxl_datumOid->IsNull();

	return SPQOS_NEW(mp) CDatumOidSPQDB(m_mdid->Sysid(), oid_value, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::GetDatumVal
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeOidSPQDB::GetDatumVal(CMemoryPool *mp, IDatum *datum) const
{
	m_mdid->AddRef();
	CDatumOidSPQDB *oid_datum = dynamic_cast<CDatumOidSPQDB *>(datum);

	return SPQOS_NEW(mp)
		CDXLDatumOid(mp, m_mdid, oid_datum->IsNull(), oid_datum->OidValue());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::GetDXLOpScConst
//
//	@doc:
// 		Generate a dxl scalar constant from a datum
//
//---------------------------------------------------------------------------
CDXLScalarConstValue *
CMDTypeOidSPQDB::GetDXLOpScConst(CMemoryPool *mp, IDatum *datum) const
{
	CDatumOidSPQDB *datum_oidSPQDB = dynamic_cast<CDatumOidSPQDB *>(datum);

	m_mdid->AddRef();
	CDXLDatumOid *dxl_datum = SPQOS_NEW(mp) CDXLDatumOid(
		mp, m_mdid, datum_oidSPQDB->IsNull(), datum_oidSPQDB->OidValue());

	return SPQOS_NEW(mp) CDXLScalarConstValue(mp, dxl_datum);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::GetDXLDatumNull
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeOidSPQDB::GetDXLDatumNull(CMemoryPool *mp) const
{
	m_mdid->AddRef();

	return SPQOS_NEW(mp) CDXLDatumOid(mp, m_mdid, true /*is_null*/, 1);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDTypeOidSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTypeOidSPQDB::DebugPrint(IOstream &os) const
{
	SPQDBTypeHelper<CMDTypeOidSPQDB>::DebugPrint(os, this);
}

#endif	// SPQOS_DEBUG

// EOF
