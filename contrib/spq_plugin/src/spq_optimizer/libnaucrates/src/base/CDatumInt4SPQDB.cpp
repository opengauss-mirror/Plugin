//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumInt4SPQDB.cpp
//
//	@doc:
//		Implementation of SPQDB int4
//---------------------------------------------------------------------------

#include "naucrates/base/CDatumInt4SPQDB.h"

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDTypeInt4.h"

using namespace spqnaucrates;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::CDatumInt4SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumInt4SPQDB::CDatumInt4SPQDB(CSystemId sysid, INT val, BOOL is_null)
	: m_mdid(NULL), m_val(val), m_is_null(is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = dynamic_cast<const CMDTypeInt4SPQDB *>(
					  md_accessor->PtMDType<IMDTypeInt4>(sysid))
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
//		CDatumInt4SPQDB::CDatumInt4SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumInt4SPQDB::CDatumInt4SPQDB(IMDId *mdid, INT val, BOOL is_null)
	: m_mdid(mdid), m_val(val), m_is_null(is_null)
{
	SPQOS_ASSERT(NULL != m_mdid);
	SPQOS_ASSERT(SPQDB_INT4_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());

	if (IsNull())
	{
		// needed for hash computation
		m_val = spqos::int_max;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::~CDatumInt4SPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDatumInt4SPQDB::~CDatumInt4SPQDB()
{
	m_mdid->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::Value
//
//	@doc:
//		Accessor of integer value
//
//---------------------------------------------------------------------------
INT
CDatumInt4SPQDB::Value() const
{
	return m_val;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::IsNull
//
//	@doc:
//		Accessor of is null
//
//---------------------------------------------------------------------------
BOOL
CDatumInt4SPQDB::IsNull() const
{
	return m_is_null;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::Size
//
//	@doc:
//		Accessor of size
//
//---------------------------------------------------------------------------
ULONG
CDatumInt4SPQDB::Size() const
{
	return 4;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::MDId
//
//	@doc:
//		Accessor of type information
//
//---------------------------------------------------------------------------
IMDId *
CDatumInt4SPQDB::MDId() const
{
	return m_mdid;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDatumInt4SPQDB::HashValue() const
{
	return spqos::CombineHashes(m_mdid->HashValue(),
							   spqos::HashValue<INT>(&m_val));
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::GetMDName
//
//	@doc:
//		Return string representation
//
//---------------------------------------------------------------------------
const CWStringConst *
CDatumInt4SPQDB::GetStrRepr(CMemoryPool *mp) const
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
//		CDatumInt4SPQDB::Matches
//
//	@doc:
//		Matches the values of datums
//
//---------------------------------------------------------------------------
BOOL
CDatumInt4SPQDB::Matches(const IDatum *datum) const
{
	if (!datum->MDId()->Equals(m_mdid))
	{
		return false;
	}

	const CDatumInt4SPQDB *datum_cast =
		dynamic_cast<const CDatumInt4SPQDB *>(datum);

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
//		CDatumInt4SPQDB::MakeCopy
//
//	@doc:
//		Returns a copy of the datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumInt4SPQDB::MakeCopy(CMemoryPool *mp) const
{
	m_mdid->AddRef();
	return SPQOS_NEW(mp) CDatumInt4SPQDB(m_mdid, m_val, m_is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt4SPQDB::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CDatumInt4SPQDB::OsPrint(IOstream &os) const
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
