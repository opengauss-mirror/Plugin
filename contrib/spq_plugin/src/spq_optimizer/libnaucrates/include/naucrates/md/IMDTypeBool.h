//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		IMDTypeBool.h
//
//	@doc:
//		Interface for BOOL types in the metadata cache
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDTypeBool_H
#define SPQMD_IMDTypeBool_H

#include "spqos/base.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/IMDType.h"

// fwd decl
namespace spqnaucrates
{
class IDatumBool;
}

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		IMDTypeBool
//
//	@doc:
//		Interface for BOOL types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeBool : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiBool;
	}

	virtual ETypeInfo
	GetDatumType() const
	{
		return IMDTypeBool::GetTypeInfo();
	}

	// factory function for BOOL datums
	virtual IDatumBool *CreateBoolDatum(CMemoryPool *mp, BOOL value,
										BOOL is_null) const = 0;
};

}  // namespace spqmd

#endif	// !SPQMD_IMDTypeBool_H

// EOF
