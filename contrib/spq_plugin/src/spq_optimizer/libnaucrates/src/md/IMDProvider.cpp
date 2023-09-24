//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDProvider.cpp
//
//	@doc:
//		Abstract class for retrieving metadata from an external location
//---------------------------------------------------------------------------

#include "naucrates/md/IMDProvider.h"

#include "naucrates/md/CMDIdSPQDB.h"

using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		IMDProvider::GetSPQDBTypeMdid
//
//	@doc:
//		Return the mdid for the requested type
//
//---------------------------------------------------------------------------
IMDId *
IMDProvider::GetSPQDBTypeMdid(CMemoryPool *mp,
							 CSystemId
#ifdef SPQOS_DEBUG
								 sysid
#endif	// SPQOS_DEBUG
							 ,
							 IMDType::ETypeInfo type_info)
{
	SPQOS_ASSERT(IMDId::EmdidGeneral == sysid.MdidType());
	SPQOS_ASSERT(IMDType::EtiGeneric > type_info);

	switch (type_info)
	{
		case IMDType::EtiInt2:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT2);

		case IMDType::EtiInt4:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4);

		case IMDType::EtiInt8:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8);

		case IMDType::EtiBool:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_BOOL);

		case IMDType::EtiOid:
			return SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OID);

		default:
			return NULL;
	}
}

// EOF
