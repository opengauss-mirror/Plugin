//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDTypeGenericSPQDB.cpp
//
//	@doc:
//		Implementation of the class for representing SPQDB generic types
//---------------------------------------------------------------------------

#include "naucrates/md/CMDTypeGenericSPQDB.h"

#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/COptCtxt.h"
#include "naucrates/base/CDatumGenericSPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatumGeneric.h"
#include "naucrates/dxl/operators/CDXLDatumStatsDoubleMappable.h"
#include "naucrates/dxl/operators/CDXLDatumStatsLintMappable.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/statistics/CStatsPredUtils.h"

using namespace spqnaucrates;
using namespace spqdxl;
using namespace spqmd;

#define SPQDB_ANYELEMENT_OID OID(2283)  // oid of SPQDB ANYELEMENT type

#define SPQDB_ANYARRAY_OID OID(2277)	 // oid of SPQDB ANYARRAY type

#define SPQDB_ANYNONARRAY_OID OID(2776)	// oid of SPQDB ANYNONARRAY type

#define SPQDB_ANYENUM_OID OID(3500)	// oid of SPQDB ANYENUM type

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::CMDTypeGenericSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDTypeGenericSPQDB::CMDTypeGenericSPQDB(
	CMemoryPool *mp, IMDId *mdid, CMDName *mdname, BOOL is_redistributable,
	BOOL is_fixed_length, ULONG length, BOOL is_passed_by_value,
	IMDId *mdid_distr_opfamily, IMDId *mdid_legacy_distr_opfamily,
	IMDId *mdid_op_eq, IMDId *mdid_op_neq, IMDId *mdid_op_lt,
	IMDId *mdid_op_leq, IMDId *mdid_op_gt, IMDId *mdid_op_geq,
	IMDId *mdid_op_cmp, IMDId *mdid_op_min, IMDId *mdid_op_max,
	IMDId *mdid_op_avg, IMDId *mdid_op_sum, IMDId *mdid_op_count,
	BOOL is_hashable, BOOL is_merge_joinable, BOOL is_composite_type,
	BOOL is_text_related, IMDId *mdid_base_relation, IMDId *mdid_type_array,
	INT spqdb_length)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_is_redistributable(is_redistributable),
	  m_is_fixed_length(is_fixed_length),
	  m_length(length),
	  m_is_passed_by_value(is_passed_by_value),
	  m_distr_opfamily(mdid_distr_opfamily),
	  m_legacy_distr_opfamily(mdid_legacy_distr_opfamily),
	  m_mdid_op_eq(mdid_op_eq),
	  m_mdid_op_neq(mdid_op_neq),
	  m_mdid_op_lt(mdid_op_lt),
	  m_mdid_op_leq(mdid_op_leq),
	  m_mdid_op_gt(mdid_op_gt),
	  m_mdid_op_geq(mdid_op_geq),
	  m_mdid_op_cmp(mdid_op_cmp),
	  m_mdid_min(mdid_op_min),
	  m_mdid_max(mdid_op_max),
	  m_mdid_avg(mdid_op_avg),
	  m_mdid_sum(mdid_op_sum),
	  m_mdid_count(mdid_op_count),
	  m_is_hashable(is_hashable),
	  m_is_merge_joinable(is_merge_joinable),
	  m_is_composite_type(is_composite_type),
	  m_is_text_related(is_text_related),
	  m_mdid_base_relation(mdid_base_relation),
	  m_mdid_type_array(mdid_type_array),
	  m_spqdb_length(spqdb_length),
	  m_datum_null(NULL)
{
	SPQOS_ASSERT_IMP(m_is_fixed_length, 0 < m_length);
	SPQOS_ASSERT_IMP(!m_is_fixed_length, 0 > m_spqdb_length);
	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);

	m_mdid->AddRef();
	m_datum_null = SPQOS_NEW(m_mp) CDatumGenericSPQDB(
		m_mp, m_mdid, default_type_modifier, NULL /*pba*/, 0 /*length*/,
		true /*constNull*/, 0 /*lValue */, 0 /*dValue */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::~CMDTypeGenericSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDTypeGenericSPQDB::~CMDTypeGenericSPQDB()
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
	CRefCount::SafeRelease(m_mdid_base_relation);
	SPQOS_DELETE(m_mdname);
	SPQOS_DELETE(m_dxl_str);
	m_datum_null->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::GetMdidForAggType
//
//	@doc:
//		Return mdid of specified aggregate type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeGenericSPQDB::GetMdidForAggType(EAggType agg_type) const
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
//		CMDTypeGenericSPQDB::MDId
//
//	@doc:
//		Returns the metadata id of this type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeGenericSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::Mdname
//
//	@doc:
//		Returns the name of this type
//
//---------------------------------------------------------------------------
CMDName
CMDTypeGenericSPQDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::GetMdidForCmpType
//
//	@doc:
//		Return mdid of specified comparison operator type
//
//---------------------------------------------------------------------------
IMDId *
CMDTypeGenericSPQDB::GetMdidForCmpType(ECmpType cmp_type) const
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
//		CMDTypeGenericSPQDB::Serialize
//
//	@doc:
//		Serialize metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDTypeGenericSPQDB::Serialize(CXMLSerializer *xml_serializer) const
{
	SPQDBTypeHelper<CMDTypeGenericSPQDB>::Serialize(xml_serializer, this);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::PGetDatumForDXLConstValdatum
//
//	@doc:
//		Factory method for generating generic datum from CDXLScalarConstValue
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeGenericSPQDB::GetDatumForDXLConstVal(
	const CDXLScalarConstValue *dxl_op) const
{
	CDXLDatumGeneric *dxl_datum =
		CDXLDatumGeneric::Cast(const_cast<CDXLDatum *>(dxl_op->GetDatumVal()));
	SPQOS_ASSERT(NULL != dxl_op);

	LINT lint_value = 0;
	if (dxl_datum->IsDatumMappableToLINT())
	{
		lint_value = dxl_datum->GetLINTMapping();
	}

	CDouble double_value = 0;
	if (dxl_datum->IsDatumMappableToDouble())
	{
		double_value = dxl_datum->GetDoubleMapping();
	}

	m_mdid->AddRef();
	return SPQOS_NEW(m_mp) CDatumGenericSPQDB(
		m_mp, m_mdid, dxl_datum->TypeModifier(), dxl_datum->GetByteArray(),
		dxl_datum->Length(), dxl_datum->IsNull(), lint_value, double_value);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::GetDatumForDXLDatum
//
//	@doc:
//		Construct a datum from a DXL datum
//
//---------------------------------------------------------------------------
IDatum *
CMDTypeGenericSPQDB::GetDatumForDXLDatum(CMemoryPool *mp,
										const CDXLDatum *dxl_datum) const
{
	m_mdid->AddRef();
	CDXLDatumGeneric *dxl_datum_generic =
		CDXLDatumGeneric::Cast(const_cast<CDXLDatum *>(dxl_datum));

	LINT lint_value = 0;
	if (dxl_datum_generic->IsDatumMappableToLINT())
	{
		lint_value = dxl_datum_generic->GetLINTMapping();
	}

	CDouble double_value = 0;
	if (dxl_datum_generic->IsDatumMappableToDouble())
	{
		double_value = dxl_datum_generic->GetDoubleMapping();
	}

	return SPQOS_NEW(m_mp) CDatumGenericSPQDB(
		mp, m_mdid, dxl_datum_generic->TypeModifier(),
		dxl_datum_generic->GetByteArray(), dxl_datum_generic->Length(),
		dxl_datum_generic->IsNull(), lint_value, double_value);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::GetDatumVal
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeGenericSPQDB::GetDatumVal(CMemoryPool *mp, IDatum *datum) const
{
	m_mdid->AddRef();
	CDatumGenericSPQDB *datum_generic = dynamic_cast<CDatumGenericSPQDB *>(datum);
	ULONG length = 0;
	BYTE *pba = NULL;
	if (!datum_generic->IsNull())
	{
		pba = datum_generic->MakeCopyOfValue(mp, &length);
	}

	LINT lValue = 0;
	if (datum_generic->IsDatumMappableToLINT())
	{
		lValue = datum_generic->GetLINTMapping();
	}

	CDouble dValue = 0;
	if (datum_generic->IsDatumMappableToDouble())
	{
		dValue = datum_generic->GetDoubleMapping();
	}

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDType *md_type = md_accessor->RetrieveType(m_mdid);
	return CreateDXLDatumVal(mp, m_mdid, md_type, datum_generic->TypeModifier(),
							 datum_generic->IsNull(), pba, length, lValue,
							 dValue);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::IsAmbiguous
//
//	@doc:
// 		Is type an ambiguous one? I.e. a "polymorphic" type in SPQDB terms
//
//---------------------------------------------------------------------------
BOOL
CMDTypeGenericSPQDB::IsAmbiguous() const
{
	OID oid = CMDIdSPQDB::CastMdid(m_mdid)->Oid();
	// This should match the IsPolymorphicType() macro in SPQDB's pg_type.h
	return (SPQDB_ANYELEMENT_OID == oid || SPQDB_ANYARRAY_OID == oid ||
			SPQDB_ANYNONARRAY_OID == oid || SPQDB_ANYENUM_OID == oid);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::CreateDXLDatumVal
//
//	@doc:
// 		Create a dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeGenericSPQDB::CreateDXLDatumVal(CMemoryPool *mp, IMDId *mdid,
									  const IMDType *md_type, INT type_modifier,
									  BOOL is_null, BYTE *pba, ULONG length,
									  LINT lValue, CDouble dValue)
{
	SPQOS_ASSERT(IMDId::EmdidGeneral == mdid->MdidType());

	if (HasByte2DoubleMapping(mdid))
	{
		return CMDTypeGenericSPQDB::CreateDXLDatumStatsDoubleMappable(
			mp, mdid, type_modifier, is_null, pba, length, lValue, dValue);
	}

	if (HasByte2IntMapping(md_type))
	{
		return CMDTypeGenericSPQDB::CreateDXLDatumStatsIntMappable(
			mp, mdid, type_modifier, is_null, pba, length, lValue, dValue);
	}

	return SPQOS_NEW(mp)
		CDXLDatumGeneric(mp, mdid, type_modifier, is_null, pba, length);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::CreateDXLDatumStatsDoubleMappable
//
//	@doc:
// 		Create a dxl datum of types that need double mapping
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeGenericSPQDB::CreateDXLDatumStatsDoubleMappable(
	CMemoryPool *mp, IMDId *mdid, INT type_modifier, BOOL is_null,
	BYTE *byte_array, ULONG length, LINT, CDouble double_value)
{
	SPQOS_ASSERT(CMDTypeGenericSPQDB::HasByte2DoubleMapping(mdid));
	return SPQOS_NEW(mp) CDXLDatumStatsDoubleMappable(
		mp, mdid, type_modifier, is_null, byte_array, length, double_value);
}


//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::CreateDXLDatumStatsIntMappable
//
//	@doc:
// 		Create a dxl datum of types having lint mapping
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeGenericSPQDB::CreateDXLDatumStatsIntMappable(
	CMemoryPool *mp, IMDId *mdid, INT type_modifier, BOOL is_null,
	BYTE *byte_array, ULONG length, LINT lint_value,
	CDouble	 // double_value
)
{
	return SPQOS_NEW(mp) CDXLDatumStatsLintMappable(
		mp, mdid, type_modifier, is_null, byte_array, length, lint_value);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::GetDXLOpScConst
//
//	@doc:
// 		Generate a dxl scalar constant from a datum
//
//---------------------------------------------------------------------------
CDXLScalarConstValue *
CMDTypeGenericSPQDB::GetDXLOpScConst(CMemoryPool *mp, IDatum *datum) const
{
	CDXLDatum *dxl_datum = GetDatumVal(mp, datum);

	return SPQOS_NEW(mp) CDXLScalarConstValue(mp, dxl_datum);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::GetDXLDatumNull
//
//	@doc:
// 		Generate dxl datum
//
//---------------------------------------------------------------------------
CDXLDatum *
CMDTypeGenericSPQDB::GetDXLDatumNull(CMemoryPool *mp) const
{
	m_mdid->AddRef();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDType *md_type = md_accessor->RetrieveType(m_mdid);
	return CreateDXLDatumVal(mp, m_mdid, md_type, default_type_modifier,
							 true /*fConstNull*/, NULL /*byte_array*/,
							 0 /*length*/, 0 /*lint_value */,
							 0 /*double_value */);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::HasByte2IntMapping
//
//	@doc:
//		Does the datum of this type need bytea -> Lint mapping for
//		statistics computation
//---------------------------------------------------------------------------
BOOL
CMDTypeGenericSPQDB::HasByte2IntMapping(const IMDType *mdtype)
{
	IMDId *mdid = mdtype->MDId();
	return mdtype->IsTextRelated() || mdid->Equals(&CMDIdSPQDB::m_mdid_uuid) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_cash) ||
		   IsTimeRelatedTypeMappableToLint(mdid);
}

IDatum *
CMDTypeGenericSPQDB::CreateGenericNullDatum(CMemoryPool *mp,
										   INT type_modifier) const
{
	return SPQOS_NEW(mp) CDatumGenericSPQDB(mp, MDId(), type_modifier,
										  NULL,	 // source value buffer
										  0,	 // source value buffer length
										  true,	 // is NULL
										  0,	 // LINT mapping for stats
										  0.0);	 // CDouble mapping for stats
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::HasByte2DoubleMapping
//
//	@doc:
//		Does the datum of this type need bytea -> double mapping for
//		statistics computation
//---------------------------------------------------------------------------
BOOL
CMDTypeGenericSPQDB::HasByte2DoubleMapping(const IMDId *mdid)
{
	return mdid->Equals(&CMDIdSPQDB::m_mdid_numeric) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_float4) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_float8) ||
		   IsTimeRelatedTypeMappableToDouble(mdid) ||
		   IsNetworkRelatedType(mdid);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::IsTimeRelatedType
//
//	@doc:
//		is this a time-related type
//---------------------------------------------------------------------------
BOOL
CMDTypeGenericSPQDB::IsTimeRelatedType(const IMDId *mdid)
{
	return mdid->Equals(&CMDIdSPQDB::m_mdid_date) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_time) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_timeTz) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_timestamp) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_timestampTz) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_abs_time) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_relative_time) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_interval) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_time_interval);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::IsNetworkRelatedType
//
//	@doc:
//		is this a network-related type
//---------------------------------------------------------------------------
BOOL
CMDTypeGenericSPQDB::IsNetworkRelatedType(const IMDId *mdid)
{
	return mdid->Equals(&CMDIdSPQDB::m_mdid_inet) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_cidr) ||
		   mdid->Equals(&CMDIdSPQDB::m_mdid_macaddr);
}

IMDId *
CMDTypeGenericSPQDB::GetDistrOpfamilyMdid() const
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

BOOL
CMDTypeGenericSPQDB::IsRedistributable() const
{
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution) &&
		SPQOS_FTRACE(EopttraceUseLegacyOpfamilies))
	{
		return (NULL != m_legacy_distr_opfamily);
	}
	// If EopttraceConsiderOpfamiliesForDistribution is set, m_is_redistributable
	// is redundant. It's still used here for MDP tests where the traceflag is
	// unset and/or opfamilies are not available for all types.
	return m_is_redistributable;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDTypeGenericSPQDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDTypeGenericSPQDB::DebugPrint(IOstream &os) const
{
	SPQDBTypeHelper<CMDTypeGenericSPQDB>::DebugPrint(os, this);
}

#endif	// SPQOS_DEBUG

// EOF
