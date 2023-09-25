//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		IMDTypeInt4.h
//
//	@doc:
//		Interface for INT4 types in the metadata cache
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDTypeInt4_H
#define SPQMD_IMDTypeInt4_H

#include "spqos/base.h"

#include "naucrates/md/IMDType.h"

// fwd decl
namespace spqnaucrates
{
class IDatumInt4;
}

namespace spqmd
{
using namespace spqos;
using namespace spqnaucrates;


//---------------------------------------------------------------------------
//	@class:
//		IMDTypeInt4
//
//	@doc:
//		Interface for INT4 types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeInt4 : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiInt4;
	}

	virtual ETypeInfo
	GetDatumType() const
	{
		return IMDTypeInt4::GetTypeInfo();
	}

	// factory function for INT4 datums
	virtual IDatumInt4 *CreateInt4Datum(CMemoryPool *mp, INT value,
										BOOL is_null) const = 0;
};

}  // namespace spqmd

#endif	// !SPQMD_IMDTypeInt4_H

// EOF
