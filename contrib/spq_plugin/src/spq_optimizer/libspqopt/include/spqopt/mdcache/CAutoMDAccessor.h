//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CAutoMDAccessor.h
//
//	@doc:
//		An auto object encapsulating metadata accessor.
//---------------------------------------------------------------------------



#ifndef SPQOPT_CAutoMDAccessor_H
#define SPQOPT_CAutoMDAccessor_H

#include "spqos/base.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "naucrates/md/CMDProviderMemory.h"

namespace spqmd
{
class IMDCacheObject;
}
namespace spqopt
{
using namespace spqos;
//---------------------------------------------------------------------------
//	@class:
//		CAutoMDAccessor
//
//	@doc:
//		An auto object encapsulating metadata accessor
//
//---------------------------------------------------------------------------
class CAutoMDAccessor : public CStackObject
{
private:
	// metadata provider
	IMDProvider *m_pimdp;

	// do we own cache object?
	BOOL m_fOwnCache;

	// metadata cache
	CMDAccessor::MDCache *m_pcache;

	// metadata accessor
	CMDAccessor *m_pmda;

	// system id
	CSystemId m_sysid;

	// private copy ctor
	CAutoMDAccessor(const CAutoMDAccessor &);

public:
	// ctor
	CAutoMDAccessor(CMemoryPool *mp, IMDProvider *pmdp, CSystemId sysid)
		: m_pimdp(pmdp), m_fOwnCache(true), m_sysid(sysid)
	{
		SPQOS_ASSERT(NULL != pmdp);

		m_pcache =
			CCacheFactory::CreateCache<spqmd::IMDCacheObject *, spqopt::CMDKey *>(
				true /*fUnique*/, 0 /* unlimited cache quota */,
				spqopt::CMDKey::UlHashMDKey, spqopt::CMDKey::FEqualMDKey);
		m_pmda = SPQOS_NEW(mp) CMDAccessor(mp, m_pcache, sysid, pmdp);
	}

	// ctor
	CAutoMDAccessor(CMemoryPool *mp, IMDProvider *pmdp, CSystemId sysid,
					CMDAccessor::MDCache *pcache)
		: m_pimdp(pmdp), m_fOwnCache(false), m_pcache(pcache), m_sysid(sysid)
	{
		SPQOS_ASSERT(NULL != pmdp);
		SPQOS_ASSERT(NULL != pcache);

		m_pmda = SPQOS_NEW(mp) CMDAccessor(mp, m_pcache, sysid, pmdp);
	}

	// dtor
	virtual ~CAutoMDAccessor()
	{
		// because of dependencies among class members, cleaning up
		// has to take place in the following order
		SPQOS_DELETE(m_pmda);
		if (m_fOwnCache)
		{
			SPQOS_DELETE(m_pcache);
		}
	}

	// accessor of cache
	CMDAccessor::MDCache *
	Pcache() const
	{
		return m_pcache;
	}

	// accessor of metadata accessor
	CMDAccessor *
	Pmda() const
	{
		return m_pmda;
	}

	// accessor of metadata provider
	IMDProvider *
	Pimdp() const
	{
		return m_pimdp;
	}

	// accessor of system id
	CSystemId
	Sysid() const
	{
		return m_sysid;
	}
};
}  // namespace spqopt

#endif	// !SPQOPT_CAutoMDAccessor_H

// EOF
