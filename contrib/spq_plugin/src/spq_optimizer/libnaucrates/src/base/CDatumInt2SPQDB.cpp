//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CDatumInt2SPQDB.cpp
//
//	@doc:
//		Implementation of SPQDB int2
//---------------------------------------------------------------------------

#include "naucrates/base/CDatumInt2SPQDB.h"

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDTypeInt2.h"

using namespace spqnaucrates;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::CDatumInt2SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumInt2SPQDB::CDatumInt2SPQDB(CSystemId sysid, SINT val, BOOL is_null)
	: m_mdid(NULL), m_val(val), m_is_null(is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = dynamic_cast<const CMDTypeInt2SPQDB *>(
					  md_accessor->PtMDType<IMDTypeInt2>(sysid))
					  ->MDId();
	mdid->AddRef();

	m_mdid = mdid;

	if (IsNull())
	{
		// needed for hash computation
		m_val = spqos::sint_max;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::CDatumInt2SPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumInt2SPQDB::CDatumInt2SPQDB(IMDId *mdid, SINT val, BOOL is_null)
	: m_mdid(mdid), m_val(val), m_is_null(is_null)
{
	SPQOS_ASSERT(NULL != m_mdid);
	SPQOS_ASSERT(SPQDB_INT2_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());

	if (IsNull())
	{
		// needed for hash computation
		m_val = spqos::sint_max;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::~CDatumInt2SPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDatumInt2SPQDB::~CDatumInt2SPQDB()
{
	m_mdid->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::Value
//
//	@doc:
//		Accessor of integer value
//
//---------------------------------------------------------------------------
SINT
CDatumInt2SPQDB::Value() const
{
	return m_val;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::IsNull
//
//	@doc:
//		Accessor of is null
//
//---------------------------------------------------------------------------
BOOL
CDatumInt2SPQDB::IsNull() const
{
	return m_is_null;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::Size
//
//	@doc:
//		Accessor of size
//
//---------------------------------------------------------------------------
ULONG
CDatumInt2SPQDB::Size() const
{
	return 2;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::MDId
//
//	@doc:
//		Accessor of type information
//
//---------------------------------------------------------------------------
IMDId *
CDatumInt2SPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDatumInt2SPQDB::HashValue() const
{
	return spqos::CombineHashes(m_mdid->HashValue(),
							   spqos::HashValue<SINT>(&m_val));
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::GetMDName
//
//	@doc:
//		Return string representation
//
//---------------------------------------------------------------------------
const CWStringConst *
CDatumInt2SPQDB::GetStrRepr(CMemoryPool *mp) const
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
//		CDatumInt2SPQDB::Matches
//
//	@doc:
//		Matches the values of datums
//
//---------------------------------------------------------------------------
BOOL
CDatumInt2SPQDB::Matches(const IDatum *datum) const
{
	if (!datum->MDId()->Equals(m_mdid))
	{
		return false;
	}

	const CDatumInt2SPQDB *datum_cast =
		dynamic_cast<const CDatumInt2SPQDB *>(datum);

	if (!datum_cast->IsNull() && !IsNull())
	{
		return (datum_cast->Value() == Value());
	}

	return datum_cast->IsNull() && IsNull();
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::MakeCopy
//
//	@doc:
//		Returns a copy of the datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumInt2SPQDB::MakeCopy(CMemoryPool *mp) const
{
	m_mdid->AddRef();
	return SPQOS_NEW(mp) CDatumInt2SPQDB(m_mdid, m_val, m_is_null);
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumInt2SPQDB::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CDatumInt2SPQDB::OsPrint(IOstream &os) const
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
