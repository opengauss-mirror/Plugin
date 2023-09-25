//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPatternMultiLeaf.h
//
//	@doc:
//		Pattern that matches a variable number of leaves
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPatternMultiLeaf_H
#define SPQOPT_CPatternMultiLeaf_H

#include "spqos/base.h"

#include "spqopt/operators/CPattern.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPatternMultiLeaf
//
//	@doc:
//		Pattern that matches a variable number of expressions, eg inputs to
//		union operator
//
//---------------------------------------------------------------------------
class CPatternMultiLeaf : public CPattern
{
private:
	// private copy ctor
	CPatternMultiLeaf(const CPatternMultiLeaf &);

public:
	// ctor
	explicit CPatternMultiLeaf(CMemoryPool *mp) : CPattern(mp)
	{
	}

	// dtor
	virtual ~CPatternMultiLeaf()
	{
	}

	// check if operator is a pattern leaf
	virtual BOOL
	FLeaf() const
	{
		return true;
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPatternMultiLeaf;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPatternMultiLeaf";
	}

};	// class CPatternMultiLeaf

}  // namespace spqopt


#endif	// !SPQOPT_CPatternMultiLeaf_H

// EOF
