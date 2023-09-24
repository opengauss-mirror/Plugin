//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CMDTypeInt2SPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB-specific int2 type in the
//		MD cache
//---------------------------------------------------------------------------

#include "naucrates/md/CMDTypeInt2SPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/base/CDatumInt2SPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatum.h"
#include "naucrates/dxl/operators/CDXLDatumInt2.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqnaucrates;

// static member initialization
CWStringConst CMDTypeInt2SPQDB::m_str = CWStringConst(SPQOS_WSZ_LIT("int2"));
CMDName CMDTypeInt2SPQDB::m_mdname(&m_str);

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::CMDTypeInt2SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTypeInt2SPQDB::CMDTypeInt2SPQDB(CMemoryPool *mp) : m_mp(mp)
{
	m_mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_OID);
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		m_distr_opfamily =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_OPFAMILY);
		m_legacy_distr_opfamily = SPQOS_NEW(mp)
			CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_LEGACY_OPFAMILY);
	}
	else
	{
		m_distr_opfamily = NULL;
		m_legacy_distr_opfamily = NULL;
	}
	m_mdid_op_eq = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_EQ_OP);
	m_mdid_op_neq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_NEQ_OP);
	m_mdid_op_lt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_LT_OP);
	m_mdid_op_leq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_LEQ_OP);
	m_mdid_op_gt = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_GT_OP);
	m_mdid_op_geq =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_GEQ_OP);
	m_mdid_op_cmp =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_COMP_OP);
	m_mdid_type_array =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_ARRAY_TYPE);
	m_mdid_min = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_AGG_MIN);
	m_mdid_max = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_AGG_MAX);
	m_mdid_avg = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_AGG_AVG);
	m_mdid_sum = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_AGG_SUM);
	m_mdid_count =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2_AGG_COUNT);

	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);

	SPQOS_ASSERT(SPQDB_INT2_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());
	m_mdid->AddRef();
	m_datum_null =
		SPQOS_NEW(mp) CDatumInt2SPQDB(m_mdid, 1 /* value */, true /* is_null */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::~CMDTypeInt2SPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTypeInt2SPQDB::~CMDTypeInt2SPQDB()
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
//		CMDTypeInt2SPQDB::GetDatum
//
//	@doc:
//		Factory function for creating INT2 datums
//
//---------------------------------------------------------------------------
IDatumInt2 *
CMDTypeInt2SPQDB::CreateInt2Datum(CMemoryPool *mp, SINT value,
								 BOOL is_null) const
{
	return SPQOS_NEW(mp) CDatumInt2SPQDB(m_mdid->Sysid(), value, is_null);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt2SPQDB::MDId() const
{
	return m_mdid;
}

IMDId *
CMDTypeInt2SPQDB::GetDistrOpfamilyMdid() const
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
//		CMDTypeInt2SPQDB::Mdname
//
//	@doc:
//		Returns the name of this type
//
//---------------------------------------------------------------------------
CMDName
CMDTypeInt2SPQDB::Mdname() const
{
	return CMDTypeInt2SPQDB::m_mdname;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::GetMdidForCmpType
//
//	@doc:
//		Return mdid of specified comparison operator type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt2SPQDB::GetMdidForCmpType(ECmpType cmp_type) const
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
//		CMDTypeInt2SPQDB::GetMdidForAggType
//
//	@doc:
//		Return mdid of specified aggregate type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeInt2SPQDB::GetMdidForAggType(EAggType agg_type) const
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
//		CMDTypeInt2SPQDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTypeInt2SPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	SPQDBTypeHelper<CMDTypeInt2SPQDB>::Serialize(xml_serializer, this);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::GetDatumForDXLConstVal
//
//	@doc:
//		Transformation method for generating int2 datum from CDXLScalarConstValue
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeInt2SPQDB::GetDatumForDXLConstVal(
	const CDXLScalarConstValue *dxl_op) const
{
	CDXLDatumInt2 *dxl_datum =
		CDXLDatumInt2::Cast(const_cast<CDXLDatum *>(dxl_op->GetDatumVal()));

	return SPQOS_NEW(m_mp) CDatumInt2SPQDB(m_mdid->Sysid(), dxl_datum->Value(),
										 dxl_datum->IsNull());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::GetDatumForDXLDatum
//
//	@doc:
//		Construct an int2 datum from a DXL datum
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeInt2SPQDB::GetDatumForDXLDatum(CMemoryPool *mp,
									 const CDXLDatum *dxl_datum) const
{
	CDXLDatumInt2 *int2_dxl_datum =
		CDXLDatumInt2::Cast(const_cast<CDXLDatum *>(dxl_datum));
	SINT val = int2_dxl_datum->Value();
	BOOL is_null = int2_dxl_datum->IsNull();

	return SPQOS_NEW(mp) CDatumInt2SPQDB(m_mdid->Sysid(), val, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::GetDatumVal
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeInt2SPQDB::GetDatumVal(CMemoryPool *mp, IDatum *datum) const
{
	m_mdid->AddRef();
	CDatumInt2SPQDB *int2_datum = dynamic_cast<CDatumInt2SPQDB *>(datum);

	return SPQOS_NEW(mp)
		CDXLDatumInt2(mp, m_mdid, int2_datum->IsNull(), int2_datum->Value());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::GetDXLOpScConst
//
//	@doc:
// 		Generate a dxl scalar constant from a datum
//
//---------------------------------------------------------------------------
CDXLScalarConstValue *
CMDTypeInt2SPQDB::GetDXLOpScConst(CMemoryPool *mp, IDatum *datum) const
{
	CDatumInt2SPQDB *int2spqdb_datum = dynamic_cast<CDatumInt2SPQDB *>(datum);

	m_mdid->AddRef();
	CDXLDatumInt2 *dxl_datum = SPQOS_NEW(mp) CDXLDatumInt2(
		mp, m_mdid, int2spqdb_datum->IsNull(), int2spqdb_datum->Value());

	return SPQOS_NEW(mp) CDXLScalarConstValue(mp, dxl_datum);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::GetDXLDatumNull
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeInt2SPQDB::GetDXLDatumNull(CMemoryPool *mp) const
{
	m_mdid->AddRef();

	return SPQOS_NEW(mp)
		CDXLDatumInt2(mp, m_mdid, true /*is_null*/, 1 /* a dummy value */);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDTypeInt2SPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTypeInt2SPQDB::DebugPrint(IOstream &os) const
{
	SPQDBTypeHelper<CMDTypeInt2SPQDB>::DebugPrint(os, this);
}

#endif	// SPQOS_DEBUG

// EOF
