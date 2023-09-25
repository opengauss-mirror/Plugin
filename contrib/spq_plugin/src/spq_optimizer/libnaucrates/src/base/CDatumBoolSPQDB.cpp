//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumBoolSPQDB.cpp
//
//	@doc:
//		Implementation of SPQDB bool
//---------------------------------------------------------------------------

#include "naucrates/base/CDatumBoolSPQDB.h"

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqnaucrates;
using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::CDatumBoolSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumBoolSPQDB::CDatumBoolSPQDB(CSystemId sysid, BOOL value, BOOL is_null)
	: m_value(value), m_is_null(is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = dynamic_cast<const CMDTypeBoolSPQDB *>(
					  md_accessor->PtMDType<IMDTypeBool>(sysid))
					  ->MDId();
	mdid->AddRef();

	m_mdid = mdid;

	if (IsNull())
	{
		// needed for hash computation
		m_value = false;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::CDatumBoolSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumBoolSPQDB::CDatumBoolSPQDB(IMDId *mdid, BOOL value, BOOL is_null)
	: m_mdid(mdid), m_value(value), m_is_null(is_null)
{
	SPQOS_ASSERT(NULL != m_mdid);
	SPQOS_ASSERT(SPQDB_BOOL_OID == CMDIdSPQDB::CastMdid(m_mdid)->Oid());

	if (IsNull())
	{
		// needed for hash computation
		m_value = false;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::~CDatumBoolSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDatumBoolSPQDB::~CDatumBoolSPQDB()
{
	m_mdid->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::MakeCopyOfValue
//
//	@doc:
//		Accessor of boolean value
//
//---------------------------------------------------------------------------
BOOL
CDatumBoolSPQDB::GetValue() const
{
	return m_value;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::IsNull
//
//	@doc:
//		Accessor of is null
//
//---------------------------------------------------------------------------
BOOL
CDatumBoolSPQDB::IsNull() const
{
	return m_is_null;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::Size
//
//	@doc:
//		Accessor of size
//
//---------------------------------------------------------------------------
ULONG
CDatumBoolSPQDB::Size() const
{
	return 1;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::MDId
//
//	@doc:
//		Accessor of type information (MDId)
//
//---------------------------------------------------------------------------
IMDId *
CDatumBoolSPQDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDatumBoolSPQDB::HashValue() const
{
	return spqos::CombineHashes(m_mdid->HashValue(),
							   spqos::HashValue<BOOL>(&m_value));
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::GetMDName
//
//	@doc:
//		Return string representation
//
//---------------------------------------------------------------------------
const CWStringConst *
CDatumBoolSPQDB::GetStrRepr(CMemoryPool *mp) const
{
	CWStringDynamic str(mp);
	if (!IsNull())
	{
		str.AppendFormat(SPQOS_WSZ_LIT("%d"), m_value);
	}
	else
	{
		str.AppendFormat(SPQOS_WSZ_LIT("null"));
	}

	return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::Matches
//
//	@doc:
//		Matches the values of datums
//
//---------------------------------------------------------------------------
BOOL
CDatumBoolSPQDB::Matches(const IDatum *other) const
{
	if (!other->MDId()->Equals(m_mdid))
	{
		return false;
	}

	const CDatumBoolSPQDB *other_cast =
		dynamic_cast<const CDatumBoolSPQDB *>(other);

	if (!other_cast->IsNull() && !IsNull())
	{
		return (other_cast->GetValue() == GetValue());
	}

	if (other_cast->IsNull() && IsNull())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::MakeCopy
//
//	@doc:
//		Returns a copy of the datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumBoolSPQDB::MakeCopy(CMemoryPool *mp) const
{
	m_mdid->AddRef();
	return SPQOS_NEW(mp) CDatumBoolSPQDB(m_mdid, m_value, m_is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumBoolSPQDB::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CDatumBoolSPQDB::OsPrint(IOstream &os) const
{
	if (!IsNull())
	{
		os << m_value;
	}
	else
	{
		os << "null";
	}

	return os;
}

// EOF
