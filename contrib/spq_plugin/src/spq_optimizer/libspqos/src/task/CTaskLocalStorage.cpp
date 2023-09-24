//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTaskLocalStorage.cpp
//
//	@doc:
//		Task-local storage implementation
//---------------------------------------------------------------------------

#include "spqos/task/CTaskLocalStorage.h"

#include "spqos/common/CSyncHashtableAccessByKey.h"
#include "spqos/task/CTaskLocalStorageObject.h"

using namespace spqos;


// shorthand for HT accessor
typedef CSyncHashtableAccessByKey<CTaskLocalStorageObject,
								  CTaskLocalStorage::Etlsidx>
	HashTableAccessor;


// invalid idx
const CTaskLocalStorage::Etlsidx CTaskLocalStorage::m_invalid_idx =
	EtlsidxInvalid;


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorage::~CTaskLocalStorage
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CTaskLocalStorage::~CTaskLocalStorage()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorage::Reset
//
//	@doc:
//		(re-)init TLS
//
//---------------------------------------------------------------------------
void
CTaskLocalStorage::Reset(CMemoryPool *mp)
{
	// destroy old
	m_hash_table.Cleanup();

	// realloc
	m_hash_table.Init(mp,
					  128,	// number of hashbuckets
					  SPQOS_OFFSET(CTaskLocalStorageObject, m_link),
					  SPQOS_OFFSET(CTaskLocalStorageObject, m_etlsidx),
					  &(CTaskLocalStorage::m_invalid_idx),
					  CTaskLocalStorage::HashIdx, CTaskLocalStorage::Equals);
}


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorage::Store
//
//	@doc:
//		Store object in TLS
//
//---------------------------------------------------------------------------
void
CTaskLocalStorage::Store(CTaskLocalStorageObject *obj)
{
	SPQOS_ASSERT(NULL != obj);

#ifdef SPQOS_DEBUG
	{
		HashTableAccessor HashTableAccessor(m_hash_table, obj->idx());
		SPQOS_ASSERT(NULL == HashTableAccessor.Find() &&
					"Duplicate TLS object key");
	}
#endif	// SPQOS_DEBUG

	m_hash_table.Insert(obj);
}


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorage::Get
//
//	@doc:
//		Lookup object in TLS
//
//---------------------------------------------------------------------------
CTaskLocalStorageObject *
CTaskLocalStorage::Get(CTaskLocalStorage::Etlsidx idx)
{
	HashTableAccessor HashTableAccessor(m_hash_table, idx);
	return HashTableAccessor.Find();
}


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorage::Remove
//
//	@doc:
//		Delete given object from TLS
//
//---------------------------------------------------------------------------
void
CTaskLocalStorage::Remove(CTaskLocalStorageObject *obj)
{
	SPQOS_ASSERT(NULL != obj);

	// lookup object
	HashTableAccessor HashTableAccessor(m_hash_table, obj->idx());
	SPQOS_ASSERT(NULL != HashTableAccessor.Find() && "Object not found in TLS");

	HashTableAccessor.Remove(obj);
}

// EOF
