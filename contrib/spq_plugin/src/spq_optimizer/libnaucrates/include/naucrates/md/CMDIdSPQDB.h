//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDIdSPQDB.h
//
//	@doc:
//		Class for representing id and version of metadata objects in SPQDB
//---------------------------------------------------------------------------



#ifndef SPQMD_CMDIdSPQDB_H
#define SPQMD_CMDIdSPQDB_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/string/CWStringConst.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CSystemId.h"
#include "naucrates/md/IMDId.h"


// TODO:  - Feb 1, 2012; remove once system id is part of the mdid
#define SPQMD_SPQDB_SYSID SPQOS_WSZ_LIT("SPQDB")

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		CMDIdSPQDB
//
//	@doc:
//		Class for representing ids of SPQDB metadata objects
//
//---------------------------------------------------------------------------
class CMDIdSPQDB : public IMDId
{
protected:
	// source system id
	CSystemId m_sysid;

	// id from SPQDB system catalog
	OID m_oid;

	// major version number
	ULONG m_major_version;

	// minor version number
	ULONG m_minor_version;

	// buffer for the serialized mdid
	WCHAR m_mdid_array[SPQDXL_MDID_LENGTH];

	// string representation of the mdid
	CWStringStatic m_str;

	// serialize mdid
	virtual void Serialize();

public:
	// ctors
	CMDIdSPQDB(CSystemId sysid, OID oid);
	CMDIdSPQDB(EMDIdType mdIdType, OID oid, ULONG version_major = 1,
			  ULONG version_minor = 0);

	// copy ctor
	explicit CMDIdSPQDB(const CMDIdSPQDB &mdidSource);

	virtual EMDIdType
	MdidType() const
	{
		return m_sysid.MdidType();
	}

	// string representation of mdid
	virtual const WCHAR *GetBuffer() const;

	// source system id
	virtual CSystemId
	Sysid() const
	{
		return m_sysid;
	}

	// oid
	virtual OID Oid() const;

	// major version
	virtual ULONG VersionMajor() const;

	// minor version
	virtual ULONG VersionMinor() const;

	// equality check
	virtual BOOL Equals(const IMDId *mdid) const;

	// computes the hash value for the metadata id
	virtual ULONG
	HashValue() const
	{
		return spqos::CombineHashes(
			MdidType(),
			spqos::CombineHashes(
				spqos::HashValue(&m_oid),
				spqos::CombineHashes(spqos::HashValue(&m_major_version),
									spqos::HashValue(&m_minor_version))));
	}

	// is the mdid valid
	virtual BOOL IsValid() const;

	// serialize mdid in DXL as the value of the specified attribute
	virtual void Serialize(CXMLSerializer *xml_serializer,
						   const CWStringConst *pstrAttribute) const;

	// debug print of the metadata id
	virtual IOstream &OsPrint(IOstream &os) const;

	// const converter
	static const CMDIdSPQDB *
	CastMdid(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid);

		return dynamic_cast<const CMDIdSPQDB *>(mdid);
	}

	// non-const converter
	static CMDIdSPQDB *
	CastMdid(IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid);

		return dynamic_cast<CMDIdSPQDB *>(mdid);
	}

	// invalid mdid
	static CMDIdSPQDB m_mdid_invalid_key;

	// int2 mdid
	static CMDIdSPQDB m_mdid_int2;

	// int4 mdid
	static CMDIdSPQDB m_mdid_int4;

	// int8 mdid
	static CMDIdSPQDB m_mdid_int8;

	// oid mdid
	static CMDIdSPQDB m_mdid_oid;

	// bool mdid
	static CMDIdSPQDB m_mdid_bool;

	// numeric mdid
	static CMDIdSPQDB m_mdid_numeric;

	// date mdid
	static CMDIdSPQDB m_mdid_date;

	// time mdid
	static CMDIdSPQDB m_mdid_time;

	// time with time zone mdid
	static CMDIdSPQDB m_mdid_timeTz;

	// timestamp mdid
	static CMDIdSPQDB m_mdid_timestamp;

	// timestamp with time zone mdid
	static CMDIdSPQDB m_mdid_timestampTz;

	// absolute time mdid
	static CMDIdSPQDB m_mdid_abs_time;

	// relative time mdid
	static CMDIdSPQDB m_mdid_relative_time;

	// interval mdid
	static CMDIdSPQDB m_mdid_interval;

	// time interval mdid
	static CMDIdSPQDB m_mdid_time_interval;

	// char mdid
	static CMDIdSPQDB m_mdid_char;

	// bpchar mdid
	static CMDIdSPQDB m_mdid_bpchar;

	// varchar mdid
	static CMDIdSPQDB m_mdid_varchar;

	// text mdid
	static CMDIdSPQDB m_mdid_text;

	// name mdid
	static CMDIdSPQDB m_mdid_name;

	// float4 mdid
	static CMDIdSPQDB m_mdid_float4;

	// float8 mdid
	static CMDIdSPQDB m_mdid_float8;

	// cash mdid
	static CMDIdSPQDB m_mdid_cash;

	// inet mdid
	static CMDIdSPQDB m_mdid_inet;

	// cidr mdid
	static CMDIdSPQDB m_mdid_cidr;

	// macaddr mdid
	static CMDIdSPQDB m_mdid_macaddr;

	// count(*) mdid
	static CMDIdSPQDB m_mdid_count_star;

	// count(Any) mdid
	static CMDIdSPQDB m_mdid_count_any;

	// uuid mdid
	static CMDIdSPQDB m_mdid_uuid;

	// unknown datatype mdid
	static CMDIdSPQDB m_mdid_unknown;
};

}  // namespace spqmd



#endif	// !SPQMD_CMDIdSPQDB_H

// EOF
