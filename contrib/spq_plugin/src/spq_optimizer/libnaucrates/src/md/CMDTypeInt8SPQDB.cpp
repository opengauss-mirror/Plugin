//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDTypeInt8SPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific int8 type
//		in the MD cache
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "naucrates/md/CMDTypeInt8SPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/base/CDatumInt8SPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatum.h"
#include "naucrates/dxl/operators/CDXLDatumInt8.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqnaucrates;

// static member initialization
CWStringConst CMDTypeInt8SPQDB::m_str = CWStringConst(SPQOS_WSZ_LIT("Int8"));
CMDName CMDTypeInt8SPQDB::m_mdname(&m_str);

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::CMDTypeInt8SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTypeInt8SPQDB::CMDTypeInt8SPQDB(CMemoryPool *mp) : m_mp(mp)
{
	m_mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_OID);
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		m_distr_opfamily =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_OPFAMILY);
		m_legacy_distr_opfamily = SPQOS_NEW(mp)
			CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_LEGACY_OPFAMILY);
	}
	else
	{
		m_distr_opfamily = NULL;
		m_legacy_distr_opfamily = NULL;
	}
	m_mdid_op_eq = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_EQ_OP);
	m_mdid_op_neq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_NEQ_OP);
	m_mdid_op_lt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_LT_OP);
	m_mdid_op_leq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_LEQ_OP);
	m_mdid_op_gt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_GT_OP);
	m_mdid_op_geq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_GEQ_OP);
	m_mdid_op_cmp =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_COMP_OP);
	m_mdid_type_array =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_ARRAY_TYPE);

	m_mdid_min = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_AGG_MIN);
	m_mdid_max = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_AGG_MAX);
	m_mdid_avg = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_AGG_AVG);
	m_mdid_sum = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_AGG_SUM);
	m_mdid_count =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_AGG_COUNT);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);

	SPQOS_ASSERT(SPQDB_INT8_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());
	m_mdid->AddRef();
	m_datum_null =
		SPQOS_NEW(mp) CDatumInt8SPQDB(m_mdid, 1 /* value */, true /* is_null */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::~CMDTypeInt8SPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTypeInt8SPQDB::~CMDTypeInt8SPQDB()
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
//		CMDTypeInt8SPQDB::GetDatum
//
//	@doc:
//		Factory function for creating Int8 datums
//
//---------------------------------------------------------------------------
IDatumInt8 *
CMDTypeInt8SPQDB::CreateInt8Datum(CMemoryPool *mp, LINT value,
								 BOOL is_null) const
{
	return SPQOS_NEW(mp) CDatumInt8SPQDB(m_mdid->Sysid(), value, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt8SPQDB::MDId() const
{
	return m_mdid;
}

IMDId *
CMDTypeInt8SPQDB::GetDistrOpfamilyMdid() const
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
//		CMDTypeInt8SPQDB::Mdname
//
//	@doc:
//		Returns the name of this type
//
//---------------------------------------------------------------------------
CMDName
CMDTypeInt8SPQDB::Mdname() const
{
	return m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::GetMdidForCmpType
//
//	@doc:
//		Return mdid of specified comparison operator type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt8SPQDB::GetMdidForCmpType(ECmpType cmp_type) const
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
//		CMDTypeInt8SPQDB::GetMdidForAggType
//
//	@doc:
//		Return mdid of specified aggregate type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt8SPQDB::GetMdidForAggType(EAggType agg_type) const
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
//		CMDTypeInt8SPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTypeInt8SPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	SPQDBTypeHelper<CMDTypeInt8SPQDB>::Serialize(xml_serializer, this);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::GetDatumForDXLConstVal
//
//	@doc:
//		Transformation method for generating Int8 datum from CDXLScalarConstValue
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeInt8SPQDB::GetDatumForDXLConstVal(
	const CDXLScalarConstValue *dxl_op) const
{
	CDXLDatumInt8 *dxl_datum =
		CDXLDatumInt8::Cast(const_cast<CDXLDatum *>(dxl_op->GetDatumVal()));

	return SPQOS_NEW(m_mp) CDatumInt8SPQDB(m_mdid->Sysid(), dxl_datum->Value(),
										 dxl_datum->IsNull());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::GetDatumForDXLDatum
//
//	@doc:
//		Construct an int8 datum from a DXL datum
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeInt8SPQDB::GetDatumForDXLDatum(CMemoryPool *mp,
									 const CDXLDatum *dxl_datum) const
{
	CDXLDatumInt8 *int8_dxl_datum =
		CDXLDatumInt8::Cast(const_cast<CDXLDatum *>(dxl_datum));

	LINT val = int8_dxl_datum->Value();
	BOOL is_null = int8_dxl_datum->IsNull();

	return SPQOS_NEW(mp) CDatumInt8SPQDB(m_mdid->Sysid(), val, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::GetDatumVal
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeInt8SPQDB::GetDatumVal(CMemoryPool *mp, IDatum *datum) const
{
	CDatumInt8SPQDB *int8_datum = dynamic_cast<CDatumInt8SPQDB *>(datum);

	m_mdid->AddRef();
	return SPQOS_NEW(mp)
		CDXLDatumInt8(mp, m_mdid, int8_datum->IsNull(), int8_datum->Value());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::GetDXLOpScConst
//
//	@doc:
// 		Generate a dxl scalar constant from a datum
//
//---------------------------------------------------------------------------
CDXLScalarConstValue *
CMDTypeInt8SPQDB::GetDXLOpScConst(CMemoryPool *mp, IDatum *datum) const
{
	CDatumInt8SPQDB *int8spqdb_datum = dynamic_cast<CDatumInt8SPQDB *>(datum);

	m_mdid->AddRef();
	CDXLDatumInt8 *dxl_datum = SPQOS_NEW(mp) CDXLDatumInt8(
		mp, m_mdid, int8spqdb_datum->IsNull(), int8spqdb_datum->Value());

	return SPQOS_NEW(mp) CDXLScalarConstValue(mp, dxl_datum);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::GetDXLDatumNull
//
//	@doc:
// 		Generate dxl null datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeInt8SPQDB::GetDXLDatumNull(CMemoryPool *mp) const
{
	m_mdid->AddRef();

	return SPQOS_NEW(mp) CDXLDatumInt8(mp, m_mdid, true /*is_null*/, 1);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt8SPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTypeInt8SPQDB::DebugPrint(IOstream &os) const
{
	SPQDBTypeHelper<CMDTypeInt8SPQDB>::DebugPrint(os, this);
}

#endif	// SPQOS_DEBUG

// EOF
