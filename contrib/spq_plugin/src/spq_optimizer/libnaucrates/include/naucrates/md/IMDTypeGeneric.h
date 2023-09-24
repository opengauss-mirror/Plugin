//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		IMDTypeGeneric.h
//
//	@doc:
//		Interface for types in the metadata cache
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDTypeGeneric_H
#define SPQMD_IMDTypeGeneric_H

#include "spqos/base.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/IMDType.h"

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		IMDTypeGeneric
//
//	@doc:
//		Interface for generic types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeGeneric : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiGeneric;
	}

	// type id
	virtual ETypeInfo
	GetDatumType() const
	{
		return IMDTypeGeneric::GetTypeInfo();
	}

	virtual IDatum *CreateGenericNullDatum(CMemoryPool *mp,
										   INT type_modifier) const = 0;
};
}  // namespace spqmd

#endif	// !SPQMD_IMDTypeGeneric_H

// EOF
