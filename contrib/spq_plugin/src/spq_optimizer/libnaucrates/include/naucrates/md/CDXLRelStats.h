//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLRelStats.h
//
//	@doc:
//		Class representing relation stats
//---------------------------------------------------------------------------



#ifndef SPQMD_CDXLRelStats_H
#define SPQMD_CDXLRelStats_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/md/CMDIdRelStats.h"
#include "naucrates/md/IMDRelStats.h"

namespace spqdxl
{
class CXMLSerializer;
}

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CDXLRelStats
//
//	@doc:
//		Class representing relation stats
//
//---------------------------------------------------------------------------
class CDXLRelStats : public IMDRelStats
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// metadata id of the object
	CMDIdRelStats *m_rel_stats_mdid;

	// table name
	CMDName *m_mdname;

	// number of rows
	CDouble m_rows;

	// flag to indicate if input relation is empty
	BOOL m_empty;

	// DXL string for object
	CWStringDynamic *m_dxl_str;

	// private copy ctor
	CDXLRelStats(const CDXLRelStats &);

	// number of blocks (not always up to-to-date)
	ULONG m_relpages;

	// number of all-visible blocks (not always up-to-date)
	ULONG m_relallvisible;

public:
	CDXLRelStats(CMemoryPool *mp, CMDIdRelStats *rel_stats_mdid,
				 CMDName *mdname, CDouble rows, BOOL is_empty, ULONG relpages,
				 ULONG relallvisible);

	virtual ~CDXLRelStats();

	// the metadata id
	virtual IMDId *MDId() const;

	// relation name
	virtual CMDName Mdname() const;

	// DXL string representation of cache object
	virtual const CWStringDynamic *GetStrRepr() const;

	// number of rows
	virtual CDouble Rows() const;

	// number of blocks (not always up to-to-date)
	virtual ULONG
	RelPages() const
	{
		return m_relpages;
	}

	// number of all-visible blocks (not always up-to-date)
	virtual ULONG
	RelAllVisible() const
	{
		return m_relallvisible;
	}

	// is statistics on an empty input
	virtual BOOL
	IsEmpty() const
	{
		return m_empty;
	}

	// serialize relation stats in DXL format given a serializer object
	virtual void Serialize(spqdxl::CXMLSerializer *) const;

#ifdef SPQOS_DEBUG
	// debug print of the metadata relation
	virtual void DebugPrint(IOstream &os) const;
#endif

	// dummy relstats
	static CDXLRelStats *CreateDXLDummyRelStats(CMemoryPool *mp, IMDId *mdid);
};

}  // namespace spqmd



#endif	// !SPQMD_CDXLRelStats_H

// EOF
