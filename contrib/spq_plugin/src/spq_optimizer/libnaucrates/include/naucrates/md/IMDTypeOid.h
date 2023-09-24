//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDTypeOid.h
//
//	@doc:
//		Interface for OID types in the metadata cache
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDTypeOid_H
#define SPQMD_IMDTypeOid_H

#include "spqos/base.h"

#include "naucrates/md/IMDType.h"

// fwd decl
namespace spqnaucrates
{
class IDatumOid;
}

namespace spqmd
{
using namespace spqos;
using namespace spqnaucrates;


//---------------------------------------------------------------------------
//	@class:
//		IMDTypeOid
//
//	@doc:
//		Interface for OID types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeOid : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiOid;
	}

	virtual ETypeInfo
	GetDatumType() const
	{
		return IMDTypeOid::GetTypeInfo();
	}

	// factory function for OID datums
	virtual IDatumOid *CreateOidDatum(CMemoryPool *mp, OID oid_value,
									  BOOL is_null) const = 0;
};
}  // namespace spqmd

#endif	// !SPQMD_IMDTypeOid_H

// EOF
