//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CPartInfo.h
//
//	@doc:
//		Derived partition information at the logical level
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPartInfo_H
#define SPQOPT_CPartInfo_H

#include "spqos/base.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CPartKeys.h"

// fwd decl
namespace spqmd
{
class IMDId;
}

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

// fwd decl
class CPartConstraint;

//---------------------------------------------------------------------------
//	@class:
//		CPartInfo
//
//	@doc:
//		Derived partition information at the logical level
//
//---------------------------------------------------------------------------
class CPartInfo : public CRefCount, public DbgPrintMixin<CPartInfo>
{
private:
	//---------------------------------------------------------------------------
	//	@class:
	//		CPartInfoEntry
	//
	//	@doc:
	//		A single entry of the CPartInfo
	//
	//---------------------------------------------------------------------------
	class CPartInfoEntry : public CRefCount,
						   public DbgPrintMixin<CPartInfoEntry>
	{
	private:
		// scan id
		ULONG m_scan_id;

		// partition table mdid
		IMDId *m_mdid;

		// partition keys
		CPartKeysArray *m_pdrspqpartkeys;

		// part constraint of the relation
		CPartConstraint *m_ppartcnstrRel;

		// private copy ctor
		CPartInfoEntry(const CPartInfoEntry &);

	public:
		// ctor
		CPartInfoEntry(ULONG scan_id, IMDId *mdid,
					   CPartKeysArray *pdrspqpartkeys,
					   CPartConstraint *ppartcnstrRel);

		// dtor
		virtual ~CPartInfoEntry();

		// scan id
		virtual ULONG
		ScanId() const
		{
			return m_scan_id;
		}

		// relation part constraint
		CPartConstraint *
		PpartcnstrRel() const
		{
			return m_ppartcnstrRel;
		}

		// create a copy of the current object, and add a set of remapped
		// part keys to this entry, using the existing keys and the given hashmap
		CPartInfoEntry *PpartinfoentryAddRemappedKeys(
			CMemoryPool *mp, CColRefSet *pcrs,
			UlongToColRefMap *colref_mapping);

		// mdid of partition table
		virtual IMDId *
		MDId() const
		{
			return m_mdid;
		}

		// partition keys of partition table
		virtual CPartKeysArray *
		Pdrspqpartkeys() const
		{
			return m_pdrspqpartkeys;
		}

		// print function
		virtual IOstream &OsPrint(IOstream &os) const;

		// copy part info entry into given memory pool
		CPartInfoEntry *PpartinfoentryCopy(CMemoryPool *mp);

	};	// CPartInfoEntry

	typedef CDynamicPtrArray<CPartInfoEntry, CleanupRelease>
		CPartInfoEntryArray;

	// partition table consumers
	CPartInfoEntryArray *m_pdrspqpartentries;

	// private ctor
	explicit CPartInfo(CPartInfoEntryArray *pdrspqpartentries);

	//private copy ctor
	CPartInfo(const CPartInfo &);

public:
	// ctor
	explicit CPartInfo(CMemoryPool *mp);

	// dtor
	virtual ~CPartInfo();

	// number of part table consumers
	ULONG
	UlConsumers() const
	{
		return m_pdrspqpartentries->Size();
	}

	// add part table consumer
	void AddPartConsumer(CMemoryPool *mp, ULONG scan_id, IMDId *mdid,
						 CColRef2dArray *pdrspqdrspqcrPart,
						 CPartConstraint *ppartcnstrRel);

	// scan id of the entry at the given position
	ULONG ScanId(ULONG ulPos) const;

	// relation mdid of the entry at the given position
	IMDId *GetRelMdId(ULONG ulPos) const;

	// part keys of the entry at the given position
	CPartKeysArray *Pdrspqpartkeys(ULONG ulPos) const;

	// part constraint of the entry at the given position
	CPartConstraint *Ppartcnstr(ULONG ulPos) const;

	// check if part info contains given scan id
	BOOL FContainsScanId(ULONG scan_id) const;

	// part keys of the entry with the given scan id
	CPartKeysArray *PdrspqpartkeysByScanId(ULONG scan_id) const;

	// return a new part info object with an additional set of remapped keys
	CPartInfo *PpartinfoWithRemappedKeys(CMemoryPool *mp,
										 CColRefArray *pdrspqcrSrc,
										 CColRefArray *pdrspqcrDest) const;

	// print
	virtual IOstream &OsPrint(IOstream &) const;

	// combine two part info objects
	static CPartInfo *PpartinfoCombine(CMemoryPool *mp, CPartInfo *ppartinfoFst,
									   CPartInfo *ppartinfoSnd);

};	// CPartInfo

// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CPartInfo &partinfo)
{
	return partinfo.OsPrint(os);
}
}  // namespace spqopt

#endif	// !SPQOPT_CPartInfo_H

// EOF
