//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumper.h
//
//	@doc:
//		Interface for minidump handler;
//---------------------------------------------------------------------------
#ifndef SPQOS_CMiniDumper_H
#define SPQOS_CMiniDumper_H

#include "spqos/base.h"
#include "spqos/common/CStackObject.h"
#include "spqos/io/COstream.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CMiniDumper
//
//	@doc:
//		Interface for minidump handler;
//
//---------------------------------------------------------------------------
class CMiniDumper : CStackObject
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// flag indicating if handler is initialized
	BOOL m_initialized;

	// flag indicating if handler is finalized
	BOOL m_finalized;

	// private copy ctor
	CMiniDumper(const CMiniDumper &);

protected:
	// stream to serialize objects to
	COstream *m_oos;

public:
	// ctor
	CMiniDumper(CMemoryPool *mp);

	// dtor
	virtual ~CMiniDumper();

	// initialize
	void Init(COstream *oos);

	// finalize
	void Finalize();

	// get stream to serialize to
	COstream &GetOStream();

	// serialize minidump header
	virtual void SerializeHeader() = 0;

	// serialize minidump footer
	virtual void SerializeFooter() = 0;

	// serialize entry header
	virtual void SerializeEntryHeader() = 0;

	// serialize entry footer
	virtual void SerializeEntryFooter() = 0;

};	// class CMiniDumper
}  // namespace spqos

#endif	// !SPQOS_CMiniDumper_H

// EOF
