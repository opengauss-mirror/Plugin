//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDIdSPQDB.cpp
//
//	@doc:
//		Implementation of metadata identifiers
//---------------------------------------------------------------------------

#include "naucrates/md/CMDIdSPQDB.h"

#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqmd;


// initialize static members
// invalid key
CMDIdSPQDB CMDIdSPQDB::m_mdid_invalid_key(IMDId::EmdidGeneral, 0, 0, 0);

// int2 mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_int2(IMDId::EmdidGeneral, SPQDB_INT2);

// int4 mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_int4(IMDId::EmdidGeneral, SPQDB_INT4);

// int8 mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_int8(IMDId::EmdidGeneral, SPQDB_INT8);

// bool mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_bool(IMDId::EmdidGeneral, SPQDB_BOOL);

// oid mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_oid(IMDId::EmdidGeneral, SPQDB_OID);

// numeric mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_numeric(IMDId::EmdidGeneral, SPQDB_NUMERIC);

// date mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_date(IMDId::EmdidGeneral, SPQDB_DATE);

// time mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_time(IMDId::EmdidGeneral, SPQDB_TIME);

// time with time zone mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_timeTz(IMDId::EmdidGeneral, SPQDB_TIMETZ);

// timestamp mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_timestamp(IMDId::EmdidGeneral, SPQDB_TIMESTAMP);

// timestamp with time zone mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_timestampTz(IMDId::EmdidGeneral, SPQDB_TIMESTAMPTZ);

// absolute time mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_abs_time(IMDId::EmdidGeneral, SPQDB_ABSTIME);

// relative time mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_relative_time(IMDId::EmdidGeneral, SPQDB_RELTIME);

// interval mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_interval(IMDId::EmdidGeneral, SPQDB_INTERVAL);

// time interval mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_time_interval(IMDId::EmdidGeneral,
										  SPQDB_TIMEINTERVAL);

// char mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_char(IMDId::EmdidGeneral, SPQDB_SINGLE_CHAR);

// bpchar mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_bpchar(IMDId::EmdidGeneral, SPQDB_CHAR);

// varchar mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_varchar(IMDId::EmdidGeneral, SPQDB_VARCHAR);

// text mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_text(IMDId::EmdidGeneral, SPQDB_TEXT);

// text mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_name(IMDId::EmdidGeneral, SPQDB_NAME);

// float4 mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_float4(IMDId::EmdidGeneral, SPQDB_FLOAT4);

// float8 mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_float8(IMDId::EmdidGeneral, SPQDB_FLOAT8);

// cash mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_cash(IMDId::EmdidGeneral, SPQDB_CASH);

// inet mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_inet(IMDId::EmdidGeneral, SPQDB_INET);

// cidr mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_cidr(IMDId::EmdidGeneral, SPQDB_CIDR);

// macaddr mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_macaddr(IMDId::EmdidGeneral, SPQDB_MACADDR);

// count(*) mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_count_star(IMDId::EmdidGeneral, SPQDB_COUNT_STAR);

// count(Any) mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_count_any(IMDId::EmdidGeneral, SPQDB_COUNT_ANY);

// uuid mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_uuid(IMDId::EmdidGeneral, SPQDB_UUID);

// unknown mdid
CMDIdSPQDB CMDIdSPQDB::m_mdid_unknown(IMDId::EmdidGeneral, SPQDB_UNKNOWN);

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::CMDIdSPQDB
//
//	@doc:
//		Constructs a metadata identifier with specified oid and default version
//		of 1.0
//
//---------------------------------------------------------------------------
CMDIdSPQDB::CMDIdSPQDB(CSystemId sysid, OID oid)
	: m_sysid(sysid),
	  m_oid(oid),
	  m_major_version(1),
	  m_minor_version(0),
	  m_str(m_mdid_array, SPQOS_ARRAY_SIZE(m_mdid_array))
{
	if (CMDIdSPQDB::m_mdid_invalid_key.Oid() == oid)
	{
		// construct an invalid mdid 0.0.0
		m_major_version = 0;
	}

	// serialize mdid into static string
	Serialize();
}

CMDIdSPQDB::CMDIdSPQDB(EMDIdType mdIdType, OID oid, ULONG version_major,
					 ULONG version_minor)
	: m_sysid(mdIdType, SPQMD_SPQDB_SYSID),
	  m_oid(oid),
	  m_major_version(version_major),
	  m_minor_version(version_minor),
	  m_str(m_mdid_array, SPQOS_ARRAY_SIZE(m_mdid_array))
{
	SPQOS_RTL_ASSERT(
		mdIdType == IMDId::EmdidGeneral ||
		(mdIdType >= IMDId::EmdidRel && mdIdType < IMDId::EmdidSentinel));
	if (CMDIdSPQDB::m_mdid_invalid_key.Oid() == oid)
	{
		// construct an invalid mdid 0.0.0
		m_major_version = 0;
	}
	// serialize mdid into static string
	Serialize();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::CMDIdSPQDB
//
//	@doc:
//		Copy constructor
//
//---------------------------------------------------------------------------
CMDIdSPQDB::CMDIdSPQDB(const CMDIdSPQDB &mdid_source)
	: IMDId(),
	  m_sysid(mdid_source.Sysid()),
	  m_oid(mdid_source.Oid()),
	  m_major_version(mdid_source.VersionMajor()),
	  m_minor_version(mdid_source.VersionMinor()),
	  m_str(m_mdid_array, SPQOS_ARRAY_SIZE(m_mdid_array))
{
	SPQOS_ASSERT(mdid_source.IsValid());

	// serialize mdid into static string
	Serialize();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::Serialize
//
//	@doc:
//		Serialize mdid into static string
//
//---------------------------------------------------------------------------
void
CMDIdSPQDB::Serialize()
{
	m_str.Reset();
	// serialize mdid as SystemType.Oid.Major.Minor
	m_str.AppendFormat(SPQOS_WSZ_LIT("%d.%d.%d.%d"), MdidType(), m_oid,
					   m_major_version, m_minor_version);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::GetBuffer
//
//	@doc:
//		Returns the string representation of the mdid
//
//---------------------------------------------------------------------------
const WCHAR *
CMDIdSPQDB::GetBuffer() const
{
	return m_str.GetBuffer();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::Oid
//
//	@doc:
//		Returns the object id
//
//---------------------------------------------------------------------------
OID
CMDIdSPQDB::Oid() const
{
	return m_oid;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::VersionMajor
//
//	@doc:
//		Returns the object's major version
//
//---------------------------------------------------------------------------
ULONG
CMDIdSPQDB::VersionMajor() const
{
	return m_major_version;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::VersionMinor
//
//	@doc:
//		Returns the object's minor version
//
//---------------------------------------------------------------------------
ULONG
CMDIdSPQDB::VersionMinor() const
{
	return m_minor_version;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::Equals
//
//	@doc:
//		Checks if the version of the current object is compatible with another version
//		of the same object
//
//---------------------------------------------------------------------------
BOOL
CMDIdSPQDB::Equals(const IMDId *mdid) const
{
	if (NULL == mdid || MdidType() != mdid->MdidType())
	{
		return false;
	}

	const CMDIdSPQDB *mdidSPQDB =
		static_cast<CMDIdSPQDB *>(const_cast<IMDId *>(mdid));

	return (m_oid == mdidSPQDB->Oid() &&
			m_major_version == mdidSPQDB->VersionMajor() &&
			m_minor_version == mdidSPQDB->VersionMinor());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::IsValid
//
//	@doc:
//		Is the mdid valid
//
//---------------------------------------------------------------------------
BOOL
CMDIdSPQDB::IsValid() const
{
	const CMDIdSPQDB *invalid = &CMDIdSPQDB::m_mdid_invalid_key;
	return !(m_oid == invalid->Oid() &&
			 m_major_version == invalid->VersionMajor() &&
			 m_minor_version == invalid->VersionMinor());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::Serialize
//
//	@doc:
//		Serializes the mdid as the value of the given attribute
//
//---------------------------------------------------------------------------
void
CMDIdSPQDB::Serialize(CXMLSerializer *xml_serializer,
					 const CWStringConst *attribute_str) const
{
	xml_serializer->AddAttribute(attribute_str, &m_str);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDB::OsPrint
//
//	@doc:
//		Debug print of the id in the provided stream
//
//---------------------------------------------------------------------------
IOstream &
CMDIdSPQDB::OsPrint(IOstream &os) const
{
	os << "(" << m_str.GetBuffer() << ")";
	return os;
}

// EOF
