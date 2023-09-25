//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformPushGbDedupBelowJoin.h
//
//	@doc:
//		Push dedup group by below join transform
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformPushGbDedupBelowJoin_H
#define SPQOPT_CXformPushGbDedupBelowJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformPushGbBelowJoin.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformPushGbDedupBelowJoin
//
//	@doc:
//		Push dedup group by below join transform
//
//---------------------------------------------------------------------------
class CXformPushGbDedupBelowJoin : public CXformPushGbBelowJoin
{
private:
	// private copy ctor
	CXformPushGbDedupBelowJoin(const CXformPushGbDedupBelowJoin &);

public:
	// ctor
	explicit CXformPushGbDedupBelowJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformPushGbDedupBelowJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfPushGbDedupBelowJoin;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformPushGbDedupBelowJoin";
	}

};	// class CXformPushGbDedupBelowJoin

}  // namespace spqopt

#endif	// !SPQOPT_CXformPushGbDedupBelowJoin_H

// EOF
