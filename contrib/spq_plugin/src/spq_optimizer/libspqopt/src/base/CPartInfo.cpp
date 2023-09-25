//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CPartInfo.cpp
//
//	@doc:
//		Implementation of derived partition information at the logical level
//---------------------------------------------------------------------------

#include "spqopt/base/CPartInfo.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CPartConstraint.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CPartInfo);
FORCE_GENERATE_DBGSTR(CPartInfo::CPartInfoEntry);

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfoEntry::CPartInfoEntry
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPartInfo::CPartInfoEntry::CPartInfoEntry(ULONG scan_id, IMDId *mdid,
										  CPartKeysArray *pdrspqpartkeys,
										  CPartConstraint *ppartcnstrRel)
	: m_scan_id(scan_id),
	  m_mdid(mdid),
	  m_pdrspqpartkeys(pdrspqpartkeys),
	  m_ppartcnstrRel(ppartcnstrRel)
{
	SPQOS_ASSERT(mdid->IsValid());
	SPQOS_ASSERT(pdrspqpartkeys != NULL);
	SPQOS_ASSERT(0 < pdrspqpartkeys->Size());
	SPQOS_ASSERT(NULL != ppartcnstrRel);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfoEntry::~CPartInfoEntry
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPartInfo::CPartInfoEntry::~CPartInfoEntry()
{
	m_mdid->Release();
	m_pdrspqpartkeys->Release();
	m_ppartcnstrRel->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfoEntry::PpartinfoentryAddRemappedKeys
//
//	@doc:
//		Create a copy of the current object, and add a set of remapped
//		part keys to this entry, using the existing keys and the given hashmap
//
//---------------------------------------------------------------------------
CPartInfo::CPartInfoEntry *
CPartInfo::CPartInfoEntry::PpartinfoentryAddRemappedKeys(
	CMemoryPool *mp, CColRefSet *pcrs, UlongToColRefMap *colref_mapping)
{
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(NULL != colref_mapping);

	CPartKeysArray *pdrspqpartkeys =
		CPartKeys::PdrspqpartkeysCopy(mp, m_pdrspqpartkeys);

	const ULONG size = m_pdrspqpartkeys->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CPartKeys *ppartkeys = (*m_pdrspqpartkeys)[ul];

		if (ppartkeys->FOverlap(pcrs))
		{
			pdrspqpartkeys->Append(
				ppartkeys->PpartkeysRemap(mp, colref_mapping));
			break;
		}
	}

	m_mdid->AddRef();
	CPartConstraint *ppartcnstrRel =
		m_ppartcnstrRel->PpartcnstrCopyWithRemappedColumns(
			mp, colref_mapping, false /*must_exist*/);

	return SPQOS_NEW(mp)
		CPartInfoEntry(m_scan_id, m_mdid, pdrspqpartkeys, ppartcnstrRel);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfoEntry::OsPrint
//
//	@doc:
//		Print functions
//
//---------------------------------------------------------------------------
IOstream &
CPartInfo::CPartInfoEntry::OsPrint(IOstream &os) const
{
	os << m_scan_id;

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfoEntry::PpartinfoentryCopy
//
//	@doc:
//		Copy part info entry into given memory pool
//
//---------------------------------------------------------------------------
CPartInfo::CPartInfoEntry *
CPartInfo::CPartInfoEntry::PpartinfoentryCopy(CMemoryPool *mp)
{
	IMDId *mdid = MDId();
	mdid->AddRef();

	// copy part keys
	CPartKeysArray *pdrspqpartkeysCopy =
		CPartKeys::PdrspqpartkeysCopy(mp, Pdrspqpartkeys());

	// copy part constraint using empty remapping to get exact copy
	UlongToColRefMap *colref_mapping = SPQOS_NEW(mp) UlongToColRefMap(mp);
	CPartConstraint *ppartcnstrRel =
		PpartcnstrRel()->PpartcnstrCopyWithRemappedColumns(
			mp, colref_mapping, false /*must_exist*/);
	colref_mapping->Release();

	return SPQOS_NEW(mp)
		CPartInfoEntry(ScanId(), mdid, pdrspqpartkeysCopy, ppartcnstrRel);
}


//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfo
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPartInfo::CPartInfo(CPartInfoEntryArray *pdrspqpartentries)
	: m_pdrspqpartentries(pdrspqpartentries)
{
	SPQOS_ASSERT(NULL != pdrspqpartentries);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::CPartInfo
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPartInfo::CPartInfo(CMemoryPool *mp)
{
	m_pdrspqpartentries = SPQOS_NEW(mp) CPartInfoEntryArray(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::~CPartInfo
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPartInfo::~CPartInfo()
{
	CRefCount::SafeRelease(m_pdrspqpartentries);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::AddPartConsumer
//
//	@doc:
//		Add part table consumer
//
//---------------------------------------------------------------------------
void
CPartInfo::AddPartConsumer(CMemoryPool *mp, ULONG scan_id, IMDId *mdid,
						   CColRef2dArray *pdrspqdrspqcrPart,
						   CPartConstraint *ppartcnstrRel)
{
	CPartKeysArray *pdrspqpartkeys = SPQOS_NEW(mp) CPartKeysArray(mp);
	pdrspqpartkeys->Append(SPQOS_NEW(mp) CPartKeys(pdrspqdrspqcrPart));

	m_pdrspqpartentries->Append(SPQOS_NEW(mp) CPartInfoEntry(
		scan_id, mdid, pdrspqpartkeys, ppartcnstrRel));
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::FContainsScanId
//
//	@doc:
//		Check if part info contains given scan id
//
//---------------------------------------------------------------------------
BOOL
CPartInfo::FContainsScanId(ULONG scan_id) const
{
	const ULONG size = m_pdrspqpartentries->Size();

	for (ULONG ul = 0; ul < size; ul++)
	{
		CPartInfoEntry *ppartinfoentry = (*m_pdrspqpartentries)[ul];
		if (scan_id == ppartinfoentry->ScanId())
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::ScanId
//
//	@doc:
//		Return scan id of the entry at the given position
//
//---------------------------------------------------------------------------
ULONG
CPartInfo::ScanId(ULONG ulPos) const
{
	return (*m_pdrspqpartentries)[ulPos]->ScanId();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::GetRelMdId
//
//	@doc:
//		Return relation mdid of the entry at the given position
//
//---------------------------------------------------------------------------
IMDId *
CPartInfo::GetRelMdId(ULONG ulPos) const
{
	return (*m_pdrspqpartentries)[ulPos]->MDId();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::Pdrspqpartkeys
//
//	@doc:
//		Return part keys of the entry at the given position
//
//---------------------------------------------------------------------------
CPartKeysArray *
CPartInfo::Pdrspqpartkeys(ULONG ulPos) const
{
	return (*m_pdrspqpartentries)[ulPos]->Pdrspqpartkeys();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::Ppartcnstr
//
//	@doc:
//		Return part constraint of the entry at the given position
//
//---------------------------------------------------------------------------
CPartConstraint *
CPartInfo::Ppartcnstr(ULONG ulPos) const
{
	return (*m_pdrspqpartentries)[ulPos]->PpartcnstrRel();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::PdrspqpartkeysByScanId
//
//	@doc:
//		Return part keys of the entry with the given scan id
//
//---------------------------------------------------------------------------
CPartKeysArray *
CPartInfo::PdrspqpartkeysByScanId(ULONG scan_id) const
{
	const ULONG size = m_pdrspqpartentries->Size();

	for (ULONG ul = 0; ul < size; ul++)
	{
		CPartInfoEntry *ppartinfoentry = (*m_pdrspqpartentries)[ul];
		if (scan_id == ppartinfoentry->ScanId())
		{
			return ppartinfoentry->Pdrspqpartkeys();
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::PpartinfoWithRemappedKeys
//
//	@doc:
//		Return a new part info object with an additional set of remapped keys
//
//---------------------------------------------------------------------------
CPartInfo *
CPartInfo::PpartinfoWithRemappedKeys(CMemoryPool *mp, CColRefArray *pdrspqcrSrc,
									 CColRefArray *pdrspqcrDest) const
{
	SPQOS_ASSERT(NULL != pdrspqcrSrc);
	SPQOS_ASSERT(NULL != pdrspqcrDest);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrSrc);
	UlongToColRefMap *colref_mapping =
		CUtils::PhmulcrMapping(mp, pdrspqcrSrc, pdrspqcrDest);

	CPartInfoEntryArray *pdrspqpartentries =
		SPQOS_NEW(mp) CPartInfoEntryArray(mp);

	const ULONG size = m_pdrspqpartentries->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CPartInfoEntry *ppartinfoentry = (*m_pdrspqpartentries)[ul];

		// if this entry has keys that overlap with the source columns then
		// add another set of keys to it using the destination columns
		CPartInfoEntry *ppartinfoentryNew =
			ppartinfoentry->PpartinfoentryAddRemappedKeys(mp, pcrs,
														  colref_mapping);
		pdrspqpartentries->Append(ppartinfoentryNew);
	}

	pcrs->Release();
	colref_mapping->Release();

	return SPQOS_NEW(mp) CPartInfo(pdrspqpartentries);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::PpartinfoCombine
//
//	@doc:
//		Combine two part info objects
//
//---------------------------------------------------------------------------
CPartInfo *
CPartInfo::PpartinfoCombine(CMemoryPool *mp, CPartInfo *ppartinfoFst,
							CPartInfo *ppartinfoSnd)
{
	SPQOS_ASSERT(NULL != ppartinfoFst);
	SPQOS_ASSERT(NULL != ppartinfoSnd);

	CPartInfoEntryArray *pdrspqpartentries =
		SPQOS_NEW(mp) CPartInfoEntryArray(mp);

	// copy part entries from first part info object
	CUtils::AddRefAppend(pdrspqpartentries, ppartinfoFst->m_pdrspqpartentries);

	// copy part entries from second part info object, except those which already exist
	const ULONG length = ppartinfoSnd->m_pdrspqpartentries->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CPartInfoEntry *ppartinfoentry =
			(*(ppartinfoSnd->m_pdrspqpartentries))[ul];
		CPartKeysArray *pdrspqpartkeys =
			ppartinfoFst->PdrspqpartkeysByScanId(ppartinfoentry->ScanId());

		if (NULL != pdrspqpartkeys)
		{
			// there is already an entry with the same scan id; need to add to it
			// the keys from the current entry
			CPartKeysArray *pdrspqpartkeysCopy = CPartKeys::PdrspqpartkeysCopy(
				mp, ppartinfoentry->Pdrspqpartkeys());
			CUtils::AddRefAppend(pdrspqpartkeys, pdrspqpartkeysCopy);
			pdrspqpartkeysCopy->Release();
		}
		else
		{
			CPartInfoEntry *ppartinfoentryCopy =
				ppartinfoentry->PpartinfoentryCopy(mp);
			pdrspqpartentries->Append(ppartinfoentryCopy);
		}
	}

	return SPQOS_NEW(mp) CPartInfo(pdrspqpartentries);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartInfo::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPartInfo::OsPrint(IOstream &os) const
{
	const ULONG length = m_pdrspqpartentries->Size();
	os << "Part Consumers: ";
	for (ULONG ul = 0; ul < length; ul++)
	{
		CPartInfoEntry *ppartinfoentry = (*m_pdrspqpartentries)[ul];
		ppartinfoentry->OsPrint(os);

		// separator
		os << (ul == length - 1 ? "" : ", ");
	}

	os << ", Part Keys: ";
	for (ULONG ulCons = 0; ulCons < length; ulCons++)
	{
		CPartKeysArray *pdrspqpartkeys = Pdrspqpartkeys(ulCons);
		os << "(";
		const ULONG ulPartKeys = pdrspqpartkeys->Size();
		;
		for (ULONG ulPartKey = 0; ulPartKey < ulPartKeys; ulPartKey++)
		{
			os << *(*pdrspqpartkeys)[ulPartKey];
			os << (ulPartKey == ulPartKeys - 1 ? "" : ", ");
		}
		os << ")";
		os << (ulCons == length - 1 ? "" : ", ");
	}

	return os;
}

// EOF
