//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumperDXL.h
//
//	@doc:
//		DXL-based minidump handler;
//---------------------------------------------------------------------------
#ifndef SPQOPT_CMiniDumperDXL_H
#define SPQOPT_CMiniDumperDXL_H

#include "spqos/base.h"
#include "spqos/error/CMiniDumper.h"

using namespace spqos;

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CMiniDumperDXL
//
//	@doc:
//		DXL-specific minidump handler
//
//---------------------------------------------------------------------------
class CMiniDumperDXL : public CMiniDumper
{
private:
	// private copy ctor
	CMiniDumperDXL(const CMiniDumperDXL &);

public:
	// ctor
	explicit CMiniDumperDXL(CMemoryPool *mp);

	// dtor
	virtual ~CMiniDumperDXL();

	// serialize minidump header
	virtual void SerializeHeader();

	// serialize minidump footer
	virtual void SerializeFooter();

	// serialize entry header
	virtual void SerializeEntryHeader();

	// serialize entry footer
	virtual void SerializeEntryFooter();

};	// class CMiniDumperDXL
}  // namespace spqopt

#endif	// !SPQOPT_CMiniDumperDXL_H

// EOF
