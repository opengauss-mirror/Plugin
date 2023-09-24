//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTaskLocalStorageObjectObject.h
//
//	@doc:
//		Base class for objects stored in TLS
//---------------------------------------------------------------------------
#ifndef SPQOS_CTaskLocalStorageObject_H
#define SPQOS_CTaskLocalStorageObject_H

#include "spqos/base.h"
#include "spqos/task/CTaskLocalStorage.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CTaskLocalStorageObject
//
//	@doc:
//		Abstract TLS object base class; provides hooks for hash table, key, and
//		hash function;
//
//---------------------------------------------------------------------------
class CTaskLocalStorageObject
{
private:
	// private copy ctor
	CTaskLocalStorageObject(const CTaskLocalStorageObject &);

public:
	// ctor
	CTaskLocalStorageObject(CTaskLocalStorage::Etlsidx etlsidx)
		: m_etlsidx(etlsidx)
	{
		SPQOS_ASSERT(CTaskLocalStorage::EtlsidxSentinel > etlsidx &&
					"TLS index out of range");
	}

	// dtor
	virtual ~CTaskLocalStorageObject()
	{
	}

	// accessor
	const CTaskLocalStorage::Etlsidx &
	idx() const
	{
		return m_etlsidx;
	}

	// link
	SLink m_link;

	// key
	const CTaskLocalStorage::Etlsidx m_etlsidx;

};	// class CTaskLocalStorageObject
}  // namespace spqos

#endif	// !SPQOS_CTaskLocalStorageObject_H

// EOF
