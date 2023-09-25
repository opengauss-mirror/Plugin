//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDTypeInt4SPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific int4 type in the
//		MD cache
//---------------------------------------------------------------------------

#include "naucrates/md/CMDTypeInt4SPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/base/CDatumInt4SPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatum.h"
#include "naucrates/dxl/operators/CDXLDatumInt4.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqnaucrates;

// static member initialization
CWStringConst CMDTypeInt4SPQDB::m_str = CWStringConst(SPQOS_WSZ_LIT("int4"));
CMDName CMDTypeInt4SPQDB::m_mdname(&m_str);

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::CMDTypeInt4SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTypeInt4SPQDB::CMDTypeInt4SPQDB(CMemoryPool *mp) : m_mp(mp)
{
	m_mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_OID);
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		m_distr_opfamily =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_OPFAMILY);
		m_legacy_distr_opfamily = SPQOS_NEW(mp)
			CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_LEGACY_OPFAMILY);
	}
	else
	{
		m_distr_opfamily = NULL;
		m_legacy_distr_opfamily = NULL;
	}
	m_mdid_op_eq = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_EQ_OP);
	m_mdid_op_neq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_NEQ_OP);
	m_mdid_op_lt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_LT_OP);
	m_mdid_op_leq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_LEQ_OP);
	m_mdid_op_gt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_GT_OP);
	m_mdid_op_geq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_GEQ_OP);
	m_mdid_op_cmp =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_COMP_OP);
	m_mdid_type_array =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_ARRAY_TYPE);
	m_mdid_min = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_AGG_MIN);
	m_mdid_max = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_AGG_MAX);
	m_mdid_avg = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_AGG_AVG);
	m_mdid_sum = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_AGG_SUM);
	m_mdid_count =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_AGG_COUNT);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);

	SPQOS_ASSERT(SPQDB_INT4_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());
	m_mdid->AddRef();
	m_datum_null =
		SPQOS_NEW(mp) CDatumInt4SPQDB(m_mdid, 1 /* value */, true /* is_null */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::~CMDTypeInt4SPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTypeInt4SPQDB::~CMDTypeInt4SPQDB()
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
//		CMDTypeInt4SPQDB::GetDatum
//
//	@doc:
//		Factory function for creating INT4 datums
//
//---------------------------------------------------------------------------
IDatumInt4 *
CMDTypeInt4SPQDB::CreateInt4Datum(CMemoryPool *mp, INT value, BOOL is_null) const
{
	return SPQOS_NEW(mp) CDatumInt4SPQDB(m_mdid->Sysid(), value, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt4SPQDB::MDId() const
{
	return m_mdid;
}

IMDId *
CMDTypeInt4SPQDB::GetDistrOpfamilyMdid() const
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
//		CMDTypeInt4SPQDB::Mdname
//
//	@doc:
//		Returns the name of this type
//
//---------------------------------------------------------------------------
CMDName
CMDTypeInt4SPQDB::Mdname() const
{
	return CMDTypeInt4SPQDB::m_mdname;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::GetMdidForCmpType
//
//	@doc:
//		Return mdid of specified comparison operator type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt4SPQDB::GetMdidForCmpType(ECmpType cmp_type) const
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
//		CMDTypeInt4SPQDB::GetMdidForAggType
//
//	@doc:
//		Return mdid of specified aggregate type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt4SPQDB::GetMdidForAggType(EAggType agg_type) const
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
//		CMDTypeInt4SPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTypeInt4SPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	SPQDBTypeHelper<CMDTypeInt4SPQDB>::Serialize(xml_serializer, this);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::GetDatumForDXLConstVal
//
//	@doc:
//		Transformation method for generating int4 datum from CDXLScalarConstValue
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeInt4SPQDB::GetDatumForDXLConstVal(
	const CDXLScalarConstValue *dxl_op) const
{
	CDXLDatumInt4 *dxl_datum =
		CDXLDatumInt4::Cast(const_cast<CDXLDatum *>(dxl_op->GetDatumVal()));

	return SPQOS_NEW(m_mp) CDatumInt4SPQDB(m_mdid->Sysid(), dxl_datum->Value(),
										 dxl_datum->IsNull());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::GetDatumForDXLDatum
//
//	@doc:
//		Construct an int4 datum from a DXL datum
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeInt4SPQDB::GetDatumForDXLDatum(CMemoryPool *mp,
									 const CDXLDatum *dxl_datum) const
{
	CDXLDatumInt4 *int4_dxl_datum =
		CDXLDatumInt4::Cast(const_cast<CDXLDatum *>(dxl_datum));
	INT val = int4_dxl_datum->Value();
	BOOL is_null = int4_dxl_datum->IsNull();

	return SPQOS_NEW(mp) CDatumInt4SPQDB(m_mdid->Sysid(), val, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::GetDatumVal
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeInt4SPQDB::GetDatumVal(CMemoryPool *mp, IDatum *datum) const
{
	m_mdid->AddRef();
	CDatumInt4SPQDB *int4_datum = dynamic_cast<CDatumInt4SPQDB *>(datum);

	return SPQOS_NEW(mp)
		CDXLDatumInt4(mp, m_mdid, int4_datum->IsNull(), int4_datum->Value());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::GetDXLOpScConst
//
//	@doc:
// 		Generate a dxl scalar constant from a datum
//
//---------------------------------------------------------------------------
CDXLScalarConstValue *
CMDTypeInt4SPQDB::GetDXLOpScConst(CMemoryPool *mp, IDatum *datum) const
{
	CDatumInt4SPQDB *int4spqdb_datum = dynamic_cast<CDatumInt4SPQDB *>(datum);

	m_mdid->AddRef();
	CDXLDatumInt4 *dxl_datum = SPQOS_NEW(mp) CDXLDatumInt4(
		mp, m_mdid, int4spqdb_datum->IsNull(), int4spqdb_datum->Value());

	return SPQOS_NEW(mp) CDXLScalarConstValue(mp, dxl_datum);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::GetDXLDatumNull
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeInt4SPQDB::GetDXLDatumNull(CMemoryPool *mp) const
{
	m_mdid->AddRef();

	return SPQOS_NEW(mp) CDXLDatumInt4(mp, m_mdid, true /*is_null*/, 1);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt4SPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTypeInt4SPQDB::DebugPrint(IOstream &os) const
{
	SPQDBTypeHelper<CMDTypeInt4SPQDB>::DebugPrint(os, this);
}

#endif	// SPQOS_DEBUG

// EOF
