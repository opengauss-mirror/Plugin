//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CTraceFlagIter.h
//
//	@doc:
//		Trace flag iterator
//---------------------------------------------------------------------------
#ifndef SPQOS_CTraceFlagIter_H
#define SPQOS_CTraceFlagIter_H

#include "spqos/base.h"
#include "spqos/common/CBitSetIter.h"
#include "spqos/task/CTask.h"


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CTraceFlagIter
//
//	@doc:
//		Trace flag iterator for the currently executing task
//
//---------------------------------------------------------------------------
class CTraceFlagIter : public CBitSetIter
{
private:
	// no copy ctor
	CTraceFlagIter(const CTraceFlagIter &);

public:
	// ctor
	CTraceFlagIter() : CBitSetIter(*CTask::Self()->GetTaskCtxt()->m_bitset)
	{
	}

	// dtor
	virtual ~CTraceFlagIter()
	{
	}

};	// class CTraceFlagIter

}  // namespace spqos


#endif	// !SPQOS_CTraceFlagIter_H

// EOF
