//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPatternLeaf.h
//
//	@doc:
//		Pattern that matches a single leaf
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPatternLeaf_H
#define SPQOPT_CPatternLeaf_H

#include "spqos/base.h"

#include "spqopt/operators/CPattern.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPatternLeaf
//
//	@doc:
//		Pattern that matches a single leaf
//
//---------------------------------------------------------------------------
class CPatternLeaf : public CPattern
{
private:
	// private copy ctor
	CPatternLeaf(const CPatternLeaf &);

public:
	// ctor
	explicit CPatternLeaf(CMemoryPool *mp) : CPattern(mp)
	{
	}

	// dtor
	virtual ~CPatternLeaf()
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
		return EopPatternLeaf;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPatternLeaf";
	}

};	// class CPatternLeaf

}  // namespace spqopt


#endif	// !SPQOPT_CPatternLeaf_H

// EOF
