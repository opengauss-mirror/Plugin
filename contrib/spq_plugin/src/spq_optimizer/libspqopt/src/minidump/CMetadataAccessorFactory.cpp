//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/minidump/CMetadataAccessorFactory.h"

#include "spqos/common/CAutoRef.h"

#include "spqopt/mdcache/CMDCache.h"
#include "naucrates/md/CMDProviderMemory.h"

namespace spqopt
{
CMetadataAccessorFactory::CMetadataAccessorFactory(CMemoryPool *mp,
												   CDXLMinidump *pdxlmd,
												   const CHAR *file_name)
{
	// set up MD providers
	CAutoRef<CMDProviderMemory> apmdp(SPQOS_NEW(mp)
										  CMDProviderMemory(mp, file_name));
	const CSystemIdArray *pdrspqsysid = pdxlmd->GetSysidPtrArray();
	CAutoRef<CMDProviderArray> apdrspqmdp(SPQOS_NEW(mp) CMDProviderArray(mp));

	// ensure there is at least ONE system id
	apmdp->AddRef();
	apdrspqmdp->Append(apmdp.Value());

	for (ULONG ul = 1; ul < pdrspqsysid->Size(); ul++)
	{
		apmdp->AddRef();
		apdrspqmdp->Append(apmdp.Value());
	}

	m_apmda = SPQOS_NEW(mp) CMDAccessor(
		mp, CMDCache::Pcache(), pdxlmd->GetSysidPtrArray(), apdrspqmdp.Value());
}

CMDAccessor *
CMetadataAccessorFactory::Pmda()
{
	return m_apmda.Value();
}
}  // namespace spqopt
