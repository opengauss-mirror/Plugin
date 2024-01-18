//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CAutoExceptionStack.cpp
//
//	@doc:
//		Implementation of auto object for saving and restoring exception stack
//
//	@owner:
//		elhela
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spqos/error/CAutoExceptionStack.h"
#include "miscadmin.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CAutoExceptionStack::CAutoExceptionStack
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CAutoExceptionStack::CAutoExceptionStack(void **global_exception_stack,
										 void **global_error_context_stack)
	: m_global_exception_stack(global_exception_stack),
	  m_exception_stack(*global_exception_stack),
	  m_global_error_context_stack(global_error_context_stack),
	  m_error_context_stack(*global_error_context_stack)
{
	HOLD_INTERRUPTS();
}

//---------------------------------------------------------------------------
//	@function:
//		CAutoExceptionStack::~CAutoExceptionStack
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CAutoExceptionStack::~CAutoExceptionStack()
{
	*m_global_exception_stack = m_exception_stack;
	*m_global_error_context_stack = m_error_context_stack;
	RESUME_INTERRUPTS();
}

//---------------------------------------------------------------------------
//	@function:
//		CAutoExceptionStack::SetLocalJmp
//
//	@doc:
//		Set the exception stack to the given address
//
//---------------------------------------------------------------------------
void
CAutoExceptionStack::SetLocalJmp(void *local_jump)
{
	*m_global_exception_stack = local_jump;
}

// EOF
