//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTaskLocalStorageTest.h
//
//	@doc:
//		Task-local storage facility; implements TLS to store an instance
//---------------------------------------------------------------------------
#ifndef SPQOS_CTaskLocalStorageTest_H
#define SPQOS_CTaskLocalStorageTest_H

#include "spqos/base.h"
#include "spqos/common/CSyncHashtable.h"
#include "spqos/task/CTaskLocalStorageObject.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CTaskLocalStorageTest
//
//	@doc:
//		Unittest for TLS implementation
//
//---------------------------------------------------------------------------
class CTaskLocalStorageTest
{
private:
	//---------------------------------------------------------------------------
	//	@class:
	//		CTestObject
	//
	//	@doc:
	//		Simple subclass of CTaskLocalStorageObject
	//
	//---------------------------------------------------------------------------
	class CTestObject : public CTaskLocalStorageObject
	{
	public:
		// ctor
		CTestObject() : CTaskLocalStorageObject(CTaskLocalStorage::EtlsidxTest)
		{
		}

#ifdef SPQOS_DEBUG
		// overwrite abstract member
		IOstream &
		OsPrint(IOstream &os) const
		{
			return os;
		}
#endif	// SPQOS_DEBUG
	};

public:
	// actual unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_TraceFlags();
};

}  // namespace spqos

#endif	// !SPQOS_CTaskLocalStorageTest_H

// EOF
