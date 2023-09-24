//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CAutoSuspendAbort.h
//
//	@doc:
//		Auto object for suspending and resuming task cancellation
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoSuspendAbort_H
#define SPQOS_CAutoSuspendAbort_H

#include "spqos/base.h"
#include "spqos/common/CStackObject.h"

namespace spqos
{
class CTask;

//---------------------------------------------------------------------------
//	@class:
//		CAutoSuspendAbort
//
//	@doc:
//		Auto object for suspending and resuming task cancellation
//
//---------------------------------------------------------------------------
class CAutoSuspendAbort : public CStackObject
{
private:
	// pointer to task in current execution context
	CTask *m_task;

public:
	// ctor - suspends CFA
	CAutoSuspendAbort();

	// dtor - resumes CFA
	virtual ~CAutoSuspendAbort();

};	// class CAutoSuspendAbort

}  // namespace spqos

#endif	// SPQOS_CAutoSuspendAbort_H


// EOF
