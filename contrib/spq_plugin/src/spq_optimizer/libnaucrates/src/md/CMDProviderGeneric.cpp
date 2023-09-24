//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDProviderGeneric.cpp
//
//	@doc:
//		Implementation of a generic MD provider.
//---------------------------------------------------------------------------

#include "naucrates/md/CMDProviderGeneric.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CMemoryPool.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/exception.h"
#include "naucrates/md/CDXLColStats.h"
#include "naucrates/md/CDXLRelStats.h"
#include "naucrates/md/CMDTypeBoolSPQDB.h"
#include "naucrates/md/CMDTypeInt2SPQDB.h"
#include "naucrates/md/CMDTypeInt4SPQDB.h"
#include "naucrates/md/CMDTypeInt8SPQDB.h"

using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderGeneric::CMDProviderGeneric
//
//	@doc:
//		Constructs a file-based metadata provider
//
//---------------------------------------------------------------------------
CMDProviderGeneric::CMDProviderGeneric(CMemoryPool *mp)
{
	// TODO:  - Jan 25, 2012; those should not be tied to a particular system
	m_mdid_int2 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2);
	m_mdid_int4 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4);
	m_mdid_int8 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8);
	m_mdid_bool = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL);
	m_mdid_oid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderGeneric::~CMDProviderGeneric
//
//	@doc:
//		Destructor
//
//---------------------------------------------------------------------------
CMDProviderGeneric::~CMDProviderGeneric()
{
	m_mdid_int2->Release();
	m_mdid_int4->Release();
	m_mdid_int8->Release();
	m_mdid_bool->Release();
	m_mdid_oid->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderGeneric::MDId
//
//	@doc:
//		return the mdid of a requested type
//
//---------------------------------------------------------------------------
IMDId *
CMDProviderGeneric::MDId(IMDType::ETypeInfo type_info) const
{
	SPQOS_ASSERT(IMDType::EtiGeneric > type_info);

	switch (type_info)
	{
		case IMDType::EtiInt2:
			return m_mdid_int2;

		case IMDType::EtiInt4:
			return m_mdid_int4;

		case IMDType::EtiInt8:
			return m_mdid_int8;

		case IMDType::EtiBool:
			return m_mdid_bool;

		case IMDType::EtiOid:
			return m_mdid_oid;

		default:
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderGeneric::SysidDefault
//
//	@doc:
//		Get the default system id of the MD provider
//
//---------------------------------------------------------------------------
CSystemId
CMDProviderGeneric::SysidDefault() const
{
	return CSystemId(IMDId::EmdidGeneral, SPQMD_SPQDB_SYSID);
}

// EOF
