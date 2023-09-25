//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CMDIdSPQDBCtas.cpp
//
//	@doc:
//		Implementation of metadata identifiers for SPQDB CTAS queries
//---------------------------------------------------------------------------

#include "naucrates/md/CMDIdSPQDBCtas.h"

#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqmd;

#define SPQMD_SPQDB_CTAS_SYSID SPQOS_WSZ_LIT("CTAS")

// invalid key
CMDIdSPQDBCtas CMDIdSPQDBCtas::m_mdid_invalid_key(0);

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDBCtas::CMDIdSPQDBCtas
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDIdSPQDBCtas::CMDIdSPQDBCtas(OID oid)
	: CMDIdSPQDB(CSystemId(IMDId::EmdidGeneral, SPQMD_SPQDB_CTAS_SYSID), oid)
{
	Serialize();
}


//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDBCtas::CMDIdSPQDBCtas
//
//	@doc:
//		Copy constructor
//
//---------------------------------------------------------------------------
CMDIdSPQDBCtas::CMDIdSPQDBCtas(const CMDIdSPQDBCtas &mdid_source)
	: CMDIdSPQDB(mdid_source.Sysid(), mdid_source.Oid())
{
	SPQOS_ASSERT(mdid_source.IsValid());
	SPQOS_ASSERT(IMDId::EmdidSPQDBCtas == mdid_source.MdidType());
	Serialize();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDBCtas::Equals
//
//	@doc:
//		Checks if the version of the current object is compatible with another version
//		of the same object
//
//---------------------------------------------------------------------------
BOOL
CMDIdSPQDBCtas::Equals(const IMDId *mdid) const
{
	if (NULL == mdid || EmdidSPQDBCtas != mdid->MdidType())
	{
		return false;
	}

	const CMDIdSPQDBCtas *mdidSPQDBCTAS =
		CMDIdSPQDBCtas::CastMdid(const_cast<IMDId *>(mdid));

	return m_oid == mdidSPQDBCTAS->Oid();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDBCtas::IsValid
//
//	@doc:
//		Is the mdid valid
//
//---------------------------------------------------------------------------
BOOL
CMDIdSPQDBCtas::IsValid() const
{
	return !Equals(&CMDIdSPQDBCtas::m_mdid_invalid_key);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDIdSPQDBCtas::OsPrint
//
//	@doc:
//		Debug print of the id in the provided stream
//
//---------------------------------------------------------------------------
IOstream &
CMDIdSPQDBCtas::OsPrint(IOstream &os) const
{
	os << "(" << Oid() << "," << VersionMajor() << "." << VersionMinor() << ")";
	return os;
}

// EOF
