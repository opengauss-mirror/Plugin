//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSubqNAryJoin2Apply.h
//
//	@doc:
//		Transform NAry Join to Apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSubqNAryJoin2Apply_H
#define SPQOPT_CXformSubqNAryJoin2Apply_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/xforms/CXformSubqJoin2Apply.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSubqNAryJoin2Apply
//
//	@doc:
//		Transform NAry Join with subquery predicates to Apply
//
//---------------------------------------------------------------------------
class CXformSubqNAryJoin2Apply : public CXformSubqJoin2Apply
{
private:
	// private copy ctor
	CXformSubqNAryJoin2Apply(const CXformSubqNAryJoin2Apply &);

public:
	// ctor
	explicit CXformSubqNAryJoin2Apply(CMemoryPool *mp)
		: CXformSubqJoin2Apply(
			  // pattern
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp),
				  SPQOS_NEW(mp)
					  CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp)),
				  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))))
	{
	}

	// dtor
	virtual ~CXformSubqNAryJoin2Apply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSubqNAryJoin2Apply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformSubqNAryJoin2Apply";
	}

};	// class CXformSubqNAryJoin2Apply

}  // namespace spqopt

#endif	// !SPQOPT_CXformSubqNAryJoin2Apply_H

// EOF
