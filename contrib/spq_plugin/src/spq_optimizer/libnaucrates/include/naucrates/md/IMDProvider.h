//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		IMDProvider.h
//
//	@doc:
//		Abstract class for retrieving metadata from an external location.
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDProvider_H
#define SPQMD_IMDProvider_H

#include "spqos/base.h"
#include "spqos/common/CHashMap.h"
#include "spqos/string/CWStringBase.h"
#include "spqos/string/CWStringConst.h"

#include "naucrates/md/IMDFunction.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDType.h"

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		IMDProvider
//
//	@doc:
//		Abstract class for retrieving metadata from an external location.
//
//---------------------------------------------------------------------------
class IMDProvider : public CRefCount
{
protected:
	// return the mdid for the requested type
	static IMDId *GetSPQDBTypeMdid(CMemoryPool *mp, CSystemId sysid,
								  IMDType::ETypeInfo type_info);

public:
	virtual ~IMDProvider()
	{
	}

	// returns the DXL string of the requested metadata object
	virtual CWStringBase *GetMDObjDXLStr(
		CMemoryPool *mp, CMDAccessor *md_accessor, IMDId *mdid,
		IMDCacheObject::Emdtype mdtype) const = 0;

	// return the mdid for the specified system id and type
	virtual IMDId *MDId(CMemoryPool *mp, CSystemId sysid,
						IMDType::ETypeInfo type_info) const = 0;
};

// arrays of MD providers
typedef CDynamicPtrArray<IMDProvider, CleanupRelease> CMDProviderArray;

}  // namespace spqmd



#endif	// !SPQMD_IMDProvider_H

// EOF
