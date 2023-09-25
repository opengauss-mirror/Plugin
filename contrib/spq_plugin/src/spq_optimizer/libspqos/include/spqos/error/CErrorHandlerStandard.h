//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CErrorHandlerStandard.h
//
//	@doc:
//		Standard error handler
//---------------------------------------------------------------------------
#ifndef SPQOS_CErrorHandlerStandard_H
#define SPQOS_CErrorHandlerStandard_H

#include "spqos/error/CErrorHandler.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CErrorHandlerStandard
//
//	@doc:
//		Default error handler;
//
//---------------------------------------------------------------------------
class CErrorHandlerStandard : public CErrorHandler
{
private:
	// private copy ctor
	CErrorHandlerStandard(const CErrorHandlerStandard &);

public:
	// ctor
	CErrorHandlerStandard()
	{
	}

	// dtor
	virtual ~CErrorHandlerStandard()
	{
	}

	// process error
	virtual void Process(CException exception);

};	// class CErrorHandlerStandard
}  // namespace spqos

#endif	// !SPQOS_CErrorHandlerStandard_H

// EOF
