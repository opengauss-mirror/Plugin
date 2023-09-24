//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CMDProviderMemory.cpp
//
//	@doc:
//		Implementation of a memory-based metadata provider, which loads all
//		objects in memory and provides a function for looking them up by id.
//---------------------------------------------------------------------------

#include "naucrates/md/CMDProviderMemory.h"

#include "spqos/common/CAutoP.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CMemoryPool.h"
#include "spqos/task/CWorker.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/exception.h"
#include "naucrates/md/CDXLColStats.h"
#include "naucrates/md/CDXLRelStats.h"
#include "naucrates/md/CMDTypeBoolSPQDB.h"
#include "naucrates/md/CMDTypeInt4SPQDB.h"
#include "naucrates/md/CMDTypeInt8SPQDB.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderMemory::CMDProviderMemory
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDProviderMemory::CMDProviderMemory(CMemoryPool *mp, const CHAR *file_name)
	: m_mdmap(NULL)
{
	SPQOS_ASSERT(NULL != file_name);

	// read DXL file
	CAutoRg<CHAR> dxl_file;
	dxl_file = CDXLUtils::Read(mp, file_name);

	CAutoRef<IMDCacheObjectArray> mdcache_obj_array;
	mdcache_obj_array = CDXLUtils::ParseDXLToIMDObjectArray(
		mp, dxl_file.Rgt(), NULL /*xsd_file_path*/);

	LoadMetadataObjectsFromArray(mp, mdcache_obj_array.Value());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderMemory::CMDProviderMemory
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDProviderMemory::CMDProviderMemory(CMemoryPool *mp,
									 IMDCacheObjectArray *mdcache_obj_array)
	: m_mdmap(NULL)
{
	LoadMetadataObjectsFromArray(mp, mdcache_obj_array);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderMemory::LoadMetadataObjectsFromArray
//
//	@doc:
//		Loads the metadata objects from the given file
//
//---------------------------------------------------------------------------
void
CMDProviderMemory::LoadMetadataObjectsFromArray(
	CMemoryPool *mp, IMDCacheObjectArray *mdcache_obj_array)
{
	SPQOS_ASSERT(NULL != mdcache_obj_array);

	// load metadata objects from the file
	CAutoRef<MDIdToSerializedMDIdMap> md_map;
	m_mdmap = SPQOS_NEW(mp) MDIdToSerializedMDIdMap(mp);
	md_map = m_mdmap;

	const ULONG size = mdcache_obj_array->Size();

	// load objects into the hash map
	for (ULONG ul = 0; ul < size; ul++)
	{
		SPQOS_CHECK_ABORT;

		IMDCacheObject *mdcache_obj = (*mdcache_obj_array)[ul];
		IMDId *mdid_key = mdcache_obj->MDId();
		mdid_key->AddRef();
		CAutoRef<IMDId> mdid_key_autoref;
		mdid_key_autoref = mdid_key;

		CAutoP<CWStringDynamic> str;
		str = CDXLUtils::SerializeMDObj(
			mp, mdcache_obj, true /*fSerializeHeaders*/, false /*findent*/);

		SPQOS_CHECK_ABORT;
		BOOL fInserted = m_mdmap->Insert(mdid_key, str.Value());
		if (!fInserted)
		{
			SPQOS_RAISE(spqdxl::ExmaMD, spqdxl::ExmiMDCacheEntryDuplicate,
					   mdid_key->GetBuffer());
		}
		(void) mdid_key_autoref.Reset();
		(void) str.Reset();
	}

	// safely completed loading
	(void) md_map.Reset();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderMemory::~CMDProviderMemory
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDProviderMemory::~CMDProviderMemory()
{
	CRefCount::SafeRelease(m_mdmap);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderMemory::GetMDObjDXLStr
//
//	@doc:
//		Returns the DXL of the requested object in the provided memory pool
//
//---------------------------------------------------------------------------
CWStringBase *
CMDProviderMemory::GetMDObjDXLStr(CMemoryPool *mp,
								  CMDAccessor *,  //md_accessor
								  IMDId *mdid, IMDCacheObject::Emdtype) const
{
	SPQOS_ASSERT(NULL != m_mdmap);

	const CWStringDynamic *pstrObj = m_mdmap->Find(mdid);

	// result string
	CAutoP<CWStringDynamic> a_pstrResult;

	a_pstrResult = NULL;

	if (NULL == pstrObj)
	{
		// Relstats and colstats are special as they may not
		// exist in the metadata file. Provider must return dummy objects
		// in this case.
		switch (mdid->MdidType())
		{
			case IMDId::EmdidRelStats:
			{
				mdid->AddRef();
				CAutoRef<CDXLRelStats> a_pdxlrelstats;
				a_pdxlrelstats = CDXLRelStats::CreateDXLDummyRelStats(mp, mdid);
				a_pstrResult = CDXLUtils::SerializeMDObj(
					mp, a_pdxlrelstats.Value(), true /*fSerializeHeaders*/,
					false /*findent*/);
				break;
			}
			case IMDId::EmdidColStats:
			{
				CAutoP<CWStringDynamic> a_pstr;
				a_pstr = SPQOS_NEW(mp) CWStringDynamic(mp, mdid->GetBuffer());
				CAutoP<CMDName> a_pmdname;
				a_pmdname = SPQOS_NEW(mp) CMDName(mp, a_pstr.Value());
				mdid->AddRef();
				CAutoRef<CDXLColStats> a_pdxlcolstats;
				a_pdxlcolstats = CDXLColStats::CreateDXLDummyColStats(
					mp, mdid, a_pmdname.Value(),
					CStatistics::DefaultColumnWidth /* width */);
				a_pmdname.Reset();
				a_pstrResult = CDXLUtils::SerializeMDObj(
					mp, a_pdxlcolstats.Value(), true /*fSerializeHeaders*/,
					false /*findent*/);
				break;
			}
			default:
			{
				SPQOS_RAISE(spqdxl::ExmaMD, spqdxl::ExmiMDCacheEntryNotFound,
						   mdid->GetBuffer());
			}
		}
	}
	else
	{
		// copy string into result
		a_pstrResult = SPQOS_NEW(mp) CWStringDynamic(mp, pstrObj->GetBuffer());
	}

	SPQOS_ASSERT(NULL != a_pstrResult.Value());

	return a_pstrResult.Reset();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDProviderMemory::MDId
//
//	@doc:
//		Returns the mdid for the requested system and type info.
//		The caller takes ownership over the object.
//
//---------------------------------------------------------------------------
IMDId *
CMDProviderMemory::MDId(CMemoryPool *mp, CSystemId sysid,
						IMDType::ETypeInfo type_info) const
{
	return GetSPQDBTypeMdid(mp, sysid, type_info);
}

// EOF
