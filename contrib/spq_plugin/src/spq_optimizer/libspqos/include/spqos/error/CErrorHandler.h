//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CErrorHandler.h
//
//	@doc:
//		Error handler base class;
//---------------------------------------------------------------------------
#ifndef SPQOS_CErrorHandler_H
#define SPQOS_CErrorHandler_H

#include "spqos/assert.h"
#include "spqos/error/CException.h"
#include "spqos/types.h"

namespace spqos
{
// fwd declarations
class CMemoryPool;

//---------------------------------------------------------------------------
//	@class:
//		CErrorHandler
//
//	@doc:
//		Error handler to be installed inside a worker;
//
//---------------------------------------------------------------------------
class CErrorHandler
{
private:
	// private copy ctor
	CErrorHandler(const CErrorHandler &);

public:
	// ctor
	CErrorHandler()
	{
	}

	// dtor
	virtual ~CErrorHandler()
	{
	}

	// process error
	virtual void Process(CException exception) = 0;

};	// class CErrorHandler
}  // namespace spqos

#endif	// !SPQOS_CErrorHandler_H

// EOF
