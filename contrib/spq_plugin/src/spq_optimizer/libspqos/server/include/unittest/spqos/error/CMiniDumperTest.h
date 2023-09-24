//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumperTest.h
//
//	@doc:
//		Test for minidump handler
//---------------------------------------------------------------------------
#ifndef SPQOS_CMiniDumperTest_H
#define SPQOS_CMiniDumperTest_H

#include "spqos/base.h"
#include "spqos/error/CMiniDumper.h"
#include "spqos/error/CSerializable.h"

#define SPQOS_MINIDUMP_BUF_SIZE (1024 * 8)

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CMiniDumperTest
//
//	@doc:
//		Static unit tests for minidump handler
//
//---------------------------------------------------------------------------
class CMiniDumperTest
{
private:
	//---------------------------------------------------------------------------
	//	@class:
	//		CMiniDumperStream
	//
	//	@doc:
	//		Local minidump handler
	//
	//---------------------------------------------------------------------------
	class CMiniDumperStream : public CMiniDumper
	{
	public:
		// ctor
		CMiniDumperStream(CMemoryPool *mp);

		// dtor
		virtual ~CMiniDumperStream();

		// serialize minidump header
		virtual void SerializeHeader();

		// serialize minidump footer
		virtual void SerializeFooter();

		// serialize entry header
		virtual void SerializeEntryHeader();

		// serialize entry footer
		virtual void SerializeEntryFooter();

	};	// class CMiniDumperStream

	//---------------------------------------------------------------------------
	//	@class:
	//		CSerializableStack
	//
	//	@doc:
	//		Stack serializer
	//
	//---------------------------------------------------------------------------
	class CSerializableStack : public CSerializable
	{
	public:
		// ctor
		CSerializableStack();

		// dtor
		virtual ~CSerializableStack();

		// serialize object to passed stream
		virtual void Serialize(COstream &oos);

	};	// class CSerializableStack

	// helper functions
	static void *PvRaise(void *);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CMiniDumperTest
}  // namespace spqos

#endif	// !SPQOS_CMiniDumperTest_H

// EOF
