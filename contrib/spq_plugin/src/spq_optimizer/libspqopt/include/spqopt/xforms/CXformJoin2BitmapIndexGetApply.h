//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (c) 2020 VMware and affiliates, Inc.
//
// Transform a join into a bitmap index apply. Allow a variety of nodes on
// the inner side, including a mandatory get, plus optional select,
// project and aggregate nodes.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformJoin2BitmapIndexGetApply_H
#define SPQOPT_CXformJoin2BitmapIndexGetApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternNode.h"
#include "spqopt/xforms/CXformJoin2IndexApplyGeneric.h"

namespace spqopt
{
using namespace spqos;

class CXformJoin2BitmapIndexGetApply : public CXformJoin2IndexApplyGeneric
{
private:
	// no copy ctor
	CXformJoin2BitmapIndexGetApply(const CXformJoin2BitmapIndexGetApply &);

public:
	// ctor
	explicit CXformJoin2BitmapIndexGetApply(CMemoryPool *mp)
		: CXformJoin2IndexApplyGeneric(mp, true)
	{
	}

	// dtor
	virtual ~CXformJoin2BitmapIndexGetApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfJoin2BitmapIndexGetApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformJoin2BitmapIndexGetApply";
	}

};	// class CXformJoin2BitmapIndexGetApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformJoin2BitmapIndexGetApply_H

// EOF
