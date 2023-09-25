//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CMetadataAccessorFactory_H
#define SPQOPT_CMetadataAccessorFactory_H

#include "spqos/common/CAutoP.h"
#include "spqos/memory/CMemoryPool.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/minidump/CDXLMinidump.h"

namespace spqopt
{
class CMetadataAccessorFactory
{
public:
	CMetadataAccessorFactory(CMemoryPool *mp, CDXLMinidump *pdxlmd,
							 const CHAR *file_name);

	CMDAccessor *Pmda();

private:
	CAutoP<CMDAccessor> m_apmda;
};

}  // namespace spqopt
#endif
