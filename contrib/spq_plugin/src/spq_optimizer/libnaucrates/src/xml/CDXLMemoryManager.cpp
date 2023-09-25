//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLMemoryManager.cpp
//
//	@doc:
//		Implementation of the DXL memory manager to be plugged in Xerces.
//---------------------------------------------------------------------------

#include "naucrates/dxl/xml/CDXLMemoryManager.h"

#include "spqos/base.h"

using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLMemoryManager::CDXLMemoryManager
//
//	@doc:
//		Constructs a memory manager around a given memory pool.
//
//---------------------------------------------------------------------------
CDXLMemoryManager::CDXLMemoryManager(CMemoryPool *mp) : m_mp(mp)
{
	SPQOS_ASSERT(NULL != m_mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLMemoryManager::allocate
//
//	@doc:
//		Memory allocation.
//
//---------------------------------------------------------------------------
void *
CDXLMemoryManager::allocate(XMLSize_t xmlsize)
{
	SPQOS_ASSERT(NULL != m_mp);
	return SPQOS_NEW_ARRAY(m_mp, BYTE, xmlsize);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLMemoryManager::deallocate
//
//	@doc:
//		Memory deallocation.
//
//---------------------------------------------------------------------------
void
CDXLMemoryManager::deallocate(void *pv)
{
	SPQOS_DELETE_ARRAY(reinterpret_cast<BYTE *>(pv));
}


// EOF
