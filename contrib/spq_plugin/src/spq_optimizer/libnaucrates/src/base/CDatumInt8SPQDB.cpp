//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDatumInt8SPQDB.cpp
//
//	@doc:
//		Implementation of SPQDB Int8
//---------------------------------------------------------------------------

#include "naucrates/base/CDatumInt8SPQDB.h"

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDTypeInt8.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::CDatumInt8SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumInt8SPQDB::CDatumInt8SPQDB(CSystemId sysid, LINT val, BOOL is_null)
	: m_mdid(NULL), m_val(val), m_is_null(is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = dynamic_cast<const CMDTypeInt8SPQDB *>(
					  md_accessor->PtMDType<IMDTypeInt8>(sysid))
					  ->MDId();
	mdid->AddRef();
	m_mdid = mdid;

	if (IsNull())
	{
		// needed for hash computation
		m_val = LINT(spqos::ulong_max >> 1);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::CDatumInt8SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumInt8SPQDB::CDatumInt8SPQDB(IMDId *mdid, LINT val, BOOL is_null)
	: m_mdid(mdid), m_val(val), m_is_null(is_null)
{
	SPQOS_ASSERT(NULL != m_mdid);
	SPQOS_ASSERT(SPQDB_INT8_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());

	if (IsNull())
	{
		// needed for hash computation
		m_val = LINT(spqos::ulong_max >> 1);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::~CDatumInt8SPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDatumInt8SPQDB::~CDatumInt8SPQDB()
{
	m_mdid->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::Value
//
//	@doc:
//		Accessor of integer value
//
//---------------------------------------------------------------------------
LINT
CDatumInt8SPQDB::Value() const
{
	return m_val;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::IsNull
//
//	@doc:
//		Accessor of is null
//
//---------------------------------------------------------------------------
BOOL
CDatumInt8SPQDB::IsNull() const
{
	return m_is_null;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::Size
//
//	@doc:
//		Accessor of size
//
//---------------------------------------------------------------------------
ULONG
CDatumInt8SPQDB::Size() const
{
	return 8;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::MDId
//
//	@doc:
//		Accessor of type information
//
//---------------------------------------------------------------------------
IMDId *
CDatumInt8SPQDB::MDId() const
{
	return m_mdid;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDatumInt8SPQDB::HashValue() const
{
	return spqos::CombineHashes(m_mdid->HashValue(),
							   spqos::HashValue<LINT>(&m_val));
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::GetMDName
//
//	@doc:
//		Return string representation
//
//---------------------------------------------------------------------------
const CWStringConst *
CDatumInt8SPQDB::GetStrRepr(CMemoryPool *mp) const
{
	CWStringDynamic str(mp);
	if (!IsNull())
	{
		str.AppendFormat(SPQOS_WSZ_LIT("%ld"), m_val);
	}
	else
	{
		str.AppendFormat(SPQOS_WSZ_LIT("null"));
	}

	return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::Matches
//
//	@doc:
//		Matches the values of datums
//
//---------------------------------------------------------------------------
BOOL
CDatumInt8SPQDB::Matches(const IDatum *datum) const
{
	if (!m_mdid->Equals(datum->MDId()))
	{
		return false;
	}

	const CDatumInt8SPQDB *datum_cast =
		dynamic_cast<const CDatumInt8SPQDB *>(datum);

	if (!datum_cast->IsNull() && !IsNull())
	{
		return (datum_cast->Value() == Value());
	}

	if (datum_cast->IsNull() && IsNull())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::MakeCopy
//
//	@doc:
//		Returns a copy of the datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumInt8SPQDB::MakeCopy(CMemoryPool *mp) const
{
	m_mdid->AddRef();
	return SPQOS_NEW(mp) CDatumInt8SPQDB(m_mdid, m_val, m_is_null);
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt8SPQDB::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CDatumInt8SPQDB::OsPrint(IOstream &os) const
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
