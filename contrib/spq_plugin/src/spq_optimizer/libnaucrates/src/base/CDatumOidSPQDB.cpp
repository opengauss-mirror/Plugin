//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDatumOidSPQDB.cpp
//
//	@doc:
//		Implementation of SPQDB oid datum
//---------------------------------------------------------------------------

#include "naucrates/base/CDatumOidSPQDB.h"

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDTypeOidSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDTypeOid.h"

using namespace spqnaucrates;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::CDatumOidSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumOidSPQDB::CDatumOidSPQDB(CSystemId sysid, OID oid_val, BOOL is_null)
	: m_mdid(NULL), m_val(oid_val), m_is_null(is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = dynamic_cast<const CMDTypeOidSPQDB *>(
					  md_accessor->PtMDType<IMDTypeOid>(sysid))
					  ->MDId();
	mdid->AddRef();

	m_mdid = mdid;

	if (IsNull())
	{
		// needed for hash computation
		m_val = spqos::int_max;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::CDatumOidSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumOidSPQDB::CDatumOidSPQDB(IMDId *mdid, OID oid_val, BOOL is_null)
	: m_mdid(mdid), m_val(oid_val), m_is_null(is_null)
{
	SPQOS_ASSERT(NULL != m_mdid);
	SPQOS_ASSERT(SPQDB_OID_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());

	if (IsNull())
	{
		// needed for hash computation
		m_val = spqos::int_max;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::~CDatumOidSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDatumOidSPQDB::~CDatumOidSPQDB()
{
	m_mdid->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::OidValue
//
//	@doc:
//		Accessor of oid value
//
//---------------------------------------------------------------------------
OID
CDatumOidSPQDB::OidValue() const
{
	return m_val;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::IsNull
//
//	@doc:
//		Accessor of is null
//
//---------------------------------------------------------------------------
BOOL
CDatumOidSPQDB::IsNull() const
{
	return m_is_null;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::Size
//
//	@doc:
//		Accessor of size
//
//---------------------------------------------------------------------------
ULONG
CDatumOidSPQDB::Size() const
{
	return 4;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::MDId
//
//	@doc:
//		Accessor of type information
//
//---------------------------------------------------------------------------
IMDId *
CDatumOidSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDatumOidSPQDB::HashValue() const
{
	return spqos::CombineHashes(m_mdid->HashValue(),
							   spqos::HashValue<OID>(&m_val));
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::GetMDName
//
//	@doc:
//		Return string representation
//
//---------------------------------------------------------------------------
const CWStringConst *
CDatumOidSPQDB::GetStrRepr(CMemoryPool *mp) const
{
	CWStringDynamic str(mp);
	if (!IsNull())
	{
		str.AppendFormat(SPQOS_WSZ_LIT("%d"), m_val);
	}
	else
	{
		str.AppendFormat(SPQOS_WSZ_LIT("null"));
	}

	return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::Matches
//
//	@doc:
//		Matches the values of datums
//
//---------------------------------------------------------------------------
BOOL
CDatumOidSPQDB::Matches(const IDatum *datum) const
{
	if (!datum->MDId()->Equals(m_mdid))
	{
		return false;
	}

	const CDatumOidSPQDB *datum_cast =
		dynamic_cast<const CDatumOidSPQDB *>(datum);

	if (!datum_cast->IsNull() && !IsNull())
	{
		return (datum_cast->OidValue() == OidValue());
	}

	if (datum_cast->IsNull() && IsNull())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::MakeCopy
//
//	@doc:
//		Returns a copy of the datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumOidSPQDB::MakeCopy(CMemoryPool *mp) const
{
	m_mdid->AddRef();
	return SPQOS_NEW(mp) CDatumOidSPQDB(m_mdid, m_val, m_is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumOidSPQDB::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CDatumOidSPQDB::OsPrint(IOstream &os) const
{
	if (!IsNull())
	{
		os << m_val;
	}
	else
	{
		os << "null";
	}

	return os;
}

// EOF
