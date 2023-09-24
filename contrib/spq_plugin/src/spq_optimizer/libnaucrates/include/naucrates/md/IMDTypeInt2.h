//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		IMDTypeInt2.h
//
//	@doc:
//		Interface for INT2 types in the metadata cache
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDTypeInt2_H
#define SPQMD_IMDTypeInt2_H

#include "spqos/base.h"

#include "naucrates/md/IMDType.h"

// fwd decl
namespace spqnaucrates
{
class IDatumInt2;
}

namespace spqmd
{
using namespace spqos;
using namespace spqnaucrates;


//---------------------------------------------------------------------------
//	@class:
//		IMDTypeInt2
//
//	@doc:
//		Interface for INT2 types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeInt2 : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiInt2;
	}

	virtual ETypeInfo
	GetDatumType() const
	{
		return IMDTypeInt2::GetTypeInfo();
	}

	// factory function for INT2 datums
	virtual IDatumInt2 *CreateInt2Datum(CMemoryPool *mp, SINT value,
										BOOL is_null) const = 0;
};

}  // namespace spqmd

#endif	// !SPQMD_IMDTypeInt2_H

// EOF
