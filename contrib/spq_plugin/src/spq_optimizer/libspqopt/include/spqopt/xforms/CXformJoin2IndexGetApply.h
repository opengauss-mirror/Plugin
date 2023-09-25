//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (c) 2020 VMware and affiliates, Inc.
//
// Transform a join into a non-bitmap index apply. Allow a variety of nodes on
// the inner side, including a mandatory get, plus optional select,
// project and aggregate nodes.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformJoin2IndexGetApply_H
#define SPQOPT_CXformJoin2IndexGetApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternNode.h"
#include "spqopt/xforms/CXformJoin2IndexApplyGeneric.h"

namespace spqopt
{
using namespace spqos;

class CXformJoin2IndexGetApply : public CXformJoin2IndexApplyGeneric
{
private:
	// no copy ctor
	CXformJoin2IndexGetApply(const CXformJoin2IndexGetApply &);

public:
	// ctor
	explicit CXformJoin2IndexGetApply(CMemoryPool *mp)
		: CXformJoin2IndexApplyGeneric(mp, false)
	{
	}

	// dtor
	virtual ~CXformJoin2IndexGetApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfJoin2IndexGetApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformJoin2IndexGetApply";
	}

};	// class CXformJoin2IndexGetApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformJoin2IndexGetApply_H

// EOF
