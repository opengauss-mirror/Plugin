//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CPatternMultiTree.h
//
//	@doc:
//		Pattern that matches a variable number of trees
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPatternMultiTree_H
#define SPQOPT_CPatternMultiTree_H

#include "spqos/base.h"

#include "spqopt/operators/CPattern.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPatternMultiTree
//
//	@doc:
//		Pattern that matches a variable number of trees
//
//---------------------------------------------------------------------------
class CPatternMultiTree : public CPattern
{
private:
	// private copy ctor
	CPatternMultiTree(const CPatternMultiTree &);

public:
	// ctor
	explicit CPatternMultiTree(CMemoryPool *mp) : CPattern(mp)
	{
	}

	// dtor
	virtual ~CPatternMultiTree()
	{
	}

	// check if operator is a pattern leaf
	virtual BOOL
	FLeaf() const
	{
		return false;
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPatternMultiTree;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPatternMultiTree";
	}

};	// class CPatternMultiTree

}  // namespace spqopt


#endif	// !SPQOPT_CPatternMultiTree_H

// EOF
