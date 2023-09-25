//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDTypeInt8.h
//
//	@doc:
//		Interface for INT8 types in the metadata cache
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDTypeInt8_H
#define SPQMD_IMDTypeInt8_H

#include "spqos/base.h"

#include "naucrates/md/IMDType.h"

// fwd decl
namespace spqnaucrates
{
class IDatumInt8;
}

namespace spqmd
{
using namespace spqos;
using namespace spqnaucrates;


//---------------------------------------------------------------------------
//	@class:
//		IMDTypeInt8
//
//	@doc:
//		Interface for INT8 types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeInt8 : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiInt8;
	}

	virtual ETypeInfo
	GetDatumType() const
	{
		return IMDTypeInt8::GetTypeInfo();
	}

	// factory function for INT8 datums
	virtual IDatumInt8 *CreateInt8Datum(CMemoryPool *mp, LINT value,
										BOOL is_null) const = 0;
};

}  // namespace spqmd

#endif	// !SPQMD_IMDTypeInt8_H

// EOF
